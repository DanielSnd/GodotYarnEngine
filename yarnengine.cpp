#include "yarnengine.h"

#include "yarnsave.h"
#include "core/config/project_settings.h"
#include "core/io/dir_access.h"
#include "core/io/json.h"
#include "core/math/random_number_generator.h"
#include "scene/gui/control.h"

bool YEngine::is_exiting = false;

YEngine* YEngine::singleton = nullptr;

YEngine * YEngine::get_singleton() {
    return singleton;
}

String YEngine::EKey(const Dictionary &dict, int val) {
    return dict.find_key(val);
}

Ref<Resource> YEngine::get_game_resource(int resource_type, int resource_id) {
        if (game_resources.has(resource_type)) {
            auto hashtable_resources = game_resources[resource_type];
            if (hashtable_resources.has(resource_id)) {
                return hashtable_resources[resource_id];
            }
        }
        if (game_resource_paths.has(resource_type)) {
            auto respaths = game_resource_paths[resource_type];
            if (respaths.has(resource_id)) {
                auto res_loaded = ResourceLoader::load(respaths[resource_id]);
                if (res_loaded.is_valid()) {
                    if (!game_resources.has(resource_type)) game_resources[resource_type] = {};
                    game_resources[resource_type][resource_id] = res_loaded;
                } else {
                    respaths.erase(resource_id);
                }
            }
        }
        return nullptr;
}

bool YEngine::has_game_resource(int resource_type, int resource_id) {
    if (game_resources.has(resource_type)) {
        return game_resources[resource_type].has(resource_id);
    }
    if (game_resource_paths.has(resource_type)) {
        auto respaths = game_resource_paths[resource_type];
        if (respaths.has(resource_id)) {
            return true;
        }
    }
    return false;
}

Dictionary YEngine::get_script_base_properties(Node* p_node) {
    Dictionary base_values;
    if (!p_node) {
        return base_values;
    }

    Ref<Script> script = p_node->get_script();
    if (!script.is_valid()) {
        return base_values;
    }

    // Get script property list
    List<PropertyInfo> script_property_list;
    script->get_script_property_list(&script_property_list);

    // Get allowed property names
    HashSet<StringName> allowed_properties;
    for (const PropertyInfo &pi : script_property_list) {
        allowed_properties.insert(pi.name);
    }

    // Get all properties and filter them
    List<PropertyInfo> property_list;
    p_node->get_property_list(&property_list);

    for (const PropertyInfo &pi : property_list) {
        if (allowed_properties.has(pi.name) && 
            !pi.name.begins_with("_") && 
            (pi.usage & PROPERTY_USAGE_EDITOR) != 0 && 
            pi.name != "script") {
            base_values[pi.name] = p_node->get(pi.name);
        }
    }

    return base_values;
}

