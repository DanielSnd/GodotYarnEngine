//
// Created by Daniel on 2024-02-03.
//

#ifdef TOOLS_ENABLED
#include "AOBakeEditorPlugin.h"

#include "editor/editor_data.h"
#include "editor/editor_node.h"
#include "scene/gui/progress_bar.h"

/////////////////////////////////////////////////////////////////////////////////

class AOBakeEditorButtons : public Control {
	GDCLASS(AOBakeEditorButtons, Control)

	Ref<AOBakeGenerator> ao_bake_generator;
	AOBakeableMeshInstance* _aobakeable = nullptr;
	static const int PREVIEW_HEIGHT = 140;
	static const int PADDING_PREVIEW_INFO = 2;
	VBoxContainer * _vbox = nullptr;
	Button *_bake_AO_button = nullptr;
	Button *_create_mesh_instance_button = nullptr;
	Button *_bake_combine_meshes = nullptr;
	ProgressBar *_progress_bar = nullptr;
	// Button *_3d_space_switch = nullptr;

	struct SurfaceData {
		Vector<bool> surface_is_parent;
		Vector<Node3D *> surface_owner_array;
		Vector<Array> surface_arrays;
		Ref<Material> surface_material;
		String surface_name;
	};
public:
	inline static AOBakeEditorButtons* singleton = nullptr;

	AOBakeEditorButtons() {
		if (singleton == nullptr) {
			singleton = this;
			set_h_size_flags(SIZE_EXPAND_FILL);
			set_v_size_flags(SIZE_EXPAND_FILL);
			set_custom_minimum_size(Size2(0, EDSCALE * (PREVIEW_HEIGHT)));
			_vbox = memnew(VBoxContainer);
			_vbox->set_anchors_preset(LayoutPreset::PRESET_FULL_RECT);
			_vbox->set_h_size_flags(SIZE_EXPAND_FILL);
			_vbox->set_v_size_flags(SIZE_EXPAND_FILL);
			add_child(_vbox);
			_bake_AO_button = memnew(Button);
			_bake_AO_button->set_text(TTR("Bake AO"));
			_bake_AO_button->set_modulate(Color{0.68,0.3,0.72,1.0});
			_bake_AO_button->set_tooltip_text(TTR("Bakes the AO to vertex colors of the mesh."));
			_bake_AO_button->connect("pressed", callable_mp(this, &AOBakeEditorButtons::_on_bake_ao_button_pressed));
			_bake_AO_button->set_h_size_flags(SIZE_EXPAND_FILL);
			_vbox->add_child(_bake_AO_button);
		}
	}

	void create_create_mesh_instance_button() {
		if (_create_mesh_instance_button != nullptr) {
			return;
		}
		_create_mesh_instance_button = memnew(Button);
		_create_mesh_instance_button->set_text(TTR("Create mesh instance"));
		_create_mesh_instance_button->set_modulate(Color{0.68,0.9,0.12,1.0});
		_create_mesh_instance_button->connect("pressed", callable_mp(this, &AOBakeEditorButtons::_on_create_mesh_instance_pressed));
		_create_mesh_instance_button->set_h_size_flags(SIZE_EXPAND_FILL);
		_vbox->add_child(_create_mesh_instance_button);
	}

	void _on_create_mesh_instance_pressed() {
		if (!_aobakeable->baked_mesh.is_valid()) {
			ERR_PRINT("Invalid baked mesh");
			return;
		}
		MeshInstance3D* new_bakeable_mesh = memnew(MeshInstance3D);
		if(_aobakeable->get_parent() != nullptr) {
			_aobakeable->get_parent()->add_child(new_bakeable_mesh);
			new_bakeable_mesh->set_owner(_aobakeable->get_owner());
			_aobakeable->get_parent()->move_child(new_bakeable_mesh,_aobakeable->get_index());
		} else {
			_aobakeable->add_child(new_bakeable_mesh);
			new_bakeable_mesh->set_owner(_aobakeable);
		}
		for (int i = 0; i < _aobakeable->get_surface_override_material_count(); ++i) {
			if(_aobakeable->get_surface_override_material(i) != nullptr) {
				new_bakeable_mesh->set_surface_override_material(i,_aobakeable->get_surface_override_material(i));
			}
		}
		new_bakeable_mesh->set_mesh(_aobakeable->baked_mesh);
		new_bakeable_mesh->set_name(_aobakeable->get_name());
		new_bakeable_mesh->set_global_position(_aobakeable->get_global_position());
		new_bakeable_mesh->set_global_rotation(_aobakeable->get_global_rotation());
		new_bakeable_mesh->set_material_override(_aobakeable->get_material_override());
		new_bakeable_mesh->set_material_overlay(_aobakeable->get_material_overlay());
	}
	void wait_another_frame() {
		if (!SceneTree::get_singleton()->is_connected(SNAME("process_frame"),callable_mp(this,&AOBakeEditorButtons::waiting_on_generate))) {
			SceneTree::get_singleton()->connect(SNAME("process_frame"),callable_mp(this,&AOBakeEditorButtons::waiting_on_generate), CONNECT_ONE_SHOT);
		}
	}

