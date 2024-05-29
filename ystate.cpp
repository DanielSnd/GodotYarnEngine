//
// Created by Daniel on 2024-03-13.
//

#include "ystate.h"

#include "yarnengine.h"

void YState::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_node_owner"), &YState::get_node_owner);

    ClassDB::bind_method(D_METHOD("get_pass_condition_state"), &YState::get_pass_condition_state);
    ClassDB::bind_method(D_METHOD("set_pass_condition_state","pass_condition_state"), &YState::set_pass_condition_state);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "pass_condition_state", PROPERTY_HINT_NODE_TYPE, "YState"), "set_pass_condition_state", "get_pass_condition_state");

    ClassDB::bind_method(D_METHOD("get_fail_condition_state"), &YState::get_fail_condition_state);
    ClassDB::bind_method(D_METHOD("set_fail_condition_state","fail_condition_state"), &YState::set_fail_condition_state);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "fail_condition_state",  PROPERTY_HINT_NODE_TYPE, "YState"), "set_fail_condition_state", "get_fail_condition_state");

    ClassDB::bind_method(D_METHOD("get_state_machine"), &YState::get_fsm_machine);
    ClassDB::bind_method(D_METHOD("set_state_machine","new_state_machine"), &YState::set_fsm_machine);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "state_machine",  PROPERTY_HINT_NODE_TYPE, "YState",PROPERTY_USAGE_NO_EDITOR), "set_state_machine", "get_state_machine");

    ClassDB::bind_method(D_METHOD("state_machine_setup","fsm"), &YState::state_machine_setup);
    ClassDB::bind_method(D_METHOD("enter_state"), &YState::enter_state);
    ClassDB::bind_method(D_METHOD("exit_state"), &YState::exit_state);
    ClassDB::bind_method(D_METHOD("process_state"), &YState::process_state);

    ClassDB::bind_method(D_METHOD("finished_navigation"), &YState::finished_navigation);
    ClassDB::bind_method(D_METHOD("get_speed"), &YState::get_speed);
    ClassDB::bind_method(D_METHOD("can_move"), &YState::can_move);
    ClassDB::bind_method(D_METHOD("override_animation"), &YState::override_animation);

    GDVIRTUAL_BIND(_on_state_machine_setup,"fsm")
    GDVIRTUAL_BIND(_on_ready,"fsm")
    GDVIRTUAL_BIND(_on_enter_state)
    GDVIRTUAL_BIND(_on_exit_state)
    GDVIRTUAL_BIND(_on_process_state,"delta")
    GDVIRTUAL_BIND(_on_finished_navigation)

    GDVIRTUAL_BIND(_get_speed)
    GDVIRTUAL_BIND(_can_move)
    GDVIRTUAL_BIND(_override_animation)
    GDVIRTUAL_BIND(_fail_condition)
    GDVIRTUAL_BIND(_pass_condition)

    // ADD_SIGNAL(MethodInfo("attempt_override"));
    // ADD_SIGNAL(MethodInfo("state_transitioned"));
}

bool YState::pass_condition() {
    bool ret = false;
    GDVIRTUAL_CALL(_pass_condition,ret);
    return ret;
}

bool YState::fail_condition() {
    bool ret = false;
    GDVIRTUAL_CALL(_fail_condition,ret);
    return ret;
}

Node * YState::get_node_owner() const {
    return fsm_machine != nullptr ? fsm_machine->fsm_owner : nullptr;
}

void YState::state_machine_setup(Ref<YStateMachine> fsm) {
    fsm_machine = fsm;
    GDVIRTUAL_CALL(_on_state_machine_setup,fsm);
}

void YState::enter_state() {
    //print_line("Called enter state on cpp");
    GDVIRTUAL_CALL(_on_enter_state);
}

void YState::exit_state() {
    GDVIRTUAL_CALL(_on_exit_state);
}

void YState::process_state(float _delta) {
    GDVIRTUAL_CALL(_on_process_state,_delta);
}

void YState::finished_navigation() {
    GDVIRTUAL_CALL(_on_finished_navigation);
}

float YState::get_speed() {
    float ret = -1.0f;
    GDVIRTUAL_CALL(_get_speed,ret);
    return ret;
}

bool YState::can_move() {
    bool ret = false;
    GDVIRTUAL_CALL(_can_move,ret);
    return ret;
}

bool YState::override_animation() {
    bool ret = false;
    GDVIRTUAL_CALL(_override_animation,ret);
    return ret;
}

void YState::ready() {
    GDVIRTUAL_CALL(_on_ready,fsm_machine);
}

