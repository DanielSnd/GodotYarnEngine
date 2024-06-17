//
// Created by Daniel on 2024-03-13.
//

#ifndef YSTATE_H
#define YSTATE_H
#include "scene/main/node.h"
#include "scene/main/multiplayer_api.h"
#include "ystateoverride.h"

class YStateOverride;
class YStateMachine;

class YState : public Node{
    GDCLASS(YState, Node);

protected:
    static void _bind_methods();

public:
    YState* pass_condition_state;
    YState* fail_condition_state;
    YState* get_pass_condition_state() const { return pass_condition_state;};
    YState* get_fail_condition_state() const { return fail_condition_state;};
    void set_pass_condition_state(YState* val) {pass_condition_state = val;}
    void set_fail_condition_state(YState* val) {fail_condition_state = val;}

    Ref<YStateMachine> fsm_machine;
    Ref<YStateMachine> get_fsm_machine() const {return fsm_machine;}
    void set_fsm_machine(const Ref<YStateMachine> &val) {fsm_machine = val;}
    Node* get_node_owner() const;
    //
    virtual bool pass_condition();
    virtual bool fail_condition();

    virtual void state_machine_setup(Ref<YStateMachine> owner);
    virtual void enter_state();
    virtual void exit_state();
    virtual void process_state(float _delta);
    virtual void finished_navigation();
    virtual float get_speed();
    virtual bool can_move();
    virtual bool override_animation();

    void ready();

    GDVIRTUAL0(_on_enter_state)
    GDVIRTUAL0(_on_exit_state)
    GDVIRTUAL0(_on_finished_navigation)
    GDVIRTUAL1(_on_state_machine_setup,Ref<YStateMachine>)
    GDVIRTUAL1(_on_ready,Ref<YStateMachine>)
    GDVIRTUAL1(_on_process_state,float)
    GDVIRTUAL0RC(float,_get_speed)
    GDVIRTUAL0RC(bool,_can_move)
    GDVIRTUAL0RC(bool,_override_animation)
    GDVIRTUAL0RC(bool,_fail_condition)
    GDVIRTUAL0RC(bool,_pass_condition)

    YState() {
        pass_condition_state = nullptr;
        fail_condition_state = nullptr;
    }
    ~YState() {
        pass_condition_state = nullptr;
        fail_condition_state = nullptr;
        if (fsm_machine.is_valid())
            fsm_machine.unref();
    }
};

class YStateMachine : public RefCounted{
    GDCLASS(YStateMachine, RefCounted);

protected:
    static void _bind_methods();

    virtual void start_state_machine(Node* p_owner);
    virtual void end_state_machine();
    virtual void process(float p_delta);
    virtual void transition(YState* p_state);
    virtual void finished_navigation();

    // GDVIRTUAL0(_end_state_machine)
    // GDVIRTUAL0(_finished_navigation)
    GDVIRTUAL1(_on_state_machine_started,Node*)
    GDVIRTUAL1(_on_process,float)
    GDVIRTUAL1(_on_transition,YState*)

public:
    YState* current_state;
    Node* fsm_owner;
    YState* override_with_state;
    int attempt_transition_interval;
    int get_overrider_check_interval() const { return attempt_transition_interval; }
    void set_overrider_check_interval(int v) { attempt_transition_interval = v; }
    int _counting;

    bool has_overriders;
    bool block_other_overrides;

    bool run_on_server_only;
    bool get_run_on_server_only() const { return run_on_server_only; }
    void set_run_on_server_only(bool v) { run_on_server_only = v; }

    YState* get_current_state() const {return current_state;}
    void set_current_state(YState* val)  { transition(val); }
    Node* get_fsm_owner() const {return fsm_owner;}

    YStateMachine() {
        has_overriders = false;
        block_other_overrides = false;
        run_on_server_only = false;
        current_state = nullptr;
        fsm_owner = nullptr;
        override_with_state = nullptr;
        attempt_transition_interval = 2;
        _counting = 0;
    }
    ~YStateMachine() {
        current_state = nullptr;
        fsm_owner = nullptr;
        override_with_state = nullptr;
    }
};

#endif //YSTATE_H
