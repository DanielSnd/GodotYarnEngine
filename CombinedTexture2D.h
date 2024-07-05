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

    Ref<Texture2D> TextureBack;
    Ref<Texture2D> TextureFront;

    Vector<uint8_t> image_data;
    mutable RID texture;

    int width = 64;
    int height = 64;

    Color _get_color_at(int x, int y) const;

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

    void set_texture_back(const Ref<Texture2D> &p_texture_back);

    Ref<Texture2D> get_texture_back() const;

    void set_texture_front(const Ref<Texture2D> &p_texture_front);

    Ref<Texture2D> get_texture_front() const;

    virtual bool has_alpha() const override { return true; }
    virtual Ref<Image> get_image() const override;
    void update_now();

    CombinedTexture2D();
    virtual ~CombinedTexture2D();
};


#endif //COMBINEDTEXTURE2D_H