void YEngine::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_current_scene"), &YEngine::get_current_scene);
    ClassDB::bind_method(D_METHOD("get_current_scene_name"), &YEngine::get_current_scene_name);
    ClassDB::bind_method(D_METHOD("set_current_scene_name","scene_name"), &YEngine::set_current_scene_name);

    ClassDB::bind_method(D_METHOD("find_packedscenes_in", "path", "name_contains"), &YEngine::find_packedscenes_in,DEFVAL(""));
    ClassDB::bind_method(D_METHOD("find_resources_in", "path", "name_contains"), &YEngine::find_resources_in,DEFVAL(""));
    ClassDB::bind_method(D_METHOD("find_resources_paths_in", "path", "name_contains"), &YEngine::find_resources_paths_in,DEFVAL(""));

    ClassDB::bind_method(D_METHOD("find_node_with_type","parent_node","node_type"), &YEngine::find_node_with_type);
    ClassDB::bind_method(D_METHOD("find_nodes_with_type","parent_node","node_type"), &YEngine::find_nodes_with_type);
    ClassDB::bind_method(D_METHOD("find_node_with_method","parent_node","method_name"), &YEngine::find_node_with_method);
    ClassDB::bind_method(D_METHOD("find_nodes_with_method","parent_node","method_name"), &YEngine::find_nodes_with_method);
    ClassDB::bind_method(D_METHOD("find_node_with_meta","parent_node","meta_name"), &YEngine::find_node_with_meta);
    ClassDB::bind_method(D_METHOD("find_nodes_with_meta","parent_node","meta_name"), &YEngine::find_nodes_with_meta);

    ClassDB::bind_method(D_METHOD("set_flag_value","flags","flag","set_value"), &YEngine::set_flag_value);
    ClassDB::bind_method(D_METHOD("check_flag_value","flags","check_flag"), &YEngine::check_flag_value);

    ClassDB::bind_method(D_METHOD("spawn","packed_scene","parent","global_pos","force_readable_name"), &YEngine::spawn, DEFVAL(false));

    ClassDB::bind_method(D_METHOD("spawn_with_rot","packed_scene","parent","global_pos", "global_rot","force_readable_name"), &YEngine::spawn_with_rot, DEFVAL(false));

    ClassDB::bind_method(D_METHOD("EKey","dict","value"), &YEngine::EKey);
    ClassDB::bind_method(D_METHOD("string_to_hash","input_string"), &YEngine::string_to_hash);
    ClassDB::bind_method(D_METHOD("seeded_shuffle","array","seed"), &YEngine::seeded_shuffle);

    ClassDB::bind_method(D_METHOD("set_game_resource_path","resource_type","resource_id", "resource_path"), &YEngine::set_game_resource_path);
    ClassDB::bind_method(D_METHOD("set_game_resource","resource_type","resource_id", "resource"), &YEngine::set_game_resource);
    ClassDB::bind_method(D_METHOD("get_game_resource","resource_type","resource_id"), &YEngine::get_game_resource);
    ClassDB::bind_method(D_METHOD("has_game_resource","resource_type","resource_id"), &YEngine::has_game_resource);
    ClassDB::bind_method(D_METHOD("remove_game_resource","resource_type","resource_id"), &YEngine::remove_game_resource);
    ClassDB::bind_method(D_METHOD("get_all_game_resources_of_type","resource_type"), &YEngine::get_all_game_resources_of_type);
    ClassDB::bind_method(D_METHOD("get_all_game_resources_types"), &YEngine::get_all_game_resources_types);

    ClassDB::bind_method(D_METHOD("fade_to_black","duration","text"), &YEngine::fade_to_black, DEFVAL(0.5), DEFVAL(""));
    ClassDB::bind_method(D_METHOD("fade_to_transparent","duration"), &YEngine::fade_to_transparent, DEFVAL(0.5));
    ClassDB::bind_method(D_METHOD("fade_to_black_then_transparent","duration","text","callable_in_middle"), &YEngine::fade_to_black_then_transparent, DEFVAL(0.5), DEFVAL(""), DEFVAL(Callable()));

    ClassDB::bind_method(D_METHOD("get_fader_label"), &YEngine::get_fader_label);
    ClassDB::bind_method(D_METHOD("set_fader_text","text"), &YEngine::set_fader_text);
    ClassDB::bind_method(D_METHOD("is_fading"), &YEngine::is_fading);

    ClassDB::bind_method(D_METHOD("are_resources_virtually_the_same","resource_a","resource_b"), &YEngine::are_resources_virtually_the_same);
    ClassDB::bind_method(D_METHOD("get_diverging_variables_in_resources","resource_a","resource_b"), &YEngine::get_diverging_variables_in_resources);

    ClassDB::bind_method(D_METHOD("get_control_pos_for_3d_pos","global_pos","sticky"), &YEngine::get_control_pos_for_3d_pos, DEFVAL(false));
    
    ClassDB::bind_method(D_METHOD("get_script_base_properties","node"), &YEngine::get_script_base_properties);

    ClassDB::bind_method(D_METHOD("get_canvas_layer","layer_index"), &YEngine::get_canvas_layer, DEFVAL(0));

    ClassDB::bind_method(D_METHOD("instant_fade_to_alpha","alpha"), &YEngine::instant_fade_to_alpha);

    ClassDB::bind_method(D_METHOD("set_last_button_click_time", "last_button_click_time"), &YEngine::set_last_button_click_time);
    ClassDB::bind_method(D_METHOD("get_last_button_click_time"), &YEngine::get_last_button_click_time);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "last_button_click_time"), "set_last_button_click_time", "get_last_button_click_time");
    
    ClassDB::bind_method(D_METHOD("set_can_button_click", "can_button_click"), &YEngine::set_can_button_click);
    ClassDB::bind_method(D_METHOD("get_can_button_click"), &YEngine::get_can_button_click);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "can_button_click"), "set_can_button_click", "get_can_button_click");

    ADD_SIGNAL(MethodInfo("changed_pause", PropertyInfo(Variant::BOOL, "pause_value")));
    ADD_SIGNAL(MethodInfo("initialized"));

    // Add RPC methods for action steps
    ClassDB::bind_method(D_METHOD("_rpc_request_action_step_approval", "action_id", "step_identifier", "step_data", "sender_id"), &YEngine::_rpc_request_action_step_approval);
    ClassDB::bind_method(D_METHOD("_rpc_apply_action_step", "action_id", "step_identifier", "step_data"), &YEngine::_rpc_apply_action_step);

    // Add RPC methods for game actions
    ClassDB::bind_method(D_METHOD("_rpc_register_game_action", "action_data"), &YEngine::_rpc_register_game_action);
}

CanvasLayer* YEngine::get_canvas_layer(int layer_index) {
    if (is_exiting) {
        return nullptr;
    }
    if (!canvas_layers.has(layer_index)) {
        CanvasLayer* new_canvas_layer = memnew(CanvasLayer);
        canvas_layers[layer_index] = new_canvas_layer;
        add_child(new_canvas_layer);
        new_canvas_layer->set_name(vformat("CanvasLayer_%d",layer_index));
        new_canvas_layer->set_layer(layer_index);
    }

    return canvas_layers[layer_index];
}

RichTextLabel* YEngine::get_fader_label() {
        if (fader_label == nullptr) {
            create_fader();
        }
        return fader_label;
}

void YEngine::create_fader() {
    if (fader_control != nullptr) {
        return;
    }
    
    fader_control = memnew(Control);
    fader_control->set_anchors_preset(Control::PRESET_FULL_RECT);
    fader_control->set_mouse_filter(Control::MOUSE_FILTER_IGNORE);
    fader_control->set_focus_mode(Control::FOCUS_NONE);
    
    ColorRect* rect = memnew(ColorRect);
    rect->set_anchors_preset(Control::PRESET_FULL_RECT);
    rect->set_color(Color(0, 0, 0, 1.0));
    rect->set_mouse_filter(Control::MOUSE_FILTER_IGNORE);
    rect->set_focus_mode(Control::FOCUS_NONE);
    
    fader_label = memnew(RichTextLabel);
    fader_label->set_anchors_preset(Control::PRESET_FULL_RECT);
    fader_label->set_text("");
    fader_label->set_scroll_active(false);
    fader_label->set_use_bbcode(true);
    fader_label->set_horizontal_alignment(HorizontalAlignment::HORIZONTAL_ALIGNMENT_CENTER);
    fader_label->set_vertical_alignment(VerticalAlignment::VERTICAL_ALIGNMENT_CENTER);
    fader_label->set_mouse_filter(Control::MOUSE_FILTER_IGNORE);
    fader_label->set_focus_mode(Control::FOCUS_NONE);
    
    get_canvas_layer(100)->add_child(fader_control);
    fader_control->add_child(rect);
    fader_control->add_child(fader_label);
}

