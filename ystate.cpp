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

    ClassDB::bind_method(D_METHOD("get_last_transitioned_from_or_null"), &YState::get_last_transitioned_from_or_null);



    ADD_SUBGROUP("Auto Override","");

    ClassDB::bind_method(D_METHOD("get_auto_override"), &YState::get_auto_override);
    ClassDB::bind_method(D_METHOD("set_auto_override","auto_override"), &YState::set_auto_override);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_override"), "set_auto_override", "get_auto_override");

    ClassDB::bind_method(D_METHOD("get_autooverride_cooldown"), &YState::get_autooverride_cooldown);
    ClassDB::bind_method(D_METHOD("set_autooverride_cooldown", "autooverride_cooldown"), &YState::set_autooverride_cooldown);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cooldown"), "set_autooverride_cooldown", "get_autooverride_cooldown");

    ClassDB::bind_method(D_METHOD("get_autooverride_weight"), &YState::get_autooverride_weight);
    ClassDB::bind_method(D_METHOD("set_autooverride_weight", "autooverride_weight"), &YState::set_autooverride_weight, DEFVAL(1));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "weight"), "set_autooverride_weight", "get_autooverride_weight");

    ClassDB::bind_method(D_METHOD("get_autooverride_prevent_repeat"), &YState::get_autooverride_prevent_repeat);
    ClassDB::bind_method(D_METHOD("set_autooverride_prevent_repeat", "autooverride_prevent_repeat"), &YState::set_autooverride_prevent_repeat);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "prevent_repeat"), "set_autooverride_prevent_repeat", "get_autooverride_prevent_repeat");

    ClassDB::bind_method(D_METHOD("get_autooverride_only_if_states"), &YState::get_autooverride_only_if_states);
    ClassDB::bind_method(D_METHOD("set_autooverride_only_if_states","ignore_if_states"), &YState::set_autooverride_only_if_states);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "only_if_states",  PROPERTY_HINT_ARRAY_TYPE, "NodePath"), "set_autooverride_only_if_states", "get_autooverride_only_if_states");

    ClassDB::bind_method(D_METHOD("get_autooverride_ignore_if_states"), &YState::get_autooverride_ignore_if_states);
    ClassDB::bind_method(D_METHOD("set_autooverride_ignore_if_states","ignore_if_states"), &YState::set_autooverride_ignore_if_states);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "ignore_if_states",  PROPERTY_HINT_ARRAY_TYPE, "NodePath"), "set_autooverride_ignore_if_states", "get_autooverride_ignore_if_states");

    ClassDB::bind_method(D_METHOD("get_autooverride_check_type"), &YState::get_autooverride_check_type);
    ClassDB::bind_method(D_METHOD("set_autooverride_check_type", "autooverride_check_type"), &YState::set_autooverride_check_type);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "check_type", PROPERTY_HINT_ENUM, "None,True,False,Equal,NotEqual,BiggerThan,SmallerThan,BiggerThanOrEqual,SmallerThanOrEqual"), "set_autooverride_check_type", "get_autooverride_check_type");

    ClassDB::bind_method(D_METHOD("get_autooverride_check_node"), &YState::get_autooverride_check_node);
    ClassDB::bind_method(D_METHOD("set_autooverride_check_node", "autooverride_check_node"), &YState::set_autooverride_check_node);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "check_node",  PROPERTY_HINT_NODE_TYPE, "Node"), "set_autooverride_check_node", "get_autooverride_check_node");

    ClassDB::bind_method(D_METHOD("get_autooverride_check_property"), &YState::get_autooverride_check_property);
    ClassDB::bind_method(D_METHOD("set_autooverride_check_property", "autooverride_check_property"), &YState::set_autooverride_check_property);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "check_property"), "set_autooverride_check_property", "get_autooverride_check_property");

    ClassDB::bind_method(D_METHOD("get_autooverride_check_value"), &YState::get_autooverride_check_value);
    ClassDB::bind_method(D_METHOD("set_autooverride_check_value", "autooverride_check_value"), &YState::set_autooverride_check_value);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "check_value"), "set_autooverride_check_value", "get_autooverride_check_value");

    ClassDB::bind_method(D_METHOD("get_print_debugs"), &YState::get_print_debugs);
    ClassDB::bind_method(D_METHOD("set_print_debugs","print_debugs"), &YState::set_print_debugs);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "print_debugs"), "set_print_debugs", "get_print_debugs");

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

    BIND_ENUM_CONSTANT(NONE);
    BIND_ENUM_CONSTANT(TRUE);
    BIND_ENUM_CONSTANT(FALSE);
    BIND_ENUM_CONSTANT(EQUAL);
    BIND_ENUM_CONSTANT(NOT_EQUAL);
    BIND_ENUM_CONSTANT(BIGGER_THAN);
    BIND_ENUM_CONSTANT(SMALLER_THAN);
    BIND_ENUM_CONSTANT(BIGGER_THAN_OR_EQUAL);
    BIND_ENUM_CONSTANT(SMALLER_THAN_OR_EQUAL);
    // ADD_SIGNAL(MethodInfo("attempt_override"));
    // ADD_SIGNAL(MethodInfo("state_transitioned"));
}


