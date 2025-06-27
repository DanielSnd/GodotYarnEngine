//
// Created by Daniel on 2024-03-11.
//
#ifdef TOOLS_ENABLED

#include "AssetPlacer.h"

Variant yarnengine::AsssetPanelItemList::get_drag_data(const Point2 &p_point) {
    if (bottom_panel_owner != nullptr && bottom_panel_owner->current_activated_item_i >= 0 && !bottom_panel_owner->current_activated_item_path.is_empty() ) {
        Dictionary returndict;
        Array asset_path_return;
        asset_path_return.push_back(bottom_panel_owner->current_activated_item_path);
        returndict["files"] = asset_path_return;
        returndict["type"] = "files";
        returndict["from_slot"] = bottom_panel_owner->current_activated_item_i;
        returndict["self_dragging"] = true;
        return returndict;
    }
    return Control::get_drag_data(p_point);
}

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

void yarnengine::AssetPanelEditorBottomPanel::changed_one_of_the_other_randomizers(bool new_value) {
    if (randomize_y_scale != nullptr) {
        ProjectSettings::get_singleton()->set("assetplacer/randomize_y_scale", randomize_y_scale->is_pressed());
    }
    if (rotate_y_check_button != nullptr) {
        ProjectSettings::get_singleton()->set("assetplacer/randomize_rotate_y", rotate_y_check_button->is_pressed());
    }
    if (rotate_z_check_button != nullptr) {
        ProjectSettings::get_singleton()->set("assetplacer/randomize_rotate_z", rotate_z_check_button->is_pressed());
    }
    if (align_with_check_button != nullptr) {
        ProjectSettings::get_singleton()->set("assetplacer/align_with_ground", align_with_check_button->is_pressed());
    }
    EditorNode::get_singleton()->get_project_settings()->queue_save();
}

void yarnengine::AssetPanelEditorBottomPanel::_grid_changed(uint32_t p_grid) {
    ground_layer = p_grid;
    ProjectSettings::get_singleton()->set("assetplacer/lock_to_layer", ground_layer);
    EditorNode::get_singleton()->get_project_settings()->queue_save();
}

void yarnengine::AssetPanelEditorBottomPanel::changed_min_randomize_scale(float new_min_scale_randomize_value) {
    ProjectSettings::get_singleton()->set("assetplacer/min_random_scale", new_min_scale_randomize_value);
    if (random_scale_max != nullptr) {
        if (random_scale_max->get_value() < new_min_scale_randomize_value) {
            random_scale_max->set_value(new_min_scale_randomize_value+0.01);
        }
    }
    EditorNode::get_singleton()->get_project_settings()->queue_save();
}

void yarnengine::AssetPanelEditorBottomPanel::changed_max_randomize_scale(float new_max_scale_randomize_value) {
    ProjectSettings::get_singleton()->set("assetplacer/max_random_scale", new_max_scale_randomize_value);
    if (random_scale_max != nullptr) {
        if (random_scale_min->get_value() > new_max_scale_randomize_value) {
            random_scale_min->set_value(new_max_scale_randomize_value-0.01);
        }
    }
    EditorNode::get_singleton()->get_project_settings()->queue_save();
}
void yarnengine::AssetPanelEditorBottomPanel::changed_randomize_scale(bool new_scale_randomize_value) {
    ProjectSettings::get_singleton()->set("assetplacer/randomize_scale", new_scale_randomize_value);
    if (random_scale_min != nullptr) {
        auto parent_node = cast_to<Control>(random_scale_min->get_parent());
        if (parent_node != nullptr) {
            parent_node->set_visible(new_scale_randomize_value);
        }
    }
    EditorNode::get_singleton()->get_project_settings()->queue_save();
}
void yarnengine::AsssetPanelItemList::_zoom_changed(double p_value) {
    ProjectSettings::get_singleton()->set("assetplacer/zoom_value", p_value);
    set_fixed_icon_size(Size2i(zoom->get_value(),zoom->get_value()));
    queue_redraw();
    EditorNode::get_singleton()->get_project_settings()->queue_save();
}

void yarnengine::AssetPanelEditorBottomPanel::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_itemlist_thumbnail_done"), &AssetPanelEditorBottomPanel::_itemlist_thumbnail_done);

    ADD_SIGNAL(MethodInfo("selected_pscene"));
    ADD_SIGNAL(MethodInfo("deselected_pscene"));
}

