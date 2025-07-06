#ifndef EDITOR_YARN_3D_TOOLS_H
#define EDITOR_YARN_3D_TOOLS_H

#ifdef TOOLS_ENABLED
#include "editor/inspector/editor_inspector.h"
#include "editor/plugins/editor_plugin.h"
#include "editor/editor_interface.h"
#include "scene/3d/mesh_instance_3d.h"
#include "editor/editor_node.h"
#include "scene/3d/node_3d.h"
#include "editor/editor_undo_redo_manager.h"
#include "scene/resources/mesh.h"
#include "editor/file_system/editor_file_system.h"
#include "editor/inspector/editor_inspector.h"
#include "editor/gui/editor_file_dialog.h"
#include "editor/import/3d/scene_import_settings.h"
#include "editor/themes/editor_scale.h"
#include "modules/gltf/gltf_document.h"
#include "modules/gltf/gltf_state.h"

class EditorYarn3DTools : public EditorPlugin {
    GDCLASS(EditorYarn3DTools, EditorPlugin);

private:
    PopupMenu *tools_menu = nullptr;
    void _combine_meshes(Node3D *p_node);
    void _add_menu_item();
    void _menu_callback(int p_id);
    void _combine_meshes_callback();
    void _create_parent_node_callback();
    void _export_gltf_callback();
    void _export_gltf_file_selected(const String &p_path, Node *p_temp_root);

    // Helper functions for mesh combining
    void _collect_meshes(Node *p_node, List<MeshInstance3D *> &r_meshes);
    void _combine_mesh_arrays(const List<MeshInstance3D *> &p_meshes, Ref<ArrayMesh> &r_combined_mesh, const Transform3D &p_base_transform);
    void _transform_vertices(PackedVector3Array &r_vertices, const Transform3D &p_transform);
    void _transform_normals(PackedVector3Array &r_normals, const Transform3D &p_transform);

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:
    EditorYarn3DTools();
    ~EditorYarn3DTools();
};
#endif // TOOLS_ENABLED

#endif // EDITOR_YARN_3D_TOOLS_H 