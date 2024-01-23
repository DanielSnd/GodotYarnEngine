#ifndef GODOT_YARNENGINE_H
#define GODOT_YARNENGINE_H

#include "core/object/ref_counted.h"
#include "scene/main/node.h"

class YarnEngine : public Node {
    GDCLASS(YarnEngine, Node);

protected:
    void _setup_node();

    static void _bind_methods();
    void _notification(int p_what);
	static YarnEngine* singleton;

public:
    bool already_setup_in_tree = false;
    YarnEngine();
    static YarnEngine* get_singleton();
};

#endif