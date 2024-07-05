#include "yarnengine.h"

#include "yarnsave.h"
#include "core/config/project_settings.h"
#include "core/io/dir_access.h"
#include "core/io/json.h"
#include "core/math/random_number_generator.h"
#include "core/string/translation.h"
#include "scene/gui/control.h"

YEngine* YEngine::singleton = nullptr;

YEngine * YEngine::get_singleton() {
    return singleton;
}

String YEngine::EKey(const Dictionary &dict, int val) {
    return dict.find_key(val);
}

Ref<Resource> YEngine::get_game_resource(int resource_type, int resource_id) {
        if (game_resources.has(resource_type)) {
            auto hashtable_resources = game_resources[resource_type];
            if (hashtable_resources.has(resource_id)) {
                return hashtable_resources[resource_id];
            }
        }
        if (game_resource_paths.has(resource_type)) {
            auto respaths = game_resource_paths[resource_type];
            if (respaths.has(resource_id)) {
                auto res_loaded = ResourceLoader::load(respaths[resource_id]);
                if (res_loaded.is_valid()) {
                    if (!game_resources.has(resource_type)) game_resources[resource_type] = {};
                    game_resources[resource_type][resource_id] = res_loaded;
                } else {
                    respaths.erase(resource_id);
                }
            }
        }
        return nullptr;
}

bool YEngine::has_game_resource(int resource_type, int resource_id) {
    if (game_resources.has(resource_type)) {
        return game_resources[resource_type].has(resource_id);
    }
    if (game_resource_paths.has(resource_type)) {
        auto respaths = game_resource_paths[resource_type];
        if (respaths.has(resource_id)) {
            auto res_loaded = ResourceLoader::load(respaths[resource_id]);
            if (res_loaded.is_valid()) {
                if (!game_resources.has(resource_type)) game_resources[resource_type] = {};
                game_resources[resource_type][resource_id] = res_loaded;
                return true;
            } else {
                respaths.erase(resource_id);
            }
        }
    }
    return false;
}

