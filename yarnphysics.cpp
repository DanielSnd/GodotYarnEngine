//
// Created by Daniel on 2024-03-03.
//

#include "yarnphysics.h"

Ref<YPhysics> YPhysics::singleton;

void YPhysics::_bind_methods() {

}

YPhysics *YPhysics::get_singleton() {
    return *singleton;
}

YPhysics::YPhysics() {
}

YPhysics::~YPhysics() {
    if(singleton == this) {
        singleton.unref();
        singleton = nullptr;
    }
}
