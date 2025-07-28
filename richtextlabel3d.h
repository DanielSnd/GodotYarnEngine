/**************************************************************************/
/*  rich_text_label.h                                                     */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include "scene/3d/visual_instance_3d.h"
#include "core/object/worker_thread_pool.h"
#include "core/templates/rid_owner.h"
#include "core/math/rect2.h"
#include "scene/gui/popup_menu.h"
#include "scene/gui/scroll_bar.h"
#include "scene/resources/image_texture.h"
#include "scene/resources/text_paragraph.h"
#include "scene/resources/font.h"
#include "servers/text_server.h"
#include "core/object/object.h"
#include "core/variant/variant.h"
#include "scene/resources/atlas_texture.h"

#include "scene/main/window.h"
#include "scene/resources/theme.h"
#include "scene/theme/theme_db.h"

#include "modules/modules_enabled.gen.h" // For regex.
#ifdef MODULE_REGEX_ENABLED
#include "modules/regex/regex.h"
#endif

#ifdef TOOLS_ENABLED
#include "editor/settings/editor_settings.h"
#include "editor/scene/3d/node_3d_editor_plugin.h"
#include "editor/richtextlabel3d_gizmo_plugin.h"
#endif

class RichTextLabel3D : public GeometryInstance3D {
	GDCLASS(RichTextLabel3D, GeometryInstance3D);

public:
	enum DrawFlags {
		FLAG_SHADED,
		FLAG_DOUBLE_SIDED,
		FLAG_DISABLE_DEPTH_TEST,
		FLAG_FIXED_SIZE,
		FLAG_MAX
	};

	enum AlphaCutMode {
		ALPHA_CUT_DISABLED,
		ALPHA_CUT_DISCARD,
		ALPHA_CUT_OPAQUE_PREPASS,
		ALPHA_CUT_HASH,
		ALPHA_CUT_MAX
	};

	enum RTLDrawStep {
		DRAW_STEP_BACKGROUND,
		DRAW_STEP_SHADOW_OUTLINE,
		DRAW_STEP_SHADOW,
		DRAW_STEP_OUTLINE,
		DRAW_STEP_TEXT,
		DRAW_STEP_FOREGROUND,
		DRAW_STEP_MAX,
	};

	enum ListType {
		LIST_NUMBERS,
		LIST_LETTERS,
		LIST_ROMAN,
		LIST_DOTS
	};

	enum MetaUnderline {
		META_UNDERLINE_NEVER,
		META_UNDERLINE_ALWAYS,
		META_UNDERLINE_ON_HOVER,
	};

	enum ItemType {
		ITEM_FRAME,
		ITEM_TEXT,
		ITEM_IMAGE,
		ITEM_NEWLINE,
		ITEM_FONT,
		ITEM_FONT_SIZE,
		ITEM_FONT_FEATURES,
		ITEM_COLOR,
		ITEM_OUTLINE_SIZE,
		ITEM_OUTLINE_COLOR,
		ITEM_UNDERLINE,
		ITEM_STRIKETHROUGH,
		ITEM_PARAGRAPH,
		ITEM_INDENT,
		ITEM_LIST,
		ITEM_TABLE,
		ITEM_FADE,
		ITEM_SHAKE,
		ITEM_WAVE,
		ITEM_TORNADO,
		ITEM_RAINBOW,
		ITEM_PULSE,
		ITEM_BGCOLOR,
		ITEM_FGCOLOR,
		ITEM_META,
		ITEM_HINT,
		ITEM_DROPCAP,
		ITEM_CUSTOMFX,
		ITEM_CONTEXT,
		ITEM_LANGUAGE,
	};

	enum MenuItems {
		MENU_COPY,
		MENU_SELECT_ALL,
		MENU_MAX
	};

	enum DefaultFont {
		RTL_NORMAL_FONT,
		RTL_BOLD_FONT,
		RTL_ITALICS_FONT,
		RTL_BOLD_ITALICS_FONT,
		RTL_MONO_FONT,
		RTL_CUSTOM_FONT,
	};

	enum ImageUpdateMask {
		UPDATE_TEXTURE = 1 << 0,
		UPDATE_SIZE = 1 << 1,
		UPDATE_COLOR = 1 << 2,
		UPDATE_ALIGNMENT = 1 << 3,
		UPDATE_REGION = 1 << 4,
		UPDATE_PAD = 1 << 5,
		UPDATE_TOOLTIP = 1 << 6,
		UPDATE_WIDTH_IN_PERCENT = 1 << 7,
	};

protected:
	void _notification(int p_what);
	static void _bind_methods();

#ifdef TOOLS_ENABLED
    static bool has_gizmo_plugin;
#endif

	void _font_changed();
	void queue_redraw();
	void _im_update();
	void _draw();


private:
	real_t pixel_size = 0.005;
	bool flags[FLAG_MAX] = {};
	AlphaCutMode alpha_cut = ALPHA_CUT_DISABLED;
	float alpha_scissor_threshold = 0.5;
	float alpha_hash_scale = 1.0;
	StandardMaterial3D::AlphaAntiAliasing alpha_antialiasing_mode = StandardMaterial3D::ALPHA_ANTIALIASING_OFF;
	float alpha_antialiasing_edge = 0.0f;

