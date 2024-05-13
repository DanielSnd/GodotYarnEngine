//
// Created by Daniel on 2024-03-11.
//

#ifndef ASSETPLACER_H
#define ASSETPLACER_H
#include "yscenespawner3d_gizmo_plugin.h"
#if TOOLS_ENABLED
#include "scene/resources/style_box_flat.h"
#include "YSpecialPoint3DGizmoPlugin.h"
#include "editor/plugins/node_3d_editor_plugin.h"
#include "editor/themes/editor_scale.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/gui/texture_rect.h"
#include "editor/editor_settings.h"
#include "scene/gui/slider.h"
#include "scene/gui/button.h"
#include "editor/editor_plugin.h"
#include "scene/gui/box_container.h"
#include "scene/gui/item_list.h"
#include "editor/editor_node.h"
#include "editor/gui/editor_bottom_panel.h"
#include "editor/editor_resource_preview.h"
#include "scene/gui/margin_container.h"
#include "scene/gui/check_button.h"
#include "scene/gui/separator.h"
#include "scene/main/viewport.h"
#include "scene/gui/box_container.h"
#include "scene/resources/style_box_flat.h"
#include "scene/gui/panel_container.h"
#include "scene/resources/environment.h"
#include "scene/resources/3d/world_3d.h"
#include "scene/3d/light_3d.h"
#include "scene/3d/camera_3d.h"
#include "scene/resources/portable_compressed_texture.h"
#include "core/object/undo_redo.h"
#include "editor/editor_undo_redo_manager.h"
#include "editor/project_settings_editor.h"
#include "scene/resources/packed_scene.h"
#include "scene/resources/3d/primitive_meshes.h"
#include "editor/editor_undo_redo_manager.h"
#include "../yarnphysics.h"
#include "core/math/random_number_generator.h"
#include "editor/editor_interface.h"
#include "editor/filesystem_dock.h"
#include "editor/editor_properties.h"
class YSpecialPoint3D;
namespace yarnengine {
    class AssetPanelEditorBottomPanel;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class AsssetPanelItemList : public ItemList {
    GDCLASS(AsssetPanelItemList, ItemList);

    protected:
        AssetPanelEditorBottomPanel* bottom_panel_owner;
        Range *zoom;

    public:

        Variant get_drag_data(const Point2 &p_point) override;
        void set_bottom_panel_owner(AssetPanelEditorBottomPanel* new_owner) {bottom_panel_owner = new_owner;}
        virtual bool can_drop_data(const Point2 &p_point, const Variant &p_data) const override;
        virtual void drop_data(const Point2 &p_point, const Variant &p_data) override;

        void _zoom_changed(double p_value);
    	Range *get_zoom() const { return zoom; }
        void set_zoom(Range *p_zoom) {
            zoom = p_zoom;
            zoom->connect("value_changed", callable_mp(this, &AsssetPanelItemList::_zoom_changed));
    	    set_fixed_icon_size(Size2i(zoom->get_value(),zoom->get_value()));
        }
    };


class AssetPanelEditorBottomPanel : public VBoxContainer {
    GDCLASS(AssetPanelEditorBottomPanel, VBoxContainer);

private:
    EditorUndoRedoManager *undo_redo;
protected:
    void changed_min_randomize_scale(float new_min_scale_randomize_value);

    void changed_max_randomize_scale(float new_max_scale_randomize_value);

    void changed_randomize_scale(bool new_scale_randomize_value);

	static void _bind_methods();
    // HBoxContainer* top_container;
    // Label* debug_label;
    // HSlider* brush_slider;
    // HSlider* brush_strength_slider;
    // Label* brush_slider_label;
    // Label* brush_strength_slider_label;
    void _notification(int p_what);


    void handle_preview_generating_queue();

    String get_asset_name_from_resource_path(const String &p_res_path);