void YState::_validate_property(PropertyInfo &p_property) const {
    if (auto_override) {
        if (autooverride_check_type == YState::CheckType::NONE && p_property.name != "check_type" &&
            p_property.name.begins_with("check_")) {
            p_property.usage = PROPERTY_USAGE_NO_EDITOR;
        }
        if ((autooverride_check_type == CheckType::FALSE || autooverride_check_type == CheckType::TRUE) && p_property.name == "check_value") {
            p_property.usage = PROPERTY_USAGE_NO_EDITOR;
        }
    } else {
        if (p_property.name.begins_with("autooverride") || p_property.name.begins_with("check_") || p_property.name.ends_with("_if_states") ||
            p_property.name == "weight" || p_property.name == "cooldown" || p_property.name == "prevent_repeat") {
            p_property.usage = PROPERTY_USAGE_NO_EDITOR;
        }
    }
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
    last_time_state_started = YTime::get_singleton()->time;
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

void YState::set_auto_override(bool val) {
    if (val == auto_override) {
        return;
    }
    auto_override = val;
    notify_property_list_changed();
    update_configuration_warnings();
}

Node * YState::get_last_transitioned_from_or_null() const {
    if (last_transitioned_from.is_empty()) {
        return nullptr;
    }
    return get_node_or_null(last_transitioned_from);
}

PackedStringArray YState::get_configuration_warnings() const {
    PackedStringArray warnings = Node::get_configuration_warnings();

    if (auto_override && autooverride_check_type != CheckType::NONE) {
        if (autooverride_check_node == nullptr) {
            warnings.push_back(RTR("This YState has an auto override with a check type but no node to check the value of."));
        } else if(autooverride_check_property.is_empty()) {
            warnings.push_back(RTR("This YState has an auto override with a check type and a node but no written property to check."));
        }else {
            bool p_has_property = false;
            List<PropertyInfo> pinfo;
            autooverride_check_node->get_property_list(&pinfo);
            for (const PropertyInfo &pi : pinfo) {
                if (pi.name == autooverride_check_property) {
                    if ((autooverride_check_type == CheckType::FALSE || autooverride_check_type == CheckType::TRUE)) {
                        if (pi.type != Variant::BOOL) {
                            warnings.push_back(RTR("This YState has an auto override with a check type, a node and a property but the property is not a boolean."));
                        } else {
                            p_has_property = true;
                        }
                    } else {
                        if (pi.type == Variant::INT || pi.type == Variant::FLOAT) {
                            p_has_property = true;
                        } else {
                            warnings.push_back(RTR("This YState has an auto override with a check type, a node and a property but the property is not a number."));
                        }
                    }
                    break;
                }
            }
            if (!p_has_property) {
#ifdef TOOLS_ENABLED
                if (Engine::get_singleton()->is_editor_hint()) {
                    if (ScriptInstance* _script_instance = (autooverride_check_node->get_script_instance()); _script_instance != nullptr) {
                        auto script_found = _script_instance->get_script();
                        if (script_found.is_valid()) {

                            List<PropertyInfo> ppinfo;
                            script_found->get_script_property_list(&ppinfo);
                            for (const PropertyInfo &pi : ppinfo) {
                                if (pi.name == autooverride_check_property) {
                                    if ((autooverride_check_type == CheckType::FALSE || autooverride_check_type == CheckType::TRUE)) {
                                        if (pi.type != Variant::BOOL) {
                                            warnings.push_back(RTR("This YState has an auto override with a check type, a node and a property but the property is not a boolean."));
                                        } else {
                                            p_has_property = true;
                                        }
                                    } else {
                                        if (pi.type == Variant::INT || pi.type == Variant::FLOAT) {
                                            p_has_property = true;
                                        } else {
                                            warnings.push_back(RTR("This YState has an auto override with a check type, a node and a property but the property is not a number."));
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
#endif
                if(!p_has_property)
                    warnings.push_back(RTR("This YState has an auto override with a check type, a node and a property but the property could not be found on the node."));
            }
        }
    }

    return warnings;
}

bool YState::has_valid_auto_override() {
    if (print_debugs)
        print_line("Checking if has valid auto override, is check node null? ",autooverride_check_node == nullptr);
    if (autooverride_check_type == CheckType::NONE)
        return true;
    if (autooverride_check_node == nullptr)
        return false;
    if (autooverride_check_property.is_empty())
        return false;
    bool p_has_property = false;
    List<PropertyInfo> pinfo;
    autooverride_check_node->get_property_list(&pinfo);
    for (const PropertyInfo &pi : pinfo) {
        if (pi.name == autooverride_check_property) {
            if ((autooverride_check_type == CheckType::FALSE || autooverride_check_type == CheckType::TRUE)) {
                if (pi.type != Variant::BOOL) {
                    ERR_PRINT(vformat("[YState] Property %s was not a bool. (found in node %s when checking for auto override in %s)", autooverride_check_property, autooverride_check_node->get_name(), get_name()));
                    return false;
                } else {
                    return true;
                }
            }
            if (pi.type == Variant::INT || pi.type == Variant::FLOAT) {
                p_has_property = true;
            } else {
                ERR_PRINT(vformat("[YState] Property %s was not a number. (found in node %s when checking for auto override in %s)", autooverride_check_property, autooverride_check_node->get_name(), get_name()));
                return false;
            }
            break;
        }
    }
    if (!p_has_property) {
        ERR_PRINT(vformat("[YState] Property %s not found in node %s when checking for auto override in %s", autooverride_check_property, autooverride_check_node->get_name(),get_name()));
        return false;
    }
    return true;
}

bool YState::can_auto_override() const {
    if (!auto_override)
        return false;
    const bool can_based_on_ignore_and_only_if = ((ignore_if_objectids.is_empty() || !ignore_if_objectids.has(fsm_machine->current_state->get_instance_id()))
        && (only_if_objectids.is_empty() || (only_if_objectids.has(fsm_machine->current_state->get_instance_id()))));

    if (print_debugs)
        print_line("can auto override based on ignore and only if? ",can_based_on_ignore_and_only_if);

    if (!can_based_on_ignore_and_only_if) return false;

    if (autooverride_prevent_repeat && fsm_machine.is_valid() && (fsm_machine->overrides_count == last_overrided_with_count + 1 || fsm_machine->transitions_count == last_started_count + 1)) {
        return false;
    }
    if (autooverride_check_type == CheckType::NONE)
        return true;
    if (autooverride_check_node != nullptr) {
        if (autooverride_cooldown > 0.0001 && !YTime::get_singleton()->has_time_elapsed(last_time_state_started,autooverride_cooldown)) {
            return false;
        }
        if (autooverride_check_type == CheckType::FALSE || autooverride_check_type == CheckType::TRUE) {
            const bool current_bool_value = static_cast<bool>(autooverride_check_node->get(autooverride_check_property));
            if ((autooverride_check_type == CheckType::FALSE && !current_bool_value) || (autooverride_check_type == CheckType::TRUE && current_bool_value)) {
                return true;
            }
            return false;
        }
        const float current_value = static_cast<float>(autooverride_check_node->get(autooverride_check_property));
        bool found_result = false;
        switch (autooverride_check_type) {
            case NONE:
                break;
            case EQUAL: {
                const float epsilon = 0.00001f;
                found_result = abs(current_value- autooverride_check_value) < epsilon;
            }break;
            case NOT_EQUAL: {
                const float epsilon = 0.00001f;
                found_result = abs(current_value- autooverride_check_value) > epsilon;
            }break;
            case BIGGER_THAN:
                found_result = current_value > autooverride_check_value;
                break;
            case SMALLER_THAN:
                found_result = current_value < autooverride_check_value;
                break;
            case BIGGER_THAN_OR_EQUAL: {
                const float epsilon = 0.00001f;
                found_result = current_value > autooverride_check_value || (abs(current_value- autooverride_check_value) < epsilon);
            }break;
            case SMALLER_THAN_OR_EQUAL:{
                const float epsilon = 0.00001f;
                found_result = current_value < autooverride_check_value || (abs(current_value- autooverride_check_value) < epsilon);
            }break;
            default: ;
        }

        if (print_debugs)
            print_line("Checking can transition, found result ",found_result," current value ",current_value);
        return found_result;
    }
    return false;
}

void YState::execute_auto_override() {
    if (auto_override) {
        if (!fsm_machine.is_valid()) return;

        if (print_debugs)
            print_line("Attempt override called from",this,", not blocking others? ",!fsm_machine->block_other_overrides, " their override with state null? ",fsm_machine->override_with_state == nullptr);
        if (!fsm_machine->block_other_overrides && fsm_machine != nullptr) {
            if (fsm_machine->current_state != this) {
                if (print_debugs)
                    print_line("Set override with state ",this);
                fsm_machine->override_with_state = this;
            }
            fsm_machine->block_other_overrides = true;
        }
    }
}

void YState::set_autooverride_check_type(const CheckType val) {
    autooverride_check_type = val;
    notify_property_list_changed();
    update_configuration_warnings();
}

void YState::attempt_override() {
    if (auto_override) {
        if (!fsm_machine.is_valid()) return;
        if (print_debugs)
            print_line("Attempt override called, not blocking others? ",!fsm_machine->block_other_overrides," my override not null? ", this, " their override with state null? ",fsm_machine->override_with_state == nullptr);
        if (!fsm_machine->block_other_overrides && fsm_machine != nullptr) {
            // if (print_debugs)
            //     print_line("Getting here? ignore_if_state == nullptr ", ignore_if_state == nullptr, " not current ignoring ",fsm_owner->current_state != ignore_if_state);
            if ((ignore_if_objectids.is_empty() || !ignore_if_objectids.has(fsm_machine->current_state->get_instance_id()))
                && (only_if_objectids.is_empty() || (only_if_objectids.has(fsm_machine->current_state->get_instance_id())))
                && can_auto_override()) {
                if (fsm_machine->current_state != this) {
                    //print_line("Set override with state");
                    fsm_machine->override_with_state = this;
                }
                fsm_machine->block_other_overrides = true;
            }
        }
    }
}

void YState::ready() {
    GDVIRTUAL_CALL(_on_ready,fsm_machine);
    if (auto_override) {
        for (NodePath ignoreif: autooverride_ignore_if_states) {
            auto ignoreystate = Object::cast_to<YState>(get_node(ignoreif));
            // print_line(vformat("ignore if y state path %s is node null? %s", ignoreif, ignoreystate == nullptr));
            if (ignoreystate != nullptr) {
                ignore_if_objectids.push_back(ignoreystate->get_instance_id());
            }
        }
        for (NodePath onlyif: autooverride_only_if_states) {
            auto onlyystate = Object::cast_to<YState>(get_node(onlyif));
            // print_line(vformat("only if y state path %s is node null? %s", onlyif, onlyystate == nullptr));
            if (onlyystate != nullptr) {
                only_if_objectids.push_back(onlyystate->get_instance_id());
            }
        }
    }
}

void YStateMachine::_bind_methods() {
    ClassDB::bind_method(D_METHOD("start_state_machine","node_owner"), &YStateMachine::start_state_machine);
    ClassDB::bind_method(D_METHOD("transition","new_state"), &YStateMachine::transition);

    ClassDB::bind_method(D_METHOD("get_fsm_owner"), &YStateMachine::get_fsm_owner);

    ClassDB::bind_method(D_METHOD("get_current_state"), &YStateMachine::get_current_state);
    ClassDB::bind_method(D_METHOD("set_current_state","new_state"), &YStateMachine::set_current_state);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "current_state",  PROPERTY_HINT_NODE_TYPE, "YState",PROPERTY_USAGE_NO_EDITOR), "set_current_state", "get_current_state");

    ClassDB::bind_method(D_METHOD("finished_navigation"), &YStateMachine::finished_navigation);
    ClassDB::bind_method(D_METHOD("process","delta"), &YStateMachine::process);

    ClassDB::bind_method(D_METHOD("get_print_debugs"), &YStateMachine::get_print_debugs);
    ClassDB::bind_method(D_METHOD("set_print_debugs","print_debugs"), &YStateMachine::set_print_debugs);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "print_debugs"), "set_print_debugs", "get_print_debugs");

    
    ClassDB::bind_method(D_METHOD("get_run_on_server_only"), &YStateMachine::get_run_on_server_only);
    ClassDB::bind_method(D_METHOD("set_run_on_server_only","run_on_server_only"), &YStateMachine::set_run_on_server_only);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "run_on_server_only"), "set_run_on_server_only", "get_run_on_server_only");

    ClassDB::bind_method(D_METHOD("get_overrider_check_interval"), &YStateMachine::get_overrider_check_interval);
    ClassDB::bind_method(D_METHOD("set_overrider_check_interval","overrider_check_interval"), &YStateMachine::set_overrider_check_interval);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "overrider_check_interval"), "set_overrider_check_interval", "get_overrider_check_interval");
    
    GDVIRTUAL_BIND(_on_state_machine_started,"node_owner")
    // GDVIRTUAL_BIND(_end_state_machine)
    GDVIRTUAL_BIND(_on_process,"delta")
    GDVIRTUAL_BIND(_on_transition,"new_state")
    GDVIRTUAL_BIND(_on_set_state_target,"new_state_target")
    GDVIRTUAL_BIND(_on_set_state_target_position,"new_state_target_position")


    ClassDB::bind_method(D_METHOD("set_state_target_position"), &YStateMachine::set_state_target_position);
    ClassDB::bind_method(D_METHOD("get_state_target_position"), &YStateMachine::get_state_target_position);

    ClassDB::bind_method(D_METHOD("get_state_target"), &YStateMachine::get_state_target);
    ClassDB::bind_method(D_METHOD("set_state_target","new_target"), &YStateMachine::set_state_target);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "state_target",  PROPERTY_HINT_NODE_TYPE, "Node",PROPERTY_USAGE_NO_EDITOR), "set_state_target", "get_state_target");

    ClassDB::bind_method(D_METHOD("get_state_target_distance"), &YStateMachine::get_state_target_distance);
    ClassDB::bind_method(D_METHOD("get_state_target_2d"), &YStateMachine::get_state_target_2d);
    ClassDB::bind_method(D_METHOD("get_state_target_3d"), &YStateMachine::get_state_target_3d);


    ADD_SIGNAL(MethodInfo("attempt_override"));
    ADD_SIGNAL(MethodInfo("state_transitioned"));
    ADD_SIGNAL(MethodInfo("state_target_changed"));
}

