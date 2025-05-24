//
// Created by Daniel on 2024-03-02.
//

#include "yarnmenu.h"

#include "scene/animation/tween.h"

void YMenu::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_is_active", "active"), &YMenu::set_is_active);
    ClassDB::bind_method(D_METHOD("get_is_active"), &YMenu::get_is_active);

    ClassDB::bind_method(D_METHOD("set_auto_start_menu", "active"), &YMenu::set_auto_start_menu);
    ClassDB::bind_method(D_METHOD("get_auto_start_menu"), &YMenu::get_auto_start_menu);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_start_menu"), "set_auto_start_menu", "get_auto_start_menu");
    ClassDB::bind_method(D_METHOD("set_ui_cancel_presses_back_button", "active"), &YMenu::set_ui_cancel_presses_back_button);
    ClassDB::bind_method(D_METHOD("get_ui_cancel_presses_back_button"), &YMenu::get_ui_cancel_presses_back_button);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "ui_cancel_presses_back_button"), "set_ui_cancel_presses_back_button", "get_ui_cancel_presses_back_button");

    ClassDB::bind_method(D_METHOD("set_fade_in_time", "time"), &YMenu::set_fade_in_time);
    ClassDB::bind_method(D_METHOD("get_fade_in_time"), &YMenu::get_fade_in_time);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fade_in_time"), "set_fade_in_time", "get_fade_in_time");

    ClassDB::bind_method(D_METHOD("set_fade_out_time", "time"), &YMenu::set_fade_out_time);
    ClassDB::bind_method(D_METHOD("get_fade_out_time"), &YMenu::get_fade_out_time);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fade_out_time"), "set_fade_out_time", "get_fade_out_time");

    ClassDB::bind_method(D_METHOD("on_back_button_pressed"), &YMenu::_on_back_button_pressed);
    ClassDB::bind_method(D_METHOD("on_back_to_menu"), &YMenu::_on_back_to_menu);
    ClassDB::bind_method(D_METHOD("on_started_menu"), &YMenu::_on_started_menu);

    ClassDB::bind_static_method("YMenu", D_METHOD("add_to_menu_stack", "menu"), &YMenu::add_to_menu_stack);
    ClassDB::bind_static_method("YMenu", D_METHOD("remove_from_menu_stack", "menu"), &YMenu::remove_from_menu_stack);
    ClassDB::bind_static_method("YMenu", D_METHOD("is_top_of_menu_stack", "menu"), &YMenu::is_top_of_menu_stack);
    ClassDB::bind_static_method("YMenu", D_METHOD("make_top_of_menu_stack", "menu"), &YMenu::make_top_of_menu_stack);
    ClassDB::bind_static_method("YMenu", D_METHOD("get_top_of_menu_stack"), &YMenu::get_top_of_menu_stack);
    ClassDB::bind_method(D_METHOD("set_back_button","back_button"), &YMenu::set_back_button);

    ClassDB::bind_method(D_METHOD("get_can_click_buttons"), &YMenu::get_can_click_buttons);

    ClassDB::bind_method(D_METHOD("fade_out"), &YMenu::fade_out);
    ClassDB::bind_method(D_METHOD("fade_out_and_queue_free"), &YMenu::fade_out_and_queue_free);
    ClassDB::bind_method(D_METHOD("set_last_clicked_time"), &YMenu::set_last_clicked_time);

    ClassDB::bind_method(D_METHOD("instantiate_child_menu", "parent_node","child_menu_scene","auto_start"), &YMenu::instantiate_child_menu, DEFVAL(true));
    ClassDB::bind_method(D_METHOD("instantiate_replacement_menu", "parent_node","child_menu_scene","auto_start"), &YMenu::instantiate_replacement_menu, DEFVAL(true));

    ClassDB::bind_static_method("YMenu", D_METHOD("button_click_callable", "callable"), &YMenu::button_click_callable);
    ClassDB::bind_static_method("YMenu", D_METHOD("button_click_callable_if_modulate", "callable","control"), &YMenu::button_click_callable_if_modulate);

    ClassDB::bind_static_method("YMenu", D_METHOD("get_menu_stack"), &YMenu::get_menu_stack);
    ClassDB::bind_static_method("YMenu", D_METHOD("get_menu_stack_size"), &YMenu::get_menu_stack_size);
    
    ClassDB::bind_static_method("YMenu", D_METHOD("instantiate_menu", "menu_scene", "layer_index", "auto_start"), &YMenu::instantiate_menu, DEFVAL(0), DEFVAL(true));
    ClassDB::bind_static_method("YMenu", D_METHOD("calculate_ideal_control_center","control_size", "control_parent"), &YMenu::calculate_ideal_control_center);

    ClassDB::bind_method(D_METHOD("get_is_fading"), &YMenu::get_is_fading);
    ClassDB::bind_method(D_METHOD("set_is_fading", "val"), &YMenu::set_is_fading);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_fading", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_is_fading", "get_is_fading");

    ADD_SIGNAL(MethodInfo("go_back_to_menu"));
    ADD_SIGNAL(MethodInfo("started_menu"));
    ADD_SIGNAL(MethodInfo("on_back_to_menu"));
    
    GDVIRTUAL_BIND(_can_back_button_auto_close_menu)
    GDVIRTUAL_BIND(_on_back_button_pressed);
    GDVIRTUAL_BIND(_on_back_to_menu);
    GDVIRTUAL_BIND(_on_started_menu);
    GDVIRTUAL_BIND(_on_fade_out)
}