    Button* delete_pscene = nullptr;
    Button* save_pscene_list = nullptr;
    AsssetPanelItemList* item_list = nullptr;
    HSlider *zoom = nullptr;
    HSlider *radius = nullptr;
    HSlider *place_amount = nullptr;
	PopupMenu *tree_popup = nullptr;
    Label* place_amount_label = nullptr;
    Label* place_radius_label = nullptr;
    Label* lock_to_layer_text = nullptr;
    Label* debug_text_label = nullptr;
    SpinBox* random_scale_min = nullptr;
    SpinBox* random_scale_max = nullptr;
    SpinBox* offset_spin_box_x = nullptr;
    SpinBox* offset_spin_box_y = nullptr;
    SpinBox* offset_spin_box_z = nullptr;
    CheckButton* enabled_button = nullptr;
    CheckButton* align_with_check_button = nullptr;
    CheckButton* randomize_scale = nullptr;
    CheckButton* custom_offset = nullptr;
    CheckButton* randomize_y_scale = nullptr;
    CheckButton* rotate_y_check_button = nullptr;
    CheckButton* rotate_z_check_button = nullptr;
	TextureRect *zoom_icon = nullptr;
    Environment *preview_environment = nullptr;
    World3D *preview_world3d = nullptr;
    SubViewport *preview_subviewport = nullptr;
    DirectionalLight3D *preview_directional_light = nullptr;
    Camera3D* preview_camera_3d = nullptr;
    EditorPropertyLayersGrid* lock_to_ground_layers = nullptr;

    struct QueueItem {
        Ref<Resource> resource;
        String path;
        ObjectID id;
        StringName function;
        Variant userdata;
    };
	List<QueueItem> queue;

    void finished_rendering_preview(Node3D* node_insantiated, const String &q_item_path,ObjectID q_item_id, const String &q_item_function, const Variant &q_item_userdata);
    bool is_rendering_preview=false;
public:

    uint32_t ground_layer = 0;
    void add_packedscene(const String& scene_path);

    void generate_preview_image(const String &p_path, Object *p_receiver, const StringName &p_receiver_func, const Variant &p_userdata);

    void _itemlist_thumbnail_done(const String &p_path, const Ref<Texture2D> &p_preview,
                                  const Ref<Texture2D> &p_small_preview, const Variant &p_udata);

    virtual bool can_drop_data(const Point2 &p_point, const Variant &p_data) const override;
    virtual void drop_data(const Point2 &p_point, const Variant &p_data) override;
    void set_debug_text(const String &p_debug_text) {
       // debug_text_label->set_text(p_debug_text);
    }
    void _delete_pressed();
    void _save_list_pressed();

    bool has_item_selected() const {
        return item_list->is_anything_selected();
    }

    void deselect_all_items() {
        if (item_list->is_anything_selected()) {
            item_list->deselect_all();
            current_activated_item_i = -1;
            if (current_activated_pscene.is_valid())
                current_activated_pscene.unref();
            current_activated_item_path = "";
            if (current_cativated_item_mesh.is_valid())
                current_cativated_item_mesh.unref();
            emit_signal("deselected_pscene");
        }
    }

    Vector<String> all_selected_paths() {
        //Vector<int> get_selected_items();
        Vector<String> returning_paths;
        if (item_list != nullptr) {
            auto _selected_items = item_list->get_selected_items();
            for (auto _selected_item: _selected_items) {
                returning_paths.append(item_list->get_item_metadata(_selected_item));
            }
        }
        return returning_paths;
    }
    bool should_be_enabled() {
        return enabled_button != nullptr && enabled_button->is_pressed();
    }
    bool should_align_to_ground() {
        return align_with_check_button != nullptr && align_with_check_button->is_pressed();
    }
    bool should_random_y_scale() {
        return randomize_y_scale != nullptr && randomize_y_scale->is_pressed();
    }

    bool should_randomize_scale() {
        return randomize_scale != nullptr && randomize_scale->is_pressed();
    }

    bool should_random_y_rotate() {
        return rotate_y_check_button != nullptr && rotate_y_check_button->is_pressed();
    }

    bool should_random_z_rotate() {
        return rotate_z_check_button != nullptr && rotate_z_check_button->is_pressed();
    }

    Vector2 get_min_max_random_scale() {
        return random_scale_min != nullptr && random_scale_max != nullptr ? Vector2(random_scale_min->get_value(),random_scale_max->get_value()): Vector2(1.0,1.0);
    }

    void _item_clicked(int p_item, const Vector2 &p_pos, MouseButton p_mouse_button_index);

