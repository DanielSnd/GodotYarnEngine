#ifndef YPERSISTENTID_H
#define YPERSISTENTID_H

#include "scene/main/node.h"
#include "core/templates/hash_map.h"
#include "core/object/object.h"
#include "scene/3d/node_3d.h"
#include "core/config/project_settings.h"
#include "yarntime.h"
#include "scene/resources/packed_scene.h"

class PackedScene;

class YPersistentID : public Node {
    GDCLASS(YPersistentID, Node);

private:
    static void _bind_methods();
    void _notification(int p_what);

    // The persistent ID for this node
    uint64_t persistent_id;
    
    // Static map to store all persistent IDs and their corresponding nodes
    static HashMap<uint64_t, YPersistentID*> persistent_id_map;
    
    // Static map to store deleted persistent IDs with their deletion times
    static HashMap<uint64_t, int> deleted_persistent_ids;
    
    // Static map to store scene paths and their assigned IDs
    static HashMap<String, int> scene_path_to_id;
    static HashMap<int, String> packed_scene_id_to_scene_path;
    
    // Static counter for generating new scene path IDs
    static int next_scene_path_id;
    
    // Static counter for generating new IDs
    static uint64_t next_persistent_id;

    // Respawn information
    struct RespawnInfo {
        int scene_path_id;
        Transform3D global_transform;
    };

    // Static map to store respawn info for deleted nodes
    static HashMap<uint64_t, RespawnInfo> respawn_info_map;

    // Helper methods
    void generate_new_persistent_id();
    void register_persistent_id();
    void unregister_persistent_id();
    int get_or_create_scene_path_id(const String& p_path);

protected:
    // Getters and setters for the persistent ID
    uint64_t get_persistent_id() const { return persistent_id; }
    void set_persistent_id(uint64_t p_id) { persistent_id = p_id; }

public:
    // GDScript bound methods
    static Node* find_by_persistent_id(uint64_t p_id);
    static void add_to_deleted_ids(uint64_t p_id, int p_time = 0);
    static void remove_from_deleted_ids(uint64_t p_id);
    static Dictionary get_deleted_ids_with_times();
    static void clear_deleted_ids();
    static int count_deleted_ids();
    static int get_persistent_id_from_node(Node* p_node);

    // Scene path methods
    bool is_saved_scene() const;
    String get_scene_path() const;
    void store_respawn_info();

    // Getters for respawn info
    static Array get_respawn_info(uint64_t p_id);
    static bool has_respawn_info(uint64_t p_id);
    static void remove_respawn_info(uint64_t p_id);

    // Scene path ID management
    static int get_scene_path_id(const String& p_path);
    static String get_scene_path_from_id(int p_id);
    static Dictionary get_all_scene_paths_and_ids();

    // Respawn functionality
    static Node* respawn(uint64_t p_id, Node* p_parent);

    // Cleanup functionality
    static void cleanup_invalidly_spawned_nodes();

    YPersistentID();
    ~YPersistentID();
};

#endif // YPERSISTENTID_H