
#include "richtextlabel3d_gizmo_plugin.h"

#include "../richtextlabel3d.h"

bool RichTextLabel3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<RichTextLabel3D>(p_spatial) != nullptr;
}

String RichTextLabel3DGizmoPlugin::get_gizmo_name() const {
	return "RichTextLabel3D";
}

int RichTextLabel3DGizmoPlugin::get_priority() const {
	return -1;
}

bool RichTextLabel3DGizmoPlugin::can_be_hidden() const {
	return false;
}

void RichTextLabel3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	RichTextLabel3D *label = Object::cast_to<RichTextLabel3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	Ref<TriangleMesh> tm = label->generate_triangle_mesh();
	if (tm.is_valid()) {
		p_gizmo->add_collision_triangles(tm);
	}
}
