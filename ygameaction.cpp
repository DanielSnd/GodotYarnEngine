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

    ClassDB::bind_method(D_METHOD("set_step_waiting","step_waiting"), &YActionStep::set_step_waiting, DEFVAL(false));
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
    ClassDB::bind_method(D_METHOD("remove_action_parameter","param_id"), &YGameAction::remove_action_parameter);
    ClassDB::bind_method(D_METHOD("increment_action_parameter","param_id", "increment_amount"), &YGameAction::increment_action_parameter);
    ClassDB::bind_method(D_METHOD("set_action_parameters","parameters_in_array"), &YGameAction::set_action_parameter_array);
    ClassDB::bind_method(D_METHOD("erase_action_parameters","parameters_in_array"), &YGameAction::erase_action_parameter_array);
    ClassDB::bind_method(D_METHOD("erase_action_parameter","param_id"), &YGameAction::erase_action_parameter);
    ClassDB::bind_method(D_METHOD("get_action_parameter","param_id","param_default"), &YGameAction::get_action_parameter);
    ClassDB::bind_method(D_METHOD("has_action_parameter","param_id"), &YGameAction::has_action_parameter);
    ClassDB::bind_method(D_METHOD("get_all_action_parameters"), &YGameAction::get_all_action_parameters);
    ClassDB::bind_method(D_METHOD("get_all_action_parameters_named","naming_dictionary"), &YGameAction::get_all_action_parameters_named);

    ClassDB::bind_method(D_METHOD("end_action", "sync_to_others"), &YGameAction::end_action, DEFVAL(false));
    ClassDB::bind_method(D_METHOD("register_step","step_identifier","step_data"), &YGameAction::register_step);

    ClassDB::bind_method(D_METHOD("get_step_by_index","step_index"), &YGameAction::get_step_by_index);
    ClassDB::bind_method(D_METHOD("get_all_steps_count"), &YGameAction::get_all_steps_count);
    ClassDB::bind_method(D_METHOD("get_steps_consumed"), &YGameAction::get_steps_consumed);

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

    ClassDB::bind_method(D_METHOD("set_priority","priority"), &YGameAction::set_priority, DEFVAL(-1));
    ClassDB::bind_method(D_METHOD("get_priority"), &YGameAction::get_priority);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "priority"), "set_priority", "get_priority");

    ClassDB::bind_method(D_METHOD("set_turn_player_id","player_id"), &YGameAction::set_player_turn_id);

    ClassDB::bind_method(D_METHOD("set_player_turn","player_turn"), &YGameAction::set_player_turn);
    ClassDB::bind_method(D_METHOD("get_player_turn"), &YGameAction::get_player_turn);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "turn_player_id"), "set_player_turn", "get_player_turn");

    ClassDB::bind_method(D_METHOD("check_is_last_step","step_index"), &YGameAction::get_is_last_step);
    ClassDB::bind_method(D_METHOD("check_waiting_for_step"), &YGameAction::get_waiting_for_step);
    ClassDB::bind_method(D_METHOD("wait_for_step","prevent_processing"), &YGameAction::wait_for_step);
    ClassDB::bind_method(D_METHOD("release_step"), &YGameAction::release_step);

    ClassDB::bind_method(D_METHOD("get_from_step_data", "step_data", "get_index", "get_default"), &YGameAction::get_from_step_data);

    ClassDB::bind_method(D_METHOD("serialize"), &YGameAction::serialize);
    ClassDB::bind_method(D_METHOD("deserialize", "dict"), &YGameAction::deserialize);

    ClassDB::bind_method(D_METHOD("set_runs_parallel", "runs_parallel"), &YGameAction::set_runs_parallel);
    ClassDB::bind_method(D_METHOD("get_runs_parallel"), &YGameAction::get_runs_parallel);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "runs_parallel"), "set_runs_parallel", "get_runs_parallel");

    ClassDB::bind_method(D_METHOD("set_max_in_parallel", "max_in_parallel"), &YGameAction::set_max_in_parallel);
    ClassDB::bind_method(D_METHOD("get_max_in_parallel"), &YGameAction::get_max_in_parallel);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_in_parallel"), "set_max_in_parallel", "get_max_in_parallel");

    ClassDB::bind_method(D_METHOD("set_start_check_interval", "interval"), &YGameAction::set_start_check_interval);
    ClassDB::bind_method(D_METHOD("get_start_check_interval"), &YGameAction::get_start_check_interval);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "start_check_interval"), "set_start_check_interval", "get_start_check_interval");

    ClassDB::bind_method(D_METHOD("set_can_be_serialized", "can_be_serialized"), &YGameAction::set_can_be_serialized);
    ClassDB::bind_method(D_METHOD("get_can_be_serialized"), &YGameAction::get_can_be_serialized);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "can_be_serialized"), "set_can_be_serialized", "get_can_be_serialized");

    ClassDB::bind_method(D_METHOD("request_step_approval", "step_identifier", "step_data"), &YGameAction::request_step_approval);

    {
        MethodInfo mi;
        mi.name = "broadcast_call";
        mi.arguments.push_back(PropertyInfo(Variant::CALLABLE, "method"));

        ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, "broadcast_call", &YGameAction::broadcast_call, mi);
    }

    {
        MethodInfo mi;
        mi.name = "broadcast_call_and_execute";
        mi.arguments.push_back(PropertyInfo(Variant::CALLABLE, "method"));

        ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, "broadcast_call_and_execute", &YGameAction::broadcast_call_and_execute, mi);
    }

    ADD_SIGNAL(MethodInfo(SNAME("started_action")));
    ADD_SIGNAL(MethodInfo(SNAME("registered_step"), PropertyInfo(Variant::INT, "step_index")));
    ADD_SIGNAL(MethodInfo(SNAME("action_stepped"), PropertyInfo(Variant::INT, "step_index")));
    ADD_SIGNAL(MethodInfo(SNAME("waiting_for_step"), PropertyInfo(Variant::INT, "step_index")));
    ADD_SIGNAL(MethodInfo(SNAME("released_waited_step"), PropertyInfo(Variant::INT, "step_index")));
    ADD_SIGNAL(MethodInfo(SNAME("ended_action")));
    ADD_SIGNAL(MethodInfo(SNAME("on_changed_action_parameter"), 
        PropertyInfo(Variant::INT, "param_id"),
        PropertyInfo(Variant::NIL, "old_value"),
        PropertyInfo(Variant::NIL, "new_value")));

    GDVIRTUAL_BIND(_on_created)
    GDVIRTUAL_BIND(_on_enter_action)
    GDVIRTUAL_BIND(_on_stepped_action,"step_index","step_identifier","step_data","is_ending")
    GDVIRTUAL_BIND(_on_waiting_step_released,"step_index","step_identifier","step_data","is_ending")
    GDVIRTUAL_BIND(_on_exit_action)
    GDVIRTUAL_BIND(_on_process_action,"delta")
    GDVIRTUAL_BIND(_on_serialize,"dict")
    GDVIRTUAL_BIND(_on_deserialize,"dict")
    GDVIRTUAL_BIND(_only_starts_if)
    GDVIRTUAL_BIND(_step_request_approval, "step_identifier", "step_data", "sender_id")
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
            GDVIRTUAL_CALL(_on_waiting_step_released, data->get_step_index(), data->get_step_identifier(), data->step_data, false);
            emit_signal("released_waited_step",data->step_index);
            emit_signal("action_stepped",data->step_index);
            if (data->step_has_to_reconsume) {
                steps_consumed+=1;
                data->step_has_to_reconsume = false;
            }
            break;
        }
    }
}

