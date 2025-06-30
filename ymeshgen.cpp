#include "ymeshgen.h"

YMeshGen::YMeshGen() {
    min_pos = Vector3(9999, 1, 9999);
    max_pos = Vector3(-9999, 1, -9999);
    
    // Initialize properties with default values
    uv_scale_walls = 0.5f;
    uv_scale_floor = 0.25f;
    wall_height = 2.0f;
    wall_thickness = 0.1f;
    door_width = 0.92f;
    window_width = 0.84f;
    auto_rebuild = true;
}

YMeshGen::~YMeshGen() {
    if (last_built_mesh.is_valid()) {
        last_built_mesh = Ref<ArrayMesh>();
    }
}

void YMeshGen::_bind_methods() {
    ClassDB::bind_method(D_METHOD("create_mesh_from_aabbs_vcolor", "aabbs", "encompassing_aabb", "margin", "resolution", "push_verts_down"), 
                        &YMeshGen::create_mesh_from_aabbs_vcolor, DEFVAL(0.0f), DEFVAL(1.0f), DEFVAL(0.0f));
    
    ClassDB::bind_method(D_METHOD("create_wall", "wall_center_pos", "dir_right", "dir_front", "desired_byte", "height"),
                        &YMeshGen::create_wall, DEFVAL(2.0f));
    
    ClassDB::bind_method(D_METHOD("rebuild_walls"),
                        &YMeshGen::rebuild_walls);
    
    ClassDB::bind_method(D_METHOD("make_world_change", "pos", "cell_type"),
                        &YMeshGen::make_world_change);

    ClassDB::bind_method(D_METHOD("get_cell_byte", "pos"),
                        &YMeshGen::get_cell_byte);

    // Property bindings
    ClassDB::bind_method(D_METHOD("set_uv_scale_walls", "scale"), &YMeshGen::set_uv_scale_walls);
    ClassDB::bind_method(D_METHOD("get_uv_scale_walls"), &YMeshGen::get_uv_scale_walls);
    ClassDB::bind_method(D_METHOD("set_uv_scale_floor", "scale"), &YMeshGen::set_uv_scale_floor);
    ClassDB::bind_method(D_METHOD("get_uv_scale_floor"), &YMeshGen::get_uv_scale_floor);
    ClassDB::bind_method(D_METHOD("set_wall_height", "height"), &YMeshGen::set_wall_height);
    ClassDB::bind_method(D_METHOD("get_wall_height"), &YMeshGen::get_wall_height);
    ClassDB::bind_method(D_METHOD("set_wall_thickness", "thickness"), &YMeshGen::set_wall_thickness);
    ClassDB::bind_method(D_METHOD("get_wall_thickness"), &YMeshGen::get_wall_thickness);
    ClassDB::bind_method(D_METHOD("set_door_width", "width"), &YMeshGen::set_door_width);
    ClassDB::bind_method(D_METHOD("get_door_width"), &YMeshGen::get_door_width);
    ClassDB::bind_method(D_METHOD("set_window_width", "width"), &YMeshGen::set_window_width);
    ClassDB::bind_method(D_METHOD("get_window_width"), &YMeshGen::get_window_width);
    ClassDB::bind_method(D_METHOD("set_auto_rebuild", "auto_rebuild"), &YMeshGen::set_auto_rebuild);
    ClassDB::bind_method(D_METHOD("get_auto_rebuild"), &YMeshGen::get_auto_rebuild);
    ClassDB::bind_method(D_METHOD("get_last_built_mesh"), &YMeshGen::get_last_built_mesh);
    // World manipulation bindings
    ClassDB::bind_method(D_METHOD("clear_world"), &YMeshGen::clear_world);
    ClassDB::bind_method(D_METHOD("fill_rect", "start", "end", "cell_type"), &YMeshGen::fill_rect);
    ClassDB::bind_method(D_METHOD("get_cells_of_type", "cell_type"), &YMeshGen::get_cells_of_type);
    ClassDB::bind_method(D_METHOD("has_wall_at", "pos"), &YMeshGen::has_wall_at);
    ClassDB::bind_method(D_METHOD("get_world_bounds_min"), &YMeshGen::get_world_bounds_min);
    ClassDB::bind_method(D_METHOD("get_world_bounds_max"), &YMeshGen::get_world_bounds_max);

    // Add properties to inspector
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "uv_scale_walls"), "set_uv_scale_walls", "get_uv_scale_walls");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "uv_scale_floor"), "set_uv_scale_floor", "get_uv_scale_floor");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "wall_height"), "set_wall_height", "get_wall_height");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "wall_thickness"), "set_wall_thickness", "get_wall_thickness");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "door_width"), "set_door_width", "get_door_width");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "window_width"), "set_window_width", "get_window_width");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_rebuild"), "set_auto_rebuild", "get_auto_rebuild");

    // Add CellType enum
    BIND_ENUM_CONSTANT(Inside);
    BIND_ENUM_CONSTANT(InsideDoor);
    BIND_ENUM_CONSTANT(InsideWindow);
    BIND_ENUM_CONSTANT(InsideB);
    BIND_ENUM_CONSTANT(InsideDoorB);
    BIND_ENUM_CONSTANT(InsideDoorB2);
    BIND_ENUM_CONSTANT(InsideWindow2);
    BIND_ENUM_CONSTANT(InsideB2);
    BIND_ENUM_CONSTANT(InsideDoor2);
    BIND_ENUM_CONSTANT(InsideWindow2_2);
    BIND_ENUM_CONSTANT(Inside2);
    BIND_ENUM_CONSTANT(Outside);
    BIND_ENUM_CONSTANT(OutsideDoor);
    BIND_ENUM_CONSTANT(OutsideWindow);
    BIND_ENUM_CONSTANT(None);

    BIND_ENUM_CONSTANT(Wall);
    BIND_ENUM_CONSTANT(Door);
    BIND_ENUM_CONSTANT(Window);
    BIND_ENUM_CONSTANT(SideColumn);
    BIND_ENUM_CONSTANT(SideColumnLeft);
    BIND_ENUM_CONSTANT(SideColumnRight);
    BIND_ENUM_CONSTANT(Floor);
}