void yarnengine::AssetPanelEditorBottomPanel::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_READY: {
            set_process(true);
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
        break;
        case NOTIFICATION_THEME_CHANGED: {
            delete_pscene->set_button_icon(get_editor_theme_icon(SNAME("Remove")));
			zoom_icon->set_texture(get_editor_theme_icon(SNAME("Zoom")));
			save_pscene_list->set_button_icon(get_editor_theme_icon(SNAME("Save")));
        }break;
        case NOTIFICATION_PROCESS: {
            if (queue.size()>0 && !is_rendering_preview) {
                handle_preview_generating_queue();
            }
        }break;
    }
}

void yarnengine::AssetPanelEditorBottomPanel::finished_rendering_preview(Node3D* node_insantiated, const String &q_item_path, const ObjectID q_item_id, const String &q_item_function, const Variant &q_item_userdata) {
    Ref<PortableCompressedTexture2D> tex;
    tex.instantiate();
    tex->create_from_image(preview_subviewport->get_texture()->get_image(),PortableCompressedTexture2D::COMPRESSION_MODE_LOSSY);

    if (node_insantiated != nullptr) {
        node_insantiated->call_deferred("queue_free");
    }
    //print_line("Finished rendering preview item path ",q_item_path);
    Callable(q_item_id, q_item_function).call_deferred(q_item_path, tex, tex, q_item_userdata);
    is_rendering_preview = false;
}

void yarnengine::AssetPanelEditorBottomPanel::handle_preview_generating_queue() {
    QueueItem item = queue.front()->get();
    queue.pop_front();
    is_rendering_preview = true;
    Ref<PackedScene> ppscene_to_preview = ResourceLoader::load(item.path);
    //print_line("handling preview generating queue is valid? ",ppscene_to_preview.is_valid());
    if (ppscene_to_preview.is_valid()) {
        auto instantiated = ppscene_to_preview->instantiate();
        //print_line("instantiated instance");
        auto node_3d = cast_to<Node3D>(instantiated);
        if (node_3d != nullptr) {
            preview_subviewport->add_child(node_3d);
            preview_subviewport->set_size(Size2i(128,128));

            AABB new_aabb = AABB();
            TypedArray<Node> find_stuff = node_3d->get_children();
            while (find_stuff.size()) {
               auto found_node = Object::cast_to<Node>(find_stuff.pop_back());
               auto *_child = Object::cast_to<VisualInstance3D>(found_node);
               if (_child != nullptr) {
                   AABB child_aabb = _child->get_aabb().abs();
                   AABB transformed_aabb = AABB(child_aabb.position + _child->get_global_position(), child_aabb.size);
                   new_aabb = new_aabb.merge(transformed_aabb);
               }
               find_stuff.append_array(found_node->get_children());
            }
            if (new_aabb.size.is_zero_approx()) {
                is_rendering_preview = false;
                return;
            }
            auto max_size = MAX(MAX(new_aabb.size.x, new_aabb.size.y), new_aabb.size.z);

            preview_camera_3d->set_size(max_size * 2.0);
            preview_camera_3d->look_at_from_position(Vector3(max_size, max_size, max_size), new_aabb.get_center());
            RenderingServer::get_singleton()->connect("frame_post_draw",callable_mp(this,&yarnengine::AssetPanelEditorBottomPanel::finished_rendering_preview).bind(node_3d,item.path,item.id,item.function,item.userdata), CONNECT_ONE_SHOT);
        }
//        String q_item_path,ObjectID q_item_id,String q_item_function, Variant q_item_userdata)

    }
    // gui_instance.preview_viewport.add_child(node)
    // gui_instance.preview_viewport.size = Vector2i(preview_size, preview_size)
    //
    // var aabb := get_aabb(node)
    //
    // if is_zero_approx(aabb.size.length()):
    //     return
    //
    // var max_size := max(aabb.size.x, aabb.size.y, aabb.size.z) as float
    //
    // gui_instance.preview_camera.size = max_size * 2.0
    // gui_instance.preview_camera.look_at_from_position(Vector3(max_size, max_size, max_size), aabb.get_center())
    //
    // await RenderingServer.frame_post_draw
    // var viewport_image := gui_instance.preview_viewport.get_texture().get_image()
    // var preview := PortableCompressedTexture2D.new()
    // preview.create_from_image(viewport_image, PortableCompressedTexture2D.COMPRESSION_MODE_LOSSY)
	   //
    // gui_instance.preview_viewport.remove_child(node)
    // node.queue_free()
}

