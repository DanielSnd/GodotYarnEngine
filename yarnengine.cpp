#include "yarnengine.h"

#include "yarnsave.h"
#include "core/config/project_settings.h"
#include "core/io/dir_access.h"
#include "core/io/json.h"
#include "core/string/translation.h"
#include "scene/gui/control.h"

YEngine* YEngine::singleton = nullptr;

YEngine * YEngine::get_singleton() {
    return singleton;
}

void YEngine::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_current_scene"), &YEngine::get_current_scene);

    ClassDB::bind_method(D_METHOD("set_last_button_click_time", "last_button_click_time"), &YEngine::set_last_button_click_time);
    ClassDB::bind_method(D_METHOD("get_last_button_click_time"), &YEngine::get_last_button_click_time);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "last_button_click_time"), "set_last_button_click_time", "get_last_button_click_time");
    
    ClassDB::bind_method(D_METHOD("set_can_button_click", "can_button_click"), &YEngine::set_can_button_click);
    ClassDB::bind_method(D_METHOD("get_can_button_click"), &YEngine::get_can_button_click);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "can_button_click"), "set_can_button_click", "get_can_button_click");

    ClassDB::bind_method(D_METHOD("button_click_callable", "callable"), &YEngine::button_click_callable);
    ClassDB::bind_method(D_METHOD("button_click_callable_if_modulate", "callable","control"), &YEngine::button_click_callable_if_modulate);

    ClassDB::bind_method(D_METHOD("find_resources_in", "path", "name_contains"), &YEngine::find_resources_in,DEFVAL(""));
    ClassDB::bind_method(D_METHOD("find_resources_paths_in", "path", "name_contains"), &YEngine::find_resources_paths_in,DEFVAL(""));

    ClassDB::bind_method(D_METHOD("get_menu_stack"), &YEngine::get_menu_stack);
    ClassDB::bind_method(D_METHOD("get_menu_stack_size"), &YEngine::get_menu_stack_size);

    ADD_SIGNAL(MethodInfo("changed_pause", PropertyInfo(Variant::BOOL, "pause_value")));
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

Callable YEngine::button_click_callable(const Callable &p_callable) {
    return (callable_mp(this,&YEngine::execute_button_click_callable).bind(p_callable));
}

Callable YEngine::button_click_callable_if_modulate(const Callable &p_callable,Control* p_control) {
    return (callable_mp(this,&YEngine::execute_button_click_callable_if_modulate).bind(p_callable,p_control));
}

Node* YEngine::get_current_scene() {
    return SceneTree::get_singleton()->get_current_scene();
}

void YEngine::setup_node() {
    if(!already_setup_in_tree && SceneTree::get_singleton() != nullptr) {
        ysave = YSave::get_singleton();
        ytime = YTime::get_singleton();
        ytime->is_paused = false;
        String appname = GLOBAL_GET("application/config/window_name");
        if (appname.is_empty()) {
            appname = GLOBAL_GET("application/config/name");
        }
        DisplayServer::get_singleton()->window_set_title(TranslationServer::get_singleton()->translate(appname));

        SceneTree::get_singleton()->get_root()->add_child(this);
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
            if (!Engine::get_singleton()->is_editor_hint() && OS::get_singleton() != nullptr) {
                do_process();
            }
            //print_line(pause_independent_time);
        } break;
        default:
            break;
    }
}

void YEngine::do_process() {
    ytime->handle_time_setting();
    ysave->process(ytime->pause_independent_time);
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
}

YEngine::~YEngine() {
    if (singleton != nullptr && singleton == this) {
        singleton = nullptr;
    }
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
            if (file_name.ends_with(".tres"))
                if (!has_name_contains || file_name.contains(name_contains))
                    return_paths.append(file_name);
            file_name = dir->get_next();
        }
    }
    return return_paths;
}

// func find_unlock_infos():
//     var upgrades_paths :Array[String] = ["res://hentai_resources/"]
//     #print(DirAccess.get_files_at(upgrades_paths[0]))
//     for path in upgrades_paths:
//         var dir = DirAccess.open(path)
//         if DirAccess.get_open_error() == OK:
//             #print(dir.dir_exists("res://hentai_resources/"))
//             dir.list_dir_begin()
//             var file_name = dir.get_next()
//             while (file_name != ""):
//                 file_name = file_name.trim_suffix(".remap")
//                 if (file_name.ends_with(".tres")):
//                     var res = load(path + file_name)
//                     if res == null:
//                         print("Error loading ",path,file_name)
//                     if res is UnlockableSpriteFrames:
//                         unlock_pool.append(res)
//                 file_name = dir.get_next()
//         else:
//             print("An error occurred when trying to access the ",path," path.")
//     print("Loaded ", str(unlock_pool.size()), " UnlockInfos.")
//     if save_data.has("unlocked") or all_unlocked:
//         for unlockable in unlock_pool.duplicate():
//             if check_has_unlock(unlockable.unlock_id):
//                 already_unlocked.append(unlockable)
//                 #if using_steam and unlockable.has_steam_achievement_id:
//                     #Steam.setAchievement(unlockable.steam_achievement_id)
//                     #unlocked_stema_achievement = true
//                 unlock_pool.erase(unlockable)