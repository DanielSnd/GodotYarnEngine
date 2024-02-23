#ifndef GODOT_YARNENGINE_H
#define GODOT_YARNENGINE_H

#include "core/object/ref_counted.h"
#include "scene/main/node.h"
#include "scene/main/window.h"

class YarnEngine : public Node {
    GDCLASS(YarnEngine, Node);

protected:
    void setup_node();

    void create_save_backup();

    Variant get_from_save(String p_save_key, Variant p_save_default);

    YarnEngine *set_to_save(String p_save_key, Variant p_save_value);

    YarnEngine *add_to_save_data(String p_save_key, Variant p_save_value);

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

    String save_path = "user://game.save";
    String get_save_path() {return save_path;}
    void set_save_path(String val) {save_path = val;}

    String save_backup_path = "user://game.save.backup";
    String get_save_backup_path() {return save_backup_path;}
    void set_save_backup_path(String val) {save_backup_path = val;}

    String save_reset_backup_path = "user://game.reset.save.backup";
    String get_save_reset_backup_path() {return save_reset_backup_path;}
    void set_save_reset_backup_path(String val) {save_reset_backup_path = val;}

    bool save_requested;
    bool get_save_requested() {return save_requested;}
    void set_save_requested(bool val) {save_requested = val;}

    float last_time_save_requested;
    bool save_next_frame;
    Dictionary save_data;
    Dictionary get_save_data() {return save_data;}
    void set_save_data(const Dictionary &val) { save_data = val;};


    bool is_paused;
    bool get_is_paused() {return is_paused;}
    void set_is_paused(bool val);

    float time;
    float get_time() {return time;}
    void set_time(const float val) {time = val;}

    float pause_independent_time;
    float get_pause_independent_time() {return pause_independent_time;}
    void set_pause_independent_time(const float val) {pause_independent_time = val;}
    void handle_time_setting();

    void execute_save();

    void actually_save();

    bool request_load();

    void request_save(bool immediate);

    bool has_time_elapsed(float test_time, float interval);

    bool has_pause_independent_time_elapsed(float test_time, float interval);

    Variant test_callback_thing(const Callable &p_callable);
    YarnEngine();
    ~YarnEngine();
    static YarnEngine* get_singleton();
};

#endif