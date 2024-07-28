//
// Created by Daniel on 2024-03-03.
//

#include "yarnphysics.h"

#include "scene/main/scene_tree.h"
#include "scene/main/window.h"
#include "scene/resources/world_2d.h"
#include "scene/resources/2d/shape_2d.h"
#include "scene/resources/3d/world_3d.h"
#include "servers/physics_server_2d.h"

Ref<YPhysics> YPhysics::singleton;
bool YPhysics::has_sphere_shape = false;
RID YPhysics::sphere_rid;

void YPhysics::_bind_methods() {
    ClassDB::bind_static_method("YPhysics",D_METHOD("raycast2d_to", "ray_origin", "ray_end", "collide_type", "collision_mask", "exclude"), &YPhysics::raycast2d_to,DEFVAL(COLLIDE_WITH_BODIES),DEFVAL(UINT32_MAX), DEFVAL(TypedArray<RID>()));
    ClassDB::bind_static_method("YPhysics",D_METHOD("raycast3d_to", "ray_origin", "ray_end", "collide_type", "collision_mask", "exclude"), &YPhysics::raycast3d_to,DEFVAL(COLLIDE_WITH_BODIES),DEFVAL(UINT32_MAX), DEFVAL(TypedArray<RID>()));
    ClassDB::bind_static_method("YPhysics",D_METHOD("free_line_check", "ray_origin", "ray_end", "collide_type", "collision_mask", "exclude"), &YPhysics::free_line_check,DEFVAL(COLLIDE_WITH_BODIES),DEFVAL(UINT32_MAX), DEFVAL(TypedArray<RID>()));
    ClassDB::bind_static_method("YPhysics",D_METHOD("raycast2d", "ray_origin", "ray_direction", "ray_distance", "collide_type", "collision_mask", "exclude"), &YPhysics::raycast2d,DEFVAL(COLLIDE_WITH_BODIES),DEFVAL(UINT32_MAX), DEFVAL(TypedArray<RID>()));
    ClassDB::bind_static_method("YPhysics",D_METHOD("raycast3d", "ray_origin", "ray_direction", "ray_distance", "collide_type", "collision_mask", "exclude"), &YPhysics::raycast3d,DEFVAL(COLLIDE_WITH_BODIES),DEFVAL(UINT32_MAX), DEFVAL(TypedArray<RID>()));
    ClassDB::bind_static_method("YPhysics",D_METHOD("intersect_sphere", "world_position", "radius", "collide_type","max_results", "collision_mask", "exclude"), &YPhysics::_intersect_sphere,DEFVAL(COLLIDE_WITH_BODIES),DEFVAL(32),DEFVAL(UINT32_MAX), DEFVAL(TypedArray<RID>()));
    ClassDB::bind_static_method("YPhysics",D_METHOD("intersect_shape_3d", "world_position", "margin", "collide_type","max_results", "collision_mask", "exclude"), &YPhysics::_intersect_shape_3d,DEFVAL(0.04),DEFVAL(COLLIDE_WITH_BODIES),DEFVAL(32),DEFVAL(UINT32_MAX), DEFVAL(TypedArray<RID>()));
    ClassDB::bind_static_method("YPhysics",D_METHOD("check_collision_sphere", "world_position", "radius", "collision_mask", "exclude"), &YPhysics::check_collision_sphere,DEFVAL(1.0),DEFVAL(UINT32_MAX), DEFVAL(TypedArray<RID>()));
    ClassDB::bind_static_method("YPhysics",D_METHOD("free_sphere_check", "world_position", "radius", "collision_mask", "exclude"), &YPhysics::free_sphere_check,DEFVAL(1.0),DEFVAL(UINT32_MAX), DEFVAL(TypedArray<RID>()));
    ClassDB::bind_static_method("YPhysics",D_METHOD("check_rest_info_3d", "shape", "world_transform", "margin", "collide_type", "collision_mask", "exclude"),
                         &YPhysics::check_rest_info,
                         DEFVAL(0.04),
                         DEFVAL(COLLIDE_WITH_BODIES),
                         DEFVAL(UINT32_MAX),
                         DEFVAL(TypedArray<RID>()));
    ClassDB::bind_static_method("YPhysics",D_METHOD("cast_motion_2d", "shape", "world_transform", "margin", "motion", "collide_type", "collision_mask", "exclude"),
                     &YPhysics::cast_motion,
                     DEFVAL(0.04),
                     DEFVAL(Vector2()),
                     DEFVAL(COLLIDE_WITH_BODIES),
                     DEFVAL(UINT32_MAX),
                     DEFVAL(TypedArray<RID>()));
    ClassDB::bind_static_method("YPhysics",D_METHOD("cast_motion_3d", "shape", "world_transform", "margin", "motion", "collide_type", "collision_mask", "exclude"),
                     &YPhysics::cast_motion,
                     DEFVAL(0.04),
                     DEFVAL(Vector3()),
                     DEFVAL(COLLIDE_WITH_BODIES),
                     DEFVAL(UINT32_MAX),
                     DEFVAL(TypedArray<RID>()));
    ClassDB::bind_static_method("YPhysics",D_METHOD("intersect_point_3d", "world_position", "collide_type", "max_results", "collision_mask", "exclude"),
                    &YPhysics::_intersect_point_3d,
                    DEFVAL(YPhysics::COLLIDE_WITH_BODIES),
                    DEFVAL(32),
                    DEFVAL(UINT32_MAX),
                    DEFVAL(TypedArray<RID>()));
    ClassDB::bind_static_method("YPhysics",D_METHOD("shapecast", "shape", "world_transform", "margin", "motion", "max_results", "collide_type", "collision_mask", "exclude"),
                     &YPhysics::shapecast,
                     DEFVAL(0.04),
                     DEFVAL(Vector3()),
                     DEFVAL(32),
                     DEFVAL(YPhysics::COLLIDE_WITH_BODIES),
                     DEFVAL(UINT32_MAX),
                     DEFVAL(TypedArray<RID>()));
    ClassDB::bind_static_method("YPhysics",D_METHOD("spherecast", "world_position", "radius", "motion", "max_results", "collide_type", "collision_mask", "exclude"),
                     &YPhysics::spherecast,
                     DEFVAL(1.0),
                     DEFVAL(Vector3()),
                     DEFVAL(32),
                     DEFVAL(YPhysics::COLLIDE_WITH_BODIES),
                     DEFVAL(UINT32_MAX),
                     DEFVAL(TypedArray<RID>()));
    BIND_ENUM_CONSTANT(COLLIDE_WITH_BODIES);
    BIND_ENUM_CONSTANT(COLLIDE_WITH_AREAS);
    BIND_ENUM_CONSTANT(COLLIDE_WITH_BOTH);
}

