//
// Created by Daniel on 2024-03-21.
//

#include "ytween.h"

double YTween::last_delta_time = 0.0;

void JiggleTweener::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_trans", "trans", "param1", "param2"), &JiggleTweener::set_trans, DEFVAL(INFINITY), DEFVAL(INFINITY));
    ClassDB::bind_method(D_METHOD("set_ease", "ease"), &JiggleTweener::set_ease);
    ClassDB::bind_method(D_METHOD("set_delay", "delay"), &JiggleTweener::set_delay);
}

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
    connect("finished",callable_mp(this,&YTweenWrap::emitted_finished), CONNECT_DEFERRED);
}

void YTweenWrap::register_node_kill_when(Node* p_node_owner) {
    if (!is_valid() || p_node_owner == nullptr || !p_node_owner->is_inside_tree()) {
        return;
    }

    p_node_owner->connect(SceneStringName(tree_exiting), callable_mp(this,&YTweenWrap::kill_due_to_node_tree_exiting), CONNECT_DEFERRED);
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

    ClassDB::bind_method(D_METHOD("tween_scale_to_and_then","owner","to_scale","then_scale","duration_to","duration_then","ease_type_to","ease_type_then","transition","trans_amount","delay_to","delay_then","tag"), &YTween::tween_scale_to_and_then,DEFVAL(1.25),DEFVAL(1.0),DEFVAL(0.65),DEFVAL(0.25),DEFVAL(Tween::EaseType::EASE_IN),DEFVAL(Tween::EaseType::EASE_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(1.68),DEFVAL(0.0),DEFVAL(0.0),DEFVAL(0));
    
    ClassDB::bind_method(D_METHOD("tween_scale_unique","owner","scale","duration","ease","transition","delay","tag"), &YTween::tween_scale_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_scale","owner","scale","duration","ease","transition","delay","tag"), &YTween::tween_scale,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_alpha_unique","owner","alpha","duration","ease","transition","delay","tag"), &YTween::tween_alpha_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_alpha","owner","alpha","duration","ease","transition","delay","tag"), &YTween::tween_alpha,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_modulate_unique","owner","color","duration","ease","transition","delay","tag"), &YTween::tween_modulate_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_modulate","owner","color","duration","ease","transition","delay","tag"), &YTween::tween_modulate,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));

    ClassDB::bind_method(D_METHOD("tween_jiggle","owner","power","tilt","frequency","scale_axis","rotation_axis","jiggle_deceleration","additional_speed","constant_jiggle","random_level","rejiggle_power","tag"), &YTween::tween_jiggle,DEFVAL(1.0f),DEFVAL(6.0f),DEFVAL(16.0f),DEFVAL(Vector3(1.0,1.0,1.0)),DEFVAL(Vector3(1.0,0.0,1.0)), DEFVAL(1.5f), DEFVAL(1.0f), DEFVAL(0.0f), DEFVAL(1),DEFVAL(2.0f),DEFVAL(55));

    ClassDB::bind_method(D_METHOD("tween_jiggle_scale","owner","power","tilt","frequency","scale_axis","rotation_axis","jiggle_deceleration","additional_speed","constant_jiggle","random_level","rejiggle_power","tag"), &YTween::tween_jiggle,DEFVAL(1.0f),DEFVAL(6.0f),DEFVAL(16.0f),DEFVAL(Vector3(1.0,1.0,1.0)),DEFVAL(Vector3(0.0,0.0,0.0)), DEFVAL(1.5f), DEFVAL(1.0f), DEFVAL(0.0f), DEFVAL(1),DEFVAL(2.0f),DEFVAL(55));

    ClassDB::bind_method(D_METHOD("tween_jiggle_tilt","owner","power","tilt","frequency","scale_axis","rotation_axis","jiggle_deceleration","additional_speed","constant_jiggle","random_level","rejiggle_power","tag"), &YTween::tween_jiggle,DEFVAL(1.0f),DEFVAL(6.0f),DEFVAL(16.0f),DEFVAL(Vector3(0.0,0.0,0.0)),DEFVAL(Vector3(1.0,0.0,1.0)), DEFVAL(1.5f), DEFVAL(1.0f), DEFVAL(0.0f), DEFVAL(1),DEFVAL(2.0f),DEFVAL(55));

    ClassDB::bind_method(D_METHOD("tween_global_position_unique","owner","global_position","duration","ease","transition","delay","tag"), &YTween::tween_global_position_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_position_unique","owner","position","duration","ease","transition","delay","tag"), &YTween::tween_position_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_position_rotation_unique","owner","position","rotation","duration","ease","transition","delay","tag"), &YTween::tween_position_rotation_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_position_rotation_scale_unique","owner","position","rotation","scale","duration","ease","transition","delay","tag"), &YTween::tween_position_rotation_scale_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
}

