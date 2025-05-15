#include "editor_yarn_3d_tools.h"

#ifdef TOOLS_ENABLED

void EditorYarn3DTools::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_menu_callback"), &EditorYarn3DTools::_menu_callback);
    ClassDB::bind_method(D_METHOD("_create_parent_node_callback"), &EditorYarn3DTools::_create_parent_node_callback);
    ClassDB::bind_method(D_METHOD("_export_gltf_callback"), &EditorYarn3DTools::_export_gltf_callback);
    ClassDB::bind_method(D_METHOD("_export_gltf_file_selected"), &EditorYarn3DTools::_export_gltf_file_selected);
}

EditorYarn3DTools::EditorYarn3DTools() {
    tools_menu = memnew(PopupMenu);
    tools_menu->add_item("Create Combined Mesh", 0);
    tools_menu->add_item("Create parent Node3D", 1);
    tools_menu->add_item("Export Node3D as GLTF", 2);
    tools_menu->connect("id_pressed", callable_mp(this, &EditorYarn3DTools::_menu_callback));
    _add_menu_item();
}

EditorYarn3DTools::~EditorYarn3DTools() {
    if (tools_menu) {
        tools_menu->queue_free();
        tools_menu = nullptr;
    }
}

void EditorYarn3DTools::_add_menu_item() {
    add_tool_submenu_item("3D Tools", tools_menu);
}

void EditorYarn3DTools::_menu_callback(int p_id) {
    switch (p_id) {
        case 0: // Create Combined Mesh
            _combine_meshes_callback();
            break;
        case 1: // Create parent Node3D
            _create_parent_node_callback();
            break;
        case 2: // Export as GLTF
            _export_gltf_callback();
            break;
    }
}

void EditorYarn3DTools::_combine_meshes_callback() {
    Array selected = EditorNode::get_singleton()->get_editor_selection()->get_selected_nodes();
    
    if (selected.is_empty()) {
        EditorNode::get_singleton()->show_warning("Please select a Node3D to combine meshes.");
        return;
    }

    for (int i = 0; i < selected.size(); i++) {
        Node3D *n = Object::cast_to<Node3D>(selected[i]);
        if (!n) {
            EditorNode::get_singleton()->show_warning("Selected node must be a Node3D.");
            return;
        }

        _combine_meshes(n);
    }
}

void EditorYarn3DTools::_create_parent_node_callback() {
    Array selected = EditorNode::get_singleton()->get_editor_selection()->get_selected_nodes();
    
    if (selected.is_empty()) {
        EditorNode::get_singleton()->show_warning("Please select at least one node.");
        return;
    }

    // Find the common parent of all selected nodes
    Node *common_parent = nullptr;
    for (int i = 0; i < selected.size(); i++) {
        Node *node = Object::cast_to<Node>(selected[i]);
        if (!node) continue;

        if (!common_parent) {
            common_parent = node->get_parent();
        } else if (common_parent != node->get_parent()) {
            EditorNode::get_singleton()->show_warning("Selected nodes must have the same parent.");
            return;
        }
    }

    if (!common_parent) {
        EditorNode::get_singleton()->show_warning("Selected nodes must have a parent.");
        return;
    }

    // Calculate the center position of all selected nodes
    Vector3 center_position;
    int valid_nodes = 0;
    for (int i = 0; i < selected.size(); i++) {
        Node3D *node_3d = Object::cast_to<Node3D>(selected[i]);
        if (node_3d) {
            center_position += node_3d->get_global_position();
            valid_nodes++;
        }
    }

    if (valid_nodes == 0) {
        EditorNode::get_singleton()->show_warning("No valid Node3D nodes selected.");
        return;
    }

    center_position /= valid_nodes;

    // Create undo/redo for the operation
    EditorUndoRedoManager *undo_redo = EditorUndoRedoManager::get_singleton();
    undo_redo->create_action("Create Parent Node3D");

    // Create new Node3D
    Node3D *new_parent = memnew(Node3D);
    new_parent->set_name("ParentNode3D");

    // Add the new node as a sibling of the selected nodes
    undo_redo->add_do_method(common_parent, "add_child", new_parent);
    Node *root_node = SceneTree::get_singleton()->get_edited_scene_root();
    undo_redo->add_do_method(new_parent, "set_owner", root_node);
    undo_redo->add_do_method(new_parent, "set_global_position", center_position);

    // Find the index of the first selected node to place the new parent close to it
    int target_index = -1;
    for (int i = 0; i < selected.size(); i++) {
        Node *node = Object::cast_to<Node>(selected[i]);
        if (node) {
            target_index = node->get_index();
            break;
        }
    }

    if (target_index != -1) {
        undo_redo->add_do_method(common_parent, "move_child", new_parent, target_index);
    }

    // Reparent all selected nodes
    for (int i = 0; i < selected.size(); i++) {
        Node3D *node_3d = Object::cast_to<Node3D>(selected[i]);
        if (node_3d) {
            // Store the global transform
            Transform3D global_transform = node_3d->get_global_transform();
            
            // Calculate local transform relative to the new parent
            Transform3D local_transform = global_transform;
            local_transform.origin -= center_position;
            
            // Reparent
            undo_redo->add_do_method(node_3d, "reparent", new_parent);
            
            // Set the local transform
            undo_redo->add_do_method(node_3d, "set_transform", local_transform);
            
            // Add undo operations
            undo_redo->add_undo_method(node_3d, "reparent", common_parent);
            undo_redo->add_undo_method(node_3d, "set_global_transform", global_transform);
        }
    }

    // Add undo operation for removing the new parent
    undo_redo->add_undo_method(common_parent, "remove_child", new_parent);

    undo_redo->commit_action();
}