Dictionary YPhysics::raycast2d(Vector2 ray_origin, Vector2 ray_dir, float ray_dist, CollideType _collide_type, uint32_t layer_mask, const TypedArray<
                               RID> &p_exclude) {
    return raycast2d_to(ray_origin,ray_origin + ray_dir * ray_dist, _collide_type, layer_mask, p_exclude);
}

Dictionary YPhysics::raycast3d(Vector3 ray_origin, Vector3 ray_dir, float ray_dist, CollideType _collide_type, uint32_t layer_mask, const TypedArray<
                               RID> &p_exclude) {
    return raycast3d_to(ray_origin,ray_origin + ray_dir * ray_dist, _collide_type, layer_mask, p_exclude);
}

Dictionary YPhysics::raycast2d_to(Vector2 ray_origin, Vector2 ray_end, CollideType _collide_type, uint32_t layer_mask, const TypedArray<RID> &p_exclude) {
    Dictionary return_dictionary;
    auto world_2d = SceneTree::get_singleton()->get_root()->get_world_2d();
    if (world_2d.is_valid()) {
        auto space_state = world_2d->get_direct_space_state();
        if (space_state != nullptr) {
            PhysicsDirectSpaceState2D::RayResult result;
            PhysicsDirectSpaceState2D::RayParameters ray_params;

            ray_params.from = ray_origin;
            ray_params.to = ray_end;
            if (!p_exclude.is_empty()) {
                for (int i = 0; i < p_exclude.size(); i++) {
                    ray_params.exclude.insert(p_exclude[i]);
                }
            }
            if (_collide_type == COLLIDE_WITH_BOTH) {
                ray_params.collide_with_areas = true;
                ray_params.collide_with_bodies = true;
            } else {
                ray_params.collide_with_areas = _collide_type == COLLIDE_WITH_AREAS;
                ray_params.collide_with_bodies = _collide_type == COLLIDE_WITH_BODIES;
            }
            ray_params.collision_mask = layer_mask;
            if (space_state->intersect_ray(ray_params, result)) {
                return_dictionary["position"] = result.position;
                return_dictionary["normal"] = result.normal;
                return_dictionary["collider_id"] = result.collider_id;
                return_dictionary["collider"] = result.collider;
                return_dictionary["shape"] = result.shape;
                return_dictionary["rid"] = result.rid;
            }
        }
    }
    return return_dictionary;
}

