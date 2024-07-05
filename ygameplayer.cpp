//
// Created by Daniel on 2024-03-19.
//

#include "ygameplayer.h"

#include "core/object/script_language.h"

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

    ClassDB::bind_method(D_METHOD("set_owns_current_turn", "status"), &YGamePlayer::set_owns_current_turn);
    ClassDB::bind_method(D_METHOD("get_owns_current_turn"), &YGamePlayer::get_owns_current_turn);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "owns_current_turn"), "set_owns_current_turn", "get_owns_current_turn");

    ClassDB::bind_method(D_METHOD("set_player_parameter","param_id", "param_value"), &YGamePlayer::set_player_parameter);
    ClassDB::bind_method(D_METHOD("get_player_parameter","param_id","param_default"), &YGamePlayer::get_player_parameter);
    ClassDB::bind_method(D_METHOD("has_player_parameter","param_id"), &YGamePlayer::has_player_parameter);
    ClassDB::bind_method(D_METHOD("remove_player_parameter","param_id"), &YGamePlayer::remove_player_parameter);
    ClassDB::bind_method(D_METHOD("get_all_player_parameters"), &YGamePlayer::get_all_player_parameters);

    ClassDB::bind_method(D_METHOD("serialize"), &YGamePlayer::serialize);
    ClassDB::bind_method(D_METHOD("deserialize", "dict"), &YGamePlayer::deserialize);

    GDVIRTUAL_BIND(_on_player_registered)
    GDVIRTUAL_BIND(_on_turn_started)
    GDVIRTUAL_BIND(_on_turn_ended)
    GDVIRTUAL_BIND(_on_serialize,"dict")
    GDVIRTUAL_BIND(_on_deserialize,"dict")
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