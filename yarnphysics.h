//
// Created by Daniel on 2024-03-03.
//

#ifndef YARNPHYSICS_H
#define YARNPHYSICS_H

#include "scene/main/scene_tree.h"
#include "scene/main/window.h"
#include "scene/resources/world_2d.h"
#include "scene/resources/2d/shape_2d.h"
#include "scene/resources/3d/world_3d.h"
#include "servers/physics_server_2d.h"
#include "core/object/ref_counted.h"
#include "core/variant/typed_array.h"
#include "scene/resources/2d/shape_2d.h"
#include "scene/resources/3d/shape_3d.h"

class IntersectResult : public RefCounted {
    GDCLASS(IntersectResult, RefCounted);

    public:
        Vector3 position;
        Vector3 normal;
        int collider_id;
        RID rid;
        Node* collider;
        Node* node;
};

class YPhysics : public RefCounted {
    GDCLASS(YPhysics, RefCounted);

protected:
    static YPhysics* singleton;

    static void _bind_methods();

public:
    static YPhysics *get_singleton();

    enum CollideType {
        COLLIDE_WITH_BODIES = 0,
         COLLIDE_WITH_AREAS = 1,
        COLLIDE_WITH_BOTH = 2
    };

    static Vector3 stored_position_hit;
    static Vector3 stored_hit_normal;
    static ObjectID stored_collider_id;
    static Vector2 stored_position_hit_2d;
    static Vector2 stored_hit_normal_2d;
    static ObjectID stored_collider_id_2d;


    static RID sphere_rid;
    static bool has_sphere_shape;
    static RID circle_rid;
    static bool has_circle_shape;
    static Dictionary raycast3d(Vector3 ray_origin, Vector3 ray_dir, float ray_dist, CollideType _collide_type = COLLIDE_WITH_BODIES, uint32_t layer_mask = UINT32_MAX, const TypedArray<RID> &p_exclude = {});
    static Dictionary raycast2d_to(Vector2 ray_origin, Vector2 ray_end, CollideType _collide_type = COLLIDE_WITH_BODIES, uint32_t layer_mask = UINT32_MAX, const TypedArray<RID> &p_exclude = {});
    static bool has_raycast3d_hit(Vector3 ray_origin, Vector3 ray_dir, float ray_dist, CollideType _collide_type = COLLIDE_WITH_BODIES, uint32_t layer_mask = UINT32_MAX, const TypedArray<RID> &p_exclude = {});
    static bool has_raycast2d_hit(Vector2 ray_origin, Vector2 ray_dir, float ray_dist, CollideType _collide_type = COLLIDE_WITH_BODIES, uint32_t layer_mask = UINT32_MAX, const TypedArray<RID> &p_exclude = {});

    static bool has_spherecast3d_hit(Vector3 ray_origin, real_t radius, Vector3 ray_dir, float ray_dist, CollideType _collide_type = COLLIDE_WITH_BODIES, uint32_t collision_mask = UINT32_MAX, const TypedArray<RID> &p_exclude = {});

    static Ref<IntersectResult> dictionary_to_intersect_result(const Dictionary &p_dict);
    static Vector3 get_stored_hit_position();
    static Vector3 get_stored_hit_normal();
    static Node* get_stored_hit_collider();
    static Vector2 get_stored_hit_position_2d();
    static Vector2 get_stored_hit_normal_2d();
    static Node* get_stored_hit_collider_2d();
 
    static bool free_line_check(Vector3 ray_origin, Vector3 ray_end, CollideType _collide_type = COLLIDE_WITH_BODIES, uint32_t layer_mask = UINT32_MAX, const TypedArray<RID> &p_exclude = {});
    static bool free_line_check_2d(Vector2 ray_origin, Vector2 ray_end, CollideType _collide_type = COLLIDE_WITH_BODIES, uint32_t layer_mask = UINT32_MAX, const TypedArray<RID> &p_exclude = {});

    static Dictionary raycast3d_to(Vector3 ray_origin, Vector3 ray_end, CollideType _collide_type = COLLIDE_WITH_BODIES, uint32_t layer_mask = UINT32_MAX, const TypedArray<RID> &p_exclude = {});
    
    static TypedArray<Dictionary> _intersect_sphere(Vector3 p_world_position, real_t radius, CollideType _collide_type = COLLIDE_WITH_BODIES,
                                             int p_max_results = 32, uint32_t collision_mask = UINT32_MAX, const TypedArray<RID> &p_exclude = {});

    static TypedArray<Dictionary> _intersect_point_3d(Vector3 p_world_position, CollideType _collide_type = COLLIDE_WITH_BODIES,
                                             int p_max_results = 32, uint32_t collision_mask = UINT32_MAX, const TypedArray<RID> &p_exclude = {});

    static TypedArray<Dictionary> _intersect_shape_3d(const Ref<Shape3D> &p_shape, const Transform3D &p_world_transform,
                                                real_t margin, CollideType _collide_type, int p_max_results,
                                                uint32_t collision_mask = UINT32_MAX, const TypedArray<RID> &p_exclude = {});

    static Dictionary check_rest_info(const Ref<Shape3D> &p_shape, const Transform3D &p_world_transform, real_t p_margin,
                                      CollideType _collide_type = COLLIDE_WITH_BODIES, uint32_t collision_mask = UINT32_MAX, const TypedArray<RID> &p_exclude = {});

    static Object *check_collision_sphere(Vector3 p_world_position, real_t radius, uint32_t collision_mask = UINT32_MAX, const TypedArray<RID> &p_exclude = {});

    static TypedArray<Dictionary> spherecast(const Vector3 p_world_position, real_t radius, const Vector3 &p_dir, float p_max_dist, int p_max_results,
                                             CollideType p_collide_type, uint32_t collision_mask, const TypedArray<RID> &p_exclude);

    static TypedArray<Dictionary> shapecast(const Ref<Shape3D> &p_shape, const Transform3D &p_world_transform, real_t p_margin, const Vector3 &p_dir, float p_max_dist, int
                                            p_max_results,
                                            CollideType p_collide_type, uint32_t collision_mask, const TypedArray<RID> &p_exclude);

    static bool free_sphere_check(Vector3 p_world_position, real_t radius, uint32_t collision_mask = UINT32_MAX, const TypedArray<RID> &p_exclude = {});
    static bool free_circle_check_2d(Vector2 p_world_position, real_t radius, uint32_t collision_mask = UINT32_MAX, const TypedArray<RID> &p_exclude = {});

    static Vector<real_t> cast_motion(const Ref<Shape3D> &p_shape, const Transform3D &p_world_transform, real_t p_margin,
                                      const Vector3 &p_motion, CollideType p_collide_type = COLLIDE_WITH_BODIES, uint32_t p_collision_mask = UINT32_MAX,
                                      const TypedArray<RID> &p_exclude = {});

    static Vector<real_t> cast_motion_2D(const Ref<Shape2D> &p_shape, const Transform2D &p_world_transform, real_t p_margin,
                                  const Vector2 &p_motion, CollideType p_collide_type, uint32_t p_collision_mask,
                                  const TypedArray<RID> &p_exclude);

    static Dictionary raycast2d(Vector2 ray_origin, Vector2 ray_dir, float ray_dist, CollideType _collide_type = COLLIDE_WITH_BODIES, uint32_t layer_mask = UINT32_MAX, const TypedArray<RID> &p_exclude = {});

    YPhysics();
    ~YPhysics();
};

VARIANT_ENUM_CAST(YPhysics::CollideType);

#endif //YARNPHYSICS