	void add_to_surfacedatas(Vector<SurfaceData> &surface_datas, MeshInstance3D &mesh_inst_to_add, const bool is_parent) {
		Ref<ArrayMesh> mesh = mesh_inst_to_add.get_mesh();
		auto* meshnode3d = Object::cast_to<Node3D>(&mesh_inst_to_add);
		//print_line("Adding parent? ",is_parent," has meshnode3d? ",meshnode3d, " Surfaces in this one ",mesh->get_surface_count());
		if (meshnode3d == nullptr || mesh == nullptr) return;
		for (int i = 0; i < mesh->get_surface_count(); ++i) {
			auto surface_arrays = mesh->surface_get_arrays(i);
			Ref<Material> surface_mat = mesh->surface_get_material(i);
			//print_line(i," i has material? ",surface_mat.is_valid()," ", surface_mat.is_valid() ? vformat(" %d",surface_mat->get_instance_id()) :" null"," surface override material count ",mesh_inst_to_add.get_surface_override_material_count());
			if (mesh_inst_to_add.get_surface_override_material_count() > i && mesh_inst_to_add.get_surface_override_material(i) != nullptr
				&& !mesh_inst_to_add.get_surface_override_material(i).is_null() && mesh_inst_to_add.get_surface_override_material(i).is_valid()) {
				surface_mat = mesh_inst_to_add.get_surface_override_material(i);
			}
			//print_line(i," i has material afterwards? ",surface_mat.is_valid()," ", surface_mat.is_valid() ? vformat(" %d",surface_mat->get_instance_id())  :" null");
			SurfaceData* surface_data = nullptr;
			const auto material_instance_id = surface_mat->get_instance_id();

			bool is_new_one_created = false;
			for (int j = 0; j < surface_datas.size(); ++j) {
				//print_line("Surface datas size ",surface_datas.size()," iterating ",j," do I have surface mat? ",
				//	surface_mat.is_valid()," does this one have surface material? ",surface_datas[j].surface_material.is_valid(),
				//	( surface_datas[j].surface_material.is_valid() ? vformat(" %d",surface_datas[j].surface_material->get_instance_id())  :" null"));
				if (surface_datas[j].surface_material != nullptr && surface_datas[j].surface_material.is_valid()) {
					if (surface_datas[j].surface_material->get_instance_id() == material_instance_id) {
						surface_data = &surface_datas.write[j];
						//print_line("Found surface data in slot",j);
						break;
					}
				} else if (surface_datas[j].surface_name == mesh->surface_get_name(i)) {
					surface_data = &surface_datas.write[j];
					break;
				}
			}
			if (surface_data == nullptr) {
				SurfaceData new_surface_data = SurfaceData{};
				surface_data = &new_surface_data;
				surface_data->surface_name = mesh->surface_get_name(i);
				//print_line("Setting surface name ",surface_data->surface_name," to ",mesh->surface_get_name(i));
				//print_line("Did not find surface data, created a new one at",surface_datas.size()-1);
				is_new_one_created = true;
			}
			//print_line("Before me appending surface arrays had ",surface_data->surface_arrays.size());
			surface_data->surface_arrays.append(surface_arrays);
			surface_data->surface_owner_array.append(meshnode3d);
			surface_data->surface_is_parent.append(is_parent);
			if (!surface_data->surface_material.is_valid() && surface_mat.is_valid())
				surface_data->surface_material = surface_mat;
			if (surface_data->surface_name.is_empty())
				surface_data->surface_name = mesh->surface_get_name(i);
			//print_line("After me appending surface arrays has ",surface_data->surface_arrays.size());
			if (is_new_one_created) {
				surface_datas.append(*surface_data);
				//print_line("Created new. My surface data material ",
				//	(surface_data->surface_material.is_valid() ? vformat(" %d",surface_data->surface_material->get_instance_id())  :" null")," the one registered ",
				//	( surface_datas[surface_datas.size()-1].surface_material.is_valid() ? vformat(" %d",surface_datas[surface_datas.size()-1].surface_material->get_instance_id())  :" null"));
			}
		}
	}

