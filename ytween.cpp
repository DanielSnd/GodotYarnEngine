//
// Created by Daniel on 2024-03-21.
//

#include "ytween.h"

YTween* YTween::singleton = nullptr;
double YTween::last_delta_time = 0.0;


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

    ClassDB::bind_method(D_METHOD("tween_jiggle","owner","power","tilt","frequency","scale_axis","rotation_axis","jiggle_deceleration","additional_speed","constant_jiggle","random_level","rejiggle_power","tag"), &YTween::tween_jiggle,DEFVAL(1.0f),DEFVAL(6.0f),DEFVAL(16.0f),DEFVAL(Vector3(1.0,1.0,1.0)),DEFVAL(Vector3(1.0,0.0,1.0)), DEFVAL(1.5f), DEFVAL(1.0f), DEFVAL(0.0f), DEFVAL(1),DEFVAL(2.0f),DEFVAL(55));

    ClassDB::bind_method(D_METHOD("tween_jiggle_scale","owner","power","tilt","frequency","scale_axis","rotation_axis","jiggle_deceleration","additional_speed","constant_jiggle","random_level","rejiggle_power","tag"), &YTween::tween_jiggle,DEFVAL(1.0f),DEFVAL(6.0f),DEFVAL(16.0f),DEFVAL(Vector3(1.0,1.0,1.0)),DEFVAL(Vector3(0.0,0.0,0.0)), DEFVAL(1.5f), DEFVAL(1.0f), DEFVAL(0.0f), DEFVAL(1),DEFVAL(2.0f),DEFVAL(55));


    ClassDB::bind_method(D_METHOD("tween_jiggle_tilt","owner","power","tilt","frequency","scale_axis","rotation_axis","jiggle_deceleration","additional_speed","constant_jiggle","random_level","rejiggle_power","tag"), &YTween::tween_jiggle,DEFVAL(1.0f),DEFVAL(6.0f),DEFVAL(16.0f),DEFVAL(Vector3(0.0,0.0,0.0)),DEFVAL(Vector3(1.0,0.0,1.0)), DEFVAL(1.5f), DEFVAL(1.0f), DEFVAL(0.0f), DEFVAL(1),DEFVAL(2.0f),DEFVAL(55));

    ClassDB::bind_method(D_METHOD("tween_global_position_unique","owner","global_position","duration","ease","transition","delay","tag"), &YTween::tween_global_position_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_position_unique","owner","position","duration","ease","transition","delay","tag"), &YTween::tween_position_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_position_rotation_unique","owner","position","rotation","duration","ease","transition","delay","tag"), &YTween::tween_position_rotation_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
    ClassDB::bind_method(D_METHOD("tween_position_rotation_scale_unique","owner","position","rotation","scale","duration","ease","transition","delay","tag"), &YTween::tween_position_rotation_scale_unique,DEFVAL(1.0),DEFVAL(0.22),DEFVAL(Tween::EaseType::EASE_IN_OUT),DEFVAL(Tween::TransitionType::TRANS_QUAD),DEFVAL(0.0),DEFVAL(0));
}

