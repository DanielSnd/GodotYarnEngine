//
// Created by Daniel on 2024-03-19.
//

#ifndef YGAMELOG_H
#define YGAMELOG_H
#include "core/object/ref_counted.h"

class YGameLog : public RefCounted {
    GDCLASS(YGameLog, RefCounted);

protected:
    static Ref<YGameLog> singleton;
    static void _bind_methods();


public:
    static YGameLog *get_singleton();

    static void set_singleton(const Ref<YGameLog> &ref) {
        singleton = ref;
    }

    Vector<String> logged;

    YGameLog() {
    }

    ~YGameLog() {
        logged.clear();
        if(singleton.is_valid() && singleton == this) {
            singleton.unref();
        }
    }
};



#endif //YGAMELOG_H
