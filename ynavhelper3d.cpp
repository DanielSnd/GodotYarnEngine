// nav_helper_3d.cpp
#include "ynavhelper3d.h"

YNavHelper3D::YNavHelper3D() {
    rng = memnew(RandomNumberGenerator);
    rng->randomize();
    random_unique_number = rng->randf_range(0, 9999);
}

YNavHelper3D::~YNavHelper3D() {
    memdelete(rng);
    
    // Clean up debug visualization objects
    if (debug_mesh_instance) {
        debug_mesh_instance->queue_free();
    }
    
    if (debug_immediate_mesh) {
        memdelete(debug_immediate_mesh);
    }
    
    if (debug_material) {
        memdelete(debug_material);
    }
}

void YNavHelper3D::_notification(int p_what) {
    if (p_what == NOTIFICATION_READY) {
        // Distribute raycast directions around a sphere
        initialize_raycasts();
        
        // Set initial position
        center_location = get_parent_node_3d()->get_global_position();
        
        initialized = true;
    } else if (p_what == NOTIFICATION_PROCESS) {
        do_process(get_process_delta_time());
    } else if (p_what == NOTIFICATION_PHYSICS_PROCESS) {
        do_physics_process(get_physics_process_delta_time());
    }
}

void YNavHelper3D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_auto_steer", "auto_steer"), &YNavHelper3D::set_auto_steer);
    ClassDB::bind_method(D_METHOD("get_auto_steer"), &YNavHelper3D::get_auto_steer);
    
    ClassDB::bind_method(D_METHOD("set_auto_steering_speed", "speed"), &YNavHelper3D::set_auto_steering_speed);
    ClassDB::bind_method(D_METHOD("get_auto_steering_speed"), &YNavHelper3D::get_auto_steering_speed);
    
    ClassDB::bind_method(D_METHOD("set_follow_target", "target"), &YNavHelper3D::set_follow_target);
    ClassDB::bind_method(D_METHOD("get_follow_target"), &YNavHelper3D::get_follow_target);
    
    ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &YNavHelper3D::set_draw_debug);
    ClassDB::bind_method(D_METHOD("get_draw_debug"), &YNavHelper3D::get_draw_debug);
    
    ClassDB::bind_method(D_METHOD("set_verbose_debug", "verbose_debug"), &YNavHelper3D::set_verbose_debug);
    ClassDB::bind_method(D_METHOD("get_verbose_debug"), &YNavHelper3D::get_verbose_debug);
    
    ClassDB::bind_method(D_METHOD("set_requires_line_of_sight", "requires_los"), &YNavHelper3D::set_requires_line_of_sight);
    ClassDB::bind_method(D_METHOD("get_requires_line_of_sight"), &YNavHelper3D::get_requires_line_of_sight);
    
    ClassDB::bind_method(D_METHOD("set_navigation_enabled", "enabled"), &YNavHelper3D::set_navigation_enabled);
    ClassDB::bind_method(D_METHOD("get_navigation_enabled"), &YNavHelper3D::get_navigation_enabled);
    
    ClassDB::bind_method(D_METHOD("set_navigation_mode", "mode"), &YNavHelper3D::set_navigation_mode);
    ClassDB::bind_method(D_METHOD("get_navigation_mode"), &YNavHelper3D::get_navigation_mode);
    
    ClassDB::bind_method(D_METHOD("set_keep_looking_at_position", "keep_looking"), &YNavHelper3D::set_keep_looking_at_position);
    ClassDB::bind_method(D_METHOD("get_keep_looking_at_position"), &YNavHelper3D::get_keep_looking_at_position);
    
    ClassDB::bind_method(D_METHOD("set_max_wander", "max_wander"), &YNavHelper3D::set_max_wander);
    ClassDB::bind_method(D_METHOD("get_max_wander"), &YNavHelper3D::get_max_wander);
    
    ClassDB::bind_method(D_METHOD("set_navigation_collide_mask", "mask"), &YNavHelper3D::set_navigation_collide_mask);
    ClassDB::bind_method(D_METHOD("get_navigation_collide_mask"), &YNavHelper3D::get_navigation_collide_mask);
    
    ClassDB::bind_method(D_METHOD("set_direction_amount", "amount"), &YNavHelper3D::set_direction_amount);
    ClassDB::bind_method(D_METHOD("get_direction_amount"), &YNavHelper3D::get_direction_amount);
    
    ClassDB::bind_method(D_METHOD("set_extend_length", "length"), &YNavHelper3D::set_extend_length);
    ClassDB::bind_method(D_METHOD("get_extend_length"), &YNavHelper3D::get_extend_length);
    
    ClassDB::bind_method(D_METHOD("set_stop_close_enough", "distance"), &YNavHelper3D::set_stop_close_enough);
    ClassDB::bind_method(D_METHOD("get_stop_close_enough"), &YNavHelper3D::get_stop_close_enough);
    
    ClassDB::bind_method(D_METHOD("set_stop_away_enough", "distance"), &YNavHelper3D::set_stop_away_enough);
    ClassDB::bind_method(D_METHOD("get_stop_away_enough"), &YNavHelper3D::get_stop_away_enough);
    
    ClassDB::bind_method(D_METHOD("set_encircle_when_close_enough", "encircle"), &YNavHelper3D::set_encircle_when_close_enough);
    ClassDB::bind_method(D_METHOD("get_encircle_when_close_enough"), &YNavHelper3D::get_encircle_when_close_enough);
    
    ClassDB::bind_method(D_METHOD("set_away_when_close_enough", "away"), &YNavHelper3D::set_away_when_close_enough);
    ClassDB::bind_method(D_METHOD("get_away_when_close_enough"), &YNavHelper3D::get_away_when_close_enough);
    
    ClassDB::bind_method(D_METHOD("set_way_too_close_multiplier", "multiplier"), &YNavHelper3D::set_way_too_close_multiplier);
    ClassDB::bind_method(D_METHOD("get_way_too_close_multiplier"), &YNavHelper3D::get_way_too_close_multiplier);
    
    ClassDB::bind_method(D_METHOD("set_way_too_close_flee_speed", "speed"), &YNavHelper3D::set_way_too_close_flee_speed);
    ClassDB::bind_method(D_METHOD("get_way_too_close_flee_speed"), &YNavHelper3D::get_way_too_close_flee_speed);
    
    ClassDB::bind_method(D_METHOD("set_direction_lerp_speed_closer", "speed"), &YNavHelper3D::set_direction_lerp_speed_closer);
    ClassDB::bind_method(D_METHOD("get_direction_lerp_speed_closer"), &YNavHelper3D::get_direction_lerp_speed_closer);
    
    ClassDB::bind_method(D_METHOD("set_direction_lerp_speed_far", "speed"), &YNavHelper3D::set_direction_lerp_speed_far);
    ClassDB::bind_method(D_METHOD("get_direction_lerp_speed_far"), &YNavHelper3D::get_direction_lerp_speed_far);
    
    ClassDB::bind_method(D_METHOD("set_steering_lerp_speed", "speed"), &YNavHelper3D::set_steering_lerp_speed);
    ClassDB::bind_method(D_METHOD("get_steering_lerp_speed"), &YNavHelper3D::get_steering_lerp_speed);
    
    ClassDB::bind_method(D_METHOD("set_speed_multiplier", "multiplier"), &YNavHelper3D::set_speed_multiplier);
    ClassDB::bind_method(D_METHOD("get_speed_multiplier"), &YNavHelper3D::get_speed_multiplier);
    
    ClassDB::bind_method(D_METHOD("get_desired_direction"), &YNavHelper3D::get_desired_direction);
    
    ClassDB::bind_method(D_METHOD("set_relevant_position", "position"), &YNavHelper3D::set_relevant_position);
    ClassDB::bind_method(D_METHOD("get_relevant_position"), &YNavHelper3D::get_relevant_position);
    
    ClassDB::bind_method(D_METHOD("set_relevant_direction", "direction"), &YNavHelper3D::set_relevant_direction);
    ClassDB::bind_method(D_METHOD("get_relevant_direction"), &YNavHelper3D::get_relevant_direction);

    // Register properties
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_steer"), "set_auto_steer", "get_auto_steer");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "auto_steering_speed", PROPERTY_HINT_RANGE, "0,200,0.1"), "set_auto_steering_speed", "get_auto_steering_speed");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "follow_target", PROPERTY_HINT_NODE_TYPE, "Node3D"), "set_follow_target", "get_follow_target");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "verbose_debug"), "set_verbose_debug", "get_verbose_debug");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "requires_line_of_sight"), "set_requires_line_of_sight", "get_requires_line_of_sight");
    
    ADD_GROUP("Navigation Options", "navigation_");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "navigation_enabled"), "set_navigation_enabled", "get_navigation_enabled");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "navigation_mode", PROPERTY_HINT_ENUM, "TowardsTarget,AwayFromTarget,EncircleTarget,TowardsPosition,TowardsDirection,AwayFromPosition,EncirclePosition,AwayFromDirection,Wandering"), "set_navigation_mode", "get_navigation_mode");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "keep_looking_at_position"), "set_keep_looking_at_position", "get_keep_looking_at_position");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_wander", PROPERTY_HINT_RANGE, "30,250,1"), "set_max_wander", "get_max_wander");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "navigation_collide_mask", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_navigation_collide_mask", "get_navigation_collide_mask");
    
    ADD_GROUP("Raycasts", "");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "direction_amount", PROPERTY_HINT_RANGE, "4,32,1"), "set_direction_amount", "get_direction_amount");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "extend_length", PROPERTY_HINT_RANGE, "5,100,0.1"), "set_extend_length", "get_extend_length");
    
    ADD_GROUP("Distance", "");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "stop_close_enough", PROPERTY_HINT_RANGE, "0,200,0.1"), "set_stop_close_enough", "get_stop_close_enough");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "stop_away_enough", PROPERTY_HINT_RANGE, "0,300,0.1"), "set_stop_away_enough", "get_stop_away_enough");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "encircle_when_close_enough"), "set_encircle_when_close_enough", "get_encircle_when_close_enough");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "away_when_close_enough"), "set_away_when_close_enough", "get_away_when_close_enough");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "way_too_close_multiplier", PROPERTY_HINT_RANGE, "0,0.9,0.01"), "set_way_too_close_multiplier", "get_way_too_close_multiplier");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "way_too_close_flee_speed", PROPERTY_HINT_RANGE, "1,3,0.1"), "set_way_too_close_flee_speed", "get_way_too_close_flee_speed");
    
    ADD_GROUP("Lerping Speeds", "");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "direction_lerp_speed_closer", PROPERTY_HINT_RANGE, "0,16,0.1"), "set_direction_lerp_speed_closer", "get_direction_lerp_speed_closer");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "direction_lerp_speed_far", PROPERTY_HINT_RANGE, "0,16,0.1"), "set_direction_lerp_speed_far", "get_direction_lerp_speed_far");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "steering_lerp_speed", PROPERTY_HINT_RANGE, "0,16,0.1"), "set_steering_lerp_speed", "get_steering_lerp_speed");

    // Register enum
    BIND_ENUM_CONSTANT(TOWARDS_TARGET);
    BIND_ENUM_CONSTANT(AWAY_FROM_TARGET);
    BIND_ENUM_CONSTANT(ENCIRCLE_TARGET);
    BIND_ENUM_CONSTANT(TOWARDS_POSITION);
    BIND_ENUM_CONSTANT(TOWARDS_DIRECTION);
    BIND_ENUM_CONSTANT(AWAY_FROM_POSITION);
    BIND_ENUM_CONSTANT(ENCIRCLE_POSITION);
    BIND_ENUM_CONSTANT(AWAY_FROM_DIRECTION);
    BIND_ENUM_CONSTANT(WANDERING);
}


