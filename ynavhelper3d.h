// nav_helper_3d.h
#ifndef YNAV_HELPER_3D_H
#define YNAV_HELPER_3D_H

#include "core/math/random_number_generator.h"
#include "scene/3d/node_3d.h"
#include "core/variant/typed_array.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/resources/immediate_mesh.h"
#include "scene/resources/material.h"
#include "yarntime.h"
#include "yarnphysics.h"
#include "modules/noise/fastnoise_lite.h"

class YNavHelper3D : public Node3D {
    GDCLASS(YNavHelper3D, Node3D);

protected:
    static void _bind_methods();

private:
    void initialize_raycasts();
    void reset_danger_values();
    void calculate_danger_and_interest_values(const Vector3& use_direction);
    void calculate_best_direction();
    void calculate_context_map(double delta, const Vector3& use_direction);
    void update_wander_direction(double delta);
    void update_wander_speed(double delta);
    void update_line_of_sight();
    void update_debug_visualization();
    void print_debug_info();
    int get_preferred_break_direction() const;
    int get_next_index(int curr_index) const;
    int get_prev_index(int curr_index) const;
    
    // Core navigation properties
    Vector<Vector3> directions;
    Vector<float> interest_values;
    Vector<float> danger_values;
    Vector<float> context_map;
    
    bool auto_steer = true;
    float auto_steering_speed = 25.0f;
    float update_interval = 0.01f;
    Node3D* follow_target = nullptr;
    bool draw_debug = false;
    bool verbose_debug = false;
    bool requires_line_of_sight = true;
    bool navigation_enabled = true;
    bool keep_looking_at_position = true;
    float max_wander = 15.0f;
    uint32_t navigation_collide_mask = 1;
    int direction_amount = 12;
    float extend_length = 2.0f;
    float stop_close_enough = 3.0f;
    float stop_away_enough = 6.0f;
    bool encircle_when_close_enough = false;
    bool away_when_close_enough = false;
    float way_too_close_multiplier = 0.4f;
    float way_too_close_flee_speed = 2.0f;
    float direction_lerp_speed_closer = 4.0f;
    float direction_lerp_speed_far = 1.5f;
    float steering_lerp_speed = 3.0f;
    
    // State variables
    int best_direction_index = 0;
    float best_value = 0.0f;
    float worst_value = 0.0f;
    Vector3 desired_direction = Vector3(0, 0, -1);
    Vector3 center_location = Vector3(0, 0, 0);
    float wander_speed = 0.0f;
    Vector3 wander_direction = Vector3(0, 0, -1);
    
    Vector3 relevant_position = Vector3(0, 0, 0);
    Vector3 relevant_direction = Vector3(0, 0, -1);
    
    float last_time_updated_context_map = 0.0f;
    float distance_to_relevant = 0.0f;
    bool away_from = false;
    bool using_position = false;
    bool encircling = false;
    bool is_close_enough = false;
    bool way_too_close = false;
    bool last_draw_debug = false;
    bool initialized = false;
    
    bool has_line_of_sight = false;
    double last_time_checked_los = 0.0;
    double time_missing_line_of_sight = 0.0;
    double last_time_verbose_debug = 0.0;
    
    // Debug visualization
    MeshInstance3D* debug_mesh_instance = nullptr;
    Ref<ImmediateMesh> debug_immediate_mesh;
    Ref<StandardMaterial3D> debug_material;
    
    float random_unique_number = 0.0f;
    float speed_multiplier = 1.0f;
    static Ref<FastNoiseLite> noise;
    float wander_speed_multiplier = 1.0f;
    bool wander_prefer_horizontal = false;
    float horizontal_preference_strength = 1.5f;
    bool use_flat_raycasts = false;

    
public:
    enum NavigationMode {
        TOWARDS_TARGET = 0,
        AWAY_FROM_TARGET = 1,
        ENCIRCLE_TARGET = 2,
        TOWARDS_POSITION = 3,
        TOWARDS_DIRECTION = 4,
        AWAY_FROM_POSITION = 5,
        ENCIRCLE_POSITION = 6,
        AWAY_FROM_DIRECTION = 7,
        WANDERING = 8
    };

    NavigationMode navigation_mode = WANDERING;

    YNavHelper3D();
    ~YNavHelper3D();

    void do_process(double delta);
    void do_physics_process(double delta);
    void _notification(int p_what);

    // Getters and setters
    void set_auto_steer(bool p_auto_steer);
    bool get_auto_steer() const;

    void set_update_interval(float p_interval);
    float get_update_interval() const;

    void set_auto_steering_speed(float p_speed);
    float get_auto_steering_speed() const;

    void set_follow_target(Node3D* p_target);
    Node3D* get_follow_target() const;

    void set_draw_debug(bool p_draw_debug);
    bool get_draw_debug() const;
    
    void set_verbose_debug(bool p_verbose_debug);
    bool get_verbose_debug() const;

    void set_requires_line_of_sight(bool p_requires_los);
    bool get_requires_line_of_sight() const;

    void set_navigation_enabled(bool p_enabled);
    bool get_navigation_enabled() const;

    void set_navigation_mode(NavigationMode p_mode);
    NavigationMode get_navigation_mode() const;

    void set_keep_looking_at_position(bool p_keep_looking);
    bool get_keep_looking_at_position() const;

    void set_max_wander(float p_max_wander);
    float get_max_wander() const;

    void set_navigation_collide_mask(uint32_t p_mask);
    uint32_t get_navigation_collide_mask() const;

    void set_direction_amount(int p_amount);
    int get_direction_amount() const;

    void set_extend_length(float p_length);
    float get_extend_length() const;

    void set_stop_close_enough(float p_distance);
    float get_stop_close_enough() const;

    void set_stop_away_enough(float p_distance);
    float get_stop_away_enough() const;

    void set_encircle_when_close_enough(bool p_encircle);
    bool get_encircle_when_close_enough() const;

    void set_away_when_close_enough(bool p_away);
    bool get_away_when_close_enough() const;

    void set_way_too_close_multiplier(float p_multiplier);
    float get_way_too_close_multiplier() const;

    void set_way_too_close_flee_speed(float p_speed);
    float get_way_too_close_flee_speed() const;

    void set_direction_lerp_speed_closer(float p_speed);
    float get_direction_lerp_speed_closer() const;

    void set_direction_lerp_speed_far(float p_speed);
    float get_direction_lerp_speed_far() const;

    void set_steering_lerp_speed(float p_speed);
    float get_steering_lerp_speed() const;

    void set_speed_multiplier(float p_multiplier);
    float get_speed_multiplier() const;

    Vector3 get_desired_direction() const;
    
    void set_relevant_position(const Vector3& p_position);
    Vector3 get_relevant_position() const;
    
    void set_relevant_direction(const Vector3& p_direction);
    Vector3 get_relevant_direction() const;

    void set_wander_prefer_horizontal(bool p_prefer_horizontal);
    bool get_wander_prefer_horizontal() const;
    
    void set_horizontal_preference_strength(float p_strength);
    float get_horizontal_preference_strength() const;

    void set_wander_speed_multiplier(float p_multiplier);
    float get_wander_speed_multiplier() const;

    void set_use_flat_raycasts(bool p_use_flat);
    bool get_use_flat_raycasts() const;
};

VARIANT_ENUM_CAST(YNavHelper3D::NavigationMode);
#endif // NAV_HELPER_3D_H
