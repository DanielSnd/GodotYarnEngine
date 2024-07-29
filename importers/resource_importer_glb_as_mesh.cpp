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
            result.append_array(recursive_found);
        }
    }
    return result;
}

Error ResourceImporterGLBasMesh::import(const String &p_source_file, const String &p_save_path,
    const HashMap<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files,
    Variant *r_metadata) {
    bool should_save_mesh = p_options.has("save_to_file") ? bool(p_options["save_to_file"]) : false;
    Vector3 replace_scale = p_options.has("scale") ? Vector3(p_options["scale_mesh"]) : Vector3(0.0,0.0,0.0);
    Vector3 offset = p_options.has("offset") ? Vector3(p_options["offset"]) : Vector3(0,0,0);
    bool has_scale_replacement = !replace_scale.is_zero_approx();
    Ref<Material> replace_material;
    if (p_options.has(("surface_material"))) {
        replace_material = p_options["surface_material"];
    }
    bool has_replacement_material = !replace_material.is_null() && replace_material.is_valid();

    Ref<FileAccess> f = FileAccess::open(p_source_file, FileAccess::READ);
    ERR_FAIL_COND_V_MSG(f.is_null(), ERR_CANT_OPEN, vformat("Couldn't open GLB file '%s', it may not exist or not be readable.", p_source_file));

    // Use GLTFDocument instead of glTF importer to keep image references.
    Ref<GLTFDocument> gltf;
    gltf.instantiate();
    Ref<GLTFState> state;
    state.instantiate();
    print_verbose(vformat("glTF path: %s", p_source_file));
    Error err = gltf->append_from_file(p_source_file, state);
    if (err != OK) {
        return ERR_BUG;
    }

    auto root_node = gltf->generate_scene(state);
    TypedArray<ImporterMeshInstance3D> found_importers = find_first_matched_nodes(root_node);
    Ref<ArrayMesh> find_mesh;
    int i = 0;

    for (auto found_importer: found_importers) {
        auto* importer = Object::cast_to<ImporterMeshInstance3D>(found_importer);
        if (importer != nullptr) {
            Ref<ArrayMesh> mesh = importer->get_mesh();
			find_mesh = mesh;
			if (has_scale_replacement || !offset.is_zero_approx()) {
				Ref<ArrayMesh> new_mesh;
				new_mesh.instantiate();
				if (should_save_mesh) {
					String _file_path_save = found_importers.size() ? vformat("%s.mesh",p_source_file.replace(".glb","").replace(".gltf","")) : vformat("%s_%02d.mesh", p_source_file.replace(".glb","").replace(".gltf",""), i);
					if (const bool _file_already_existed = ResourceLoader::exists(_file_path_save)) {
						new_mesh = ResourceLoader::load(_file_path_save,"ArrayMesh");
						new_mesh->clear_surfaces();
					}
				}
				for (int surface_i = 0; surface_i < find_mesh->get_surface_count(); ++surface_i) {
					auto arrays = find_mesh->surface_get_arrays(surface_i).duplicate();
					Array array_vertex = arrays[Mesh::ARRAY_VERTEX];
					for (int _vert = 0; _vert < array_vertex.size(); ++_vert) {
						array_vertex[_vert] = (Vector3(array_vertex[_vert]) * replace_scale) + offset;
					}
					arrays[Mesh::ARRAY_VERTEX] = array_vertex;
					new_mesh->add_surface_from_arrays(find_mesh->surface_get_primitive_type(surface_i), arrays);
				}
				if (!has_replacement_material || !replace_material.is_valid()) {
					for (int _surf_index = 0; _surf_index < find_mesh->get_surface_count(); ++_surf_index) {
						new_mesh->surface_set_material(_surf_index, find_mesh->surface_get_material(_surf_index));
						new_mesh->surface_set_name(_surf_index, find_mesh->surface_get_name(_surf_index));
					}
				}
				new_mesh->set_name(find_mesh->get_name());
				find_mesh = new_mesh;
			}
			if (has_replacement_material && replace_material.is_valid()) {
				for (int surf_index = 0; i < find_mesh->get_surface_count(); ++surf_index) {
					find_mesh->surface_set_material(surf_index, replace_material);
				}
			}

			find_mesh->set_name(find_mesh->get_name().replace("Root Scene_",""));
			if (!should_save_mesh) {
				break;
			}
			else {
				String file_path_save = found_importers.size() == 1 ? vformat("%s.mesh", p_source_file.replace(".glb","").replace(".gltf","")) : vformat("%s_%02d.mesh", p_source_file.replace(".glb","").replace(".gltf",""), i);
				bool file_already_existed = ResourceLoader::exists(file_path_save);
				ResourceSaver::save(find_mesh, file_path_save);
#ifdef TOOLS_ENABLED
				if (file_already_existed) {
					find_mesh->set_path(file_path_save, true);
					EditorFileSystem::get_singleton()->update_file(file_path_save);
				}
#endif
			}
			i += 1;
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

