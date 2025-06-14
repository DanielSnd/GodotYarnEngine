#ifndef YBOX_CONTAINER_3D_GIZMO_PLUGIN_H
#define YBOX_CONTAINER_3D_GIZMO_PLUGIN_H


#ifdef TOOLS_ENABLED
#include "editor/plugins/node_3d_editor_gizmos.h"
#include "../yboxcontainer3D.h"

class YBoxContainer3DGizmoPlugin : public EditorNode3DGizmoPlugin {
    GDCLASS(YBoxContainer3DGizmoPlugin, EditorNode3DGizmoPlugin);

public:
    bool has_gizmo(Node3D *p_spatial) override;
    String get_gizmo_name() const override;
    int get_priority() const override;
    void redraw(EditorNode3DGizmo *p_gizmo) override;

    YBoxContainer3DGizmoPlugin();
};
#endif

#endif // YBOX_CONTAINER_3D_GIZMO_PLUGIN_H
