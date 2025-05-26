//
// Created by Daniel on 2024-03-19.
//

#include "ygameplayer.h"

void YGamePlayer::_notification(int p_what) {
    if (p_what == NOTIFICATION_READY) {
        this->rpc_config(rpc_set_property_stringname,YGameState::get_singleton()->create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_ANY_PEER, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
        this->rpc_config(rpc_set_player_parameter_stringname,YGameState::get_singleton()->create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_ANY_PEER, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
        this->rpc_config(rpc_remove_player_parameter_stringname,YGameState::get_singleton()->create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_ANY_PEER, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
        this->rpc_config(rpc_request_sync_stringname,YGameState::get_singleton()->create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_ANY_PEER, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
        this->rpc_config(rpc_grant_sync_stringname,YGameState::get_singleton()->create_rpc_dictionary_config(MultiplayerAPI::RPC_MODE_ANY_PEER, MultiplayerPeer::TRANSFER_MODE_RELIABLE, false, 0));
        #ifdef YNET
        print_line(vformat("YGamePlayer with YNet spawning %s", get_name()));
        #endif
    }
}


void YGamePlayer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_player_id", "player_id"), &YGamePlayer::set_player_id);
    ClassDB::bind_method(D_METHOD("get_player_id"), &YGamePlayer::get_player_id);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "player_id"), "set_player_id", "get_player_id");

    ClassDB::bind_method(D_METHOD("set_player_type", "player_type"), &YGamePlayer::set_player_type);
    ClassDB::bind_method(D_METHOD("get_player_type"), &YGamePlayer::get_player_type);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "player_type"), "set_player_type", "get_player_type");
    
    ClassDB::bind_method(D_METHOD("set_player_controller", "player_controller"), &YGamePlayer::set_player_controller);
    ClassDB::bind_method(D_METHOD("get_player_controller"), &YGamePlayer::get_player_controller);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "player_controller"), "set_player_controller", "get_player_controller");
    
    ClassDB::bind_method(D_METHOD("set_is_remote", "remote"), &YGamePlayer::set_is_remote);
    ClassDB::bind_method(D_METHOD("get_is_remote"), &YGamePlayer::get_is_remote);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_remote"), "set_is_remote", "get_is_remote");

    ClassDB::bind_method(D_METHOD("set_remote_player_id", "remote_player_id"), &YGamePlayer::set_remote_player_id);
    ClassDB::bind_method(D_METHOD("get_remote_player_id"), &YGamePlayer::get_remote_player_id);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "remote_player_id"), "set_remote_player_id", "get_remote_player_id");

    ClassDB::bind_method(D_METHOD("set_owns_current_turn", "status"), &YGamePlayer::set_owns_current_turn);
    ClassDB::bind_method(D_METHOD("get_owns_current_turn"), &YGamePlayer::get_owns_current_turn);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "owns_current_turn"), "set_owns_current_turn", "get_owns_current_turn");


    ClassDB::bind_method(D_METHOD("set_player_parameter","param_id", "param_value"), &YGamePlayer::set_player_parameter);
    ClassDB::bind_method(D_METHOD("get_player_parameter","param_id","param_default"), &YGamePlayer::get_player_parameter);
    ClassDB::bind_method(D_METHOD("has_player_parameter","param_id"), &YGamePlayer::has_player_parameter);
    ClassDB::bind_method(D_METHOD("remove_player_parameter","param_id"), &YGamePlayer::remove_player_parameter);
    ClassDB::bind_method(D_METHOD("get_all_player_parameters"), &YGamePlayer::get_all_player_parameters);

    ClassDB::bind_method(D_METHOD("request_sync"), &YGamePlayer::request_sync);

    ClassDB::bind_method(D_METHOD("serialize"), &YGamePlayer::serialize);
    ClassDB::bind_method(D_METHOD("deserialize", "dict"), &YGamePlayer::deserialize);

    ADD_SIGNAL(MethodInfo("player_type_changed", PropertyInfo(Variant::INT, "new_player_type")));
    ADD_SIGNAL(MethodInfo("player_parameter_changed", PropertyInfo(Variant::INT, "param_id"), PropertyInfo(Variant::VARIANT_MAX, "old_value"), PropertyInfo(Variant::VARIANT_MAX, "new_value")));
    ADD_SIGNAL(MethodInfo("player_parameter_removed", PropertyInfo(Variant::INT, "param_id")));
    
    GDVIRTUAL_BIND(_on_player_registered)
    GDVIRTUAL_BIND(_on_turn_started)
    GDVIRTUAL_BIND(_on_turn_ended)
    GDVIRTUAL_BIND(_on_player_parameter_changed,"param_id","old_value","new_value")
    GDVIRTUAL_BIND(_on_player_parameter_removed,"param_id")
    GDVIRTUAL_BIND(_on_serialize,"dict")
    GDVIRTUAL_BIND(_on_deserialize,"dict")

    // Add RPC methods
    ClassDB::bind_method(D_METHOD("_rpc_set_player_parameter", "param_id", "param_value"), &YGamePlayer::_rpc_set_player_parameter);
    ClassDB::bind_method(D_METHOD("_rpc_remove_player_parameter", "param_id"), &YGamePlayer::_rpc_remove_player_parameter);
    ClassDB::bind_method(D_METHOD("_rpc_set_property", "property_name", "value"), &YGamePlayer::_rpc_set_property);
    ClassDB::bind_method(D_METHOD("_rpc_request_sync"), &YGamePlayer::_rpc_request_sync);
    ClassDB::bind_method(D_METHOD("_rpc_grant_sync", "dict"), &YGamePlayer::_rpc_grant_sync);
}

