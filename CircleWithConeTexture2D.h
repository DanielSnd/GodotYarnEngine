//
// Created by Daniel on 2024-03-06.
//

#ifndef CIRCLEWITHCONETEXTURE2D_H
#define CIRCLEWITHCONETEXTURE2D_H
#include "scene/resources/texture.h"


class CircleWithConeTexture2D : public Texture2D {
    GDCLASS(CircleWithConeTexture2D, Texture2D);

public:

private:
    Ref<Image> use_image;
    Vector<uint8_t> image_data;
    mutable RID texture;

    int width = 64;
    int height = 64;

    Vector2 cone_from = Vector2(0,0);
    Vector2 cone_to = Vector2(1, 0);

    float cone_angle = 0.5;
    float cone_smoothing_angle = 0.1;
    float circle_end_radius = 0.5;
    float circle_start_radius = 0.5;
    float circle_max_radius = 500.0;
    float circle_max_smoothing = 0.5;

    Color color_inside = Color(0.0,0.0,0.0,1.0);
    Color color_outside = Color(1.0,1.0,1.0,1.0);

    float _get_color_at(int x, int y) const;

    bool update_pending = false;
    void _queue_update();
    void _update();

protected:
    static void _bind_methods();

public:
    void set_width(int p_width);
    virtual int get_width() const override;
    void set_height(int p_height);
    virtual int get_height() const override;

    void set_cone_from(Vector2 p_cone_from) {
        cone_from = p_cone_from;
        _queue_update();
        emit_changed();
    }
    Vector2 get_cone_from() { return cone_from;}

    void set_cone_to(Vector2 p_cone_to) {
        cone_to = p_cone_to;
        _queue_update();
        emit_changed();
    }
    Vector2 get_cone_to() { return cone_to; }

    void set_cone_angle(float p_angle) {
        cone_angle = p_angle;
        _queue_update();
        emit_changed();
    }
    virtual float get_cone_angle() { return cone_angle; }

    void set_cone_smoothing_angle(float p_angle) {
        cone_smoothing_angle = p_angle;
        _queue_update();
        emit_changed();
    }
    virtual float get_cone_smoothing_angle() { return cone_smoothing_angle; }

    void set_circle_start_radius(float p_radius) {
        circle_start_radius = p_radius;
        _queue_update();
        emit_changed();
    }
    virtual float get_circle_start_radius() {return circle_start_radius;}
    
    void set_circle_end_radius(float p_radius) {
        circle_end_radius = p_radius;
        _queue_update();
        emit_changed();
    }
    virtual float get_circle_end_radius() {return circle_end_radius;}
    
    void set_circle_max_radius(float p_radius) {
        circle_max_radius = p_radius;
        _queue_update();
        emit_changed();
    }
    virtual float get_circle_max_radius() {return circle_max_radius;}

    void set_circle_max_smoothing(float p_radius) {
        circle_max_smoothing = p_radius;
        _queue_update();
        emit_changed();
    }
    virtual float get_circle_max_smoothing() {return circle_max_smoothing;}

    void set_color_inside(Color p_angle) {
        color_inside = p_angle;
        _queue_update();
        emit_changed();
    }
    virtual Color get_color_inside() { return color_inside; }
    
    void set_color_outside(Color p_angle) {
        color_outside = p_angle;
        _queue_update();
        emit_changed();
    }
    virtual Color get_color_outside() { return color_outside; }

    virtual RID get_rid() const override;
    virtual bool has_alpha() const override { return true; }
    virtual Ref<Image> get_image() const override;
    void update_now();

    CircleWithConeTexture2D();
    virtual ~CircleWithConeTexture2D();
};




#endif //CIRCLEWITHCONETEXTURE2D_H
