#include "yeventdata.h"

HashMap<int, YEventData::RegEventCallback> YEventData::reg_event_callbacks;
HashMap<ObjectID, int> YEventData::count_node_callbacks;

void YEventData::_bind_methods() {
    ClassDB::bind_method(D_METHOD("is_event_type", "type"), &YEventData::is_event_type);
    ClassDB::bind_method(D_METHOD("set_event_type", "type"), &YEventData::set_event_type);
    ClassDB::bind_method(D_METHOD("get_event_type"), &YEventData::get_event_type);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "event_type"), "set_event_type", "get_event_type");
    
    ClassDB::bind_method(D_METHOD("get_data_as_dictionary"), &YEventData::get_data_as_dictionary);
    ClassDB::bind_method(D_METHOD("set_data_from_dictionary", "dict"), &YEventData::set_data_from_dictionary);
    ClassDB::bind_method(D_METHOD("add_data_from_dictionary", "dict"), &YEventData::add_data_from_dictionary);
    ClassDB::bind_method(D_METHOD("has_value", "identifier"), &YEventData::has_value);
    ClassDB::bind_method(D_METHOD("get_value", "identifier", "default_value"), &YEventData::get_value, DEFVAL(Variant()));
    ClassDB::bind_method(D_METHOD("set_value", "identifier", "value"), &YEventData::set_value);
    ClassDB::bind_method(D_METHOD("increment_value", "identifier", "amount"), &YEventData::increment_value);
    ClassDB::bind_method(D_METHOD("emit"), &YEventData::emit);
    ClassDB::bind_method(D_METHOD("emit_to_node", "node"), &YEventData::emit_to_node);
    ClassDB::bind_method(D_METHOD("duplicate"), &YEventData::duplicate);

    ClassDB::bind_static_method("YEventData", D_METHOD("register_listener", "event_id", "callable", "priority"), &YEventData::register_listener, DEFVAL(50));
    ClassDB::bind_static_method("YEventData", D_METHOD("unregister_listener", "event_id", "callable"), &YEventData::unregister_listener);
    ClassDB::bind_static_method("YEventData", D_METHOD("register_listener_with_node", "node", "event_id", "callable", "priority"), &YEventData::register_listener_with_node, DEFVAL(50));
    ClassDB::bind_static_method("YEventData", D_METHOD("unregister_listener_with_node", "node", "event_id", "callable"), &YEventData::unregister_listener_with_node);
    ClassDB::bind_static_method("YEventData", D_METHOD("create", "with_type", "parameter_one", "value_one"), &YEventData::create, DEFVAL(0), DEFVAL(Variant()));
}

void YEventData::_sort_event_callbacks(int p_event_id) {
    if (!reg_event_callbacks.has(p_event_id)) {
        return;
    }

    RegEventCallback &callback = reg_event_callbacks[p_event_id];
    callback.callbacks.sort_custom<YEventData::YEventDataPriorityCompare>();
}

void YEventData::_node_exiting_tree(Node *p_node) {
    ERR_FAIL_NULL(p_node);
    clear_node_callbacks(p_node);
}

void YEventData::register_listener(int p_event_id, const Callable &p_callable, int p_priority) {
    RegEventCallback &callback = reg_event_callbacks[p_event_id];
    callback.event_id = p_event_id;
    callback.callbacks.push_back(RegEventCallbackInstance(ObjectID(), p_callable, p_priority));
    _sort_event_callbacks(p_event_id);
}

void YEventData::unregister_listener(int p_event_id, const Callable &p_callable) {
    if (!reg_event_callbacks.has(p_event_id)) {
        return;
    }

    RegEventCallback &callback = reg_event_callbacks[p_event_id];
    for (int i = 0; i < callback.callbacks.size(); i++) {
        if (callback.callbacks[i].node_inst_id.is_null() && callback.callbacks[i].callable == p_callable) {
            callback.callbacks.remove_at(i);
            break;
        }
    }

    if (callback.callbacks.is_empty()) {
        reg_event_callbacks.erase(p_event_id);
    }
}