void EditorYarn3DTools::_combine_meshes(Node3D *p_node) {
    List<MeshInstance3D *> meshes;
    _collect_meshes(p_node, meshes);

    if (meshes.is_empty()) {
        EditorNode::get_singleton()->show_warning("No meshes found under the selected node.");
        return;
    }

    Ref<ArrayMesh> combined_mesh;
    combined_mesh.instantiate();
    _combine_mesh_arrays(meshes, combined_mesh, p_node->get_global_transform());

    if (!combined_mesh.is_valid() || combined_mesh->get_surface_count() == 0) {
        EditorNode::get_singleton()->show_warning("Failed to combine meshes.");
        return;
    }

    // Create undo/redo for the operation
    EditorUndoRedoManager *undo_redo = EditorUndoRedoManager::get_singleton();
    undo_redo->create_action("Combine Meshes");

    // Create new MeshInstance3D
    MeshInstance3D *new_mesh_instance = memnew(MeshInstance3D);
    new_mesh_instance->set_mesh(combined_mesh);
    new_mesh_instance->set_name(vformat("%s_combined",p_node->get_name()));
    new_mesh_instance->set_transform(p_node->get_transform());

    // Add the new mesh instance as a sibling of the original node
    Node *parent = p_node->get_parent();
    if (parent) {
        undo_redo->add_do_method(parent, "add_child", new_mesh_instance);
        Node *root_node = SceneTree::get_singleton()->get_edited_scene_root();
        undo_redo->add_do_method(new_mesh_instance, "set_owner", root_node);
        
        // Move the new mesh instance to be right after the original node
        int target_index = p_node->get_index() + 1;
        undo_redo->add_do_method(parent, "move_child", new_mesh_instance, target_index);
        
        undo_redo->add_undo_method(parent, "remove_child", new_mesh_instance);
    }

    undo_redo->commit_action();
}

void EditorYarn3DTools::_collect_meshes(Node *p_node, List<MeshInstance3D *> &r_meshes) {
    MeshInstance3D *mesh_instance = Object::cast_to<MeshInstance3D>(p_node);
    if (mesh_instance && mesh_instance->get_mesh().is_valid()) {
        r_meshes.push_back(mesh_instance);
    }

    for (int i = 0; i < p_node->get_child_count(); i++) {
        _collect_meshes(p_node->get_child(i), r_meshes);
    }
}

