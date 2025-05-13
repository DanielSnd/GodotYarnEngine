//
// Created by Daniel on 2024-03-13.
//

#include "ystateoverride.h"

void YStateOverride::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_ignore_if_state"), &YStateOverride::get_ignore_if_state);
    ClassDB::bind_method(D_METHOD("set_ignore_if_state","ignore_if_state"), &YStateOverride::set_ignore_if_state);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "ignore_if_state", PROPERTY_HINT_NODE_TYPE, "YState"), "set_ignore_if_state", "get_ignore_if_state");

    ClassDB::bind_method(D_METHOD("get_override_with_state"), &YStateOverride::get_override_with_state);
    ClassDB::bind_method(D_METHOD("set_override_with_state","override_with_state"), &YStateOverride::set_override_with_state);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "override_with_state", PROPERTY_HINT_NODE_TYPE, "YState"), "set_override_with_state", "get_override_with_state");

    GDVIRTUAL_BIND(_on_state_machine_setup,"fsm")
    GDVIRTUAL_BIND(_override_condition)
}


void YStateOverride::state_machine_setup(const Ref<YStateMachine> &p_owner) {
    fsm_owner = p_owner;
    if (p_owner.is_valid()) {
        p_owner->connect(SNAME("attempt_override"),callable_mp(this,&YStateOverride::attempt_override));
        GDVIRTUAL_CALL(_on_state_machine_setup,p_owner);
        p_owner->has_overriders = true;
    }
}

bool YStateOverride::override_condition() const {
    bool ret = false;
    GDVIRTUAL_CALL(_override_condition,ret);
    return ret;
}

void YStateOverride::attempt_override()  {
    if (!fsm_owner.is_valid()) return;
    //print_line("Attempt override called, not blocking others? ",!fsm_owner->block_other_overrides," my override not null? ",override_with_state != nullptr ," their override with state null? ",fsm_owner->override_with_state == nullptr);
    if (!fsm_owner->block_other_overrides && override_with_state != nullptr) {
        //print_line("Getting here? ignore_if_state == nullptr ",ignore_if_state == nullptr, " not current ignoring ",fsm_owner->current_state != ignore_if_state);
        if ((ignore_if_state == nullptr || fsm_owner->current_state != ignore_if_state) && override_condition()) {
            if (fsm_owner->current_state != override_with_state) {
                //print_line("Set override with state");
                fsm_owner->override_with_state = override_with_state;
            }
            fsm_owner->block_other_overrides = true;
        }
    }
}
