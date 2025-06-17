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
#include "core/config/project_settings.h"
#include "modules/multiplayer/scene_multiplayer.h"

/// Forward Declaring YGameAction for use.
class YGameAction;
class YGamePlayer;
class YVisualAction;
class YEngine;
class YActionStep;
///////////////////////////////
/// YGAMESTATE
///////////////////////////////

class YGameState : public Node {
    GDCLASS(YGameState, Node);

private:
    struct YGSRPCConfig {
		StringName name;
		MultiplayerAPI::RPCMode rpc_mode = MultiplayerAPI::RPC_MODE_DISABLED;
		bool call_local = false;
		MultiplayerPeer::TransferMode transfer_mode = MultiplayerPeer::TRANSFER_MODE_RELIABLE;
		int channel = 0;

		bool operator==(YGSRPCConfig const &p_other) const {
			return name == p_other.name;
		}
	};

    struct YGSRPCConfigCache {
        HashMap<uint16_t, YGSRPCConfig> configs;
        HashMap<StringName, uint16_t> ids;
    };

    // Cache for RPC configs, keyed by ObjectID
    HashMap<ObjectID, YGSRPCConfigCache> rpc_config_cache;
    

protected:
    void _parse_rpc_config(const Dictionary &p_config, bool p_for_node, YGSRPCConfigCache &r_cache);
    YGSRPCConfig _get_rpc_config(Object *p_node, const StringName &p_method);
    static YGameState* singleton;
    static void _bind_methods();

    Ref<SceneMultiplayer> scene_multiplayer;

public:
    static YGameState *get_singleton();
    void _notification(int p_what);

    bool has_started=false;

    HashSet<Vector2i> state_parameter_authorities;

    int next_execution_order_number;
    int next_player_unique_id;
    int get_next_player_unique_id() const {return next_player_unique_id;}
    int next_game_action_unique_id;
    int next_visual_action_unique_id;
    int next_visual_element_unique_id;
    
    String get_current_game_action_name();

    int debugging_level = 0;
    void set_debugging_level(int f) {debugging_level = f;}
    int get_debugging_level() const {return debugging_level;}

    bool last_action_was_denied = false;
    void set_last_action_was_denied(bool f) {last_action_was_denied = f;}
    bool get_last_action_was_denied() const {return last_action_was_denied;}

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

    bool is_waiting_for_next_action_approval = false;
    void set_is_waiting_for_next_action_approval(bool b);
    bool get_is_waiting_for_next_action_approval() const { return is_waiting_for_next_action_approval; }

    int pending_waiting_for_start_approval_attempts = 0;

    int game_actions_done_since_started_counting = 0;
    int get_game_actions_counted() const {return game_actions_done_since_started_counting;}
    void restart_action_counting() {
        game_actions_done_since_started_counting = 0;
    }
    int prevent_proceed_to_next_action = 0;
    void set_prevent_proceed_to_next_action(int v) { prevent_proceed_to_next_action = MAX(v,0); }
    int get_prevent_proceed_to_next_action() const { return prevent_proceed_to_next_action; }

    bool showed_out_of_actions_message=false;

    int register_player(YGamePlayer* ygp, String rename_to="");

    int register_player_specific_id(YGamePlayer *ygp, int desired_id, String rename_to="");

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

    Ref<YGameAction> get_game_action(int netid) const;

    bool has_current_turn_player() const {return current_turn_player != nullptr;}

    YGamePlayer* get_current_turn_player() const {return current_turn_player;}
    void set_current_turn_player(YGamePlayer* _new_current_player);

    void clear_all_players();
    void clear_all_game_actions();

    bool ensure_has_initialized();
    
    HashMap<int,Variant> state_parameters;

    void add_state_parameter_authority(int param, int network_id_authority);
    void remove_state_parameter_authority(int param, int network_id_authority);
    bool has_state_parameter_authority(int param, int network_id_authority) const;
    void clear_state_parameter_authorities() {state_parameter_authorities.clear();}

