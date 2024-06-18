//
// Created by Daniel on 2024-03-13.
//

#ifndef YSTATEOVERRIDE_H
#define YSTATEOVERRIDE_H

#include "scene/main/node.h"
#include "ystate.h"

class YState;
class YStateMachine;

class YStateOverride : public Node{
    GDCLASS(YStateOverride, Node);
private:

protected:
    static void _bind_methods();

public:
    Ref<YStateMachine> fsm_owner;

    void state_machine_setup(const Ref<YStateMachine> &p_owner);
    bool override_condition() const;
    void attempt_override();
    bool wants_to_override = false;

    GDVIRTUAL0RC(bool,_override_condition)
    GDVIRTUAL1(_on_state_machine_setup,Ref<YStateMachine>)

    YState* override_with_state;
    YState* ignore_if_state;
    YState* get_ignore_if_state() const {return ignore_if_state;};
    YState* get_override_with_state() const {return override_with_state;};
    void set_ignore_if_state(YState* val) {ignore_if_state = val;}
    void set_override_with_state(YState* val) {override_with_state = val;}


    YStateOverride() {
        override_with_state = nullptr;
        ignore_if_state = nullptr;
    }

    ~YStateOverride() override {
        override_with_state = nullptr;
        ignore_if_state = nullptr;
        if (fsm_owner.is_valid()) fsm_owner.unref();
    }
};


#endif //YSTATEOVERRIDE_H
