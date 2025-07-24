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
	bool size_changed = false;
	bool first_time = !use_image.is_valid();

	// Ensure we have valid dimensions
	if (width <= 0 || height <= 0) {
		width = MAX(width, 1);
		height = MAX(height, 1);
		size_changed = true;
	}
    if (use_image.is_valid() && (use_image->get_width() != width || use_image->get_height() != height)) {
		size_changed = true;
	}
	if (first_time || size_changed) {
		use_image = Image::create_empty(width,height,false,Image::FORMAT_RGBA8);
	}

	const int data_size = width*height*4;
	if (image_data.size() != data_size) {
		image_data.resize(data_size);
		image_data.fill(0);
	}

	{
		// Auto-fit calculation for front image
		Vector2 calculated_front_offset = front_offset;
		Vector2 calculated_front_scale = front_scale;
		
		if (auto_fit_mode != AutoFitMode::AUTO_FIT_NONE && ImageFront.is_valid() && ImageBack.is_valid()) {
			_calculate_auto_fit(calculated_front_offset, calculated_front_scale);
			
			// Apply manual scale as a relative zoom factor (centered)
			if (front_scale != Vector2(1.0, 1.0)) {
				// Scale the calculated scale by the manual scale factor
				calculated_front_scale *= front_scale;
				
				// Adjust offset to keep it centered when scaling
				// This compensates for the change in scaled dimensions
				float original_scaled_width = static_cast<float>(ImageFront->get_width()) * (calculated_front_scale.x / front_scale.x);
				float original_scaled_height = static_cast<float>(ImageFront->get_height()) * (calculated_front_scale.y / front_scale.y);
				float new_scaled_width = static_cast<float>(ImageFront->get_width()) * calculated_front_scale.x;
				float new_scaled_height = static_cast<float>(ImageFront->get_height()) * calculated_front_scale.y;
				
				// Adjust offset to maintain center position
				if (auto_fit_mode == AUTO_FIT_CONTAIN_TOP || auto_fit_mode == AUTO_FIT_COVER_TOP) {
					// Center horizontally, keep top at y=0
					calculated_front_offset.x -= (new_scaled_width - original_scaled_width) / 2.0f;
					// No vertical offset: top stays at y=0
				} else {
					// Center both horizontally and vertically
					calculated_front_offset.x -= (new_scaled_width - original_scaled_width) / 2.0f;
					calculated_front_offset.y -= (new_scaled_height - original_scaled_height) / 2.0f;
				}
			}
			
			// Apply manual offset as percentage of the target dimensions
			if (front_offset != Vector2(0.0, 0.0)) {
				calculated_front_offset.x += front_offset.x * static_cast<float>(width) / 100.0f;
				calculated_front_offset.y += front_offset.y * static_cast<float>(height) / 100.0f;
			}
		}
		// Calculate aspect ratio
		float aspect_ratio = 1.0;
		if (ImageFront.is_valid()) {
			aspect_ratio = static_cast<float>(ImageFront->get_width()) / static_cast<float>(ImageFront->get_height());
		}
		uint8_t *wd8 = image_data.ptrw();
		bool has_texture_overlay = get_texture_overlay().is_valid();
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				const Vector4 ofs = _get_color_at(x, y, aspect_ratio, has_texture_overlay, calculated_front_offset, calculated_front_scale);
				const int starting_index = (x + (y * width)) * 4;
				wd8[starting_index + 0] = static_cast<uint8_t>(CLAMP(ofs.x * 255.0, 0, 255));
				wd8[starting_index + 1] = static_cast<uint8_t>(CLAMP(ofs.y * 255.0, 0, 255));
				wd8[starting_index + 2] = static_cast<uint8_t>(CLAMP(ofs.z * 255.0, 0, 255));
				wd8[starting_index + 3] = static_cast<uint8_t>(CLAMP(ofs.w * 255.0, 0, 255));
			}
		}
	}

	use_image->set_data(width, height, false, Image::FORMAT_RGBA8, image_data);

	// Create or update texture
	if (texture.is_valid()) {
		if (first_time || size_changed) {
			// For size changes, we need to replace the texture entirely
			RID new_texture = RS::get_singleton()->texture_2d_create(use_image);
			RS::get_singleton()->texture_replace(texture, new_texture);
			RS::get_singleton()->free(new_texture);
		} else {
			// Update existing texture (same dimensions)
			RS::get_singleton()->texture_2d_update(texture, use_image);
		}
	} else {
		// Create new texture for the first time
		if (use_image.is_valid() && use_image->get_width() > 0 && use_image->get_height() > 0) {
			texture = RS::get_singleton()->texture_2d_create(use_image);
		} else {
			// Create a fallback texture if image is invalid
			Ref<Image> fallback_image = Image::create_empty(MAX(width, 1), MAX(height, 1), false, Image::FORMAT_RGBA8);
			texture = RS::get_singleton()->texture_2d_create(fallback_image);
		}
	}
}

