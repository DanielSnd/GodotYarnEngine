//
// Created by Daniel on 2024-03-19.
//

#include "ygamestate.h"

YGameState* YGameState::singleton = nullptr;

void YGameState::_bind_methods() {
    ClassDB::bind_method(D_METHOD("register_player", "game_player","rename_to"), &YGameState::register_player,DEFVAL(""));
    ClassDB::bind_method(D_METHOD("register_player_specific_id", "game_player","desired_id","rename_to"), &YGameState::register_player_specific_id,DEFVAL(""));

    ClassDB::bind_method(D_METHOD("get_all_players"), &YGameState::get_all_players);
    ClassDB::bind_method(D_METHOD("get_game_player", "player_id"), &YGameState::get_game_player);
    ClassDB::bind_method(D_METHOD("get_game_player_by_controller", "player_controller"), &YGameState::get_game_player_by_controller);
    ClassDB::bind_method(D_METHOD("get_game_player_by_type", "player_type"), &YGameState::get_game_player_by_type);
    ClassDB::bind_method(D_METHOD("remove_player", "game_player"), &YGameState::remove_player);
    ClassDB::bind_method(D_METHOD("remove_player_with_id", "game_player_id"), &YGameState::remove_player_with_id);

    ClassDB::bind_method(D_METHOD("get_next_player_unique_id"), &YGameState::get_next_player_unique_id);

    ClassDB::bind_method(D_METHOD("add_game_action", "game_action","desired_id"), &YGameState::add_game_action,DEFVAL(-1));
    ClassDB::bind_method(D_METHOD("add_override_game_action", "game_action","desired_id"), &YGameState::add_override_game_action,DEFVAL(-1));

    ClassDB::bind_method(D_METHOD("add_game_action_with", "game_action","initial_parameter","param_value","desired_id"), &YGameState::add_game_action_with_param,DEFVAL(-1),DEFVAL(Variant{}),DEFVAL(-1));
    ClassDB::bind_method(D_METHOD("add_override_game_action_with","initial_parameter","param_value", "game_action","desired_id"), &YGameState::add_override_game_action_with_param,DEFVAL(-1),DEFVAL(Variant{}),DEFVAL(-1));

    ClassDB::bind_method(D_METHOD("has_valid_multiplayer_peer"), &YGameState::has_valid_multiplayer_peer);
    ClassDB::bind_method(D_METHOD("ensure_has_initialized"), &YGameState::ensure_has_initialized);

    ClassDB::bind_method(D_METHOD("clear_all_players"), &YGameState::clear_all_players);
    ClassDB::bind_method(D_METHOD("clear_all_game_actions"), &YGameState::clear_all_game_actions);
    ClassDB::bind_method(D_METHOD("clear_state_parameter_authorities"), &YGameState::clear_state_parameter_authorities);

    ClassDB::bind_method(D_METHOD("get_current_game_action"), &YGameState::get_current_game_action);
    ClassDB::bind_method(D_METHOD("get_current_game_action_name"), &YGameState::get_current_game_action_name);

    ClassDB::bind_method(D_METHOD("has_current_game_action"), &YGameState::has_current_game_action);
    ClassDB::bind_method(D_METHOD("end_current_game_action"), &YGameState::end_current_game_action);

    ClassDB::bind_method(D_METHOD("set_current_turn_player","current_turn_player"), &YGameState::set_current_turn_player);
    ClassDB::bind_method(D_METHOD("get_current_turn_player"), &YGameState::get_current_turn_player);
    ClassDB::bind_method(D_METHOD("has_current_turn_player"), &YGameState::has_current_turn_player);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "current_turn_player", PROPERTY_HINT_NODE_TYPE, "YGamePlayer",PROPERTY_USAGE_NO_EDITOR), "set_current_turn_player", "get_current_turn_player");

    ClassDB::bind_method(D_METHOD("set_wait_before_going_to_next_step","time_to_wait"), &YGameState::set_wait_before_going_to_next_step);
    ClassDB::bind_method(D_METHOD("get_wait_before_going_to_next_step"), &YGameState::get_wait_before_going_to_next_step);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "wait_before_going_to_next_step"), "set_wait_before_going_to_next_step", "get_wait_before_going_to_next_step");

    ClassDB::bind_method(D_METHOD("set_wait_before_going_to_next_action","time_to_wait"), &YGameState::set_wait_before_going_to_next_action);
    ClassDB::bind_method(D_METHOD("get_wait_before_going_to_next_action"), &YGameState::get_wait_before_going_to_next_action);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "wait_before_going_to_next_action"), "set_wait_before_going_to_next_action", "get_wait_before_going_to_next_action");

    ClassDB::bind_method(D_METHOD("set_prevent_proceed_to_next_action","preventing_amount"), &YGameState::set_prevent_proceed_to_next_action);
    ClassDB::bind_method(D_METHOD("get_prevent_proceed_to_next_action"), &YGameState::get_prevent_proceed_to_next_action);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "prevent_proceed_to_next_action"), "set_prevent_proceed_to_next_action", "get_prevent_proceed_to_next_action");

    ClassDB::bind_method(D_METHOD("set_is_playing_back", "is_playing_back"), &YGameState::set_is_playing_back);
    ClassDB::bind_method(D_METHOD("get_is_playing_back"), &YGameState::get_is_playing_back);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_playing_back"), "set_is_playing_back", "get_is_playing_back");

    ClassDB::bind_method(D_METHOD("set_debugging_level", "debugging_level"), &YGameState::set_debugging_level, DEFVAL(0));
    ClassDB::bind_method(D_METHOD("get_debugging_level"), &YGameState::get_debugging_level);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "debugging_level"), "set_debugging_level", "get_debugging_level");

    ClassDB::bind_method(D_METHOD("set_last_action_was_denied", "last_action_was_denied"), &YGameState::set_last_action_was_denied, DEFVAL(false));
    ClassDB::bind_method(D_METHOD("get_last_action_was_denied"), &YGameState::get_last_action_was_denied);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "last_action_was_denied"), "set_last_action_was_denied", "get_last_action_was_denied");

    ClassDB::bind_method(D_METHOD("get_game_action", "action_id"), &YGameState::get_game_action);
    ClassDB::bind_method(D_METHOD("get_overriding_game_action_count"), &YGameState::get_overridinge_game_action_count);
    ClassDB::bind_method(D_METHOD("get_future_game_action_count"), &YGameState::get_future_game_action_count);
    ClassDB::bind_method(D_METHOD("get_past_game_action_count"), &YGameState::get_past_game_action_count);
    ClassDB::bind_method(D_METHOD("get_previous_game_action"), &YGameState::get_previous_game_action);
    ClassDB::bind_method(D_METHOD("get_next_game_action"), &YGameState::get_next_game_action);
    ClassDB::bind_method(D_METHOD("get_past_game_action_by_index","index"), &YGameState::get_past_game_action_by_index);
    ClassDB::bind_method(D_METHOD("get_future_game_action_by_index","index"), &YGameState::get_future_game_action_by_index);
    ClassDB::bind_method(D_METHOD("get_override_game_action_by_index","index"), &YGameState::get_overriding_game_action_by_index);

    ClassDB::bind_method(D_METHOD("set_state_parameter","param_id", "param_value"), &YGameState::set_state_parameter);
    ClassDB::bind_method(D_METHOD("get_state_parameter","param_id","param_default"), &YGameState::get_state_parameter);
    ClassDB::bind_method(D_METHOD("has_state_parameter","param_id"), &YGameState::has_state_parameter);
    ClassDB::bind_method(D_METHOD("remove_state_parameter","param_id"), &YGameState::remove_state_parameter);
    ClassDB::bind_method(D_METHOD("get_all_state_parameters"), &YGameState::get_all_state_parameters);

    ClassDB::bind_method(D_METHOD("add_state_parameter_authority","param_id","network_id_authority"), &YGameState::add_state_parameter_authority);
    ClassDB::bind_method(D_METHOD("remove_state_parameter_authority","param_id","network_id_authority"), &YGameState::remove_state_parameter_authority);
    ClassDB::bind_method(D_METHOD("has_state_parameter_authority","param_id","network_id_authority"), &YGameState::has_state_parameter_authority);

    ClassDB::bind_method(D_METHOD("restart_action_counting"), &YGameState::restart_action_counting);
    ClassDB::bind_method(D_METHOD("get_game_actions_counted"), &YGameState::get_game_actions_counted);

    ClassDB::bind_method(D_METHOD("serialize"), &YGameState::serialize);
    ClassDB::bind_method(D_METHOD("deserialize", "data","playback_after","instant_playback"), &YGameState::deserialize,DEFVAL(false),DEFVAL(false));

    ClassDB::bind_method(D_METHOD("get_parallel_action_count"), &YGameState::get_parallel_action_count);
    ClassDB::bind_method(D_METHOD("get_parallel_action", "index"), &YGameState::get_parallel_action, DEFVAL(0));
    ClassDB::bind_method(D_METHOD("get_all_parallel_actions"), &YGameState::get_all_parallel_actions);
    ClassDB::bind_method(D_METHOD("end_parallel_action", "index"), &YGameState::end_parallel_action, DEFVAL(0));
    ClassDB::bind_method(D_METHOD("end_all_parallel_actions"), &YGameState::end_all_parallel_actions);

    ClassDB::bind_method(D_METHOD("add_parallel_game_action", "game_action", "desired_id"), &YGameState::add_parallel_game_action, DEFVAL(-1));
    ClassDB::bind_method(D_METHOD("add_parallel_game_action_with", "game_action", "initial_parameter", "param_value", "desired_id"), &YGameState::add_parallel_game_action_with_param, DEFVAL(-1), DEFVAL(Variant{}), DEFVAL(-1));

    ADD_SIGNAL(MethodInfo("registered_action",PropertyInfo(Variant::OBJECT, "new_action", PROPERTY_HINT_RESOURCE_TYPE, "YGameAction")));
    ADD_SIGNAL(MethodInfo("changed_current_action",PropertyInfo(Variant::OBJECT, "new_action", PROPERTY_HINT_RESOURCE_TYPE, "YGameAction")));
    ADD_SIGNAL(MethodInfo("changed_turn_player_id", PropertyInfo(Variant::INT, "player_turn_id_before"),
                          PropertyInfo(Variant::INT, "player_turn_id_after")));
    ADD_SIGNAL(MethodInfo("state_parameter_changed", PropertyInfo(Variant::INT, "param_id"), PropertyInfo(Variant::VARIANT_MAX, "old_value"), PropertyInfo(Variant::VARIANT_MAX, "new_value")));
    ADD_SIGNAL(MethodInfo("state_parameter_removed", PropertyInfo(Variant::INT, "param_id")));
    ADD_SIGNAL(MethodInfo("ended_all_actions"));
    ADD_SIGNAL(MethodInfo("player_registered",PropertyInfo(Variant::OBJECT, "new_player", PROPERTY_HINT_RESOURCE_TYPE, "YGamePlayer")));
    ADD_SIGNAL(MethodInfo("player_removed",PropertyInfo(Variant::INT, "removed_player_id")));
    ADD_SIGNAL(MethodInfo("changed_waiting_for_next_action_approval", PropertyInfo(Variant::BOOL, "is_waiting")));

    // Add RPC methods for action steps
    ClassDB::bind_method(D_METHOD("_rpc_request_action_step_approval", "action_id", "step_identifier", "step_data"), &YGameState::_rpc_request_action_step_approval);
    ClassDB::bind_method(D_METHOD("_rpc_apply_action_step", "action_id", "step_identifier", "step_data"), &YGameState::_rpc_apply_action_step);
    // Add RPC methods for game actions
    ClassDB::bind_method(D_METHOD("_rpc_register_game_action", "action_data"), &YGameState::_rpc_register_game_action);
    ClassDB::bind_method(D_METHOD("_rpc_end_game_action", "action_id"), &YGameState::_rpc_end_game_action);
    ClassDB::bind_method(D_METHOD("_rpc_mark_action_finished", "action_id"), &YGameState::_rpc_mark_action_finished);
    // Add new RPC methods for action approval
    ClassDB::bind_method(D_METHOD("_rpc_request_start_action_approval", "action_id"), &YGameState::_rpc_request_start_action_approval);
    ClassDB::bind_method(D_METHOD("_rpc_response_start_action_approval", "action_id", "desired_action_id", "approved"), &YGameState::_rpc_response_start_action_approval);
    // Add new RPC methods for state parameters
    ClassDB::bind_method(D_METHOD("_rpc_set_state_parameter", "param_id", "param_value"), &YGameState::_rpc_set_state_parameter);
    ClassDB::bind_method(D_METHOD("_rpc_remove_state_parameter", "param_id"), &YGameState::_rpc_remove_state_parameter);

    ClassDB::bind_method(D_METHOD("mark_action_finished_and_sync", "action_id"), &YGameState::mark_action_finished_and_sync);

    


