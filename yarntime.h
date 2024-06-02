//
// Created by Daniel on 2024-02-29.
//

#ifndef YTIME_H
#define YTIME_H


#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "scene/main/node.h"


class YTime : public RefCounted {
    GDCLASS(YTime, RefCounted);

protected:
    static Ref<YTime> singleton;

    static void _bind_methods();

    struct RegTimeCallback {
        struct RegTimeCallbackInstance {
            ObjectID node_inst_id;
            Callable callable;
            RegTimeCallbackInstance() : node_inst_id(), callable() {}
            RegTimeCallbackInstance(ObjectID node_inst_id, const Callable &callable) : node_inst_id(node_inst_id), callable(callable) {}
        };
        int event_id;
        Vector<RegTimeCallbackInstance> callbacks;
        RegTimeCallback() : event_id(0) {}
        explicit RegTimeCallback(int time) : event_id(0) {}
    };
    HashMap<int,RegTimeCallback> reg_clock_callbacks;
    HashMap<ObjectID,int> count_node_callbacks;
    HashMap<int,int> registered_clock_events;

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

    void handle_clock_callbacks_for(int val);

    float time;
    float get_time() {return time;}
    void set_time(const float val) {time = val;}

    int clock = 0;
    int get_clock() {return clock;}
    void set_clock(const int val) {clock = val;}
    void set_clock_and_emit_signal(const int val);

    int clock_days_in_month = 30;
    int get_clock_days_in_month() {return clock_days_in_month;}
    void set_clock_days_in_month(const int val) {clock_days_in_month = val;}

    int clock_months_in_year = 4u;
    int get_clock_months_in_year() {return clock_months_in_year;}
    void set_clock_months_in_year(const int val) {clock_months_in_year = val;}

    PackedStringArray clock_month_names;
    PackedStringArray get_clock_month_names() {return clock_month_names;}
    void set_clock_month_names(const PackedStringArray val) {clock_month_names = val;}

    int get_clock_year(const int val) const { return 1 + static_cast<int>(static_cast<float>(val == 0 ? clock : val) * 0.00000578703f);}
    int get_clock_month(const int val) const { return 1 + static_cast<int>(static_cast<float>(val == 0 ? clock : val) * 0.000023149f) % (clock_months_in_year);}
    int get_clock_day(const int val) const { return 1 + static_cast<int>(static_cast<float>(val == 0 ? clock : val) * 0.000694445f) % (clock_days_in_month);}
    int get_clock_day_unconstrained(const int val) const { return 1 + static_cast<int>(static_cast<float>(val == 0 ? clock : val) * 0.000694445f);}
    int get_clock_hour(const int val) const { return static_cast<uint32_t>(static_cast<float>((val == 0 ? clock : val) % 1440u) * 0.01666667f) % 24u;}
    int get_clock_hour_unconstrained(const int val) const { return static_cast<int>(static_cast<float>((val == 0 ? clock : val) % 1440u) * 0.01666667f);}
    int get_clock_minute(const int val) const { return (val == 0 ? clock : val) % 60;}
    int get_clock_minute_of_day(const int val) const { return (get_clock_hour(val) * 60) + get_clock_minute(val);}
    String clock_formatted(const int val) const { return vformat("%02d:%02d",get_clock_hour(val),get_clock_minute(val));}
    String clock_formatted_day_month(const int val) const {
        int current_month = get_clock_month(val);
        if (clock_month_names.size() > current_month)
            return vformat("%02d %s",get_clock_day(val),clock_month_names[current_month]);
        return vformat("%02d/%02d",get_clock_day(val),current_month);
    }
    String clock_formatted_day_month_year(const int val) const {
        int current_month = get_clock_month(val);
        if (clock_month_names.size() > current_month)
            return vformat("%02d %s %02d",get_clock_day(val),clock_month_names[current_month],get_clock_year(val));
        return vformat("%02d/%02d/%02d",get_clock_day(val),current_month,get_clock_year(val));
    }
    String clock_formatted_full_date_time(const int val) const {
        int current_month = get_clock_month(val);
        if (clock_month_names.size() > current_month)
            return vformat("%02d %s %02d, %02d:%02d",get_clock_day(val),clock_month_names[current_month],get_clock_year(val),get_clock_hour(val),get_clock_minute(val));
        return vformat("%02d/%02d/%02d %02d:%02d",get_clock_day(val),current_month,get_clock_year(val),get_clock_hour(val),get_clock_minute(val));
    }

    void register_clock_callback(Node* _reference, int _clock_time, const Callable &callable);
    void clear_clock_callbacks(ObjectID p_node_inst_id, int _clock_time);
    void clear_clock_callbacks_node(Node* _reference, int _clock_time);

    bool has_clock_time_passed(int from_time,int check_interval) {return from_time + check_interval < clock;}

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