	void _on_bake_combine_meshes_pressed() {
		if(_aobakeable->get_child_count() <= 0) return;
		TypedArray<Node> children = _aobakeable->get_children();
		Vector<SurfaceData> surface_datas;

		if (_aobakeable->get_mesh() != nullptr) {
			auto* aobakeable = Object::cast_to<MeshInstance3D>(_aobakeable);
			//print_line("AOBakeable casted is null? ",aobakeable == nullptr);
			if (aobakeable != nullptr) {
				add_to_surfacedatas(surface_datas,*aobakeable,true);
			}
		}
		for (int i = 0; i < children.size(); i++) {
			auto* child = Object::cast_to<MeshInstance3D>(children[i].operator Object*());
			//print_line("Casted child to meshinstance3d... is null? ",child!=nullptr);
			if (child == nullptr) { continue; }
			add_to_surfacedatas(surface_datas,*child,false);
		}

		Ref<ArrayMesh> new_mesh;
		new_mesh.instantiate();
		//print_line(surface_datas.size(),"amount of surface datas");
		for (int s = 0; s < surface_datas.size(); ++s) {
			auto surface_data_info = surface_datas[s];
			Array new_mesh_arrays;
			new_mesh_arrays.resize(Mesh::ARRAY_MAX);
			//print_line(surface_data_info.surface_arrays.size()," amount of surface arrays in surface data ",s);
			for (int i = 0; i < surface_data_info.surface_arrays.size(); ++i) {
				Array surface_array = surface_data_info.surface_arrays[i];
				//print_line("Surface ",s," Doing index ",i," surface array owner ",surface_data_info.surface_owner_array[i]," is parent? ",surface_data_info.surface_is_parent[i]);
				combine_surface_arrays(surface_array,surface_data_info.surface_owner_array[i],new_mesh_arrays,surface_data_info.surface_is_parent[i]);
			}
			new_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES,new_mesh_arrays);
			new_mesh->surface_set_material(s,surface_data_info.surface_material);
			new_mesh->surface_set_name(s,surface_data_info.surface_name);
			if (new_mesh->get_name().is_empty())
				new_mesh->set_name(vformat("Combined %s",surface_data_info.surface_owner_array[s]->get_name()));
		}