    void _empty_clicked(const Vector2 &p_pos, MouseButton p_mouse_button_index);

    void _item_selected(int index_selected);

    void _multi_selected(int p_index, bool p_selected);

    void _item_activated(int index_activated);

    void show_in_filesystem();

    void changed_one_of_the_other_randomizers(bool new_value);

    void _changed_place_amount(float new_amount = 0.0) {
        place_amount_label->set_text(vformat("Place Amount: %d",static_cast<int>(place_amount->get_value())));
        if (radius != nullptr) {
            auto parent_node = cast_to<Control>(radius->get_parent());
            if (parent_node != nullptr) {
                parent_node->set_visible(new_amount > 1);
            }
        }
    }

    void _changed_place_radius(float new_radius = 0.0) {
        place_radius_label->set_text(vformat("Place Radius: %.2f",static_cast<int>(radius->get_value())));
    }

    float get_radius() {
        if (radius != nullptr) {
            return radius->get_value();
        }
        return 1.0;
    }
    int get_amount() {
        if (place_amount != nullptr) {
            return static_cast<int>(place_amount->get_value());
        }
        return 1;
    }
    Vector3 get_offset() {
        if (offset_spin_box_x != nullptr && offset_spin_box_y != nullptr && offset_spin_box_z != nullptr) {
            return Vector3(offset_spin_box_x->get_value(),offset_spin_box_y->get_value(),offset_spin_box_z->get_value());
        }
        return Vector3(0.0,0.0,0.0);
    }

    void _grid_changed(uint32_t p_grid);

