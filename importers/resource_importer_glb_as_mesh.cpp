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
    return 1;
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
    r_options->push_back(ImportOption(PropertyInfo(Variant::OBJECT, "surface_material", PROPERTY_HINT_RESOURCE_TYPE, "Material"), Variant()));
}

bool ResourceImporterGLBasMesh::get_option_visibility(const String &p_path, const String &p_option,
                                                      const HashMap<StringName, Variant> &p_options) const {
    return true;
}
TypedArray<ImporterMeshInstance3D> ResourceImporterGLBasMesh::find_first_matched_nodes(const Node *checking_node) {
    if (checking_node == nullptr) return {};
    TypedArray<Node> children = checking_node->get_children();
    TypedArray<ImporterMeshInstance3D> result;
    for (const auto& node_child: children) {
        auto* importer_child = Object::cast_to<ImporterMeshInstance3D>(node_child);
        if (importer_child != nullptr) {
            result.append(importer_child);
            continue;
        }
        auto* node_child_as_node = Object::cast_to<Node>(node_child);
        if (node_child_as_node != nullptr) {
            TypedArray<ImporterMeshInstance3D> recursive_found = find_first_matched_nodes(node_child_as_node);
        	if (!recursive_found.is_empty())
	            result.append_array(recursive_found);
        }
    }
    return result;
}

Error ResourceImporterGLBasMesh::import(const String &p_source_file, const String &p_save_path,
    const HashMap<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files,
    Variant *r_metadata) {
    bool should_save_mesh = p_options.has("save_to_file") ? bool(p_options["save_to_file"]) : false;
    Vector3 replace_scale = p_options.has("scale_mesh") ? Vector3(p_options["scale_mesh"]) : Vector3(0.0,0.0,0.0);
    Vector3 offset = p_options.has("offset_mesh") ? Vector3(p_options["offset_mesh"]) : Vector3(0,0,0);
    bool has_scale_replacement = !replace_scale.is_zero_approx();
    Ref<Material> replace_material;
    if (p_options.has(("surface_material"))) {
        replace_material = p_options["surface_material"];
    }
    bool has_replacement_material = !replace_material.is_null() && replace_material.is_valid();

    ERR_FAIL_COND_V_MSG(!FileAccess::exists(p_source_file), ERR_CANT_OPEN, vformat("Couldn't find GLB file '%s', it may not exist or not be readable.", p_source_file));

    // Use GLTFDocument instead of glTF importer to keep image references.
    Ref<GLTFDocument> gltf;
    gltf.instantiate();
    Ref<GLTFState> state;
    state.instantiate();
    print_verbose(vformat("glTF path: %s", p_source_file));
    Error err = gltf->append_from_file(p_source_file, state);

	ERR_FAIL_COND_V_MSG(err != OK, err, vformat("Couldn't open loading GLB from file '%s', it may not exist or not be readable.", p_source_file));

	ERR_FAIL_COND_V_MSG(gltf.is_null() || !gltf.is_valid(), ERR_CANT_OPEN, vformat("Couldn't open GLB file '%s', it may not exist or not be readable.", p_source_file));

	Node* root_node = gltf->generate_scene(state);
	ERR_FAIL_COND_V_MSG(root_node == nullptr, ERR_CANT_OPEN, vformat("Couldn't find root node in GLB file '%s', it may not exist or not be readable.", p_source_file));

    TypedArray<ImporterMeshInstance3D> found_importers = find_first_matched_nodes(root_node);
    Ref<ArrayMesh> find_mesh;
    int overall_i = 0;
    for (const auto& found_importer: found_importers) {
        auto* importer = Object::cast_to<ImporterMeshInstance3D>(found_importer);
        if (importer != nullptr) {
            Ref<ImporterMesh> imp_mesh = importer->get_mesh();
        	if (imp_mesh.is_null() || !imp_mesh.is_valid()) continue;
        	Ref<ArrayMesh> mesh = imp_mesh->get_mesh();
        	if (mesh.is_null() || !mesh.is_valid()) continue;
			find_mesh = mesh;
			if (has_scale_replacement || !offset.is_zero_approx()) {
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
					for (int surface_i = 0; surface_i < static_cast<int>(find_mesh->get_surface_count()); ++surface_i) {
						Array surface_array = find_mesh->surface_get_arrays(surface_i);
						Vector<Vector3> vertex_to = surface_array[Mesh::ARRAY_VERTEX];
						for (int copy_i = 0; copy_i < static_cast<int>(vertex_to.size()); ++copy_i) {
							const Vector3 new_vertex_pos = (Vector3(vertex_to[copy_i]) * replace_scale) + offset;
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
					// print_line("Surface count ", find_mesh_surface_count," current index ",sindex," replace material ",replace_material);
					find_mesh->surface_set_material(sindex, replace_material);
				}
			}

        	if (!find_mesh.is_null() && find_mesh.is_valid()) {
        		find_mesh->set_name(find_mesh->get_name().replace("Root Scene_",""));
        		if (!should_save_mesh) break;
        		String file_path_save = found_importers.size() == 1 ? vformat("%s.mesh", p_source_file.replace(".glb","").replace(".gltf","")) : vformat("%s_%02d.mesh", p_source_file.replace(".glb","").replace(".gltf",""), overall_i);
        		bool file_already_existed = ResourceLoader::exists(file_path_save);
        		ResourceSaver::save(find_mesh, file_path_save);
#ifdef TOOLS_ENABLED
        		if (file_already_existed) {
        			find_mesh->set_path(file_path_save, true);
        			EditorFileSystem::get_singleton()->update_file(file_path_save);
        		}
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

	return OK;
}

ResourceImporterGLBasMesh::ResourceImporterGLBasMesh() = default;

ResourceImporterGLBasMesh::~ResourceImporterGLBasMesh() = default;

