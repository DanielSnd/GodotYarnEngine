#include "yarnengine.h"

#include "core/io/dir_access.h"
#include "core/io/json.h"

YarnEngine* YarnEngine::singleton = nullptr;

YarnEngine * YarnEngine::get_singleton() {
    return singleton;
}

void YarnEngine::_bind_methods() {
    ClassDB::bind_method(D_METHOD("setup_node"), &YarnEngine::setup_node);
    ClassDB::bind_method(D_METHOD("get_current_scene"), &YarnEngine::get_current_scene);
    ClassDB::bind_method(D_METHOD("attempt_pause"), &YarnEngine::attempt_pause);

    ClassDB::bind_method(D_METHOD("set_is_paused", "is_paused"), &YarnEngine::set_is_paused);
    ClassDB::bind_method(D_METHOD("get_is_paused"), &YarnEngine::get_is_paused);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "is_paused"), "set_is_paused", "get_is_paused");

    ClassDB::bind_method(D_METHOD("set_time", "time"), &YarnEngine::set_time);
    ClassDB::bind_method(D_METHOD("get_time"), &YarnEngine::get_time);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "time"), "set_time", "get_time");

    ClassDB::bind_method(D_METHOD("set_pause_independent_time", "pause_independent_time"), &YarnEngine::set_pause_independent_time);
    ClassDB::bind_method(D_METHOD("get_pause_independent_time"), &YarnEngine::get_pause_independent_time);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "pause_independent_time"), "set_pause_independent_time", "get_pause_independent_time");

    ClassDB::bind_method(D_METHOD("has_time_elapsed", "time_checking", "interval"), &YarnEngine::has_time_elapsed);
    ClassDB::bind_method(D_METHOD("has_pause_independent_time_elapsed", "time_checking", "interval"), &YarnEngine::has_pause_independent_time_elapsed);

    ClassDB::bind_method(D_METHOD("test_callback_thing", "callable"), &YarnEngine::test_callback_thing);

    ClassDB::bind_method(D_METHOD("set_save_path", "save_path"), &YarnEngine::set_save_path);
    ClassDB::bind_method(D_METHOD("get_save_path"), &YarnEngine::get_save_path);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "save_path"), "set_save_path", "get_save_path");
    
    ClassDB::bind_method(D_METHOD("set_save_backup_path", "save_backup_path"), &YarnEngine::set_save_backup_path);
    ClassDB::bind_method(D_METHOD("get_save_backup_path"), &YarnEngine::get_save_backup_path);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "save_backup_path"), "set_save_backup_path", "get_save_backup_path");
    
    ClassDB::bind_method(D_METHOD("set_save_requested", "save_requested"), &YarnEngine::set_save_requested);
    ClassDB::bind_method(D_METHOD("get_save_requested"), &YarnEngine::get_save_requested);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "save_requested"), "set_save_requested", "get_save_requested");
    
    ClassDB::bind_method(D_METHOD("set_save_data", "save_data"), &YarnEngine::set_save_data);
    ClassDB::bind_method(D_METHOD("get_save_data"), &YarnEngine::get_save_data);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "save_data"), "set_save_data", "get_save_data");

    ClassDB::bind_method(D_METHOD("add_to_save_data", "save_key","adding"), &YarnEngine::add_to_save_data);

    ClassDB::bind_method(D_METHOD("set_to_save", "save_key","save"), &YarnEngine::set_to_save);

    ClassDB::bind_method(D_METHOD("get_from_save", "save_key","default"), &YarnEngine::get_from_save);

    ClassDB::bind_method(D_METHOD("request_save", "immediate"), &YarnEngine::request_save,DEFVAL(false));

    ADD_SIGNAL(MethodInfo("changed_pause", PropertyInfo(Variant::BOOL, "pause_value")));

    ADD_SIGNAL(MethodInfo("after_full_save_reset"));
    ADD_SIGNAL(MethodInfo("prepare_save"));
    ADD_SIGNAL(MethodInfo("loaded_save", PropertyInfo(Variant::DICTIONARY, "save_data")));
}

Node* YarnEngine::get_current_scene() {
    return SceneTree::get_singleton()->get_current_scene();
}

void YarnEngine::setup_node() {
    if(!already_setup_in_tree && SceneTree::get_singleton() != nullptr) {
        SceneTree::get_singleton()->get_root()->add_child(this);
        already_setup_in_tree=true;
    }
}

