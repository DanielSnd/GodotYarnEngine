#ifndef YDIR_H
#define YDIR_H

#include "scene/2d/node_2d.h"
#include "scene/3d/node_3d.h"
#include "core/object/ref_counted.h"
#include "core/math/random_number_generator.h"
#include "scene/3d/mesh_instance_3d.h"

class YDir : public RefCounted {
    GDCLASS(YDir, RefCounted);

protected:
    static void _bind_methods();

public:
    Ref<RandomNumberGenerator> rng;
    static YDir* singleton;
    static bool is_safe_for_cross(const Vector3& a, const Vector3& b);
    static Vector3 safe_cross(const Vector3& a, const Vector3& b, const Vector3& fallback);

    // 3D Methods - Local Space
    static Vector3 get_forward(Node3D* node);
    static Vector3 get_right(Node3D* node);
    static Vector3 get_up(Node3D* node);
    
    static void set_forward(Node3D* node, const Vector3& direction, float t = 1.0);
    static void set_right(Node3D* node, const Vector3& direction, float t = 1.0);
    static void set_up(Node3D* node, const Vector3& direction, float t = 1.0);

    // 3D Position Methods - Local Space
    static Vector3 get_pos_above(Node3D* node, float amount);
    static Vector3 get_pos_forward(Node3D* node, float amount);
    static Vector3 get_pos_right(Node3D* node, float amount);

    // 3D Methods - Global Space
    static Vector3 get_global_forward(Node3D* node);
    static Vector3 get_global_right(Node3D* node);
    static Vector3 get_global_up(Node3D* node);
    
    static void set_global_forward(Node3D* node, const Vector3& direction, float t = 1.0);
    static void set_global_right(Node3D* node, const Vector3& direction, float t = 1.0);
    static void set_global_up(Node3D* node, const Vector3& direction, float t = 1.0);

    // 3D Position Methods - Global Space
    static Vector3 get_global_pos_above(Node3D* node, float amount);
    static Vector3 get_global_pos_forward(Node3D* node, float amount);
    static Vector3 get_global_pos_right(Node3D* node, float amount);

    // 3D Transform Methods
    static Vector3 transform_point(Node3D* node, const Vector3& point);
    static Vector3 transform_dir(Node3D* node, const Vector3& dir);
    static Vector3 inverse_transform_point(Node3D* node, const Vector3& point);
    static Vector3 inverse_transform_dir(Node3D* node, const Vector3& dir);

    // LookAt Methods
    static void look_at(Node3D* node, const Vector3& target, const Vector3& up = Vector3(0, 1, 0), float t = 1.0);
    static void look_at_global(Node3D* node, const Vector3& target, const Vector3& up = Vector3(0, 1, 0), float t = 1.0);

    // Forward and Up Methods
    static void set_forward_and_up(Node3D* node, const Vector3& forward, const Vector3& up = Vector3(0, 1, 0), float t = 1.0);
    static void set_global_forward_and_up(Node3D* node, const Vector3& forward, const Vector3& up = Vector3(0, 1, 0), float t = 1.0);

    // 2D Methods - Local Space
    static Vector2 get_forward_2d(Node2D* node);
    static Vector2 get_right_2d(Node2D* node);
    
    static void set_forward_2d(Node2D* node, const Vector2& direction, float t = 1.0);
    static void set_right_2d(Node2D* node, const Vector2& direction, float t = 1.0);

    // 2D Methods - Global Space
    static Vector2 get_global_forward_2d(Node2D* node);
    static Vector2 get_global_right_2d(Node2D* node);
    
    static void set_global_forward_2d(Node2D* node, const Vector2& direction, float t = 1.0);
    static void set_global_right_2d(Node2D* node, const Vector2& direction, float t = 1.0);
    
    // 2D Transform Methods
    static Vector2 transform_point_2d(Node2D* node, const Vector2& point);
    static Vector2 transform_dir_2d(Node2D* node, const Vector2& dir);
    static Vector2 inverse_transform_point_2d(Node2D* node, const Vector2& point);
    static Vector2 inverse_transform_dir_2d(Node2D* node, const Vector2& dir);

    // Random Direction Methods
    static Vector2 random_direction_2d();
    static Vector3 random_direction();
    
    // Random Direction from Direction Methods
    static Vector2 random_from_dir_2d(const Vector2& direction, float angle_radians);
    static Vector3 random_from_dir(const Vector3& direction, float angle_radians);
    static Vector3 random_from_dir_flat(const Vector3& direction, float angle_radians);

    // Evenly Spaced Directions Methods
    static TypedArray<Vector2> get_even_directions_2d(const Vector2& forward, int amount, float min_angle, float max_angle);
    static TypedArray<Vector3> get_even_directions(const Vector3& forward, int amount, float min_angle, float max_angle);
    static TypedArray<Vector3> get_even_directions_flat(const Vector3& forward, int amount, float min_angle, float max_angle);

    // Evenly Spaced Positions Methods
    static TypedArray<Vector2> get_even_positions_2d(const Vector2& pos1, const Vector2& pos2, int amount);
    static TypedArray<Vector3> get_even_positions(const Vector3& pos1, const Vector3& pos2, int amount);

    static Vector3 get_random_point_on_top_of_mesh(MeshInstance3D *p_meshInstance);

    YDir();
    ~YDir();
};

#endif // YDIR_H 