	AABB aabb;

	mutable Ref<TriangleMesh> triangle_mesh;
	RID mesh;
	struct SurfaceData {
		PackedVector3Array mesh_vertices;
		PackedVector3Array mesh_normals;
		PackedFloat32Array mesh_tangents;
		PackedColorArray mesh_colors;
		PackedVector2Array mesh_uvs;
		PackedInt32Array indices;
		int offset = 0;
		float z_shift = 0.0;
		RID material;
	};

	struct SurfaceKey {
		uint64_t texture_id;
		int32_t priority;
		int32_t outline_size;

		bool operator==(const SurfaceKey &p_b) const {
			return (texture_id == p_b.texture_id) && (priority == p_b.priority) && (outline_size == p_b.outline_size);
		}

		SurfaceKey(uint64_t p_texture_id, int p_priority, int p_outline_size) {
			texture_id = p_texture_id;
			priority = p_priority;
			outline_size = p_outline_size;
		}
	};

	struct SurfaceKeyHasher {
		_FORCE_INLINE_ static uint32_t hash(const SurfaceKey &p_a) {
			return hash_murmur3_buffer(&p_a, sizeof(SurfaceKey));
		}
	};

	HashMap<SurfaceKey, SurfaceData, SurfaceKeyHasher> surfaces;

	float width = 500.0;
	float height = 500.0;
	int font_size = 32;
	Ref<Font> font_override;
	mutable Ref<Font> theme_font;
	Ref<Font> bold_font;
	Ref<Font> italics_font;
	Ref<Font> bold_italics_font;
	Ref<Font> mono_font;
	int bold_font_size = 22;
	int italics_font_size = 22;
	int bold_italics_font_size = 22;
	int mono_font_size = 22;
	int outline_size = 12;
	float outline_depth = 0.15f;
	float cell_children_depth = 0.15f;


	Color modulate = Color(1, 1, 1, 1);
	Color outline_modulate = Color(0, 0, 0, 1);

	Point2 lbl_offset;
	int outline_render_priority = -1;
	int render_priority = 0;

	float line_spacing = 0.0f;

	RID base_material;
	StandardMaterial3D::BillboardMode billboard_mode = StandardMaterial3D::BILLBOARD_DISABLED;
	StandardMaterial3D::TextureFilter texture_filter = StandardMaterial3D::TEXTURE_FILTER_LINEAR_WITH_MIPMAPS;

	void set_billboard_mode(StandardMaterial3D::BillboardMode p_mode);
	StandardMaterial3D::BillboardMode get_billboard_mode() const;

	void set_texture_filter(StandardMaterial3D::TextureFilter p_filter);
	StandardMaterial3D::TextureFilter get_texture_filter() const;

	void set_draw_flag(DrawFlags p_flag, bool p_enable);
	bool get_draw_flag(DrawFlags p_flag) const;

	void set_outline_depth(float p_depth);
	float get_outline_depth() const;

	void set_cell_children_depth(float p_depth);
	float get_cell_children_depth() const;

	void set_alpha_cut_mode(AlphaCutMode p_mode);
	AlphaCutMode get_alpha_cut_mode() const;

	void set_alpha_scissor_threshold(float p_threshold);
	float get_alpha_scissor_threshold() const;

	void set_alpha_hash_scale(float p_hash_scale);
	float get_alpha_hash_scale() const;

	void set_alpha_antialiasing(BaseMaterial3D::AlphaAntiAliasing p_alpha_aa);
	BaseMaterial3D::AlphaAntiAliasing get_alpha_antialiasing() const;

	void set_alpha_antialiasing_edge(float p_edge);
	float get_alpha_antialiasing_edge() const;

	bool pending_update = false;

	bool dirty_lines = true;
	bool dirty_font = true;
	bool dirty_text = true;
	
	void _generate_rich_glyph_surface(const Glyph &p_glyph, Vector2 &r_offset, Vector3 &extra_offset, Vector3 &extra_rotation, const Color &p_color, int p_priority = 0, int p_outline_size = 0, int p_draw_step = 0);
	void _generate_border_surface(const Vector2 &p_position, const Size2 &p_size,  Vector3 &extra_offset, Vector3 &extra_rotation, const Color &p_color, int p_priority);
	void _generate_rect_surface(const Vector2 &p_position, const Size2 &p_size, Vector3 &extra_offset, Vector3 &extra_rotation, const Color &p_color, int p_priority, float thickness = 0.0f, float corner_radius = 0.0f, int corner_segments = 8, int bevel_segments = 1);
	void _generate_image_surface(const Ref<Texture2D> &p_image, const Vector2 &p_position, const Size2 &p_size, Vector3 &extra_offset, Vector3 &extra_rotation, const Color &p_color, int p_priority);

	void set_render_priority(int p_priority);
	int get_render_priority() const;

	void set_outline_render_priority(int p_priority);
	int get_outline_render_priority() const;

