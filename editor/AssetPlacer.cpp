//
// Created by Daniel on 2024-03-11.
//
#include "editor/editor_interface.h"
#ifdef TOOLS_ENABLED

#include "AssetPlacer.h"

using namespace godot;

bool yarnengine::AsssetPanelItemList::can_drop_data(const Point2 &p_point, const Variant &p_data) const {
    if (bottom_panel_owner != nullptr)
        return bottom_panel_owner->can_drop_data(p_point,p_data);
    return ItemList::can_drop_data(p_point, p_data);
}

void yarnengine::AsssetPanelItemList::drop_data(const Point2 &p_point, const Variant &p_data) {
    if (bottom_panel_owner != nullptr)
        return bottom_panel_owner->drop_data(p_point,p_data);
    ItemList::drop_data(p_point, p_data);
}

void yarnengine::AsssetPanelItemList::_zoom_changed(double p_value) {
    set_fixed_icon_size(Size2i(zoom->get_value(),zoom->get_value()));
    queue_redraw();
}

void yarnengine::AssetPanelEditorBottomPanel::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_itemlist_thumbnail_done"), &AssetPanelEditorBottomPanel::_itemlist_thumbnail_done);

    ADD_SIGNAL(MethodInfo("selected_pscene"));
    ADD_SIGNAL(MethodInfo("deselected_pscene"));
}

void yarnengine::AssetPanelEditorBottomPanel::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_READY: {
            if (item_list != nullptr && item_list->get_item_count() <=0) {
                Vector<String> saved_item_list;
                if (ProjectSettings::get_singleton()->has_setting("assetplacer/scenelist")) {
                    saved_item_list = GLOBAL_GET("assetplacer/scenelist");
                }
                for (int i = 0; i < saved_item_list.size(); i++) {
                    add_packedscene(saved_item_list[i]);
                }
            }
        }
        case NOTIFICATION_THEME_CHANGED: {
            delete_pscene->set_icon(get_editor_theme_icon(SNAME("Remove")));
			zoom_icon->set_texture(get_editor_theme_icon(SNAME("Zoom")));
			save_pscene_list->set_icon(get_editor_theme_icon(SNAME("Save")));
        }break;
    }
}

void yarnengine::AssetPanelEditorBottomPanel::add_packedscene(const String &scene_path) {
    if (!scene_path.to_lower().contains("scn")) return;
    auto default_icon = get_editor_theme_icon(SNAME("PackedScene"));
    const String pscene_name = scene_path.get_file().get_basename();
    const int _index = item_list->get_item_count();
    item_list->add_item(pscene_name,default_icon);
    item_list->set_item_metadata(_index,scene_path);
    Array udata;
    udata.push_back(_index);
    udata.push_back(scene_path);
    EditorResourcePreview::get_singleton()->queue_resource_preview(scene_path, this, "_itemlist_thumbnail_done", udata);
    //print_line("Queued resource preview? ",udata);
}

void yarnengine::AssetPanelEditorBottomPanel::_itemlist_thumbnail_done(const String &p_path, const Ref<Texture2D> &p_preview, const Ref<Texture2D> &p_small_preview, const Variant &p_udata) {
   // print_line("Thumbnail done ",p_path," udata ",p_udata," is preview valid? ",p_preview);
    Array uarr = p_udata;
    int item_index = uarr[0];
    if (item_list->get_item_count() > item_index && item_list->get_item_metadata(item_index) == uarr[1]) {
        // Found item correctly
      //  print_line("Found item correctly");
    } else {
        for (int i = 0; i < item_list->get_item_count(); ++i) {
            if (item_list->get_item_metadata(i) == uarr[1]) {
                item_index = i;
                break;
            }
        }
    }
    if(!p_preview.is_null() && p_preview.is_valid()) {
        item_list->set_item_icon(item_index,p_preview);
    }
}

