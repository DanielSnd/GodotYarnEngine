#include "ydir.h"
#include "core/math/random_number_generator.h"

YDir* YDir::singleton = nullptr;

// Helper function to check if two vectors are safe to use with cross product
bool YDir::is_safe_for_cross(const Vector3& a, const Vector3& b) {
    // Check if either vector is too small
    if (a.length_squared() < CMP_EPSILON || b.length_squared() < CMP_EPSILON) {
        return false;
    }
    
    // Check if vectors are parallel (angle between them is close to 0 or 180 degrees)
    float dot = a.normalized().dot(b.normalized());
    return Math::abs(dot) < 1.0f - CMP_EPSILON;
}

// Helper function to get a safe cross product with fallback
Vector3 YDir::safe_cross(const Vector3& a, const Vector3& b, const Vector3& fallback) {
    if (!is_safe_for_cross(a, b)) {
        return fallback;
    }
    return a.cross(b).normalized();
}

void YDir::_bind_methods() {
    // 3D Methods
    ClassDB::bind_static_method("YDir", D_METHOD("get_forward", "node"), &YDir::get_forward);
    ClassDB::bind_static_method("YDir", D_METHOD("get_right", "node"), &YDir::get_right);
    ClassDB::bind_static_method("YDir", D_METHOD("get_up", "node"), &YDir::get_up);
    
    ClassDB::bind_static_method("YDir", D_METHOD("set_forward", "node", "direction"), &YDir::set_forward);
    ClassDB::bind_static_method("YDir", D_METHOD("set_right", "node", "direction"), &YDir::set_right);
    ClassDB::bind_static_method("YDir", D_METHOD("set_up", "node", "direction"), &YDir::set_up);

    // 3D Methods - Global Space
    ClassDB::bind_static_method("YDir", D_METHOD("get_global_forward", "node"), &YDir::get_global_forward);
    ClassDB::bind_static_method("YDir", D_METHOD("get_global_right", "node"), &YDir::get_global_right);
    ClassDB::bind_static_method("YDir", D_METHOD("get_global_up", "node"), &YDir::get_global_up);
    
    ClassDB::bind_static_method("YDir", D_METHOD("set_global_forward", "node", "direction"), &YDir::set_global_forward);
    ClassDB::bind_static_method("YDir", D_METHOD("set_global_right", "node", "direction"), &YDir::set_global_right);
    ClassDB::bind_static_method("YDir", D_METHOD("set_global_up", "node", "direction"), &YDir::set_global_up);

    // 2D Methods
    ClassDB::bind_static_method("YDir", D_METHOD("get_forward_2d", "node"), &YDir::get_forward_2d);
    ClassDB::bind_static_method("YDir", D_METHOD("get_right_2d", "node"), &YDir::get_right_2d);
    
    ClassDB::bind_static_method("YDir", D_METHOD("set_forward_2d", "node", "direction"), &YDir::set_forward_2d);
    ClassDB::bind_static_method("YDir", D_METHOD("set_right_2d", "node", "direction"), &YDir::set_right_2d);

    // 2D Methods - Global Space
    ClassDB::bind_static_method("YDir", D_METHOD("get_global_forward_2d", "node"), &YDir::get_global_forward_2d);
    ClassDB::bind_static_method("YDir", D_METHOD("get_global_right_2d", "node"), &YDir::get_global_right_2d);
    
    ClassDB::bind_static_method("YDir", D_METHOD("set_global_forward_2d", "node", "direction"), &YDir::set_global_forward_2d);
    ClassDB::bind_static_method("YDir", D_METHOD("set_global_right_2d", "node", "direction"), &YDir::set_global_right_2d);

    // Random Direction Methods
    ClassDB::bind_static_method("YDir", D_METHOD("random_direction_2d"), &YDir::random_direction_2d);
    ClassDB::bind_static_method("YDir", D_METHOD("random_direction"), &YDir::random_direction);
    
    // Random Direction from Direction Methods
    ClassDB::bind_static_method("YDir", D_METHOD("random_from_dir_2d", "direction", "angle_radians"), &YDir::random_from_dir_2d);
    ClassDB::bind_static_method("YDir", D_METHOD("random_from_dir", "direction", "angle_radians"), &YDir::random_from_dir);
    ClassDB::bind_static_method("YDir", D_METHOD("random_from_dir_flat", "direction", "angle_radians"), &YDir::random_from_dir_flat);

    // LookAt Methods
    ClassDB::bind_static_method("YDir", D_METHOD("look_at", "node", "target", "up"), &YDir::look_at, DEFVAL(Vector3(0, 1, 0)));
    ClassDB::bind_static_method("YDir", D_METHOD("look_at_global", "node", "target", "up"), &YDir::look_at_global, DEFVAL(Vector3(0, 1, 0)));

    // Forward and Up Methods
    ClassDB::bind_static_method("YDir", D_METHOD("set_forward_and_up", "node", "forward", "up"), &YDir::set_forward_and_up, DEFVAL(Vector3(0, 1, 0)));
    ClassDB::bind_static_method("YDir", D_METHOD("set_global_forward_and_up", "node", "forward", "up"), &YDir::set_global_forward_and_up, DEFVAL(Vector3(0, 1, 0)));

    // Evenly Spaced Directions Methods
    ClassDB::bind_static_method("YDir", D_METHOD("get_even_directions_2d", "forward", "amount", "min_angle", "max_angle"), &YDir::get_even_directions_2d);
    ClassDB::bind_static_method("YDir", D_METHOD("get_even_directions", "forward", "amount", "min_angle", "max_angle"), &YDir::get_even_directions);
    ClassDB::bind_static_method("YDir", D_METHOD("get_even_directions_flat", "forward", "amount", "min_angle", "max_angle"), &YDir::get_even_directions_flat);
}

