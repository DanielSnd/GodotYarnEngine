#include "ymeshgen.h"

YMeshGen* YMeshGen::singleton = nullptr;

YMeshGen::YMeshGen() {
    singleton = this;
}

YMeshGen::~YMeshGen() {
    if(singleton !=nullptr && singleton == this) {
        singleton = nullptr;
    }
}

void YMeshGen::_bind_methods() {
    ClassDB::bind_method(D_METHOD("create_mesh_from_aabbs_vcolor", "aabbs", "encompassing_aabb", "margin", "resolution"), 
                        &YMeshGen::create_mesh_from_aabbs_vcolor);
}

YMeshGen* YMeshGen::get_singleton() {
    return singleton;
}

Ref<Mesh> YMeshGen::create_mesh_from_aabbs_vcolor(const TypedArray<Rect2>& p_aabbs, const Rect2& p_encompassing_aabb, float p_margin, float p_resolution) {
    Ref<ArrayMesh> mesh;
    mesh.instantiate();

    // Expand the encompassing AABB by the margin
    Rect2 encompassing_aabb = p_encompassing_aabb;
    encompassing_aabb.position.x -= p_margin;
    encompassing_aabb.position.y -= p_margin;
    encompassing_aabb.size.x += 2 * p_margin;
    encompassing_aabb.size.y += 2 * p_margin;

    // Calculate steps
    int x_steps = int(encompassing_aabb.size.x / p_resolution) + 1;
    int z_steps = int(encompassing_aabb.size.y / p_resolution) + 1;

    // Initialize arrays for mesh data
    PackedVector3Array vertices;
    PackedColorArray vertices_colors;
    PackedInt32Array indices;
    HashMap<int64_t, int> vertex_to_index; // Using int64_t as key for x,z coordinates

    // Iterate through the grid
    for (int o = 0; o < x_steps; o++) {
        float x = encompassing_aabb.position.x + (o * p_resolution);
        for (int j = 0; j < z_steps; j++) {
            float z = encompassing_aabb.position.y + (j * p_resolution);

            // Define vertices for a face
            Vector3 new_verts[4] = {
                Vector3(x, 0.0, z),
                Vector3(x + p_resolution, 0.0, z),
                Vector3(x + p_resolution, 0.0, z + p_resolution),
                Vector3(x, 0.0, z + p_resolution)
            };

            // Process vertices and create indices
            for (int i = 0; i < 4; i++) {
                // Create a unique key for this vertex position
                int64_t key = (int64_t(o) << 32) | int64_t(j + (i >= 2 ? 1 : 0));
                
                if (!vertex_to_index.has(key)) {
                    vertex_to_index[key] = vertices.size();
                    vertices.push_back(new_verts[i]);

                    // Check if vertex is inside any AABB
                    bool is_inside = false;
                    for (int aabb_idx = 0; aabb_idx < p_aabbs.size(); aabb_idx++) {
                        Rect2 aabb = p_aabbs[aabb_idx];
                        if (aabb.has_point(Vector2(new_verts[i].x, new_verts[i].z))) {
                            is_inside = true;
                            break;
                        }
                    }
                    vertices_colors.push_back(is_inside ? Color(1, 0, 0) : Color(1, 1, 1));
                }
            }

            // Add indices for the two triangles
            indices.push_back(vertex_to_index[(int64_t(o) << 32) | int64_t(j)]);
            indices.push_back(vertex_to_index[(int64_t(o + 1) << 32) | int64_t(j)]);
            indices.push_back(vertex_to_index[(int64_t(o + 1) << 32) | int64_t(j + 1)]);
            indices.push_back(vertex_to_index[(int64_t(o + 1) << 32) | int64_t(j + 1)]);
            indices.push_back(vertex_to_index[(int64_t(o) << 32) | int64_t(j + 1)]);
            indices.push_back(vertex_to_index[(int64_t(o) << 32) | int64_t(j)]);
        }
    }

    // Create surface arrays
    Array surface_array;
    surface_array.resize(Mesh::ARRAY_MAX);
    surface_array[Mesh::ARRAY_VERTEX] = vertices;
    surface_array[Mesh::ARRAY_COLOR] = vertices_colors;
    surface_array[Mesh::ARRAY_INDEX] = indices;

    // Add surface to mesh
    mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surface_array);

    return mesh;
}