bool yarnengine::AssetPanelEditorBottomPanel::can_drop_data(const Point2 &p_point, const Variant &p_data) const {
   // print_line("Can drop data? ",p_data);
    Dictionary as_dictionary = p_data;
    if (as_dictionary.get("type","") == "files")
        return true;
    return false;
}

void yarnengine::AssetPanelEditorBottomPanel::drop_data(const Point2 &p_point, const Variant &p_data) {
    //print_line("Dropping ",p_data);
    Dictionary p_data_dictionary = p_data;
    Array files_array = p_data_dictionary["files"];
    for (int i = 0; i < files_array.size(); ++i) {
        add_packedscene(files_array[i]);
    }
}

void yarnengine::AssetPanelEditorBottomPanel::_delete_pressed() {
    Vector<int> selected_items = item_list->get_selected_items();

    int nb_selected_items = selected_items.size();
    if (nb_selected_items <= 0) {
        return;
    }
    for (int i = 0; i < selected_items.size(); ++i) {
        item_list->remove_item(selected_items[i]);
    }
}

void yarnengine::AssetPanelEditorBottomPanel::_save_list_pressed() {
    Vector<String> p_scenes_in_list;
    for (int i = 0; i < item_list->get_item_count(); ++i) {
        p_scenes_in_list.append(item_list->get_item_metadata(i));
    }
    if (p_scenes_in_list.size() == 0) {
        ProjectSettings::get_singleton()->set("assetplacer/scenelist", Variant());
    } else {
        p_scenes_in_list.sort();
        ProjectSettings::get_singleton()->set("assetplacer/scenelist", p_scenes_in_list);
    }
    EditorNode::get_singleton()->get_project_settings()->queue_save();
}

void yarnengine::AssetPanelEditorBottomPanel::_item_clicked(int p_item, const Vector2 &p_pos, MouseButton p_mouse_button_index) {
    print_line("Clicked item ",p_item);
}

void yarnengine::AssetPanelEditorBottomPanel::_empty_clicked(const Vector2 &p_pos, MouseButton p_mouse_button_index) {
    print_line("Empty clicked");
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

void yarnengine::AssetPanelEditorBottomPanel::_item_selected(int index_selected) {
    print_line("Item selected");
    //item_list->set_item_custom_fg_color(index_activated,Color(0.2,0.8,0.75));
    current_activated_item_path = item_list->get_item_metadata(index_selected);
    current_activated_pscene = ResourceLoader::load(current_activated_item_path);
    current_activated_item_i = index_selected;
    print_line("Attempted to load ",current_activated_item_path," is loaded? ",current_activated_pscene.is_valid());
    if (current_activated_pscene.is_valid()) {
        auto instantiated = current_activated_pscene->instantiate();
        print_line("instantiated instance");
        auto mesh_instance = cast_to<MeshInstance3D>(instantiated);
        bool found_mesh = false;
        print_line("First attemmpt try find mesh instance ",mesh_instance != nullptr);
        if (mesh_instance != nullptr) {
            current_cativated_item_mesh = mesh_instance->get_mesh();
            if (current_cativated_item_mesh.is_valid())
                found_mesh=true;
        }
        if (!found_mesh) {
            TypedArray<Node> look_for_array = instantiated->find_children("*","MeshInstance3D");
            for (int i = 0; i < look_for_array.size(); ++i) {
                auto* possible_mesh = Object::cast_to<MeshInstance3D>(look_for_array[i]);

                print_line("loop attemmpt try find mesh instance ",possible_mesh != nullptr);
                if (possible_mesh != nullptr) {
                    current_cativated_item_mesh = possible_mesh->get_mesh();
                    print_line("current_cativated_item_mesh ",current_cativated_item_mesh.is_valid());
                    break;
                }
            }
        }
    }
    emit_signal("selected_pscene");
}
void yarnengine::AssetPanelEditorBottomPanel::_item_activated(int index_activated) {
    print_line("Activated item ",index_activated);

}


void yarnengine::AssetPlacerPlugin::_selected_item() {
    print_line("selected item, previewbrush? ",preview_brush != nullptr, " item mesh ", bottom_panel->current_cativated_item_mesh.is_valid());
    if(preview_brush != nullptr) {
        if (bottom_panel->current_cativated_item_mesh.is_valid()) {
            preview_brush->set_mesh(bottom_panel->current_cativated_item_mesh);
        } else {
            Ref<SphereMesh> sphere_mesh;
            sphere_mesh.instantiate();
            preview_brush->set_mesh(sphere_mesh);
        }
    }
}

void yarnengine::AssetPlacerPlugin::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
        } break;
        case NOTIFICATION_READY: {
            SceneTree::get_singleton()->connect("process_frame",callable_mp(this,&AssetPlacerPlugin::late_initialize),CONNECT_ONE_SHOT);
        } break;
        case NOTIFICATION_EXIT_TREE: {
        } break;
        case NOTIFICATION_PROCESS : {
            if(!handled_editor_scene_change) {
                handled_editor_scene_change=true;
                editor_changed_scene_deferred();
            }
            //print_line(vformat("Doing process %d",OS::get_singleton()->get_ticks_msec()));
        }break;
        default:
            break;
    }
}