// broadcast_call_on_game_action
    {
        MethodInfo mi;
        mi.name = "_receive_call_on_game_action";
        mi.arguments.push_back(PropertyInfo(Variant::CALLABLE, "method"));

        ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, "_receive_call_on_game_action", &YGameState::_receive_call_on_game_action, mi);
    }
    {
        MethodInfo mi;
        mi.name = "_receive_call_on_game_action_also_local";
        mi.arguments.push_back(PropertyInfo(Variant::CALLABLE, "method"));

        ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, "_receive_call_on_game_action_also_local", &YGameState::_receive_call_on_game_action, mi);
    }
    {
        MethodInfo mi;
        mi.name = "broadcast_call_on_game_action";
        mi.arguments.push_back(PropertyInfo(Variant::CALLABLE, "method"));

        ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, "broadcast_call_on_game_action", &YGameState::broadcast_call_on_game_action, mi);
    }

    {
        MethodInfo mi;
        mi.name = "broadcast_call_on_game_action_and_execute";
        mi.arguments.push_back(PropertyInfo(Variant::CALLABLE, "method"));

        ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, "broadcast_call_on_game_action_and_execute", &YGameState::broadcast_call_on_game_action_and_execute, mi);
    }

}

Ref<YGameAction> YGameState::get_game_action(int netid) const {
        if (current_game_action.is_valid() && current_game_action->get_unique_id() == netid) {
            return current_game_action;
        }
        for (const auto& action : past_game_actions) {
            if (action.is_valid() && action->get_unique_id() == netid) {
                return action;
            }
        }
        for (const auto& action : future_game_actions) {
            if (action.is_valid() && action->get_unique_id() == netid) {
                return action;
            }
        }
        for (const auto& action : overriding_game_actions) {
            if (action.is_valid() && action->get_unique_id() == netid) {
                return action;
            }
        }
        for (const auto& action : current_parallel_actions) {
            if (action.is_valid() && action->get_unique_id() == netid) {
                return action;
            }
        }
        return nullptr;
    }

YGameState *YGameState::get_singleton() {
    return singleton;
}

String YGameState::get_current_game_action_name() {
    if (current_game_action == nullptr) return "NULL";
    return current_game_action->get_name();
}

int YGameState::register_player(YGamePlayer *ygp, String rename_to) {
    if (ygp != nullptr) {
       // int started_with = game_players.size();
        if (!rename_to.is_empty()) {
            if (rename_to.contains("%d")) {
                rename_to = vformat(rename_to, next_player_unique_id);
            }
            ygp->set_name(rename_to);
        }
        ygp->player_id = next_player_unique_id;
        next_player_unique_id++;
        if (game_players.has(ygp->player_id)) {
            for (int i = 0; i < 5000; ++i) {
                ygp->player_id = next_player_unique_id;
                next_player_unique_id++;
                if (!game_players.has(ygp->player_id))
                    break;
            }
        }
        game_players[ygp->player_id] = ygp;
        ygp->player_registered();
        emit_signal("player_registered",ygp);
        //print_line("Called register player started with ",started_with," ended with ",game_players.size());
        return ygp->player_id;
    }
    return -1;
}

int YGameState::register_player_specific_id(YGamePlayer *ygp,int desired_id, String rename_to) {
    if (ygp != nullptr) {
        if (!rename_to.is_empty()) {
            if (rename_to.contains("%d")) {
                rename_to = vformat(rename_to, desired_id);
            }
            ygp->set_name(rename_to);
        }
        ygp->player_id = desired_id;
        if (next_player_unique_id <= desired_id)
            next_player_unique_id = desired_id+1;
        game_players[ygp->player_id] = ygp;
        ygp->player_registered();
        emit_signal("player_registered",ygp);
        return ygp->player_id;
    }
    return -1;
}

bool YGameState::remove_player_with_id(int ygp_id) {
    if (game_players.has(ygp_id)) {
        emit_signal("player_removed",ygp_id);
        game_players.erase(ygp_id);
        return true;
    }
    return false;
}

bool YGameState::set_player_id(YGamePlayer *ygp, int new_pid) {
    if (ygp == nullptr) {
        return false;
    }
    for (const auto& pl: game_players) {
        if (pl.value != nullptr && ygp == pl.value) {
            game_players.erase(pl.key);
            ygp->player_id = new_pid;
            game_players[new_pid] = ygp;
            return true;
        }
    }
    game_players[new_pid] = ygp;
    ygp->player_id = new_pid;
    return true;
}

bool YGameState::remove_player(YGamePlayer *ygp) {
    for (const auto& pl: game_players) {
        if (pl.value != nullptr && ygp == pl.value) {
            emit_signal("player_removed",ygp->player_id);
            game_players.erase(pl.key);
            ygp->player_id = -1;
            return true;
            break;
        }
    }
    return false;
}

YGamePlayer * YGameState::get_game_player(int ypg_id) {
    if (game_players.has(ypg_id)) {
        return game_players[ypg_id];
    }
    return nullptr;
}

YGamePlayer * YGameState::get_game_player_by_controller(int ypg_id) {
    for (const auto& game_player: game_players) {
        if (game_player.value != nullptr && game_player.value->player_controller == ypg_id) return game_player.value;
    }
    return nullptr;
}

TypedArray<YGamePlayer> YGameState::get_all_players() {
    TypedArray<YGamePlayer> arr;
    for (const KeyValue<int, YGamePlayer*> &E : game_players) {
        if (E.value != nullptr) {
            arr.append(E.value);
        }
    }
    return arr;
}

YGamePlayer * YGameState::get_game_player_by_type(int ypg_type) {
    for (const auto& game_player: game_players) {
        if (game_player.value != nullptr && game_player.value->player_type == ypg_type) return game_player.value;
    }
    return nullptr;
}

