#include "yboxcontainer3D.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/resources/3d/primitive_meshes.h"

#ifdef TOOLS_ENABLED
bool YBoxContainer3D::has_gizmo_plugin = false;
#endif

void YBoxContainer3D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_alignment", "alignment"), &YBoxContainer3D::set_alignment);
    ClassDB::bind_method(D_METHOD("get_alignment"), &YBoxContainer3D::get_alignment);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "alignment", PROPERTY_HINT_ENUM, "Begin,Center,End"), "set_alignment", "get_alignment");

    ClassDB::bind_method(D_METHOD("set_positioning", "positioning"), &YBoxContainer3D::set_positioning);
    ClassDB::bind_method(D_METHOD("get_positioning"), &YBoxContainer3D::get_positioning);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "positioning", PROPERTY_HINT_ENUM, "Instant,YTween,Manual"), "set_positioning", "get_positioning");

    ClassDB::bind_method(D_METHOD("set_space_fill_mode", "mode"), &YBoxContainer3D::set_space_fill_mode);
    ClassDB::bind_method(D_METHOD("get_space_fill_mode"), &YBoxContainer3D::get_space_fill_mode);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "space_fill_mode", PROPERTY_HINT_ENUM, "None,Contained,Uniform,Stretch,Proportional,Distribute"), "set_space_fill_mode", "get_space_fill_mode");

    ClassDB::bind_method(D_METHOD("set_container_size", "size"), &YBoxContainer3D::set_container_size);
    ClassDB::bind_method(D_METHOD("get_container_size"), &YBoxContainer3D::get_container_size);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "container_size"), "set_container_size", "get_container_size");

    ClassDB::bind_method(D_METHOD("set_spacing", "spacing"), &YBoxContainer3D::set_spacing);
    ClassDB::bind_method(D_METHOD("get_spacing"), &YBoxContainer3D::get_spacing);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "spacing"), "set_spacing", "get_spacing");

    ClassDB::bind_method(D_METHOD("set_tween_duration", "duration"), &YBoxContainer3D::set_tween_duration);
    ClassDB::bind_method(D_METHOD("get_tween_duration"), &YBoxContainer3D::get_tween_duration);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tween_duration"), "set_tween_duration", "get_tween_duration");

    ClassDB::bind_method(D_METHOD("set_tween_overshoot", "overshoot"), &YBoxContainer3D::set_tween_overshoot);
    ClassDB::bind_method(D_METHOD("get_tween_overshoot"), &YBoxContainer3D::get_tween_overshoot);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tween_overshoot"), "set_tween_overshoot", "get_tween_overshoot");

    ClassDB::bind_method(D_METHOD("set_tween_new_element_offset", "offset"), &YBoxContainer3D::set_tween_new_element_offset);
    ClassDB::bind_method(D_METHOD("get_tween_new_element_offset"), &YBoxContainer3D::get_tween_new_element_offset);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "tween_new_element_offset"), "set_tween_new_element_offset", "get_tween_new_element_offset");

    ClassDB::bind_method(D_METHOD("set_tween_ease_type", "type"), &YBoxContainer3D::set_tween_ease_type);
    ClassDB::bind_method(D_METHOD("get_tween_ease_type"), &YBoxContainer3D::get_tween_ease_type);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "tween_ease_type", PROPERTY_HINT_ENUM, "In,Out,InOut,OutIn"), "set_tween_ease_type", "get_tween_ease_type");

    ClassDB::bind_method(D_METHOD("set_tween_transition_type", "type"), &YBoxContainer3D::set_tween_transition_type);
    ClassDB::bind_method(D_METHOD("get_tween_transition_type"), &YBoxContainer3D::get_tween_transition_type);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "tween_transition_type", PROPERTY_HINT_ENUM, "Linear,Sine,Quint,Quart,Quad,Expo,Elastic,Cubic,Circ,Bounce,Back"), "set_tween_transition_type", "get_tween_transition_type");

    ClassDB::bind_method(D_METHOD("set_vertical", "vertical"), &YBoxContainer3D::set_vertical);
    ClassDB::bind_method(D_METHOD("is_vertical"), &YBoxContainer3D::is_vertical);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "vertical"), "set_vertical", "is_vertical");

    ClassDB::bind_method(D_METHOD("set_layout_axis", "axis"), &YBoxContainer3D::set_layout_axis);
    ClassDB::bind_method(D_METHOD("get_layout_axis"), &YBoxContainer3D::get_layout_axis);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "layout_axis", PROPERTY_HINT_ENUM, "X,Y,Z"), "set_layout_axis", "get_layout_axis");

    BIND_ENUM_CONSTANT(ALIGNMENT_BEGIN);
    BIND_ENUM_CONSTANT(ALIGNMENT_CENTER);
    BIND_ENUM_CONSTANT(ALIGNMENT_END);

    BIND_ENUM_CONSTANT(POSITION_INSTANT);
    BIND_ENUM_CONSTANT(POSITION_YTWEEN);
    BIND_ENUM_CONSTANT(POSITION_MANUAL);

    BIND_ENUM_CONSTANT(SPACE_FILL_NONE);
    BIND_ENUM_CONSTANT(SPACE_FILL_CONTAINED);
    BIND_ENUM_CONSTANT(SPACE_FILL_UNIFORM);
    BIND_ENUM_CONSTANT(SPACE_FILL_STRETCH);
    BIND_ENUM_CONSTANT(SPACE_FILL_PROPORTIONAL);
    BIND_ENUM_CONSTANT(SPACE_FILL_DISTRIBUTE);

    BIND_ENUM_CONSTANT(AXIS_X);
    BIND_ENUM_CONSTANT(AXIS_Y);
    BIND_ENUM_CONSTANT(AXIS_Z);
}

