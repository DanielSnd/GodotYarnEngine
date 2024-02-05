//
// Created by Daniel on 2024-02-03.
//

#ifndef AOBAKEEDITORPLUGIN_H
#define AOBAKEEDITORPLUGIN_H
#include "../AOBakeableMeshInstance.h"
#include "editor/editor_inspector.h"
#include "editor/editor_plugin.h"
#include "editor/editor_scale.h"
#include "scene/gui/button.h"
#include "AOBakeGenerator.h"
#include "core/object/ref_counted.h"
#include "scene/3d/camera_3d.h"
#include "scene/gui/texture_rect.h"

class AOBakeEditorPlugin : public EditorPlugin {
    GDCLASS(AOBakeEditorPlugin, EditorPlugin);

protected:

public:
    inline static AOBakeEditorPlugin* singleton = nullptr;

    static AOBakeEditorPlugin* get_singleton();
    String get_name() const;
    AOBakeEditorPlugin();

    void CreateBakeableMeshInstance();

    ~AOBakeEditorPlugin();
};



#endif //AOBAKEEDITORPLUGIN_H