String yarnengine::AssetPanelEditorBottomPanel::get_asset_name_from_resource_path(const String &p_res_path) {
    String return_string = p_res_path;
    if (p_res_path.contains("/")) {
        return_string = return_string.rsplit("/",true,1)[1];
    }
    if (return_string.contains(".")) {
        return_string = return_string.split(".",false,1)[0];
    }
    return return_string;
}
void yarnengine::AssetPanelEditorBottomPanel::add_packedscene(const String &scene_path) {
    if (!scene_path.to_lower().contains("scn")) {
        WARN_PRINT(vformat("PackedScene not found in %s",scene_path));
        return;
    }
    auto default_icon = get_editor_theme_icon(SNAME("PackedScene"));
    const String pscene_name = get_asset_name_from_resource_path(scene_path);
    const int _index = item_list->get_item_count();
    item_list->add_item(pscene_name,default_icon);
    item_list->set_item_metadata(_index,scene_path);
    Array udata;
    udata.push_back(_index);
    udata.push_back(scene_path);
    generate_preview_image(scene_path, this, "_itemlist_thumbnail_done", udata);
    //EditorResourcePreview::get_singleton()->queue_resource_preview(scene_path, this, "_itemlist_thumbnail_done", udata);
    //print_line("Queued resource preview? ",udata);
}

void yarnengine::AssetPanelEditorBottomPanel::generate_preview_image(const String &p_path, Object *p_receiver, const StringName &p_receiver_func, const Variant &p_userdata) {
    QueueItem item;
    item.function = p_receiver_func;
    item.id = p_receiver->get_instance_id();
    item.path = p_path;
    item.userdata = p_userdata;

    queue.push_back(item);
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
    if (as_dictionary.get("type","") == "files" && as_dictionary.has("files") && !as_dictionary.has("self_dragging"))
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

    for (int i = selected_items.size() - 1; i >= 0; --i) {
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
        if (current_activated_pscene.is_valid()) {
            current_activated_pscene = nullptr;
        }
        current_activated_item_path = "";
        if (current_cativated_item_mesh.is_valid()) {
            current_cativated_item_mesh = nullptr;
        }

        emit_signal("deselected_pscene");
    }
}

void yarnengine::AssetPanelEditorBottomPanel::_item_selected(int index_selected) {
    //print_line("Item selected");
    //item_list->set_item_custom_fg_color(index_activated,Color(0.2,0.8,0.75));
    current_activated_item_path = item_list->get_item_metadata(index_selected);
    current_activated_pscene = ResourceLoader::load(current_activated_item_path);
    current_activated_item_i = index_selected;
    //print_line("Attempted to load ",current_activated_item_path," is loaded? ",current_activated_pscene.is_valid());
    if (current_activated_pscene.is_valid()) {
        auto instantiated = current_activated_pscene->instantiate();
        //print_line("instantiated instance");
        auto mesh_instance = cast_to<MeshInstance3D>(instantiated);
        bool found_mesh = false;
        //print_line("First attemmpt try find mesh instance ",mesh_instance != nullptr);
        if (mesh_instance != nullptr) {
            current_cativated_item_mesh = mesh_instance->get_mesh();
            if (current_cativated_item_mesh.is_valid())
                found_mesh=true;
        }
        if (!found_mesh) {
            TypedArray<Node> find_stuff = instantiated->get_children();
            while (find_stuff.size()) {
                auto found_node = Object::cast_to<Node>(find_stuff.pop_back());
                auto *_child = Object::cast_to<MeshInstance3D>(found_node);
                if (_child != nullptr) {
                    current_cativated_item_mesh = _child->get_mesh();
                    if (current_cativated_item_mesh.is_valid())
                        break;
                }
                find_stuff.append_array(found_node->get_children());
            }
        }
    }
    emit_signal("selected_pscene");
}

