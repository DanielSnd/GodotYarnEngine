// /**************************************************************************/
// /*  rich_text_label.cpp                                                   */
// /**************************************************************************/
// /*                         This file is part of:                          */
// /*                             GODOT ENGINE                               */
// /*                        https://godotengine.org                         */
// /**************************************************************************/
// /* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
// /* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
// /*                                                                        */
// /* Permission is hereby granted, free of charge, to any person obtaining  */
// /* a copy of this software and associated documentation files (the        */
// /* "Software"), to deal in the Software without restriction, including    */
// /* without limitation the rights to use, copy, modify, merge, publish,    */
// /* distribute, sublicense, and/or sell copies of the Software, and to     */
// /* permit persons to whom the Software is furnished to do so, subject to  */
// /* the following conditions:                                              */
// /*                                                                        */
// /* The above copyright notice and this permission notice shall be         */
// /* included in all copies or substantial portions of the Software.        */
// /*                                                                        */
// /* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
// /* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
// /* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
// /* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
// /* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
// /* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
// /* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
// /**************************************************************************/

#include "richtextlabel3d.h"

// #include "core/input/input_map.h"
// #include "core/math/math_defs.h"
// #include "core/os/keyboard.h"
// #include "core/os/os.h"
// #include "scene/gui/label.h"
// #include "scene/gui/rich_text_effect.h"
// #include "scene/resources/atlas_texture.h"
// #include "scene/theme/theme_db.h"
// #include "servers/display_server.h"

#ifdef TOOLS_ENABLED
bool RichTextLabel3D::has_gizmo_plugin = false;
#endif

Rect2i _richtextlabel3d_merge_or_copy_rect(const Rect2i &p_a, const Rect2i &p_b) {
	if (!p_a.has_area()) {
		return p_b;
	} else {
		return p_a.merge(p_b);
	}
}

RichTextLabel3D::Item *RichTextLabel3D::_get_next_item(Item *p_item, bool p_free) const {
	if (!p_item) {
		return nullptr;
	}
	if (p_free) {
		if (p_item->subitems.size()) {
			return p_item->subitems.front()->get();
		} else if (!p_item->parent) {
			return nullptr;
		} else if (p_item->E->next()) {
			return p_item->E->next()->get();
		} else {
			// Go up until something with a next is found.
			while (p_item->parent && !p_item->E->next()) {
				p_item = p_item->parent;
			}

			if (p_item->parent) {
				return p_item->E->next()->get();
			} else {
				return nullptr;
			}
		}

	} else {
		if (p_item->subitems.size() && p_item->type != ITEM_TABLE) {
			return p_item->subitems.front()->get();
		} else if (p_item->type == ITEM_FRAME) {
			return nullptr;
		} else if (p_item->E->next()) {
			return p_item->E->next()->get();
		} else {
			// Go up until something with a next is found.
			while (p_item->parent && p_item->type != ITEM_FRAME && !p_item->E->next()) {
				p_item = p_item->parent;
			}

			if (p_item->type != ITEM_FRAME) {
				return p_item->E->next()->get();
			} else {
				return nullptr;
			}
		}
	}
}

RichTextLabel3D::Item *RichTextLabel3D::_get_prev_item(Item *p_item, bool p_free) const {
	if (!p_item) {
		return nullptr;
	}
	if (p_free) {
		if (!p_item->parent) {
			return nullptr;
		} else if (p_item->E->prev()) {
			p_item = p_item->E->prev()->get();
			while (p_item->subitems.size()) {
				p_item = p_item->subitems.back()->get();
			}
			return p_item;
		} else {
			if (p_item->parent) {
				return p_item->parent;
			} else {
				return nullptr;
			}
		}

	} else {
		if (p_item->type == ITEM_FRAME) {
			return nullptr;
		} else if (p_item->E->prev()) {
			p_item = p_item->E->prev()->get();
			while (p_item->subitems.size() && p_item->type != ITEM_TABLE) {
				p_item = p_item->subitems.back()->get();
			}
			return p_item;
		} else {
			if (p_item->parent && p_item->type != ITEM_FRAME) {
				return p_item->parent;
			} else {
				return nullptr;
			}
		}
	}
}

Rect2 RichTextLabel3D::_get_text_rect() {
	return Rect2(Size2i(0,0), Size2i(width, height));
}

RichTextLabel3D::Item *RichTextLabel3D::_get_item_at_pos(RichTextLabel3D::Item *p_item_from, RichTextLabel3D::Item *p_item_to, int p_position) {
	int offset = 0;
	for (Item *it = p_item_from; it && it != p_item_to; it = _get_next_item(it)) {
		switch (it->type) {
			case ITEM_TEXT: {
				ItemText *t = static_cast<ItemText *>(it);
				offset += t->text.length();
				if (offset > p_position) {
					return it;
				}
			} break;
			case ITEM_NEWLINE: {
				offset += 1;
				if (offset == p_position) {
					return it;
				}
			} break;
			case ITEM_IMAGE: {
				offset += 1;
				if (offset > p_position) {
					return it;
				}
			} break;
			case ITEM_TABLE: {
				offset += 1;
			} break;
			default:
				break;
		}
	}
	return p_item_from;
}

