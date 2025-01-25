//
// Created by Daniel on 2024-03-19.
//

#ifndef YSPECIALPOINT3DGIZMOPLUGIN_H
#define YSPECIALPOINT3DGIZMOPLUGIN_H

#if TOOLS_ENABLED
#include "editor/plugins/node_3d_editor_gizmos.h"

class YSpecialPoint3DGizmoPlugin : public EditorNode3DGizmoPlugin {
    GDCLASS(YSpecialPoint3DGizmoPlugin, EditorNode3DGizmoPlugin);

public:
    bool has_created_material = false;
    bool has_gizmo(Node3D *p_spatial) override;
    String get_gizmo_name() const override;
    int get_priority() const override;

    void redraw(EditorNode3DGizmo *p_gizmo) override;

    YSpecialPoint3DGizmoPlugin();
};
#endif

#endif //YSPECIALPOINT3DGIZMOPLUGIN_H