void YEngine::instant_fade_to_alpha(float alpha) {
    if (fader_control == nullptr) {
        create_fader();
    }
    fader_control->set_modulate(Color(1.0,1.0,1.0,alpha));
    fader_control->set_visible(alpha > 0.0001);
}

Ref<YTweenWrap> YEngine::fade_to_black(float duration, const String& text) {
    if (is_exiting) return Ref<YTweenWrap>();
    if (fader_control == nullptr) {
        create_fader();
    }

    // Set the text if provided
    if (!text.is_empty()) {
        set_fader_text(text);
    }
    
    // Get the ColorRect and animate its alpha
    fader_control->set_visible(true);
    
    Ref<YTweenWrap> tween = YTween::get_singleton()->create_unique_tween(fader_control);
    tween->tween_property(fader_control, NodePath("modulate"), Color(1.0,1.0,1.0,1.0), duration)
            ->set_ease(Tween::EaseType::EASE_IN)
            ->set_trans(Tween::TransitionType::TRANS_QUAD);
    return tween;
}

Ref<YTweenWrap> YEngine::fade_to_transparent(float duration) {
    if (!fader_control || is_exiting) {
        return Ref<YTweenWrap>();
    }
    
    Ref<YTweenWrap> tween = YTween::get_singleton()->create_unique_tween(fader_control);
    tween->tween_property(fader_control, NodePath("modulate"), Color(1.0,1.0,1.0,0.0), duration)
            ->set_ease(Tween::EaseType::EASE_OUT)
            ->set_trans(Tween::TransitionType::TRANS_QUAD);
    tween->connect("finished",(callable_mp(this,&YEngine::hide_fader)));
    return tween;
}

Ref<YTweenWrap> YEngine::fade_to_black_then_transparent(float duration, const String& text, const Callable &p_callable_in_middle) {
    if (is_exiting) return Ref<YTweenWrap>();
    if (fader_control == nullptr) {
        create_fader();
    }
    if (!text.is_empty()) {
        set_fader_text(text);
    }
    fader_control->set_visible(true);

    Ref<YTweenWrap> tween = YTween::get_singleton()->create_unique_tween(fader_control);
    tween->tween_property(fader_control, NodePath("modulate"), Color(1.0,1.0,1.0,1.0), duration * 0.375)
            ->set_ease(Tween::EaseType::EASE_IN)
            ->set_trans(Tween::TransitionType::TRANS_QUAD);
    tween->connect("step_finished",p_callable_in_middle, CONNECT_ONE_SHOT);
    tween->chain()->tween_interval(duration * 0.25);
    tween->chain()->tween_property(fader_control, NodePath("modulate"), Color(1.0,1.0,1.0,0.0), duration * 0.375)
            ->set_ease(Tween::EaseType::EASE_OUT)
            ->set_trans(Tween::TransitionType::TRANS_QUAD);
    tween->connect("finished",(callable_mp(this,&YEngine::hide_fader)));
    return tween;
}

void YEngine::hide_fader() {
    if (fader_control) {
        fader_control->set_visible(false);
    }
}

void YEngine::set_fader_text(const String& text) {
    if (!fader_control) {
        create_fader();
    }
    fader_label->set_text(text);
}

bool YEngine::is_fading() const {
    if (!fader_control) {
        return false;
    }
    
    return fader_control->is_visible() && fader_control->get_modulate().a > 0.0f && fader_control->get_modulate().a < 1.0f;
}

Variant YEngine::execute_button_click_callable_if_modulate(const Callable &p_callable,Control* p_control) {
    if(p_control == nullptr || p_control->get_modulate().a < 0.94) return Variant{};
    if(ytime->has_pause_independent_time_elapsed(last_button_click_time,0.15)) {
        last_button_click_time = ytime->pause_independent_time;
        return p_callable.call();
    }
    return Variant{};
}
Variant YEngine::execute_button_click_callable(const Callable &p_callable) {
    if(ytime->has_pause_independent_time_elapsed(last_button_click_time,0.15)) {
        last_button_click_time = ytime->pause_independent_time;
        return p_callable.call();
    }
    return Variant{};
}

void YEngine::game_state_starting(const Ref<YGameState> &ygs) {
    if (ygs.is_valid()) {
        ygamestate = ygs.ptr();
        using_game_state=true;
        // print_line("Registered using game state ",ygamestate," using game state now? ",using_game_state);
    }
}

Callable YEngine::button_click_callable(const Callable &p_callable) {
    return (callable_mp(this,&YEngine::execute_button_click_callable).bind(p_callable));
}

Callable YEngine::button_click_callable_if_modulate(const Callable &p_callable,Control* p_control) {
    return (callable_mp(this,&YEngine::execute_button_click_callable_if_modulate).bind(p_callable,p_control));
}

Node* YEngine::get_current_scene() {
    return SceneTree::get_singleton()->get_current_scene();
}

Array YEngine::seeded_shuffle(Array array_to_shuffle,int seed_to_use) {
    Ref<RandomNumberGenerator> rng;
    rng.instantiate();
    rng->set_seed(seed_to_use);
    for (int i = array_to_shuffle.size() - 1; i >= 1; i--) {
        const int j = rng->randi_range(0,i);
        const Variant tmp = array_to_shuffle[j];
        array_to_shuffle[j] = array_to_shuffle[i];
        array_to_shuffle[i] = tmp;
    }
    return array_to_shuffle;
}