String RichTextLabel3D::_roman(int p_num, bool p_capitalize) const {
	if (p_num > 3999) {
		return "ERR";
	};
	String s;
	if (p_capitalize) {
		const String roman_M[] = { "", "M", "MM", "MMM" };
		const String roman_C[] = { "", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM" };
		const String roman_X[] = { "", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC" };
		const String roman_I[] = { "", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX" };
		s = roman_M[p_num / 1000] + roman_C[(p_num % 1000) / 100] + roman_X[(p_num % 100) / 10] + roman_I[p_num % 10];
	} else {
		const String roman_M[] = { "", "m", "mm", "mmm" };
		const String roman_C[] = { "", "c", "cc", "ccc", "cd", "d", "dc", "dcc", "dccc", "cm" };
		const String roman_X[] = { "", "x", "xx", "xxx", "xl", "l", "lx", "lxx", "lxxx", "xc" };
		const String roman_I[] = { "", "i", "ii", "iii", "iv", "v", "vi", "vii", "viii", "ix" };
		s = roman_M[p_num / 1000] + roman_C[(p_num % 1000) / 100] + roman_X[(p_num % 100) / 10] + roman_I[p_num % 10];
	}
	return s;
}

String RichTextLabel3D::_letters(int p_num, bool p_capitalize) const {
	int64_t n = p_num;

	int chars = 0;
	do {
		n /= 24;
		chars++;
	} while (n);

	String s;
	s.resize_uninitialized(chars + 1);
	char32_t *c = s.ptrw();
	c[chars] = 0;
	n = p_num;
	do {
		int mod = Math::abs(n % 24);
		char a = (p_capitalize ? 'A' : 'a');
		c[--chars] = a + mod - 1;

		n /= 24;
	} while (n);

	return s;
}

String RichTextLabel3D::_get_prefix(Item *p_item, const Vector<int> &p_list_index, const Vector<ItemList *> &p_list_items) {
	String prefix;
	int segments = 0;
	for (int i = 0; i < p_list_index.size(); i++) {
		String segment;
		if (p_list_items[i]->list_type == LIST_DOTS) {
			if (segments == 0) {
				prefix = p_list_items[i]->bullet;
			}
			break;
		}
		prefix = "." + prefix;
		if (p_list_items[i]->list_type == LIST_NUMBERS) {
			segment = itos(p_list_index[i]);
			segments++;
		} else if (p_list_items[i]->list_type == LIST_LETTERS) {
			segment = _letters(p_list_index[i], p_list_items[i]->capitalize);
			segments++;
		} else if (p_list_items[i]->list_type == LIST_ROMAN) {
			segment = _roman(p_list_index[i], p_list_items[i]->capitalize);
			segments++;
		}
		prefix = segment + prefix;
	}
	return prefix + " ";
}

void RichTextLabel3D::_update_line_font(ItemFrame *p_frame, int p_line, const Ref<Font> &p_base_font, int p_base_font_size) {
	ERR_FAIL_NULL(p_frame);
	ERR_FAIL_COND(p_line < 0 || p_line >= (int)p_frame->lines.size());

	Line &l = p_frame->lines[p_line];
	MutexLock lock(l.text_buf->get_mutex());

	// Prefix.
	Vector<int> list_index;
	Vector<int> list_count;
	Vector<ItemList *> list_items;
	_find_list(l.from, list_index, list_count, list_items);

	if (list_items.size() > 0) {
		if (list_index[0] == 1) {
			// List level start, shape all prefixes for this level and compute max. prefix width.
			list_items[0]->max_width = 0;
			int index = 0;
			for (int i = p_line; i < (int)p_frame->lines.size(); i++) {
				Line &list_l = p_frame->lines[i];
				if (_find_list_item(list_l.from) == list_items[0]) {
					index++;

					Ref<Font> font = _get_font_or_default();
					int font_size = font_size;

					ItemFont *font_it = _find_font(list_l.from);
					if (font_it) {
						if (font_it->font.is_valid()) {
							font = font_it->font;
						}
						if (font_it->font_size > 0) {
							font_size = font_it->font_size;
						}
					}
					ItemFontSize *font_size_it = _find_font_size(list_l.from);
					if (font_size_it && font_size_it->font_size > 0) {
						font_size = font_size_it->font_size;
					}

					list_index.write[0] = index;
					String prefix = _get_prefix(list_l.from, list_index, list_items);
					list_l.text_prefix.instantiate();
					list_l.text_prefix->set_direction(_find_direction(list_l.from));
					list_l.text_prefix->add_string(prefix, font, font_size);
					list_items.write[0]->max_width = MAX(list_items[0]->max_width, list_l.text_prefix->get_size().x);
				}
			}
		}
		l.prefix_width = list_items[0]->max_width;
	}

	RID t = l.text_buf->get_rid();
	int spans = TS->shaped_get_span_count(t);
	for (int i = 0; i < spans; i++) {
		Item *it_span = items.get_or_null(TS->shaped_get_span_meta(t, i));
		ItemText *it = reinterpret_cast<ItemText *>(it_span);
		if (it) {
			Ref<Font> font = p_base_font;
			int font_size = p_base_font_size;

			ItemFont *font_it = _find_font(it);
			if (font_it) {
				if (font_it->font.is_valid()) {
					font = font_it->font;
				}
				if (font_it->font_size > 0) {
					font_size = font_it->font_size;
				}
			}
			ItemFontSize *font_size_it = _find_font_size(it);
			if (font_size_it && font_size_it->font_size > 0) {
				font_size = font_size_it->font_size;
			}
			TS->shaped_set_span_update_font(t, i, font->get_rids(), font_size, font->get_opentype_features());
		} else {
			TS->shaped_set_span_update_font(t, i, p_base_font->get_rids(), p_base_font_size, p_base_font->get_opentype_features());
		}
	}

	Item *it_to = (p_line + 1 < (int)p_frame->lines.size()) ? p_frame->lines[p_line + 1].from : nullptr;
	for (Item *it = l.from; it && it != it_to; it = _get_next_item(it)) {
		switch (it->type) {
			case ITEM_TABLE: {
				ItemTable *table = static_cast<ItemTable *>(it);
				for (Item *E : table->subitems) {
					ERR_CONTINUE(E->type != ITEM_FRAME); // Children should all be frames.
					ItemFrame *frame = static_cast<ItemFrame *>(E);
					for (int i = 0; i < (int)frame->lines.size(); i++) {
						_update_line_font(frame, i, p_base_font, p_base_font_size);
					}
				}
			} break;
			default:
				break;
		}
	}
}

float RichTextLabel3D::_resize_line(ItemFrame *p_frame, int p_line, const Ref<Font> &p_base_font, int p_base_font_size, int p_width, float p_h) {
	ERR_FAIL_NULL_V(p_frame, p_h);
	ERR_FAIL_COND_V(p_line < 0 || p_line >= (int)p_frame->lines.size(), p_h);

	Line &l = p_frame->lines[p_line];

	MutexLock lock(l.text_buf->get_mutex());

	l.indent = _find_margin(l.from, p_base_font, p_base_font_size) + l.prefix_width;
	l.offset.x = l.indent;
	l.text_buf->set_width(p_width - l.offset.x);

	PackedFloat32Array tab_stops = _find_tab_stops(l.from);
	if (!tab_stops.is_empty()) {
		l.text_buf->tab_align(tab_stops);
	} else if (tab_size > 0) { // Align inline tabs.
		Vector<float> tabs;
		tabs.push_back(tab_size * p_base_font->get_char_size(' ', p_base_font_size).width);
		l.text_buf->tab_align(tabs);
	}

	Item *it_to = (p_line + 1 < (int)p_frame->lines.size()) ? p_frame->lines[p_line + 1].from : nullptr;
	for (Item *it = l.from; it && it != it_to; it = _get_next_item(it)) {
		switch (it->type) {
			case ITEM_IMAGE: {
				ItemImage *img = static_cast<ItemImage *>(it);
				Size2 img_size = img->size;
				if (img->width_in_percent || img->height_in_percent) {
					img_size = _get_image_size(img->image, img->width_in_percent ? (p_width * img->rq_size.width / 100.f) : img->rq_size.width, img->height_in_percent ? (p_width * img->rq_size.height / 100.f) : img->rq_size.height, img->region);
					l.text_buf->resize_object(it->rid, img_size, img->inline_align);
				}
			} break;
			case ITEM_TABLE: {
				ItemTable *table = static_cast<ItemTable *>(it);
				int col_count = table->columns.size();

				for (int i = 0; i < col_count; i++) {
					table->columns[i].width = 0;
				}

				const int available_width = p_width - theme_cache.table_h_separation * (col_count - 1);
				int base_column_width = available_width / col_count;

				for (Item *E : table->subitems) {
					ERR_CONTINUE(E->type != ITEM_FRAME); // Children should all be frames.
					ItemFrame *frame = static_cast<ItemFrame *>(E);

					float prev_h = 0;
					for (int i = 0; i < (int)frame->lines.size(); i++) {
						MutexLock sub_lock(frame->lines[i].text_buf->get_mutex());
						int w = base_column_width - frame->padding.position.x - frame->padding.size.x;
						w = MAX(w, _find_margin(frame->lines[i].from, p_base_font, p_base_font_size) + 1);
						prev_h = _resize_line(frame, i, p_base_font, p_base_font_size, w, prev_h);
					}
				}

				_set_table_size(table, available_width);

				int row_idx = (table->align_to_row < 0) ? table->rows_baseline.size() - 1 : table->align_to_row;
				if (table->rows_baseline.size() != 0 && row_idx < (int)table->rows_baseline.size()) {
					l.text_buf->resize_object(it->rid, Size2(table->total_width, table->total_height), table->inline_align, Math::round(table->rows_baseline[row_idx]));
				} else {
					l.text_buf->resize_object(it->rid, Size2(table->total_width, table->total_height), table->inline_align);
				}
			} break;
			default:
				break;
		}
	}

	l.offset.y = p_h;
	return _calculate_line_vertical_offset(l);
}

float RichTextLabel3D::_shape_line(ItemFrame *p_frame, int p_line, const Ref<Font> &p_base_font, int p_base_font_size, int p_width, float p_h, int *r_char_offset) {
	ERR_FAIL_NULL_V(p_frame, p_h);
	ERR_FAIL_COND_V(p_line < 0 || p_line >= (int)p_frame->lines.size(), p_h);

	Line &l = p_frame->lines[p_line];

	MutexLock lock(l.text_buf->get_mutex());

	BitField<TextServer::LineBreakFlag> autowrap_flags = TextServer::BREAK_MANDATORY;
	switch (autowrap_mode) {
		case TextServer::AUTOWRAP_WORD_SMART:
			autowrap_flags = TextServer::BREAK_WORD_BOUND | TextServer::BREAK_ADAPTIVE | TextServer::BREAK_MANDATORY;
			break;
		case TextServer::AUTOWRAP_WORD:
			autowrap_flags = TextServer::BREAK_WORD_BOUND | TextServer::BREAK_MANDATORY;
			break;
		case TextServer::AUTOWRAP_ARBITRARY:
			autowrap_flags = TextServer::BREAK_GRAPHEME_BOUND | TextServer::BREAK_MANDATORY;
			break;
		case TextServer::AUTOWRAP_OFF:
			break;
	}
	autowrap_flags = autowrap_flags | autowrap_flags_trim;

	// Clear cache.
	l.dc_item = nullptr;
	l.text_buf->clear();
	l.text_buf->set_break_flags(autowrap_flags);
	l.text_buf->set_justification_flags(_find_jst_flags(l.from));
	l.char_offset = *r_char_offset;
	l.char_count = 0;

	// List prefix.
	Vector<int> list_index;
	Vector<int> list_count;
	Vector<ItemList *> list_items;
	_find_list(l.from, list_index, list_count, list_items);

	if (list_items.size() > 0) {
		if (list_index[0] == 1) {
			// List level start, shape all prefixes for this level and compute max. prefix width.
			list_items[0]->max_width = 0;
			int index = 0;
			for (int i = p_line; i < (int)p_frame->lines.size(); i++) {
				Line &list_l = p_frame->lines[i];
				if (_find_list_item(list_l.from) == list_items[0]) {
					index++;

					Ref<Font> font = _get_font_or_default();
					int font_size = font_size;

					ItemFont *font_it = _find_font(list_l.from);
					if (font_it) {
						if (font_it->font.is_valid()) {
							font = font_it->font;
						}
						if (font_it->font_size > 0) {
							font_size = font_it->font_size;
						}
					}
					ItemFontSize *font_size_it = _find_font_size(list_l.from);
					if (font_size_it && font_size_it->font_size > 0) {
						font_size = font_size_it->font_size;
					}

					list_index.write[0] = index;
					String prefix = _get_prefix(list_l.from, list_index, list_items);
					list_l.text_prefix.instantiate();
					list_l.text_prefix->set_direction(_find_direction(list_l.from));
					list_l.text_prefix->add_string(prefix, font, font_size);
					list_items.write[0]->max_width = MAX(list_items[0]->max_width, list_l.text_prefix->get_size().x);
				}
			}
		}
		l.prefix_width = list_items[0]->max_width;
	}

	// Add indent.
	l.indent = _find_margin(l.from, p_base_font, p_base_font_size) + l.prefix_width;
	l.offset.x = l.indent;
	l.text_buf->set_width(p_width - l.offset.x);
	l.text_buf->set_alignment(_find_alignment(l.from));
	l.text_buf->set_direction(_find_direction(l.from));

	PackedFloat32Array tab_stops = _find_tab_stops(l.from);
	if (!tab_stops.is_empty()) {
		l.text_buf->tab_align(tab_stops);
	} else if (tab_size > 0) { // Align inline tabs.
		Vector<float> tabs;
		tabs.push_back(tab_size * p_base_font->get_char_size(' ', p_base_font_size).width);
		l.text_buf->tab_align(tabs);
	}

	// Shape current paragraph.
	String txt;
	Item *it_to = (p_line + 1 < (int)p_frame->lines.size()) ? p_frame->lines[p_line + 1].from : nullptr;
	int remaining_characters = visible_characters - l.char_offset;
	for (Item *it = l.from; it && it != it_to; it = _get_next_item(it)) {
		switch (it->type) {
			case ITEM_DROPCAP: {
				// Add dropcap.
				ItemDropcap *dc = static_cast<ItemDropcap *>(it);
				l.text_buf->set_dropcap(dc->text, dc->font, dc->font_size, dc->dropcap_margins);
				l.dc_item = dc;
				l.dc_color = dc->color;
				l.dc_ol_size = dc->ol_size;
				l.dc_ol_color = dc->ol_color;
			} break;
			case ITEM_NEWLINE: {
				Ref<Font> font = p_base_font;
				int font_size = p_base_font_size;

				ItemFont *font_it = _find_font(it);
				if (font_it) {
					if (font_it->font.is_valid()) {
						font = font_it->font;
					}
					if (font_it->font_size > 0) {
						font_size = font_it->font_size;
					}
				}
				ItemFontSize *font_size_it = _find_font_size(it);
				if (font_size_it && font_size_it->font_size > 0) {
					font_size = font_size_it->font_size;
				}
				l.text_buf->add_string(String::chr(0x200B), font, font_size, String(), it->rid);
				txt += "\n";
				l.char_count++;
				remaining_characters--;
			} break;
			case ITEM_TEXT: {
				ItemText *t = static_cast<ItemText *>(it);
				Ref<Font> font = p_base_font;
				int font_size = p_base_font_size;

				ItemFont *font_it = _find_font(it);
				if (font_it) {
					if (font_it->font.is_valid()) {
						font = font_it->font;
					}
					if (font_it->font_size > 0) {
						font_size = font_it->font_size;
					}
				}
				ItemFontSize *font_size_it = _find_font_size(it);
				if (font_size_it && font_size_it->font_size > 0) {
					font_size = font_size_it->font_size;
				}
				String lang = _find_language(it);
				String tx = t->text;
				if (visible_chars_behavior == TextServer::VC_CHARS_BEFORE_SHAPING && visible_characters >= 0 && remaining_characters >= 0 && tx.length() > remaining_characters) {
					String first = tx.substr(0, remaining_characters);
					String second = tx.substr(remaining_characters, -1);
					l.text_buf->add_string(first, font, font_size, lang, it->rid);
					l.text_buf->add_string(second, font, font_size, lang, it->rid);
				}
				remaining_characters -= tx.length();

				l.text_buf->add_string(tx, font, font_size, lang, it->rid);
				txt += tx;
				l.char_count += tx.length();
			} break;
			case ITEM_IMAGE: {
				ItemImage *img = static_cast<ItemImage *>(it);
				Size2 img_size = img->size;
				if (img->width_in_percent || img->height_in_percent) {
					img_size = _get_image_size(img->image, img->width_in_percent ? (p_width * img->rq_size.width / 100.f) : img->rq_size.width, img->height_in_percent ? (p_width * img->rq_size.height / 100.f) : img->rq_size.height, img->region);
				}
				l.text_buf->add_object(it->rid, img_size, img->inline_align, 1);
				txt += String::chr(0xfffc);
				l.char_count++;
				remaining_characters--;
			} break;
			case ITEM_TABLE: {
				ItemTable *table = static_cast<ItemTable *>(it);
				int col_count = table->columns.size();
				int t_char_count = 0;
				// Set minimums to zero.
				for (int i = 0; i < col_count; i++) {
					table->columns[i].min_width = 0;
					table->columns[i].max_width = 0;
					table->columns[i].width = 0;
				}
				// Compute minimum width for each cell.
				const int available_width = p_width - theme_cache.table_h_separation * (col_count - 1);
				int base_column_width = available_width / col_count;
				int idx = 0;
				for (Item *E : table->subitems) {
					ERR_CONTINUE(E->type != ITEM_FRAME); // Children should all be frames.
					ItemFrame *frame = static_cast<ItemFrame *>(E);

					int column = idx % col_count;
					float prev_h = 0;
					for (int i = 0; i < (int)frame->lines.size(); i++) {
						MutexLock sub_lock(frame->lines[i].text_buf->get_mutex());

						int char_offset = l.char_offset + l.char_count;
						int w = _find_margin(frame->lines[i].from, p_base_font, p_base_font_size) + 1;
						prev_h = _shape_line(frame, i, p_base_font, p_base_font_size, w, prev_h, &char_offset);
						int cell_ch = (char_offset - (l.char_offset + l.char_count));
						l.char_count += cell_ch;
						t_char_count += cell_ch;
						remaining_characters -= cell_ch;
						
						int min_width_override = frame->min_size_over.x;
						int max_width_override = frame->max_size_over.x;
						
						// Calculate content-based widths
						float wrapped_content_width = frame->lines[i].indent + std::ceil(frame->lines[i].text_buf->get_size().x);
						float unwrapped_content_width = frame->lines[i].indent + std::ceil(frame->lines[i].text_buf->get_non_wrapped_size().x);
						
						// Update min_width (use wrapped size - minimum space needed when text can wrap)
						if (min_width_override > 0) {
							table->columns[column].min_width = MAX(table->columns[column].min_width, min_width_override);
						}
						table->columns[column].min_width = MAX(table->columns[column].min_width, wrapped_content_width);
						
						// Update max_width
						if (max_width_override > 0) {
							// Override constrains the maximum width - don't let it grow beyond override
							table->columns[column].max_width = max_width_override;
						} else {
							// No override - use natural unwrapped width
							table->columns[column].max_width = MAX(table->columns[column].max_width, unwrapped_content_width);
						}
						
						// Ensure min_width <= max_width
						table->columns[column].min_width = MIN(table->columns[column].min_width, table->columns[column].max_width);
					}
					idx++;
				}
				for (Item *E : table->subitems) {
					ERR_CONTINUE(E->type != ITEM_FRAME); // Children should all be frames.
					ItemFrame *frame = static_cast<ItemFrame *>(E);

					float prev_h = 0;
					for (int i = 0; i < (int)frame->lines.size(); i++) {
						int w = base_column_width - frame->padding.position.x - frame->padding.size.x;
						w = MAX(w, _find_margin(frame->lines[i].from, p_base_font, p_base_font_size) + 1);
						prev_h = _resize_line(frame, i, p_base_font, p_base_font_size, w, prev_h);
					}
				}
				_set_table_size(table, available_width);

				int row_idx = (table->align_to_row < 0) ? table->rows_baseline.size() - 1 : table->align_to_row;
				if (table->rows_baseline.size() != 0 && row_idx < (int)table->rows_baseline.size()) {
					l.text_buf->add_object(it->rid, Size2(table->total_width, table->total_height), table->inline_align, t_char_count, Math::round(table->rows_baseline[row_idx]));
				} else {
					l.text_buf->add_object(it->rid, Size2(table->total_width, table->total_height), table->inline_align, t_char_count);
				}
				txt += String::chr(0xfffc).repeat(t_char_count);
			} break;
			default:
				break;
		}
	}

	// Apply BiDi override.
	l.text_buf->set_bidi_override(TS->parse_structured_text(_find_stt(l.from), st_args, txt));

	*r_char_offset = l.char_offset + l.char_count;

	l.offset.y = p_h;
	return _calculate_line_vertical_offset(l);
}

void RichTextLabel3D::_set_table_size(ItemTable *p_table, int p_available_width) {
	int col_count = p_table->columns.size();

	// Compute available width and total ratio (for expanders).
	int total_ratio = 0;
	int remaining_width = p_available_width;
	p_table->total_width = theme_cache.table_h_separation;

	for (int i = 0; i < col_count; i++) {
		remaining_width -= p_table->columns[i].min_width;
		if (p_table->columns[i].max_width > p_table->columns[i].min_width) {
			p_table->columns[i].expand = true;
		}
		if (p_table->columns[i].expand) {
			total_ratio += p_table->columns[i].expand_ratio;
		}
	}

	// Assign actual widths.
	for (int i = 0; i < col_count; i++) {
		p_table->columns[i].width = p_table->columns[i].min_width;
		if (p_table->columns[i].expand && total_ratio > 0 && remaining_width > 0) {
			p_table->columns[i].width += p_table->columns[i].expand_ratio * remaining_width / total_ratio;
		}
		if (i != col_count - 1) {
			p_table->total_width += p_table->columns[i].width + theme_cache.table_h_separation;
		} else {
			p_table->total_width += p_table->columns[i].width;
		}
		p_table->columns[i].width_with_padding = p_table->columns[i].width;
	}

	// Resize to max_width if needed and distribute the remaining space.
	bool table_need_fit = true;
	while (table_need_fit) {
		table_need_fit = false;
		// Fit slim.
		for (int i = 0; i < col_count; i++) {
			if (!p_table->columns[i].expand || !p_table->columns[i].shrink) {
				continue;
			}
			int dif = p_table->columns[i].width - p_table->columns[i].max_width;
			if (dif > 0) {
				table_need_fit = true;
				p_table->columns[i].width = p_table->columns[i].max_width;
				p_table->total_width -= dif;
				total_ratio -= p_table->columns[i].expand_ratio;
				p_table->columns[i].width_with_padding = p_table->columns[i].width;
			}
		}
		// Grow.
		remaining_width = p_available_width - p_table->total_width;
		if (remaining_width > 0 && total_ratio > 0) {
			for (int i = 0; i < col_count; i++) {
				if (p_table->columns[i].expand) {
					int dif = p_table->columns[i].max_width - p_table->columns[i].width;
					if (dif > 0) {
						int slice = p_table->columns[i].expand_ratio * remaining_width / total_ratio;
						int incr = MIN(dif, slice);
						p_table->columns[i].width += incr;
						p_table->total_width += incr;
						p_table->columns[i].width_with_padding = p_table->columns[i].width;
					}
				}
			}
		}
	}

	// Update line width and get total height.
	int idx = 0;
	p_table->total_height = 0;
	p_table->rows.clear();
	p_table->rows_no_padding.clear();
	p_table->rows_baseline.clear();

	Vector2 offset = Vector2(theme_cache.table_h_separation * 0.5, theme_cache.table_v_separation * 0.5).floor();
	float row_height = 0.0;
	float row_top_padding = 0.0;
	float row_bottom_padding = 0.0;
	const List<Item *>::Element *prev = p_table->subitems.front();

	for (const List<Item *>::Element *E = prev; E; E = E->next()) {
		ERR_CONTINUE(E->get()->type != ITEM_FRAME); // Children should all be frames.
		ItemFrame *frame = static_cast<ItemFrame *>(E->get());

		int column = idx % col_count;

		offset.x += frame->padding.position.x;
		float yofs = 0.0;
		float prev_h = 0.0;
		float row_baseline = 0.0;
		for (int i = 0; i < (int)frame->lines.size(); i++) {
			MutexLock sub_lock(frame->lines[i].text_buf->get_mutex());

			frame->lines[i].text_buf->set_width(p_table->columns[column].width);
			p_table->columns[column].width = MAX(p_table->columns[column].width, std::ceil(frame->lines[i].text_buf->get_size().x));
			p_table->columns[column].width_with_padding = MAX(p_table->columns[column].width_with_padding, std::ceil(frame->lines[i].text_buf->get_size().x + frame->padding.position.x + frame->padding.size.x));

			frame->lines[i].offset.y = prev_h;

			float h = frame->lines[i].text_buf->get_size().y + (frame->lines[i].text_buf->get_line_count() - 1) * theme_cache.line_separation;
			if (i > 0) {
				h += theme_cache.paragraph_separation;
			}
			if (frame->min_size_over.y > 0) {
				h = MAX(h, frame->min_size_over.y);
			}
			if (frame->max_size_over.y > 0) {
				h = MIN(h, frame->max_size_over.y);
			}
			yofs += h;
			prev_h = frame->lines[i].offset.y + frame->lines[i].text_buf->get_size().y + (frame->lines[i].text_buf->get_line_count() - 1) * theme_cache.line_separation + theme_cache.paragraph_separation;

			frame->lines[i].offset += offset;
			row_baseline = MAX(row_baseline, frame->lines[i].text_buf->get_line_ascent(frame->lines[i].text_buf->get_line_count() - 1));
		}
		row_top_padding = MAX(row_top_padding, frame->padding.position.y);
		row_bottom_padding = MAX(row_bottom_padding, frame->padding.size.y);
		offset.x += p_table->columns[column].width + theme_cache.table_h_separation + frame->padding.size.x;

		row_height = MAX(yofs, row_height);
		// Add row height after last column of the row or last cell of the table.
		if (column == col_count - 1 || E->next() == nullptr) {
			offset.x = Math::floor(theme_cache.table_h_separation * 0.5);
			float row_contents_height = row_height;
			row_height += row_top_padding + row_bottom_padding;
			row_height += theme_cache.table_v_separation;
			p_table->total_height += row_height;
			offset.y += row_height;
			p_table->rows.push_back(row_height);
			p_table->rows_no_padding.push_back(row_contents_height);
			p_table->rows_baseline.push_back(p_table->total_height - row_height + row_baseline + Math::floor(theme_cache.table_v_separation * 0.5));
			for (const List<Item *>::Element *F = prev; F; F = F->next()) {
				ItemFrame *in_frame = static_cast<ItemFrame *>(F->get());
				for (int i = 0; i < (int)in_frame->lines.size(); i++) {
					in_frame->lines[i].offset.y += row_top_padding;
				}
				if (in_frame == frame) {
					break;
				}
			}
			row_height = 0.0;
			row_top_padding = 0.0;
			row_bottom_padding = 0.0;
			prev = E->next();
		}
		idx++;
	}

	// Recalculate total width.
	p_table->total_width = 0;
	for (int i = 0; i < col_count; i++) {
		p_table->total_width += p_table->columns[i].width_with_padding + theme_cache.table_h_separation;
	}
}

void RichTextLabel3D::set_line_spacing(float p_line_spacing) {
	if (line_spacing != p_line_spacing) {
		line_spacing = p_line_spacing;
		queue_redraw();
	}
}

float RichTextLabel3D::get_line_spacing() const {
	return line_spacing;
}

void RichTextLabel3D::set_draw_flag(DrawFlags p_flag, bool p_enable) {
	ERR_FAIL_INDEX(p_flag, FLAG_MAX);
	if (flags[p_flag] != p_enable) {
		flags[p_flag] = p_enable;
		queue_redraw();
	}
}

bool RichTextLabel3D::get_draw_flag(DrawFlags p_flag) const {
	ERR_FAIL_INDEX_V(p_flag, FLAG_MAX, false);
	return flags[p_flag];
}

void RichTextLabel3D::set_billboard_mode(StandardMaterial3D::BillboardMode p_mode) {
	ERR_FAIL_INDEX(p_mode, 3);
	if (billboard_mode != p_mode) {
		billboard_mode = p_mode;
		queue_redraw();
	}
}

void RichTextLabel3D::set_alpha_cut_mode(AlphaCutMode p_mode) {
	ERR_FAIL_INDEX(p_mode, ALPHA_CUT_MAX);
	if (alpha_cut != p_mode) {
		alpha_cut = p_mode;
		queue_redraw();
		notify_property_list_changed();
	}
}

void RichTextLabel3D::set_texture_filter(StandardMaterial3D::TextureFilter p_filter) {
	if (texture_filter != p_filter) {
		texture_filter = p_filter;
		queue_redraw();
	}
}

StandardMaterial3D::TextureFilter RichTextLabel3D::get_texture_filter() const {
	return texture_filter;
}

RichTextLabel3D::AlphaCutMode RichTextLabel3D::get_alpha_cut_mode() const {
	return alpha_cut;
}

void RichTextLabel3D::set_alpha_hash_scale(float p_hash_scale) {
	if (alpha_hash_scale != p_hash_scale) {
		alpha_hash_scale = p_hash_scale;
		queue_redraw();
	}
}

float RichTextLabel3D::get_alpha_hash_scale() const {
	return alpha_hash_scale;
}

void RichTextLabel3D::set_alpha_scissor_threshold(float p_threshold) {
	if (alpha_scissor_threshold != p_threshold) {
		alpha_scissor_threshold = p_threshold;
		queue_redraw();
	}
}

float RichTextLabel3D::get_alpha_scissor_threshold() const {
	return alpha_scissor_threshold;
}

void RichTextLabel3D::set_alpha_antialiasing(BaseMaterial3D::AlphaAntiAliasing p_alpha_aa) {
	if (alpha_antialiasing_mode != p_alpha_aa) {
		alpha_antialiasing_mode = p_alpha_aa;
		queue_redraw();
	}
}

BaseMaterial3D::AlphaAntiAliasing RichTextLabel3D::get_alpha_antialiasing() const {
	return alpha_antialiasing_mode;
}

void RichTextLabel3D::set_alpha_antialiasing_edge(float p_edge) {
	if (alpha_antialiasing_edge != p_edge) {
		alpha_antialiasing_edge = p_edge;
		queue_redraw();
	}
}

float RichTextLabel3D::get_alpha_antialiasing_edge() const {
	return alpha_antialiasing_edge;
}

StandardMaterial3D::BillboardMode RichTextLabel3D::get_billboard_mode() const {
	return billboard_mode;
}

void RichTextLabel3D::_generate_rich_glyph_surface( const Glyph &p_glyph, Vector2 &r_offset,  Vector3 &extra_offset, Vector3 &extra_rotation, const Color &p_color, int p_priority, int p_outline_size, int p_draw_step) {
    if (p_glyph.index == 0) {
        r_offset.x += p_glyph.advance * pixel_size * p_glyph.repeat; // Non visual character, skip.
        return;
    }
	// print_line("Generating glyph surface");
    Vector2 gl_of;
    Vector2 gl_sz;
    Rect2 gl_uv;
    Size2 texs;
    RID tex;

    // Get glyph texture information
    if (p_glyph.font_rid.is_valid()) {
        tex = TS->font_get_glyph_texture_rid(p_glyph.font_rid, Vector2i(p_glyph.font_size, p_outline_size), p_glyph.index);
        if (tex.is_valid()) {
            gl_of = (TS->font_get_glyph_offset(p_glyph.font_rid, Vector2i(p_glyph.font_size, p_outline_size), p_glyph.index) + Vector2(p_glyph.x_off, p_glyph.y_off)) * pixel_size;
            gl_sz = TS->font_get_glyph_size(p_glyph.font_rid, Vector2i(p_glyph.font_size, p_outline_size), p_glyph.index) * pixel_size;
            gl_uv = TS->font_get_glyph_uv_rect(p_glyph.font_rid, Vector2i(p_glyph.font_size, p_outline_size), p_glyph.index);
            texs = TS->font_get_glyph_texture_size(p_glyph.font_rid, Vector2i(p_glyph.font_size, p_outline_size), p_glyph.index);
        }
    } else if (((p_glyph.flags & TextServer::GRAPHEME_IS_VIRTUAL) != TextServer::GRAPHEME_IS_VIRTUAL) && ((p_glyph.flags & TextServer::GRAPHEME_IS_EMBEDDED_OBJECT) != TextServer::GRAPHEME_IS_EMBEDDED_OBJECT)) {
        gl_sz = TS->get_hex_code_box_size(p_glyph.font_size, p_glyph.index) * pixel_size;
        gl_of = Vector2(0, -gl_sz.y);
    }

    if (gl_uv.size.x <= 2 || gl_uv.size.y <= 2) {
        r_offset.x += p_glyph.advance * pixel_size * p_glyph.repeat; // Nothing to draw.
        return;
    }

    bool msdf = TS->font_is_multichannel_signed_distance_field(p_glyph.font_rid);

    // Apply shadow offset for shadow draw steps
    for (int j = 0; j < p_glyph.repeat; j++) {
        // Create surface key - use draw step in priority to separate different draw types
        int surface_priority = p_priority;
        SurfaceKey key = SurfaceKey(tex.get_id(), surface_priority, p_outline_size);
        
        // Create surface if it doesn't exist
        if (!surfaces.has(key)) {
            SurfaceData surf;
            surf.material = RenderingServer::get_singleton()->material_create();
            print_line("Creating material");
            // Set material defaults (similar to Label3D)
            RS::get_singleton()->material_set_param(surf.material, "albedo", Color(1, 1, 1, 1));
            RS::get_singleton()->material_set_param(surf.material, "specular", 0.5);
            RS::get_singleton()->material_set_param(surf.material, "metallic", 0.0);
            RS::get_singleton()->material_set_param(surf.material, "roughness", 1.0);
            RS::get_singleton()->material_set_param(surf.material, "uv1_offset", Vector3(0, 0, 0));
            RS::get_singleton()->material_set_param(surf.material, "uv1_scale", Vector3(1, 1, 1));
            RS::get_singleton()->material_set_param(surf.material, "uv2_offset", Vector3(0, 0, 0));
            RS::get_singleton()->material_set_param(surf.material, "uv2_scale", Vector3(1, 1, 1));
            RS::get_singleton()->material_set_param(surf.material, "alpha_scissor_threshold", alpha_scissor_threshold);
            RS::get_singleton()->material_set_param(surf.material, "alpha_hash_scale", alpha_hash_scale);
            RS::get_singleton()->material_set_param(surf.material, "alpha_antialiasing_edge", alpha_antialiasing_edge);
            
            if (msdf) {
                RS::get_singleton()->material_set_param(surf.material, "msdf_pixel_range", TS->font_get_msdf_pixel_range(p_glyph.font_rid));
                RS::get_singleton()->material_set_param(surf.material, "msdf_outline_size", p_outline_size);
            }

            // Set transparency mode
            BaseMaterial3D::Transparency mat_transparency = BaseMaterial3D::Transparency::TRANSPARENCY_ALPHA;
            if (get_alpha_cut_mode() == ALPHA_CUT_DISCARD) {
                mat_transparency = BaseMaterial3D::Transparency::TRANSPARENCY_ALPHA_SCISSOR;
            } else if (get_alpha_cut_mode() == ALPHA_CUT_OPAQUE_PREPASS) {
                mat_transparency = BaseMaterial3D::Transparency::TRANSPARENCY_ALPHA_DEPTH_PRE_PASS;
            } else if (get_alpha_cut_mode() == ALPHA_CUT_HASH) {
                mat_transparency = BaseMaterial3D::Transparency::TRANSPARENCY_ALPHA_HASH;
            }

            // Get appropriate shader
            RID shader_rid;
            StandardMaterial3D::get_material_for_2d(
                get_draw_flag(FLAG_SHADED), 
                mat_transparency, 
                get_draw_flag(FLAG_DOUBLE_SIDED), 
                get_billboard_mode() == StandardMaterial3D::BILLBOARD_ENABLED, 
                get_billboard_mode() == StandardMaterial3D::BILLBOARD_FIXED_Y, 
                msdf, 
                get_draw_flag(FLAG_DISABLE_DEPTH_TEST), 
                get_draw_flag(FLAG_FIXED_SIZE), 
                texture_filter, 
                alpha_antialiasing_mode,
                &shader_rid
            );

            RS::get_singleton()->material_set_shader(surf.material, shader_rid);
            RS::get_singleton()->material_set_param(surf.material, "texture_albedo", tex);
            RS::get_singleton()->material_set_param(surf.material, "albedo_texture_size", texs);
            
            if (get_alpha_cut_mode() == ALPHA_CUT_DISABLED) {
                RS::get_singleton()->material_set_render_priority(surf.material, surface_priority);
            } else {
				if (outline_size <= 0) {
					surf.z_shift = 1.0f * pixel_size;
				} else {
					surf.z_shift = surface_priority * pixel_size;
				}
            }

            surfaces[key] = surf;
        }

        SurfaceData &s = surfaces[key];

        // Resize arrays for new quad
        s.mesh_vertices.resize((s.offset + 1) * 4);
        s.mesh_normals.resize((s.offset + 1) * 4);
        s.mesh_tangents.resize((s.offset + 1) * 16);
        s.mesh_colors.resize((s.offset + 1) * 4);
        s.mesh_uvs.resize((s.offset + 1) * 4);


		if (extra_rotation.is_zero_approx()) {
			// No rotation - directly write vertices
			s.mesh_vertices.write[(s.offset * 4) + 3] = Vector3(r_offset.x + gl_of.x + extra_offset.x, r_offset.y - gl_of.y - gl_sz.y + extra_offset.y, s.z_shift + extra_offset.z);
			s.mesh_vertices.write[(s.offset * 4) + 2] = Vector3(r_offset.x + gl_of.x + gl_sz.x + extra_offset.x, r_offset.y - gl_of.y - gl_sz.y + extra_offset.y, s.z_shift + extra_offset.z);
			s.mesh_vertices.write[(s.offset * 4) + 1] = Vector3(r_offset.x + gl_of.x + gl_sz.x + extra_offset.x, r_offset.y - gl_of.y + extra_offset.y, s.z_shift + extra_offset.z);
			s.mesh_vertices.write[(s.offset * 4) + 0] = Vector3(r_offset.x + gl_of.x + extra_offset.x, r_offset.y - gl_of.y + extra_offset.y, s.z_shift + extra_offset.z);
		} else {
			// Convert rotation to radians
			Vector3 extra_rotation_rad = extra_rotation * Math::PI / 180.0;

			// Create quad vertices for rotation
			Vector3 vertices[4] = {
				Vector3(r_offset.x + gl_of.x + extra_offset.x, r_offset.y - gl_of.y + extra_offset.y, s.z_shift + extra_offset.z),
				Vector3(r_offset.x + gl_of.x + gl_sz.x + extra_offset.x, r_offset.y - gl_of.y + extra_offset.y, s.z_shift + extra_offset.z),
				Vector3(r_offset.x + gl_of.x + gl_sz.x + extra_offset.x, r_offset.y - gl_of.y - gl_sz.y + extra_offset.y, s.z_shift + extra_offset.z),
				Vector3(r_offset.x + gl_of.x + extra_offset.x, r_offset.y - gl_of.y - gl_sz.y + extra_offset.y, s.z_shift + extra_offset.z)
			};

			// Apply rotations in XYZ order
			for (int i = 0; i < 4; i++) {
				// Rotate around X axis
				float y = vertices[i].y;
				float z = vertices[i].z;
				vertices[i].y = y * Math::cos(extra_rotation_rad.x) - z * Math::sin(extra_rotation_rad.x);
				vertices[i].z = y * Math::sin(extra_rotation_rad.x) + z * Math::cos(extra_rotation_rad.x);

				// Rotate around Y axis
				float x = vertices[i].x;
				z = vertices[i].z;
				vertices[i].x = x * Math::cos(extra_rotation_rad.y) + z * Math::sin(extra_rotation_rad.y);
				vertices[i].z = -x * Math::sin(extra_rotation_rad.y) + z * Math::cos(extra_rotation_rad.y);

				// Rotate around Z axis
				x = vertices[i].x;
				y = vertices[i].y;
				vertices[i].x = x * Math::cos(extra_rotation_rad.z) - y * Math::sin(extra_rotation_rad.z);
				vertices[i].y = x * Math::sin(extra_rotation_rad.z) + y * Math::cos(extra_rotation_rad.z);
			}

			// Write rotated vertices to mesh
			s.mesh_vertices.write[(s.offset * 4) + 0] = vertices[0];
			s.mesh_vertices.write[(s.offset * 4) + 1] = vertices[1];
			s.mesh_vertices.write[(s.offset * 4) + 2] = vertices[2];
			s.mesh_vertices.write[(s.offset * 4) + 3] = vertices[3];
		}

		// Set normals and tangents for all 4 vertices
		for (int i = 0; i < 4; i++) {
			s.mesh_normals.write[(s.offset * 4) + i] = Vector3(0.0, 0.0, 1.0);
			s.mesh_tangents.write[(s.offset * 16) + (i * 4) + 0] = 1.0;
			s.mesh_tangents.write[(s.offset * 16) + (i * 4) + 1] = 0.0;
			s.mesh_tangents.write[(s.offset * 16) + (i * 4) + 2] = 0.0;
			s.mesh_tangents.write[(s.offset * 16) + (i * 4) + 3] = 1.0;
			s.mesh_colors.write[(s.offset * 4) + i] = p_color; // Use the rich text color
			s.mesh_uvs.write[(s.offset * 4) + i] = Vector2(); // Initialize UVs
		}

        // Set texture UVs if texture is valid
        if (tex.is_valid()) {
            s.mesh_uvs.write[(s.offset * 4) + 3] = Vector2(gl_uv.position.x / texs.x, (gl_uv.position.y + gl_uv.size.y) / texs.y);
            s.mesh_uvs.write[(s.offset * 4) + 2] = Vector2((gl_uv.position.x + gl_uv.size.x) / texs.x, (gl_uv.position.y + gl_uv.size.y) / texs.y);
            s.mesh_uvs.write[(s.offset * 4) + 1] = Vector2((gl_uv.position.x + gl_uv.size.x) / texs.x, gl_uv.position.y / texs.y);
            s.mesh_uvs.write[(s.offset * 4) + 0] = Vector2(gl_uv.position.x / texs.x, gl_uv.position.y / texs.y);
        }

        // Create triangle indices for the quad
        s.indices.resize((s.offset + 1) * 6);
        s.indices.write[(s.offset * 6) + 0] = (s.offset * 4) + 0;
        s.indices.write[(s.offset * 6) + 1] = (s.offset * 4) + 1;
        s.indices.write[(s.offset * 6) + 2] = (s.offset * 4) + 2;
        s.indices.write[(s.offset * 6) + 3] = (s.offset * 4) + 0;
        s.indices.write[(s.offset * 6) + 4] = (s.offset * 4) + 2;
        s.indices.write[(s.offset * 6) + 5] = (s.offset * 4) + 3;

        s.offset++;
        r_offset.x += p_glyph.advance * pixel_size;
    }
}

int RichTextLabel3D::_draw_line(ItemFrame *p_frame, int p_line, const Vector2 &p_ofs, const Vector3 &l_extra_offset, Vector3 &l_extra_rotation, int p_width, float p_vsep, bool inside_table, const Color &p_base_color, int p_outline_size, const Color &p_outline_color, const Color &p_font_shadow_color, int p_shadow_outline_size, const Point2 &p_shadow_ofs, int &r_processed_glyphs) {
	ERR_FAIL_NULL_V(p_frame, 0);
	ERR_FAIL_COND_V(p_line < 0 || p_line >= (int)p_frame->lines.size(), 0);

	Vector2 off;

	Line &l = p_frame->lines[p_line];
	MutexLock lock(l.text_buf->get_mutex());

	Item *it_from = l.from;

	if (it_from == nullptr) {
		return 0;
	}

	bool rtl = (l.text_buf->get_direction() == TextServer::DIRECTION_RTL);
	bool lrtl = false;

	bool trim_chars = (visible_characters >= 0) && (visible_chars_behavior == TextServer::VC_CHARS_AFTER_SHAPING || visible_chars_behavior == TextServer::VC_CHARS_BEFORE_SHAPING);
	bool trim_glyphs_ltr = (visible_characters >= 0) && ((visible_chars_behavior == TextServer::VC_GLYPHS_LTR) || ((visible_chars_behavior == TextServer::VC_GLYPHS_AUTO) && !lrtl));
	bool trim_glyphs_rtl = (visible_characters >= 0) && ((visible_chars_behavior == TextServer::VC_GLYPHS_RTL) || ((visible_chars_behavior == TextServer::VC_GLYPHS_AUTO) && lrtl));
	int total_glyphs = (trim_glyphs_ltr || trim_glyphs_rtl) ? get_total_glyph_count() : 0;
	int visible_glyphs = total_glyphs * visible_ratio;

	int line_count = 0;
	bool has_visible_chars = false;
	// Bottom margin for text clipping.
	float v_limit = 0.0f;//theme_cache.normal_style->get_margin(SIDE_BOTTOM);
	// Size2 ctrl_size = Size2(width, height);
	// Draw text.
	for (int line = 0; line < l.text_buf->get_line_count(); line++) {
		if (line > 0) {
			off.y -= p_vsep;
		}

		// if (p_ofs.y + off.y >= ctrl_size.height - v_limit) {
		// 	break;
		// }

		// double l_height = (l.text_buf->get_line_ascent(line) + l.text_buf->get_line_descent(line));
		// if (p_ofs.y + off.y + l_height <= 0) {
		// 	off.y += l_height;
		// 	continue;
		// }

		float width = l.text_buf->get_width();
		float length = l.text_buf->get_line_size(line).x;

		// Draw line.
		if (rtl) {
			// off.x = p_width - l.offset.x - width;
			off.x = l.offset.x - width;
			// if (!lrtl && p_frame == main) { // Skip Scrollbar.
			// 	// off.x -= scroll_w;
			// }
		} else {
			off.x = l.offset.x;
			// if (lrtl && p_frame == main) { // Skip Scrollbar.
			// 	// off.x += scroll_w;
			// }
		}

		// Draw text.
		switch (l.text_buf->get_alignment()) {
			case HORIZONTAL_ALIGNMENT_FILL:
			case HORIZONTAL_ALIGNMENT_LEFT: {
				if (rtl) {
					off.x += width - length;
				} else {
					off.x = 0.0;
				}
			} break;
			case HORIZONTAL_ALIGNMENT_CENTER: {
				if (inside_table) {
					off.x += Math::floor((width - length) / 2.0);
				} else {
					off.x += -length / 2.0;
				}
			} break;
			case HORIZONTAL_ALIGNMENT_RIGHT: {
				if (inside_table) {
					if (!rtl) {
						off.x += width - length;
					}
				} else {
					if (!rtl) {
						off.x = -length;
					} else {
						off.x = 0.0;
					}
				}
			} break;
		}

		bool skip_prefix = (trim_chars && l.char_offset > visible_characters) || (trim_glyphs_ltr && (r_processed_glyphs >= visible_glyphs)) || (trim_glyphs_rtl && (r_processed_glyphs < total_glyphs - visible_glyphs));
		if (l.text_prefix.is_valid() && line == 0 && !skip_prefix) {
			Color font_color = _find_color(l.from, p_base_color);
			int outline_size = _find_outline_size(l.from, p_outline_size);
			Color font_outline_color = _find_outline_color(l.from, p_outline_color);
			Color font_shadow_color = p_font_shadow_color * Color(1, 1, 1, font_color.a);
			if (rtl) {
				//TODO: DRAW?
				// l.text_prefix->draw(ci, p_ofs + Vector2(off.x + length, 0), font_color);
			} else {
				// l.text_prefix->draw(ci, p_ofs + Vector2(off.x - l.text_prefix->get_size().x, 0), font_color);
				//TODO: DRAW?
			}
		}

		RID rid = l.text_buf->get_line_rid(line);
		double l_ascent = TS->shaped_text_get_ascent(rid);
		Size2 l_size = TS->shaped_text_get_size(rid);
		double upos = TS->shaped_text_get_underline_position(rid);
		double uth = TS->shaped_text_get_underline_thickness(rid);

		print_line(vformat("Line %d. Ascent: %f Descent: %f current offset is %.2f", line, l_ascent, TS->shaped_text_get_descent(rid) * pixel_size, off.y));
		off.y -= l_ascent;

		const Glyph *glyphs = TS->shaped_text_get_glyphs(rid);
		int gl_size = TS->shaped_text_get_glyph_count(rid);
		Vector2i chr_range = TS->shaped_text_get_range(rid);

		if (aabb == AABB()) {
			aabb.position = Vector3(off.x * pixel_size, off.y * pixel_size, 0);
			aabb.expand_to(Vector3(off.x * pixel_size + l_size.x * pixel_size, off.y * pixel_size - (l_size.y + line_spacing) * pixel_size, 0));
		} else {
			aabb.expand_to(Vector3(off.x * pixel_size, off.y * pixel_size, 0));
			aabb.expand_to(Vector3(off.x + l_size.x, off.y - (l_size.y + line_spacing) * pixel_size, 0));
		}

		int processed_glyphs_step = 0;
		for (int step = DRAW_STEP_BACKGROUND; step < DRAW_STEP_MAX; step++) {
			if (step == DRAW_STEP_TEXT) {
				// Draw inlined objects.
				Array objects = TS->shaped_text_get_objects(rid);
				for (int i = 0; i < objects.size(); i++) {
					Item *it = items.get_or_null(objects[i]);
					if (it != nullptr) {
						Vector2i obj_range = TS->shaped_text_get_object_range(rid, objects[i]);
						if (trim_chars && l.char_offset + obj_range.y > visible_characters) {
							continue;
						}
						if (trim_glyphs_ltr || trim_glyphs_rtl) {
							int obj_glyph = r_processed_glyphs + TS->shaped_text_get_object_glyph(rid, objects[i]);
							if ((trim_glyphs_ltr && (obj_glyph >= visible_glyphs)) || (trim_glyphs_rtl && (obj_glyph < total_glyphs - visible_glyphs))) {
								continue;
							}
						}
						Rect2 objrect = TS->shaped_text_get_object_rect(rid, objects[i]);
						Rect2 rect = Rect2(Point2(objrect.position.x, objrect.position.y * -1), objrect.size);
						switch (it->type) {
							case ITEM_IMAGE: {
								ItemImage *img = static_cast<ItemImage *>(it);
								Vector2 img_pos = (p_ofs) + (rect.position) + (off);
								if (img->pad) {
									Size2 pad_size = rect.size.min(img->image->get_size());
									Vector2 pad_off = (rect.size - pad_size) / 2;
									Vector3 extra_offset = l_extra_offset + Vector3(0, 0, 0);
									Vector3 extra_rotation = l_extra_rotation + Vector3(0, 0, 0);
									_generate_image_surface(img->image, img_pos + pad_off, pad_size, extra_offset, extra_rotation, img->color, render_priority);
									visible_rect = _richtextlabel3d_merge_or_copy_rect(visible_rect, Rect2(p_ofs + rect.position + off, rect.size));
								} else {
									Vector3 extra_offset = l_extra_offset + Vector3(0, 0, 0);
									Vector3 extra_rotation = l_extra_rotation + Vector3(0, 0, 0);
									_generate_image_surface(img->image, img_pos, rect.size, extra_offset, extra_rotation, img->color, render_priority);
									visible_rect = _richtextlabel3d_merge_or_copy_rect(visible_rect, Rect2(p_ofs + rect.position + off, rect.size));
								}
							} break;
							case ITEM_TABLE: {
								ItemTable *table = static_cast<ItemTable *>(it);
								Color odd_row_bg = Color(0, 0, 0, 0);
								Color even_row_bg = Color(0, 0, 0, 0);
								Color border = Color(0, 0, 0, 0);
								float h_separation = 0.0f;
								float v_separation = 0.0f;

								int col_count = table->columns.size();
								int row_count = table->rows.size();

								int idx = 0;
								for (Item *E : table->subitems) {
									ItemFrame *frame = static_cast<ItemFrame *>(E);

									int col = idx % col_count;
									int row = idx / col_count;

									if (frame->lines.size() != 0 && row < row_count) {
										Vector2 coff = frame->lines[0].offset;
										coff.x -= frame->lines[0].indent;
										if (rtl) {
											coff.x = rect.size.width - table->columns[col].width - coff.x;
										}
										
										// Draw cell background
										if (frame->has_image_background) {
											ItemFrameImageBackground *img_bg = static_cast<ItemFrameImageBackground *>(frame);
											Color img_bg_color = row % 2 == 0 ? (frame->odd_row_bg != Color(0, 0, 0, 0) ? frame->odd_row_bg : odd_row_bg) : (frame->even_row_bg != Color(0, 0, 0, 0) ? frame->even_row_bg : even_row_bg);
											
											if (img_bg != nullptr && img_bg->image.is_valid()) {
												Rect2 img_rect = Rect2((p_ofs) + rect.position + (off) + coff - (frame->padding.position * Vector2(1, -1)) - Vector2(h_separation * 0.5, v_separation * 0.5).floor(), Size2(table->columns[col].width + h_separation + frame->padding.position.x + frame->padding.size.x, table->rows_no_padding[row] + frame->padding.position.y + frame->padding.size.y));
												
												if (frame->max_size_over.y > 0 && img_rect.size.y > frame->max_size_over.y) {
													img_rect.size.y = frame->max_size_over.y;
												}

												if (img_bg->keep_aspect_center) {
													img_rect.size.x = img_rect.size.y * img_bg->image->get_size().x / img_bg->image->get_size().y;
												}

												if (img_bg->has_rect_offset) {
													img_rect.position += img_bg->rect_offset.position;
													img_rect.size += img_bg->rect_offset.size;
												}
												Vector3 extra_offset = l_extra_offset + Vector3(0, 0, 0);
												Vector3 extra_rotation = l_extra_rotation + Vector3(0, 0, 0);
												_generate_image_surface(img_bg->image, img_rect.position, img_rect.size, extra_offset, extra_rotation, img_bg_color, outline_render_priority);
											}
										} else {
											// Draw solid color cell background
											Color bg_color;
											if (row % 2 == 0) {
												bg_color = frame->odd_row_bg != Color(0, 0, 0, 0) ? frame->odd_row_bg : odd_row_bg;
											} else {
												bg_color = frame->even_row_bg != Color(0, 0, 0, 0) ? frame->even_row_bg : even_row_bg;
											}
											if (bg_color.a > 0.0) {
												Rect2 cell_rect = Rect2((p_ofs) + rect.position + (off) + coff - (frame->padding.position * Vector2(1, -1)) - Vector2(h_separation * 0.5, v_separation * 0.5).floor(), Size2(table->columns[col].width + h_separation + frame->padding.position.x + frame->padding.size.x, table->rows_no_padding[row] + frame->padding.position.y + frame->padding.size.y));
												Vector3 extra_offset = l_extra_offset + frame->offset;
												Vector3 extra_rotation = l_extra_rotation + frame->rotation;
												_generate_rect_surface(cell_rect.position, cell_rect.size, extra_offset, extra_rotation, bg_color, outline_render_priority, frame->thickness, frame->radius.x, frame->radius.y, frame->bevel_segments);
											}
										}
										
										// Draw cell border
										Color bc = frame->border != Color(0, 0, 0, 0) ? frame->border : border;
										if (bc.a > 0.0) {
											Rect2 cell_rect = Rect2((p_ofs) + rect.position + (off) + coff - (frame->padding.position * Vector2(1, -1)) - Vector2(h_separation * 0.5, v_separation * 0.5).floor(), Size2(table->columns[col].width + h_separation + frame->padding.position.x + frame->padding.size.x, table->rows_no_padding[row] + frame->padding.position.y + frame->padding.size.y));
											Vector3 extra_offset = l_extra_offset + frame->offset;
											Vector3 extra_rotation = l_extra_rotation + frame->rotation;
											_generate_border_surface(cell_rect.position, cell_rect.size, extra_offset, extra_rotation, bc, outline_render_priority);
										}
									}
									
									// Calculate margins for cell content - use obj_pos as base
									Rect2 margins_rect = Rect2(rect.position, rect.size);
									if (frame->margins.position.x > 0 || frame->margins.position.y > 0 || frame->margins.size.x > 0 || frame->margins.size.y > 0) {
										margins_rect.position.x = frame->margins.position.x;
										margins_rect.position.y = frame->margins.position.y;
										margins_rect.size.x -= frame->margins.position.x + frame->margins.size.x;
										margins_rect.size.y -= frame->margins.position.y + frame->margins.size.y;
									}

									// Calculate vertical centering offset
									float vertical_center_offset = 0;
									
									if (frame->vertical_align_center) {
										// Calculate total text height using actual line offsets
										float total_text_height = 0;
										
										if (frame->lines.size() > 0) {
											// Get the last line's offset and add its actual height
											int last_line_idx = frame->lines.size() - 1;
											total_text_height = frame->lines[last_line_idx].offset.y;
											
											// Add the height of the last line itself
											// Use a single line's actual rendered height
											total_text_height += frame->lines[last_line_idx].text_buf->get_size().y;
										}
										
										float cell_content_height = table->rows_no_padding[row];
										
										vertical_center_offset = (cell_content_height - total_text_height) / 2.0;
									}
									
									
									// Draw cell content (text lines)
									for (int j = 0; j < (int)frame->lines.size(); j++) {
										Vector2 line_offset = Vector2(0, frame->lines[j].offset.y);
										if (frame->vertical_align_center) {
											line_offset.y -= vertical_center_offset;
										} else {
											line_offset.y -= frame->margins.position.y;
										}

										Vector3 child_extra_offset = l_extra_offset + frame->offset + Vector3(0.0f, 0.0f, cell_children_depth * pixel_size);
										Vector3 child_extra_rotation = l_extra_rotation + frame->rotation + Vector3(0.0f, 0.0f, 0.0f);
										_draw_line(frame, j, p_ofs + off + margins_rect.position + line_offset, child_extra_offset, child_extra_rotation, margins_rect.size.x, 0, true, p_base_color, p_outline_size, p_outline_color, p_font_shadow_color, p_shadow_outline_size, p_shadow_ofs, r_processed_glyphs);
									}

									idx++;
								}
							} break;
							default:
								break;
						}
					}
				}
			}


			Vector2 off_step = off * pixel_size;
			processed_glyphs_step = r_processed_glyphs;

			Vector2 ul_start;
			bool ul_started = false;
			Color ul_color_prev;
			Color ul_color;

			Vector2 dot_ul_start;
			bool dot_ul_started = false;
			Color dot_ul_color_prev;
			Color dot_ul_color;

			Vector2 st_start;
			bool st_started = false;
			Color st_color_prev;
			Color st_color;

			float box_start = 0.0;
			Color last_color = Color(0, 0, 0, 0);

			Item *it = it_from;
			int span = -1;
			for (int i = 0; i < gl_size; i++) {
				if (glyphs[i].span_index != span) {
					span = glyphs[i].span_index;
					if (span >= 0) {
						if ((glyphs[i].flags & TextServer::GRAPHEME_IS_EMBEDDED_OBJECT) == TextServer::GRAPHEME_IS_EMBEDDED_OBJECT) {
							Item *new_it = items.get_or_null(TS->shaped_get_span_embedded_object(rid, span));
							if (new_it) {
								it = new_it;
							}
						} else {
							Item *new_it = items.get_or_null(TS->shaped_get_span_meta(rid, span));
							if (new_it) {
								it = new_it;
							}
						}
					}
				}

				Color font_color = (step == DRAW_STEP_SHADOW_OUTLINE || step == DRAW_STEP_SHADOW || step == DRAW_STEP_OUTLINE || step == DRAW_STEP_TEXT) ? _find_color(it, p_base_color) : Color();
				int outline_size = (step == DRAW_STEP_OUTLINE) ? _find_outline_size(it, p_outline_size) : 0;
				Color font_outline_color = (step == DRAW_STEP_OUTLINE) ? _find_outline_color(it, p_outline_color) : Color();
				Color font_shadow_color = p_font_shadow_color;
				bool txt_visible = (font_color.a != 0);

				if (step == DRAW_STEP_OUTLINE && (outline_size <= 0 || font_outline_color.a == 0)) {
					processed_glyphs_step += glyphs[i].repeat;
					off_step.x += glyphs[i].advance * glyphs[i].repeat * pixel_size;
					continue;
				// } else if (step == DRAW_STEP_SHADOW_OUTLINE && (font_shadow_color.a == 0 || p_shadow_outline_size <= 0)) {
				// 	processed_glyphs_step += glyphs[i].repeat;
				// 	off_step.x += glyphs[i].advance * glyphs[i].repeat;
				// 	continue;
				// } else if (step == DRAW_STEP_SHADOW && (font_shadow_color.a == 0)) {
				// 	processed_glyphs_step += glyphs[i].repeat;
				// 	off_step.x += glyphs[i].advance * glyphs[i].repeat;
				// 	continue;
				} else if (step == DRAW_STEP_TEXT) {
					// UL/ lines etc.

				}

				if (step == DRAW_STEP_OUTLINE || step == DRAW_STEP_TEXT) {
					// Point2 fx_offset = Vector2(glyphs[i].x_off, glyphs[i].y_off);
					RID frid = glyphs[i].font_rid;
					uint32_t gl = glyphs[i].index;
					uint16_t gl_fl = glyphs[i].flags;
					uint8_t gl_cn = glyphs[i].count;
					bool cprev_cluster = false;
					bool cprev_conn = false;
					if (gl_cn == 0) { // Parts of the same grapheme cluster, always connected.
						cprev_cluster = true;
					}
					if (gl_fl & TextServer::GRAPHEME_IS_RTL) { // Check if previous grapheme cluster is connected.
						if (i > 0 && (glyphs[i - 1].flags & TextServer::GRAPHEME_IS_CONNECTED)) {
							cprev_conn = true;
						}
					} else {
						if (glyphs[i].flags & TextServer::GRAPHEME_IS_CONNECTED) {
							cprev_conn = true;
						}
					}

					Transform2D char_xform;
					char_xform.set_origin((p_ofs * pixel_size) + off_step);


					Vector2 char_off = char_xform.get_origin();
					Transform2D char_reverse_xform;
					if (step == DRAW_STEP_TEXT) {
						char_reverse_xform.set_origin(-char_off);
						Transform2D char_final_xform = char_xform * char_reverse_xform;
						// draw_set_transform_matrix(char_final_xform);
					} else if (step == DRAW_STEP_OUTLINE) {
						font_color = font_outline_color * Color(1, 1, 1, font_color.a);
						char_reverse_xform.set_origin(-char_off);
						Transform2D char_final_xform = char_xform * char_reverse_xform;
						// draw_set_transform_matrix(char_final_xform);
					}

					// Draw glyphs.
					for (int j = 0; j < glyphs[i].repeat; j++) {
						bool skip = (trim_chars && l.char_offset + glyphs[i].end > visible_characters) || (trim_glyphs_ltr && (processed_glyphs_step >= visible_glyphs)) || (trim_glyphs_rtl && (processed_glyphs_step < total_glyphs - visible_glyphs));
						if (!skip) {
							if (txt_visible) {
								has_visible_chars = true;
								visible_rect =_richtextlabel3d_merge_or_copy_rect(visible_rect, Rect2i(char_off - Vector2i(0, l_ascent * pixel_size), Point2i(glyphs[i].advance * pixel_size, l_size.y * pixel_size)));
								if (step == DRAW_STEP_TEXT) {
									if (frid != RID()) {
										 Glyph current_glyph = glyphs[i];
										current_glyph.font_rid = frid;
										Vector2 draw_offset = Vector2(char_off.x, char_off.y);
										Vector3 extra_offset = l_extra_offset + Vector3(0, 0, 0);
										Vector3 extra_rotation = l_extra_rotation + Vector3(0, 0, 0);
										_generate_rich_glyph_surface(current_glyph, draw_offset, extra_offset, extra_rotation, font_color, render_priority, 0, step);
										// TS->font_draw_glyph(frid, ci, glyphs[i].font_size, fx_offset + char_off, gl, font_color);
									} else if (((glyphs[i].flags & TextServer::GRAPHEME_IS_VIRTUAL) != TextServer::GRAPHEME_IS_VIRTUAL) && ((glyphs[i].flags & TextServer::GRAPHEME_IS_EMBEDDED_OBJECT) != TextServer::GRAPHEME_IS_EMBEDDED_OBJECT)) {
										// TS->draw_hex_code_box(ci, glyphs[i].font_size, fx_offset + char_off, gl, font_color);
										// TODO: DRAW?
									}
								} else if (step == DRAW_STEP_SHADOW_OUTLINE && frid != RID()) {
									// TS->font_draw_glyph_outline(frid, ci, glyphs[i].font_size, p_shadow_outline_size, fx_offset + char_off + p_shadow_ofs, gl, font_color);
								} else if (step == DRAW_STEP_SHADOW && frid != RID()) {
									// TS->font_draw_glyph(frid, ci, glyphs[i].font_size, fx_offset + char_off + p_shadow_ofs, gl, font_color);
								} else if (step == DRAW_STEP_OUTLINE && frid != RID()) {
									if (outline_size > 0 && font_outline_color.a > 0) {
										Glyph current_glyph = glyphs[i];
										current_glyph.font_rid = frid;
										Vector2 draw_offset = Vector2(char_off.x, char_off.y);
										Vector3 extra_offset = l_extra_offset + Vector3(0, 0, -outline_depth * pixel_size);
										Vector3 extra_rotation = l_extra_rotation + Vector3(0, 0, 0);
										_generate_rich_glyph_surface(current_glyph, draw_offset, extra_offset, extra_rotation, font_color, outline_render_priority, outline_size, step);
										// TS->font_draw_glyph_outline(frid, ci, glyphs[i].font_size, outline_size, fx_offset + char_off, gl, font_color);
									}
								}
							}
						}
						processed_glyphs_step++;
						off_step.x += glyphs[i].advance * pixel_size;
					}
					// draw_set_transform_matrix(Transform2D());
				}
			}
		}

		r_processed_glyphs = processed_glyphs_step;
		print_line(vformat("Ending draw Line %d. Descent: %f current offset is %.2f", line, TS->shaped_text_get_descent(rid) * pixel_size, off.y));
		off.y -= (TS->shaped_text_get_descent(rid) + line_spacing);
		if (has_visible_chars) {
			line_count++;
			has_visible_chars = false;
		}
	}

	return line_count;
}

void RichTextLabel3D::_generate_image_surface(const Ref<Texture2D> &p_texture, const Vector2 &p_position, const Size2 &p_size, Vector3 &extra_offset, Vector3 &extra_rotation, const Color &p_color, int p_priority) {
    if (p_texture.is_null() || p_size.x <= 0 || p_size.y <= 0) {
		print_line("Image surface is null or size is 0");
        return;
    }

    RID tex_rid = p_texture->get_rid();
    SurfaceKey key = SurfaceKey(tex_rid.get_id(), p_priority, 0);
    
    if (!surfaces.has(key)) {
        SurfaceData surf;
        surf.material = RenderingServer::get_singleton()->material_create();
        
        // Set up material for image rendering
		// Set defaults for material, names need to match up those in StandardMaterial3D.
		RS::get_singleton()->material_set_param(surf.material, "albedo", Color(1, 1, 1, 1));
		RS::get_singleton()->material_set_param(surf.material, "specular", 0.5);
		RS::get_singleton()->material_set_param(surf.material, "metallic", 0.0);
		RS::get_singleton()->material_set_param(surf.material, "roughness", 1.0);
		RS::get_singleton()->material_set_param(surf.material, "uv1_offset", Vector3(0, 0, 0));
		RS::get_singleton()->material_set_param(surf.material, "uv1_scale", Vector3(1, 1, 1));
		RS::get_singleton()->material_set_param(surf.material, "uv2_offset", Vector3(0, 0, 0));
		RS::get_singleton()->material_set_param(surf.material, "uv2_scale", Vector3(1, 1, 1));
		RS::get_singleton()->material_set_param(surf.material, "texture_albedo", tex_rid);
		RS::get_singleton()->material_set_param(surf.material, "albedo_texture_size", Vector2i(p_texture->get_width(), p_texture->get_height()));
        // Get appropriate shader for image rendering
        RID shader_rid;
        StandardMaterial3D::get_material_for_2d(
            get_draw_flag(FLAG_SHADED), 
            BaseMaterial3D::Transparency::TRANSPARENCY_ALPHA, 
            get_draw_flag(FLAG_DOUBLE_SIDED), 
            get_billboard_mode() == StandardMaterial3D::BILLBOARD_ENABLED, 
            get_billboard_mode() == StandardMaterial3D::BILLBOARD_FIXED_Y, 
            false, // not msdf
            get_draw_flag(FLAG_DISABLE_DEPTH_TEST), 
            get_draw_flag(FLAG_FIXED_SIZE), 
            texture_filter, 
            alpha_antialiasing_mode, 
            &shader_rid
        );
        
        RS::get_singleton()->material_set_shader(surf.material, shader_rid);
        RS::get_singleton()->material_set_render_priority(surf.material, p_priority);
        
        surf.offset = 0;
        surf.z_shift = 0;
        surfaces[key] = surf;
    }

    SurfaceData &s = surfaces[key];
    
    // Scale position and size by pixel_size
    Vector2 pos = p_position * pixel_size;
    Vector2 size = p_size * pixel_size;
    
    // Resize arrays
    s.mesh_vertices.resize((s.offset + 1) * 4);
    s.mesh_normals.resize((s.offset + 1) * 4);
    s.mesh_tangents.resize((s.offset + 1) * 16);
    s.mesh_colors.resize((s.offset + 1) * 4);
    s.mesh_uvs.resize((s.offset + 1) * 4);

    // Create quad vertices
    s.mesh_vertices.write[(s.offset * 4) + 3] = Vector3(pos.x, pos.y - size.y, s.z_shift);         // Bottom-left
    s.mesh_vertices.write[(s.offset * 4) + 2] = Vector3(pos.x + size.x, pos.y - size.y, s.z_shift); // Bottom-right
    s.mesh_vertices.write[(s.offset * 4) + 1] = Vector3(pos.x + size.x, pos.y, s.z_shift);         // Top-right
    s.mesh_vertices.write[(s.offset * 4) + 0] = Vector3(pos.x, pos.y, s.z_shift);                 // Top-left

    // Set normals, tangents, colors, and UVs
    for (int i = 0; i < 4; i++) {
        s.mesh_normals.write[(s.offset * 4) + i] = Vector3(0.0, 0.0, 1.0);
        s.mesh_tangents.write[(s.offset * 16) + (i * 4) + 0] = 1.0;
        s.mesh_tangents.write[(s.offset * 16) + (i * 4) + 1] = 0.0;
        s.mesh_tangents.write[(s.offset * 16) + (i * 4) + 2] = 0.0;
        s.mesh_tangents.write[(s.offset * 16) + (i * 4) + 3] = 1.0;
        s.mesh_colors.write[(s.offset * 4) + i] = p_color;
    }

    // Set UVs for full texture
    s.mesh_uvs.write[(s.offset * 4) + 3] = Vector2(0, 1); // Bottom-left
    s.mesh_uvs.write[(s.offset * 4) + 2] = Vector2(1, 1); // Bottom-right
    s.mesh_uvs.write[(s.offset * 4) + 1] = Vector2(1, 0); // Top-right
    s.mesh_uvs.write[(s.offset * 4) + 0] = Vector2(0, 0); // Top-left

    // Create triangle indices
    s.indices.resize((s.offset + 1) * 6);
    s.indices.write[(s.offset * 6) + 0] = (s.offset * 4) + 0;
    s.indices.write[(s.offset * 6) + 1] = (s.offset * 4) + 1;
    s.indices.write[(s.offset * 6) + 2] = (s.offset * 4) + 2;
    s.indices.write[(s.offset * 6) + 3] = (s.offset * 4) + 0;
    s.indices.write[(s.offset * 6) + 4] = (s.offset * 4) + 2;
    s.indices.write[(s.offset * 6) + 5] = (s.offset * 4) + 3;

    s.offset++;
}

void RichTextLabel3D::_generate_rect_surface(const Vector2 &p_position, const Size2 &p_size, Vector3 &extra_offset, Vector3 &extra_rotation, const Color &p_color, int p_priority, float thickness, float corner_radius, int corner_segments, int bevel_segments) {
    if (p_size.x <= 0 || p_size.y <= 0 || p_color.a <= 0) {
        return;
    }

    // Create a unique key for solid color rectangles (using 0 as texture ID)
    SurfaceKey key = SurfaceKey(0, p_priority, 0);
    
    if (!surfaces.has(key)) {
        SurfaceData surf;
        surf.material = RenderingServer::get_singleton()->material_create();
        
        // Set up material for solid color rendering (no texture)
        RS::get_singleton()->material_set_param(surf.material, "albedo", Color(1, 1, 1, 1));
        
        // Get appropriate shader for solid color rendering
        RID shader_rid;
        StandardMaterial3D::get_material_for_2d(
            get_draw_flag(FLAG_SHADED), 
            BaseMaterial3D::Transparency::TRANSPARENCY_DISABLED,
            !get_draw_flag(FLAG_DOUBLE_SIDED), 
            get_billboard_mode() == StandardMaterial3D::BILLBOARD_ENABLED, 
            get_billboard_mode() == StandardMaterial3D::BILLBOARD_FIXED_Y, 
            false, // not msdf
            get_draw_flag(FLAG_DISABLE_DEPTH_TEST), 
            get_draw_flag(FLAG_FIXED_SIZE), 
            texture_filter, 
            BaseMaterial3D::AlphaAntiAliasing::ALPHA_ANTIALIASING_OFF, 
            &shader_rid
        );
        
        RS::get_singleton()->material_set_shader(surf.material, shader_rid);
        RS::get_singleton()->material_set_render_priority(surf.material, p_priority);
        
        surf.offset = 0;
        surf.z_shift = 0;
        surfaces[key] = surf;
    }

    SurfaceData &s = surfaces[key];
    
    // Scale position and size by pixel_size
    Vector2 pos = p_position * pixel_size;
    Vector2 size = p_size * pixel_size;
    
    // Transform extra_offset by rotation if rotation is applied
    Vector3 transformed_offset = extra_offset;
    if (!extra_rotation.is_zero_approx()) {
        Vector3 extra_rotation_rad = extra_rotation * Math::PI / 180.0;
        
        // Create rotation matrix and apply to offset
        Basis rotation_basis;
        rotation_basis = rotation_basis.rotated(Vector3(1, 0, 0), extra_rotation_rad.x);
        rotation_basis = rotation_basis.rotated(Vector3(0, 1, 0), extra_rotation_rad.y);
        rotation_basis = rotation_basis.rotated(Vector3(0, 0, 1), extra_rotation_rad.z);
        
        transformed_offset = rotation_basis.xform(extra_offset);
    }
    
    // Clamp corner radius to maximum possible
    float _radius = MIN(corner_radius, MIN(size.x, size.y) * 0.5f);
    
    if (corner_segments <= 0) {
        _radius = 0.0f;
    }

	Vector<Vector3> _verts;
	Vector<Vector3> _normals;
	Vector<Color> _colors;
	Vector<Vector2> _uvs;
	Vector<int> _indices;

	// # Calculate corner radius, ensuring it doesn't exceed the smallest dimension

	// var r := minf(corner_radius, minf(extents_with_margin.x, extents_with_margin.y))
	// var inner_extents := Vector2(maxf(0, extents_with_margin.x - r), maxf(0, extents_with_margin.y - r))
	Vector2 inner_extents = Vector2(MAX(0, size.x - _radius), MAX(0, size.y - _radius)) * 0.55;

	// # Calculate vertex and triangle counts for different parts of the mesh
	int border_vert_count = 0;
	int border_tri_count = 0;
	int side_tri_count = 0;
	int face_vert_count = 0;

	// # Calculate counts based on whether we have rounded corners
	if (_radius > 0) {
		// Each corner needs corner_segments + 1 vertices
		border_vert_count = (corner_segments + 1) * 4;
		face_vert_count = 4 + border_vert_count;
		border_tri_count = (corner_segments + 2) * 4;
	}
	else {
		border_vert_count = 4;
		face_vert_count = 4;
	}

	int face_tri_count = 2 + border_tri_count;
	int total_vert_count = face_vert_count * 2;
	int total_tri_count = face_tri_count * 2;

	if (thickness > 0) {
		total_vert_count *= 2;
		side_tri_count = 8;
		if (_radius > 0) {
			side_tri_count += (corner_segments * 2) * 4;
		}
		total_tri_count += side_tri_count;
	}

	int total_index_count = total_tri_count * 3;

	// Initialize arrays if needed
	if (_verts.is_empty()) {
		_verts.resize(total_vert_count);
		_uvs.resize(total_vert_count);
		_normals.resize(total_vert_count);
		_colors.resize(total_vert_count);
		_indices.resize(total_index_count);
	}
	
	// Calculate front face Z position based on thickness grow direction
	float front_z = 0;

	// Create front face inner quad vertices
	_verts.write[0] = Vector3((pos.x + size.x * 0.5) + -inner_extents.x, (pos.y - size.y * 0.5) + inner_extents.y,  s.z_shift + front_z);
	_verts.write[1] = Vector3((pos.x + size.x * 0.5) + inner_extents.x, (pos.y - size.y * 0.5) + inner_extents.y, s.z_shift + front_z);
	_verts.write[2] = Vector3((pos.x + size.x * 0.5) + inner_extents.x, (pos.y - size.y * 0.5) + -inner_extents.y, s.z_shift + front_z);
	_verts.write[3] = Vector3((pos.x + size.x * 0.5) + -inner_extents.x, (pos.y - size.y * 0.5) + -inner_extents.y, s.z_shift + front_z);

	// Create front face inner quad triangles
	_indices.write[0] = 0;
	_indices.write[1] = 1;
	_indices.write[2] = 2;
	_indices.write[3] = 2;
	_indices.write[4] = 3;
	_indices.write[5] = 0;

	
	// Set face normals
	for (int i = 0; i < face_vert_count; i++) {
		_normals.write[i] = Vector3(0, 0, -1);
		_normals.write[face_vert_count + i] = Vector3(0, 0, 1);
	}


	// Create rounded corners if needed
	if (_radius > 0) {
		float q_inc = -Math::PI / 2 / corner_segments;
		for (int corner_idx = 0; corner_idx < 4; corner_idx++) {
			int start_index = 4 + corner_idx * (corner_segments + 1);
			int start_tri_index = 2 + corner_idx * (corner_segments + 2);
			int start_index_index = start_tri_index * 3;
			float start_q = Math::PI - corner_idx * (Math::PI / 2);
			int side_start_index = face_vert_count * 2 + start_index;
			Vector3 o = _verts[corner_idx];

			// Create vertices and triangles for each corner segment
			for (int i = 0; i <= corner_segments; i++) {
				float q = start_q + i * q_inc;
				Vector3 dir = Vector3(Math::cos(q), Math::sin(q), 0);
				int seg_start_index_index = start_index_index + i * 3;
				int index = start_index + i;
				_verts.write[index] = o + dir * _radius;

				// Set side normals if thickness is used
				if (thickness > 0) {
					_normals.write[side_start_index + i] = dir;
					_normals.write[side_start_index + face_vert_count + i] = dir;
				}

				// Create triangles for the corner
				if (i < corner_segments) {
					_indices.write[seg_start_index_index + 0] = index;
					_indices.write[seg_start_index_index + 1] = index + 1;
					_indices.write[seg_start_index_index + 2] = corner_idx;
				} else {
					int p0 = index;
					int p1 = index + 1;
					int p2 = corner_idx + 1;
					int p3 = corner_idx;
					if (corner_idx == 3) {
						p1 = 4;
						p2 = 0;
					}
					_indices.write[seg_start_index_index + 0] = p0;
					_indices.write[seg_start_index_index + 1] = p1;
					_indices.write[seg_start_index_index + 2] = p2;
					_indices.write[seg_start_index_index + 3] = p2;
					_indices.write[seg_start_index_index + 4] = p3;
					_indices.write[seg_start_index_index + 5] = p0;
				}
			}
		}
	}

	// Create back face by duplicating and offsetting front face vertices
	int face_index_count = face_tri_count * 3;
	for (int i = 0; i < face_vert_count; i++) {
		Vector3 front_p = _verts[i];
		Vector3 back_p = Vector3(front_p.x, front_p.y, front_p.z - thickness);
		_verts.write[face_vert_count + i] = back_p;
		_normals.write[face_vert_count + i] = Vector3(0, 0, 1);
	}

	// Create back face triangles by reversing front face triangles
	for (int i = 0; i < face_index_count; i++) {
		_indices.write[face_index_count + i] = _indices.get(face_index_count - 1 - i) + face_vert_count;
	}

	// Create side faces if thickness is used
	if (thickness > 0) {
		// Duplicate vertices for sides
		for (int i = 0; i < face_vert_count; i++) {
			Vector3 front_p = _verts[i];
			Vector3 back_p = _verts[face_vert_count + i];
			_verts.write[face_vert_count * 2 + i] = front_p;
			_verts.write[face_vert_count * 3 + i] = back_p;
		}
		// Create side triangles
		for (int i = 0; i < border_vert_count; i++) {
			int side_front_start_index = face_vert_count * 2;
			int side_back_start_index = face_vert_count * 3;
			if (_radius > 0) {
				side_front_start_index += 4;
				side_back_start_index += 4;
			}

			int i_next = (i + 1) % border_vert_count;
			int front_index0 = side_front_start_index + i;
			int back_index0 = side_back_start_index + i;
			int front_index1 = side_front_start_index + i_next;
			int back_index1 = side_back_start_index + i_next;
			int side_start_index_index = face_index_count * 2 + i * 6;

			// Create two triangles for each side segment
			_indices.write[side_start_index_index + 0] = back_index0;
			_indices.write[side_start_index_index + 1] = back_index1;
			_indices.write[side_start_index_index + 2] = front_index1;
			_indices.write[side_start_index_index + 3] = front_index1;
			_indices.write[side_start_index_index + 4] = front_index0;
			_indices.write[side_start_index_index + 5] = back_index0;
		}
	}

	// Set vertex colors (black for corners, white for rest)
	for (int i = 0; i < total_vert_count; i++) {
		_colors.write[i] = p_color.darkened(0.5);
	}
	for (int i = 0; i < 4; i++) {
		_colors.write[i] = p_color;
		_colors.write[face_vert_count + i] = p_color;
	}

    // Set UVs
    for (int i = 0; i < total_vert_count; i++) {
        _uvs.write[i] = Vector2(0, 0);
    }
    
    // Apply rotation and offset to all vertices
    if (!extra_rotation.is_zero_approx()) {
        Vector3 extra_rotation_rad = extra_rotation * Math::PI / 180.0;
        
        for (int i = 0; i < total_vert_count; i++) {
            Vector3 &vertex = _verts.write[i];
            
            // Rotate around X axis
            float y = vertex.y;
            float z = vertex.z;
            vertex.y = y * Math::cos(extra_rotation_rad.x) - z * Math::sin(extra_rotation_rad.x);
            vertex.z = y * Math::sin(extra_rotation_rad.x) + z * Math::cos(extra_rotation_rad.x);
            
            // Rotate around Y axis
            float x = vertex.x;
            z = vertex.z;
            vertex.x = x * Math::cos(extra_rotation_rad.y) + z * Math::sin(extra_rotation_rad.y);
            vertex.z = -x * Math::sin(extra_rotation_rad.y) + z * Math::cos(extra_rotation_rad.y);
            
            // Rotate around Z axis
            x = vertex.x;
            y = vertex.y;
            vertex.x = x * Math::cos(extra_rotation_rad.z) - y * Math::sin(extra_rotation_rad.z);
            vertex.y = x * Math::sin(extra_rotation_rad.z) + y * Math::cos(extra_rotation_rad.z);
        }
    }
    
    // Apply transformed offset to all vertices
    for (int i = 0; i < total_vert_count; i++) {
        _verts.write[i] += transformed_offset;
    }
    
	int start_offset = s.offset;
    s.mesh_vertices.resize((s.offset + total_vert_count));
    s.mesh_normals.resize((s.offset + total_vert_count));
    s.mesh_tangents.resize((s.offset + total_vert_count) * 4);
    s.mesh_colors.resize((s.offset + total_vert_count));
    s.mesh_uvs.resize((s.offset + total_vert_count));

    // Copy to surface arrays
    for (int i = 0; i < total_vert_count; i++) {
        s.mesh_vertices.write[start_offset + i] = _verts[i];
        s.mesh_normals.write[start_offset + i] = _normals[i];
        s.mesh_colors.write[start_offset + i] = _colors[i];
        s.mesh_uvs.write[start_offset + i] = _uvs[i];
        
        // Set tangents
        s.mesh_tangents.write[(start_offset + i) * 4 + 0] = 1.0;
        s.mesh_tangents.write[(start_offset + i) * 4 + 1] = 0.0;
        s.mesh_tangents.write[(start_offset + i) * 4 + 2] = 0.0;
        s.mesh_tangents.write[(start_offset + i) * 4 + 3] = 1.0;
    }	
    
    // Copy indices to surface arrays
    int current_index_count = s.indices.size();
    s.indices.resize(current_index_count + total_tri_count * 3);
    
    for (int i = 0; i < total_tri_count * 3; i++) {
        s.indices.write[current_index_count + i] = _indices[i] + start_offset;
    }
    
    s.offset += total_vert_count;	
}


void RichTextLabel3D::_generate_border_surface(const Vector2 &p_position, const Size2 &p_size, Vector3 &extra_offset, Vector3 &extra_rotation, const Color &p_color, int p_priority) {
    if (p_size.x <= 0 || p_size.y <= 0 || p_color.a <= 0) {
        return;
    }

    // Create border as 4 separate rectangles (top, right, bottom, left)
    real_t border_width = 2.0f; 
    
    // Top border
    _generate_rect_surface(p_position, Size2(p_size.x, border_width), extra_offset, extra_rotation, p_color, p_priority);
    
    // Right border
    _generate_rect_surface(Vector2(p_position.x + p_size.x - border_width, p_position.y), Size2(border_width, p_size.y), extra_offset, extra_rotation, p_color, p_priority);
    
    // Bottom border
	_generate_rect_surface(Vector2(p_position.x, p_position.y - p_size.y + border_width), Size2(p_size.x, border_width), extra_offset, extra_rotation, p_color, p_priority);
    
    // Left border
    _generate_rect_surface(Vector2(p_position.x, p_position.y - border_width), Size2(border_width, p_size.y), extra_offset, extra_rotation, p_color, p_priority);
}


// void RichTextLabel3D::_find_click(ItemFrame *p_frame, const Point2i &p_click, ItemFrame **r_click_frame, int *r_click_line, Item **r_click_item, int *r_click_char, bool *r_outside, bool p_meta) {
// 	if (r_click_item) {
// 		*r_click_item = nullptr;
// 	}
// 	if (r_click_char != nullptr) {
// 		*r_click_char = 0;
// 	}
// 	if (r_outside != nullptr) {
// 		*r_outside = true;
// 	}

// 	Size2 size = get_size();
// 	Rect2 text_rect = _get_text_rect();

// 	int vofs = vscroll->get_value();

// 	// Search for the first line.
// 	int to_line = main->first_invalid_line.load();
// 	int from_line = _find_first_line(0, to_line, vofs);

// 	int total_height = INT32_MAX;
// 	if (to_line && vertical_alignment != VERTICAL_ALIGNMENT_TOP) {
// 		MutexLock lock(main->lines[to_line - 1].text_buf->get_mutex());
// 		if (theme_cache.line_separation < 0) {
// 			// Do not apply to the last line to avoid cutting text.
// 			total_height = main->lines[to_line - 1].offset.y + main->lines[to_line - 1].text_buf->get_size().y + (main->lines[to_line - 1].text_buf->get_line_count() - 1) * theme_cache.line_separation;
// 		} else {
// 			total_height = main->lines[to_line - 1].offset.y + main->lines[to_line - 1].text_buf->get_size().y + (main->lines[to_line - 1].text_buf->get_line_count() - 1) * theme_cache.line_separation + theme_cache.paragraph_separation;
// 		}
// 	}
// 	float vbegin = 0, vsep = 0;
// 	if (text_rect.size.y > total_height) {
// 		switch (vertical_alignment) {
// 			case VERTICAL_ALIGNMENT_TOP: {
// 				// Nothing.
// 			} break;
// 			case VERTICAL_ALIGNMENT_CENTER: {
// 				vbegin = (text_rect.size.y - total_height) / 2;
// 			} break;
// 			case VERTICAL_ALIGNMENT_BOTTOM: {
// 				vbegin = text_rect.size.y - total_height;
// 			} break;
// 			case VERTICAL_ALIGNMENT_FILL: {
// 				int lines = 0;
// 				for (int l = from_line; l < to_line; l++) {
// 					MutexLock lock(main->lines[l].text_buf->get_mutex());
// 					lines += main->lines[l].text_buf->get_line_count();
// 				}
// 				if (lines > 1) {
// 					vsep = (text_rect.size.y - total_height) / (lines - 1);
// 				}
// 			} break;
// 		}
// 	}

// 	Point2 ofs = text_rect.get_position() + Vector2(0, vbegin + main->lines[from_line].offset.y - vofs);
// 	while (ofs.y < size.height && from_line < to_line) {
// 		MutexLock lock(main->lines[from_line].text_buf->get_mutex());
// 		_find_click_in_line(p_frame, from_line, ofs, text_rect.size.x, vsep, p_click, r_click_frame, r_click_line, r_click_item, r_click_char, false, p_meta);
// 		ofs.y += main->lines[from_line].text_buf->get_size().y + (main->lines[from_line].text_buf->get_line_count() - 1) * (theme_cache.line_separation + vsep) + (theme_cache.paragraph_separation + vsep);
// 		if (((r_click_item != nullptr) && ((*r_click_item) != nullptr)) || ((r_click_frame != nullptr) && ((*r_click_frame) != nullptr))) {
// 			if (r_outside != nullptr) {
// 				*r_outside = false;
// 			}
// 			return;
// 		}
// 		from_line++;
// 	}
// }

int RichTextLabel3D::_find_first_line(int p_from, int p_to, int p_vofs) const {
	int l = p_from;
	int r = p_to;
	while (l < r) {
		int m = Math::floor(double(l + r) / 2.0);
		MutexLock lock(main->lines[m].text_buf->get_mutex());
		int ofs = _calculate_line_vertical_offset(main->lines[m]);
		if (ofs < p_vofs) {
			l = m + 1;
		} else {
			r = m;
		}
	}
	return MIN(l, (int)main->lines.size() - 1);
}

_FORCE_INLINE_ float RichTextLabel3D::_calculate_line_vertical_offset(const RichTextLabel3D::Line &line) const {
	return line.get_height(theme_cache.line_separation, theme_cache.paragraph_separation);
}

// RID RichTextLabel3D::get_focused_accessibility_element() const {
// 	if (keyboard_focus_frame && keyboard_focus_item) {
// 		if (keyboard_focus_on_text) {
// 			return keyboard_focus_frame->lines[keyboard_focus_line].accessibility_text_element;
// 		} else {
// 			if (keyboard_focus_item->accessibility_item_element.is_valid()) {
// 				return keyboard_focus_item->accessibility_item_element;
// 			}
// 		}
// 	} else {
// 		if (!main->lines.is_empty()) {
// 			return main->lines[0].accessibility_text_element;
// 		}
// 	}
// 	return get_accessibility_element();
// }

void RichTextLabel3D::_notification(int p_what) {
	switch (p_what) {

		// case NOTIFICATION_MOUSE_EXIT: {
		// 	if (meta_hovering) {
		// 		meta_hovering = nullptr;
		// 		emit_signal(SNAME("meta_hover_ended"), current_meta);
		// 		current_meta = false;
		// 		queue_redraw();
		// 	}
		// } break;

		// case NOTIFICATION_RESIZED: {
		// 	_stop_thread();
		// 	main->first_resized_line.store(0); // Invalidate all lines.
		// 	_invalidate_accessibility();
		// 	queue_accessibility_update();
		// 	queue_redraw();
		// } break;

		case NOTIFICATION_ENTER_TREE: {
			_stop_thread();
			if (!text.is_empty()) {
				set_text(text);
			}

			main->first_invalid_line.store(0); // Invalidate all lines.
			queue_redraw();
		} break;

		case NOTIFICATION_PREDELETE:
		case NOTIFICATION_EXIT_TREE: {
			_stop_thread();
		} break;

		//case NOTIFICATION_LAYOUT_DIRECTION_CHANGED:
		case NOTIFICATION_TRANSLATION_CHANGED: {
			if (!stack_externally_modified) {
				_apply_translation();
			}

			queue_redraw();
		} break;

		case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
			if (is_visible_in_tree()) {
				queue_redraw();
			}
		} break;

		// case NOTIFICATION_DRAW: {
		// 	RID ci = get_canvas_item();
		// 	Size2 size = get_size();

		// 	draw_style_box(theme_cache.normal_style, Rect2(Point2(), size));

		// 	// if (has_focus()) {
		// 	// 	RenderingServer::get_singleton()->canvas_item_add_clip_ignore(ci, true);
		// 	// 	draw_style_box(theme_cache.focus_style, Rect2(Point2(), size));
		// 	// 	RenderingServer::get_singleton()->canvas_item_add_clip_ignore(ci, false);
		// 	// }

		// 	// Start text shaping.
		// 	if (_validate_line_caches()) {
		// 		set_physics_process_internal(false); // Disable auto refresh, if text is fully processed.
		// 	} else {
		// 		// Draw loading progress bar.
		// 		if ((progress_delay > 0) && (OS::get_singleton()->get_ticks_msec() - loading_started >= (uint64_t)progress_delay)) {
		// 			Vector2 p_size = Vector2(size.width - (theme_cache.normal_style->get_offset().x + vscroll->get_combined_minimum_size().width) * 2, vscroll->get_combined_minimum_size().width);
		// 			Vector2 p_pos = Vector2(theme_cache.normal_style->get_offset().x, size.height - theme_cache.normal_style->get_offset().y - vscroll->get_combined_minimum_size().width);

		// 			draw_style_box(theme_cache.progress_bg_style, Rect2(p_pos, p_size));

		// 			bool right_to_left = false;
		// 			double r = loaded.load();
		// 			int mp = theme_cache.progress_fg_style->get_minimum_size().width;
		// 			int p = std::round(r * (p_size.width - mp));
		// 			if (right_to_left) {
		// 				int p_remaining = std::round((1.0 - r) * (p_size.width - mp));
		// 				draw_style_box(theme_cache.progress_fg_style, Rect2(p_pos + Point2(p_remaining, 0), Size2(p + theme_cache.progress_fg_style->get_minimum_size().width, p_size.height)));
		// 			} else {
		// 				draw_style_box(theme_cache.progress_fg_style, Rect2(p_pos, Size2(p + theme_cache.progress_fg_style->get_minimum_size().width, p_size.height)));
		// 			}
		// 		}
		// 	}

		// 	// Draw main text.
		// 	Rect2 text_rect = _get_text_rect();
		// 	float vofs = vscroll->get_value();

		// 	// Search for the first line.
		// 	int to_line = main->first_invalid_line.load();
		// 	int from_line = _find_first_line(0, to_line, vofs);

		// 	// Bottom margin for text clipping.
		// 	float v_limit = theme_cache.normal_style->get_margin(SIDE_BOTTOM);

		// 	int total_height = INT32_MAX;
		// 	if (to_line && vertical_alignment != VERTICAL_ALIGNMENT_TOP) {
		// 		MutexLock lock(main->lines[to_line - 1].text_buf->get_mutex());
		// 		if (theme_cache.line_separation < 0) {
		// 			// Do not apply to the last line to avoid cutting text.
		// 			total_height = main->lines[to_line - 1].offset.y + main->lines[to_line - 1].text_buf->get_size().y + (main->lines[to_line - 1].text_buf->get_line_count() - 1) * theme_cache.line_separation;
		// 		} else {
		// 			total_height = main->lines[to_line - 1].offset.y + main->lines[to_line - 1].text_buf->get_size().y + (main->lines[to_line - 1].text_buf->get_line_count() - 1) * theme_cache.line_separation + theme_cache.paragraph_separation;
		// 		}
		// 	}
		// 	float vbegin = 0, vsep = 0;
		// 	if (text_rect.size.y > total_height) {
		// 		switch (vertical_alignment) {
		// 			case VERTICAL_ALIGNMENT_TOP: {
		// 				// Nothing.
		// 			} break;
		// 			case VERTICAL_ALIGNMENT_CENTER: {
		// 				vbegin = (text_rect.size.y - total_height) / 2;
		// 			} break;
		// 			case VERTICAL_ALIGNMENT_BOTTOM: {
		// 				vbegin = text_rect.size.y - total_height;
		// 			} break;
		// 			case VERTICAL_ALIGNMENT_FILL: {
		// 				int lines = 0;
		// 				for (int l = from_line; l < to_line; l++) {
		// 					MutexLock lock(main->lines[l].text_buf->get_mutex());
		// 					lines += main->lines[l].text_buf->get_line_count();
		// 				}
		// 				if (lines > 1) {
		// 					vsep = (text_rect.size.y - total_height) / (lines - 1);
		// 				}
		// 			} break;
		// 		}
		// 	}

		// 	Point2 shadow_ofs(theme_cache.shadow_offset_x, theme_cache.shadow_offset_y);

		// 	visible_paragraph_count = 0;
		// 	visible_line_count = 0;
		// 	visible_rect = Rect2i();

		// 	// New cache draw.
		// 	Point2 ofs = text_rect.get_position() + Vector2(0, vbegin + main->lines[from_line].offset.y - vofs);
		// 	int processed_glyphs = 0;
		// 	while (ofs.y < size.height - v_limit && from_line < to_line) {
		// 		MutexLock lock(main->lines[from_line].text_buf->get_mutex());

		// 		int drawn_lines = _draw_line(main, from_line, ofs, text_rect.size.x, vsep, false, theme_cache.default_color, theme_cache.outline_size, theme_cache.font_outline_color, theme_cache.font_shadow_color, theme_cache.shadow_outline_size, shadow_ofs, processed_glyphs);
		// 		visible_line_count += drawn_lines;
		// 		if (drawn_lines > 0) {
		// 			visible_paragraph_count++;
		// 		}
		// 		ofs.y += main->lines[from_line].text_buf->get_size().y + (main->lines[from_line].text_buf->get_line_count() - 1) * (theme_cache.line_separation + vsep) + (theme_cache.paragraph_separation + vsep);
		// 		from_line++;
		// 	}
		// 	if (scroll_follow_visible_characters && scroll_active) {
		// 		vscroll->set_visible(follow_vc_pos > 0);
		// 	}
		// 	if (has_focus() && get_tree()->is_accessibility_enabled()) {
		// 		RID ae;
		// 		if (keyboard_focus_frame && keyboard_focus_item) {
		// 			if (keyboard_focus_on_text) {
		// 				ae = keyboard_focus_frame->lines[keyboard_focus_line].accessibility_text_element;
		// 			} else {
		// 				if (keyboard_focus_item->accessibility_item_element.is_valid()) {
		// 					ae = keyboard_focus_item->accessibility_item_element;
		// 				}
		// 			}
		// 		} else {
		// 			if (!main->lines.is_empty()) {
		// 				ae = main->lines[0].accessibility_text_element;
		// 			}
		// 		}
		// 		if (ac_element_bounds_cache.has(ae)) {
		// 			draw_style_box(theme_cache.focus_style, ac_element_bounds_cache[ae]);
		// 		}
		// 	}
		// } break;

		case NOTIFICATION_INTERNAL_PROCESS: {
			if (is_visible_in_tree()) {
				if (!is_finished()) {
					return;
				}
				// double dt = get_process_delta_time();
				// queue_redraw();
			}
		} break;
	}
}

void RichTextLabel3D::_im_update() {
	_stop_thread();
	
	// Clear mesh.
	RS::get_singleton()->mesh_clear(mesh);
	aabb = AABB();

	// Clear materials.
	for (const KeyValue<SurfaceKey, SurfaceData> &E : surfaces) {
		RenderingServer::get_singleton()->free(E.value.material);
	}
	surfaces.clear();

	Ref<Font> font = _get_font_or_default();
	ERR_FAIL_COND(font.is_null());

	_draw();

	triangle_mesh.unref();
	update_gizmos();

	pending_update = false;
}

void RichTextLabel3D::_draw() {
	if (_validate_line_caches()) {
		set_physics_process_internal(false); // Disable auto refresh, if text is fully processed.
	}
	Size2 size = Size2(width, height);
	// Draw main text.
	Rect2 text_rect = _get_text_rect();
	float vofs = 0.0f;

	// Search for the first line.
	int to_line = main->first_invalid_line.load();
	int first_line = _find_first_line(0, to_line, vofs);

	// Bottom margin for text clipping.
	float v_limit = 0.0f;//theme_cache.normal_style->get_margin(SIDE_BOTTOM);

	int total_height = INT32_MAX;
	if (to_line && vertical_alignment != VERTICAL_ALIGNMENT_TOP) {
		MutexLock lock(main->lines[to_line - 1].text_buf->get_mutex());
		// if (line spa< 0) {
			// Do not apply to the last line to avoid cutting text.
			// total_height = main->lines[to_line - 1].offset.y + main->lines[to_line - 1].text_buf->get_size().y + (main->lines[to_line - 1].text_buf->get_line_count() - 1) * theme_cache.line_separation;
		// } else {
			total_height = (main->lines[to_line - 1].offset.y + main->lines[to_line - 1].text_buf->get_size().y + (main->lines[to_line - 1].text_buf->get_line_count() - 1));// * theme_cache.line_separation + theme_cache.paragraph_separation);
		// }
	}


	float vbegin = 0, vsep = 0;
	// if (text_rect.size.y > total_height) {
		switch (vertical_alignment) {
			case VERTICAL_ALIGNMENT_TOP: {
				// Nothing.
			} break;
			case VERTICAL_ALIGNMENT_CENTER: {
				vbegin = (total_height) / 2;
			} break;
			case VERTICAL_ALIGNMENT_BOTTOM: {
				vbegin = total_height;
			} break;
			case VERTICAL_ALIGNMENT_FILL: {
				int lines = 0;
				for (int l = first_line; l < to_line; l++) {
					MutexLock lock(main->lines[l].text_buf->get_mutex());
					lines += main->lines[l].text_buf->get_line_count();
				}
				// if (lines > 1) {
				// 	vsep = (text_rect.size.y - total_height * pixel_size) / (lines - 1);
				// }
			} break;
		}
	// }

	Point2 shadow_ofs(theme_cache.shadow_offset_x, theme_cache.shadow_offset_y);

	visible_paragraph_count = 0;
	visible_line_count = 0;
	visible_rect = Rect2i();

	// New cache draw.
	// Point2 ofs = text_rect.get_position() + Vector2(0, vbegin + main->lines[from_line].offset.y - vofs);
	Point2 ofs = Vector2(0, vbegin + ((main->lines[first_line].offset.y - vofs)));
	// Vector2 offset = Vector2(0, vbegin + lbl_offset.y * pixel_size);
	int processed_glyphs = 0;
	Vector3 extra_offset = Vector3(0, 0, 0.0f);
	Vector3 extra_rotation = Vector3(0.0f, 0.0f, 0.0f);
	for (int from_line = first_line; from_line < to_line; from_line++)
	{
	// while (ofs.y < size.height - v_limit && from_line < to_line) {
		MutexLock lock(main->lines[from_line].text_buf->get_mutex());
		print_line(vformat("Drawing line %d. Ofs.y: %f to line %d", from_line, ofs.y, to_line));
		int drawn_lines = _draw_line(main, from_line, ofs, extra_offset, extra_rotation, text_rect.size.x, vsep, false, modulate, outline_size, outline_modulate, outline_modulate, 0.0f, shadow_ofs, processed_glyphs);
		visible_line_count += drawn_lines;
		if (drawn_lines > 0) {
			visible_paragraph_count++;
		}
		ofs.y -= ((main->lines[from_line].text_buf->get_size().y + (main->lines[from_line].text_buf->get_line_count() - 1) * (vsep) + (vsep)) * line_spacing);// * (theme_cache.line_separation + vsep) + (theme_cache.paragraph_separation + vsep);
		// from_line++;
	}

	switch (get_billboard_mode()) {
		case StandardMaterial3D::BILLBOARD_ENABLED: {
			real_t size_new = MAX(Math::abs(aabb.position.x), (aabb.position.x + aabb.size.x));
			size_new = MAX(size_new, MAX(Math::abs(aabb.position.y), (aabb.position.y + aabb.size.y)));
			aabb.position = Vector3(-size_new, -size_new, -size_new);
			aabb.size = Vector3(size_new * 2.0, size_new * 2.0, size_new * 2.0);
		} break;
		case StandardMaterial3D::BILLBOARD_FIXED_Y: {
			real_t size_new = MAX(Math::abs(aabb.position.x), (aabb.position.x + aabb.size.x));
			aabb.position.x = -size_new;
			aabb.position.z = -size_new;
			aabb.size.x = size_new * 2.0;
			aabb.size.z = size_new * 2.0;
		} break;
		default:
			break;
	}

	print_line(vformat("Drawing mesh. Surfaces: %d visible line count: %d", surfaces.size(), visible_line_count));
	for (const KeyValue<SurfaceKey, SurfaceData> &E : surfaces) {
		Array mesh_array;
		mesh_array.resize(RS::ARRAY_MAX);
		mesh_array[RS::ARRAY_VERTEX] = E.value.mesh_vertices;
		mesh_array[RS::ARRAY_NORMAL] = E.value.mesh_normals;
		mesh_array[RS::ARRAY_TANGENT] = E.value.mesh_tangents;
		mesh_array[RS::ARRAY_COLOR] = E.value.mesh_colors;
		mesh_array[RS::ARRAY_TEX_UV] = E.value.mesh_uvs;
		mesh_array[RS::ARRAY_INDEX] = E.value.indices;

		RS::SurfaceData sd;
		RS::get_singleton()->mesh_create_surface_data_from_arrays(&sd, RS::PRIMITIVE_TRIANGLES, mesh_array);

		sd.material = E.value.material;

		RS::get_singleton()->mesh_add_surface(mesh, sd);
	}
}

void RichTextLabel3D::queue_redraw() {
	if (pending_update) {
		return;
	}

	pending_update = true;
	
	callable_mp(this, &RichTextLabel3D::_im_update).call_deferred();
}

void RichTextLabel3D::_find_frame(Item *p_item, ItemFrame **r_frame, int *r_line) {
	if (r_frame != nullptr) {
		*r_frame = nullptr;
	}
	if (r_line != nullptr) {
		*r_line = 0;
	}

	Item *item = p_item;

	while (item) {
		if (item->parent != nullptr && item->parent->type == ITEM_FRAME) {
			if (r_frame != nullptr) {
				*r_frame = static_cast<ItemFrame *>(item->parent);
			}
			if (r_line != nullptr) {
				*r_line = item->line;
			}
			return;
		}

		item = item->parent;
	}
}

RichTextLabel3D::Item *RichTextLabel3D::_find_indentable(Item *p_item) {
	Item *indentable = p_item;

	while (indentable) {
		if (indentable->type == ITEM_INDENT || indentable->type == ITEM_LIST) {
			return indentable;
		}
		indentable = indentable->parent;
	}

	return indentable;
}

RichTextLabel3D::ItemFont *RichTextLabel3D::_find_font(Item *p_item) {
	Item *fontitem = p_item;

	while (fontitem) {
		if (fontitem->type == ITEM_FONT) {
			ItemFont *fi = static_cast<ItemFont *>(fontitem);
			switch (fi->def_font) {
				case RTL_NORMAL_FONT: {
					if (fi->variation) {
						Ref<FontVariation> fc = fi->font;
						if (fc.is_valid()) {
							fc->set_base_font(_get_font_or_default());
						}
					} else {
						fi->font = _get_font_or_default();
					}
					if (fi->def_size) {
						fi->font_size = font_size;
					}
				} break;
				case RTL_BOLD_FONT: {
					if (fi->variation) {
						Ref<FontVariation> fc = fi->font;
						if (fc.is_valid()) {
							fc->set_base_font(bold_font);
						}
					} else {
						fi->font = bold_font;
					}
					if (fi->def_size) {
						fi->font_size = bold_font_size;
					}
				} break;
				case RTL_ITALICS_FONT: {
					if (fi->variation) {
						Ref<FontVariation> fc = fi->font;
						if (fc.is_valid()) {
							fc->set_base_font(italics_font);
						}
					} else {
						fi->font = italics_font;
					}
					if (fi->def_size) {
						fi->font_size = italics_font_size;
					}
				} break;
				case RTL_BOLD_ITALICS_FONT: {
					if (fi->variation) {
						Ref<FontVariation> fc = fi->font;
						if (fc.is_valid()) {
							fc->set_base_font(bold_italics_font);
						}
					} else {
						fi->font = bold_italics_font;
					}
					if (fi->def_size) {
						fi->font_size = bold_italics_font_size;
					}
				} break;
				case RTL_MONO_FONT: {
					if (fi->variation) {
						Ref<FontVariation> fc = fi->font;
						if (fc.is_valid()) {
							fc->set_base_font(mono_font);
						}
					} else {
						fi->font = mono_font;
					}
					if (fi->def_size) {
						fi->font_size = mono_font_size;
					}
				} break;
				default: {
				} break;
			}
			return fi;
		}

		fontitem = fontitem->parent;
	}

	return nullptr;
}

RichTextLabel3D::ItemFontSize *RichTextLabel3D::_find_font_size(Item *p_item) {
	Item *sizeitem = p_item;

	while (sizeitem) {
		if (sizeitem->type == ITEM_FONT_SIZE) {
			ItemFontSize *fi = static_cast<ItemFontSize *>(sizeitem);
			return fi;
		}

		sizeitem = sizeitem->parent;
	}

	return nullptr;
}

int RichTextLabel3D::_find_outline_size(Item *p_item, int p_default) {
	Item *sizeitem = p_item;

	while (sizeitem) {
		if (sizeitem->type == ITEM_OUTLINE_SIZE) {
			ItemOutlineSize *fi = static_cast<ItemOutlineSize *>(sizeitem);
			return fi->outline_size;
		}

		sizeitem = sizeitem->parent;
	}

	return p_default;
}

RichTextLabel3D::ItemDropcap *RichTextLabel3D::_find_dc_item(Item *p_item) {
	Item *item = p_item;

	while (item) {
		if (item->type == ITEM_DROPCAP) {
			return static_cast<ItemDropcap *>(item);
		}
		item = item->parent;
	}

	return nullptr;
}

RichTextLabel3D::ItemList *RichTextLabel3D::_find_list_item(Item *p_item) {
	Item *item = p_item;

	while (item) {
		if (item->type == ITEM_LIST) {
			return static_cast<ItemList *>(item);
		}
		item = item->parent;
	}

	return nullptr;
}

int RichTextLabel3D::_find_list(Item *p_item, Vector<int> &r_index, Vector<int> &r_count, Vector<ItemList *> &r_list) {
	Item *item = p_item;
	Item *prev_item = p_item;

	int level = 0;

	while (item) {
		if (item->type == ITEM_LIST) {
			ItemList *list = static_cast<ItemList *>(item);

			ItemFrame *frame = nullptr;
			int line = -1;
			_find_frame(list, &frame, &line);

			int index = 1;
			int count = 1;
			if (frame != nullptr) {
				for (int i = list->line + 1; i < (int)frame->lines.size(); i++) {
					if (_find_list_item(frame->lines[i].from) == list) {
						if (i <= prev_item->line) {
							index++;
						}
						count++;
					}
				}
			}

			r_index.push_back(index);
			r_count.push_back(count);
			r_list.push_back(list);

			prev_item = item;
		}
		level++;
		item = item->parent;
	}

	return level;
}

int RichTextLabel3D::_find_margin(Item *p_item, const Ref<Font> &p_base_font, int p_base_font_size) {
	Item *item = p_item;

	float margin = 0.0;

	while (item) {
		if (item->type == ITEM_FRAME) {
			break;
		}

		if (item->type == ITEM_INDENT) {
			Ref<Font> font = p_base_font;
			int font_size = p_base_font_size;

			ItemFont *font_it = _find_font(item);
			if (font_it) {
				if (font_it->font.is_valid()) {
					font = font_it->font;
				}
				if (font_it->font_size > 0) {
					font_size = font_it->font_size;
				}
			}
			ItemFontSize *font_size_it = _find_font_size(item);
			if (font_size_it && font_size_it->font_size > 0) {
				font_size = font_size_it->font_size;
			}
			margin += tab_size * font->get_char_size(' ', font_size).width;

		} else if (item->type == ITEM_LIST) {
			Ref<Font> font = p_base_font;
			int font_size = p_base_font_size;

			ItemFont *font_it = _find_font(item);
			if (font_it) {
				if (font_it->font.is_valid()) {
					font = font_it->font;
				}
				if (font_it->font_size > 0) {
					font_size = font_it->font_size;
				}
			}
			ItemFontSize *font_size_it = _find_font_size(item);
			if (font_size_it && font_size_it->font_size > 0) {
				font_size = font_size_it->font_size;
			}
			margin += tab_size * font->get_char_size(' ', font_size).width;
		}

		item = item->parent;
	}

	return margin;
}

BitField<TextServer::JustificationFlag> RichTextLabel3D::_find_jst_flags(Item *p_item) {
	Item *item = p_item;

	while (item) {
		if (item->type == ITEM_PARAGRAPH) {
			ItemParagraph *p = static_cast<ItemParagraph *>(item);
			return p->jst_flags;
		}

		item = item->parent;
	}

	return default_jst_flags;
}

PackedFloat32Array RichTextLabel3D::_find_tab_stops(Item *p_item) {
	Item *item = p_item;

	while (item) {
		if (item->type == ITEM_PARAGRAPH) {
			ItemParagraph *p = static_cast<ItemParagraph *>(item);
			return p->tab_stops;
		}

		item = item->parent;
	}

	return default_tab_stops;
}

HorizontalAlignment RichTextLabel3D::_find_alignment(Item *p_item) {
	Item *item = p_item;

	while (item) {
		if (item->type == ITEM_PARAGRAPH) {
			ItemParagraph *p = static_cast<ItemParagraph *>(item);
			return p->alignment;
		}

		item = item->parent;
	}

	return default_alignment;
}

TextServer::Direction RichTextLabel3D::_find_direction(Item *p_item) {
	Item *item = p_item;

	while (item) {
		if (item->type == ITEM_PARAGRAPH) {
			ItemParagraph *p = static_cast<ItemParagraph *>(item);
			if (p->direction != Control::TEXT_DIRECTION_INHERITED) {
				return (TextServer::Direction)p->direction;
			}
		}

		item = item->parent;
	}

	// if (text_direction == Control::TEXT_DIRECTION_INHERITED) {
		return TextServer::DIRECTION_LTR;
	// } else {
		// return (TextServer::Direction)text_direction;
	// }
}

TextServer::StructuredTextParser RichTextLabel3D::_find_stt(Item *p_item) {
	Item *item = p_item;

	while (item) {
		if (item->type == ITEM_PARAGRAPH) {
			ItemParagraph *p = static_cast<ItemParagraph *>(item);
			return p->st_parser;
		}

		item = item->parent;
	}

	return st_parser;
}

String RichTextLabel3D::_find_language(Item *p_item) {
	Item *item = p_item;

	while (item) {
		if (item->type == ITEM_LANGUAGE) {
			ItemLanguage *p = static_cast<ItemLanguage *>(item);
			return p->language;
		} else if (item->type == ITEM_PARAGRAPH) {
			ItemParagraph *p = static_cast<ItemParagraph *>(item);
			return p->language;
		}

		item = item->parent;
	}

	return language;
}

Color RichTextLabel3D::_find_color(Item *p_item, const Color &p_default_color) {
	Item *item = p_item;

	while (item) {
		if (item->type == ITEM_COLOR) {
			ItemColor *color = static_cast<ItemColor *>(item);
			return color->color;
		}

		item = item->parent;
	}

	return p_default_color;
}


Color RichTextLabel3D::_find_outline_color(Item *p_item, const Color &p_default_color) {
	Item *item = p_item;

	while (item) {
		if (item->type == ITEM_OUTLINE_COLOR) {
			ItemOutlineColor *color = static_cast<ItemOutlineColor *>(item);
			return color->color;
		}

		item = item->parent;
	}

	return p_default_color;
}

bool RichTextLabel3D::_find_underline(Item *p_item, Color *r_color) {
	Item *item = p_item;

	while (item) {
		if (item->type == ITEM_UNDERLINE) {
			if (r_color) {
				ItemUnderline *ul = static_cast<ItemUnderline *>(item);
				*r_color = ul->color;
			}
			return true;
		}

		item = item->parent;
	}

	return false;
}

bool RichTextLabel3D::_find_strikethrough(Item *p_item, Color *r_color) {
	Item *item = p_item;

	while (item) {
		if (item->type == ITEM_STRIKETHROUGH) {
			if (r_color) {
				ItemStrikethrough *st = static_cast<ItemStrikethrough *>(item);
				*r_color = st->color;
			}
			return true;
		}

		item = item->parent;
	}

	return false;
}

void RichTextLabel3D::_normalize_subtags(Vector<String> &subtags) {
	for (String &subtag : subtags) {
		subtag = subtag.unquote();
	}
}

Color RichTextLabel3D::_find_bgcolor(Item *p_item) {
	Item *item = p_item;

	while (item) {
		if (item->type == ITEM_BGCOLOR) {
			ItemBGColor *color = static_cast<ItemBGColor *>(item);
			return color->color;
		}

		item = item->parent;
	}

	return Color(0, 0, 0, 0);
}

Color RichTextLabel3D::_find_fgcolor(Item *p_item) {
	Item *item = p_item;

	while (item) {
		if (item->type == ITEM_FGCOLOR) {
			ItemFGColor *color = static_cast<ItemFGColor *>(item);
			return color->color;
		}

		item = item->parent;
	}

	return Color(0, 0, 0, 0);
}

bool RichTextLabel3D::_find_layout_subitem(Item *from, Item *to) {
	if (from && from != to) {
		if (from->type != ITEM_FONT && from->type != ITEM_COLOR && from->type != ITEM_UNDERLINE && from->type != ITEM_STRIKETHROUGH) {
			return true;
		}

		for (Item *E : from->subitems) {
			bool layout = _find_layout_subitem(E, to);

			if (layout) {
				return true;
			}
		}
	}

	return false;
}

void RichTextLabel3D::_thread_function(void *p_userdata) {
	set_current_thread_safe_for_nodes(true);
	_process_line_caches();
	callable_mp(this, &RichTextLabel3D::_thread_end).call_deferred();
}

void RichTextLabel3D::_thread_end() {
	set_physics_process_internal(false);
	// if (!scroll_visible) {
	// 	vscroll->hide();
	// }
	if (is_visible_in_tree()) {
		queue_accessibility_update();
		queue_redraw();
	}
}

void RichTextLabel3D::_stop_thread() {
	if (threaded) {
		stop_thread.store(true);
		wait_until_finished();
	}
}

int RichTextLabel3D::get_pending_paragraphs() const {
	int to_line = main->first_invalid_line.load();
	int lines = main->lines.size();

	return lines - to_line;
}

bool RichTextLabel3D::is_finished() const {
	const_cast<RichTextLabel3D *>(this)->_validate_line_caches();

	if (updating.load()) {
		return false;
	}
	return (main->first_invalid_line.load() == (int)main->lines.size() && main->first_resized_line.load() == (int)main->lines.size() && main->first_invalid_font_line.load() == (int)main->lines.size());
}

bool RichTextLabel3D::is_updating() const {
	return updating.load() || validating.load();
}

void RichTextLabel3D::wait_until_finished() {
	if (task != WorkerThreadPool::INVALID_TASK_ID) {
		WorkerThreadPool::get_singleton()->wait_for_task_completion(task);
		task = WorkerThreadPool::INVALID_TASK_ID;
	}
}

void RichTextLabel3D::set_threaded(bool p_threaded) {
	if (threaded != p_threaded) {
		_stop_thread();
		threaded = p_threaded;
		queue_redraw();
	}
}

bool RichTextLabel3D::is_threaded() const {
	return threaded;
}

// void RichTextLabel3D::set_progress_bar_delay(int p_delay_ms) {
// 	progress_delay = p_delay_ms;
// }

// int RichTextLabel3D::get_progress_bar_delay() const {
// 	return progress_delay;
// }

// _FORCE_INLINE_ float RichTextLabel3D::_update_scroll_exceeds(float p_total_height, float p_ctrl_height, float p_width, int p_idx, float p_old_scroll, float p_text_rect_height) {
// 	updating_scroll = true;

// 	float total_height = p_total_height;
// 	bool exceeds = p_total_height > p_ctrl_height && scroll_active;
// 	if (exceeds != scroll_visible) {
// 		if (exceeds) {
// 			scroll_visible = true;
// 			scroll_w = vscroll->get_combined_minimum_size().width;
// 			vscroll->show();
// 			vscroll->set_anchor_and_offset(SIDE_LEFT, ANCHOR_END, -scroll_w);
// 		} else {
// 			scroll_visible = false;
// 			scroll_w = 0;
// 		}

// 		main->first_resized_line.store(0);

// 		total_height = 0;
// 		for (int j = 0; j <= p_idx; j++) {
// 			total_height = _resize_line(main, j, _get_font_or_default(), font_size, p_width, total_height);

// 			main->first_resized_line.store(j);
// 		}
// 	}
// 	vscroll->set_max(total_height);
// 	vscroll->set_page(p_text_rect_height);
// 	if (scroll_follow && scroll_following) {
// 		vscroll->set_value(total_height);
// 	} else {
// 		vscroll->set_value(p_old_scroll);
// 	}
// 	updating_scroll = false;

// 	return total_height;
// }

bool RichTextLabel3D::_validate_line_caches() {
	if (updating.load()) {
		return false;
	}
	validating.store(true);
	if (main->first_invalid_line.load() == (int)main->lines.size()) {
		MutexLock data_lock(data_mutex);
		Rect2 text_rect = _get_text_rect();

		float ctrl_height = height;

		// Update fonts.
		float old_scroll = 0.0f;
		if (main->first_invalid_font_line.load() != (int)main->lines.size()) {
			for (int i = main->first_invalid_font_line.load(); i < (int)main->lines.size(); i++) {
				_update_line_font(main, i, _get_font_or_default(), font_size);
			}
			main->first_resized_line.store(main->first_invalid_font_line.load());
			main->first_invalid_font_line.store(main->lines.size());
		}

		if (main->first_resized_line.load() == (int)main->lines.size()) {
			// vscroll->set_value(old_scroll);
			validating.store(false);
			// if (!scroll_visible) {
				// vscroll->hide();
			// }
			// queue_accessibility_update();
			return true;
		}

		// Resize lines without reshaping.
		int fi = main->first_resized_line.load();

		float total_height = (fi == 0) ? 0 : _calculate_line_vertical_offset(main->lines[fi - 1]);
		for (int i = fi; i < (int)main->lines.size(); i++) {
			total_height = _resize_line(main, i, _get_font_or_default(), font_size, text_rect.get_size().width, total_height);
			// total_height = _update_scroll_exceeds(total_height, ctrl_height, text_rect.get_size().width, i, old_scroll, text_rect.size.height);
			main->first_resized_line.store(i);
		}

		main->first_resized_line.store(main->lines.size());

		// if (fit_content) {
		// 	update_minimum_size();
		// }
		validating.store(false);
		// if (!scroll_visible) {
		// 	vscroll->hide();
		// }
		// queue_accessibility_update();
		return true;
	}
	validating.store(false);
	stop_thread.store(false);
	if (threaded) {
		updating.store(true);
		loaded.store(true);
		task = WorkerThreadPool::get_singleton()->add_template_task(this, &RichTextLabel3D::_thread_function, nullptr, true, vformat("RichTextLabel3DShape:%x", (int64_t)get_instance_id()));
		set_physics_process_internal(true);
		loading_started = OS::get_singleton()->get_ticks_msec();
		// queue_accessibility_update();
		return false;
	} else {
		updating.store(true);
		_process_line_caches();
		// if (!scroll_visible) {
		// 	vscroll->hide();
		// }
		// queue_accessibility_update();
		queue_redraw();
		return true;
	}
}

void RichTextLabel3D::_process_line_caches() {
	// Shape invalid lines.
	if (!is_inside_tree()) {
		updating.store(false);
		return;
	}

	MutexLock data_lock(data_mutex);
	float p_width = width;
	// Rect2 text_rect = _get_text_rect();

	float ctrl_height = height;
	int fi = main->first_invalid_line.load();
	int total_chars = main->lines[fi].char_offset;
	float old_scroll = 0.0f;

	float total_height = 0;
	if (fi != 0) {
		int sr = MIN(main->first_invalid_font_line.load(), main->first_resized_line.load());

		// Update fonts.
		for (int i = main->first_invalid_font_line.load(); i < fi; i++) {
			_update_line_font(main, i, _get_font_or_default(), font_size);

			main->first_invalid_font_line.store(i);

			if (stop_thread.load()) {
				updating.store(false);
				return;
			}
		}

		// Resize lines without reshaping.
		if (sr != 0) {
			total_height = _calculate_line_vertical_offset(main->lines[sr - 1]);
		}

		for (int i = sr; i < fi; i++) {
			total_height = _resize_line(main, i, _get_font_or_default(), font_size, p_width, total_height);
			// total_height = _update_scroll_exceeds(total_height, ctrl_height, text_rect.get_size().width, i, old_scroll, text_rect.size.height);

			main->first_resized_line.store(i);

			if (stop_thread.load()) {
				updating.store(false);
				return;
			}
		}
	}

	total_height = (fi == 0) ? 0 : _calculate_line_vertical_offset(main->lines[fi - 1]);
	for (int i = fi; i < (int)main->lines.size(); i++) {
		total_height = _shape_line(main, i, _get_font_or_default(), font_size, p_width, total_height, &total_chars);
		// total_height = _update_scroll_exceeds(total_height, ctrl_height, text_rect.get_size().width, i, old_scroll, text_rect.size.height);

		main->first_invalid_line.store(i);
		main->first_resized_line.store(i);
		main->first_invalid_font_line.store(i);

		if (stop_thread.load()) {
			updating.store(false);
			return;
		}
		loaded.store(double(i) / double(main->lines.size()));
	}

	main->first_invalid_line.store(main->lines.size());
	main->first_resized_line.store(main->lines.size());
	main->first_invalid_font_line.store(main->lines.size());
	updating.store(false);

	// if (fit_content) {
	// 	update_minimum_size();
	// }
	emit_signal(SceneStringName(finished));
}

void RichTextLabel3D::_invalidate_current_line(ItemFrame *p_frame) {
	if ((int)p_frame->lines.size() - 1 <= p_frame->first_invalid_line) {
		p_frame->first_invalid_line = (int)p_frame->lines.size() - 1;
		queue_accessibility_update();
	}
}

// void RichTextLabel3D::_texture_changed(RID p_item) {
// 	Item *it = items.get_or_null(p_item);
// 	if (it && it->type == ITEM_IMAGE) {
// 		ItemImage *img = reinterpret_cast<ItemImage *>(it);
// 		Size2 new_size = _get_image_size(img->image, img->rq_size.width, img->rq_size.height, img->region);
// 		if (img->size != new_size) {
// 			main->first_invalid_line.store(0);
// 			img->size = new_size;
// 		}
// 	}
// 	queue_redraw();
// }

void RichTextLabel3D::add_text(const String &p_text) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	if (current->type == ITEM_TABLE) {
		return; //can't add anything here
	}

	int pos = 0;
	String t = p_text.replace("\r\n", "\n");

	while (pos < t.length()) {
		int end = t.find_char('\n', pos);
		String line;
		bool eol = false;
		if (end == -1) {
			end = t.length();
		} else {
			eol = true;
		}

		if (pos == 0 && end == t.length()) {
			line = t;
		} else {
			line = t.substr(pos, end - pos);
		}

		if (line.length() > 0) {
			if (current->subitems.size() && current->subitems.back()->get()->type == ITEM_TEXT) {
				//append text condition!
				ItemText *ti = static_cast<ItemText *>(current->subitems.back()->get());
				ti->text += line;
				_invalidate_current_line(main);

			} else {
				//append item condition
				ItemText *item = memnew(ItemText);
				item->owner = get_instance_id();
				item->rid = items.make_rid(item);
				item->text = line;
				_add_item(item, false);
			}
		}

		if (eol) {
			ItemNewline *item = memnew(ItemNewline);
			item->owner = get_instance_id();
			item->rid = items.make_rid(item);
			item->line = current_frame->lines.size();
			_add_item(item, false);
			current_frame->lines.resize(current_frame->lines.size() + 1);
			if (item->type != ITEM_NEWLINE) {
				current_frame->lines[current_frame->lines.size() - 1].from = item;
			}
			_invalidate_current_line(current_frame);
		}

		pos = end + 1;
	}
	queue_redraw();
}

void RichTextLabel3D::_add_item(Item *p_item, bool p_enter, bool p_ensure_newline) {
	if (!internal_stack_editing) {
		stack_externally_modified = true;
	}

	if (p_enter && !parsing_bbcode.load() && !tag_stack.is_empty()) {
		tag_stack.push_back(U"?");
	}

	p_item->parent = current;
	p_item->E = current->subitems.push_back(p_item);
	p_item->index = current_idx++;
	p_item->char_ofs = current_char_ofs;
	if (p_item->type == ITEM_TEXT) {
		ItemText *t = static_cast<ItemText *>(p_item);
		current_char_ofs += t->text.length();
	} else if (p_item->type == ITEM_IMAGE) {
		current_char_ofs++;
	} else if (p_item->type == ITEM_NEWLINE) {
		current_char_ofs++;
	}

	if (p_enter) {
		current = p_item;
	}

	if (p_ensure_newline) {
		Item *from = current_frame->lines[current_frame->lines.size() - 1].from;
		// only create a new line for Item types that generate content/layout, ignore those that represent formatting/styling
		if (_find_layout_subitem(from, p_item)) {
			_invalidate_current_line(current_frame);
			current_frame->lines.resize(current_frame->lines.size() + 1);
		}
	}

	if (current_frame->lines[current_frame->lines.size() - 1].from == nullptr) {
		current_frame->lines[current_frame->lines.size() - 1].from = p_item;
	}
	p_item->line = current_frame->lines.size() - 1;

	_invalidate_current_line(current_frame);

	queue_accessibility_update();
	queue_redraw();
}

Size2 RichTextLabel3D::_get_image_size(const Ref<Texture2D> &p_image, int p_width, int p_height, const Rect2 &p_region) {
	Size2 ret;
	if (p_width > 0) {
		// custom width
		ret.width = p_width;
		if (p_height > 0) {
			// custom height
			ret.height = p_height;
		} else {
			// calculate height to keep aspect ratio
			if (p_region.has_area()) {
				ret.height = p_region.get_size().height * p_width / p_region.get_size().width;
			} else {
				ret.height = p_image->get_height() * p_width / p_image->get_width();
			}
		}
	} else {
		if (p_height > 0) {
			// custom height
			ret.height = p_height;
			// calculate width to keep aspect ratio
			if (p_region.has_area()) {
				ret.width = p_region.get_size().width * p_height / p_region.get_size().height;
			} else {
				ret.width = p_image->get_width() * p_height / p_image->get_height();
			}
		} else {
			if (p_region.has_area()) {
				// if the image has a region, keep the region size
				ret = p_region.get_size();
			} else {
				// keep original width and height
				ret = p_image->get_size();
			}
		}
	}
	return ret;
}

void RichTextLabel3D::add_hr(int p_width, int p_height, const Color &p_color, HorizontalAlignment p_alignment, bool p_width_in_percent, bool p_height_in_percent) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	if (current->type == ITEM_TABLE) {
		return;
	}

	ERR_FAIL_COND(p_width < 0);
	ERR_FAIL_COND(p_height < 0);

	ItemParagraph *p_item = memnew(ItemParagraph);
	p_item->owner = get_instance_id();
	p_item->rid = items.make_rid(p_item);
	p_item->alignment = p_alignment;
	_add_item(p_item, true, true);

	ItemImage *item = memnew(ItemImage);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);

	item->image = theme_cache.horizontal_rule;
	item->color = p_color;
	item->inline_align = INLINE_ALIGNMENT_CENTER;
	item->rq_size = Size2(p_width, p_height);
	item->size = _get_image_size(theme_cache.horizontal_rule, p_width, p_height, Rect2());
	item->width_in_percent = p_width_in_percent;
	item->height_in_percent = p_height_in_percent;

	// item->image->connect_changed(callable_mp(this, &RichTextLabel3D::_texture_changed).bind(item->rid), CONNECT_REFERENCE_COUNTED);

	_add_item(item, false);
	hr_list.insert(item->rid);

	if (current->type == ITEM_FRAME) {
		current_frame = static_cast<ItemFrame *>(current)->parent_frame;
	}
	current = current->parent;
	if (!parsing_bbcode.load() && !tag_stack.is_empty()) {
		tag_stack.pop_back();
	}
}