void YStateMachine::start_state_machine(Node *p_owner) {
    if (run_on_server_only && !YEngine::get_singleton()->get_multiplayer()->is_server())
        return;
    if (print_debugs)
        print_line("Start state machine called");
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
                if (child->auto_override && child->has_valid_auto_override()) {
                    auto_overridable_states.push_back(child);
                }
            }
            else {
                auto* overrider_child = Object::cast_to<YStateOverride>(children[i].operator Object*());
                if (overrider_child != nullptr) {
                    overrider_child->state_machine_setup(this);
                    if (print_debugs)
                        print_line("Found overrider child, has overriders?",has_overriders);
                }
            }
        }
        for (int i = 0; i < found_states.size(); i++) {
            found_states[i]->ready();
        }
        if (current_state != nullptr) {
            if (print_debugs)
                print_line("Established current state isn't null and will call enter state on it ",current_state);
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
        overrides_count++;
        override_with_state->last_overrided_with_count = overrides_count;
        override_with_state = nullptr;
        block_other_overrides = false;
        _counting = 0;
        if (print_debugs)
            print_line("Did an override transition, currnet override with state? ",override_with_state," block others? ",block_other_overrides," counting? ",_counting);
    }

    if (print_debugs)
        print_line(vformat("Counting %d attempt interval %d has overriders %s", _counting, attempt_transition_interval, has_overriders));

    if (_counting >= attempt_transition_interval) {
        _counting = 0;
        block_other_overrides = false;
        if (print_debugs)
            print_line("auto_overridable_states ",auto_overridable_states.size());
        if (!auto_overridable_states.is_empty()) {
            LocalVector<YState*> valid_auto_overrides;
            for (auto ystate: auto_overridable_states) {
                if (ystate != nullptr && ystate->auto_override && ystate->can_auto_override()) {
                    valid_auto_overrides.push_back(ystate);
                }
            }
            if (!valid_auto_overrides.is_empty()) {
                if (valid_auto_overrides.size() == 1) {
                    valid_auto_overrides[0]->execute_auto_override();
                } else {
                    // Calculate total weight
                    int totalWeight = 0;
                    for (int i = 0; i < static_cast<int>(valid_auto_overrides.size()); i++) {
                        if (valid_auto_overrides[i] == nullptr)
                            continue;
                        totalWeight += valid_auto_overrides[i]->autooverride_weight;
                    }
                    // Generate a random int from 0 to totalWeight
                    int randomValue = Math::random(0, totalWeight-1);
                    // Find the element that this random value falls into
                    int weightSum = 0;
                    for (int i = 0; i < static_cast<int>(valid_auto_overrides.size()); i++) {
                        if (valid_auto_overrides[i] == nullptr)
                            continue;
                        weightSum += valid_auto_overrides[i]->autooverride_weight;
                        if (randomValue < weightSum) {
                            valid_auto_overrides[i]->execute_auto_override();
                            break;
                        }
                    }
                }
            }
        }
        if (has_overriders && !block_other_overrides) {
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
        if (p_state != nullptr) {
            p_state->last_transitioned_from = p_state->get_path_to(current_state);
        }
        current_state->exit_state();
    }

    current_state = p_state;
    if (current_state != nullptr) {
        current_state->enter_state();
        current_state->last_started_count = transitions_count + 1;
    }
    transitions_count++;
    emit_signal("state_transitioned");
    GDVIRTUAL_CALL(_on_transition,p_state);
}

