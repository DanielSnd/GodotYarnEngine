//
// Created by Daniel on 2024-03-19.
//

#include "ygameaction.h"

void YGameAction::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_POSTINITIALIZE: {
            created();
        } break;
    }
}

void YGameAction::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_action_parameter","param_id", "param_value"), &YGameAction::set_action_parameter);
    ClassDB::bind_method(D_METHOD("get_action_parameter","param_id","param_default"), &YGameAction::get_action_parameter);
    ClassDB::bind_method(D_METHOD("has_action_parameter","param_id"), &YGameAction::has_action_parameter);
    ClassDB::bind_method(D_METHOD("get_all_action_parameters"), &YGameAction::get_all_action_parameters);
    ClassDB::bind_method(D_METHOD("end_action"), &YGameAction::end_action);
    ClassDB::bind_method(D_METHOD("register_step","step_data"), &YGameAction::register_step);
    //ClassDB::bind_method(D_METHOD("add_action_step"), &YGameAction::add_step);

    ClassDB::bind_method(D_METHOD("copy_parameters_from","other_action"), &YGameAction::copy_parameters_from);
    ClassDB::bind_method(D_METHOD("copy_parameters_to","other_action"), &YGameAction::copy_parameters_to);

    ClassDB::bind_method(D_METHOD("has_elapsed_since_started","interval"), &YGameAction::has_elapsed_since_started);
    ClassDB::bind_method(D_METHOD("set_time_started","time_start"), &YGameAction::set_time_started);
    ClassDB::bind_method(D_METHOD("get_time_started"), &YGameAction::get_time_started);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "time_started"), "set_time_started", "get_time_started");

    ClassDB::bind_method(D_METHOD("has_pause_indp_elapsed_since_started","interval"), &YGameAction::has_pause_indp_elapsed_since_started);
    ClassDB::bind_method(D_METHOD("set_pause_indp_time_started","time_start"), &YGameAction::set_pause_indp_time_started);
    ClassDB::bind_method(D_METHOD("get_pause_indp_time_started"), &YGameAction::get_pause_indp_time_started);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "pause_indp_time_started"), "set_pause_indp_time_started", "get_pause_indp_time_started");

    ClassDB::bind_method(D_METHOD("set_unique_id","unique_id"), &YGameAction::set_unique_id);
    ClassDB::bind_method(D_METHOD("get_unique_id"), &YGameAction::get_unique_id);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "unique_id"), "set_unique_id", "get_unique_id");

    ClassDB::bind_method(D_METHOD("set_player_turn","player_turn"), &YGameAction::set_player_turn);
    ClassDB::bind_method(D_METHOD("get_player_turn"), &YGameAction::get_player_turn);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "player_turn"), "set_player_turn", "get_player_turn");

    GDVIRTUAL_BIND(_on_created)
    GDVIRTUAL_BIND(_on_enter_action)
    GDVIRTUAL_BIND(_on_stepped_action)
    GDVIRTUAL_BIND(_on_exit_action)
    GDVIRTUAL_BIND(_on_process_action,"delta")
    GDVIRTUAL_BIND(_on_serialize,"dict")
    GDVIRTUAL_BIND(_on_deserialize,"dict")
}

void YGameAction::register_step(const Variant v) {
    action_steps.append(v);
    step_action(v,false);
}

void YGameAction::end_action() {
    started=true;
    finished=true;
    exit_action();
    executed_exit_action_call=true;
    if (player_turn > 0) {
        // auto ctp = YGameState::get_singleton()->current_turn_player;
        // if (ctp != nullptr && ctp
    }
}

void YGameAction::enter_action() {
    if (player_turn > 0) {
        auto ygamestate =YGameState::get_singleton();
         auto ctp = ygamestate->current_turn_player;
        if (ctp == nullptr || ctp->player_id != player_turn) {
            if (YGamePlayer* found_player = ygamestate->get_game_player(player_turn); found_player!=nullptr) {
                YGameState::get_singleton()->set_current_turn_player(found_player);
            }
        }
    }
    GDVIRTUAL_CALL(_on_enter_action);
    started=true;
}

void YGameAction::step_action(Variant step_data,bool is_ending) {
    steps_consumed += 1;
    GDVIRTUAL_CALL(_on_stepped_action,step_data,is_ending);
}

void YGameAction::exit_action() {
    if(executed_exit_action_call) return;
    executed_exit_action_call=true;
    for (int i = 0; i < action_steps.size(); ++i) {
        if (i < steps_consumed) {
            continue;
        }
        auto action_data = action_steps[i];
        step_action(action_data,true);
    }
    GDVIRTUAL_CALL(_on_exit_action);
    finished=true;
}

bool YGameAction::process_action(float _delta) {
    bool rert = true;
    GDVIRTUAL_CALL(_on_process_action,_delta,rert);
    return rert;
}

bool YGameAction::slow_process_action(float _delta) {
    bool rert = true;
    GDVIRTUAL_CALL(_on_slow_process_action,_delta,rert);
    return rert;
}

Dictionary YGameAction::serialize(Dictionary dict) {
    GDVIRTUAL_CALL(_on_serialize,dict,dict);
    return dict;
}

Dictionary YGameAction::deserialize(Dictionary dict) {
    GDVIRTUAL_CALL(_on_deserialize,dict,dict);
    return dict;
}

void YGameAction::created() {
    Ref<Script> _find_script = get_script();
    if (_find_script.is_valid()) {
        String string_global_name = _find_script->get_global_name();
        if (!string_global_name.is_empty()) {
            set_name(string_global_name);
        }
    }
    GDVIRTUAL_CALL(_on_created);
}

YGameAction::YGameAction() {
    player_turn=-1;
}