void yarnengine::AssetPanelEditorBottomPanel::_multi_selected(int p_index, bool p_selected) {
    if (p_selected) {
        _item_selected(p_index);
    } else {
        if (!item_list->is_anything_selected()) {
            deselect_all_items();
        }
    }
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
            Ref<YSpecialPoint3DGizmoPlugin> gizmo_plugin;
            gizmo_plugin.instantiate();
            Node3DEditor::get_singleton()->add_gizmo_plugin(gizmo_plugin);

            Ref<YSceneSpawner3DGizmoPlugin> yscenespawner_gizmo_plugin;
            yscenespawner_gizmo_plugin.instantiate();
            Node3DEditor::get_singleton()->add_gizmo_plugin(yscenespawner_gizmo_plugin);
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
    if (!is_panel_visible && preview_brush != nullptr && bottom_panel != nullptr && preview_brush->is_visible()) {
        bottom_panel->deselect_all_items();
    }
}

void yarnengine::AssetPlacerPlugin::_deselected_item() {
    if (preview_brush != nullptr) {
        preview_brush->set_visible(false);
        preview_radius->set_visible(false);
    }
}

void yarnengine::AssetPlacerPlugin::editor_changed_scene_deferred() {
    if (OS::get_singleton() == nullptr) return;
    uint64_t ticks = OS::get_singleton()->get_ticks_usec() / 1000;
    if (ticks - ticks_late_initialized > 1000) {
        if (!last_editor_was_3d) {
            if (bottom_panel != nullptr) {
                EditorNode::get_bottom_panel()->remove_item(bottom_panel);
                if (preview_brush != nullptr && has_brush_ever_been_visible && preview_brush->is_visible()) {
                    preview_brush->set_visible(false);
                    preview_radius->set_visible(false);
                }
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
    preview_radius = memnew(MeshInstance3D);
    add_child(preview_radius);
    add_child(preview_brush);
    Ref<SphereMesh> sphere_mesh;
    sphere_mesh.instantiate();
    preview_brush->set_mesh(sphere_mesh);
    preview_radius->set_mesh(sphere_mesh);
    Ref<ShaderMaterial> preview_material;
    preview_material.instantiate();
    Ref<ShaderMaterial> preview_radius_material;
    preview_radius_material.instantiate();
    Ref<Shader> preview_material_shader;
    preview_material_shader.instantiate();
    preview_material_shader->set_code(R"(
shader_type spatial;
render_mode blend_add,depth_draw_opaque,cull_back,unshaded,shadows_disabled;
void fragment() {
	ALBEDO = vec3(0.14,0.69,0.7);
	ALPHA = 0.36;
}
)");
    preview_material->set_shader(preview_material_shader);
    Ref<Shader> preview_radius_material_shader;
    preview_radius_material_shader.instantiate();
    preview_radius_material_shader->set_code(R"(
shader_type spatial;
render_mode blend_add,depth_draw_opaque,cull_disabled,unshaded,shadows_disabled,depth_test_disabled;
void fragment() {
	ALBEDO = FRONT_FACING ? vec3(0.2,0.79,0.8) : vec3(0.04,0.32,0.32);
	ALPHA = ((UV.y >0.48 && UV.y < 0.52) || UV.y < 0.015) ? 0.3 : 0.0;
}
)");
    preview_radius_material->set_shader(preview_radius_material_shader);
    preview_brush->set_material_override(preview_material);
    preview_brush->set_visible(false);

    preview_radius->set_material_override(preview_radius_material);
    preview_radius->set_visible(false);
}

void yarnengine::AssetPanelEditorBottomPanel::show_in_filesystem() {
    // Ref<Resource> current_res = _get_current_resource();
    // ERR_FAIL_COND(current_res.is_null());
    // FileSystemDock::get_singleton()->navigate_to_path(current_res->get_path());
}
void yarnengine::AssetPlacerPlugin::display_brush() {
    //print_line("is raycast hit? ",is_raycast_hit);
    if (is_raycast_hit && bottom_panel != nullptr) {
        if (!has_brush_ever_been_visible || !preview_brush->is_visible()) {
            preview_brush->set_visible(true);
            preview_radius->set_visible(true);
            has_brush_ever_been_visible = true;
        }
        preview_brush->set_global_position(raycast_hit_pos + (raycast_hit_normal * bottom_panel->get_offset()));
        preview_radius->set_global_position(raycast_hit_pos + raycast_hit_normal);
        preview_radius->set_scale(Vector3(bottom_panel->get_radius(),bottom_panel->get_radius() * 0.5,bottom_panel->get_radius()));
        if (bottom_panel->should_randomize_scale()) {
            preview_brush->set_scale(Vector3(1.0,1.0,1.0) * bottom_panel->get_min_max_random_scale().y);
        }
        if (bottom_panel->should_align_to_ground()) {
            preview_radius->set_quaternion(Quaternion(Vector3(0.0,1.0,0.0),raycast_hit_normal));
            preview_brush->set_quaternion(preview_radius->get_quaternion());
        } else {
            preview_radius->set_rotation(Vector3(0.0,0.0,0.0));
        }
    } else if (preview_brush->is_visible()) {
        preview_brush->set_visible(false);
        preview_radius->set_visible(false);
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
                    int ideal_id=1;
                    TypedArray<Node> find_stuff = parent_node_3d_->get_children();
                    while (find_stuff.size()) {
                        auto found_node = Object::cast_to<Node>(find_stuff.pop_back());
                        if (found_node != nullptr) {
                            const String name_string = found_node->get_name();
                            int found_integer = name_string.to_int();
                            if (found_integer > ideal_id) {
                                ideal_id = found_integer + 1;
                            }
                        }
                    }

                    EditorUndoRedoManager *undo_redo = EditorUndoRedoManager::get_singleton();
                    undo_redo->create_action(TTR("Asset Placer Placement"), UndoRedo::MERGE_DISABLE);
                    Dictionary info_for_placing;
                    info_for_placing["resource_path"] = bottom_panel->current_activated_item_path;
                    Vector<String> multiple_paths = bottom_panel->all_selected_paths();
                    if (multiple_paths.size() > 1) {
                        info_for_placing["multiple_paths"] = multiple_paths;
                    }
                    info_for_placing["hit_pos"] = raycast_hit_pos;
                    info_for_placing["hit_normal"] = raycast_hit_normal;
                    info_for_placing["align_to_ground"] = bottom_panel->should_align_to_ground();
                    info_for_placing["placing_radius"] = bottom_panel->get_radius() * 0.5;
                    info_for_placing["placing_amount"] = bottom_panel->get_amount();
                    info_for_placing["random_y_rotate"] = bottom_panel->should_random_y_rotate();
                    info_for_placing["random_y_scale"] = bottom_panel->should_random_y_scale();
                    info_for_placing["random_z_rotate"] = bottom_panel->should_random_z_rotate();
                    info_for_placing["randomize_scale"] = bottom_panel->should_randomize_scale();
                    if (bottom_panel != nullptr && bottom_panel->ground_layer != 0) {
                        info_for_placing["lock_to_layer"] = bottom_panel->ground_layer;
                    }
                    if (bottom_panel->should_randomize_scale()) {
                        info_for_placing["min_max_random_scale"] = bottom_panel->get_min_max_random_scale();
                    }
                    info_for_placing["offset"] = bottom_panel->get_offset();
                    info_for_placing["initial_id"] = ideal_id;
                    undo_redo->add_do_method(this, "do_or_undo_placement",bottom_panel->current_activated_pscene, info_for_placing, true);
                    undo_redo->add_undo_method(this, "do_or_undo_placement",bottom_panel->current_activated_pscene, info_for_placing, false);
                    undo_redo->commit_action();
                }
            }
        }
    }
}