void RichTextLabel3D::add_image(const Ref<Texture2D> &p_image, int p_width, int p_height, const Color &p_color, InlineAlignment p_alignment, const Rect2 &p_region, const Variant &p_key, bool p_pad, const String &p_tooltip, bool p_width_in_percent, bool p_height_in_percent, const String &p_alt_text) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	if (current->type == ITEM_TABLE) {
		return;
	}

	ERR_FAIL_COND(p_image.is_null());
	ERR_FAIL_COND(p_image->get_width() == 0);
	ERR_FAIL_COND(p_image->get_height() == 0);
	ERR_FAIL_COND(p_width < 0);
	ERR_FAIL_COND(p_height < 0);

	ItemImage *item = memnew(ItemImage);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);

	if (p_region.has_area()) {
		Ref<AtlasTexture> atlas_tex = memnew(AtlasTexture);
		atlas_tex->set_atlas(p_image);
		atlas_tex->set_region(p_region);
		item->image = atlas_tex;
	} else {
		item->image = p_image;
	}
	item->color = p_color;
	item->inline_align = p_alignment;
	item->rq_size = Size2(p_width, p_height);
	item->region = p_region;
	item->size = _get_image_size(p_image, p_width, p_height, p_region);
	item->width_in_percent = p_width_in_percent;
	item->height_in_percent = p_height_in_percent;
	item->pad = p_pad;
	item->key = p_key;
	item->tooltip = p_tooltip;
	item->alt_text = p_alt_text;

	// item->image->connect_changed(callable_mp(this, &RichTextLabel3D::_texture_changed).bind(item->rid), CONNECT_REFERENCE_COUNTED);

	_add_item(item, false);
	// update_configuration_warnings();
}

