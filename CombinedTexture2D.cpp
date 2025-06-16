//
// Created by Daniel on 2024-07-04.
//

#include "CombinedTexture2D.h"

CombinedTexture2D::CombinedTexture2D() {
	_queue_update();
}

CombinedTexture2D::~CombinedTexture2D() {
	if (use_image.is_valid()) {
		use_image = nullptr;	
	}
	if (texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RS::get_singleton()->free(texture);
		texture = RID();
	}
}

void CombinedTexture2D::_queue_update() {
	if (update_pending) {
		return;
	}
	update_pending = true;
	callable_mp(this, &CombinedTexture2D::update_now).call_deferred();
}

void CombinedTexture2D::_update() {
	update_pending = false;

	bool first_time = !use_image.is_valid();

	if (first_time) {
		use_image.instantiate();
	} else  if (use_image->get_width() != width || use_image->get_height() != height ) {
		use_image = nullptr;
		use_image.instantiate();
		use_image->resize(width,height);
		first_time=true;
	}

	const int data_size = width*height*4;
	if (image_data.size() < data_size)
		image_data.resize(data_size);

	{
		// calculate aspect ratio
		float aspect_ratio = 1.0;
		if (ImageFront.is_valid()) {
			aspect_ratio = static_cast<float>(ImageFront->get_width()) / static_cast<float>(ImageFront->get_height());
		}
		uint8_t *wd8 = image_data.ptrw();
		bool has_texture_overlay = get_texture_overlay().is_valid();
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				const Vector4 ofs = _get_color_at(x, y, aspect_ratio, has_texture_overlay);
				const int starting_index = (x + (y * width)) * 4;
				wd8[starting_index + 0] = static_cast<uint8_t>(CLAMP(ofs.x * 255.0, 0, 255));
				wd8[starting_index + 1] = static_cast<uint8_t>(CLAMP(ofs.y * 255.0, 0, 255));
				wd8[starting_index + 2] = static_cast<uint8_t>(CLAMP(ofs.z * 255.0, 0, 255));
				wd8[starting_index + 3] = static_cast<uint8_t>(CLAMP(ofs.w * 255.0, 0, 255));
			}
		}
	}

	use_image->set_data(width, height, false, Image::FORMAT_RGBA8, image_data);

	if (texture.is_valid()) {
		if (first_time) {
			RID new_texture = RS::get_singleton()->texture_2d_create(use_image);
			RS::get_singleton()->texture_replace(texture, new_texture);
		} else {
			RS::get_singleton()->texture_2d_update(texture, use_image);
		}
	} else {
		texture = RS::get_singleton()->texture_2d_create(use_image);
	}
}

