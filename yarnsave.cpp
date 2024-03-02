//
// Created by Daniel on 2024-02-26.
//

#include "yarnsave.h"
#include "core/io/dir_access.h"
#include "core/io/file_access.h"
#include "core/io/json.h"

Ref<YSave> YSave::singleton = nullptr;

YSave *YSave::get_singleton() {
    return *singleton;
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

    ClassDB::bind_method(D_METHOD("load_save_details", "desired_path"), &YSave::set_save_detail,DEFVAL(""));

    ClassDB::bind_method(D_METHOD("set_save_detail", "save_detail","detail_value"), &YSave::set_save_detail);

    ClassDB::bind_method(D_METHOD("set_to_save", "save_key","save"), &YSave::set_to_save);

    ClassDB::bind_method(D_METHOD("get_from_save", "save_key","default"), &YSave::get_from_save);

    ClassDB::bind_method(D_METHOD("request_save", "immediate"), &YSave::request_save,DEFVAL(false));

    ClassDB::bind_method(D_METHOD("request_load"), &YSave::request_load);
    ClassDB::bind_method(D_METHOD("reset_save"), &YSave::reset_save);

    ADD_SIGNAL(MethodInfo("executed_save_reset"));
    ADD_SIGNAL(MethodInfo("prepare_save"));
    ADD_SIGNAL(MethodInfo("saved", PropertyInfo(Variant::STRING, "save_path")));
    ADD_SIGNAL(MethodInfo("loaded_save", PropertyInfo(Variant::DICTIONARY, "save_data")));
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

Variant YSave::get_from_settings(String p_save_key, Variant p_save_default) {
    return settings_data.get(p_save_key,p_save_default);
}
YSave *YSave::set_to_settings(String p_save_key, Variant p_save_value) {
    settings_data[p_save_key] = p_save_value;
    return this;
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
        jason.unref();
        print_line(vformat("Parse JSON failed. Error at line %d: %s", jason->get_error_line(), jason->get_error_message()));
        return false;
    }
    settings_path = jason->get_data();
    return true;
}

YSave::YSave() {
}

YSave::~YSave() {
    if(singleton == this) {
        singleton = nullptr;
    }
}

Variant YSave::get_from_save(String p_save_key, Variant p_save_default) {
    if (p_save_key.contains("/")) {
        auto splitted = p_save_key.split("/");
        Dictionary desired_dict = save_data;
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
    }
    return save_data.get(p_save_key,p_save_default);
}

YSave *YSave::set_to_save(String p_save_key, Variant p_save_value) {
    if (p_save_key.contains("/")) {
        auto splitted = p_save_key.split("/");
        Dictionary desired_dict = save_data;
        for (int i = 0; i < splitted.size(); ++i) {
            if (i == splitted.size()-1) {
                desired_dict[splitted[i]] = p_save_value;
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
    save_data[p_save_key] = p_save_value;
    return this;
}

YSave *YSave::add_to_save_data(String p_save_key, Variant p_save_value) {
    if (p_save_key.contains("/")) {
        auto splitted = p_save_key.split("/");
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
        return false;
    }
    auto file = FileAccess::open(save_path, FileAccess::READ);
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
        jason.unref();
        print_line(vformat("Parse JSON failed. Error at line %d: %s", jason->get_error_line(), jason->get_error_message()));
        return false;
    }
    save_data = jason->get_data();
    print_line("Jason loaded data ",save_data);
    emit_signal(SNAME("loaded_save"),save_data);
    return true;
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
        emit_signal(SNAME("prepare_save"));
        actually_save();
    } else {
        save_requested = true;
        last_time_save_requested = pause_independent_time;
    }
}