void RichTextLabel3D::add_newline() {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	if (current->type == ITEM_TABLE) {
		return;
	}
	ItemNewline *item = memnew(ItemNewline);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->line = current_frame->lines.size();
	_add_item(item, false);
	current_frame->lines.resize(current_frame->lines.size() + 1);
	_invalidate_current_line(current_frame);
	queue_redraw();
}

void RichTextLabel3D::_remove_frame(HashSet<Item *> &r_erase_list, ItemFrame *p_frame, int p_line, bool p_erase, int p_char_offset, int p_line_offset) {
	Line &l = p_frame->lines[p_line];
	Item *it_to = (p_line + 1 < (int)p_frame->lines.size()) ? p_frame->lines[p_line + 1].from : nullptr;
	if (!p_erase) {
		l.char_offset -= p_char_offset;
	}

	for (Item *it = l.from; it && it != it_to;) {
		Item *next_it = _get_next_item(it);
		it->line -= p_line_offset;
		if (!p_erase) {
			while (r_erase_list.has(it->parent)) {
				it->E->erase();
				it->parent = it->parent->parent;
				it->E = it->parent->subitems.push_back(it);
			}
		}
		if (it->type == ITEM_TABLE) {
			ItemTable *table = static_cast<ItemTable *>(it);
			for (List<Item *>::Element *sub_it = table->subitems.front(); sub_it; sub_it = sub_it->next()) {
				ERR_CONTINUE(sub_it->get()->type != ITEM_FRAME); // Children should all be frames.
				ItemFrame *frame = static_cast<ItemFrame *>(sub_it->get());
				for (int i = 0; i < (int)frame->lines.size(); i++) {
					_remove_frame(r_erase_list, frame, i, p_erase, p_char_offset, 0);
				}
				if (p_erase) {
					r_erase_list.insert(frame);
				} else {
					frame->char_ofs -= p_char_offset;
				}
			}
		}
		if (p_erase) {
			r_erase_list.insert(it);
		} else {
			it->char_ofs -= p_char_offset;
		}
		it = next_it;
	}
}