void YNavHelper3D::initialize_raycasts() {
    // Using the golden spiral method for distributing points evenly on a sphere

    for (int i = 0; i < direction_amount; i++) {
        // Golden spiral points on a sphere
        float y = 1.0f - (2.0f * i) / (direction_amount - 1.0f);
        float radius = sqrt(1.0f - y * y);
        
        float theta = Math_PI * (3.0f - sqrt(5.0f)); // Golden angle
        float phi = theta * i;
        
        float x = cos(phi) * radius;
        float z = sin(phi) * radius;
        
        Vector3 direction = Vector3(x, y, z).normalized();
        directions.push_back(direction);
        // Initialize arrays
        interest_values.push_back(0.0f);
        danger_values.push_back(0.0f);
        context_map.push_back(0.0f);
    }
}

void YNavHelper3D::do_process(double delta) {
    if (!initialized || !navigation_enabled) {
        return;
    }

    // Handle invalid follow target
    if (follow_target != nullptr && !follow_target->is_inside_tree()) {
        follow_target = nullptr;
    }

    // Debug drawing (in 3D, you might want to use debug lines/meshes)
    if (last_draw_debug != draw_debug) {
        last_draw_debug = draw_debug;
        
        // Implement debug visualization (e.g., with MeshInstances or ImmediateMesh)
        if (draw_debug) {
            // Create immediate mesh for debug visualization if not already created
            if (!debug_mesh_instance) {
                debug_mesh_instance = memnew(MeshInstance3D);
                add_child(debug_mesh_instance);
                debug_immediate_mesh = memnew(ImmediateMesh);
                debug_material = memnew(StandardMaterial3D);
                debug_material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
                debug_material->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);
                debug_mesh_instance->set_mesh(debug_immediate_mesh);
                debug_mesh_instance->set_material_override(debug_material);
            }
        } else {
            // Clean up debug visualization
            if (debug_mesh_instance) {
                debug_mesh_instance->queue_free();
                debug_mesh_instance = nullptr;
                memdelete(debug_immediate_mesh);
                debug_immediate_mesh = nullptr;
                memdelete(debug_material);
                debug_material = nullptr;
            }
        }
    }
    
    // Update debug visualization if enabled
    if (draw_debug && debug_immediate_mesh) {
        update_debug_visualization();
    }

    // Print verbose debug information if enabled
    if (verbose_debug && YTime::get_singleton()->has_time_elapsed(last_time_verbose_debug, 0.5f)) {
        last_time_verbose_debug = YTime::get_singleton()->get_time();
        print_debug_info();
    }

    // Check line of sight
    if (requires_line_of_sight && YTime::get_singleton()->has_time_elapsed(last_time_checked_los, 0.22f)) {
        update_line_of_sight();
    }

    // Determine navigation mode and parameters
    bool using_follow_target = (navigation_mode == TOWARDS_TARGET || 
                              navigation_mode == AWAY_FROM_TARGET || 
                              navigation_mode == ENCIRCLE_TARGET);
                              
    away_from = (navigation_mode == AWAY_FROM_DIRECTION || 
                navigation_mode == AWAY_FROM_POSITION || 
                navigation_mode == AWAY_FROM_TARGET);

    if (navigation_mode == WANDERING || 
        navigation_mode == TOWARDS_DIRECTION || 
        navigation_mode == AWAY_FROM_DIRECTION) {
        using_position = false;
    }

    if (using_follow_target) {
        if (follow_target && (has_line_of_sight || !requires_line_of_sight)) {
            relevant_position = follow_target->get_global_position();
        }
        using_position = true;
    }

    if (using_follow_target || 
        navigation_mode == AWAY_FROM_POSITION || 
        navigation_mode == TOWARDS_POSITION || 
        navigation_mode == ENCIRCLE_POSITION) {
        using_position = true;
    }

    if (using_position) {
        distance_to_relevant = get_parent_node_3d()->get_global_position().distance_to(relevant_position);
        is_close_enough = distance_to_relevant < stop_close_enough;
        way_too_close = distance_to_relevant < stop_close_enough * way_too_close_multiplier;
        encircling = (navigation_mode == ENCIRCLE_TARGET || navigation_mode == ENCIRCLE_POSITION);

        if (away_from || !is_close_enough || encircle_when_close_enough || 
            (away_when_close_enough && is_close_enough) || way_too_close || encircling) {
            calculate_context_map(delta, (relevant_position - get_parent_node_3d()->get_global_position()).normalized());
        } else {
        }
    } else {
        if (navigation_mode == WANDERING) {
            update_wander_direction(delta);
            update_wander_speed(delta);
            calculate_context_map(delta, wander_direction);
        } else {
            calculate_context_map(delta, relevant_direction);
        }
    }
}

