#ifndef GODOT_YARNENGINE_H
#define GODOT_YARNENGINE_H

#include "yarnsave.h"
#include "yarntime.h"
#include "ytween.h"
#include "ydir.h"
#include "ygamestate.h"
#include "core/math/random_number_generator.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/main/node.h"
#include "scene/main/window.h"
#include "scene/resources/packed_scene.h"
#include "scene/main/canvas_layer.h"
#include "scene/gui/rich_text_label.h"
#include "scene/gui/control.h"
#include "scene/gui/color_rect.h"
#include <core/string/translation_server.h>
#include "ypersistentid.h"
#include "scene/3d/camera_3d.h"

class YGameState;
class YTween;
class YGamePlayer;
class YGameAction;

class YEngine : public Node {
    GDCLASS(YEngine, Node);

protected:
    void setup_node();

    void cleanup_node();
    
    static void _bind_methods();

    Variant execute_button_click_callable_if_modulate(const Callable &p_callable, Control *p_control);

    Variant execute_button_click_callable(const Callable &p_callable);

    Node *get_current_scene();

    Array seeded_shuffle(Array array_to_shuffle, int seed_to_use);

    void _notification(int p_what);

    Node *find_node_with_meta(Node *parent_node, const String &p_type);
    Node *find_node_with_method(Node *parent_node, const String &p_type);
    Node* find_node_with_type(Node* parent_node, const String& type);

    TypedArray<Node> find_nodes_with_meta(Node *parent_node, const String &p_type);
    TypedArray<Node> find_nodes_with_method(Node *parent_node, const String &p_type);
    TypedArray<Node> find_nodes_with_type(Node* parent_node, const String& type);

    static YEngine* singleton;
    HashMap<StringName, Node *> othersingletons;
    YGameState* ygamestate = nullptr;

    Camera3D* main_camera = nullptr;

public:
    Control* fader_control = nullptr;
    RichTextLabel* fader_label = nullptr;

    bool using_game_state=false;
    bool already_setup_in_tree = false;
    static bool is_exiting;
    HashMap<StringName,String> class_name_to_script_path;
    HashMap<int, CanvasLayer*> canvas_layers;

    void game_state_starting(YGameState* ygs);
    Callable button_click_callable(const Callable &p_callable);
    Callable button_click_callable_if_modulate(const Callable &p_callable, Control* p_control);

    List<Node *> menu_stack;
    String current_scene_name;
    int get_menu_stack_size() const {return menu_stack.size();}

    void set_current_scene_name(const String& val) {current_scene_name = val;}
    String get_current_scene_name() { if(current_scene_name.is_empty()) {current_scene_name = get_current_scene()->get_name();} return current_scene_name; }
    TypedArray<Node> get_menu_stack() {
        TypedArray<Node> btns;
        for (const auto _node: menu_stack) {
            btns.append(_node);
        }
        return btns;
    }

    bool is_top_of_menu_stack(const Node* test_menu) const {
        if (test_menu == nullptr) return false;
        int iterating_index = 0;
        const int top_of_stack = menu_stack.size()-1;
        for (const auto _node: menu_stack) {
            if (iterating_index == top_of_stack && _node == test_menu)
                return true;
            iterating_index++;
        }
        return false;
    }

    void make_top_of_menu_stack(Node* test_menu) {
        if (test_menu == nullptr) return;
        if (menu_stack.find(test_menu) != nullptr) {
            remove_from_menu_stack(test_menu);
            add_to_menu_stack(test_menu);
        } else {
            add_to_menu_stack(test_menu);
        }
    }

    Node* get_top_of_menu_stack() {
        if (menu_stack.size() > 0) {
            return menu_stack.get(menu_stack.size() - 1);
        }
        return nullptr;
    }

    void add_to_menu_stack(Node* adding_menu) {
        if (adding_menu == nullptr) return;
        for (const auto _node: menu_stack)
            if (_node == adding_menu) return;
        menu_stack.push_back(adding_menu);
        Callable remove_bind_callable = callable_mp(this, &YEngine::remove_from_menu_stack).bind(adding_menu);
        if (!adding_menu->is_connected(SceneStringName(tree_exiting), remove_bind_callable))
            adding_menu->connect(SceneStringName(tree_exiting), remove_bind_callable, CONNECT_ONE_SHOT);
    }

    void remove_from_menu_stack(Node* removing_menu) {
        if (removing_menu == nullptr) return;
        for (const auto _node: menu_stack)
            if (_node == removing_menu) {
                menu_stack.erase(removing_menu);
                Callable remove_bind_callable = callable_mp(this, &YEngine::remove_from_menu_stack).bind(removing_menu);
                if (removing_menu->is_connected(SceneStringName(tree_exiting), remove_bind_callable))
                    removing_menu->disconnect(SceneStringName(tree_exiting), remove_bind_callable);
                return;
            }
    }

