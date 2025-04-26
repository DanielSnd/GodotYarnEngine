#ifndef YARNMESHGEN_H
#define YARNMESHGEN_H

#include "core/object/ref_counted.h"
#include "scene/resources/mesh.h"
#include "scene/resources/surface_tool.h"
#include "core/math/rect2.h"
#include "core/math/vector3.h"
#include "core/math/color.h"
#include "core/variant/typed_array.h"
#include "core/math/vector3i.h"
#include "core/templates/hash_map.h"

class YMeshGen : public RefCounted {
    GDCLASS(YMeshGen, RefCounted);

protected:
    static void _bind_methods();

    struct FaceData {
        Vector3 point1, point2, point3, point4;
        Vector2 uv1, uv2, uv3, uv4;
        Vector3 normal;
        uint8_t desired_byte;

        FaceData() : desired_byte(0) {} // Default constructor

        FaceData(const Vector3& p1, const Vector3& p2, const Vector3& p3, const Vector3& p4, uint8_t byte) 
            : point1(p1), point2(p2), point3(p3), point4(p4), desired_byte(byte) {
            // Calculate face normal
            Vector3 edge1 = p2 - p1;
            Vector3 edge2 = p4 - p1;
            normal = edge1.cross(edge2).normalized();
            
            // Default UVs based on position
            uv1 = Vector2(p1.x, p1.z);
            uv2 = Vector2(p2.x, p2.z);
            uv3 = Vector2(p3.x, p3.z);
            uv4 = Vector2(p4.x, p4.z);
        }
    };

    struct WallColliderData {
        Vector3 point;
        Vector3 scale;

        WallColliderData(const Vector3& p, const Vector3& s) : point(p), scale(s) {}
    };

    // Wall generation state
    HashMap<Vector3i, uint8_t> world;
    Vector<Vector3i> world_inside;
    Vector<FaceData> collider_faces;
    Vector<FaceData> mesh_faces;
    Vector<FaceData> floor_faces;
    Vector<Vector3> output_vertices;
    Vector<Color> output_colors;
    Vector<int32_t> output_triangles;
    Vector<Vector2> output_uvs;
    Vector<Vector2> final_uvs;
    Vector<int32_t> output_collider_triangles;
    HashMap<Vector3, int32_t> output_pos_to_index;
    HashMap<Vector3, int32_t> output_collider_pos_to_index;
    Vector<Vector3> output_collider_verts;
    Vector<Vector3> final_vertices;
    Vector<Color> final_colors;
    HashSet<Vector3> inside_window_bars;
    HashSet<Vector3> block_center_created;
    Vector3 min_pos;
    Vector3 max_pos;
    Vector<Vector3> output_normals;

    // Helper functions for wall generation
    bool is_door(uint8_t cell_type) const;
    bool is_window(uint8_t cell_type) const;
    bool is_different_existing_cell(uint8_t v, uint8_t other_cell, uint8_t& out_other) const;
    Vector3i pos_north(const Vector3i& v) const;
    Vector3i pos_south(const Vector3i& v) const;
    Vector3i pos_east(const Vector3i& v) const;
    Vector3i pos_west(const Vector3i& v) const;
    Vector3 west_forward() const;
    Vector3 west_right() const;
    Vector3 east_forward() const;
    Vector3 east_right() const;
    Vector3 south_forward() const;
    Vector3 south_right() const;
    Vector3 north_forward() const;
    Vector3 north_right() const;

    // Helper functions for wall generation
    void create_window(const Vector3& wall_center_pos, const Vector3& dir_right, const Vector3& dir_front, 
                      uint8_t desired_byte, float height, float thickness, uint8_t bar_byte);
    void create_regular_wall(const Vector3& wall_center_pos, const Vector3& dir_right, const Vector3& dir_front, 
                           uint8_t desired_byte, float height, float thickness);
    void create_side_wall(const Vector3& wall_center_pos, const Vector3& dir_right, const Vector3& dir_front, 
                         uint8_t desired_byte, float column_thick, float thickness, float base_above, float height_b, bool is_left);

