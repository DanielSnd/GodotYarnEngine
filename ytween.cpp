//
// Created by Daniel on 2024-03-21.
//

#include "ytween.h"

Ref<YTween> YTween::singleton;

void YTweenWrap::_bind_methods() {
    ADD_SIGNAL(MethodInfo("finished_or_killed"));
}

void YTweenWrap::emitted_finished() {
    if (!emitted_finished_or_killed) {
        emitted_finished_or_killed=true;
        emit_signal("finished_or_killed");
    }
}

void YTween::_bind_methods() {
    ClassDB::bind_method(D_METHOD("kill_tweens","owner","tag"), &YTween::kill_tweens,DEFVAL(0));
    ClassDB::bind_method(D_METHOD("create_unique_tween","owner","tag"), &YTween::create_unique_tween,DEFVAL(0));
    ClassDB::bind_method(D_METHOD("create_tween","owner","tag"), &YTween::create_tween,DEFVAL(0));

    ClassDB::bind_method(D_METHOD("tween_scale_unique","owner","scale","duration","ease","transition","delay","tag"), &YTween::tween_scale_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_scale","owner","scale","duration","ease","transition","delay","tag"), &YTween::tween_scale,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_alpha_unique","owner","alpha","duration","ease","transition","delay","tag"), &YTween::tween_alpha_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_alpha","owner","alpha","duration","ease","transition","delay","tag"), &YTween::tween_alpha,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_modulate_unique","owner","color","duration","ease","transition","delay","tag"), &YTween::tween_modulate_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_modulate","owner","color","duration","ease","transition","delay","tag"), &YTween::tween_modulate,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));

    ClassDB::bind_method(D_METHOD("tween_global_position_unique","owner","global_position","duration","ease","transition","delay","tag"), &YTween::tween_global_position_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_position_unique","owner","position","duration","ease","transition","delay","tag"), &YTween::tween_position_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_position_rotation_unique","owner","position","rotation","duration","ease","transition","delay","tag"), &YTween::tween_position_rotation_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_position_rotation_scale_unique","owner","position","rotation","scale","duration","ease","transition","delay","tag"), &YTween::tween_position_rotation_scale_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
}

YTween * YTween::get_singleton() {
    return *singleton;
}

void YTween::do_process(double delta) {
    process_tweens(delta,false);
}

void YTween::do_physics_process(double delta) {
    process_tweens(delta,true);
}

void YTween::kill_tweens(Node *p_owner, uint64_t p_tag) {
    uint64_t desired_key = p_tag;
    if (p_owner != nullptr) {
        uint64_t node_id = p_owner->get_instance_id();
        node_id += p_tag;
    }
    if (tween_finder.has(desired_key)) {
        for (const auto& tween_wrap: tween_finder[desired_key]) {
            if (tween_wrap.is_valid()) {
                tween_wrap->emitted_finished();
                tween_wrap->kill();
                tween_wrap->clear();
                tweens.erase(tween_wrap);
            }
        }
        tween_finder.erase(desired_key);
    }
}

Ref<YTweenWrap> YTween::tween_scale_unique(Node *p_owner, float desired_size,float desired_duration,Tween::EaseType ease_type,Tween::TransitionType trans_type,float desired_delay, uint64_t p_tag) {
    auto created_tween = create_unique_tween(p_owner,p_tag);
    auto *item3d = Object::cast_to<Node3D>(p_owner);
    if (item3d != nullptr) {
        created_tween->tween_property(p_owner,NodePath("scale"),Vector3(1.0,1.0,1.0) * desired_size,desired_duration)->set_ease(ease_type)->set_trans(trans_type)->set_delay(desired_delay);
    } else {
        created_tween->tween_property(p_owner,NodePath("scale"),Vector2(1.0,1.0) * desired_size ,desired_duration)->set_ease(ease_type)->set_trans(trans_type)->set_delay(desired_delay);
    }
    return created_tween;
}

