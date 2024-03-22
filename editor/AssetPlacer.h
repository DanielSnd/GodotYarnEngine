//
// Created by Daniel on 2024-03-11.
//

#ifndef ASSETPLACER_H
#define ASSETPLACER_H
#if TOOLS_ENABLED
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
#include "scene/gui/separator.h"

#include "core/object/undo_redo.h"
#include "editor/editor_undo_redo_manager.h"
#include "editor/project_settings_editor.h"
#include "scene/resources/packed_scene.h"
#include "scene/resources/3d/primitive_meshes.h"

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
	static void _bind_methods();
    // HBoxContainer* top_container;
    // Label* debug_label;
    // HSlider* brush_slider;
    // HSlider* brush_strength_slider;
    // Label* brush_slider_label;
    // Label* brush_strength_slider_label;
    void _notification(int p_what);
    Button* delete_pscene;
    Button* save_pscene_list;
    AsssetPanelItemList* item_list;
    HSlider *zoom = nullptr;
	TextureRect *zoom_icon = nullptr;


public:

    void add_packedscene(const String& scene_path);

    void _itemlist_thumbnail_done(const String &p_path, const Ref<Texture2D> &p_preview,
                                  const Ref<Texture2D> &p_small_preview, const Variant &p_udata);

    virtual bool can_drop_data(const Point2 &p_point, const Variant &p_data) const override;
    virtual void drop_data(const Point2 &p_point, const Variant &p_data) override;

    void _delete_pressed();
    void _save_list_pressed();

    void _item_clicked(int p_item, const Vector2 &p_pos, MouseButton p_mouse_button_index);

    void _empty_clicked(const Vector2 &p_pos, MouseButton p_mouse_button_index);

    void _item_selected(int index_selected);

    void _item_activated(int index_activated);
    String current_activated_item_path;
    int current_activated_item_i = -1;
    Ref<Mesh> current_cativated_item_mesh;
    Ref<PackedScene> current_activated_pscene;
    AssetPanelEditorBottomPanel() {
        HBoxContainer* side_by_side_top = memnew(HBoxContainer);
        add_child(side_by_side_top);

        save_pscene_list = memnew(Button);
        save_pscene_list->set_theme_type_variation("FlatButton");
        side_by_side_top->add_child(save_pscene_list);

        save_pscene_list->connect("pressed", callable_mp(this,&AssetPanelEditorBottomPanel::_save_list_pressed));
        delete_pscene = memnew(Button);
        delete_pscene->set_theme_type_variation("FlatButton");
        side_by_side_top->add_child(delete_pscene);
        add_child(memnew(HSeparator));
        MarginContainer* margin_container = memnew(MarginContainer);
        item_list = memnew(AsssetPanelItemList);
        item_list->set_bottom_panel_owner(this);
        item_list->set_icon_mode(ItemList::ICON_MODE_TOP);
        // item_list->set_icon_scale(2.4);
        item_list->set_fixed_icon_size(Size2i(64,64));
        item_list->set_max_columns(8);
        item_list->set_select_mode(ItemList::SelectMode::SELECT_SINGLE);
        item_list->connect("item_activated",callable_mp(this,&AssetPanelEditorBottomPanel::_item_activated));
        item_list->connect("item_selected",callable_mp(this,&AssetPanelEditorBottomPanel::_item_selected));
        item_list->connect("item_clicked",callable_mp(this,&AssetPanelEditorBottomPanel::_item_clicked));
        item_list->connect("empty_clicked",callable_mp(this,&AssetPanelEditorBottomPanel::_empty_clicked));
        add_child(item_list);
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
        zoom->set_min(16.0);
        zoom->set_max(128.0);
        zoom->set_value(32.0);
        zoom->set_custom_minimum_size(Size2(200, 0) * EDSCALE);
        zoom->set_v_size_flags(SIZE_SHRINK_CENTER);
        side_by_side_top->add_child(zoom);
        item_list->set_zoom(zoom);
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

    EditorPlugin::AfterGUIInput handle_input_event(const Ref<InputEvent> &p_event);

    bool is_holding_left_click=false;
    bool is_holding_ctrl = false;
    MeshInstance3D* preview_brush;
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
            parent_node_3d_ = nullptr;
            ticks_late_initialized = 0;
            preview_brush = nullptr;
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
