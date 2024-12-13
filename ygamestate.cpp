//
// Created by Daniel on 2024-03-19.
//

#include "ygamestate.h"

#include "core/config/project_settings.h"

YGameState* YGameState::singleton = nullptr;

void YGameState::_bind_methods() {
    ClassDB::bind_method(D_METHOD("register_player", "game_player"), &YGameState::register_player);
    ClassDB::bind_method(D_METHOD("register_player_specific_id", "game_player","desired_id"), &YGameState::register_player_specific_id);

    ClassDB::bind_method(D_METHOD("get_all_players"), &YGameState::get_all_players);
    ClassDB::bind_method(D_METHOD("get_game_player", "player_id"), &YGameState::get_game_player);
    ClassDB::bind_method(D_METHOD("get_game_player_by_controller", "player_controller"), &YGameState::get_game_player_by_controller);
    ClassDB::bind_method(D_METHOD("get_game_player_by_type", "player_type"), &YGameState::get_game_player_by_type);
    ClassDB::bind_method(D_METHOD("remove_player", "game_player"), &YGameState::remove_player);
    ClassDB::bind_method(D_METHOD("remove_player_with_id", "game_player_id"), &YGameState::remove_player_with_id);

    ClassDB::bind_method(D_METHOD("add_game_action", "game_action","desired_id"), &YGameState::add_game_action,DEFVAL(-1));
    ClassDB::bind_method(D_METHOD("add_override_game_action", "game_action","desired_id"), &YGameState::add_override_game_action,DEFVAL(-1));

    ClassDB::bind_method(D_METHOD("add_game_action_with", "game_action","initial_parameter","param_value","desired_id"), &YGameState::add_game_action_with_param,DEFVAL(-1),DEFVAL(Variant{}),DEFVAL(-1));
    ClassDB::bind_method(D_METHOD("add_override_game_action_with","initial_parameter","param_value", "game_action","desired_id"), &YGameState::add_override_game_action_with_param,DEFVAL(-1),DEFVAL(Variant{}),DEFVAL(-1));

    ClassDB::bind_method(D_METHOD("clear_all_players"), &YGameState::clear_all_players);
    ClassDB::bind_method(D_METHOD("clear_all_game_actions"), &YGameState::clear_all_game_actions);

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

    ClassDB::bind_method(D_METHOD("restart_action_counting"), &YGameState::restart_action_counting);
    ClassDB::bind_method(D_METHOD("get_game_actions_counted"), &YGameState::get_game_actions_counted);

    ClassDB::bind_method(D_METHOD("serialize"), &YGameState::serialize);
    ClassDB::bind_method(D_METHOD("deserialize", "data","playback_after","instant_playback"), &YGameState::deserialize,DEFVAL(false),DEFVAL(false));

    ADD_SIGNAL(MethodInfo("registered_action",PropertyInfo(Variant::OBJECT, "new_action", PROPERTY_HINT_RESOURCE_TYPE, "YGameAction")));
    ADD_SIGNAL(MethodInfo("changed_current_action",PropertyInfo(Variant::OBJECT, "new_action", PROPERTY_HINT_RESOURCE_TYPE, "YGameAction")));
    ADD_SIGNAL(MethodInfo("changed_turn_player_id", PropertyInfo(Variant::INT, "player_turn_id_before"),
                          PropertyInfo(Variant::INT, "player_turn_id_after")));
    ADD_SIGNAL(MethodInfo("ended_all_actions"));
}

YGameState *YGameState::get_singleton() {
    return singleton;
}

String YGameState::get_current_game_action_name() {
    if (current_game_action == nullptr) return "NULL";
    return current_game_action->get_name();
}

int YGameState::register_player(YGamePlayer *ygp) {
    if (ygp != nullptr) {
       // int started_with = game_players.size();
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
        //print_line("Called register player started with ",started_with," ended with ",game_players.size());
        return ygp->player_id;
    }
    return -1;
}

