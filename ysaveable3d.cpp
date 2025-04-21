#include "ysaveable3d.h"

Node* YSaveable3D::spawn_under_node = nullptr;

YSaveable3D::YSaveable3D() {
    has_loaded_save = false;
    original_rotation = Vector3(0.0,0.0,0.0);
    original_scale = Vector3(1.0,1.0,1.0);
}

YSaveable3D::~YSaveable3D() {
}

void YSaveable3D::_notification(int p_what) {
    if (p_what == NOTIFICATION_READY) {
        do_ready();
    }
}

void YSaveable3D::_bind_methods() {
    GDVIRTUAL_BIND(_on_prepare_save, "save_values")
    GDVIRTUAL_BIND(_on_load_save, "saved_values")
    ClassDB::bind_static_method("YSaveable3D", D_METHOD("set_spawn_under_node", "node"), &YSaveable3D::set_spawn_under_node);
    ClassDB::bind_static_method("YSaveable3D", D_METHOD("cleanup_all_saved_nodes"), &YSaveable3D::cleanup_all_saved_nodes);
    ClassDB::bind_static_method("YSaveable3D", D_METHOD("load_all_saved_nodes"), &YSaveable3D::load_all_saved_nodes);
}

void YSaveable3D::cleanup_all_saved_nodes() {
    if (YSave::get_singleton() != nullptr) {
        YSave::get_singleton()->emit_signal(SNAME("free_all_saveable_node3d"));
    }
}

void YSaveable3D::set_spawn_under_node(Node* p_node) {
    if (p_node == nullptr) {
        return;
    }
    spawn_under_node = p_node;
}

void YSaveable3D::do_ready() {
    // Wait for one frame
    SceneTree::get_singleton()->connect("process_frame", callable_mp(this, &YSaveable3D::on_ready_delayed), CONNECT_ONE_SHOT);
}

void YSaveable3D::on_ready_delayed() {
    Node3D *parent = Object::cast_to<Node3D>(get_parent());
    if (parent == nullptr) return;
    String scene_path_id = parent->get_scene_file_path();
    if (scene_path_id.is_empty()) return;
    YSave::get_singleton()->connect("prepare_save", callable_mp(this, &YSaveable3D::on_prepare_save));
    if (has_loaded_save) return;
    original_rotation = parent->get_global_rotation();
    original_scale = parent->get_scale();
}

void YSaveable3D::on_prepare_save() {
    Node3D *parent = Object::cast_to<Node3D>(get_parent());
    if (!parent) return;

    Dictionary save_values;
    List<StringName> meta_list;
    parent->get_meta_list(&meta_list);

    save_values["scn"] = YPersistentID::get_scene_path_id(parent->get_scene_file_path());

    for (int i = 0; i < meta_list.size(); i++) {
        StringName meta_name = meta_list.get(i);
        if (meta_name.operator String().begins_with("_")) continue;
        save_values[meta_name] = parent->get_meta(meta_name);
    }

    String parent_name = parent->get_name();
    if (parent_name[0] != '@') {
        save_values["n"] = parent_name;
    }
    save_values["p"] = parent->get_global_position();
    if (!parent->get_global_rotation().is_zero_approx() && !original_rotation.is_equal_approx(parent->get_global_rotation())) {  
        save_values["r"] = parent->get_global_rotation();
    }
    if (!parent->get_scale().is_equal_approx(Vector3(1.0,1.0,1.0)) && !original_scale.is_equal_approx(parent->get_scale())) {
        save_values["s"] = parent->get_scale();
    }

    String current_scene_name = vformat("%s_saved_nodes3d", YEngine::get_singleton()->get_current_scene_name());
    
    if (!YSave::get_singleton()->get_save_data().has(current_scene_name)) {
        YSave::get_singleton()->get_save_data()[current_scene_name] = Array();
    }

    Array save_array = YSave::get_singleton()->get_save_data()[current_scene_name];
    //print_line(vformat("Doing save at %s save array before %s", parent->get_name(), save_array));
    bool can_save = true;
    GDVIRTUAL_CALL(_on_prepare_save, save_values, can_save);
    if (!can_save) return;

    save_array.push_back(save_values);
    //print_line(vformat("Saved at %s save array after %s", parent->get_name(), save_array));
}