// 3D Methods
Vector3 YDir::get_forward(Node3D* node) {
    ERR_FAIL_COND_V_MSG(!node, Vector3(), "Node is null");
    return -node->get_transform().basis.get_column(2);
}

Vector3 YDir::get_right(Node3D* node) {
    ERR_FAIL_COND_V_MSG(!node, Vector3(), "Node is null");
    return node->get_transform().basis.get_column(0);
}

Vector3 YDir::get_up(Node3D* node) {
    ERR_FAIL_COND_V_MSG(!node, Vector3(), "Node is null");
    return node->get_transform().basis.get_column(1);
}

void YDir::set_forward(Node3D* node, const Vector3& direction) {
    ERR_FAIL_COND_MSG(!node, "Node is null");
    
    // Normalize the forward direction
    Vector3 forward = direction.normalized();
    
    // Get the current scale
    Vector3 scale = node->get_scale();
    
    // Find a suitable up vector
    Vector3 up = Vector3(0, -1, 0);
    if (Math::abs(forward.dot(up)) > 0.99) {
        up = Vector3(1, 0, 0);
    }
    
    // Calculate right vector
    Vector3 right = up.cross(forward).normalized();
    
    // Recalculate up vector to ensure orthogonality
    up = forward.cross(right).normalized();
    
    // Create the basis with negative Z as forward (Godot convention)
    Basis basis = Basis(right, up, -forward);
    // Set the basis
    node->set_basis(basis);
    
    // Restore the scale that might have been changed by set_basis
    node->set_scale(scale);
}

void YDir::set_right(Node3D* node, const Vector3& direction) {
    ERR_FAIL_COND_MSG(!node, "Node is null");
    
    // Get the current scale and position
    Vector3 scale = node->get_scale();
    Vector3 position = node->get_position();
    
    // Normalize the right direction
    Vector3 right = direction.normalized();
    
    // Set up the desired coordinate system
    Vector3 world_up = Vector3(0, 1, 0);
    
    // Handle case where right is parallel to world_up
    if (Math::abs(right.dot(world_up)) > 0.99) {
        // Use a forward direction
        Vector3 world_forward = Vector3(0, 0, -1);
        // Calculate a perpendicular right vector
        right = world_up.cross(world_forward).normalized();
        // Ensure it points in the same general direction as input
        if (right.dot(direction) < 0) {
            right = -right;
        }
    }
    
    // Calculate forward to be perpendicular to right and world_up
    Vector3 forward = right.cross(world_up).normalized();
    
    // Now calculate a properly perpendicular up vector
    Vector3 up = forward.cross(right).normalized();
    
    // Create a proper orthonormal basis and use it for rotation
    Basis basis = Basis(right, up, -forward);
    
    // Ensure the basis is orthonormal to avoid scale issues
    basis.orthonormalize();
    
    // Create transform
    Transform3D transform;
    transform.basis = basis;
    transform.origin = position;
    
    // Apply transform
    node->set_transform(transform);
    
    // Restore scale
    node->set_scale(scale);
}