void YStateMachine::_bind_methods() {
    ClassDB::bind_method(D_METHOD("start_state_machine","node_owner"), &YStateMachine::start_state_machine);
    ClassDB::bind_method(D_METHOD("transition","new_state"), &YStateMachine::transition);

    ClassDB::bind_method(D_METHOD("get_fsm_owner"), &YStateMachine::get_fsm_owner);

    ClassDB::bind_method(D_METHOD("get_current_state"), &YStateMachine::get_current_state);
    ClassDB::bind_method(D_METHOD("set_current_state","new_state"), &YStateMachine::set_current_state);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "current_state",  PROPERTY_HINT_NODE_TYPE, "YState",PROPERTY_USAGE_NO_EDITOR), "set_current_state", "get_current_state");

    //
    ClassDB::bind_method(D_METHOD("process","delta"), &YStateMachine::process);


    ClassDB::bind_method(D_METHOD("get_run_on_server_only"), &YStateMachine::get_run_on_server_only);
    ClassDB::bind_method(D_METHOD("set_run_on_server_only","run_on_server_only"), &YStateMachine::set_run_on_server_only);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "run_on_server_only"), "set_run_on_server_only", "get_run_on_server_only");


    GDVIRTUAL_BIND(_on_state_machine_started,"node_owner")
    // GDVIRTUAL_BIND(_end_state_machine)
    GDVIRTUAL_BIND(_on_process,"delta")
    GDVIRTUAL_BIND(_on_transition,"new_state")
    // GDVIRTUAL_BIND(_finished_navigation)
    //
    ADD_SIGNAL(MethodInfo("attempt_override"));
    ADD_SIGNAL(MethodInfo("state_transitioned"));
}

void YStateMachine::start_state_machine(Node *p_owner) {
    if (run_on_server_only && !YEngine::get_singleton()->get_multiplayer()->is_server())
        return;
    //print_line("Start state machine called");
    if (p_owner != nullptr) {
        fsm_owner = p_owner;

        TypedArray<Node> children = p_owner->get_children();
        Vector<YState*> found_states;
        for (int i = 0; i < children.size(); i++) {
            auto* child = Object::cast_to<YState>(children[i].operator Object*());
            if (child != nullptr) {
                child->state_machine_setup(this);
                found_states.append(child);
                if (current_state == nullptr) {
                    current_state = child;
                }
            }
            else {
                auto* overrider_child = Object::cast_to<YStateOverride>(children[i].operator Object*());
                if (overrider_child != nullptr) {
                    overrider_child->state_machine_setup(this);
                    has_overriders = true;
                }
            }
        }
        for (int i = 0; i < found_states.size(); i++) {
            found_states[i]->ready();
        }
        if (current_state != nullptr) {
            //print_line("Established current state isn't null ",current_state);
            //print_line("About to call enter_state on it.");
            current_state->enter_state();
        }
    }
    GDVIRTUAL_CALL(_on_state_machine_started,p_owner);
}

void YStateMachine::end_state_machine() {
  //  GDVIRTUAL_CALL(_end_state_machine);
}

void YStateMachine::process(float p_delta) {
    if (run_on_server_only && !YEngine::get_singleton()->get_multiplayer()->is_server())
        return;
    _counting++;
    if (override_with_state != nullptr && override_with_state != current_state) {
        transition(override_with_state);
        override_with_state = nullptr;
        block_other_overrides = false;
        _counting = 0;
        //print_line("Did an override transition, currnet override with state? ",override_with_state," block others? ",block_other_overrides," counting? ",_counting);
    }
    if (_counting >= attempt_transition_interval) {
        _counting = 0;
        block_other_overrides = false;
        if (has_overriders) {
            emit_signal("attempt_override");
        }
    }

    if (current_state != nullptr) {
        bool changed_state = false;

        if (current_state->pass_condition() && current_state->pass_condition_state != nullptr) {
            transition(current_state->pass_condition_state);
            changed_state = true;
        }
        else if (current_state->fail_condition() && current_state->fail_condition_state != nullptr) {
            transition(current_state->fail_condition_state);
            changed_state = true;
        }
        if (!changed_state) {
            current_state->process_state(p_delta);
        }
    }

    GDVIRTUAL_CALL(_on_process,p_delta);
}

void YStateMachine::transition(YState *p_state) {
    if (run_on_server_only && !YEngine::get_singleton()->get_multiplayer()->is_server())
        return;
    if (current_state != nullptr) {
        current_state->exit_state();
    }

    current_state = p_state;
    if (current_state != nullptr) {
        current_state->enter_state();
    }
    emit_signal("state_transitioned");
    GDVIRTUAL_CALL(_on_transition,p_state);
}

void YStateMachine::finished_navigation() {
    // if (current_state != nullptr) {
    //     current_state->_finished_navigation();
    // }
   // GDVIRTUAL_CALL(_finished_navigation);
}