YBoxContainer3D::YBoxContainer3D(bool p_vertical) {
    vertical = p_vertical;
}

YBoxContainer3D::~YBoxContainer3D() {
    tracking_objects.clear();
}

void YBoxContainer3D::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE:
#ifdef TOOLS_ENABLED
            if (!has_gizmo_plugin && Engine::get_singleton()->is_editor_hint()) {
                has_gizmo_plugin = true;
                Ref<YBoxContainer3DGizmoPlugin> gizmo_plugin;
                gizmo_plugin.instantiate();
                Node3DEditor::get_singleton()->add_gizmo_plugin(gizmo_plugin);
            }
#endif
            _resort();
            break;
        case NOTIFICATION_MOVED_IN_PARENT:
            _resort();
            break;
        case NOTIFICATION_TRANSFORM_CHANGED:
            _resort();
            break;
        case NOTIFICATION_CHILD_ORDER_CHANGED:
            _resort();
            break;
    }
}

AABB YBoxContainer3D::get_child_bounds(Node3D *p_child) const {
    if (!p_child) return AABB();
    
    AABB bounds;
    if (MeshInstance3D *mesh = Object::cast_to<MeshInstance3D>(p_child)) {
        if (mesh->get_mesh().is_valid()) {
            bounds = mesh->get_mesh()->get_aabb();
        }
    }
    else if (GeometryInstance3D *geometry = Object::cast_to<GeometryInstance3D>(p_child)) {
        bounds = geometry->get_aabb();
    }
     else {
        if (p_child->get_child_count() > 0) {
            for (int i = 0; i < p_child->get_child_count(); i++) {
                MeshInstance3D* child_mesh = Object::cast_to<MeshInstance3D>(p_child->get_child(i));
                if (child_mesh != nullptr) {
                    if (child_mesh->get_mesh().is_valid()) {
                        bounds = child_mesh->get_mesh()->get_aabb();
                        return bounds;
                    }
                } else if (GeometryInstance3D *child_geometry = Object::cast_to<GeometryInstance3D>(p_child->get_child(i))) {
                    bounds = child_geometry->get_aabb();
                    return bounds;
                }
            }
        }
        // For non-mesh nodes, use a default size
        bounds = AABB(Vector3(), Vector3(1, 1, 1));
    }


    return bounds;
}