void YDir::set_up(Node3D* node, const Vector3& direction) {
    ERR_FAIL_COND_MSG(!node, "Node is null");
    
    // Get the current scale and position
    Vector3 scale = node->get_scale();
    Vector3 position = node->get_position();
    
    // Normalize the up direction
    Vector3 up = direction.normalized();
    
    // Set up the desired coordinate system
    Vector3 world_forward = Vector3(0, 0, -1);
    
    // Handle case where up is parallel to world_forward
    if (Math::abs(up.dot(world_forward)) > 0.99) {
        // Use a right direction
        Vector3 world_right = Vector3(1, 0, 0);
        // Calculate a perpendicular up vector
        up = world_right.cross(world_forward).normalized();
        // Ensure it points in the same general direction as input
        if (up.dot(direction) < 0) {
            up = -up;
        }
    }
    
    // Calculate right to be perpendicular to up and world_forward
    Vector3 right = up.cross(world_forward).normalized();
    
    // Now calculate a properly perpendicular forward vector
    Vector3 forward = right.cross(up).normalized();
    
    // Create a proper orthonormal basis and use it for rotation
    Basis basis = Basis(right, up, -forward);
    
    // Ensure the basis is orthonormal to avoid scale issues
    basis.orthonormalize();
    
    // Create transform
    Transform3D transform;
    transform.basis = basis;
    transform.origin = position;
    
    // Apply transform
    node->set_transform(transform);
    
    // Restore scale
    node->set_scale(scale);
}

// 3D Global Space Methods
Vector3 YDir::get_global_forward(Node3D* node) {
    ERR_FAIL_COND_V_MSG(!node, Vector3(), "Node is null");
    return -node->get_global_transform().basis.get_column(2);
}

Vector3 YDir::get_global_right(Node3D* node) {
    ERR_FAIL_COND_V_MSG(!node, Vector3(), "Node is null");
    return node->get_global_transform().basis.get_column(0);
}

Vector3 YDir::get_global_up(Node3D* node) {
    ERR_FAIL_COND_V_MSG(!node, Vector3(), "Node is null");
    return node->get_global_transform().basis.get_column(1);
}

void YDir::set_global_forward(Node3D* node, const Vector3& direction) {
    ERR_FAIL_COND_MSG(!node, "Node is null");
    
    // Save the current state
    Vector3 position = node->get_global_position();
    Vector3 scale = node->get_scale();
    
    // Normalize the forward direction
    Vector3 forward = direction.normalized();
    
    // Find a suitable up vector
    Vector3 up = Vector3(0, -1, 0);
    if (Math::abs(forward.dot(up)) > 0.99) {
        up = Vector3(1, 0, 0);
    }
    
    // Calculate right vector
    Vector3 right = up.cross(forward).normalized();
    
    // Recalculate up vector to ensure orthogonality
    up = forward.cross(right).normalized();
    
    // Create the global transform
    Transform3D transform = Transform3D(Basis(right, up, -forward), position);
    
    // Set the global transform
    node->set_global_transform(transform);
    
    // Restore the scale
    node->set_scale(scale);
}