void YGamePlayer::set_player_type(int v) {
    if (is_inside_tree() && get_multiplayer()->has_multiplayer_peer() && remote_player_id == get_multiplayer()->get_unique_id()) {
        Array p_arguments;
        p_arguments.push_back("player_type");
        p_arguments.push_back(v);
        int argcount = p_arguments.size();
        const Variant **argptrs = (const Variant **)alloca(sizeof(Variant *) * argcount);
        for (int i = 0; i < argcount; i++) {
            argptrs[i] = &p_arguments[i];
        }
#ifdef YNET
        if (YNet::get_singleton() != nullptr)
            YNet::get_singleton()->_send_yrpc_direct(this, rpc_set_property_stringname, argptrs, argcount);
#else
        rpcp(0, rpc_set_property_stringname, argptrs, argcount);
#endif
    }
    player_type = v;
    emit_signal("player_type_changed",v);
}

void YGamePlayer::set_player_id(int v) {
    if (is_inside_tree() && get_multiplayer()->has_multiplayer_peer() && remote_player_id == get_multiplayer()->get_unique_id()) {
        Array p_arguments;
        p_arguments.push_back("player_id");
        p_arguments.push_back(v);
        int argcount = p_arguments.size();
        const Variant **argptrs = (const Variant **)alloca(sizeof(Variant *) * argcount);
        for (int i = 0; i < argcount; i++) {
            argptrs[i] = &p_arguments[i];
        }
#ifdef YNET
        if (YNet::get_singleton() != nullptr)
            YNet::get_singleton()->_send_yrpc_direct(this, rpc_set_property_stringname, argptrs, argcount);
#else
        rpcp(0, rpc_set_property_stringname, argptrs, argcount);
#endif
    }
#ifdef YNET
    if (YNet::get_singleton() != nullptr) {
        YNet::get_singleton()->register_for_yrpcs(this, player_id);
    }
#endif
    player_id = v;
}

void YGamePlayer::set_remote_player_id(int v) {
    if (is_inside_tree() && get_multiplayer()->has_multiplayer_peer() && get_multiplayer()->get_unique_id() == 1) {
        Array p_arguments;
        p_arguments.push_back("remote_player_id");
        p_arguments.push_back(v);
        int argcount = p_arguments.size();
        const Variant **argptrs = (const Variant **)alloca(sizeof(Variant *) * argcount);
        for (int i = 0; i < argcount; i++) {
            argptrs[i] = &p_arguments[i];
        }
#ifdef YNET
        if (YNet::get_singleton() != nullptr)
            YNet::get_singleton()->_send_yrpc_direct(this, rpc_set_property_stringname, argptrs, argcount);
#else
        rpcp(0, rpc_set_property_stringname, argptrs, argcount);
#endif
    }
    remote_player_id = v;
}