void YStateMachine::finished_navigation() {
    if (current_state != nullptr) {
        current_state->finished_navigation();
    }
}

float YStateMachine::get_state_target_distance() const {
    if (fsm_owner == nullptr || !fsm_owner->has_method("get_global_position"))
        return -1.0;

    if (state_target == nullptr) {
        return fsm_owner->call("get_global_position").call("distance_to", state_target_position_only);
    } else if (state_target_2d != nullptr) {
        return state_target_2d->get_global_position().distance_to(fsm_owner->call("get_global_position"));
    } else if (state_target_3d != nullptr) {
        return state_target_3d->get_global_position().distance_to(fsm_owner->call("get_global_position"));
    }
    return -1.0f;
}

Node * YStateMachine::get_state_target() const {
    return state_target;
}

void YStateMachine::clear_state_target() {
    if (state_target != nullptr) {
        state_target = nullptr;
        state_target_3d = nullptr;
        state_target_2d = nullptr;
        emit_signal("state_target_changed");
    }
}

void YStateMachine::set_state_target(Node *new_target) {
    if (new_target != state_target) {
        if (state_target != nullptr) {
            if (state_target->is_connected(SceneStringName(tree_exited), callable_mp(this, &YStateMachine::clear_state_target)))
                state_target->disconnect(SceneStringName(tree_exited), callable_mp(this, &YStateMachine::clear_state_target));
            clear_state_target();
        }
        state_target = new_target;
        GDVIRTUAL_CALL(_on_set_state_target,new_target);
        if (state_target != nullptr) {
            if (!state_target->is_connected(SceneStringName(tree_exited), callable_mp(this, &YStateMachine::clear_state_target)))
                state_target->connect(SceneStringName(tree_exited), callable_mp(this, &YStateMachine::clear_state_target), CONNECT_ONE_SHOT);

            state_target_3d = Object::cast_to<Node3D>(state_target);
            state_target_2d = Object::cast_to<Node2D>(state_target);
        }
        emit_signal("state_target_changed");
    }
}

void YStateMachine::set_state_target_position(const Variant &new_target) {
        state_target_position_only = new_target;
        GDVIRTUAL_CALL(_on_set_state_target_position,new_target);
        if (state_target != nullptr) {
            clear_state_target();
        }
}