void YEngine::cleanup_node() {
    if (is_inside_tree()) {
        // First remove from tree to prevent duplicate cleanup
        get_parent()->remove_child(this);
        
        // Clear references
        ysave = nullptr;
        ytime = nullptr;
        ytween = nullptr;
        ygamestate = nullptr;
        
        // Clear singleton if this is the singleton instance
        if (singleton == this) {
            singleton = nullptr;
        }
        
        queue_free();
    }
}

void YEngine::before_prepare_save() {
    if (ysave != nullptr) {
        String current_scene_name = vformat("%s_saved_nodes3d", get_current_scene_name());
        if (ysave->get_save_data().has(current_scene_name)) {
            ysave->get_save_data()[current_scene_name] = Array();
        }
    }
}

void YEngine::after_prepare_save() {
    if (ysave != nullptr) {
        if (YPersistentID::amount_of_saved_scene_paths() > 0) {
            ysave->get_save_data()["scene_paths_and_ids"] = YPersistentID::get_all_scene_paths_and_ids();
        }
    }
}

Vector2 YEngine::get_control_pos_for_3d_pos(const Vector3 &p_global_pos, bool p_sticky) {
    if (main_camera != nullptr && !main_camera->is_current()) {
        main_camera = get_viewport()->get_camera_3d();
    }
    if (main_camera == nullptr) {
        main_camera = get_viewport()->get_camera_3d();
        if (main_camera == nullptr) {
            return Vector2();
        }
    }
    
    const Transform3D camera_transform = main_camera->get_global_transform();
    const Vector3 camera_position = camera_transform.get_origin();

    // Check if object is behind camera
    bool is_behind = camera_transform.get_basis().get_column(2).dot(p_global_pos - camera_position) > 0;

    Vector2 unprojected_position = main_camera->unproject_position(p_global_pos);
    
    // Get viewport size
    Vector2i viewport_base_size;
    Size2i content_scale = get_viewport()->get_visible_rect().size;
    viewport_base_size = content_scale;

    const float MARGIN = 32.0f; // Define margin constant

    // For non-sticky waypoints
    if (!p_sticky) {
        return unprojected_position;
    }

    // Handle X axis for sticky waypoints
    if (is_behind) {
        if (unprojected_position.x < viewport_base_size.x / 2) {
            unprojected_position.x = viewport_base_size.x - MARGIN;
        } else {
            unprojected_position.x = MARGIN;
        }
    }

    // Handle Y axis for sticky waypoints
    if (is_behind || unprojected_position.x < MARGIN || 
        unprojected_position.x > viewport_base_size.x - MARGIN) {
        Transform3D look = camera_transform.looking_at(p_global_pos, Vector3(0, 1, 0));
        float diff = Math::angle_difference(look.get_basis().get_euler().x, camera_transform.get_basis().get_euler().x);
        unprojected_position.y = viewport_base_size.y * (0.5 + (diff / Math::deg_to_rad(main_camera->get_fov())));
    }

    // Clamp position to screen bounds
    Vector2 final_position = Vector2(
        CLAMP(unprojected_position.x, MARGIN, viewport_base_size.x - MARGIN),
        CLAMP(unprojected_position.y, MARGIN, viewport_base_size.y - MARGIN)
    );

    return final_position;
}

void YEngine::on_loaded_save(Dictionary save_data) {
    // print_line("YEngine On loaded save");
    if (save_data.has("scene_paths_and_ids")) {
        YPersistentID::set_scene_paths_and_ids(save_data["scene_paths_and_ids"]);
    }
}

void YEngine::setup_node() {
    if(!already_setup_in_tree && SceneTree::get_singleton() != nullptr) {
        ysave = YSave::get_singleton();
        if (ysave != nullptr) {
            ysave->connect("before_prepare_save",callable_mp(this,&YEngine::before_prepare_save));
            ysave->connect("after_prepare_save",callable_mp(this,&YEngine::after_prepare_save));
            ysave->connect("loaded_save",callable_mp(this,&YEngine::on_loaded_save));
        }
        ytime = YTime::get_singleton();
        ydir.instantiate();
        ytween = YTween::get_singleton();
        ytime->is_paused = false;
        last_button_click_time = 0.0;
        String appname = GLOBAL_GET("application/config/window_name");
        if (appname.is_empty()) {
            appname = GLOBAL_GET("application/config/name");
        }
        DisplayServer::get_singleton()->window_set_title(TranslationServer::get_singleton()->translate(appname));

        SceneTree::get_singleton()->get_root()->connect(SceneStringName(tree_exiting), callable_mp(this, &YEngine::cleanup_node));
        SceneTree::get_singleton()->get_root()->call_deferred("add_child", this);
        set_name("YEngine");
        already_setup_in_tree=true;
        ysave->time_started = OS::get_singleton()->get_ticks_msec() * 0.001f;
    }
}

