//
// Created by Daniel on 2024-07-29.
//

#include "resource_importer_glb_as_mesh.h"


String ResourceImporterGLBasMesh::get_importer_name() const {
    return "gltf_as_mesh";
}

String ResourceImporterGLBasMesh::get_visible_name() const {
        return "GLTF as Mesh";
}

void ResourceImporterGLBasMesh::get_recognized_extensions(List<String> *p_extensions) const {
    p_extensions->push_back("gltf");
    p_extensions->push_back("glb");
}

String ResourceImporterGLBasMesh::get_save_extension() const {
    return "mesh";
}

String ResourceImporterGLBasMesh::get_resource_type() const {
    return "ArrayMesh";
}

int ResourceImporterGLBasMesh::get_format_version() const {
    return 2;
}

int ResourceImporterGLBasMesh::get_preset_count() const {
    return 1;
}

String ResourceImporterGLBasMesh::get_preset_name(int p_idx) const {
    return "Default";
}

void ResourceImporterGLBasMesh::get_import_options(const String &p_path, List<ImportOption> *r_options,
                                                   int p_preset) const {
    r_options->push_back(ImportOption(PropertyInfo(Variant::BOOL, "save_to_file"), false));
    r_options->push_back(ImportOption(PropertyInfo(Variant::VECTOR3, "scale_mesh", PROPERTY_HINT_LINK), Vector3(1, 1, 1)));
    r_options->push_back(ImportOption(PropertyInfo(Variant::VECTOR3, "offset_mesh"), Vector3(0, 0, 0)));
    r_options->push_back(ImportOption(PropertyInfo(Variant::VECTOR3, "rotate_mesh"), Vector3(0, 0, 0)));
    r_options->push_back(ImportOption(PropertyInfo(Variant::OBJECT, "optional/surface_material", PROPERTY_HINT_RESOURCE_TYPE, "Material"), Variant()));
    r_options->push_back(ImportOption(PropertyInfo(Variant::BOOL, "optional/auto_smooth"), true));
    r_options->push_back(ImportOption(PropertyInfo(Variant::FLOAT, "optional/auto_smooth_angle", PROPERTY_HINT_RANGE, "0,180,0.1"), 30.0));
    r_options->push_back(ImportOption(PropertyInfo(Variant::BOOL, "optional/optimize_color_only_materials"), false));
}

bool ResourceImporterGLBasMesh::get_option_visibility(const String &p_path, const String &p_option,
                                                      const HashMap<StringName, Variant> &p_options) const {
	return true;
}

TypedArray<ImporterMeshInstance3D> ResourceImporterGLBasMesh::find_first_matched_nodes(const Node *checking_node) {
    if (checking_node == nullptr) return {};
    TypedArray<Node> children = checking_node->get_children();
    TypedArray<ImporterMeshInstance3D> result;
    for (int i = 0; i < static_cast<int>(children.size()); ++i) {
        auto* importer_child = Object::cast_to<ImporterMeshInstance3D>(children[i]);
        if (importer_child != nullptr) {
            result.append(importer_child);
            continue;
        }
        auto* node_child_as_node = Object::cast_to<Node>(children[i]);
        if (node_child_as_node != nullptr) {
            TypedArray<ImporterMeshInstance3D> recursive_found = find_first_matched_nodes(node_child_as_node);
        	if (!recursive_found.is_empty())
	            result.append_array(recursive_found);
        }
    }
    return result;
}