    void actually_set_state_parameter(int param, Variant v) {state_parameters[param] = v;}
    void set_state_parameter(int param, Variant v);
    void remove_state_parameter(int param);
    void actually_remove_state_parameter(int param) {state_parameters.erase(param);}
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
        rpc_request_action_step_approval_stringname = SNAME("_rpc_request_action_step_approval");
        rpc_apply_action_step_stringname = SNAME("_rpc_apply_action_step");
        rpc_register_game_action_stringname = SNAME("_rpc_register_game_action");
        _receive_call_on_game_action_also_local_stringname = SNAME("_receive_call_on_game_action_also_local");
        _receive_call_on_game_action_stringname = SNAME("_receive_call_on_game_action");
        rpc_end_game_action_stringname = SNAME("_rpc_end_game_action");
        rpc_mark_action_finished_stringname = SNAME("_rpc_mark_action_finished");
        rpc_request_start_action_approval_stringname = SNAME("_rpc_request_start_action_approval");
        rpc_response_start_action_approval_stringname = SNAME("_rpc_response_start_action_approval");
        rpc_set_state_parameter_stringname = SNAME("_rpc_set_state_parameter");
        rpc_remove_state_parameter_stringname = SNAME("_rpc_remove_state_parameter");
        rpc_past_actions_order_stringname = SNAME("_rpc_past_actions_order");
        rpc_acknowledge_past_actions_order_stringname = SNAME("_rpc_acknowledge_past_actions_order");
        rpc_request_specific_action_resend_stringname = SNAME("_rpc_request_specific_action_resend");
        rpc_grant_specific_action_resend_stringname = SNAME("_rpc_grant_specific_action_resend");
    }

    ~YGameState() {
        clear_state_parameter_authorities();
        tracking_past_actions_order.clear();
        rpc_config_cache.clear();
        if(singleton != nullptr && singleton == this) {
            singleton = nullptr;
        }
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


    // Action step RPC methods
    void request_action_step_approval(YGameAction* action, int step_identifier, const Variant& step_data);
    void broadcast_action_step(YGameAction* action, int step_identifier, const Variant& step_data);
    void _rpc_request_action_step_approval(int action_id, int step_identifier, const Variant& step_data);
    void _rpc_apply_action_step(int action_id, int step_identifier, const Variant& step_data);

    // State parameter RPC methods
    void _rpc_set_state_parameter(int param_id, const Variant& param_value);
    void _rpc_remove_state_parameter(int param_id);

    StringName rpc_request_action_step_approval_stringname;
    StringName rpc_apply_action_step_stringname;
    StringName rpc_end_game_action_stringname;
    StringName _receive_call_on_game_action_stringname;
    StringName _receive_call_on_game_action_also_local_stringname;
    StringName rpc_register_game_action_stringname;
    StringName rpc_mark_action_finished_stringname;
    StringName rpc_response_start_action_approval_stringname;
    StringName rpc_request_start_action_approval_stringname;
    StringName rpc_set_state_parameter_stringname;
    StringName rpc_remove_state_parameter_stringname;
    StringName rpc_past_actions_order_stringname;
    StringName rpc_acknowledge_past_actions_order_stringname;
    StringName rpc_request_specific_action_resend_stringname;
    StringName rpc_grant_specific_action_resend_stringname;

    static Dictionary create_rpc_dictionary_config(MultiplayerAPI::RPCMode p_rpc_mode,
                                            MultiplayerPeer::TransferMode p_transfer_mode, bool p_call_local,
                                            int p_channel);

    // Game action RPC methods
    void broadcast_game_action(const Dictionary& action_data);
    void broadcast_action_end(int action_id);
    Error broadcast_call_on_game_action(const Variant **p_args, int p_argcount, Callable::CallError &r_error);
    Error broadcast_call_on_game_action_and_execute(const Variant **p_args, int p_argcount, Callable::CallError &r_error);
    void _rpc_end_game_action(int action_id);
    void _rpc_mark_action_finished(int action_id);
    Variant _receive_call_on_game_action(const Variant **p_args, int p_argcount, Callable::CallError &r_error);
    void _rpc_register_game_action(const Dictionary& action_data);
    void send_past_actions_order_to_client(int client_id);
    void _rpc_past_actions_order(const TypedArray<int>& past_actions_order);
    void _rpc_acknowledge_past_actions_order(const TypedArray<int>& past_actions_order);
    void _rpc_request_specific_action_resend(int action_id);
    void _rpc_grant_specific_action_resend(int action_id, const Dictionary& action_data);

    HashMap<int,Vector<int>> tracking_past_actions_order;

    bool has_valid_multiplayer_peer() const;

    void mark_action_finished_and_sync(int action_id);

    // Add new RPC method declarations
    void _rpc_request_start_action_approval(int previous_action_id, int action_id);
    void _rpc_response_start_action_approval(int action_id, int desired_action_id, int desired_previous_action_id, bool approved);
    void request_action_approval(YGameAction* action);
};

#endif //YGAMESTATE_H
