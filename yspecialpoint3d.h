//
// Created by Daniel on 2024-03-19.
//

#ifndef YSPECIALPOINT3D_H
#define YSPECIALPOINT3D_H

#include "scene/3d/node_3d.h"

class YSpecialPoint3D : public Node3D {
    GDCLASS(YSpecialPoint3D, Node3D);

private:
    static void _bind_methods();
    void _notification(int p_what);

public:
    int point_priority;
    void set_point_priority(int v) {point_priority = v;}
    int get_point_priority() const {return point_priority;}
    
    int point_type;
    void set_point_type(int v) {point_type = v;}
    int get_point_type() const {return point_type;}

    int point_status;
    void set_point_status(int v) {point_status = v;}
    int get_point_status() const {return point_status;}

    YSpecialPoint3D() = default;
};



#endif //YSPECIALPOINT3D_H