void YMenu::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
            if (!Engine::get_singleton()->is_editor_hint())
                not_editor=true;
        } break;
        case NOTIFICATION_EXIT_TREE: {
            if(not_editor) {
                remove_from_menu_stack(this);
            }
        } break;
        case NOTIFICATION_READY: {
            if(not_editor) {
                if (fade_in_time > 0.00001) {
                    set_modulate(Color{1.0,1.0,1.0,0.0});
                    auto tween = create_tween();
                    tween->tween_property(this,NodePath{"modulate"},Color{1.0,1.0,1.0,1.0}, fade_in_time)->set_ease(Tween::EASE_IN_OUT)->set_trans(Tween::TRANS_QUAD);
                }
                add_to_menu_stack(this);
                if (auto_start_menu) {
                    called_start_from_process = true;
                    get_tree()->connect(SNAME("process_frame"),callable_mp(this,&YMenu::_on_started_menu), CONNECT_ONE_SHOT);
                }
            }
        } break;
        case NOTIFICATION_PROCESS: {
            if(not_editor) {
                do_process();
            }
        } break;
        case NOTIFICATION_WM_CLOSE_REQUEST: {

        } break;
        default:
            break;
    }
}

void YMenu::_on_back_button_pressed() {
    if (!get_can_click_buttons())
        return;
    YEngine::get_singleton()->last_button_click_time = YEngine::get_singleton()->ytime->pause_independent_time;
    bool can_auto_close = can_back_button_auto_close_menu();
    if (can_auto_close) {
        set_process(false);
        remove_from_menu_stack(this);
    }
    on_back_button_pressed();
    GDVIRTUAL_CALL(_on_back_button_pressed);
    // 	go_back_to_menu.emit()
    // 	await fade_out()
    // 	queue_free()
    if (can_auto_close) {
        emit_signal("go_back_to_menu");
        fade_out_and_queue_free();
    }
}

void YMenu::menu_queue_free() {
    queue_free();
}

void YMenu::_on_started_menu() {
    auto_start_menu = false;
    YEngine::get_singleton()->last_button_click_time = YEngine::get_singleton()->ytime->pause_independent_time;
    set_process(true);
    GDVIRTUAL_CALL(_on_started_menu);
    on_started_menu();
    emit_signal(SNAME("started_menu"));
}

void YMenu::_on_back_to_menu() {
    GDVIRTUAL_CALL(_on_back_to_menu);
    set_last_clicked_time();
    set_process(true);
    on_back_to_menu();
    emit_signal(SNAME("on_back_to_menu"));
}

void YMenu::do_process() {
    if (Input::get_singleton()->is_action_just_pressed("ui_cancel") && get_is_active() && ui_cancel_presses_back_button && get_can_click_buttons())
 		_on_back_button_pressed();
}

void YMenu::on_back_button_pressed() {
    //print_line("On back button pressed on cpp");
}

void YMenu::on_started_menu() {

}

void YMenu::on_back_to_menu() {
}

bool YMenu::can_back_button_auto_close_menu() const {
    bool rert=true;
    GDVIRTUAL_CALL(_can_back_button_auto_close_menu,rert);
    return rert;
}

Vector2 YMenu::calculate_ideal_control_center(Vector2 size,Control *parent) {
    if (parent == nullptr) {
        return {0.0,0.0};
    }
    return parent->get_size() * 0.5 - size * 0.5;
}

void YMenu::fade_out_completed() {
    is_fading = false;
    GDVIRTUAL_CALL(_on_fade_out);
    if (is_fading_out_to_queue_free) {
        queue_free();
    }
}