void yarnengine::AssetPlacerPlugin::do_or_undo_placement(const Ref<PackedScene>& p_packed_scene, const Dictionary& placement_info, bool do_or_undo) {
    int amount = placement_info.get("placing_amount",0);
    String resource_path = placement_info.get("resource_path","");
    Vector<String> multiple_paths = placement_info.get("multiple_paths",Vector<String>{});
    bool has_multiple_paths = placement_info.has("multiple_paths");

    int initial_id = placement_info.get("initial_id",0);
    if (do_or_undo) {
        float radius = placement_info.get("placing_radius",0.0);
        Vector3 hit_pos = placement_info.get("hit_pos",Vector3(0.0,0.0,0.0));
        Vector3 hit_normal = placement_info.get("hit_normal",Vector3(0.0,1.0,0.0));
        Vector3 offset = placement_info.get("offset",Vector3(0.0,0.0,0.0));
        bool align_to_ground = placement_info.get("align_to_ground",false);
        bool random_y_rotate = placement_info.get("random_y_rotate",false);
        bool random_z_rotate = placement_info.get("random_z_rotate",false);
        bool random_y_scale = placement_info.get("random_y_scale",false);
        bool randomize_scale = placement_info.get("randomize_scale",false);
        uint32_t p_ground_layer = placement_info.get("lock_to_layer",UINT32_MAX);
        Vector2 random_min_max_scale = placement_info.get("min_max_random_scale",Vector2(1.0,1.0));
        int seed_of_placement = initial_id + static_cast<int>(hit_pos.x + hit_pos.y + hit_pos.z + hit_normal.x + hit_normal.y + hit_normal.z);
        Ref<RandomNumberGenerator> rng;
        rng.instantiate();
        rng->set_seed(seed_of_placement);

        Ref<RandomNumberGenerator> rng_for_path;
        rng_for_path.instantiate();
        rng_for_path->set_seed(seed_of_placement);
        auto yphysics_singleton = YPhysics::get_singleton();
        if (amount != 0) {
            for (int i = 0; i < amount; ++i) {
                Ref<PackedScene> get_scene_from_path = p_packed_scene;
                if (has_multiple_paths) {
                    get_scene_from_path = ResourceLoader::load(multiple_paths[rng_for_path->randi_range(0,static_cast<int>(multiple_paths.size())-1)]);
                }
                auto* spawned = cast_to<Node3D>(get_scene_from_path->instantiate());
               // print_line("Spawning ",bottom_panel->current_activated_pscene->get_name());
                if (spawned != nullptr) {
                    parent_node_3d_->add_child(spawned);
                    spawned->set_owner(parent_node_3d_->get_owner() != nullptr ? parent_node_3d_->get_owner() : parent_node_3d_);
                    Vector3 pos_offset_check = Vector3(rng->randf_range(-1.1,1.1),0.0,rng->randf_range(-1.1,1.1)).normalized() * rng->randf_range(-radius,radius);
                    pos_offset_check.y += 8.0;
                    auto result_raycast = yphysics_singleton->raycast3d(hit_pos+ pos_offset_check,Vector3(0.0,-1.0,0.0),50.0,YPhysics::CollideType::COLLIDE_WITH_BODIES,p_ground_layer);
                    if (!result_raycast.is_empty()) {
                        Vector3 new_hit_pos = result_raycast["position"];
                        Vector3 new_hit_normal = result_raycast["normal"];
                        Quaternion quaternion = spawned->get_quaternion();
                        if (i == 0) {
                            spawned->set_global_position(hit_pos + (offset * hit_normal));
                        } else {
                            spawned->set_global_position(new_hit_pos + (offset * new_hit_normal));
                        }
                        if (align_to_ground) {
                            quaternion = Quaternion(Vector3(0.0,1.0,0.0),i == 0 ? hit_normal : new_hit_normal) * quaternion;
                        }
                        if (random_z_rotate) {
                            quaternion = quaternion * Quaternion(Vector3(0.0,0.0,1.0),Math::deg_to_rad(rng->randf_range(0.01,359.9)));
                        }
                        if (random_y_rotate) {
                            quaternion = quaternion * Quaternion(Vector3(0.0,1.0,0.0),Math::deg_to_rad(rng->randf_range(-359.9,359.9)));
                        }
                        spawned->set_quaternion(quaternion);
                        if(randomize_scale) {
                            spawned->set_scale(spawned->get_scale() * rng->randf_range(random_min_max_scale.x,random_min_max_scale.y));
                        }
                        if (random_y_scale) {
                            Vector3 current_spawned_scale = spawned->get_scale();
                            float initial_scale_y = current_spawned_scale.y;
                            current_spawned_scale.y = current_spawned_scale.y + rng->randf_range(-1 * (initial_scale_y * 0.166f),(initial_scale_y * 0.166f));
                            spawned->set_scale(current_spawned_scale);
                        }
                    }
                    spawned->set_name(vformat("%s_%d",get_asset_name_from_resource_path(get_scene_from_path->get_path()),initial_id + i));
                }
            }
        }
    } else {
        TypedArray<Node> find_stuff = parent_node_3d_->get_children();
        Vector<String> names_to_delete;
        Vector<Node*> nodes_to_delete;

        Vector3 hit_pos = placement_info.get("hit_pos",Vector3(0.0,0.0,0.0));
        Vector3 hit_normal = placement_info.get("hit_normal",Vector3(0.0,1.0,0.0));
        int seed_of_placement = initial_id + static_cast<int>(hit_pos.x + hit_pos.y + hit_pos.z + hit_normal.x + hit_normal.y + hit_normal.z);
        Ref<RandomNumberGenerator> rng_for_path;
        rng_for_path.instantiate();
        rng_for_path->set_seed(seed_of_placement);
        for (int i = 0; i < amount; ++i) {
            if (has_multiple_paths) {
               resource_path = multiple_paths[rng_for_path->randi_range(0,static_cast<int>(multiple_paths.size())-1)];
            }
            names_to_delete.push_back((vformat("%s_%d",get_asset_name_from_resource_path(resource_path),initial_id + i)));
            //print_line("Adding to names to delete: ",names_to_delete[names_to_delete.size()-1]);
        }

        while (find_stuff.size()) {
            auto found_node = Object::cast_to<Node>(find_stuff.pop_back());
            if (found_node != nullptr) {
                const String name_string = found_node->get_name();
                //print_line("Found name: ",name_string," is in delete array? ",names_to_delete.has(name_string));
                if (names_to_delete.has(name_string)) {
                    nodes_to_delete.push_back(found_node);
                }
            }
        }
        for (auto delete_node: nodes_to_delete) {
            if (delete_node != nullptr) {
                //print_line("Attempting to delete name");
                delete_node->queue_free();
            }
        }
    }

}

