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

/// Forward Declaring YGameAction for use.
class YGameAction;
class YGamePlayer;
class YVisualAction;
class YEngine;

///////////////////////////////
/// YGAMESTATE
///////////////////////////////

class YGameState : public RefCounted {
    GDCLASS(YGameState, RefCounted);

protected:
    static Ref<YGameState> singleton;
    static void _bind_methods();

public:
    static YGameState *get_singleton();

    static void set_singleton(const Ref<YGameState> &ref) {
        singleton = ref;
    }
    bool has_started=false;
    int next_game_action_unique_id;
    int next_visual_action_unique_id;
    int next_player_unique_id;
    int next_visual_element_unique_id;
    double wait_before_going_to_next_action =0.0;
    void set_wait_before_going_to_next_action(double f) {wait_before_going_to_next_action = f;}
    double get_wait_before_going_to_next_action() const {return wait_before_going_to_next_action;}

    YGamePlayer* current_turn_player;
    Ref<YGameAction> current_game_action;
    Vector<Ref<YGameAction>> overriding_game_actions;
    Vector<Ref<YGameAction>> future_game_actions;
    Vector<Ref<YGameAction>> past_game_actions;
    HashMap<int,YGamePlayer*> game_players;
    HashMap<int,YVisualElement3D*> visual_elements_3d;

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
    bool has_current_game_action() const {
        return !current_game_action.is_null() && current_game_action.is_valid();
    }

    bool end_current_game_action() const;

    Ref<YGameAction> add_game_action(const Ref<YGameAction>& ygs, int desired_game_state_id = -1);

    Ref<YGameAction> add_override_game_action(const Ref<YGameAction>& ygs, int desired_game_state_id = -1);

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
                gmp.value->queue_free();
            }
        }
        game_players.clear();
    }
    void clear_all_game_actions() {
        if (!current_game_action.is_null() && current_game_action.is_valid()) {
            current_game_action.unref();
        }
        overriding_game_actions.clear();
        future_game_actions.clear();
        past_game_actions.clear();
        showed_out_of_actions_message=false;
    }

    YGameState() {
        next_game_action_unique_id = 1;
        next_visual_action_unique_id = 1;
        next_player_unique_id = 1;
        next_visual_element_unique_id = 1;
        frame_count_before_doing_slow_process = 0;
        current_turn_player = nullptr;
        has_started = false;
    }

    ~YGameState() {
        if(singleton.is_valid() && singleton == this) {
            singleton.unref();
        }
        current_turn_player = nullptr;
        if (!current_game_action.is_null() && current_game_action.is_valid()) current_game_action.unref();
        overriding_game_actions.clear();
        future_game_actions.clear();
        past_game_actions.clear();
    }
};

#endif //YGAMESTATE_H