void yarnengine::AssetPlacerPlugin::late_initialize() {
    uint64_t ticks = OS::get_singleton()->get_ticks_usec() / 1000;
    ticks_late_initialized = ticks;
    connect("main_screen_changed",callable_mp(this,&yarnengine::AssetPlacerPlugin::editor_changed_scene));
    set_input_event_forwarding_always_enabled();
    set_process(true);
}

void yarnengine::AssetPlacerPlugin::update_bottom_visibility() {
    is_panel_visible = bottom_panel != nullptr && bottom_panel->is_visible();
}

void yarnengine::AssetPlacerPlugin::_deselected_item() {
    if (preview_brush != nullptr)
        preview_brush->set_visible(false);
}

void yarnengine::AssetPlacerPlugin::editor_changed_scene_deferred() {
    if (OS::get_singleton() == nullptr) return;
    uint64_t ticks = OS::get_singleton()->get_ticks_usec() / 1000;
    if (ticks - ticks_late_initialized > 1000) {
        if (!last_editor_was_3d) {
            if (bottom_panel != nullptr) {
                EditorNode::get_bottom_panel()->remove_item(bottom_panel);
                if (preview_brush != nullptr && has_brush_ever_been_visible && preview_brush->is_visible())
                    preview_brush->set_visible(false);
            }
        } else {
            if (bottom_panel != nullptr) {
                EditorNode::get_bottom_panel()->add_item("Asset Placer", bottom_panel);
                if (preview_brush == nullptr) {
                    create_display_brush();
                }
            }
        }
    }
}

void yarnengine::AssetPlacerPlugin::editor_changed_scene(const String &p_editor) {
    last_editor_was_3d = p_editor == "3D";
    handled_editor_scene_change=false;
    // if (SceneTree::get_singleton() != nullptr && !SceneTree::get_singleton()->is_connected("process_frame",callable_mp(this,&yarnengine::AssetPlacerPlugin::editor_changed_scene_deferred))) {
    //    SceneTree::get_singleton()->connect("process_frame",callable_mp(this,&yarnengine::AssetPlacerPlugin::editor_changed_scene_deferred), CONNECT_ONE_SHOT);
    // }
}


void yarnengine::AssetPlacerPlugin::create_display_brush() {
    preview_brush = memnew(MeshInstance3D);
    add_child(preview_brush);
    Ref<SphereMesh> sphere_mesh;
    sphere_mesh.instantiate();
    preview_brush->set_mesh(sphere_mesh);
    Ref<ShaderMaterial> preview_material;
    preview_material.instantiate();
    Ref<Shader> preview_material_shader;
    preview_material_shader.instantiate();
    preview_material_shader->set_code(R"(
shader_type spatial;
render_mode blend_add,depth_draw_opaque,cull_back,unshaded,shadows_disabled;
void fragment() {
	ALBEDO = vec3(0.2,0.79,0.8);
	ALPHA = 0.6;
}
)");
    preview_material->set_shader(preview_material_shader);
    preview_brush->set_material_override(preview_material);
    preview_brush->set_visible(false);
}

