#ifndef YARNMESHGEN_H
#define YARNMESHGEN_H

#include "core/object/ref_counted.h"
#include "scene/resources/mesh.h"
#include "scene/resources/surface_tool.h"
#include "core/math/rect2.h"
#include "core/math/vector3.h"
#include "core/math/color.h"
#include "core/variant/typed_array.h"

class YMeshGen : public RefCounted {
    GDCLASS(YMeshGen, RefCounted);

protected:
    static YMeshGen* singleton;

    static void _bind_methods();

public:
    static YMeshGen *get_singleton();

    Ref<Mesh> create_mesh_from_aabbs_vcolor(const TypedArray<Rect2>& p_aabbs, const Rect2& p_encompassing_aabb, float p_margin, float p_resolution);
    
    YMeshGen();
    ~YMeshGen();
};

#endif //YARNMESHGEN_H
