//
// Created by Daniel on 2024-05-01.
//

#include "yscenespawner3d.h"

#include "yarnphysics.h"


void YSceneSpawner3D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_placing_radius", "placing_radius"), &YSceneSpawner3D::set_placing_radius);
    ClassDB::bind_method(D_METHOD("get_placing_radius"), &YSceneSpawner3D::get_placing_radius);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "placing_radius"), "set_placing_radius", "get_placing_radius");

    // debug_show_spawn_area
    ClassDB::bind_method(D_METHOD("set_debug_show_spawn_area", "debug_show_spawn_area"), &YSceneSpawner3D::set_debug_show_spawn_area);
    ClassDB::bind_method(D_METHOD("get_debug_show_spawn_area"), &YSceneSpawner3D::get_debug_show_spawn_area);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_show_spawn_area"), "set_debug_show_spawn_area", "get_debug_show_spawn_area");

    // auto_spawn_on_ready
    ClassDB::bind_method(D_METHOD("set_auto_spawn_on_ready", "auto_spawn_on_ready"), &YSceneSpawner3D::set_auto_spawn_on_ready);
    ClassDB::bind_method(D_METHOD("get_auto_spawn_on_ready"), &YSceneSpawner3D::get_auto_spawn_on_ready);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_spawn_on_ready"), "set_auto_spawn_on_ready", "get_auto_spawn_on_ready");

    // align_to_ground
    ClassDB::bind_method(D_METHOD("set_align_to_ground", "align_to_ground"), &YSceneSpawner3D::set_align_to_ground);
    ClassDB::bind_method(D_METHOD("get_align_to_ground"), &YSceneSpawner3D::get_align_to_ground);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "align_to_ground"), "set_align_to_ground", "get_align_to_ground");

    // spawn_amount
    ClassDB::bind_method(D_METHOD("set_spawn_amount", "spawn_amount"), &YSceneSpawner3D::set_spawn_amount);
    ClassDB::bind_method(D_METHOD("get_spawn_amount"), &YSceneSpawner3D::get_spawn_amount);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "spawn_amount"), "set_spawn_amount", "get_spawn_amount");

    // random_y_rotate
    ClassDB::bind_method(D_METHOD("set_random_y_rotate", "random_y_rotate"), &YSceneSpawner3D::set_random_y_rotate);
    ClassDB::bind_method(D_METHOD("get_random_y_rotate"), &YSceneSpawner3D::get_random_y_rotate);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "random_y_rotate"), "set_random_y_rotate", "get_random_y_rotate");

    // random_z_rotate
    ClassDB::bind_method(D_METHOD("set_random_z_rotate", "random_z_rotate"), &YSceneSpawner3D::set_random_z_rotate);
    ClassDB::bind_method(D_METHOD("get_random_z_rotate"), &YSceneSpawner3D::get_random_z_rotate);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "random_z_rotate"), "set_random_z_rotate", "get_random_z_rotate");

    // lock_to_layer
    ClassDB::bind_method(D_METHOD("set_lock_to_layer", "lock_to_layer"), &YSceneSpawner3D::set_lock_to_layer);
    ClassDB::bind_method(D_METHOD("get_lock_to_layer"), &YSceneSpawner3D::get_lock_to_layer);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "lock_to_layer", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_lock_to_layer", "get_lock_to_layer");

    // random_y_scale
    ClassDB::bind_method(D_METHOD("set_random_y_scale", "random_y_scale"), &YSceneSpawner3D::set_random_y_scale);
    ClassDB::bind_method(D_METHOD("get_random_y_scale"), &YSceneSpawner3D::get_random_y_scale);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "random_y_scale"), "set_random_y_scale", "get_random_y_scale");

    // randomize_scale
    ClassDB::bind_method(D_METHOD("set_randomize_scale", "randomize_scale"), &YSceneSpawner3D::set_randomize_scale);
    ClassDB::bind_method(D_METHOD("get_randomize_scale"), &YSceneSpawner3D::get_randomize_scale);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "randomize_scale"), "set_randomize_scale", "get_randomize_scale");

    // min_max_random_scale
    ClassDB::bind_method(D_METHOD("set_min_max_random_scale", "min_max_random_scale"), &YSceneSpawner3D::set_min_max_random_scale);
    ClassDB::bind_method(D_METHOD("get_min_max_random_scale"), &YSceneSpawner3D::get_min_max_random_scale);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "min_max_random_scale"), "set_min_max_random_scale", "get_min_max_random_scale");

    ClassDB::bind_method(D_METHOD("set_prevent_stacking_radius", "prevent_stacking_layer"), &YSceneSpawner3D::set_prevent_stacking_radius);
    ClassDB::bind_method(D_METHOD("get_prevent_stacking_radius"), &YSceneSpawner3D::get_prevent_stacking_radius);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "prevent_stacking_radius"), "set_prevent_stacking_radius", "get_prevent_stacking_radius");

    // prevent_stacking
    ClassDB::bind_method(D_METHOD("set_prevent_stacking_layer", "prevent_stacking_layer"), &YSceneSpawner3D::set_prevent_stacking_layer);
    ClassDB::bind_method(D_METHOD("get_prevent_stacking_layer"), &YSceneSpawner3D::get_prevent_stacking_layer);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "prevent_stacking_layer", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_prevent_stacking_layer", "get_prevent_stacking_layer");

    // prevent_spawn_under
    ClassDB::bind_method(D_METHOD("set_prevent_spawn_under", "prevent_spawn_under"), &YSceneSpawner3D::set_prevent_spawn_under);
    ClassDB::bind_method(D_METHOD("get_prevent_spawn_under"), &YSceneSpawner3D::get_prevent_spawn_under);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "prevent_spawn_under"), "set_prevent_spawn_under", "get_prevent_spawn_under");

    // force_spawn_under
    ClassDB::bind_method(D_METHOD("set_force_spawn_under", "force_spawn_under"), &YSceneSpawner3D::set_force_spawn_under);
    ClassDB::bind_method(D_METHOD("get_force_spawn_under"), &YSceneSpawner3D::get_force_spawn_under);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "force_spawn_under"), "set_force_spawn_under", "get_force_spawn_under");

    // under_value
    ClassDB::bind_method(D_METHOD("set_under_value", "under_value"), &YSceneSpawner3D::set_under_value);
    ClassDB::bind_method(D_METHOD("get_under_value"), &YSceneSpawner3D::get_under_value);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "under_value"), "set_under_value", "get_under_value");

    ClassDB::bind_method(D_METHOD("add_spawnable_scene", "path"), &YSceneSpawner3D::add_spawnable_scene);
    ClassDB::bind_method(D_METHOD("get_spawnable_scene_count"), &YSceneSpawner3D::get_spawnable_scene_count);
    ClassDB::bind_method(D_METHOD("get_spawnable_scene", "index"), &YSceneSpawner3D::get_spawnable_scene);
    ClassDB::bind_method(D_METHOD("clear_spawnable_scenes"), &YSceneSpawner3D::clear_spawnable_scenes);

    ClassDB::bind_method(D_METHOD("_get_spawnable_scenes"), &YSceneSpawner3D::_get_spawnable_scenes);
    ClassDB::bind_method(D_METHOD("_set_spawnable_scenes", "scenes"), &YSceneSpawner3D::_set_spawnable_scenes);

    ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "_spawnable_scenes", PROPERTY_HINT_NONE, "", (PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_INTERNAL)), "_set_spawnable_scenes", "_get_spawnable_scenes");


}

