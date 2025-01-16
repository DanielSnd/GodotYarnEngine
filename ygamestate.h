//
// Created by Daniel on 2024-03-19.
//

#ifndef YGAMESTATE_H
#define YGAMESTATE_H

#include "yarnengine.h"
#include "core/object/ref_counted.h"
#include "core/variant/typed_array.h"
#include "ygameaction.h"
#include "ygameplayer.h"
#include "yvisualaction.h"
#include "yvisualelement3d.h"
#include "yarntime.h"

/// Forward Declaring YGameAction for use.
class YGameAction;
class YGamePlayer;
class YVisualAction;
class YEngine;
class YActionStep;
///////////////////////////////
/// YGAMESTATE
///////////////////////////////

class YGameState : public RefCounted {
    GDCLASS(YGameState, RefCounted);

protected:
    static YGameState* singleton;
    static void _bind_methods();

public:
    static YGameState *get_singleton();

    bool has_started=false;

    int next_execution_order_number;
    int next_game_action_unique_id;
    int next_visual_action_unique_id;
    int next_player_unique_id;
    int next_visual_element_unique_id;
    String get_current_game_action_name();

    double wait_before_going_to_next_action =0.0;
    void set_wait_before_going_to_next_action(double f) {wait_before_going_to_next_action = f;}
    double get_wait_before_going_to_next_action() const {return wait_before_going_to_next_action;}

    double wait_before_going_to_next_step =0.0;
    void set_wait_before_going_to_next_step(double f) {wait_before_going_to_next_step = f;}
    double get_wait_before_going_to_next_step() const {return wait_before_going_to_next_step;}

    Dictionary serialize();

    void deserialize_individual_game_action_into(Vector<Ref<YGameAction>> &list_into, const Dictionary &action_dict, bool p_front_instead);

    void deserialize_game_actions_into(Vector<Ref<YGameAction>> &list_into, Array serialized_actions);

    Dictionary deserialize(Dictionary dict, bool p_playback_after, bool p_instant_playback);

    YGamePlayer* current_turn_player;
    Ref<YGameAction> current_game_action;
    Vector<Ref<YGameAction>> overriding_game_actions;
    Vector<Ref<YGameAction>> future_game_actions;
    Vector<Ref<YGameAction>> past_game_actions;
    Vector<Ref<YGameAction>> current_parallel_actions;
    Vector<Ref<YGameAction>> future_parallel_actions;
    HashMap<int,YGamePlayer*> game_players;
    HashMap<int,YVisualElement3D*> visual_elements_3d;

    int game_actions_done_since_started_counting = 0;
    int get_game_actions_counted() const {return game_actions_done_since_started_counting;}
    void restart_action_counting() {
        game_actions_done_since_started_counting = 0;
    }
    int prevent_proceed_to_next_action = 0;
    void set_prevent_proceed_to_next_action(int v) { prevent_proceed_to_next_action = MAX(v,0); }
    int get_prevent_proceed_to_next_action() const { return prevent_proceed_to_next_action; }

    bool showed_out_of_actions_message=false;

    int register_player(YGamePlayer* ygp);

    int register_player_specific_id(YGamePlayer *ygp, int desired_id);

    bool remove_player_with_id(int ygp_id);
    bool set_player_id(YGamePlayer* ygp,int new_pid);
    bool remove_player(YGamePlayer* ygp);
    YGamePlayer* get_game_player(int ypg_id);
    YGamePlayer* get_game_player_by_controller(int ypg_id);
    TypedArray<YGamePlayer> get_all_players();
    YGamePlayer *get_game_player_by_type(int ypg_type);

    double frame_count_before_doing_slow_process;
    void do_process(double delta);

    Ref<YGameAction> get_current_game_action() const {
        return current_game_action;
    }

    int get_overridinge_game_action_count() const {
        return overriding_game_actions.size();
    }
    int get_future_game_action_count() const {
        return future_game_actions.size();
    }

    int get_past_game_action_count() const {
        return past_game_actions.size();
    }

    Ref<YGameAction> get_previous_game_action() const {
        if (past_game_actions.size() > 0)
            return past_game_actions[past_game_actions.size()-1];
        return nullptr;
    }
    Ref<YGameAction> get_next_game_action() const {
        if (overriding_game_actions.size() > 0)
            return overriding_game_actions[0];
        if (future_game_actions.size() > 0)
            return future_game_actions[0];
        return nullptr;
    }
    Ref<YGameAction> get_past_game_action_by_index(int future_index) const {
        if (future_index>=0 && future_index < past_game_actions.size())
            return past_game_actions[future_index];
        return nullptr;
    }

