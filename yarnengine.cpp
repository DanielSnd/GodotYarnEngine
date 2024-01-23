#include "yarnengine.h"

#include "scene/main/window.h"

YarnEngine* YarnEngine::singleton = nullptr;

void YarnEngine::_setup_node() {
    if(!already_setup_in_tree) {
        SceneTree::get_singleton()->get_root()->add_child(this);
        already_setup_in_tree=true;
    }
}

YarnEngine * YarnEngine::get_singleton() {
    return singleton;
}

void YarnEngine::_bind_methods() {
    ClassDB::bind_method(D_METHOD("setup_node"), &YarnEngine::_setup_node);
}

void YarnEngine::_notification(int p_what) {
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

        } break;
        case NOTIFICATION_PROCESS: {

        } break;
        default:
            break;
    }
}

YarnEngine::YarnEngine() {}