void YGameState::do_process(double delta) {
    // Check if any future parallel actions can start
    check_future_parallel_actions();

    // Process parallel actions first
    for (int i = current_parallel_actions.size() - 1; i >= 0; i--) {
        Ref<YGameAction> parallel_action = current_parallel_actions[i];
        if (!parallel_action.is_valid()) {
            current_parallel_actions.remove_at(i);
            continue;
        }

        if (parallel_action->finished && !parallel_action->waiting_for_step) {
            end_parallel_action(i);
            continue;
        }

        // Process steps and actions similar to regular actions
        if (!parallel_action->waiting_for_step_no_processing) {
            if (!parallel_action->process_action(static_cast<float>(delta)) && !parallel_action->waiting_for_step) {
                end_parallel_action(i);
                continue;
            }

            if (frame_count_before_doing_slow_process > 0.5) {
                if (!parallel_action->slow_process_action(static_cast<float>(delta)) && !parallel_action->waiting_for_step) {
                    end_parallel_action(i);
                    continue;
                }
            }
        }
    }

    if (current_game_action.is_valid()) {
        //HAS A CURRENT GAMEACTION!!
        //Has it ended?

        if (current_game_action->finished  && !current_game_action->waiting_for_step) {
            if (wait_before_going_to_next_action > 0.0) {
                wait_before_going_to_next_action-=delta;
                return;
            }
            current_game_action->end_action(false);
            game_actions_done_since_started_counting+=1;
            if (current_game_action->can_be_serialized) {
                past_game_actions.push_back(current_game_action);
            }
            current_game_action.unref();
            return;
        }

        //IF WAITING BEFORE GOING TO NEXT STEP THEN DON'T GO TO NEXT STEP!
        if (wait_before_going_to_next_step > 0.0) {
            wait_before_going_to_next_step -= delta;
        } else {
            //IF THERE'S A NEW STEP AND WE'RE NOT CURRENTLY WAITING FOR A STEP TO CONCLUDE THEN GO TO NEXT STEP!!
            if (current_game_action->steps_consumed < current_game_action->action_steps.size()) {
                if (!current_game_action->waiting_for_step) {
                    if (!current_game_action->action_steps[current_game_action->steps_consumed].is_valid()) {
                        ERR_PRINT("YGameState Attempted to take an action step that wasn't valid");
                        current_game_action->steps_consumed += 1;
                    } else {
                        auto current_possible_step = current_game_action->action_steps[current_game_action->steps_consumed];
                        if (current_possible_step.is_valid()) {
                            // THIS STEP WAS ALREADY TAKEN, CONSUME!
                            if (current_possible_step->get_step_taken()) {
                                current_game_action->steps_consumed+=1;
                            }
                            else {
                                if (is_playing_back && stop_playing_back_when_current_action_steps_done && current_game_action->steps_consumed + 1 >= current_game_action->action_steps.size()) {
                                    stop_playing_back_when_current_action_steps_done = false;
                                    is_playing_back = false;
                                    current_game_action->is_playing_back = false;
                                }
                                current_game_action->step_action(current_game_action->action_steps[current_game_action->steps_consumed], false);
                            }
                        }
                    }
                }
            } else {
                if (is_playing_back && stop_playing_back_when_current_action_steps_done) {
                    stop_playing_back_when_current_action_steps_done = false;
                    is_playing_back = false;
                }
            }
        }

        //IF WAITING FOR STEP AND NOT PROCESSING DON'T DO PROCESSING
        if (!current_game_action->waiting_for_step_no_processing) {
            if(!current_game_action->process_action(static_cast<float>(delta)) && !current_game_action->waiting_for_step) {
                current_game_action->end_action(false);
                game_actions_done_since_started_counting+=1;
                if (current_game_action->can_be_serialized) {
                    past_game_actions.push_back(current_game_action);
                }
                current_game_action.unref();
                return;
            }

            frame_count_before_doing_slow_process+=delta;

            if (frame_count_before_doing_slow_process > 0.5) {
                frame_count_before_doing_slow_process=0;
                if(!current_game_action->slow_process_action(static_cast<float>(delta)) && !current_game_action->waiting_for_step) {
                    current_game_action->end_action(false);
                    game_actions_done_since_started_counting+=1;
                    if (current_game_action->can_be_serialized) {
                        past_game_actions.push_back(current_game_action);
                    }
                    current_game_action.unref();
                    return;
                }
            }
        }
    } else {
        if (prevent_proceed_to_next_action > 0)
            return;
        if (wait_before_going_to_next_action > 0.0) {
            wait_before_going_to_next_action-=delta;
            return;
        }
        int overriding_game_actions_remaining = static_cast<int>(overriding_game_actions.size());
        int future_game_actions_remaining = static_cast<int>(future_game_actions.size());

        if (is_playing_back && ((overriding_game_actions_remaining + future_game_actions_remaining == 1) || (!future_game_actions.is_empty() && future_game_actions[0]->unique_id == stop_playing_back_at_id))) {
            stop_playing_back_when_current_action_steps_done = true;
            if (!future_game_actions.is_empty() && future_game_actions[0]->unique_id == stop_playing_back_at_id)
                overriding_game_actions_remaining = 0;
        }

        if (overriding_game_actions_remaining > 0 && !is_playing_back) {
            Ref<YGameAction> next_action = overriding_game_actions[0];
            
            if (next_action.is_valid()) {
                if (has_valid_multiplayer_peer() && scene_multiplayer->get_unique_id() != 1) {
                    if (debugging_level >= 2) {
                        print_line(vformat("[YGameState %d] Checking override action %d approval state: %d", scene_multiplayer->get_unique_id(), next_action->get_unique_id(), next_action->remote_start_approval));
                    }
                    switch (next_action->remote_start_approval) {
                        case YGameAction::RemoteStartApproval::REMOTE_START_APPROVAL_PENDING:
                            set_is_waiting_for_next_action_approval(true);
                            pending_waiting_for_start_approval_attempts++;
                            if (pending_waiting_for_start_approval_attempts > 20) {
                                if (debugging_level >= 2) {
                                    print_line(vformat("[YGameState %d] Too many pending waiting for start approval attempts, requesting again.", scene_multiplayer->get_unique_id()));
                                }
                                next_action->remote_start_approval = YGameAction::RemoteStartApproval::REMOTE_START_APPROVAL_NOT_REQUESTED;
                                pending_waiting_for_start_approval_attempts = 0;
                            } else {
                                if (debugging_level >= 2 && pending_waiting_for_start_approval_attempts == 1) {
                                    print_line(vformat("[YGameState %d] Action %d approval still pending, waiting...", scene_multiplayer->get_unique_id(), next_action->get_unique_id()));
                                }
                            }
                            return;
                        case YGameAction::RemoteStartApproval::REMOTE_START_APPROVAL_APPROVED:
                            if (debugging_level >= 2) {
                                print_line(vformat("[YGameState %d] Action approved, proceeding", scene_multiplayer->get_unique_id()));
                            }
                            break;
                        case YGameAction::RemoteStartApproval::REMOTE_START_APPROVAL_DENIED:
                            if (debugging_level >= 2) {
                                print_line(vformat("[YGameState %d] Action denied, will retry", scene_multiplayer->get_unique_id()));
                            }
                            next_action->remote_start_approval = YGameAction::RemoteStartApproval::REMOTE_START_APPROVAL_NOT_REQUESTED;
                            return;
                        default:
                            // REQUEST ACTION APPROVAL
                            request_action_approval(next_action.ptr());
                            return;
                    }
                }

                overriding_game_actions.remove_at(0);
                set_is_waiting_for_next_action_approval(false);
                if (next_action->runs_parallel) {
                    current_parallel_actions.push_back(next_action);
                    next_action->enter_action();
                } else {
                    current_game_action = next_action;
                    if (current_game_action.is_valid()) {
                        if (last_turn_player_id != current_game_action->player_turn) {
                            emit_signal("changed_turn_player_id",last_turn_player_id,current_game_action->player_turn);
                            last_turn_player_id = current_game_action->player_turn;
                        }
                        emit_signal("changed_current_action",current_game_action);
                        current_game_action->enter_action();
                    }
                    return;
                }
            } else {
                overriding_game_actions.remove_at(0);
            }
        }

        if (future_game_actions_remaining > 0) {
            Ref<YGameAction> next_action = future_game_actions[0];
            
            if (next_action.is_valid()) {
                // Check if this is a remote player's action that needs approval
                if (has_valid_multiplayer_peer() && scene_multiplayer->get_unique_id() != 1 && !is_playing_back) {
                    if (debugging_level >= 2) {
                        print_line(vformat("[YGameState %d] Checking future action %d approval state: %d", scene_multiplayer->get_unique_id(), next_action->get_unique_id(), next_action->remote_start_approval));
                    }
                    switch (next_action->remote_start_approval) {
                        case YGameAction::RemoteStartApproval::REMOTE_START_APPROVAL_PENDING:
                            set_is_waiting_for_next_action_approval(true);
                            pending_waiting_for_start_approval_attempts++;
                            if (pending_waiting_for_start_approval_attempts > 20) {
                                if (debugging_level >= 2) {
                                    print_line(vformat("[YGameState %d] Too many pending waiting for start approval attempts, requesting again.", scene_multiplayer->get_unique_id()));
                                }
                                next_action->remote_start_approval = YGameAction::RemoteStartApproval::REMOTE_START_APPROVAL_NOT_REQUESTED;
                                pending_waiting_for_start_approval_attempts = 0;
                            } else {
                                if (debugging_level >= 2 && pending_waiting_for_start_approval_attempts == 1) {
                                    print_line(vformat("[YGameState %d] Action %d approval still pending, waiting...", scene_multiplayer->get_unique_id(), next_action->get_unique_id()));
                                }
                            }
                            return;
                        case YGameAction::RemoteStartApproval::REMOTE_START_APPROVAL_APPROVED:
                            if (debugging_level >= 2) {
                                print_line(vformat("[YGameState %d] Action %d approved, proceeding", scene_multiplayer->get_unique_id(), next_action->get_unique_id()));
                            }
                            break;
                        case YGameAction::RemoteStartApproval::REMOTE_START_APPROVAL_DENIED:
                            if (debugging_level >= 2) {
                                print_line(vformat("[YGameState %d] Action %d denied, will retry", scene_multiplayer->get_unique_id(), next_action->get_unique_id()));
                            }
                            next_action->remote_start_approval = YGameAction::RemoteStartApproval::REMOTE_START_APPROVAL_NOT_REQUESTED;
                            return;
                        default:
                            // REQUEST ACTION APPROVAL IF NOT APPROVED OR DENIED
                            request_action_approval(next_action.ptr());
                            return;
                    }
                }

                future_game_actions.remove_at(0);

                set_is_waiting_for_next_action_approval(false);
                if (next_action->runs_parallel) {
                    current_parallel_actions.push_back(next_action);
                    next_action->enter_action();
                } else {
                    current_game_action = next_action;
                    if (current_game_action.is_valid()) {
                        if (stop_playing_back_when_current_action_steps_done) {
                            is_playing_back = false;
                            current_game_action->is_playing_back = false;
                            current_game_action->instant_execute = false;
                        } else if(is_playing_back) {
                            current_game_action->is_playing_back = true;
                        }
                        int current_action_steps = current_game_action->get_all_steps_count();
                        if ((is_playing_back || stop_playing_back_when_current_action_steps_done) && current_action_steps == 0 && get_overridinge_game_action_count() == 0 && get_future_game_action_count() == 0) {
                            stop_playing_back_when_current_action_steps_done=false;
                            is_playing_back = false;
                            current_game_action->is_playing_back = false;
                        }

                        emit_signal("changed_current_action",current_game_action);
                        current_game_action->enter_action();

                        if (stop_playing_back_when_current_action_steps_done && current_action_steps > 0) {
                            is_playing_back = true;
                            current_game_action->is_playing_back =true;
                        }
                        return;
                    }
                }
            } else {
                future_game_actions.remove_at(0);
            }
        }
        if (!showed_out_of_actions_message) {
            showed_out_of_actions_message=true;
            emit_signal("ended_all_actions");
        }
    }
}