void YTween::do_process(double delta) {
    YTween::last_delta_time = delta;
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
        // Remove from main list first to prevent any further processing
        tweens.erase(specific_tween);
        tweens_to_unref_next_frame.push_back(specific_tween);
        
        if (specific_tween->is_running()) {
            specific_tween->stop();
            // Then kill and clear the tween
            specific_tween->kill();
        }
        
        specific_tween->clear();
        
        // Finally emit the finished signal if not already emitted
        if (!specific_tween->emitted_finished_or_killed && specific_tween.is_valid()) {
            specific_tween->emitted_finished();
        }
    }
}


Ref<YTweenWrap> YTween::find_tween(Node *p_owner, uint64_t p_tag) {
    uint64_t desired_key = p_tag;
    
    // If owner is valid, include its ID in the key
    if (p_owner != nullptr && p_owner->is_inside_tree()) {
        uint64_t node_id = p_owner->get_instance_id();
        desired_key += node_id;
    }
    
    if (!tween_finder.has(desired_key)) {
        return Ref<YTweenWrap>();
    }
    
    Vector<Ref<YTweenWrap>> &tweens_list = tween_finder[desired_key];
    
    // Return the first valid tween found
    for (int i = 0; i < tweens_list.size(); i++) {
        Ref<YTweenWrap> tween_wrap = tweens_list[i];
        if (!tween_wrap.is_null() && tween_wrap.is_valid() && tween_wrap->is_running() && !tween_wrap->emitted_finished_or_killed) {
            return tween_wrap;
        }
    }
    
    return Ref<YTweenWrap>();
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
        tweens_to_unref_next_frame.push_back(tween_wrap);
        tween_wrap->clear();
    }
}


Ref<YTweenWrap> YTween::tween_scale_to_and_then(Node *p_owner, float desired_to_size, float desired_then_size, float desired_duration_to, float desired_duration_then, Tween::EaseType ease_type_to, Tween::EaseType ease_type_then, Tween::TransitionType trans_type, float trans_amount, float desired_delay_to, float desired_delay_then, uint64_t p_tag) {
    auto created_tween = create_unique_tween(p_owner,p_tag);
    auto *item3d = Object::cast_to<Node3D>(p_owner);
    if (item3d != nullptr) {
        created_tween->tween_property(p_owner,NodePath("scale"),Vector3(1.0,1.0,1.0) * desired_to_size,desired_duration_to)->set_ease(ease_type_to)->set_trans(trans_type)->set_delay(desired_delay_to);
        created_tween->tween_property(p_owner,NodePath("scale"),Vector3(1.0,1.0,1.0) * desired_then_size,desired_duration_then)->set_ease(ease_type_then)->set_trans(trans_type)->set_delay(desired_delay_then);
    } else {
        created_tween->tween_property(p_owner,NodePath("scale"),Vector2(1.0,1.0) * desired_to_size ,desired_duration_to)->set_ease(ease_type_to)->set_trans(trans_type)->set_delay(desired_delay_to);
        created_tween->tween_property(p_owner,NodePath("scale"),Vector2(1.0,1.0) * desired_then_size,desired_duration_then)->set_ease(ease_type_then)->set_trans(trans_type)->set_delay(desired_delay_then);
    }
    return created_tween;
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
    //print_line("Create tween with tag ",tag);
    SceneTree *tree = SceneTree::get_singleton();
    if (tree == nullptr) {
        Ref<YTweenWrap> new_wrap;
        return new_wrap;
    }

    // Create the tween
    Ref<YTweenWrap> tween;
    tween.instantiate(tree);
    
    return create_tween_from(node_owner, tree, tween, tag);
}