void YNavHelper3D::do_physics_process(double delta) {
    if (!initialized || !navigation_enabled || !auto_steer) {
        return;
    }

    // The actual steering and movement would be applied to the parent CharacterBody3D
    // We'll calculate the desired velocity and emit signals for the parent to use
    
    // Get the parent node
    Node3D* parent = get_parent_node_3d();
    if (!parent) {
        return;
    }
    
    // Assuming the parent has velocity property
    if (navigation_mode == WANDERING) {
        // Apply wandering behavior
        Vector3 new_velocity = desired_direction.normalized() * wander_speed;
        parent->set("velocity", new_velocity);
        return;
    }

    Vector3 desired_velocity = desired_direction.normalized() * (auto_steering_speed * speed_multiplier);

    if (using_position && !encircle_when_close_enough && !away_from && !way_too_close) {
        if ((is_close_enough && !away_when_close_enough && !encircle_when_close_enough) || 
            (away_when_close_enough && is_close_enough && distance_to_relevant > stop_away_enough)) {
            desired_velocity = Vector3(0, 0, 0);
        }
    }
    
    if ((away_from || encircling) && using_position && distance_to_relevant > stop_away_enough && !way_too_close) {
        desired_velocity = Vector3(0, 0, 0);
    }

    float lerping_multiplier = 1.0f;
    if ((encircle_when_close_enough && is_close_enough) || encircling) {
        lerping_multiplier = 2.5f;
    }

    if (way_too_close) {
        lerping_multiplier = 1.6f;
        desired_velocity *= way_too_close_flee_speed;
    }

    Vector3 current_velocity = parent->get("velocity");
    Vector3 steering_force = desired_velocity - current_velocity;
    
    current_velocity += steering_force * (steering_lerp_speed * lerping_multiplier) * delta;
    parent->set("velocity", current_velocity);
    
    // Make the parent face in the movement direction if needed
    if (using_position && keep_looking_at_position) {
        Vector3 look_dir = parent->get_global_position().direction_to(relevant_position);
        if (look_dir.length() > 0.001f) {
            parent->call("look_at", parent->get_global_position() + look_dir, Vector3(0, 1, 0));
        }
    } else {
        Vector3 look_dir = current_velocity;
        if (look_dir.length() > 0.001f) {
            parent->call("look_at", parent->get_global_position() + look_dir, Vector3(0, 1, 0));
        }
    }
}