// Helper functions for wall generation
bool YMeshGen::is_door(uint8_t cell_type) const {
    return cell_type == CellType::InsideDoor || cell_type == CellType::InsideDoorB || 
           cell_type == CellType::InsideDoorB2 || cell_type == CellType::InsideDoor2 || 
           cell_type == CellType::OutsideDoor;
}

bool YMeshGen::is_window(uint8_t cell_type) const {
    return cell_type == CellType::InsideWindow || cell_type == CellType::OutsideWindow;
}

bool YMeshGen::is_different_existing_cell(uint8_t v, uint8_t other_cell, uint8_t& out_other) const {
    out_other = other_cell;
    return ((v < 100 && other_cell >= 100) || (v >= 100 && other_cell < 100)) && 
           v != 255 && other_cell != 255;
}

const int TILE_SIZE = 2;

Vector3i YMeshGen::pos_north(const Vector3i& v) const {
    return Vector3i(v.x, v.y, v.z + TILE_SIZE);
}

Vector3i YMeshGen::pos_south(const Vector3i& v) const {
    return Vector3i(v.x, v.y, v.z - TILE_SIZE);
}

Vector3i YMeshGen::pos_east(const Vector3i& v) const {
    return Vector3i(v.x + TILE_SIZE, v.y, v.z);
}

Vector3i YMeshGen::pos_west(const Vector3i& v) const {
    return Vector3i(v.x - TILE_SIZE, v.y, v.z);
}

Vector3 YMeshGen::west_forward() const {
    return Vector3(-1, 0, 0);
}

Vector3 YMeshGen::west_right() const {
    return Vector3(0, 0, 1);
}

Vector3 YMeshGen::east_forward() const {
    return Vector3(1, 0, 0);
}

Vector3 YMeshGen::east_right() const {
    return Vector3(0, 0, -1);
}

Vector3 YMeshGen::south_forward() const {
    return Vector3(0, 0, -1);
}

Vector3 YMeshGen::south_right() const {
    return Vector3(-1, 0, 0);
}

Vector3 YMeshGen::north_forward() const {
    return Vector3(0, 0, 1);
}

Vector3 YMeshGen::north_right() const {
    return Vector3(1, 0, 0);
}

uint8_t YMeshGen::get_cell_byte(const Vector3i& pos) const {
    const uint8_t* cell = world.getptr(pos);
    return cell ? *cell : CellType::Outside;
}