void YDir::set_global_right(Node3D* node, const Vector3& direction) {
    ERR_FAIL_COND_MSG(!node, "Node is null");
    
    // Get the current scale and global position
    Vector3 scale = node->get_scale();
    Vector3 position = node->get_global_position();
    
    // Normalize the right direction
    Vector3 right = direction.normalized();
    
    // Set up the desired coordinate system
    Vector3 world_up = Vector3(0, 1, 0);
    
    // Handle case where right is parallel to world_up
    if (Math::abs(right.dot(world_up)) > 0.99) {
        // Use a forward direction
        Vector3 world_forward = Vector3(0, 0, -1);
        // Calculate a perpendicular right vector
        right = world_up.cross(world_forward).normalized();
        // Ensure it points in the same general direction as input
        if (right.dot(direction) < 0) {
            right = -right;
        }
    }
    
    // Calculate forward to be perpendicular to right and world_up
    Vector3 forward = right.cross(world_up).normalized();
    
    // Now calculate a properly perpendicular up vector
    Vector3 up = forward.cross(right).normalized();
    
    // Create a proper orthonormal basis and use it for rotation
    Basis basis = Basis(right, up, -forward);
    
    // Ensure the basis is orthonormal to avoid scale issues
    basis.orthonormalize();
    
    // Create global transform
    Transform3D transform;
    transform.basis = basis;
    transform.origin = position;
    
    // Apply global transform
    node->set_global_transform(transform);
    
    // Restore scale
    node->set_scale(scale);
}

void YDir::set_global_up(Node3D* node, const Vector3& direction) {
    ERR_FAIL_COND_MSG(!node, "Node is null");
    
    // Get the current scale and global position
    Vector3 scale = node->get_scale();
    Vector3 position = node->get_global_position();
    
    // Normalize the up direction
    Vector3 up = direction.normalized();
    
    // Set up the desired coordinate system
    Vector3 world_forward = Vector3(0, 0, -1);
    
    // Handle case where up is parallel to world_forward
    if (Math::abs(up.dot(world_forward)) > 0.99) {
        // Use a right direction
        Vector3 world_right = Vector3(1, 0, 0);
        // Calculate a perpendicular up vector
        up = world_right.cross(world_forward).normalized();
        // Ensure it points in the same general direction as input
        if (up.dot(direction) < 0) {
            up = -up;
        }
    }
    
    // Calculate right to be perpendicular to up and world_forward
    Vector3 right = up.cross(world_forward).normalized();
    
    // Now calculate a properly perpendicular forward vector
    Vector3 forward = right.cross(up).normalized();
    
    // Create a proper orthonormal basis and use it for rotation
    Basis basis = Basis(right, up, -forward);
    
    // Ensure the basis is orthonormal to avoid scale issues
    basis.orthonormalize();
    
    // Create global transform
    Transform3D transform;
    transform.basis = basis;
    transform.origin = position;
    
    // Apply global transform
    node->set_global_transform(transform);
    
    // Restore scale
    node->set_scale(scale);
}

// 2D Methods
Vector2 YDir::get_forward_2d(Node2D* node) {
    ERR_FAIL_COND_V_MSG(!node, Vector2(), "Node is null");
    float angle = node->get_rotation();
    return Vector2(-Math::sin(angle), Math::cos(angle));
}

Vector2 YDir::get_right_2d(Node2D* node) {
    ERR_FAIL_COND_V_MSG(!node, Vector2(), "Node is null");
    float angle = node->get_rotation();
    return Vector2(Math::cos(angle), Math::sin(angle));
}

void YDir::set_forward_2d(Node2D* node, const Vector2& direction) {
    ERR_FAIL_COND_MSG(!node, "Node is null");
    ERR_FAIL_COND_MSG(direction.length_squared() < CMP_EPSILON, "Direction vector is too small");
    
    Vector2 forward = direction.normalized();
    float angle = Math::atan2(-forward.x, forward.y);
    node->set_rotation(angle);
}

void YDir::set_right_2d(Node2D* node, const Vector2& direction) {
    ERR_FAIL_COND_MSG(!node, "Node is null");
    ERR_FAIL_COND_MSG(direction.length_squared() < CMP_EPSILON, "Direction vector is too small");
    
    Vector2 right = direction.normalized();
    float angle = Math::atan2(right.y, right.x);
    node->set_rotation(angle);
}

Vector2 YDir::random_direction_2d() {
   if (singleton == nullptr) {
    return Vector2();
   }
    float angle = singleton->rng->randf_range(0, Math_TAU);
    return Vector2(Math::cos(angle), Math::sin(angle));
}