bool RichTextLabel3D::remove_paragraph(int p_paragraph, bool p_no_invalidate) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	if (p_paragraph >= (int)main->lines.size() || p_paragraph < 0) {
		return false;
	}

	stack_externally_modified = true;

	if (main->lines.size() == 1) {
		// Clear all.
		main->_clear_children();
		current = main;
		current_frame = main;
		main->lines.clear();
		main->lines.resize(1);

		current_char_ofs = 0;
	} else {
		HashSet<Item *> erase_list;
		Line &l = main->lines[p_paragraph];
		int off = l.char_count;
		for (int i = p_paragraph; i < (int)main->lines.size(); i++) {
			if (i == p_paragraph) {
				_remove_frame(erase_list, main, i, true, off, 0);
			} else {
				_remove_frame(erase_list, main, i, false, off, 1);
			}
		}
		for (HashSet<Item *>::Iterator E = erase_list.begin(); E; ++E) {
			Item *it = *E;
			if (current_frame == it) {
				current_frame = main;
			}
			if (current == it) {
				current = main;
			}
			if (!erase_list.has(it->parent)) {
				it->E->erase();
			}
			items.free(it->rid);
			it->subitems.clear();
			memdelete(it);
		}
		main->lines.remove_at(p_paragraph);
		current_char_ofs -= off;
	}

	selection.click_frame = nullptr;
	selection.click_item = nullptr;
	selection.active = false;

	if (is_processing_internal()) {
		bool process_enabled = false;
		// Item *it = main;
		// while (it) {
		// 	Vector<ItemFX *> fx_stack;
		// 	_fetch_item_fx_stack(it, fx_stack);
		// 	if (fx_stack.size()) {
		// 		process_enabled = true;
		// 		break;
		// 	}
		// 	it = _get_next_item(it, true);
		// }
		set_process_internal(process_enabled);
	}

	if (p_no_invalidate) {
		// Do not invalidate cache, only update vertical offsets of the paragraphs after deleted one and scrollbar.
		int to_line = main->first_invalid_line.load() - 1;
		float total_height = (p_paragraph == 0) ? 0 : _calculate_line_vertical_offset(main->lines[p_paragraph - 1]);
		for (int i = p_paragraph; i < to_line; i++) {
			MutexLock lock(main->lines[to_line - 1].text_buf->get_mutex());
			main->lines[i].offset.y = total_height;
			total_height = _calculate_line_vertical_offset(main->lines[i]);
		}
		// updating_scroll = true;
		// vscroll->set_max(total_height);
		// updating_scroll = false;

		main->first_invalid_line.store(MAX(main->first_invalid_line.load() - 1, 0));
		main->first_resized_line.store(MAX(main->first_resized_line.load() - 1, 0));
		main->first_invalid_font_line.store(MAX(main->first_invalid_font_line.load() - 1, 0));
	} else {
		// Invalidate cache after the deleted paragraph.
		main->first_invalid_line.store(MIN(main->first_invalid_line.load(), p_paragraph));
		main->first_resized_line.store(MIN(main->first_resized_line.load(), p_paragraph));
		main->first_invalid_font_line.store(MIN(main->first_invalid_font_line.load(), p_paragraph));
	}
	queue_redraw();

	return true;
}

bool RichTextLabel3D::invalidate_paragraph(int p_paragraph) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	if (p_paragraph >= (int)main->lines.size() || p_paragraph < 0) {
		return false;
	}

	// Invalidate cache.
	main->first_invalid_line.store(MIN(main->first_invalid_line.load(), p_paragraph));
	main->first_resized_line.store(MIN(main->first_resized_line.load(), p_paragraph));
	main->first_invalid_font_line.store(MIN(main->first_invalid_font_line.load(), p_paragraph));

	queue_redraw();
	update_configuration_warnings();

	return true;
}

void RichTextLabel3D::push_dropcap(const String &p_string, const Ref<Font> &p_font, int p_size, const Rect2 &p_dropcap_margins, const Color &p_color, int p_ol_size, const Color &p_ol_color) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ERR_FAIL_COND(p_string.is_empty());
	ERR_FAIL_COND(p_font.is_null());
	ERR_FAIL_COND(p_size <= 0);

	ItemDropcap *item = memnew(ItemDropcap);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->text = p_string.replace("\r\n", "\n");
	item->font = p_font;
	item->font_size = p_size;
	item->color = p_color;
	item->ol_size = p_ol_size;
	item->ol_color = p_ol_color;
	item->dropcap_margins = p_dropcap_margins;
	p_font->connect_changed(callable_mp(this, &RichTextLabel3D::_invalidate_fonts), CONNECT_REFERENCE_COUNTED);

	_add_item(item, false);
}

void RichTextLabel3D::_push_def_font_var(DefaultFont p_def_font, const Ref<Font> &p_font, int p_size) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ItemFont *item = memnew(ItemFont);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->def_font = p_def_font;
	item->variation = true;
	item->font = p_font;
	item->font_size = p_size;
	item->def_size = (p_size <= 0);
	p_font->connect_changed(callable_mp(this, &RichTextLabel3D::_invalidate_fonts), CONNECT_REFERENCE_COUNTED);

	_add_item(item, true);
}

void RichTextLabel3D::_push_def_font(DefaultFont p_def_font) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ItemFont *item = memnew(ItemFont);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->def_font = p_def_font;
	item->def_size = true;
	_add_item(item, true);
}

void RichTextLabel3D::push_font(const Ref<Font> &p_font, int p_size) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ERR_FAIL_COND(p_font.is_null());
	ItemFont *item = memnew(ItemFont);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->font = p_font;
	item->font_size = p_size;
	p_font->connect_changed(callable_mp(this, &RichTextLabel3D::_invalidate_fonts), CONNECT_REFERENCE_COUNTED);

	_add_item(item, true);
}

void RichTextLabel3D::_invalidate_fonts() {
	_stop_thread();
	main->first_invalid_font_line.store(0); // Invalidate all lines.
	queue_redraw();
}

void RichTextLabel3D::push_normal() {
	ERR_FAIL_COND(_get_font_or_default().is_null());

	_push_def_font(RTL_NORMAL_FONT);
}

void RichTextLabel3D::push_bold() {
	ERR_FAIL_COND(bold_font.is_null());

	ItemFont *item_font = _find_font(current);
	_push_def_font((item_font && item_font->def_font == RTL_ITALICS_FONT) ? RTL_BOLD_ITALICS_FONT : RTL_BOLD_FONT);
}

void RichTextLabel3D::push_bold_italics() {
	ERR_FAIL_COND(bold_italics_font.is_null());

	_push_def_font(RTL_BOLD_ITALICS_FONT);
}

void RichTextLabel3D::push_italics() {
	ERR_FAIL_COND(italics_font.is_null());

	ItemFont *item_font = _find_font(current);
	_push_def_font((item_font && item_font->def_font == RTL_BOLD_FONT) ? RTL_BOLD_ITALICS_FONT : RTL_ITALICS_FONT);
}

void RichTextLabel3D::push_mono() {
	ERR_FAIL_COND(mono_font.is_null());

	_push_def_font(RTL_MONO_FONT);
}

void RichTextLabel3D::push_font_size(int p_font_size) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ItemFontSize *item = memnew(ItemFontSize);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->font_size = p_font_size;
	_add_item(item, true);
}

void RichTextLabel3D::push_outline_size(int p_ol_size) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ItemOutlineSize *item = memnew(ItemOutlineSize);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->outline_size = p_ol_size;
	_add_item(item, true);
}

void RichTextLabel3D::push_color(const Color &p_color) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ItemColor *item = memnew(ItemColor);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->color = p_color;
	_add_item(item, true);
}

void RichTextLabel3D::push_outline_color(const Color &p_color) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ItemOutlineColor *item = memnew(ItemOutlineColor);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->color = p_color;
	_add_item(item, true);
}

void RichTextLabel3D::push_underline(const Color &p_color) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ItemUnderline *item = memnew(ItemUnderline);
	item->color = p_color;
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);

	_add_item(item, true);
}

void RichTextLabel3D::push_strikethrough(const Color &p_color) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ItemStrikethrough *item = memnew(ItemStrikethrough);
	item->color = p_color;
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);

	_add_item(item, true);
}

void RichTextLabel3D::push_paragraph(HorizontalAlignment p_alignment, Control::TextDirection p_direction, const String &p_language, TextServer::StructuredTextParser p_st_parser, BitField<TextServer::JustificationFlag> p_jst_flags, const PackedFloat32Array &p_tab_stops) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);

	ItemParagraph *item = memnew(ItemParagraph);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->alignment = p_alignment;
	item->direction = p_direction;
	item->language = p_language;
	item->st_parser = p_st_parser;
	item->jst_flags = p_jst_flags;
	item->tab_stops = p_tab_stops;
	_add_item(item, true, true);
}

void RichTextLabel3D::push_indent(int p_level) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ERR_FAIL_COND(p_level < 0);

	ItemIndent *item = memnew(ItemIndent);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->level = p_level;
	_add_item(item, true, true);
}

void RichTextLabel3D::push_list(int p_level, ListType p_list, bool p_capitalize, const String &p_bullet) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ERR_FAIL_COND(p_level < 0);

	ItemList *item = memnew(ItemList);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->list_type = p_list;
	item->level = p_level;
	item->capitalize = p_capitalize;
	item->bullet = p_bullet;
	_add_item(item, true, true);
}

void RichTextLabel3D::push_meta(const Variant &p_meta, MetaUnderline p_underline_mode, const String &p_tooltip) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ItemMeta *item = memnew(ItemMeta);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->meta = p_meta;
	item->underline = p_underline_mode;
	item->tooltip = p_tooltip;
	_add_item(item, true);
}

void RichTextLabel3D::push_language(const String &p_language) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ItemLanguage *item = memnew(ItemLanguage);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->language = p_language;
	_add_item(item, true);
}

void RichTextLabel3D::push_hint(const String &p_string) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ItemHint *item = memnew(ItemHint);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->description = p_string;
	_add_item(item, true);
}

void RichTextLabel3D::push_table(int p_columns, InlineAlignment p_alignment, int p_align_to_row, const String &p_alt_text) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ERR_FAIL_COND(p_columns < 1);
	ItemTable *item = memnew(ItemTable);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->name = p_alt_text;
	item->columns.resize(p_columns);
	item->total_width = 0;
	item->inline_align = p_alignment;
	item->align_to_row = p_align_to_row;
	for (int i = 0; i < (int)item->columns.size(); i++) {
		item->columns[i].expand = false;
		item->columns[i].shrink = true;
		item->columns[i].expand_ratio = 1;
	}
	_add_item(item, true, false);
}

void RichTextLabel3D::push_bgcolor(const Color &p_color) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ItemBGColor *item = memnew(ItemBGColor);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->color = p_color;
	_add_item(item, true);
}

void RichTextLabel3D::push_fgcolor(const Color &p_color) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ItemFGColor *item = memnew(ItemFGColor);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->color = p_color;
	_add_item(item, true);
}

void RichTextLabel3D::push_context() {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type == ITEM_TABLE);
	ItemContext *item = memnew(ItemContext);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	_add_item(item, true);
}

void RichTextLabel3D::set_table_column_expand(int p_column, bool p_expand, int p_ratio, bool p_shrink) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_TABLE);

	ItemTable *table = static_cast<ItemTable *>(current);
	ERR_FAIL_INDEX(p_column, (int)table->columns.size());
	table->columns[p_column].expand = p_expand;
	table->columns[p_column].shrink = p_shrink;
	table->columns[p_column].expand_ratio = p_ratio;
}

void RichTextLabel3D::set_table_column_name(int p_column, const String &p_name) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_TABLE);

	ItemTable *table = static_cast<ItemTable *>(current);
	ERR_FAIL_INDEX(p_column, (int)table->columns.size());
	table->columns[p_column].name = p_name;
}

void RichTextLabel3D::set_cell_row_background_color(const Color &p_odd_row_bg, const Color &p_even_row_bg) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_FRAME);

	ItemFrame *cell = static_cast<ItemFrame *>(current);
	ERR_FAIL_COND(!cell->cell);
	cell->odd_row_bg = p_odd_row_bg;
	cell->even_row_bg = p_even_row_bg;
}

void RichTextLabel3D::set_cell_meta(const Variant &p_meta) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_FRAME);

	ItemFrame *cell = static_cast<ItemFrame *>(current);
	ERR_FAIL_COND(!cell->cell);
	cell->meta = p_meta;
	cell->has_meta = true;
}

void RichTextLabel3D::set_cell_offset(const Vector3 &p_offset) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_FRAME);

	ItemFrame *cell = static_cast<ItemFrame *>(current);
	ERR_FAIL_COND(!cell->cell);
	cell->offset = p_offset;
}

void RichTextLabel3D::set_cell_rotation(const Vector3 &p_rotation) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_FRAME);

	ItemFrame *cell = static_cast<ItemFrame *>(current);
	ERR_FAIL_COND(!cell->cell);
	cell->rotation = p_rotation;
}

void RichTextLabel3D::set_cell_radius(const Vector2 &p_radius) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_FRAME);

	ItemFrame *cell = static_cast<ItemFrame *>(current);
	ERR_FAIL_COND(!cell->cell);
	cell->radius = p_radius;
}

void RichTextLabel3D::set_cell_bevel_segments(int p_bevel_segments) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_FRAME);

	ItemFrame *cell = static_cast<ItemFrame *>(current);
	ERR_FAIL_COND(!cell->cell);
	cell->bevel_segments = p_bevel_segments;
}

void RichTextLabel3D::set_cell_thickness(float p_thickness) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_FRAME);

	ItemFrame *cell = static_cast<ItemFrame *>(current);
	ERR_FAIL_COND(!cell->cell);
	cell->thickness = p_thickness;
}

void RichTextLabel3D::set_cell_border_color(const Color &p_color) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_FRAME);

	ItemFrame *cell = static_cast<ItemFrame *>(current);
	ERR_FAIL_COND(!cell->cell);
	cell->border = p_color;
}

void RichTextLabel3D::set_cell_vertical_align_center(bool p_vertical_align_center) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_FRAME);

	ItemFrame *cell = static_cast<ItemFrame *>(current);
	ERR_FAIL_COND(!cell->cell);
	cell->vertical_align_center = p_vertical_align_center;
}

void RichTextLabel3D::push_cell_with_image_background(const Ref<Texture2D> &p_image, const Rect2 &p_region, const Variant &p_key, bool p_width_in_percent, bool p_height_in_percent) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_TABLE);

	ERR_FAIL_COND(p_image.is_null());
	ERR_FAIL_COND(p_image->get_width() == 0);
	ERR_FAIL_COND(p_image->get_height() == 0);

	ItemFrameImageBackground *item = memnew(ItemFrameImageBackground);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->parent_frame = current_frame;
	_add_item(item, true);
	current_frame = item;

	if (p_region.has_area()) {
		Ref<AtlasTexture> atlas_tex = memnew(AtlasTexture);
		atlas_tex->set_atlas(p_image);
		atlas_tex->set_region(p_region);
		item->image = atlas_tex;
	} else {
		item->image = p_image;
	}
	item->rq_size = Size2(p_image->get_width(), p_image->get_height());
	item->region = p_region;
	item->size = _get_image_size(p_image, 0, 0, p_region);
	item->width_in_percent = p_width_in_percent;
	item->height_in_percent = p_height_in_percent;
	item->key = p_key;
	item->odd_row_bg = Color(1, 1, 1, 1);
	item->even_row_bg = Color(1, 1, 1, 1);
	item->cell = true;
	item->lines.resize(1);
	item->lines[0].from = nullptr;
	item->first_invalid_line.store(0); // parent frame last line ???
	queue_accessibility_update();
}

void RichTextLabel3D::set_cell_size_override(const Size2 &p_min_size, const Size2 &p_max_size) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_FRAME);

	ItemFrame *cell = static_cast<ItemFrame *>(current);
	ERR_FAIL_COND(!cell->cell);
	cell->min_size_over = p_min_size;
	cell->max_size_over = p_max_size;
}

void RichTextLabel3D::set_cell_padding(const Rect2 &p_padding) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_FRAME);

	ItemFrame *cell = static_cast<ItemFrame *>(current);
	ERR_FAIL_COND(!cell->cell);
	cell->padding = p_padding;
}

void RichTextLabel3D::set_cell_margins(const Rect2 &p_margins) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_FRAME);

	ItemFrame *cell = static_cast<ItemFrame *>(current);
	ERR_FAIL_COND(!cell->cell);
	cell->margins = p_margins;
}

void RichTextLabel3D::set_cell_bg_image_offset(const Rect2 &p_img_offset) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_FRAME);

	ItemFrameImageBackground *cell = static_cast<ItemFrameImageBackground *>(current);
	ERR_FAIL_COND(!cell->cell);
	cell->rect_offset = p_img_offset;
	cell->has_rect_offset = true;
}

void RichTextLabel3D::set_cell_bg_image_keep_aspect_center(bool p_keep_aspect_center) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_FRAME);

	ItemFrameImageBackground *cell = static_cast<ItemFrameImageBackground *>(current);
	ERR_FAIL_COND(!cell->cell);
	cell->keep_aspect_center = p_keep_aspect_center;
}

void RichTextLabel3D::push_cell() {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_COND(current->type != ITEM_TABLE);

	ItemFrame *item = memnew(ItemFrame);
	item->owner = get_instance_id();
	item->rid = items.make_rid(item);
	item->parent_frame = current_frame;
	_add_item(item, true);
	current_frame = item;
	item->cell = true;
	item->lines.resize(1);
	item->lines[0].from = nullptr;
	item->first_invalid_line.store(0); // parent frame last line ???
	queue_accessibility_update();
}

int RichTextLabel3D::get_current_table_column() const {
	ERR_FAIL_COND_V(current->type != ITEM_TABLE, -1);

	ItemTable *table = static_cast<ItemTable *>(current);
	return table->subitems.size() % table->columns.size();
}

void RichTextLabel3D::pop() {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_NULL(current->parent);

	if (current->type == ITEM_FRAME) {
		current_frame = static_cast<ItemFrame *>(current)->parent_frame;
	}
	current = current->parent;
	if (!parsing_bbcode.load() && !tag_stack.is_empty()) {
		tag_stack.pop_back();
	}
}

void RichTextLabel3D::pop_context() {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	ERR_FAIL_NULL(current->parent);

	while (current->parent && current != main) {
		if (current->type == ITEM_FRAME) {
			current_frame = static_cast<ItemFrame *>(current)->parent_frame;
		} else if (current->type == ITEM_CONTEXT) {
			if (!parsing_bbcode.load() && !tag_stack.is_empty()) {
				tag_stack.pop_back();
			}
			current = current->parent;
			return;
		}
		if (!parsing_bbcode.load() && !tag_stack.is_empty()) {
			tag_stack.pop_back();
		}
		current = current->parent;
	}
}

void RichTextLabel3D::pop_all() {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	current = main;
	current_frame = main;
}

void RichTextLabel3D::clear() {
	_stop_thread();
	set_process_internal(false);
	MutexLock data_lock(data_mutex);

	stack_externally_modified = false;

	tag_stack.clear();
	main->_clear_children();
	current = main;
	current_frame = main;
	main->lines.clear();
	main->lines.resize(1);
	main->first_invalid_line.store(0);
	
	keyboard_focus_frame = nullptr;
	keyboard_focus_line = 0;
	keyboard_focus_item = nullptr;

	current_idx = 1;
	current_char_ofs = 0;
	// if (scroll_follow) {
	// 	scroll_following = true;
	// }

	// queue_accessibility_update();
	// update_configuration_warnings();
}

void RichTextLabel3D::set_tab_size(int p_spaces) {
	if (tab_size == p_spaces) {
		return;
	}

	_stop_thread();

	tab_size = p_spaces;
	main->first_resized_line.store(0);
	queue_redraw();
}

int RichTextLabel3D::get_tab_size() const {
	return tab_size;
}
 
void RichTextLabel3D::parse_bbcode(const String &p_bbcode) {
	clear();
	append_text(p_bbcode);
}

String RichTextLabel3D::_get_tag_value(const String &p_tag) {
	return p_tag.substr(p_tag.find_char('=') + 1);
}

int RichTextLabel3D::_find_unquoted(const String &p_src, char32_t p_chr, int p_from) {
	if (p_from < 0) {
		return -1;
	}

	const int len = p_src.length();
	if (len == 0) {
		return -1;
	}

	const char32_t *src = p_src.get_data();
	bool in_single_quote = false;
	bool in_double_quote = false;
	for (int i = p_from; i < len; i++) {
		if (in_double_quote) {
			if (src[i] == '"') {
				in_double_quote = false;
			}
		} else if (in_single_quote) {
			if (src[i] == '\'') {
				in_single_quote = false;
			}
		} else {
			if (src[i] == '"') {
				in_double_quote = true;
			} else if (src[i] == '\'') {
				in_single_quote = true;
			} else if (src[i] == p_chr) {
				return i;
			}
		}
	}

	return -1;
}

Vector<String> RichTextLabel3D::_split_unquoted(const String &p_src, char32_t p_splitter) {
	Vector<String> ret;

	if (p_src.is_empty()) {
		return ret;
	}

	int from = 0;
	int len = p_src.length();

	while (true) {
		int end = _find_unquoted(p_src, p_splitter, from);
		if (end < 0) {
			end = len;
		}
		if (end > from) {
			ret.push_back(p_src.substr(from, end - from));
		}
		if (end == len) {
			break;
		}

		from = end + 1;
	}

	return ret;
}