Variant YGameAction::get_from_step_data(Array p_step_data, int p_get_index, const Variant& p_get_default) {
    if (static_cast<int>(p_step_data.size()) > p_get_index) {
        return p_step_data[p_get_index];
    }
    return p_get_default;
}

void YGameAction::register_step(const int _step_identifier, const Variant v) {
    if (instant_execute) return;

    // If we're not the server and this is a networked action, request approval through YEngine
    if (YGameState::get_singleton() != nullptr && YGameState::get_singleton()->get_multiplayer()->has_multiplayer_peer() && 
        YGameState::get_singleton()->get_multiplayer()->get_unique_id() != 1) {
        request_step_approval(_step_identifier, v);
        return;
    }

    // Server or non-networked action - register step directly
    actually_register_step(_step_identifier, v);

    // If we're the server, broadcast the step to all clients through YEngine
    if (YGameState::get_singleton() != nullptr && YGameState::get_singleton()->get_multiplayer()->has_multiplayer_peer() && 
        YGameState::get_singleton()->get_multiplayer()->get_unique_id() == 1) {
        YGameState::get_singleton()->broadcast_action_step(this, _step_identifier, v);
    }
}

void YGameAction::actually_register_step(const int _step_identifier, const Variant v) {
    if (instant_execute) return;

    // Server or non-networked action - register step directly
    Ref<YActionStep> new_step;
    new_step.instantiate();
    new_step->step_index = static_cast<int>(action_steps.size());
    new_step->step_identifier = _step_identifier;
    new_step->step_data = v;
    if (is_debugging)
        print_line(vformat("Registered step index %d, identifier %d, data: %s",new_step->step_index,new_step->step_identifier,new_step->step_data));

    action_steps.append(new_step);
    emit_signal(SNAME("registered_step"),new_step->step_index);
}

