//
// Created by Daniel on 2024-03-19.
//

#ifndef YGAMEACTION_H
#define YGAMEACTION_H

#include "yarntime.h"
#include "core/io/resource.h"
#include "ygamestate.h"

class YGameState;
class YGamePlayer;

class YGameAction : public Resource {
    GDCLASS(YGameAction, Resource);

protected:
    void _notification(int p_what);
    static void _bind_methods();

public:
    HashMap<int,Variant> action_parameters;
    YGameAction* set_action_parameter(int param, Variant v) {action_parameters[param] = v; return this;}
    YGameAction* remove_action_parameter(int param) {action_parameters.erase(param); return this;}
    Variant get_action_parameter(int param,const Variant& def) {
        if (action_parameters.has(param))
            return action_parameters[param];
        else
            return def;
    }
    bool has_action_parameter(int param) const { return action_parameters.has(param); }

    Dictionary get_all_action_parameters() {
        Dictionary returndict;
        for (auto actpq: action_parameters) {
            returndict[actpq.key] = actpq.value;
        }
        return returndict;
    }
    bool started=false;
    bool finished=false;
    float time_started;
    void set_time_started(float f) {time_started = f;}
    float get_time_started() {return time_started;}

    int player_turn;
    void set_player_turn(int f) {
        player_turn = f;
    }
    int get_player_turn() const {
        return player_turn;
    }

    int unique_id;
    void set_unique_id(int f) {unique_id = f;}
    int get_unique_id() {return unique_id;}

    float pause_independent_time_started;
    void set_pause_indp_time_started(float f) {pause_independent_time_started = f;}
    float get_pause_indp_time_started() {return pause_independent_time_started;}
    bool has_elapsed_since_started(float f) { return YTime::get_singleton()->has_time_elapsed(time_started,f);}
    bool has_pause_indp_elapsed_since_started(float f) { return YTime::get_singleton()->has_time_elapsed(pause_independent_time_started,f);}

    bool executed_exit_action_call=false;
    virtual void end_action();

    YGameAction* copy_parameters_from(const Ref<YGameAction> &other_action) {
        if (other_action.is_valid()) {
            for (const auto& key_value: other_action->action_parameters) {
                action_parameters[key_value.key] = key_value.value;
                player_turn = other_action->player_turn;
            }
        }
        return this;
    }

    YGameAction* copy_parameters_to(const Ref<YGameAction> &other_action) {
        if (other_action.is_valid()) {
            for (const auto& key_value: action_parameters) {
                other_action->action_parameters[key_value.key] = key_value.value;
                other_action->player_turn = player_turn;
            }
        }
        return this;
    }
    virtual void enter_action();
    virtual void step_action();
    virtual void exit_action();
    virtual bool process_action(float _delta);
    virtual bool slow_process_action(float _delta);

    virtual Dictionary serialize(Dictionary dict);
    virtual Dictionary deserialize(Dictionary dict);

    void created();

    GDVIRTUAL0(_on_created)
    GDVIRTUAL0(_on_enter_action)
    GDVIRTUAL0(_on_stepped_action)
    GDVIRTUAL0(_on_exit_action)
    GDVIRTUAL1RC(bool, _on_process_action,float)
    GDVIRTUAL1RC(bool, _on_slow_process_action,float)
    GDVIRTUAL1RC(Dictionary,_on_serialize,Dictionary)
    GDVIRTUAL1RC(Dictionary,_on_deserialize,Dictionary)

    YGameAction();
};



#endif //YGAMEACTION_H
