//
// Created by Daniel on 2024-03-21.
//

#ifndef YTWEEN_H
#define YTWEEN_H
#include "yarntime.h"
#include "scene/3d/node_3d.h"
#include "core/object/ref_counted.h"
#include "scene/animation/tween.h"
#include "scene/gui/control.h"
#include "scene/2d/node_2d.h"
#include "core/os/mutex.h"
#include "core/os/thread_safe.h"
#include "scene/scene_string_names.h"

#define Math_PI 3.1415926535897932384626433833

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

class JiggleTweener : public Tweener {
    GDCLASS(JiggleTweener, Tweener);

public:
#ifdef YGODOT
    Ref<JiggleTweener> set_trans(Tween::TransitionType p_trans, real_t p_param1 = INFINITY, real_t p_param2 = INFINITY);
#else
    Ref<JiggleTweener> set_trans(Tween::TransitionType p_trans);
#endif
    Ref<JiggleTweener> set_ease(Tween::EaseType p_ease);
    Ref<JiggleTweener> set_delay(double p_delay);

    Node3D* jiggle_node;
    Node2D* jiggle_node_2d;
    Control* jiggle_node_control;

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

    float ease_in_out_cubic(float start, float end, float t);

    void randomize_trigs(float p_trans = 1.0);
    void calculate_trig_values(float p_delta_time, float timeMultiplier = 1.0f);

    void set_tween(const Ref<Tween> &p_tween) override;
    bool step(double &r_delta) override;
	void start() override;

    JiggleTweener(Node *p_owner, float jiggle_power = 1.0f, 
                    float jiggle_tilt = 6.0f, float jiggle_frequency = 16.0f, 
                    Vector3 scale_axes = Vector3(1,1,1), Vector3 rotation_axes = Vector3(1,0,1),
                    float jiggle_decelerate = 1.5f, float additional_speed = 1.0f,
                    float constant_jiggle = 0.0f, int random_level = 1,
                    float rejiggle_power = 2.0f, uint64_t p_tag = 55);
    JiggleTweener();

protected:
    static void _bind_methods();

private:
    double duration = 0;
    double delay = 0;
    Tween::TransitionType trans_type = Tween::TRANS_MAX;
    Tween::EaseType ease_type = Tween::EASE_MAX;
    real_t trans_params[Tween::PARAM_COUNT] = { INFINITY, INFINITY };

    Ref<RefCounted> ref_copy;
};

class YTweenJiggle : public YTweenWrap {
    GDCLASS(YTweenJiggle, YTweenWrap);

public:
    Ref<JiggleTweener> jiggle_tweener;

    YTweenJiggle() : YTweenWrap() {
        tween_list_id = 0;
    }
    explicit YTweenJiggle(SceneTree *p_parent_tree) : YTweenWrap(p_parent_tree) {
        tween_list_id = 0;
    }
};

class YTween : public RefCounted {
    GDCLASS(YTween, RefCounted);

	_THREAD_SAFE_CLASS_
    
protected:
    inline static YTween* singleton = nullptr;
    static void _bind_methods();

public:

    List<Ref<YTweenWrap>> tweens;
    List<Ref<YTweenWrap>> tweens_to_unref_next_frame;
    _FORCE_INLINE_ static YTween *get_singleton() {return singleton;}
    HashMap<uint64_t,Vector<Ref<YTweenWrap>>> tween_finder;
    void do_process(double delta);
    void do_physics_process(double delta);

    void kill_specific_tween(Ref<YTweenWrap> specific_tween);

    void kill_tweens(Node* p_owner, uint64_t p_tag = 0);

    Ref<YTweenWrap> tween_scale_unique(Node *p_owner, float desired_size = 1.0, float desired_duration = 0.25, Tween::EaseType ease_type = Tween::EASE_IN_OUT, Tween::TransitionType trans_type = Tween::TRANS_QUAD, float
                           desired_delay = 0.0, uint64_t p_tag = 0);

    Ref<YTweenWrap> tween_scale(Node *p_owner, float desired_size = 1.0, float desired_duration = 0.25, Tween::EaseType ease_type = Tween::EASE_IN_OUT, Tween::TransitionType trans_type = Tween::TRANS_QUAD, float
                           desired_delay = 0.0, uint64_t p_tag = 0);

    Ref<YTweenWrap> tween_scale_to_and_then(Node *p_owner, float desired_to_size = 1.0, float desired_then_size = 1.0, float desired_duration_to = 0.65, float desired_duration_then = 0.25, Tween::EaseType ease_type_to = Tween::EASE_IN, Tween::EaseType ease_type_then = Tween::EASE_OUT, Tween::TransitionType trans_type = Tween::TRANS_QUAD, float trans_amount = 1.68, float
                           desired_delay_to = 0.0, float desired_delay_then = 0.0, uint64_t p_tag = 0);

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

    Ref<YTweenWrap> tween_jiggle_2d(Node *p_owner, float jiggle_power = 1.0f, 
                    float jiggle_tilt = 6.0f, float jiggle_frequency = 16.0f, 
                    Vector2 scale_axes = Vector2(1,1),
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
