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

class Tween;
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

    void kill_due_to_node_tree_exiting();
    void register_finished_extra_callback();

    void register_node_kill_when(Node *p_node_owner);
    void emitted_finished();

    YTweenWrap() : Tween() {
        tween_list_id = 0;
    }
    explicit YTweenWrap(bool valid) : Tween(valid) {
        tween_list_id = 0;
    }
};

class YTween : public RefCounted {
    GDCLASS(YTween, RefCounted);

protected:
    static Ref<YTween> singleton;
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

    Ref<YTweenWrap> create_unique_tween(Node* p_owner, uint64_t p_tag = 0);
    Ref<YTweenWrap> create_tween(Node* p_owner, uint64_t p_tag = 0);

    void process_tweens(double p_delta, bool p_physics);

    static void set_singleton(const Ref<YTween> &ref) {
        singleton = ref;
    }

    YTween();
    ~YTween();
};



#endif //YTWEEN_H