void YBoxContainer3D::_resort() {
    struct ChildInfo {
        Node3D* node;
        Vector3 original_scale;
        Vector3 final_scale;
        Vector3 scaled_size;
        Vector3 actual_size; // Add this for distribute/contained modes
        AABB original_bounds;
    };
    
    Vector<ChildInfo> children;
    float total_length = 0.0f;
    Vector3 max_cross_size = Vector3(0, 0, 0);
    
    // First pass: collect child info and calculate total length
    for (int i = 0; i < get_child_count(); i++) {
        Node3D *child = Object::cast_to<Node3D>(get_child(i));
        if (!child) continue;
        
        AABB bounds = get_child_bounds(child);
        Vector3 original_scale = child->get_scale();
        Vector3 final_scale = calculate_child_scale(child, bounds);
        Vector3 scaled_size = bounds.size * final_scale;
        
        // For distribute/contained modes, calculate actual current size
        Vector3 actual_size = bounds.size * original_scale;
        
        // Track maximum cross-sectional size for container overflow checking
        if (layout_axis != AXIS_X) max_cross_size.x = MAX(max_cross_size.x, scaled_size.x);
        if (layout_axis != AXIS_Y) max_cross_size.y = MAX(max_cross_size.y, scaled_size.y);
        if (layout_axis != AXIS_Z) max_cross_size.z = MAX(max_cross_size.z, scaled_size.z);
        
        float length = 0.0f;
        switch (layout_axis) {
            case AXIS_X: length = scaled_size.x; break;
            case AXIS_Y: length = scaled_size.y; break;
            case AXIS_Z: length = scaled_size.z; break;
        }
        total_length += length;
        
        children.push_back({child, original_scale, final_scale, scaled_size, actual_size, bounds});
    }
    
    if (children.size() == 0) return;
    
    // Handle SPACE_FILL_CONTAINED mode
    if (space_fill_mode == SPACE_FILL_CONTAINED) {
        float container_length = 0.0f;
        switch (layout_axis) {
            case AXIS_X: container_length = container_size.x; break;
            case AXIS_Y: container_length = container_size.y; break;
            case AXIS_Z: container_length = container_size.z; break;
        }
        
        // Calculate total size using actual current sizes
        float total_actual_length = 0.0f;
        for (int i = 0; i < children.size(); i++) {
            float length = 0.0f;
            switch (layout_axis) {
                case AXIS_X: length = children[i].actual_size.x; break;
                case AXIS_Y: length = children[i].actual_size.y; break;
                case AXIS_Z: length = children[i].actual_size.z; break;
            }
            total_actual_length += length;
        }
        
        float total_with_spacing = total_actual_length;
        if (children.size() > 1) {
            total_with_spacing += spacing * (children.size() - 1);
        }
        
        // If content fits normally, use normal positioning
        if (total_with_spacing <= container_length) {
            // Use normal positioning logic
            float start_offset = 0.0f;
            switch (alignment) {
                case ALIGNMENT_BEGIN: 
                    start_offset = -container_length * 0.5f; 
                    break;
                case ALIGNMENT_CENTER: 
                    start_offset = -total_with_spacing * 0.5f; 
                    break;
                case ALIGNMENT_END: 
                    start_offset = container_length * 0.5f - total_with_spacing; 
                    break;
            }
            
            // Position children normally
            float current_offset = start_offset;
            for (int i = 0; i < children.size(); i++) {
                Vector3 pos = Vector3(0, 0, 0);
                
                float child_length = 0.0f;
                switch (layout_axis) {
                    case AXIS_X: 
                        child_length = children[i].actual_size.x;
                        pos.x = current_offset + child_length * 0.5f;
                        break;
                    case AXIS_Y: 
                        child_length = children[i].actual_size.y;
                        pos.y = current_offset + child_length * 0.5f;
                        break;
                    case AXIS_Z: 
                        child_length = children[i].actual_size.z;
                        pos.z = current_offset + child_length * 0.5f;
                        break;
                }
                
                // Center on other axes
                if (layout_axis != AXIS_X) pos.x = 0;
                if (layout_axis != AXIS_Y) pos.y = 0;
                if (layout_axis != AXIS_Z) pos.z = 0;
                
                children[i].node->set_position(pos);
                children[i].node->set_scale(children[i].original_scale);
                
                current_offset += child_length + spacing;
            }
        } else {
            // Content doesn't fit - compress objects to fit within container
            // Calculate compression ratio
            float compression_ratio = container_length / total_actual_length;
            
            // Position objects with compressed spacing (can be negative for overlap)
            float half_container = container_length * 0.5f;
            float current_pos = -half_container;
            
            for (int i = 0; i < children.size(); i++) {
                float object_size = 0.0f;
                switch (layout_axis) {
                    case AXIS_X: object_size = children[i].actual_size.x; break;
                    case AXIS_Y: object_size = children[i].actual_size.y; break;
                    case AXIS_Z: object_size = children[i].actual_size.z; break;
                }
                
                // Compress the effective spacing between objects
                float compressed_object_space = object_size * compression_ratio;
                
                Vector3 pos = Vector3(0, 0, 0);
                switch (layout_axis) {
                    case AXIS_X: pos.x = current_pos + compressed_object_space * 0.5f; break;
                    case AXIS_Y: pos.y = current_pos + compressed_object_space * 0.5f; break;
                    case AXIS_Z: pos.z = current_pos + compressed_object_space * 0.5f; break;
                }
                
                if (layout_axis != AXIS_X) pos.x = 0;
                if (layout_axis != AXIS_Y) pos.y = 0;
                if (layout_axis != AXIS_Z) pos.z = 0;
                
                children[i].node->set_position(pos);
                children[i].node->set_scale(children[i].original_scale);
                
                current_pos += compressed_object_space;
            }
        }
        return;
    }
    
    // Handle spacing and distribution for SPACE_FILL_DISTRIBUTE
    if (space_fill_mode == SPACE_FILL_DISTRIBUTE && children.size() > 1) {
        float container_length = 0.0f;
        switch (layout_axis) {
            case AXIS_X: container_length = container_size.x; break;
            case AXIS_Y: container_length = container_size.y; break;
            case AXIS_Z: container_length = container_size.z; break;
        }
        
        // Calculate total size using actual current sizes
        float total_object_size = 0.0f;
        for (int i = 0; i < children.size(); i++) {
            float object_size = 0.0f;
            switch (layout_axis) {
                case AXIS_X: object_size = children[i].actual_size.x; break;
                case AXIS_Y: object_size = children[i].actual_size.y; break;
                case AXIS_Z: object_size = children[i].actual_size.z; break;
            }
            total_object_size += object_size;
        }
        
        // Check if objects fit in container
        if (total_object_size > container_length) {
            // Objects don't fit - compress them to fit within container
            float compression_ratio = container_length / total_object_size;
            
            float half_container = container_length * 0.5f;
            float current_pos = -half_container;
            
            for (int i = 0; i < children.size(); i++) {
                float object_size = 0.0f;
                switch (layout_axis) {
                    case AXIS_X: object_size = children[i].actual_size.x; break;
                    case AXIS_Y: object_size = children[i].actual_size.y; break;
                    case AXIS_Z: object_size = children[i].actual_size.z; break;
                }
                
                // Compress the effective spacing between objects
                float compressed_object_space = object_size * compression_ratio;
                
                Vector3 pos = Vector3(0, 0, 0);
                switch (layout_axis) {
                    case AXIS_X: pos.x = current_pos + compressed_object_space * 0.5f; break;
                    case AXIS_Y: pos.y = current_pos + compressed_object_space * 0.5f; break;
                    case AXIS_Z: pos.z = current_pos + compressed_object_space * 0.5f; break;
                }
                
                if (layout_axis != AXIS_X) pos.x = 0;
                if (layout_axis != AXIS_Y) pos.y = 0;
                if (layout_axis != AXIS_Z) pos.z = 0;
                
                children[i].node->set_position(pos);
                children[i].node->set_scale(children[i].original_scale);
                
                current_pos += compressed_object_space;
            }
            return;
        }
        
        // Calculate available space for gaps between objects
        float available_space = container_length - total_object_size;
        float gap_size = available_space / (children.size() - 1);
        
        // Position objects with proper spacing
        float half_container = container_length * 0.5f;
        float current_pos = -half_container;
        
        for (int i = 0; i < children.size(); i++) {
            float object_size = 0.0f;
            switch (layout_axis) {
                case AXIS_X: object_size = children[i].actual_size.x; break;
                case AXIS_Y: object_size = children[i].actual_size.y; break;
                case AXIS_Z: object_size = children[i].actual_size.z; break;
            }
            
            Vector3 pos = Vector3(0, 0, 0);
            
            // For first object, align to start edge
            if (i == 0) {
                switch (layout_axis) {
                    case AXIS_X: pos.x = current_pos + object_size * 0.5f; break;
                    case AXIS_Y: pos.y = current_pos + object_size * 0.5f; break;
                    case AXIS_Z: pos.z = current_pos + object_size * 0.5f; break;
                }
                current_pos += object_size + gap_size;
            }
            // For last object, align to end edge
            else if (i == children.size() - 1) {
                switch (layout_axis) {
                    case AXIS_X: pos.x = half_container - object_size * 0.5f; break;
                    case AXIS_Y: pos.y = half_container - object_size * 0.5f; break;
                    case AXIS_Z: pos.z = half_container - object_size * 0.5f; break;
                }
            }
            // For middle objects, use calculated position
            else {
                switch (layout_axis) {
                    case AXIS_X: pos.x = current_pos + object_size * 0.5f; break;
                    case AXIS_Y: pos.y = current_pos + object_size * 0.5f; break;
                    case AXIS_Z: pos.z = current_pos + object_size * 0.5f; break;
                }
                current_pos += object_size + gap_size;
            }
            
            // Center on other axes
            if (layout_axis != AXIS_X) pos.x = 0;
            if (layout_axis != AXIS_Y) pos.y = 0;
            if (layout_axis != AXIS_Z) pos.z = 0;
            
            children[i].node->set_position(pos);
            children[i].node->set_scale(children[i].original_scale);
        }
        return;
    }


    
    // Logic for other space fill modes
    if (children.size() > 1 && space_fill_mode != SPACE_FILL_DISTRIBUTE) {
        total_length += spacing * (children.size() - 1);
    }
    
    // Check if content overflows container and adjust scaling if needed
    float container_length = 0.0f;
    switch (layout_axis) {
        case AXIS_X: container_length = container_size.x; break;
        case AXIS_Y: container_length = container_size.y; break;
        case AXIS_Z: container_length = container_size.z; break;
    }
    
    float scale_factor = 1.0f;
    if (total_length > container_length && space_fill_mode != SPACE_FILL_DISTRIBUTE) {
        scale_factor = container_length / total_length;
        
        // Rescale all children proportionally
        for (int i = 0; i < children.size(); i++) {
            children.write[i].final_scale = children[i].final_scale * scale_factor;
            children.write[i].scaled_size = children[i].scaled_size * scale_factor;
        }
        total_length = container_length;
    }
    
    // Calculate alignment offset
    float start_offset = 0.0f;
    switch (alignment) {
        case ALIGNMENT_BEGIN: 
            start_offset = -container_length * 0.5f; 
            break;
        case ALIGNMENT_CENTER: 
            start_offset = -total_length * 0.5f; 
            break;
        case ALIGNMENT_END: 
            start_offset = container_length * 0.5f - total_length; 
            break;
    }
    
    // Position and scale children
    float current_offset = start_offset;
    for (int i = 0; i < children.size(); i++) {
        Vector3 pos = Vector3(0, 0, 0);
        
        // Calculate position along layout axis
        float child_length = 0.0f;
        switch (layout_axis) {
            case AXIS_X: 
                child_length = children[i].scaled_size.x;
                pos.x = current_offset + child_length * 0.5f;
                break;
            case AXIS_Y: 
                child_length = children[i].scaled_size.y;
                pos.y = current_offset + child_length * 0.5f;
                break;
            case AXIS_Z: 
                child_length = children[i].scaled_size.z;
                pos.z = current_offset + child_length * 0.5f;
                break;
        }
        
        // Center on other axes within container bounds
        if (layout_axis != AXIS_X) pos.x = 0;
        if (layout_axis != AXIS_Y) pos.y = 0;
        if (layout_axis != AXIS_Z) pos.z = 0;
        
        // Apply transform
        children[i].node->set_position(pos);
        children[i].node->set_scale(children[i].final_scale);
        
        // Advance offset
        current_offset += child_length + spacing;
    }
}

