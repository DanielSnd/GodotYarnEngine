//
// Created by Daniel on 2024-02-26.
//

#include "yarnsave.h"
#include "core/io/dir_access.h"
#include "core/io/file_access.h"
#include "core/io/json.h"


YSave* YSave::singleton = nullptr;

YSave *YSave::get_singleton() {
    return singleton;
}

void YSave::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_save_path", "save_path"), &YSave::set_save_path);
    ClassDB::bind_method(D_METHOD("get_save_path"), &YSave::get_save_path);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "save_path"), "set_save_path", "get_save_path");

    ClassDB::bind_method(D_METHOD("set_settings_path", "settings_path"), &YSave::set_settings_path);
    ClassDB::bind_method(D_METHOD("get_settings_path"), &YSave::get_settings_path);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "settings_path"), "set_settings_path", "get_settings_path");

    ClassDB::bind_method(D_METHOD("set_save_backup_path", "save_backup_path"), &YSave::set_save_backup_path);
    ClassDB::bind_method(D_METHOD("get_save_backup_path"), &YSave::get_save_backup_path);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "save_backup_path"), "set_save_backup_path", "get_save_backup_path");
    
    ClassDB::bind_method(D_METHOD("set_save_requested", "save_requested"), &YSave::set_save_requested);
    ClassDB::bind_method(D_METHOD("get_save_requested"), &YSave::get_save_requested);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "save_requested"), "set_save_requested", "get_save_requested");
    
    ClassDB::bind_method(D_METHOD("set_save_data", "save_data"), &YSave::set_save_data);
    ClassDB::bind_method(D_METHOD("get_save_data"), &YSave::get_save_data);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "save_data"), "set_save_data", "get_save_data");

    ClassDB::bind_method(D_METHOD("get_from_settings", "settings_key","default"), &YSave::get_from_settings);

    ClassDB::bind_method(D_METHOD("set_to_settings", "settings_key","setting"), &YSave::set_to_settings);

    ClassDB::bind_method(D_METHOD("save_settings"), &YSave::save_settings);
    ClassDB::bind_method(D_METHOD("load_settings"), &YSave::load_settings);

    ClassDB::bind_method(D_METHOD("load_save_details", "desired_path"), &YSave::load_save_details,DEFVAL(""));

    ClassDB::bind_method(D_METHOD("set_save_detail", "save_detail","detail_value"), &YSave::set_save_detail);

    ClassDB::bind_method(D_METHOD("set_to_save", "save_key","save"), &YSave::set_to_save);
    ClassDB::bind_method(D_METHOD("set_to_save_if_higher", "save_key","save_value"), &YSave::set_to_save_if_higher);
    ClassDB::bind_method(D_METHOD("set_to_save_if_lower", "save_key","save_value"), &YSave::set_to_save_if_lower);
    ClassDB::bind_method(D_METHOD("increment_to_save", "save_key","increment"), &YSave::increment_to_save);

    ClassDB::bind_method(D_METHOD("get_from_save", "save_key","default"), &YSave::get_from_save);

    ClassDB::bind_method(D_METHOD("request_save", "immediate"), &YSave::request_save,DEFVAL(false));

    ClassDB::bind_method(D_METHOD("request_load"), &YSave::request_load);
    ClassDB::bind_method(D_METHOD("reset_save"), &YSave::reset_save);

    ClassDB::bind_method(D_METHOD("load_registered_events_from_save_data"), &YSave::load_registered_events_from_save_data);
    ClassDB::bind_method(D_METHOD("set_registered_events_to_save_data"), &YSave::set_registered_events_to_save_data);

    ClassDB::bind_method(D_METHOD("get_registered_events"), &YSave::get_registered_events);
    ClassDB::bind_method(D_METHOD("get_registered_event_time","event_id"), &YSave::get_registered_event_time);
    ClassDB::bind_method(D_METHOD("set_registered_event_time","event_id","time_happened"), &YSave::set_registered_event_time);
    ClassDB::bind_method(D_METHOD("remove_registered_event","event_id"), &YSave::remove_registered_event);
    ClassDB::bind_method(D_METHOD("has_time_elapsed_since_registered_event","event_id","current_time","elapsed_time"), &YSave::has_time_elapsed_since_registered_event);
    ClassDB::bind_method(D_METHOD("has_registered_event","event_id"), &YSave::has_registered_event);
    ClassDB::bind_method(D_METHOD("serialize_registered_events"), &YSave::serialize_registered_events);
    ClassDB::bind_method(D_METHOD("deserialize_registered_events","registered_events_array"), &YSave::deserialize_registered_events);


    ClassDB::bind_method(D_METHOD("register_event_callback","node","event_id","callback"), &YSave::register_event_callback);
    ClassDB::bind_method(D_METHOD("clear_registered_event_callbacks","node","event_id"), &YSave::clear_registered_event_callbacks_node,DEFVAL(-1));

    ClassDB::bind_method(D_METHOD("get_registered_event_callbacks"), &YSave::get_registered_event_callbacks);

    ADD_SIGNAL(MethodInfo("executed_save_reset"));
    ADD_SIGNAL(MethodInfo("before_prepare_save"));
    ADD_SIGNAL(MethodInfo("prepare_save"));
    ADD_SIGNAL(MethodInfo("saved", PropertyInfo(Variant::STRING, "save_path")));
    ADD_SIGNAL(MethodInfo("loaded_save", PropertyInfo(Variant::DICTIONARY, "save_data")));

    ClassDB::bind_method(D_METHOD("set_is_debugging", "is_debugging"), &YSave::set_is_debugging);
    ClassDB::bind_method(D_METHOD("get_is_debugging"), &YSave::get_is_debugging);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_debugging"), "set_is_debugging", "get_is_debugging");
}