Vector4 CombinedTexture2D::_get_color_at(int x, int y, float aspect_ratio, bool has_texture_overlay) const {
	if (!ImageFront.is_valid() || !ImageBack.is_valid()) {
		return Vector4{1.0,1.0,1.0,1.0};
	}

	Vector2 pixel;
	if (width > 1) {
		pixel.x = static_cast<float>(front_flips_h ? (width - x) : x) / static_cast<float>(width);  // normalize x-coordinate
	}
	if (height > 1) {
		pixel.y = static_cast<float>(y) / static_cast<float>(height);  // normalize y-coordinate
	}

	// Now we can calculate the corresponding pixel coordinates for each image.
	// Note that we need to subtract 1 because the get_pixel() method expects zero-based indices.
	int back_x = static_cast<int>(CLAMP(Math::round((pixel.x * (static_cast<float>(ImageBack->get_width()) - 1))), 0, ImageBack->get_width() - 1));
	int back_y = static_cast<int>(CLAMP(Math::round((pixel.y * (static_cast<float>(ImageBack->get_height()) - 1))), 0, ImageBack->get_height() - 1));

	// add offset and scale vector
	back_x = static_cast<int>((static_cast<float>(back_x) + (back_offset.x * 100.0)) * (back_scale.x));
	back_y = static_cast<int>((static_cast<float>(back_y) + (back_offset.y * 100.0)) * (back_scale.y));

	int front_x = static_cast<int>(CLAMP(Math::round((pixel.x * (static_cast<float>(ImageFront->get_width()) - 1))), 0, ImageFront->get_width() - 1));
	int front_y = static_cast<int>(CLAMP(Math::round((pixel.y * (static_cast<float>(ImageFront->get_height()) - 1) * aspect_ratio)), 0, ImageFront->get_height() - 1));

	// add offset and scale vector
	front_x = static_cast<int>((static_cast<float>(front_x) + (front_offset.x * 100.0)) * front_scale.x);
	front_y = static_cast<int>((static_cast<float>(front_y) + (front_offset.y * 100.0)) * front_scale.y);

	Color colorBack = ImageBack->get_pixel(CLAMP(back_x,0,ImageBack->get_width()-1),  CLAMP(back_y,0,ImageBack->get_height()-1));
	Color colorFront = ImageFront->get_pixel(CLAMP(front_x,0,ImageFront->get_width()-1), CLAMP(front_y,0,ImageFront->get_height()-1));

	if (has_texture_overlay) {
		Color coloroverlay = ImageBack->get_pixel(CLAMP(back_x,0,ImageBack->get_width()-1),  CLAMP(back_y,0,ImageBack->get_height()-1));
		colorFront = colorFront.lerp(coloroverlay, coloroverlay.a);
	}

	Vector4 v4back = Vector4{colorBack.r,colorBack.g,colorBack.b,colorBack.a} *
		(Vector4{modulate_back.r + additive_back.x,modulate_back.g + additive_back.y,modulate_back.b + additive_back.z,modulate_back.a});
	Vector4 v4front = Vector4{colorFront.r,colorFront.g,colorFront.b,colorFront.a} *
		(Vector4{modulate_front.r + additive_front.x,modulate_front.g + additive_front.y,modulate_front.b + additive_front.z,modulate_front.a});

	if (only_use_front_full_alpha && v4front.w < 0.95) {
		v4front.w = 0.0;
	}
	if (black_on_back_overrides_front && v4back.x < 0.04 && v4back.y < 0.04 && v4back.z < 0.04 && v4back.w > 0.95) {
		if (black_on_black_overrides_under == 0 || y > black_on_black_overrides_under)
			v4front = v4back;
	}

	if (front_crop_out == Vector4i{0,0,0,0})
		return v4back.lerp(v4front, show_based_on_back_alpha && y > show_based_on_back_alpha_under ? MIN(v4back.w,v4front.w) : v4front.w);

	return v4back.lerp(v4front, ((y < (height - front_crop_out.z) && (y > front_crop_out.w) ) && ((front_flips_h ? width - x : x) < (width - front_crop_out.y) && ((front_flips_h ? width - x : x) > front_crop_out.x) )) ? (show_based_on_back_alpha && y > show_based_on_back_alpha_under  ? MIN(v4back.w,v4front.w) : v4front.w) : 0.0f);
}

void CombinedTexture2D::set_width(int p_width) {
	// ERR_FAIL_COND_MSG(p_width <= 0 || p_width > 16384, "Texture dimensions have to be within 1 to 16384 range.");
	width = MIN(MAX(p_width,1),16384);
	_queue_update();
	emit_changed();
}

int CombinedTexture2D::get_width() const {
	return width;
}

void CombinedTexture2D::set_height(int p_height) {
	// ERR_FAIL_COND_MSG(p_height <= 0 || p_height > 16384, "Texture dimensions have to be within 1 to 16384 range.");
	height = MIN(MAX(p_height,1),16384);
	_queue_update();
	emit_changed();
}

int CombinedTexture2D::get_height() const {
	return height;
}

RID CombinedTexture2D::get_rid() const {
	if (!texture.is_valid()) {
		texture = RS::get_singleton()->texture_2d_placeholder_create();
	}
	return texture;
}


Ref<Texture2D> CombinedTexture2D::get_texture_back() const {
	return TextureBack;
}