YTween * YTween::get_singleton() {
    return singleton;
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
        specific_tween->kill();
        specific_tween->clear();
        
        if (!specific_tween->emitted_finished_or_killed && specific_tween.is_valid()) {
            specific_tween->emitted_finished();
        }
    }
    
    // Finally remove from the main list
    tweens.erase(specific_tween);
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
        if (!tween_wrap.is_null() && tween_wrap.is_valid()) {
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
    //print_line("Create tween with tag ",tag);
    SceneTree *tree = SceneTree::get_singleton();
    if (tree == nullptr) {
        Ref<YTweenWrap> new_wrap;
        return new_wrap;
    }

    // Create the tween
    Ref<YTweenWrap> tween = memnew(YTweenWrap(tree));
    
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

// Helper function to create jiggle animation
Ref<YTweenWrap> YTween::tween_jiggle(Node *p_owner, float jiggle_power, float jiggle_tilt, float jiggle_frequency,
    Vector3 scale_axes, Vector3 rotation_axes, float jiggle_decelerate, float additional_speed, float constant_jiggle, int random_level, float rejiggle_power, uint64_t p_tag) {
    
    SceneTree *tree = SceneTree::get_singleton();
    Node3D* node3d = Object::cast_to<Node3D>(p_owner);
    if (node3d == nullptr && p_owner != nullptr) {
        node3d = Object::cast_to<Node3D>(p_owner->get_parent());
    }
    if (tree == nullptr || node3d == nullptr) {
        Ref<YTweenWrap> new_wrap;
        return new_wrap;
    }

    // Create the tween
    Ref<YTweenJiggle> created_tween = find_tween(p_owner, p_tag);
    Ref<YTweenJiggle> tween;
    if (created_tween.is_null() || !created_tween.is_valid()) {
        created_tween = memnew(YTweenJiggle(tree));
        tween = create_tween_from(p_owner, tree, created_tween, p_tag);
    } else {
        tween = created_tween;
    }
    if (!tween.is_valid()) {
        return tween;
    }

    // Store jiggle parameters
    tween->jiggle_tilt = jiggle_tilt;
    tween->jiggle_frequency = jiggle_frequency;
    tween->jiggle_decelerate = jiggle_decelerate;
    tween->additional_speed = additional_speed;
    tween->constant_jiggle = constant_jiggle;
    tween->random_level = random_level;
    tween->scale_axes = scale_axes;
    tween->rotation_axes = rotation_axes;
    tween->power_multiplier = jiggle_power;
    tween->rejiggle_power = rejiggle_power;
    tween->is_jiggling = true;
    tween->current_jiggle_power = 1.0f;
    tween->is_jiggling_finished = false;
    tween->jiggle_node = node3d;
    if (tween->started_jiggle) {
        tween->eased_power_progress = 1.1f * jiggle_power;
        tween->rejiggled = true;
        tween->rejiggle_progress = 1.0f * jiggle_power;
        tween->randomize_trigs((rejiggle_power * jiggle_power) / 4.0f);
        tween->stop();
        tween->play();
    } else {
        tween->target_jiggle_power = 1.15f * jiggle_power;
        tween->eased_power_progress = 1.0f;
        tween->current_jiggle_power = 0.15f * jiggle_power;
        tween->rejiggled = false;
        tween->randomize_trigs();
        tween->tween_method(tween->get_jiggle_callable(), 0.0, 1.0, 10.0f);
    }

    tween->eased_power_progress = 1.0f;
    tween->rejiggle_random_offset = Math::random(-0.2f, 0.2f);
    // print_line("Created jiggle tween with tag ",p_tag);
    return tween;
}

void YTweenJiggle::randomize_trigs(float p_trans)
{
    if (p_trans >= 1.0f)
    {
        jiggle_time = Math::random(-M_PI * 5.0f, M_PI * 5.0f);
        trig_values.clear();
        
        for (int i = 0; i < random_level; i++)
            for (int t = 0; t < 2; t++)
                trig_values.push_back(get_random_trig_value());
    }
    else
    {
        jiggle_time = Math::lerp(jiggle_time, static_cast<double>(Math::random(-M_PI * 5.0, M_PI * 5.0)), static_cast<double>(p_trans));

        for (int i = 0; i < random_level; i++)
        {
            for (int t = 0; t < 2; t++)
            {
                Vector4 new_t_values = get_random_trig_value();
                Vector4 current_t_values = trig_values.get(i+t);
                current_t_values.x = Math::lerp(current_t_values.x, new_t_values.x, p_trans);
                current_t_values.y = Math::lerp(current_t_values.y, new_t_values.y, p_trans);
                current_t_values.z = Math::lerp(current_t_values.z, new_t_values.z, p_trans);
                current_t_values.w = Math::lerp(current_t_values.w, new_t_values.w, p_trans);
            }
        }
    }
}

void YTweenJiggle::calculate_jiggle(float p_delta)
{
    if (!started_jiggle) {
        if (jiggle_node != nullptr && jiggle_node->is_inside_tree()) {
            initial_rotation = jiggle_node->get_rotation_degrees();
            initial_scale = jiggle_node->get_scale();
            randomize_trigs();
            is_jiggling = true;
            started_jiggle = true;
        }
    }
    // print_line("Calculating jiggle ",p_delta," last delta time ",YTween::last_delta_time);
    if (!is_jiggling || jiggle_node == nullptr || !jiggle_node->is_inside_tree()) {
        if (YTween::get_singleton() != nullptr) {
            YTween::get_singleton()->kill_specific_tween(this);
        }
        return;
    }

    calculate_trig_values();

    if (current_jiggle_power <= 0.0f && constant_jiggle <= 0.0f) is_jiggling_finished = true;
    

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
            rejiggle_progress -= YTween::last_delta_time * jiggle_decelerate * 1.75f;
            if (rejiggle_progress < 0.001f) rejiggle_progress = 0.001f;
            float targetValue = Math::sin(jiggle_tilt * 1.45f * static_cast<Vector4>(trig_values.get(1)).w + static_cast<Vector4>(trig_values.get(0)).z + static_cast<Vector4>(trig_values.get(1)).z) * jiggle_tilt * static_cast<Vector4>(trig_values.get(0)).y * rejiggle_progress * rejiggle_power;
            rejiggle_value = static_cast<float>(Math::lerp(static_cast<double>(rejiggle_value), static_cast<double>(targetValue), YTween::last_delta_time * 12.0));
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

    if (is_jiggling_finished) {
        if (jiggle_node != nullptr && jiggle_node->is_inside_tree()) {
            jiggle_node->set_rotation_degrees(initial_rotation);
            jiggle_node->set_scale(initial_scale);
        }
        is_jiggling = false;
        if (YTween::get_singleton() != nullptr) {
            YTween::get_singleton()->kill_specific_tween(this);
        }
    } else {
        if (p_delta > 0.9f && constant_jiggle > 0.0001f) {
            stop();
            play();
        }
    }
}

float YTweenJiggle::ease_in_out_cubic(float start, float end, float t) {
    t /= 0.5f;
    end -= start;
    if (t < 1) return end * 0.5f * t * t * t + start;
    t -= 2;
    return end * 0.5f * (t * t * t + 2) + start;
}

Vector4 YTweenJiggle::get_random_trig_value()
{
    // x: Value, y: Multiplier, z: TimeOffset, w: RandomTimeMult
    return Vector4(0.0f, Math::random(0.85f, 1.15f), Math::random(-M_PI, M_PI), Math::random(0.8f, 1.2f));
}

void YTweenJiggle::calculate_trig_values(float time_multiplier) {
        float finishingSafeRange = current_jiggle_power - 0.01f;
        if (finishingSafeRange <= 0.0f) finishingSafeRange = 0.0f;

        jiggle_time += YTween::last_delta_time * (jiggle_frequency * time_multiplier);

        current_jiggle_power = Math::lerp(current_jiggle_power, target_jiggle_power, static_cast<float>(YTween::last_delta_time) * 2.0f);

        eased_power_progress = MAX(constant_jiggle, eased_power_progress - YTween::last_delta_time * jiggle_decelerate * additional_speed);

        target_jiggle_power = ease_in_out_cubic(-0.001f, 1.0f, eased_power_progress);

        for (int i = 0; i < random_level; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                Vector4 parameters = trig_values.get(i + j);

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

Callable YTweenJiggle::get_jiggle_callable()
{
    return callable_mp(this, &YTweenJiggle::calculate_jiggle);
}
