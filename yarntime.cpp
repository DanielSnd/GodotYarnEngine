//
// Created by Daniel on 2024-02-29.
//

#include "yarntime.h"

YTime* YTime::singleton = nullptr;

YTime *YTime::get_singleton() {
    return singleton;
}

void YTime::_bind_methods() {
    ClassDB::bind_method(D_METHOD("attempt_pause"), &YTime::attempt_pause);

    ClassDB::bind_method(D_METHOD("set_is_paused", "is_paused"), &YTime::set_is_paused);
    ClassDB::bind_method(D_METHOD("get_is_paused"), &YTime::get_is_paused);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "is_paused"), "set_is_paused", "get_is_paused");

    ClassDB::bind_method(D_METHOD("set_is_debugging", "is_debugging"), &YTime::set_is_debugging);
    ClassDB::bind_method(D_METHOD("get_is_debugging"), &YTime::get_is_debugging);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_debugging"), "set_is_debugging", "get_is_debugging");

    ClassDB::bind_method(D_METHOD("set_time", "time"), &YTime::set_time);
    ClassDB::bind_method(D_METHOD("get_time"), &YTime::get_time);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "time"), "set_time", "get_time");

    ClassDB::bind_method(D_METHOD("set_pause_independent_time", "pause_independent_time"), &YTime::set_pause_independent_time);
    ClassDB::bind_method(D_METHOD("get_pause_independent_time"), &YTime::get_pause_independent_time);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "pause_independent_time"), "set_pause_independent_time", "get_pause_independent_time");

    ClassDB::bind_method(D_METHOD("has_time_elapsed", "time_checking", "interval"), &YTime::has_time_elapsed);
    ClassDB::bind_method(D_METHOD("has_pause_independent_time_elapsed", "time_checking", "interval"), &YTime::has_pause_independent_time_elapsed);
    ClassDB::bind_method(D_METHOD("has_clock_time_elapsed", "clock_time_checking", "interval"), &YTime::has_clock_time_passed);

    ClassDB::bind_method(D_METHOD("set_clock", "clock"), &YTime::set_clock);
    ClassDB::bind_method(D_METHOD("get_clock"), &YTime::get_clock);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "clock"), "set_clock", "get_clock");

    ClassDB::bind_method(D_METHOD("set_clock_and_emit_signal", "clock"), &YTime::set_clock_and_emit_signal);

    ClassDB::bind_method(D_METHOD("set_clock_interval", "clock_interval"), &YTime::set_clock_interval);
    ClassDB::bind_method(D_METHOD("get_clock_interval"), &YTime::get_clock_interval);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "clock_interval"), "set_clock_interval", "get_clock_interval");

    ClassDB::bind_method(D_METHOD("set_is_clock_running", "clock_running"), &YTime::set_is_clock_running);
    ClassDB::bind_method(D_METHOD("get_is_clock_running"), &YTime::get_is_clock_running);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "is_clock_running"), "set_is_clock_running", "get_is_clock_running");
    
    ClassDB::bind_method(D_METHOD("set_clock_days_in_month", "clock_days_in_month"), &YTime::set_clock_days_in_month);
    ClassDB::bind_method(D_METHOD("get_clock_days_in_month"), &YTime::get_clock_days_in_month);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "clock_days_in_month"), "set_clock_days_in_month", "get_clock_days_in_month");
    
    ClassDB::bind_method(D_METHOD("set_clock_months_in_year", "clock_months_in_year"), &YTime::set_clock_months_in_year);
    ClassDB::bind_method(D_METHOD("get_clock_months_in_year"), &YTime::get_clock_months_in_year);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "clock_months_in_year"), "set_clock_months_in_year", "get_clock_months_in_year");

    ClassDB::bind_method(D_METHOD("set_clock_month_names", "clock_month_names"), &YTime::set_clock_month_names);
    ClassDB::bind_method(D_METHOD("get_clock_month_names"), &YTime::get_clock_month_names);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "clock_month_names"), "set_clock_month_names", "get_clock_month_names");

    ClassDB::bind_method(D_METHOD("register_clock_callback","node","clock_time","callback"), &YTime::register_clock_callback);
    ClassDB::bind_method(D_METHOD("clear_clock_callbacks","node","clock_time"), &YTime::clear_clock_callbacks_node,DEFVAL(-1));

    ClassDB::bind_method(D_METHOD("get_clock_year", "clock_time"), &YTime::get_clock_year,DEFVAL(0u));
    ClassDB::bind_method(D_METHOD("get_clock_month", "clock_time"), &YTime::get_clock_month,DEFVAL(0u));
    ClassDB::bind_method(D_METHOD("get_clock_day", "clock_time"), &YTime::get_clock_day,DEFVAL(0u));
    ClassDB::bind_method(D_METHOD("get_clock_day_unconstrained", "clock_time"), &YTime::get_clock_day_unconstrained,DEFVAL(0u));
    ClassDB::bind_method(D_METHOD("get_clock_hour", "clock_time"), &YTime::get_clock_hour,DEFVAL(0u));
    ClassDB::bind_method(D_METHOD("get_clock_hour_unconstrained", "clock_time"), &YTime::get_clock_hour_unconstrained,DEFVAL(0u));
    ClassDB::bind_method(D_METHOD("get_clock_minute", "clock_time"), &YTime::get_clock_minute,DEFVAL(0u));
    ClassDB::bind_method(D_METHOD("get_clock_minute_of_day", "clock_time"), &YTime::get_clock_minute_of_day,DEFVAL(0u));
    ClassDB::bind_method(D_METHOD("get_clock_formatted", "clock_time"), &YTime::clock_formatted,DEFVAL(0u));
    ClassDB::bind_method(D_METHOD("get_clock_formatted_day_month", "clock_time"), &YTime::clock_formatted_day_month,DEFVAL(0u));
    ClassDB::bind_method(D_METHOD("get_clock_formatted_day_month_year", "clock_time"), &YTime::clock_formatted_day_month_year,DEFVAL(0u));
    ClassDB::bind_method(D_METHOD("get_clock_formatted_full_date_time", "clock_time"), &YTime::clock_formatted_full_date_time,DEFVAL(0u));

    ClassDB::bind_method(D_METHOD("get_registered_clock_callbacks"), &YTime::get_registered_clock_callbacks);

    ADD_SIGNAL(MethodInfo("clock_time_changed"));
    ADD_SIGNAL(MethodInfo("clock_hour_changed"));
    ADD_SIGNAL(MethodInfo("clock_day_changed"));
}