void yarnengine::AssetPlacerPlugin::display_brush() {
    //print_line("is raycast hit? ",is_raycast_hit);
    if (is_raycast_hit) {
        if (!has_brush_ever_been_visible || !preview_brush->is_visible()) {
            preview_brush->set_visible(true);
            has_brush_ever_been_visible = true;
        }
        preview_brush->set_global_position(raycast_hit_pos);
    } else if (preview_brush->is_visible()) {
        preview_brush->set_visible(false);
    }
}

void yarnengine::AssetPlacerPlugin::handle_holding_down_sculpt() {
    if (last_actually_sculpted_time + 120UL < OS::get_singleton()->get_ticks_msec()) {
        last_actually_sculpted_time = OS::get_singleton()->get_ticks_msec();
        if (is_raycast_hit && bottom_panel != nullptr) {
            if(bottom_panel->current_activated_pscene.is_valid()) {
                if (parent_node_3d_ == nullptr) {
                    if (EditorInterface::get_singleton()) {
                        Node *current_scene = EditorInterface::get_singleton()->get_edited_scene_root();
                        print_line("current scene root ",current_scene->get_name()," path ",current_scene->get_path());
                        if (current_scene != nullptr) {
                            TypedArray<Node> nchildren = current_scene->find_children("Env","Node3D",true);
                            if (nchildren.size() > 0) {
                                auto found_node = Object::cast_to<Node3D>(nchildren.pop_front());
                                if (found_node != nullptr) {
                                    parent_node_3d_ = found_node;
                                }
                            } else {
                                parent_node_3d_ = memnew(Node3D);
                                if (parent_node_3d_ == nullptr) {
                                    ERR_PRINT("[AssetPlacer] Failed creating main node");
                                }
                                parent_node_3d_->set_name("Env");
                                print_line("Adding child ",parent_node_3d_," to ",current_scene);
                                current_scene->add_child(parent_node_3d_);
                                parent_node_3d_->set_owner(current_scene->get_owner() != nullptr ? current_scene->get_owner() : current_scene);
                            }
                        }
                    }
                }

                if (parent_node_3d_ != nullptr) {
                    auto* spawned = cast_to<Node3D>(bottom_panel->current_activated_pscene->instantiate());
                    print_line("Spawning ",bottom_panel->current_activated_pscene->get_name());
                    if (spawned != nullptr) {
                        parent_node_3d_->add_child(spawned);
                        spawned->set_owner(parent_node_3d_->get_owner() != nullptr ? parent_node_3d_->get_owner() : parent_node_3d_);
                        spawned->set_global_position(raycast_hit_pos);
                        spawned->set_name(vformat("%s_%d",bottom_panel->current_activated_item_path.get_basename(),parent_node_3d_->get_child_count()));
                    }
                }
            }
        }
    }
}

EditorPlugin::AfterGUIInput  yarnengine::AssetPlacerPlugin::handle_input_event(const Ref<InputEvent> &p_event) {
    Ref<InputEventMouseButton> mb = p_event;
    if (mb.is_valid() && mb->get_button_index() == MouseButton::LEFT) {
        if (mb->is_pressed()) {
            is_holding_left_click = true;
            handle_holding_down_sculpt();
            return AFTER_GUI_INPUT_STOP;
        }
        if (mb->is_released()) {
            is_holding_left_click = false;
        }
    }
    return AFTER_GUI_INPUT_PASS;
}

