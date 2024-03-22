//
// Created by Daniel on 2024-03-19.
//

#ifndef YGAMEPLAYER_H
#define YGAMEPLAYER_H
#include "scene/main/node.h"

class YGamePlayer : public Node {
    GDCLASS(YGamePlayer, Node);

protected:
    static void _bind_methods();

public:
    int player_id;
    void set_player_id(int v) {}
    int get_player_id() const {return player_id;}
    int player_type;
    void set_player_type(int v) {player_type = v;}
    int get_player_type() const {return player_type;}
    int player_controller;
    void set_player_controller(int v) {player_controller = v;}
    int get_player_controller() const {return player_controller;}
    bool is_remote;
    void set_is_remote(bool v) {is_remote = v;}
    bool get_is_remote() const {return is_remote;}

    bool owns_current_turn;
    void set_owns_current_turn(bool v) {owns_current_turn = v;}
    bool get_owns_current_turn() const {return owns_current_turn;}

    HashMap<int,Variant> player_parameters;
    void set_player_parameter(int param, Variant v) {player_parameters[param] = v;}
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

    Dictionary serialize(Dictionary dict);

    Dictionary deserialize(Dictionary dict);

    GDVIRTUAL0(_on_player_registered)
    GDVIRTUAL0(_on_turn_started)
    GDVIRTUAL0(_on_turn_ended)
    GDVIRTUAL1RC(Dictionary,_on_serialize,Dictionary)
    GDVIRTUAL1RC(Dictionary,_on_deserialize,Dictionary)

    YGamePlayer() {
        is_remote=false;
        player_type = 0;
        player_controller = -1;
    }
    ~YGamePlayer() {
    }
};

#endif //YGAMEPLAYER_H