Vector3 YDir::random_direction() {
   if (singleton == nullptr) {
    return Vector3();
   }
   
    // Generate random angles for spherical coordinates
    float theta = singleton->rng->randf_range(0, Math_TAU); // Azimuthal angle
    float phi = Math::acos(singleton->rng->randf_range(-1, 1)); // Polar angle
    
    // Convert to Cartesian coordinates
    float sin_phi = Math::sin(phi);
    return Vector3(
        sin_phi * Math::cos(theta),
        sin_phi * Math::sin(theta),
        Math::cos(phi)
    );
}

Vector2 YDir::random_from_dir_2d(const Vector2& direction, float angle_radians) {
    if (singleton == nullptr) {
        return Vector2();
    }
    
    // Generate a random angle within the specified range
    float random_angle = singleton->rng->randf_range(-angle_radians, angle_radians);
    
    // Get the base angle of the direction
    float base_angle = Math::atan2(direction.y, direction.x);
    
    // Calculate the new angle
    float new_angle = base_angle + random_angle;
    
    // Convert back to vector
    return Vector2(Math::cos(new_angle), Math::sin(new_angle));
}

Vector3 YDir::random_from_dir(const Vector3& direction, float angle_radians) {
    if (singleton == nullptr) {
        return Vector3();
    }
    
    // Normalize the input direction
    Vector3 dir = direction.normalized();
    
    // Generate a random direction perpendicular to the input direction
    Vector3 perp;
    if (Math::abs(dir.x) < 0.9) {
        perp = Vector3(1, 0, 0);
    } else {
        perp = Vector3(0, 1, 0);
    }
    perp = perp.cross(dir).normalized();
    
    // Generate a random angle within the specified range
    float random_angle = singleton->rng->randf_range(0, angle_radians);
    
    // Generate a random rotation around the original direction
    float rotation_angle = singleton->rng->randf_range(0, Math_TAU);
    
    // Create a rotation matrix
    Basis rotation = Basis(dir, random_angle) * Basis(dir, rotation_angle);
    
    // Apply the rotation to the perpendicular vector
    return rotation.xform(perp);
}

Vector3 YDir::random_from_dir_flat(const Vector3& direction, float angle_radians) {
    if (singleton == nullptr) {
        return Vector3();
    }
    
    // Normalize the input direction
    Vector3 dir = direction.normalized();
    
    // Project the direction onto the XZ plane
    Vector2 dir_2d = Vector2(dir.x, dir.z).normalized();
    
    // Generate a random angle within the specified range
    float random_angle = singleton->rng->randf_range(-angle_radians, angle_radians);
    
    // Get the base angle of the 2D direction
    float base_angle = Math::atan2(dir_2d.y, dir_2d.x);
    
    // Calculate the new angle
    float new_angle = base_angle + random_angle;
    
    // Convert back to 3D vector (keeping Y component 0)
    return Vector3(Math::cos(new_angle), 0, Math::sin(new_angle));
}

// 2D Global Space Methods
Vector2 YDir::get_global_forward_2d(Node2D* node) {
    ERR_FAIL_COND_V_MSG(!node, Vector2(), "Node is null");
    float angle = node->get_global_rotation();
    return Vector2(-Math::sin(angle), Math::cos(angle));
}

Vector2 YDir::get_global_right_2d(Node2D* node) {
    ERR_FAIL_COND_V_MSG(!node, Vector2(), "Node is null");
    float angle = node->get_global_rotation();
    return Vector2(Math::cos(angle), Math::sin(angle));
}

void YDir::set_global_forward_2d(Node2D* node, const Vector2& direction) {
    ERR_FAIL_COND_MSG(!node, "Node is null");
    Vector2 forward = direction.normalized();
    
    // Get the parent's global transform if it exists
    Transform2D parent_transform;
    if (node->get_parent()) {
        Node2D* parent = Object::cast_to<Node2D>(node->get_parent());
        if (parent) {
            parent_transform = parent->get_global_transform();
        }
    }
    
    // Calculate the global rotation
    float global_rotation = Math::atan2(-forward.x, forward.y);
    
    // Set the local rotation
    if (node->get_parent()) {
        Node2D* parent = Object::cast_to<Node2D>(node->get_parent());
        if (parent) {
            node->set_rotation(global_rotation - parent->get_global_rotation());
            return;
        }
    }
    node->set_rotation(global_rotation);
}