Vector3 YBoxContainer3D::calculate_child_scale(Node3D *p_child, const AABB &p_bounds) const {
    if (!p_child || space_fill_mode == SPACE_FILL_NONE || space_fill_mode == SPACE_FILL_DISTRIBUTE || space_fill_mode == SPACE_FILL_CONTAINED) {
        return Vector3(1, 1, 1);
    }

    Vector3 child_size = p_bounds.size;
    if (child_size.x <= 0 || child_size.y <= 0 || child_size.z <= 0) {
        return Vector3(1, 1, 1);
    }
    
    Vector3 available_size = container_size;
    Vector3 scale_factors = Vector3(
        available_size.x / child_size.x,
        available_size.y / child_size.y,
        available_size.z / child_size.z
    );

    switch (space_fill_mode) {
        case SPACE_FILL_UNIFORM: {
            float min_scale = MIN(scale_factors.x, MIN(scale_factors.y, scale_factors.z));
            return Vector3(min_scale, min_scale, min_scale);
        }
        case SPACE_FILL_STRETCH: {
            // Don't stretch along layout axis for proper spacing
            if (layout_axis == AXIS_X) scale_factors.x = 1.0f;
            if (layout_axis == AXIS_Y) scale_factors.y = 1.0f;
            if (layout_axis == AXIS_Z) scale_factors.z = 1.0f;
            return scale_factors;
        }
        case SPACE_FILL_PROPORTIONAL: {
            float avg_scale = (scale_factors.x + scale_factors.y + scale_factors.z) / 3.0f;
            return Vector3(avg_scale, avg_scale, avg_scale);
        }
        default:
            return Vector3(1, 1, 1);
    }
}

