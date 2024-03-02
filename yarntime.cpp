//
// Created by Daniel on 2024-02-29.
//

#include "yarntime.h"

#include "core/os/os.h"

Ref<YTime> YTime::singleton = nullptr;

YTime *YTime::get_singleton() {
    return *singleton;
}

void YTime::_bind_methods() {
    ClassDB::bind_method(D_METHOD("attempt_pause"), &YTime::attempt_pause);

    ClassDB::bind_method(D_METHOD("set_is_paused", "is_paused"), &YTime::set_is_paused);
    ClassDB::bind_method(D_METHOD("get_is_paused"), &YTime::get_is_paused);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "is_paused"), "set_is_paused", "get_is_paused");

    ClassDB::bind_method(D_METHOD("set_time", "time"), &YTime::set_time);
    ClassDB::bind_method(D_METHOD("get_time"), &YTime::get_time);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "time"), "set_time", "get_time");

    ClassDB::bind_method(D_METHOD("set_pause_independent_time", "pause_independent_time"), &YTime::set_pause_independent_time);
    ClassDB::bind_method(D_METHOD("get_pause_independent_time"), &YTime::get_pause_independent_time);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "pause_independent_time"), "set_pause_independent_time", "get_pause_independent_time");

    ClassDB::bind_method(D_METHOD("has_time_elapsed", "time_checking", "interval"), &YTime::has_time_elapsed);
    ClassDB::bind_method(D_METHOD("has_pause_independent_time_elapsed", "time_checking", "interval"), &YTime::has_pause_independent_time_elapsed);
}

YTime::YTime() {
    amount_time_last_frame = 0.0;
    amount_when_paused = 0.0;
    amount_when_unpaused = 0.0;
    is_paused = false;
}

YTime::~YTime() {
    if(singleton == this) {
        singleton = nullptr;
    }
}

bool YTime::has_time_elapsed(float test_time, float interval) {
    return test_time + interval < time;
}

bool YTime::has_pause_independent_time_elapsed(float test_time, float interval) {
    return test_time + interval < pause_independent_time;
}

void YTime::handle_time_setting() {
    pause_independent_time = OS::get_singleton()->get_ticks_msec() * 0.001f;
    if (!is_paused) {
        time += pause_independent_time - amount_time_last_frame;
        amount_time_last_frame = pause_independent_time;
    }
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