#ifdef TOOLS_ENABLED

/* This is editor only */
bool YSceneSpawner3D::_set(const StringName &p_name, const Variant &p_value) {
    if (p_name == "_spawnable_scene_count") {
        spawnable_scenes.resize(p_value);
        notify_property_list_changed();
        return true;
    } else {
        String ns = p_name;
        if (ns.begins_with("scenes/")) {
            uint32_t index = ns.get_slicec('/', 1).to_int();
            ERR_FAIL_UNSIGNED_INDEX_V(index, spawnable_scenes.size(), false);
            spawnable_scenes[index].path = p_value;
            return true;
        }
    }
    return false;
}

bool YSceneSpawner3D::_get(const StringName &p_name, Variant &r_ret) const {
    if (p_name == "_spawnable_scene_count") {
        r_ret = spawnable_scenes.size();
        return true;
    } else {
        String ns = p_name;
        if (ns.begins_with("scenes/")) {
            uint32_t index = ns.get_slicec('/', 1).to_int();
            ERR_FAIL_UNSIGNED_INDEX_V(index, spawnable_scenes.size(), false);
            r_ret = spawnable_scenes[index].path;
            return true;
        }
    }
    return false;
}

void YSceneSpawner3D::_get_property_list(List<PropertyInfo> *p_list) const {
    p_list->push_back(PropertyInfo(Variant::INT, "_spawnable_scene_count", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_ARRAY, "Spawn List,scenes/"));
    List<String> exts;
    ResourceLoader::get_recognized_extensions_for_type("PackedScene", &exts);
    String ext_hint;
    for (const String &E : exts) {
        if (!ext_hint.is_empty()) {
            ext_hint += ",";
        }
        ext_hint += "*." + E;
    }
    for (uint32_t i = 0; i < spawnable_scenes.size(); i++) {
        p_list->push_back(PropertyInfo(Variant::STRING, "scenes/" + itos(i), PROPERTY_HINT_FILE, ext_hint, PROPERTY_USAGE_EDITOR));
    }
}
#endif