bool YPhysics::free_line_check(Vector3 ray_origin, Vector3 ray_end, CollideType _collide_type, uint32_t layer_mask, const TypedArray<RID> &p_exclude) {
    auto world_3d = SceneTree::get_singleton()->get_root()->get_world_3d();
    if (world_3d.is_valid()) {
        auto space_state = world_3d->get_direct_space_state();
        if (space_state != nullptr) {
            PhysicsDirectSpaceState3D::RayResult result;
            PhysicsDirectSpaceState3D::RayParameters ray_params;
            ray_params.from = ray_origin;
            ray_params.to = ray_end;
            if (!p_exclude.is_empty()) {
                for (int i = 0; i < p_exclude.size(); i++) {
                    ray_params.exclude.insert(p_exclude[i]);
                }
            }
            if (_collide_type == COLLIDE_WITH_BOTH) {
                ray_params.collide_with_areas = true;
                ray_params.collide_with_bodies = true;
            } else {
                ray_params.collide_with_areas = _collide_type == COLLIDE_WITH_AREAS;
                ray_params.collide_with_bodies = _collide_type == COLLIDE_WITH_BODIES;
            }
            ray_params.collision_mask = layer_mask;
            if (space_state->intersect_ray(ray_params, result)) {
                return true;
            }
        }
    }
    return false;
}
Dictionary YPhysics::raycast3d_to(Vector3 ray_origin, Vector3 ray_end, CollideType _collide_type, uint32_t layer_mask, const TypedArray<RID> &p_exclude) {
    Dictionary return_dictionary;
    auto world_3d = SceneTree::get_singleton()->get_root()->get_world_3d();
    if (world_3d.is_valid()) {
        auto space_state = world_3d->get_direct_space_state();
        if (space_state != nullptr) {
            PhysicsDirectSpaceState3D::RayResult result;
            PhysicsDirectSpaceState3D::RayParameters ray_params;
            ray_params.from = ray_origin;
            ray_params.to = ray_end;
            if (!p_exclude.is_empty()) {
                for (int i = 0; i < p_exclude.size(); i++) {
                    ray_params.exclude.insert(p_exclude[i]);
                }
            }
            if (_collide_type == COLLIDE_WITH_BOTH) {
                ray_params.collide_with_areas = true;
                ray_params.collide_with_bodies = true;
            } else {
                ray_params.collide_with_areas = _collide_type == COLLIDE_WITH_AREAS;
                ray_params.collide_with_bodies = _collide_type == COLLIDE_WITH_BODIES;
            }
            ray_params.collision_mask = layer_mask;
            if (space_state->intersect_ray(ray_params, result)) {
                return_dictionary["position"] = result.position;
                return_dictionary["normal"] = result.normal;
                return_dictionary["face_index"] = result.face_index;
                return_dictionary["collider_id"] = result.collider_id;
                return_dictionary["collider"] = result.collider;
                return_dictionary["shape"] = result.shape;
                return_dictionary["rid"] = result.rid;
            }
        }
    }
    return return_dictionary;
}