void YGamePlayer::player_registered() {
    GDVIRTUAL_CALL(_on_player_registered);
}

void YGamePlayer::turn_started() {
    GDVIRTUAL_CALL(_on_turn_started);
}

void YGamePlayer::turn_ended() {
    GDVIRTUAL_CALL(_on_turn_ended);
}

void YGamePlayer::request_sync() {
    rpc("_rpc_request_sync");
}

YGamePlayer* YGamePlayer::set_player_parameter(int param, Variant v) {
    // If this is a remote player and we're the owner, send RPC
    if (is_inside_tree() && get_multiplayer()->has_multiplayer_peer() && remote_player_id == get_multiplayer()->get_unique_id()) {
        Array p_arguments;
        p_arguments.push_back(param);
        p_arguments.push_back(v);
        int argcount = p_arguments.size();
        const Variant **argptrs = (const Variant **)alloca(sizeof(Variant *) * argcount);
        for (int i = 0; i < argcount; i++) {
            argptrs[i] = &p_arguments[i];
        }
#ifdef YNET
        if (YNet::get_singleton() != nullptr)
            YNet::get_singleton()->_send_yrpc_direct(this, rpc_set_player_parameter_stringname, argptrs, argcount);
#else
        rpcp(0, rpc_set_player_parameter_stringname, argptrs, argcount);
#endif
    }
    
    if (player_parameters.has(param)) {
        Variant old_value = player_parameters[param];
        player_parameters[param] = v;
        GDVIRTUAL_CALL(_on_player_parameter_changed,param,old_value, v);
        emit_signal("player_parameter_changed",param,old_value, v);
    } else {
        player_parameters[param] = v;
        GDVIRTUAL_CALL(_on_player_parameter_changed,param, v, v);
        emit_signal("player_parameter_changed",param, v, v);
    }
    return this;
}

YGamePlayer* YGamePlayer::remove_player_parameter(int param) {
    // If this is a remote player and we're the owner, send RPC
    if (is_inside_tree() && get_multiplayer()->has_multiplayer_peer() && remote_player_id == get_multiplayer()->get_unique_id()) {
        Array p_arguments;
        p_arguments.push_back(param);
        int argcount = p_arguments.size();
        const Variant **argptrs = (const Variant **)alloca(sizeof(Variant *) * argcount);
        for (int i = 0; i < argcount; i++) {
            argptrs[i] = &p_arguments[i];
        }
#ifdef YNET
        if (YNet::get_singleton() != nullptr)
            YNet::get_singleton()->_send_yrpc_direct(this, rpc_remove_player_parameter_stringname, argptrs, argcount);
#else
        rpcp(0, rpc_remove_player_parameter_stringname, argptrs, argcount);
#endif
    }

    if (player_parameters.has(param)) {
        Variant old_value = player_parameters[param];
        player_parameters.erase(param);
        GDVIRTUAL_CALL(_on_player_parameter_removed,param);
        emit_signal("player_parameter_removed",param);
    }
    return this;
}

void YGamePlayer::_rpc_set_player_parameter(int param, Variant v) {
    // Verify that the sender is authorized to modify this player
    if (is_inside_tree() && get_multiplayer()->has_multiplayer_peer()
         && (remote_player_id != get_multiplayer()->get_remote_sender_id() && get_multiplayer()->get_remote_sender_id() != 1)) {
        return; // Ignore RPC if sender is not authorized
    }

    if (player_parameters.has(param)) {
        Variant old_value = player_parameters[param];
        player_parameters[param] = v;
        GDVIRTUAL_CALL(_on_player_parameter_changed,param,old_value,v);
        emit_signal("player_parameter_changed",param,old_value,v);
    } else {
        player_parameters[param] = v;
        GDVIRTUAL_CALL(_on_player_parameter_changed,param, v, v);
        emit_signal("player_parameter_changed",param, v, v);
    }
}

void YGamePlayer::_rpc_remove_player_parameter(int param) {
    // Verify that the sender is authorized to modify this player
    if (get_multiplayer()->get_unique_id() != 1 && get_multiplayer()->get_remote_sender_id() != 1 && (!is_remote || remote_player_id != get_multiplayer()->get_remote_sender_id())) {
        return; // Ignore RPC if sender is not authorized
    }

    if (player_parameters.has(param)) {
        Variant old_value = player_parameters[param];
        player_parameters.erase(param);
        GDVIRTUAL_CALL(_on_player_parameter_removed,param);
        emit_signal("player_parameter_removed",param);
    }
}