	void set_outline_size(int p_size);
	int get_outline_size() const;

	void set_line_spacing(float p_size);
	float get_line_spacing() const;

	void set_modulate(const Color &p_color);
	Color get_modulate() const;

	void set_outline_modulate(const Color &p_color);
	Color get_outline_modulate() const;

	void set_pixel_size(real_t p_amount);
	real_t get_pixel_size() const;

	struct Item;

	struct Line {
		Item *from = nullptr;

		Ref<TextLine> text_prefix;
		float prefix_width = 0;
		Ref<TextParagraph> text_buf;

		Item *dc_item = nullptr;
		Color dc_color;
		int dc_ol_size = 0;
		Color dc_ol_color;

		Vector2 offset;
		float indent = 0.0;
		int char_offset = 0;
		int char_count = 0;

		Line() {
			text_buf.instantiate();
		}
		~Line() {
		}

		_FORCE_INLINE_ float get_height(float p_line_separation, float p_paragraph_separation) const {
			return offset.y + text_buf->get_size().y + (text_buf->get_line_count() - 1) * p_line_separation + p_paragraph_separation;
		}
	};

	struct Item {
		int index = 0;
		int char_ofs = 0;
		Item *parent = nullptr;
		ItemType type = ITEM_FRAME;
		List<Item *> subitems;
		List<Item *>::Element *E = nullptr;
		ObjectID owner;
		int line = 0;
		RID rid;

		void _clear_children() {
			RichTextLabel3D *owner_rtl = ObjectDB::get_instance<RichTextLabel3D>(owner);
			while (subitems.size()) {
				Item *subitem = subitems.front()->get();
				if (subitem && subitem->rid.is_valid() && owner_rtl) {
					owner_rtl->items.free(subitem->rid);
				}
				memdelete(subitem);
				subitems.pop_front();
			}
		}

		virtual ~Item() { _clear_children(); }
	};

	struct ItemFrame : public Item {
		bool cell = false;

		LocalVector<Line> lines;
		std::atomic<int> first_invalid_line;
		std::atomic<int> first_invalid_font_line;
		std::atomic<int> first_resized_line;

		ItemFrame *parent_frame = nullptr;

		Color odd_row_bg = Color(0, 0, 0, 0);
		Color even_row_bg = Color(0, 0, 0, 0);
		Color border = Color(0, 0, 0, 0);
		Size2 min_size_over = Size2(-1, -1);
		Size2 max_size_over = Size2(-1, -1);
		Rect2 padding;
		Rect2 margins;
		Variant meta;
		Vector3 offset;
		Vector3 rotation;
		Vector2 radius;
		int bevel_segments = 0;
		float thickness = 0.0f;
		int indent_level = 0;
		bool has_meta = false;
		bool has_image_background = false;
		bool vertical_align_center = false;

		ItemFrame() {
			type = ITEM_FRAME;
			first_invalid_line.store(0);
			first_invalid_font_line.store(0);
			first_resized_line.store(0);
		}
	};

	struct ItemFrameImageBackground : public ItemFrame {
		Ref<Texture2D> image;
		bool width_in_percent = false;
		bool height_in_percent = false;
		bool has_rect_offset = false;
		bool keep_aspect_center = false;
		Rect2 region;
		Size2 size;
		Size2 rq_size;
		Rect2 rect_offset;
		Variant key;
		ItemFrameImageBackground() {
			type = ITEM_FRAME;
			has_image_background = true;
			first_invalid_line.store(0);
			first_invalid_font_line.store(0);
			first_resized_line.store(0);
		}
		~ItemFrameImageBackground() {
			if (image.is_valid()) {
				RichTextLabel3D *owner_rtl = ObjectDB::get_instance<RichTextLabel3D>(owner);
				if (owner_rtl) {
					if (owner_rtl->hr_list.has(rid)) {
						owner_rtl->hr_list.erase((rid));
					}
				}
			}
		}
	};

	struct ItemText : public Item {
		String text;
		ItemText() { type = ITEM_TEXT; }
	};

	struct ItemDropcap : public Item {
		String text;
		Ref<Font> font;
		int font_size = 0;
		Color color;
		int ol_size = 0;
		Color ol_color;
		Rect2 dropcap_margins;
		ItemDropcap() { type = ITEM_DROPCAP; }
		~ItemDropcap() {
			if (font.is_valid()) {
				RichTextLabel3D *owner_rtl = ObjectDB::get_instance<RichTextLabel3D>(owner);
				if (owner_rtl) {
					font->disconnect_changed(callable_mp(owner_rtl, &RichTextLabel3D::_invalidate_fonts));
				}
			}
		}
	};

	struct ItemImage : public Item {
		Ref<Texture2D> image;
		String alt_text;
		InlineAlignment inline_align = INLINE_ALIGNMENT_CENTER;
		bool pad = false;
		bool width_in_percent = false;
		bool height_in_percent = false;
		Rect2 region;
		Size2 size;
		Size2 rq_size;
		Color color;
		Variant key;
		String tooltip;
		ItemImage() { type = ITEM_IMAGE; }
		~ItemImage() {
			if (image.is_valid()) {
				RichTextLabel3D *owner_rtl = ObjectDB::get_instance<RichTextLabel3D>(owner);
				if (owner_rtl) {
					if (owner_rtl->hr_list.has(rid)) {
						owner_rtl->hr_list.erase((rid));
					}
				}
			}
		}
	};

