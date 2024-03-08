//
// Created by Daniel on 2024-03-02.
//

#ifndef YARNMENU_H
#define YARNMENU_H
#include "yarnengine.h"
#include "scene/animation/tween.h"
#include "scene/gui/panel.h"
#include "scene/resources/packed_scene.h"

class YMenu : public Panel {
    GDCLASS(YMenu, Panel);

protected:
    static void _bind_methods();
    void _notification(int p_what);
    void _on_back_button_pressed();

    void menu_queue_free();

    void _on_started_menu();
    void _on_back_to_menu();
    bool not_editor=false;
public:
    void do_process();
    virtual void on_back_button_pressed();
    virtual void on_started_menu();
    virtual void on_back_to_menu();

    void set_last_clicked_time() { YEngine::get_singleton()->last_button_click_time = YEngine::get_singleton()->ytime->pause_independent_time; }
    bool can_go_back_to_menu = false;
    bool get_can_go_back_to_menu() { return can_go_back_to_menu; }
    void set_can_go_back_to_menu(bool val) { can_go_back_to_menu = val; }

    bool auto_start_menu = false;
    bool get_auto_start_menu() { return auto_start_menu; }
    void set_auto_start_menu(bool val) { auto_start_menu = val; }

    bool is_active = false;
    bool get_is_active() {
        return not_editor && is_inside_tree() && YEngine::get_singleton() != nullptr && YEngine::get_singleton()->is_top_of_menu_stack(this);
    }
    void set_is_active(bool val) {
        if (not_editor && val && is_inside_tree() && YEngine::get_singleton() != nullptr) {
            YEngine::get_singleton()->remove_from_menu_stack(this);
            YEngine::get_singleton()->add_to_menu_stack(this);
        }
    }

    bool is_top_of_menu_stack() { return YEngine::get_singleton()->is_top_of_menu_stack(this); }

    void add_to_menu_stack() { YEngine::get_singleton()->add_to_menu_stack(this); }

    void remove_from_menu_stack() { YEngine::get_singleton()->remove_from_menu_stack(this); }

    void set_back_button(Control* button) {
        if (button != nullptr && button->has_signal(SNAME("pressed")))
            button->connect("pressed", callable_mp(this, &YMenu::_on_back_button_pressed));
    }

    bool get_can_click_buttons() {
        const YEngine* yengine = YEngine::get_singleton();
        return get_is_active() && yengine->ytime->has_pause_independent_time_elapsed(yengine->last_button_click_time,0.15) && get_modulate().a > 0.95;
    }

    Ref<Tween> fade_out() {
        auto tween = create_tween();
        tween->tween_property(this,NodePath{"modulate"},Color{1.0,1.0,1.0,0.0},0.18)->set_ease(Tween::EASE_IN_OUT)->set_trans(Tween::TRANS_QUAD);
        return tween;
    }

    Node* instantiate_child_menu(Control *parent_node, const PackedScene *child_menu_scene, bool auto_start = true);
    Node* instantiate_replacement_menu(Control* parent_node, const PackedScene* child_menu_scene, bool auto_start = true);

    GDVIRTUAL0(_on_back_button_pressed)
    GDVIRTUAL0(_on_started_menu)
    GDVIRTUAL0(_on_back_to_menu)
    YMenu();
    ~YMenu();

};

#endif //YARNMENU_H