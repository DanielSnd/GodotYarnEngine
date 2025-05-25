//
// Created by Daniel on 2024-03-19.
//

#ifndef YGAMEPLAYER_H
#define YGAMEPLAYER_H
#include "scene/main/node.h"
#include "core/variant/variant.h"
#include "core/variant/dictionary.h"
#include "scene/main/multiplayer_api.h"
#include "scene/main/multiplayer_peer.h"
#include "core/object/script_language.h"
#include "ygamestate.h"
class YGameState;
#ifdef YNET
#include "../yarnnet/yarnnet.h"
class YNet;
#endif

class YGamePlayer : public Node {
    GDCLASS(YGamePlayer, Node);

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:
    String player_class;
    int player_id;
    void set_player_id(int v);
    int get_player_id() const {return player_id;}
    int player_type;
    void set_player_type(int v);
    int get_player_type() const {return player_type;}
    int player_controller;
    void set_player_controller(int v) {
        player_controller = v;
    }
    int get_player_controller() const {return player_controller;}

    bool is_remote;
    void set_is_remote(bool v) {is_remote = v;}
    bool get_is_remote() const {return is_remote;}

    int remote_player_id;
    void set_remote_player_id(int v);
    int get_remote_player_id() const {return remote_player_id;}

    bool owns_current_turn;
    void set_owns_current_turn(bool v) {owns_current_turn = v;}
    bool get_owns_current_turn() const {return owns_current_turn;}

    HashMap<int,Variant> player_parameters;
    YGamePlayer* set_player_parameter(int param, Variant v);
    YGamePlayer* remove_player_parameter(int param);
    Variant get_player_parameter(int param,const Variant& def) {
        if (player_parameters.has(param))
            return player_parameters[param];
        else
            return def;
    }
    bool has_player_parameter(int param) const { return player_parameters.has(param); }

    Dictionary get_all_player_parameters() {
        Dictionary returndict;
        for (auto actpq: player_parameters) {
            returndict[actpq.key] = actpq.value;
        }
        return returndict;
    }

    void player_registered();
    void turn_started();
    void turn_ended();

    Dictionary serialize();

    Dictionary deserialize(Dictionary dict);

    GDVIRTUAL0(_on_player_registered)
    GDVIRTUAL0(_on_turn_started)
    GDVIRTUAL0(_on_turn_ended)
    GDVIRTUAL3(_on_player_parameter_changed,int,Variant,Variant)
    GDVIRTUAL1(_on_player_parameter_removed,int)

    GDVIRTUAL1RC(Dictionary,_on_serialize,Dictionary)
    GDVIRTUAL1RC(Dictionary,_on_deserialize,Dictionary)

    void request_sync();

    StringName rpc_set_player_parameter_stringname;
    StringName rpc_remove_player_parameter_stringname;
    StringName rpc_set_property_stringname;
    StringName rpc_request_sync_stringname;
    StringName rpc_grant_sync_stringname;

    // RPC methods for multiplayer parameter syncing
    void _rpc_set_player_parameter(int param, Variant v);
    void _rpc_remove_player_parameter(int param);
    void _rpc_set_property(const String& property_name, const Variant& value);
    void _rpc_request_sync();
    void _rpc_grant_sync(Dictionary dict);
                                            
    YGamePlayer() {
        is_remote=false;
        player_type = 0;
        player_controller = -1;
        player_id = -1;
        remote_player_id = -1;
        owns_current_turn = false;
        rpc_set_player_parameter_stringname = SNAME("_rpc_set_player_parameter");
        rpc_remove_player_parameter_stringname = SNAME("_rpc_remove_player_parameter");
        rpc_set_property_stringname = SNAME("_rpc_set_property");
        rpc_request_sync_stringname = SNAME("_rpc_request_sync");
        rpc_grant_sync_stringname = SNAME("_rpc_grant_sync");
    }
    ~YGamePlayer() {
    }
};

#endif //YGAMEPLAYER_H