Ref<YTweenWrap> YTween::tween_scale(Node *p_owner, float desired_size,float desired_duration,Tween::EaseType ease_type,Tween::TransitionType trans_type,float desired_delay, uint64_t p_tag) {
    auto created_tween = create_tween(p_owner,p_tag);
    auto *item3d = Object::cast_to<Node3D>(p_owner);
    if (item3d != nullptr) {
        created_tween->tween_property(p_owner,NodePath("scale"),Vector3(1.0,1.0,1.0) * desired_size,desired_duration)->set_ease(ease_type)->set_trans(trans_type)->set_delay(desired_delay);
    } else {
        created_tween->tween_property(p_owner,NodePath("scale"),Vector2(1.0,1.0) * desired_size ,desired_duration)->set_ease(ease_type)->set_trans(trans_type)->set_delay(desired_delay);
    }
    return created_tween;
}

Ref<YTweenWrap> YTween::tween_alpha_unique(Node *p_owner, float desired_size,float desired_duration,Tween::EaseType ease_type,Tween::TransitionType trans_type,float desired_delay, uint64_t p_tag) {
    auto created_tween = create_unique_tween(p_owner,p_tag);
    auto *control_item = Object::cast_to<Control>(p_owner);
    auto *node2d_item = Object::cast_to<Node2D>(p_owner);
    Color starting_color = Color(1.0,1.0,1.0,1.0);
    if (control_item != nullptr)
        starting_color = control_item->get_modulate();
    else if (node2d_item != nullptr)
        starting_color = node2d_item->get_modulate();
    starting_color.a = desired_size;
    created_tween->tween_property(p_owner,NodePath("modulate"),starting_color,desired_duration)->set_ease(ease_type)->set_trans(trans_type)->set_delay(desired_delay);
    return created_tween;
}

Ref<YTweenWrap> YTween::tween_alpha(Node *p_owner, float desired_size,float desired_duration,Tween::EaseType ease_type,Tween::TransitionType trans_type,float desired_delay, uint64_t p_tag) {
    auto created_tween = create_tween(p_owner,p_tag);
    auto *control_item = Object::cast_to<Control>(p_owner);
    auto *node2d_item = Object::cast_to<Node2D>(p_owner);
    Color starting_color = Color(1.0,1.0,1.0,1.0);
    if (control_item != nullptr)
        starting_color = control_item->get_modulate();
    else if (node2d_item != nullptr)
        starting_color = node2d_item->get_modulate();
    starting_color.a = desired_size;
    created_tween->tween_property(p_owner,NodePath("modulate"),starting_color,desired_duration)->set_ease(ease_type)->set_trans(trans_type)->set_delay(desired_delay);
    return created_tween;
}

Ref<YTweenWrap> YTween::tween_global_position_unique(Node *p_owner, Variant desired_pos,float desired_duration,Tween::EaseType ease_type,Tween::TransitionType trans_type,float desired_delay, uint64_t p_tag) {
    auto created_tween = create_unique_tween(p_owner,p_tag);
    created_tween->tween_property(p_owner,NodePath("global_position"),desired_pos,desired_duration)->set_ease(ease_type)->set_trans(trans_type)->set_delay(desired_delay);
    return created_tween;
}

Ref<YTweenWrap> YTween::tween_position_unique(Node *p_owner, Variant desired_pos,float desired_duration,Tween::EaseType ease_type,Tween::TransitionType trans_type,float desired_delay, uint64_t p_tag) {
    auto created_tween = create_unique_tween(p_owner,p_tag);
    created_tween->tween_property(p_owner,NodePath("position"),desired_pos,desired_duration)->set_ease(ease_type)->set_trans(trans_type)->set_delay(desired_delay);
    return created_tween;
}

Ref<YTweenWrap> YTween::tween_position_rotation_unique(Node *p_owner, Variant desired_pos, Variant desired_rotation,float desired_duration,Tween::EaseType ease_type,Tween::TransitionType trans_type,float desired_delay, uint64_t p_tag) {
    auto created_tween = create_unique_tween(p_owner,p_tag);
    created_tween->set_parallel(true);
    created_tween->tween_property(p_owner,NodePath("position"),desired_pos,desired_duration)->set_ease(ease_type)->set_trans(trans_type)->set_delay(desired_delay);
    created_tween->tween_property(p_owner,NodePath("rotation"),desired_rotation,desired_duration)->set_ease(ease_type)->set_trans(trans_type)->set_delay(desired_delay);
    return created_tween;
}

