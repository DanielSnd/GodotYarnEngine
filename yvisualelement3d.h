//
// Created by Daniel on 2024-03-19.
//

#ifndef YVISUALELEMENT3D_H
#define YVISUALELEMENT3D_H
#include "scene/3d/physics/character_body_3d.h"


class YVisualElement3D : public CharacterBody3D {
    GDCLASS(YVisualElement3D,CharacterBody3D);

protected:
    void _notification(int p_what);
    static void _bind_methods();

public:

    int visual_element_type;

    YVisualElement3D() {
        visual_element_type = 0;
    }
};



#endif //YVISUALELEMENT3D_H