void YGameAction::request_step_approval(int step_identifier, const Variant& step_data) {
    if (YGameState::get_singleton() != nullptr) {
        YGameState::get_singleton()->request_action_step_approval(this, step_identifier, step_data);
    }
}

bool YGameAction::check_if_has_step_approval(int step_identifier, const Variant& step_data, int sender_id) {
    bool approved = false;
    GDVIRTUAL_CALL(_step_request_approval, step_identifier, step_data, sender_id, approved);
    return approved;
}

void YGameAction::wait_for_step(bool prevent_processing) {
    waiting_for_step=true;
    waiting_for_step_no_processing = prevent_processing;
    int current_action_steps_in_list = static_cast<int>(action_steps.size());
    for (int i = current_action_steps_in_list-1; i >= 0; --i) {
        if (action_steps[i] != nullptr && action_steps[i].is_valid() && !action_steps[i]->step_taken && action_steps[i]->step_index == last_step_ran) {
            Ref<YActionStep> data = action_steps[i];
            data->set_step_waiting(true);
            emit_signal(SNAME("waiting_for_step"),data->step_index);
            break;
        }
    }
}

void YGameAction::end_action(bool sync_to_others) {
    started=true;
    finished=true;
    if (waiting_for_step) {
        ERR_PRINT(vformat("YGameState Action %s called end action while waiting for a step.", get_name()));
    }
    if (is_debugging) {
        print_line(vformat("%s is calling end action",get_name()));
    }
    if (sync_to_others && YGameState::get_singleton() != nullptr && YGameState::get_singleton()->get_multiplayer()->has_multiplayer_peer() && 
        YGameState::get_singleton()->get_multiplayer()->get_unique_id() == 1) {
        YGameState::get_singleton()->broadcast_action_end(unique_id);
    }
    exit_action();
    executed_exit_action_call=true;
    emit_signal(SNAME("ended_action"));
}

void YGameAction::enter_action() {
    if (!has_executed_created_method)
        created();

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
    emit_signal(SNAME("started_action"));
    started=true;
}