void RichTextLabel3D::append_text(const String &p_bbcode) {
	_stop_thread();
	MutexLock data_lock(data_mutex);

	parsing_bbcode.store(true);

	int pos = 0;

	bool in_bold = false;
	bool in_italics = false;
	bool after_list_open_tag = false;
	bool after_list_close_tag = false;

	String bbcode = p_bbcode.replace("\r\n", "\n");

	while (pos <= bbcode.length()) {
		int brk_pos = bbcode.find_char('[', pos);

		if (brk_pos < 0) {
			brk_pos = bbcode.length();
		}

		String txt = brk_pos > pos ? bbcode.substr(pos, brk_pos - pos) : "";

		// Trim the first newline character, it may be added later as needed.
		if (after_list_close_tag || after_list_open_tag) {
			txt = txt.trim_prefix("\n");
		}

		if (brk_pos == bbcode.length()) {
			// For tags that are not properly closed.
			if (txt.is_empty() && after_list_open_tag) {
				txt = "\n";
			}

			if (!txt.is_empty()) {
				add_text(txt);
			}
			break; //nothing else to add
		}

		int brk_end = _find_unquoted(bbcode, ']', brk_pos + 1);

		if (brk_end == -1) {
			//no close, add the rest
			txt += bbcode.substr(brk_pos);
			add_text(txt);
			break;
		}

		String tag = bbcode.substr(brk_pos + 1, brk_end - brk_pos - 1);
		Vector<String> split_tag_block = _split_unquoted(tag, ' ');

		// Find optional parameters.
		String bbcode_name;
		typedef HashMap<String, String> OptionMap;
		OptionMap bbcode_options;
		if (!split_tag_block.is_empty()) {
			bbcode_name = split_tag_block[0];
			for (int i = 1; i < split_tag_block.size(); i++) {
				const String &expr = split_tag_block[i];
				int value_pos = expr.find_char('=');
				if (value_pos > -1) {
					bbcode_options[expr.substr(0, value_pos)] = expr.substr(value_pos + 1).unquote();
				}
			}
		} else {
			bbcode_name = tag;
		}

		// Find main parameter.
		String bbcode_value;
		int main_value_pos = bbcode_name.find_char('=');
		if (main_value_pos > -1) {
			bbcode_value = bbcode_name.substr(main_value_pos + 1);
			bbcode_name = bbcode_name.substr(0, main_value_pos);
		}

		if (tag.begins_with("/") && tag_stack.size()) {
			bool tag_ok = tag_stack.size() && tag_stack.front()->get() == tag.substr(1);

			if (tag_stack.front()->get() == "b") {
				in_bold = false;
			}
			if (tag_stack.front()->get() == "i") {
				in_italics = false;
			}
			if ((tag_stack.front()->get() == "indent") || (tag_stack.front()->get() == "ol") || (tag_stack.front()->get() == "ul")) {
				current_frame->indent_level--;
			}

			if (!tag_ok) {
				txt += "[" + tag;
				add_text(txt);
				after_list_open_tag = false;
				after_list_close_tag = false;
				pos = brk_end;
				continue;
			}

			if (txt.is_empty() && after_list_open_tag) {
				txt = "\n"; // Make empty list have at least one item.
			}
			after_list_open_tag = false;

			if (tag == "/ol" || tag == "/ul") {
				if (!txt.is_empty()) {
					// Make sure text ends with a newline character, that is, the last item
					// will wrap at the end of block.
					if (!txt.ends_with("\n")) {
						txt += "\n";
					}
				} else if (!after_list_close_tag) {
					txt = "\n"; // Make the innermost list item wrap at the end of lists.
				}
				after_list_close_tag = true;
			} else {
				after_list_close_tag = false;
			}

			if (!txt.is_empty()) {
				add_text(txt);
			}

			tag_stack.pop_front();
			pos = brk_end + 1;
			if (tag != "/img" && tag != "/dropcap") {
				pop();
			}
			continue;
		}

		if (tag == "ol" || tag.begins_with("ol ") || tag == "ul" || tag.begins_with("ul ")) {
			if (txt.is_empty() && after_list_open_tag) {
				txt = "\n"; // Make each list have at least one item at the beginning.
			}
			after_list_open_tag = true;
		} else {
			after_list_open_tag = false;
		}
		if (!txt.is_empty()) {
			add_text(txt);
		}
		after_list_close_tag = false;

		if (tag == "b") {
			//use bold font
			in_bold = true;
			if (in_italics) {
				_push_def_font(RTL_BOLD_ITALICS_FONT);
			} else {
				_push_def_font(RTL_BOLD_FONT);
			}
			pos = brk_end + 1;
			tag_stack.push_front(tag);
		} else if (tag == "i") {
			//use italics font
			in_italics = true;
			if (in_bold) {
				_push_def_font(RTL_BOLD_ITALICS_FONT);
			} else {
				_push_def_font(RTL_ITALICS_FONT);
			}
			pos = brk_end + 1;
			tag_stack.push_front(tag);
		} else if (tag == "code") {
			//use monospace font
			_push_def_font(RTL_MONO_FONT);
			pos = brk_end + 1;
			tag_stack.push_front(tag);
		} else if (tag.begins_with("table=")) {
			Vector<String> subtag = _split_unquoted(_get_tag_value(tag), U',');
			_normalize_subtags(subtag);

			int columns = (subtag.is_empty()) ? 1 : subtag[0].to_int();
			if (columns < 1) {
				columns = 1;
			}

			int alignment = INLINE_ALIGNMENT_TOP;
			if (subtag.size() > 2) {
				if (subtag[1] == "top" || subtag[1] == "t") {
					alignment = INLINE_ALIGNMENT_TOP_TO;
				} else if (subtag[1] == "center" || subtag[1] == "c") {
					alignment = INLINE_ALIGNMENT_CENTER_TO;
				} else if (subtag[1] == "baseline" || subtag[1] == "l") {
					alignment = INLINE_ALIGNMENT_BASELINE_TO;
				} else if (subtag[1] == "bottom" || subtag[1] == "b") {
					alignment = INLINE_ALIGNMENT_BOTTOM_TO;
				}
				if (subtag[2] == "top" || subtag[2] == "t") {
					alignment |= INLINE_ALIGNMENT_TO_TOP;
				} else if (subtag[2] == "center" || subtag[2] == "c") {
					alignment |= INLINE_ALIGNMENT_TO_CENTER;
				} else if (subtag[2] == "baseline" || subtag[2] == "l") {
					alignment |= INLINE_ALIGNMENT_TO_BASELINE;
				} else if (subtag[2] == "bottom" || subtag[2] == "b") {
					alignment |= INLINE_ALIGNMENT_TO_BOTTOM;
				}
			} else if (subtag.size() > 1) {
				if (subtag[1] == "top" || subtag[1] == "t") {
					alignment = INLINE_ALIGNMENT_TOP;
				} else if (subtag[1] == "center" || subtag[1] == "c") {
					alignment = INLINE_ALIGNMENT_CENTER;
				} else if (subtag[1] == "bottom" || subtag[1] == "b") {
					alignment = INLINE_ALIGNMENT_BOTTOM;
				}
			}
			int row = -1;
			if (subtag.size() > 3) {
				row = subtag[3].to_int();
			}

			OptionMap::Iterator alt_text_option = bbcode_options.find("name");
			String alt_text;
			if (alt_text_option) {
				alt_text = alt_text_option->value;
			}

			push_table(columns, (InlineAlignment)alignment, row, alt_text);
			pos = brk_end + 1;
			tag_stack.push_front("table");
		} else if (tag == "cell") {
			push_cell();
			pos = brk_end + 1;
			tag_stack.push_front(tag);
		} else if (tag.begins_with("cell=")) {
			int ratio = _get_tag_value(tag).to_int();
			if (ratio < 1) {
				ratio = 1;
			}

			set_table_column_expand(get_current_table_column(), true, ratio);
			push_cell();

			pos = brk_end + 1;
			tag_stack.push_front("cell");
		} else if (tag.begins_with("cell ")) {
			bool shrink = true;
			OptionMap::Iterator shrink_option = bbcode_options.find("shrink");
			if (shrink_option) {
				shrink = (shrink_option->value == "true");
			}

			OptionMap::Iterator expand_option = bbcode_options.find("expand");
			if (expand_option) {
				int ratio = expand_option->value.to_int();
				if (ratio < 1) {
					ratio = 1;
				}
				set_table_column_expand(get_current_table_column(), true, ratio, shrink);
			}

			OptionMap::Iterator img_bg_option = bbcode_options.find("imgbg");
			if (img_bg_option) {
				Ref<Texture2D> texture = ResourceLoader::load(img_bg_option->value, "Texture2D");
				if (texture.is_valid()) {
					Rect2 region;
					OptionMap::Iterator region_option = bbcode_options.find("imgregion");
					if (region_option) {
						Vector<String> region_values = _split_unquoted(region_option->value, U',');
						if (region_values.size() == 4) {
							region.position.x = region_values[0].to_float();
							region.position.y = region_values[1].to_float();
							region.size.x = region_values[2].to_float();
							region.size.y = region_values[3].to_float();
						}
					}
					push_cell_with_image_background(texture, region, Variant(), false, false);
					OptionMap::Iterator img_offset_option = bbcode_options.find("imgoffset");
					if (img_offset_option) {
						Rect2 img_offset;
						Vector<String> img_offset_values = _split_unquoted(img_offset_option->value, U',');
						if (img_offset_values.size() == 4) {
							img_offset.position.x = img_offset_values[0].to_float();
							img_offset.position.y = img_offset_values[1].to_float();
							img_offset.size.x = img_offset_values[2].to_float();
							img_offset.size.y = img_offset_values[3].to_float();
							set_cell_bg_image_offset(img_offset);
						} else if (img_offset_values.size() == 2) {
							img_offset.position.x = img_offset_values[0].to_float();
							img_offset.position.y = img_offset_values[1].to_float();
							set_cell_bg_image_offset(img_offset);
						}
					}
					OptionMap::Iterator keep_aspect_center_option = bbcode_options.find("keepaspect");
					if (keep_aspect_center_option) {
						set_cell_bg_image_keep_aspect_center(keep_aspect_center_option->value == "true");
					}
				} else {
					push_cell();
				}
			} else {
				push_cell();
			}
			OptionMap::Iterator cell_offset_option = bbcode_options.find("offset");
			if (cell_offset_option) {
				Vector3 offset;
				Vector<String> offset_values = _split_unquoted(cell_offset_option->value, U',');
				if (offset_values.size() == 3) {
					offset.x = offset_values[0].to_float();
					offset.y = offset_values[1].to_float();
					offset.z = offset_values[2].to_float();
					set_cell_offset(offset);
				}
			}
			OptionMap::Iterator cell_rotation_option = bbcode_options.find("rotation");
			if (cell_rotation_option) {
				Vector3 rotation;
				Vector<String> rotation_values = _split_unquoted(cell_rotation_option->value, U',');
				if (rotation_values.size() == 3) {
					rotation.x = rotation_values[0].to_float();
					rotation.y = rotation_values[1].to_float();
					rotation.z = rotation_values[2].to_float();
					set_cell_rotation(rotation);
				}
			}
			OptionMap::Iterator cell_radius_option = bbcode_options.find("radius");
			if (cell_radius_option) {
				Vector2 radius;
				Vector<String> radius_values = _split_unquoted(cell_radius_option->value, U',');
				if (radius_values.size() == 2) {
					radius.x = radius_values[0].to_float();
					radius.y = radius_values[1].to_float();
					set_cell_radius(radius);
				}
			}
			OptionMap::Iterator cell_bevel_segments_option = bbcode_options.find("bevel");
			if (cell_bevel_segments_option) {
				int bevel_segments = cell_bevel_segments_option->value.to_int();
				set_cell_bevel_segments(bevel_segments);
			}
			OptionMap::Iterator cell_thickness_option = bbcode_options.find("thickness");
			if (cell_thickness_option) {
				float thickness = cell_thickness_option->value.to_float();
				set_cell_thickness(thickness);
			}
			const Color fallback_color = Color(0, 0, 0, 0);
			OptionMap::Iterator vertical_align_center_option = bbcode_options.find("vcenter");
			if (vertical_align_center_option) {
				set_cell_vertical_align_center(vertical_align_center_option->value == "true");
			}
			OptionMap::Iterator href_option = bbcode_options.find("meta");
			if (href_option) {
				set_cell_meta(href_option->value);
			}
			OptionMap::Iterator border_option = bbcode_options.find("border");
			if (border_option) {
				Color color = Color::from_string(border_option->value, fallback_color);
				set_cell_border_color(color);
			}
			OptionMap::Iterator bg_option = bbcode_options.find("bg");
			if (bg_option) {
				Vector<String> subtag_b = _split_unquoted(bg_option->value, U',');
				_normalize_subtags(subtag_b);

				if (subtag_b.size() == 2) {
					Color color1 = Color::from_string(subtag_b[0], fallback_color);
					Color color2 = Color::from_string(subtag_b[1], fallback_color);
					set_cell_row_background_color(color1, color2);
				}
				if (subtag_b.size() == 1) {
					Color color1 = Color::from_string(bg_option->value, fallback_color);
					set_cell_row_background_color(color1, color1);
				}
			}
			OptionMap::Iterator padding_option = bbcode_options.find("padding");
			if (padding_option) {
				Vector<String> subtag_b = _split_unquoted(padding_option->value, U',');
				_normalize_subtags(subtag_b);

				if (subtag_b.size() == 4) {
					set_cell_padding(Rect2(subtag_b[0].to_float(), subtag_b[1].to_float(), subtag_b[2].to_float(), subtag_b[3].to_float()));
				} else if (subtag_b.size() == 2) {
					set_cell_padding(Rect2(subtag_b[0].to_float(), subtag_b[1].to_float(), subtag_b[0].to_float(), subtag_b[1].to_float()));
				}
			}
			OptionMap::Iterator margin_option = bbcode_options.find("margin");
			if (margin_option) {
				Vector<String> subtag_b = _split_unquoted(margin_option->value, U',');
				_normalize_subtags(subtag_b);

				if (subtag_b.size() == 4) {
					set_cell_margins(Rect2(subtag_b[0].to_float(), subtag_b[1].to_float(), subtag_b[2].to_float(), subtag_b[3].to_float()));
				} else if (subtag_b.size() == 2) {
					set_cell_margins(Rect2(subtag_b[0].to_float(), subtag_b[1].to_float(), 0, 0));
				}
			}
			OptionMap::Iterator size_override_option = bbcode_options.find("size");
			if (size_override_option) {
				Vector<String> subtag_b = _split_unquoted(size_override_option->value, U',');
				_normalize_subtags(subtag_b);

				if (subtag_b.size() == 4) {
					set_cell_size_override(Size2(subtag_b[0].to_float(), subtag_b[1].to_float()), Size2(subtag_b[2].to_float(), subtag_b[3].to_float()));
				} else if (subtag_b.size() == 2) {
					set_cell_size_override(Size2(subtag_b[0].to_float(), subtag_b[1].to_float()), Size2(subtag_b[0].to_float(), subtag_b[1].to_float()));
				}
			}

			pos = brk_end + 1;
			tag_stack.push_front("cell");
		} else if (tag == "u") {
			push_underline();
			pos = brk_end + 1;
			tag_stack.push_front(tag);
		} else if (tag.begins_with("u ")) {
			Color color = Color(0, 0, 0, 0);
			OptionMap::Iterator color_option = bbcode_options.find("color");
			if (color_option) {
				color = Color::from_string(color_option->value, color);
			}

			push_underline(color);
			pos = brk_end + 1;
			tag_stack.push_front("u");
		} else if (tag == "s") {
			push_strikethrough();
			pos = brk_end + 1;
			tag_stack.push_front(tag);
		} else if (tag.begins_with("s ")) {
			Color color = Color(0, 0, 0, 0);
			OptionMap::Iterator color_option = bbcode_options.find("color");
			if (color_option) {
				color = Color::from_string(color_option->value, color);
			}

			push_strikethrough(color);
			pos = brk_end + 1;
			tag_stack.push_front("s");
		} else if (tag.begins_with("char=")) {
			int32_t char_code = _get_tag_value(tag).hex_to_int();
			add_text(String::chr(char_code));
			pos = brk_end + 1;
		} else if (tag == "lb") {
			add_text("[");
			pos = brk_end + 1;
		} else if (tag == "rb") {
			add_text("]");
			pos = brk_end + 1;
		} else if (tag == "lrm") {
			add_text(String::chr(0x200E));
			pos = brk_end + 1;
		} else if (tag == "rlm") {
			add_text(String::chr(0x200F));
			pos = brk_end + 1;
		} else if (tag == "lre") {
			add_text(String::chr(0x202A));
			pos = brk_end + 1;
		} else if (tag == "rle") {
			add_text(String::chr(0x202B));
			pos = brk_end + 1;
		} else if (tag == "lro") {
			add_text(String::chr(0x202D));
			pos = brk_end + 1;
		} else if (tag == "rlo") {
			add_text(String::chr(0x202E));
			pos = brk_end + 1;
		} else if (tag == "pdf") {
			add_text(String::chr(0x202C));
			pos = brk_end + 1;
		} else if (tag == "alm") {
			add_text(String::chr(0x061c));
			pos = brk_end + 1;
		} else if (tag == "lri") {
			add_text(String::chr(0x2066));
			pos = brk_end + 1;
		} else if (tag == "rli") {
			add_text(String::chr(0x2027));
			pos = brk_end + 1;
		} else if (tag == "fsi") {
			add_text(String::chr(0x2068));
			pos = brk_end + 1;
		} else if (tag == "pdi") {
			add_text(String::chr(0x2069));
			pos = brk_end + 1;
		} else if (tag == "zwj") {
			add_text(String::chr(0x200D));
			pos = brk_end + 1;
		} else if (tag == "zwnj") {
			add_text(String::chr(0x200C));
			pos = brk_end + 1;
		} else if (tag == "wj") {
			add_text(String::chr(0x2060));
			pos = brk_end + 1;
		} else if (tag == "shy") {
			add_text(String::chr(0x00AD));
			pos = brk_end + 1;
		} else if (tag == "center") {
			push_paragraph(HORIZONTAL_ALIGNMENT_CENTER, Control::TEXT_DIRECTION_LTR, language, st_parser, default_jst_flags, default_tab_stops);
			pos = brk_end + 1;
			tag_stack.push_front(tag);
		} else if (tag == "fill") {
			push_paragraph(HORIZONTAL_ALIGNMENT_FILL, Control::TEXT_DIRECTION_LTR, language, st_parser, default_jst_flags, default_tab_stops);
			pos = brk_end + 1;
			tag_stack.push_front(tag);
		} else if (tag == "left") {
			push_paragraph(HORIZONTAL_ALIGNMENT_LEFT, Control::TEXT_DIRECTION_LTR, language, st_parser, default_jst_flags, default_tab_stops);
			pos = brk_end + 1;
			tag_stack.push_front(tag);
		} else if (tag == "right") {
			push_paragraph(HORIZONTAL_ALIGNMENT_RIGHT, Control::TEXT_DIRECTION_LTR, language, st_parser, default_jst_flags, default_tab_stops);
			pos = brk_end + 1;
			tag_stack.push_front(tag);
		} else if (tag == "ul") {
			current_frame->indent_level++;
			push_list(current_frame->indent_level, LIST_DOTS, false);
			pos = brk_end + 1;
			tag_stack.push_front(tag);
		} else if (tag.begins_with("ul bullet=")) {
			String bullet = _get_tag_value(tag);
			current_frame->indent_level++;
			push_list(current_frame->indent_level, LIST_DOTS, false, bullet);
			pos = brk_end + 1;
			tag_stack.push_front("ul");
		} else if ((tag == "ol") || (tag == "ol type=1")) {
			current_frame->indent_level++;
			push_list(current_frame->indent_level, LIST_NUMBERS, false);
			pos = brk_end + 1;
			tag_stack.push_front("ol");
		} else if (tag == "ol type=a") {
			current_frame->indent_level++;
			push_list(current_frame->indent_level, LIST_LETTERS, false);
			pos = brk_end + 1;
			tag_stack.push_front("ol");
		} else if (tag == "ol type=A") {
			current_frame->indent_level++;
			push_list(current_frame->indent_level, LIST_LETTERS, true);
			pos = brk_end + 1;
			tag_stack.push_front("ol");
		} else if (tag == "ol type=i") {
			current_frame->indent_level++;
			push_list(current_frame->indent_level, LIST_ROMAN, false);
			pos = brk_end + 1;
			tag_stack.push_front("ol");
		} else if (tag == "ol type=I") {
			current_frame->indent_level++;
			push_list(current_frame->indent_level, LIST_ROMAN, true);
			pos = brk_end + 1;
			tag_stack.push_front("ol");
		} else if (tag == "indent") {
			current_frame->indent_level++;
			push_indent(current_frame->indent_level);
			pos = brk_end + 1;
			tag_stack.push_front(tag);
		} else if (tag.begins_with("lang=")) {
			String lang = _get_tag_value(tag).unquote();
			push_language(lang);
			pos = brk_end + 1;
			tag_stack.push_front("lang");
		} else if (tag == "br") {
			add_text("\r");
			pos = brk_end + 1;
		} else if (tag == "p") {
			push_paragraph(HORIZONTAL_ALIGNMENT_LEFT);
			pos = brk_end + 1;
			tag_stack.push_front("p");
		} else if (tag.begins_with("p ")) {
			HorizontalAlignment alignment = HORIZONTAL_ALIGNMENT_LEFT;
			Control::TextDirection dir = Control::TEXT_DIRECTION_INHERITED;
			String lang = language;
			PackedFloat32Array tab_stops = default_tab_stops;
			TextServer::StructuredTextParser st_parser_type = TextServer::STRUCTURED_TEXT_DEFAULT;
			BitField<TextServer::JustificationFlag> jst_flags = default_jst_flags;

			OptionMap::Iterator justification_flags_option = bbcode_options.find("justification_flags");
			if (!justification_flags_option) {
				justification_flags_option = bbcode_options.find("jst");
			}
			if (justification_flags_option) {
				Vector<String> subtag_b = _split_unquoted(justification_flags_option->value, U',');
				jst_flags = 0; // Clear flags.
				for (const String &E : subtag_b) {
					if (E == "kashida" || E == "k") {
						jst_flags.set_flag(TextServer::JUSTIFICATION_KASHIDA);
					} else if (E == "word" || E == "w") {
						jst_flags.set_flag(TextServer::JUSTIFICATION_WORD_BOUND);
					} else if (E == "trim" || E == "tr") {
						jst_flags.set_flag(TextServer::JUSTIFICATION_TRIM_EDGE_SPACES);
					} else if (E == "after_last_tab" || E == "lt") {
						jst_flags.set_flag(TextServer::JUSTIFICATION_AFTER_LAST_TAB);
					} else if (E == "skip_last" || E == "sl") {
						jst_flags.set_flag(TextServer::JUSTIFICATION_SKIP_LAST_LINE);
					} else if (E == "skip_last_with_chars" || E == "sv") {
						jst_flags.set_flag(TextServer::JUSTIFICATION_SKIP_LAST_LINE_WITH_VISIBLE_CHARS);
					} else if (E == "do_not_skip_single" || E == "ns") {
						jst_flags.set_flag(TextServer::JUSTIFICATION_DO_NOT_SKIP_SINGLE_LINE);
					}
				}
			}
			OptionMap::Iterator tab_stops_option = bbcode_options.find("tab_stops");
			if (tab_stops_option) {
				Vector<String> splitters;
				splitters.push_back(",");
				splitters.push_back(";");
				tab_stops = tab_stops_option->value.split_floats_mk(splitters);
			}
			OptionMap::Iterator align_option = bbcode_options.find("align");
			if (align_option) {
				if (align_option->value == "l" || align_option->value == "left") {
					alignment = HORIZONTAL_ALIGNMENT_LEFT;
				} else if (align_option->value == "c" || align_option->value == "center") {
					alignment = HORIZONTAL_ALIGNMENT_CENTER;
				} else if (align_option->value == "r" || align_option->value == "right") {
					alignment = HORIZONTAL_ALIGNMENT_RIGHT;
				} else if (align_option->value == "f" || align_option->value == "fill") {
					alignment = HORIZONTAL_ALIGNMENT_FILL;
				}
			}
			OptionMap::Iterator direction_option = bbcode_options.find("direction");
			if (!direction_option) {
				direction_option = bbcode_options.find("dir");
			}
			if (direction_option) {
				if (direction_option->value == "a" || direction_option->value == "auto") {
					dir = Control::TEXT_DIRECTION_AUTO;
				} else if (direction_option->value == "l" || direction_option->value == "ltr") {
					dir = Control::TEXT_DIRECTION_LTR;
				} else if (direction_option->value == "r" || direction_option->value == "rtl") {
					dir = Control::TEXT_DIRECTION_RTL;
				}
			}
			OptionMap::Iterator language_option = bbcode_options.find("language");
			if (!language_option) {
				language_option = bbcode_options.find("lang");
			}
			if (language_option) {
				lang = language_option->value;
			}
			OptionMap::Iterator bidi_override_option = bbcode_options.find("bidi_override");
			if (!bidi_override_option) {
				bidi_override_option = bbcode_options.find("st");
			}
			if (bidi_override_option) {
				if (bidi_override_option->value == "d" || bidi_override_option->value == "default") {
					st_parser_type = TextServer::STRUCTURED_TEXT_DEFAULT;
				} else if (bidi_override_option->value == "u" || bidi_override_option->value == "uri") {
					st_parser_type = TextServer::STRUCTURED_TEXT_URI;
				} else if (bidi_override_option->value == "f" || bidi_override_option->value == "file") {
					st_parser_type = TextServer::STRUCTURED_TEXT_FILE;
				} else if (bidi_override_option->value == "e" || bidi_override_option->value == "email") {
					st_parser_type = TextServer::STRUCTURED_TEXT_EMAIL;
				} else if (bidi_override_option->value == "l" || bidi_override_option->value == "list") {
					st_parser_type = TextServer::STRUCTURED_TEXT_LIST;
				} else if (bidi_override_option->value == "n" || bidi_override_option->value == "gdscript") {
					st_parser_type = TextServer::STRUCTURED_TEXT_GDSCRIPT;
				} else if (bidi_override_option->value == "c" || bidi_override_option->value == "custom") {
					st_parser_type = TextServer::STRUCTURED_TEXT_CUSTOM;
				}
			}

			push_paragraph(alignment, dir, lang, st_parser_type, jst_flags, tab_stops);
			pos = brk_end + 1;
			tag_stack.push_front("p");
		} else if (tag == "url") {
			int end = bbcode.find_char('[', brk_end);
			if (end == -1) {
				end = bbcode.length();
			}
			String url = bbcode.substr(brk_end + 1, end - brk_end - 1).unquote();
			push_meta(url, META_UNDERLINE_ALWAYS);

			pos = brk_end + 1;
			tag_stack.push_front(tag);

		} else if (tag.begins_with("url ")) {
			String url;
			MetaUnderline underline = META_UNDERLINE_ALWAYS;
			String tooltip;

			OptionMap::Iterator underline_option = bbcode_options.find("underline");
			if (underline_option) {
				if (underline_option->value == "never") {
					underline = META_UNDERLINE_NEVER;
				} else if (underline_option->value == "always") {
					underline = META_UNDERLINE_ALWAYS;
				} else if (underline_option->value == "hover") {
					underline = META_UNDERLINE_ON_HOVER;
				}
			}
			OptionMap::Iterator tooltip_option = bbcode_options.find("tooltip");
			if (tooltip_option) {
				tooltip = tooltip_option->value;
			}
			OptionMap::Iterator href_option = bbcode_options.find("href");
			if (href_option) {
				url = href_option->value;
			}

			push_meta(url, underline, tooltip);

			pos = brk_end + 1;
			tag_stack.push_front("url");
		} else if (tag.begins_with("url=")) {
			String url = _get_tag_value(tag).unquote();
			push_meta(url, META_UNDERLINE_ALWAYS);
			pos = brk_end + 1;
			tag_stack.push_front("url");
		} else if (tag.begins_with("hint=")) {
			String description = _get_tag_value(tag).unquote();
			push_hint(description);
			pos = brk_end + 1;
			tag_stack.push_front("hint");
		} else if (tag.begins_with("hr")) {
			HorizontalAlignment alignment = HORIZONTAL_ALIGNMENT_CENTER;
			OptionMap::Iterator align_option = bbcode_options.find("align");
			if (align_option) {
				if (align_option->value == "l" || align_option->value == "left") {
					alignment = HORIZONTAL_ALIGNMENT_LEFT;
				} else if (align_option->value == "c" || align_option->value == "center") {
					alignment = HORIZONTAL_ALIGNMENT_CENTER;
				} else if (align_option->value == "r" || align_option->value == "right") {
					alignment = HORIZONTAL_ALIGNMENT_RIGHT;
				}
			}

			Color color = modulate;
			OptionMap::Iterator color_option = bbcode_options.find("color");
			if (color_option) {
				color = Color::from_string(color_option->value, color);
			}
			int width = 90;
			bool width_in_percent = true;
			OptionMap::Iterator width_option = bbcode_options.find("width");
			if (width_option) {
				width = width_option->value.to_int();
				width_in_percent = (width_option->value.ends_with("%"));
			}

			int height = 2;
			bool height_in_percent = false;
			OptionMap::Iterator height_option = bbcode_options.find("height");
			if (height_option) {
				height = height_option->value.to_int();
				height_in_percent = (height_option->value.ends_with("%"));
			}

			add_hr(width, height, color, alignment, width_in_percent, height_in_percent);

			pos = brk_end + 1;
		} else if (tag.begins_with("img")) {
			int alignment = INLINE_ALIGNMENT_CENTER;
			if (tag.begins_with("img=")) {
				Vector<String> subtag = _split_unquoted(_get_tag_value(tag), U',');
				_normalize_subtags(subtag);

				if (subtag.size() > 1) {
					if (subtag[0] == "top" || subtag[0] == "t") {
						alignment = INLINE_ALIGNMENT_TOP_TO;
					} else if (subtag[0] == "center" || subtag[0] == "c") {
						alignment = INLINE_ALIGNMENT_CENTER_TO;
					} else if (subtag[0] == "bottom" || subtag[0] == "b") {
						alignment = INLINE_ALIGNMENT_BOTTOM_TO;
					}
					if (subtag[1] == "top" || subtag[1] == "t") {
						alignment |= INLINE_ALIGNMENT_TO_TOP;
					} else if (subtag[1] == "center" || subtag[1] == "c") {
						alignment |= INLINE_ALIGNMENT_TO_CENTER;
					} else if (subtag[1] == "baseline" || subtag[1] == "l") {
						alignment |= INLINE_ALIGNMENT_TO_BASELINE;
					} else if (subtag[1] == "bottom" || subtag[1] == "b") {
						alignment |= INLINE_ALIGNMENT_TO_BOTTOM;
					}
				} else if (subtag.size() > 0) {
					if (subtag[0] == "top" || subtag[0] == "t") {
						alignment = INLINE_ALIGNMENT_TOP;
					} else if (subtag[0] == "center" || subtag[0] == "c") {
						alignment = INLINE_ALIGNMENT_CENTER;
					} else if (subtag[0] == "bottom" || subtag[0] == "b") {
						alignment = INLINE_ALIGNMENT_BOTTOM;
					}
				}
			}

			int end = bbcode.find_char('[', brk_end);
			if (end == -1) {
				end = bbcode.length();
			}

			String image = bbcode.substr(brk_end + 1, end - brk_end - 1);
			String alt_text;

			Ref<Texture2D> texture = ResourceLoader::load(image, "Texture2D");
			if (texture.is_valid()) {
				Rect2 region;
				OptionMap::Iterator region_option = bbcode_options.find("region");
				if (region_option) {
					Vector<String> region_values = _split_unquoted(region_option->value, U',');
					if (region_values.size() == 4) {
						region.position.x = region_values[0].to_float();
						region.position.y = region_values[1].to_float();
						region.size.x = region_values[2].to_float();
						region.size.y = region_values[3].to_float();
					}
				}

				Color color = Color(1.0, 1.0, 1.0, 1.0);
				OptionMap::Iterator color_option = bbcode_options.find("color");
				if (color_option) {
					color = Color::from_string(color_option->value, color);
				}

				OptionMap::Iterator alt_text_option = bbcode_options.find("alt");
				if (alt_text_option) {
					alt_text = alt_text_option->value;
				}

				int width = 0;
				int height = 0;
				bool pad = false;
				String tooltip;
				bool width_in_percent = false;
				bool height_in_percent = false;
				if (!bbcode_value.is_empty()) {
					int sep = bbcode_value.find_char('x');
					if (sep == -1) {
						width = bbcode_value.to_int();
					} else {
						width = bbcode_value.substr(0, sep).to_int();
						height = bbcode_value.substr(sep + 1).to_int();
					}
				} else {
					OptionMap::Iterator align_option = bbcode_options.find("align");
					if (align_option) {
						Vector<String> subtag = _split_unquoted(align_option->value, U',');
						_normalize_subtags(subtag);

						if (subtag.size() > 1) {
							if (subtag[0] == "top" || subtag[0] == "t") {
								alignment = INLINE_ALIGNMENT_TOP_TO;
							} else if (subtag[0] == "center" || subtag[0] == "c") {
								alignment = INLINE_ALIGNMENT_CENTER_TO;
							} else if (subtag[0] == "bottom" || subtag[0] == "b") {
								alignment = INLINE_ALIGNMENT_BOTTOM_TO;
							}
							if (subtag[1] == "top" || subtag[1] == "t") {
								alignment |= INLINE_ALIGNMENT_TO_TOP;
							} else if (subtag[1] == "center" || subtag[1] == "c") {
								alignment |= INLINE_ALIGNMENT_TO_CENTER;
							} else if (subtag[1] == "baseline" || subtag[1] == "l") {
								alignment |= INLINE_ALIGNMENT_TO_BASELINE;
							} else if (subtag[1] == "bottom" || subtag[1] == "b") {
								alignment |= INLINE_ALIGNMENT_TO_BOTTOM;
							}
						} else if (subtag.size() > 0) {
							if (subtag[0] == "top" || subtag[0] == "t") {
								alignment = INLINE_ALIGNMENT_TOP;
							} else if (subtag[0] == "center" || subtag[0] == "c") {
								alignment = INLINE_ALIGNMENT_CENTER;
							} else if (subtag[0] == "bottom" || subtag[0] == "b") {
								alignment = INLINE_ALIGNMENT_BOTTOM;
							}
						}
					}
					OptionMap::Iterator width_option = bbcode_options.find("width");
					if (width_option) {
						width = width_option->value.to_int();
						if (width_option->value.ends_with("%")) {
							width_in_percent = true;
						}
					}

					OptionMap::Iterator height_option = bbcode_options.find("height");
					if (height_option) {
						height = height_option->value.to_int();
						if (height_option->value.ends_with("%")) {
							height_in_percent = true;
						}
					}

					OptionMap::Iterator tooltip_option = bbcode_options.find("tooltip");
					if (tooltip_option) {
						tooltip = tooltip_option->value;
					}

					OptionMap::Iterator pad_option = bbcode_options.find("pad");
					if (pad_option) {
						pad = (pad_option->value == "true");
					}
				}

				add_image(texture, width, height, color, (InlineAlignment)alignment, region, Variant(), pad, tooltip, width_in_percent, height_in_percent, alt_text);
			}

			pos = end;
			tag_stack.push_front(bbcode_name);
		} else if (tag.begins_with("color=")) {
			String color_str = _get_tag_value(tag).unquote();
			Color color = Color::from_string(color_str, modulate);
			push_color(color);
			pos = brk_end + 1;
			tag_stack.push_front("color");

		} else if (tag.begins_with("outline_color=")) {
			String color_str = _get_tag_value(tag).unquote();
			Color color = Color::from_string(color_str, outline_modulate);
			push_outline_color(color);
			pos = brk_end + 1;
			tag_stack.push_front("outline_color");
		} else if (tag.begins_with("font_size=")) {
			int fnt_size = _get_tag_value(tag).to_int();
			if (tag.length() > 10 && (tag[10] == static_cast<char32_t>('+') || tag[10] == static_cast<char32_t>('-'))) {
				int add_font_size = tag.substr(11, tag.length()).to_int();
				if (tag[10] == static_cast<char32_t>('+')) {
					fnt_size = font_size + add_font_size;
				} else {
					fnt_size = font_size - add_font_size;
				}
			}
			push_font_size(fnt_size);
			pos = brk_end + 1;
			tag_stack.push_front("font_size");
		} else if (tag.begins_with("opentype_features=") || tag.begins_with("otf=")) {
			int value_pos = tag.find_char('=');
			String fnt_ftr = tag.substr(value_pos + 1);
			Vector<String> subtag = fnt_ftr.split(",");
			_normalize_subtags(subtag);

			if (subtag.size() > 0) {
				Ref<Font> font = _get_font_or_default();
				DefaultFont def_font = RTL_NORMAL_FONT;

				ItemFont *font_it = _find_font(current);
				if (font_it) {
					if (font_it->font.is_valid()) {
						font = font_it->font;
						def_font = font_it->def_font;
					}
				}
				Dictionary features;
				for (int i = 0; i < subtag.size(); i++) {
					Vector<String> subtag_a = subtag[i].split("=");
					_normalize_subtags(subtag_a);

					if (subtag_a.size() == 2) {
						features[TS->name_to_tag(subtag_a[0])] = subtag_a[1].to_int();
					} else if (subtag_a.size() == 1) {
						features[TS->name_to_tag(subtag_a[0])] = 1;
					}
				}

				Ref<FontVariation> fc;
				fc.instantiate();

				fc->set_base_font(font);
				fc->set_opentype_features(features);

				if (def_font != RTL_CUSTOM_FONT) {
					_push_def_font_var(def_font, fc);
				} else {
					push_font(fc);
				}
			}
			pos = brk_end + 1;
			tag_stack.push_front(tag.substr(0, value_pos));
		} else if (tag.begins_with("font=")) {
			String fnt = _get_tag_value(tag).unquote();

			Ref<Font> fc = ResourceLoader::load(fnt, "Font");
			if (fc.is_valid()) {
				push_font(fc);
			}

			pos = brk_end + 1;
			tag_stack.push_front("font");

		} else if (tag.begins_with("font ")) {
			Ref<Font> font = _get_font_or_default();
			DefaultFont def_font = RTL_NORMAL_FONT;
			int fnt_size = -1;

			ItemFont *font_it = _find_font(current);
			if (font_it) {
				if (font_it->font.is_valid()) {
					font = font_it->font;
					def_font = font_it->def_font;
				}
			}

			Ref<FontVariation> fc;
			fc.instantiate();

			OptionMap::Iterator name_option = bbcode_options.find("name");
			if (!name_option) {
				name_option = bbcode_options.find("n");
			}
			if (name_option) {
				const String &fnt = name_option->value;
				Ref<Font> font_data = ResourceLoader::load(fnt, "Font");
				if (font_data.is_valid()) {
					font = font_data;
					def_font = RTL_CUSTOM_FONT;
				}
			}
			OptionMap::Iterator size_option = bbcode_options.find("size");
			if (!size_option) {
				size_option = bbcode_options.find("s");
			}
			if (size_option) {
				fnt_size = size_option->value.to_int();
			}
			OptionMap::Iterator glyph_spacing_option = bbcode_options.find("glyph_spacing");
			if (!glyph_spacing_option) {
				glyph_spacing_option = bbcode_options.find("gl");
			}
			if (glyph_spacing_option) {
				int spacing = glyph_spacing_option->value.to_int();
				fc->set_spacing(TextServer::SPACING_GLYPH, spacing);
			}
			OptionMap::Iterator space_spacing_option = bbcode_options.find("space_spacing");
			if (!space_spacing_option) {
				space_spacing_option = bbcode_options.find("sp");
			}
			if (space_spacing_option) {
				int spacing = space_spacing_option->value.to_int();
				fc->set_spacing(TextServer::SPACING_SPACE, spacing);
			}
			OptionMap::Iterator top_spacing_option = bbcode_options.find("top_spacing");
			if (!top_spacing_option) {
				top_spacing_option = bbcode_options.find("top");
			}
			if (top_spacing_option) {
				int spacing = top_spacing_option->value.to_int();
				fc->set_spacing(TextServer::SPACING_TOP, spacing);
			}
			OptionMap::Iterator bottom_spacing_option = bbcode_options.find("bottom_spacing");
			if (!bottom_spacing_option) {
				bottom_spacing_option = bbcode_options.find("bt");
			}
			if (bottom_spacing_option) {
				int spacing = bottom_spacing_option->value.to_int();
				fc->set_spacing(TextServer::SPACING_BOTTOM, spacing);
			}
			OptionMap::Iterator embolden_option = bbcode_options.find("embolden");
			if (!embolden_option) {
				embolden_option = bbcode_options.find("emb");
			}
			if (embolden_option) {
				float emb = embolden_option->value.to_float();
				fc->set_variation_embolden(emb);
			}
			OptionMap::Iterator face_index_option = bbcode_options.find("face_index");
			if (!face_index_option) {
				face_index_option = bbcode_options.find("fi");
			}
			if (face_index_option) {
				int fi = face_index_option->value.to_int();
				fc->set_variation_face_index(fi);
			}
			OptionMap::Iterator slant_option = bbcode_options.find("slant");
			if (!slant_option) {
				slant_option = bbcode_options.find("sln");
			}
			if (slant_option) {
				float slant = slant_option->value.to_float();
				fc->set_variation_transform(Transform2D(1.0, slant, 0.0, 1.0, 0.0, 0.0));
			}
			OptionMap::Iterator opentype_variation_option = bbcode_options.find("opentype_variation");
			if (!opentype_variation_option) {
				opentype_variation_option = bbcode_options.find("otv");
			}
			if (opentype_variation_option) {
				Dictionary variations;
				if (!opentype_variation_option->value.is_empty()) {
					Vector<String> variation_tags = opentype_variation_option->value.split(",");
					for (int j = 0; j < variation_tags.size(); j++) {
						Vector<String> subtag_b = variation_tags[j].split("=");
						_normalize_subtags(subtag_b);

						if (subtag_b.size() == 2) {
							variations[TS->name_to_tag(subtag_b[0])] = subtag_b[1].to_float();
						}
					}
					fc->set_variation_opentype(variations);
				}
			}
			OptionMap::Iterator opentype_features_option = bbcode_options.find("opentype_features");
			if (!opentype_features_option) {
				opentype_features_option = bbcode_options.find("otf");
			}
			if (opentype_features_option) {
				Dictionary features;
				if (!opentype_features_option->value.is_empty()) {
					Vector<String> feature_tags = opentype_features_option->value.split(",");
					for (int j = 0; j < feature_tags.size(); j++) {
						Vector<String> subtag_b = feature_tags[j].split("=");
						_normalize_subtags(subtag_b);

						if (subtag_b.size() == 2) {
							features[TS->name_to_tag(subtag_b[0])] = subtag_b[1].to_float();
						} else if (subtag_b.size() == 1) {
							features[TS->name_to_tag(subtag_b[0])] = 1;
						}
					}
					fc->set_opentype_features(features);
				}
			}

			fc->set_base_font(font);

			if (def_font != RTL_CUSTOM_FONT) {
				_push_def_font_var(def_font, fc, fnt_size);
			} else {
				push_font(fc, fnt_size);
			}

			pos = brk_end + 1;
			tag_stack.push_front("font");

		} else if (tag.begins_with("outline_size=")) {
			int fnt_size = _get_tag_value(tag).to_int();
			if (fnt_size >= 0) {
				push_outline_size(fnt_size);
			}
			pos = brk_end + 1;
			tag_stack.push_front("outline_size");

		} else if (tag.begins_with("bgcolor=")) {
			String color_str = _get_tag_value(tag).unquote();
			Color color = Color::from_string(color_str, modulate);

			push_bgcolor(color);
			pos = brk_end + 1;
			tag_stack.push_front("bgcolor");

		} else if (tag.begins_with("fgcolor=")) {
			String color_str = _get_tag_value(tag).unquote();
			Color color = Color::from_string(color_str, modulate);

			push_fgcolor(color);
			pos = brk_end + 1;
			tag_stack.push_front("fgcolor");

		} else {
			Vector<String> &expr = split_tag_block;
			if (expr.is_empty()) {
				add_text("[");
				pos = brk_pos + 1;
			}
		}
	}

	parsing_bbcode.store(false);
}

int RichTextLabel3D::get_paragraph_count() const {
	return main->lines.size();
}

int RichTextLabel3D::get_visible_paragraph_count() const {
	if (!is_visible()) {
		return 0;
	}

	const_cast<RichTextLabel3D *>(this)->_validate_line_caches();
	return visible_paragraph_count;
}

float RichTextLabel3D::get_line_offset(int p_line) {
	_validate_line_caches();

	int line_count = 0;
	int to_line = main->first_invalid_line.load();
	for (int i = 0; i < to_line; i++) {
		MutexLock lock(main->lines[i].text_buf->get_mutex());
		if ((line_count <= p_line) && (p_line <= line_count + main->lines[i].text_buf->get_line_count())) {
			float line_offset = 0.f;
			for (int j = 0; j < p_line - line_count; j++) {
				line_offset += main->lines[i].text_buf->get_line_ascent(j) + main->lines[i].text_buf->get_line_descent(j) + theme_cache.line_separation;
			}
			return main->lines[i].offset.y + line_offset;
		}
		line_count += main->lines[i].text_buf->get_line_count();
	}
	return 0;
}

float RichTextLabel3D::get_paragraph_offset(int p_paragraph) {
	_validate_line_caches();

	int to_line = main->first_invalid_line.load();
	if (0 <= p_paragraph && p_paragraph < to_line) {
		return main->lines[p_paragraph].offset.y;
	}
	return 0;
}

int RichTextLabel3D::get_line_count() const {
	const_cast<RichTextLabel3D *>(this)->_validate_line_caches();

	int line_count = 0;
	int to_line = main->first_invalid_line.load();
	for (int i = 0; i < to_line; i++) {
		MutexLock lock(main->lines[i].text_buf->get_mutex());
		line_count += main->lines[i].text_buf->get_line_count();
	}
	return line_count;
}

Vector2i RichTextLabel3D::get_line_range(int p_line) {
	const_cast<RichTextLabel3D *>(this)->_validate_line_caches();

	int line_count = 0;
	int to_line = main->first_invalid_line.load();
	for (int i = 0; i < to_line; i++) {
		MutexLock lock(main->lines[i].text_buf->get_mutex());
		int lc = main->lines[i].text_buf->get_line_count();

		if (p_line < line_count + lc) {
			Vector2i char_offset = Vector2i(main->lines[i].char_offset, main->lines[i].char_offset);
			Vector2i line_range = main->lines[i].text_buf->get_line_range(p_line - line_count);
			return char_offset + line_range;
		}

		line_count += lc;
	}
	return Vector2i();
}

int RichTextLabel3D::get_visible_line_count() const {
	if (!is_visible()) {
		return 0;
	}
	const_cast<RichTextLabel3D *>(this)->_validate_line_caches();

	return visible_line_count;
}

bool RichTextLabel3D::_search_table(ItemTable *p_table, List<Item *>::Element *p_from, const String &p_string, bool p_reverse_search) {
	List<Item *>::Element *E = p_from;
	while (E != nullptr) {
		ERR_CONTINUE(E->get()->type != ITEM_FRAME); // Children should all be frames.
		ItemFrame *frame = static_cast<ItemFrame *>(E->get());
		if (p_reverse_search) {
			for (int i = (int)frame->lines.size() - 1; i >= 0; i--) {
				if (_search_line(frame, i, p_string, -1, p_reverse_search)) {
					return true;
				}
			}
		} else {
			for (int i = 0; i < (int)frame->lines.size(); i++) {
				if (_search_line(frame, i, p_string, 0, p_reverse_search)) {
					return true;
				}
			}
		}
		E = p_reverse_search ? E->prev() : E->next();
	}
	return false;
}