	struct ItemFont : public Item {
		DefaultFont def_font = RTL_CUSTOM_FONT;
		Ref<Font> font;
		bool variation = false;
		bool def_size = false;
		int font_size = 0;
		ItemFont() { type = ITEM_FONT; }
		~ItemFont() {
			if (font.is_valid()) {
				RichTextLabel3D *owner_rtl = ObjectDB::get_instance<RichTextLabel3D>(owner);
				if (owner_rtl) {
					font->disconnect_changed(callable_mp(owner_rtl, &RichTextLabel3D::_invalidate_fonts));
				}
			}
		}
	};

	struct ItemFontSize : public Item {
		int font_size = 16;
		ItemFontSize() { type = ITEM_FONT_SIZE; }
	};

	struct ItemColor : public Item {
		Color color;
		ItemColor() { type = ITEM_COLOR; }
	};

	struct ItemOutlineSize : public Item {
		int outline_size = 0;
		ItemOutlineSize() { type = ITEM_OUTLINE_SIZE; }
	};

	struct ItemOutlineColor : public Item {
		Color color;
		ItemOutlineColor() { type = ITEM_OUTLINE_COLOR; }
	};

	struct ItemUnderline : public Item {
		Color color;
		ItemUnderline() { type = ITEM_UNDERLINE; }
	};

	struct ItemStrikethrough : public Item {
		Color color;
		ItemStrikethrough() { type = ITEM_STRIKETHROUGH; }
	};

	struct ItemMeta : public Item {
		Variant meta;
		MetaUnderline underline = META_UNDERLINE_ALWAYS;
		String tooltip;
		ItemMeta() { type = ITEM_META; }
	};

	struct ItemHint : public Item {
		String description;
		ItemHint() { type = ITEM_HINT; }
	};

	struct ItemLanguage : public Item {
		String language;
		ItemLanguage() { type = ITEM_LANGUAGE; }
	};

	struct ItemParagraph : public Item {
		HorizontalAlignment alignment = HORIZONTAL_ALIGNMENT_LEFT;
		String language;
		Control::TextDirection direction = Control::TEXT_DIRECTION_AUTO;
		TextServer::StructuredTextParser st_parser = TextServer::STRUCTURED_TEXT_DEFAULT;
		BitField<TextServer::JustificationFlag> jst_flags = TextServer::JUSTIFICATION_WORD_BOUND | TextServer::JUSTIFICATION_KASHIDA | TextServer::JUSTIFICATION_SKIP_LAST_LINE | TextServer::JUSTIFICATION_DO_NOT_SKIP_SINGLE_LINE;
		PackedFloat32Array tab_stops;
		ItemParagraph() { type = ITEM_PARAGRAPH; }
	};

	struct ItemIndent : public Item {
		int level = 0;
		ItemIndent() { type = ITEM_INDENT; }
	};

	struct ItemList : public Item {
		ListType list_type = LIST_DOTS;
		bool capitalize = false;
		int level = 0;
		String bullet = U"•";
		float max_width = 0;
		ItemList() { type = ITEM_LIST; }
	};

	struct ItemNewline : public Item {
		ItemNewline() { type = ITEM_NEWLINE; }
	};

	struct ItemTable : public Item {
		struct Column {
			String name;
			bool expand = false;
			bool shrink = true;
			int expand_ratio = 0;
			int min_width = 0;
			int max_width = 0;
			int width = 0;
			int width_with_padding = 0;
		};

		LocalVector<Column> columns;
		LocalVector<float> rows;
		LocalVector<float> rows_no_padding;
		LocalVector<float> rows_baseline;
		String name;

		int align_to_row = -1;
		int total_width = 0;
		int total_height = 0;
		InlineAlignment inline_align = INLINE_ALIGNMENT_TOP;
		ItemTable() { type = ITEM_TABLE; }
	};

	struct ItemBGColor : public Item {
		Color color;
		ItemBGColor() { type = ITEM_BGCOLOR; }
	};

	struct ItemFGColor : public Item {
		Color color;
		ItemFGColor() { type = ITEM_FGCOLOR; }
	};

	struct ItemContext : public Item {
		ItemContext() { type = ITEM_CONTEXT; }
	};

	ItemFrame *main = nullptr;
	Item *current = nullptr;
	ItemFrame *current_frame = nullptr;

	WorkerThreadPool::TaskID task = WorkerThreadPool::INVALID_TASK_ID;
	Mutex data_mutex;
	bool threaded = false;
	std::atomic<bool> stop_thread;
	std::atomic<bool> updating;
	std::atomic<bool> validating;
	std::atomic<double> loaded;
	std::atomic<bool> parsing_bbcode;

	uint64_t loading_started = 0;
	int progress_delay = 1000;