void YGamePlayer::_rpc_set_property(const String& property_name, const Variant& value) {
    // Verify that the sender is authorized to modify this player
    if (get_multiplayer()->get_unique_id() != 1 && get_multiplayer()->get_remote_sender_id() != 1 && (!is_remote || remote_player_id != get_multiplayer()->get_remote_sender_id())) {
        return; // Ignore RPC if sender is not authorized
    }

    if (property_name == "player_id") {
        player_id = value;
#ifdef YNET
        if (YNet::get_singleton() != nullptr)
            YNet::get_singleton()->register_for_yrpcs(this, player_id);
#endif
    } else if (property_name == "player_type") {
        player_type = value;
        emit_signal("player_type_changed",value);
    } else if (property_name == "player_controller") {
        player_controller = value;
    } else if (property_name == "is_remote") {
        is_remote = value;
    } else if (property_name == "remote_player_id") {
        remote_player_id = value;
    }
}

void YGamePlayer::_rpc_request_sync() {
    if (get_multiplayer()->get_unique_id() == 1) {
        Array p_arguments;
        p_arguments.push_back(serialize());
        int argcount = p_arguments.size();
        const Variant **argptrs = (const Variant **)alloca(sizeof(Variant *) * argcount);
        for (int i = 0; i < argcount; i++) {
            argptrs[i] = &p_arguments[i];
        }
#ifdef YNET
        if (YNet::get_singleton() != nullptr)
            YNet::get_singleton()->_send_yrpc_to_direct(this, get_multiplayer()->get_remote_sender_id(), rpc_grant_sync_stringname, argptrs, argcount);
#else
        rpcp(get_multiplayer()->get_remote_sender_id(), rpc_grant_sync_stringname, argptrs, argcount);
#endif
    }
}

void YGamePlayer::_rpc_grant_sync(Dictionary dict) {
    if (get_multiplayer()->get_unique_id() != 1 && (!is_remote || remote_player_id != get_multiplayer()->get_remote_sender_id())) {
        return; // Ignore RPC if sender is not authorized
    }
    deserialize(dict);
}

Dictionary YGamePlayer::serialize() {
    Dictionary dict;
    if (player_class.is_empty()) {
        Ref<Script> _find_script = get_script();
        if (_find_script.is_valid()) {
            String string_global_name = _find_script->get_global_name();
            if (!string_global_name.is_empty()) {
                player_class = string_global_name;
            }
        }
    }
    if (!player_class.is_empty()) {
        dict["player_class"] = player_class;
    }
    dict["id"] = player_id;
    dict["type"] = player_type;
    dict["controller"] = player_controller;
    dict["parent_path"] = get_parent()->get_path();
    dict["node_name"] = get_name();
    dict["is_remote"] = is_remote;
    dict["remote_player_id"] = remote_player_id;

    if (!player_parameters.is_empty()) {
        Dictionary serialize_player_parameters;
        for (const auto& action_parameter: player_parameters) {
            serialize_player_parameters[action_parameter.key] = action_parameter.value;
        }
        dict["params"] = serialize_player_parameters;
    }

    GDVIRTUAL_CALL(_on_serialize,dict,dict);
    return dict;
}

Dictionary YGamePlayer::deserialize(Dictionary dict) {
    player_id = dict.get("id",0);
    player_type = dict.get("type",0);
    player_controller = dict.get("controller",0);
    is_remote = dict.get("is_remote",false);
    remote_player_id = dict.get("remote_player_id",0);
    owns_current_turn = dict.get("owns_current_turn",false);
    set_name(dict.get("node_name",get_name()));

    if (dict.has("params")) {
        Dictionary state_parameters_dict = dict["params"];
        player_parameters.clear();
        for (int i = 0; i < state_parameters_dict.size(); i++) {
            Variant key = state_parameters_dict.get_key_at_index(i);
            player_parameters[key.operator int()] = state_parameters_dict.get_value_at_index(i);
        }
    }

    GDVIRTUAL_CALL(_on_deserialize,dict,dict);
    return dict;
}