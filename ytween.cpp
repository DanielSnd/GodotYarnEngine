//
// Created by Daniel on 2024-03-21.
//

#include "ytween.h"

YTween* YTween::singleton = nullptr;

void YTweenWrap::_bind_methods() {
    
    ClassDB::bind_method(D_METHOD("yparallel"), &YTweenWrap::yparallel);
    ClassDB::bind_method(D_METHOD("ychain"), &YTweenWrap::ychain);
    
    ClassDB::bind_method(D_METHOD("set_ytrans", "trans", "param1", "param2"), &YTweenWrap::set_ytrans, DEFVAL(INFINITY), DEFVAL(INFINITY));
    ClassDB::bind_method(D_METHOD   ("set_ytrans_in_property", "property", "trans", "param1", "param2"), &YTweenWrap::set_ytrans_in_property, DEFVAL(INFINITY), DEFVAL(INFINITY));
    ClassDB::bind_method(D_METHOD("ytween_property", "object", "property", "final_val", "duration","ease", "trans", "param1", "param2"), &YTweenWrap::ytween_property, DEFVAL(Tween::EASE_IN_OUT), DEFVAL(Tween::TRANS_LINEAR), DEFVAL(INFINITY), DEFVAL(INFINITY));
    ADD_SIGNAL(MethodInfo("finished_or_killed"));
}

Ref<YTweenWrap> YTweenWrap::ychain() {
    chain();
    return this;
}

Ref<YTweenWrap> YTweenWrap::yparallel() {
    parallel();
    return this;
}

void YTweenWrap::register_finished_extra_callback() {
    connect("finished",callable_mp(this,&YTweenWrap::emitted_finished));
}

void YTweenWrap::register_node_kill_when(Node* p_node_owner) {
    if (!is_valid() || p_node_owner == nullptr || !p_node_owner->is_inside_tree()) {
        return;
    }

    p_node_owner->connect("tree_exiting", callable_mp(this,&YTweenWrap::kill_due_to_node_tree_exiting));
}

Ref<PropertyTweener> YTweenWrap::set_ytrans(Tween::TransitionType p_trans, real_t p_param1, real_t p_param2) {
#ifdef YGODOT
    return set_trans(p_trans,p_param1, p_param2);
#else
    return set_trans(p_trans);
#endif
}

Ref<PropertyTweener> YTweenWrap::set_ytrans_in_property(Ref<PropertyTweener> tweenproperty, Tween::TransitionType p_trans, real_t p_param1, real_t p_param2) {
    if (!tweenproperty.is_valid())
        return tweenproperty;
#ifdef YGODOT
    return tweenproperty->set_trans(p_trans,p_param1, p_param2);
#else
    return tweenproperty->set_trans(p_trans);
#endif
}

void YTweenWrap::kill_due_to_node_tree_exiting() {
    if (!emitted_finished_or_killed) {
        YTween* ytween_singleton = YTween::get_singleton();
        kill();
        clear();
        if (ytween_singleton == nullptr) {
            emitted_finished_or_killed = true;
            return;
        }
        ytween_singleton->kill_specific_tween(this);
    }
}
void YTweenWrap::emitted_finished() {
    if (!emitted_finished_or_killed) {
        emitted_finished_or_killed=true;
        //print_line("Emit finish signal");
        emit_signal("finished_or_killed");
    }
}

Ref<PropertyTweener> YTweenWrap::ytween_property(const Object *p_target, const NodePath &p_property, Variant p_to, double p_duration, Tween::EaseType p_ease, Tween::TransitionType p_trans, real_t p_param1, real_t p_param2) {
    if (!is_valid() || p_target == nullptr || p_property.is_empty() || p_duration < 0) {
        ERR_PRINT("Invalid parameters for ytween_property");
        return Ref<PropertyTweener>();
    }
    
    Ref<PropertyTweener> new_tween = tween_property(p_target, p_property, p_to, p_duration);
    if (new_tween.is_valid()) {
        new_tween->set_ease(p_ease);
#ifdef YGODOT
        new_tween->set_trans(p_trans,p_param1, p_param2);
#else
        new_tween->set_trans(p_trans);
#endif
    }
    return new_tween;
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
    return singleton;
}