bool YGameState::end_current_game_action() const {
        if (has_current_game_action()) {
            current_game_action->end_action(false);
            return true;
        }
        return false;
}

Ref<YGameAction> YGameState::add_game_action(const Ref<YGameAction> &ygs, int desired_game_state_id) {
    if (is_playing_back) {
        return ygs;
    }
    return add_game_action_with_param(ygs,-1,Variant{},desired_game_state_id);
}

void YGameState::set_current_turn_player(YGamePlayer* _new_current_player)
     {
        if (current_turn_player != nullptr) {
            current_turn_player->turn_ended();
            current_turn_player->owns_current_turn=false;
            current_turn_player = nullptr;
        }

        if (_new_current_player != nullptr) {
            current_turn_player = _new_current_player;
            current_turn_player->owns_current_turn=true;
            current_turn_player->turn_started();
        }
    }

    void YGameState::clear_all_game_actions()
    {
        pending_waiting_for_start_approval_attempts = 0;
        if (!current_game_action.is_null() && current_game_action.is_valid()) {
            current_game_action = Ref<YGameAction>();
        }
        last_turn_player_id = -1;
        if (current_turn_player != nullptr) {
            current_turn_player = nullptr;
        }
        overriding_game_actions.clear();
        future_game_actions.clear();
        past_game_actions.clear();
        current_parallel_actions.clear();
        future_parallel_actions.clear();
        showed_out_of_actions_message=false;
    }

void YGameState::clear_all_players()
     {
        for (auto gmp: game_players) {
            if (gmp.value != nullptr) {
                if (current_turn_player != nullptr && current_turn_player == gmp.value)
                    current_turn_player = nullptr;
                gmp.value->queue_free();
            }
        }
        game_players.clear();
        if (current_turn_player != nullptr) {
            current_turn_player = nullptr;
        }
    }

Ref<YGameAction> YGameState::add_game_action_with_param(const Ref<YGameAction> &ygs, int desired_initial_param, const Variant &desired_param_data, int desired_game_state_id) {
    if (is_playing_back) {
        return ygs;
    }

    // Check if we're in multiplayer and if only server can register actions
    if (has_valid_multiplayer_peer()) {
        if (scene_multiplayer->get_unique_id() != 1) {
            // Client trying to register action - ignore
            return ygs;
        }
    }

    showed_out_of_actions_message=false;
    if (desired_initial_param != -1) {
        ygs->set_action_parameter(desired_initial_param,desired_param_data);
    }
    if(!has_started) {
        YEngine::get_singleton()->game_state_starting(this);
        has_started=true;
    }
    if (desired_game_state_id == -1) {
        desired_game_state_id = next_game_action_unique_id;
        next_game_action_unique_id++;
    }
    future_game_actions.push_back(ygs);
    ygs->set_unique_id(desired_game_state_id);
    ygs->created();
    
    // If we're the server, broadcast the action to all clients
    if (has_valid_multiplayer_peer() && 
        scene_multiplayer->get_unique_id() == 1) {
        Dictionary action_data = ygs->serialize();
        action_data["a_list_type"] = 2;
        broadcast_game_action(action_data);
    }

    emit_signal("registered_action",ygs);

    return ygs;
}

Ref<YGameAction> YGameState::add_override_game_action(const Ref<YGameAction> &ygs, int desired_game_state_id) {
    if (is_playing_back) {
        return ygs;
    }
    return add_override_game_action_with_param(ygs,-1,Variant{},desired_game_state_id);
}

Ref<YGameAction> YGameState::add_override_game_action_with_param(const Ref<YGameAction> &ygs, int desired_initial_param, const Variant &desired_param_data, int desired_game_state_id) {
    if (is_playing_back) {
        return ygs;
    }

    // Check if we're in multiplayer and if only server can register actions
    if (has_valid_multiplayer_peer()) {
        if (scene_multiplayer->get_unique_id() != 1) {
            // Client trying to register action - ignore
            return ygs;
        }
    }

    showed_out_of_actions_message=false;
    if (desired_initial_param != -1) {
        ygs->set_action_parameter(desired_initial_param,desired_param_data);
    }
    if(!has_started) {
        YEngine::get_singleton()->game_state_starting(this);
        has_started=true;
    }
    if (desired_game_state_id == -1) {
        desired_game_state_id = next_game_action_unique_id;
        next_game_action_unique_id++;
    }
    if (overriding_game_actions.size()>0) {
        if (overriding_game_actions[0]->has_priority >= 0 && ygs->has_priority < overriding_game_actions[0]->has_priority) {
            // Should keep looking on the others until finding an ideal place to add where either it doesn't have priority or their priority is smaller than mine.
            for (int i = 1; i < overriding_game_actions.size(); i++) {
                if (overriding_game_actions[i]->has_priority >= 0 && ygs->has_priority >= overriding_game_actions[i]->has_priority) {
                    overriding_game_actions.insert(i,ygs);
                    break;
                }
            }
            // If we didn't find a place to add, add it to the end.
            overriding_game_actions.push_back(ygs);
        } else {
            overriding_game_actions.insert(0,ygs);
        }
    }
    else
        overriding_game_actions.append(ygs);
    ygs->set_unique_id(desired_game_state_id);
    ygs->created();

    // If we're the server, broadcast the action to all clients
    if (has_valid_multiplayer_peer() && 
        scene_multiplayer->get_unique_id() == 1) {
        Dictionary action_data = ygs->serialize();
        action_data["a_list_type"] = 1;
        broadcast_game_action(action_data);
    }

    emit_signal("registered_action",ygs);

    return ygs;
}

Dictionary YGameState::serialize() {
    Dictionary dict;

    dict["next_game_action_unique_id"] = next_game_action_unique_id;
    dict["next_visual_action_unique_id"] = next_visual_action_unique_id;
    dict["next_player_unique_id"] = next_player_unique_id;
    dict["next_visual_element_unique_id"] = next_visual_element_unique_id;

    Dictionary serialized_players;
    for (const auto& [key, player] : game_players) {
        serialized_players[Variant(key)] = player->serialize();
    }
    dict["players"] = serialized_players;

    Array overriding_game_actions_array;
    for (const auto& game_action : overriding_game_actions) {
        if (game_action->can_be_serialized) {
            overriding_game_actions_array.push_back(game_action->serialize());
        }
    }

    Array future_game_actions_array;
    for (const auto& game_action : future_game_actions) {
        if (game_action->can_be_serialized) {
            future_game_actions_array.push_back(game_action->serialize());
        }
    }

    Array past_game_actions_array;
    for (const auto& game_action : past_game_actions) {
        if (game_action->can_be_serialized) {
            past_game_actions_array.push_back(game_action->serialize());
        }
    }

    Dictionary state_parameters_dict;
    for (auto& entry : state_parameters) {
        state_parameters_dict[Variant(entry.key)] = entry.value;
    }
    dict["state_parameters"] = state_parameters_dict;

    dict["override_actions"] = overriding_game_actions_array;
    dict["future_actions"] = future_game_actions_array;
    dict["past_actions"] = past_game_actions_array;

    if (!current_game_action.is_null() && current_game_action.is_valid() && current_game_action->can_be_serialized)
        dict["current_action"] = current_game_action->serialize();

    if (!current_parallel_actions.is_empty()) {
        Array parallel_actions_array;
        for (const auto& action : current_parallel_actions) {
            if (action->can_be_serialized) {
                parallel_actions_array.push_back(action->serialize());
            }
        }
        if (!parallel_actions_array.is_empty()) {
            dict["parallel_actions"] = parallel_actions_array;
        }
    }

    if (!future_parallel_actions.is_empty()) {
        Array future_parallel_array;
        for (const auto& action : future_parallel_actions) {
            if (action->can_be_serialized) {
                future_parallel_array.push_back(action->serialize());
            }
        }
        if (!future_parallel_array.is_empty()) {
            dict["future_parallel_actions"] = future_parallel_array;
        }
    }
    return dict;
}