Vector3 YEngine::get_random_point_on_top_of_mesh(MeshInstance3D *p_meshInstance, Ref<RandomNumberGenerator> p_rng) {
    if (p_meshInstance == nullptr) {
        return Vector3{0.0,0.0,0.0};
    }
    if (p_rng.is_null() || !p_rng.is_valid())
        p_rng.instantiate();

    AABB boundingBox = p_meshInstance->get_aabb();
    Vector3 position = boundingBox.position;
    Vector3 size = boundingBox.size;

    Vector3 randomPoint;
    randomPoint.x = p_rng->randf_range(position.x, position.x + size.x);
    randomPoint.y = position.y + size.y;
    randomPoint.z = p_rng->randf_range(position.z, position.z + size.z);

    return p_meshInstance->to_global(randomPoint);;
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

    ClassDB::bind_method(D_METHOD("find_packedscenes_in", "path", "name_contains"), &YEngine::find_packedscenes_in,DEFVAL(""));
    ClassDB::bind_method(D_METHOD("find_resources_in", "path", "name_contains"), &YEngine::find_resources_in,DEFVAL(""));
    ClassDB::bind_method(D_METHOD("find_resources_paths_in", "path", "name_contains"), &YEngine::find_resources_paths_in,DEFVAL(""));

    ClassDB::bind_method(D_METHOD("find_node_with_type","parent_node","node_type"), &YEngine::find_node_with_type);

    ClassDB::bind_method(D_METHOD("find_node_with_method","parent_node","node_type"), &YEngine::find_node_with_type);

    ClassDB::bind_method(D_METHOD("set_flag_value","flags","flag","set_value"), &YEngine::set_flag_value);
    ClassDB::bind_method(D_METHOD("check_flag_value","flags","check_flag"), &YEngine::check_flag_value);

    ClassDB::bind_method(D_METHOD("get_menu_stack"), &YEngine::get_menu_stack);
    ClassDB::bind_method(D_METHOD("get_menu_stack_size"), &YEngine::get_menu_stack_size);
    ClassDB::bind_method(D_METHOD("is_top_of_menu_stack","menu_to_check"), &YEngine::is_top_of_menu_stack);

    ClassDB::bind_method(D_METHOD("spawn","packed_scene","parent","global_pos","force_readable_name"), &YEngine::spawn, DEFVAL(false));

    ClassDB::bind_method(D_METHOD("EKey","dict","value"), &YEngine::EKey);
    ClassDB::bind_method(D_METHOD("string_to_hash","input_string"), &YEngine::string_to_hash);
    ClassDB::bind_method(D_METHOD("seeded_shuffle","array","seed"), &YEngine::seeded_shuffle);

    ClassDB::bind_method(D_METHOD("set_game_resource_path","resource_type","resource_id", "resource_path"), &YEngine::set_game_resource_path);
    ClassDB::bind_method(D_METHOD("set_game_resource","resource_type","resource_id", "resource"), &YEngine::set_game_resource);
    ClassDB::bind_method(D_METHOD("get_game_resource","resource_type","resource_id"), &YEngine::get_game_resource);
    ClassDB::bind_method(D_METHOD("has_game_resource","resource_type","resource_id"), &YEngine::has_game_resource);
    ClassDB::bind_method(D_METHOD("remove_game_resource","resource_type","resource_id"), &YEngine::remove_game_resource);
    ClassDB::bind_method(D_METHOD("get_all_game_resources_of_type","resource_type"), &YEngine::get_all_game_resources_of_type);
    ClassDB::bind_method(D_METHOD("get_all_game_resources_types"), &YEngine::get_all_game_resources_types);

    ClassDB::bind_method(D_METHOD("get_random_point_on_top_of_mesh","mesh_instance_3d","random_number_generator"), &YEngine::get_random_point_on_top_of_mesh);

    ADD_SIGNAL(MethodInfo("changed_pause", PropertyInfo(Variant::BOOL, "pause_value")));
    ADD_SIGNAL(MethodInfo("initialized"));
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

void YEngine::game_state_starting(const Ref<YGameState> &ygs) {
    ygamestate = ygs;
    using_game_state=true;
    print_line("Registered using game state ",ygamestate," using game state now? ",using_game_state);
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

Array YEngine::seeded_shuffle(Array array_to_shuffle,int seed_to_use) {
    Ref<RandomNumberGenerator> rng;
    rng.instantiate();
    rng->set_seed(seed_to_use);
    for (int i = array_to_shuffle.size() - 1; i >= 1; i--) {
        const int j = rng->randi_range(0,i);
        const Variant tmp = array_to_shuffle[j];
        array_to_shuffle[j] = array_to_shuffle[i];
        array_to_shuffle[i] = tmp;
    }
    rng.unref();
    return array_to_shuffle;
}

void YEngine::setup_node() {
    if(!already_setup_in_tree && SceneTree::get_singleton() != nullptr) {
        ysave = YSave::get_singleton();
        ytime = YTime::get_singleton();
        ytween = YTween::get_singleton();
        ytime->is_paused = false;
        last_button_click_time = 0.0;
        String appname = GLOBAL_GET("application/config/window_name");
        if (appname.is_empty()) {
            appname = GLOBAL_GET("application/config/name");
        }
        DisplayServer::get_singleton()->window_set_title(TranslationServer::get_singleton()->translate(appname));

        SceneTree::get_singleton()->get_root()->call_deferred("add_child",this);
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
            emit_signal("initialized");
            set_process(true);
            set_physics_process(true);
            TypedArray<Dictionary> global_class_list = ProjectSettings::get_singleton()->get_global_class_list();
            for (Dictionary p_class_list: global_class_list) {
                if (p_class_list.has("class") && p_class_list.has("path"))
                    class_name_to_script_path[p_class_list["class"]] = p_class_list["path"];
            }
        } break;
        case NOTIFICATION_PHYSICS_PROCESS: {
            if (!Engine::get_singleton()->is_editor_hint() && OS::get_singleton() != nullptr) {
                ytween->do_process(get_physics_process_delta_time());
            }
        }
        case NOTIFICATION_PROCESS: {
            if (!Engine::get_singleton()->is_editor_hint() && OS::get_singleton() != nullptr) {
                do_process();
                if (using_game_state) {
                    ygamestate->do_process(get_process_delta_time());
                }
            }
            //print_line(pause_independent_time);
        } break;
        default:
            break;
    }
}

Node * YEngine::find_node_with_meta(Node *parent_node, const String &p_type) {
    if (parent_node != nullptr) {
        TypedArray<Node> find_stuff = parent_node->get_children();
        while (find_stuff.size()) {
            Node *_child = Object::cast_to<Node>(find_stuff.pop_back());
            if (_child->has_meta(p_type)) return _child;
        }
    }
    return nullptr;
}

Node * YEngine::find_node_with_method(Node *parent_node, const String &p_type) {
    if (parent_node != nullptr) {
        TypedArray<Node> find_stuff = parent_node->get_children();
        while (find_stuff.size()) {
            Node *_child = Object::cast_to<Node>(find_stuff.pop_back());
            if (_child->has_method(p_type)) return _child;
        }
    }
    return nullptr;
}
Node * YEngine::find_node_with_type(Node *parent_node, const String &p_type) {
    if (parent_node != nullptr) {
        TypedArray<Node> find_stuff = parent_node->find_children("*",p_type);
        if (find_stuff.size() > 0) {
            return Object::cast_to<Node>(find_stuff[0].operator Object*());
        }
    }
    return nullptr;
}

uint32_t YEngine::set_flag_value(uint32_t collision_layer, int p_layer_number, bool p_value) {
    ERR_FAIL_COND_V_MSG(p_layer_number < 1, collision_layer, "Collision layer number must be between 1 and 32 inclusive.");
    ERR_FAIL_COND_V_MSG(p_layer_number > 32, collision_layer, "Collision layer number must be between 1 and 32 inclusive.");
    uint32_t collision_layer_new = collision_layer;
    if (p_value) {
        collision_layer_new |= 1 << (p_layer_number - 1);
    } else {
        collision_layer_new &= ~(1 << (p_layer_number - 1));
    }
    return collision_layer_new;
}

bool YEngine::check_flag_value(uint32_t collision_layer, int p_layer_number) const {
    ERR_FAIL_COND_V_MSG(p_layer_number < 1, false, "Collision layer number must be between 1 and 32 inclusive.");
    ERR_FAIL_COND_V_MSG(p_layer_number > 32, false, "Collision layer number must be between 1 and 32 inclusive.");
    return collision_layer & (1 << (p_layer_number - 1));
}

void YEngine::do_process() {
    ytime->handle_time_setting();
    ysave->process(ytime->pause_independent_time);
    ytween->do_process(get_process_delta_time());
}

int YEngine::string_to_hash(const String &str) {
    uint32_t crc = 0xFFFFFFFF;
    const char32_t *chr = str.get_data();
    uint32_t c = *chr++;
    while (c) {
        crc ^= static_cast<uint8_t>(c);
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (-static_cast<int>(crc & 1) & 0xEDB88320);
        }
        c = *chr++;
    }
    return static_cast<int>(~crc);
}