void YTween::do_process(double delta) {
    process_tweens(delta,false);
}

void YTween::do_physics_process(double delta) {
    process_tweens(delta,true);
}

void YTween::kill_specific_tween(Ref<YTweenWrap> specific_tween) {
    if (specific_tween.is_null() || !specific_tween.is_valid()) {
        return;
    }
    
    // Use local copy to avoid potential reference issues
    uint64_t desired_list_id = specific_tween->tween_list_id;
    // First remove from tween_finder to prevent reentrance issues
    if (tween_finder.has(desired_list_id)) {
        if (tween_finder[desired_list_id].size() <= 1) {
            tween_finder.erase(desired_list_id);
        } else {
            // Safe erase that checks if the item exists in the vector
            Vector<Ref<YTweenWrap>> &tween_list = tween_finder[desired_list_id];
            int idx = tween_list.find(specific_tween);
            if (idx >= 0) {
                tween_list.remove_at(idx);
            }
        }
    }
    
    // Now handle the tween itself
    if (specific_tween.is_valid()) {
        specific_tween->kill();
        specific_tween->clear();
        
        if (!specific_tween->emitted_finished_or_killed && specific_tween.is_valid()) {
            specific_tween->emitted_finished();
        }
    }
    
    // Finally remove from the main list
    tweens.erase(specific_tween);
}