void YNavHelper3D::reset_danger_values() {
    for (int i = 0; i < danger_values.size(); i++) {
        danger_values.write[i] = 0.0f;
    }
}

void YNavHelper3D::calculate_danger_and_interest_values(const Vector3& use_direction) {
    for (int i = 0; i < directions.size(); i++) {
        Vector3 dir = directions[i];
        
        if (encircling || ((encircle_when_close_enough && is_close_enough) && !way_too_close)) {
            // For encircling, use a cross product to find perpendicular direction
            Vector3 up_vector = Vector3(0, 1, 0); // Usually up in 3D, might need to adjust
            Vector3 perpendicular = use_direction.cross(up_vector).normalized();
            
            if (get_preferred_break_direction() < 0) {
                perpendicular = -perpendicular;
            }
            
            interest_values.write[i] = dir.dot(perpendicular);
        } else {
            float dot_product = dir.dot(use_direction);
            
            // If moving away or too close, invert the interest
            float multiplier = (away_from || 
                               (away_when_close_enough && is_close_enough && distance_to_relevant < stop_away_enough) || 
                               way_too_close) ? -1.0f : 1.0f;
                               
            interest_values.write[i] = dot_product * multiplier;
        }
        
        // Check for collisions
        if (YPhysics::has_raycast3d_hit(directions[i], directions[i], extend_length, YPhysics::COLLIDE_WITH_BODIES, navigation_collide_mask)) {
            danger_values.write[i] += 5.0f;
            
            // Add danger to neighboring directions
            int prev_index = get_prev_index(i);
            int next_index = get_next_index(i);
            danger_values.write[prev_index] += 2.0f;
            danger_values.write[next_index] += 2.0f;
            
            // For finer direction resolution
            if (direction_amount > 8) {
                danger_values.write[get_prev_index(prev_index)] += 1.0f;
                danger_values.write[get_next_index(next_index)] += 1.0f;
            }
        }
    }
}

