//
// Created by Daniel on 2024-05-01.
//


#ifdef TOOLS_ENABLED

#include "yscenespawner3d_gizmo_plugin.h"

#include "../yscenespawner3d.h"


YSceneSpawner3DGizmoPlugin::YSceneSpawner3DGizmoPlugin() {
	const Color gizmo_color = SceneTree::get_singleton()->get_debug_collisions_color();
    create_material("shape_material", gizmo_color);
    // const float gizmo_value = gizmo_color.get_v();
    // const Color gizmo_color_disabled = Color(gizmo_value, gizmo_value, gizmo_value, 0.65);
    // create_material("shape_material_disabled", gizmo_color_disabled);
}

YSceneSpawner3DGizmoPlugin::~YSceneSpawner3DGizmoPlugin() {
}

bool YSceneSpawner3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
    return Object::cast_to<YSceneSpawner3D>(p_spatial) != nullptr;
}

String YSceneSpawner3DGizmoPlugin::get_gizmo_name() const {
    return "YSceneSpawner3D";
}

int YSceneSpawner3DGizmoPlugin::get_priority() const {
    return -1;
}

void YSceneSpawner3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	auto *cs = Object::cast_to<YSceneSpawner3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	if (cs == nullptr) {
		return;
	}

	if (!cs->debug_show_spawn_area) {
		return;
	}

	const Ref<Material> material = get_material("shape_material", p_gizmo);
	float r = cs->get_placing_radius();

	Vector<Vector3> points;

	for (int i = 0; i <= 360; i++) {
		float ra = Math::deg_to_rad((float)i);
		float rb = Math::deg_to_rad((float)i + 1);
		Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * r;
		Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * r;

		points.push_back(Vector3(a.x, 0, a.y));
		points.push_back(Vector3(b.x, 0, b.y));
		points.push_back(Vector3(0, a.x, a.y));
		points.push_back(Vector3(0, b.x, b.y));
		points.push_back(Vector3(a.x, a.y, 0));
		points.push_back(Vector3(b.x, b.y, 0));
	}

	Vector<Vector3> collision_segments;

	for (int i = 0; i < 64; i++) {
		float ra = i * (Math_TAU / 64.0);
		float rb = (i + 1) * (Math_TAU / 64.0);
		Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * r;
		Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * r;

		collision_segments.push_back(Vector3(a.x, 0, a.y));
		collision_segments.push_back(Vector3(b.x, 0, b.y));
		collision_segments.push_back(Vector3(0, a.x, a.y));
		collision_segments.push_back(Vector3(0, b.x, b.y));
		collision_segments.push_back(Vector3(a.x, a.y, 0));
		collision_segments.push_back(Vector3(b.x, b.y, 0));
	}

	p_gizmo->add_lines(points, material);
	p_gizmo->add_collision_segments(collision_segments);
}
#endif
