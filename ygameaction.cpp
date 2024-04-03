//
// Created by Daniel on 2024-03-19.
//

#include "ygameaction.h"

void YActionStep::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_step_identifier","step_identifier"), &YActionStep::set_step_identifier);
    ClassDB::bind_method(D_METHOD("get_step_identifier"), &YActionStep::get_step_identifier);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "step_identifier"), "set_step_identifier", "get_step_identifier");

    ClassDB::bind_method(D_METHOD("set_step_taken","step_taken"), &YActionStep::set_step_taken);
    ClassDB::bind_method(D_METHOD("get_step_taken"), &YActionStep::get_step_taken);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "step_taken"), "set_step_taken", "get_step_taken");

    ClassDB::bind_method(D_METHOD("set_step_waiting","step_waiting"), &YActionStep::set_step_waiting);
    ClassDB::bind_method(D_METHOD("get_step_waiting"), &YActionStep::get_step_waiting);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "step_waiting"), "set_step_waiting", "get_step_waiting");
    
    ClassDB::bind_method(D_METHOD("get_step_index"), &YActionStep::get_step_index);

    ClassDB::bind_method(D_METHOD("set_step_data","step_data"), &YActionStep::set_step_data);
    ClassDB::bind_method(D_METHOD("get_step_data"), &YActionStep::get_step_data);
}

void YGameAction::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_POSTINITIALIZE: {
            created();
        } break;
        default: ;
    }
}

void YGameAction::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_action_parameter","param_id", "param_value"), &YGameAction::set_action_parameter);
    ClassDB::bind_method(D_METHOD("increment_action_parameter","param_id", "increment_amount"), &YGameAction::increment_action_parameter);
    ClassDB::bind_method(D_METHOD("set_action_parameters","parameters_in_array"), &YGameAction::set_action_parameter_array);
    ClassDB::bind_method(D_METHOD("erase_action_parameters","parameters_in_array"), &YGameAction::erase_action_parameter_array);
    ClassDB::bind_method(D_METHOD("erase_action_parameter","param_id"), &YGameAction::erase_action_parameter);
    ClassDB::bind_method(D_METHOD("get_action_parameter","param_id","param_default"), &YGameAction::get_action_parameter);
    ClassDB::bind_method(D_METHOD("has_action_parameter","param_id"), &YGameAction::has_action_parameter);
    ClassDB::bind_method(D_METHOD("get_all_action_parameters"), &YGameAction::get_all_action_parameters);
    ClassDB::bind_method(D_METHOD("get_all_action_parameters_named","naming_dictionary"), &YGameAction::get_all_action_parameters_named);

    ClassDB::bind_method(D_METHOD("end_action"), &YGameAction::end_action);
    ClassDB::bind_method(D_METHOD("register_step","step_identifier","step_data"), &YGameAction::register_step);

    ClassDB::bind_method(D_METHOD("get_step_by_index","step_index"), &YGameAction::get_step_by_index);
    ClassDB::bind_method(D_METHOD("get_all_steps_count"), &YGameAction::get_all_steps_count);

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

    ClassDB::bind_method(D_METHOD("set_is_debugging","is_debugging"), &YGameAction::set_is_debugging);
    ClassDB::bind_method(D_METHOD("get_is_debugging"), &YGameAction::get_is_debugging);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_debugging"), "set_is_debugging", "get_is_debugging");

    ClassDB::bind_method(D_METHOD("set_instant_execute","is_instant_execute"), &YGameAction::set_instant_execute);
    ClassDB::bind_method(D_METHOD("get_instant_execute"), &YGameAction::get_instant_execute);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_instant_execute"), "set_instant_execute", "get_instant_execute");

    ClassDB::bind_method(D_METHOD("set_unique_id","unique_id"), &YGameAction::set_unique_id);
    ClassDB::bind_method(D_METHOD("get_unique_id"), &YGameAction::get_unique_id);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "unique_id"), "set_unique_id", "get_unique_id");


    ClassDB::bind_method(D_METHOD("set_turn_player_id","player_id"), &YGameAction::set_player_turn_id);

    ClassDB::bind_method(D_METHOD("set_player_turn","player_turn"), &YGameAction::set_player_turn);
    ClassDB::bind_method(D_METHOD("get_player_turn"), &YGameAction::get_player_turn);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "turn_player_id"), "set_player_turn", "get_player_turn");

    ClassDB::bind_method(D_METHOD("check_is_last_step","step_index"), &YGameAction::get_is_last_step);
    ClassDB::bind_method(D_METHOD("check_waiting_for_step"), &YGameAction::get_waiting_for_step);
    ClassDB::bind_method(D_METHOD("wait_for_step","prevent_processing"), &YGameAction::wait_for_step);
    ClassDB::bind_method(D_METHOD("release_step"), &YGameAction::release_step);

    ADD_SIGNAL(MethodInfo("started_action", PropertyInfo(Variant::STRING, "action_name")));
    ADD_SIGNAL(MethodInfo("registered_step", PropertyInfo(Variant::INT, "step_index")));
    ADD_SIGNAL(MethodInfo("action_stepped", PropertyInfo(Variant::INT, "step_index")));
    ADD_SIGNAL(MethodInfo("waiting_for_step", PropertyInfo(Variant::INT, "step_index")));
    ADD_SIGNAL(MethodInfo("released_waited_step", PropertyInfo(Variant::INT, "step_index")));
    ADD_SIGNAL(MethodInfo("ended_action", PropertyInfo(Variant::STRING, "action_name")));
    GDVIRTUAL_BIND(_on_created)
    GDVIRTUAL_BIND(_on_enter_action)
    GDVIRTUAL_BIND(_on_stepped_action,"step_index","step_identifier","step_data","is_ending")
    GDVIRTUAL_BIND(_on_waiting_step_released,"step_index","step_identifier","step_data","is_ending")
    GDVIRTUAL_BIND(_on_exit_action)
    GDVIRTUAL_BIND(_on_process_action,"delta")
    GDVIRTUAL_BIND(_on_serialize,"dict")
    GDVIRTUAL_BIND(_on_deserialize,"dict")
}


