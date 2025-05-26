//
// Created by Daniel on 2024-03-19.
//

#ifndef YGAMEACTION_H
#define YGAMEACTION_H

#include "yarntime.h"
#include "core/io/resource.h"
#include "ygamestate.h"
#include "core/variant/variant.h"
#include "core/object/script_language.h"
#include "scene/main/multiplayer_api.h"
#include "scene/main/multiplayer_peer.h"

class YGameState;
class YGamePlayer;

class YActionStep : public RefCounted {
    GDCLASS(YActionStep, RefCounted);

protected:
    static void _bind_methods();

public:
    int step_index;
    int get_step_index() const {return step_index;}
    bool step_waiting;
    bool get_step_waiting() const{return step_waiting;}
    void set_step_waiting(bool b ){ step_waiting = b;}

    bool step_has_to_reconsume;
    bool get_step_has_to_reconsume() const{return step_has_to_reconsume;}
    void set_step_has_to_reconsume(bool b ){ step_has_to_reconsume = b;}

    bool step_taken;
    bool get_step_taken() const{return step_taken;}
    void set_step_taken(bool b ){ step_taken = b;}
    bool step_taken_as_ending;
    bool get_step_taken_as_ending() const{return step_taken_as_ending;}
    void set_step_taken_as_ending(bool b ){ step_taken_as_ending = b;}
    int step_identifier;
    int get_step_identifier() const {return step_identifier;}
    void set_step_identifier(int b ){ step_identifier = b;}
    Variant step_data;
    Variant get_step_data() const{return step_data;}
    void set_step_data(const Variant & b ){ step_data = b;}

    
    YActionStep() {
        step_index=-1;
        step_taken=false;
        step_identifier=0;
        step_taken_as_ending=false;
        step_waiting = false;
        step_has_to_reconsume = false;
        step_data ={};
    }

    
    YActionStep(int p_index,int p_identifier) {
        step_index=p_index;
        step_taken=false;
        step_taken_as_ending=false;
        step_identifier=p_identifier;
        step_waiting = false;
        step_has_to_reconsume = false;
        step_data ={};
    }
};

class YGameAction : public Resource {
    GDCLASS(YGameAction, Resource);

protected:
    void _notification(int p_what);
    static void _bind_methods();

    void release_step();

public:
    bool instant_execute=false;
    bool is_playing_back=false;
    bool has_executed_created_method = false;
    Variant get_from_step_data(Array p_step_data, int p_get_index, const Variant& p_get_default);

    void set_instant_execute(bool b) { instant_execute = b; }
    bool get_instant_execute() const { return instant_execute; }
    int last_step_ran = 0;
    HashMap<int,Variant> action_parameters;
    HashMap<int,Variant> starting_parameters;
    YGameAction* set_action_parameter(int param, const Variant& v);

    void emit_action_parameter_changed(int param_id, const Variant& old_value, const Variant& new_value);
    
    YGameAction* erase_action_parameter(int param) {
        if (action_parameters.has(param))
            action_parameters.erase(param);
        return this;
    }

    YGameAction* erase_action_parameter_array(Array v) {
        for (int i = 0; i < v.size(); ++i) {
            if (action_parameters.has(v[i]))
                action_parameters.erase(v[i]);
        }
        return this;
    }