void YGameState::deserialize_individual_game_action_into(Vector<Ref<YGameAction>> &list_into, const Dictionary &action_dict, bool p_front_instead = false) {
    String class_name = action_dict.get("class","");
    Ref<YGameAction> action;
    action.instantiate();
    if (!class_name.is_empty() && YEngine::get_singleton()->class_name_to_script_path.has(class_name)) {
        Ref<Script> gdScript = ResourceLoader::load(YEngine::get_singleton()->class_name_to_script_path[class_name], "Script");
        if (gdScript.is_valid()) {
            auto temp_script_instance = gdScript->instance_create(action.ptr());
            if (temp_script_instance != nullptr) {
                action->set_script_instance(temp_script_instance);
                action->set_name(class_name);
            }
        }
    }
    action->deserialize(action_dict);
    if (p_front_instead){
        if (action->has_priority >= 0 && list_into[0]->has_priority >= 0 && action->has_priority < list_into[0]->has_priority) {
            // Should keep looking on the others until finding an ideal place to add where either it doesn't have priority or their priority is smaller than mine.
            for (int i = 1; i < list_into.size(); i++) {
                if (list_into[i]->has_priority >= 0 && action->has_priority >= list_into[i]->has_priority) {
                    list_into.insert(i,action);
                    break;
                }
            }
            // If we didn't find a place to add, add it to the end.
            list_into.push_back(action);
        } else {
            list_into.insert(0,action);
        }
    }
    else
        list_into.push_back(action);
}

void YGameState::deserialize_game_actions_into(Vector<Ref<YGameAction>> &list_into, Array serialized_game_actions) {
    for (int i = 0; i < serialized_game_actions.size(); i++) {
        Dictionary action_dict = serialized_game_actions[i];
        deserialize_individual_game_action_into(list_into, action_dict);
    }
}

void YGameState::set_is_waiting_for_next_action_approval(bool b) {
    if (b != is_waiting_for_next_action_approval) {
        is_waiting_for_next_action_approval = b;
        emit_signal(SNAME("changed_waiting_for_next_action_approval"), b);
    }
}

Dictionary YGameState::deserialize(const Dictionary dict, bool p_playback_after = false, bool p_instant_playback = false) {
    clear_all_game_actions();

    if(has_started && p_playback_after) {
        has_started=false;
        YEngine::get_singleton()->using_game_state = false;
    }

    overriding_game_actions.clear();
    future_game_actions.clear();
    past_game_actions.clear();
    if (p_playback_after) {
        if (dict.has("past_actions")) {
            Array past_game_actions_array = dict["past_actions"];
            deserialize_game_actions_into(future_game_actions , past_game_actions_array);
            for (const auto& ygame_action: future_game_actions) {
                ygame_action->set_instant_execute(p_instant_playback);
            }
        }
        if (dict.has("current_action")) {
            if (p_playback_after) {
                deserialize_individual_game_action_into(future_game_actions, dict["current_action"]);
                if (!future_game_actions.is_empty()) {
                    auto should_be_current_action = future_game_actions[static_cast<int>(future_game_actions.size()-1)];
                    stop_playing_back_at_id = should_be_current_action->get_unique_id();
                }
            }
        }
        if (dict.has("override_actions")) {
            Array overriding_game_actions_array = dict["override_actions"];
            deserialize_game_actions_into(future_game_actions , overriding_game_actions_array);
        }
        if (dict.has("future_actions")) {
            Array future_game_action_array = dict["future_actions"];
            deserialize_game_actions_into(future_game_actions , future_game_action_array);
        }
    } else {
        if (dict.has("override_actions")) {
            Array overriding_game_actions_array = dict["override_actions"];
            deserialize_game_actions_into(overriding_game_actions , overriding_game_actions_array);
        }
        if (dict.has("future_actions")) {
            Array future_game_action_array = dict["future_actions"];
            deserialize_game_actions_into(future_game_actions , future_game_action_array);
        }
        if (dict.has("past_actions")) {
            Array past_game_actions_array = dict["past_actions"];
            deserialize_game_actions_into(past_game_actions , past_game_actions_array);
        }
        if (dict.has("current_action")) {
            Vector<Ref<YGameAction>> temp_current;
            deserialize_individual_game_action_into(temp_current, dict["current_action"]);
            if (!temp_current.is_empty())
                current_game_action = temp_current[0];
        }
    }

    if (dict.has("next_game_action_unique_id")) next_game_action_unique_id = dict["next_game_action_unique_id"];
    if (dict.has("next_visual_action_unique_id")) next_visual_action_unique_id = dict["next_visual_action_unique_id"];
    if (dict.has("next_player_unique_id")) next_player_unique_id = dict["next_player_unique_id"];
    if (dict.has("next_visual_element_unique_id")) next_visual_element_unique_id = dict["next_visual_element_unique_id"];

    if (dict.has("state_parameters")) {
        Dictionary state_parameters_dict = dict["state_parameters"];
        state_parameters.clear();
        for (int i = 0; i < state_parameters_dict.size(); i++) {
            Variant key = state_parameters_dict.get_key_at_index(i);
            state_parameters[key.operator int()] = state_parameters_dict.get_value_at_index(i);
        }
    }

    if (dict.has("players")) {
        Dictionary players_dict = dict["players"];
        game_players.clear();
        for (int i = 0; i < players_dict.size(); i++) {
            int key = players_dict.get_key_at_index(i);
            YGamePlayer *player = memnew(YGamePlayer); // replace with correct constructor
            Dictionary player_dict = players_dict.get_value_at_index(i);
            String player_class = player_dict["player_class"];
            if (!player_class.is_empty() && YEngine::get_singleton()->class_name_to_script_path.has(player_class)) {
                Ref<Script> gdScript = ResourceLoader::load(YEngine::get_singleton()->class_name_to_script_path[player_class], "Script");
                if (gdScript.is_valid()) {
                    auto temp_script_instance = gdScript->instance_create(player);
                    if (temp_script_instance != nullptr)
                        player->set_script_instance(temp_script_instance);
                }
            }
            player->deserialize(player_dict);
            game_players[key] = player;
            if (player_dict.has("parent_path") && YEngine::get_singleton()->has_node(NodePath{player_dict["parent_path"]})) {
                YEngine::get_singleton()->get_node(NodePath{player_dict["parent_path"]})->add_child(player);
            } else {
                YEngine::get_singleton()->add_child(player);
            }
            if (next_player_unique_id <= player->player_id) {
                next_player_unique_id = player->player_id+1;
            }
        }
    }

    if (dict.has("parallel_actions")) {
        Array parallel_actions_array = dict["parallel_actions"];
        current_parallel_actions.clear();
        deserialize_game_actions_into(current_parallel_actions, parallel_actions_array);
    }

    if (dict.has("future_parallel_actions")) {
        Array future_parallel_array = dict["future_parallel_actions"];
        future_parallel_actions.clear();
        deserialize_game_actions_into(future_parallel_actions, future_parallel_array);
    }

    if (p_playback_after) {
        is_playing_back = true;
        if(!has_started) {
            YEngine::get_singleton()->game_state_starting(this);
            has_started=true;
        }
    }

    return dict;
}

void YGameState::end_parallel_action(int index) {
    if (index >= 0 && index < current_parallel_actions.size()) {
        if (current_parallel_actions[index].is_valid()) {
            current_parallel_actions[index]->end_action(false);
            game_actions_done_since_started_counting += 1;
            if (current_parallel_actions[index]->can_be_serialized) {
                past_game_actions.push_back(current_parallel_actions[index]);
            }
        }
        current_parallel_actions.remove_at(index);
    }
}

bool YGameState::ensure_has_initialized() {
    if(!has_started) {
        YEngine::get_singleton()->game_state_starting(this);
        has_started=true;
    }
    return has_started;
}

void YGameState::end_all_parallel_actions() {
    for (int i = current_parallel_actions.size() - 1; i >= 0; i--) {
        end_parallel_action(i);
    }
}

void YGameState::check_future_parallel_actions() {
    float current_time = YTime::get_singleton()->get_time();
    
    for (int i = future_parallel_actions.size() - 1; i >= 0; i--) {
        Ref<YGameAction> action = future_parallel_actions[i];
        if (!action.is_valid()) {
            future_parallel_actions.remove_at(i);
            continue;
        }

        // Check if it's time to check this action
        if (action->last_start_check_time == 0.0f || 
            current_time > action->last_start_check_time + action->start_check_interval) {
            
            action->last_start_check_time = current_time;
            
            // Check if action can start
            if (action->only_starts_if()) {
                // Check max parallel limit
                if (action->max_in_parallel == -1 || 
                    count_current_parallel_of_type(action->get_name()) < action->max_in_parallel) {
                    
                    current_parallel_actions.push_back(action);
                    action->enter_action();
                    future_parallel_actions.remove_at(i);
                }
            }
        }
    }
}

bool YGameState::has_valid_multiplayer_peer() const {
    return !scene_multiplayer.is_null() && scene_multiplayer.is_valid() && scene_multiplayer->has_multiplayer_peer();
}