TypedArray<Dictionary> YPhysics::_intersect_sphere(Vector3 p_world_position, real_t radius, CollideType _collide_type, int p_max_results, uint32_t collision_mask, const
                                                   TypedArray<RID> &p_exclude) {
    auto world_3d = SceneTree::get_singleton()->get_root()->get_world_3d();
    if (!has_sphere_shape) {
        sphere_rid = PhysicsServer3D::get_singleton()->sphere_shape_create();
    }
    Vector<PhysicsDirectSpaceState3D::ShapeResult> sr;
    sr.resize(p_max_results);
    PhysicsDirectSpaceState3D::ShapeParameters shape_params;
    if (_collide_type == COLLIDE_WITH_BOTH) {
        shape_params.collide_with_areas = true;
        shape_params.collide_with_bodies = true;
    } else {
        shape_params.collide_with_areas = _collide_type == COLLIDE_WITH_AREAS;
        shape_params.collide_with_bodies = _collide_type == COLLIDE_WITH_BODIES;
    }
    if (!p_exclude.is_empty()) {
        for (int i = 0; i < p_exclude.size(); i++) {
            shape_params.exclude.insert(p_exclude[i]);
        }
    }
    shape_params.collision_mask = collision_mask;
    // shape_params.exclude
    shape_params.shape_rid = sphere_rid;
    PhysicsServer3D::get_singleton()->shape_set_data(sphere_rid, radius);
    shape_params.transform = Transform3D{Basis{},p_world_position};
    int rc = world_3d->get_direct_space_state()->intersect_shape(shape_params, sr.ptrw(), p_max_results);
    TypedArray<Dictionary> ret;
    ret.resize(rc);
    for (int i = 0; i < rc; i++) {
        Dictionary d;
        d["rid"] = sr[i].rid;
        d["collider_id"] = sr[i].collider_id;
        d["collider"] = sr[i].collider;
        d["shape"] = sr[i].shape;
        ret[i] = d;
    }
    return ret;
}

TypedArray<Dictionary> YPhysics::_intersect_point_3d(const Vector3 p_world_position, const CollideType _collide_type,
                                                     const int p_max_results, const uint32_t collision_mask, const TypedArray<RID> &p_exclude) {

    auto world_3d = SceneTree::get_singleton()->get_root()->get_world_3d();

    Vector<PhysicsDirectSpaceState3D::ShapeResult> ret;
    ret.resize(p_max_results);
    PhysicsDirectSpaceState3D::PointParameters point_params;

    point_params.position = p_world_position;
    point_params.collision_mask = collision_mask;
    if (_collide_type == COLLIDE_WITH_BOTH) {
        point_params.collide_with_areas = true;
        point_params.collide_with_bodies = true;
    } else {
        point_params.collide_with_areas = _collide_type == COLLIDE_WITH_AREAS;
        point_params.collide_with_bodies = _collide_type == COLLIDE_WITH_BODIES;
    }
    if (!p_exclude.is_empty()) {
        for (int i = 0; i < p_exclude.size(); i++) {
            point_params.exclude.insert(p_exclude[i]);
        }
    }

    const int rc = world_3d->get_direct_space_state()->intersect_point(point_params, ret.ptrw(), static_cast<int>(ret.size()));
    if (rc == 0) {
        return {};
    }

    TypedArray<Dictionary> r;
    r.resize(rc);
    for (int i = 0; i < rc; i++) {
        Dictionary d;
        d["rid"] = ret[i].rid;
        d["collider_id"] = ret[i].collider_id;
        d["collider"] = ret[i].collider;
        d["shape"] = ret[i].shape;
        r[i] = d;
    }
    return r;
}

TypedArray<Dictionary> YPhysics::_intersect_shape_3d(const Ref<Shape3D> &p_shape, const Transform3D &p_world_transform, real_t margin, CollideType _collide_type, int p_max_results, uint32_t collision_mask, const
                                                     TypedArray<RID> &p_exclude) {
    const auto world_3d = SceneTree::get_singleton()->get_root()->get_world_3d();
    Vector<PhysicsDirectSpaceState3D::ShapeResult> sr;
    sr.resize(p_max_results);
    PhysicsDirectSpaceState3D::ShapeParameters shape_params;
    if (_collide_type == COLLIDE_WITH_BOTH) {
        shape_params.collide_with_areas = true;
        shape_params.collide_with_bodies = true;
    } else {
        shape_params.collide_with_areas = _collide_type == COLLIDE_WITH_AREAS;
        shape_params.collide_with_bodies = _collide_type == COLLIDE_WITH_BODIES;
    }
    if (!p_exclude.is_empty()) {
        for (int i = 0; i < p_exclude.size(); i++) {
            shape_params.exclude.insert(p_exclude[i]);
        }
    }
    shape_params.collision_mask = collision_mask;
    shape_params.shape_rid = p_shape->get_rid();
    shape_params.transform = p_world_transform;
    shape_params.margin = margin;
    const int rc = world_3d->get_direct_space_state()->intersect_shape(shape_params, sr.ptrw(), p_max_results);
    TypedArray<Dictionary> ret;
    ret.resize(rc);
    for (int i = 0; i < rc; i++) {
        Dictionary d;
        d["rid"] = sr[i].rid;
        d["collider_id"] = sr[i].collider_id;
        d["collider"] = sr[i].collider;
        d["shape"] = sr[i].shape;
        ret[i] = d;
    }
    return ret;
}