void ResourceImporterGLBasMesh::smooth_normals(Array &p_arrays, float p_angle_threshold, bool p_has_vertex_colors) {
    if (p_arrays[Mesh::ARRAY_VERTEX].get_type() == Variant::NIL ||
        p_arrays[Mesh::ARRAY_NORMAL].get_type() == Variant::NIL ||
        p_arrays[Mesh::ARRAY_INDEX].get_type() == Variant::NIL) {
        return;
    }

    PackedVector3Array vertices = p_arrays[Mesh::ARRAY_VERTEX];
    PackedVector3Array normals = p_arrays[Mesh::ARRAY_NORMAL];
    PackedInt32Array indices = p_arrays[Mesh::ARRAY_INDEX];
    PackedColorArray colors;
    bool has_colors = p_arrays[Mesh::ARRAY_COLOR].get_type() != Variant::NIL;
    if (has_colors) {
        colors = p_arrays[Mesh::ARRAY_COLOR];
    }

    float normal_merge_threshold = Math::cos(Math::deg_to_rad(p_angle_threshold));

    struct VertexKey {
        Vector3 pos;
        Color color;
		bool has_color;
        bool operator==(const VertexKey &other) const {
            return pos.is_equal_approx(other.pos) && (!has_color ||color.is_equal_approx(other.color));
        }
    };

    struct VertexKeyHasher {
        static _FORCE_INLINE_ uint32_t hash(const VertexKey &k) {
			uint32_t h = hash_murmur3_one_real(k.pos.x);
			h = hash_murmur3_one_real(k.pos.y, h);
			h = hash_murmur3_one_real(k.pos.z, h);
			if (k.has_color) {
				h = hash_murmur3_one_real(k.color.r, h);
				h = hash_murmur3_one_real(k.color.g, h);
				h = hash_murmur3_one_real(k.color.b, h);
				h = hash_murmur3_one_real(k.color.a, h);
			}
			return hash_fmix32(h);
        }
    };

    HashMap<VertexKey, Vector<int>, VertexKeyHasher> unique_vertices;
    for (int i = 0; i < vertices.size(); ++i) {
        VertexKey key;
        key.pos = vertices[i];
        key.color = has_colors ? colors[i] : Color();
		key.has_color = has_colors;
        unique_vertices[key].push_back(i);
    }

    PackedVector3Array new_normals;
    new_normals.resize(vertices.size());

    // For each group of unique vertices
    for (const KeyValue<VertexKey, Vector<int>> &E : unique_vertices) {
        const Vector<int> &group = E.value;
        for (int i = 0; i < group.size(); ++i) {
            int idx = group[i];
            Vector3 accum = Vector3();
            int count = 0;
            for (int j = 0; j < group.size(); ++j) {
                int other_idx = group[j];
                float dot = normals[idx].dot(normals[other_idx]);
                if (dot >= normal_merge_threshold) {
                    accum += normals[other_idx];
                    count++;
                }
            }
            if (count > 0) {
                new_normals.write[idx] = accum.normalized();
            } else {
                new_normals.write[idx] = normals[idx];
            }
        }
    }
    p_arrays[Mesh::ARRAY_NORMAL] = new_normals;
}