    String current_activated_item_path;
    int current_activated_item_i = -1;
    Ref<Mesh> current_cativated_item_mesh;
    Ref<PackedScene> current_activated_pscene;
    AssetPanelEditorBottomPanel() {
        HBoxContainer* side_by_side_top = memnew(HBoxContainer);
        add_child(side_by_side_top);
        HBoxContainer* underneath_hbox = memnew(HBoxContainer);
        underneath_hbox->set_anchors_preset(LayoutPreset::PRESET_FULL_RECT);
        save_pscene_list = memnew(Button);
        save_pscene_list->set_theme_type_variation("FlatButton");
        side_by_side_top->add_child(save_pscene_list);

        save_pscene_list->connect("pressed", callable_mp(this,&AssetPanelEditorBottomPanel::_save_list_pressed));
        delete_pscene = memnew(Button);
        delete_pscene->set_theme_type_variation("FlatButton");
        side_by_side_top->add_child(delete_pscene);

        debug_text_label = memnew(Label);
        side_by_side_top->add_child(debug_text_label);

        add_child(memnew(HSeparator));

        PanelContainer* left_panel_container = memnew(PanelContainer);
        Ref<StyleBoxFlat> back_flat_styelbox;
        back_flat_styelbox.instantiate();
        back_flat_styelbox->set_bg_color(Color(0.2,0.2,0.2,0.45));
        left_panel_container->add_theme_style_override("panel",back_flat_styelbox);

        VBoxContainer* options_vbox = memnew(VBoxContainer);
        options_vbox->set_custom_minimum_size(Size2(380.0,0.0));
        add_child(underneath_hbox);
        underneath_hbox->add_child(left_panel_container);
        MarginContainer* margin_container = memnew(MarginContainer);
        left_panel_container->add_child(margin_container);
        margin_container->add_theme_constant_override("margin_left",4);
        margin_container->add_theme_constant_override("margin_right",4);
        margin_container->add_theme_constant_override("margin_top",4);
        margin_container->add_theme_constant_override("margin_bottom",4);
        margin_container->add_child(options_vbox);


        HBoxContainer* place_amouunt_hbox = memnew(HBoxContainer);
        place_amouunt_hbox->set_anchors_preset(LayoutPreset::PRESET_VCENTER_WIDE);
        options_vbox->add_child(place_amouunt_hbox);
        place_amount_label = memnew(Label);
        place_amount_label->set_text("Place Amount:");
        place_amouunt_hbox->add_child(place_amount_label);

        // Wide empty separation control. (like BoxContainer::add_spacer())
        Control *amount_label_emptyc = memnew(Control);
        amount_label_emptyc->set_mouse_filter(MOUSE_FILTER_PASS);
        amount_label_emptyc->set_h_size_flags(SIZE_EXPAND_FILL);
        place_amouunt_hbox->add_child(amount_label_emptyc);

        place_amount = memnew(HSlider);
        place_amount->set_v_size_flags(SIZE_EXPAND_FILL);
        place_amount->set_step(1.0);
        place_amount->set_min(1.0);
        place_amount->set_max(50.0);
        place_amount->set_value(1.0);
        place_amount->set_custom_minimum_size(Size2(200, 0) * EDSCALE);
        place_amount->connect("value_changed", callable_mp(this, &AssetPanelEditorBottomPanel::_changed_place_amount));
        _changed_place_amount(1);
        place_amouunt_hbox->add_child(place_amount);


        HBoxContainer* place_radius_hbox = memnew(HBoxContainer);
        place_radius_hbox->set_anchors_preset(LayoutPreset::PRESET_VCENTER_WIDE);
        options_vbox->add_child(place_radius_hbox);
        place_radius_label = memnew(Label);
        place_radius_label->set_text("Radius:");
        place_radius_hbox->add_child(place_radius_label);

        // Wide empty separation control. (like BoxContainer::add_spacer())
        Control *radius_label_emptyc = memnew(Control);
        radius_label_emptyc->set_mouse_filter(MOUSE_FILTER_PASS);
        radius_label_emptyc->set_h_size_flags(SIZE_EXPAND_FILL);
        place_radius_hbox->add_child(radius_label_emptyc);

        radius = memnew(HSlider);
        radius->set_v_size_flags(SIZE_EXPAND_FILL);
        radius->set_step(0.1);
        radius->set_min(1.0);
        radius->set_max(100.0);
        radius->set_value(1.0);
        radius->set_custom_minimum_size(Size2(200, 0) * EDSCALE);
        radius->connect("value_changed", callable_mp(this, &AssetPanelEditorBottomPanel::_changed_place_radius));
        _changed_place_radius();
        place_radius_hbox->add_child(radius);
        place_radius_hbox->set_visible(place_amount->get_value() > 1.01);

        options_vbox->add_child(memnew(VSeparator));

        HBoxContainer* enabled_and_align_hbox = memnew(HBoxContainer);
        enabled_and_align_hbox->set_anchors_preset(LayoutPreset::PRESET_VCENTER_WIDE);
        options_vbox->add_child(enabled_and_align_hbox);

        enabled_button = memnew(CheckButton);
        enabled_button->set_text("Enabled");
        enabled_and_align_hbox->add_child(enabled_button);
        enabled_button->connect("toggled",callable_mp(this,&AssetPanelEditorBottomPanel::changed_one_of_the_other_randomizers));

        // Wide empty separation control. (like BoxContainer::add_spacer())
        Control *enabled_and_align_emptyc = memnew(Control);
        enabled_and_align_emptyc->set_mouse_filter(MOUSE_FILTER_PASS);
        enabled_and_align_emptyc->set_h_size_flags(SIZE_EXPAND_FILL);
        enabled_and_align_hbox->add_child(enabled_and_align_emptyc);

        align_with_check_button = memnew(CheckButton);
        align_with_check_button->set_text("Align With Ground");
        enabled_and_align_hbox->add_child(align_with_check_button);
        if (ProjectSettings::get_singleton()->has_setting("assetplacer/align_with_ground")) {
            align_with_check_button->set_pressed(GLOBAL_GET("assetplacer/align_with_ground"));
        }
        align_with_check_button->connect("toggled",callable_mp(this,&AssetPanelEditorBottomPanel::changed_one_of_the_other_randomizers));

        HBoxContainer* rotate_y_x_hbox = memnew(HBoxContainer);
        rotate_y_x_hbox->set_anchors_preset(LayoutPreset::PRESET_VCENTER_WIDE);
        options_vbox->add_child(rotate_y_x_hbox);
        rotate_y_check_button = memnew(CheckButton);
        rotate_y_check_button->set_text("Random Rotate Y");
        rotate_y_x_hbox->add_child(rotate_y_check_button);
        if (ProjectSettings::get_singleton()->has_setting("assetplacer/randomize_rotate_y")) {
            rotate_y_check_button->set_pressed(GLOBAL_GET("assetplacer/randomize_rotate_y"));
        }
        rotate_y_check_button->connect("toggled",callable_mp(this,&AssetPanelEditorBottomPanel::changed_one_of_the_other_randomizers));

        // Wide empty separation control. (like BoxContainer::add_spacer())
        Control *randomizeers_rotators_emptyc = memnew(Control);
        randomizeers_rotators_emptyc->set_mouse_filter(MOUSE_FILTER_PASS);
        randomizeers_rotators_emptyc->set_h_size_flags(SIZE_EXPAND_FILL);
        rotate_y_x_hbox->add_child(randomizeers_rotators_emptyc);

        rotate_z_check_button = memnew(CheckButton);
        rotate_z_check_button->set_text("Random Rotate Z");
        rotate_y_x_hbox->add_child(rotate_z_check_button);
        if (ProjectSettings::get_singleton()->has_setting("assetplacer/randomize_rotate_z")) {
            rotate_z_check_button->set_pressed(GLOBAL_GET("assetplacer/randomize_rotate_z"));
        }
        rotate_z_check_button->connect("toggled",callable_mp(this,&AssetPanelEditorBottomPanel::changed_one_of_the_other_randomizers));


        HBoxContainer* randomize_scales_hbox = memnew(HBoxContainer);
        randomize_scales_hbox->set_anchors_preset(LayoutPreset::PRESET_VCENTER_WIDE);
        options_vbox->add_child(randomize_scales_hbox);

        randomize_scale = memnew(CheckButton);
        randomize_scale->set_text("Random Scale");
        randomize_scales_hbox->add_child(randomize_scale);
        if (ProjectSettings::get_singleton()->has_setting("assetplacer/randomize_scale")) {
            randomize_scale->set_pressed(GLOBAL_GET("assetplacer/randomize_scale"));
        }
        randomize_scale->connect("toggled",callable_mp(this,&AssetPanelEditorBottomPanel::changed_randomize_scale));

        // Wide empty separation control. (like BoxContainer::add_spacer())
        Control *randomizeers_emptyc = memnew(Control);
        randomizeers_emptyc->set_mouse_filter(MOUSE_FILTER_PASS);
        randomizeers_emptyc->set_h_size_flags(SIZE_EXPAND_FILL);
        randomize_scales_hbox->add_child(randomizeers_emptyc);

        randomize_y_scale = memnew(CheckButton);
        randomize_y_scale->set_text("Random Y Scale");
        randomize_scales_hbox->add_child(randomize_y_scale);
        if (ProjectSettings::get_singleton()->has_setting("assetplacer/randomize_y_scale")) {
            randomize_y_scale->set_pressed(GLOBAL_GET("assetplacer/randomize_y_scale"));
        }
        randomize_y_scale->connect("toggled",callable_mp(this,&AssetPanelEditorBottomPanel::changed_one_of_the_other_randomizers));

        HBoxContainer* min_max_scale_hbox = memnew(HBoxContainer);
        min_max_scale_hbox->set_anchors_preset(LayoutPreset::PRESET_VCENTER_WIDE);
        Label* min_max_scale_label = memnew(Label);
        min_max_scale_label->set_text("Min/Max Scale: ");
        min_max_scale_hbox->add_child(min_max_scale_label);

        // Wide empty separation control. (like BoxContainer::add_spacer())
        Control *min_max_label_emptyc = memnew(Control);
        min_max_label_emptyc->set_mouse_filter(MOUSE_FILTER_PASS);
        min_max_label_emptyc->set_h_size_flags(SIZE_EXPAND_FILL);
        min_max_scale_hbox->add_child(min_max_label_emptyc);

        random_scale_min = memnew(SpinBox);
        random_scale_max = memnew(SpinBox);
        random_scale_min->set_min(0.01);
        random_scale_max->set_min(0.01);
        random_scale_min->set_max(5.00);
        random_scale_max->set_max(5.00);
        random_scale_min->set_step(0.001);
        random_scale_max->set_step(0.001);
        random_scale_min->set_value(1.0);
        random_scale_max->set_value(1.0);
        if (ProjectSettings::get_singleton()->has_setting("assetplacer/min_random_scale")) {
            random_scale_min->set_value(GLOBAL_GET("assetplacer/min_random_scale"));
        }
        if (ProjectSettings::get_singleton()->has_setting("assetplacer/max_random_scale")) {
            random_scale_max->set_value(GLOBAL_GET("assetplacer/max_random_scale"));
        }
        random_scale_min->connect("value_changed",callable_mp(this,&AssetPanelEditorBottomPanel::changed_min_randomize_scale));
        random_scale_max->connect("value_changed",callable_mp(this,&AssetPanelEditorBottomPanel::changed_max_randomize_scale));
        min_max_scale_hbox->add_child(random_scale_min);
        min_max_scale_hbox->add_child(random_scale_max);
        options_vbox->add_child(min_max_scale_hbox);
        if (!randomize_scale->is_pressed())
            min_max_scale_hbox->set_visible(false);

        options_vbox->add_child(memnew(VSeparator));

        HBoxContainer* offset_vector_hbox = memnew(HBoxContainer);
        offset_vector_hbox->set_anchors_preset(LayoutPreset::PRESET_VCENTER_WIDE);
        Label* offset_label = memnew(Label);
        offset_label->set_text("Offset: ");
        offset_vector_hbox->add_child(offset_label);

        // Wide empty separation control. (like BoxContainer::add_spacer())
        Control *offset_label_emptyc = memnew(Control);
        offset_label_emptyc->set_mouse_filter(MOUSE_FILTER_PASS);
        offset_label_emptyc->set_h_size_flags(SIZE_EXPAND_FILL);
        offset_vector_hbox->add_child(offset_label_emptyc);

        offset_spin_box_x = memnew(SpinBox);
        offset_spin_box_y = memnew(SpinBox);
        offset_spin_box_z = memnew(SpinBox);
        offset_spin_box_x->set_min(-10.0);
        offset_spin_box_y->set_min(-10.0);
        offset_spin_box_z->set_min(-10.0);
        offset_spin_box_x->set_max(10.0);
        offset_spin_box_y->set_max(10.0);
        offset_spin_box_z->set_max(10.0);
        offset_spin_box_x->set_step(0.001);
        offset_spin_box_y->set_step(0.001);
        offset_spin_box_z->set_step(0.001);
        offset_vector_hbox->add_child(offset_spin_box_x);
        offset_vector_hbox->add_child(offset_spin_box_y);
        offset_vector_hbox->add_child(offset_spin_box_z);
        options_vbox->add_child(offset_vector_hbox);

        lock_to_layer_text = memnew(Label);
        lock_to_layer_text->set_text("Lock to physics layer: ");
        options_vbox->add_child(lock_to_layer_text);

        lock_to_ground_layers = memnew(EditorPropertyLayersGrid);
        const String p_basename = "layer_names/3d_physics";
        constexpr int p_layer_group_size = 4;
        constexpr int p_layer_count = 32;
        Vector<String> p_collision_names;
        Vector<String> p_tooltips;
        for (int i = 0; i < p_layer_count; i++) {
            String p_name;

            if (ProjectSettings::get_singleton()->has_setting(p_basename + vformat("/layer_%d", i + 1))) {
                p_name = GLOBAL_GET(p_basename + vformat("/layer_%d", i + 1));
            }
            if (p_name.is_empty()) {
                p_name = vformat(TTR("Layer %d"), i + 1);
            }
            p_collision_names.push_back(p_name);
            p_tooltips.push_back(p_name + "\n" + vformat(TTR("Bit %d, value %d"), i, 1 << i));
        }
        lock_to_ground_layers->names = p_collision_names;
        lock_to_ground_layers->tooltips = p_tooltips;
        lock_to_ground_layers->layer_group_size = p_layer_group_size;
        lock_to_ground_layers->layer_count = p_layer_count;
        if (ProjectSettings::get_singleton()->has_setting("assetplacer/lock_to_layer")) {
            ground_layer = GLOBAL_GET("assetplacer/lock_to_layer");
        }
        lock_to_ground_layers->set_flag(ground_layer);
        lock_to_ground_layers->connect("flag_changed", callable_mp(this, &AssetPanelEditorBottomPanel::_grid_changed));
        lock_to_ground_layers->set_h_size_flags(SIZE_EXPAND_FILL);
        //lock_to_ground_layers->custom
        options_vbox->add_child(lock_to_ground_layers);
        lock_to_ground_layers->queue_redraw();


        // HSlider *radius = nullptr;
        // HSlider *quantity = nullptr;
        // HSlider *min_random_scale = nullptr;
        // HSlider *max_random_scale = nullptr;
        // CheckButton* align_with_check_button = nullptr;
        // CheckButton* randomize_scale = nullptr;
        // CheckButton* custom_offset = nullptr;
        // CheckButton* randomize_y_scale = nullptr;
        // CheckButton* rotate_y_check_button = nullptr;
        // CheckButton* rotate_z_check_button = nullptr;

        item_list = memnew(AsssetPanelItemList);
        item_list->set_bottom_panel_owner(this);
        item_list->set_icon_mode(ItemList::ICON_MODE_TOP);
        // item_list->set_icon_scale(2.4);
        item_list->set_fixed_icon_size(Size2i(64,64));
        item_list->set_max_columns(8);
        item_list->set_select_mode(ItemList::SelectMode::SELECT_MULTI);
        item_list->connect("item_activated",callable_mp(this,&AssetPanelEditorBottomPanel::_item_activated));
        item_list->connect("item_selected",callable_mp(this,&AssetPanelEditorBottomPanel::_item_selected));
        item_list->connect("multi_selected",callable_mp(this,&AssetPanelEditorBottomPanel::_multi_selected));
        item_list->connect("item_clicked",callable_mp(this,&AssetPanelEditorBottomPanel::_item_clicked));
        item_list->connect("empty_clicked",callable_mp(this,&AssetPanelEditorBottomPanel::_empty_clicked));
        underneath_hbox->set_v_size_flags(SIZE_EXPAND_FILL);
        underneath_hbox->add_child(item_list);
        item_list->set_h_size_flags(SIZE_EXPAND_FILL);
        item_list->set_v_size_flags(SIZE_EXPAND_FILL);

        delete_pscene->connect("pressed", callable_mp(this, &AssetPanelEditorBottomPanel::_delete_pressed));
        delete_pscene->set_shortcut_context(item_list);
        delete_pscene->set_shortcut(ED_SHORTCUT("assetplacer/delete", TTR("Delete PackedScene from list"), Key::KEY_DELETE));


        side_by_side_top->add_child(memnew(VSeparator));

        zoom_icon = memnew(TextureRect);
        zoom_icon->set_v_size_flags(SIZE_SHRINK_CENTER);
        side_by_side_top->add_child(zoom_icon);
        zoom = memnew(HSlider);
        zoom->set_step(0.1);
        zoom->set_min(20.0);
        zoom->set_max(200.0);
        zoom->set_value(64.0);
        zoom->set_custom_minimum_size(Size2(200, 0) * EDSCALE);
        zoom->set_v_size_flags(SIZE_SHRINK_CENTER);
        side_by_side_top->add_child(zoom);
        if (ProjectSettings::get_singleton()->has_setting("assetplacer/zoom_value")) {
            zoom->set_value(GLOBAL_GET("assetplacer/zoom_value"));
        }
        item_list->set_zoom(zoom);

        preview_camera_3d = memnew(Camera3D);
        preview_camera_3d->set_projection(Camera3D::PROJECTION_ORTHOGONAL);
        preview_camera_3d->set_size(10.0);
        preview_directional_light = memnew(DirectionalLight3D);
        preview_environment = memnew(Environment);
        preview_environment->set_ambient_source(Environment::AMBIENT_SOURCE_COLOR);
        preview_environment->set_ambient_light_color(Color(0.644531,0.644531,0.644531,1.0));
        preview_world3d = memnew(World3D);
        preview_world3d->set_environment(preview_environment);

        preview_subviewport = memnew(SubViewport);
        add_child(preview_subviewport);
        preview_subviewport->set_use_own_world_3d(true);
        preview_subviewport->set_world_3d(preview_world3d);
        preview_subviewport->set_transparent_background(true);
        preview_subviewport->set_fsr_sharpness(2.0);
        preview_subviewport->set_size(Size2i(64,64));
        preview_subviewport->set_update_mode(SubViewport::UPDATE_WHEN_PARENT_VISIBLE);
        preview_subviewport->add_child(preview_directional_light);
        preview_subviewport->add_child(preview_camera_3d);
        preview_camera_3d->set_transform(Transform3D(0.707107, -0.5, 0.5, 0, 0.707107, 0.707107, -0.707107, -0.5, 0.5, 26.0597, 36.8541, 26.0597));
        preview_directional_light->set_transform(Transform3D(0.707107, 0.5, -0.5, 0, 0.707107, 0.707107, 0.707107, -0.5, 0.5, 0, 0, 0));
    }

