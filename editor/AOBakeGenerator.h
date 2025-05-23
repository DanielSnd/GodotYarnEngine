//
// Created by Daniel on 2024-02-03.
//

#ifndef AOBAKEGENERATOR_H
#define AOBAKEGENERATOR_H

#ifdef TOOLS_ENABLED
#include "../AOBakeableMeshInstance.h"
#include "core/object/ref_counted.h"
#include "core/os/semaphore.h"
#include "scene/resources/image_texture.h"
#include "scene/resources/mesh.h"
#include "scene/resources/mesh_data_tool.h"
#include "scene/resources/texture.h"
#include "scene/3d/camera_3d.h"


class AOBakeGenerator : public RefCounted {
    GDCLASS(AOBakeGenerator, RefCounted);
    RID scenario;
    RID mesh_instance;
    Vector<RID> cameras;
    Vector<RID> viewports;
    Vector<RID> viewport_textures;
    RID environment;
    RID shader_material;
    RID occluder_shader;

protected:
	static void _bind_methods();
    mutable bool generating=false;
    mutable bool generation_pending=false;

public:
    int parallel_amount = 15;
    Ref<ArrayMesh> mesh_to_generate;
    Ref<Texture2D> last_generated_texture;
    Ref<ArrayMesh> last_baked_mesh;
    Ref<MeshDataTool> using_mesh_data_tool;
    String current_surface_name = "";
    real_t occlusion_distance = 10.0;
    real_t occlusion_angle = 135.0;
    mutable int all_vertices = 0;
    mutable int current_vertex = 0;
    mutable int current_surface = 0;
    mutable int all_surfaces = 0;
    mutable int vert_per_batch = 1000;
    mutable int max_msecs_per_batch = 250;

    bool is_generating();

    static Color find_color_average(const Ref<Image> &img);

    Ref<ArrayMesh> get_last_baked_mesh() const;

    void set_last_baked_mesh(const Ref<Mesh> &p_mesh);

    Basis calculate_basis(Vector3 forward);

    bool queue_generation(const Ref<Mesh> &p_from, AOBakeableMeshInstance * aomi);

    void start_generation();

    void wait_frame();

    void iterate_generation();

    void finish_generation();

    bool is_generation_pending() const {return generation_pending;}
    void set_generation_pending(bool b) const {
        generation_pending = b;
    }
    AOBakeGenerator();
    ~AOBakeGenerator();
};


#endif //TOOLS ENABLED
#endif //AOBAKEGENERATOR_H
