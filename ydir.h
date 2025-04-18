#ifndef YDIR_H
#define YDIR_H

#include "scene/2d/node_2d.h"
#include "scene/3d/node_3d.h"
#include "core/object/ref_counted.h"
#include "core/math/random_number_generator.h"

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
    
    static void set_forward(Node3D* node, const Vector3& direction);
    static void set_right(Node3D* node, const Vector3& direction);
    static void set_up(Node3D* node, const Vector3& direction);

    // 3D Methods - Global Space
    static Vector3 get_global_forward(Node3D* node);
    static Vector3 get_global_right(Node3D* node);
    static Vector3 get_global_up(Node3D* node);
    
    static void set_global_forward(Node3D* node, const Vector3& direction);
    static void set_global_right(Node3D* node, const Vector3& direction);
    static void set_global_up(Node3D* node, const Vector3& direction);

    // LookAt Methods
    static void look_at(Node3D* node, const Vector3& target, const Vector3& up = Vector3(0, 1, 0));
    static void look_at_global(Node3D* node, const Vector3& target, const Vector3& up = Vector3(0, 1, 0));

    // Forward and Up Methods
    static void set_forward_and_up(Node3D* node, const Vector3& forward, const Vector3& up = Vector3(0, 1, 0));
    static void set_global_forward_and_up(Node3D* node, const Vector3& forward, const Vector3& up = Vector3(0, 1, 0));

    // 2D Methods - Local Space
    static Vector2 get_forward_2d(Node2D* node);
    static Vector2 get_right_2d(Node2D* node);
    
    static void set_forward_2d(Node2D* node, const Vector2& direction);
    static void set_right_2d(Node2D* node, const Vector2& direction);

    // 2D Methods - Global Space
    static Vector2 get_global_forward_2d(Node2D* node);
    static Vector2 get_global_right_2d(Node2D* node);
    
    static void set_global_forward_2d(Node2D* node, const Vector2& direction);
    static void set_global_right_2d(Node2D* node, const Vector2& direction);

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

    YDir();
    ~YDir();
};

#endif // YDIR_H 