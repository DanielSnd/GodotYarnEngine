//
// Created by Daniel on 2024-02-03.
//

#ifndef AOBAKEEDITORPLUGIN_H
#define AOBAKEEDITORPLUGIN_H

#ifdef TOOLS_ENABLED
#include "../AOBakeableMeshInstance.h"
#include "editor/editor_inspector.h"
#include "editor/plugins/editor_plugin.h"
#include "scene/gui/button.h"
#include "AOBakeGenerator.h"
#include "editor/themes/editor_scale.h"
#include "core/object/ref_counted.h"
#include "scene/3d/camera_3d.h"
#include "scene/gui/texture_rect.h"

class AOBakeEditorPlugin : public EditorPlugin {
    GDCLASS(AOBakeEditorPlugin, EditorPlugin);

protected:
    bool created_menu_item = false;
public:
    inline static AOBakeEditorPlugin* singleton = nullptr;

    static AOBakeEditorPlugin* get_singleton();
    AOBakeEditorPlugin();

    void CreateBakeableMeshInstance();

    ~AOBakeEditorPlugin();
};

#endif //TOOLS_ENABLED

#endif //AOBAKEEDITORPLUGIN_H