		_aobakeable->baked_mesh = new_mesh;
		if (_aobakeable->auto_set_mesh_from_bake) {
			_aobakeable->set_mesh(_aobakeable->baked_mesh);

			for (int i = 0; i < children.size(); i++) {
				auto* child = Object::cast_to<MeshInstance3D>(children[i].operator Object*());
				if (child == nullptr) { continue; }
				child->set_visible(false);
			}

			create_create_mesh_instance_button();
		}
	}

	void combine_surface_arrays(Array &mesh_arrays_from, Node3D* from_obj, Array &mesh_arrays_to,bool is_parent=false) {
		Vector<Vector3> vertex_from = mesh_arrays_from[Mesh::ARRAY_VERTEX];
		Vector<Vector3> vertex_to = mesh_arrays_to[Mesh::ARRAY_VERTEX];
		int index_shift = vertex_to.size();
		if(!is_parent) {
			for (int i = 0; i < vertex_from.size(); ++i) {
				vertex_from.write[i] = from_obj->get_transform().xform(vertex_from[i]);
			}
		}
		vertex_to.append_array(vertex_from);
		mesh_arrays_to[Mesh::ARRAY_VERTEX] = vertex_to;

		Vector<Vector3> normal_from = mesh_arrays_from[Mesh::ARRAY_NORMAL];
		Vector<Vector3> normal_to = mesh_arrays_to[Mesh::ARRAY_NORMAL];
		if(!is_parent) {
			for (int i = 0; i < normal_from.size(); ++i) {
					normal_from.write[i] = from_obj->get_basis().xform(normal_from[i]);
			}
		}
		normal_to.append_array(normal_from);
		mesh_arrays_to[Mesh::ARRAY_NORMAL] = normal_to;
		
		Vector<Color> color_from = mesh_arrays_from[Mesh::ARRAY_COLOR];
		Vector<Color> color_to = mesh_arrays_to[Mesh::ARRAY_COLOR];
		color_to.append_array(color_from);
		mesh_arrays_to[Mesh::ARRAY_COLOR] = color_to;

		Vector<Vector2> uv_from = mesh_arrays_from[Mesh::ARRAY_TEX_UV];
		Vector<Vector2> uv_to = mesh_arrays_to[Mesh::ARRAY_TEX_UV];
		uv_to.append_array(uv_from);
		mesh_arrays_to[Mesh::ARRAY_TEX_UV] = uv_to;
		
		Vector<Vector3> tangent_from = mesh_arrays_from[Mesh::ARRAY_TANGENT];
		Vector<Vector3> tangent_to = mesh_arrays_to[Mesh::ARRAY_TANGENT];
		tangent_to.append_array(tangent_from);
		mesh_arrays_to[Mesh::ARRAY_TANGENT] = tangent_to;
		
		Vector<Vector2> uv2_from = mesh_arrays_from[Mesh::ARRAY_TEX_UV2];
		Vector<Vector2> uv2_to = mesh_arrays_to[Mesh::ARRAY_TEX_UV2];
		uv2_to.append_array(uv2_from);
		mesh_arrays_to[Mesh::ARRAY_TEX_UV2] = uv2_to;

		Vector<Vector3> custom_from = mesh_arrays_from[Mesh::ARRAY_CUSTOM0];
		Vector<Vector3> custom_to = mesh_arrays_to[Mesh::ARRAY_CUSTOM0];
		custom_to.append_array(custom_from);
		mesh_arrays_to[Mesh::ARRAY_CUSTOM0] = custom_to;
		
		Vector<Vector3> custom1_from = mesh_arrays_from[Mesh::ARRAY_CUSTOM1];
		Vector<Vector3> custom1_to = mesh_arrays_to[Mesh::ARRAY_CUSTOM1];
		custom1_to.append_array(custom1_from);
		mesh_arrays_to[Mesh::ARRAY_CUSTOM1] = custom1_to;
		
		Vector<int> index_from = mesh_arrays_from[Mesh::ARRAY_INDEX];
		for (int i = 0; i < index_from.size(); ++i) {
			index_from.write[i] = index_from[i] + index_shift;
		}
		Vector<int> index_to = mesh_arrays_to[Mesh::ARRAY_INDEX];
		int indexes_before = index_to.size();
		index_to.append_array(index_from);
		mesh_arrays_to[Mesh::ARRAY_INDEX] = index_to;
		Vector<int> new_index_to = mesh_arrays_to[Mesh::ARRAY_INDEX];
		int indexes_after = new_index_to.size();

		// ARRAY_TANGENT = RenderingServer::ARRAY_TANGENT,
		// ARRAY_COLOR = RenderingServer::ARRAY_COLOR,
		// ARRAY_TEX_UV = RenderingServer::ARRAY_TEX_UV,
		// ARRAY_TEX_UV2 = RenderingServer::ARRAY_TEX_UV2,
		// ARRAY_CUSTOM0 = RenderingServer::ARRAY_CUSTOM0,
		// ARRAY_CUSTOM1 = RenderingServer::ARRAY_CUSTOM1,
		// ARRAY_CUSTOM2 = RenderingServer::ARRAY_CUSTOM2,
		// ARRAY_CUSTOM3 = RenderingServer::ARRAY_CUSTOM3,
		// ARRAY_BONES = RenderingServer::ARRAY_BONES,
		// ARRAY_WEIGHTS = RenderingServer::ARRAY_WEIGHTS,
		// ARRAY_INDEX = RenderingServer::ARRAY_INDEX,
		// ARRAY_MAX = RenderingServer::ARRAY_MAX
	}

	void waiting_on_generate() {
		if (ao_bake_generator == nullptr) {
			return;
		}
		if (ao_bake_generator->is_generating()) {
			if (_progress_bar == nullptr) {
				_progress_bar = memnew(ProgressBar);
				_progress_bar->set_custom_minimum_size(Size2(128, 15));
				_progress_bar->set_max(ao_bake_generator->all_vertices);
				_progress_bar->set_min(0);
				_vbox->add_child(_progress_bar);
				_progress_bar->set_value(0);
			}
			if (_progress_bar != nullptr) {
				if (_progress_bar->get_max() < ao_bake_generator->all_vertices) {
					_progress_bar->set_max(ao_bake_generator->all_vertices);
				}
				_progress_bar->set_value(ao_bake_generator->current_vertex);
				_bake_AO_button->set_text(vformat("[%d/%d] %d / %d",ao_bake_generator->current_surface,ao_bake_generator->all_surfaces,ao_bake_generator->current_vertex,ao_bake_generator->all_vertices));
			}
		} else {
			if (_progress_bar != nullptr) {
				_progress_bar->queue_free();
				_progress_bar = nullptr;
				_bake_AO_button->set_text("Bake AO");
			}
		}
		if (ao_bake_generator->is_generating() || ao_bake_generator->is_generation_pending()) {
			if (!SceneTree::get_singleton()->is_connected(SNAME("process_frame"),callable_mp(this,&AOBakeEditorButtons::wait_another_frame))) {
				SceneTree::get_singleton()->connect(SNAME("process_frame"),callable_mp(this,&AOBakeEditorButtons::wait_another_frame), CONNECT_ONE_SHOT);
			}
		}
		else if (!ao_bake_generator->is_generating() && !ao_bake_generator->is_generation_pending()) {
			finished_generation();
		}
	}

	void _on_bake_ao_button_pressed() {
		//print_line("Pressed bake ao button ",_aobakeable);
		if (ao_bake_generator.is_null() || !ao_bake_generator.is_valid()) {
			ao_bake_generator.instantiate();
		}
		if (ao_bake_generator->is_generating()) {
			ERR_PRINT("WAS ALREADY GENERATING");
			return;
		}

		if (_aobakeable == nullptr) {
			ERR_PRINT("INVALID AO BAKEABLE");
			return;
		}
		Ref<Mesh> mesh_to_do = _aobakeable->get_mesh();
		if(!mesh_to_do.is_valid()) {
			ERR_PRINT("INVALID MESH");
			return;
		}
		Ref<Texture2D> generated_small;
		Dictionary d;
		_bake_AO_button->set_disabled(true);
		//print_line("About to call ao bake generator ",ao_bake_generator);
		ao_bake_generator->connect(SNAME("finished_gen"),callable_mp(const_cast<AOBakeEditorButtons *>(this), &AOBakeEditorButtons::finished_generation), CONNECT_ONE_SHOT);
		ao_bake_generator->mesh_to_generate = mesh_to_do;
		ao_bake_generator->queue_generation(mesh_to_do,_aobakeable);
		//AOBakeEditorPlugin::get_singleton()->queue_generator(ao_bake_generator);
		SceneTree::get_singleton()->connect(SNAME("process_frame"),callable_mp(this,&AOBakeEditorButtons::waiting_on_generate), CONNECT_ONE_SHOT);
	}

	void finished_generation() {
		//print_line("After AO bake generator ");
		if (ao_bake_generator.is_valid()) {
			_aobakeable->set_baked_mesh(ao_bake_generator->last_baked_mesh);
			_bake_AO_button->set_disabled(false);
		}
	}

	void set_aobakeable(AOBakeableMeshInstance* p_aobakeable) {
		if (_aobakeable!=nullptr && _aobakeable == p_aobakeable) {
			return;
		}
		_aobakeable = p_aobakeable;
		if (_aobakeable->baked_mesh.is_valid()) {
			create_create_mesh_instance_button();
		}
		if(_aobakeable->get_child_count() > 0) {
			TypedArray<Node> children = _aobakeable->get_children();
			for (int i = 0; i < children.size(); i++) {
				MeshInstance3D* child = Object::cast_to<MeshInstance3D>(children[i].operator Object*());
				//print_line("Casted child to meshinstance3d... is null? ",child!=nullptr);
				if (child != nullptr) {
					_bake_combine_meshes = memnew(Button);
					_bake_combine_meshes->set_text(TTR("Combine meshes"));
					_bake_combine_meshes->set_modulate(Color{0.3,0.2,0.8,1.0});
					_bake_combine_meshes->connect("pressed", callable_mp(this, &AOBakeEditorButtons::_on_bake_combine_meshes_pressed));
					_bake_combine_meshes->set_h_size_flags(SIZE_EXPAND_FILL);
					_vbox->add_child(_bake_combine_meshes);

					//print_line("Creating new button ... created? ",_bake_combine_meshes != nullptr);
					break;
				}
			}
		}
	}

