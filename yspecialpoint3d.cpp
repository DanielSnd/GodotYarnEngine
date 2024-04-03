//
// Created by Daniel on 2024-03-19.
//

#include "yspecialpoint3d.h"

void YSpecialPoint3D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_point_priority"), &YSpecialPoint3D::get_point_priority);
    ClassDB::bind_method(D_METHOD("set_point_priority","point_priority"), &YSpecialPoint3D::set_point_priority);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "point_priority"), "set_point_priority", "get_point_priority");

    ClassDB::bind_method(D_METHOD("get_point_type"), &YSpecialPoint3D::get_point_type);
    ClassDB::bind_method(D_METHOD("set_point_type","point_type"), &YSpecialPoint3D::set_point_type);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "point_type"), "set_point_type", "get_point_type");
    
    ClassDB::bind_method(D_METHOD("get_point_status"), &YSpecialPoint3D::get_point_status);
    ClassDB::bind_method(D_METHOD("set_point_status","point_status"), &YSpecialPoint3D::set_point_status);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "point_status"), "set_point_status", "get_point_status");
}

void YSpecialPoint3D::_notification(int p_what) {

}