void YTween::kill_tweens(Node *p_owner, uint64_t p_tag) {
     uint64_t desired_key = p_tag;
    
    // If owner is valid, include its ID in the key
    if (p_owner != nullptr && p_owner->is_inside_tree()) {
        uint64_t node_id = p_owner->get_instance_id();
        desired_key += node_id;
    }
    
    if (!tween_finder.has(desired_key)) {
        return;
    }
    
    // Make a copy of the list to avoid modification during iteration
    Vector<Ref<YTweenWrap>> tweens_to_kill = tween_finder[desired_key];
    
    // Remove the entry from tween_finder first to prevent reentrance issues
    tween_finder.erase(desired_key);
    
    // Now safely process each tween
    for (int i = 0; i < tweens_to_kill.size(); i++) {
        Ref<YTweenWrap> tween_wrap = tweens_to_kill[i];
        
        if (tween_wrap.is_null() || !tween_wrap.is_valid()) {
            continue;
        }
        
        if (!tween_wrap->emitted_finished_or_killed) {
            tween_wrap->emitted_finished();
        }
        
        tween_wrap->kill();
        tweens.erase(tween_wrap);
        tween_wrap->clear();
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
    //print_line("Create tween with tag ",tag);
    SceneTree *tree = SceneTree::get_singleton();
    if (tree == nullptr) {
        Ref<YTweenWrap> new_wrap;
        return new_wrap;
    }

    // Create the tween
    Ref<YTweenWrap> tween = memnew(YTweenWrap(tree));
    if (tween.is_null()) {
        ERR_PRINT("Failed to create YTweenWrap");
        return Ref<YTweenWrap>();
    }
    
    if (node_owner != nullptr && node_owner->is_inside_tree()) {
        uint64_t node_id = node_owner->get_instance_id();
        desired_key += node_id;
        tween->bind_node(node_owner);
        tween->register_node_kill_when(node_owner);
    }
    tween->register_finished_extra_callback();
    tween->tween_list_id = desired_key;
    //print_line("Creating a new tween with desired key ",desired_key," does tweenfinder have? ",tween_finder.has(desired_key));
    if (!tween_finder.has(desired_key)) {
        Vector<Ref<YTweenWrap>> new_list;
        new_list.push_back(tween);
        tween_finder[desired_key] = new_list;
    } else {
        tween_finder[desired_key].push_back(tween);
    }
    tweens.push_back(tween);
    //print_line("Created a new tween with desired key ",desired_key);
    return tween;
}

// void SceneTree::process_tweens(double p_delta, bool p_physics) {
//     _THREAD_SAFE_METHOD_
//     // This methods works similarly to how SceneTreeTimers are handled.
//     List<Ref<Tween>>::Element *L = tweens.back();
//
//     for (List<Ref<Tween>>::Element *E = tweens.front(); E;) {
//         List<Ref<Tween>>::Element *N = E->next();
//         // Don't process if paused or process mode doesn't match.
//         if (!E->get()->can_process(paused) || (p_physics == (E->get()->get_process_mode() == Tween::TWEEN_PROCESS_IDLE))) {
//             if (E == L) {
//                 break;
//             }
//             E = N;
//             continue;
//         }
//
//         if (!E->get()->step(p_delta)) {
//             E->get()->clear();
//             tweens.erase(E);
//         }
//         if (E == L) {
//             break;
//         }
//         E = N;
//     }
// }

void YTween::process_tweens(double p_delta, bool p_physics) {
    _THREAD_SAFE_METHOD_

    if (tweens.size() == 0) {
        return;  // No tweens to process
    }
    
    // Store the last element for loop termination check
    List<Ref<YTweenWrap>>::Element *L = tweens.back();
    
	const double unscaled_delta = Engine::get_singleton()->get_process_step();

    // Create a list of tweens to be removed to avoid modifying while iterating
    List<Ref<YTweenWrap>> tweens_to_remove;
    
    bool paused = YTime::get_singleton()->is_paused;
    for (List<Ref<YTweenWrap>>::Element *E = tweens.front(); E;) {
        List<Ref<YTweenWrap>>::Element *N = E->next();
		Ref<YTweenWrap> &tween = E->get();

        // Skip invalid tweens
        if (tween.is_null() || !tween.is_valid()) {
            tweens_to_remove.push_back(tween);
            if (E == L) {
                break;
            }
            E = N;
            continue;
        }

        // Don't process if paused or process mode doesn't match.
        //
        if (!tween->can_process(paused) || p_physics == (tween->get_process_mode() == Tween::TWEEN_PROCESS_IDLE)) {
            if (E == L) {
                break;
            }
            E = N;
            continue;
        }

        if (!tween->step(tween->is_ignoring_time_scale() ? unscaled_delta : p_delta)) {
            if (tween.is_valid()) {
                uint64_t _desired_list_id = tween->tween_list_id;
                // Find and remove this specific tween
                Vector<Ref<YTweenWrap>> &tween_list = tween_finder[_desired_list_id];
                int idx = tween_list.find(tween);
                if (idx >= 0) {
                    tween_list.remove_at(idx);
                }
                if (tween_finder.has(_desired_list_id)) {
                    if (tween_finder[_desired_list_id].size() <= 1)
                        tween_finder.erase(_desired_list_id);
                    else
                        tween_finder[_desired_list_id].erase(E->get());
                }
            }
            
            // Mark for removal
            tweens_to_remove.push_back(tween);

            if (tween.is_valid()) {
                tween->clear();
            }
        }
        if (E == L) {
            break;
        }
        E = N;
    }
    
    // Now safely remove the tweens that are done
    for (List<Ref<YTweenWrap>>::Element *E = tweens_to_remove.front(); E; E = E->next()) {
        tweens.erase(E->get());
    }
}

YTween::YTween() {
    singleton = this;
}

YTween::~YTween() {
    tween_finder.clear();
    for (Ref<YTweenWrap> &tween : tweens) {
        tween->clear();
    }
    tweens.clear();
    if (singleton != nullptr && singleton == this) {
        singleton = nullptr;
    }
}