Ref<ArrayMesh> YMeshGen::create_mesh_from_aabbs_vcolor(const TypedArray<Rect2>& p_aabbs, const Rect2& p_encompassing_aabb, float p_margin, float p_resolution, float push_verts_down) {
    // auto start = std::chrono::high_resolution_clock::now();

    Ref<ArrayMesh> mesh;
    mesh.instantiate();

    // Expand the encompassing AABB by the margin
    Rect2 encompassing_aabb = p_encompassing_aabb;
    encompassing_aabb.position.x -= p_margin;
    encompassing_aabb.position.y -= p_margin;
    encompassing_aabb.size.x += 2 * p_margin;
    encompassing_aabb.size.y += 2 * p_margin;

    // Simple adaptive resolution - use larger quads for areas far from AABBs
    const float BASE_RESOLUTION = p_resolution;
    const float COARSE_RESOLUTION = p_resolution * 2.0f; // 2x larger quads for distant areas
    
    // Calculate steps
    int x_steps = int(encompassing_aabb.size.x / MAX(BASE_RESOLUTION, 0.01f)) + 1;
    int z_steps = int(encompassing_aabb.size.y / MAX(BASE_RESOLUTION, 0.01f)) + 1;

    // Initialize arrays for mesh data
    PackedVector3Array vertices;
    PackedColorArray vertices_colors;
    PackedInt32Array indices;
    HashMap<Vector2, int> vertex_to_index;

    // Pre-calculate AABB bounds for faster intersection tests
    PackedFloat32Array aabb_min_x, aabb_max_x, aabb_min_z, aabb_max_z;
    aabb_min_x.resize(p_aabbs.size());
    aabb_max_x.resize(p_aabbs.size());
    aabb_min_z.resize(p_aabbs.size());
    aabb_max_z.resize(p_aabbs.size());
    
    for (int aabb_idx = 0; aabb_idx < p_aabbs.size(); aabb_idx++) {
        Rect2 aabb = p_aabbs[aabb_idx];
        aabb_min_x.set(aabb_idx, aabb.position.x);
        aabb_max_x.set(aabb_idx, aabb.position.x + aabb.size.x);
        aabb_min_z.set(aabb_idx, aabb.position.y);
        aabb_max_z.set(aabb_idx, aabb.position.y + aabb.size.y);
    }

    // Iterate through the grid with adaptive resolution
    for (int o = 0; o < x_steps - 1; o++) {
        float x = encompassing_aabb.position.x + (o * BASE_RESOLUTION);
        for (int j = 0; j < z_steps - 1; j++) {
            float z = encompassing_aabb.position.y + (j * BASE_RESOLUTION);

            // Check if this area is near any AABB
            float center_x = x + BASE_RESOLUTION * 0.5f;
            float center_z = z + BASE_RESOLUTION * 0.5f;
            
            bool near_aabb = false;
            for (int aabb_idx = 0; aabb_idx < p_aabbs.size(); aabb_idx++) {
                if (center_x >= aabb_min_x[aabb_idx] - 5.0f && center_x <= aabb_max_x[aabb_idx] + 5.0f &&
                    center_z >= aabb_min_z[aabb_idx] - 5.0f && center_z <= aabb_max_z[aabb_idx] + 5.0f) {
                    near_aabb = true;
                    break;
                }
            }

            // Use fine resolution near AABBs, coarse resolution far away
            float current_resolution = near_aabb ? BASE_RESOLUTION : COARSE_RESOLUTION;
            
            // Skip coarse quads that are completely outside the area of interest
            if (!near_aabb && o % 2 == 1 && j % 2 == 1) {
                continue; // Skip every other quad when using coarse resolution
            }

            // Define vertices for a quad
            Vector3 quad_verts[4] = {
                Vector3(x, 0.0, z),
                Vector3(x + current_resolution, 0.0, z),
                Vector3(x + current_resolution, 0.0, z + current_resolution),
                Vector3(x, 0.0, z + current_resolution)
            };

            // Check if any vertex of this quad is inside any AABB
            bool quad_has_inside_vertex = false;
            bool vertex_inside[4] = {false, false, false, false};
            
            for (int v = 0; v < 4; v++) {
                float vx = quad_verts[v].x;
                float vz = quad_verts[v].z;
                
                for (int aabb_idx = 0; aabb_idx < p_aabbs.size(); aabb_idx++) {
                    if (vx >= aabb_min_x[aabb_idx] && vx <= aabb_max_x[aabb_idx] &&
                        vz >= aabb_min_z[aabb_idx] && vz <= aabb_max_z[aabb_idx]) {
                        vertex_inside[v] = true;
                        quad_has_inside_vertex = true;
                        break;
                    }
                }
            }

            // if (quad_has_inside_vertex) {
            //     total_quads_inside++;
            // }
            // total_quads_processed++;

            // Process vertices and create indices
            int quad_indices[4];
            for (int i = 0; i < 4; i++) {
                Vector2 vertex_key(quad_verts[i].x, quad_verts[i].z);
                
                if (!vertex_to_index.has(vertex_key)) {
                    vertex_to_index[vertex_key] = vertices.size();

                    // Use pre-calculated vertex inside status
                    if (vertex_inside[i]) {
                        vertices_colors.push_back(Color(1, 0, 0));
                        vertices.push_back(quad_verts[i] + Vector3(0, push_verts_down, 0));
                        // total_inside_verts++;
                    } else {
                        vertices_colors.push_back(Color(1, 1, 1));
                        vertices.push_back(quad_verts[i]);
                        // total_outside_verts++;
                    }
                }
                quad_indices[i] = vertex_to_index[vertex_key];
            }

            // Add indices for the two triangles of this quad
            indices.push_back(quad_indices[0]);
            indices.push_back(quad_indices[1]);
            indices.push_back(quad_indices[2]);
            indices.push_back(quad_indices[2]);
            indices.push_back(quad_indices[3]);
            indices.push_back(quad_indices[0]);
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

    // // Calculate the duration in microseconds
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
    // // Print the duration
    // print_line(vformat("Mesh generation complete. Time taken by generation: %s us", static_cast<int64_t>(duration.count())));

    return mesh;
}

void YMeshGen::create_block(const Vector3& wall_center_pos, const Vector3& dir_right, const Vector3& dir_front, 
                          uint8_t desired_byte, float side_thick, float thickness, float base_above, 
                          float height, float top_above, bool collider_block) {
    float forward_dist = 1.0f - thickness;
    
    if (collider_block) {
        forward_dist -= desired_byte >= 100 ? 0.25f : 0.05f;
    }

    // Calculate points for front face
    Vector3 point1 = wall_center_pos + dir_front * forward_dist - dir_right * side_thick;
    point1.y += base_above;
    Vector3 point2 = point1;
    point2.y += height;
    Vector3 point3 = wall_center_pos + dir_front * forward_dist + dir_right * side_thick;
    point3.y += base_above + height;
    Vector3 point4 = point3;
    point4.y -= height;

    // Calculate points for back face
    forward_dist = 1.0f;
    Vector3 point8 = wall_center_pos + dir_front * forward_dist - dir_right * side_thick;
    point8.y += base_above;
    Vector3 point7 = point8;
    point7.y += height;
    Vector3 point6 = wall_center_pos + dir_front * forward_dist + dir_right * side_thick;
    point6.y += base_above + height;
    Vector3 point5 = point6;
    point5.y -= height;

    Vector3 block_center = (point1 + point5) * 0.5f;
    
    if (!collider_block) {
        if (block_center_created.has(block_center)) {
            return;
        }
        block_center_created.insert(block_center);
    }

    if (collider_block) {
        collider_faces.push_back(FaceData(point1, point2, point3, point4, desired_byte));
        collider_faces.push_back(FaceData(point5, point6, point7, point8, desired_byte));
        collider_faces.push_back(FaceData(point4, point3, point6, point5, desired_byte));
        collider_faces.push_back(FaceData(point8, point7, point2, point1, desired_byte));
    } else {
        if (desired_byte == 99) {
            inside_window_bars.insert(point1);
            inside_window_bars.insert(point2);
            inside_window_bars.insert(point3);
            inside_window_bars.insert(point4);
        }
        
        mesh_faces.push_back(FaceData(point1, point2, point3, point4, desired_byte));
        mesh_faces.push_back(FaceData(point5, point6, point7, point8, desired_byte));
        mesh_faces.push_back(FaceData(point2, point7, point6, point3, desired_byte));
        mesh_faces.push_back(FaceData(point4, point3, point6, point5, desired_byte));
        mesh_faces.push_back(FaceData(point8, point7, point2, point1, desired_byte));
    }
}

void YMeshGen::create_floor(const Vector3& wall_center_pos, const Vector3& dir_right, const Vector3& dir_front, 
                          uint8_t desired_byte, float base_above, float top_above) {
    Vector3 point1 = wall_center_pos - dir_front - dir_right;
    point1.y += base_above;
    Vector3 point4 = wall_center_pos + dir_right - dir_front;
    point4.y += base_above;
    Vector3 point3 = wall_center_pos + dir_right + dir_front;
    point3.y += base_above;
    Vector3 point2 = wall_center_pos - dir_right + dir_front;
    point2.y += base_above;

    Vector3 block_center = (point1 + point3) * 0.5f;
    if (block_center_created.has(block_center)) {
        return;
    }
    block_center_created.insert(block_center);
    
    // Create a floor face with correct winding order for upward normal
    FaceData floor_face(point1, point2, point3, point4, desired_byte);
    
    // Override the normal to point upward
    floor_face.normal = Vector3(0, 1, 0);
    floor_faces.push_back(floor_face);
}

void YMeshGen::make_world_change(const Vector3i& pos, uint8_t cell_type) {
    world[pos] = cell_type;
    rebuild_walls();
}

void YMeshGen::create_window(const Vector3& wall_center_pos, const Vector3& dir_right, const Vector3& dir_front, 
                           uint8_t desired_byte, float height, float thickness, uint8_t bar_byte) {
    create_block(wall_center_pos, dir_right, dir_front, desired_byte, 0.42f * TILE_SIZE, thickness, 0, 0.5f, 0.001f);
    create_block(wall_center_pos, dir_right, dir_front, desired_byte, 0.42f * TILE_SIZE, thickness, height - 0.3f, 0.3f, 0.001f);

    // Top and bottom bars
    create_block(wall_center_pos, dir_right, dir_front, bar_byte, 0.46f * TILE_SIZE, thickness * 1.25f, 0.5f, 0.16f, 0.001f);
    create_block(wall_center_pos, dir_right, dir_front, bar_byte, 0.37f * TILE_SIZE, thickness * 1.1f, height - 0.32f - 0.04f, 0.06f, 0.001f);

    // Middle window bar
    create_block(wall_center_pos, dir_right, dir_front, bar_byte, 0.37f * TILE_SIZE, thickness * 0.24f, 
                0.5f + (height - 0.32f - 0.04f - 0.5f) * 0.5f, 0.06f, 0.001f);

    // Side bars
    create_block(wall_center_pos - dir_right * (0.4f * TILE_SIZE), dir_right, dir_front, bar_byte, 
                0.06f, thickness * 1.1f, 0.5f, height - 0.3f - 0.5f, 0.001f);
    create_block(wall_center_pos + dir_right * (0.4f * TILE_SIZE), dir_right, dir_front, bar_byte, 
                0.06f, thickness * 1.1f, 0.5f, height - 0.3f - 0.5f, 0.001f);
}

void YMeshGen::create_regular_wall(const Vector3& wall_center_pos, const Vector3& dir_right, const Vector3& dir_front, 
                                uint8_t desired_byte, float height, float thickness) {
    create_block(wall_center_pos, dir_right, dir_front, desired_byte, 0.3f * TILE_SIZE, thickness, 0, 0.5f, 0.001f);
    create_block(wall_center_pos, dir_right, dir_front, desired_byte, 0.3f * TILE_SIZE, thickness, 0.5f, height - 0.5f - 0.3f, 0.001f);
    create_block(wall_center_pos, dir_right, dir_front, desired_byte, 0.3f * TILE_SIZE, thickness, height - 0.3f, 0.3f, 0.001f);
}

void YMeshGen::create_side_wall(const Vector3& wall_center_pos, const Vector3& dir_right, const Vector3& dir_front, 
                             uint8_t desired_byte, float column_thick, float thickness, float base_above, float height_b, bool is_left) {
    Vector3 offset = is_left ? -dir_right : dir_right;
    create_block(wall_center_pos + offset * (1.0f - column_thick), dir_right, dir_front, 
                desired_byte, column_thick, thickness, base_above, height_b, 0.001f);
}

void YMeshGen::create_wall(const Vector3& wall_center_pos, const Vector3& dir_right, const Vector3& dir_front, 
                          uint8_t desired_byte, uint8_t wall_type, float height) {
    float thickness = MAX(0.1f, wall_thickness);
    uint8_t bar_byte = desired_byte >= 100 ? 98 : 99;

    // Update min/max positions for wall bounds
    if (wall_center_pos.x < min_pos.x) min_pos.x = wall_center_pos.x;
    if (wall_center_pos.x > max_pos.x) max_pos.x = wall_center_pos.x;
    if (wall_center_pos.z < min_pos.z) min_pos.z = wall_center_pos.z;
    if (wall_center_pos.z > max_pos.z) max_pos.z = wall_center_pos.z;

    // Create collider block
    create_block(wall_center_pos, dir_right, dir_front, desired_byte, 0.5f * TILE_SIZE, thickness, 0, 2, 0.001f, true);

    // Handle different wall types
    if (wall_type == WallType::Window) {
        create_window(wall_center_pos, dir_right, dir_front, desired_byte, height, thickness, bar_byte);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.04f * TILE_SIZE, thickness, 0, 0.5f, true);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.04f * TILE_SIZE, thickness, 0, 0.5f, false);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.04f * TILE_SIZE, thickness, height - 0.3f, 0.3f, true);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.04f * TILE_SIZE, thickness, height - 0.3f, 0.3f, false);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.04f * TILE_SIZE, thickness, 0.5f, height - 0.5f - 0.3f, true);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.04f * TILE_SIZE, thickness, 0.5f, height - 0.5f - 0.3f, false);
    } else if (wall_type == WallType::Door) {
        // Door top
        create_block(wall_center_pos, dir_right, dir_front, desired_byte, 0.46f * TILE_SIZE, thickness, height - 0.3f, 0.3f, 0.001f);

        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.02f * TILE_SIZE, thickness, 0, 0.5f, true);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.02f * TILE_SIZE, thickness, 0, 0.5f, false);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.02f * TILE_SIZE, thickness, height - 0.3f, 0.3f, true);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.02f * TILE_SIZE, thickness, height - 0.3f, 0.3f, false);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.02f * TILE_SIZE, thickness, 0.5f, height - 0.5f - 0.3f, true);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.02f * TILE_SIZE, thickness, 0.5f, height - 0.5f - 0.3f, false);

        // Top bar
        create_block(wall_center_pos, dir_right, dir_front, bar_byte, 0.445f * TILE_SIZE, thickness * 1.1f, height - 0.32f - 0.04f, 0.06f, 0.001f);
        // Bottom bar
        create_block(wall_center_pos, dir_right, dir_front, bar_byte, 0.445f * TILE_SIZE, thickness * 1.1f, 0, 0.06f, 0.001f);

        // Side bars
        create_block(wall_center_pos - dir_right * (0.46f * TILE_SIZE), dir_right, dir_front, bar_byte, 
                    0.035f, thickness * 1.1f, 0, height - 0.3f, 0.001f);
        create_block(wall_center_pos + dir_right * (0.46f * TILE_SIZE), dir_right, dir_front, bar_byte, 
                    0.035f, thickness * 1.1f, 0, height - 0.3f, 0.001f);
    } else {
        // Regular wall
        create_regular_wall(wall_center_pos, dir_right, dir_front, desired_byte, height, thickness);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.1f * TILE_SIZE, thickness, 0, 0.5f, true);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.1f * TILE_SIZE, thickness, 0, 0.5f, false);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.1f * TILE_SIZE, thickness, height - 0.3f, 0.3f, true);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.1f * TILE_SIZE, thickness, height - 0.3f, 0.3f, false);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.1f * TILE_SIZE, thickness, 0.5f, height - 0.5f - 0.3f, true);
        create_side_wall(wall_center_pos, dir_right, dir_front, desired_byte, 0.1f * TILE_SIZE, thickness, 0.5f, height - 0.5f - 0.3f, false);
    }
}

