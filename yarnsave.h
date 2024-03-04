//
// Created by Daniel on 2024-02-26.
//

#ifndef YARNSAVE_H
#define YARNSAVE_H
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "scene/main/node.h"


class YSave : public RefCounted {
    GDCLASS(YSave, RefCounted);

private:

protected:
    static Ref<YSave> singleton;

    static void _bind_methods();

    struct RegEventCallback {
        struct RegEventCallbackInstance {
            ObjectID node_inst_id;
            Callable callable;
            RegEventCallbackInstance() : node_inst_id(), callable() {}
            RegEventCallbackInstance(ObjectID node_inst_id, const Callable &callable) : node_inst_id(node_inst_id), callable(callable) {}
        };
        int event_id;
        Vector<RegEventCallbackInstance> callbacks;
        RegEventCallback() : event_id(0) {}
        explicit RegEventCallback(int event_id) : event_id(event_id) {}
    };
    HashMap<int,RegEventCallback> reg_event_callbacks;
    HashMap<ObjectID,int> count_node_callbacks;
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

    void register_event_callback(Node* _reference, int _event_id, const Callable &callable);
    void clear_registered_event_callbacks(ObjectID p_node_inst_id, int _event_id);
    void clear_registered_event_callbacks_node(Node* _reference, int _event_id);

    int get_registered_event_time(int event_id) { return registered_events.has(event_id) ? registered_events[event_id] : -1; }
    void set_registered_event_time(int event_id, int time_happened) {
        if (time_happened == -1) {
            registered_events.erase(event_id);
            return;
        }
        bool first_time_happened = !registered_events.has(event_id);
        registered_events[event_id] = time_happened;
        if (first_time_happened && reg_event_callbacks.has(event_id)) {
            auto _reg_event_callbacks = reg_event_callbacks[event_id];
            Vector<Callable> callables_to_call;
            for (const auto& callback: _reg_event_callbacks.callbacks) {
                if (callback.callable.is_valid()) {
                    callables_to_call.append(callback.callable);
                    if (count_node_callbacks.has(callback.node_inst_id)) {
                        const int current_count = count_node_callbacks[callback.node_inst_id];
                        if (current_count - 1 <=0 ) {
                            count_node_callbacks.erase(callback.node_inst_id);
                        } else {
                            count_node_callbacks[callback.node_inst_id] = current_count-1;
                        }
                    }
                }
            }
            _reg_event_callbacks.callbacks.clear();
            reg_event_callbacks.erase(event_id);
            for (const auto& to_call: callables_to_call)
                if (!to_call.is_null() && to_call.is_valid())
                    to_call.call(event_id);
        }
    }

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