void YGameAction::release_step() {
    if(!waiting_for_step)
        return;
    waiting_for_step=false;
    waiting_for_step_no_processing = false;
    int current_action_steps_in_list = static_cast<int>(action_steps.size());
    for (int i = current_action_steps_in_list-1; i >= 0; --i) {
        if (action_steps[i] != nullptr && action_steps[i].is_valid() && action_steps[i]->step_waiting) {
            Ref<YActionStep> data = action_steps[i];
            data->set_step_waiting(false);
            GDVIRTUAL_CALL(_on_waiting_step_released,data->get_step_index(),data->get_step_identifier(),data->step_data,false);
            emit_signal("released_waited_step",data->step_index);
            emit_signal("action_stepped",data->step_index);
            steps_consumed+=1;
            break;
        }
    }
}

void YGameAction::register_step(const int _step_identifier, const Variant v) {
    Ref<YActionStep> new_step;
    new_step.instantiate();
    new_step->step_index = static_cast<int>(action_steps.size());
    new_step->step_identifier = _step_identifier;
    new_step->step_data = v;
    if (is_debugging)
        print_line(vformat("Registered step index %d, identifier %d, data: %s",new_step->step_index,new_step->step_identifier,new_step->step_data));

    action_steps.append(new_step);
    emit_signal("registered_step",new_step->step_index);
}

void YGameAction::wait_for_step(bool prevent_processing) {
    waiting_for_step=true;
    waiting_for_step_no_processing = prevent_processing;
    int current_action_steps_in_list = static_cast<int>(action_steps.size());
    for (int i = current_action_steps_in_list-1; i >= 0; --i) {
        if (action_steps[i] != nullptr && action_steps[i].is_valid() && action_steps[i]->step_index == last_step_ran) {
            Ref<YActionStep> data = action_steps[i];
            emit_signal("waiting_for_step",data->step_index);
            break;
        }
    }
}

void YGameAction::end_action() {
    started=true;
    finished=true;
    if (waiting_for_step) {
        ERR_PRINT(vformat("YGameState Action %s called end action while waiting for a step."));
    }
    if (is_debugging) {
        print_line(vformat("%s is calling end action",get_name()));
    }
    exit_action();
    executed_exit_action_call=true;
    emit_signal("ended_action",get_name());
    if (player_turn > 0) {
        // auto ctp = YGameState::get_singleton()->current_turn_player;
        // if (ctp != nullptr && ctp
    }
}

void YGameAction::enter_action() {
    auto ygs = YGameState::get_singleton();
    if (ygs != nullptr) {
        in_state_order = YGameState::get_singleton()->next_execution_order_number;
        YGameState::get_singleton()->next_execution_order_number += 1;
        if (player_turn > 0) {
            auto ctp = ygs->current_turn_player;
            if (ctp == nullptr || ctp->player_id != player_turn) {
                if (YGamePlayer* found_player = ygs->get_game_player(player_turn); found_player!=nullptr) {
                    ygs->set_current_turn_player(found_player);
                }
            }
        }
    }

    if (is_debugging) {
        print_line(vformat("%s is calling enter action",get_name()));
    }
    GDVIRTUAL_CALL(_on_enter_action);
    emit_signal("started_action",get_name());
    started=true;
}

void YGameAction::step_action(Ref<YActionStep> data,bool is_ending) {
    if (is_debugging) {
        print_line(vformat("%s is calling step action, current steps %d next step index %d",get_name(),steps_consumed,steps_consumed+1));
    }
    steps_consumed += 1;
    if (data.is_valid()) {
        last_step_ran = data->get_step_index();
        GDVIRTUAL_CALL(_on_stepped_action,data->get_step_index(),data->get_step_identifier(),data->step_data,is_ending);
        data->step_taken=true;
        data->step_taken_as_ending = is_ending;
        if (!waiting_for_step)
            emit_signal("action_stepped",data->step_index);
        else {
            steps_consumed -= 1;
        }
    }
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
    if (is_debugging) {
        print_line(vformat("%s is exiting action",get_name()));
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
    time_started = 0.0;
    steps_consumed = 0;
    unique_id = 0;
    pause_independent_time_started = 0.0;
}