Error ResourceImporterGLBasMesh::import(ResourceUID::ID p_source_id, const String &p_source_file, const String &p_save_path,
    const HashMap<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files,
    Variant *r_metadata) {
    bool should_save_mesh = p_options.has("save_to_file") ? bool(p_options["save_to_file"]) : false;
    Vector3 replace_scale = p_options.has("scale_mesh") ? Vector3(p_options["scale_mesh"]) : Vector3(0.0,0.0,0.0);
    Vector3 offset = p_options.has("offset_mesh") ? Vector3(p_options["offset_mesh"]) : Vector3(0,0,0);
    Vector3 rotate = p_options.has("rotate_mesh") ? Vector3(p_options["rotate_mesh"]) : Vector3(0,0,0);
    bool has_scale_replacement = !replace_scale.is_zero_approx();
    bool has_rotate_replacement = !rotate.is_zero_approx();
    bool optimize_color_only = p_options.has("optional/optimize_color_only_materials") ? bool(p_options["optional/optimize_color_only_materials"]) : false;
    bool auto_smooth = p_options.has("optional/auto_smooth") ? bool(p_options["optional/auto_smooth"]) : true;
    float auto_smooth_angle = p_options.has("optional/auto_smooth_angle") ? float(p_options["optional/auto_smooth_angle"]) : 30.0f;

    Ref<Material> replace_material;
    if (p_options.has(("optional/surface_material"))) {
        replace_material = p_options["optional/surface_material"];
    }
    bool has_replacement_material = !replace_material.is_null() && replace_material.is_valid();

    ERR_FAIL_COND_V_MSG(!FileAccess::exists(p_source_file), ERR_CANT_OPEN, vformat("Couldn't find GLB file '%s', it may not exist or not be readable.", p_source_file));

    // Use GLTFDocument instead of glTF importer to keep image references.
    Ref<GLTFDocument> gltf;
    gltf.instantiate();
    Ref<GLTFState> state;
    state.instantiate();
	
    Error err = gltf->append_from_file(p_source_file, state);

	ERR_FAIL_COND_V_MSG(err != OK, err, vformat("Couldn't open loading GLB from file '%s', it may not exist or not be readable.", p_source_file));

	ERR_FAIL_COND_V_MSG(gltf.is_null() || !gltf.is_valid(), ERR_CANT_OPEN, vformat("Couldn't open GLB file '%s', it may not exist or not be readable.", p_source_file));

	Node* root_node = gltf->generate_scene(state);
	ERR_FAIL_COND_V_MSG(root_node == nullptr, ERR_CANT_OPEN, vformat("Couldn't find root node in GLB file '%s', it may not exist or not be readable.", p_source_file));


	TypedArray<ImporterMeshInstance3D> found_importers = find_first_matched_nodes(root_node);
	
	if (root_node->get_child_count() == 1 && !optimize_color_only) {
		 Ref<ArrayMesh> find_mesh;
		int overall_i = 0;

		for (int i = 0; i < static_cast<int>(found_importers.size()); ++i) {
			auto* importer = Object::cast_to<ImporterMeshInstance3D>(found_importers[i]);
			if (importer != nullptr) {

			Ref<ImporterMesh> imp_mesh = importer->get_mesh();
			if (imp_mesh.is_null() || !imp_mesh.is_valid()) continue;

			Ref<ArrayMesh> mesh = imp_mesh->get_mesh();
			if (mesh.is_null() || !mesh.is_valid()) continue;
				find_mesh = mesh;

				if (has_scale_replacement || !offset.is_zero_approx() || has_rotate_replacement) {
							Ref<ArrayMesh> new_mesh;
							new_mesh.instantiate();
							if (should_save_mesh) {
								String _file_path_save = found_importers.size() ? vformat("%s.mesh",p_source_file.replace(".glb","").replace(".gltf","")) : vformat("%s_%02d.mesh", p_source_file.replace(".glb","").replace(".gltf",""), overall_i);
								if (ResourceLoader::exists(_file_path_save)) {
									new_mesh = ResourceLoader::load(_file_path_save,"ArrayMesh");
									if (!new_mesh.is_null() && new_mesh.is_valid()) {
										new_mesh->clear_surfaces();
									} else {
										new_mesh.instantiate();
									}
								}
							}
							if (!new_mesh.is_null() && new_mesh.is_valid()) {
								Transform3D transform = has_rotate_replacement ? Transform3D(Basis(Quaternion::from_euler(rotate))) : Transform3D();
								for (int surface_i = 0; surface_i < static_cast<int>(find_mesh->get_surface_count()); ++surface_i) {
									Array surface_array = find_mesh->surface_get_arrays(surface_i);
									Vector<Vector3> vertex_to = surface_array[Mesh::ARRAY_VERTEX];
									for (int copy_i = 0; copy_i < static_cast<int>(vertex_to.size()); ++copy_i) {
										const Vector3 new_vertex_pos = !has_rotate_replacement ? ((Vector3(vertex_to[copy_i]) * replace_scale) + offset) : (transform.xform(((Vector3(vertex_to[copy_i]) * replace_scale) + offset)));
										vertex_to.write[copy_i] = new_vertex_pos;
							}
									surface_array[Mesh::ARRAY_VERTEX] = vertex_to;
									new_mesh->add_surface_from_arrays(find_mesh->surface_get_primitive_type(surface_i), surface_array);
						}
						if (!has_replacement_material || !replace_material.is_valid()) {
							const int _mesh_surface_count =  static_cast<int>(find_mesh->get_surface_count());
							for (int _surf_index = 0; _surf_index < _mesh_surface_count; ++_surf_index) {
								new_mesh->surface_set_material(_surf_index, find_mesh->surface_get_material(_surf_index));
								new_mesh->surface_set_name(_surf_index, find_mesh->surface_get_name(_surf_index));
							}
						}
						new_mesh->set_name(!find_mesh.is_null() && find_mesh.is_valid() ? find_mesh->get_name() : static_cast<String>(root_node->get_name()));
						find_mesh = new_mesh;
					}
				}
				if (has_replacement_material && replace_material.is_valid() &&  !find_mesh.is_null() && find_mesh.is_valid()) {
					const int find_mesh_surface_count =  static_cast<int>(find_mesh->get_surface_count());
					for (int sindex = 0; sindex < find_mesh_surface_count; ++sindex) {
						find_mesh->surface_set_material(sindex, replace_material);
					}
				}
				if (!find_mesh.is_null() && find_mesh.is_valid()) {
					find_mesh->set_name(find_mesh->get_name().replace("Root Scene_",""));
					if (!should_save_mesh) break;
					String file_path_save = found_importers.size() == 1 ? vformat("%s.mesh", p_source_file.replace(".glb","").replace(".gltf","")) : vformat("%s_%02d.mesh", p_source_file.replace(".glb","").replace(".gltf",""), overall_i);

					#ifdef TOOLS_ENABLED
						bool file_already_existed = ResourceLoader::exists(file_path_save);
						ResourceSaver::save(find_mesh, file_path_save);
						if (file_already_existed) {
							find_mesh->set_path(file_path_save, true);
							EditorFileSystem::get_singleton()->update_file(file_path_save);
						}
					#else
						ResourceSaver::save(find_mesh, file_path_save);
					#endif
				}
				overall_i += 1;
			}
		}

		root_node->queue_free();
		String filename = p_save_path + "." + get_save_extension();
		if (!find_mesh.is_null() && find_mesh.is_valid()) {
			return ResourceSaver::save(find_mesh, filename);
		}
	} else {
		// Create a new mesh to combine all meshes into
		Ref<ArrayMesh> combined_mesh;
		combined_mesh.instantiate();
		

		// Dictionary to store surfaces by material and attribute set
		HashMap<String, Dictionary> surfaces_by_material_and_attributes;
		
		// Process each mesh
		for (int i = 0; i < static_cast<int>(found_importers.size()); ++i) {
			auto* importer = Object::cast_to<ImporterMeshInstance3D>(found_importers[i]);
			if (importer == nullptr) continue;
			
			Ref<ImporterMesh> imp_mesh = importer->get_mesh();
			if (imp_mesh.is_null() || !imp_mesh.is_valid()) continue;
			
			Ref<ArrayMesh> mesh = imp_mesh->get_mesh();
			if (mesh.is_null() || !mesh.is_valid()) continue;
			
			// Get the mesh's transform from the scene
			Transform3D mesh_transform = importer->get_transform();
			
			// Process each surface in the mesh
			for (int surface_i = 0; surface_i < static_cast<int>(mesh->get_surface_count()); ++surface_i) {
				Array surface_array = mesh->surface_get_arrays(surface_i);
				if (surface_array.is_empty()) continue;
				
				Ref<Material> material = mesh->surface_get_material(surface_i);
				
				// Transform vertices using the mesh's transform
				PackedVector3Array vertices = surface_array[Mesh::ARRAY_VERTEX];
				if (vertices.is_empty()) continue;

				// Handle color-only material optimization
				if (optimize_color_only) {
					Ref<StandardMaterial3D> standard_material = material;
					
					if (standard_material.is_valid()) {
						Color albedo_color = standard_material->get_albedo();
						PackedColorArray colors;
						colors.resize(vertices.size());
						for (int c = 0; c < vertices.size(); c++) {
							colors.write[c] = albedo_color;
						}
						surface_array[Mesh::ARRAY_COLOR] = colors;
					}
				}
				
				for (int v = 0; v < vertices.size(); v++) {
					Vector3 vertex = vertices[v];
					// Apply the mesh's transform first
					vertex = mesh_transform.xform(vertex);
					// Then apply any additional transforms from import options
					if (has_scale_replacement || !offset.is_zero_approx() || has_rotate_replacement) {
						Transform3D additional_transform = Transform3D();
						if (has_rotate_replacement) {
							additional_transform = Transform3D(Basis(Quaternion::from_euler(rotate)));
						}
						vertex = !has_rotate_replacement ? 
							((vertex * replace_scale) + offset) : 
							(additional_transform.xform(((vertex * replace_scale) + offset)));
					}
					vertices.write[v] = vertex;
				}
				surface_array[Mesh::ARRAY_VERTEX] = vertices;
				
				// Transform normals if they exist
				if (surface_array[Mesh::ARRAY_NORMAL].get_type() != Variant::NIL) {
					PackedVector3Array normals = surface_array[Mesh::ARRAY_NORMAL];
					if (normals.size() == vertices.size()) {
						Transform3D normal_transform = mesh_transform.basis.inverse().transposed();
						if (has_rotate_replacement) {
							normal_transform = normal_transform * Transform3D(Basis(Quaternion::from_euler(rotate))).basis.inverse().transposed();
						}
						for (int n = 0; n < normals.size(); n++) {
							normals.write[n] = normal_transform.xform(normals[n]);
						}
						surface_array[Mesh::ARRAY_NORMAL] = normals;
					}
				}
				
				// Create a key that includes both material and attribute set
				String material_key = optimize_color_only ? "combined" : (material.is_valid() ? String::num_uint64(material->get_instance_id()) : "null");
				String attribute_key;
				for (int array_type = 0; array_type < Mesh::ARRAY_MAX; array_type++) {
					if (surface_array[array_type].get_type() != Variant::NIL) {
						attribute_key += String::num_int64(array_type) + ",";
					}
				}
				
				String combined_key = material_key + ":" + attribute_key;
				
				if (!surfaces_by_material_and_attributes.has(combined_key)) {
					Dictionary surface_data;
					surface_data["material"] = material;
					Array arrays;
					surface_data["arrays"] = arrays;
					surfaces_by_material_and_attributes[combined_key] = surface_data;
				}
				
				Variant surface_data_variant = surfaces_by_material_and_attributes[combined_key];
				Dictionary surface_data = surface_data_variant;
				Array arrays = surface_data["arrays"];
				arrays.push_back(surface_array);
				surface_data["arrays"] = arrays;
				surfaces_by_material_and_attributes[combined_key] = surface_data;
			}
		}
		
		// Combine surfaces by material and attributes
		int surface_count = 0;
		for (const KeyValue<String, Dictionary> &E : surfaces_by_material_and_attributes) {
			const Dictionary &surface_data = E.value;
			Ref<Material> material = surface_data["material"];
			Array arrays_list = surface_data["arrays"];
			
			if (arrays_list.is_empty()) continue;
			
			// Calculate total vertices
			int total_vertices = 0;
			for (int i = 0; i < arrays_list.size(); i++) {
				Array arrays = arrays_list[i];
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
				if (arrays_list[0].get(array_type).get_type() != Variant::NIL) {
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
			for (int i = 0; i < arrays_list.size(); i++) {
				Array arrays = arrays_list[i];
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
						PackedVector3Array source_array;
						int source_size = 0;
						
						switch (array_type) {
							case Mesh::ARRAY_NORMAL: {
								source_array = arrays[array_type];
								source_size = source_array.size();
								if (source_size == vert_count) {
									PackedVector3Array combined_normals = combined_arrays[array_type];
									for (int j = 0; j < vert_count; j++) {
										combined_normals.write[vertex_offset + j] = source_array[j];
									}
									combined_arrays[array_type] = combined_normals;
								}
							} break;
							case Mesh::ARRAY_COLOR: {
								PackedColorArray colors = arrays[array_type];
								source_size = colors.size();
								if (source_size == vert_count) {
									PackedColorArray combined_colors = combined_arrays[array_type];
									for (int j = 0; j < vert_count; j++) {
										combined_colors.write[vertex_offset + j] = colors[j];
									}
									combined_arrays[array_type] = combined_colors;
								}
							} break;
							case Mesh::ARRAY_TEX_UV:
							case Mesh::ARRAY_TEX_UV2: {
								PackedVector2Array uvs = arrays[array_type];
								source_size = uvs.size();
								if (source_size == vert_count) {
									PackedVector2Array combined_uvs = combined_arrays[array_type];
									for (int j = 0; j < vert_count; j++) {
										combined_uvs.write[vertex_offset + j] = uvs[j];
									}
									combined_arrays[array_type] = combined_uvs;
								}
							} break;
						}
					} else if (array_type == Mesh::ARRAY_TANGENT || 
							(array_type >= Mesh::ARRAY_CUSTOM0 && array_type <= Mesh::ARRAY_CUSTOM3) ||
							array_type == Mesh::ARRAY_BONES || array_type == Mesh::ARRAY_WEIGHTS) {
						// 4:1 mapping arrays
						PackedFloat32Array source = arrays[array_type];
						int source_size = source.size();
						if (source_size == vert_count * 4) {
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
					int indices_size = indices.size();
					for (int j = 0; j < indices_size; j++) {
						combined_indices.append(indices[j] + vertex_offset);
					}
					combined_arrays[Mesh::ARRAY_INDEX] = combined_indices;
				}
				
				vertex_offset += vert_count;
			}
			
			// Add the combined surface
			if (auto_smooth) {
				smooth_normals(combined_arrays, auto_smooth_angle, optimize_color_only);
			}
			combined_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, combined_arrays);
			if (!optimize_color_only) {
				combined_mesh->surface_set_material(surface_count, material);
			}
			surface_count++;
		}
		
		// Apply replacement material if specified
		if (has_replacement_material && replace_material.is_valid()) {
			for (int sindex = 0; sindex < surface_count; ++sindex) {
				combined_mesh->surface_set_material(sindex, replace_material);
			}
		}
		
		// Save the combined mesh
		if (combined_mesh.is_valid()) {
			combined_mesh->set_name(combined_mesh->get_name().replace("Root Scene_",""));
			if (should_save_mesh) {
				String file_path_save = vformat("%s.mesh", p_source_file.replace(".glb","").replace(".gltf",""));
	#ifdef TOOLS_ENABLED
					bool file_already_existed = ResourceLoader::exists(file_path_save);
				ResourceSaver::save(combined_mesh, file_path_save);
					if (file_already_existed) {
					combined_mesh->set_path(file_path_save, true);
						EditorFileSystem::get_singleton()->update_file(file_path_save);
					}
	#else
				ResourceSaver::save(combined_mesh, file_path_save);
	#endif
			}
		}

		root_node->queue_free();
		String filename = p_save_path + "." + get_save_extension();
		if (combined_mesh.is_valid()) {
			return ResourceSaver::save(combined_mesh, filename);
		}
	}

	return OK;
}

ResourceImporterGLBasMesh::ResourceImporterGLBasMesh() {

}

ResourceImporterGLBasMesh::~ResourceImporterGLBasMesh() {

}