    ~AssetPanelEditorBottomPanel() {
        if (item_list == nullptr) {
            memdelete(item_list);
            item_list = nullptr;
        }
    }
};



////////////////////////////////////////////////////////////////////////////////////////////////////

class AssetPlacerPlugin : public EditorPlugin {
    GDCLASS(AssetPlacerPlugin, EditorPlugin);

    protected:
        //Ref<CatButtonsInspectorPlugin> inspector_plugin;

    AssetPanelEditorBottomPanel* bottom_panel;

    void _selected_item();

    void editor_changed_scene(const String &p_editor);

    void create_display_brush();

    void display_brush();

    void handle_holding_down_sculpt();

    void do_or_undo_placement(const Ref<PackedScene> &p_packed_scene, const Dictionary& placement_info, bool do_or_undo);

    String get_asset_name_from_resource_path(const String &p_res_path);

    EditorPlugin::AfterGUIInput handle_input_event(const Ref<InputEvent> &p_event);

    static void _bind_methods();

    bool is_holding_left_click=false;
    bool is_holding_ctrl = false;
    MeshInstance3D* preview_brush;
    MeshInstance3D* preview_radius;
    bool is_holding_shift = false;
    bool is_raycast_hit = false;
    uint64_t last_actually_sculpted_time = 0ULL;
    Vector3 raycast_hit_pos = Vector3(0.0f,0.0f,0.0f);
    Vector3 raycast_hit_normal = Vector3(0.0f,1.0f,0.0f);
    bool is_panel_visible = false;
    Node3D* parent_node_3d_;
    uint64_t ticks_late_initialized;
    bool last_editor_was_3d = false;
    bool handled_editor_scene_change = true;
    bool has_brush_ever_been_visible = false;
    public:
    void raycast(Camera3D *p_camera, const Ref<InputEvent> &p_event);