Ref<YTweenWrap> YTween::tween_position_rotation_scale_unique(Node *p_owner, Variant desired_pos, Variant desired_rotation, Variant desired_scale,float desired_duration,Tween::EaseType ease_type,Tween::TransitionType trans_type,float desired_delay, uint64_t p_tag) {
    auto created_tween = create_unique_tween(p_owner,p_tag);
    created_tween->set_parallel(true);
    created_tween->tween_property(p_owner,NodePath("position"),desired_pos,desired_duration)->set_ease(ease_type)->set_trans(trans_type)->set_delay(desired_delay);
    created_tween->tween_property(p_owner,NodePath("rotation"),desired_rotation,desired_duration)->set_ease(ease_type)->set_trans(trans_type)->set_delay(desired_delay);
    created_tween->tween_property(p_owner,NodePath("scale"),desired_scale,desired_duration)->set_ease(ease_type)->set_trans(trans_type)->set_delay(desired_delay);
    return created_tween;
}
Ref<YTweenWrap> YTween::tween_modulate_unique(Node *p_owner, Color desired_size,float desired_duration,Tween::EaseType ease_type,Tween::TransitionType trans_type,float desired_delay, uint64_t p_tag) {
    auto created_tween = create_unique_tween(p_owner,p_tag);
    created_tween->tween_property(p_owner,NodePath("modulate"),desired_size,desired_duration)->set_ease(ease_type)->set_trans(trans_type)->set_delay(desired_delay);
    return created_tween;
}
Ref<YTweenWrap> YTween::tween_modulate(Node *p_owner, Color desired_size,float desired_duration,Tween::EaseType ease_type,Tween::TransitionType trans_type,float desired_delay, uint64_t p_tag) {
    auto created_tween = create_tween(p_owner,p_tag);
    created_tween->tween_property(p_owner,NodePath("modulate"),desired_size,desired_duration)->set_ease(ease_type)->set_trans(trans_type)->set_delay(desired_delay);
    return created_tween;
}

Ref<YTweenWrap> YTween::create_unique_tween(Node *node_owner, uint64_t tag) {
    kill_tweens(node_owner,tag);
    return create_tween(node_owner,tag);
}

Ref<YTweenWrap> YTween::create_tween(Node* node_owner, uint64_t tag) {
    uint64_t desired_key = tag;
    if (node_owner != nullptr) {
        uint64_t node_id = node_owner->get_instance_id();
        node_id += tag;
    }
    Ref<YTweenWrap> tween = memnew(YTweenWrap(true));
    tween->connect("finished",callable_mp(tween.ptr(),&YTweenWrap::emitted_finished));
    tween->tween_list_id = desired_key;
    if (!tween_finder.has(desired_key)) {
        Vector<Ref<YTweenWrap>> new_list;
        tween_finder[desired_key] = new_list;
        new_list.append(tween);
    } else {
        tween_finder[desired_key].append(tween);
    }

    tweens.push_back(tween);
    return tween;
}

void YTween::process_tweens(double p_delta, bool p_physics) {
    // This methods works similarly to how SceneTreeTimers are handled.
    List<Ref<YTweenWrap>>::Element *L = tweens.back();
    bool paused = YTime::get_singleton()->is_paused;
    for (List<Ref<YTweenWrap>>::Element *E = tweens.front(); E;) {
        List<Ref<YTweenWrap>>::Element *N = E->next();
        // Don't process if paused or process mode doesn't match.
        if (!E->get()->can_process(paused) || (p_physics == (E->get()->get_process_mode() == Tween::TWEEN_PROCESS_IDLE))) {
            if (E == L) {
                break;
            }
            E = N;
            continue;
        }

        if (!E->get()->step(p_delta)) {
            if (!tween_finder.has(E->get()->tween_list_id)) {
                if (tween_finder[E->get()->tween_list_id].size() <= 1)
                    tween_finder.erase(E->get()->tween_list_id);
                else
                    tween_finder[E->get()->tween_list_id].erase(E->get());
            }
            E->get()->clear();
            tweens.erase(E);
        }
        if (E == L) {
            break;
        }
        E = N;
    }
}

YTween::YTween() {
}

YTween::~YTween() {
    tween_finder.clear();
    for (Ref<YTweenWrap> &tween : tweens) {
        tween->clear();
    }
    tweens.clear();
}
