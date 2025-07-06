#ifndef YBOX_CONTAINER_3D_H
#define YBOX_CONTAINER_3D_H

#include "scene/3d/node_3d.h"
#include "ytween.h"
#ifdef TOOLS_ENABLED
#include "editor/settings/editor_settings.h"
#include "editor/scene/3d/node_3d_editor_plugin.h"
#include "editor/yboxcontainer3d_gizmoplugin.h"
#endif

class YTween;

class YBoxContainer3D : public Node3D {
    GDCLASS(YBoxContainer3D, Node3D);

public:
    enum AlignmentMode {
        ALIGNMENT_BEGIN,
        ALIGNMENT_CENTER,
        ALIGNMENT_END
    };

    enum LayoutAxis {
        AXIS_X,
        AXIS_Y,
        AXIS_Z
    };

    enum PositioningMethod {
        POSITION_INSTANT,
        POSITION_YTWEEN,
        POSITION_MANUAL
    };

    enum SpaceFillMode {
        SPACE_FILL_NONE = 0,           // Objects maintain their original size
        SPACE_FILL_CONTAINED = 1,       // Like None but prevents objects from leaving container bounds
        SPACE_FILL_UNIFORM = 2,        // Objects are scaled uniformly to fit
        SPACE_FILL_STRETCH = 3,        // Objects are stretched to fill the space
        SPACE_FILL_PROPORTIONAL = 4,   // Objects are scaled proportionally to their original size
        SPACE_FILL_DISTRIBUTE = 5     // Objects maintain scale 1,1,1 but are distributed across space
    };

private:
    bool vertical = false;
    AlignmentMode alignment = ALIGNMENT_BEGIN;
    PositioningMethod positioning = POSITION_INSTANT;
    SpaceFillMode space_fill_mode = SPACE_FILL_UNIFORM;
    
    Tween::EaseType tween_ease_type = Tween::EaseType::EASE_IN_OUT;
    Tween::TransitionType tween_transition_type = Tween::TransitionType::TRANS_QUAD;
    float tween_duration = 0.4;
    float tween_overshoot = 1.2;
    Vector3 new_element_offset = Vector3(0.0, 1.0, 0.0);
    
    Vector<uint64_t> tracking_objects;
    Vector3 container_size = Vector3(1.0, 1.0, 1.0);
    float spacing = 0.1;

    void position_child_in_space(Node3D *p_child, const AABB &p_bounds);
    AABB get_child_bounds(Node3D *p_child) const;
    Vector3 calculate_child_scale(Node3D *p_child, const AABB &p_bounds) const;

protected:
    bool is_fixed = false;

    LayoutAxis layout_axis = AXIS_Y;
    void _notification(int p_what);
    void _validate_property(PropertyInfo &p_property) const;
    static void _bind_methods();
#ifdef TOOLS_ENABLED
    static bool has_gizmo_plugin;
#endif
public:
    void _resort();
    
    void set_layout_axis(LayoutAxis p_axis);
    LayoutAxis get_layout_axis() const;

    void set_alignment(AlignmentMode p_alignment);
    AlignmentMode get_alignment() const;

    void set_positioning(PositioningMethod p_positioning);
    PositioningMethod get_positioning() const;

    void set_space_fill_mode(SpaceFillMode p_mode);
    SpaceFillMode get_space_fill_mode() const;

    void set_container_size(Vector3 p_size);
    Vector3 get_container_size() const;

    void set_spacing(float p_spacing);
    float get_spacing() const;

    void set_tween_duration(float p_duration);
    float get_tween_duration() const;
    
    void set_tween_overshoot(float p_overshoot);
    float get_tween_overshoot() const;

    void set_tween_new_element_offset(Vector3 p_offset);
    Vector3 get_tween_new_element_offset() const;

    void set_tween_ease_type(Tween::EaseType p_type);
    Tween::EaseType get_tween_ease_type() const;

    void set_tween_transition_type(Tween::TransitionType p_type);
    Tween::TransitionType get_tween_transition_type() const;

    Ref<YTweenWrap> animated_free_child(Node3D* child, Vector3 exiting_offset, float duration);

    void set_vertical(bool p_vertical);
    bool is_vertical() const;

    void queue_sort();
    bool pending_sort = false;


    YBoxContainer3D(bool p_vertical = false);
    ~YBoxContainer3D();
};

class YHBoxContainer3D : public YBoxContainer3D {
    GDCLASS(YHBoxContainer3D, YBoxContainer3D);

public:
    YHBoxContainer3D() : YBoxContainer3D(false) { is_fixed = true; layout_axis = AXIS_X; }
};

class YVBoxContainer3D : public YBoxContainer3D {
    GDCLASS(YVBoxContainer3D, YBoxContainer3D);

public:
    YVBoxContainer3D() : YBoxContainer3D(true) { is_fixed = true; layout_axis = AXIS_Y; }
};

VARIANT_ENUM_CAST(YBoxContainer3D::AlignmentMode);
VARIANT_ENUM_CAST(YBoxContainer3D::PositioningMethod);
VARIANT_ENUM_CAST(YBoxContainer3D::SpaceFillMode);
VARIANT_ENUM_CAST(YBoxContainer3D::LayoutAxis);

#endif // YBOX_CONTAINER_3D_H