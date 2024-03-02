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

    bool attempt_pause();

    float pause_independent_time;
    float get_pause_independent_time() {return pause_independent_time;}
    void set_pause_independent_time(const float val) {pause_independent_time = val;}

    bool has_time_elapsed(float test_time, float interval);

    bool has_pause_independent_time_elapsed(float test_time, float interval);

    void handle_time_setting();

    YTime();
    ~YTime();
};



#endif //YTIME_H