Dictionary YPhysics::check_rest_info(const Ref<Shape3D> &p_shape, const Transform3D &p_world_transform, real_t p_margin, CollideType p_collide_type, uint32_t p_collision_mask, const TypedArray<RID> &p_exclude) {
    // Ensure the shape is valid
    ERR_FAIL_COND_V(!p_shape.is_valid(), Dictionary());

    auto world_3d = SceneTree::get_singleton()->get_root()->get_world_3d();
    PhysicsDirectSpaceState3D::ShapeRestInfo sri;
    PhysicsDirectSpaceState3D::ShapeParameters shape_params;

    shape_params.collision_mask = p_collision_mask;
    shape_params.shape_rid = p_shape->get_rid();
    shape_params.transform = p_world_transform;
    shape_params.margin = p_margin;

    // Determine what we are colliding with
    if (p_collide_type == COLLIDE_WITH_BOTH) {
        shape_params.collide_with_areas = true;
        shape_params.collide_with_bodies = true;
    } else {
        shape_params.collide_with_areas = p_collide_type == COLLIDE_WITH_AREAS;
        shape_params.collide_with_bodies = p_collide_type == COLLIDE_WITH_BODIES;
    }
    if (!p_exclude.is_empty()) {
        for (int i = 0; i < p_exclude.size(); i++) {
            shape_params.exclude.insert(p_exclude[i]);
        }
    }

    // Call the rest info method on the PhysicsDirectSpaceState3D singleton
    const bool res = world_3d->get_direct_space_state()->rest_info(shape_params, &sri);

    Dictionary r;
    if (!res) {
        return r;
    }

    r["point"] = sri.point;
    r["normal"] = sri.normal;
    r["rid"] = sri.rid;
    r["collider_id"] = sri.collider_id;
    r["shape"] = sri.shape;
    r["linear_velocity"] = sri.linear_velocity;

    return r;
}

Object* YPhysics::check_collision_sphere(const Vector3 p_world_position, real_t radius, uint32_t collision_mask, const
                                                   TypedArray<RID> &p_exclude) {
    auto world_3d = SceneTree::get_singleton()->get_root()->get_world_3d();
    if (!has_sphere_shape) {
        sphere_rid = PhysicsServer3D::get_singleton()->sphere_shape_create();
    }
    Vector<PhysicsDirectSpaceState3D::ShapeResult> sr;
    sr.resize(1);
    PhysicsDirectSpaceState3D::ShapeParameters shape_params;
    shape_params.collide_with_areas = false;
    shape_params.collide_with_bodies = true;
    shape_params.collision_mask = collision_mask;
    shape_params.shape_rid = sphere_rid;
    if (!p_exclude.is_empty()) {
        for (int i = 0; i < p_exclude.size(); i++) {
            shape_params.exclude.insert(p_exclude[i]);
        }
    }
    PhysicsServer3D::get_singleton()->shape_set_data(sphere_rid, radius);
    shape_params.transform = Transform3D{Basis{},p_world_position};
    int rc = world_3d->get_direct_space_state()->intersect_shape(shape_params, sr.ptrw(), 1);
    if (rc > 0) {
        return sr[0].collider;
    }
    return nullptr;
}


