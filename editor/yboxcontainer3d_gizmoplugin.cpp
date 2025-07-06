
#ifdef TOOLS_ENABLED

#include "yboxcontainer3d_gizmoplugin.h"
#include "editor/editor_node.h"
#include "editor/settings/editor_settings.h"
#include "scene/3d/mesh_instance_3d.h"

bool YBoxContainer3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
    return Object::cast_to<YBoxContainer3D>(p_spatial) != nullptr;
}

String YBoxContainer3DGizmoPlugin::get_gizmo_name() const {
    return "YBoxContainer3D";
}

int YBoxContainer3DGizmoPlugin::get_priority() const {
    return -1;
}

void YBoxContainer3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
    YBoxContainer3D *container = Object::cast_to<YBoxContainer3D>(p_gizmo->get_node_3d());
    if (!container) {
        return;
    }

    p_gizmo->clear();

    // Get the container size
    Vector3 size = container->get_container_size();
    
    // Create the box outline as connected lines
    Vector<Vector3> lines;

    // Bottom face (z = -size.z/2)
    lines.push_back(Vector3(-size.x/2, -size.y/2, -size.z/2)); // 0
    lines.push_back(Vector3( size.x/2, -size.y/2, -size.z/2)); // 1
    lines.push_back(Vector3( size.x/2, -size.y/2, -size.z/2)); // 1
    lines.push_back(Vector3( size.x/2,  size.y/2, -size.z/2)); // 2
    lines.push_back(Vector3( size.x/2,  size.y/2, -size.z/2)); // 2
    lines.push_back(Vector3(-size.x/2,  size.y/2, -size.z/2)); // 3
    lines.push_back(Vector3(-size.x/2,  size.y/2, -size.z/2)); // 3
    lines.push_back(Vector3(-size.x/2, -size.y/2, -size.z/2)); // 0
    
    // Top face (z = size.z/2)
    lines.push_back(Vector3(-size.x/2, -size.y/2,  size.z/2)); // 4
    lines.push_back(Vector3( size.x/2, -size.y/2,  size.z/2)); // 5
    lines.push_back(Vector3( size.x/2, -size.y/2,  size.z/2)); // 5
    lines.push_back(Vector3( size.x/2,  size.y/2,  size.z/2)); // 6
    lines.push_back(Vector3( size.x/2,  size.y/2,  size.z/2)); // 6
    lines.push_back(Vector3(-size.x/2,  size.y/2,  size.z/2)); // 7
    lines.push_back(Vector3(-size.x/2,  size.y/2,  size.z/2)); // 7
    lines.push_back(Vector3(-size.x/2, -size.y/2,  size.z/2)); // 4
    
    // Vertical edges connecting bottom and top faces
    lines.push_back(Vector3(-size.x/2, -size.y/2, -size.z/2)); // 0
    lines.push_back(Vector3(-size.x/2, -size.y/2,  size.z/2)); // 4
    lines.push_back(Vector3( size.x/2, -size.y/2, -size.z/2)); // 1
    lines.push_back(Vector3( size.x/2, -size.y/2,  size.z/2)); // 5
    lines.push_back(Vector3( size.x/2,  size.y/2, -size.z/2)); // 2
    lines.push_back(Vector3( size.x/2,  size.y/2,  size.z/2)); // 6
    lines.push_back(Vector3(-size.x/2,  size.y/2, -size.z/2)); // 3
    lines.push_back(Vector3(-size.x/2,  size.y/2,  size.z/2)); // 7

    // Create the gizmo
    Ref<StandardMaterial3D> material = memnew(StandardMaterial3D);
    material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
    material->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);
    material->set_albedo(Color(0.8, 0.8, 0.8, 0.5));

    p_gizmo->add_lines(lines, material, false, Color(0.8, 0.8, 0.8, 0.5));
}

YBoxContainer3DGizmoPlugin::YBoxContainer3DGizmoPlugin() {
    create_material("main", Color(0.8, 0.8, 0.8, 0.5));
}

#endif