    float last_button_click_time;
    float get_last_button_click_time() {return last_button_click_time;}
    void set_last_button_click_time(const float val) {last_button_click_time = val;}

    bool can_button_click;
    bool get_can_button_click() {return can_button_click;}
    void set_can_button_click(const bool val) {can_button_click = val;}

    uint32_t set_flag_value(uint32_t collision_layer, int p_layer_number, bool p_value);
    bool check_flag_value(uint32_t collision_layer, int p_layer_number) const;
    void do_process();
    int string_to_hash(const String& str);

    void setting_position_in_parent(Node *node_entered_tree, const Variant &p_spawn_pos);
    void setting_position_and_rotation_in_parent(Node *node_entered_tree, const Variant &p_spawn_pos, const Variant &p_spawn_rot);

    void create_fader();
    CanvasLayer* get_canvas_layer(int layer_index);

    Node *spawn(const Ref<PackedScene> &p_spawnable_scene, Node* p_desired_parent, const Variant &p_spawn_pos, bool p_force_readable_name);
    Node *spawn_with_rot(const Ref<PackedScene> &p_spawnable_scene, Node* p_desired_parent, const Variant &p_spawn_pos, const Variant &p_spawn_rot, bool p_force_readable_name);
    YEngine();
    ~YEngine();

    YSave* ysave;
    YTime* ytime;
    Ref<YDir> ydir;
    YTween* ytween;
    void add_setting(const String& name, const Variant& default_value, Variant::Type type,
            PropertyHint hint = PROPERTY_HINT_NONE, const String& hint_string = "",
            int usage = PROPERTY_USAGE_DEFAULT, bool restart_if_changed = false);

    TypedArray<PackedScene> find_packedscenes_in(const Variant &variant_path, const String &name_contains);

    TypedArray<Resource> find_resources_in(const Variant &path, const String &name_contains = "");

    bool are_resources_virtually_the_same(const Ref<Resource> &resource_a, const Ref<Resource> &resource_b);

    Vector<String> get_diverging_variables_in_resources(const Ref<Resource> &resource_a, const Ref<Resource> &resource_b);

    PackedStringArray find_resources_paths_in(const Variant &path, const String &name_contains);

    static YEngine* get_singleton();

    void before_prepare_save();
    void after_prepare_save();
    void on_loaded_save(Dictionary save_data);
    String EKey(const Dictionary &dict,int val);
    HashMap<int,HashMap<int,String>> game_resource_paths;
    HashMap<int,HashMap<int,Ref<Resource>>> game_resources;

    void set_game_resource_path(int resource_type, int resource_id, const String &respath) {
        if (!game_resource_paths.has(resource_type)) {
            game_resource_paths[resource_type] = {};
        }
        game_resource_paths[resource_type][resource_id] = respath;
    }

    void set_game_resource(int resource_type, int resource_id, Ref<Resource> v) {
        if (!game_resources.has(resource_type)) {
            game_resources[resource_type] = {};
        }
        game_resources[resource_type][resource_id] = v;
    }

    void remove_game_resource(int resource_type, int resource_id) {
        if (game_resources.has(resource_type))
            game_resources[resource_type].erase(resource_id);
    }
    Ref<Resource> get_game_resource(int resource_type, int resource_id);

    bool has_game_resource(int resource_type, int param) ;

    Vector3 get_random_point_on_top_of_mesh(MeshInstance3D* p_meshInstance, Ref<RandomNumberGenerator> p_rng);

    Array get_all_game_resources_types() {
        Array _return_array;
        for (auto actpq: game_resources) {
            _return_array.append(actpq.key);
        }
        return _return_array;
    }

    Dictionary get_all_game_resources_of_type(int resource_type) {
        Dictionary returndict;
        if (game_resources.has(resource_type)) {
            auto hashtable_resources = game_resources[resource_type];
            for (auto actpq: hashtable_resources) {
                returndict[actpq.key] = actpq.value;
            }
        }
        return returndict;
    }

    void check_if_ynet_available() {
        #ifdef YNET
        print_line("YNet is available");
        #else
        print_line("YNet is not available");
        #endif
    }

    void check_if_ygodot() {
        #ifdef YGODOT
        print_line("Using YGodot");
        #else
        print_line("Not using YGodot");
        #endif
    }
    
    Vector2 get_control_pos_for_3d_pos(const Vector3 &p_global_pos, bool sticky = false);
    Dictionary get_script_base_properties(Node* p_node);
    void instant_fade_to_alpha(float alpha);
    RichTextLabel* get_fader_label();
    Ref<YTweenWrap> fade_to_black(float duration = 0.5, const String& text = "");
    Ref<YTweenWrap> fade_to_transparent(float duration = 0.5);
    Ref<YTweenWrap> fade_to_black_then_transparent(float duration = 0.5, const String& text = "", const Callable &p_callable_in_middle = Callable());
    void set_fader_text(const String& text);
    void hide_fader();
    bool is_fading() const;

};

#endif