// Getters and setters
void YBoxContainer3D::set_alignment(AlignmentMode p_alignment) {
    if (alignment == p_alignment) return;
    alignment = p_alignment;
    _resort();
}

YBoxContainer3D::AlignmentMode YBoxContainer3D::get_alignment() const {
    return alignment;
}

void YBoxContainer3D::set_positioning(PositioningMethod p_positioning) {
    positioning = p_positioning;
}

YBoxContainer3D::PositioningMethod YBoxContainer3D::get_positioning() const {
    return positioning;
}

void YBoxContainer3D::set_space_fill_mode(SpaceFillMode p_mode) {
    if (space_fill_mode == p_mode) return;
    space_fill_mode = p_mode;
    _resort();
}

YBoxContainer3D::SpaceFillMode YBoxContainer3D::get_space_fill_mode() const {
    return space_fill_mode;
}

void YBoxContainer3D::set_container_size(Vector3 p_size) {
    if (container_size == p_size) return;
    container_size = p_size;
    _resort();
#ifdef TOOLS_ENABLED
    update_gizmos();
#endif
}

Vector3 YBoxContainer3D::get_container_size() const {
    return container_size;
}

void YBoxContainer3D::set_spacing(float p_spacing) {
    if (spacing == p_spacing) return;
    spacing = p_spacing;
    _resort();
}