	TextServer::AutowrapMode autowrap_mode = TextServer::AUTOWRAP_WORD_SMART;
	BitField<TextServer::LineBreakFlag> autowrap_flags_trim = TextServer::BREAK_TRIM_START_EDGE_SPACES | TextServer::BREAK_TRIM_END_EDGE_SPACES;

	// bool scroll_visible = false;
	// bool scroll_follow = false;
	// bool scroll_follow_visible_characters = false;
	int follow_vc_pos = 0;
	// bool scroll_following = false;
	// bool scroll_active = true;
	// int scroll_w = 0;
	// bool scroll_updated = false;
	// bool updating_scroll = false;
	int current_idx = 1;
	int current_char_ofs = 0;
	int visible_paragraph_count = 0;
	int visible_line_count = 0;
	Rect2i visible_rect;

	int tab_size = 4;
	bool underline_meta = true;
	bool underline_hint = true;
	bool use_selected_font_color = false;

	HorizontalAlignment default_alignment = HORIZONTAL_ALIGNMENT_LEFT;
	VerticalAlignment vertical_alignment = VERTICAL_ALIGNMENT_TOP;
	BitField<TextServer::JustificationFlag> default_jst_flags = TextServer::JUSTIFICATION_WORD_BOUND | TextServer::JUSTIFICATION_KASHIDA | TextServer::JUSTIFICATION_SKIP_LAST_LINE | TextServer::JUSTIFICATION_DO_NOT_SKIP_SINGLE_LINE;
	PackedFloat32Array default_tab_stops;

	ItemMeta *meta_hovering = nullptr;
	Variant current_meta;

	Array custom_effects;

	HashMap<RID, Rect2> ac_element_bounds_cache;
	void _invalidate_current_line(ItemFrame *p_frame);

	void _thread_function(void *p_userdata);
	void _thread_end();
	void _stop_thread();
	bool _validate_line_caches();
	void _process_line_caches();

	void _add_item(Item *p_item, bool p_enter = false, bool p_ensure_newline = false);
	void _remove_frame(HashSet<Item *> &r_erase_list, ItemFrame *p_frame, int p_line, bool p_erase, int p_char_offset, int p_line_offset);

	RID_PtrOwner<Item> items;
	List<String> tag_stack;
	HashSet<RID> hr_list;

	String language;
	TextServer::StructuredTextParser st_parser = TextServer::STRUCTURED_TEXT_DEFAULT;
	Array st_args;

	struct Selection {
		ItemFrame *click_frame = nullptr;
		int click_line = 0;
		Item *click_item = nullptr;
		int click_char = 0;

		ItemFrame *from_frame = nullptr;
		int from_line = 0;
		Item *from_item = nullptr;
		int from_char = 0;

		ItemFrame *to_frame = nullptr;
		int to_line = 0;
		Item *to_item = nullptr;
		int to_char = 0;

		bool double_click = false; // Selecting whole words?
		bool active = false; // anything selected? i.e. from, to, etc. valid?
		bool enabled = false; // allow selections?
		bool drag_attempt = false;
	};

	Selection selection;
	Callable selection_modifier;
	bool deselect_on_focus_loss_enabled = true;
	bool drag_and_drop_selection_enabled = true;

	ItemFrame *keyboard_focus_frame = nullptr;
	int keyboard_focus_line = 0;
	Item *keyboard_focus_item = nullptr;
	bool keyboard_focus_on_text = true;

	bool context_menu_enabled = false;
	bool shortcut_keys_enabled = true;

	// Context menu.
	PopupMenu *menu = nullptr;
	void _generate_context_menu();
	void _update_context_menu();
	Key _get_menu_action_accelerator(const String &p_action);

	int visible_characters = -1;
	float visible_ratio = 1.0;
	TextServer::VisibleCharactersBehavior visible_chars_behavior = TextServer::VC_CHARS_BEFORE_SHAPING;

	bool _is_click_inside_selection() const;
	void _find_click(ItemFrame *p_frame, const Point2i &p_click, ItemFrame **r_click_frame = nullptr, int *r_click_line = nullptr, Item **r_click_item = nullptr, int *r_click_char = nullptr, bool *r_outside = nullptr, bool p_meta = false);

	String _get_line_text(ItemFrame *p_frame, int p_line, Selection p_sel) const;
	bool _search_line(ItemFrame *p_frame, int p_line, const String &p_string, int p_char_idx, bool p_reverse_search);
	bool _search_table(ItemTable *p_table, List<Item *>::Element *p_from, const String &p_string, bool p_reverse_search);

	float _shape_line(ItemFrame *p_frame, int p_line, const Ref<Font> &p_base_font, int p_base_font_size, int p_width, float p_h, int *r_char_offset);
	float _resize_line(ItemFrame *p_frame, int p_line, const Ref<Font> &p_base_font, int p_base_font_size, int p_width, float p_h);

	void _set_table_size(ItemTable *p_table, int p_available_width);