Ref<YTweenWrap> YTween::create_tween_from(Node *node_owner, SceneTree *tree, Ref<YTweenWrap> tween, uint64_t tag)
{
    uint64_t desired_key = tag;
    //print_line("Create tween with tag ",tag);
    if (tree == nullptr || tween.is_null() || !tween.is_valid()) {
        Ref<YTweenWrap> new_wrap;
        return new_wrap;
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


void YTween::process_tweens(double p_delta, bool p_physics) {
    _THREAD_SAFE_METHOD_

    if (tweens_to_unref_next_frame.size() > 0 && !p_physics) {
        tweens_to_unref_next_frame.clear();
    }

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
        if (tween.is_null() || !tween.is_valid() || !tween->is_valid()) {
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
        tweens_to_unref_next_frame.push_back(E->get());
        tweens.erase(E->get());
    }
}

YTween::YTween() {
    singleton = this;
    last_delta_time = 0.0;
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

JiggleTweener::JiggleTweener(Node *p_owner, float p_jiggle_power, float p_jiggle_tilt, float p_jiggle_frequency,
    Vector3 p_scale_axes, Vector3 p_rotation_axes, float p_jiggle_decelerate, float p_additional_speed, float p_constant_jiggle, int p_random_level, float p_rejiggle_power, uint64_t p_tag) {
    jiggle_tilt = p_jiggle_tilt;
    jiggle_frequency = p_jiggle_frequency;
    jiggle_decelerate = p_jiggle_decelerate;
    additional_speed = p_additional_speed;
    constant_jiggle = p_constant_jiggle;
    random_level = p_random_level;
    scale_axes = p_scale_axes;
    rotation_axes = p_rotation_axes;
    power_multiplier = p_jiggle_power;
    rejiggle_power = p_rejiggle_power;
    current_jiggle_power = 1.0f;
    is_jiggling_finished = false;
    jiggle_node = Object::cast_to<Node3D>(p_owner);
    jiggle_node_2d = Object::cast_to<Node2D>(p_owner);
    jiggle_node_control = Object::cast_to<Control>(p_owner);
}

JiggleTweener::JiggleTweener() {
    jiggle_node = nullptr;
    jiggle_node_2d = nullptr;
    jiggle_node_control = nullptr;
}
// Helper function to create jiggle animation
Ref<YTweenWrap> YTween::tween_jiggle(Node *p_owner, float jiggle_power, float jiggle_tilt, float jiggle_frequency,
    Vector3 scale_axes, Vector3 rotation_axes, float jiggle_decelerate, float additional_speed, float constant_jiggle, int random_level, float rejiggle_power, uint64_t p_tag) {
    
    SceneTree *tree = SceneTree::get_singleton();
    Node3D* node3d = Object::cast_to<Node3D>(p_owner);
    if (node3d == nullptr && p_owner != nullptr) {
        node3d = Object::cast_to<Node3D>(p_owner->get_parent());
    }
    if (node3d == nullptr) {
        return tween_jiggle_2d(p_owner, jiggle_power, jiggle_tilt, jiggle_frequency, Vector2(scale_axes.x, scale_axes.y), jiggle_decelerate, additional_speed, constant_jiggle, random_level, rejiggle_power, p_tag);
    }
    if (tree == nullptr || node3d == nullptr) {
        Ref<YTweenWrap> new_wrap;
        return new_wrap;
    }

    // Check for existing tween and get its state if it exists
    Ref<YTweenJiggle> existing_tween = find_tween(p_owner, p_tag);
    bool was_jiggling = false;
    float existing_jiggle_time = 0.0f;
    Vector3 existing_initial_rotation = Vector3(0,0,0);
    Vector3 existing_initial_scale = Vector3(0,0,0);
    
    if (existing_tween.is_valid() && existing_tween->jiggle_tweener.is_valid() && existing_tween->jiggle_tweener->started_jiggle) {
        existing_jiggle_time = existing_tween->jiggle_tweener->jiggle_time;
        was_jiggling = existing_tween->jiggle_tweener->is_jiggling;
        existing_initial_rotation = existing_tween->jiggle_tweener->initial_rotation;
        existing_initial_scale = existing_tween->jiggle_tweener->initial_scale;
        // Kill the existing tween
        kill_specific_tween(existing_tween);
    }

    // Create a new tween
    Ref<YTweenJiggle> tween;
    tween.instantiate(tree);
    tween = create_tween_from(p_owner, tree, tween, p_tag);
    
    if (!tween.is_valid()) {
        return tween;
    }
    
    Ref<JiggleTweener> jiggle_tweener;
    jiggle_tweener.instantiate(p_owner, jiggle_power, jiggle_tilt, jiggle_frequency, scale_axes, rotation_axes, jiggle_decelerate, additional_speed, constant_jiggle, random_level, rejiggle_power, p_tag);
    // Store jiggle parameters
    tween->jiggle_tweener = jiggle_tweener;
    jiggle_tweener->power_multiplier = jiggle_power;
    jiggle_tweener->jiggle_tilt = jiggle_tilt;
    jiggle_tweener->jiggle_frequency = jiggle_frequency;
    jiggle_tweener->jiggle_decelerate = jiggle_decelerate;
    jiggle_tweener->additional_speed = additional_speed;
    jiggle_tweener->constant_jiggle = constant_jiggle;
    jiggle_tweener->random_level = random_level;
    jiggle_tweener->scale_axes = scale_axes;
    jiggle_tweener->rotation_axes = rotation_axes;
    jiggle_tweener->current_jiggle_power = 1.0f;
    jiggle_tweener->is_jiggling_finished = false;

    if (was_jiggling) {
        jiggle_tweener->eased_power_progress = 1.1f * jiggle_power;
        jiggle_tweener->jiggle_time = existing_jiggle_time;
        jiggle_tweener->rejiggled = true;
        jiggle_tweener->initial_rotation = existing_initial_rotation;
        jiggle_tweener->initial_scale = existing_initial_scale;
        jiggle_tweener->rejiggle_progress = 1.0f * jiggle_power;
        jiggle_tweener->randomize_trigs((rejiggle_power * jiggle_power) / 4.0f);
    } else {
        jiggle_tweener->target_jiggle_power = 1.15f * jiggle_power;
        jiggle_tweener->eased_power_progress = 1.0f;
        jiggle_tweener->current_jiggle_power = 0.15f * jiggle_power;
        jiggle_tweener->rejiggled = false;
        jiggle_tweener->randomize_trigs();
    }

    jiggle_tweener->eased_power_progress = 1.0f;
    jiggle_tweener->rejiggle_random_offset = Math::random(-0.2f, 0.2f);

    tween->append(jiggle_tweener);

    return tween;
}

Ref<YTweenWrap> YTween::tween_jiggle_2d(Node *p_owner, float jiggle_power, float jiggle_tilt, float jiggle_frequency,
    Vector2 scale_axes, float jiggle_decelerate, float additional_speed, float constant_jiggle, int random_level, float rejiggle_power, uint64_t p_tag) {
    
    SceneTree *tree = SceneTree::get_singleton();
    Node2D* node2d = Object::cast_to<Node2D>(p_owner);
    Control* control = node2d == nullptr ? Object::cast_to<Control>(p_owner) : nullptr;
    if (control == nullptr && node2d == nullptr && p_owner != nullptr) {
        node2d = Object::cast_to<Node2D>(p_owner->get_parent());
    }
    if (node2d == nullptr && control == nullptr && p_owner != nullptr) {
        control = Object::cast_to<Control>(p_owner->get_parent());
    }
    if (tree == nullptr || (node2d == nullptr && control == nullptr)) {
        Ref<YTweenWrap> new_wrap;
        return new_wrap;
    }

    // Check for existing tween and get its state if it exists
    Ref<YTweenJiggle> existing_tween = find_tween(p_owner, p_tag);
    bool was_jiggling = false;
    float existing_jiggle_time = 0.0f;
    Vector3 existing_initial_rotation = Vector3(0,0,0);
    Vector3 existing_initial_scale = Vector3(0,0,0);
    
    if (existing_tween.is_valid() && existing_tween->jiggle_tweener.is_valid() && existing_tween->jiggle_tweener->started_jiggle) {
        existing_jiggle_time = existing_tween->jiggle_tweener->jiggle_time;
        was_jiggling = existing_tween->jiggle_tweener->is_jiggling;
        existing_initial_rotation = existing_tween->jiggle_tweener->initial_rotation;
        existing_initial_scale = existing_tween->jiggle_tweener->initial_scale;
        // Kill the existing tween
        kill_specific_tween(existing_tween);
    }

    // Create a new tween
    Ref<YTweenJiggle> tween;
    tween.instantiate(tree);
    tween = create_tween_from(p_owner, tree, tween, p_tag);
    
    if (!tween.is_valid()) {
        return tween;
    }
    
    Ref<JiggleTweener> jiggle_tweener;
    jiggle_tweener.instantiate(p_owner, jiggle_power, jiggle_tilt, jiggle_frequency, Vector3(scale_axes.x, scale_axes.y, 0), Vector3(0,0,1), jiggle_decelerate, additional_speed, constant_jiggle, random_level, rejiggle_power, p_tag);
    // Store jiggle parameters
    tween->jiggle_tweener = jiggle_tweener;
    jiggle_tweener->power_multiplier = jiggle_power;
    jiggle_tweener->jiggle_tilt = jiggle_tilt;
    jiggle_tweener->jiggle_frequency = jiggle_frequency;
    jiggle_tweener->jiggle_decelerate = jiggle_decelerate;
    jiggle_tweener->additional_speed = additional_speed;
    jiggle_tweener->constant_jiggle = constant_jiggle;
    jiggle_tweener->random_level = random_level;
    jiggle_tweener->scale_axes = Vector3(scale_axes.x, scale_axes.y, 0);
    jiggle_tweener->rotation_axes = Vector3(0,0,1);
    jiggle_tweener->current_jiggle_power = 1.0f;
    jiggle_tweener->is_jiggling_finished = false;

    if (was_jiggling) {
        jiggle_tweener->initial_rotation = existing_initial_rotation;
        jiggle_tweener->initial_scale = existing_initial_scale;
        jiggle_tweener->eased_power_progress = 1.1f * jiggle_power;
        jiggle_tweener->jiggle_time = existing_jiggle_time;
        jiggle_tweener->rejiggled = true;
        jiggle_tweener->rejiggle_progress = 1.0f * jiggle_power;
        jiggle_tweener->randomize_trigs((rejiggle_power * jiggle_power) / 4.0f);
    } else {
        jiggle_tweener->target_jiggle_power = 1.15f * jiggle_power;
        jiggle_tweener->eased_power_progress = 1.0f;
        jiggle_tweener->current_jiggle_power = 0.15f * jiggle_power;
        jiggle_tweener->rejiggled = false;
        jiggle_tweener->randomize_trigs();
    }

    jiggle_tweener->eased_power_progress = 1.0f;
    jiggle_tweener->rejiggle_random_offset = Math::random(-0.2f, 0.2f);

    tween->append(jiggle_tweener);

    return tween;
}

void JiggleTweener::set_tween(const Ref<Tween> &p_tween) {
    Tweener::set_tween(p_tween);
	if (trans_type == Tween::TRANS_MAX) {
		trans_type = p_tween->get_trans();
	}
	if (ease_type == Tween::EASE_MAX) {
		ease_type = p_tween->get_ease();
	}
}

Ref<JiggleTweener> JiggleTweener::set_delay(double p_delay) {
	delay = p_delay;
	return this;
}

#ifdef YGODOT
Ref<JiggleTweener> JiggleTweener::set_trans(Tween::TransitionType p_trans, real_t p_param1, real_t p_param2) {
	trans_type = p_trans;
	trans_params[0] = p_param1;
	trans_params[1] = p_param2;
	return this;
}
#else
Ref<JiggleTweener> JiggleTweener::set_trans(Tween::TransitionType p_trans) {
	trans_type = p_trans;
	return this;
}
#endif

Ref<JiggleTweener> JiggleTweener::set_ease(Tween::EaseType p_ease) {
	ease_type = p_ease;
	return this;
}

void JiggleTweener::randomize_trigs(float p_trans)
{
    if (p_trans >= 1.0f)
    {
        jiggle_time = Math::random(-Math_PI * 5.0f, Math_PI * 5.0f);
        trig_values.clear();
        
        for (int i = 0; i < random_level; i++)
            for (int t = 0; t < 2; t++)
                trig_values.push_back(get_random_trig_value());
    }
    else
    {
        jiggle_time = Math::lerp(jiggle_time, static_cast<double>(Math::random(-Math_PI * 5.0, Math_PI * 5.0)), static_cast<double>(p_trans));

        for (int i = 0; i < random_level; i++)
        {
            for (int t = 0; t < 2; t++)
            {
                Vector4 new_t_values = get_random_trig_value();
                Vector4 current_t_values = trig_values.size() > i+t ? static_cast<Vector4>(trig_values.get(i+t)) : get_random_trig_value();
                current_t_values.x = Math::lerp(current_t_values.x, new_t_values.x, p_trans);
                current_t_values.y = Math::lerp(current_t_values.y, new_t_values.y, p_trans);
                current_t_values.z = Math::lerp(current_t_values.z, new_t_values.z, p_trans);
                current_t_values.w = Math::lerp(current_t_values.w, new_t_values.w, p_trans);
            }
        }
    }
}

void JiggleTweener::start() {
    Tweener::start();
    if (!rejiggled) {
        if (jiggle_node != nullptr && jiggle_node->is_inside_tree()) {
            initial_rotation = jiggle_node->get_rotation_degrees();
            initial_scale = jiggle_node->get_scale();
        } else if (jiggle_node_2d != nullptr && jiggle_node_2d->is_inside_tree()) {
            initial_rotation = Vector3(0,0,jiggle_node_2d->get_rotation_degrees());
            initial_scale = Vector3(jiggle_node_2d->get_scale().x, jiggle_node_2d->get_scale().y, 0);
        } else if (jiggle_node_control != nullptr && jiggle_node_control->is_inside_tree()) {
            initial_rotation = Vector3(0,0,jiggle_node_control->get_rotation_degrees());
            initial_scale = Vector3(jiggle_node_control->get_scale().x, jiggle_node_control->get_scale().y, 0);
        }
    }
    randomize_trigs();
    is_jiggling = true;
    started_jiggle = true;
}

bool JiggleTweener::step(double &p_delta)
{
    if (finished) {
		return false;
	}

	elapsed_time += p_delta;

    if (!is_jiggling || ((jiggle_node == nullptr ||!jiggle_node->is_inside_tree()) && (jiggle_node_2d == nullptr || !jiggle_node_2d->is_inside_tree()) && (jiggle_node_control == nullptr || !jiggle_node_control->is_inside_tree()))) {
        return false;
    }

    calculate_trig_values(p_delta);

    if (current_jiggle_power <= 0.0001f && constant_jiggle <= 0.0001f) {
        is_jiggling_finished = true;
        current_jiggle_power = 0.0f;
    }

    float val1 = 0.0f;
    float val2 = 0.0f;

    for (int i = 0; i < random_level * 2; i++)
    {
        if (i % 2 == 0)
            val1 += static_cast<Vector4>(trig_values.get(i)).x;
        else
            val2 += static_cast<Vector4>(trig_values.get(i)).x;
    }

    val1 /= (float)random_level;
    val2 /= (float)random_level;

    if (rejiggled)
    {
        if (rejiggle_progress > 0.0f)
        {
            rejiggle_progress -= p_delta * jiggle_decelerate * 1.75f;
            if (rejiggle_progress < 0.001f) rejiggle_progress = 0.001f;
            float targetValue = Math::sin(jiggle_tilt * 1.45f * static_cast<Vector4>(trig_values.get(1)).w + static_cast<Vector4>(trig_values.get(0)).z + static_cast<Vector4>(trig_values.get(1)).z) * jiggle_tilt * static_cast<Vector4>(trig_values.get(0)).y * rejiggle_progress * rejiggle_power;
            rejiggle_value = static_cast<float>(Math::lerp(static_cast<double>(rejiggle_value), static_cast<double>(targetValue), p_delta * 12.0));
        }
        else
        {
            rejiggle_value = 0.0f;
            rejiggle_progress = 0.0f;
        }
    }
    else
    {
        rejiggle_progress = 0.0f;
        rejiggle_value = 0.0f;
    }

    // Additional variation to rotating
    float add1 = 0.0f;
    float add2 = 0.0f;
    if (random_level > 1)
    {
        add1 = static_cast<Vector4>(trig_values.get(3)).x;
        add2 = static_cast<Vector4>(trig_values.get(2)).x;
    }

    float xAngle = (val1 + rejiggle_value + add2) * rotation_axes.x;
    float yAngle = (-val2 + rejiggle_value - add1) * rotation_axes.y;
    float zAngle = (val2 + rejiggle_value + add2) * rotation_axes.z;

    float xScale = (static_cast<Vector4>(trig_values.get(0)).x * scale_axes.x) * 0.01f;
    float yScale = ((((static_cast<Vector4>(trig_values.get(0)).x + static_cast<Vector4>(trig_values.get(1)).x) / 2.0f)) * scale_axes.y) * 0.01f;
    float zScale = (static_cast<Vector4>(trig_values.get(0)).x * scale_axes.z) * 0.01f;

    if (jiggle_node != nullptr && jiggle_node->is_inside_tree()) {
        jiggle_node->set_rotation_degrees(Vector3(initial_rotation.x + xAngle, initial_rotation.y + yAngle, initial_rotation.z + zAngle));
        jiggle_node->set_scale(Vector3(initial_scale.x + xScale, initial_scale.y + yScale, initial_scale.z + zScale));
    }
    else if (jiggle_node_2d != nullptr && jiggle_node_2d->is_inside_tree()) {
        jiggle_node_2d->set_rotation_degrees(initial_rotation.z + zAngle);
        jiggle_node_2d->set_scale(Vector2(initial_scale.x + xScale, initial_scale.y + yScale));
    }
    else if (jiggle_node_control != nullptr && jiggle_node_control->is_inside_tree()) {
        jiggle_node_control->set_rotation_degrees(initial_rotation.z + zAngle);
        jiggle_node_control->set_scale(Vector2(initial_scale.x + xScale, initial_scale.y + yScale));
    }

    if (is_jiggling_finished) {
        if (jiggle_node != nullptr && jiggle_node->is_inside_tree()) {
            jiggle_node->set_rotation_degrees(initial_rotation);
            jiggle_node->set_scale(initial_scale);
        }
        else if (jiggle_node_2d != nullptr && jiggle_node_2d->is_inside_tree()) {
            jiggle_node_2d->set_rotation_degrees(initial_rotation.z);
            jiggle_node_2d->set_scale(Vector2(initial_scale.x, initial_scale.y));
        }
        else if (jiggle_node_control != nullptr && jiggle_node_control->is_inside_tree()) {
            jiggle_node_control->set_rotation_degrees(initial_rotation.z);
            jiggle_node_control->set_scale(Vector2(initial_scale.x, initial_scale.y));
        }
        is_jiggling = false;
        p_delta = 0.0f;
        return false;
    } else {
        if (constant_jiggle > 0.0001f) {
            is_jiggling_finished = false;

            p_delta = 0.0f;
            
            return true;
        }
    }

    p_delta = 0.0f;
    
    return true;
}

float JiggleTweener::ease_in_out_cubic(float start, float end, float t) {
    t /= 0.5f;
    end -= start;
    if (t < 1) return end * 0.5f * t * t * t + start;
    t -= 2;
    return end * 0.5f * (t * t * t + 2) + start;
}

Vector4 JiggleTweener::get_random_trig_value()
{
    // x: Value, y: Multiplier, z: TimeOffset, w: RandomTimeMult
    return Vector4(0.0f, Math::random(0.85f, 1.15f), Math::random(-Math_PI, Math_PI), Math::random(0.8f, 1.2f));
}

void JiggleTweener::calculate_trig_values(float p_delta_time, float time_multiplier) {
        float finishingSafeRange = current_jiggle_power - 0.01f;
        if (finishingSafeRange <= 0.0f) finishingSafeRange = 0.0f;

        jiggle_time += p_delta_time * (jiggle_frequency * time_multiplier);

        current_jiggle_power = Math::lerp(current_jiggle_power, target_jiggle_power, static_cast<float>(p_delta_time) * 2.0f);

        eased_power_progress = MAX(constant_jiggle, eased_power_progress - p_delta_time * jiggle_decelerate * additional_speed);

        target_jiggle_power = ease_in_out_cubic(-0.001f, 1.0f, eased_power_progress);

        for (int i = 0; i < random_level; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                Vector4 parameters = trig_values.size() > i + j ? static_cast<Vector4>(trig_values.get(i + j)) : get_random_trig_value();

                float timeValue = jiggle_time * parameters.w + parameters.z;
                float multiplyValue = (jiggle_tilt * power_multiplier + additional_tilt) * parameters.y *
                    finishingSafeRange / additional_speed;
                if (j % 2 == 0)
                    parameters.x = Math::sin(timeValue) * multiplyValue;
                else
                    parameters.x = Math::cos(timeValue) * multiplyValue;

                trig_values.set(i + j, parameters);
            }
        }
}