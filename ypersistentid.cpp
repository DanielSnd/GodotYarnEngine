#include "ypersistentid.h"

// Initialize static members
HashMap<uint64_t, YPersistentID*> YPersistentID::persistent_id_map;
HashMap<uint64_t, int> YPersistentID::deleted_persistent_ids;
HashMap<String, int> YPersistentID::scene_path_to_id;
HashMap<int, String> YPersistentID::packed_scene_id_to_scene_path;
HashMap<uint64_t, YPersistentID::RespawnInfo> YPersistentID::respawn_info_map;
int YPersistentID::next_scene_path_id = 1;
uint64_t YPersistentID::next_persistent_id = 1;

void YPersistentID::_bind_methods() {
    // Instance methods
    ClassDB::bind_method(D_METHOD("get_persistent_id"), &YPersistentID::get_persistent_id);
    ClassDB::bind_method(D_METHOD("set_persistent_id", "id"), &YPersistentID::set_persistent_id);
    ClassDB::bind_method(D_METHOD("store_respawn_info"), &YPersistentID::store_respawn_info);
    ClassDB::bind_method(D_METHOD("is_saved_scene"), &YPersistentID::is_saved_scene);
    ClassDB::bind_method(D_METHOD("get_scene_path"), &YPersistentID::get_scene_path);

    // Static methods
    ClassDB::bind_static_method("YPersistentID", D_METHOD("get_persistent_id_from_node", "node"), &YPersistentID::get_persistent_id_from_node);
    ClassDB::bind_static_method("YPersistentID", D_METHOD("find_by_persistent_id", "id"), &YPersistentID::find_by_persistent_id);
    ClassDB::bind_static_method("YPersistentID", D_METHOD("add_to_deleted_ids", "id", "time"), &YPersistentID::add_to_deleted_ids);
    ClassDB::bind_static_method("YPersistentID", D_METHOD("remove_from_deleted_ids", "id"), &YPersistentID::remove_from_deleted_ids);
    ClassDB::bind_static_method("YPersistentID", D_METHOD("get_deleted_ids_with_times"), &YPersistentID::get_deleted_ids_with_times);
    ClassDB::bind_static_method("YPersistentID", D_METHOD("get_respawn_info", "id"), &YPersistentID::get_respawn_info);
    ClassDB::bind_static_method("YPersistentID", D_METHOD("has_respawn_info", "id"), &YPersistentID::has_respawn_info);
    ClassDB::bind_static_method("YPersistentID", D_METHOD("remove_respawn_info", "id"), &YPersistentID::remove_respawn_info);
    ClassDB::bind_static_method("YPersistentID", D_METHOD("get_scene_path_id", "path"), &YPersistentID::get_scene_path_id);
    ClassDB::bind_static_method("YPersistentID", D_METHOD("get_scene_path_from_id", "id"), &YPersistentID::get_scene_path_from_id);
    ClassDB::bind_static_method("YPersistentID", D_METHOD("get_all_scene_paths_and_ids"), &YPersistentID::get_all_scene_paths_and_ids);
    ClassDB::bind_static_method("YPersistentID", D_METHOD("count_deleted_ids"), &YPersistentID::count_deleted_ids);
    ClassDB::bind_static_method("YPersistentID", D_METHOD("clear_deleted_ids"), &YPersistentID::clear_deleted_ids);
    ClassDB::bind_static_method("YPersistentID", D_METHOD("respawn", "id", "parent"), &YPersistentID::respawn);
    ClassDB::bind_static_method("YPersistentID", D_METHOD("cleanup_invalidly_spawned_nodes"), &YPersistentID::cleanup_invalidly_spawned_nodes);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "persistent_id"), "set_persistent_id", "get_persistent_id");
}

YPersistentID::YPersistentID() {
    // Constructor
    persistent_id = 0;
}

YPersistentID::~YPersistentID() {
    // Destructor
    unregister_persistent_id();
}

void YPersistentID::generate_new_persistent_id() {
    if (next_persistent_id == 0) {
        next_persistent_id = ProjectSettings::get_singleton()->get_setting("yarnengine/next_persistent_id", 0);
    }

    // Generate a new unique ID
    persistent_id = next_persistent_id++;
    
    // If we're in the editor, update the counter in editor settings
    #ifdef TOOLS_ENABLED
    if (Engine::get_singleton()->is_editor_hint()) {
        ProjectSettings::get_singleton()->set_setting("yarnengine/next_persistent_id", next_persistent_id);
        ProjectSettings::get_singleton()->save();
    }
    #endif
}

