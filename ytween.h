//
// Created by Daniel on 2024-03-21.
//

#ifndef YTWEEN_H
#define YTWEEN_H
#include "core/object/ref_counted.h"
#include "scene/animation/tween.h"
#include "yarnengine.h"
#include "scene/gui/control.h"
#include "scene/2d/node_2d.h"
#include "yarntime.h"
#include "core/os/mutex.h"
#include "core/os/thread_safe.h"

class Tween;
class SceneTree;
class Control;
class Node2D;

class YTweenWrap : public Tween {
    GDCLASS(YTweenWrap, Tween);
protected:
    static void _bind_methods();

public:
    uint64_t tween_list_id;
    bool emitted_finished_or_killed = false;

    // TweenProcessMode ytween_process_mode = TweenProcessMode::TWEEN_PROCESS_IDLE;
    Ref<PropertyTweener> set_ytrans(Tween::TransitionType p_trans, real_t p_param1 = INFINITY, real_t p_param2 = INFINITY);

    Ref<PropertyTweener> set_ytrans_in_property(Ref<PropertyTweener> tweenproperty, Tween::TransitionType p_trans,
                                                real_t p_param1, real_t p_param2);

    void kill_due_to_node_tree_exiting();
    void register_finished_extra_callback();

    void register_node_kill_when(Node *p_node_owner);
    void emitted_finished();

    Ref<PropertyTweener> ytween_property(const Object *p_target, const NodePath &p_property, Variant p_to,
                                         double p_duration, Tween::EaseType p_ease, Tween::TransitionType p_trans, real_t p_param1, real_t p_param2);

    Ref<YTweenWrap> yparallel();
	Ref<YTweenWrap> ychain();
    
    YTweenWrap() : Tween() {
        tween_list_id = 0;
    }
    explicit YTweenWrap(SceneTree *p_parent_tree) : Tween(p_parent_tree) {
        tween_list_id = 0;
    }
};

class YTweenJiggle : public YTweenWrap {
    GDCLASS(YTweenJiggle, YTweenWrap);

public:
    Node3D* jiggle_node;

    // Jiggle state tracking
    bool started_jiggle = false;
    bool is_jiggling = false;
    bool is_jiggling_finished = false;
    bool rejiggled = false;
    float current_jiggle_power = 1.0f;
    double jiggle_time = 0.0;
    float target_jiggle_power = 1.2f;
    float power_multiplier = 1.0f;
    float eased_power_progress = 0.0f;
    float jiggle_tilt = 6.0f;
    float jiggle_frequency = 16.0f;
    float jiggle_decelerate = 1.5f;
    float additional_speed = 1.0f;
    float additional_tilt = 0.0f;
    float constant_jiggle = 0.0f;
    int random_level = 1;
    Vector3 scale_axes = Vector3(1,1,1);
    Vector3 rotation_axes = Vector3(1,0,1);
    Vector3 initial_rotation = Vector3(0,0,0);
    Vector3 initial_scale = Vector3(1,1,1);
    float rejiggle_power = 2.0f;
    float rejiggle_value = 0.0f;
    float rejiggle_progress = 0.0f;
    float rejiggle_random_offset = 0.0f;

    TypedArray<Vector4> trig_values;
    Vector4 get_random_trig_value();

    void calculate_jiggle(float p_delta);
    void emitted_finished();
    float ease_in_out_cubic(float start, float end, float t);

    void randomize_trigs(float p_trans = 1.0);
    void calculate_trig_values(float timeMultiplier = 1.0f);

    YTweenJiggle() : YTweenWrap() {
        tween_list_id = 0;
    }
    explicit YTweenJiggle(SceneTree *p_parent_tree) : YTweenWrap(p_parent_tree) {
        tween_list_id = 0;
    }
    Callable get_jiggle_callable();

};

class YTween : public RefCounted {
    GDCLASS(YTween, RefCounted);

	_THREAD_SAFE_CLASS_
    
protected:
    static YTween* singleton;
    static void _bind_methods();

public:

    List<Ref<YTweenWrap>> tweens;
    static YTween *get_singleton();
    HashMap<uint64_t,Vector<Ref<YTweenWrap>>> tween_finder;
    void do_process(double delta);
    void do_physics_process(double delta);

    void kill_specific_tween(Ref<YTweenWrap> specific_tween);

    void kill_tweens(Node* p_owner, uint64_t p_tag = 0);