private:
		void _notification(int p_what) {
		switch (p_what) {
			case NOTIFICATION_RESIZED: {
			} break;
			case NOTIFICATION_EXIT_TREE: {
				if (ao_bake_generator.is_valid()) {
					ao_bake_generator.unref();
				}
				if (singleton != nullptr && singleton == this) {
					singleton = nullptr;
				}
			} break;
		}
	}
};

/////////////////////////////////////////////////////////////////////////////////

class AOBakeInspectorPlugin : public EditorInspectorPlugin {
    GDCLASS(AOBakeInspectorPlugin, EditorInspectorPlugin)
public:
    bool can_handle(Object *p_object) override {
        return Object::cast_to<AOBakeableMeshInstance>(p_object) != nullptr;
    }

    void parse_begin(Object *p_object) override {
        if (auto *aobakeablepointer = Object::cast_to<AOBakeableMeshInstance>(p_object)) {
        	if (AOBakeEditorButtons::singleton == nullptr) {
        		AOBakeEditorButtons *viewer = memnew(AOBakeEditorButtons);
        		viewer->set_aobakeable(aobakeablepointer);
        		add_custom_control(viewer);
        	}
        }
    }
};

/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////

AOBakeEditorPlugin * AOBakeEditorPlugin::get_singleton() {
	return singleton;
}

