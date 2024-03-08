//
// Created by Daniel on 2024-03-06.
//

#include "CircleWithConeTexture2D.h"

CircleWithConeTexture2D::CircleWithConeTexture2D() {
	_queue_update();
}

CircleWithConeTexture2D::~CircleWithConeTexture2D() {
	if (use_image.is_valid()) {
		use_image.unref();
	}
	if (texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RS::get_singleton()->free(texture);
	}
}

void CircleWithConeTexture2D::_queue_update() {
	if (update_pending) {
		return;
	}
	update_pending = true;
	callable_mp(this, &CircleWithConeTexture2D::update_now).call_deferred();
}

void CircleWithConeTexture2D::_update() {
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
				const float ofs = _get_color_at(x, y);
				const int starting_index = (x + (y * width)) * 4;
				wd8[starting_index + 0] = uint8_t(CLAMP(Math::lerp(color_outside.r,color_inside.r,ofs) * 255.0, 0, 255));
				wd8[starting_index + 1] = uint8_t(CLAMP(Math::lerp(color_outside.g,color_inside.g,ofs) * 255.0, 0, 255));
				wd8[starting_index + 2] = uint8_t(CLAMP(Math::lerp(color_outside.b,color_inside.b,ofs) * 255.0, 0, 255));
				wd8[starting_index + 3] = uint8_t(CLAMP(Math::lerp(color_outside.a,color_inside.a,ofs) * 255.0, 0, 255));
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

// bool CircleWithConeTexture2D::_get_color_at(int x, int y) const {
// 	Vector2 pixel;
// 	if (width > 1) {
// 		pixel.x = x;//static_cast<float>(x) / (width - 1);
// 	}
// 	if (height > 1) {
// 		pixel.y = y;//static_cast<float>(y) / (height - 1);
// 	}
//
// 	Vector2 center(width / 2, height / 2);
//
//
// 	// Is it inside the circle?
// 	if ((pixel - center).length() <= circle_radius) {
// 		return true;
// 	}
//
// 	if (cone_angle > 0.0 ) {
// 		// Is it inside the cone?
// 		Vector2 cone_dir = (cone_from - (cone_from + cone_to)).normalized();
//
// 		float cos_angle = cos(cone_angle);
// 		Vector2 dir = (pixel - center).normalized();
// 		Vector2 dir_cone = (cone_from - pixel).normalized();
// 		if (dir_cone.dot(cone_dir) > cos_angle && dir_cone.dot(dir) > -0.3) {
// 			return true;
// 		}
// 	}
//
// 	// It's neither in the circle, nor in the cone
// 	return false;
// }

float CircleWithConeTexture2D::_get_color_at(int x, int y) const {
	Vector2 pixel;
	if (width > 1) {
		pixel.x = x;
	}
	if (height > 1) {
		pixel.y = y;
	}

	Vector2 center(width / 2, height / 2);

	float distanceToCenter = (pixel - center).length();
	float desired_result = 0.0;
	// Create a smooth edge for circle
	if (distanceToCenter <= circle_start_radius + circle_end_radius) {
		if (distanceToCenter > circle_start_radius)
			desired_result = fmax(desired_result,Math::inverse_lerp(circle_start_radius + circle_end_radius,circle_start_radius,distanceToCenter));
		else
			desired_result = fmax(desired_result,1.0f);
	}

	// Is it inside the cone?
	if (cone_angle > 0.0 ) {
 		Vector2 cone_dir = (cone_from - (cone_from + cone_to)).normalized();

		float cos_angle = cos(cone_angle);
		float cos_angle_smoothed = cos(cone_angle+cone_smoothing_angle);
		Vector2 dir = (pixel - center).normalized();
		Vector2 dir_cone = (pixel - cone_from).normalized();
		const float dot_found = dir_cone.dot(cone_dir);
		if (dot_found > cos_angle_smoothed && dir_cone.dot(dir) > -0.3) {
			if (dot_found > cos_angle) desired_result = fmax(desired_result,1.0);
			else desired_result = fmax(desired_result,Math::inverse_lerp(cos_angle_smoothed,cos_angle,dot_found));
		}
	}

	if (desired_result > 0.0001 && distanceToCenter > circle_max_radius) {
		if (distanceToCenter < circle_max_radius + circle_max_smoothing) {
			desired_result = fmin(desired_result,Math::inverse_lerp(circle_max_radius+circle_max_smoothing,circle_max_radius,distanceToCenter));
		} else {
			desired_result = 0.0;
		}
	}

	return desired_result;
}

void CircleWithConeTexture2D::set_width(int p_width) {
	ERR_FAIL_COND_MSG(p_width <= 0 || p_width > 16384, "Texture dimensions have to be within 1 to 16384 range.");
	width = p_width;
	_queue_update();
	emit_changed();
}

int CircleWithConeTexture2D::get_width() const {
	return width;
}

void CircleWithConeTexture2D::set_height(int p_height) {
	ERR_FAIL_COND_MSG(p_height <= 0 || p_height > 16384, "Texture dimensions have to be within 1 to 16384 range.");
	height = p_height;
	_queue_update();
	emit_changed();
}

int CircleWithConeTexture2D::get_height() const {
	return height;
}

RID CircleWithConeTexture2D::get_rid() const {
	if (!texture.is_valid()) {
		texture = RS::get_singleton()->texture_2d_placeholder_create();
	}
	return texture;
}

Ref<Image> CircleWithConeTexture2D::get_image() const {
	const_cast<CircleWithConeTexture2D *>(this)->update_now();
	if (!texture.is_valid()) {
		return Ref<Image>();
	}
	return RenderingServer::get_singleton()->texture_2d_get(texture);
}

void CircleWithConeTexture2D::update_now() {
	if (update_pending) {
		_update();
	}
}

void CircleWithConeTexture2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_width", "width"), &CircleWithConeTexture2D::set_width);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &CircleWithConeTexture2D::set_height);

	ClassDB::bind_method(D_METHOD("set_color_inside", "inside"), &CircleWithConeTexture2D::set_color_inside);
	ClassDB::bind_method(D_METHOD("get_color_inside"), &CircleWithConeTexture2D::get_color_inside);

	ClassDB::bind_method(D_METHOD("set_color_outside", "outside"), &CircleWithConeTexture2D::set_color_outside);
	ClassDB::bind_method(D_METHOD("get_color_outside"), &CircleWithConeTexture2D::get_color_outside);

	ClassDB::bind_method(D_METHOD("set_circle_start_radius", "circle_radius"), &CircleWithConeTexture2D::set_circle_start_radius);
	ClassDB::bind_method(D_METHOD("get_circle_start_radius"), &CircleWithConeTexture2D::get_circle_start_radius);

	ClassDB::bind_method(D_METHOD("set_circle_end_radius", "circle_radius"), &CircleWithConeTexture2D::set_circle_end_radius);
	ClassDB::bind_method(D_METHOD("get_circle_end_radius"), &CircleWithConeTexture2D::get_circle_end_radius);

	
	ClassDB::bind_method(D_METHOD("set_circle_max_radius", "circle_radius"), &CircleWithConeTexture2D::set_circle_max_radius);
	ClassDB::bind_method(D_METHOD("get_circle_max_radius"), &CircleWithConeTexture2D::get_circle_max_radius);

	
	ClassDB::bind_method(D_METHOD("set_circle_max_smoothing", "circle_radius"), &CircleWithConeTexture2D::set_circle_max_smoothing);
	ClassDB::bind_method(D_METHOD("get_circle_max_smoothing"), &CircleWithConeTexture2D::get_circle_max_smoothing);
	
	ClassDB::bind_method(D_METHOD("set_cone_angle", "cone_angle"), &CircleWithConeTexture2D::set_cone_angle);
	ClassDB::bind_method(D_METHOD("get_cone_angle"), &CircleWithConeTexture2D::get_cone_angle);

	ClassDB::bind_method(D_METHOD("set_cone_smoothing_angle", "cone_angle"), &CircleWithConeTexture2D::set_cone_smoothing_angle);
	ClassDB::bind_method(D_METHOD("get_cone_smoothing_angle"), &CircleWithConeTexture2D::get_cone_smoothing_angle);

	ClassDB::bind_method(D_METHOD("set_cone_to", "cone_to"), &CircleWithConeTexture2D::set_cone_to);
	ClassDB::bind_method(D_METHOD("get_cone_to"), &CircleWithConeTexture2D::get_cone_to);

	ClassDB::bind_method(D_METHOD("set_cone_from", "cone_from"), &CircleWithConeTexture2D::set_cone_from);
	ClassDB::bind_method(D_METHOD("get_cone_from"), &CircleWithConeTexture2D::get_cone_from);


	ADD_PROPERTY(PropertyInfo(Variant::INT, "width", PROPERTY_HINT_RANGE, "1,2048,or_greater,suffix:px"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "height", PROPERTY_HINT_RANGE, "1,2048,or_greater,suffix:px"), "set_height", "get_height");

	ADD_GROUP("Color", "color_");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color_outside"), "set_color_outside", "get_color_outside");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color_inside"), "set_color_inside", "get_color_inside");

	ADD_GROUP("Circle", "circle_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "circle_radius"), "set_circle_start_radius", "get_circle_start_radius");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "circle_smoothing"), "set_circle_end_radius", "get_circle_end_radius");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "circle_max_radius"), "set_circle_max_radius", "get_circle_max_radius");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "circle_max_smoothing"), "set_circle_max_smoothing", "get_circle_max_smoothing");

	ADD_GROUP("Cone", "cone_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cone_angle"), "set_cone_angle", "get_cone_angle");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cone_smoothing"), "set_cone_smoothing_angle", "get_cone_smoothing_angle");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "cone_from"), "set_cone_from", "get_cone_from");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "cone_to"), "set_cone_to", "get_cone_to");
}