void YGameState::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
        } break;
        case NOTIFICATION_EXIT_TREE: {
            this->clear_all_game_actions();
            this->clear_all_players();
            break;
        }
        case NOTIFICATION_READY: {
            scene_multiplayer = get_multiplayer();
            this->rpc_config(rpc_apply_action_step_stringname,create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_AUTHORITY, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
            this->rpc_config(rpc_register_game_action_stringname,create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_AUTHORITY, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
            this->rpc_config(rpc_request_action_step_approval_stringname,create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_ANY_PEER, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
            this->rpc_config(rpc_end_game_action_stringname,create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_AUTHORITY, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
            this->rpc_config(rpc_mark_action_finished_stringname,create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_AUTHORITY, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
            this->rpc_config(_receive_call_on_game_action_stringname,create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_ANY_PEER, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
            this->rpc_config(_receive_call_on_game_action_also_local_stringname,create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_ANY_PEER, MultiplayerPeer::TRANSFER_MODE_RELIABLE, true, 0));
            this->rpc_config(rpc_request_start_action_approval_stringname,create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_ANY_PEER, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
            this->rpc_config(rpc_response_start_action_approval_stringname,create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_ANY_PEER, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
            this->rpc_config(rpc_set_state_parameter_stringname,create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_ANY_PEER, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
            this->rpc_config(rpc_remove_state_parameter_stringname,create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_ANY_PEER, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
        }
    }
}

int YGameState::count_current_parallel_of_type(const String& action_name) const {
    int count = 0;
    for (const Ref<YGameAction>& action : current_parallel_actions) {
        if (action.is_valid() && action->get_name() == action_name) {
            count++;
        }
    }
    return count;
}

Ref<YGameAction> YGameState::add_parallel_game_action(const Ref<YGameAction>& ygs, int desired_game_state_id) {
    return add_parallel_game_action_with_param(ygs, -1, Variant{}, desired_game_state_id);
}

Ref<YGameAction> YGameState::add_parallel_game_action_with_param(const Ref<YGameAction>& ygs, int desired_initial_param, const Variant& desired_param_data, int desired_game_state_id) {
    if (is_playing_back) {
        return ygs;
    }

    if (desired_initial_param != -1) {
        ygs->set_action_parameter(desired_initial_param, desired_param_data);
    }

    if (!has_started) {
        YEngine::get_singleton()->game_state_starting(this);
        has_started = true;
    }

    if (desired_game_state_id == -1) {
        desired_game_state_id = next_game_action_unique_id;
        next_game_action_unique_id++;
    }

    ygs->runs_parallel = true;
    ygs->set_unique_id(desired_game_state_id);
    ygs->created();
    future_parallel_actions.push_back(ygs);
    emit_signal("registered_action", ygs);
    return ygs;
}


void YGameState::request_action_step_approval(YGameAction* action, int step_identifier, const Variant& step_data) {
    if (!has_valid_multiplayer_peer() || scene_multiplayer->get_unique_id() == 1) {
        return; // Don't send if we're the server
    }

    Array p_arguments;
    p_arguments.push_back(action->get_unique_id());
    p_arguments.push_back(step_identifier);
    p_arguments.push_back(step_data);
    int argcount = p_arguments.size();
    const Variant **argptrs = (const Variant **)alloca(sizeof(Variant *) * argcount);
    for (int i = 0; i < argcount; i++) {
        argptrs[i] = &p_arguments[i];
    }
    rpcp(1, "_rpc_request_action_step_approval", argptrs, argcount);
}

void YGameState::broadcast_action_step(YGameAction* action, int step_identifier, const Variant& step_data) {
    if (!has_valid_multiplayer_peer() || scene_multiplayer->get_unique_id() != 1) {
        return; // Only server can broadcast
    }

    Array p_arguments;
    p_arguments.push_back(action->get_unique_id());
    p_arguments.push_back(step_identifier);
    p_arguments.push_back(step_data);
    int argcount = p_arguments.size();
    const Variant **argptrs = (const Variant **)alloca(sizeof(Variant *) * argcount);
    for (int i = 0; i < argcount; i++) {
        argptrs[i] = &p_arguments[i];
    }
    rpcp(0, "_rpc_apply_action_step", argptrs, argcount);
}

void YGameState::_rpc_request_action_step_approval(int action_id, int step_identifier, const Variant& step_data) {
    if (!has_valid_multiplayer_peer() || scene_multiplayer->get_unique_id() != 1) {
        return; // Only server should handle approval requests
    }

    Ref<YGameAction> action = get_game_action(action_id);
    if (action.is_valid()) {
        if (action->finished && past_game_actions.has(action)) {
            if (action->is_debugging) {
                print_line(vformat("Step %d for action %d was already finished and in the past, so we don't need to do anything.", step_identifier, action_id));
            }
            // In this case the action is already finished and in the past, so we don't need to do anything.
            return;
        }

        if (action->check_if_has_step_approval(step_identifier, step_data, scene_multiplayer->get_remote_sender_id())) {
            action->actually_register_step(step_identifier, step_data, true);
            // If we're online and we're the server.
            if (YGameState::get_singleton() != nullptr && YGameState::get_singleton()->get_multiplayer()->has_multiplayer_peer() && 
                YGameState::get_singleton()->get_multiplayer()->get_unique_id() == 1) {
                YGameState::get_singleton()->broadcast_action_step(action.ptr(), step_identifier, step_data);
            }
        }
        return;
    }

    print_error(vformat("Received requested step approval for step %d for action %d, but action not found", step_identifier, action_id));
}

void YGameState::_rpc_apply_action_step(int action_id, int step_identifier, const Variant& step_data) {
    if (!has_valid_multiplayer_peer() || scene_multiplayer->get_unique_id() == 1) {
        return; // Server already applied the step
    }

    Ref<YGameAction> action = get_game_action(action_id);
    if (action.is_valid()) {
        action->actually_register_step(step_identifier, step_data, true);
        return;
    }

    print_error(vformat("Received step %d for action %d, but action not found", step_identifier, action_id));
}

void YGameState::broadcast_game_action(const Dictionary& action_data) {
    if (!has_valid_multiplayer_peer() || scene_multiplayer->get_unique_id() != 1) {
        return; // Only server can broadcast
    }
    
    // Broadcast to all clients
    rpc(rpc_register_game_action_stringname, action_data);
}

void YGameState::_rpc_register_game_action(const Dictionary& action_data) {
    if (!has_valid_multiplayer_peer() || scene_multiplayer->get_unique_id() == 1) {
        return;
    }

    if (action_data.has("a_list_type")) {
        int a_list_type = action_data["a_list_type"];
        Vector<Ref<YGameAction>>* list_to_add_to = nullptr;
        bool p_front_instead = false;
        if (a_list_type == 1) {
            list_to_add_to = &overriding_game_actions;
            p_front_instead = true;
        } else if (a_list_type == 2) {
            list_to_add_to = &future_game_actions;
        } else if (a_list_type == 3) {
            list_to_add_to = &past_game_actions;
        }else if (a_list_type == 4) {
            list_to_add_to = &current_parallel_actions;
        }else if (a_list_type == 5) {
            list_to_add_to = &future_parallel_actions;
        }
        if (list_to_add_to != nullptr) {
            deserialize_individual_game_action_into(*list_to_add_to, action_data, p_front_instead);
        }
    }
}

Dictionary YGameState::create_rpc_dictionary_config(MultiplayerAPI::RPCMode p_rpc_mode,
                                            MultiplayerPeer::TransferMode p_transfer_mode, bool p_call_local,
                                            int p_channel) {
    Dictionary config;
    config["rpc_mode"] = p_rpc_mode;
    config["transfer_mode"] = p_transfer_mode;
    config["call_local"] = p_call_local;
    config["channel"] = p_channel;
    return config;
}

void YGameState::broadcast_action_end(int action_id) {
    if (!has_valid_multiplayer_peer() || scene_multiplayer->get_unique_id() != 1) {
        return; // Only server can broadcast
    }

    rpc(rpc_end_game_action_stringname, action_id);
}

void YGameState::_rpc_end_game_action(int action_id) {
    if (!has_valid_multiplayer_peer() || scene_multiplayer->get_unique_id() == 1) {
        return; // Server already ended the action or the one sending is not the server
    }

    // Check current action
    if (has_current_game_action() && get_current_game_action()->get_unique_id() == action_id) {
        get_current_game_action()->end_action(false);
        return;
    }

    // Check parallel actions
    for (int i = 0; i < get_parallel_action_count(); i++) {
        Ref<YGameAction> action = get_parallel_action(i);
        if (action.is_valid() && action->get_unique_id() == action_id) {
            action->end_action(false);
            return;
        }
    }
}

YGameState::YGSRPCConfig YGameState::_get_rpc_config(Object *p_node, const StringName &p_method) {
    const ObjectID oid = p_node->get_instance_id();
    // If we don't have a cache for this node yet, create it
    if (!rpc_config_cache.has(oid)) {
        YGSRPCConfigCache cache;
        // Parse script config if it exists
        if (p_node->get_script_instance()) {
            Dictionary script_config = p_node->get_script_instance()->get_rpc_config();
            _parse_rpc_config(script_config, false, cache);
        }
        rpc_config_cache[oid] = cache;
    }
    // Get the config from cache
    YGSRPCConfigCache &cache = rpc_config_cache[oid];
    if (cache.ids.has(p_method)) {
        uint16_t id = cache.ids[p_method];
        return cache.configs[id];
    }
    // Default config if not found
    YGSRPCConfig config;
    config.name = p_method;
    config.rpc_mode = MultiplayerAPI::RPC_MODE_DISABLED;
    config.transfer_mode = MultiplayerPeer::TRANSFER_MODE_RELIABLE;
    config.call_local = false;
    config.channel = 0;
    return config;
}

void YGameState::add_state_parameter_authority(int param, int network_id_authority) {
    state_parameter_authorities.insert(Vector2i(param, network_id_authority));
}

void YGameState::remove_state_parameter_authority(int param, int network_id_authority) {
    state_parameter_authorities.erase(Vector2i(param, network_id_authority));
}

bool YGameState::has_state_parameter_authority(int param, int network_id_authority) const {
    return state_parameter_authorities.has(Vector2i(param, network_id_authority));
}

// Either called this from server or from a client.
void YGameState::set_state_parameter(int param, Variant v) {
    if (has_valid_multiplayer_peer()) {
        Array sending_rpc_array;
        sending_rpc_array.push_back(param);
        sending_rpc_array.push_back(v);
        int argcount = sending_rpc_array.size();
        const Variant **argptrs = (const Variant **)alloca(sizeof(Variant *) * argcount);
        for (int i = 0; i < argcount; i++) {
            argptrs[i] = &sending_rpc_array[i];
        }
        // If we're online, we're either sending this as a request to the server, or if we're calling this in the server we're sending it to all clients.
        if (scene_multiplayer->get_unique_id() == 1) {
            // If we're the server, we should do it now if we're alowed and send it to others.
            if (has_state_parameter_authority(param, 1)) {
                rpcp(0, rpc_set_state_parameter_stringname, argptrs, argcount);
                actually_set_state_parameter(param, v);
                emit_signal("state_parameter_changed", param, Variant{}, v);
            }
        }else{
            // If we're a client, we're sending it to the server.
            rpcp(1, rpc_set_state_parameter_stringname, argptrs, argcount);
        }
        return;
    }
    actually_set_state_parameter(param, v);
    emit_signal("state_parameter_changed", param, Variant{}, v);
}

// Either called this from server or from a client.
void YGameState::remove_state_parameter(int param) {
    if (has_valid_multiplayer_peer()) {
        Array sending_rpc_array;
        sending_rpc_array.push_back(param);
        int argcount = sending_rpc_array.size();
        const Variant **argptrs = (const Variant **)alloca(sizeof(Variant *) * argcount);
        for (int i = 0; i < argcount; i++) {
            argptrs[i] = &sending_rpc_array[i];
        }
        // If we're online, we're either sending this as a request to the server, or if we're calling this in the server we're sending it to all clients.
        if (scene_multiplayer->get_unique_id() == 1) {
            // If we're the server, we should do it now if we're alowed.
            if (has_state_parameter_authority(param, 1)) {
                rpcp(0, rpc_remove_state_parameter_stringname, argptrs, argcount);
                actually_remove_state_parameter(param);
                emit_signal("state_parameter_removed", param);
            }
        }else{
            // If we're a client, we're sending it to the server.
            rpcp(1, rpc_remove_state_parameter_stringname, argptrs, argcount);
        }
        return;
    } 

    // If we're not online, we're just doing it locally.
    actually_remove_state_parameter(param);
    emit_signal("state_parameter_removed", param);
}


void YGameState::_rpc_set_state_parameter(int param_id, const Variant& param_value) {
    if (!has_valid_multiplayer_peer()) {
        return;
    }
    if (scene_multiplayer->get_unique_id() == 1) {
        // Check if the sender has authority to set this parameter.
        if (has_state_parameter_authority(param_id, scene_multiplayer->get_remote_sender_id())) {
            actually_set_state_parameter(param_id, param_value);
            if (state_parameters.has(param_id)) {
                emit_signal("state_parameter_changed", param_id, state_parameters[param_id], param_value);
            }else{
                emit_signal("state_parameter_changed", param_id, Variant{}, param_value);
            }
            Array sending_rpc_array;
            sending_rpc_array.push_back(param_id);
            sending_rpc_array.push_back(param_value);
            int argcount = sending_rpc_array.size();
            const Variant **argptrs = (const Variant **)alloca(sizeof(Variant *) * argcount);
            for (int i = 0; i < argcount; i++) {
                argptrs[i] = &sending_rpc_array[i];
            }
            // Send to others
            rpcp(0, rpc_set_state_parameter_stringname, argptrs, argcount);
        }
    } else{
        if (scene_multiplayer->get_remote_sender_id() == 1) {
            // If we're not the server, and the sender is the server, just do it.
            actually_set_state_parameter(param_id, param_value);
            if (state_parameters.has(param_id)) {
                emit_signal("state_parameter_changed", param_id, state_parameters[param_id], param_value);
            }else{
                emit_signal("state_parameter_changed", param_id, Variant{}, param_value);
            }
        }
    }
}

void YGameState::_rpc_remove_state_parameter(int param_id) {
    if (!has_valid_multiplayer_peer()) {
        return;
    }
    if (scene_multiplayer->get_unique_id() == 1) {
        // Check if the sender has authority to remove this parameter.
        if (has_state_parameter_authority(param_id, scene_multiplayer->get_remote_sender_id())) {
            if (state_parameters.has(param_id)) {
                actually_remove_state_parameter(param_id);
                emit_signal("state_parameter_removed", param_id);
            }
        }
    } else{
        if (scene_multiplayer->get_remote_sender_id() == 1) {
            // If we're not the server, and the sender is the server, just do it.
            if (state_parameters.has(param_id)) {
                actually_remove_state_parameter(param_id);
                emit_signal("state_parameter_removed", param_id);
            }
        }
    }
}


void YGameState::_parse_rpc_config(const Dictionary &p_config, bool p_for_node, YGSRPCConfigCache &r_cache) {
    Array names = p_config.keys();
    names.sort_custom(callable_mp_static(&StringLikeVariantOrder::compare)); // Ensure ID order
    for (int i = 0; i < names.size(); i++) {
        ERR_CONTINUE(!names[i].is_string());
        String name = names[i].operator String();
        ERR_CONTINUE(p_config[name].get_type() != Variant::DICTIONARY);
        ERR_CONTINUE(!p_config[name].operator Dictionary().has("rpc_mode"));
        Dictionary dict = p_config[name];
        YGSRPCConfig cfg;
        cfg.name = name;
        cfg.rpc_mode = ((MultiplayerAPI::RPCMode)dict.get("rpc_mode", MultiplayerAPI::RPC_MODE_AUTHORITY).operator int());
        cfg.transfer_mode = ((MultiplayerPeer::TransferMode)dict.get("transfer_mode", MultiplayerPeer::TRANSFER_MODE_RELIABLE).operator int());
        cfg.call_local = dict.get("call_local", false).operator bool();
        cfg.channel = dict.get("channel", 0).operator int();
        uint16_t id = ((uint16_t)i);
        if (p_for_node) {
            id |= (1 << 15);
        }
        r_cache.configs[id] = cfg;
        r_cache.ids[name] = id;
    }
}

Error YGameState::broadcast_call_on_game_action(const Variant **p_args, int p_argcount, Callable::CallError &r_error) {
    if (!has_valid_multiplayer_peer()) {
        return ERR_UNCONFIGURED;
    }
    if (p_argcount < 1) {
        r_error.error = Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS;
        r_error.expected = 1;
        return ERR_INVALID_PARAMETER;
    }

    Variant::Type type = p_args[0]->get_type();
    if (type != Variant::CALLABLE) {
        r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
        r_error.argument = 0;
        r_error.expected = Variant::CALLABLE;
        return ERR_INVALID_PARAMETER;
    }

    Callable p_callable = p_args[0]->operator Callable();
    ERR_FAIL_COND_V(!is_inside_tree(), ERR_UNCONFIGURED);

    YGameAction* callable_object = Object::cast_to<YGameAction>(p_callable.get_object());
    ERR_FAIL_COND_V(callable_object == nullptr, ERR_DOES_NOT_EXIST);

    String method_name = p_callable.get_method();
    // Get RPC config for this method
    bool any_peer = method_name.begins_with("_any_");
    // Check if we can send this RPC
    bool can_send = any_peer;
    if (!any_peer) {
        if (callable_object->player_turn != -1) {
            YGamePlayer* ygp = get_game_player(callable_object->player_turn);
            if (ygp != nullptr) {
                can_send = ygp->remote_player_id == scene_multiplayer->get_unique_id() || scene_multiplayer->get_unique_id() == 1;
            }else{
                can_send = scene_multiplayer->get_unique_id() == get_multiplayer_authority();
            }
        }else {
            can_send = scene_multiplayer->get_unique_id() == get_multiplayer_authority();
        }
    }

    if (!can_send) {
        print_error(vformat("Invalid call for function %s. Doesn't have authority.", p_callable.get_method()));
        return ERR_UNAUTHORIZED;
    }

    int net_id = callable_object->get_unique_id();
    ERR_FAIL_COND_V(net_id == -1, ERR_UNCONFIGURED);

    Array sending_rpc_array;
    sending_rpc_array.push_back(net_id);
    sending_rpc_array.push_back(p_callable.get_method());
    p_args[0] = new Variant(sending_rpc_array);
    return rpcp(0, _receive_call_on_game_action_stringname, p_args, p_argcount);
}

Error YGameState::broadcast_call_on_game_action_and_execute(const Variant **p_args, int p_argcount, Callable::CallError &r_error) {
    if (!has_valid_multiplayer_peer()) {
        return ERR_UNCONFIGURED;
    }
    if (p_argcount < 1) {
        r_error.error = Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS;
        r_error.expected = 1;
        return ERR_INVALID_PARAMETER;
    }

    Variant::Type type = p_args[0]->get_type();
    if (type != Variant::CALLABLE) {
        r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
        r_error.argument = 0;
        r_error.expected = Variant::CALLABLE;
        return ERR_INVALID_PARAMETER;
    }

    Callable p_callable = p_args[0]->operator Callable();
    ERR_FAIL_COND_V(!is_inside_tree(), ERR_UNCONFIGURED);

    YGameAction* callable_object = Object::cast_to<YGameAction>(p_callable.get_object());
    ERR_FAIL_COND_V(callable_object == nullptr, ERR_DOES_NOT_EXIST);

    String method_name = p_callable.get_method();
    // Get RPC config for this method
    bool any_peer = method_name.begins_with("_any_");
    // Check if we can send this RPC
    bool can_send = any_peer;
    if (!any_peer) {
        if (callable_object->player_turn != -1) {
            YGamePlayer* ygp = get_game_player(callable_object->player_turn);
            if (ygp != nullptr) {
                can_send = ygp->remote_player_id == scene_multiplayer->get_unique_id() || scene_multiplayer->get_unique_id() == 1;
            }else{
                can_send = scene_multiplayer->get_unique_id() == get_multiplayer_authority();
            }
        }else {
            can_send = scene_multiplayer->get_unique_id() == get_multiplayer_authority();
        }
    }

    if (!can_send) {
        print_error(vformat("Invalid call for function %s. Doesn't have authority.", p_callable.get_method()));
        return ERR_UNAUTHORIZED;
    }

    int net_id = callable_object->get_unique_id();
    ERR_FAIL_COND_V(net_id == -1, ERR_UNCONFIGURED);

    Array sending_rpc_array;
    sending_rpc_array.push_back(net_id);
    sending_rpc_array.push_back(p_callable.get_method());
    p_args[0] = new Variant(sending_rpc_array);
    return rpcp(0, _receive_call_on_game_action_also_local_stringname,p_args, p_argcount);
}

Variant YGameState::_receive_call_on_game_action(const Variant **p_args, int p_argcount, Callable::CallError &r_error) {
    if (p_argcount < 1) {
        r_error.error = Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS;
        r_error.expected = 1;
        print_line("Received call on game action with too few arguments");
        return ERR_INVALID_PARAMETER;
    }
    Variant::Type itype = p_args[0]->get_type();
    if (itype != Variant::ARRAY) {
        r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
        r_error.argument = 0;
        r_error.expected = Variant::ARRAY;
        print_line("Received call on game action with invalid argument");
        return ERR_INVALID_PARAMETER;
    }
    Array yrpc_info = p_args[0]->operator Array();
    uint32_t netid = yrpc_info[0];
    String method_name = yrpc_info[1];

    Ref<YGameAction> action = get_game_action(netid);
    // print_line(vformat("Received call on game action with unique action id %d and method name %s. Is valid? %s", netid, method_name, action.is_valid() ? "true" : "false"));
    if (action.is_valid()) {
        bool can_receive = method_name.begins_with("_any_");
        if (!can_receive) {
            if (action->player_turn != -1) {
                YGamePlayer* ygp = get_game_player(action->player_turn);
                if (ygp != nullptr) {
                    can_receive = ygp->remote_player_id == scene_multiplayer->get_remote_sender_id() || scene_multiplayer->get_remote_sender_id() == 1;
                }else{
                    can_receive = scene_multiplayer->get_remote_sender_id() == get_multiplayer_authority();
                }
            }else {
                can_receive = scene_multiplayer->get_remote_sender_id() == get_multiplayer_authority();
            }
        }

        if (!can_receive) {
            print_line("Received call on game action but can't receive");
            return ERR_UNAUTHORIZED;
        }

        if (action->has_method(method_name)) {
            action->callp(method_name, &p_args[1], p_argcount - 1, r_error);
            if (r_error.error != Callable::CallError::CALL_OK) {
                print_line("Received call on game action: Error calling method", r_error.error);
                return r_error.error;
            }
        }else{
            print_line("Received call on game action: Action does not have method", method_name);
        }
    }

    return 0;
}

void YGameState::mark_action_finished_and_sync(int action_id) {
    if (!has_valid_multiplayer_peer() || scene_multiplayer->get_unique_id() != 1) {
        return; // Only server can mark actions as finished
    }

    // Find the action in the game state
    Ref<YGameAction> action = get_game_action(action_id);
    if (action.is_valid()) {
        // Mark it as finished and set it for playback
        action->is_playing_back = true;
        action->instant_execute = true;
        action->finished = true;
        
        // If it's waiting for a step, release it
        if (action->waiting_for_step) {
            action->release_step();
        }

        // Broadcast to all clients
        rpc(rpc_mark_action_finished_stringname, action_id);
    }
}

void YGameState::_rpc_mark_action_finished(int action_id) {
    if (!has_valid_multiplayer_peer() || scene_multiplayer->get_unique_id() == 1) {
        return; // Server already marked it as finished
    }

    // Find the action in the game state
    Ref<YGameAction> action = get_game_action(action_id);
    if (action.is_valid()) {
        // Mark it as finished and set it for playback
        action->is_playing_back = true;
        action->instant_execute = true;
        action->finished = true;
        
        // If it's waiting for a step, release it
        if (action->waiting_for_step) {
            action->release_step();
        }
    }
}

void YGameState::_rpc_request_start_action_approval(int action_id) {
    if (!has_valid_multiplayer_peer() || scene_multiplayer->get_unique_id() != 1) {
        if (debugging_level >= 2) {
            print_line("[YGameState] Ignoring approval request - not server");
        }
        return; // Only server should handle approval requests
    }
    if (debugging_level >= 2) {
        print_line(vformat("[YGameState] Server received approval request for action %d from %d", action_id, scene_multiplayer->get_remote_sender_id()));
    }
    int desired_action_id = 0;

    bool approved = false;
    
    // Check if action is in past actions
    for (const auto& action : past_game_actions) {
        if (action.is_valid() && action->get_unique_id() == action_id) {
            approved = true;
            desired_action_id = action_id;
            if (debugging_level >= 2) {
                print_line(vformat("[YGameState] Action found in past actions: %d", action_id));
            }
            break;
        }
    }

    // Check if action is current action
    if (!approved) {
        if (has_current_game_action() && get_current_game_action()->get_unique_id() == action_id) {
            approved = true;
            desired_action_id = action_id;
            if (debugging_level >= 2) {
                print_line(vformat("[YGameState] Action is current action: %d", action_id));
            }
        } else if (has_current_game_action()) {
            desired_action_id = get_current_game_action()->get_unique_id();
            if (debugging_level >= 2) {
                print_line(vformat("[YGameState] Current action is different: %d", desired_action_id));
            }
        }
    }

    // If it's not in past or current actions, don't approve it.
    // Otherwise they might start a future action when in reality I might be about to add a new override action.

    if (approved) {
        desired_action_id = action_id;
    }

    if (debugging_level >= 2) {
        print_line(vformat("[YGameState] Sending approval response - Action: %d, Desired: %d, Approved: %s", 
            action_id, desired_action_id, approved ? "true" : "false"));
    }

    Array sending_rpc_array;
    sending_rpc_array.push_back(action_id);
    sending_rpc_array.push_back(desired_action_id);
    sending_rpc_array.push_back(approved);
    int argcount = sending_rpc_array.size();
    const Variant **argptrs = (const Variant **)alloca(sizeof(Variant *) * argcount);
    for (int i = 0; i < argcount; i++) {
        argptrs[i] = &sending_rpc_array[i];
    }
    rpcp(scene_multiplayer->get_remote_sender_id(), rpc_response_start_action_approval_stringname, argptrs, argcount);
}

void YGameState::_rpc_response_start_action_approval(int action_id, int desired_action_id, bool approved) {
    if (!has_valid_multiplayer_peer() || scene_multiplayer->get_unique_id() == 1) {
        if (debugging_level >= 2) {
            print_line("[YGameState] Ignoring approval response - not client");
        }
        return; // Only clients should handle approval responses
    }

    if (debugging_level >= 2) {
        print_line(vformat("[YGameState %d] Client received approval response - Action: %d, Desired: %d, Approved: %s", 
            scene_multiplayer->get_unique_id(),
            action_id, desired_action_id, approved ? "true" : "false"));
    }

    // Find the action in the game state
    Ref<YGameAction> action = get_game_action(action_id);
    if (action.is_valid()) {
        if (approved) {
            if (debugging_level >= 2) {
                print_line(vformat("[YGameState %d] Marking action as approved", scene_multiplayer->get_unique_id()));
            }
            action->remote_start_approval = YGameAction::RemoteStartApproval::REMOTE_START_APPROVAL_APPROVED;
            last_action_was_denied = false;
        } else {
            if (debugging_level >= 2) {
                print_line(vformat("[YGameState %d] Marking action as denied", scene_multiplayer->get_unique_id()));
            }
            action->remote_start_approval = YGameAction::RemoteStartApproval::REMOTE_START_APPROVAL_DENIED;
            last_action_was_denied = true;
        }
    } else {
        if (debugging_level >= 2) {
            print_line(vformat("[YGameState %d] Could not find action to update approval state", scene_multiplayer->get_unique_id()));
        }
    }

    if (!approved && desired_action_id != action_id) {
        Ref<YGameAction> desired_action = get_game_action(desired_action_id);
        if (desired_action.is_valid()) {
            if(!desired_action->started) {
                if (debugging_level >= 2) {
                    print_line(vformat("[YGameState %d] Marking the server's desired action as approved", scene_multiplayer->get_unique_id()));
                }
                desired_action->remote_start_approval = YGameAction::RemoteStartApproval::REMOTE_START_APPROVAL_APPROVED;
            } else {
                if (debugging_level >= 2) {
                    print_line(vformat("[YGameState %d] The server's desired action has already started.", scene_multiplayer->get_unique_id()));
                }
            }
        } else {
            if (debugging_level >= 2) {
                print_line(vformat("[YGameState %d] The server's desired action %d does not exist.", scene_multiplayer->get_unique_id(), desired_action_id));
            }
        }
    }

    pending_waiting_for_start_approval_attempts = 0;
}

void YGameState::request_action_approval(YGameAction* action) {
    if (!has_valid_multiplayer_peer() || scene_multiplayer->get_unique_id() == 1) {
        if (debugging_level >= 2) {
            print_line(vformat("[YGameState %d] Ignoring approval request - not client", scene_multiplayer->get_unique_id()));
        }
        return; // Don't send if we're the server
    }
    if (action == nullptr) {
        if (debugging_level >= 2) {
            print_line(vformat("[YGameState %d] Cannot request approval for null action", scene_multiplayer->get_unique_id()));
        }
        return;
    }
    if (debugging_level >= 2) {
        print_line(vformat("[YGameState %d] Requesting approval for action %d", scene_multiplayer->get_unique_id(), action->unique_id));
    }
    action->remote_start_approval = YGameAction::RemoteStartApproval::REMOTE_START_APPROVAL_PENDING;
    Array sending_rpc_array;
    sending_rpc_array.push_back(action->unique_id);
    int argcount = sending_rpc_array.size();
    const Variant **argptrs = (const Variant **)alloca(sizeof(Variant *) * argcount);
    for (int i = 0; i < argcount; i++) {
        argptrs[i] = &sending_rpc_array[i];
    }
    rpcp(1, rpc_request_start_action_approval_stringname, argptrs, argcount);
}

                    
                    