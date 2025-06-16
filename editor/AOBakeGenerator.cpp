#ifdef TOOLS_ENABLED
#include "AOBakeGenerator.h"

#include "../AOBakeableMeshInstance.h"
#include "core/config/project_settings.h"
#include "scene/main/scene_tree.h"
#include "scene/resources/mesh.h"
#include "scene/resources/mesh_data_tool.h"

void AOBakeGenerator::_bind_methods() {
	ADD_SIGNAL(MethodInfo("finished_gen"));
}

bool AOBakeGenerator::is_generating() {
	return generating;
}

Color AOBakeGenerator::find_color_average(const Ref<Image> &img) {
	const auto width = img->get_width();
	const auto height = img->get_height();

	float totalR = 0, totalG = 0, totalB = 0;
	float colors_added = 0;
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			const auto get_color = img->get_pixel(x,y);
			totalR += get_color.r;
			totalG += get_color.g;
			totalB += get_color.b;
			colors_added += 1;
		}
	}

	const float averageR = totalR / colors_added;
	const float averageG = totalG / colors_added;
	const float averageB = totalB / colors_added;
	const float calculateA = 0.2126 * averageR + 0.7152 * averageG + 0.0722 * averageB;
	return Color{averageR,averageG,averageB,calculateA};
}

Ref<ArrayMesh> AOBakeGenerator::get_last_baked_mesh() const {
	return last_baked_mesh;
}

void AOBakeGenerator::set_last_baked_mesh(const Ref<Mesh> &p_mesh) {
	last_baked_mesh = p_mesh;
}

Basis AOBakeGenerator::calculate_basis(Vector3 forward) {
	Vector3 up = {0.0,1.0,0.0};
	// Check if the forward vector is nearly parallel to the world up vector
	if (Math::abs(forward.dot(up)) > 0.999) {
		// If the forward vector is nearly parallel to the world up vector, choose a different up vector
		up = Vector3(1, 0, 0); // Use world right as up vector
	} else {
		// If not parallel, use the world up vector
		up = Vector3(0, 1, 0);
	}
	return Basis::looking_at(forward,up);
}

bool AOBakeGenerator::queue_generation(const Ref<Mesh> &p_from, AOBakeableMeshInstance* aomi) {
	if (p_from.is_null()) {
		ERR_PRINT("Mesh is null");
		generating = false;
		generation_pending = false;
		return false;
	}
	if (aomi->debug_vertex > 0 && aomi->get_parent() != nullptr) {
		TypedArray<Node> children = aomi->get_parent()->get_children();

		for (int i = 0; i < children.size(); i++) {
			Camera3D* child = Object::cast_to<Camera3D>(children[i].operator Object*());
			if (child != nullptr) {
				float use_separation = 1.0;
				Array vertex_array = p_from->surface_get_arrays(0)[Mesh::ARRAY_VERTEX];
				Array normal_array = p_from->surface_get_arrays(0)[Mesh::ARRAY_NORMAL];
				int desired_index = CLAMP(aomi->debug_vertex,0,vertex_array.size()-1);
				const Vector3 vertice_pos = vertex_array[desired_index];
				const Vector3 vertice_normal = normal_array[desired_index];
				const Vector3 viewport_pos = vertice_pos + (vertice_normal * use_separation);
				const Vector3 look_at_forward = viewport_pos.direction_to(vertice_pos).normalized() * -1;
				child->set_transform({calculate_basis(look_at_forward), viewport_pos});
				child->set_perspective(aomi->get_occlusion_angle(),0.001,aomi->get_occlusion_distance());
			}
		}
	}
	set_last_baked_mesh(p_from);
	generation_pending = true;
	RS::get_singleton()->environment_set_bg_color(environment, aomi->get_environment_color());
	occlusion_distance = aomi->get_occlusion_distance();
	occlusion_angle = aomi->get_occlusion_angle();
	for (int i = 0; i < parallel_amount; i++) {
		RS::get_singleton()->camera_set_perspective(cameras[i],aomi->get_occlusion_angle(),0.001,aomi->get_occlusion_distance());
		RS::get_singleton()->camera_set_environment(cameras[i], environment);
	}
	RS::get_singleton()->material_set_param(shader_material,"occluder_color",aomi->get_occlusion_color());
	vert_per_batch = aomi->vertices_per_batch;
	max_msecs_per_batch = aomi->max_msecs_per_batch;
	SceneTree::get_singleton()->connect(SNAME("process_frame"),callable_mp(this,&AOBakeGenerator::start_generation), CONNECT_ONE_SHOT);
	return true;
}