void YDir::set_global_right_2d(Node2D* node, const Vector2& direction) {
    ERR_FAIL_COND_MSG(!node, "Node is null");
    Vector2 right = direction.normalized();
    
    // Get the parent's global transform if it exists
    Transform2D parent_transform;
    if (node->get_parent()) {
        Node2D* parent = Object::cast_to<Node2D>(node->get_parent());
        if (parent) {
            parent_transform = parent->get_global_transform();
        }
    }
    
    // Calculate the global rotation
    float global_rotation = Math::atan2(right.y, right.x);
    
    // Set the local rotation
    if (node->get_parent()) {
        Node2D* parent = Object::cast_to<Node2D>(node->get_parent());
        if (parent) {
            node->set_rotation(global_rotation - parent->get_global_rotation());
            return;
        }
    }
    node->set_rotation(global_rotation);
}

void YDir::look_at(Node3D* node, const Vector3& target, const Vector3& up_dir) {
    ERR_FAIL_COND_MSG(!node, "Node is null");
    
    // Get the current scale
    Vector3 scale = node->get_scale();
    
    // Calculate the forward direction (from node to target)
    Vector3 forward = (target - node->get_position()).normalized();
    
    // Normalize the up direction
    Vector3 up = -up_dir.normalized();
    
    // If forward and up are too close to parallel, adjust up
    if (Math::abs(forward.dot(up)) > 0.99) {
        // Try using right as up
        Vector3 right = Vector3(1, 0, 0);
        if (Math::abs(forward.dot(right)) > 0.99) {
            right = Vector3(0, 0, 1);
        }
        up = forward.cross(right).normalized();
    }
    
    // Calculate right vector
    Vector3 right = up.cross(forward).normalized();
    
    // Recalculate up vector to ensure orthogonality
    up = forward.cross(right).normalized();
    
    // Create the basis
    Basis basis = Basis(right, up, -forward);

    // Set the basis
    node->set_basis(basis);
    
    // Restore the scale
    node->set_scale(scale);
}

void YDir::look_at_global(Node3D* node, const Vector3& target, const Vector3& up_dir) {
    ERR_FAIL_COND_MSG(!node, "Node is null");
    
    // Save the current state
    Vector3 position = node->get_global_position();
    Vector3 scale = node->get_scale();
    
    // Calculate the forward direction (from node to target)
    Vector3 forward = (target - position).normalized();
    
    // Normalize the up direction
    Vector3 up = -up_dir.normalized();
    
    // If forward and up are too close to parallel, adjust up
    if (Math::abs(forward.dot(up)) > 0.99) {
        // Try using right as up
        Vector3 right = Vector3(1, 0, 0);
        if (Math::abs(forward.dot(right)) > 0.99) {
            right = Vector3(0, 0, 1);
        }
        up = forward.cross(right).normalized();
    }
    
    // Calculate right vector
    Vector3 right = up.cross(forward).normalized();
    
    // Recalculate up vector to ensure orthogonality
    up = forward.cross(right).normalized();
    
    // Create the global transform
    Transform3D transform;
    transform.basis.set_column(0, right);
    transform.basis.set_column(1, up);
    transform.basis.set_column(2, -forward); // In Godot, -Z is forward
    transform.origin = position;
    
    // Set the global transform
    node->set_global_transform(transform);
    
    // Restore the scale
    node->set_scale(scale);
}

void YDir::set_forward_and_up(Node3D* node, const Vector3& forward_dir, const Vector3& up_dir) {
    ERR_FAIL_COND_MSG(!node, "Node is null");
    
    // Get the current scale
    Vector3 scale = node->get_scale();
    
    // Normalize the input vectors
    Vector3 forward = forward_dir.normalized();
    Vector3 up = -up_dir.normalized();
    
    // If forward and up are too close to parallel, adjust up
    if (Math::abs(forward.dot(up)) > 0.99) {
        // Try using right as up
        Vector3 right = Vector3(1, 0, 0);
        if (Math::abs(forward.dot(right)) > 0.99) {
            right = Vector3(0, 0, 1);
        }
        up = forward.cross(right).normalized();
    }
    
    // Calculate right vector
    Vector3 right = up.cross(forward).normalized();
    
    // Recalculate up vector to ensure orthogonality
    up = forward.cross(right).normalized();
    
    // Create the basis
    Basis basis = Basis(right, up, -forward);

    // Set the basis
    node->set_basis(basis);
    
    // Restore the scale
    node->set_scale(scale);
}