void CombinedTexture2D::_calculate_auto_fit(Vector2 &out_offset, Vector2 &out_scale) const {
    if (!ImageFront.is_valid() || !ImageBack.is_valid()) {
        return;
    }
    float target_width = static_cast<float>(width);
    float target_height = static_cast<float>(height);
    float image_width = static_cast<float>(ImageFront->get_width());
    float image_height = static_cast<float>(ImageFront->get_height());
    
    float scale_x = 1.0f;
    float scale_y = 1.0f;
    
    if (auto_fit_mode == AUTO_FIT_CONTAIN || auto_fit_mode == AUTO_FIT_CONTAIN_TOP) {
        // Calculate both scaling factors
        float scale_by_width = target_width / image_width;
        float scale_by_height = target_height / image_height;
        
        // Use the smaller scaling factor to ensure the image fits completely
        // This maintains aspect ratio and ensures the entire image is visible
        float scale = MIN(scale_by_width, scale_by_height);
        
        scale_x = scale;
        scale_y = scale;
    } else if (auto_fit_mode == AUTO_FIT_COVER || auto_fit_mode == AUTO_FIT_COVER_TOP) {
        // Calculate both scaling factors
        float scale_by_width = target_width / image_width;
        float scale_by_height = target_height / image_height;
        
        // Use the larger scaling factor to ensure the image covers the area
        // This maintains aspect ratio and ensures the target is fully covered
        float scale = MAX(scale_by_width, scale_by_height);
        
        scale_x = scale;
        scale_y = scale;
    } else if (auto_fit_mode == AUTO_FIT_STRETCH) {
        // Stretch to fill exactly (may distort aspect ratio)
        scale_x = target_width / image_width;
        scale_y = target_height / image_height;
    }
    
    out_scale = Vector2(scale_x, scale_y);
    
    // Calculate centering offset
    float scaled_width = image_width * scale_x;
    float scaled_height = image_height * scale_y;
    
    // Calculate the offset based on the auto-fit mode
    if (auto_fit_mode == AUTO_FIT_STRETCH) {
        // No offset needed as it fills exactly
        out_offset.x = 0.0f;
        out_offset.y = 0.0f;
    } else if (auto_fit_mode == AUTO_FIT_CONTAIN_TOP || auto_fit_mode == AUTO_FIT_COVER_TOP) {
        // Top-anchored modes: center horizontally, align to top vertically
        out_offset.x = (target_width - scaled_width) / 2.0f;
        out_offset.y = 0.0f;  // Align to top
    } else {
        // Center-anchored modes: center both horizontally and vertically
        out_offset.x = (target_width - scaled_width) / 2.0f;
        out_offset.y = (target_height - scaled_height) / 2.0f;
    }

	if (transparency_aware_fitting && 
    (auto_fit_mode == AUTO_FIT_CONTAIN_TOP || auto_fit_mode == AUTO_FIT_COVER_TOP) &&
    ImageFront.is_valid()) {

		int img_width = ImageFront->get_width();
		int img_height = ImageFront->get_height();
		int first_opaque_row = 0;
		bool found = false;

		// Scan from top to bottom
		for (int y = 0; y < img_height; ++y) {
			for (int x = 0; x < img_width; ++x) {
				if (ImageFront->get_pixel(x, y).a > 0.01f) {
					first_opaque_row = y;
					found = true;
					break;
				}
			}
			if (found) break;
		}

		// Adjust the offset so the first opaque row is at the top
		if (found && first_opaque_row > 0) {
			// Convert to scaled space
			float offset_in_target = static_cast<float>(first_opaque_row) * out_scale.y;
			out_offset.y -= offset_in_target;
		}
	}
}