void YMeshGen::calculate_uvs_for_face(FaceData& face, bool is_wall) {
    if (is_wall) {
        // For walls, use height and width for UV mapping
        float width = (face.point2 - face.point1).length();
        float height = (face.point2 - face.point3).length();
        
        // Scale UVs to create proper tiling
        const float UV_SCALE = 0.5f; // Adjust this to change texture tiling
        face.uv1 = Vector2(0, 0) * UV_SCALE;
        face.uv2 = Vector2(width, 0) * UV_SCALE;
        face.uv3 = Vector2(width, height) * UV_SCALE;
        face.uv4 = Vector2(0, height) * UV_SCALE;
    } else {
        // For floors/ceilings, use X and Z coordinates
        const float UV_SCALE = 0.25f; // Adjust this to change texture tiling
        face.uv1 = Vector2(face.point1.x, face.point1.z) * UV_SCALE;
        face.uv2 = Vector2(face.point2.x, face.point2.z) * UV_SCALE;
        face.uv3 = Vector2(face.point3.x, face.point3.z) * UV_SCALE;
        face.uv4 = Vector2(face.point4.x, face.point4.z) * UV_SCALE;
    }
}

void YMeshGen::add_face_to_mesh(const FaceData& face) {
    int base_idx = output_vertices.size();
    
    // Add vertices
    output_vertices.push_back(face.point1);
    output_vertices.push_back(face.point2);
    output_vertices.push_back(face.point3);
    output_vertices.push_back(face.point4);
    
    // Add UVs
    output_uvs.push_back(face.uv1);
    output_uvs.push_back(face.uv2);
    output_uvs.push_back(face.uv3);
    output_uvs.push_back(face.uv4);
    
    // Add normals
    output_normals.push_back(face.normal);
    output_normals.push_back(face.normal);
    output_normals.push_back(face.normal);
    output_normals.push_back(face.normal);
    
    // Add colors based on cell type
    Color color;
    if (face.desired_byte >= 100) {
        color = Color(1, 0, 0); // Outside walls
    } else if (face.desired_byte == 99 || face.desired_byte == 98) {
        color = Color(0, 1, 0); // Window/door frames
    } else {
        color = Color(0, 0, 1); // Inside walls
    }
    
    output_colors.push_back(color);
    output_colors.push_back(color);
    output_colors.push_back(color);
    output_colors.push_back(color);
    
    // Add triangles 
    // Counter-clockwise winding
    output_triangles.push_back(base_idx);
    output_triangles.push_back(base_idx + 2);
    output_triangles.push_back(base_idx + 1);
    output_triangles.push_back(base_idx + 2);
    output_triangles.push_back(base_idx);
    output_triangles.push_back(base_idx + 3);
}

