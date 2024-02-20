#include "yarnengine.h"

YarnEngine* YarnEngine::singleton = nullptr;

YarnEngine * YarnEngine::get_singleton() {
    return singleton;
}

void YarnEngine::_bind_methods() {
    ClassDB::bind_method(D_METHOD("setup_node"), &YarnEngine::setup_node);
    ClassDB::bind_method(D_METHOD("get_current_scene"), &YarnEngine::get_current_scene);
    ClassDB::bind_method(D_METHOD("attempt_pause"), &YarnEngine::attempt_pause);

    ClassDB::bind_method(D_METHOD("set_is_paused", "is_paused"), &YarnEngine::set_is_paused);
    ClassDB::bind_method(D_METHOD("get_is_paused"), &YarnEngine::get_is_paused);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "is_paused"), "set_is_paused", "get_is_paused");

    ClassDB::bind_method(D_METHOD("set_time", "time"), &YarnEngine::set_time);
    ClassDB::bind_method(D_METHOD("get_time"), &YarnEngine::get_time);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "time"), "set_time", "get_time");

    ClassDB::bind_method(D_METHOD("set_pause_independent_time", "pause_independent_time"), &YarnEngine::set_pause_independent_time);
    ClassDB::bind_method(D_METHOD("get_pause_independent_time"), &YarnEngine::get_pause_independent_time);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "pause_independent_time"), "set_pause_independent_time", "get_pause_independent_time");

    ClassDB::bind_method(D_METHOD("test_callback_thing", "callable"), &YarnEngine::test_callback_thing);
    ADD_SIGNAL(MethodInfo("changed_pause", PropertyInfo(Variant::BOOL, "pause_value")));
}

Node* YarnEngine::get_current_scene() {
    return SceneTree::get_singleton()->get_current_scene();
}

void YarnEngine::setup_node() {
    if(!already_setup_in_tree && SceneTree::get_singleton() != nullptr) {
        SceneTree::get_singleton()->get_root()->add_child(this);
        already_setup_in_tree=true;
    }
}

void YarnEngine::_notification(int p_what) {
    if (p_what == 0) {
        call_deferred("setup_node");
    }
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
        } break;
        // case NOTIFICATION_ENTER_WORLD: {
        // } break;
        case NOTIFICATION_EXIT_TREE: {
            break;
        }
        case NOTIFICATION_PARENTED: {
            if (Engine::get_singleton()->is_editor_hint()) {
            }
        }
        case NOTIFICATION_READY: {
            set_process(true);
        } break;
        case NOTIFICATION_PROCESS: {
            if (!Engine::get_singleton()->is_editor_hint()) {
                handle_time_setting();
            }
            //print_line(pause_independent_time);
        } break;
        default:
            break;
    }
}

bool YarnEngine::attempt_pause() {
    if (last_time_ended_pause + 0.3 < pause_independent_time) {
        return false;
    }
    return is_paused;
}

void YarnEngine::set_is_paused(bool val) {
    if (is_paused != val) {
        is_paused = val;
        if (is_paused)
            amount_when_paused = OS::get_singleton()->get_ticks_msec() * 0.001f;
        else
            amount_when_unpaused = OS::get_singleton()->get_ticks_msec() * 0.001f;
        amount_time_last_frame = amount_when_unpaused;
        emit_signal(SNAME("changed_pause"),val);
    }
}

void YarnEngine::handle_time_setting() {
    pause_independent_time = OS::get_singleton()->get_ticks_msec() * 0.001f;
    if (!is_paused) {
        time += pause_independent_time - amount_time_last_frame;
        amount_time_last_frame = pause_independent_time;
    }

    // if save_requested and pause_independent_time_elapsed(last_time_save_requested,0.33):
    //     save_requested = false
    //     do_save()
}

bool YarnEngine::has_time_elapsed(float test_time, float interval) {
    return test_time + interval < time;
}

bool YarnEngine::has_pause_independent_time_elapsed(float test_time, float interval) {
    return test_time + interval < pause_independent_time;
}

Variant YarnEngine::test_callback_thing(const Callable &p_callable) {
    if (!p_callable.is_null() && p_callable.is_valid()) {
        return p_callable.call();
    }
    return Variant{};
}

YarnEngine::YarnEngine() {
    singleton = this;
    is_paused = false;
    amount_time_last_frame = 0.0;
    amount_when_paused = 0.0;
    amount_when_unpaused = 0.0;
}

YarnEngine::~YarnEngine() {
    if (singleton != nullptr && singleton == this) {
        singleton = nullptr;
    }
}