void YPersistentID::register_persistent_id() {
    if (persistent_id == 0) {
        generate_new_persistent_id();
    }
    
    // Check if this ID is already registered to a different object
    if (persistent_id_map.has(persistent_id)) {
        YPersistentID* existing = persistent_id_map[persistent_id];
        if (existing != this) {
            // ID is already in use by another object, generate a new one
            generate_new_persistent_id();
        }
    }
    
    // Register this object with its ID
    persistent_id_map[persistent_id] = this;
}

void YPersistentID::unregister_persistent_id() {
    if (persistent_id != 0 && persistent_id_map.has(persistent_id)) {
        store_respawn_info();
        if (persistent_id_map[persistent_id] == this) {
            persistent_id_map.erase(persistent_id);
        }
    }
}

Node* YPersistentID::find_by_persistent_id(uint64_t p_id) {
    if (persistent_id_map.has(p_id)) {
        return persistent_id_map[p_id];
    }
    return nullptr;
}

void YPersistentID::add_to_deleted_ids(uint64_t p_id, int p_time) {
    deleted_persistent_ids[p_id] = p_time;
}

void YPersistentID::remove_from_deleted_ids(uint64_t p_id) {
    if (deleted_persistent_ids.has(p_id)) {
        deleted_persistent_ids.erase(p_id);
    }
}

Dictionary YPersistentID::get_deleted_ids_with_times() {
    Dictionary result;
    for (const auto& pair : deleted_persistent_ids) {
        result[pair.key] = pair.value;
    }
    return result;
}

void YPersistentID::clear_deleted_ids() {
    deleted_persistent_ids.clear();
}

int YPersistentID::count_deleted_ids()
{
    return deleted_persistent_ids.size();
}

int YPersistentID::get_persistent_id_from_node(Node *p_node)
{
    if (p_node == nullptr || !p_node->has_meta("persistent_id")) {
        return -1;
    }
    return p_node->get_meta("persistent_id");
}

void YPersistentID::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
            register_persistent_id();
        }
        break;
        case NOTIFICATION_READY: {
            // Check if this persistent ID was previously deleted
            if (deleted_persistent_ids.has(persistent_id)) {
                // If it was deleted, queue free this node
                queue_free();
                return;
            }
            // Get parent and set the persistent ID as a meta property on the parent.
            Node* parent = get_parent();
            if (parent != nullptr) {
                parent->set_meta("persistent_id", persistent_id);
            }
        }
        break;
        case NOTIFICATION_EXIT_TREE: {
            unregister_persistent_id();
        }
        break;
    }   
}

bool YPersistentID::is_saved_scene() const {
    // Check if this node is part of a scene
    Node* parent_node = Object::cast_to<Node>(get_parent());
    if (parent_node != nullptr) {
        // Get the scene path from the parent node
        String scene_path = parent_node->get_scene_file_path();
        return !scene_path.is_empty();
    }
    return false;
}

String YPersistentID::get_scene_path() const {
    // Get the scene path from the parent node
    Node* parent_node = Object::cast_to<Node>(get_parent());
    if (parent_node != nullptr) {
        return parent_node->get_scene_file_path();
    }
    return String();
}

int YPersistentID::get_scene_path_id(const String& p_path) {
    if (scene_path_to_id.has(p_path)) {
        return scene_path_to_id[p_path];
    }
    
    // Create new ID for this scene path
    int new_id = next_scene_path_id++;
    scene_path_to_id[p_path] = new_id;
    packed_scene_id_to_scene_path[new_id] = p_path;
    return new_id;
}

String YPersistentID::get_scene_path_from_id(int p_id) {
    if (packed_scene_id_to_scene_path.has(p_id)) {
        return packed_scene_id_to_scene_path[p_id];
    }
    return String();
}

Dictionary YPersistentID::get_all_scene_paths_and_ids() {
    Dictionary result;
    for (const auto& pair : scene_path_to_id) {
        result[pair.key] = pair.value;
    }
    return result;
}

