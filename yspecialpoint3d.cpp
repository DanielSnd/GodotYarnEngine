//
// Created by Daniel on 2024-03-19.
//

#include "yspecialpoint3d.h"

#include "yarnphysics.h"
#include "core/math/random_number_generator.h"

Vector<YSpecialPoint3D *> YSpecialPoint3D::all_points;
HashMap<ObjectID, YSpecialPoint3D *> YSpecialPoint3D::point_dictionary;

void YSpecialPoint3D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_point_priority"), &YSpecialPoint3D::get_point_priority);
    ClassDB::bind_method(D_METHOD("set_point_priority","point_priority"), &YSpecialPoint3D::set_point_priority,DEFVAL(0));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "point_priority"), "set_point_priority", "get_point_priority");

    ClassDB::bind_method(D_METHOD("get_point_type"), &YSpecialPoint3D::get_point_type);
    ClassDB::bind_method(D_METHOD("set_point_type","point_type"), &YSpecialPoint3D::set_point_type,DEFVAL(0));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "point_type"), "set_point_type", "get_point_type");
    
    ClassDB::bind_method(D_METHOD("get_point_status"), &YSpecialPoint3D::get_point_status);
    ClassDB::bind_method(D_METHOD("set_point_status","point_status"), &YSpecialPoint3D::set_point_status,DEFVAL(0));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "point_status"), "set_point_status", "get_point_status");

    ClassDB::bind_method(D_METHOD("get_connection_count"), &YSpecialPoint3D::get_connection_count);
    ClassDB::bind_method(D_METHOD("get_connection_in_index","index"), &YSpecialPoint3D::get_connection_in_index);
    ClassDB::bind_method(D_METHOD("add_connection","yspecialpoint3d","dist"), &YSpecialPoint3D::add_connection,DEFVAL(-1.0));
    ClassDB::bind_method(D_METHOD("has_connection","yspecialpoint3d"), &YSpecialPoint3D::has_connection);
    ClassDB::bind_method(D_METHOD("remove_connection","yspecialpoint3d"), &YSpecialPoint3D::remove_connection);

    ClassDB::bind_static_method("YSpecialPoint3D",D_METHOD("create_connections","max_dist","blocking_collisionmask"), &YSpecialPoint3D::create_connections,DEFVAL(UINT32_MAX));
    ClassDB::bind_static_method("YSpecialPoint3D",D_METHOD("get_closest_point_to","global_pos"), &YSpecialPoint3D::get_closest_point_to);
    ClassDB::bind_static_method("YSpecialPoint3D",D_METHOD("get_point_count"), &YSpecialPoint3D::get_point_count);
    ClassDB::bind_static_method("YSpecialPoint3D",D_METHOD("get_point_with_index","index"), &YSpecialPoint3D::get_point_with_index);
    ClassDB::bind_static_method("YSpecialPoint3D",D_METHOD("get_points_in_range_of","origin","range"), &YSpecialPoint3D::get_points_in_range_of);

    ClassDB::bind_static_method("YSpecialPoint3D",D_METHOD("get_dijkstra_path_from_to","start","end"), &YSpecialPoint3D::get_dijkstra_path_from_to);
}

void YSpecialPoint3D::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
            all_points.push_back(this);
            point_dictionary[get_instance_id()] = this;
        } break;
        case NOTIFICATION_EXIT_TREE: {
            if (all_points.has(this)) all_points.erase(this);
            auto temp_instance_id = get_instance_id();
            if (point_dictionary.has(temp_instance_id))
                point_dictionary.erase(temp_instance_id);
        } break;
    }
}

YSpecialPoint3D * YSpecialPoint3D::get_closest_point_to(Vector3 p_global_pos) {
    if (all_points.is_empty()) return nullptr;
    float best_distance = 9999.0;
    YSpecialPoint3D* return_ypoint = nullptr;
    for (auto ypoint_testing: all_points) {
        if (ypoint_testing == nullptr) continue;
        const float test_distance = ypoint_testing->get_global_position().distance_to(p_global_pos);
        if (best_distance > test_distance) {
            best_distance = test_distance;
            return_ypoint = ypoint_testing;
        }
    }
    return return_ypoint;
}

TypedArray<Node> YSpecialPoint3D::get_points_in_range_of(const Vector3 p_global_pos, const float p_range) {
    TypedArray<Node> return_points;
    for (auto return_point: all_points) {
        if (return_point != nullptr) {
            const Vector3 return_point_global_pos = return_point->get_global_position();
            if (return_point_global_pos.distance_to(p_global_pos) <= p_range) {
                return_points.push_back(return_point);
            }
        }
    }
    return return_points;
}


struct LessComparator {
    bool operator()(YSpecialPoint3D* p1, YSpecialPoint3D* p2) const {
        return p1->temp_distance < p2->temp_distance;
    }
};