void YEngine::_notification(int p_what) {
    if (p_what == NOTIFICATION_POSTINITIALIZE && !Engine::get_singleton()->is_editor_hint()) {
        callable_mp(this,&YEngine::setup_node).call_deferred();
    }
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
        } break;
        // case NOTIFICATION_ENTER_WORLD: {
        // } break;
        case NOTIFICATION_EXIT_TREE: {
            ydir = Ref<YDir>();
            canvas_layers.clear();
            fader_control = nullptr;
            fader_label = nullptr;
            break;
        }
        case NOTIFICATION_PARENTED: {
            if (Engine::get_singleton()->is_editor_hint()) {
            }
        }
        case NOTIFICATION_WM_CLOSE_REQUEST: {
            if (using_game_state) {
                if (ygamestate != nullptr) {
                    ygamestate->clear_all_game_actions();
                    ygamestate = nullptr;
                }
                using_game_state = false;
            }
        }
        case NOTIFICATION_READY: {
            this->rpc_config(rpc_apply_action_step_stringname,create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_AUTHORITY, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
            this->rpc_config(rpc_register_game_action_stringname,create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_AUTHORITY, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
            this->rpc_config(rpc_request_action_step_approval_stringname,create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_ANY_PEER, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
            emit_signal("initialized");
            set_process_mode(PROCESS_MODE_ALWAYS);
            set_process(true);
            set_physics_process(true);
            TypedArray<Dictionary> global_class_list = ProjectSettings::get_singleton()->get_global_class_list();
            for (int i = 0; i < static_cast<int>(global_class_list.size()); ++i) {
                Dictionary p_class_list = global_class_list[i];
                if (p_class_list.has("class") && p_class_list.has("path"))
                    class_name_to_script_path[p_class_list["class"]] = p_class_list["path"];
            }
        } break;
        case NOTIFICATION_PHYSICS_PROCESS: {
            if (!Engine::get_singleton()->is_editor_hint() && OS::get_singleton() != nullptr) {
                ytween->do_process(get_physics_process_delta_time());
            }
        }
        case NOTIFICATION_PROCESS: {
            if (OS::get_singleton() != nullptr) {
                if (!Engine::get_singleton()->is_editor_hint()) {
                    do_process();
                    if (using_game_state) {
                        ygamestate->do_process(get_process_delta_time());
                    }
                }
            }
            //print_line(pause_independent_time);
        } break;
        default:
            break;
    }
}

Node * YEngine::find_node_with_meta(Node *parent_node, const String &p_type) {
    if (parent_node != nullptr) {
        TypedArray<Node> find_stuff = parent_node->get_children();
        while (find_stuff.size()) {
            Node *_child = Object::cast_to<Node>(find_stuff.pop_back());
            if (_child->has_meta(p_type)) return _child;
        }
    }
    return nullptr;
}

Node * YEngine::find_node_with_method(Node *parent_node, const String &p_type) {
    if (parent_node != nullptr) {
        TypedArray<Node> find_stuff = parent_node->get_children();
        while (find_stuff.size()) {
            Node *_child = Object::cast_to<Node>(find_stuff.pop_back());
            if (_child->has_method(p_type)) return _child;
        }
    }
    return nullptr;
}
Node * YEngine::find_node_with_type(Node *parent_node, const String &p_type) {
    if (parent_node != nullptr) {
        TypedArray<Node> find_stuff = parent_node->find_children("*",p_type);
        if (find_stuff.size() > 0) {
            return Object::cast_to<Node>(find_stuff[0].operator Object*());
        }
    }
    return nullptr;
}

TypedArray<Node> YEngine::find_nodes_with_meta(Node *parent_node, const String &p_type) {
    TypedArray<Node> found_nodes;
    if (parent_node != nullptr) {
        TypedArray<Node> find_stuff = parent_node->get_children();
        while (find_stuff.size()) {
            Node *_child = Object::cast_to<Node>(find_stuff.pop_back());
            if (_child->has_meta(p_type)) {
                found_nodes.push_back(_child);
            }
        }
    }
    return found_nodes;
}

TypedArray<Node> YEngine::find_nodes_with_method(Node *parent_node, const String &p_type) {
    TypedArray<Node> found_nodes;
    if (parent_node != nullptr) {
        TypedArray<Node> find_stuff = parent_node->get_children();
        while (find_stuff.size()) {
            Node *_child = Object::cast_to<Node>(find_stuff.pop_back());
            if (_child->has_method(p_type)) {
                found_nodes.push_back(_child);
            }
        }
    }
    return found_nodes;
}

TypedArray<Node> YEngine::find_nodes_with_type(Node *parent_node, const String &p_type) {
    TypedArray<Node> found_nodes;
    if (parent_node != nullptr) {
        TypedArray<Node> find_stuff = parent_node->find_children("*",p_type);
        while (find_stuff.size()) {
            found_nodes.push_back(Object::cast_to<Node>(find_stuff.pop_back()));
        }
    }
    return found_nodes;
}

uint32_t YEngine::set_flag_value(uint32_t collision_layer, int p_layer_number, bool p_value) {
    ERR_FAIL_COND_V_MSG(p_layer_number < 1, collision_layer, "Collision layer number must be between 1 and 32 inclusive.");
    ERR_FAIL_COND_V_MSG(p_layer_number > 32, collision_layer, "Collision layer number must be between 1 and 32 inclusive.");
    uint32_t collision_layer_new = collision_layer;
    if (p_value) {
        collision_layer_new |= 1 << (p_layer_number - 1);
    } else {
        collision_layer_new &= ~(1 << (p_layer_number - 1));
    }
    return collision_layer_new;
}

bool YEngine::check_flag_value(uint32_t collision_layer, int p_layer_number) const {
    ERR_FAIL_COND_V_MSG(p_layer_number < 1, false, "Collision layer number must be between 1 and 32 inclusive.");
    ERR_FAIL_COND_V_MSG(p_layer_number > 32, false, "Collision layer number must be between 1 and 32 inclusive.");
    return collision_layer & (1 << (p_layer_number - 1));
}

void YEngine::do_process() {
    ytime->handle_time_setting();
    ysave->process(ytime->pause_independent_time);
    ytween->do_process(get_process_delta_time());
}

int YEngine::string_to_hash(const String &str) {
    uint32_t crc = 0xFFFFFFFF;
    const char32_t *chr = str.get_data();
    uint32_t c = *chr++;
    while (c) {
        crc ^= static_cast<uint8_t>(c);
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (-static_cast<int>(crc & 1) & 0xEDB88320);
        }
        c = *chr++;
    }
    return static_cast<int>(~crc);
}