void YPersistentID::store_respawn_info() {
    // Check if parent is a saved scene
    if (is_saved_scene()) {
        // Get the scene path
        String scene_path = get_scene_path();
        
        // Get or create an ID for this scene path
        RespawnInfo info;
        info.scene_path_id = get_scene_path_id(scene_path);
        
        // Get the parent Node3D and store its global transform
        Node3D* parent_3d = Object::cast_to<Node3D>(get_parent());
        if (parent_3d != nullptr) {
            info.global_transform = parent_3d->get_global_transform();
        }

        // Store the respawn info in the static map
        respawn_info_map[persistent_id] = info;
    }
}

Array YPersistentID::get_respawn_info(uint64_t p_id) {
    Array returnArray;
    if (respawn_info_map.has(p_id)) {
        returnArray.push_back(respawn_info_map[p_id].scene_path_id);
        returnArray.push_back(respawn_info_map[p_id].global_transform);
    }
    return returnArray;
}

bool YPersistentID::has_respawn_info(uint64_t p_id) {
    return respawn_info_map.has(p_id);
}

void YPersistentID::remove_respawn_info(uint64_t p_id) {
    if (respawn_info_map.has(p_id)) {
        respawn_info_map.erase(p_id);
    }
}

Node* YPersistentID::respawn(uint64_t p_id, Node* p_parent) {
    // Check if we have respawn info for this ID
    if (!has_respawn_info(p_id)) {
        return nullptr;
    }

    // Get the respawn info
    Array respawn_data = get_respawn_info(p_id);
    if (respawn_data.size() < 2) {
        return nullptr;
    }

    int scene_path_id = respawn_data[0];
    Transform3D global_transform = respawn_data[1];

    // Get the scene path from the ID
    String scene_path = get_scene_path_from_id(scene_path_id);
    if (scene_path.is_empty()) {
        return nullptr;
    }

    // Load and instantiate the scene
    Node *spawned_node = nullptr;

    if (ResourceLoader::exists(scene_path) && ResourceLoader::get_resource_type(scene_path) == "PackedScene") {
		// Cache the scene reference before loading it (for cyclic references)
		Ref<PackedScene> scn;
		scn.instantiate();
		scn->set_path(scene_path);
		scn->reload_from_file();
		ERR_FAIL_COND_V_MSG(scn.is_null(), nullptr, vformat("Failed to respawn YPersistentID, can't load from path: %s.", scene_path));

		if (scn.is_valid()) {
			spawned_node = scn->instantiate();
		}
    }

    if (spawned_node == nullptr) {
        return nullptr;
    }

    // Find the YPersistentID node in the spawned scene
    YPersistentID* persistent_id_node = nullptr;
    for (int i = 0; i < spawned_node->get_child_count(); i++) {
        YPersistentID* child = Object::cast_to<YPersistentID>(spawned_node->get_child(i));
        if (child != nullptr) {
            persistent_id_node = child;
            break;
        }
    }

    // If we found a YPersistentID node, set its ID
    if (persistent_id_node != nullptr) {
        persistent_id_node->set_persistent_id(p_id);
    }
    
    // Remove the respawn info since we've used it
    remove_respawn_info(p_id);
    deleted_persistent_ids.erase(p_id);

    // Add the spawned node to the parent
    p_parent->add_child(spawned_node);

    // If the spawned node is a Node3D, set its global transform
    Node3D* node_3d = Object::cast_to<Node3D>(spawned_node);
    if (node_3d) {
        node_3d->set_global_transform(global_transform);
    }


    return spawned_node;
}

void YPersistentID::cleanup_invalidly_spawned_nodes() {
    // Create a list of nodes to delete to avoid modifying the map while iterating
    List<Node*> nodes_to_delete;

    // Iterate through all deleted IDs
    for (const auto& pair : deleted_persistent_ids) {
        uint64_t deleted_id = pair.key;
        
        // Check if this ID is currently in the persistent_id_map
        if (persistent_id_map.has(deleted_id)) {
            YPersistentID* persistent_node = persistent_id_map[deleted_id];
            
            // Check if the node is still valid and in the tree
            if (persistent_node && persistent_node->is_inside_tree()) {
                // Get the parent node and add it to our deletion list
                Node* parent = persistent_node->get_parent();
                if (parent) {
                    nodes_to_delete.push_back(parent);
                }
            }
        }
    }

    // Delete all the collected nodes
    for (Node* node : nodes_to_delete) {
        if (node && node->is_inside_tree()) {
            node->queue_free();
        }
    }
}