YTime::YTime(): last_time_ended_pause(0), last_time_stepped_clock(0) {
    singleton = this;
    amount_time_last_frame = 0.0;
    amount_when_paused = 0.0;
    amount_when_unpaused = 0.0;
    pause_independent_time = 0.0;
    time = 0.0;
    clock = 0u;
    is_paused = false;
    is_debugging = false;
}

YTime::~YTime() {
    if(singleton == this) {
        singleton = nullptr;
    }
}

bool YTime::has_time_elapsed(float test_time, float interval) const {
    return test_time + interval < time;
}

bool YTime::has_pause_independent_time_elapsed(float test_time, float interval) const {
    return test_time + interval < pause_independent_time;
}

void YTime::handle_time_setting() {
    if (OS::get_singleton() == nullptr) return;
    pause_independent_time = static_cast<float>(OS::get_singleton()->get_ticks_msec()) * 0.001f;
    if (!is_paused) {
        time += pause_independent_time - amount_time_last_frame;
        amount_time_last_frame = pause_independent_time;
        if (is_clock_running) {
            if (has_time_elapsed(last_time_stepped_clock,clock_interval)) {
                last_time_stepped_clock = time;
                set_clock_and_emit_signal(clock + 1);
            }
        }
    }
}

void YTime::register_clock_callback(Node *p_reference, int _clock_time, const Callable &p_callable) {
    if (p_reference == nullptr) return;
    ObjectID _node_id = p_reference->get_instance_id();
    if (!reg_clock_callbacks.has(_clock_time))
        reg_clock_callbacks[_clock_time] = RegTimeCallback{_clock_time};
    if (!count_node_callbacks.has(_node_id)) {
        count_node_callbacks[_node_id] = 0;
        if (!p_reference->is_connected(SceneStringName(tree_exiting), callable_mp(this, &YTime::clear_clock_callbacks).bind(_node_id, -1)))
            p_reference->connect(SceneStringName(tree_exiting), callable_mp(this, &YTime::clear_clock_callbacks).bind(_node_id, -1), CONNECT_ONE_SHOT);
    }
    count_node_callbacks[_node_id] += 1;
    reg_clock_callbacks[_clock_time].callbacks.append(RegTimeCallback::RegTimeCallbackInstance(_node_id,p_callable));
}

void YTime::clear_clock_callbacks(ObjectID p_node_inst_id, int _event_id) {
    Array event_ids_to_remove;
    Array ints_to_remove;
    if (_event_id != -1) {
        if (reg_clock_callbacks.has(_event_id)) {
            int index = 0;
            for (auto _regEventCallback_instance: reg_clock_callbacks[_event_id].callbacks) {
                if (_regEventCallback_instance.node_inst_id == p_node_inst_id) {
                    ints_to_remove.push_front(index);
                }
                index++;
            }
            for (int i = 0; i < ints_to_remove.size(); ++i) {
                reg_clock_callbacks[_event_id].callbacks.remove_at(ints_to_remove[i]);
            }
            ints_to_remove.clear();
            if (reg_clock_callbacks[_event_id].callbacks.size() == 0) {
                reg_clock_callbacks.erase(_event_id);
            }
        }
    } else {
        for (auto _regEventCallback: reg_clock_callbacks) {
            int _index = 0;
            for (auto _regEventCallback_instance: _regEventCallback.value.callbacks) {
                if (_regEventCallback_instance.node_inst_id == p_node_inst_id) {
                    ints_to_remove.push_front(_index);
                }
                _index++;
            }
            for (int i = 0; i < ints_to_remove.size(); ++i) {
                _regEventCallback.value.callbacks.remove_at(ints_to_remove[i]);
            }
            ints_to_remove.clear();
            if (_regEventCallback.value.callbacks.size() == 0) {
                event_ids_to_remove.push_front(_regEventCallback.key);
            }
        }
    }
    if (event_ids_to_remove.size()>0) {
        for (int i = 0; i < event_ids_to_remove.size(); ++i) {
            reg_clock_callbacks.erase(event_ids_to_remove[i]);
        }
        event_ids_to_remove.clear();
    }
}