void YGameAction::step_action(Ref<YActionStep> data,bool is_ending) {
    if (is_debugging) {
        print_line(vformat("%s is calling step action, current steps %d next step index %d",get_name(),steps_consumed,steps_consumed+1));
    }
    steps_consumed += 1;
    if (data.is_valid()) {
        last_step_ran = data->get_step_index();
        if (data->get_step_waiting() && !waiting_for_step) {
            waiting_for_step = true;
            emit_signal(SNAME("waiting_for_step"),data->step_index);
        }
        GDVIRTUAL_CALL(_on_stepped_action,data->get_step_index(),data->get_step_identifier(),data->step_data,is_ending);
        data->step_taken=true;
        data->step_taken_as_ending = is_ending;
        if (!waiting_for_step)
            emit_signal(SNAME("action_stepped"),data->step_index);
        else {
            steps_consumed -= 1;
            data->step_has_to_reconsume = true;
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
        if (!action_data->get_step_taken()) {
            step_action(action_data, true);
        }
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

Dictionary YGameAction::serialize() {
    Dictionary dict;
    GDVIRTUAL_CALL(_on_serialize,dict,dict);

    dict["id"] = unique_id;
    dict["class"] = get_name();

    if (runs_parallel)
        dict["prll"] = true;
    if (max_in_parallel != -1)
        dict["max_prll"] = max_in_parallel;

    if (player_turn != -1)
        dict["pturn"] = player_turn;

    if (!action_parameters.is_empty()) {
        Dictionary serialize_action_parameters;
        Dictionary serialize_start_action_parameters;
        for (const auto& action_parameter: action_parameters) {
            serialize_action_parameters[action_parameter.key] = action_parameter.value;
            if (starting_parameters.has(action_parameter.key) && starting_parameters[action_parameter.key] != action_parameter.value)
                serialize_start_action_parameters[action_parameter.key] = starting_parameters[action_parameter.key];
        }
        dict["params"] = serialize_action_parameters;
        if (!serialize_start_action_parameters.is_empty())
            dict["start_params"] = serialize_start_action_parameters;
    }

    if (!action_steps.is_empty()) {
        TypedArray<Dictionary> serialize_action_steps;
        for (const auto& serialize_action_step: action_steps) {
            Dictionary p_step_info;
            p_step_info["i"] = serialize_action_step->get_step_index();
            p_step_info["id"] = serialize_action_step->get_step_identifier();
            p_step_info["data"] = serialize_action_step->get_step_data();
            serialize_action_steps.push_back(p_step_info);
        }
        dict["steps"] = serialize_action_steps;
    }

    if (Math::abs(start_check_interval- 3.0f) > 0.001f)
        dict["start_int"] = start_check_interval;

    return dict;
}

Dictionary YGameAction::deserialize(Dictionary dict) {
    GDVIRTUAL_CALL(_on_deserialize, dict, dict);

    unique_id = dict.get("id",0);
    player_turn = dict.get("pturn",-1);

    runs_parallel = dict.get("prll", false);
    max_in_parallel = dict.get("max_prll", -1);

    if (dict.has("params")) {
        Dictionary serialized_action_parameters = dict["params"];
        Array serialized_action_keys = serialized_action_parameters.keys();
        for (int i = 0; i < static_cast<int>(serialized_action_keys.size()); ++i) {
            action_parameters[serialized_action_keys[i]] = serialized_action_parameters[serialized_action_keys[i]];
        }
    }
    
    if (dict.has("start_params")) {
        Dictionary serialized_action_parameters = dict["start_params"];
        Array serialized_action_keys = serialized_action_parameters.keys();
        for (int i = 0; i < static_cast<int>(serialized_action_keys.size()); ++i) {
            starting_parameters[serialized_action_keys[i]] = serialized_action_parameters[serialized_action_keys[i]];
            action_parameters[serialized_action_keys[i]] = serialized_action_parameters[serialized_action_keys[i]];
        }
    }

    if (dict.has("params")) {
        Dictionary serialized_action_parameters = dict["params"];
        Array serialized_action_keys = serialized_action_parameters.keys();
        for (int i = 0; i < static_cast<int>(serialized_action_keys.size()); ++i) {
            action_parameters[serialized_action_keys[i]] = serialized_action_parameters[serialized_action_keys[i]];
        }
    }
    

    if (dict.has("steps")) {
        TypedArray<Dictionary> serialized_action_steps = dict["steps"];
        for (int i = 0; i < serialized_action_steps.size(); i++) {
            Dictionary p_step_info = serialized_action_steps[i];
            Ref<YActionStep> new_step;
            new_step.instantiate();
            if (p_step_info.has("id"))
                new_step->set_step_identifier(p_step_info["id"]);
            if (p_step_info.has("data"))
                new_step->set_step_data(p_step_info["data"]);
            if (p_step_info.has("i"))
                new_step->step_index = p_step_info["i"];

            action_steps.push_back(new_step);
        }
    }

    start_check_interval = dict.get("start_int", 3.0f);

    return dict;
}

void YGameAction::created() {
    if (has_executed_created_method) {
        return;
    }
    has_executed_created_method = true;
    Ref<Script> _find_script = get_script();
    if (_find_script.is_valid()) {
        String string_global_name = _find_script->get_global_name();
        if (!string_global_name.is_empty()) {
            set_name(string_global_name);
        }
        GDVIRTUAL_CALL(_on_created);
    } else {
        has_executed_created_method = false;
    }
}

YGameAction::YGameAction() {
    is_playing_back = false;
    has_executed_created_method = false;
    player_turn=-1;
    instant_execute = false;
    time_started = 0.0;
    steps_consumed = 0;
    unique_id = 0;
    pause_independent_time_started = 0.0;
    runs_parallel = false;
    max_in_parallel = -1;
    start_check_interval = 3.0f;
    last_start_check_time = 0.0f;
    can_be_serialized = true;
}

bool YGameAction::only_starts_if() {
    bool can_start = true;
    GDVIRTUAL_CALL(_only_starts_if, can_start);
    return can_start;
}

void YGameAction::emit_action_parameter_changed(int param_id, const Variant& old_value, const Variant& new_value) {
    emit_signal(SNAME("on_changed_action_parameter"), param_id, old_value, new_value);
}

YGameAction* YGameAction::set_action_parameter(int param, const Variant& v) {
    Variant old_value;
    if (action_parameters.has(param)) {
        old_value = action_parameters[param];
        emit_action_parameter_changed(param, old_value, v);
    } else {
        starting_parameters[param] = v;
    }
    action_parameters[param] = v;
    return this;
}

Error YGameAction::broadcast_call(const Variant **p_args, int p_argcount, Callable::CallError &r_error) {
    return YGameState::get_singleton()->broadcast_call_on_game_action(p_args, p_argcount, r_error);
}

Error YGameAction::broadcast_call_and_execute(const Variant **p_args, int p_argcount, Callable::CallError &r_error) {
    return YGameState::get_singleton()->broadcast_call_on_game_action_and_execute(p_args, p_argcount, r_error);
}