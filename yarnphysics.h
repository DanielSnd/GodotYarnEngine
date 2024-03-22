//
// Created by Daniel on 2024-03-03.
//

#ifndef YARNPHYSICS_H
#define YARNPHYSICS_H
#include "core/object/ref_counted.h"
#include "core/variant/typed_array.h"

class YPhysics : public RefCounted {
    GDCLASS(YPhysics, RefCounted);

protected:
    static Ref<YPhysics> singleton;

    static void _bind_methods();


public:
    static YPhysics *get_singleton();

    static void set_singleton(const Ref<YPhysics> &ref) {
        singleton = ref;
    }

    enum CollideType {
        COLLIDE_WITH_BODIES = 0,
         COLLIDE_WITH_AREAS = 1,
        COLLIDE_WITH_BOTH = 2
    };

    RID sphere_rid;
    bool has_sphere_shape = false;
    Dictionary raycast3d(Vector3 ray_origin, Vector3 ray_dir, float ray_dist, CollideType _collide_type, uint32_t layer_mask = UINT32_MAX);
    Dictionary raycast2d_to(Vector2 ray_origin, Vector2 ray_end, CollideType _collide_type, uint32_t layer_mask = UINT32_MAX);
    Dictionary raycast3d_to(Vector3 ray_origin, Vector3 ray_end, CollideType _collide_type, uint32_t layer_mask = UINT32_MAX);

    TypedArray<Dictionary> _intersect_sphere(Vector3 p_world_position, real_t radius, CollideType _collide_type,
                                             int p_max_results, uint32_t collision_mask);

    Dictionary raycast2d(Vector2 ray_origin, Vector2 ray_dir, float ray_dist, CollideType _collide_type, uint32_t layer_mask = UINT32_MAX);

    YPhysics();
    ~YPhysics();
};

VARIANT_ENUM_CAST(YPhysics::CollideType);

#endif //YARNPHYSICS_H