float YBoxContainer3D::get_spacing() const {
    return spacing;
}

void YBoxContainer3D::set_tween_duration(float p_duration) {
    tween_duration = p_duration;
}

float YBoxContainer3D::get_tween_duration() const {
    return tween_duration;
}

void YBoxContainer3D::set_tween_overshoot(float p_overshoot) {
    tween_overshoot = p_overshoot;
}

float YBoxContainer3D::get_tween_overshoot() const {
    return tween_overshoot;
}

void YBoxContainer3D::set_tween_new_element_offset(Vector3 p_offset) {
    new_element_offset = p_offset;
}

Vector3 YBoxContainer3D::get_tween_new_element_offset() const {
    return new_element_offset;
}

void YBoxContainer3D::set_tween_ease_type(Tween::EaseType p_type) {
    tween_ease_type = p_type;
}

Tween::EaseType YBoxContainer3D::get_tween_ease_type() const {
    return tween_ease_type;
}

void YBoxContainer3D::set_tween_transition_type(Tween::TransitionType p_type) {
    tween_transition_type = p_type;
}

Tween::TransitionType YBoxContainer3D::get_tween_transition_type() const {
    return tween_transition_type;
}

void YBoxContainer3D::set_vertical(bool p_vertical) {
    if (vertical == p_vertical) return;
    vertical = p_vertical;
    _resort();
}

