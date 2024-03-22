//
// Created by Daniel on 2024-03-19.
//

#include "ygameplayer.h"

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

Dictionary YGamePlayer::serialize(Dictionary dict) {
    GDVIRTUAL_CALL(_on_serialize,dict,dict);
    return dict;
}

Dictionary YGamePlayer::deserialize(Dictionary dict) {
    GDVIRTUAL_CALL(_on_deserialize,dict,dict);
    return dict;
}