void YTime::clear_clock_callbacks_node(Node *_reference, int _event_id) {
    if (_reference != nullptr)
        clear_clock_callbacks(_reference->get_instance_id(),_event_id);
}


bool YTime::attempt_pause() {
    if (last_time_ended_pause + 0.3 < pause_independent_time) {
        return false;
    }
    return is_paused;
}

void YTime::set_is_paused(bool val) {
    if (is_paused != val) {
        is_paused = val;
        if (is_paused)
            amount_when_paused = OS::get_singleton()->get_ticks_msec() * 0.001f;
        else
            amount_when_unpaused = OS::get_singleton()->get_ticks_msec() * 0.001f;
        amount_time_last_frame = amount_when_unpaused;
        emit_signal(SNAME("changed_pause"),val);
    }
}

void YTime::set_is_debugging(bool val) {
    is_debugging = val;
}

void YTime::handle_clock_callbacks_for(const int val) {
    if (reg_clock_callbacks.has(val)) {
        if (is_debugging) {
            print_line(vformat("[YTime] Found callbacks for clock time %d", val));
        }
        auto _reg_event_callbacks = reg_clock_callbacks[val];
        Vector<Callable> callables_to_call;
        for (const auto& callback: _reg_event_callbacks.callbacks) {
            if (callback.callable.is_valid()) {
                if (is_debugging) {
                    print_line(vformat("[YTime] Adding valid callable for node %d", callback.node_inst_id));
                }
                callables_to_call.append(callback.callable);
                if (count_node_callbacks.has(callback.node_inst_id)) {
                    const int current_count = count_node_callbacks[callback.node_inst_id];
                    if (current_count - 1 <=0 ) {
                        if (is_debugging) {
                            print_line(vformat("[YTime] Removing node %d from count callbacks", callback.node_inst_id));
                        }
                        count_node_callbacks.erase(callback.node_inst_id);
                    } else {
                        if (is_debugging) {
                            print_line(vformat("[YTime] Reducing count for node %d from %d to %d", 
                                callback.node_inst_id, current_count, current_count-1));
                        }
                        count_node_callbacks[callback.node_inst_id] = current_count-1;
                    }
                }
            } else if (is_debugging) {
                print_line("[YTime] Found invalid callable");
            }
        }
        _reg_event_callbacks.callbacks.clear();
        reg_clock_callbacks.erase(val);
        
        if (is_debugging) {
            print_line(vformat("[YTime] Executing %d callables for clock time %d", 
                callables_to_call.size(), val));
        }
        
        for (const auto& to_call: callables_to_call) {
            if (!to_call.is_null() && to_call.is_valid()) {
                if (is_debugging) {
                    print_line("[YTime] Calling callable");
                }
                const auto result = to_call.call();
            }
        }
    }
}

void YTime::set_clock_and_emit_signal(const int val)  {
    const int previous_clock = clock;
    clock = val;
    emit_signal(SNAME("clock_time_changed"));
    if (get_clock_hour(previous_clock) != get_clock_hour(clock)) {
        emit_signal(SNAME("clock_hour_changed"));
        if (get_clock_day(previous_clock) != get_clock_day(clock)) {
            emit_signal(SNAME("clock_day_changed"));
        }
    }
    if (ABS(previous_clock - val) >= 1) {
        Vector<int> _passed_times_to_callback;
        for (const auto& errlist: reg_clock_callbacks) {
            if (errlist.key < clock) {
                _passed_times_to_callback.push_back(errlist.key);
            }
        }
        if (_passed_times_to_callback.size() > 0) {
            for (int i : _passed_times_to_callback) {
                handle_clock_callbacks_for(i);
            }
        }
    }
    if (is_debugging) {
        print_line(vformat("[YTime] Clock changed from %d to %d. Registered clock callbacks %d. Callbacks have new time? %s"
            ,previous_clock,val,static_cast<int>(reg_clock_callbacks.size()), reg_clock_callbacks.has(val)));
    }
    if (reg_clock_callbacks.has(val)) {
        handle_clock_callbacks_for(val);
    }
}

Dictionary YTime::get_registered_clock_callbacks() const {
    Dictionary result;
    
    for (const KeyValue<int, RegTimeCallback> &E : reg_clock_callbacks) {
        Array callbacks;
        for (const RegTimeCallback::RegTimeCallbackInstance &callback : E.value.callbacks) {
            Dictionary callback_info;
            callback_info["node_id"] = callback.node_inst_id;
            callback_info["callable"] = callback.callable;
            callbacks.push_back(callback_info);
        }
        result[E.key] = callbacks;
    }
    
    return result;
}