Ref<Texture2D> CombinedTexture2D::get_texture_front() const {
	return TextureFront;
}


void CombinedTexture2D::set_show_based_on_back_alpha_under(int p_show_based_on_back_alpha_under) {
	show_based_on_back_alpha_under = p_show_based_on_back_alpha_under;
	_queue_update();
	emit_changed();
}

void CombinedTexture2D::set_show_based_on_back_alpha(bool p_show_based) {
	show_based_on_back_alpha = p_show_based;
	_queue_update();
	emit_changed();
}

void CombinedTexture2D::set_only_use_front_full_alpha(bool p_only_use_front_full_alpha) {
	only_use_front_full_alpha = p_only_use_front_full_alpha;
	_queue_update();
	emit_changed();
}

void CombinedTexture2D::set_black_on_back_overrides_front(bool p_black_on_back_overrides_front) {
	black_on_back_overrides_front = p_black_on_back_overrides_front;
	_queue_update();
	emit_changed();
}

void CombinedTexture2D::set_black_on_black_overrides_under(int p_black_on_black_overrides_under) {
	black_on_black_overrides_under = p_black_on_black_overrides_under;
	_queue_update();
	emit_changed();
}

void CombinedTexture2D::set_front_flips_h(bool p_front_flips_h) {
	front_flips_h = p_front_flips_h;
	_queue_update();
	emit_changed();
}

void CombinedTexture2D::set_front_offset(Vector2 offset) {
	front_offset = offset;
	_queue_update();
	emit_changed();
}

void CombinedTexture2D::set_front_scale(Vector2 scale) {
	front_scale = scale;
	_queue_update();
	emit_changed();
}

void CombinedTexture2D::set_back_offset(const Vector2 offset) {
	{ back_offset = offset; }
	_queue_update();
}

void CombinedTexture2D::set_back_scale(const Vector2 scale) {
	{ back_scale = scale; }
	_queue_update();
}

void CombinedTexture2D::set_modulate_back(Color p_color) {
	{
		modulate_back = p_color;
	}
	_queue_update();
}

void CombinedTexture2D::set_modulate_front(Color p_color) {
	{
		modulate_front = p_color;
	}
	_queue_update();
}


void CombinedTexture2D::set_additive_back(Vector3 p_vector){
	additive_back = p_vector;
	_queue_update();
}

void CombinedTexture2D::set_additive_front(Vector3 p_vector){
	additive_front = p_vector;
	_queue_update();
}

Vector3 CombinedTexture2D::get_additive_back() const {
	return additive_back;
}

Vector3 CombinedTexture2D::get_additive_front() const {
	return additive_front;
}

void CombinedTexture2D::set_front_crop_out(Vector4i crop_under) {
	front_crop_out = crop_under;

	_queue_update();
	emit_changed();
}

void CombinedTexture2D::set_texture_back(const Ref<Texture2D> &p_texture_back) {
	TextureBack = p_texture_back;
	if (TextureBack.is_valid()) {
		ImageBack = TextureBack->get_image();
	} else {
		ImageBack = nullptr;	
	}
	_queue_update();
}

void CombinedTexture2D::set_texture_front(const Ref<Texture2D> &p_texture_front) {
	TextureFront = p_texture_front;
	if (TextureFront.is_valid()) {
		ImageFront = TextureFront->get_image();
	} else {
		ImageFront = nullptr;
	}
	_queue_update();
}

void CombinedTexture2D::set_texture_overlay(const Ref<Texture2D> &p_texture_overlay) {
	TextureOverlay = p_texture_overlay;
	if (TextureOverlay.is_valid()) {
		ImageOverlay = TextureOverlay->get_image();
	} else {
		ImageOverlay = nullptr;
	}
	_queue_update();
}

Ref<Texture2D> CombinedTexture2D::get_texture_overlay() const {
	return TextureOverlay;
}

