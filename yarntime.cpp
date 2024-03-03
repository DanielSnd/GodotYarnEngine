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

    ADD_SIGNAL(MethodInfo("clock_time_changed"));
    ADD_SIGNAL(MethodInfo("clock_hour_changed"));
    ADD_SIGNAL(MethodInfo("clock_day_changed"));
}

YTime::YTime() {
    amount_time_last_frame = 0.0;
    amount_when_paused = 0.0;
    amount_when_unpaused = 0.0;
    clock = 0u;
    is_paused = false;
}

YTime::~YTime() {
    if(singleton == this) {
        singleton.unref();
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
    pause_independent_time = static_cast<float>(OS::get_singleton()->get_ticks_msec()) * 0.001f;
    if (!is_paused) {
        time += pause_independent_time - amount_time_last_frame;
        amount_time_last_frame = pause_independent_time;
        if (is_clock_running) {
            if (has_time_elapsed(last_time_stepped_clock,clock_interval)) {
                last_time_stepped_clock = time;
                set_clock_and_emit_signal(clock + 1u);
            }
        }
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

void YTime::set_clock_and_emit_signal(const uint32_t val)  {
    const uint32_t previous_clock = clock;
    clock = val;
    emit_signal(SNAME("clock_time_changed"));
    if (get_clock_hour(previous_clock) != get_clock_hour(clock)) {
        emit_signal(SNAME("clock_hour_changed"));
        if (get_clock_day(previous_clock) != get_clock_day(clock)) {
            emit_signal(SNAME("clock_day_changed"));
        }
    }
}