void YDir::set_global_forward_and_up(Node3D* node, const Vector3& forward_dir, const Vector3& up_dir) {
    ERR_FAIL_COND_MSG(!node, "Node is null");
    
    // Save the current state
    Vector3 position = node->get_global_position();
    Vector3 scale = node->get_scale();
    
    // Normalize the input vectors
    Vector3 forward = forward_dir.normalized();
    Vector3 up = -up_dir.normalized();
    
    // If forward and up are too close to parallel, adjust up
    if (Math::abs(forward.dot(up)) > 0.99) {
        // Try using right as up
        Vector3 right = Vector3(1, 0, 0);
        if (Math::abs(forward.dot(right)) > 0.99) {
            right = Vector3(0, 0, 1);
        }
        up = forward.cross(right).normalized();
    }
    
    // Calculate right vector
    Vector3 right = up.cross(forward).normalized();
    
    // Recalculate up vector to ensure orthogonality
    up = forward.cross(right).normalized();
    
    // Create the global transform
    Transform3D transform = Transform3D(Basis(right, up, -forward), position);
    
    // Set the global transform
    node->set_global_transform(transform);
    
    // Restore the scale
    node->set_scale(scale);
}

TypedArray<Vector2> YDir::get_even_directions_2d(const Vector2& forward, int amount, float min_angle, float max_angle) {
    TypedArray<Vector2> directions;
    directions.resize(amount);
    
    if (amount <= 0) {
        return directions;
    }
    
    // Convert angles from degrees to radians
    float min_rad = Math::deg_to_rad(min_angle);
    float max_rad = Math::deg_to_rad(max_angle);
    
    // Normalize the forward direction
    Vector2 forward_norm = forward.normalized();
    
    // Calculate the base angle
    float base_angle = Math::atan2(forward_norm.y, forward_norm.x);
    
    if (amount == 1) {
        // Single direction case
        directions[0] = forward_norm;
        return directions;
    }
    
    bool is_odd = (amount % 2) == 1;
    int half_amount = amount / 2;
    
    if (is_odd) {
        // For odd numbers, put one in the middle and split the rest
        directions[half_amount] = forward_norm;
        
        // Calculate angle step for the sides
        float total_spread = max_rad - min_rad;
        float angle_step = total_spread / (amount - 1);
        
        // Generate left side (negative angles)
        for (int i = 0; i < half_amount; i++) {
            float angle = base_angle - (angle_step * (half_amount - i));
            directions[i] = Vector2(Math::cos(angle), Math::sin(angle));
        }
        
        // Generate right side (positive angles)
        for (int i = 0; i < half_amount; i++) {
            float angle = base_angle + (angle_step * (i + 1));
            directions[amount - 1 - i] = Vector2(Math::cos(angle), Math::sin(angle));
        }
    } else {
        // For even numbers, spread evenly
        float total_spread = max_rad - min_rad;
        float angle_step = total_spread / (amount - 1);
        for (int i = 0; i < amount; i++) {
            float angle = base_angle + min_rad + (angle_step * i);
            directions[i] = Vector2(Math::cos(angle), Math::sin(angle));
        }
    }
    
    return directions;
}