Vector3 YSaveable3D::destringify_vector3(const Variant &got_position) {
    if (got_position.get_type() == Variant::STRING) {
        String pos_str = got_position;
        pos_str = pos_str.replace("(", "").replace(")", "");
        Vector<String> vec_array = pos_str.split(",", false);
        if (vec_array.size() >= 3) {
            return Vector3(
                vec_array[0].to_float(),
                vec_array[1].to_float(),
                vec_array[2].to_float()
            );
        }
    }
    return got_position;
}

void YSaveable3D::load_all_saved_nodes() {
    String current_scene_name = vformat("%s_saved_nodes3d", YEngine::get_singleton()->get_current_scene_name());
    if (!YSave::get_singleton()->get_save_data().has(current_scene_name)) {
        // print_line("Current scene name not found in save data");
        return;
    }   
    Array save_array = YSave::get_singleton()->get_save_data()[current_scene_name];
    // print_line(vformat("Loading %d saved nodes", save_array.size()));
    for (int i = 0; i < save_array.size(); i++) {
        Dictionary save_values = save_array[i];
        // print_line(vformat("Loading saved node %d", i));
        spawn_from_saved_values(save_values);
    }
}

Node3D *YSaveable3D::spawn_from_saved_values(const Dictionary &saved_values) {
    if (!saved_values.has("scn")) 
        return nullptr;

    int scene_path_id = saved_values.get("scn",-1);
    // print_line(vformat("Scene path id: %d", scene_path_id));
    Ref<PackedScene> _packed_scene = YPersistentID::get_packed_scene_from_id(scene_path_id);
    // print_line(vformat("Packed scene: %s", _packed_scene.is_valid() ? "valid" : "invalid"));
    if (_packed_scene.is_null() || !_packed_scene.is_valid()) return nullptr;

    Node3D* spawned_node = Object::cast_to<Node3D>(_packed_scene->instantiate());
    if (spawned_node == nullptr) return nullptr;

    for (int i = 0; i < spawned_node->get_child_count(); i++) {
        YSaveable3D* child = Object::cast_to<YSaveable3D>(spawned_node->get_child(i));
        if (child != nullptr) {
            child->connect_to_queue_free_from_signal();
            child->load_save(saved_values);
            return spawned_node;
        }
    }

    if (spawn_under_node != nullptr) {
            spawn_under_node->add_child(spawned_node);
    } else {
        Node* spawnunder = SceneTree::get_singleton()->get_current_scene();
        if (spawnunder != nullptr) {
            spawnunder->add_child(spawned_node);
        }
    }

    return spawned_node; 
}

void YSaveable3D::connect_to_queue_free_from_signal() {
    YSave::get_singleton()->connect(SNAME("free_all_saveable_node3d"), callable_mp(this, &YSaveable3D::queue_free_from_signal));
}

void YSaveable3D::queue_free_from_signal() {
    if (is_inside_tree() && get_parent() != nullptr) {
        get_parent()->queue_free();
    }
}

void YSaveable3D::load_save(const Dictionary &saved_values) {
    Node *parent = get_parent();
    if (!parent) return;

    Node3D *parent3d = Object::cast_to<Node3D>(parent);
    if (!parent3d) {
        return;
    }

    if (!parent->is_inside_tree()) {
        if (spawn_under_node != nullptr) {
            spawn_under_node->add_child(parent);
        } else {
            Node* spawnunder = SceneTree::get_singleton()->get_current_scene();
            if (spawnunder != nullptr) {
                spawnunder->add_child(parent);
            }
        }
    }

    has_loaded_save = true;

    original_rotation = parent3d->get_global_rotation();
    original_scale = parent3d->get_scale();
    
    if (saved_values.has("n")) {
        parent->set_name(saved_values.get("n", parent->get_name()));
    }

    if (saved_values.has("p")) {
        parent3d->set_global_position(destringify_vector3(saved_values.get("p", parent3d->get_global_position())));
    }
    
    if (saved_values.has("r")) {
        parent3d->set_global_rotation(destringify_vector3(saved_values.get("r", parent3d->get_global_rotation())));
    }

    if (saved_values.has("s")) {
        parent3d->set_scale(destringify_vector3(saved_values.get("s", parent3d->get_scale())));
    }

    Array keys = saved_values.keys();
    Array values = saved_values.values();
    for (int i = 0; i < keys.size(); i++) {
        if (keys[i] == "p" || keys[i] == "r" || keys[i] == "s") continue;
        parent->set_meta(keys[i], values[i]);
    }
    GDVIRTUAL_CALL(_on_load_save, saved_values);
}