void YNavHelper3D::calculate_best_direction() {
    int prev_best_direction_index = best_direction_index;
    best_direction_index = -1;
    best_value = -99.0f;
    worst_value = 99.0f;
    
    for (int i = 0; i < directions.size(); i++) {
        float context_value = interest_values[i] - danger_values[i];
        context_map.write[i] = context_value;
        
        if (context_value > best_value) {
            best_value = context_value;
            best_direction_index = i;
        }
        
        if (context_value < worst_value) {
            worst_value = context_value;
        }
    }
    
    // Extend the best raycast for visualization
//     if (prev_best_direction_index != best_direction_index && prev_best_direction_index >= 0) {
//         Object::cast_to<RayCast3D>(raycasts[prev_best_direction_index])->set_target_position(
//             Object::cast_to<Vector3>(directions[prev_best_direction_index]) * extend_length);
        
//         Object::cast_to<RayCast3D>(raycasts[best_direction_index])->set_target_position(
//             Object::cast_to<Vector3>(directions[best_direction_index]) * (extend_length * 1.4f));
//     }
}

void YNavHelper3D::calculate_context_map(double delta, const Vector3& use_direction) {
    reset_danger_values();
    calculate_danger_and_interest_values(use_direction);
    calculate_best_direction();
    
    if (best_direction_index < 0 || best_direction_index >= directions.size()) {
        return; // No valid direction found
    }
    
    Vector3 best_dir = directions[best_direction_index];
    
    // Calculate how different the new direction is from current desired direction
    float dot_product = best_dir.dot(desired_direction.normalized());
    float difference = 1.0f - (dot_product + 1.0f) / 2.0f; // Map from [-1,1] to [0,1]
    
    float use_lerp_direction = Math::lerp(direction_lerp_speed_far, direction_lerp_speed_closer, difference);
    
    // Adjust lerp speed based on distance
    if (distance_to_relevant < 16.5f) {
        use_lerp_direction *= 3.0f;
    }
    
    // Apply additional multiplier for encircling
    float encircle_multiplier = ((encircle_when_close_enough && is_close_enough) || encircling) ? 2.0f : 1.0f;
    
    // Update desired direction with smooth interpolation
    desired_direction = desired_direction.lerp(
        best_dir, 
        delta * use_lerp_direction * encircle_multiplier
    ).normalized();
}