	void _update_line_font(ItemFrame *p_frame, int p_line, const Ref<Font> &p_base_font, int p_base_font_size);
	int _draw_line(ItemFrame *p_frame, int p_line, const Vector2 &p_ofs, const Vector3 &l_extra_offset, Vector3 &l_extra_rotation, int p_width, float p_vsep, bool inside_table, const Color &p_base_color, int p_outline_size, const Color &p_outline_color, const Color &p_font_shadow_color, int p_shadow_outline_size, const Point2 &p_shadow_ofs, int &r_processed_glyphs);
	float _find_click_in_line(ItemFrame *p_frame, int p_line, const Vector2 &p_ofs, int p_width, float p_vsep, const Point2i &p_click, ItemFrame **r_click_frame = nullptr, int *r_click_line = nullptr, Item **r_click_item = nullptr, int *r_click_char = nullptr, bool p_table = false, bool p_meta = false);

	String _roman(int p_num, bool p_capitalize) const;
	String _letters(int p_num, bool p_capitalize) const;

	Item *_find_indentable(Item *p_item);
	Item *_get_item_at_pos(Item *p_item_from, Item *p_item_to, int p_position);
	void _find_frame(Item *p_item, ItemFrame **r_frame, int *r_line);
	ItemFontSize *_find_font_size(Item *p_item);
	ItemFont *_find_font(Item *p_item);
	int _find_outline_size(Item *p_item, int p_default);
	ItemList *_find_list_item(Item *p_item);
	ItemDropcap *_find_dc_item(Item *p_item);
	int _find_list(Item *p_item, Vector<int> &r_index, Vector<int> &r_count, Vector<ItemList *> &r_list);
	int _find_margin(Item *p_item, const Ref<Font> &p_base_font, int p_base_font_size);
	PackedFloat32Array _find_tab_stops(Item *p_item);
	HorizontalAlignment _find_alignment(Item *p_item);
	BitField<TextServer::JustificationFlag> _find_jst_flags(Item *p_item);
	TextServer::Direction _find_direction(Item *p_item);
	TextServer::StructuredTextParser _find_stt(Item *p_item);
	String _find_language(Item *p_item);
	Color _find_color(Item *p_item, const Color &p_default_color);
	Color _find_outline_color(Item *p_item, const Color &p_default_color);
	bool _find_underline(Item *p_item, Color *r_color = nullptr);
	bool _find_strikethrough(Item *p_item, Color *r_color = nullptr);
	bool _find_meta(Item *p_item, Variant *r_meta, ItemMeta **r_item = nullptr);
	bool _find_hint(Item *p_item, String *r_description);
	Color _find_bgcolor(Item *p_item);
	Color _find_fgcolor(Item *p_item);
	bool _find_layout_subitem(Item *from, Item *to);
	void _normalize_subtags(Vector<String> &subtags);

	int _find_first_line(int p_from, int p_to, int p_vofs) const;

	_FORCE_INLINE_ float _calculate_line_vertical_offset(const Line &line) const;

	Item *_get_next_item(Item *p_item, bool p_free = false) const;
	Item *_get_prev_item(Item *p_item, bool p_free = false) const;

	Rect2 _get_text_rect();

	virtual Dictionary parse_expressions_for_values(Vector<String> p_expressions);

	void _invalidate_fonts();

	Size2 _get_image_size(const Ref<Texture2D> &p_image, int p_width = 0, int p_height = 0, const Rect2 &p_region = Rect2());

	String _get_prefix(Item *p_item, const Vector<int> &p_list_index, const Vector<ItemList *> &p_list_items);

	static int _find_unquoted(const String &p_src, char32_t p_chr, int p_from);
	static Vector<String> _split_unquoted(const String &p_src, char32_t p_splitter);
	static String _get_tag_value(const String &p_tag);

	bool use_bbcode = false;
	String text;
	void _apply_translation();

	bool internal_stack_editing = false;
	bool stack_externally_modified = false;

	bool fit_content = false;

	struct ThemeCache {
		// Ref<StyleBox> normal_style;
		// Ref<StyleBox> focus_style;
		// Ref<StyleBox> progress_bg_style;
		// Ref<StyleBox> progress_fg_style;

		Ref<Texture2D> horizontal_rule;

		int line_separation = 0;
		int paragraph_separation = 0;

		Color font_shadow_color;
		int shadow_outline_size;
		int shadow_offset_x;
		int shadow_offset_y;

		int text_highlight_h_padding;
		int text_highlight_v_padding;

		int underline_alpha;
		int strikethrough_alpha;

		int table_h_separation = 3;
		int table_v_separation = 3;
		Color table_odd_row_bg = Color(0.0, 0.0, 0.0, 0.0);
		Color table_even_row_bg = Color(0.0, 0.0, 0.0, 0.0);
		Color table_border = Color(0.0, 0.0, 0.0, 0.0);

		float base_scale = 1.0;
	} theme_cache;

public:

	String get_parsed_text() const;
	void add_text(const String &p_text);
	void add_hr(int p_width = 90, int p_height = 2, const Color &p_color = Color(1.0, 1.0, 1.0), HorizontalAlignment p_alignment = HORIZONTAL_ALIGNMENT_LEFT, bool p_width_in_percent = true, bool p_height_in_percent = false);
	void add_image(const Ref<Texture2D> &p_image, int p_width = 0, int p_height = 0, const Color &p_color = Color(1.0, 1.0, 1.0), InlineAlignment p_alignment = INLINE_ALIGNMENT_CENTER, const Rect2 &p_region = Rect2(), const Variant &p_key = Variant(), bool p_pad = false, const String &p_tooltip = String(), bool p_width_in_percent = false, bool p_height_in_percent = false, const String &p_alt_text = String());
	// void update_image(const Variant &p_key, BitField<ImageUpdateMask> p_mask, const Ref<Texture2D> &p_image, int p_width = 0, int p_height = 0, const Color &p_color = Color(1.0, 1.0, 1.0), InlineAlignment p_alignment = INLINE_ALIGNMENT_CENTER, const Rect2 &p_region = Rect2(), bool p_pad = false, const String &p_tooltip = String(), bool p_width_in_percent = false, bool p_height_in_percent = false);
	void add_newline();
	bool remove_paragraph(int p_paragraph, bool p_no_invalidate = false);
	bool invalidate_paragraph(int p_paragraph);
	void push_dropcap(const String &p_string, const Ref<Font> &p_font, int p_size, const Rect2 &p_dropcap_margins = Rect2(), const Color &p_color = Color(1, 1, 1), int p_ol_size = 0, const Color &p_ol_color = Color(0, 0, 0, 0));
	void _push_def_font(DefaultFont p_def_font);
	void _push_def_font_var(DefaultFont p_def_font, const Ref<Font> &p_font, int p_size = -1);
	void push_font(const Ref<Font> &p_font, int p_size = 0);
	void push_font_size(int p_font_size);
	void push_outline_size(int p_font_size);
	void push_normal();
	void push_bold();
	void push_bold_italics();
	void push_italics();
	void push_mono();
	void push_color(const Color &p_color);
	void push_outline_color(const Color &p_color);
	void push_underline(const Color &p_color = Color(0, 0, 0, 0));
	void push_strikethrough(const Color &p_color = Color(0, 0, 0, 0));
	void push_language(const String &p_language);
	void push_paragraph(HorizontalAlignment p_alignment, Control::TextDirection p_direction = Control::TEXT_DIRECTION_INHERITED, const String &p_language = "", TextServer::StructuredTextParser p_st_parser = TextServer::STRUCTURED_TEXT_DEFAULT, BitField<TextServer::JustificationFlag> p_jst_flags = TextServer::JUSTIFICATION_WORD_BOUND | TextServer::JUSTIFICATION_KASHIDA | TextServer::JUSTIFICATION_SKIP_LAST_LINE | TextServer::JUSTIFICATION_DO_NOT_SKIP_SINGLE_LINE, const PackedFloat32Array &p_tab_stops = PackedFloat32Array());
	void push_indent(int p_level);
	void push_list(int p_level, ListType p_list, bool p_capitalize, const String &p_bullet = String::utf8("•"));
	void push_meta(const Variant &p_meta, MetaUnderline p_underline_mode = META_UNDERLINE_ALWAYS, const String &p_tooltip = String());
	void push_hint(const String &p_string);
	void push_table(int p_columns, InlineAlignment p_alignment = INLINE_ALIGNMENT_TOP, int p_align_to_row = -1, const String &p_name = String());
	void push_bgcolor(const Color &p_color);
	void push_fgcolor(const Color &p_color);
	void push_context();
	void set_table_column_expand(int p_column, bool p_expand, int p_ratio = 1, bool p_shrink = true);
	void set_table_column_name(int p_column, const String &p_name);
	void set_cell_meta(const Variant &p_meta);
	void set_cell_offset(const Vector3 &p_offset);
	void set_cell_rotation(const Vector3 &p_rotation);
	void set_cell_radius(const Vector2 &cell_radius);
	void set_cell_bevel_segments(int p_bevel_segments);
	void set_cell_thickness(float p_thickness);
	void set_cell_row_background_color(const Color &p_odd_row_bg, const Color &p_even_row_bg);
	void set_cell_border_color(const Color &p_color);
	void set_cell_vertical_align_center(bool p_vertical_align_center);
	void push_cell_with_image_background(const Ref<Texture2D> &p_image, const Rect2 &p_region, const Variant &p_key, bool p_width_in_percent, bool p_height_in_percent);
	void set_cell_size_override(const Size2 &p_min_size, const Size2 &p_max_size);
	void set_cell_padding(const Rect2 &p_padding);
	void set_cell_margins(const Rect2 &p_margins);
	void set_cell_bg_image_offset(const Rect2 &p_img_offset);
	void set_cell_bg_image_keep_aspect_center(bool p_keep_aspect_center);
	int get_current_table_column() const;
	void push_cell();
	void pop();
	void pop_context();
	void pop_all();