void YEngine::setting_position_in_parent(Node *node_entered_tree, const Variant &p_spawn_pos) {
    if (node_entered_tree != nullptr) {
        node_entered_tree->call("set_global_position",p_spawn_pos);
    }
}
void YEngine::setting_position_and_rotation_in_parent(Node *node_entered_tree, const Variant &p_spawn_pos, const Variant &p_spawn_rot) {
    if (node_entered_tree != nullptr) {
        node_entered_tree->call("set_global_position",p_spawn_pos);
        node_entered_tree->call("set_global_rotation",p_spawn_rot);
    }
}

Node * YEngine::spawn(const Ref<PackedScene> &p_spawnable_scene, Node *p_desired_parent, const Variant &p_spawn_pos, bool p_force_readable_name) {
    ERR_FAIL_COND_V_MSG(p_spawnable_scene.is_null() || !p_spawnable_scene.is_valid(), nullptr, "ERROR: Spawnable scene is not valid");
    ERR_FAIL_COND_V_MSG(p_desired_parent == nullptr, nullptr, "ERROR: Parent node is not valid");
    auto spawned_instance = p_spawnable_scene->instantiate();
    ERR_FAIL_COND_V_MSG(spawned_instance == nullptr, nullptr, "ERROR: Spawned Instance is null");
    p_desired_parent->connect("child_entered_tree",callable_mp(this,&YEngine::setting_position_in_parent).bind(p_spawn_pos), CONNECT_ONE_SHOT);
    p_desired_parent->add_child(spawned_instance,p_force_readable_name);
    return spawned_instance;
}

Node * YEngine::spawn_with_rot(const Ref<PackedScene> &p_spawnable_scene, Node *p_desired_parent, const Variant &p_spawn_pos, const Variant &p_spawn_rot, bool p_force_readable_name) {
    ERR_FAIL_COND_V_MSG(p_spawnable_scene.is_null() || !p_spawnable_scene.is_valid(), nullptr, "ERROR: Spawnable scene is not valid");
    ERR_FAIL_COND_V_MSG(p_desired_parent == nullptr, nullptr, "ERROR: Parent node is not valid");
    auto spawned_instance = p_spawnable_scene->instantiate();
    ERR_FAIL_COND_V_MSG(spawned_instance == nullptr, nullptr, "ERROR: Spawned Instance is null");
    p_desired_parent->connect("child_entered_tree",callable_mp(this,&YEngine::setting_position_and_rotation_in_parent).bind(p_spawn_pos,p_spawn_rot), CONNECT_ONE_SHOT);
    p_desired_parent->add_child(spawned_instance, p_force_readable_name);
    return spawned_instance;
}

//
// Variant YEngine::test_callback_thing(const Callable &p_callable) {
//     if (!p_callable.is_null() && p_callable.is_valid()) {
//         return p_callable.call();
//     }
//     return Variant{};
// }

YEngine::YEngine() {
    singleton = this;
    is_exiting = false;
    last_button_click_time = 0.0;
    using_game_state = false;
    can_button_click=true;
    ytween=nullptr;
    ysave=nullptr;
    ytime=nullptr;
    ygamestate = nullptr;
    current_scene_name = "";
    rpc_request_action_step_approval_stringname = SNAME("_rpc_request_action_step_approval");
    rpc_apply_action_step_stringname = SNAME("_rpc_apply_action_step");
    rpc_register_game_action_stringname = SNAME("_rpc_register_game_action");
    ydir.instantiate();
}

YEngine::~YEngine() {
    is_exiting = true;
    // Remove singleton reference if not already cleared
    if (singleton == this) {
        singleton = nullptr;
    }
    current_scene_name = "";
    // Clear any remaining references
    if (using_game_state) {
        using_game_state = false;
        ygamestate = nullptr;
    }
    ytween = nullptr;
    ysave = nullptr;
    ytime = nullptr;
}

void YEngine::add_setting(const String& p_name, const Variant& p_default_value, Variant::Type p_type,
PropertyHint p_hint, const String& p_hint_string, int p_usage,bool restart_if_changed) {
    if (!ProjectSettings::get_singleton()->has_setting(p_name)) {
        ProjectSettings::get_singleton()->set_setting(p_name, p_default_value);
    }
    ProjectSettings::get_singleton()->set_custom_property_info(PropertyInfo(p_type, p_name, p_hint, p_hint_string,p_usage));
    ProjectSettings::get_singleton()->set_initial_value(p_name, p_default_value);
    ProjectSettings::get_singleton()->set_restart_if_changed(p_name, restart_if_changed);
}

TypedArray<PackedScene> YEngine::find_packedscenes_in(const Variant &variant_path, const String &name_contains) {
    PackedStringArray _paths;
    if(variant_path.get_type() == Variant::Type::ARRAY) { _paths = variant_path;
    } else if (variant_path.get_type() != Variant::Type::STRING) { return Array{}; }
    else { _paths.append(variant_path); }
    TypedArray<PackedScene> return_paths;
    bool has_name_contains = !name_contains.is_empty();

    for (auto _path: _paths) {
        Error check_error;
        auto dir = DirAccess::open(_path,&check_error);
        if (check_error != OK) {
            WARN_PRINT(vformat("[find_packedscenes_in] Couldn't open dir %s",_path));
            continue;
        }

        dir->list_dir_begin();
        String file_name = dir->get_next();
        while (!file_name.is_empty()) {
            file_name = file_name.trim_suffix(".remap");
            if (file_name.ends_with(".tscn"))
                if (!has_name_contains || file_name.contains(name_contains)) {
                    Ref<PackedScene> loaded_resource = ResourceLoader::load(vformat("%s/%s",_path,file_name));
                    if(loaded_resource.is_valid())
                        return_paths.append(loaded_resource);
                    else
                        WARN_PRINT(vformat("[find_packedscenes_in] Failed loading packedscene at %s/%s",_path,file_name));
                }
            file_name = dir->get_next();
        }
    }
    return return_paths;
}