void EditorYarn3DTools::_combine_mesh_arrays(const List<MeshInstance3D *> &p_meshes, Ref<ArrayMesh> &r_combined_mesh, const Transform3D &p_base_transform) {
    // Dictionary to store surfaces by material
    HashMap<Ref<Material>, List<Array>> surfaces_by_material;

    // Process each mesh
    for (const MeshInstance3D *mesh_instance : p_meshes) {
        Ref<Mesh> mesh = mesh_instance->get_mesh();
        if (!mesh.is_valid()) continue;

        // Get the relative transform from the base node to this mesh
        Transform3D relative_transform = p_base_transform.affine_inverse() * mesh_instance->get_global_transform();

        // Process each surface in the mesh
        for (int surface_i = 0; surface_i < mesh->get_surface_count(); surface_i++) {
            Array surface_array = mesh->surface_get_arrays(surface_i);
            if (surface_array.is_empty()) continue;

            Ref<Material> material = mesh->surface_get_material(surface_i);

            // Transform vertices
            PackedVector3Array vertices = surface_array[Mesh::ARRAY_VERTEX];
            if (!vertices.is_empty()) {
                _transform_vertices(vertices, relative_transform);
                surface_array[Mesh::ARRAY_VERTEX] = vertices;
            }

            // Transform normals if they exist
            if (surface_array[Mesh::ARRAY_NORMAL].get_type() != Variant::NIL) {
                PackedVector3Array normals = surface_array[Mesh::ARRAY_NORMAL];
                if (normals.size() == vertices.size()) {
                    _transform_normals(normals, relative_transform);
                    surface_array[Mesh::ARRAY_NORMAL] = normals;
                }
            }

            // Add to the appropriate material group
            if (!surfaces_by_material.has(material)) {
                surfaces_by_material[material] = List<Array>();
            }
            surfaces_by_material[material].push_back(surface_array);
        }
    }

    // Combine surfaces by material
    int surface_count = 0;
    for (const KeyValue<Ref<Material>, List<Array>> &E : surfaces_by_material) {
        const Ref<Material> &material = E.key;
        const List<Array> &arrays_list = E.value;

        if (arrays_list.is_empty()) continue;

        // Calculate total vertices
        int total_vertices = 0;
        for (const Array &arrays : arrays_list) {
            if (arrays[Mesh::ARRAY_VERTEX].get_type() != Variant::NIL) {
                PackedVector3Array vertices = arrays[Mesh::ARRAY_VERTEX];
                total_vertices += vertices.size();
            }
        }

        // Create combined arrays
        Array combined_arrays;
        combined_arrays.resize(Mesh::ARRAY_MAX);

        // Initialize arrays with correct sizes
        for (int array_type = 0; array_type < Mesh::ARRAY_MAX; array_type++) {
            if (arrays_list.front()->get()[array_type].get_type() != Variant::NIL) {
                if (array_type == Mesh::ARRAY_INDEX) {
                    combined_arrays[array_type] = PackedInt32Array();
                } else if (array_type == Mesh::ARRAY_VERTEX) {
                    PackedVector3Array vertices;
                    vertices.resize(total_vertices);
                    combined_arrays[array_type] = vertices;
                } else if (array_type == Mesh::ARRAY_NORMAL) {
                    PackedVector3Array normals;
                    normals.resize(total_vertices);
                    combined_arrays[array_type] = normals;
                } else if (array_type == Mesh::ARRAY_TANGENT) {
                    PackedFloat32Array tangents;
                    tangents.resize(total_vertices * 4);
                    combined_arrays[array_type] = tangents;
                } else if (array_type == Mesh::ARRAY_COLOR) {
                    PackedColorArray colors;
                    colors.resize(total_vertices);
                    combined_arrays[array_type] = colors;
                } else if (array_type == Mesh::ARRAY_TEX_UV) {
                    PackedVector2Array uvs;
                    uvs.resize(total_vertices);
                    combined_arrays[array_type] = uvs;
                } else if (array_type == Mesh::ARRAY_TEX_UV2) {
                    PackedVector2Array uvs2;
                    uvs2.resize(total_vertices);
                    combined_arrays[array_type] = uvs2;
                } else if (array_type >= Mesh::ARRAY_CUSTOM0 && array_type <= Mesh::ARRAY_CUSTOM3) {
                    PackedFloat32Array custom;
                    custom.resize(total_vertices * 4);
                    combined_arrays[array_type] = custom;
                } else if (array_type == Mesh::ARRAY_BONES) {
                    PackedInt32Array bones;
                    bones.resize(total_vertices * 4);
                    combined_arrays[array_type] = bones;
                } else if (array_type == Mesh::ARRAY_WEIGHTS) {
                    PackedFloat32Array weights;
                    weights.resize(total_vertices * 4);
                    combined_arrays[array_type] = weights;
                }
            }
        }

        // Combine arrays
        int vertex_offset = 0;
        for (const Array &arrays : arrays_list) {
            if (arrays[Mesh::ARRAY_VERTEX].get_type() == Variant::NIL) continue;

            PackedVector3Array vertices = arrays[Mesh::ARRAY_VERTEX];
            int vert_count = vertices.size();

            // Copy vertices
            PackedVector3Array combined_vertices = combined_arrays[Mesh::ARRAY_VERTEX];
            for (int v = 0; v < vert_count; v++) {
                combined_vertices.write[vertex_offset + v] = vertices[v];
            }
            combined_arrays[Mesh::ARRAY_VERTEX] = combined_vertices;

            // Copy other arrays
            for (int array_type = 0; array_type < Mesh::ARRAY_MAX; array_type++) {
                if (array_type == Mesh::ARRAY_VERTEX || array_type == Mesh::ARRAY_INDEX) continue;

                if (arrays[array_type].get_type() == Variant::NIL) continue;

                if (array_type == Mesh::ARRAY_NORMAL || array_type == Mesh::ARRAY_COLOR || 
                    array_type == Mesh::ARRAY_TEX_UV || array_type == Mesh::ARRAY_TEX_UV2) {
                    // 1:1 mapping arrays
                    if (array_type == Mesh::ARRAY_NORMAL) {
                        PackedVector3Array normals = arrays[array_type];
                        if (normals.size() == vert_count) {
                            PackedVector3Array combined_normals = combined_arrays[array_type];
                            for (int j = 0; j < vert_count; j++) {
                                combined_normals.write[vertex_offset + j] = normals[j];
                            }
                            combined_arrays[array_type] = combined_normals;
                        }
                    } else if (array_type == Mesh::ARRAY_COLOR) {
                        PackedColorArray colors = arrays[array_type];
                        if (colors.size() == vert_count) {
                            PackedColorArray combined_colors = combined_arrays[array_type];
                            for (int j = 0; j < vert_count; j++) {
                                combined_colors.write[vertex_offset + j] = colors[j];
                            }
                            combined_arrays[array_type] = combined_colors;
                        }
                    } else if (array_type == Mesh::ARRAY_TEX_UV || array_type == Mesh::ARRAY_TEX_UV2) {
                        PackedVector2Array uvs = arrays[array_type];
                        if (uvs.size() == vert_count) {
                            PackedVector2Array combined_uvs = combined_arrays[array_type];
                            for (int j = 0; j < vert_count; j++) {
                                combined_uvs.write[vertex_offset + j] = uvs[j];
                            }
                            combined_arrays[array_type] = combined_uvs;
                        }
                    }
                } else if (array_type == Mesh::ARRAY_TANGENT || 
                          (array_type >= Mesh::ARRAY_CUSTOM0 && array_type <= Mesh::ARRAY_CUSTOM3) ||
                          array_type == Mesh::ARRAY_BONES || array_type == Mesh::ARRAY_WEIGHTS) {
                    // 4:1 mapping arrays
                    PackedFloat32Array source = arrays[array_type];
                    if (source.size() == vert_count * 4) {
                        PackedFloat32Array target = combined_arrays[array_type];
                        for (int j = 0; j < vert_count * 4; j++) {
                            target.write[vertex_offset * 4 + j] = source[j];
                        }
                        combined_arrays[array_type] = target;
                    }
                }
            }

            // Handle indices
            if (arrays[Mesh::ARRAY_INDEX].get_type() != Variant::NIL) {
                PackedInt32Array indices = arrays[Mesh::ARRAY_INDEX];
                PackedInt32Array combined_indices = combined_arrays[Mesh::ARRAY_INDEX];
                for (int j = 0; j < indices.size(); j++) {
                    combined_indices.append(indices[j] + vertex_offset);
                }
                combined_arrays[Mesh::ARRAY_INDEX] = combined_indices;
            }

            vertex_offset += vert_count;
        }

        // Add the combined surface
        r_combined_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, combined_arrays);
        r_combined_mesh->surface_set_material(surface_count, material);
        surface_count++;
    }
}