void YNavHelper3D::update_wander_direction(double delta) {
    // Use noise for organic wandering
    float time = YTime::get_singleton()->get_time();
    float noise_value = sin(time + random_unique_number) * 0.1f; // Simplified noise
    
    // Create a basis to rotate around the UP vector
    Basis rotation = Basis(Vector3(0, 1, 0), noise_value);
    wander_direction = rotation.xform(wander_direction).normalized();
    
    // Bias direction to stay within max wander distance
    float distance_from_initial = get_parent_node_3d()->get_global_position().distance_to(center_location);
    
    if (distance_from_initial > max_wander * 0.5f) {
        float bias_amount = Math::inverse_lerp(max_wander * 0.6f, max_wander, distance_from_initial);
        Vector3 direction_to_center = get_parent_node_3d()->get_global_position().direction_to(center_location);
        
        wander_direction = wander_direction.lerp(
            direction_to_center, 
            delta * (1.0f + (bias_amount * 3.0f))
        ).normalized();
    }
}

void YNavHelper3D::update_wander_speed(double delta) {
    float time = YTime::get_singleton()->get_time();
    float noise_found = sin(time * 2.0f + random_unique_number) * 0.5f + 0.5f; // Range [0, 1]
    
    float desired_wander_speed = noise_found * (auto_steering_speed * speed_multiplier);
    float lerping_wander_speed = 16.5f;
    
    if (desired_wander_speed < std::min(8.5f, (auto_steering_speed * speed_multiplier) * 0.4f)) {
        lerping_wander_speed = 8.0f;
        desired_wander_speed = 0.0f;
    }
    
    wander_speed = Math::lerp(wander_speed, desired_wander_speed, static_cast<float>(delta) * lerping_wander_speed);
}

void YNavHelper3D::update_line_of_sight() {
    last_time_checked_los = YTime::get_singleton()->get_time();
    
    if (follow_target == nullptr) {
        has_line_of_sight = false;
        return;
    }
    
    Vector3 target_pos = follow_target->get_global_position();
    Vector3 my_pos = get_parent_node_3d()->get_global_position();
    
    // Check if target is reachable via raycast
    if (my_pos.distance_to(target_pos) < 800.0f && !YPhysics::free_line_check(my_pos, target_pos, YPhysics::COLLIDE_WITH_BODIES, navigation_collide_mask)) {
        has_line_of_sight = true;
        time_missing_line_of_sight = 0.0f;
        return;
    }

    time_missing_line_of_sight += 0.35f;
}

int YNavHelper3D::get_preferred_break_direction() const {
    float time = YTime::get_singleton()->get_time();
    return sin(time * 0.5f + random_unique_number) > 0 ? 1 : -1;
}

int YNavHelper3D::get_next_index(int curr_index) const {
    return (curr_index + 1) % danger_values.size();
}

int YNavHelper3D::get_prev_index(int curr_index) const {
    return (curr_index - 1 + danger_values.size()) % danger_values.size();
}

// Getters and Setters
void YNavHelper3D::set_auto_steer(bool p_auto_steer) {
    auto_steer = p_auto_steer;
}

bool YNavHelper3D::get_auto_steer() const {
    return auto_steer;
}

void YNavHelper3D::set_auto_steering_speed(float p_speed) {
    auto_steering_speed = p_speed;
}

float YNavHelper3D::get_auto_steering_speed() const {
    return auto_steering_speed;
}

void YNavHelper3D::set_follow_target(Node3D* p_target) {
    follow_target = p_target;
}

Node3D* YNavHelper3D::get_follow_target() const {
    return follow_target;
}