Ref<Image> CombinedTexture2D::get_image() const {
	const_cast<CombinedTexture2D *>(this)->update_now();
	if (!texture.is_valid()) {
		return {};
	}
	return RenderingServer::get_singleton()->texture_2d_get(texture);
}

void CombinedTexture2D::update_now() {
	if (update_pending) {
		_update();
	}
}

void CombinedTexture2D::_bind_methods() {

	// ClassDB::bind_method(D_METHOD("get_width"), &CombinedTexture2D::get_width);
	// ClassDB::bind_method(D_METHOD("get_height"), &CombinedTexture2D::get_height);
	ClassDB::bind_method(D_METHOD("set_width", "width"), &CombinedTexture2D::set_width,DEFVAL(64));
	ClassDB::bind_method(D_METHOD("set_height", "height"), &CombinedTexture2D::set_height,DEFVAL(64));

	ADD_PROPERTY(PropertyInfo(Variant::INT, "width", PROPERTY_HINT_RANGE, "1,2048,or_greater,suffix:px"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "height", PROPERTY_HINT_RANGE, "1,2048,or_greater,suffix:px"), "set_height", "get_height");

	ClassDB::bind_method(D_METHOD("set_texture_back", "texture"), &CombinedTexture2D::set_texture_back);
	ClassDB::bind_method(D_METHOD("get_texture_back"), &CombinedTexture2D::get_texture_back);
	ClassDB::bind_method(D_METHOD("set_texture_front", "texture"), &CombinedTexture2D::set_texture_front);
	ClassDB::bind_method(D_METHOD("get_texture_front"), &CombinedTexture2D::get_texture_front);
	ClassDB::bind_method(D_METHOD("set_texture_overlay", "texture"), &CombinedTexture2D::set_texture_overlay);
	ClassDB::bind_method(D_METHOD("get_texture_overlay"), &CombinedTexture2D::get_texture_overlay);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_back", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_texture_back", "get_texture_back");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_front", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_texture_front", "get_texture_front");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_overlay", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_texture_overlay", "get_texture_overlay");

	ClassDB::bind_method(D_METHOD("set_modulate_back", "color"), &CombinedTexture2D::set_modulate_back, DEFVAL(Color(1.0,1.0,1.0,1.0)));
	ClassDB::bind_method(D_METHOD("set_modulate_front", "color"), &CombinedTexture2D::set_modulate_front, DEFVAL(Color(1.0,1.0,1.0,1.0)));
	ClassDB::bind_method(D_METHOD("get_modulate_front"), &CombinedTexture2D::get_modulate_front);
	ClassDB::bind_method(D_METHOD("get_modulate_back"), &CombinedTexture2D::get_modulate_back);

	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "modulate_back"), "set_modulate_back", "get_modulate_back");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "modulate_front"), "set_modulate_front", "get_modulate_front");

	ClassDB::bind_method(D_METHOD("set_only_use_front_full_alpha", "only_use_front_full_alpha"), &CombinedTexture2D::set_only_use_front_full_alpha,DEFVAL(false));
	ClassDB::bind_method(D_METHOD("get_only_use_front_full_alpha"), &CombinedTexture2D::get_only_use_front_full_alpha);

	ClassDB::bind_method(D_METHOD("set_show_based_on_back_alpha", "show_based_on_back_alpha"), &CombinedTexture2D::set_show_based_on_back_alpha,DEFVAL(false));
	ClassDB::bind_method(D_METHOD("get_show_based_on_back_alpha"), &CombinedTexture2D::get_show_based_on_back_alpha);

	ClassDB::bind_method(D_METHOD("set_black_on_back_overrides_front", "black_on_back_overrides_front"), &CombinedTexture2D::set_black_on_back_overrides_front,DEFVAL(false));
	ClassDB::bind_method(D_METHOD("get_black_on_back_overrides_front"), &CombinedTexture2D::get_black_on_back_overrides_front);

	ClassDB::bind_method(D_METHOD("set_front_flips_h", "front_flips_h"), &CombinedTexture2D::set_front_flips_h,DEFVAL(false));
	ClassDB::bind_method(D_METHOD("get_front_flips_h"), &CombinedTexture2D::get_front_flips_h);

	ClassDB::bind_method(D_METHOD("get_black_on_black_overrides_under"), &CombinedTexture2D::get_black_on_black_overrides_under);
	ClassDB::bind_method(D_METHOD("set_black_on_black_overrides_under", "under"), &CombinedTexture2D::set_black_on_black_overrides_under,DEFVAL(0));

	ClassDB::bind_method(D_METHOD("get_show_based_on_back_alpha_under"), &CombinedTexture2D::get_show_based_on_back_alpha_under);
	ClassDB::bind_method(D_METHOD("set_show_based_on_back_alpha_under", "under"), &CombinedTexture2D::set_show_based_on_back_alpha_under,DEFVAL(0));

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "front_flips_h"), "set_front_flips_h", "get_front_flips_h");

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "show_based_on_back_alpha"), "set_show_based_on_back_alpha", "get_show_based_on_back_alpha");

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "only_use_front_full_alpha"), "set_only_use_front_full_alpha", "get_only_use_front_full_alpha");

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "black_on_back_overrides_front"), "set_black_on_back_overrides_front", "get_black_on_back_overrides_front");

	ADD_PROPERTY(PropertyInfo(Variant::INT, "black_on_back_overrides_under"), "set_black_on_black_overrides_under", "get_black_on_black_overrides_under");

	ADD_PROPERTY(PropertyInfo(Variant::INT, "show_based_on_back_alpha_under"), "set_show_based_on_back_alpha_under", "get_show_based_on_back_alpha_under");

	ClassDB::bind_method(D_METHOD("set_additive_back", "vector"), &CombinedTexture2D::set_additive_back);
	ClassDB::bind_method(D_METHOD("set_additive_front", "vector"), &CombinedTexture2D::set_additive_front);

	ClassDB::bind_method(D_METHOD("get_additive_back"), &CombinedTexture2D::get_additive_back);
	ClassDB::bind_method(D_METHOD("get_additive_front"), &CombinedTexture2D::get_additive_front);

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "additive_back"), "set_additive_back", "get_additive_back");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "additive_front"), "set_additive_front", "get_additive_front");

	ClassDB::bind_method(D_METHOD("get_front_offset"), &CombinedTexture2D::get_front_offset);
	ClassDB::bind_method(D_METHOD("set_front_offset", "offset"), &CombinedTexture2D::set_front_offset);

	ClassDB::bind_method(D_METHOD("get_front_scale"), &CombinedTexture2D::get_front_scale);
	ClassDB::bind_method(D_METHOD("set_front_scale", "scale"), &CombinedTexture2D::set_front_scale, DEFVAL(Vector2(1.0,1.0)));
	ClassDB::bind_method(D_METHOD("get_front_crop_out"), &CombinedTexture2D::get_front_crop_out);
	ClassDB::bind_method(D_METHOD("set_front_crop_out", "crop_under"), &CombinedTexture2D::set_front_crop_out, DEFVAL(Vector4i(0,0,0,0)));

	ClassDB::bind_method(D_METHOD("get_back_offset"), &CombinedTexture2D::get_back_offset);
	ClassDB::bind_method(D_METHOD("set_back_offset", "offset"), &CombinedTexture2D::set_back_offset);

	ClassDB::bind_method(D_METHOD("get_back_scale"), &CombinedTexture2D::get_back_scale);
	ClassDB::bind_method(D_METHOD("set_back_scale", "scale"), &CombinedTexture2D::set_back_scale, DEFVAL(Vector2(1.0,1.0)));
    
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "front_scale", PROPERTY_HINT_LINK), "set_front_scale", "get_front_scale");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "front_offset"), "set_front_offset", "get_front_offset");

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "back_scale", PROPERTY_HINT_LINK), "set_back_scale", "get_back_scale");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "back_offset"), "set_back_offset", "get_back_offset");

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR4I, "front_crop_out"), "set_front_crop_out", "get_front_crop_out");
}
