//
// Created by Daniel on 2024-02-03.
//

#include "AOBakeableMeshInstance.h"


void AOBakeableMeshInstance::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_baked_mesh", "mesh"), &AOBakeableMeshInstance::set_baked_mesh);
    ClassDB::bind_method(D_METHOD("get_baked_mesh"), &AOBakeableMeshInstance::get_baked_mesh);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "baked_mesh", PROPERTY_HINT_RESOURCE_TYPE, "Mesh"), "set_baked_mesh", "get_baked_mesh");

	ClassDB::bind_method(D_METHOD("set_auto_set_mesh_from_bake", "auto"), &AOBakeableMeshInstance::set_auto_set_mesh_from_bake,DEFVAL(false));
	ClassDB::bind_method(D_METHOD("get_auto_set_mesh_from_bake"), &AOBakeableMeshInstance::get_auto_set_mesh_from_bake);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_set_mesh_from_bake"), "set_auto_set_mesh_from_bake", "get_auto_set_mesh_from_bake");


	ClassDB::bind_method(D_METHOD("set_debug_vertex", "v"), &AOBakeableMeshInstance::set_debug_vertex);
	ClassDB::bind_method(D_METHOD("get_debug_vertex"), &AOBakeableMeshInstance::get_debug_vertex);

	ClassDB::bind_method(D_METHOD("set_occlusion_distance", "distance"), &AOBakeableMeshInstance::set_occlusion_distance);
	ClassDB::bind_method(D_METHOD("get_occlusion_distance"), &AOBakeableMeshInstance::get_occlusion_distance);

	ClassDB::bind_method(D_METHOD("set_occlusion_angle", "angle"), &AOBakeableMeshInstance::set_occlusion_angle);
	ClassDB::bind_method(D_METHOD("get_occlusion_angle"), &AOBakeableMeshInstance::get_occlusion_angle);

	ADD_GROUP("Colors","");
	ClassDB::bind_method(D_METHOD("set_occlusion_color", "occlusion_color"), &AOBakeableMeshInstance::set_occlusion_color,DEFVAL(Color(0.799,0.811,0.823,1.0)));
	ClassDB::bind_method(D_METHOD("get_occlusion_color"), &AOBakeableMeshInstance::get_occlusion_color);
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "occlusion_color"), "set_occlusion_color", "get_occlusion_color");

	ClassDB::bind_method(D_METHOD("set_environment_color", "environment_color"), &AOBakeableMeshInstance::set_environment_color,DEFVAL(Color(1.0,0.9803922,0.9019608,1.0)));
	ClassDB::bind_method(D_METHOD("get_environment_color"), &AOBakeableMeshInstance::get_environment_color);
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "environment_color"), "set_environment_color", "get_environment_color");

	ADD_GROUP("Occlusion","");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "occlusion_distance", PROPERTY_HINT_RANGE, "0.01,100,0.01,suffix:m"), "set_occlusion_distance", "get_occlusion_distance");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "occlusion_angle", PROPERTY_HINT_RANGE, "45,145,1,suffix:deg"), "set_occlusion_angle", "get_occlusion_angle");

	//ADD_PROPERTY(PropertyInfo(Variant::INT, "debug_vertex"), "set_debug_vertex", "get_debug_vertex");
}