void YarnEngine::create_save_backup() {
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

Variant YarnEngine::get_from_save(String p_save_key, Variant p_save_default) {
    if (p_save_key.contains("/")) {
        auto splitted = p_save_key.split("/");
        Dictionary* desired_dict = &save_data;
        for (int i = 0; i < splitted.size(); ++i) {
            if (i == splitted.size()-1)
                return desired_dict->get(p_save_key,p_save_default);
            if (!desired_dict->has(splitted[i])) return p_save_default;
            auto found_pointer = Object::cast_to<Dictionary>(desired_dict->get(splitted[i],Dictionary{}));
            if (found_pointer != nullptr) {
                desired_dict = found_pointer;
            } else {
                return p_save_default;
            }
        }
    }
    return save_data.get(p_save_key,p_save_default);
}

YarnEngine *YarnEngine::set_to_save(String p_save_key, Variant p_save_value) {
    if (p_save_key.contains("/")) {
        auto splitted = p_save_key.split("/");
        Dictionary* desired_dict = &save_data;
        for (int i = 0; i < splitted.size(); ++i) {
            if (i == splitted.size()-1) {
                if (desired_dict->has(splitted[i])) {
                    desired_dict->erase(splitted[i]);
                }
                desired_dict->get_or_add(splitted[i],p_save_value);
                return this;
            }
            if (!desired_dict->has(splitted[i]))
                desired_dict->get_or_add(splitted[i],Dictionary{});
            auto found_pointer = Object::cast_to<Dictionary>(desired_dict->get(splitted[i],Dictionary{}));
            if (found_pointer != nullptr) {
                desired_dict = found_pointer;
            } else {
                ERR_PRINT(vformat("FAILED TO ADD DICTIONARY WITH STRING %s original %s",splitted,p_save_key));
            }
        }
    }
    save_data[p_save_key] = p_save_value;
    return this;
}



YarnEngine *YarnEngine::add_to_save_data(String p_save_key, Variant p_save_value) {
    if (p_save_key.contains("/")) {
        auto splitted = p_save_key.split("/");
        Dictionary* desired_dict = &save_data;
        for (int i = 0; i < splitted.size(); ++i) {
            if (i == splitted.size()-1) {
                float initial_value = desired_dict->get(splitted[i],0.0);
                if (desired_dict->has(splitted[i])) {
                    desired_dict->erase(splitted[i]);
                }
                desired_dict->get_or_add(splitted[i],static_cast<float>(p_save_value) + initial_value);
                return this;
            }
            if (!desired_dict->has(splitted[i]))
                desired_dict->get_or_add(splitted[i],Dictionary{});
            auto found_pointer = Object::cast_to<Dictionary>(desired_dict->get(splitted[i],Dictionary{}));
            if (found_pointer != nullptr) {
                desired_dict = found_pointer;
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

void YarnEngine::_notification(int p_what) {
    if (p_what == 0) {
        call_deferred("setup_node");
    }
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
        } break;
        // case NOTIFICATION_ENTER_WORLD: {
        // } break;
        case NOTIFICATION_EXIT_TREE: {
            break;
        }
        case NOTIFICATION_PARENTED: {
            if (Engine::get_singleton()->is_editor_hint()) {
            }
        }
        case NOTIFICATION_READY: {
            set_process(true);
        } break;
        case NOTIFICATION_PROCESS: {
            if (!Engine::get_singleton()->is_editor_hint()) {
                handle_time_setting();
            }
            //print_line(pause_independent_time);
        } break;
        default:
            break;
    }
}

bool YarnEngine::attempt_pause() {
    if (last_time_ended_pause + 0.3 < pause_independent_time) {
        return false;
    }
    return is_paused;
}

void YarnEngine::set_is_paused(bool val) {
    if (is_paused != val) {
        is_paused = val;
        if (is_paused)
            amount_when_paused = OS::get_singleton()->get_ticks_msec() * 0.001f;
        else
            amount_when_unpaused = OS::get_singleton()->get_ticks_msec() * 0.001f;
        amount_time_last_frame = amount_when_unpaused;
        emit_signal(SNAME("changed_pause"),val);
    }
}


void YarnEngine::handle_time_setting() {
    pause_independent_time = OS::get_singleton()->get_ticks_msec() * 0.001f;
    if (!is_paused) {
        time += pause_independent_time - amount_time_last_frame;
        amount_time_last_frame = pause_independent_time;
    }
    if(save_next_frame) {
        save_next_frame = false;
        actually_save();
    } else if (!save_next_frame && save_requested && (last_time_save_requested + 0.33 < pause_independent_time)) {
        save_requested = false;
        execute_save();
    }
}

void YarnEngine::execute_save() {
    emit_signal(SNAME("prepare_save"));
    save_next_frame = true;
}

void YarnEngine::actually_save() {
    auto file = FileAccess::open(save_path,FileAccess::WRITE);
    file->store_string(JSON::stringify(save_data));
    file->close();
    print_line("Saved");
}

bool YarnEngine::request_load() {
    auto dir = DirAccess::create(DirAccess::ACCESS_USERDATA);
    if (!dir->file_exists(save_path)) {
        return false;
    }
    auto file = FileAccess::open(save_path, FileAccess::READ);
    auto file_string = file->get_as_text();
    if (file_string.is_empty())
        return false;
    Ref<JSON> jason;
    jason.instantiate();
    Error error = jason->parse(file_string);
    if(error != Error::OK) {
        jason.unref();
        print_line(vformat("Parse JSON failed. Error at line %d: %s", jason->get_error_line(), jason->get_error_message()));
        return false;
    }
    save_data = jason->get_data();
    Variant args[1] = { save_data };
    const Variant *argptrs[1] = { &args[0] };
    emit_signalp(SNAME("loaded_save"),argptrs,1);
    return true;
}

void YarnEngine::request_save(bool immediate = false) {
    if(immediate) {
        emit_signal(SNAME("prepare_save"));
        actually_save();
    } else {
        save_requested = true;
        last_time_save_requested = pause_independent_time;
    }
}

bool YarnEngine::has_time_elapsed(float test_time, float interval) {
    return test_time + interval < time;
}

bool YarnEngine::has_pause_independent_time_elapsed(float test_time, float interval) {
    return test_time + interval < pause_independent_time;
}

Variant YarnEngine::test_callback_thing(const Callable &p_callable) {
    if (!p_callable.is_null() && p_callable.is_valid()) {
        return p_callable.call();
    }
    return Variant{};
}

YarnEngine::YarnEngine() {
    singleton = this;
    is_paused = false;
    amount_time_last_frame = 0.0;
    amount_when_paused = 0.0;
    amount_when_unpaused = 0.0;
}

YarnEngine::~YarnEngine() {
    if (singleton != nullptr && singleton == this) {
        singleton = nullptr;
    }
}
