//
// Created by Daniel on 2024-05-01.
//

#ifndef YSCENESPAWNER3D_GIZMO_PLUGIN_H
#define YSCENESPAWNER3D_GIZMO_PLUGIN_H

#ifdef TOOLS_ENABLED
#include "editor/plugins/node_3d_editor_gizmos.h"
#include "editor/editor_settings.h"

class YSceneSpawner3DGizmoPlugin : public EditorNode3DGizmoPlugin {
    GDCLASS(YSceneSpawner3DGizmoPlugin, EditorNode3DGizmoPlugin);

public:
    bool has_gizmo(Node3D *p_spatial) override;
    String get_gizmo_name() const override;
    int get_priority() const override;
    void redraw(EditorNode3DGizmo *p_gizmo) override;

    YSceneSpawner3DGizmoPlugin();
    ~YSceneSpawner3DGizmoPlugin();
};

#endif

#endif //YSCENESPAWNER3D_GIZMO_PLUGIN_H
