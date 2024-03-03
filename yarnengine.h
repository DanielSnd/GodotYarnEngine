#ifndef GODOT_YARNENGINE_H
#define GODOT_YARNENGINE_H

#include "yarnsave.h"
#include "yarntime.h"
#include "scene/main/node.h"
#include "scene/main/window.h"

class YEngine : public Node {
    GDCLASS(YEngine, Node);

protected:
    void setup_node();

    static void _bind_methods();

    Variant execute_button_click_callable_if_modulate(const Callable &p_callable, Control *p_control);

    Variant execute_button_click_callable(const Callable &p_callable);

    Node *get_current_scene();

    void _notification(int p_what);

    static YEngine* singleton;
    HashMap<StringName, Node *> othersingletons;

public:
    bool already_setup_in_tree = false;

    Callable button_click_callable(const Callable &p_callable);
    Callable button_click_callable_if_modulate(const Callable &p_callable, Control* p_control);

    List<Node *> menu_stack;

    int get_menu_stack_size() const {return menu_stack.size();}

    TypedArray<Node> get_menu_stack() {
        TypedArray<Node> btns;
        for (int i = 0; i < menu_stack.size(); ++i) {
            btns.append(menu_stack[i]);
        }
        return btns;
    }

    bool is_top_of_menu_stack(Node* test_menu) const {
        return menu_stack[menu_stack.size()-1] == test_menu;
    }

    void add_to_menu_stack(Node* adding_menu) {
        for (int i = 0; i < menu_stack.size(); ++i)
            if (menu_stack[i] == adding_menu) return;
        menu_stack.push_back(adding_menu);
    }

    void remove_from_menu_stack(Node* removing_menu) {
        for (int i = 0; i < menu_stack.size(); ++i)
            if (menu_stack[i] == removing_menu) {
                menu_stack.erase(removing_menu);
                return;
            }
    }

    float last_button_click_time;
    float get_last_button_click_time() {return last_button_click_time;}
    void set_last_button_click_time(const float val) {last_button_click_time = val;}

    bool can_button_click;
    bool get_can_button_click() {return can_button_click;}
    void set_can_button_click(const bool val) {can_button_click = val;}

    void do_process();

    YEngine();
    ~YEngine();

    YSave* ysave;
    YTime* ytime;
    void add_setting(const String& name, const Variant& default_value, Variant::Type type,
            PropertyHint hint = PROPERTY_HINT_NONE, const String& hint_string = "",
            int usage = PROPERTY_USAGE_DEFAULT, bool restart_if_changed = false);

    TypedArray<Resource> find_resources_in(const Variant &path, const String &name_contains = "");

    PackedStringArray find_resources_paths_in(const Variant &path, const String &name_contains);

    static YEngine* get_singleton();
};

#endif