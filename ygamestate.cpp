//
// Created by Daniel on 2024-03-19.
//

#include "ygamestate.h"

Ref<YGameState> YGameState::singleton;

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

    ClassDB::bind_method(D_METHOD("clear_all_players"), &YGameState::clear_all_players);
    ClassDB::bind_method(D_METHOD("clear_all_game_actions"), &YGameState::clear_all_game_actions);

    ClassDB::bind_method(D_METHOD("get_current_game_action"), &YGameState::get_current_game_action);
    ClassDB::bind_method(D_METHOD("has_current_game_action"), &YGameState::has_current_game_action);
    ClassDB::bind_method(D_METHOD("end_current_game_action"), &YGameState::end_current_game_action);

    ClassDB::bind_method(D_METHOD("set_current_turn_player","current_turn_player"), &YGameState::set_current_turn_player);
    ClassDB::bind_method(D_METHOD("get_current_turn_player"), &YGameState::get_current_turn_player);
    ClassDB::bind_method(D_METHOD("has_current_turn_player"), &YGameState::has_current_turn_player);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "current_turn_player", PROPERTY_HINT_NODE_TYPE, "YGamePlayer",PROPERTY_USAGE_NO_EDITOR), "set_current_turn_player", "get_current_turn_player");

    ClassDB::bind_method(D_METHOD("set_wait_before_going_to_next_action","time_to_wait"), &YGameState::set_wait_before_going_to_next_action);
    ClassDB::bind_method(D_METHOD("get_wait_before_going_to_next_action"), &YGameState::get_wait_before_going_to_next_action);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "wait_before_going_to_next_action"), "set_wait_before_going_to_next_action", "get_wait_before_going_to_next_action");

    ClassDB::bind_method(D_METHOD("set_prevent_proceed_to_next_action","preventing_amount"), &YGameState::set_prevent_proceed_to_next_action);
    ClassDB::bind_method(D_METHOD("get_prevent_proceed_to_next_action"), &YGameState::get_prevent_proceed_to_next_action);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "prevent_proceed_to_next_action"), "set_prevent_proceed_to_next_action", "get_prevent_proceed_to_next_action");

    ClassDB::bind_method(D_METHOD("get_future_game_action_count"), &YGameState::get_future_game_action_count);
    ClassDB::bind_method(D_METHOD("get_past_game_action_count"), &YGameState::get_past_game_action_count);
    ClassDB::bind_method(D_METHOD("get_previous_game_action"), &YGameState::get_previous_game_action);
    ClassDB::bind_method(D_METHOD("get_next_game_action"), &YGameState::get_next_game_action);
    ClassDB::bind_method(D_METHOD("get_past_game_action_by_index","index"), &YGameState::get_past_game_action_by_index);
    ClassDB::bind_method(D_METHOD("get_future_game_action_by_index","index"), &YGameState::get_future_game_action_by_index);

    ADD_SIGNAL(MethodInfo("ended_all_actions"));
}

YGameState *YGameState::get_singleton() {
    return *singleton;
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
        if (current_game_action->finished) {
            current_game_action->end_action();
            past_game_actions.push_back(current_game_action);
            current_game_action.unref();
            return;
        }
        if(!current_game_action->process_action(static_cast<float>(delta))) {
            current_game_action->end_action();
            past_game_actions.push_back(current_game_action);
            current_game_action.unref();
            return;
        }
        frame_count_before_doing_slow_process+=delta;
        if (frame_count_before_doing_slow_process > 0.5) {
            frame_count_before_doing_slow_process=0;
            if(!current_game_action->slow_process_action(static_cast<float>(delta))) {
                current_game_action->end_action();
                past_game_actions.push_back(current_game_action);
                current_game_action.unref();
                return;
            }
        }
    } else {
        if (prevent_proceed_to_next_action > 0)
            return;
        if (wait_before_going_to_next_action > 0.0) {
            wait_before_going_to_next_action-=delta;
            return;
        }
        if (overriding_game_actions.size() > 0) {
            current_game_action = overriding_game_actions[0];
            overriding_game_actions.remove_at(0);
            if (current_game_action.is_valid())
                current_game_action->enter_action();
            return;
        }
        if (future_game_actions.size() > 0) {
            current_game_action = future_game_actions[0];
            //print_line("Set the current game action to be the one in front of future_game_actions ",current_game_action);
            future_game_actions.remove_at(0);
            //print_line("Popped the front of futuregameactions, is current game action valid? ",current_game_action," ",current_game_action.is_valid());
            if (current_game_action.is_valid()) {
                current_game_action->enter_action();
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
    showed_out_of_actions_message=false;
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
    //print_line("Added game action, future game actions now ",future_game_actions.size());
    return ygs;
}

Ref<YGameAction> YGameState::add_override_game_action(const Ref<YGameAction> &ygs, int desired_game_state_id) {
    showed_out_of_actions_message=false;
    if(!has_started) {
        YEngine::get_singleton()->game_state_starting(this);
        has_started=true;
    }
    if (desired_game_state_id == -1) {
        desired_game_state_id = next_game_action_unique_id;
        next_game_action_unique_id++;
    }
    if (overriding_game_actions.size()>0)
        overriding_game_actions.insert(0,ygs);
    else
        overriding_game_actions.append(ygs);
    ygs->set_unique_id(desired_game_state_id);
    ygs->created();
    return ygs;
}