TypedArray<Resource> YEngine::find_resources_in(const Variant &variant_path, const String &name_contains) {
    PackedStringArray _paths;
    if(variant_path.get_type() == Variant::Type::ARRAY) { _paths = variant_path;
    } else if (variant_path.get_type() != Variant::Type::STRING) { return Array{}; }
    else { _paths.append(variant_path); }
    TypedArray<Resource> return_paths;
    bool has_name_contains = !name_contains.is_empty();

    for (auto _path: _paths) {
        Error check_error;
        auto dir = DirAccess::open(_path,&check_error);
        if (check_error != OK) {
            WARN_PRINT(vformat("[find_resources_in] Couldn't open dir %s",_path));
            continue;
        }

        dir->list_dir_begin();
        String file_name = dir->get_next();
        while (!file_name.is_empty()) {
            file_name = file_name.trim_suffix(".remap");
            if (file_name.ends_with(".tres"))
                if (!has_name_contains || file_name.contains(name_contains)) {
                    Ref<Resource> loaded_resource = ResourceLoader::load(vformat("%s/%s",_path,file_name));
                    if(loaded_resource.is_valid())
                        return_paths.append(loaded_resource);
                    else
                        WARN_PRINT(vformat("[find_resources_in] Failed loading resource at %s/%s",_path,file_name));
                }
            file_name = dir->get_next();
        }
    }
    return return_paths;
}

bool YEngine::are_resources_virtually_the_same(const Ref<Resource> &resource_a, const Ref<Resource> &resource_b) {
    if (resource_a.is_null() || resource_b.is_null() || !resource_a.is_valid() || !resource_b.is_valid())
        return false;

    List<PropertyInfo> pinfo;
    resource_a->get_property_list(&pinfo);

    for (const PropertyInfo &E : pinfo) {
        if ((E.usage & (PROPERTY_USAGE_CATEGORY | PROPERTY_USAGE_GROUP | PROPERTY_USAGE_SUBGROUP | PROPERTY_USAGE_INTERNAL))
            || (E.name == "script" || E.name == "scripts" || E.name == "resource_path" || E.name == "resource_local_to_scene")) {
            continue;
        }

        bool valid;
        const Variant &resource_a_val = resource_a->get(E.name, &valid);
        if (valid) {
            const Variant &resource_b_val = resource_b->get(E.name, &valid);
            if (!valid || resource_a_val != resource_b_val)
                return false;
        } else {
            return false;
        }
    }
    return true;
}

Vector<String> YEngine::get_diverging_variables_in_resources(const Ref<Resource> &resource_a, const Ref<Resource> &resource_b) {
    Vector<String> return_strings;
    if (resource_a.is_null() || resource_b.is_null() || !resource_a.is_valid() || !resource_b.is_valid())
        return return_strings;

    List<PropertyInfo> pinfo;
    resource_a->get_property_list(&pinfo);

    for (const PropertyInfo &E : pinfo) {
        if ((E.usage & (PROPERTY_USAGE_CATEGORY | PROPERTY_USAGE_GROUP | PROPERTY_USAGE_SUBGROUP | PROPERTY_USAGE_INTERNAL))
            || (E.name == "script" || E.name == "scripts" || E.name == "resource_path" || E.name == "resource_local_to_scene")) {
            continue;
            }

        bool valid;
        const Variant &resource_a_val = resource_a->get(E.name, &valid);
        if (valid) {
            const Variant &resource_b_val = resource_b->get(E.name, &valid);
            if (valid && resource_a_val != resource_b_val)
                return_strings.push_back(E.name);
        }
    }
    return return_strings;
}

String replace_color_bbcode_tags(const String &input) {
    String output = input;
    int start_pos = 0;
    
    while ((start_pos = output.find("[c=", start_pos)) != -1) {
        int end_pos = output.find("]", start_pos);
        if (end_pos == -1) break;

        String color = output.substr(start_pos + 3, end_pos - (start_pos + 3)).strip_edges();

        // Replace "[c=COLOR]" with "[color=COLOR]"
        output = output.replace("[c=" + color + "]", "[color=" + color + "]");

        // Replace "[/c]" with "[/color]"
        int close_tag_pos = output.find("[/c]", end_pos);
        if (close_tag_pos != -1) {
            output = output.replace("[/c]", "[/color]");
            start_pos = close_tag_pos + 8; // Move past the [/color] tag for subsequent finds.
        } else {
            break; // No closing tag found, stop processing
        }
    }

    return output;
}

PackedStringArray YEngine::find_resources_paths_in(const Variant &variant_path, const String &name_contains) {
    PackedStringArray _paths;
    if(variant_path.get_type() == Variant::Type::ARRAY) { _paths = variant_path;
    } else if (variant_path.get_type() != Variant::Type::STRING) { return PackedStringArray{}; }
    else { _paths.append(variant_path); }
    bool has_name_contains = !name_contains.is_empty();
    PackedStringArray return_paths;
    for (auto _path: _paths) {
        Error check_error;
        auto dir = DirAccess::open(_path,&check_error);

        if (check_error != OK) {
            WARN_PRINT(vformat("[find_resources_paths_in] Couldn't open dir %s",_path));
            continue;
        }

        dir->list_dir_begin();
        String file_name = dir->get_next();
        while (!file_name.is_empty()) {
            file_name = file_name.trim_suffix(".remap");
            if (file_name.ends_with(".tres") || file_name.ends_with(".res"))
                if (!has_name_contains || file_name.contains(name_contains))
                    return_paths.append(file_name);
            file_name = dir->get_next();
        }
    }
    return return_paths;
}