bool RichTextLabel3D::_search_line(ItemFrame *p_frame, int p_line, const String &p_string, int p_char_idx, bool p_reverse_search) {
	ERR_FAIL_NULL_V(p_frame, false);
	ERR_FAIL_COND_V(p_line < 0 || p_line >= (int)p_frame->lines.size(), false);

	Line &l = p_frame->lines[p_line];

	String txt;
	Item *it_to = (p_line + 1 < (int)p_frame->lines.size()) ? p_frame->lines[p_line + 1].from : nullptr;
	for (Item *it = l.from; it && it != it_to; it = _get_next_item(it)) {
		switch (it->type) {
			case ITEM_NEWLINE: {
				txt += "\n";
			} break;
			case ITEM_TEXT: {
				ItemText *t = static_cast<ItemText *>(it);
				txt += t->text;
			} break;
			case ITEM_IMAGE: {
				txt += " ";
			} break;
			case ITEM_TABLE: {
				ItemTable *table = static_cast<ItemTable *>(it);
				List<Item *>::Element *E = p_reverse_search ? table->subitems.back() : table->subitems.front();
				if (_search_table(table, E, p_string, p_reverse_search)) {
					return true;
				}
			} break;
			default:
				break;
		}
	}

	int sp = -1;
	if (p_reverse_search) {
		sp = txt.rfindn(p_string, p_char_idx);
	} else {
		sp = txt.findn(p_string, p_char_idx);
	}

	if (sp != -1) {
		selection.from_frame = p_frame;
		selection.from_line = p_line;
		selection.from_item = _get_item_at_pos(l.from, it_to, sp);
		selection.from_char = sp;
		selection.to_frame = p_frame;
		selection.to_line = p_line;
		selection.to_item = _get_item_at_pos(l.from, it_to, sp + p_string.length());
		selection.to_char = sp + p_string.length();
		selection.active = true;
		queue_accessibility_update();
		return true;
	}

	return false;
}

String RichTextLabel3D::_get_line_text(ItemFrame *p_frame, int p_line, Selection p_selection) const {
	String txt;

	ERR_FAIL_NULL_V(p_frame, txt);
	ERR_FAIL_COND_V(p_line < 0 || p_line >= (int)p_frame->lines.size(), txt);

	Line &l = p_frame->lines[p_line];

	Item *it_to = (p_line + 1 < (int)p_frame->lines.size()) ? p_frame->lines[p_line + 1].from : nullptr;
	int end_idx = 0;
	if (it_to != nullptr) {
		end_idx = it_to->index;
	} else {
		for (Item *it = l.from; it; it = _get_next_item(it)) {
			end_idx = it->index + 1;
		}
	}
	for (Item *it = l.from; it && it != it_to; it = _get_next_item(it)) {
		if (it->type == ITEM_TABLE) {
			ItemTable *table = static_cast<ItemTable *>(it);
			for (Item *E : table->subitems) {
				ERR_CONTINUE(E->type != ITEM_FRAME); // Children should all be frames.
				ItemFrame *frame = static_cast<ItemFrame *>(E);
				for (int i = 0; i < (int)frame->lines.size(); i++) {
					txt += _get_line_text(frame, i, p_selection);
				}
			}
		}
		if ((p_selection.to_item != nullptr) && (p_selection.to_item->index < l.from->index)) {
			continue;
		}
		if ((p_selection.from_item != nullptr) && (p_selection.from_item->index >= end_idx)) {
			continue;
		}
		if (it->type == ITEM_DROPCAP) {
			const ItemDropcap *dc = static_cast<ItemDropcap *>(it);
			txt += dc->text;
		} else if (it->type == ITEM_TEXT) {
			const ItemText *t = static_cast<ItemText *>(it);
			txt += t->text;
		} else if (it->type == ITEM_NEWLINE) {
			txt += "\n";
		} else if (it->type == ITEM_IMAGE) {
			txt += " ";
		}
	}
	if ((l.from != nullptr) && (p_frame == p_selection.to_frame) && (p_selection.to_item != nullptr) && (p_selection.to_item->index >= l.from->index) && (p_selection.to_item->index < end_idx)) {
		txt = txt.substr(0, p_selection.to_char);
	}
	if ((l.from != nullptr) && (p_frame == p_selection.from_frame) && (p_selection.from_item != nullptr) && (p_selection.from_item->index >= l.from->index) && (p_selection.from_item->index < end_idx)) {
		txt = txt.substr(p_selection.from_char);
	}
	return txt;
}

void RichTextLabel3D::set_text(const String &p_bbcode) {
	// Allow clearing the tag stack.
	if (!p_bbcode.is_empty() && text == p_bbcode) {
		return;
	}

	stack_externally_modified = false;

	text = p_bbcode;
	if (text.is_empty()) {
		clear();
	} else {
		_apply_translation();
	}
}

void RichTextLabel3D::_apply_translation() {
	if (text.is_empty()) {
		return;
	}

	internal_stack_editing = true;

	String xl_text = atr(text);
	if (use_bbcode) {
		parse_bbcode(xl_text);
	} else { // Raw text.
		clear();
		add_text(xl_text);
	}

	internal_stack_editing = false;
}

String RichTextLabel3D::get_text() const {
	return text;
}

void RichTextLabel3D::set_use_bbcode(bool p_enable) {
	if (use_bbcode == p_enable) {
		return;
	}
	use_bbcode = p_enable;
	notify_property_list_changed();

	if (!stack_externally_modified) {
		_apply_translation();
	}
}

bool RichTextLabel3D::is_using_bbcode() const {
	return use_bbcode;
}

String RichTextLabel3D::get_parsed_text() const {
	String txt;
	Item *it = main;
	while (it) {
		if (it->type == ITEM_DROPCAP) {
			ItemDropcap *dc = static_cast<ItemDropcap *>(it);
			txt += dc->text;
		} else if (it->type == ITEM_TEXT) {
			ItemText *t = static_cast<ItemText *>(it);
			txt += t->text;
		} else if (it->type == ITEM_NEWLINE) {
			txt += "\n";
		} else if (it->type == ITEM_IMAGE) {
			txt += " ";
		} else if (it->type == ITEM_INDENT || it->type == ITEM_LIST) {
			txt += "\t";
		}
		it = _get_next_item(it, true);
	}
	return txt;
}

// void RichTextLabel3D::set_text_direction(Control::TextDirection p_text_direction) {
// 	ERR_FAIL_COND((int)p_text_direction < -1 || (int)p_text_direction > 3);
// 	_stop_thread();

// 	if (text_direction != p_text_direction) {
// 		text_direction = p_text_direction;
// 		if (!stack_externally_modified) {
// 			_apply_translation();
// 		} else {
// 			main->first_invalid_line.store(0); // Invalidate all lines.
// 			_invalidate_accessibility();
// 			_validate_line_caches();
// 		}
// 		queue_redraw();
// 	}
// }

// Control::TextDirection RichTextLabel3D::get_text_direction() const {
// 	return text_direction;
// }

void RichTextLabel3D::set_horizontal_alignment(HorizontalAlignment p_alignment) {
	ERR_FAIL_INDEX((int)p_alignment, 4);
	_stop_thread();

	if (default_alignment != p_alignment) {
		default_alignment = p_alignment;
		if (!stack_externally_modified) {
			_apply_translation();
		} else {
			main->first_invalid_line.store(0); // Invalidate all lines.
			_validate_line_caches();
		}
		queue_redraw();
	}
}

HorizontalAlignment RichTextLabel3D::get_horizontal_alignment() const {
	return default_alignment;
}

void RichTextLabel3D::set_vertical_alignment(VerticalAlignment p_alignment) {
	ERR_FAIL_INDEX((int)p_alignment, 4);

	if (vertical_alignment == p_alignment) {
		return;
	}

	vertical_alignment = p_alignment;
	queue_redraw();
}

VerticalAlignment RichTextLabel3D::get_vertical_alignment() const {
	return vertical_alignment;
}

void RichTextLabel3D::set_justification_flags(BitField<TextServer::JustificationFlag> p_flags) {
	_stop_thread();

	if (default_jst_flags != p_flags) {
		default_jst_flags = p_flags;
		if (!stack_externally_modified) {
			_apply_translation();
		} else {
			main->first_invalid_line.store(0); // Invalidate all lines.
			_validate_line_caches();
		}
		queue_redraw();
	}
}

BitField<TextServer::JustificationFlag> RichTextLabel3D::get_justification_flags() const {
	return default_jst_flags;
}

void RichTextLabel3D::set_tab_stops(const PackedFloat32Array &p_tab_stops) {
	_stop_thread();

	if (default_tab_stops != p_tab_stops) {
		default_tab_stops = p_tab_stops;
		if (!stack_externally_modified) {
			_apply_translation();
		} else {
			main->first_invalid_line.store(0); // Invalidate all lines.
			_validate_line_caches();
		}
		queue_redraw();
	}
}

PackedFloat32Array RichTextLabel3D::get_tab_stops() const {
	return default_tab_stops;
}

// void RichTextLabel3D::set_structured_text_bidi_override(TextServer::StructuredTextParser p_parser) {
// 	if (st_parser != p_parser) {
// 		_stop_thread();

// 		st_parser = p_parser;
// 		if (!stack_externally_modified) {
// 			_apply_translation();
// 		} else {
// 			main->first_invalid_line.store(0); // Invalidate all lines.
// 			_invalidate_accessibility();
// 			_validate_line_caches();
// 		}
// 		queue_redraw();
// 	}
// }

// TextServer::StructuredTextParser RichTextLabel3D::get_structured_text_bidi_override() const {
// 	return st_parser;
// }

// void RichTextLabel3D::set_structured_text_bidi_override_options(Array p_args) {
// 	if (st_args != p_args) {
// 		_stop_thread();

// 		st_args = p_args;
// 		main->first_invalid_line.store(0); // Invalidate all lines.
// 		_invalidate_accessibility();
// 		_validate_line_caches();
// 		queue_redraw();
// 	}
// }

// Array RichTextLabel3D::get_structured_text_bidi_override_options() const {
// 	return st_args;
// }

// void RichTextLabel3D::set_language(const String &p_language) {
// 	if (language != p_language) {
// 		_stop_thread();

// 		language = p_language;
// 		if (!stack_externally_modified) {
// 			_apply_translation();
// 		} else {
// 			main->first_invalid_line.store(0); // Invalidate all lines.
// 			_invalidate_accessibility();
// 			_validate_line_caches();
// 		}
// 		queue_redraw();
// 	}
// }

// String RichTextLabel3D::get_language() const {
// 	return language;
// }

void RichTextLabel3D::set_autowrap_mode(TextServer::AutowrapMode p_mode) {
	if (autowrap_mode != p_mode) {
		_stop_thread();

		autowrap_mode = p_mode;
		main->first_invalid_line = 0; // Invalidate all lines.
		_validate_line_caches();
		queue_redraw();
	}
}

TextServer::AutowrapMode RichTextLabel3D::get_autowrap_mode() const {
	return autowrap_mode;
}

void RichTextLabel3D::set_autowrap_trim_flags(BitField<TextServer::LineBreakFlag> p_flags) {
	if (autowrap_flags_trim != (p_flags & TextServer::BREAK_TRIM_MASK)) {
		_stop_thread();

		autowrap_flags_trim = p_flags & TextServer::BREAK_TRIM_MASK;
		main->first_invalid_line = 0; // Invalidate all lines.
		_validate_line_caches();
		queue_redraw();
	}
}

BitField<TextServer::LineBreakFlag> RichTextLabel3D::get_autowrap_trim_flags() const {
	return autowrap_flags_trim;
}

void RichTextLabel3D::set_visible_ratio(float p_ratio) {
	if (visible_ratio != p_ratio) {
		_stop_thread();

		int prev_vc = visible_characters;
		if (p_ratio >= 1.0) {
			visible_characters = -1;
			visible_ratio = 1.0;
		} else if (p_ratio < 0.0) {
			visible_characters = 0;
			visible_ratio = 0.0;
		} else {
			visible_characters = get_total_character_count() * p_ratio;
			visible_ratio = p_ratio;
		}

		if (visible_chars_behavior == TextServer::VC_CHARS_BEFORE_SHAPING && visible_characters != prev_vc) {
			int new_vc = (visible_characters < 0) ? get_total_character_count() : visible_characters;
			int old_vc = (prev_vc < 0) ? get_total_character_count() : prev_vc;
			int to_line = main->first_invalid_line.load();
			int old_from_l = to_line;
			int new_from_l = to_line;
			for (int i = 0; i < to_line; i++) {
				const Line &l = main->lines[i];
				if (l.char_offset <= old_vc && l.char_offset + l.char_count > old_vc) {
					old_from_l = i;
				}
				if (l.char_offset <= new_vc && l.char_offset + l.char_count > new_vc) {
					new_from_l = i;
				}
			}
			Rect2 text_rect = _get_text_rect();
			int first_invalid = MIN(new_from_l, old_from_l);
			int second_invalid = MAX(new_from_l, old_from_l);

			float total_height = (first_invalid == 0) ? 0 : _calculate_line_vertical_offset(main->lines[first_invalid - 1]);
			if (first_invalid < to_line) {
				int total_chars = main->lines[first_invalid].char_offset;
				total_height = _shape_line(main, first_invalid, _get_font_or_default(), font_size, text_rect.get_size().width, total_height, &total_chars);
			}
			if (first_invalid != second_invalid) {
				for (int i = first_invalid + 1; i < second_invalid; i++) {
					main->lines[i].offset.y = total_height;
					total_height = _calculate_line_vertical_offset(main->lines[i]);
				}
				if (second_invalid < to_line) {
					int total_chars = main->lines[second_invalid].char_offset;
					total_height = _shape_line(main, second_invalid, _get_font_or_default(), font_size, text_rect.get_size().width, total_height, &total_chars);
				}
			}
			for (int i = second_invalid + 1; i < to_line; i++) {
				main->lines[i].offset.y = total_height;
				total_height = _calculate_line_vertical_offset(main->lines[i]);
			}
		}
		
		queue_redraw();
	}
}

Ref<Font> RichTextLabel3D::get_font() const {
	return font_override;
}

void RichTextLabel3D::_font_changed() {
	dirty_font = true;
	queue_redraw();
}

void RichTextLabel3D::set_font(const Ref<Font> &p_font) {
	if (font_override != p_font) {
		if (font_override.is_valid()) {
			font_override->disconnect_changed(callable_mp(this, &RichTextLabel3D::_font_changed));
		}
		font_override = p_font;
		dirty_font = true;
		if (font_override.is_valid()) {
			font_override->connect_changed(callable_mp(this, &RichTextLabel3D::_font_changed));
		}
		queue_redraw();
	}
}

void RichTextLabel3D::set_bold_font(const Ref<Font> &p_font) {
	if (bold_font != p_font) {
		if (bold_font.is_valid()) {
			bold_font->disconnect_changed(callable_mp(this, &RichTextLabel3D::_font_changed));
		}
		bold_font = p_font;
		dirty_font = true;
		if (bold_font.is_valid()) {
			bold_font->connect_changed(callable_mp(this, &RichTextLabel3D::_font_changed));
		}
		queue_redraw();
	}
}

Ref<Font> RichTextLabel3D::get_bold_font() const {
	return bold_font;
}

void RichTextLabel3D::set_italics_font(const Ref<Font> &p_font) {
	if (italics_font != p_font) {
		if (italics_font.is_valid()) {
			italics_font->disconnect_changed(callable_mp(this, &RichTextLabel3D::_font_changed));
		}
		italics_font = p_font;
		dirty_font = true;
		if (italics_font.is_valid()) {
			italics_font->connect_changed(callable_mp(this, &RichTextLabel3D::_font_changed));
		}
		queue_redraw();
	}
}

Ref<Font> RichTextLabel3D::get_italics_font() const {
	return italics_font;
}

void RichTextLabel3D::set_bold_italics_font(const Ref<Font> &p_font) {
	if (bold_italics_font != p_font) {
		if (bold_italics_font.is_valid()) {
			bold_italics_font->disconnect_changed(callable_mp(this, &RichTextLabel3D::_font_changed));
		}
		bold_italics_font = p_font;
		dirty_font = true;
		if (bold_italics_font.is_valid()) {
			bold_italics_font->connect_changed(callable_mp(this, &RichTextLabel3D::_font_changed));
		}
		queue_redraw();
	}
}

Ref<Font> RichTextLabel3D::get_bold_italics_font() const {
	return bold_italics_font;
}

void RichTextLabel3D::set_mono_font(const Ref<Font> &p_font) {
	if (mono_font != p_font) {
		if (mono_font.is_valid()) {
			mono_font->disconnect_changed(callable_mp(this, &RichTextLabel3D::_font_changed));
		}
		mono_font = p_font;
		dirty_font = true;
		if (mono_font.is_valid()) {
			mono_font->connect_changed(callable_mp(this, &RichTextLabel3D::_font_changed));
		}
		queue_redraw();
	}
}

Ref<Font> RichTextLabel3D::get_mono_font() const {
	return mono_font;
}

Ref<Font> RichTextLabel3D::_get_font_or_default() const {
	// Similar code taken from `FontVariation::_get_base_font_or_default`.

	if (theme_font.is_valid()) {
		theme_font->disconnect_changed(callable_mp(const_cast<RichTextLabel3D *>(this), &RichTextLabel3D::_font_changed));
		theme_font.unref();
	}

	if (font_override.is_valid()) {
		return font_override;
	}

	const StringName theme_name = SceneStringName(font);
	Vector<StringName> theme_types;
	ThemeDB::get_singleton()->get_native_type_dependencies(get_class_name(), theme_types);

	ThemeContext *global_context = ThemeDB::get_singleton()->get_default_theme_context();
	Vector<Ref<Theme>> themes = global_context->get_themes();
	if (Engine::get_singleton()->is_editor_hint()) {
		themes.insert(0, ThemeDB::get_singleton()->get_project_theme());
	}

	for (const Ref<Theme> &theme : themes) {
		if (theme.is_null()) {
			continue;
		}

		for (const StringName &E : theme_types) {
			if (!theme->has_font(theme_name, E)) {
				continue;
			}

			Ref<Font> f = theme->get_font(theme_name, E);
			if (f.is_valid()) {
				theme_font = f;
				theme_font->connect_changed(callable_mp(const_cast<RichTextLabel3D *>(this), &RichTextLabel3D::_font_changed));
			}
			return f;
		}
	}

	Ref<Font> f = global_context->get_fallback_theme()->get_font(theme_name, StringName());
	if (f.is_valid()) {
		theme_font = f;
		theme_font->connect_changed(callable_mp(const_cast<RichTextLabel3D *>(this), &RichTextLabel3D::_font_changed));
	}
	return f;
}


float RichTextLabel3D::get_visible_ratio() const {
	return visible_ratio;
}

int RichTextLabel3D::get_content_height() const {
	const_cast<RichTextLabel3D *>(this)->_validate_line_caches();

	int total_height = 0;
	int to_line = main->first_invalid_line.load();
	if (to_line) {
		MutexLock lock(main->lines[to_line - 1].text_buf->get_mutex());
		if (theme_cache.line_separation < 0) {
			// Do not apply to the last line to avoid cutting text.
			total_height = main->lines[to_line - 1].offset.y + main->lines[to_line - 1].text_buf->get_size().y + (main->lines[to_line - 1].text_buf->get_line_count() - 1) * theme_cache.line_separation;
		} else {
			total_height = main->lines[to_line - 1].offset.y + main->lines[to_line - 1].text_buf->get_size().y + (main->lines[to_line - 1].text_buf->get_line_count() - 1) * theme_cache.line_separation + theme_cache.paragraph_separation;
		}
	}
	return total_height;
}

Rect2i RichTextLabel3D::get_visible_content_rect() const {
	return visible_rect;
}

int RichTextLabel3D::get_content_width() const {
	const_cast<RichTextLabel3D *>(this)->_validate_line_caches();

	int total_width = 0;
	int to_line = main->first_invalid_line.load();
	for (int i = 0; i < to_line; i++) {
		MutexLock lock(main->lines[i].text_buf->get_mutex());
		total_width = MAX(total_width, main->lines[i].offset.x + main->lines[i].text_buf->get_size().x);
	}
	return total_width;
}

int RichTextLabel3D::get_line_height(int p_line) const {
	const_cast<RichTextLabel3D *>(this)->_validate_line_caches();

	int line_count = 0;
	int to_line = main->first_invalid_line.load();
	for (int i = 0; i < to_line; i++) {
		MutexLock lock(main->lines[i].text_buf->get_mutex());
		int lc = main->lines[i].text_buf->get_line_count();

		if (p_line < line_count + lc) {
			const Ref<TextParagraph> text_buf = main->lines[i].text_buf;
			return text_buf->get_line_ascent(p_line - line_count) + text_buf->get_line_descent(p_line - line_count) + theme_cache.line_separation;
		}
		line_count += lc;
	}
	return 0;
}

int RichTextLabel3D::get_line_width(int p_line) const {
	const_cast<RichTextLabel3D *>(this)->_validate_line_caches();

	int line_count = 0;
	int to_line = main->first_invalid_line.load();
	for (int i = 0; i < to_line; i++) {
		MutexLock lock(main->lines[i].text_buf->get_mutex());
		int lc = main->lines[i].text_buf->get_line_count();

		if (p_line < line_count + lc) {
			return main->lines[i].text_buf->get_line_width(p_line - line_count);
		}
		line_count += lc;
	}
	return 0;
}

Ref<Mesh> RichTextLabel3D::get_mesh() const {
	Ref<ArrayMesh> new_mesh;
	new_mesh.instantiate();
	
	for (const KeyValue<SurfaceKey, SurfaceData> &E : surfaces) {
		Array mesh_array;
		mesh_array.resize(RS::ARRAY_MAX);
		mesh_array[RS::ARRAY_VERTEX] = E.value.mesh_vertices;
		mesh_array[RS::ARRAY_NORMAL] = E.value.mesh_normals;
		mesh_array[RS::ARRAY_TANGENT] = E.value.mesh_tangents;
		mesh_array[RS::ARRAY_COLOR] = E.value.mesh_colors;
		mesh_array[RS::ARRAY_TEX_UV] = E.value.mesh_uvs;
		mesh_array[RS::ARRAY_INDEX] = E.value.indices;

		new_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, mesh_array);
	}
	
	return new_mesh;
}

void RichTextLabel3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_width"), &RichTextLabel3D::get_width);
	ClassDB::bind_method(D_METHOD("set_width", "width"), &RichTextLabel3D::set_width);
	ClassDB::bind_method(D_METHOD("get_height"), &RichTextLabel3D::get_height);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &RichTextLabel3D::set_height);
	ClassDB::bind_method(D_METHOD("set_pixel_size", "pixel_size"), &RichTextLabel3D::set_pixel_size);
	ClassDB::bind_method(D_METHOD("get_pixel_size"), &RichTextLabel3D::get_pixel_size);
	ClassDB::bind_method(D_METHOD("get_font_size"), &RichTextLabel3D::get_font_size);
	ClassDB::bind_method(D_METHOD("set_font_size", "font_size"), &RichTextLabel3D::set_font_size);
	ClassDB::bind_method(D_METHOD("get_bold_font_size"), &RichTextLabel3D::get_bold_font_size);
	ClassDB::bind_method(D_METHOD("set_bold_font_size", "font_size"), &RichTextLabel3D::set_bold_font_size);
	ClassDB::bind_method(D_METHOD("get_italics_font_size"), &RichTextLabel3D::get_italics_font_size);
	ClassDB::bind_method(D_METHOD("set_italics_font_size", "font_size"), &RichTextLabel3D::set_italics_font_size);
	ClassDB::bind_method(D_METHOD("get_bold_italics_font_size"), &RichTextLabel3D::get_bold_italics_font_size);
	ClassDB::bind_method(D_METHOD("set_bold_italics_font_size", "font_size"), &RichTextLabel3D::set_bold_italics_font_size);
	ClassDB::bind_method(D_METHOD("set_mono_font", "font"), &RichTextLabel3D::set_mono_font);
	ClassDB::bind_method(D_METHOD("get_mono_font"), &RichTextLabel3D::get_mono_font);
	ClassDB::bind_method(D_METHOD("get_mono_font_size"), &RichTextLabel3D::get_mono_font_size);
	ClassDB::bind_method(D_METHOD("set_mono_font_size", "font_size"), &RichTextLabel3D::set_mono_font_size);
	ClassDB::bind_method(D_METHOD("get_parsed_text"), &RichTextLabel3D::get_parsed_text);
	ClassDB::bind_method(D_METHOD("add_text", "text"), &RichTextLabel3D::add_text);
	ClassDB::bind_method(D_METHOD("set_text", "text"), &RichTextLabel3D::set_text);
	ClassDB::bind_method(D_METHOD("get_mesh"), &RichTextLabel3D::get_mesh);
	ClassDB::bind_method(D_METHOD("set_font", "font"), &RichTextLabel3D::set_font);
	ClassDB::bind_method(D_METHOD("get_font"), &RichTextLabel3D::get_font);
	ClassDB::bind_method(D_METHOD("set_bold_font", "font"), &RichTextLabel3D::set_bold_font);
	ClassDB::bind_method(D_METHOD("get_bold_font"), &RichTextLabel3D::get_bold_font);
	ClassDB::bind_method(D_METHOD("set_italics_font", "font"), &RichTextLabel3D::set_italics_font);
	ClassDB::bind_method(D_METHOD("get_italics_font"), &RichTextLabel3D::get_italics_font);
	ClassDB::bind_method(D_METHOD("set_bold_italics_font", "font"), &RichTextLabel3D::set_bold_italics_font);
	ClassDB::bind_method(D_METHOD("get_bold_italics_font"), &RichTextLabel3D::get_bold_italics_font);
	ClassDB::bind_method(D_METHOD("add_hr", "width", "height", "color", "alignment", "width_in_percent", "height_in_percent"), &RichTextLabel3D::add_hr, DEFVAL(90), DEFVAL(2), DEFVAL(Color(1, 1, 1, 1)), DEFVAL(HORIZONTAL_ALIGNMENT_CENTER), DEFVAL(true), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("add_image", "image", "width", "height", "color", "inline_align", "region", "key", "pad", "tooltip", "width_in_percent", "height_in_percent", "alt_text"), &RichTextLabel3D::add_image, DEFVAL(0), DEFVAL(0), DEFVAL(Color(1.0, 1.0, 1.0)), DEFVAL(INLINE_ALIGNMENT_CENTER), DEFVAL(Rect2()), DEFVAL(Variant()), DEFVAL(false), DEFVAL(String()), DEFVAL(false), DEFVAL(false), DEFVAL(String()));
	// ClassDB::bind_method(D_METHOD("update_image", "key", "mask", "image", "width", "height", "color", "inline_align", "region", "pad", "tooltip", "width_in_percent", "height_in_percent"), &RichTextLabel3D::update_image, DEFVAL(0), DEFVAL(0), DEFVAL(Color(1.0, 1.0, 1.0)), DEFVAL(INLINE_ALIGNMENT_CENTER), DEFVAL(Rect2()), DEFVAL(false), DEFVAL(String()), DEFVAL(false), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("newline"), &RichTextLabel3D::add_newline);
	ClassDB::bind_method(D_METHOD("remove_paragraph", "paragraph", "no_invalidate"), &RichTextLabel3D::remove_paragraph, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("invalidate_paragraph", "paragraph"), &RichTextLabel3D::invalidate_paragraph);
	ClassDB::bind_method(D_METHOD("push_font", "font", "font_size"), &RichTextLabel3D::push_font, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("push_font_size", "font_size"), &RichTextLabel3D::push_font_size);
	ClassDB::bind_method(D_METHOD("push_normal"), &RichTextLabel3D::push_normal);
	ClassDB::bind_method(D_METHOD("push_bold"), &RichTextLabel3D::push_bold);
	ClassDB::bind_method(D_METHOD("push_bold_italics"), &RichTextLabel3D::push_bold_italics);
	ClassDB::bind_method(D_METHOD("push_italics"), &RichTextLabel3D::push_italics);
	ClassDB::bind_method(D_METHOD("push_mono"), &RichTextLabel3D::push_mono);
	ClassDB::bind_method(D_METHOD("push_color", "color"), &RichTextLabel3D::push_color);
	ClassDB::bind_method(D_METHOD("push_outline_size", "outline_size"), &RichTextLabel3D::push_outline_size);
	ClassDB::bind_method(D_METHOD("push_outline_color", "color"), &RichTextLabel3D::push_outline_color);
	ClassDB::bind_method(D_METHOD("push_paragraph", "alignment", "base_direction", "language", "st_parser", "justification_flags", "tab_stops"), &RichTextLabel3D::push_paragraph, DEFVAL(TextServer::DIRECTION_AUTO), DEFVAL(""), DEFVAL(TextServer::STRUCTURED_TEXT_DEFAULT), DEFVAL(TextServer::JUSTIFICATION_WORD_BOUND | TextServer::JUSTIFICATION_KASHIDA | TextServer::JUSTIFICATION_SKIP_LAST_LINE | TextServer::JUSTIFICATION_DO_NOT_SKIP_SINGLE_LINE), DEFVAL(PackedFloat32Array()));
	ClassDB::bind_method(D_METHOD("push_indent", "level"), &RichTextLabel3D::push_indent);
	ClassDB::bind_method(D_METHOD("push_list", "level", "type", "capitalize", "bullet"), &RichTextLabel3D::push_list, DEFVAL(String::utf8("•")));
	ClassDB::bind_method(D_METHOD("push_meta", "data", "underline_mode", "tooltip"), &RichTextLabel3D::push_meta, DEFVAL(META_UNDERLINE_ALWAYS), DEFVAL(String()));
	ClassDB::bind_method(D_METHOD("push_hint", "description"), &RichTextLabel3D::push_hint);
	ClassDB::bind_method(D_METHOD("push_language", "language"), &RichTextLabel3D::push_language);
	ClassDB::bind_method(D_METHOD("push_underline", "color"), &RichTextLabel3D::push_underline, DEFVAL(Color(0, 0, 0, 0)));
	ClassDB::bind_method(D_METHOD("push_strikethrough", "color"), &RichTextLabel3D::push_strikethrough, DEFVAL(Color(0, 0, 0, 0)));
	ClassDB::bind_method(D_METHOD("push_table", "columns", "inline_align", "align_to_row", "name"), &RichTextLabel3D::push_table, DEFVAL(INLINE_ALIGNMENT_TOP), DEFVAL(-1), DEFVAL(String()));
	ClassDB::bind_method(D_METHOD("push_dropcap", "string", "font", "size", "dropcap_margins", "color", "outline_size", "outline_color"), &RichTextLabel3D::push_dropcap, DEFVAL(Rect2()), DEFVAL(Color(1, 1, 1)), DEFVAL(0), DEFVAL(Color(0, 0, 0, 0)));
	ClassDB::bind_method(D_METHOD("set_table_column_expand", "column", "expand", "ratio", "shrink"), &RichTextLabel3D::set_table_column_expand, DEFVAL(1), DEFVAL(true));
	ClassDB::bind_method(D_METHOD("set_table_column_name", "column", "name"), &RichTextLabel3D::set_table_column_name);
	ClassDB::bind_method(D_METHOD("set_cell_row_background_color", "odd_row_bg", "even_row_bg"), &RichTextLabel3D::set_cell_row_background_color);
	ClassDB::bind_method(D_METHOD("set_cell_border_color", "color"), &RichTextLabel3D::set_cell_border_color);
	ClassDB::bind_method(D_METHOD("set_cell_size_override", "min_size", "max_size"), &RichTextLabel3D::set_cell_size_override);
	ClassDB::bind_method(D_METHOD("set_cell_padding", "padding"), &RichTextLabel3D::set_cell_padding);
	ClassDB::bind_method(D_METHOD("set_cell_margins", "margins"), &RichTextLabel3D::set_cell_margins);
	ClassDB::bind_method(D_METHOD("push_cell"), &RichTextLabel3D::push_cell);
	ClassDB::bind_method(D_METHOD("push_fgcolor", "fgcolor"), &RichTextLabel3D::push_fgcolor);
	ClassDB::bind_method(D_METHOD("push_bgcolor", "bgcolor"), &RichTextLabel3D::push_bgcolor);
	ClassDB::bind_method(D_METHOD("push_context"), &RichTextLabel3D::push_context);
	ClassDB::bind_method(D_METHOD("pop_context"), &RichTextLabel3D::pop_context);
	ClassDB::bind_method(D_METHOD("pop"), &RichTextLabel3D::pop);
	ClassDB::bind_method(D_METHOD("pop_all"), &RichTextLabel3D::pop_all);

	ClassDB::bind_method(D_METHOD("set_outline_size", "outline_size"), &RichTextLabel3D::set_outline_size);
	ClassDB::bind_method(D_METHOD("get_outline_size"), &RichTextLabel3D::get_outline_size);

	ClassDB::bind_method(D_METHOD("set_outline_depth", "depth"), &RichTextLabel3D::set_outline_depth);
	ClassDB::bind_method(D_METHOD("get_outline_depth"), &RichTextLabel3D::get_outline_depth);

	ClassDB::bind_method(D_METHOD("set_cell_children_depth", "depth"), &RichTextLabel3D::set_cell_children_depth);
	ClassDB::bind_method(D_METHOD("get_cell_children_depth"), &RichTextLabel3D::get_cell_children_depth);

	ClassDB::bind_method(D_METHOD("set_line_spacing", "line_spacing"), &RichTextLabel3D::set_line_spacing);
	ClassDB::bind_method(D_METHOD("get_line_spacing"), &RichTextLabel3D::get_line_spacing);
	
	ClassDB::bind_method(D_METHOD("clear"), &RichTextLabel3D::clear);


	ClassDB::bind_method(D_METHOD("set_modulate", "modulate"), &RichTextLabel3D::set_modulate);
	ClassDB::bind_method(D_METHOD("get_modulate"), &RichTextLabel3D::get_modulate);

	ClassDB::bind_method(D_METHOD("set_outline_modulate", "modulate"), &RichTextLabel3D::set_outline_modulate);
	ClassDB::bind_method(D_METHOD("get_outline_modulate"), &RichTextLabel3D::get_outline_modulate);

	// ClassDB::bind_method(D_METHOD("set_structured_text_bidi_override", "parser"), &RichTextLabel3D::set_structured_text_bidi_override);
	// ClassDB::bind_method(D_METHOD("get_structured_text_bidi_override"), &RichTextLabel3D::get_structured_text_bidi_override);
	// ClassDB::bind_method(D_METHOD("set_structured_text_bidi_override_options", "args"), &RichTextLabel3D::set_structured_text_bidi_override_options);
	// ClassDB::bind_method(D_METHOD("get_structured_text_bidi_override_options"), &RichTextLabel3D::get_structured_text_bidi_override_options);
	// ClassDB::bind_method(D_METHOD("set_language", "language"), &RichTextLabel3D::set_language);
	// ClassDB::bind_method(D_METHOD("get_language"), &RichTextLabel3D::get_language);

	ClassDB::bind_method(D_METHOD("set_horizontal_alignment", "alignment"), &RichTextLabel3D::set_horizontal_alignment);
	ClassDB::bind_method(D_METHOD("get_horizontal_alignment"), &RichTextLabel3D::get_horizontal_alignment);
	ClassDB::bind_method(D_METHOD("set_vertical_alignment", "alignment"), &RichTextLabel3D::set_vertical_alignment);
	ClassDB::bind_method(D_METHOD("get_vertical_alignment"), &RichTextLabel3D::get_vertical_alignment);
	ClassDB::bind_method(D_METHOD("set_justification_flags", "justification_flags"), &RichTextLabel3D::set_justification_flags);
	ClassDB::bind_method(D_METHOD("get_justification_flags"), &RichTextLabel3D::get_justification_flags);
	ClassDB::bind_method(D_METHOD("set_tab_stops", "tab_stops"), &RichTextLabel3D::set_tab_stops);
	ClassDB::bind_method(D_METHOD("get_tab_stops"), &RichTextLabel3D::get_tab_stops);

	ClassDB::bind_method(D_METHOD("set_autowrap_mode", "autowrap_mode"), &RichTextLabel3D::set_autowrap_mode);
	ClassDB::bind_method(D_METHOD("get_autowrap_mode"), &RichTextLabel3D::get_autowrap_mode);

	ClassDB::bind_method(D_METHOD("set_autowrap_trim_flags", "autowrap_trim_flags"), &RichTextLabel3D::set_autowrap_trim_flags);
	ClassDB::bind_method(D_METHOD("get_autowrap_trim_flags"), &RichTextLabel3D::get_autowrap_trim_flags);

	ClassDB::bind_method(D_METHOD("parse_bbcode", "bbcode"), &RichTextLabel3D::parse_bbcode);
	ClassDB::bind_method(D_METHOD("append_text", "bbcode"), &RichTextLabel3D::append_text);

	ClassDB::bind_method(D_METHOD("get_text"), &RichTextLabel3D::get_text);

