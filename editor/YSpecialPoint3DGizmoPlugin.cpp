//
// Created by Daniel on 2024-03-19.
//

#include "YSpecialPoint3DGizmoPlugin.h"

#if TOOLS_ENABLED
#include "editor/editor_node.h"
#include "editor/editor_string_names.h"
#include "editor/plugins/node_3d_editor_plugin.h"
#include "scene/3d/audio_listener_3d.h"
#include "../yspecialpoint3d.h"

class YSpecialPoint3D;

YSpecialPoint3DGizmoPlugin::YSpecialPoint3DGizmoPlugin() {
    create_icon_material("yspecialpoint_gizmo_3d_icon", EditorNode::get_singleton()->get_editor_theme()->get_icon(SNAME("GizmoDecal"), EditorStringName(EditorIcons)));
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
    const Ref<Material> icon = get_material("yspecialpoint_gizmo_3d_icon", p_gizmo);
    p_gizmo->add_unscaled_billboard(icon, 0.03);
}

#endif