void AOBakeGenerator::start_generation() {
	set_generation_pending(false);
	generating = true;
	if (mesh_to_generate.is_null() || mesh_to_generate->get_surface_count() <= 0) {
		ERR_PRINT("Mesh is null or has no surfaces");
		generating = false;
		return;
	}
	RS::get_singleton()->instance_set_base(mesh_instance, mesh_to_generate->get_rid());
	RS::get_singleton()->instance_geometry_set_material_override(mesh_instance,shader_material);
	set_last_baked_mesh(memnew(ArrayMesh));
	last_baked_mesh->set_name(mesh_to_generate->get_name());
	//print_line("How many surfaces are we dealing with? ",mesh_to_generate->get_surface_count());
	all_surfaces = mesh_to_generate->get_surface_count();
	current_surface = 0;
	current_surface_name = mesh_to_generate->surface_get_name(current_surface);

	if (!using_mesh_data_tool.is_valid()) {
		using_mesh_data_tool.instantiate();
	}
	using_mesh_data_tool->clear();
	using_mesh_data_tool->create_from_surface(mesh_to_generate,current_surface);

	all_vertices = using_mesh_data_tool->get_vertex_count();
	current_vertex=0;

	// SET MESH POSITION
	RS::get_singleton()->instance_set_transform(mesh_instance, {});

	//print_line("Starting baking ",all_vertices);

	if (!SceneTree::get_singleton()->is_connected(SNAME("process_frame"),callable_mp(this,&AOBakeGenerator::wait_frame))) {
		SceneTree::get_singleton()->connect(SNAME("process_frame"),callable_mp(this,&AOBakeGenerator::wait_frame), CONNECT_ONE_SHOT);
	}
}

void AOBakeGenerator::wait_frame() {
	if (!SceneTree::get_singleton()->is_connected(SNAME("process_frame"),callable_mp(this,&AOBakeGenerator::iterate_generation))) {
		SceneTree::get_singleton()->connect(SNAME("process_frame"),callable_mp(this,&AOBakeGenerator::iterate_generation), CONNECT_ONE_SHOT);
	}
}

void AOBakeGenerator::iterate_generation() {
	const uint64_t timestarted_iteration = OS::get_singleton()->get_ticks_msec();

	const float sampling_bias = 0.001f;
	Vector<Ref<Image>> imgs;
	imgs.resize(parallel_amount);
	int vertices_done_this_iteration = 0;
	
	// Process vertices in batches of 3
	while (current_vertex < all_vertices) {
		for (int i = 0; i < parallel_amount; i++) {
			if (current_vertex + i < all_vertices) {
				const Vector3 vertice_pos = using_mesh_data_tool->get_vertex(current_vertex + i);
				const Vector3 viewport_pos = vertice_pos + (using_mesh_data_tool->get_vertex_normal(current_vertex + i) * sampling_bias);
				const Vector3 look_at_forward = viewport_pos.direction_to(vertice_pos).normalized() * -1;
				RS::get_singleton()->camera_set_transform(cameras[i], {calculate_basis(look_at_forward), viewport_pos});
				RS::get_singleton()->viewport_set_update_mode(viewports[i], RS::VIEWPORT_UPDATE_ONCE);
			}
		}

		// Single draw call for all viewports
		RS::get_singleton()->draw(true);

		for (int i = 0; i < parallel_amount; i++) {
			if (current_vertex + i < all_vertices) {
				imgs.write[i] = RS::get_singleton()->texture_2d_get(viewport_textures[i]);

				if(imgs[i].is_null()) {
					print_line("Camera produced null image for vertex ", current_vertex);
					generating = false;
					return;
				}
				auto color_average = find_color_average(imgs[i]);
				using_mesh_data_tool->set_vertex_color(current_vertex + i, color_average);
				vertices_done_this_iteration++;
			}
		}
		
		// Move to next batch
		current_vertex += parallel_amount;
		
		if (vertices_done_this_iteration > vert_per_batch) {
			vertices_done_this_iteration = 0;
			const uint64_t time_interval_check_iteration = OS::get_singleton()->get_ticks_msec() - timestarted_iteration;
			if (current_vertex < all_vertices-2 && time_interval_check_iteration > max_msecs_per_batch) {
				break;
			}
		}
	}

	if (current_vertex >= all_vertices-1) {
		using_mesh_data_tool->commit_to_surface(last_baked_mesh);
		last_baked_mesh->surface_set_name(current_surface,current_surface_name);
		last_baked_mesh->surface_set_material(current_surface,mesh_to_generate->surface_get_material(current_surface));
		current_surface += 1;
		if (current_surface < all_surfaces) {
			using_mesh_data_tool->clear();
			using_mesh_data_tool->create_from_surface(mesh_to_generate,current_surface);
			current_surface_name = mesh_to_generate->surface_get_name(current_surface);
			all_vertices = using_mesh_data_tool->get_vertex_count();
			current_vertex=0;
		} else {
			generating=false;
			generation_pending=false;
			emit_signal(SNAME("finished_gen"));
			return;
		}
	}

	if (!SceneTree::get_singleton()->is_connected(SNAME("process_frame"),callable_mp(this,&AOBakeGenerator::wait_frame))) {
		SceneTree::get_singleton()->connect(SNAME("process_frame"),callable_mp(this,&AOBakeGenerator::wait_frame), CONNECT_ONE_SHOT);
	}
}

