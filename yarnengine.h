#ifndef GODOT_YARNENGINE_H
#define GODOT_YARNENGINE_H

#include "core/object/ref_counted.h"
#include "scene/main/node.h"
#include "scene/main/window.h"

class YarnEngine : public Node {
    GDCLASS(YarnEngine, Node);

protected:
    void setup_node();

    static void _bind_methods();

    Node *get_current_scene();

    void _notification(int p_what);

    bool attempt_pause();

    static YarnEngine* singleton;
    HashMap<StringName, Node *> othersingletons;

    float amount_when_paused;
    float amount_when_unpaused;
    float amount_time_last_frame;
    float last_time_ended_pause;
public:
    bool already_setup_in_tree = false;

    bool is_paused;
    bool get_is_paused() {return is_paused;}
    void set_is_paused(bool val);

    float time;
    float get_time() {return time;}
    void set_time(float val) {time = val;}

    float pause_independent_time;
    float get_pause_independent_time() {return pause_independent_time;}
    void set_pause_independent_time(float val) {pause_independent_time = val;}
    void handle_time_setting();

    bool has_time_elapsed(float test_time, float interval);

    bool has_pause_independent_time_elapsed(float test_time, float interval);

    Variant test_callback_thing(const Callable &p_callable);
    YarnEngine();
    ~YarnEngine();
    static YarnEngine* get_singleton();
};

#endif