    Ref<YGameAction> get_future_game_action_by_index(int future_index) const {
        if (future_index>=0 && future_index < future_game_actions.size())
            return future_game_actions[future_index];
        return nullptr;
    }
    Ref<YGameAction> get_overriding_game_action_by_index(int future_index) const {
        if (future_index>=0 && future_index < overriding_game_actions.size())
            return overriding_game_actions[future_index];
        return nullptr;
    }
    bool has_current_game_action() const {
        return !current_game_action.is_null() && current_game_action.is_valid();
    }

    bool end_current_game_action() const;

    int last_turn_player_id=-1;
    bool is_playing_back = false;
    bool stop_playing_back_when_current_action_steps_done = false;
    int stop_playing_back_at_id = -1;
    void set_is_playing_back(bool b) {
        is_playing_back = b;
    }
    bool get_is_playing_back() const {
        return is_playing_back;
    }

    Ref<YGameAction> add_game_action(const Ref<YGameAction>& ygs, int desired_game_state_id = -1);
    Ref<YGameAction> add_override_game_action(const Ref<YGameAction>& ygs, int desired_game_state_id = -1);
    Ref<YGameAction> add_game_action_with_param(const Ref<YGameAction>& ygs, int desired_initial_param, const Variant &desired_param_data, int desired_game_state_id = -1);
    Ref<YGameAction> add_override_game_action_with_param(const Ref<YGameAction>& ygs, int desired_initial_param, const Variant &desired_param_data, int desired_game_state_id = -1);

    bool has_current_turn_player() const {return current_turn_player != nullptr;}

    YGamePlayer* get_current_turn_player() const {return current_turn_player;}
    void set_current_turn_player(YGamePlayer* _new_current_player) {
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

    void clear_all_players() {
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

    void clear_all_game_actions() {
        if (!current_game_action.is_null() && current_game_action.is_valid()) {
            current_game_action.unref();
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

    
    HashMap<int,Variant> state_parameters;
    void set_state_parameter(int param, Variant v) {state_parameters[param] = v;}
    void remove_state_parameter(int param) {state_parameters.erase(param);}
    Variant get_state_parameter(int param,const Variant& def) {
        if (state_parameters.has(param))
            return state_parameters[param];
        else
            return def;
    }
    bool has_state_parameter(int param) const { return state_parameters.has(param); }

    Dictionary get_all_state_parameters() {
        Dictionary returndict;
        for (auto actpq: state_parameters) {
            returndict[actpq.key] = actpq.value;
        }
        return returndict;
    }
    
    YGameState() {
        singleton = this;
        next_execution_order_number = 0;
        next_game_action_unique_id = 1;
        next_visual_action_unique_id = 1;
        next_player_unique_id = 1;
        next_visual_element_unique_id = 1;
        frame_count_before_doing_slow_process = 0;
        wait_before_going_to_next_action = 0.0;
        wait_before_going_to_next_step = 0.0;
        current_turn_player = nullptr;
        stop_playing_back_when_current_action_steps_done = false;
        has_started = false;
        is_playing_back = false;
    }

    ~YGameState() {
        if(singleton != nullptr && singleton == this) {
            singleton = nullptr;
        }
        current_turn_player = nullptr;
        if (!current_game_action.is_null() && current_game_action.is_valid()) current_game_action.unref();
        overriding_game_actions.clear();
        future_game_actions.clear();
        past_game_actions.clear();
    }


    int get_parallel_action_count() const {
        return current_parallel_actions.size();
    }

    Ref<YGameAction> get_parallel_action(int index) const {
        if (index >= 0 && index < current_parallel_actions.size())
            return current_parallel_actions[index];
        return nullptr;
    }

    TypedArray<YGameAction> get_all_parallel_actions() const {
        TypedArray<YGameAction> arr;
        for (const Ref<YGameAction>& action : current_parallel_actions) {
            if (action.is_valid()) {
                arr.append(action);
            }
        }
        return arr;
    }

    void end_parallel_action(int index);
    void end_all_parallel_actions();
    int count_current_parallel_of_type(const String& action_name) const;
    void check_future_parallel_actions();
    Ref<YGameAction> add_parallel_game_action(const Ref<YGameAction>& ygs, int desired_game_state_id = -1);
    Ref<YGameAction> add_parallel_game_action_with_param(const Ref<YGameAction>& ygs, int desired_initial_param, const Variant& desired_param_data, int desired_game_state_id = -1);
};

#endif //YGAMESTATE_H