AOBakeEditorPlugin::AOBakeEditorPlugin() {
	singleton = this;
    Ref<AOBakeInspectorPlugin> plugin;
    plugin.instantiate();
    add_inspector_plugin(plugin);\
    
//     case NOTIFICATION_ENTER_TREE: {
				// EditorNode::get_singleton()->add_tool_menu_item("Create BakeableMeshInstance", callable_mp(this, &AOBakeEditorPlugin::CreateBakeableMeshInstance));					
			// } break;
    
    // Defer the menu item creation to _enter_tree()
    created_menu_item = false;
}

void AOBakeEditorPlugin::CreateBakeableMeshInstance() {
	EditorSelection *editor_selection = EditorNode::get_singleton()->get_editor_selection();
	MeshInstance3D* converting_node = nullptr;

	Array selected_node_list = editor_selection->get_selected_nodes();
	ERR_FAIL_COND(selected_node_list.size() == 0);

	converting_node = Object::cast_to<MeshInstance3D>(selected_node_list[0]);
	if (converting_node != nullptr) {
		AOBakeableMeshInstance* new_bakeable_mesh = memnew(AOBakeableMeshInstance);
		if(converting_node->get_parent() != nullptr) {
			converting_node->get_parent()->add_child(new_bakeable_mesh);
			new_bakeable_mesh->set_owner(converting_node->get_owner());
			converting_node->get_parent()->move_child(new_bakeable_mesh,converting_node->get_index());
		} else {
			converting_node->add_child(new_bakeable_mesh);
			new_bakeable_mesh->set_owner(converting_node);
		}
		new_bakeable_mesh->set_mesh(converting_node->get_mesh());
		for (int i = 0; i < converting_node->get_surface_override_material_count(); ++i) {
			if(converting_node->get_surface_override_material(i) != nullptr) {
				new_bakeable_mesh->set_surface_override_material(i,converting_node->get_surface_override_material(i));
			}
		}
		new_bakeable_mesh->set_name(converting_node->get_name());
		new_bakeable_mesh->set_global_position(converting_node->get_global_position());
		new_bakeable_mesh->set_global_rotation(converting_node->get_global_rotation());
		new_bakeable_mesh->set_material_override(converting_node->get_material_override());
		new_bakeable_mesh->set_material_overlay(converting_node->get_material_overlay());
	}
}

AOBakeEditorPlugin::~AOBakeEditorPlugin() {
	if(singleton != nullptr && singleton == this) {
		if (created_menu_item)
			EditorPlugin::remove_tool_menu_item("Create BakeableMeshInstance");
		singleton = nullptr;
	}
}

#endif //TOOLS_ENABLED