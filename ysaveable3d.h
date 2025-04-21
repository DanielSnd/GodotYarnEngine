#ifndef YSAVEABLE3D_H
#define YSAVEABLE3D_H

#include "scene/3d/node_3d.h"
#include "core/object/ref_counted.h"
#include "core/variant/dictionary.h"
#include "core/string/string_name.h"
#include "core/math/vector3.h"
#include "scene/main/scene_tree.h"
#include "scene/resources/packed_scene.h"
#include "core/io/resource_loader.h"
#include "ypersistentid.h"
#include "yarnsave.h"
#include "yarnengine.h"

class YSaveable3D : public Node {
    GDCLASS(YSaveable3D, Node);

protected:
    static void _bind_methods();
    void _notification(int p_what);

private:
    void on_prepare_save();
    void on_ready_delayed();
    Vector3 destringify_vector3(const Variant &got_position);

public:
    YSaveable3D();
    virtual ~YSaveable3D();

    bool has_loaded_save = false;
    Vector3 original_rotation;
    Vector3 original_scale;
    GDVIRTUAL1RC(bool,_on_prepare_save, Dictionary)
    GDVIRTUAL1(_on_load_save, Dictionary)
    static Node3D *spawn_from_saved_values(const Dictionary &saved_values);
    static Node* spawn_under_node;
    static void set_spawn_under_node(Node* p_node);
    static void load_all_saved_nodes();
    static void cleanup_all_saved_nodes();
    virtual void do_ready();
    void queue_free_from_signal();
    void connect_to_queue_free_from_signal();
    void load_save(const Dictionary &saved_values);
};

#endif // YSAVEABLE3D_H