#ifndef DISABLE_DEPRECATED
	ClassDB::bind_method(D_METHOD("is_ready"), &RichTextLabel3D::is_finished);
#endif // DISABLE_DEPRECATED
	ClassDB::bind_method(D_METHOD("is_finished"), &RichTextLabel3D::is_finished);

	ClassDB::bind_method(D_METHOD("set_threaded", "threaded"), &RichTextLabel3D::set_threaded);
	ClassDB::bind_method(D_METHOD("is_threaded"), &RichTextLabel3D::is_threaded);

	ClassDB::bind_method(D_METHOD("set_visible_characters", "amount"), &RichTextLabel3D::set_visible_characters);
	ClassDB::bind_method(D_METHOD("get_visible_characters"), &RichTextLabel3D::get_visible_characters);

	ClassDB::bind_method(D_METHOD("get_visible_characters_behavior"), &RichTextLabel3D::get_visible_characters_behavior);
	ClassDB::bind_method(D_METHOD("set_visible_characters_behavior", "behavior"), &RichTextLabel3D::set_visible_characters_behavior);

	ClassDB::bind_method(D_METHOD("set_visible_ratio", "ratio"), &RichTextLabel3D::set_visible_ratio);
	ClassDB::bind_method(D_METHOD("get_visible_ratio"), &RichTextLabel3D::get_visible_ratio);

	ClassDB::bind_method(D_METHOD("get_character_line", "character"), &RichTextLabel3D::get_character_line);
	ClassDB::bind_method(D_METHOD("get_character_paragraph", "character"), &RichTextLabel3D::get_character_paragraph);
	ClassDB::bind_method(D_METHOD("get_total_character_count"), &RichTextLabel3D::get_total_character_count);

	ClassDB::bind_method(D_METHOD("set_use_bbcode", "enable"), &RichTextLabel3D::set_use_bbcode);
	ClassDB::bind_method(D_METHOD("is_using_bbcode"), &RichTextLabel3D::is_using_bbcode);

	ClassDB::bind_method(D_METHOD("get_line_count"), &RichTextLabel3D::get_line_count);
	ClassDB::bind_method(D_METHOD("get_line_range", "line"), &RichTextLabel3D::get_line_range);
	ClassDB::bind_method(D_METHOD("get_visible_line_count"), &RichTextLabel3D::get_visible_line_count);

	ClassDB::bind_method(D_METHOD("get_paragraph_count"), &RichTextLabel3D::get_paragraph_count);
	ClassDB::bind_method(D_METHOD("get_visible_paragraph_count"), &RichTextLabel3D::get_visible_paragraph_count);

	ClassDB::bind_method(D_METHOD("get_content_height"), &RichTextLabel3D::get_content_height);
	ClassDB::bind_method(D_METHOD("get_content_width"), &RichTextLabel3D::get_content_width);

	ClassDB::bind_method(D_METHOD("get_line_height", "line"), &RichTextLabel3D::get_line_height);
	ClassDB::bind_method(D_METHOD("get_line_width", "line"), &RichTextLabel3D::get_line_width);

	ClassDB::bind_method(D_METHOD("get_visible_content_rect"), &RichTextLabel3D::get_visible_content_rect);

	ClassDB::bind_method(D_METHOD("get_line_offset", "line"), &RichTextLabel3D::get_line_offset);
	ClassDB::bind_method(D_METHOD("get_paragraph_offset", "paragraph"), &RichTextLabel3D::get_paragraph_offset);

	ClassDB::bind_method(D_METHOD("parse_expressions_for_values", "expressions"), &RichTextLabel3D::parse_expressions_for_values);

	ClassDB::bind_method(D_METHOD("set_tab_size", "spaces"), &RichTextLabel3D::set_tab_size);
	ClassDB::bind_method(D_METHOD("get_tab_size"), &RichTextLabel3D::get_tab_size);


	ClassDB::bind_method(D_METHOD("set_draw_flag", "flag", "enabled"), &RichTextLabel3D::set_draw_flag);
	ClassDB::bind_method(D_METHOD("get_draw_flag", "flag"), &RichTextLabel3D::get_draw_flag);

	ClassDB::bind_method(D_METHOD("set_billboard_mode", "mode"), &RichTextLabel3D::set_billboard_mode);
	ClassDB::bind_method(D_METHOD("get_billboard_mode"), &RichTextLabel3D::get_billboard_mode);

	ClassDB::bind_method(D_METHOD("set_alpha_cut_mode", "mode"), &RichTextLabel3D::set_alpha_cut_mode);
	ClassDB::bind_method(D_METHOD("get_alpha_cut_mode"), &RichTextLabel3D::get_alpha_cut_mode);

	ClassDB::bind_method(D_METHOD("set_alpha_scissor_threshold", "threshold"), &RichTextLabel3D::set_alpha_scissor_threshold);
	ClassDB::bind_method(D_METHOD("get_alpha_scissor_threshold"), &RichTextLabel3D::get_alpha_scissor_threshold);

	ClassDB::bind_method(D_METHOD("set_alpha_hash_scale", "threshold"), &RichTextLabel3D::set_alpha_hash_scale);
	ClassDB::bind_method(D_METHOD("get_alpha_hash_scale"), &RichTextLabel3D::get_alpha_hash_scale);

	ClassDB::bind_method(D_METHOD("set_alpha_antialiasing", "alpha_aa"), &RichTextLabel3D::set_alpha_antialiasing);
	ClassDB::bind_method(D_METHOD("get_alpha_antialiasing"), &RichTextLabel3D::get_alpha_antialiasing);

	ClassDB::bind_method(D_METHOD("set_alpha_antialiasing_edge", "edge"), &RichTextLabel3D::set_alpha_antialiasing_edge);
	ClassDB::bind_method(D_METHOD("get_alpha_antialiasing_edge"), &RichTextLabel3D::get_alpha_antialiasing_edge);

	ClassDB::bind_method(D_METHOD("set_texture_filter", "mode"), &RichTextLabel3D::set_texture_filter);
	ClassDB::bind_method(D_METHOD("get_texture_filter"), &RichTextLabel3D::get_texture_filter);

	ClassDB::bind_method(D_METHOD("generate_triangle_mesh"), &RichTextLabel3D::generate_triangle_mesh);

	ClassDB::bind_method(D_METHOD("set_render_priority", "priority"), &RichTextLabel3D::set_render_priority);
	ClassDB::bind_method(D_METHOD("get_render_priority"), &RichTextLabel3D::get_render_priority);
	ClassDB::bind_method(D_METHOD("set_outline_render_priority", "priority"), &RichTextLabel3D::set_outline_render_priority);
	ClassDB::bind_method(D_METHOD("get_outline_render_priority"), &RichTextLabel3D::get_outline_render_priority);

	// Note: set "bbcode_enabled" first, to avoid unnecessary "text" resets.
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "bbcode_enabled"), "set_use_bbcode", "is_using_bbcode");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "text", PROPERTY_HINT_MULTILINE_TEXT), "set_text", "get_text");
	
	// ADD_PROPERTY(PropertyInfo(Variant::BOOL, "fit_content"), "set_fit_content", "is_fit_content_enabled");
	// ADD_PROPERTY(PropertyInfo(Variant::BOOL, "scroll_active"), "set_scroll_active", "is_scroll_active");
	// ADD_PROPERTY(PropertyInfo(Variant::BOOL, "scroll_following"), "set_scroll_follow", "is_scroll_following");
	// ADD_PROPERTY(PropertyInfo(Variant::BOOL, "scroll_following_visible_characters"), "set_scroll_follow_visible_characters", "is_scroll_following_visible_characters");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "autowrap_mode", PROPERTY_HINT_ENUM, "Off,Arbitrary,Word,Word (Smart)"), "set_autowrap_mode", "get_autowrap_mode");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "autowrap_trim_flags", PROPERTY_HINT_FLAGS, vformat("Trim Spaces After Break:%d,Trim Spaces Before Break:%d", TextServer::BREAK_TRIM_START_EDGE_SPACES, TextServer::BREAK_TRIM_END_EDGE_SPACES)), "set_autowrap_trim_flags", "get_autowrap_trim_flags");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "tab_size", PROPERTY_HINT_RANGE, "0,24,1"), "set_tab_size", "get_tab_size");
	// ADD_PROPERTY(PropertyInfo(Variant::BOOL, "context_menu_enabled"), "set_context_menu_enabled", "is_context_menu_enabled");
	// ADD_PROPERTY(PropertyInfo(Variant::BOOL, "shortcut_keys_enabled"), "set_shortcut_keys_enabled", "is_shortcut_keys_enabled");

	ADD_PROPERTY(PropertyInfo(Variant::INT, "horizontal_alignment", PROPERTY_HINT_ENUM, "Left,Center,Right,Fill"), "set_horizontal_alignment", "get_horizontal_alignment");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "vertical_alignment", PROPERTY_HINT_ENUM, "Top,Center,Bottom,Fill"), "set_vertical_alignment", "get_vertical_alignment");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "line_spacing", PROPERTY_HINT_NONE, "suffix:px"), "set_line_spacing", "get_line_spacing");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "justification_flags", PROPERTY_HINT_FLAGS, "Kashida Justification:1,Word Justification:2,Justify Only After Last Tab:8,Skip Last Line:32,Skip Last Line With Visible Characters:64,Do Not Skip Single Line:128"), "set_justification_flags", "get_justification_flags");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "tab_stops"), "set_tab_stops", "get_tab_stops");

	ADD_GROUP("Fonts", "");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "font", PROPERTY_HINT_RESOURCE_TYPE, "Font"), "set_font", "get_font");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "font_size"), "set_font_size", "get_font_size");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "outline_size", PROPERTY_HINT_RANGE, "0,127,1,suffix:px"), "set_outline_size", "get_outline_size");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "outline_depth", PROPERTY_HINT_RANGE, "0,5,0.01"), "set_outline_depth", "get_outline_depth");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cell_children_depth", PROPERTY_HINT_RANGE, "0,5,0.01"), "set_cell_children_depth", "get_cell_children_depth");

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "bold_font", PROPERTY_HINT_RESOURCE_TYPE, "Font"), "set_bold_font", "get_bold_font");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bold_font_size"), "set_bold_font_size", "get_bold_font_size");

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "italics_font", PROPERTY_HINT_RESOURCE_TYPE, "Font"), "set_italics_font", "get_italics_font");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "italics_font_size"), "set_italics_font_size", "get_italics_font_size");

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "bold_italics_font", PROPERTY_HINT_RESOURCE_TYPE, "Font"), "set_bold_italics_font", "get_bold_italics_font");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bold_italics_font_size"), "set_bold_italics_font_size", "get_bold_italics_font_size");
	
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mono_font", PROPERTY_HINT_RESOURCE_TYPE, "Font"), "set_mono_font", "get_mono_font");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "mono_font_size"), "set_mono_font_size", "get_mono_font_size");

	ADD_GROUP("Colors", "");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "modulate"), "set_modulate", "get_modulate");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "outline_modulate"), "set_outline_modulate", "get_outline_modulate");

	ADD_GROUP("Size", "");

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "width"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "height"), "set_height", "get_height");

	ADD_GROUP("Threading", "");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "threaded"), "set_threaded", "is_threaded");
	// ADD_PROPERTY(PropertyInfo(Variant::INT, "progress_bar_delay", PROPERTY_HINT_NONE, "suffix:ms"), "set_progress_bar_delay", "get_progress_bar_delay");

	// ADD_GROUP("Text Selection", "");
	// ADD_PROPERTY(PropertyInfo(Variant::BOOL, "selection_enabled"), "set_selection_enabled", "is_selection_enabled");
	// ADD_PROPERTY(PropertyInfo(Variant::BOOL, "deselect_on_focus_loss_enabled"), "set_deselect_on_focus_loss_enabled", "is_deselect_on_focus_loss_enabled");
	// ADD_PROPERTY(PropertyInfo(Variant::BOOL, "drag_and_drop_selection_enabled"), "set_drag_and_drop_selection_enabled", "is_drag_and_drop_selection_enabled");

	ADD_GROUP("Displayed Text", "");
	// Note: "visible_characters" and "visible_ratio" should be set after "text" to be correctly applied.
	ADD_PROPERTY(PropertyInfo(Variant::INT, "visible_characters", PROPERTY_HINT_RANGE, "-1,128000,1"), "set_visible_characters", "get_visible_characters");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "visible_characters_behavior", PROPERTY_HINT_ENUM, "Characters Before Shaping,Characters After Shaping,Glyphs (Layout Direction),Glyphs (Left-to-Right),Glyphs (Right-to-Left)"), "set_visible_characters_behavior", "get_visible_characters_behavior");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "visible_ratio", PROPERTY_HINT_RANGE, "0,1,0.001"), "set_visible_ratio", "get_visible_ratio");

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "pixel_size", PROPERTY_HINT_RANGE, "0.0001,128,0.0001,suffix:m"), "set_pixel_size", "get_pixel_size");
	// ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "offset", PROPERTY_HINT_NONE, "suffix:px"), "set_offset", "get_offset");

	ADD_GROUP("Flags", "");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "billboard", PROPERTY_HINT_ENUM, "Disabled,Enabled,Y-Billboard"), "set_billboard_mode", "get_billboard_mode");
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "shaded"), "set_draw_flag", "get_draw_flag", FLAG_SHADED);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "double_sided"), "set_draw_flag", "get_draw_flag", FLAG_DOUBLE_SIDED);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "no_depth_test"), "set_draw_flag", "get_draw_flag", FLAG_DISABLE_DEPTH_TEST);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "fixed_size"), "set_draw_flag", "get_draw_flag", FLAG_FIXED_SIZE);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "alpha_cut", PROPERTY_HINT_ENUM, "Disabled,Discard,Opaque Pre-Pass,Alpha Hash"), "set_alpha_cut_mode", "get_alpha_cut_mode");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "alpha_scissor_threshold", PROPERTY_HINT_RANGE, "0,1,0.001"), "set_alpha_scissor_threshold", "get_alpha_scissor_threshold");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "alpha_hash_scale", PROPERTY_HINT_RANGE, "0,2,0.01"), "set_alpha_hash_scale", "get_alpha_hash_scale");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "alpha_antialiasing_mode", PROPERTY_HINT_ENUM, "Disabled,Alpha Edge Blend,Alpha Edge Clip"), "set_alpha_antialiasing", "get_alpha_antialiasing");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "alpha_antialiasing_edge", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_alpha_antialiasing_edge", "get_alpha_antialiasing_edge");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "texture_filter", PROPERTY_HINT_ENUM, "Nearest,Linear,Nearest Mipmap,Linear Mipmap,Nearest Mipmap Anisotropic,Linear Mipmap Anisotropic"), "set_texture_filter", "get_texture_filter");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "render_priority", PROPERTY_HINT_RANGE, itos(RS::MATERIAL_RENDER_PRIORITY_MIN) + "," + itos(RS::MATERIAL_RENDER_PRIORITY_MAX) + ",1"), "set_render_priority", "get_render_priority");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "outline_render_priority", PROPERTY_HINT_RANGE, itos(RS::MATERIAL_RENDER_PRIORITY_MIN) + "," + itos(RS::MATERIAL_RENDER_PRIORITY_MAX) + ",1"), "set_outline_render_priority", "get_outline_render_priority");

	// ADD_GROUP("BiDi", "");
	// ADD_PROPERTY(PropertyInfo(Variant::INT, "text_direction", PROPERTY_HINT_ENUM, "Auto,Left-to-Right,Right-to-Left,Inherited"), "set_text_direction", "get_text_direction");
	// ADD_PROPERTY(PropertyInfo(Variant::STRING, "language", PROPERTY_HINT_LOCALE_ID, ""), "set_language", "get_language");
	// ADD_PROPERTY(PropertyInfo(Variant::INT, "structured_text_bidi_override", PROPERTY_HINT_ENUM, "Default,URI,File,Email,List,None,Custom"), "set_structured_text_bidi_override", "get_structured_text_bidi_override");
	// ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "structured_text_bidi_override_options"), "set_structured_text_bidi_override_options", "get_structured_text_bidi_override_options");

	ADD_SIGNAL(MethodInfo("meta_clicked", PropertyInfo(Variant::NIL, "meta", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT)));
	ADD_SIGNAL(MethodInfo("meta_hover_started", PropertyInfo(Variant::NIL, "meta", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT)));
	ADD_SIGNAL(MethodInfo("meta_hover_ended", PropertyInfo(Variant::NIL, "meta", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT)));

	ADD_SIGNAL(MethodInfo("finished"));

	BIND_ENUM_CONSTANT(LIST_NUMBERS);
	BIND_ENUM_CONSTANT(LIST_LETTERS);
	BIND_ENUM_CONSTANT(LIST_ROMAN);
	BIND_ENUM_CONSTANT(LIST_DOTS);

	BIND_ENUM_CONSTANT(MENU_COPY);
	BIND_ENUM_CONSTANT(MENU_SELECT_ALL);
	BIND_ENUM_CONSTANT(MENU_MAX);

	BIND_ENUM_CONSTANT(META_UNDERLINE_NEVER);
	BIND_ENUM_CONSTANT(META_UNDERLINE_ALWAYS);
	BIND_ENUM_CONSTANT(META_UNDERLINE_ON_HOVER);

	BIND_BITFIELD_FLAG(UPDATE_TEXTURE);
	BIND_BITFIELD_FLAG(UPDATE_SIZE);
	BIND_BITFIELD_FLAG(UPDATE_COLOR);
	BIND_BITFIELD_FLAG(UPDATE_ALIGNMENT);
	BIND_BITFIELD_FLAG(UPDATE_REGION);
	BIND_BITFIELD_FLAG(UPDATE_PAD);
	BIND_BITFIELD_FLAG(UPDATE_TOOLTIP);
	BIND_BITFIELD_FLAG(UPDATE_WIDTH_IN_PERCENT);
}

TextServer::VisibleCharactersBehavior RichTextLabel3D::get_visible_characters_behavior() const {
	return visible_chars_behavior;
}


void RichTextLabel3D::set_render_priority(int p_priority) {
	ERR_FAIL_COND(p_priority < RS::MATERIAL_RENDER_PRIORITY_MIN || p_priority > RS::MATERIAL_RENDER_PRIORITY_MAX);
	if (render_priority != p_priority) {
		render_priority = p_priority;
		queue_redraw();
	}
}

int RichTextLabel3D::get_render_priority() const {
	return render_priority;
}

void RichTextLabel3D::set_outline_render_priority(int p_priority) {
	ERR_FAIL_COND(p_priority < RS::MATERIAL_RENDER_PRIORITY_MIN || p_priority > RS::MATERIAL_RENDER_PRIORITY_MAX);
	if (outline_render_priority != p_priority) {
		outline_render_priority = p_priority;
		queue_redraw();
	}
}

int RichTextLabel3D::get_outline_render_priority() const {
	return outline_render_priority;
}

void RichTextLabel3D::set_outline_size(int p_size) {
	if (outline_size != p_size) {
		outline_size = p_size;
		queue_redraw();
	}
}

void RichTextLabel3D::set_outline_depth(float p_depth) {
	if (outline_depth != p_depth) {
		outline_depth = p_depth;
		queue_redraw();
	}
}

float RichTextLabel3D::get_outline_depth() const {
	return outline_depth;
}

void RichTextLabel3D::set_cell_children_depth(float p_depth) {
	if (cell_children_depth != p_depth) {
		cell_children_depth = p_depth;
		queue_redraw();
	}
}

float RichTextLabel3D::get_cell_children_depth() const {
	return cell_children_depth;
}

int RichTextLabel3D::get_outline_size() const {
	return outline_size;
}

void RichTextLabel3D::set_modulate(const Color &p_color) {
	if (modulate != p_color) {
		modulate = p_color;
		queue_redraw();
	}
}

Color RichTextLabel3D::get_modulate() const {
	return modulate;
}

void RichTextLabel3D::set_outline_modulate(const Color &p_color) {
	if (outline_modulate != p_color) {
		outline_modulate = p_color;
		queue_redraw();
	}
}

Color RichTextLabel3D::get_outline_modulate() const {
	return outline_modulate;
}

void RichTextLabel3D::set_visible_characters_behavior(TextServer::VisibleCharactersBehavior p_behavior) {
	if (visible_chars_behavior != p_behavior) {
		_stop_thread();

		visible_chars_behavior = p_behavior;
		main->first_invalid_line.store(0); // Invalidate all lines.
		_validate_line_caches();
		queue_redraw();
	}
}

void RichTextLabel3D::set_visible_characters(int p_visible) {
	if (visible_characters != p_visible) {
		_stop_thread();

		int prev_vc = visible_characters;
		visible_characters = p_visible;
		if (p_visible == -1) {
			visible_ratio = 1;
		} else {
			int total_char_count = get_total_character_count();
			if (total_char_count > 0) {
				visible_ratio = (float)p_visible / (float)total_char_count;
			}
		}
		if (visible_chars_behavior == TextServer::VC_CHARS_BEFORE_SHAPING && visible_characters != prev_vc) {
			int new_vc = (visible_characters < 0) ? get_total_character_count() : visible_characters;
			int old_vc = (prev_vc < 0) ? get_total_character_count() : prev_vc;
			int to_line = main->first_invalid_line.load();
			int old_from_l = to_line;
			int new_from_l = to_line;
			for (int i = 0; i < to_line; i++) {
				const Line &l = main->lines[i];
				if (l.char_offset <= old_vc && l.char_offset + l.char_count > old_vc) {
					old_from_l = i;
				}
				if (l.char_offset <= new_vc && l.char_offset + l.char_count > new_vc) {
					new_from_l = i;
				}
			}
			Rect2 text_rect = _get_text_rect();
			int first_invalid = MIN(new_from_l, old_from_l);
			int second_invalid = MAX(new_from_l, old_from_l);

			float total_height = (first_invalid == 0) ? 0 : _calculate_line_vertical_offset(main->lines[first_invalid - 1]);
			if (first_invalid < to_line) {
				int total_chars = main->lines[first_invalid].char_offset;
				total_height = _shape_line(main, first_invalid, _get_font_or_default(), font_size, text_rect.get_size().width, total_height, &total_chars);
			}
			if (first_invalid != second_invalid) {
				for (int i = first_invalid + 1; i < second_invalid; i++) {
					main->lines[i].offset.y = total_height;
					total_height = _calculate_line_vertical_offset(main->lines[i]);
				}
				if (second_invalid < to_line) {
					int total_chars = main->lines[second_invalid].char_offset;
					total_height = _shape_line(main, second_invalid, _get_font_or_default(), font_size, text_rect.get_size().width, total_height, &total_chars);
				}
			}
			for (int i = second_invalid + 1; i < to_line; i++) {
				main->lines[i].offset.y = total_height;
				total_height = _calculate_line_vertical_offset(main->lines[i]);
			}
		}
		
		queue_redraw();
	}
}

int RichTextLabel3D::get_visible_characters() const {
	return visible_characters;
}

int RichTextLabel3D::get_character_line(int p_char) {
	_validate_line_caches();

	int line_count = 0;
	int to_line = main->first_invalid_line.load();
	for (int i = 0; i < to_line; i++) {
		MutexLock lock(main->lines[i].text_buf->get_mutex());
		int char_offset = main->lines[i].char_offset;
		int char_count = main->lines[i].char_count;
		if (char_offset <= p_char && p_char < char_offset + char_count) {
			int lc = main->lines[i].text_buf->get_line_count();
			for (int j = 0; j < lc; j++) {
				Vector2i range = main->lines[i].text_buf->get_line_range(j);
				if (char_offset + range.x <= p_char && p_char < char_offset + range.y) {
					break;
				}
				if (char_offset + range.x > p_char && line_count > 0) {
					line_count--; // Character is not rendered and is between the lines (e.g., edge space).
					break;
				}
				if (j != lc - 1) {
					line_count++;
				}
			}
			return line_count;
		} else {
			line_count += main->lines[i].text_buf->get_line_count();
		}
	}
	return -1;
}

int RichTextLabel3D::get_character_paragraph(int p_char) {
	_validate_line_caches();

	int to_line = main->first_invalid_line.load();
	for (int i = 0; i < to_line; i++) {
		int char_offset = main->lines[i].char_offset;
		if (char_offset <= p_char && p_char < char_offset + main->lines[i].char_count) {
			return i;
		}
	}
	return -1;
}

int RichTextLabel3D::get_total_character_count() const {
	// Note: Do not use line buffer "char_count", it includes only visible characters.
	int tc = 0;
	Item *it = main;
	while (it) {
		if (it->type == ITEM_TEXT) {
			ItemText *t = static_cast<ItemText *>(it);
			tc += t->text.length();
		} else if (it->type == ITEM_NEWLINE) {
			tc++;
		} else if (it->type == ITEM_IMAGE) {
			tc++;
		}
		it = _get_next_item(it, true);
	}
	return tc;
}

int RichTextLabel3D::get_total_glyph_count() const {
	const_cast<RichTextLabel3D *>(this)->_validate_line_caches();

	int tg = 0;
	Item *it = main;
	while (it) {
		if (it->type == ITEM_FRAME) {
			ItemFrame *f = static_cast<ItemFrame *>(it);
			for (int i = 0; i < (int)f->lines.size(); i++) {
				MutexLock lock(f->lines[i].text_buf->get_mutex());
				tg += TS->shaped_text_get_glyph_count(f->lines[i].text_buf->get_rid());
			}
		}
		it = _get_next_item(it, true);
	}

	return tg;
}

Dictionary RichTextLabel3D::parse_expressions_for_values(Vector<String> p_expressions) {
	Dictionary d;
	for (int i = 0; i < p_expressions.size(); i++) {
		Array a;
		Vector<String> parts = p_expressions[i].split("=", true);
		const String &key = parts[0];
		if (parts.size() != 2) {
			return d;
		}

		Vector<String> values = parts[1].split(",", false);

#ifdef MODULE_REGEX_ENABLED
		RegEx color = RegEx();
		color.compile("^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$");
		RegEx nodepath = RegEx();
		nodepath.compile("^\\$");
		RegEx boolean = RegEx();
		boolean.compile("^(true|false)$");
		RegEx numerical = RegEx();
		numerical.compile("^[-+]?\\d+$");
		RegEx decimal = RegEx();
		decimal.compile("^[+-]?\\d*(\\.\\d*)?([eE][+-]?\\d+)?$");

		for (int j = 0; j < values.size(); j++) {
			if (color.search(values[j]).is_valid()) {
				a.append(Color::html(values[j]));
			} else if (nodepath.search(values[j]).is_valid()) {
				if (values[j].begins_with("$")) {
					String v = values[j].substr(1);
					a.append(NodePath(v));
				}
			} else if (boolean.search(values[j]).is_valid()) {
				if (values[j] == "true") {
					a.append(true);
				} else if (values[j] == "false") {
					a.append(false);
				}
			} else if (numerical.search(values[j]).is_valid()) {
				a.append(values[j].to_int());
			} else if (decimal.search(values[j]).is_valid()) {
				a.append(values[j].to_float());
			} else {
				a.append(values[j]);
			}
		}
#endif

		if (values.size() > 1) {
			d[key] = a;
		} else if (values.size() == 1) {
			d[key] = a[0];
		}
	}
	return d;
}


AABB RichTextLabel3D::get_aabb() const {
	return aabb;
}

Ref<TriangleMesh> RichTextLabel3D::generate_triangle_mesh() const {
	if (triangle_mesh.is_valid()) {
		return triangle_mesh;
	}

	if (main == nullptr) {
		return Ref<TriangleMesh>();
	}

	Vector<Vector3> faces;
	faces.resize(6);
	Vector3 *facesw = faces.ptrw();

	// Calculate total bounds by iterating through all frames and lines
	float total_h = 0.0;
	float max_line_w = 0.0;
	
	Rect2 content_bounds;
	bool first_line = true;

	// Helper function to process a frame recursively
	std::function<void(ItemFrame*)> process_frame = [&](ItemFrame* frame) {
		if (frame == nullptr) return;

		for (int line_idx = 0; line_idx < (int)frame->lines.size(); line_idx++) {
			Line &line = frame->lines[line_idx];
			if (line.text_buf.is_null()) continue;

			MutexLock lock(line.text_buf->get_mutex());

			// Get line dimensions
			for (int text_line = 0; text_line < line.text_buf->get_line_count(); text_line++) {
				Size2 line_size = line.text_buf->get_line_size(text_line);
				float line_width = line_size.x;
				float line_height = line.text_buf->get_line_ascent(text_line) + line.text_buf->get_line_descent(text_line);

				max_line_w = MAX(max_line_w, line_width);

				// Calculate line position based on alignment
				Vector2 line_pos = Vector2(line.offset.x, total_h);
				
				// Apply horizontal alignment offset
				float width_offset = 0;
				switch (line.text_buf->get_alignment()) {
					case HORIZONTAL_ALIGNMENT_CENTER:
						width_offset = (width - line_width) / 2.0;
						break;
					case HORIZONTAL_ALIGNMENT_RIGHT:
						width_offset = width - line_width;
						break;
					case HORIZONTAL_ALIGNMENT_FILL:
					case HORIZONTAL_ALIGNMENT_LEFT:
					default:
						width_offset = 0;
						break;
				}
				line_pos.x += width_offset;

				// Expand content bounds
				Rect2 line_rect = Rect2(line_pos, Size2(line_width, line_height));
				if (first_line) {
					content_bounds = line_rect;
					first_line = false;
				} else {
					content_bounds = content_bounds.merge(line_rect);
				}

				total_h += line_height + line_spacing;
			}
		}

		// Process child frames (for nested elements like tables, etc.)
		for (int i = 0; i < frame->subitems.size(); i++) {
			if (frame->subitems.get(i)->type == ITEM_FRAME) {
				process_frame(static_cast<ItemFrame*>(frame->subitems.get(i)));
			}
		}
	};

	// Process the main frame
	process_frame(main);

	// If no content, return empty mesh
	if (first_line || content_bounds.size.x <= 0 || content_bounds.size.y <= 0) {
		return Ref<TriangleMesh>();
	}

	// Apply vertical alignment
	float vbegin = 0;
	switch (vertical_alignment) {
		case VERTICAL_ALIGNMENT_FILL:
		case VERTICAL_ALIGNMENT_TOP: {
			// Nothing.
		} break;
		case VERTICAL_ALIGNMENT_CENTER: {
			vbegin = (total_h - line_spacing) / 2.0;
		} break;
		case VERTICAL_ALIGNMENT_BOTTOM: {
			vbegin = total_h - line_spacing;
		} break;
	}

	// Apply horizontal alignment to the overall content
	Vector2 offset = Vector2(0, vbegin);
	switch (default_alignment) {
		case HORIZONTAL_ALIGNMENT_LEFT:
			break;
		case HORIZONTAL_ALIGNMENT_FILL:
		case HORIZONTAL_ALIGNMENT_CENTER: {
			offset.x = (-max_line_w) / 2.0;
		} break;
		case HORIZONTAL_ALIGNMENT_RIGHT: {
			offset.x = -max_line_w;
		} break;
	}

	// Create final rectangle
	Rect2 final_rect = Rect2(
		content_bounds.position + offset,
		Size2(MAX(content_bounds.size.x, max_line_w), total_h)
	);

	// Ensure minimum size
	if (final_rect.size.x <= 0 || final_rect.size.y <= 0) {
		final_rect.size = Size2(10, 10); // Minimum gizmo size
	}

	real_t px_size = get_pixel_size();

	// Create vertices for the bounding rectangle
	Vector2 vertices[4] = {
		(final_rect.position + Vector2(0, -final_rect.size.y)) * px_size,           // Bottom-left
		(final_rect.position + Vector2(final_rect.size.x, -final_rect.size.y)) * px_size, // Bottom-right
		(final_rect.position + Vector2(final_rect.size.x, 0)) * px_size,           // Top-right
		final_rect.position * px_size,                                              // Top-left
	};

	// Triangle indices for two triangles forming a quad
	static const int indices[6] = {
		0, 1, 2,  // First triangle
		0, 2, 3   // Second triangle
	};

	// Create face vertices
	for (int j = 0; j < 6; j++) {
		int i = indices[j];
		Vector3 vtx;
		vtx[0] = vertices[i][0];
		vtx[1] = vertices[i][1];
		vtx[2] = 0.0f; // Z is 0 for flat label
		facesw[j] = vtx;
	}

	triangle_mesh.instantiate();
	triangle_mesh->create(faces);

	return triangle_mesh;
}


void RichTextLabel3D::set_pixel_size(real_t p_amount) {
	if (pixel_size != p_amount) {
		pixel_size = p_amount;
		queue_redraw();
	}
}

real_t RichTextLabel3D::get_pixel_size() const {
	return pixel_size;
}

RichTextLabel3D::RichTextLabel3D(const String &p_text) {
	for (int i = 0; i < FLAG_MAX; i++) {
		flags[i] = (i == FLAG_DOUBLE_SIDED);
	}

	mesh = RenderingServer::get_singleton()->mesh_create();

	// Disable shadow casting by default to improve performance and avoid unintended visual artifacts.
	set_cast_shadows_setting(SHADOW_CASTING_SETTING_OFF);

	// Label3D can't contribute to GI in any way, so disable it to improve performance.
	set_gi_mode(GI_MODE_DISABLED);

	set_base(mesh);

	main = memnew(ItemFrame);
	main->owner = get_instance_id();
	main->rid = items.make_rid(main);
	main->index = 0;
	current = main;
	main->lines.resize(1);
	main->lines[0].from = main;
	main->first_invalid_line.store(0);
	main->first_resized_line.store(0);
	main->first_invalid_font_line.store(0);
	current_frame = main;



	set_text(p_text);
	updating.store(false);
	validating.store(false);
	stop_thread.store(false);
	parsing_bbcode.store(false);
}

RichTextLabel3D::~RichTextLabel3D() {
	_stop_thread();
	
	items.free(main->rid);
	memdelete(main);

	ERR_FAIL_NULL(RenderingServer::get_singleton());
	RenderingServer::get_singleton()->free(mesh);
	for (KeyValue<SurfaceKey, SurfaceData> E : surfaces) {
		RenderingServer::get_singleton()->free(E.value.material);
	}
	surfaces.clear();

}
