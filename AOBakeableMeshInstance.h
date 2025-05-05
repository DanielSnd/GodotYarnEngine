//
// Created by Daniel on 2024-02-03.
//

#ifndef AOBAKEABLEMESHINSTANCE_H
#define AOBAKEABLEMESHINSTANCE_H
#include "scene/3d/mesh_instance_3d.h"
#include "scene/3d/camera_3d.h"


class AOBakeableMeshInstance : public MeshInstance3D{
    GDCLASS(AOBakeableMeshInstance, MeshInstance3D);
protected:
        static void _bind_methods();
    public:
    Ref<Mesh> baked_mesh;
    Ref<Mesh> get_baked_mesh() { return baked_mesh;}
    void set_baked_mesh(Ref<Mesh> p_baked_mesh) {
        baked_mesh = p_baked_mesh;
        if (auto_set_mesh_from_bake) {
            set_mesh(baked_mesh);
        }
    }

    int debug_vertex = 0;
    void set_debug_vertex(const int b) { debug_vertex = b;}
    int get_debug_vertex() const { return debug_vertex; }

    int vertices_per_batch = 1000;
    void set_vertices_per_batch(const int b) { vertices_per_batch = b;}
    int get_vertices_per_batch() const { return vertices_per_batch; }

    int max_msecs_per_batch = 250;
    void set_max_msecs_per_batch(const int b) { max_msecs_per_batch = b;}
    int get_max_msecs_per_batch() const { return max_msecs_per_batch; }

    real_t occlusion_distance = 10.0;
    void set_occlusion_distance(const real_t b) { occlusion_distance = b;}
    real_t get_occlusion_distance() const { return occlusion_distance; }

    real_t occlusion_angle = 135.0;
    void set_occlusion_angle(const real_t b) { occlusion_angle = b; }
    real_t get_occlusion_angle() const { return occlusion_angle; }

    bool auto_set_mesh_from_bake = false;
    void set_auto_set_mesh_from_bake(bool b) {auto_set_mesh_from_bake = b;}
    bool get_auto_set_mesh_from_bake() const {return auto_set_mesh_from_bake;}

    Color occlusion_color = Color{0.799,0.811,0.823,1.0};
    Color environment_color = Color{1.0,0.9803922,0.9019608,1.0};

    void set_occlusion_color(Color b) {occlusion_color = b;}
    Color get_occlusion_color() const {return occlusion_color;}

    void set_environment_color(Color b) {environment_color = b;}
    Color get_environment_color() const {return environment_color;}
};

#endif //AOBAKEABLEMESHINSTANCE_H