Ref<ArrayMesh> YMeshGen::rebuild_walls() {
    // Reset state
    min_pos = Vector3(9999, 1, 9999);
    max_pos = Vector3(-9999, 1, -9999);
    mesh_faces.clear();
    floor_faces.clear();
    collider_faces.clear();
    block_center_created.clear();
    inside_window_bars.clear();
    output_vertices.clear();
    output_triangles.clear();
    output_uvs.clear();
    output_normals.clear();
    final_uvs.clear();
    output_colors.clear();
    output_pos_to_index.clear();
    output_collider_triangles.clear();
    output_collider_pos_to_index.clear();
    output_collider_verts.clear();
    final_vertices.clear();
    final_colors.clear();

    // Generate geometry
    for (const KeyValue<Vector3i, uint8_t>& kv : world) {
        uint8_t cell_byte = kv.value;
        const Vector3i& pos = kv.key;

        if (cell_byte < 100) {
            create_floor(Vector3(pos), north_right(), north_forward(), cell_byte);
        }

        uint8_t north_byte, south_byte, east_byte, west_byte;
        bool has_north = is_different_existing_cell(cell_byte, get_cell_byte(pos_north(pos)), north_byte);
        bool has_south = is_different_existing_cell(cell_byte, get_cell_byte(pos_south(pos)), south_byte);
        bool has_east = is_different_existing_cell(cell_byte, get_cell_byte(pos_east(pos)), east_byte);
        bool has_west = is_different_existing_cell(cell_byte, get_cell_byte(pos_west(pos)), west_byte);

        if (has_north) create_wall(Vector3(pos), north_right(), north_forward(), cell_byte, desiredWallType(static_cast<CellType>(cell_byte), static_cast<CellType>(north_byte)));
        if (has_south) create_wall(Vector3(pos), south_right(), south_forward(), cell_byte, desiredWallType(static_cast<CellType>(cell_byte), static_cast<CellType>(south_byte)));
        if (has_east) create_wall(Vector3(pos), east_right(), east_forward(), cell_byte, desiredWallType(static_cast<CellType>(cell_byte), static_cast<CellType>(east_byte)));
        if (has_west) create_wall(Vector3(pos), west_right(), west_forward(), cell_byte, desiredWallType(static_cast<CellType>(cell_byte), static_cast<CellType>(west_byte)));
    }

    // Process all faces and add them to the mesh
    for (int i = 0; i < mesh_faces.size(); i++) {
        calculate_uvs_for_face(mesh_faces.write[i], true);
        add_face_to_mesh(mesh_faces[i]);
    }

    for (int i = 0; i < floor_faces.size(); i++) {
        calculate_uvs_for_face(floor_faces.write[i], false);
        add_face_to_mesh(floor_faces[i]);
    }

    // Create the mesh
    if (last_built_mesh.is_null()) {
        last_built_mesh.instantiate();
    } else {
        last_built_mesh->clear_surfaces();
    }

    // Check if we have any data to create the mesh
    if (output_vertices.size() > 0 && output_triangles.size() > 0) {
        Array surface_array;
        surface_array.resize(Mesh::ARRAY_MAX);
        surface_array[Mesh::ARRAY_VERTEX] = output_vertices;
        surface_array[Mesh::ARRAY_NORMAL] = output_normals;
        surface_array[Mesh::ARRAY_TEX_UV] = output_uvs;
        surface_array[Mesh::ARRAY_COLOR] = output_colors;
        surface_array[Mesh::ARRAY_INDEX] = output_triangles;

        // Add surface to mesh
        last_built_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surface_array);
    }

    return last_built_mesh;
}