Color CombinedTexture2D::_sample_bilinear(const Ref<Image> &image, float x, float y) const {
	if (!image.is_valid()) {
		return Color(0, 0, 0, 0);
	}
	int img_width = image->get_width();
	int img_height = image->get_height();
	// Clamp coordinates
	x = CLAMP(x, 0.0f, static_cast<float>(img_width - 1));
	y = CLAMP(y, 0.0f, static_cast<float>(img_height - 1));
	int x0 = static_cast<int>(x);
	int y0 = static_cast<int>(y);
	int x1 = MIN(x0 + 1, img_width - 1);
	int y1 = MIN(y0 + 1, img_height - 1);
	float fx = x - static_cast<float>(x0);
	float fy = y - static_cast<float>(y0);
	Color c00 = image->get_pixel(x0, y0);
	Color c10 = image->get_pixel(x1, y0);
	Color c01 = image->get_pixel(x0, y1);
	Color c11 = image->get_pixel(x1, y1);
	// Bilinear interpolation
	Color c0 = c00.lerp(c10, fx);
	Color c1 = c01.lerp(c11, fx);
	return c0.lerp(c1, fy);
}

Vector4 CombinedTexture2D::_get_color_at(int x, int y, float aspect_ratio, bool has_texture_overlay, const Vector2 &front_offset_calc, const Vector2 &front_scale_calc) const {
	if (!ImageFront.is_valid() || !ImageBack.is_valid()) {
		return Vector4{1.0,1.0,1.0,1.0};
	}
	
	// Additional safety checks for image dimensions
	if (ImageFront->get_width() <= 0 || ImageFront->get_height() <= 0 ||
		ImageBack->get_width() <= 0 || ImageBack->get_height() <= 0) {
		return Vector4{1.0,1.0,1.0,1.0};
	}

	Vector2 pixel;
	if (width > 1) {
		pixel.x = static_cast<float>(x) / static_cast<float>(width);  // normalize x-coordinate
	}
	if (height > 1) {
		pixel.y = static_cast<float>(y) / static_cast<float>(height);  // normalize y-coordinate
	}

	// Calculate back image coordinates
	float back_x = (pixel.x * (static_cast<float>(ImageBack->get_width()) - 1));
	float back_y = (pixel.y * (static_cast<float>(ImageBack->get_height()) - 1));
	// Apply offset and scale
	back_x = (back_x + (back_offset.x * 100.0)) * back_scale.x;
	back_y = (back_y + (back_offset.y * 100.0)) * back_scale.y;
	// Calculate front image coordinates with proper auto-fit handling
	float front_x, front_y;
	
	if (auto_fit_mode != AUTO_FIT_NONE) {
		// For auto-fit modes, we need to transform from target pixel coordinates to source image coordinates
		// First, apply the offset to get the position within the scaled image
		float adjusted_x = static_cast<float>(front_flips_h ? (width - x) : x) - front_offset_calc.x;
		float adjusted_y = static_cast<float>(y) - front_offset_calc.y;
		
		// Then scale back to source image coordinates
		front_x = adjusted_x / front_scale_calc.x;
		front_y = adjusted_y / front_scale_calc.y;
	} else {
		float front_pixel_x = front_flips_h ? (1.0f - pixel.x) : pixel.x;
		front_x = static_cast<float>(CLAMP(Math::round((front_pixel_x * (static_cast<float>(ImageFront->get_width()) - 1))), 0, ImageFront->get_width() - 1));
		front_y = static_cast<float>(CLAMP(Math::round((pixel.y * (static_cast<float>(ImageFront->get_height()) - 1) * aspect_ratio)), 0, ImageFront->get_height() - 1));
		// add offset and scale vector
		front_x = static_cast<float>((static_cast<float>(front_x) + (front_offset.x * 100.0)) * front_scale.x);
		front_y = static_cast<float>((static_cast<float>(front_y) + (front_offset.y * 100.0)) * front_scale.y);
	}
	// Use bilinear sampling for better quality
	Color colorBack = enable_bilinear_filtering ? 
		_sample_bilinear(ImageBack, back_x, back_y) :
		ImageBack->get_pixel(
			CLAMP(static_cast<int>(Math::round(back_x)), 0, ImageBack->get_width() - 1),
			CLAMP(static_cast<int>(Math::round(back_y)), 0, ImageBack->get_height() - 1)
		);
	Color colorFront = enable_bilinear_filtering ?
		_sample_bilinear(ImageFront, front_x, front_y) :
		ImageFront->get_pixel(
			CLAMP(static_cast<int>(Math::round(front_x)), 0, ImageFront->get_width() - 1),
			CLAMP(static_cast<int>(Math::round(front_y)), 0, ImageFront->get_height() - 1)
		);
		
	if (has_texture_overlay) {
		Color coloroverlay = ImageBack->get_pixel(
			CLAMP(static_cast<int>(Math::round(back_x)), 0, ImageBack->get_width() - 1),
			CLAMP(static_cast<int>(Math::round(back_y)), 0, ImageBack->get_height() - 1)
		);
		colorFront = colorFront.lerp(coloroverlay, coloroverlay.a);
	}

	Vector4 v4back = Vector4{colorBack.r, colorBack.g, colorBack.b, colorBack.a} *
		(Vector4{modulate_back.r + additive_back.x, modulate_back.g + additive_back.y, modulate_back.b + additive_back.z, modulate_back.a});
	Vector4 v4front = Vector4{colorFront.r, colorFront.g, colorFront.b, colorFront.a} *
		(Vector4{modulate_front.r + additive_front.x, modulate_front.g + additive_front.y, modulate_front.b + additive_front.z, modulate_front.a});

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
void CombinedTexture2D::set_enable_bilinear_filtering(bool b) {
	enable_bilinear_filtering = b;
	_queue_update();
	emit_changed();
}
void CombinedTexture2D::set_transparency_aware_fitting(bool b) {
	transparency_aware_fitting = b;
	_queue_update();
	emit_changed();
}
void CombinedTexture2D::set_auto_fit_mode(AutoFitMode p_mode) {
	auto_fit_mode = p_mode;
	_queue_update();
	emit_changed();
}

CombinedTexture2D::AutoFitMode CombinedTexture2D::get_auto_fit_mode() const {
	return auto_fit_mode;
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
		// Force an update to create a proper texture
		const_cast<CombinedTexture2D *>(this)->update_now();
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
	emit_changed();
}

void CombinedTexture2D::set_back_scale(const Vector2 scale) {
	{ back_scale = scale; }
	_queue_update();
	emit_changed();
}

void CombinedTexture2D::set_modulate_back(Color p_color) {
	{
		modulate_back = p_color;
	}
	_queue_update();
	emit_changed();
}

void CombinedTexture2D::set_modulate_front(Color p_color) {
	{
		modulate_front = p_color;
	}
	_queue_update();
	emit_changed();
}


void CombinedTexture2D::set_additive_back(Vector3 p_vector){
	additive_back = p_vector;
	_queue_update();
	emit_changed();
}

void CombinedTexture2D::set_additive_front(Vector3 p_vector){
	additive_front = p_vector;
	_queue_update();
	emit_changed();
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
	emit_changed();
}

void CombinedTexture2D::set_texture_front(const Ref<Texture2D> &p_texture_front) {
	TextureFront = p_texture_front;
	if (TextureFront.is_valid()) {
		ImageFront = TextureFront->get_image();
	} else {
		ImageFront = nullptr;
	}
	_queue_update();
	emit_changed();
}

void CombinedTexture2D::set_texture_overlay(const Ref<Texture2D> &p_texture_overlay) {
	TextureOverlay = p_texture_overlay;
	if (TextureOverlay.is_valid()) {
		ImageOverlay = TextureOverlay->get_image();
	} else {
		ImageOverlay = nullptr;
	}
	_queue_update();
	emit_changed();
}

Ref<Texture2D> CombinedTexture2D::get_texture_overlay() const {
	return TextureOverlay;
}

Ref<Image> CombinedTexture2D::get_image() const {
	const_cast<CombinedTexture2D *>(this)->update_now();
	if (!texture.is_valid()) {
		return {};
	}
	// Add additional validation to ensure the texture is still valid
	ERR_FAIL_NULL_V(RenderingServer::get_singleton(), {});
	return RenderingServer::get_singleton()->texture_2d_get(texture);
}

void CombinedTexture2D::update_now() {
	if (update_pending) {
		_update();
	}
}

void CombinedTexture2D::_bind_methods() {
	BIND_ENUM_CONSTANT(AUTO_FIT_NONE);
	BIND_ENUM_CONSTANT(AUTO_FIT_CONTAIN);
	BIND_ENUM_CONSTANT(AUTO_FIT_COVER);
	BIND_ENUM_CONSTANT(AUTO_FIT_CONTAIN_TOP);
	BIND_ENUM_CONSTANT(AUTO_FIT_COVER_TOP);
	BIND_ENUM_CONSTANT(AUTO_FIT_STRETCH);
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
    
	ClassDB::bind_method(D_METHOD("get_auto_fit_mode"), &CombinedTexture2D::get_auto_fit_mode);
	ClassDB::bind_method(D_METHOD("set_auto_fit_mode", "mode"), &CombinedTexture2D::set_auto_fit_mode, DEFVAL(AUTO_FIT_NONE));

	ClassDB::bind_method(D_METHOD("get_transparency_aware_fitting"), &CombinedTexture2D::get_transparency_aware_fitting);
	ClassDB::bind_method(D_METHOD("set_transparency_aware_fitting", "enable"), &CombinedTexture2D::set_transparency_aware_fitting, DEFVAL(true));

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "transparency_aware_fitting"), "set_transparency_aware_fitting", "get_transparency_aware_fitting");

	ADD_PROPERTY(PropertyInfo(Variant::INT, "auto_fit_mode", PROPERTY_HINT_ENUM, "None,Contain,Cover,Contain Top,Cover Top,Stretch"), "set_auto_fit_mode", "get_auto_fit_mode");

	ClassDB::bind_method(D_METHOD("get_enable_bilinear_filtering"), &CombinedTexture2D::get_enable_bilinear_filtering);
	ClassDB::bind_method(D_METHOD("set_enable_bilinear_filtering", "enable"), &CombinedTexture2D::set_enable_bilinear_filtering, DEFVAL(true));

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enable_bilinear_filtering"), "set_enable_bilinear_filtering", "get_enable_bilinear_filtering");

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "front_scale", PROPERTY_HINT_LINK), "set_front_scale", "get_front_scale");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "front_offset"), "set_front_offset", "get_front_offset");

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "back_scale", PROPERTY_HINT_LINK), "set_back_scale", "get_back_scale");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "back_offset"), "set_back_offset", "get_back_offset");

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR4I, "front_crop_out"), "set_front_crop_out", "get_front_crop_out");
}