void yarnengine::AssetPlacerPlugin::raycast(Camera3D *p_camera, const Ref<InputEvent> &p_event) {
	if (p_camera == nullptr)
		return;

	Ref<InputEventMouseMotion> mm = p_event;
	if (mm.is_null() || !mm.is_valid()) return;
	Vector3 ray_origin = p_camera->project_ray_origin(mm->get_position());
	Vector3 ray_dir = p_camera->project_ray_normal(mm->get_position());
	float ray_dist = p_camera->get_far();
	ERR_FAIL_COND(get_viewport() == nullptr || get_viewport()->get_world_3d().is_null() || !get_viewport()->get_world_3d().is_valid());
	auto space_state = get_viewport()->get_world_3d()->get_direct_space_state();
	ERR_FAIL_COND(space_state == nullptr);
	PhysicsDirectSpaceState3D::RayResult result;
	PhysicsDirectSpaceState3D::RayParameters ray_params;
	ray_params.from = ray_origin;
	ray_params.to = ray_origin + ray_dir * ray_dist;
	if (space_state->intersect_ray(ray_params, result)) {
		is_raycast_hit = true;
		raycast_hit_normal = result.normal;
		raycast_hit_pos = result.position + (result.normal * (is_holding_ctrl ? -0.1f : 0.1f));
	} else {
		is_raycast_hit = false;
		float t = (-ray_origin.y) / ray_dir.y;
		raycast_hit_pos = ray_origin + t * ray_dir;
		raycast_hit_normal = Vector3(0.0f,1.0f,0.0f);
	}

	if(is_holding_left_click && (is_raycast_hit)) {
		handle_holding_down_sculpt();
	}
}

EditorPlugin::AfterGUIInput yarnengine::AssetPlacerPlugin::forward_3d_gui_input(Camera3D *p_camera, const Ref<InputEvent> &p_event) {
    if (bottom_panel == nullptr || !is_panel_visible) return EditorPlugin::forward_3d_gui_input(p_camera, p_event);
    if (bottom_panel->current_activated_item_i < 0) {
        is_raycast_hit = false;
        if (preview_brush != nullptr && preview_brush->is_visible()) {
            preview_brush->set_visible(false);
        }
        return EditorPlugin::forward_3d_gui_input(p_camera, p_event);
    }
    Ref<InputEventMouseMotion> motion = p_event;
    if (motion.is_valid()) {
        // Check if the mouse position is within the 3D viewport
        Vector2 mouse_pos = motion->get_position();
        Rect2 viewport_rect = p_camera->get_viewport()->get_visible_rect();

        // Get the size of the viewport
        Size2 viewport_size = viewport_rect.get_size();

        // Define a margin value to consider proximity to the edges
        float edge_margin = 10.0; // Adjust as needed

        // If the mouse is close to any of the edges
        if (mouse_pos.x < edge_margin || mouse_pos.x > viewport_size.width - edge_margin ||
            mouse_pos.y < edge_margin || mouse_pos.y > viewport_size.height - edge_margin) {
            is_raycast_hit = false;
            if (preview_brush != nullptr && preview_brush->is_visible()) {
                preview_brush->set_visible(false);
            }
            return AFTER_GUI_INPUT_PASS;
        }
    }

    Ref<InputEventKey> key_event = p_event;
    if (key_event.is_valid()) {
        if(key_event->get_physical_keycode() == Key::CMD_OR_CTRL) {
            if(key_event->is_pressed()) {
                is_holding_ctrl = true;
            } else if(key_event->is_released()) {
                is_holding_ctrl = false;
            }
        }
        if(key_event->get_physical_keycode() == Key::SHIFT) {
            if(key_event->is_pressed()) {
                is_holding_shift = true;
            } else if(key_event->is_released()) {
                is_holding_shift = false;
            }
        }
    }

    display_brush();
    raycast(p_camera,p_event);

    if (is_raycast_hit) {
         return handle_input_event(p_event);;
    } else {
        is_holding_left_click = false;
    }
    return EditorPlugin::forward_3d_gui_input(p_camera, p_event);
}


#endif