TypedArray<Dictionary> YPhysics::spherecast(const Vector3 p_world_position, real_t radius, const Vector3 &p_motion, int p_max_results, CollideType p_collide_type, uint32_t collision_mask, const
                                                   TypedArray<RID> &p_exclude) {
    auto world_3d = SceneTree::get_singleton()->get_root()->get_world_3d();
    if (!has_sphere_shape) {
        sphere_rid = PhysicsServer3D::get_singleton()->sphere_shape_create();
    }
    Vector<PhysicsDirectSpaceState3D::ShapeResult> sr;
    sr.resize(p_max_results);
    PhysicsDirectSpaceState3D::ShapeParameters shape_params;
    shape_params.collision_mask = collision_mask;
    shape_params.shape_rid = sphere_rid;
    shape_params.motion = p_motion;

    // Determine what we are colliding with
    if (p_collide_type == COLLIDE_WITH_BOTH) {
        shape_params.collide_with_areas = true;
        shape_params.collide_with_bodies = true;
    } else {
        shape_params.collide_with_areas = p_collide_type == COLLIDE_WITH_AREAS;
        shape_params.collide_with_bodies = p_collide_type == COLLIDE_WITH_BODIES;
    }
    if (!p_exclude.is_empty()) {
        for (int i = 0; i < p_exclude.size(); i++) {
            shape_params.exclude.insert(p_exclude[i]);
        }
    }
    PhysicsServer3D::get_singleton()->shape_set_data(sphere_rid, radius);
    shape_params.transform = Transform3D{Basis{},p_world_position};

    int rc = world_3d->get_direct_space_state()->intersect_shape(shape_params, sr.ptrw(), p_max_results);

    if (rc == 0) {
        return TypedArray<Dictionary>();
    }

    TypedArray<Dictionary> r;
    r.resize(rc);
    for (int i = 0; i < rc; i++) {
        Dictionary d;
        d["rid"] = sr[i].rid;
        d["collider_id"] = sr[i].collider_id;
        d["collider"] = sr[i].collider;
        d["shape"] = sr[i].shape;
        r[i] = d;
    }
    return r;
}

TypedArray<Dictionary> YPhysics::shapecast(const Ref<Shape3D> &p_shape, const Transform3D &p_world_transform,
    real_t p_margin, const Vector3 &p_motion, int p_max_results, CollideType p_collide_type,
    uint32_t collision_mask, const TypedArray<RID> &p_exclude) {

    ERR_FAIL_COND_V(!p_shape.is_valid(), TypedArray<Dictionary>());

    auto world_3d = SceneTree::get_singleton()->get_root()->get_world_3d();

    Vector<PhysicsDirectSpaceState3D::ShapeResult> sr;
    sr.resize(p_max_results);
    PhysicsDirectSpaceState3D::ShapeParameters shape_params;
    shape_params.collision_mask = collision_mask;
    shape_params.shape_rid = p_shape->get_rid();
    shape_params.motion = p_motion;

    // Determine what we are colliding with
    if (p_collide_type == COLLIDE_WITH_BOTH) {
        shape_params.collide_with_areas = true;
        shape_params.collide_with_bodies = true;
    } else {
        shape_params.collide_with_areas = p_collide_type == COLLIDE_WITH_AREAS;
        shape_params.collide_with_bodies = p_collide_type == COLLIDE_WITH_BODIES;
    }
    if (!p_exclude.is_empty()) {
        for (int i = 0; i < p_exclude.size(); i++) {
            shape_params.exclude.insert(p_exclude[i]);
        }
    }
    shape_params.transform = p_world_transform;

    int rc = world_3d->get_direct_space_state()->intersect_shape(shape_params, sr.ptrw(), p_max_results);

    if (rc == 0) {
        return TypedArray<Dictionary>();
    }

    TypedArray<Dictionary> r;
    r.resize(rc);
    for (int i = 0; i < rc; i++) {
        Dictionary d;
        d["rid"] = sr[i].rid;
        d["collider_id"] = sr[i].collider_id;
        d["collider"] = sr[i].collider;
        d["shape"] = sr[i].shape;
        r[i] = d;
    }
    return r;
}

bool YPhysics::free_sphere_check(Vector3 p_world_position, real_t radius, uint32_t collision_mask, const TypedArray<RID> &p_exclude) {
    auto world_3d = SceneTree::get_singleton()->get_root()->get_world_3d();
    if (!has_sphere_shape) {
        sphere_rid = PhysicsServer3D::get_singleton()->sphere_shape_create();
    }
    Vector<PhysicsDirectSpaceState3D::ShapeResult> sr;
    sr.resize(1);
    PhysicsDirectSpaceState3D::ShapeParameters shape_params;
    shape_params.collide_with_areas = false;
    shape_params.collide_with_bodies = true;
    shape_params.collision_mask = collision_mask;
    shape_params.shape_rid = sphere_rid;
    if (!p_exclude.is_empty()) {
        for (int i = 0; i < p_exclude.size(); i++) {
            shape_params.exclude.insert(p_exclude[i]);
        }
    }
    PhysicsServer3D::get_singleton()->shape_set_data(sphere_rid, radius);
    shape_params.transform = Transform3D{Basis{},p_world_position};
    int rc = world_3d->get_direct_space_state()->intersect_shape(shape_params, sr.ptrw(), 1);
    if (rc > 0) {
        return true;
    }
    return false;
}