void EditorYarn3DTools::_transform_vertices(PackedVector3Array &r_vertices, const Transform3D &p_transform) {
    for (int i = 0; i < r_vertices.size(); i++) {
        r_vertices.write[i] = p_transform.xform(r_vertices[i]);
    }
}

void EditorYarn3DTools::_transform_normals(PackedVector3Array &r_normals, const Transform3D &p_transform) {
    Transform3D normal_transform = p_transform.basis.inverse().transposed();
    for (int i = 0; i < r_normals.size(); i++) {
        r_normals.write[i] = normal_transform.xform(r_normals[i]);
    }
}

void EditorYarn3DTools::_export_gltf_file_selected(const String &p_path, Node *p_root) {
    // Create GLTF document and state
    Ref<GLTFDocument> gltf_document;
    gltf_document.instantiate();
    Ref<GLTFState> state;
    state.instantiate();

    // Set export flags
    int32_t flags = 0;
    flags |= EditorSceneFormatImporter::IMPORT_USE_NAMED_SKIN_BINDS;
    state->set_bake_fps(30);

    bool created_fake_root = false;
    if (p_root->get_child_count() == 0) {
        MeshInstance3D *mesh_instance = Object::cast_to<MeshInstance3D>(p_root);
        if (mesh_instance != nullptr) {
            // If the root is a MeshInstance3D, we need to create a temporary new parent node, add a copy of the mesh instance as a child, and use that as the root.
            Node3D *new_parent = memnew(Node3D);
            new_parent->set_name("GLTF_Root");
            auto duplicated = mesh_instance->duplicate();
            p_root->get_parent()->add_child(new_parent);
            new_parent->add_child(duplicated);
            duplicated->set_owner(new_parent);
            p_root = new_parent;
            created_fake_root = true;
        }
    }

    // Export the scene
    Error err = gltf_document->append_from_scene(p_root, state, flags);
    if (err != OK) {
        EditorNode::get_singleton()->show_warning(vformat("Failed to export GLTF scene: %s", itos(err)));
        return;
    }

    // Write to filesystem
    err = gltf_document->write_to_filesystem(state, p_path);
    if (err != OK) {
        EditorNode::get_singleton()->show_warning(vformat("Failed to save GLTF file: %s", itos(err)));
    } else {
        EditorFileSystem::get_singleton()->scan_changes();
    }

    if (created_fake_root) {
        p_root->queue_free();
    }
}