String yarnengine::AssetPlacerPlugin::get_asset_name_from_resource_path(const String &p_res_path) {
    String return_string = p_res_path;
    if (p_res_path.contains("/")) {
        return_string = return_string.rsplit("/",true,1)[1];
    }
    if (return_string.contains(".")) {
        return_string = return_string.split(".",false,1)[0];
    }
    return return_string;
}

EditorPlugin::AfterGUIInput  yarnengine::AssetPlacerPlugin::handle_input_event(const Ref<InputEvent> &p_event) {
    Ref<InputEventKey> ek = p_event;
    if (ek.is_valid() && ek->get_keycode() == Key::ESCAPE && bottom_panel != nullptr && bottom_panel->has_item_selected()) {
        bottom_panel->deselect_all_items();
    }
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

void yarnengine::AssetPlacerPlugin::_bind_methods() {
    ClassDB::bind_method(D_METHOD("do_or_undo_placement", "p_packed_scene","placement_info","do_or_undo")
        , &AssetPlacerPlugin::do_or_undo_placement);

}

void yarnengine::AssetPlacerPlugin::raycast(Camera3D *p_camera, const Ref<InputEvent> &p_event) {
	if (p_camera == nullptr) {
	    if (bottom_panel != nullptr)
	        //bottom_panel->set_debug_text("Disabled because camera is null");
	    return;
	}
	Ref<InputEventMouseMotion> mm = p_event;
	if (mm.is_null() || !mm.is_valid()) {
	    if (bottom_panel != nullptr)
	        //bottom_panel->set_debug_text("Disabled because input event is invalid");
	    return;
	}
	Vector3 ray_origin = p_camera->project_ray_origin(mm->get_position());
	Vector3 ray_dir = p_camera->project_ray_normal(mm->get_position());
	float ray_dist = p_camera->get_far();
	ERR_FAIL_COND(get_viewport() == nullptr || get_viewport()->get_world_3d().is_null() || !get_viewport()->get_world_3d().is_valid());
	auto space_state = get_viewport()->get_world_3d()->get_direct_space_state();
	ERR_FAIL_COND(space_state == nullptr);
	PhysicsDirectSpaceState3D::RayResult result;
	PhysicsDirectSpaceState3D::RayParameters ray_params;
    if (bottom_panel != nullptr && bottom_panel->ground_layer != 0)
        ray_params.collision_mask = bottom_panel->ground_layer;
    else {
        ray_params.collision_mask = UINT32_MAX;
    }
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
    if (bottom_panel->current_activated_item_i < 0 || bottom_panel->should_be_enabled() == false) {
        is_raycast_hit = false;
        if (preview_brush != nullptr && preview_brush->is_visible()) {
            preview_brush->set_visible(false);
            preview_radius->set_visible(false);
        }
        //bottom_panel->set_debug_text(vformat("Disabled because current activated item is %d or shouldn't be enabled",bottom_panel->current_activated_item_i));
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
                preview_radius->set_visible(false);
            }

            //bottom_panel->set_debug_text("Disabled because mouse isn't in a valid viewport position");
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
        //bottom_panel->set_debug_text("Not a raycast hit");
    }
    return EditorPlugin::forward_3d_gui_input(p_camera, p_event);
}


#endif
