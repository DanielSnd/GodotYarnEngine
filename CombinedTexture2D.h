//
// Created by Daniel on 2024-07-04.
//

#ifndef COMBINEDTEXTURE2D_H
#define COMBINEDTEXTURE2D_H

#include "scene/resources/texture.h"

class CombinedTexture2D : public Texture2D {
    GDCLASS(CombinedTexture2D, Texture2D);

public:

private:
    Ref<Image> use_image;
    Ref<Image> ImageBack;
    Ref<Image> ImageFront;
    Ref<Image> ImageOverlay;

    Ref<Texture2D> TextureBack;
    Ref<Texture2D> TextureFront;
    Ref<Texture2D> TextureOverlay;

    Vector<uint8_t> image_data;
    mutable RID texture;

    int width = 64;
    int height = 64;

    Vector4 _get_color_at(int x, int y, float aspect_ration, bool has_texture_overlay) const;

    bool show_based_on_back_alpha = false;
    bool only_use_front_full_alpha = false;
    bool black_on_back_overrides_front = false;
    bool front_flips_h = false;

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

    virtual RID get_rid() const override;

    Color modulate_back = {1.0,1.0,1.0,1.0};
    Color modulate_front = {1.0,1.0,1.0,1.0};
    Vector3 additive_back = {0.0,0.0,0.0};
    Vector3 additive_front = {0.0,0.0,0.0};
    Vector2 back_offset = {0.0,0.0};
    Vector2 back_scale = {1.0,1.0};
    Vector2 front_offset = {0.0,0.0};
    Vector2 front_scale = {1.0,1.0};
    Vector4i front_crop_out = {0,0,0,0};
    int black_on_black_overrides_under = 0;
    int show_based_on_back_alpha_under = 0;

    int get_show_based_on_back_alpha_under() const { return show_based_on_back_alpha_under; }
    void set_show_based_on_back_alpha_under(int p_show_based_on_back_alpha_under);

    bool get_show_based_on_back_alpha() const { return show_based_on_back_alpha; }

    void set_show_based_on_back_alpha(bool p_show_based);

    bool get_only_use_front_full_alpha() const { return only_use_front_full_alpha; }

    void set_only_use_front_full_alpha(bool p_only_use_front_full_alpha);

    bool get_black_on_back_overrides_front() const { return black_on_back_overrides_front; }
    void set_black_on_back_overrides_front(bool p_black_on_back_overrides_front);

    int get_black_on_black_overrides_under() const { return black_on_black_overrides_under; }
    void set_black_on_black_overrides_under(int p_black_on_black_overrides_under);

    bool get_front_flips_h() const { return front_flips_h; }
    void set_front_flips_h(bool p_front_flips_h);

    Vector2 get_front_offset() const { return front_offset; }

    void set_front_offset(Vector2 offset);

    Vector2 get_front_scale() const { return front_scale; }

    void set_front_scale(Vector2 scale);

    Vector2 get_back_offset() const { return back_offset; }

    void set_back_offset(Vector2 offset);

    Vector2 get_back_scale() const { return back_scale; }

    void set_back_scale(Vector2 scale) ;

    void set_modulate_back(Color p_color);
    void set_modulate_front(Color p_color);

    void set_additive_back(Vector3 p_vector);

    void set_additive_front(Vector3 p_vector);

    Vector3 get_additive_back() const;

    Vector3 get_additive_front() const;

    Color get_modulate_front() const {
        return modulate_front;
    }
    Color get_modulate_back() const {
        return modulate_back;
    }

    Vector4i get_front_crop_out() const { return front_crop_out; }
    void set_front_crop_out(Vector4i crop_under);

    void set_texture_back(const Ref<Texture2D> &p_texture_back);

    Ref<Texture2D> get_texture_back() const;

    void set_texture_front(const Ref<Texture2D> &p_texture_front);
    Ref<Texture2D> get_texture_front() const;

    void set_texture_overlay(const Ref<Texture2D> &p_texture_overlay);
    Ref<Texture2D> get_texture_overlay() const;

    virtual bool has_alpha() const override { return true; }
    virtual Ref<Image> get_image() const override;
    void update_now();

    CombinedTexture2D();
    virtual ~CombinedTexture2D();
};


#endif //COMBINEDTEXTURE2D_H