void YSave::process(float _pause_independent_time) {
    pause_independent_time = _pause_independent_time;
    if (!has_started) has_started=true;
    if (save_next_frame) {
        save_next_frame = false;
        actually_save();
    } else if (!save_next_frame && save_requested && (last_time_save_requested + 0.33 < pause_independent_time)) {
        save_requested = false;
        execute_save();
    }
}

void YSave::create_save_backup() {
    auto dir = DirAccess::create(DirAccess::ACCESS_USERDATA);
    if (!dir->file_exists(save_path)) {
        return;
    }
    auto file = FileAccess::open(save_path, FileAccess::READ);
    auto file_string = file->get_as_text();
    file->close();
    if (!file_string.is_empty()) {
        auto write = FileAccess::open(save_backup_path, FileAccess::WRITE);
        write->store_string(file_string);
        write->close();
    }
}

Variant YSave::get_from_settings(const String &p_save_key, const Variant &p_save_default) {
    return get_from_save_dictionary(settings_data, p_save_key,p_save_default);
}
YSave *YSave::set_to_settings(const String &p_save_key, const Variant &p_save_value) {
    return set_to_save_dictionary(settings_data,p_save_key,p_save_value);
}

YSave *YSave::reset_save() {
    if (!has_started || Engine::get_singleton()->is_editor_hint()) {
        return this;
    }
    create_save_backup();
    save_data.clear();
    auto file = FileAccess::open(save_path,FileAccess::WRITE);
    file->store_string(JSON::stringify(save_data));
    file->close();
    emit_signal(SNAME("loaded_save"),save_data);
    emit_signal(SNAME("executed_save_reset"));
    return this;
}

