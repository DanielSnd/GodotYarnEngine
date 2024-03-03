//
// Created by Daniel on 2024-03-03.
//

#ifndef YARNPHYSICS_H
#define YARNPHYSICS_H
#include "core/object/ref_counted.h"


class YPhysics : public RefCounted {
    GDCLASS(YPhysics, RefCounted);

protected:
    static Ref<YPhysics> singleton;

    static void _bind_methods();

public:
    static YPhysics *get_singleton();

    static void set_singleton(const Ref<YPhysics> &ref) {
        singleton = ref;
    }

    YPhysics();
    ~YPhysics();
};


#endif //YARNPHYSICS_H