void YSceneSpawner3D::add_spawnable_scene(const String &p_path) {
    SpawnableScene sc;
    sc.path = p_path;
    if (Engine::get_singleton()->is_editor_hint()) {
        ERR_FAIL_COND(!FileAccess::exists(p_path));
    }
    spawnable_scenes.push_back(sc);
}

int YSceneSpawner3D::get_spawnable_scene_count() const {
    return spawnable_scenes.size();
}

String YSceneSpawner3D::get_spawnable_scene(int p_idx) const {
    ERR_FAIL_INDEX_V(p_idx, (int)spawnable_scenes.size(), "");
    return spawnable_scenes[p_idx].path;
}

void YSceneSpawner3D::clear_spawnable_scenes() {
    spawnable_scenes.clear();
}

Vector<String> YSceneSpawner3D::_get_spawnable_scenes() const {
    Vector<String> ss;
    ss.resize(spawnable_scenes.size());
    for (int i = 0; i < ss.size(); i++) {
        ss.write[i] = spawnable_scenes[i].path;
    }
    return ss;
}

void YSceneSpawner3D::_set_spawnable_scenes(const Vector<String> &p_scenes) {
    clear_spawnable_scenes();
    for (int i = 0; i < p_scenes.size(); i++) {
        add_spawnable_scene(p_scenes[i]);
    }
}

void YSceneSpawner3D::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_READY: {
            if (!Engine::get_singleton()->is_editor_hint()) {
                get_tree()->create_timer(0.1)->connect("timeout",callable_mp(this,&YSceneSpawner3D::spawn_objects),CONNECT_ONE_SHOT);
            }
        } break;
    }
}

void YSceneSpawner3D::spawn_objects()
{
    print_line("Spawn objects visible?",is_visible()," empty spawnable? ",spawnable_scenes.is_empty());
    if (!is_visible() || spawnable_scenes.is_empty()) return;

    print_line("Actually spawn objects");
    //Debug.Log("Spawn custom spawner");
    const int randomAmount = spawning_rng->randi_range(spawn_amount.x, spawn_amount.y);
    LocalVector<SpawnableScene> spawnObjects;
    for (int i = 0; i < randomAmount; i++) {
        int max_spawnable_scenes = static_cast<int>(spawnable_scenes.size());
        SpawnableScene &newSpawn = spawnable_scenes[spawning_rng->randi_range(0,max_spawnable_scenes-1)];
        spawnObjects.push_back(newSpawn);
    }

    if (spawn_one_of_each)
    {
        spawnObjects.clear();
        for (SpawnableScene &spawn_object: spawnable_scenes) {
            spawnObjects.push_back(spawn_object);
        }
    }
    Vector<Ref<PackedScene>> packed_scenes;

    for (size_t i = 0; i < spawnObjects.size(); i++) {
        SpawnableScene &sc = spawnObjects[i];

        print_line(vformat("Check if cache is null for %s has cached? %s",sc.path,!sc.cache.is_null()));
        if (sc.cache.is_null()) {
            sc.cache = ResourceLoader::load(sc.path);

        }
        if (sc.cache.is_valid()) packed_scenes.push_back(sc.cache);
    }
    do_spawn(packed_scenes);
}

void YSceneSpawner3D::clear_spawned_objects() const {
    TypedArray<Node> children_nodes = get_children();
    for (int i = 0; i < children_nodes.size(); i++) {
        Node3D* child = Object::cast_to<Node3D>(children_nodes[i].operator Object*());
        if (child != nullptr) {
            child->queue_free();
        }
    }
}

void YSceneSpawner3D::do_spawn(const Vector<Ref<PackedScene>> &spawn_list) {
    clear_spawned_objects();
    // addedRecently.Clear();
    // if (PreventStacking) PopulateAddedRecentlyWithAll();
    //Debug.Log("Spawner "+gameObject.name.ColorString(Color.cyan));
    print_line(vformat("Called do spawn with list with amount %d",spawn_list.size()));
    for (int i = 0; i < static_cast<int>(spawn_list.size()); i++) {
        add_object_random_position(spawn_list[i]);
    }
}

