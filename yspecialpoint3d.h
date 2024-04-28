//
// Created by Daniel on 2024-03-19.
//

#ifndef YSPECIALPOINT3D_H
#define YSPECIALPOINT3D_H

#include "scene/3d/node_3d.h"

class YSpecialPoint3D : public Node3D {
    GDCLASS(YSpecialPoint3D, Node3D);

private:
    static void _bind_methods();

    void _notification(int p_what);

protected:
    static Vector<YSpecialPoint3D*> all_points;
    static HashMap<ObjectID,YSpecialPoint3D*> point_dictionary;
    static YSpecialPoint3D* get_closest_point_to(Vector3 p_global_pos);
    static TypedArray<Vector3> get_rert_path_from_to(Vector3 start, Vector3 end);
    static auto create_connections(float p_max_dist, uint32_t blocking_layermask) -> void;
    static TypedArray<Vector3> get_dijkstra_path_from_to(Vector3 start, Vector3 end);
    Vector<ObjectID> connections;
    Vector<float> distances;
    ObjectID temp_direct_connection;
    Vector<YSpecialPoint3D*> temp_connections;

public:
    float temp_distance = -1.0f;
    int get_connection_count() {
        return static_cast<int>(connections.size());
    }

    YSpecialPoint3D* get_connection_in_index(int index) {
        if (index>=0 && index < get_connection_count()) {
            auto _obj_id = connections[index];
            if (point_dictionary.has(_obj_id))
                return point_dictionary[_obj_id];
        }
        return nullptr;
    }

    bool has_connection(YSpecialPoint3D* p_new_connection) const {
        if (p_new_connection == nullptr) return false;
        auto _obj_id = p_new_connection->get_instance_id();
        return connections.has(_obj_id);
    }
    void add_connection(YSpecialPoint3D* p_new_connection, float p_dist = -1) {
        if (p_new_connection == nullptr) return;
        auto _obj_id = p_new_connection->get_instance_id();
        if (p_dist <-0.01) {
            p_dist = p_new_connection->get_global_position().distance_to(get_global_position());
        }
        connections.push_back(_obj_id);
        distances.push_back(p_dist);
    }

    void remove_connection(YSpecialPoint3D* p_new_connection) {
        if (p_new_connection == nullptr) return;
        auto _obj_id = p_new_connection->get_instance_id();
        connections.erase(_obj_id);
    }

    int point_priority;
    void set_point_priority(int v) {point_priority = v;}
    int get_point_priority() const {return point_priority;}
    
    int point_type;
    void set_point_type(int v) {point_type = v;}
    int get_point_type() const {return point_type;}

    int point_status;
    void set_point_status(int v) {point_status = v;}
    int get_point_status() const {return point_status;}

    static TypedArray<Node> get_points_in_range_of(Vector3 p_global_pos, float p_range);



    static int get_point_count();
    static YSpecialPoint3D* get_point_with_index(int index);
    YSpecialPoint3D() = default;
};



#endif //YSPECIALPOINT3D_H