// Property implementations
void YMeshGen::set_uv_scale_walls(float p_scale) { uv_scale_walls = p_scale; }
float YMeshGen::get_uv_scale_walls() const { return uv_scale_walls; }
void YMeshGen::set_uv_scale_floor(float p_scale) { uv_scale_floor = p_scale; }
float YMeshGen::get_uv_scale_floor() const { return uv_scale_floor; }
void YMeshGen::set_wall_height(float p_height) { wall_height = p_height; }
float YMeshGen::get_wall_height() const { return wall_height; }
void YMeshGen::set_wall_thickness(float p_thickness) { wall_thickness = p_thickness; }
float YMeshGen::get_wall_thickness() const { return wall_thickness; }
void YMeshGen::set_door_width(float p_width) { door_width = p_width; }
float YMeshGen::get_door_width() const { return door_width; }
void YMeshGen::set_window_width(float p_width) { window_width = p_width; }
float YMeshGen::get_window_width() const { return window_width; }
void YMeshGen::set_auto_rebuild(bool p_auto) { auto_rebuild = p_auto; }
bool YMeshGen::get_auto_rebuild() const { return auto_rebuild; }

// World manipulation implementations
void YMeshGen::clear_world() {
    world.clear();
    if (auto_rebuild) {
        rebuild_walls();
    }
}