Node* YMenu::instantiate_menu(const Ref<PackedScene> menu_scene, int layer_index, bool auto_start) {
    if (menu_scene.is_null() || !menu_scene.is_valid()) {
        ERR_PRINT("Attempted to instantiate empty or invalidmenu scene");
        return nullptr;
    }
    
    Node* instantiated = menu_scene->instantiate(PackedScene::GEN_EDIT_STATE_DISABLED);
    if (instantiated == nullptr) {
        ERR_PRINT("Error instantiating menu");
        return nullptr;
    }
    
    CanvasLayer* find_canvas_layer = YEngine::get_singleton()->get_canvas_layer(layer_index);
    if (find_canvas_layer != nullptr) {
        find_canvas_layer->add_child(instantiated);
        if (auto_start && instantiated->has_method("_on_started_menu")) {
            auto ymenu = Object::cast_to<YMenu>(instantiated);
            if (ymenu!=nullptr && !ymenu->called_start_from_process) ymenu->_on_started_menu();
        }
    }
    return instantiated;
}

Ref<Tween> YMenu::fade_out() {
    is_fading = true;
    auto tween = YTween::get_singleton()->create_unique_tween(this);
    tween->tween_property(this,NodePath{"modulate"},Color{1.0,1.0,1.0,0.0}, fade_out_time)->set_ease(Tween::EASE_IN_OUT)->set_trans(Tween::TRANS_QUAD);
    is_fading_out_to_queue_free = false;
    if (tween.is_valid()) tween->tween_callback(callable_mp(this,&YMenu::fade_out_completed));
    return tween;
}

Ref<Tween> YMenu::fade_out_and_queue_free() {
    is_fading = true;
    if (fade_out_time > 0.00001) {
        auto tween = YTween::get_singleton()->create_unique_tween(this);
        tween->tween_property(this,NodePath{"modulate"},Color{1.0,1.0,1.0,0.0}, fade_out_time)->set_ease(Tween::EASE_IN_OUT)->set_trans(Tween::TRANS_QUAD);
        if (tween.is_valid()) tween->tween_callback(callable_mp(this,&YMenu::fade_out_completed));
        is_fading_out_to_queue_free = true;
        return tween;
    }
    is_fading_out_to_queue_free = true;
    fade_out_completed();
    return {};
}

Callable YMenu::button_click_callable(const Callable &callable) {
    return YEngine::get_singleton()->button_click_callable(callable);
}

Callable YMenu::button_click_callable_if_modulate(const Callable &callable, Control* control) {
    return YEngine::get_singleton()->button_click_callable_if_modulate(callable, control);
}

Node* YMenu::instantiate_child_menu(Control *parent_node, const PackedScene *child_menu_scene, const bool auto_start) {
    if (child_menu_scene == nullptr || parent_node == nullptr) {
        ERR_PRINT("Attempted to instantiate empty child menu or at an empty parent");
        return nullptr;
    }
    Node* instantiated = child_menu_scene->instantiate(PackedScene::GEN_EDIT_STATE_DISABLED);
    if (instantiated == nullptr) {
        ERR_PRINT("Error instantiating child menu");
        return nullptr;
    }
    parent_node->add_child(instantiated);
    if (instantiated->has_signal("go_back_to_menu")) {
        instantiated->connect("go_back_to_menu",callable_mp(this,&YMenu::_on_back_to_menu), CONNECT_ONE_SHOT);
    }
    if (auto_start && instantiated->has_method("_on_started_menu")) {
        auto ymenu = Object::cast_to<YMenu>(instantiated);
        if (ymenu!=nullptr && !ymenu->called_start_from_process) ymenu->_on_started_menu();
    }
    set_process(false);
    return instantiated;
}

Node*  YMenu::instantiate_replacement_menu(Control *parent_node, const PackedScene *child_menu_scene, const bool auto_start) {
    if (child_menu_scene == nullptr || parent_node == nullptr) {
        ERR_PRINT("Attempted to instantiate empty child menu or at an empty parent");
        return nullptr;
    }
    Node* instantiated = child_menu_scene->instantiate(PackedScene::GEN_EDIT_STATE_DISABLED);
    if (instantiated == nullptr) {
        ERR_PRINT("Error instantiating child menu");
        return nullptr;
    }
    parent_node->add_child(instantiated);
    if (auto_start && instantiated->has_method("_on_started_menu"))  {
        auto ymenu = Object::cast_to<YMenu>(instantiated);
        if (ymenu!=nullptr&& !ymenu->called_start_from_process) ymenu->_on_started_menu();
    }
    set_process(false);
    queue_free();
    return instantiated;
}

YMenu::YMenu() {

}

YMenu::~YMenu() {
    YEngine* yengine = YEngine::get_singleton();
    if (yengine != nullptr)
        yengine->remove_from_menu_stack(this);
}
