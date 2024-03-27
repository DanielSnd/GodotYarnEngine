//
// Created by Daniel on 2024-03-27.
//

#ifndef YBOX_CONTAINER_H
#define YBOX_CONTAINER_H

#include "scene/gui/container.h"
#include "ytween.h"
class YTween;

class YBoxContainer : public Container {
    GDCLASS(YBoxContainer, Container);

public:
    enum AlignmentMode {
        ALIGNMENT_BEGIN,
        ALIGNMENT_CENTER,
        ALIGNMENT_END
    };

    enum PositioningMethod {
	    POSITION_INSTANT,
        POSITION_YTWEEN,
        POSITION_MANUAL
    };

private:
    bool vertical = false;
    AlignmentMode alignment = ALIGNMENT_BEGIN;
    PositioningMethod positioning = POSITION_INSTANT;
    Tween::EaseType tween_ease_type = Tween::EaseType::EASE_IN_OUT;
    Tween::TransitionType tween_transition_type = Tween::TransitionType::TRANS_QUAD;
    float tween_duration = 0.4;
    float tween_overshoot = 1.2;
    Vector2 new_element_offset = Vector2(0.0,1.0);
    Vector<uint64_t> tracking_objects;
    struct ThemeCache {
        int separation = 0;
    } theme_cache;

    void _resort();

    void position_child_in_rect(Control *p_child, const Rect2 &p_rect);

protected:
    bool is_fixed = false;

    void _notification(int p_what);
    void _validate_property(PropertyInfo &p_property) const;
    static void _bind_methods();

public:
    Control *add_spacer(bool p_begin = false);

    void set_alignment(AlignmentMode p_alignment);
    AlignmentMode get_alignment() const;

    void set_positioning(PositioningMethod p_positioning) {positioning = p_positioning; notify_property_list_changed();}
    PositioningMethod get_positioning() const { return positioning;}

    void set_tween_duration(float p_positioning) {tween_duration = p_positioning;}
    float get_tween_duration() const {return tween_duration;}
    
    void set_tween_overshoot(float p_positioning) {tween_overshoot = p_positioning;}
    float get_tween_overshoot() const {return tween_overshoot;}

    void set_tween_new_element_offset(Vector2 p_positioning) {new_element_offset = p_positioning;}
    Vector2 get_tween_new_element_offset() const {return new_element_offset;}

    void set_tween_ease_type(Tween::EaseType p_positioning) {tween_ease_type = p_positioning;}
    Tween::EaseType get_tween_ease_type() const {return tween_ease_type;}

    void set_tween_transition_type(Tween::TransitionType p_positioning) {tween_transition_type = p_positioning;}
    Tween::TransitionType get_tween_transition_type() const {return tween_transition_type;}

    Ref<Tween> animated_free_child(Control* child, Vector2 exiting_offset, float duration);



    void set_vertical(bool p_vertical);
    bool is_vertical() const;

    virtual Size2 get_minimum_size() const override;

    virtual Vector<int> get_allowed_size_flags_horizontal() const override;
    virtual Vector<int> get_allowed_size_flags_vertical() const override;

    YBoxContainer(bool p_vertical = false);
    ~YBoxContainer() {
        tracking_objects.clear();
    }
};

class YHBoxContainer : public YBoxContainer {
    GDCLASS(YHBoxContainer, YBoxContainer);

public:
    YHBoxContainer() :
            YBoxContainer(false) { is_fixed = true; }
};

class MarginContainer;
class YVBoxContainer : public YBoxContainer {
    GDCLASS(YVBoxContainer, YBoxContainer);

public:
    MarginContainer *add_margin_child(const String &p_label, Control *p_control, bool p_expand = false);

    YVBoxContainer() :
            YBoxContainer(true) { is_fixed = true; }
};

VARIANT_ENUM_CAST(YBoxContainer::AlignmentMode);
VARIANT_ENUM_CAST(YBoxContainer::PositioningMethod);

#endif // YBOX_CONTAINER_H