void YMeshGen::fill_rect(const Vector3i& start, const Vector3i& end, uint8_t cell_type) {
    for (int x = start.x; x <= end.x; x += TILE_SIZE) {
        for (int z = start.z; z <= end.z; z += TILE_SIZE) {
            world[Vector3i(x, start.y, z)] = cell_type;
        }
    }
    if (auto_rebuild) {
        rebuild_walls();
    }
}

TypedArray<Vector3i> YMeshGen::get_cells_of_type(uint8_t cell_type) const {
    TypedArray<Vector3i> result;
    for (const KeyValue<Vector3i, uint8_t>& kv : world) {
        if (kv.value == cell_type) {
            result.push_back(kv.key);
        }
    }
    return result;
}

bool YMeshGen::has_wall_at(const Vector3i& pos) const {
    if (!world.has(pos)) {
        return false;
    }
    
    uint8_t cell_byte = world[pos];
    uint8_t temp;
    return is_different_existing_cell(cell_byte, get_cell_byte(pos_north(pos)), temp) ||
           is_different_existing_cell(cell_byte, get_cell_byte(pos_south(pos)), temp) ||
           is_different_existing_cell(cell_byte, get_cell_byte(pos_east(pos)), temp) ||
           is_different_existing_cell(cell_byte, get_cell_byte(pos_west(pos)), temp);
}

Vector3 YMeshGen::get_world_bounds_min() const {
    return min_pos;
}

Vector3 YMeshGen::get_world_bounds_max() const {
    return max_pos;
}

Ref<ArrayMesh> YMeshGen::get_last_built_mesh() const {
    return last_built_mesh;
}