void EditorYarn3DTools::_export_gltf_callback() {
    Array selected = EditorNode::get_singleton()->get_editor_selection()->get_selected_nodes();
    
    if (selected.is_empty()) {
        EditorNode::get_singleton()->show_warning("Please select at least one Node3D to export.");
        return;
    }

    bool found_node_3d = false;
    for (int i = 0; i < selected.size(); i++) {
        Node3D *node_3d = Object::cast_to<Node3D>(selected[i]);
        if (!node_3d) continue;

        found_node_3d = true;

        // Show file dialog to choose save location
        EditorFileDialog *file_dialog = memnew(EditorFileDialog);
        file_dialog->set_title("Save GLTF Scene");
        file_dialog->set_file_mode(EditorFileDialog::FILE_MODE_SAVE_FILE);
        file_dialog->add_filter("*.gltf", "GLTF Scene");
        file_dialog->add_filter("*.glb", "GLTF Binary");
        file_dialog->set_access(EditorFileDialog::ACCESS_FILESYSTEM);
        
        EditorNode::get_singleton()->get_gui_base()->add_child(file_dialog);
        file_dialog->popup_centered_ratio();

        file_dialog->connect("file_selected", callable_mp(this, &EditorYarn3DTools::_export_gltf_file_selected).bind(node_3d));
        break;
    }

    if (!found_node_3d) {
        EditorNode::get_singleton()->show_warning("Please select at least one Node3D to export.");
        return;
    }
}

#endif // TOOLS_ENABLED