int YGameState::register_player_specific_id(YGamePlayer *ygp,int desired_id) {
    if (ygp != nullptr) {
        ygp->player_id = desired_id;
        if (next_player_unique_id < desired_id)
            next_player_unique_id= desired_id+1;
        game_players[ygp->player_id] = ygp;
        ygp->player_registered();
        return ygp->player_id;
    }
    return -1;
}
bool YGameState::remove_player_with_id(int ygp_id) {
    if (game_players.has(ygp_id)) {
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
    //print_line("do process, is current valid? ",current_game_action.is_valid()," future actions size ",future_game_actions.size());
    if (current_game_action.is_valid()) {
        //HAS A CURRENT GAMEACTION!!
        //Has it ended?

        if (current_game_action->finished  && !current_game_action->waiting_for_step) {
            if (wait_before_going_to_next_action > 0.0) {
                wait_before_going_to_next_action-=delta;
                return;
            }
            current_game_action->end_action();
            game_actions_done_since_started_counting+=1;
            past_game_actions.push_back(current_game_action);
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
                        current_game_action->steps_consumed+=1;
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
                                current_game_action->step_action(current_game_action->action_steps[current_game_action->steps_consumed],false);
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
                current_game_action->end_action();
                game_actions_done_since_started_counting+=1;
                past_game_actions.push_back(current_game_action);
                current_game_action.unref();
                return;
            }

            frame_count_before_doing_slow_process+=delta;

            if (frame_count_before_doing_slow_process > 0.5) {
                frame_count_before_doing_slow_process=0;
                if(!current_game_action->slow_process_action(static_cast<float>(delta)) && !current_game_action->waiting_for_step) {
                    current_game_action->end_action();
                    game_actions_done_since_started_counting+=1;
                    past_game_actions.push_back(current_game_action);
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
            current_game_action = overriding_game_actions[0];
            overriding_game_actions.remove_at(0);
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

        if (future_game_actions_remaining > 0) {
            current_game_action = future_game_actions[0];
            //print_line("Set the current game action to be the one in front of future_game_actions ",current_game_action);
            future_game_actions.remove_at(0);
            //print_line("Popped the front of futuregameactions, is current game action valid? ",current_game_action," ",current_game_action.is_valid());
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
        if (!showed_out_of_actions_message) {
            showed_out_of_actions_message=true;
            emit_signal("ended_all_actions");
        }
    }
}

bool YGameState::end_current_game_action() const {
        if (has_current_game_action()) {
            current_game_action->end_action();
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
Ref<YGameAction> YGameState::add_game_action_with_param(const Ref<YGameAction> &ygs, int desired_initial_param, const Variant &desired_param_data, int desired_game_state_id) {
    if (is_playing_back) {
        return ygs;
    }
    showed_out_of_actions_message=false;
    if (desired_initial_param != -1) {
        ygs->set_action_parameter(desired_initial_param,desired_param_data);
    }
    //print_line("Adding game action ",ygs," has started? ",has_started);
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
    emit_signal("registered_action",ygs);
    //print_line("Added game action, future game actions now ",future_game_actions.size());
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
            overriding_game_actions.insert(1,ygs);
        } else {
            overriding_game_actions.insert(0,ygs);
        }
    }
    else
        overriding_game_actions.append(ygs);
    ygs->set_unique_id(desired_game_state_id);
    ygs->created();
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
        overriding_game_actions_array.push_back(game_action->serialize());
    }

    Array future_game_actions_array;
    for (const auto& game_action : future_game_actions) {
        future_game_actions_array.push_back(game_action->serialize());
    }

    Array past_game_actions_array;
    for (const auto& game_action : past_game_actions) {
        past_game_actions_array.push_back(game_action->serialize());
    }

    Dictionary state_parameters_dict;
    for (auto& entry : state_parameters) {
        state_parameters_dict[Variant(entry.key)] = entry.value;
    }
    dict["state_parameters"] = state_parameters_dict;

    dict["override_actions"] = overriding_game_actions_array;
    dict["future_actions"] = future_game_actions_array;
    dict["past_actions"] = past_game_actions_array;

    if (!current_game_action.is_null() && current_game_action.is_valid())
        dict["current_action"] = current_game_action->serialize();

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
    if (p_front_instead)
        list_into.insert(0,action);
    else
        list_into.push_back(action);
}

void YGameState::deserialize_game_actions_into(Vector<Ref<YGameAction>> &list_into, Array serialized_game_actions) {
    for (int i = 0; i < serialized_game_actions.size(); i++) {
        Dictionary action_dict = serialized_game_actions[i];
        deserialize_individual_game_action_into(list_into, action_dict);
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
            if (player_dict.has("parent_path") && YEngine::get_singleton()->has_node(NodePath{player_dict["parent_path"]})) {
                YEngine::get_singleton()->get_node(NodePath{player_dict["parent_path"]})->add_child(player);
            } else {
                YEngine::get_singleton()->add_child(player);
            }
            player->deserialize(player_dict);
            game_players[key] = player;
        }
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