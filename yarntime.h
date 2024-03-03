//
// Created by Daniel on 2024-02-29.
//

#ifndef YTIME_H
#define YTIME_H


#include "core/object/object.h"
#include "core/object/ref_counted.h"


class YTime : public RefCounted {
    GDCLASS(YTime, RefCounted);

protected:
    static Ref<YTime> singleton;

    static void _bind_methods();

    float amount_when_paused;
    float amount_when_unpaused;
    float amount_time_last_frame;
    float last_time_ended_pause;
    float last_time_stepped_clock;
public:
    static YTime *get_singleton();

    static void set_singleton(const Ref<YTime> &ref) {
        singleton = ref;
    }

    bool is_paused;
    bool get_is_paused() {return is_paused;}
    void set_is_paused(bool val);

    float time;
    float get_time() {return time;}
    void set_time(const float val) {time = val;}

    uint32_t clock = 0u;
    uint32_t get_clock() {return clock;}
    void set_clock(const uint32_t val) {clock = val;}
    void set_clock_and_emit_signal(const uint32_t val);

    uint32_t clock_days_in_month = 30u;
    uint32_t get_clock_days_in_month() {return clock_days_in_month;}
    void set_clock_days_in_month(const uint32_t val) {clock_days_in_month = val;}

    uint32_t clock_months_in_year = 4u;
    uint32_t get_clock_months_in_year() {return clock_months_in_year;}
    void set_clock_months_in_year(const uint32_t val) {clock_months_in_year = val;}

    PackedStringArray clock_month_names;
    PackedStringArray get_clock_month_names() {return clock_month_names;}
    void set_clock_month_names(const PackedStringArray val) {clock_month_names = val;}

    uint32_t get_clock_year(const uint32_t val) const { return 1u + static_cast<uint32_t>(static_cast<float>(val == 0 ? clock : val) * 0.00000578703f);}
    uint32_t get_clock_month(const uint32_t val) const { return 1u + static_cast<uint32_t>(static_cast<float>(val == 0 ? clock : val) * 0.000023149f) % (clock_months_in_year);}
    uint32_t get_clock_day(const uint32_t val) const { return 1u + static_cast<uint32_t>(static_cast<float>(val == 0 ? clock : val) * 0.000694445f) % (clock_days_in_month);}
    uint32_t get_clock_day_unconstrained(const uint32_t val) const { return 1u + static_cast<uint32_t>(static_cast<float>(val == 0 ? clock : val) * 0.000694445f);}
    uint32_t get_clock_hour(const uint32_t val) const { return static_cast<uint32_t>(static_cast<float>((val == 0u ? clock : val) % 1440u) * 0.01666667f) % 24u;}
    uint32_t get_clock_hour_unconstrained(const uint32_t val) const { return static_cast<uint32_t>(static_cast<float>((val == 0u ? clock : val) % 1440u) * 0.01666667f);}
    uint32_t get_clock_minute(const uint32_t val) const { return (val == 0u ? clock : val) % 60u;}
    uint32_t get_clock_minute_of_day(const uint32_t val) const { return (get_clock_hour(val) * 60u) + get_clock_minute(val);}
    String clock_formatted(const uint32_t val) const { return vformat("%02d:%02d",get_clock_hour(val),get_clock_minute(val));}
    String clock_formatted_day_month(const uint32_t val) const {
        uint32_t current_month = get_clock_month(val);
        if (clock_month_names.size() > current_month)
            return vformat("%02d %s",get_clock_day(val),clock_month_names[current_month]);
        return vformat("%02d/%02d",get_clock_day(val),current_month);
    }
    String clock_formatted_day_month_year(const uint32_t val) const {
        uint32_t current_month = get_clock_month(val);
        if (clock_month_names.size() > current_month)
            return vformat("%02d %s %02d",get_clock_day(val),clock_month_names[current_month],get_clock_year(val));
        return vformat("%02d/%02d/%02d",get_clock_day(val),current_month,get_clock_year(val));
    }
    String clock_formatted_full_date_time(const uint32_t val) const {
        uint32_t current_month = get_clock_month(val);
        if (clock_month_names.size() > current_month)
            return vformat("%02d %s %02d, %02d:%02d",get_clock_day(val),clock_month_names[current_month],get_clock_year(val),get_clock_hour(val),get_clock_minute(val));
        return vformat("%02d/%02d/%02d %02d:%02d",get_clock_day(val),current_month,get_clock_year(val),get_clock_hour(val),get_clock_minute(val));
    }

    bool has_clock_time_passed(uint32_t from_time,uint32_t check_interval) {return from_time + check_interval < clock;}

    float clock_interval = 1.0;
    float get_clock_interval() {return clock_interval;}
    void set_clock_interval(const float val) {clock_interval = val;}

    bool is_clock_running = false;
    bool get_is_clock_running() {return is_clock_running;}
    void set_is_clock_running(bool val) { is_clock_running = val;}

    bool attempt_pause();

    float pause_independent_time;
    float get_pause_independent_time() {return pause_independent_time;}
    void set_pause_independent_time(const float val) {pause_independent_time = val;}

    bool has_time_elapsed(float test_time, float interval) const;

    bool has_pause_independent_time_elapsed(float test_time, float interval) const;

    void handle_time_setting();

    YTime();
    ~YTime();
};



#endif //YTIME_H
