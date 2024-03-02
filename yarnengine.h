#ifndef GODOT_YARNENGINE_H
#define GODOT_YARNENGINE_H

#include "yarnsave.h"
#include "yarntime.h"
#include "core/object/ref_counted.h"
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