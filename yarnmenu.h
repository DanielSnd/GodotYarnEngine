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

    bool called_start_from_process = false;
    void set_last_clicked_time() { YEngine::get_singleton()->last_button_click_time = YEngine::get_singleton()->ytime->pause_independent_time; }
    bool ui_cancel_presses_back_button = false;
    bool get_ui_cancel_presses_back_button() { return ui_cancel_presses_back_button; }
    void set_ui_cancel_presses_back_button(bool val) { ui_cancel_presses_back_button = val; }

    bool auto_start_menu = false;
    bool get_auto_start_menu() { return auto_start_menu; }
    void set_auto_start_menu(bool val) { auto_start_menu = val; }

    float fade_in_time = 0.4;
    float get_fade_in_time() { return fade_in_time; }
    void set_fade_in_time(float val) { fade_in_time = val; }
    float fade_out_time = 0.18;
    float get_fade_out_time() { return fade_out_time; }
    void set_fade_out_time(float val) { fade_out_time = val; }

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

    bool can_back_button_auto_close_menu() const;

    static Vector2 calculate_ideal_control_center(Vector2 size, Control *parent);

    static void add_to_menu_stack(Node* menu) { YEngine::get_singleton()->add_to_menu_stack(menu); }
    static void remove_from_menu_stack(Node* menu) { YEngine::get_singleton()->remove_from_menu_stack(menu); }
    static bool is_top_of_menu_stack(Node* menu) { return YEngine::get_singleton()->is_top_of_menu_stack(menu); }
    static void make_top_of_menu_stack(Node* menu) { YEngine::get_singleton()->make_top_of_menu_stack(menu); }
    static Node* get_top_of_menu_stack() { return YEngine::get_singleton()->get_top_of_menu_stack(); }
    
    void set_back_button(Control* button) {
        if (button != nullptr && button->has_signal(SNAME("pressed")))
            button->connect("pressed", callable_mp(this, &YMenu::_on_back_button_pressed));
    }

    bool get_can_click_buttons() {
        const YEngine* yengine = YEngine::get_singleton();
        return get_is_active() && yengine->ytime->has_pause_independent_time_elapsed(yengine->last_button_click_time,0.15) && get_modulate().a > 0.95;
    }

    Ref<Tween> fade_out();
    Ref<Tween> fade_out_and_queue_free();
    void fade_out_completed();
    bool is_fading_out_to_queue_free = false;
    Node* instantiate_child_menu(Control *parent_node, const PackedScene *child_menu_scene, bool auto_start = true);
    Node* instantiate_replacement_menu(Control* parent_node, const PackedScene* child_menu_scene, bool auto_start = true);

    static Callable button_click_callable(const Callable &callable);
    static Callable button_click_callable_if_modulate(const Callable &callable, Control* control);

    static Array get_menu_stack() { return YEngine::get_singleton()->get_menu_stack(); }
    static int get_menu_stack_size() { return YEngine::get_singleton()->get_menu_stack_size(); }
    

    static Node* instantiate_menu(const Ref<PackedScene> menu_scene, int layer_index = 0, bool auto_start = true);
    bool is_fading = false;
    bool get_is_fading() { return is_fading; }
    void set_is_fading(bool val) {  }

    GDVIRTUAL0(_on_back_button_pressed)
    GDVIRTUAL0(_on_started_menu)
    GDVIRTUAL0(_on_fade_out)
    GDVIRTUAL0(_on_back_to_menu)
    GDVIRTUAL0RC(bool,_can_back_button_auto_close_menu)
    YMenu();
    ~YMenu();

};

#endif //YARNMENU_H