    YGameAction* set_action_parameter_array(int param, Array v) {
        if (v.size() % 2 != 0) {
            WARN_PRINT("Attempting to set action parameters with an array that's not even. It needs to follow action parameter then value format.");
        } else {
            for (int i = 0; i < v.size(); i+=2) {
                if (i < v.size() && i+1 < v.size()) {
                    action_parameters[v[i]] = v[i+1];
                }
            }
            return this;
        }
        return this;
    }
    YGameAction* increment_action_parameter(int param, const Variant &v) {
        if (action_parameters.has(param)) {
            Variant current_value = action_parameters[param];
            if (current_value.is_num()) {
                action_parameters[param] = static_cast<float>(current_value) + static_cast<float>(v);
                return this;
            } else {
                WARN_PRINT("Attempting to increment an action parameter that's not a number");
            }
        }
        action_parameters[param] = v; return this;
    }
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
        for (const auto& actpq: action_parameters) {
            returndict[actpq.key] = actpq.value;
        }
        return returndict;
    }

    Dictionary get_all_action_parameters_named(Dictionary naming_dictionary) {
        Dictionary returndict;
        for (const auto& actpq: action_parameters) {
            if (naming_dictionary.values().has(actpq.key)) {
                returndict[naming_dictionary.find_key(actpq.key)] = actpq.value;
            } else {
                returndict[actpq.key] = actpq.value;
            }
        }
        return returndict;
    }

    Vector<Ref<YActionStep>> action_steps;

    void register_step(int step_identifier, Variant v);
    void actually_register_step(int step_identifier, Variant v);

    bool started=false;
    bool finished=false;
    bool waiting_for_step=false;
    bool waiting_for_step_no_processing=false;

    bool can_be_serialized = true;
    void set_can_be_serialized(bool f) { can_be_serialized = f; }
    bool get_can_be_serialized() const { return can_be_serialized; }
    
    bool runs_parallel = false;
    void set_runs_parallel(bool f) { runs_parallel = f; }
    bool get_runs_parallel() const { return runs_parallel; }

    int max_in_parallel = -1;
    void set_max_in_parallel(int f) { max_in_parallel = f; }
    int get_max_in_parallel() const { return max_in_parallel; }
    
    float start_check_interval = 3.0f;
    void set_start_check_interval(float f) { start_check_interval = f; }
    float get_start_check_interval() const { return start_check_interval; }

    float last_start_check_time = 0.0f;
    void set_last_start_check_time(float f) { last_start_check_time = f; }
    float get_last_start_check_time() const { return last_start_check_time; }
    
    bool get_is_last_step(int step_index) const {return step_index == action_steps.size()-1;}
    bool get_waiting_for_step() const {return waiting_for_step;}
    void wait_for_step(bool prevent_processing = false) ;

    bool is_debugging = false;
    void set_is_debugging(bool f) {is_debugging = f;}
    bool get_is_debugging() const {return is_debugging;}

    float time_started;
    void set_time_started(float f) {time_started = f;}
    float get_time_started() const {return time_started;}
    
    int in_state_order = INT16_MAX;
    void set_in_state_order(int f) { in_state_order = f; }
    int get_in_state_order() const { return in_state_order; }

    int steps_consumed;
    void set_steps_consumed(int f) { steps_consumed = f; }
    int get_steps_consumed() const { return steps_consumed; }

    int has_priority = -1;
    void set_priority(int f) { has_priority = f; }
    int get_priority() const { return has_priority; }

    bool only_starts_if();

    int get_all_steps_count() const {
        return static_cast<int>(action_steps.size());
    }
    Ref<YActionStep> get_step_by_index(int p_index) const {
        for (const auto & action_step : action_steps) {
            if (action_step != nullptr && action_step->step_index == p_index) {
                return action_step;
            }
        }
        return nullptr;
    }

    // ClassDB::bind_method(D_METHOD("get_step_by_index","step_index"), &YGameAction::get_step_by_index);
    // ClassDB::bind_method(D_METHOD("get_all_steps_count"), &YGameAction::get_all_steps_count);


    Ref<YGameAction> set_player_turn_id(const int f) {
        player_turn = f;
        return this;
    }

    int player_turn;
    void set_player_turn(int f) {
        player_turn = f;
    }
    int get_player_turn() const {
        return player_turn;
    }

    int unique_id;
    void set_unique_id(int f) {unique_id = f;}
    int get_unique_id() const {return unique_id;}

    float pause_independent_time_started;
    void set_pause_indp_time_started(float f) {pause_independent_time_started = f;}
    float get_pause_indp_time_started() const {return pause_independent_time_started;}
    bool has_elapsed_since_started(float f) const { return YTime::get_singleton()->has_time_elapsed(time_started,f);}
    bool has_pause_indp_elapsed_since_started(float f) const { return YTime::get_singleton()->has_time_elapsed(pause_independent_time_started,f);}

    bool executed_exit_action_call=false;

    virtual void end_action(bool sync_to_others = true);

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
    virtual void step_action(Ref<YActionStep> step_data,bool is_ending);
    virtual void exit_action();
    virtual bool process_action(float _delta);
    virtual bool slow_process_action(float _delta);

    virtual Dictionary serialize();
    virtual Dictionary deserialize(Dictionary dict);

    void created();

    GDVIRTUAL0(_on_created)
    GDVIRTUAL0(_on_enter_action)
    GDVIRTUAL4(_on_stepped_action,int,int,Variant,bool)
    GDVIRTUAL4(_on_waiting_step_released,int,int,Variant,bool)
    GDVIRTUAL0(_on_exit_action)
    GDVIRTUAL1RC(bool, _on_process_action,float)
    GDVIRTUAL1RC(bool, _on_slow_process_action,float)
    GDVIRTUAL1RC(Dictionary,_on_serialize,Dictionary)
    GDVIRTUAL1RC(Dictionary,_on_deserialize,Dictionary)
    GDVIRTUAL0RC(bool, _only_starts_if)

    // Virtual function for step approval
    GDVIRTUAL3RC(bool, _step_request_approval, int, Variant, int)

    // Method to request step approval through YEngine
    void request_step_approval(int step_identifier, const Variant& step_data);

    bool check_if_has_step_approval(int step_identifier, const Variant& step_data, int sender_id);

    Error broadcast_call(const Variant **p_args, int p_argcount, Callable::CallError &r_error);
    Error broadcast_call_and_execute(const Variant **p_args, int p_argcount, Callable::CallError &r_error);

    YGameAction();
    ~YGameAction() {
        
    }

};



#endif //YGAMEACTION_H