bool YBoxContainer3D::is_vertical() const {
    return vertical;
}

void YBoxContainer3D::set_layout_axis(LayoutAxis p_axis) {
    if (layout_axis == p_axis) return;
    layout_axis = p_axis;
    _resort();
#ifdef TOOLS_ENABLED    
    update_gizmos();
#endif
}

YBoxContainer3D::LayoutAxis YBoxContainer3D::get_layout_axis() const {
    return layout_axis;
}

Ref<YTweenWrap> YBoxContainer3D::animated_free_child(Node3D* child, Vector3 exiting_offset, float duration) {
    if (!child) return Ref<YTweenWrap>();
    
    if (YTween::get_singleton() != nullptr) {
        auto new_tween = YTween::get_singleton()->create_unique_tween(child);
        new_tween->set_parallel(true);
#ifdef YGODOT
        new_tween->tween_property(child, NodePath("position"), child->get_position() + exiting_offset, duration)->set_ease(tween_ease_type)->set_trans(tween_transition_type, tween_overshoot);
        new_tween->tween_property(child, NodePath("modulate"), Color(1.0, 1.0, 1.0, 0.0), duration)->set_ease(tween_ease_type)->set_trans(tween_transition_type, tween_overshoot);
#else
        new_tween->tween_property(child, NodePath("position"), child->get_position() + exiting_offset, duration)->set_ease(tween_ease_type)->set_trans(tween_transition_type);
        new_tween->tween_property(child, NodePath("modulate"), Color(1.0, 1.0, 1.0, 0.0), duration)->set_ease(tween_ease_type)->set_trans(tween_transition_type);
#endif
        new_tween->connect("finished_or_killed", callable_mp(static_cast<Node*>(child), &Node::queue_free));
        return new_tween;
    }
    return Ref<YTweenWrap>();
}

void YBoxContainer3D::_validate_property(PropertyInfo &p_property) const {
    if (is_fixed && p_property.name == "vertical") {
        p_property.usage = PROPERTY_USAGE_NONE;
    }
    if (p_property.name.begins_with("tween_") && positioning != POSITION_YTWEEN) {
        p_property.usage = PROPERTY_USAGE_NONE;
    }
}