void YNavHelper3D::set_draw_debug(bool p_draw_debug) {
    draw_debug = p_draw_debug;
}

bool YNavHelper3D::get_draw_debug() const {
    return draw_debug;
}

void YNavHelper3D::set_verbose_debug(bool p_verbose_debug) {
    verbose_debug = p_verbose_debug;
}

bool YNavHelper3D::get_verbose_debug() const {
    return verbose_debug;
}

void YNavHelper3D::set_requires_line_of_sight(bool p_requires_los) {
    requires_line_of_sight = p_requires_los;
}

bool YNavHelper3D::get_requires_line_of_sight() const {
    return requires_line_of_sight;
}

void YNavHelper3D::set_navigation_enabled(bool p_enabled) {
    navigation_enabled = p_enabled;
}

bool YNavHelper3D::get_navigation_enabled() const {
    return navigation_enabled;
}

void YNavHelper3D::set_navigation_mode(NavigationMode p_mode) {
    navigation_mode = p_mode;
}

YNavHelper3D::NavigationMode YNavHelper3D::get_navigation_mode() const {
    return navigation_mode;
}

void YNavHelper3D::set_keep_looking_at_position(bool p_keep_looking) {
    keep_looking_at_position = p_keep_looking;
}

bool YNavHelper3D::get_keep_looking_at_position() const {
    return keep_looking_at_position;
}

void YNavHelper3D::set_max_wander(float p_max_wander) {
    max_wander = p_max_wander;
}

float YNavHelper3D::get_max_wander() const {
    return max_wander;
}

void YNavHelper3D::set_navigation_collide_mask(uint32_t p_mask) {
    navigation_collide_mask = p_mask;
}

uint32_t YNavHelper3D::get_navigation_collide_mask() const {
    return navigation_collide_mask;
}

void YNavHelper3D::set_direction_amount(int p_amount) {
    if (initialized) {
        return; // Can't change after initialization
    }
    direction_amount = p_amount;
}

int YNavHelper3D::get_direction_amount() const {
    return direction_amount;
}

void YNavHelper3D::set_extend_length(float p_length) {
    extend_length = p_length;
}

float YNavHelper3D::get_extend_length() const {
    return extend_length;
}

void YNavHelper3D::set_stop_close_enough(float p_distance) {
    stop_close_enough = p_distance;
}

float YNavHelper3D::get_stop_close_enough() const {
    return stop_close_enough;
}

void YNavHelper3D::set_stop_away_enough(float p_distance) {
    stop_away_enough = p_distance;
}

float YNavHelper3D::get_stop_away_enough() const {
    return stop_away_enough;
}

void YNavHelper3D::set_encircle_when_close_enough(bool p_encircle) {
    encircle_when_close_enough = p_encircle;
}

bool YNavHelper3D::get_encircle_when_close_enough() const {
    return encircle_when_close_enough;
}

void YNavHelper3D::set_away_when_close_enough(bool p_away) {
    away_when_close_enough = p_away;
}

bool YNavHelper3D::get_away_when_close_enough() const {
    return away_when_close_enough;
}

void YNavHelper3D::set_way_too_close_multiplier(float p_multiplier) {
    way_too_close_multiplier = p_multiplier;
}

float YNavHelper3D::get_way_too_close_multiplier() const {
    return way_too_close_multiplier;
}

void YNavHelper3D::set_way_too_close_flee_speed(float p_speed) {
    way_too_close_flee_speed = p_speed;
}

float YNavHelper3D::get_way_too_close_flee_speed() const {
    return way_too_close_flee_speed;
}

void YNavHelper3D::set_direction_lerp_speed_closer(float p_speed) {
    direction_lerp_speed_closer = p_speed;
}

float YNavHelper3D::get_direction_lerp_speed_closer() const {
    return direction_lerp_speed_closer;
}

void YNavHelper3D::set_direction_lerp_speed_far(float p_speed) {
    direction_lerp_speed_far = p_speed;
}

float YNavHelper3D::get_direction_lerp_speed_far() const {
    return direction_lerp_speed_far;
}

void YNavHelper3D::set_steering_lerp_speed(float p_speed) {
    steering_lerp_speed = p_speed;
}

float YNavHelper3D::get_steering_lerp_speed() const {
    return steering_lerp_speed;
}

void YNavHelper3D::set_speed_multiplier(float p_multiplier) {
    speed_multiplier = p_multiplier;
}

float YNavHelper3D::get_speed_multiplier() const {
    return speed_multiplier;
}

