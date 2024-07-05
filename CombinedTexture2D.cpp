//
// Created by Daniel on 2024-07-04.
//

#include "CombinedTexture2D.h"

CombinedTexture2D::CombinedTexture2D() {
	_queue_update();
}

CombinedTexture2D::~CombinedTexture2D() {
	if (use_image.is_valid()) {
		use_image.unref();
	}
	if (texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RS::get_singleton()->free(texture);
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
			use_image->resize(width,height);
			use_image.unref();
			use_image.instantiate();
			first_time=true;
		}

	const int data_size = width*height*4;
	if (image_data.size() < data_size)
		image_data.resize(data_size);

	{
		uint8_t *wd8 = image_data.ptrw();
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				const Color ofs = _get_color_at(x, y);
				const int starting_index = (x + (y * width)) * 4;
				wd8[starting_index + 0] = uint8_t(CLAMP(ofs.r * 255.0, 0, 255));
				wd8[starting_index + 1] = uint8_t(CLAMP(ofs.g * 255.0, 0, 255));
				wd8[starting_index + 2] = uint8_t(CLAMP(ofs.b * 255.0, 0, 255));
				wd8[starting_index + 3] = uint8_t(CLAMP(ofs.a * 255.0, 0, 255));
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

Color CombinedTexture2D::_get_color_at(int x, int y) const {
	if (!ImageFront.is_valid() || !ImageBack.is_valid())
		return Color{1.0,1.0,1.0,1.0};

	Vector2 pixel;
	if (width > 1) {
		pixel.x = static_cast<float>(x) / static_cast<float>(width);  // normalize x-coordinate
	}
	if (height > 1) {
		pixel.y = static_cast<float>(y) / static_cast<float>(height);  // normalize y-coordinate
	}

	// Now we can calculate the corresponding pixel coordinates for each image.
	// Note that we need to subtract 1 because the get_pixel() method expects zero-based indices.
	int back_x = static_cast<int>(CLAMP(Math::round((pixel.x * (static_cast<float>(ImageBack->get_width()) - 1))), 0, ImageBack->get_width() - 1));
	int back_y = static_cast<int>(CLAMP(Math::round((pixel.y * (static_cast<float>(ImageBack->get_height()) - 1))), 0, ImageBack->get_height() - 1));

	int front_x = static_cast<int>(CLAMP(Math::round((pixel.x * (static_cast<float>(ImageFront->get_width()) - 1))), 0, ImageFront->get_width() - 1));
	int front_y = static_cast<int>(CLAMP(Math::round((pixel.y * (static_cast<float>(ImageFront->get_height()) - 1))), 0, ImageFront->get_height() - 1));

	Color colorBack = ImageBack->get_pixel(back_x, back_y) * modulate_back;
	Color colorFront = ImageFront->get_pixel(front_x, front_y) * modulate_front;

	// Here, you can decide how you want to combine the two sampled colors.
	// For instance, you could average them, or perhaps blend them based on some other criterion.
	// For simplicity, we'll just average them for now:
	Color desired_result = colorBack.lerp(colorFront, colorFront.a);

	return desired_result;
}

void CombinedTexture2D::set_width(int p_width) {
	ERR_FAIL_COND_MSG(p_width <= 0 || p_width > 16384, "Texture dimensions have to be within 1 to 16384 range.");
	width = p_width;
	_queue_update();
	emit_changed();
}

int CombinedTexture2D::get_width() const {
	return width;
}

void CombinedTexture2D::set_height(int p_height) {
	ERR_FAIL_COND_MSG(p_height <= 0 || p_height > 16384, "Texture dimensions have to be within 1 to 16384 range.");
	height = p_height;
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

void CombinedTexture2D::set_texture_back(const Ref<Texture2D> &p_texture_back) {
	TextureBack = p_texture_back;
	if (TextureBack.is_valid()) {
		ImageBack = TextureBack->get_image();
	} else {
		ImageBack.unref();
	}
	_queue_update();
}

void CombinedTexture2D::set_texture_front(const Ref<Texture2D> &p_texture_front) {
	TextureFront = p_texture_front;
	if (TextureFront.is_valid()) {
		ImageFront = TextureFront->get_image();
	} else {
		ImageFront.unref();
	}
	_queue_update();
}

Ref<Image> CombinedTexture2D::get_image() const {
	const_cast<CombinedTexture2D *>(this)->update_now();
	if (!texture.is_valid()) {
		return Ref<Image>();
	}
	return RenderingServer::get_singleton()->texture_2d_get(texture);
}

void CombinedTexture2D::update_now() {
	if (update_pending) {
		_update();
	}
}

void CombinedTexture2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_width", "width"), &CombinedTexture2D::set_width);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &CombinedTexture2D::set_height);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "width", PROPERTY_HINT_RANGE, "1,2048,or_greater,suffix:px"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "height", PROPERTY_HINT_RANGE, "1,2048,or_greater,suffix:px"), "set_height", "get_height");

	ClassDB::bind_method(D_METHOD("set_texture_back", "texture"), &CombinedTexture2D::set_texture_back);
	ClassDB::bind_method(D_METHOD("get_texture_back"), &CombinedTexture2D::get_texture_back);
	ClassDB::bind_method(D_METHOD("set_texture_front", "texture"), &CombinedTexture2D::set_texture_front);
	ClassDB::bind_method(D_METHOD("get_texture_front"), &CombinedTexture2D::get_texture_front);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_back", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_texture_back", "get_texture_back");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_front", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_texture_front", "get_texture_front");

	ClassDB::bind_method(D_METHOD("set_modulate_back", "color"), &CombinedTexture2D::set_modulate_back);
	ClassDB::bind_method(D_METHOD("set_modulate_front", "color"), &CombinedTexture2D::set_modulate_front);
	ClassDB::bind_method(D_METHOD("get_modulate_front"), &CombinedTexture2D::get_modulate_front);
	ClassDB::bind_method(D_METHOD("get_modulate_back"), &CombinedTexture2D::get_modulate_back);

	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "modulate_back"), "set_modulate_back", "get_modulate_back");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "modulate_front"), "set_modulate_front", "get_modulate_front");

	ClassDB::bind_method(D_METHOD("set_additive_back", "vector"), &CombinedTexture2D::set_additive_back);
	ClassDB::bind_method(D_METHOD("set_additive_front", "vector"), &CombinedTexture2D::set_additive_front);

	ClassDB::bind_method(D_METHOD("get_additive_back"), &CombinedTexture2D::get_additive_back);
	ClassDB::bind_method(D_METHOD("get_additive_front"), &CombinedTexture2D::get_additive_front);

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "additive_back"), "set_additive_back", "get_additive_back");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "additive_front"), "set_additive_front", "get_additive_front");
}
