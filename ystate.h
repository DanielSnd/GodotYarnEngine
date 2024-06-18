//
// Created by Daniel on 2024-03-13.
//

#ifndef YSTATE_H
#define YSTATE_H
#include "scene/main/node.h"
#include "scene/main/multiplayer_api.h"
#include "ystateoverride.h"
#include "scene/2d/node_2d.h"

class YStateOverride;
class YStateMachine;

class YState : public Node{
    GDCLASS(YState, Node);

protected:
    static void _bind_methods();
    void _validate_property(PropertyInfo &p_property) const;

public:
    enum CheckType {
        NONE = 0,
        TRUE = 1,
        FALSE = 2,
        EQUAL = 3,
        NOT_EQUAL = 4,
        BIGGER_THAN = 5,
        SMALLER_THAN = 6,
        BIGGER_THAN_OR_EQUAL = 7,
        SMALLER_THAN_OR_EQUAL = 8
    };

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

    int last_started_count;
    int last_overrided_with_count;
    
    bool auto_override;
    void set_auto_override(bool val);

    PackedStringArray get_configuration_warnings() const;

    bool has_valid_auto_override();

    bool can_auto_override() const;

    void execute_auto_override();

    bool get_auto_override() const {return auto_override;}

    float autooverride_cooldown;
    void set_autooverride_cooldown(const float val) { autooverride_cooldown = val;}
    float get_autooverride_cooldown() const {return autooverride_cooldown;}

    int autooverride_weight;
    void set_autooverride_weight(const int val) { autooverride_weight = val;}
    int get_autooverride_weight() const {return autooverride_weight;}

    bool autooverride_prevent_repeat;
    void set_autooverride_prevent_repeat(const bool val) { autooverride_prevent_repeat = val;}
    bool get_autooverride_prevent_repeat() const {return autooverride_prevent_repeat;}

    TypedArray<NodePath> autooverride_only_if_states;
    void set_autooverride_only_if_states(const TypedArray<NodePath> &val) {autooverride_only_if_states = val;}
    TypedArray<NodePath> get_autooverride_only_if_states() const {return autooverride_only_if_states;}

    TypedArray<NodePath> autooverride_ignore_if_states;
    void set_autooverride_ignore_if_states(const TypedArray<NodePath> &val) {autooverride_ignore_if_states = val;}
    TypedArray<NodePath> get_autooverride_ignore_if_states() const {return autooverride_ignore_if_states;}

    CheckType autooverride_check_type;
    void set_autooverride_check_type(CheckType val);
    CheckType get_autooverride_check_type() const {return autooverride_check_type;}

    Node* autooverride_check_node;
    void set_autooverride_check_node(Node* val) {
        autooverride_check_node = val;
    update_configuration_warnings();
    }
    Node* get_autooverride_check_node() const {return autooverride_check_node;}

    String autooverride_check_property;
    void set_autooverride_check_property(const String &val) { autooverride_check_property = val;
    update_configuration_warnings();}
    String get_autooverride_check_property() const {return autooverride_check_property;}

    float autooverride_check_value;
    void set_autooverride_check_value(const float val) { autooverride_check_value = val;}
    float get_autooverride_check_value() const {return autooverride_check_value;}
    Vector<ObjectID> ignore_if_objectids;
    Vector<ObjectID> only_if_objectids;
    float last_time_state_started;
    void attempt_override();
    bool wants_to_override = false;
    /*@export var parameter_name:String = ""
@export var check_type:CheckTypes = CheckTypes.BiggerThan
@export var check_value:float = 0.0
@export var cooldown:float = 0.0
@export var weight:int = 5
@export var prevent_repeat:bool = false
var last_time_used:float = 0.0
var last_used_in_count:int = 0*/
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

    YState(): last_started_count(0), last_overrided_with_count(0), autooverride_cooldown(0), autooverride_weight(1),
              autooverride_prevent_repeat(false),
              autooverride_check_type(),
              autooverride_check_node(nullptr),
              autooverride_check_value(0), last_time_state_started(0) {
        auto_override = false;
        pass_condition_state = nullptr;
        fail_condition_state = nullptr;
    }

    ~YState() {
        pass_condition_state = nullptr;
        fail_condition_state = nullptr;
        autooverride_check_node = nullptr;
        if (fsm_machine.is_valid())
            fsm_machine.unref();
    }
};

VARIANT_ENUM_CAST(YState::CheckType)

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
    int transitions_count;
    int overrides_count;
    int attempt_transition_interval;
    int get_overrider_check_interval() const { return attempt_transition_interval; }
    void set_overrider_check_interval(int v) { attempt_transition_interval = v; }
    int _counting;

    bool has_overriders;
    bool block_other_overrides;

    bool run_on_server_only;
    bool get_run_on_server_only() const { return run_on_server_only; }
    void set_run_on_server_only(bool v) { run_on_server_only = v; }

    LocalVector<YState*> auto_overridable_states;
    YState* get_current_state() const {return current_state;}
    void set_current_state(YState* val)  { transition(val); }
    Node* get_fsm_owner() const {return fsm_owner;}

    Node* state_target;
    Node3D* state_target_3d;
    Node2D* state_target_2d;

    Node3D* get_state_target_3d() const { return state_target_3d; }
    Node2D* get_state_target_2d() const { return state_target_2d; }

    float get_state_target_distance();
    Node* get_state_target();
    void clear_state_target();
    void set_state_target(Node* new_target);


    YStateMachine(): state_target(nullptr), state_target_3d(nullptr), state_target_2d(nullptr) {
        transitions_count = 0;
        overrides_count = 0;
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
        state_target = nullptr;
        state_target_3d = nullptr;
        state_target_2d = nullptr;
        current_state = nullptr;
        fsm_owner = nullptr;
        override_with_state = nullptr;
    }
};

#endif //YSTATE_H