Vector3 YNavHelper3D::get_desired_direction() const {
    return desired_direction;
}

void YNavHelper3D::set_relevant_position(const Vector3& p_position) {
    relevant_position = p_position;
}

Vector3 YNavHelper3D::get_relevant_position() const {
    return relevant_position;
}

void YNavHelper3D::set_relevant_direction(const Vector3& p_direction) {
    relevant_direction = p_direction.normalized();
}

Vector3 YNavHelper3D::get_relevant_direction() const {
    return relevant_direction;
}

void YNavHelper3D::update_debug_visualization() {
    if (!debug_immediate_mesh) {
        return;
    }
    
    // Clear previous debug visualization
    debug_immediate_mesh->clear_surfaces();
    
    // Start drawing
    debug_immediate_mesh->surface_begin(Mesh::PRIMITIVE_LINES);
    
    Vector3 origin = Vector3(0, 0, 0);
    
    // Draw all direction rays
    for (int i = 0; i < directions.size(); i++) {
        // Normalize the context value to a color (red = danger, green = interest)
        float context_value = context_map[i];
        float normalized_value = Math::inverse_lerp(worst_value, best_value, context_value);
        
        // Use different colors based on the value
        Color ray_color;
        if (danger_values[i] > 0) {
            // Danger rays in red
            ray_color = Color(1.0, 0.0, 0.0, 0.7);
        } else if (i == best_direction_index) {
            // Best direction in bright green
            ray_color = Color(0.0, 1.0, 0.0, 1.0);
        } else {
            // Interest rays in blue to cyan gradient based on value
            ray_color = Color(0.0, normalized_value, 1.0, 0.5);
        }
        
        debug_immediate_mesh->surface_set_color(ray_color);
        
        // Draw the ray
        float ray_length = extend_length;
        if (i == best_direction_index) {
            ray_length *= 1.4f; // Make best direction ray longer
        }
        
        debug_immediate_mesh->surface_add_vertex(origin);
        debug_immediate_mesh->surface_add_vertex(directions[i] * ray_length);
    }
    
    // Draw desired direction in yellow
    debug_immediate_mesh->surface_set_color(Color(1.0, 1.0, 0.0, 1.0));
    debug_immediate_mesh->surface_add_vertex(origin);
    debug_immediate_mesh->surface_add_vertex(desired_direction * extend_length * 1.2f);
    
    // If following a target, draw a line to it
    if (using_position && follow_target) {
        debug_immediate_mesh->surface_set_color(Color(1.0, 0.5, 0.0, 0.8)); // Orange
        debug_immediate_mesh->surface_add_vertex(origin);
        Vector3 direction_to_target = (relevant_position - get_global_position()).normalized();
        debug_immediate_mesh->surface_add_vertex(direction_to_target * std::min(extend_length, distance_to_relevant));
    }
    
    // Finish drawing
    debug_immediate_mesh->surface_end();
}

void YNavHelper3D::print_debug_info() {
    String debug_info = vformat("[YNavHelper3D] Debug Info for %s:", get_name());
    debug_info += vformat("\n  Navigation Mode: %d", navigation_mode);
    debug_info += vformat("\n  Desired Direction: (%.2f, %.2f, %.2f)", desired_direction.x, desired_direction.y, desired_direction.z);
    
    if (using_position) {
        debug_info += vformat("\n  Distance to target: %.2f (Close enough: %s)", 
                              distance_to_relevant, is_close_enough ? "Yes" : "No");
        debug_info += vformat("\n  Way too close: %s", way_too_close ? "Yes" : "No");
    }
    
    if (best_direction_index >= 0 && best_direction_index < directions.size()) {
        debug_info += vformat("\n  Best Direction Index: %d, Value: %.2f", best_direction_index, best_value);
        debug_info += vformat("\n  Worst Value: %.2f", worst_value);
    }
    
    if (follow_target) {
        debug_info += vformat("\n  Has Line of Sight: %s", has_line_of_sight ? "Yes" : "No");
        if (!has_line_of_sight) {
            debug_info += vformat("\n  Time Missing LOS: %.2f", time_missing_line_of_sight);
        }
    }
    
    if (navigation_mode == WANDERING) {
        debug_info += vformat("\n  Wander Speed: %.2f", wander_speed);
        debug_info += vformat("\n  Distance from Center: %.2f (Max: %.2f)",
                             get_parent_node_3d()->get_global_position().distance_to(center_location), max_wander);
    }
    
    print_line(debug_info);
}