	Ref<Mesh> get_mesh() const;
	void set_font(const Ref<Font> &p_font);
	Ref<Font> get_font() const;
	void set_italics_font(const Ref<Font> &p_font);
	Ref<Font> get_italics_font() const;
	void set_bold_font(const Ref<Font> &p_font);
	Ref<Font> get_bold_font() const;
	void set_bold_italics_font(const Ref<Font> &p_font);
	Ref<Font> get_bold_italics_font() const;
	void set_mono_font(const Ref<Font> &p_font);
	Ref<Font> get_mono_font() const;
	Ref<Font> _get_font_or_default() const;
	float get_width() const { return width; }
	float get_height() const { return height; }
	void set_width(float p_width) { width = p_width; }
	void set_height(float p_height) { height = p_height; }
	int get_font_size() const { return font_size; }
	void set_font_size(int p_font_size) { font_size = p_font_size; queue_redraw(); }
	void set_bold_font_size(int p_font_size) { bold_font_size = p_font_size; queue_redraw(); }
	void set_italics_font_size(int p_font_size) { italics_font_size = p_font_size; queue_redraw(); }
	void set_bold_italics_font_size(int p_font_size) { bold_italics_font_size = p_font_size; queue_redraw(); }
	void set_mono_font_size(int p_font_size) { mono_font_size = p_font_size; queue_redraw(); }
	int get_bold_font_size() const { return bold_font_size; }
	int get_italics_font_size() const { return italics_font_size; }
	int get_bold_italics_font_size() const { return bold_italics_font_size; }
	int get_mono_font_size() const { return mono_font_size; }

	void clear();

	void set_offset(int p_pixel);

	void set_tab_size(int p_spaces);
	int get_tab_size() const;

	int get_paragraph_count() const;
	int get_visible_paragraph_count() const;

	float get_line_offset(int p_line);
	float get_paragraph_offset(int p_paragraph);

	int get_line_count() const;
	Vector2i get_line_range(int p_line);
	int get_visible_line_count() const;

	int get_content_height() const;
	int get_content_width() const;

	int get_line_height(int p_line) const;
	int get_line_width(int p_line) const;

	Rect2i get_visible_content_rect() const;


	// virtual CursorShape get_cursor_shape(const Point2 &p_pos) const override;
	// virtual Variant get_drag_data(const Point2 &p_point) override;

	int get_pending_paragraphs() const;
	bool is_finished() const;
	bool is_updating() const;
	void wait_until_finished();

	void set_threaded(bool p_threaded);
	bool is_threaded() const;

	void parse_bbcode(const String &p_bbcode);
	void append_text(const String &p_bbcode);

	void set_use_bbcode(bool p_enable);
	bool is_using_bbcode() const;

	void set_text(const String &p_bbcode);
	String get_text() const;

	void set_horizontal_alignment(HorizontalAlignment p_alignment);
	HorizontalAlignment get_horizontal_alignment() const;

	void set_vertical_alignment(VerticalAlignment p_alignment);
	VerticalAlignment get_vertical_alignment() const;

	void set_justification_flags(BitField<TextServer::JustificationFlag> p_flags);
	BitField<TextServer::JustificationFlag> get_justification_flags() const;

	void set_tab_stops(const PackedFloat32Array &p_tab_stops);
	PackedFloat32Array get_tab_stops() const;

	// void set_text_direction(TextDirection p_text_direction);
	// TextDirection get_text_direction() const;

	// void set_language(const String &p_language);
	// String get_language() const;

	void set_autowrap_mode(TextServer::AutowrapMode p_mode);
	TextServer::AutowrapMode get_autowrap_mode() const;

	void set_autowrap_trim_flags(BitField<TextServer::LineBreakFlag> p_flags);
	BitField<TextServer::LineBreakFlag> get_autowrap_trim_flags() const;

	// void set_structured_text_bidi_override(TextServer::StructuredTextParser p_parser);
	// TextServer::StructuredTextParser get_structured_text_bidi_override() const;

	// void set_structured_text_bidi_override_options(Array p_args);
	// Array get_structured_text_bidi_override_options() const;

	void set_visible_characters(int p_visible);
	int get_visible_characters() const;
	int get_character_line(int p_char);
	int get_character_paragraph(int p_char);
	int get_total_character_count() const;
	int get_total_glyph_count() const;

	void set_visible_ratio(float p_ratio);
	float get_visible_ratio() const;

	TextServer::VisibleCharactersBehavior get_visible_characters_behavior() const;
	void set_visible_characters_behavior(TextServer::VisibleCharactersBehavior p_behavior);

	virtual AABB get_aabb() const override;
	virtual Ref<TriangleMesh> generate_triangle_mesh() const override;

	RichTextLabel3D(const String &p_text = String());
	~RichTextLabel3D();
};

VARIANT_ENUM_CAST(RichTextLabel3D::ListType);
VARIANT_ENUM_CAST(RichTextLabel3D::MenuItems);
VARIANT_ENUM_CAST(RichTextLabel3D::MetaUnderline);
VARIANT_BITFIELD_CAST(RichTextLabel3D::ImageUpdateMask);
VARIANT_ENUM_CAST(RichTextLabel3D::DrawFlags);
VARIANT_ENUM_CAST(RichTextLabel3D::AlphaCutMode);
