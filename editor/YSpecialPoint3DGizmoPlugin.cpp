//
// Created by Daniel on 2024-03-19.
//

#include "YSpecialPoint3DGizmoPlugin.h"

#if TOOLS_ENABLED
#include "editor/editor_node.h"
#include "editor/editor_string_names.h"
#include "editor/scene/3d/node_3d_editor_plugin.h"
#include "scene/3d/audio_listener_3d.h"
#include "../yspecialpoint3d.h"

class YSpecialPoint3D;

YSpecialPoint3DGizmoPlugin::YSpecialPoint3DGizmoPlugin() {
    has_created_material = false;
}

bool YSpecialPoint3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
    return Object::cast_to<YSpecialPoint3D>(p_spatial) != nullptr;
}

String YSpecialPoint3DGizmoPlugin::get_gizmo_name() const {
    return "YSpecialPoint3D";
}

int YSpecialPoint3DGizmoPlugin::get_priority() const {
    return -1;
}

void YSpecialPoint3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
    if (!has_created_material && EditorNode::get_singleton() != nullptr) {
        has_created_material = true;
        Color desired_color = Color(1.0,1.0,1.0,1.0);
        create_icon_material("yspecialpoint_gizmo_3d_icon",
            EditorNode::get_singleton()->get_editor_theme()->
            get_icon(SNAME("GizmoDecal"), EditorStringName(EditorIcons)),false,desired_color);
    }
    YSpecialPoint3D *specialpoint = Object::cast_to<YSpecialPoint3D>(p_gizmo->get_node_3d());

    const Ref<StandardMaterial3D> icon = get_material("yspecialpoint_gizmo_3d_icon", p_gizmo);
    p_gizmo->add_unscaled_billboard(icon, 0.03);

    auto priority_point = specialpoint->get_point_priority();
    icon->set_albedo(Color(0.2, 0.9, 0.98).lerp(Color(0.6, 0.7, 0.4),
                                                static_cast<float>(CLAMP(Math::inverse_lerp(static_cast<float>(-1.0), static_cast<float>(20.0), static_cast<float>(priority_point)),0.0,1.0))));
}

#endif