    virtual EditorPlugin::AfterGUIInput forward_3d_gui_input(Camera3D *p_camera, const Ref<InputEvent> &p_event) override;

        void _notification(int p_what);

    void late_initialize();

    void update_bottom_visibility();

    void _deselected_item();

    void editor_changed_scene_deferred();

        AssetPlacerPlugin() {
            Ref<YSpecialPoint3DGizmoPlugin> gizmo_plugin;
            gizmo_plugin.instantiate();
            Node3DEditor::get_singleton()->add_gizmo_plugin(gizmo_plugin);

            Ref<YSceneSpawner3DGizmoPlugin> yscenespawner_gizmo_plugin;
            yscenespawner_gizmo_plugin.instantiate();
            Node3DEditor::get_singleton()->add_gizmo_plugin(yscenespawner_gizmo_plugin);
            parent_node_3d_ = nullptr;
            ticks_late_initialized = 0;
            preview_brush = nullptr;
            preview_radius = nullptr;
            bottom_panel = memnew(AssetPanelEditorBottomPanel);
            bottom_panel->connect("selected_pscene",callable_mp(this,&yarnengine::AssetPlacerPlugin::_selected_item));
            bottom_panel->connect("deselected_pscene",callable_mp(this,&yarnengine::AssetPlacerPlugin::_deselected_item));
            bottom_panel->connect("visibility_changed", callable_mp(this, &yarnengine::AssetPlacerPlugin::update_bottom_visibility));
            //set_process(true);
        }
        ~AssetPlacerPlugin() override {
            // if(bottom_panel != nullptr) {
            memdelete(bottom_panel);
            //     bottom_panel = nullptr;
            // }
            bottom_panel = nullptr;

        }


};



}

#endif
#endif //ASSETPLACER_H
