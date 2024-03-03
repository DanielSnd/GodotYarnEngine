//
// Created by Daniel on 2024-02-26.
//

#ifndef YARNSAVE_H
#define YARNSAVE_H
#include "core/object/object.h"
#include "core/object/ref_counted.h"


class YSave : public RefCounted {
    GDCLASS(YSave, RefCounted);

protected:
    static Ref<YSave> singleton;

    static void _bind_methods();
    HashMap<int,int> registered_events;
    bool has_started = false;
    void create_save_backup();
    float pause_independent_time = 0;
    Variant get_from_settings(const String &p_save_key, const Variant &p_save_default);
    YSave *set_to_settings(const String &p_save_key, const Variant &p_save_value);

    YSave *reset_save();

    void save_settings();
    Variant get_from_save(const String &p_save_key, const Variant &p_save_default);
    YSave *set_to_save(const String &p_save_key, const Variant &p_save_value);

    Variant get_from_save_dictionary(const Dictionary &save_dictionary, const String &p_save_key,
                                            const Variant &p_save_default);

    YSave *set_to_save_dictionary(Dictionary &save_dictionary, const String &p_save_key,
                                  const Variant &p_save_value);

    YSave *add_to_save_data(String p_save_key, Variant p_save_value);
    void actually_save();
    bool load_settings();
    void execute_save();
public:

    float time_started = 0;
    void process(float pause_independent_time);
    String settings_path = "user://game.settings";
    String get_settings_path() { return settings_path; }
    void set_settings_path(String val) { settings_path = val; }

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

    Dictionary settings_data;
    Dictionary get_settings_data() const {return settings_data;}
    void set_settings_data(const Dictionary &val) { settings_data = val;};

    Dictionary save_data;
    Dictionary get_save_data() const {return save_data;}
    void set_save_data(const Dictionary &val) { save_data = val;};

    bool request_load();

    void load_registered_events_from_save_data();
    void set_registered_events_to_save_data();
    int get_registered_event_time(int event_id) { return registered_events.has(event_id) ? registered_events[event_id] : -1; }
    void set_registered_event_time(int event_id, int time_happened) { registered_events[event_id] = time_happened;}
    void remove_registered_event(int event_id) { registered_events.erase(event_id);}
    bool has_time_elapsed_since_registered_event(int event_id, int current_time, int elapsed_time) {
        int event_happened_time = registered_events.has(event_id) ? registered_events[event_id] : -1;
        return event_happened_time == -1 ? false : event_happened_time + elapsed_time < current_time;
    }

    Array get_registered_events() {
        Array return_array;
        for (auto _event: registered_events) {
            return_array.append(Vector2i{_event.key,_event.value});
        }
        return return_array;
    }

    void set_save_detail(String save_detail, Variant detail_value);

    Dictionary load_save_details(const String &path = "");

    void request_save(bool immediate);

    static YSave *get_singleton();

    static void set_singleton(const Ref<YSave> &ref) {
        singleton = ref;
    }

    YSave();
    ~YSave();
};



#endif //YARNSAVE_H