void YEngine::request_action_step_approval(YGameAction* action, int step_identifier, const Variant& step_data) {
    if (!get_multiplayer()->has_multiplayer_peer() || get_multiplayer()->get_unique_id() == 1) {
        return; // Don't send if we're the server
    }

    Array p_arguments;
    p_arguments.push_back(action->get_unique_id());
    p_arguments.push_back(step_identifier);
    p_arguments.push_back(step_data);
    p_arguments.push_back(get_multiplayer()->get_unique_id());
    int argcount = p_arguments.size();
    const Variant **argptrs = (const Variant **)alloca(sizeof(Variant *) * argcount);
    for (int i = 0; i < argcount; i++) {
        argptrs[i] = &p_arguments[i];
    }
    rpcp(1, "_rpc_request_action_step_approval", argptrs, argcount);
}

void YEngine::broadcast_action_step(YGameAction* action, int step_identifier, const Variant& step_data) {
    if (!get_multiplayer()->has_multiplayer_peer() || get_multiplayer()->get_unique_id() != 1) {
        return; // Only server can broadcast
    }

    Array p_arguments;
    p_arguments.push_back(action->get_unique_id());
    p_arguments.push_back(step_identifier);
    p_arguments.push_back(step_data);
    int argcount = p_arguments.size();
    const Variant **argptrs = (const Variant **)alloca(sizeof(Variant *) * argcount);
    for (int i = 0; i < argcount; i++) {
        argptrs[i] = &p_arguments[i];
    }
    rpcp(0, "_rpc_apply_action_step", argptrs, argcount);
}

void YEngine::_rpc_request_action_step_approval(int action_id, int step_identifier, const Variant& step_data, int sender_id) {
    if (get_multiplayer()->get_unique_id() != 1) {
        return; // Only server should handle approval requests
    }

    // Find the action in the game state
    if (ygamestate != nullptr) {
        // Check current action
        if (ygamestate->has_current_game_action() && ygamestate->get_current_game_action()->get_unique_id() == action_id) {
            bool approved = true;
            ygamestate->get_current_game_action()->GDVIRTUAL_CALL(_step_request_approval, step_identifier, sender_id, approved);
            if (approved) {
                ygamestate->get_current_game_action()->register_step(step_identifier, step_data);
            }
            return;
        }

        // Check parallel actions
        for (int i = 0; i < ygamestate->get_parallel_action_count(); i++) {
            Ref<YGameAction> action = ygamestate->get_parallel_action(i);
            if (action.is_valid() && action->get_unique_id() == action_id) {
                bool approved = true;
                action->GDVIRTUAL_CALL(_step_request_approval, step_identifier, sender_id, approved);
                if (approved) {
                    action->register_step(step_identifier, step_data);
                }
                return;
            }
        }
    }
}

void YEngine::_rpc_apply_action_step(int action_id, int step_identifier, const Variant& step_data) {
    if (get_multiplayer()->get_unique_id() == 1) {
        return; // Server already applied the step
    }

    // Find the action in the game state
    if (ygamestate != nullptr) {
        // Check current action
        if (ygamestate->has_current_game_action() && ygamestate->get_current_game_action()->get_unique_id() == action_id) {
            ygamestate->get_current_game_action()->register_step(step_identifier, step_data);
            return;
        }

        // Check parallel actions
        for (int i = 0; i < ygamestate->get_parallel_action_count(); i++) {
            Ref<YGameAction> action = ygamestate->get_parallel_action(i);
            if (action.is_valid() && action->get_unique_id() == action_id) {
                action->register_step(step_identifier, step_data);
                return;
            }
        }
    }
}

void YEngine::broadcast_game_action(const Dictionary& action_data) {
    if (!get_multiplayer()->has_multiplayer_peer() || get_multiplayer()->get_unique_id() != 1) {
        return; // Only server can broadcast
    }
    
    // Broadcast to all clients
    rpc("_rpc_register_game_action", action_data);
}

void YEngine::_rpc_register_game_action(const Dictionary& action_data) {
    if (!get_multiplayer()->has_multiplayer_peer() || ygamestate == nullptr) {
        return;
    }

    // Only process if we're a client
    if (get_multiplayer()->get_unique_id() == 1) {
        return;
    }

    if (action_data.has("a_list_type")) {
        int a_list_type = action_data["a_list_type"];
        Vector<Ref<YGameAction>>* list_to_add_to = nullptr;
        if (a_list_type == 1) {
            list_to_add_to = &ygamestate->overriding_game_actions;
        } else if (a_list_type == 2) {
            list_to_add_to = &ygamestate->future_game_actions;
        } else if (a_list_type == 3) {
            list_to_add_to = &ygamestate->past_game_actions;
        }else if (a_list_type == 4) {
            list_to_add_to = &ygamestate->current_parallel_actions;
        }else if (a_list_type == 5) {
            list_to_add_to = &ygamestate->future_parallel_actions;
        }
        if (list_to_add_to != nullptr) {
            ygamestate->deserialize_individual_game_action_into(*list_to_add_to, action_data, false);
        }
    }
}

Dictionary YEngine::create_rpc_dictionary_config(MultiplayerAPI::RPCMode p_rpc_mode,
                                            MultiplayerPeer::TransferMode p_transfer_mode, bool p_call_local,
                                            int p_channel) {
    Dictionary config;
    config["rpc_mode"] = p_rpc_mode;
    config["transfer_mode"] = p_transfer_mode;
    config["call_local"] = p_call_local;
    config["channel"] = p_channel;
    return config;
}