void YEventData::register_listener_with_node(Node *p_node, int p_event_id, const Callable &p_callable, int p_priority) {
    ERR_FAIL_NULL(p_node);

    RegEventCallback &callback = reg_event_callbacks[p_event_id];
    callback.event_id = p_event_id;
    callback.callbacks.push_back(RegEventCallbackInstance(p_node->get_instance_id(), p_callable, p_priority));
    _sort_event_callbacks(p_event_id);

    ObjectID node_id = p_node->get_instance_id();
    if (!count_node_callbacks.has(node_id)) {
        count_node_callbacks[node_id] = 1;
        p_node->connect("tree_exiting", callable_mp_static(&YEventData::_node_exiting_tree).bind(p_node));
    } else {
        count_node_callbacks[node_id]++;
    }
}

void YEventData::unregister_listener_with_node(Node *p_node, int p_event_id, const Callable &p_callable) {
    ERR_FAIL_NULL(p_node);

    if (!reg_event_callbacks.has(p_event_id)) {
        return;
    }

    ObjectID node_id = p_node->get_instance_id();
    RegEventCallback &callback = reg_event_callbacks[p_event_id];
    
    for (int i = 0; i < callback.callbacks.size(); i++) {
        if (callback.callbacks[i].node_inst_id == node_id && callback.callbacks[i].callable == p_callable) {
            callback.callbacks.remove_at(i);
            
            if (count_node_callbacks.has(node_id)) {
                count_node_callbacks[node_id]--;
                if (count_node_callbacks[node_id] <= 0) {
                    count_node_callbacks.erase(node_id);
                    if (p_node->is_connected("tree_exiting", callable_mp_static(&YEventData::_node_exiting_tree).bind(p_node))) {
                        p_node->disconnect("tree_exiting", callable_mp_static(&YEventData::_node_exiting_tree).bind(p_node));
                    }
                }
            }
            break;
        }
    }

    if (callback.callbacks.is_empty()) {
        reg_event_callbacks.erase(p_event_id);
    }
}

void YEventData::clear_node_callbacks(Node *p_node) {
    ERR_FAIL_NULL(p_node);
    ObjectID node_id = p_node->get_instance_id();

    for (KeyValue<int, RegEventCallback> &E : reg_event_callbacks) {
        RegEventCallback &callback = E.value;
        for (int i = callback.callbacks.size() - 1; i >= 0; i--) {
            if (callback.callbacks[i].node_inst_id == node_id) {
                callback.callbacks.remove_at(i);
            }
        }
        if (callback.callbacks.is_empty()) {
            reg_event_callbacks.erase(E.key);
        }
    }

    count_node_callbacks.erase(node_id);
    if (p_node->is_connected("tree_exiting", callable_mp_static(&YEventData::_node_exiting_tree).bind(p_node))) {
        p_node->disconnect("tree_exiting", callable_mp_static(&YEventData::_node_exiting_tree).bind(p_node));
    }
}

YEventData::YEventData() {
    event_type = 0;
}


Ref<YEventData> YEventData::set_event_type(int p_type) {
    event_type = p_type;
    return this;
}

int YEventData::get_event_type() const {
    return event_type;
}

Ref<YEventData> YEventData::create(int with_type, int parameter_one, Variant value_one) {
    Ref<YEventData> event;
    event.instantiate();
    event->event_type = with_type;
    event->set_value(parameter_one, value_one);
    return event;
}

bool YEventData::has_value(int p_identifier) const {
    return data.has(p_identifier);
}

Variant YEventData::get_value(int p_identifier, const Variant &p_default) const {
    if (const Variant *value = data.getptr(p_identifier)) {
        return *value;
    }
    return p_default;
}

Ref<YEventData> YEventData::set_value(int p_identifier, const Variant &p_value) {
    data[p_identifier] = p_value;
    return this;
}

