
#pragma once

#include "editor/scene/3d/node_3d_editor_gizmos.h"

class RichTextLabel3DGizmoPlugin : public EditorNode3DGizmoPlugin {
	GDCLASS(RichTextLabel3DGizmoPlugin, EditorNode3DGizmoPlugin);

public:
	bool has_gizmo(Node3D *p_spatial) override;
	String get_gizmo_name() const override;
	int get_priority() const override;
	bool can_be_hidden() const override;
	void redraw(EditorNode3DGizmo *p_gizmo) override;
};