Dictionary YSceneSpawner3D::random_position_on_registred_surface() const {
    Vector3 centerPos = get_global_position() + Vector3{0.0, 6.0, 0.0};
    Vector3 randomPos = centerPos + Vector3{spawning_rng->randf_range(-placing_radius,placing_radius), 0.0, spawning_rng->randf_range(-placing_radius,placing_radius)};

    Dictionary raycast_result;
    // if (LineSpawn) {
    //     Vector3 startLine = transform.TransformPoint(new Vector3(-LineSpawnDistance,0,0));
    //     Vector3 endLine = transform.TransformPoint(new Vector3(LineSpawnDistance,0,0));
    //     randomPos = startLine.Lerp(endLine, Random.value);
    //     randomPos.y += 6;
    //     randomPos.x += Random.Range(-placementRadius, placementRadius);
    //     randomPos.z += Random.Range(-placementRadius, placementRadius);
    // }

    if (lock_to_layer) raycast_result = YPhysics::get_singleton()->raycast3d(randomPos,Vector3{0.0,-1.0,0.0},400.0,YPhysics::COLLIDE_WITH_BODIES);
    else raycast_result = YPhysics::get_singleton()->raycast3d(randomPos,Vector3{0.0,-1.0,0.0},400.0,YPhysics::COLLIDE_WITH_BODIES);

    return raycast_result;
}

Node3D* YSceneSpawner3D::add_object_random_position(const Ref<PackedScene> &p_packed_scene) {
    Vector3 pos = Vector3{0.0,0.0,0.0};
    int count = 0;
    Dictionary raycast_results;
    while (pos.is_zero_approx() && count < 500) {
        raycast_results = random_position_on_registred_surface();
        if (!raycast_results.is_empty()) {
            pos = raycast_results.get("position",pos);
        }
        if (prevent_stacking_radius > 0.001) {
            if(YPhysics::get_singleton()->free_sphere_check(pos,prevent_stacking_radius,prevent_stacking_layer)) {
                pos = Vector3{0.0,0.0,0.0};
                count++;
                continue;
            }
        }
        if ((prevent_spawn_under && pos.y < under_value) || (force_spawn_under && pos.y > under_value)) {
            pos = Vector3{0.0,0.0,0.0};
            count++;
            continue;
        }

        // if (SpawnerPreventer.IsSpotPrevented(pos))
        // {
        //     if (DoingDebug) Debug.Log(string.Format("{0} Spot prevented",itemID));
        //     pos = Vector3.zero;
        // }
        count++;
    }
    print_line("Ended the attempts with count",count);
    if (!pos.is_zero_approx() && !raycast_results.is_empty()) {
        //item.prefabName
        print_line("Spawn single packed scene");
        return spawn_single(p_packed_scene, pos,raycast_results.get("normal",Vector3{0.0,1.0,0.0}));
    }
    return nullptr;
}

Node3D* YSceneSpawner3D::spawn_single(const Ref<PackedScene> &spawn_scene, Vector3 spawn_pos, Vector3 spawn_normal) {
    auto spawned_scene = spawn_scene->instantiate();
    auto spawned_instance = cast_to<Node3D>(spawned_scene);
    if (spawned_instance != nullptr) {
        add_child(spawned_instance);
        spawned_instance->set_owner(get_owner());
        spawned_instance->set_global_position(spawn_pos);
        auto quaternion = spawned_instance->get_quaternion();
        if (align_to_ground)
            quaternion = Quaternion(Vector3(0.0,1.0,0.0),spawn_normal) * quaternion;
        if (random_z_rotate)
            quaternion = quaternion * Quaternion(Vector3(0.0,0.0,1.0),Math::deg_to_rad(spawning_rng->randf_range(0.01,359.9)));
        if (random_y_rotate) {
            quaternion = quaternion * Quaternion(Vector3(0.0,1.0,0.0),Math::deg_to_rad(spawning_rng->randf_range(-359.9,359.9)));
        }
        spawned_instance->set_quaternion(quaternion);
        if(randomize_scale) {
            spawned_instance->set_scale(spawned_instance->get_scale() * spawning_rng->randf_range(min_max_random_scale.x,min_max_random_scale.y));
        }
        if (random_y_scale) {
            Vector3 current_spawned_scale = spawned_instance->get_scale();
            float initial_scale_y = current_spawned_scale.y;
            current_spawned_scale.y = current_spawned_scale.y + spawning_rng->randf_range(-1 * (initial_scale_y * 0.166f),(initial_scale_y * 0.166f));
            spawned_instance->set_scale(current_spawned_scale);
        }
    }
    return spawned_instance;
}

YSceneSpawner3D::YSceneSpawner3D(): lock_to_layer(0), prevent_stacking_radius(0), prevent_stacking_layer(0) {
    spawning_rng.instantiate();
}
