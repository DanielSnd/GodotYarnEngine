//
// Created by Daniel on 2024-05-01.
//

#ifndef YSCENESPAWNER3D_H
#define YSCENESPAWNER3D_H
#include "core/math/random_number_generator.h"
#include "scene/3d/node_3d.h"
#include "core/templates/local_vector.h"
#include "core/variant/typed_array.h"
#include "scene/resources/packed_scene.h"
#include "yarnphysics.h"
#include "yarntime.h"

class YSceneSpawner3D : public Node3D {
    GDCLASS(YSceneSpawner3D , Node3D);

private:
    static void _bind_methods();

    void _notification(int p_what);

    void attempt_respawn_objects();

    void spawn_objects();

    void clear_spawned_objects() const;

protected:
#ifdef TOOLS_ENABLED
    bool _set(const StringName &p_name, const Variant &p_value);
    bool _get(const StringName &p_name, Variant &r_ret) const;
    void _get_property_list(List<PropertyInfo> *p_list) const;
#endif

    void add_spawnable_scene(const String &p_path);

    int get_spawnable_scene_count() const;

    String get_spawnable_scene(int p_idx) const;

    void clear_spawnable_scenes();

    TypedArray<String> _get_spawnable_scenes() const;

    void _set_spawnable_scenes(const TypedArray<String> &p_scenes);

public:
    struct SpawnableScene {
        String path;
        Ref<PackedScene> cache;
    };

    LocalVector<SpawnableScene> spawnable_scenes;
    Vector<ObjectID> spawned_nodes;

    float placing_radius = 10.0;
    bool auto_spawn_on_ready = false;
    bool despawn_when_destroyed = false;
    bool spawn_one_of_each = false;
    bool align_to_ground = false;
    bool debug_show_spawn_area = false;
    bool debug_spawn_messages = false;
    Vector2i spawn_amount = Vector2i{1,1};
    bool random_y_rotate = false;
    bool random_z_rotate = false;
    uint32_t lock_to_layer;
    bool random_y_scale = false;
    bool randomize_scale = false;
    Vector2 min_max_random_scale = Vector2{1,1};
    float prevent_stacking_radius = 0.0;
    uint32_t prevent_stacking_layer;
    bool prevent_spawn_under = false;
    bool force_spawn_under = false;
    float under_value = 0.0;

    int respawn_only_until_maximum = 0;
    int respawns_after_time = 0;

    // debug_spawn_messages
    bool get_debug_spawn_messages() const { return debug_spawn_messages; }
    void set_debug_spawn_messages(bool value) { debug_spawn_messages = value; }

    // despawn_when_destroyed
    bool get_despawn_when_destroyed() const { return despawn_when_destroyed; }
    void set_despawn_when_destroyed(bool value) { despawn_when_destroyed = value; }

    // respawn_only_until_maximum
    int get_respawn_only_until_maximum() const { return respawn_only_until_maximum; }
    void set_respawn_only_until_maximum(int value) { respawn_only_until_maximum = value; }

    // respawns_after_time
    int get_respawns_after_time() const { return respawns_after_time; }
    void set_respawns_after_time(int value) { respawns_after_time = value; }

    // placing_radius
    float get_placing_radius() const { return placing_radius; }
    void set_placing_radius(float f) { placing_radius = f; }

    // prevent_spawn_under
    bool get_prevent_spawn_under() const { return prevent_spawn_under; }
    void set_prevent_spawn_under(bool value) { prevent_spawn_under = value; }

    // force_spawn_under
    bool get_force_spawn_under() const { return force_spawn_under; }
    void set_force_spawn_under(bool value) { force_spawn_under = value; }

    // under_value
    float get_under_value() const { return under_value; }
    void set_under_value(float value) { under_value = value; }

    // prevent_stacking_radius
    float get_prevent_stacking_radius() const { return prevent_stacking_radius; }
    void set_prevent_stacking_radius(float f) { prevent_stacking_radius = f; }

    // align_to_ground
    bool get_align_to_ground() const { return align_to_ground; }
    void set_align_to_ground(bool b) { align_to_ground = b; }

    // auto_spawn_on_ready
    bool get_auto_spawn_on_ready() const { return auto_spawn_on_ready; }
    void set_auto_spawn_on_ready(bool b) { auto_spawn_on_ready = b; }

    // debug_show_spawn_area
    bool get_debug_show_spawn_area() const { return debug_show_spawn_area; }
    void set_debug_show_spawn_area(bool b) { debug_show_spawn_area = b; }

    // spawn_amount
    Vector2i get_spawn_amount() const { return spawn_amount; }
    void set_spawn_amount(Vector2i v) { spawn_amount = v; }

    // random_y_rotate
    bool get_random_y_rotate() const { return random_y_rotate; }
    void set_random_y_rotate(bool b) { random_y_rotate = b; }

    // random_z_rotate
    bool get_random_z_rotate() const { return random_z_rotate; }
    void set_random_z_rotate(bool b) { random_z_rotate = b; }

    // lock_to_layer
    int get_lock_to_layer() const { return lock_to_layer; }
    void set_lock_to_layer(int i) { lock_to_layer = i; }

    // prevent_stacking_layer
    int get_prevent_stacking_layer() const { return prevent_stacking_layer; }
    void set_prevent_stacking_layer(int i) { prevent_stacking_layer = i; }

    // random_y_scale
    bool get_random_y_scale() const { return random_y_scale; }
    void set_random_y_scale(bool b) { random_y_scale = b; }

    // randomize_scale
    bool get_randomize_scale() const { return randomize_scale; }
    void set_randomize_scale(bool b) { randomize_scale = b; }

    // min_max_random_scale
    Vector2 get_min_max_random_scale() const { return min_max_random_scale; }
    void set_min_max_random_scale(Vector2 v) { min_max_random_scale = v; }

    Ref<RandomNumberGenerator> spawning_rng;
    Node3D* spawn_single(const Ref<PackedScene> &spawn_scene,Vector3 spawn_pos, Vector3 spawn_normal = Vector3{0.0,0.0,0.0});

    void remove_from_spawned_nodes(ObjectID removing_id);

    void do_spawn(const Vector<Ref<PackedScene>> &spawn_list);
    Node3D* add_object_random_position(const Ref<PackedScene> &p_packed_scene);
    Dictionary random_position_on_registred_surface() const;

    bool registered_next_spawn_time = false;

    YSceneSpawner3D();
};



#endif //YSCENESPAWNER3D_H