Ref<YEventData> YEventData::increment_value(int p_identifier, const Variant &p_amount) {
    if (data.has(p_identifier)) {
        Variant current = data[p_identifier];
        if (current.get_type() == Variant::INT) {
            data[p_identifier] = current.operator int64_t() + p_amount.operator int64_t();
        } else if (current.get_type() == Variant::FLOAT) {
            data[p_identifier] = current.operator double() + p_amount.operator double();
        } else {
            // If it's not a number, just set it to the amount
            data[p_identifier] = p_amount;
        }
    } else {
        data[p_identifier] = p_amount;
    }
    return this;
}

Ref<YEventData> YEventData::duplicate() const {
    Ref<YEventData> new_event;
    new_event.instantiate();
    new_event->event_type = event_type;
    
    for (const KeyValue<int, Variant> &E : data) {
        new_event->set_value(E.key, E.value);
    }
    
    return new_event;
}

bool YEventData::is_event_type(int p_type) const {
    return event_type == p_type;
}

Dictionary YEventData::get_data_as_dictionary() const {
    Dictionary dict;
    for (const KeyValue<int, Variant> &E : data) {
        dict[E.key] = E.value;
    }
    return dict;
}

Ref<YEventData> YEventData::set_data_from_dictionary(const Dictionary &p_dict) {
    data.clear();
    for (const Variant *key = p_dict.next(nullptr); key; key = p_dict.next(key)) {
        if (key->get_type() == Variant::INT) {
            data[key->operator int64_t()] = p_dict[*key];
        }
    }
    return this;
}

Ref<YEventData> YEventData::add_data_from_dictionary(const Dictionary &p_dict) {
    for (const Variant *key = p_dict.next(nullptr); key; key = p_dict.next(key)) {
        if (key->get_type() == Variant::INT) {
            data[key->operator int64_t()] = p_dict[*key];
        }
    }
    return this;
}

void YEventData::_clean_invalid_callables(int p_event_id) {
    if (!reg_event_callbacks.has(p_event_id)) {
        return;
    }

    RegEventCallback &callback = reg_event_callbacks[p_event_id];
    for (int i = callback.callbacks.size() - 1; i >= 0; i--) {
        if (!callback.callbacks[i].callable.is_valid()) {
            // If this was a node callback, decrease the count
            if (!callback.callbacks[i].node_inst_id.is_null() && count_node_callbacks.has(callback.callbacks[i].node_inst_id)) {
                count_node_callbacks[callback.callbacks[i].node_inst_id]--;
                if (count_node_callbacks[callback.callbacks[i].node_inst_id] <= 0) {
                    count_node_callbacks.erase(callback.callbacks[i].node_inst_id);
                }
            }
            callback.callbacks.remove_at(i);
        }
    }

    if (callback.callbacks.is_empty()) {
        reg_event_callbacks.erase(p_event_id);
    }
}

Ref<YEventData> YEventData::emit() const {
    if (!reg_event_callbacks.has(event_type)) {
        return Ref<YEventData>(this);
    }

    bool has_invalid = false;
    const RegEventCallback &callback = reg_event_callbacks[event_type];
    for (const RegEventCallbackInstance &instance : callback.callbacks) {
        if (instance.callable.is_valid()) {
            instance.callable.call(this);
        } else {
            has_invalid = true;
        }
    }

    if (has_invalid) {
        _clean_invalid_callables(event_type);
    }
    return Ref<YEventData>(this);
}

Ref<YEventData> YEventData::emit_to_node(Node *p_node) const {
    if (p_node == nullptr) {
        return Ref<YEventData>(this );
    }
    
    if (!reg_event_callbacks.has(event_type)) {
        return Ref<YEventData>(this);
    }

    bool has_invalid = false;
    ObjectID node_id = p_node->get_instance_id();
    const RegEventCallback &callback = reg_event_callbacks[event_type];
    for (const RegEventCallbackInstance &instance : callback.callbacks) {
        if (instance.node_inst_id == node_id) {
            if (instance.callable.is_valid()) {
                instance.callable.call(this);
            } else {
                has_invalid = true;
            }
        }
    }

    if (has_invalid) {
        _clean_invalid_callables(event_type);
    }
    return Ref<YEventData>(this);
}