Vector<real_t> YPhysics::cast_motion(const Ref<Shape3D> &p_shape, const Transform3D &p_world_transform, real_t p_margin, const Vector3 &p_motion, CollideType p_collide_type, uint32_t p_collision_mask, const TypedArray<RID> &p_exclude) {
    // Ensure the shape is valid

    ERR_FAIL_COND_V(!p_shape.is_valid(), Vector<real_t>());

    auto world_3d = SceneTree::get_singleton()->get_root()->get_world_3d();
    PhysicsDirectSpaceState3D::ShapeParameters motion_params;

    motion_params.collision_mask = p_collision_mask;
    motion_params.shape_rid = p_shape->get_rid();
    motion_params.transform = p_world_transform;
    motion_params.margin = p_margin;
    motion_params.motion = p_motion;

    // Determine what we are colliding with
    if (p_collide_type == COLLIDE_WITH_BOTH) {
        motion_params.collide_with_areas = true;
        motion_params.collide_with_bodies = true;
    } else {
        motion_params.collide_with_areas = p_collide_type == COLLIDE_WITH_AREAS;
        motion_params.collide_with_bodies = p_collide_type == COLLIDE_WITH_BODIES;
    }
    // Exclude certain RIDS
    if (!p_exclude.is_empty()) {
        for (int i = 0; i < p_exclude.size(); i++) {
            motion_params.exclude.insert(p_exclude[i]);
        }
    }

    real_t closest_safe = 1.0f, closest_unsafe = 1.0f;
    // Call the rest info method on the PhysicsDirectSpaceState3D singleton
    const bool res = world_3d->get_direct_space_state()->cast_motion(motion_params, closest_safe, closest_unsafe);
    if (!res) {
        return {};
    }
    Vector<real_t> ret;
    ret.resize(2);
    ret.write[0] = closest_safe;
    ret.write[1] = closest_unsafe;
    return ret;
}


Vector<real_t> YPhysics::cast_motion_2D(const Ref<Shape2D> &p_shape, const Transform2D &p_world_transform, real_t p_margin, const Vector2 &p_motion, CollideType p_collide_type, uint32_t p_collision_mask, const TypedArray<RID> &p_exclude) {
    // Ensure the shape is valid

    ERR_FAIL_COND_V(!p_shape.is_valid(), Vector<real_t>());

    auto world_2d = SceneTree::get_singleton()->get_root()->get_world_2d();
    PhysicsDirectSpaceState2D::ShapeParameters motion_params;

    motion_params.collision_mask = p_collision_mask;
    motion_params.shape_rid = p_shape->get_rid();
    motion_params.transform = p_world_transform;
    motion_params.margin = p_margin;
    motion_params.motion = p_motion;

    // Determine what we are colliding with
    if (p_collide_type == COLLIDE_WITH_BOTH) {
        motion_params.collide_with_areas = true;
        motion_params.collide_with_bodies = true;
    } else {
        motion_params.collide_with_areas = p_collide_type == COLLIDE_WITH_AREAS;
        motion_params.collide_with_bodies = p_collide_type == COLLIDE_WITH_BODIES;
    }
    // Exclude certain RIDS
    if (!p_exclude.is_empty()) {
        for (int i = 0; i < p_exclude.size(); i++) {
            motion_params.exclude.insert(p_exclude[i]);
        }
    }

    real_t closest_safe = 1.0f, closest_unsafe = 1.0f;
    // Call the rest info method on the PhysicsDirectSpaceState3D singleton
    const bool res = world_2d->get_direct_space_state()->cast_motion(motion_params, closest_safe, closest_unsafe);
    if (!res) {
        return {};
    }
    Vector<real_t> ret;
    ret.resize(2);
    ret.write[0] = closest_safe;
    ret.write[1] = closest_unsafe;
    return ret;
}

YPhysics *YPhysics::get_singleton() {
    return *singleton;
}

YPhysics::YPhysics() {
}

YPhysics::~YPhysics() {
    if(singleton.is_valid() && singleton == this) {
        singleton.unref();
    }
}
