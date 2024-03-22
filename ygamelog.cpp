//
// Created by Daniel on 2024-03-19.
//

#include "ygamelog.h"

Ref<YGameLog> YGameLog::singleton;

void YGameLog::_bind_methods() {
    //ClassDB::bind_method(D_METHOD("raycast2d_to", "ray_origin", "ray_end", "collide_type", "collision_mask"), &YPhysics::raycast2d_to,DEFVAL(COLLIDE_WITH_BODIES),DEFVAL(UINT32_MAX));
}

YGameLog *YGameLog::get_singleton() {
    return *singleton;
}