    // Properties
    float uv_scale_walls;
    float uv_scale_floor;
    float wall_height;
    float wall_thickness;
    float door_width;
    float window_width;
    bool auto_rebuild;

public:
    enum CellType {
        Inside = 0,
        InsideDoor = 1,
        InsideWindow = 2,
        InsideB = 3,
        InsideDoorB = 4,
        InsideDoorB2 = 5,
        InsideWindow2 = 6,
        InsideB2 = 7,
        InsideDoor2 = 8,
        InsideWindow2_2 = 9,
        Inside2 = 10,
        Outside = 100,
        OutsideDoor = 101,
        OutsideWindow = 102,
        None = 255
    };

    enum WallType {
        Wall = 0,
        Door = 1,
        Window = 2,
        SideColumn = 3,
        SideColumnLeft = 4,
        SideColumnRight = 5,
        Floor = 6
    };


    WallType desiredWallType(CellType myType, CellType otherType)
    {
        if (is_door(myType) || is_door(otherType)) return WallType::Door;
        else if (is_window(myType) || is_window(otherType)) return WallType::Window;
        return WallType::Wall;
    }

    // bool HasWallOnCell(Vector3i v)
    // {
    //     if (!world.TryGetValue(v, out byte b)) return false;
    //     bool createdWall = false;
    //     uint8_t cellByte = b;
    //     bool hasNorth = cellByte.IsDifferentExistingCell(v.CellNorthByte(), out var northByte);
    //     bool hasSouth = cellByte.IsDifferentExistingCell(v.CellSouthByte(), out var southByte);
    //     bool hasEast = cellByte.IsDifferentExistingCell(v.CellEastByte(), out var eastByte);
    //     bool hasWest = cellByte.IsDifferentExistingCell(v.CellWestByte(), out var westByte);
    //     return hasNorth || hasSouth || hasEast || hasWest;
    // }

    void calculate_uvs_for_face(FaceData& face, bool is_wall);
    void add_face_to_mesh(const FaceData& face);

    Ref<Mesh> create_mesh_from_aabbs_vcolor(const TypedArray<Rect2>& p_aabbs, const Rect2& p_encompassing_aabb, float p_margin, float p_resolution, float push_verts_down);
    
    // Wall generation functions
    void create_wall(const Vector3& wall_center_pos, const Vector3& dir_right, const Vector3& dir_front, uint8_t desired_byte, uint8_t wall_type, float height = 2.0f);
    void create_block(const Vector3& wall_center_pos, const Vector3& dir_right, const Vector3& dir_front, uint8_t desired_byte, float side_thick = 0.4f, float thickness = 0.2f, float base_above = 0.0f, float height = 2.0f, float top_above = 0.0f, bool collider_block = false);
    void create_floor(const Vector3& wall_center_pos, const Vector3& dir_right, const Vector3& dir_front, uint8_t desired_byte, float base_above = 0.0f, float top_above = 0.0f);
    Ref<ArrayMesh> rebuild_walls();
    void make_world_change(const Vector3i& pos, uint8_t cell_type);
    uint8_t get_cell_byte(const Vector3i& pos) const;
    Ref<ArrayMesh> last_built_mesh;
    Ref<ArrayMesh> get_last_built_mesh() const;
    // Property getters/setters
    void set_uv_scale_walls(float p_scale);
    float get_uv_scale_walls() const;
    void set_uv_scale_floor(float p_scale);
    float get_uv_scale_floor() const;
    void set_wall_height(float p_height);
    float get_wall_height() const;
    void set_wall_thickness(float p_thickness);
    float get_wall_thickness() const;
    void set_door_width(float p_width);
    float get_door_width() const;
    void set_window_width(float p_width);
    float get_window_width() const;
    void set_auto_rebuild(bool p_auto);
    bool get_auto_rebuild() const;

    // World manipulation methods
    void clear_world();
    void fill_rect(const Vector3i& start, const Vector3i& end, uint8_t cell_type);
    TypedArray<Vector3i> get_cells_of_type(uint8_t cell_type) const;
    bool has_wall_at(const Vector3i& pos) const;
    Vector3 get_world_bounds_min() const;
    Vector3 get_world_bounds_max() const;

    YMeshGen();
    ~YMeshGen();
};

VARIANT_ENUM_CAST(YMeshGen::CellType);
VARIANT_ENUM_CAST(YMeshGen::WallType);
#endif //YARNMESHGEN_H