void AOBakeGenerator::finish_generation() {
	RS::get_singleton()->instance_set_base(mesh_instance, RID());
	generating = false;
}

AOBakeGenerator::AOBakeGenerator() {
    scenario = RS::get_singleton()->scenario_create();

	// Main viewport setup
	for (int i = 0; i < parallel_amount; i++) {
		viewports.push_back(RS::get_singleton()->viewport_create());
		RS::get_singleton()->viewport_set_update_mode(viewports[i], RS::VIEWPORT_UPDATE_DISABLED);
		RS::get_singleton()->viewport_set_scenario(viewports[i], scenario);
		RS::get_singleton()->viewport_set_size(viewports[i], 32, 32);
		RS::get_singleton()->viewport_set_transparent_background(viewports[i], false);
		RS::get_singleton()->viewport_set_active(viewports[i], true);
		viewport_textures.push_back(RS::get_singleton()->viewport_get_texture(viewports[i]));
		cameras.push_back(RS::get_singleton()->camera_create());
		RS::get_singleton()->viewport_attach_camera(viewports[i], cameras[i]);
		RS::get_singleton()->camera_set_transform(cameras[i], Transform3D(Basis(), Vector3(0, 0, 3)));
		RS::get_singleton()->camera_set_perspective(cameras[i],135,0.001,10.0);
	}

	shader_material = RS::get_singleton()->material_create();
	occluder_shader = RS::get_singleton()->shader_create();
	RS::get_singleton()->shader_set_code(occluder_shader,R"(
		shader_type spatial;
		render_mode unshaded,cull_disabled;
		uniform vec3 occluder_color : source_color = vec3(0.799,0.811,0.823);
		void fragment() {
			ALBEDO = occluder_color;
		}
		)");
	RS::get_singleton()->material_set_shader(shader_material,occluder_shader);
	environment = RS::get_singleton()->environment_create();
	RS::get_singleton()->environment_set_background(environment, RenderingServer::ENV_BG_COLOR);
	RS::get_singleton()->environment_set_bg_color(environment, Color(0.5882353,0.6078432,0.627451,1.0));

	for (int i = 0; i < cameras.size(); i++) {
		RS::get_singleton()->camera_set_environment(cameras[i], environment);
	}

	mesh_instance = RS::get_singleton()->instance_create();
	RS::get_singleton()->instance_set_scenario(mesh_instance, scenario);
}

AOBakeGenerator::~AOBakeGenerator() {
    ERR_FAIL_NULL(RenderingServer::get_singleton());

	RS::get_singleton()->free(mesh_instance);
	for (int i = 0; i < viewports.size(); i++) {
		RS::get_singleton()->free(viewports[i]);
	}
	for (int i = 0; i < viewport_textures.size(); i++) {
		RS::get_singleton()->free(viewport_textures[i]);
	}
	for (int i = 0; i < cameras.size(); i++) {
		RS::get_singleton()->free(cameras[i]);
	}
    RS::get_singleton()->free(scenario);
    RS::get_singleton()->free(environment);
	if (!mesh_to_generate.is_null() && mesh_to_generate.is_valid()) {
		mesh_to_generate = nullptr;
	}
}

#endif //TOOLS ENABLED