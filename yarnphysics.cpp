//
// Created by Daniel on 2024-03-03.
//

#include "yarnphysics.h"

#include "scene/main/scene_tree.h"
#include "scene/main/window.h"
#include "scene/resources/world_2d.h"
#include "scene/resources/3d/world_3d.h"
#include "servers/physics_server_2d.h"

Ref<YPhysics> YPhysics::singleton;


void YPhysics::_bind_methods() {
    ClassDB::bind_method(D_METHOD("raycast2d_to", "ray_origin", "ray_end", "collide_type", "collision_mask"), &YPhysics::raycast2d_to,DEFVAL(COLLIDE_WITH_BODIES),DEFVAL(UINT32_MAX));
    ClassDB::bind_method(D_METHOD("raycast3d_to", "ray_origin", "ray_end", "collide_type", "collision_mask"), &YPhysics::raycast3d_to,DEFVAL(COLLIDE_WITH_BODIES),DEFVAL(UINT32_MAX));
    ClassDB::bind_method(D_METHOD("free_line_check", "ray_origin", "ray_end", "collide_type", "collision_mask"), &YPhysics::free_line_check,DEFVAL(COLLIDE_WITH_BODIES),DEFVAL(UINT32_MAX));
    ClassDB::bind_method(D_METHOD("raycast2d", "ray_origin", "ray_direction", "ray_distance", "collide_type", "collision_mask"), &YPhysics::raycast2d,DEFVAL(COLLIDE_WITH_BODIES),DEFVAL(UINT32_MAX));
    ClassDB::bind_method(D_METHOD("raycast3d", "ray_origin", "ray_direction", "ray_distance", "collide_type", "collision_mask"), &YPhysics::raycast3d,DEFVAL(COLLIDE_WITH_BODIES),DEFVAL(UINT32_MAX));
    ClassDB::bind_method(D_METHOD("intersect_sphere", "world_position", "radius", "collide_type","max_results", "collision_mask"), &YPhysics::_intersect_sphere,DEFVAL(COLLIDE_WITH_BODIES),DEFVAL(32),DEFVAL(UINT32_MAX));
    ClassDB::bind_method(D_METHOD("check_collision_sphere", "world_position", "radius", "collision_mask"), &YPhysics::check_collision_sphere,DEFVAL(1.0),DEFVAL(UINT32_MAX));
    ClassDB::bind_method(D_METHOD("free_sphere_check", "world_position", "radius", "collision_mask"), &YPhysics::check_collision_sphere,DEFVAL(1.0),DEFVAL(UINT32_MAX));

//    TypedArray<Dictionary> YPhysics::_intersect_sphere(const Vector3 p_world_position, real_t radius, CollideType _collide_type, int p_max_results, uint32_t collision_mask) {
    BIND_ENUM_CONSTANT(COLLIDE_WITH_BODIES);
    BIND_ENUM_CONSTANT(COLLIDE_WITH_AREAS);
    BIND_ENUM_CONSTANT(COLLIDE_WITH_BOTH);
}

Dictionary YPhysics::raycast2d(Vector2 ray_origin, Vector2 ray_dir, float ray_dist, CollideType _collide_type, uint32_t layer_mask) {
    return raycast2d_to(ray_origin,ray_origin + ray_dir * ray_dist, _collide_type, layer_mask);
}

Dictionary YPhysics::raycast3d(Vector3 ray_origin, Vector3 ray_dir, float ray_dist, CollideType _collide_type, uint32_t layer_mask) {
    return raycast3d_to(ray_origin,ray_origin + ray_dir * ray_dist, _collide_type, layer_mask);
}

Dictionary YPhysics::raycast2d_to(Vector2 ray_origin, Vector2 ray_end, CollideType _collide_type, uint32_t layer_mask) {
    Dictionary return_dictionary;
    auto world_2d = SceneTree::get_singleton()->get_root()->get_world_2d();
    if (world_2d.is_valid()) {
        auto space_state = world_2d->get_direct_space_state();
        if (space_state != nullptr) {
            PhysicsDirectSpaceState2D::RayResult result;
            PhysicsDirectSpaceState2D::RayParameters ray_params;
            ray_params.from = ray_origin;
            ray_params.to = ray_end;
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

bool YPhysics::free_line_check(Vector3 ray_origin, Vector3 ray_end, CollideType _collide_type, uint32_t layer_mask) {
    auto world_3d = SceneTree::get_singleton()->get_root()->get_world_3d();
    if (world_3d.is_valid()) {
        auto space_state = world_3d->get_direct_space_state();
        if (space_state != nullptr) {
            PhysicsDirectSpaceState3D::RayResult result;
            PhysicsDirectSpaceState3D::RayParameters ray_params;
            ray_params.from = ray_origin;
            ray_params.to = ray_end;
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
Dictionary YPhysics::raycast3d_to(Vector3 ray_origin, Vector3 ray_end, CollideType _collide_type, uint32_t layer_mask) {
    Dictionary return_dictionary;
    auto world_3d = SceneTree::get_singleton()->get_root()->get_world_3d();
    if (world_3d.is_valid()) {
        auto space_state = world_3d->get_direct_space_state();
        if (space_state != nullptr) {
            PhysicsDirectSpaceState3D::RayResult result;
            PhysicsDirectSpaceState3D::RayParameters ray_params;
            ray_params.from = ray_origin;
            ray_params.to = ray_end;
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

TypedArray<Dictionary> YPhysics::_intersect_sphere(const Vector3 p_world_position, real_t radius, CollideType _collide_type, int p_max_results, uint32_t collision_mask) {
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
    shape_params.collision_mask = collision_mask;
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


Object* YPhysics::check_collision_sphere(const Vector3 p_world_position, real_t radius, uint32_t collision_mask) {
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
    PhysicsServer3D::get_singleton()->shape_set_data(sphere_rid, radius);
    shape_params.transform = Transform3D{Basis{},p_world_position};
    int rc = world_3d->get_direct_space_state()->intersect_shape(shape_params, sr.ptrw(), 1);
    if (rc > 0) {
        return sr[0].collider;
    }
    return nullptr;
}

bool YPhysics::free_sphere_check(const Vector3 p_world_position, real_t radius, uint32_t collision_mask) {
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
    PhysicsServer3D::get_singleton()->shape_set_data(sphere_rid, radius);
    shape_params.transform = Transform3D{Basis{},p_world_position};
    int rc = world_3d->get_direct_space_state()->intersect_shape(shape_params, sr.ptrw(), 1);
    if (rc > 0) {
        return true;
    }
    return false;
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