void YSave::save_settings() {
    if (!has_started || Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    auto file = FileAccess::open(settings_path,FileAccess::WRITE);
    file->store_string(JSON::stringify(settings_data));
    file->close();
    emit_signal(SNAME("saved"),settings_path);
}

bool YSave::load_settings() {
    auto dir = FileAccess::create_for_path(settings_path);
    if (!dir->file_exists(settings_path)) {
        print_line("Settings doesn't exist at path ",settings_path);
        return false;
    }
    auto file = FileAccess::open(settings_path, FileAccess::READ);
    auto file_string = file->get_as_text();
    file->close();
    if (file_string.is_empty()) {
        print_line("File string is empty");
        return false;
    }
    Ref<JSON> jason;
    jason.instantiate();
    Error error = jason->parse(file_string);
    if(error != Error::OK) {
        print_line(vformat("Parse JSON failed. Error at line %d: %s", jason->get_error_line(), jason->get_error_message()));
        return false;
    }
    settings_data = jason->get_data();
    return true;
}

YSave::YSave() {
    singleton = this;
}

YSave::~YSave() {
    if (singleton == this)
        singleton = nullptr;
    if (reg_event_callbacks.size() > 0) {
        reg_event_callbacks.clear();
    }
}

Variant YSave::get_from_save(const String &p_save_key,const Variant &p_save_default) {
    return get_from_save_dictionary(save_data,p_save_key,p_save_default);
}

YSave *YSave::set_to_save(const String &p_save_key, const Variant &p_save_value) {
    return set_to_save_dictionary(save_data,p_save_key,p_save_value);
}

YSave *YSave::set_to_save_if_higher(const String &p_save_key, const Variant &p_save_value) {
    Variant amount = get_from_save_dictionary(save_data, p_save_key, INT16_MIN);
    if (p_save_value.get_type() == Variant::FLOAT && amount.get_type() == Variant::FLOAT) {
        float amount_float = amount;
        float increment_float = p_save_value;
        if (amount_float < increment_float) {
            set_to_save(p_save_key, increment_float);
        }
    }
    else if (p_save_value.get_type() == Variant::INT && amount.get_type() == Variant::INT) {
        int amount_int = amount;
        int increment_int = p_save_value;
        if (amount_int < increment_int) {
            set_to_save(p_save_key, increment_int);
        }
    }
    else {
        float amount_float = amount;
        float increment_float = p_save_value;
        if (amount_float < increment_float) {
            set_to_save(p_save_key, increment_float);
        }
    }
    return this;
}

YSave *YSave::set_to_save_if_lower(const String &p_save_key, const Variant &p_save_value) {
    Variant amount = get_from_save_dictionary(save_data, p_save_key, INT16_MAX);
    if (p_save_value.get_type() == Variant::FLOAT && amount.get_type() == Variant::FLOAT) {
        float amount_float = amount;
        float increment_float = p_save_value;
        if (amount_float > increment_float) {
            set_to_save(p_save_key, increment_float);
        }
    }
    else if (p_save_value.get_type() == Variant::INT && amount.get_type() == Variant::INT) {
        int amount_int = amount;
        int increment_int = p_save_value;
        if (amount_int > increment_int) {
            set_to_save(p_save_key, increment_int);
        }
    }
    else {
        float amount_float = amount;
        float increment_float = p_save_value;
        if (amount_float > increment_float) {
            set_to_save(p_save_key, increment_float);
        }
    }
    return this;
}

YSave *YSave::increment_to_save(const String &p_save_key, const Variant &p_increment) {
    Variant amount = get_from_save_dictionary(save_data, p_save_key, 0);
    if (p_increment.get_type() == Variant::FLOAT && amount.get_type() == Variant::FLOAT) {
        float amount_float = amount;
        float increment_float = p_increment;
        set_to_save(p_save_key, amount_float + increment_float);
    }
    else if (p_increment.get_type() == Variant::INT && amount.get_type() == Variant::INT) {
        int amount_int = amount;
        int increment_int = p_increment;
        set_to_save(p_save_key, amount_int + increment_int);
    }
    else {
        float amount_float = amount;
        float increment_float = p_increment;
        set_to_save(p_save_key, amount_float + increment_float);
    }
    return this;
}

Variant YSave::get_from_save_dictionary(const Dictionary &save_dictionary, const String &p_save_key,const Variant &p_save_default) {
    if (!p_save_key.contains("/")) return save_dictionary.get(p_save_key,p_save_default);
    auto splitted = p_save_key.split("/",false);
    Dictionary desired_dict = save_dictionary;
    for (int i = 0; i < splitted.size(); ++i) {
        if (i == splitted.size()-1) {
            Variant found_variant = desired_dict.get(splitted[i],p_save_default);
            if (found_variant.get_type() != p_save_default.get_type()) {
                if (found_variant.get_type() == Variant::STRING && p_save_default.get_type() == Variant::VECTOR3) {
                    //TODO: HANDLE CONVERTING FROM STRING TO OTHER POSSIBLE VARIANTS.
                }
            }
            return desired_dict.get(splitted[i],p_save_default);
        }
        if (!desired_dict.has(splitted[i])) return p_save_default;
        desired_dict = desired_dict[splitted[i]];
    }
    return save_dictionary.get(p_save_key,p_save_default);
}

YSave *YSave::set_to_save_dictionary(Dictionary &save_dictionary,const  String &p_save_key,const  Variant &p_save_value) {
    if (!p_save_key.contains("/")) {
        save_dictionary[p_save_key] = p_save_value;
        return this;
    }
    Dictionary desired_dict = save_dictionary;
    auto splitted = p_save_key.split("/",false);
    for (int i = 0; i < splitted.size(); ++i) {
        if (i == splitted.size()-1) {
            desired_dict[splitted[i]] = p_save_value;
            return this;
        }
        if (!desired_dict.has(splitted[i]))
            desired_dict[splitted[i]] = Dictionary{};
        if (desired_dict.has(splitted[i]))
            desired_dict = desired_dict[splitted[i]];
        else
            ERR_PRINT(vformat("FAILED TO ADD DICTIONARY WITH STRING %s original %s",splitted,p_save_key));
    }
    return this;
}

YSave *YSave::add_to_save_data(String p_save_key, Variant p_save_value) {
    if (p_save_key.contains("/")) {
        auto splitted = p_save_key.split("/",false);
        Dictionary desired_dict = save_data;
        for (int i = 0; i < splitted.size(); ++i) {
            if (i == splitted.size()-1) {
                float initial_value = desired_dict.get(splitted[i],0.0);
                desired_dict[splitted[i]] = static_cast<float>(p_save_value) + initial_value;
                return this;
            }
            if (!desired_dict.has(splitted[i]))
                desired_dict[splitted[i]] = Dictionary{};
            if (desired_dict.has(splitted[i])) {
                desired_dict = desired_dict[splitted[i]];
            } else {
                ERR_PRINT(vformat("FAILED TO ADD DICTIONARY WITH STRING %s original %s",splitted,p_save_key));
            }
        }
    }
    else {
        if (save_data.has(p_save_key)) {
            auto current = save_data[p_save_key];
            if (current.is_num() && p_save_value.is_num()) {
                save_data[p_save_key] = Variant{static_cast<float>(current) + static_cast<float>(p_save_value)};
            }
        } else {
            save_data[p_save_key] = p_save_value;
        }
    }
    return this;
}


void YSave::execute_save() {
    emit_signal(SNAME("before_prepare_save"));
    emit_signal(SNAME("prepare_save"));
    save_next_frame = true;
}

void YSave::actually_save() {
    if (!has_started || Engine::get_singleton()->is_editor_hint() || pause_independent_time == 0 || (time_started + 3 > pause_independent_time)) {
        save_next_frame=false;
        save_requested = false;
        return;
    }
    auto file = FileAccess::open(save_path,FileAccess::WRITE);
    file->store_string(JSON::stringify(save_data));
    file->close();
    emit_signal(SNAME("saved"),save_path);
    print_line("Saved ",save_path);
}

bool YSave::request_load() {
    auto dir = FileAccess::create_for_path(save_path);
    if (!dir->file_exists(save_path)) {
        print_line("Save doesn't exist at path ",save_path);
        emit_signal(SNAME("loaded_save"),save_data);
        return false;
    }
    auto file = FileAccess::open(save_path, FileAccess::READ);
    auto file_string = file->get_as_text();
    file->close();
    if (file_string.is_empty()) {
        print_line("File string is empty");
        emit_signal(SNAME("loaded_save"),save_data);
        return false;
    }
    Ref<JSON> jason;
    jason.instantiate();
    Error error = jason->parse(file_string);
    if(error != Error::OK) {
        print_line(vformat("Parse JSON failed. Error at line %d: %s", jason->get_error_line(), jason->get_error_message()));
        emit_signal(SNAME("loaded_save"),save_data);
        return false;
    }
    save_data = jason->get_data();
    // print_line("Jason loaded data ",save_data);
    emit_signal(SNAME("loaded_save"),save_data);
    return true;
}

void YSave::deserialize_registered_events(Array _events_save) {
    registered_events.clear();
    _events_save = _events_save.duplicate();
    while (_events_save.size() > 0) {
        if (_events_save.size() <= 1)
            _events_save.clear();
        else {
            const int key = _events_save.pop_front();
            const int val = _events_save.pop_front();
            registered_events[key] = val;
        }
    }
}

void YSave::load_registered_events_from_save_data() {
    registered_events.clear();
    if (!save_data.has("regevents")) return;
    Array _events_save = save_data["regevents"];
    deserialize_registered_events(_events_save);
}

void YSave::set_registered_events_to_save_data() {
    save_data["regevents"] = serialize_registered_events();
}

Array YSave::serialize_registered_events() {
    Array _events_save;
    for (const auto& registered_event: registered_events) {
        _events_save.append(registered_event.key);
        _events_save.append(registered_event.value);
    }
    return _events_save;
}

void YSave::clear_registered_event_callbacks_node(Node *_reference, int _event_id) {
    if (_reference != nullptr)
        clear_registered_event_callbacks(_reference->get_instance_id(),_event_id);
}

void YSave::clear_registered_event_callbacks(ObjectID p_node_inst_id,int _event_id) {
    Array event_ids_to_remove;
    Array ints_to_remove;
    if (_event_id != -1) {
        if (reg_event_callbacks.has(_event_id)) {
            int index = 0;
            for (auto _regEventCallback_instance: reg_event_callbacks[_event_id].callbacks) {
                if (_regEventCallback_instance.node_inst_id == p_node_inst_id) {
                    ints_to_remove.push_front(index);
                }
                index++;
            }
            for (int i = 0; i < ints_to_remove.size(); ++i) {
                reg_event_callbacks[_event_id].callbacks.remove_at(ints_to_remove[i]);
            }
            ints_to_remove.clear();
            if (reg_event_callbacks[_event_id].callbacks.size() == 0) {
                reg_event_callbacks.erase(_event_id);
            }
        }
    } else {
        for (auto _regEventCallback: reg_event_callbacks) {
            int _index = 0;
            for (auto _regEventCallback_instance: _regEventCallback.value.callbacks) {
                if (_regEventCallback_instance.node_inst_id == p_node_inst_id) {
                    ints_to_remove.push_front(_index);
                }
                _index++;
            }
            for (int i = 0; i < ints_to_remove.size(); ++i) {
                _regEventCallback.value.callbacks.remove_at(ints_to_remove[i]);
            }
            ints_to_remove.clear();
            if (_regEventCallback.value.callbacks.size() == 0) {
                event_ids_to_remove.push_front(_regEventCallback.key);
            }
        }
    }
    if (event_ids_to_remove.size()>0) {
        for (int i = 0; i < event_ids_to_remove.size(); ++i) {
            reg_event_callbacks.erase(event_ids_to_remove[i]);
        }
        event_ids_to_remove.clear();
    }
}

void YSave::register_event_callback(Node *p_reference, int _event_id, const Callable &p_callable) {
    if (p_reference == nullptr) {
        return;
    }
    if (registered_events.has(_event_id)) {
        if (is_debugging) {
            print_line(vformat("[YSave] Event %d already registered, skipping callback registration", _event_id));
        }
        WARN_PRINT(vformat("Trying to register a callback for an already existing event_id %d. Callbacks only happen the first time an event is set.",_event_id));
        return;
    }
    
    if (is_debugging) {
        print_line(vformat("[YSave] Registering callback for event %d", _event_id));
    }
    
    ObjectID _node_id = p_reference->get_instance_id();
    if (!reg_event_callbacks.has(_event_id))
        reg_event_callbacks[_event_id] = RegEventCallback{_event_id};
    if (!count_node_callbacks.has(_node_id)) {
        count_node_callbacks[_node_id] = 0;
        p_reference->connect("tree_exiting", callable_mp(this, &YSave::clear_registered_event_callbacks).bind(_node_id,-1), CONNECT_ONE_SHOT);
    }
    count_node_callbacks[_node_id] += 1;
    reg_event_callbacks[_event_id].callbacks.append(RegEventCallback::RegEventCallbackInstance(_node_id,p_callable));
    
    if (is_debugging) {
        print_line(vformat("[YSave] Successfully registered callback for event %d. Total callbacks for this event: %d", 
            _event_id, reg_event_callbacks[_event_id].callbacks.size()));
    }
}

void YSave::set_save_detail(String save_detail,Variant detail_value) {
    if (!save_data.has("save_details")) save_data["save_details"] = Dictionary{};
    Dictionary _details = save_data["save_details"];
    _details[save_detail] = detail_value;
}

Dictionary YSave::load_save_details(const String &_path) {
    bool is_path_empty = _path.is_empty();
    auto dir = FileAccess::create_for_path(is_path_empty ? save_path : _path);
    if (!dir->file_exists(is_path_empty ? save_path : _path)) return Dictionary{};
    auto file = FileAccess::open(is_path_empty ? save_path : _path, FileAccess::READ);
    auto file_string = file->get_as_text();
    file->close();
    if (file_string.is_empty()) return Dictionary{};
    Dictionary dict = JSON::parse_string(file_string);
    return dict.get("save_details",Dictionary{});
}

void YSave::request_save(bool immediate = false) {
    if (!has_started) return;
    if(immediate) {
        emit_signal(SNAME("before_prepare_save"));
        emit_signal(SNAME("prepare_save"));
        actually_save();
    } else {
        save_requested = true;
        last_time_save_requested = pause_independent_time;
    }
}

Dictionary YSave::get_registered_event_callbacks() const {
    Dictionary result;
    
    for (const KeyValue<int, RegEventCallback> &E : reg_event_callbacks) {
        Array callbacks;
        for (const RegEventCallback::RegEventCallbackInstance &callback : E.value.callbacks) {
            Dictionary callback_info;
            callback_info["node_id"] = callback.node_inst_id;
            callback_info["callable"] = callback.callable;
            callbacks.push_back(callback_info);
        }
        result[E.key] = callbacks;
    }
    
    return result;
}