TypedArray<Vector3> YDir::get_even_directions(const Vector3& forward, int amount, float min_angle, float max_angle) {
    TypedArray<Vector3> directions;
    directions.resize(amount);
    
    if (amount <= 0) {
        return directions;
    }
    
    // Convert angles from degrees to radians
    float min_rad = Math::deg_to_rad(min_angle);
    float max_rad = Math::deg_to_rad(max_angle);
    
    // Normalize the forward direction
    Vector3 forward_norm = forward.normalized();
    
    // Find a perpendicular vector to use as reference
    Vector3 perp;
    if (Math::abs(forward_norm.x) < 0.9) {
        perp = Vector3(1, 0, 0);
    } else {
        perp = Vector3(0, 1, 0);
    }
    perp = perp.cross(forward_norm).normalized();
    
    if (amount == 1) {
        // Single direction case
        directions[0] = forward_norm;
        return directions;
    }
    
    bool is_odd = (amount % 2) == 1;
    int half_amount = amount / 2;
    
    if (is_odd) {
        // For odd numbers, put one in the middle and split the rest
        directions[half_amount] = forward_norm;
        
        // Calculate angle step for the sides
        float total_spread = max_rad - min_rad;
        float angle_step = total_spread / (amount - 1);
        
        // Generate left side (negative angles)
        for (int i = 0; i < half_amount; i++) {
            float angle = -(angle_step * (half_amount - i));
            Basis rotation = Basis(perp, angle);
            directions[i] = rotation.xform(forward_norm);
        }
        
        // Generate right side (positive angles)
        for (int i = 0; i < half_amount; i++) {
            float angle = angle_step * (i + 1);
            Basis rotation = Basis(perp, angle);
            directions[amount - 1 - i] = rotation.xform(forward_norm);
        }
    } else {
        // For even numbers, spread evenly
        float total_spread = max_rad - min_rad;
        float angle_step = total_spread / (amount - 1);
        for (int i = 0; i < amount; i++) {
            float angle = min_rad + (angle_step * i);
            Basis rotation = Basis(perp, angle);
            directions[i] = rotation.xform(forward_norm);
        }
    }
    
    return directions;
}

TypedArray<Vector3> YDir::get_even_directions_flat(const Vector3& forward, int amount, float min_angle, float max_angle) {
    TypedArray<Vector3> directions;
    directions.resize(amount);
    
    if (amount <= 0) {
        return directions;
    }
    
    // Convert angles from degrees to radians
    float min_rad = Math::deg_to_rad(min_angle);
    float max_rad = Math::deg_to_rad(max_angle);
    
    // Normalize the forward direction and project onto XZ plane
    Vector3 forward_norm = forward.normalized();
    Vector2 forward_2d = Vector2(forward_norm.x, forward_norm.z).normalized();
    
    // Calculate the base angle
    float base_angle = Math::atan2(forward_2d.y, forward_2d.x);
    
    if (amount == 1) {
        // Single direction case
        directions[0] = Vector3(forward_2d.x, 0, forward_2d.y);
        return directions;
    }
    
    bool is_odd = (amount % 2) == 1;
    int half_amount = amount / 2;
    
    if (is_odd) {
        // For odd numbers, put one in the middle and split the rest
        directions[half_amount] = Vector3(forward_2d.x, 0, forward_2d.y);
        
        // Calculate angle step for the sides
        float total_spread = max_rad - min_rad;
        float angle_step = total_spread / (amount - 1);
        
        // Generate left side (negative angles)
        for (int i = 0; i < half_amount; i++) {
            float angle = base_angle - (angle_step * (half_amount - i));
            Vector2 dir_2d = Vector2(Math::cos(angle), Math::sin(angle));
            directions[i] = Vector3(dir_2d.x, 0, dir_2d.y);
        }
        
        // Generate right side (positive angles)
        for (int i = 0; i < half_amount; i++) {
            float angle = base_angle + (angle_step * (i + 1));
            Vector2 dir_2d = Vector2(Math::cos(angle), Math::sin(angle));
            directions[amount - 1 - i] = Vector3(dir_2d.x, 0, dir_2d.y);
        }
    } else {
        // For even numbers, spread evenly
        float total_spread = max_rad - min_rad;
        float angle_step = total_spread / (amount - 1);
        for (int i = 0; i < amount; i++) {
            float angle = base_angle + min_rad + (angle_step * i);
            Vector2 dir_2d = Vector2(Math::cos(angle), Math::sin(angle));
            directions[i] = Vector3(dir_2d.x, 0, dir_2d.y);
        }
    }
    
    return directions;
}

YDir::YDir() {
    rng.instantiate();
    rng->randomize();
    singleton = this;
}

YDir::~YDir() {
    if (rng.is_valid()) {
        rng = Ref<RandomNumberGenerator>();
    }
    if (singleton == this) {
        singleton = nullptr;
    }
} 