TypedArray<Vector3> YSpecialPoint3D::get_dijkstra_path_from_to(Vector3 start, Vector3 end) {
    TypedArray<Vector3> return_path;
    if (all_points.is_empty()) return return_path;
    int max_iterations = 2000;
    YSpecialPoint3D* start_point = get_closest_point_to(start);
    YSpecialPoint3D* end_point = get_closest_point_to(end);
    Dictionary previous;
    Vector<YSpecialPoint3D*> queue;

    queue.push_back(start_point);
    for (auto yp: all_points) {
        if (yp != nullptr) yp->temp_distance = -1.0f;
    }
    start_point->temp_distance = 0.0;
    while(!queue.is_empty()) {
        queue.sort_custom<LessComparator>();
        YSpecialPoint3D* current_point = queue[0];
        queue.remove_at(0);

        if(current_point == end_point) {
            break;
        }

        for(int i = 0; i < current_point->connections.size(); ++i) {
            if (!point_dictionary.has(current_point->connections[i])) continue;
            YSpecialPoint3D* adj_point = point_dictionary[current_point->connections[i]];
            if (adj_point == nullptr) continue;
            float adj_distance = current_point->distances[i];
            float new_dist = current_point->temp_distance + adj_distance;
            if (new_dist > 1e8) {
                break;
            }
            if (adj_point->temp_distance < -0.01 || new_dist < adj_point->temp_distance) {
                adj_point->temp_distance = new_dist;
                previous[adj_point] = current_point;
                queue.push_back(adj_point);
            }
        }

        if (--max_iterations <= 0) {
            print_line("DIJKSTRA'S ALGORITHM failed to find path");
            return return_path;
        }
    }

    for(Variant p = end_point; p.get_type() != Variant::NIL; p = previous[p]) {
        auto ypoint = dynamic_cast<YSpecialPoint3D*>(p.operator Object*());
        return_path.push_back(ypoint != nullptr ? ypoint->get_global_position() : Vector3{0.0,0.0,0.0});
    }

    return_path.reverse();
    return return_path;
}


TypedArray<Vector3> YSpecialPoint3D::get_rert_path_from_to(Vector3 start, Vector3 end) {
    TypedArray<Vector3> return_path;
    if (all_points.is_empty()) return return_path;
    Vector<YSpecialPoint3D*> visited;

    YSpecialPoint3D* starting_point = get_closest_point_to(start);
    YSpecialPoint3D* ending_point = get_closest_point_to(end);
    visited.push_back(ending_point);

    if (starting_point == nullptr || ending_point == nullptr) return return_path;

    YSpecialPoint3D* current_checking = starting_point;

    Ref<RandomNumberGenerator> rd;
    rd.instantiate();
    int attempts = 99999;
    while (current_checking != ending_point && attempts > 0) {
        YSpecialPoint3D* rnd = all_points[rd->randi_range(0,static_cast<int>(all_points.size()))]; // Get random point

        attempts--;
        if (rnd == nullptr)
            continue;

        // Get closest node in visited list to the random point
        YSpecialPoint3D* closest = nullptr;
        float minClosestDist = FLT_MAX;
        for (YSpecialPoint3D* p : visited) {
            float dist = p->get_global_position().distance_squared_to(rnd->get_global_position());
            if (dist < minClosestDist) {
                minClosestDist = dist;
                closest = p;
            }
        }
        // Add to connections
        closest->temp_connections.push_back(rnd);
        rnd->temp_connections.push_back(closest);

        // Set current node for next iteration and add to visited
        current_checking = rnd;
        visited.push_back(rnd);
    }

    // Return visited as godot::Array
    for (YSpecialPoint3D * p : visited)
        return_path.push_back(p->get_global_position());

    return return_path;
}

void YSpecialPoint3D::create_connections(float p_max_dist,uint32_t blocking_layermask) {
    const int point_count = get_point_count();
    for (auto ypoint: all_points) {
        if (ypoint == nullptr) continue;
        for (auto other_ypoint: all_points) {
            if (other_ypoint == nullptr) continue;
            if (ypoint->connections.has(other_ypoint->get_instance_id()))
                continue;
            const float distance_between_points = other_ypoint->get_global_position().distance_to(ypoint->get_global_position());
            if (distance_between_points > p_max_dist) continue;
            if (YPhysics::get_singleton()->free_line_check(ypoint->get_global_position(),other_ypoint->get_global_position(),YPhysics::COLLIDE_WITH_BODIES,blocking_layermask))
                continue;
            ypoint->add_connection(other_ypoint,distance_between_points);
            other_ypoint->add_connection(ypoint,distance_between_points);
        }
    }
}

int YSpecialPoint3D::get_point_count() {
        return static_cast<int>(all_points.size());
}

YSpecialPoint3D * YSpecialPoint3D::get_point_with_index(int index) {
        if (index < 0 || index >= get_point_count()) return nullptr;
        return all_points[index];
}

YSpecialPoint3D::YSpecialPoint3D(): point_priority(0), point_type(0), point_status(0) {
}