    Ref<YTweenWrap> tween_scale_unique(Node *p_owner, float desired_size = 1.0, float desired_duration = 0.25, Tween::EaseType ease_type = Tween::EASE_IN_OUT, Tween::TransitionType trans_type = Tween::TRANS_QUAD, float
                           desired_delay = 0.0, uint64_t p_tag = 0);

    Ref<YTweenWrap> tween_scale(Node *p_owner, float desired_size = 1.0, float desired_duration = 0.25, Tween::EaseType ease_type = Tween::EASE_IN_OUT, Tween::TransitionType trans_type = Tween::TRANS_QUAD, float
                           desired_delay = 0.0, uint64_t p_tag = 0);

    Ref<YTweenWrap> tween_alpha_unique(Node *p_owner, float desired_alpha = 1.0, float desired_duration = 0.25, Tween::EaseType ease_type = Tween::EASE_IN_OUT, Tween::TransitionType trans_type = Tween::TRANS_QUAD, float
                           desired_delay = 0.0, uint64_t p_tag = 0);

    Ref<YTweenWrap> tween_alpha(Node *p_owner, float desired_alpha = 1.0, float desired_duration = 0.25, Tween::EaseType ease_type = Tween::EASE_IN_OUT, Tween::TransitionType trans_type = Tween::TRANS_QUAD, float
                           desired_delay = 0.0, uint64_t p_tag = 0);

    Ref<YTweenWrap> tween_global_position_unique(Node *p_owner, Variant desired_pos, float desired_duration,
                                                 Tween::EaseType ease_type, Tween::TransitionType trans_type,
                                                 float desired_delay, uint64_t p_tag);

    Ref<YTweenWrap> tween_position_unique(Node *p_owner, Variant desired_pos, float desired_duration,
                                          Tween::EaseType ease_type, Tween::TransitionType trans_type,
                                          float desired_delay,
                                          uint64_t p_tag);

    Ref<YTweenWrap> tween_position_rotation_unique(Node *p_owner, Variant desired_pos, Variant desired_rotation,
                                                   float desired_duration, Tween::EaseType ease_type,
                                                   Tween::TransitionType trans_type, float desired_delay,
                                                   uint64_t p_tag);

    Ref<YTweenWrap> tween_position_rotation_scale_unique(Node *p_owner, Variant desired_pos, Variant desired_rotation,
                                                         Variant desired_scale, float desired_duration,
                                                         Tween::EaseType ease_type, Tween::TransitionType trans_type,
                                                         float desired_delay, uint64_t p_tag);

    Ref<YTweenWrap> tween_modulate_unique(Node *p_owner, Color desired_color = Color(1.0,1.0,1.0,1.0), float desired_duration = 0.25, Tween::EaseType ease_type = Tween::EASE_IN_OUT, Tween::TransitionType trans_type = Tween::TRANS_QUAD, float
                                          desired_delay = 0.0, uint64_t p_tag = 0);

    Ref<YTweenWrap> tween_modulate(Node *p_owner, Color desired_color = Color(1.0,1.0,1.0,1.0), float desired_duration = 0.25, Tween::EaseType ease_type = Tween::EASE_IN_OUT, Tween::TransitionType trans_type = Tween::TRANS_QUAD, float
                           desired_delay = 0.0, uint64_t p_tag = 0);

    Ref<YTweenWrap> find_tween(Node *p_owner, uint64_t p_tag);

    // Helper function for jiggle effect
    Ref<YTweenWrap> tween_jiggle(Node *p_owner, float jiggle_power = 1.0f, 
                    float jiggle_tilt = 6.0f, float jiggle_frequency = 16.0f, 
                    Vector3 scale_axes = Vector3(1,1,1), Vector3 rotation_axes = Vector3(1,0,1),
                    float jiggle_decelerate = 1.5f, float additional_speed = 1.0f,
                    float constant_jiggle = 0.0f, int random_level = 1,
                    float rejiggle_power = 2.0f, uint64_t p_tag = 55);

    Ref<YTweenWrap> create_unique_tween(Node* p_owner, uint64_t p_tag = 0);
    Ref<YTweenWrap> create_tween(Node* p_owner, uint64_t p_tag = 0);
    Ref<YTweenWrap> create_tween_from(Node* p_owner, SceneTree* p_tree, Ref<YTweenWrap> p_tween, uint64_t p_tag = 0);

    void process_tweens(double p_delta, bool p_physics);

    static double last_delta_time;

    YTween();
    ~YTween();
};



#endif //YTWEEN_H