void YEngine::setting_position_in_parent(Node *node_entered_tree, const Variant &p_spawn_pos) {
    if (node_entered_tree != nullptr) {
        node_entered_tree->call("set_global_position",p_spawn_pos);
    }
}

Node * YEngine::spawn(const Ref<PackedScene> &p_spawnable_scene, Node *p_desired_parent, const Variant &p_spawn_pos, bool p_force_readable_name) {
    ERR_FAIL_COND_V_MSG(p_spawnable_scene.is_null() || !p_spawnable_scene.is_valid(), nullptr, "ERROR: Spawnable scene is not valid");
    ERR_FAIL_COND_V_MSG(p_desired_parent == nullptr, nullptr, "ERROR: Parent node is not valid");
    auto spawned_instance = p_spawnable_scene->instantiate();
    ERR_FAIL_COND_V_MSG(spawned_instance == nullptr, nullptr, "ERROR: Spawned Instance is null");
    p_desired_parent->connect("child_entered_tree",callable_mp(this,&YEngine::setting_position_in_parent).bind(p_spawn_pos), CONNECT_ONE_SHOT);
    p_desired_parent->add_child(spawned_instance,p_force_readable_name);
    return spawned_instance;
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
    last_button_click_time = 0.0;
    using_game_state = false;
    can_button_click=true;
    ytween=nullptr;
    ysave=nullptr;
    ytime=nullptr;
}

YEngine::~YEngine() {
    if (singleton != nullptr && singleton == this) {
        singleton = nullptr;
    }
    if (using_game_state) {
        using_game_state=false;
        ygamestate.unref();
    }
    ytween=nullptr;
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

TypedArray<PackedScene> YEngine::find_packedscenes_in(const Variant &variant_path, const String &name_contains) {
    PackedStringArray _paths;
    if(variant_path.get_type() == Variant::Type::ARRAY) { _paths = variant_path;
    } else if (variant_path.get_type() != Variant::Type::STRING) { return Array{}; }
    else { _paths.append(variant_path); }
    TypedArray<PackedScene> return_paths;
    bool has_name_contains = !name_contains.is_empty();

    for (auto _path: _paths) {
        Error check_error;
        auto dir = DirAccess::open(_path,&check_error);
        if (check_error != OK) {
            WARN_PRINT(vformat("[find_packedscenes_in] Couldn't open dir %s",_path));
            continue;
        }

        dir->list_dir_begin();
        String file_name = dir->get_next();
        while (!file_name.is_empty()) {
            file_name = file_name.trim_suffix(".remap");
            if (file_name.ends_with(".tscn"))
                if (!has_name_contains || file_name.contains(name_contains)) {
                    Ref<PackedScene> loaded_resource = ResourceLoader::load(vformat("%s/%s",_path,file_name));
                    if(loaded_resource.is_valid())
                        return_paths.append(loaded_resource);
                    else
                        WARN_PRINT(vformat("[find_packedscenes_in] Failed loading packedscene at %s/%s",_path,file_name));
                }
            file_name = dir->get_next();
        }
    }
    return return_paths;
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