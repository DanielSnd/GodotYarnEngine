//
// Created by Daniel on 2024-06-02.
//

#ifndef GSHEETIMPORTEREDITORPLUGIN_H
#define GSHEETIMPORTEREDITORPLUGIN_H

#ifdef TOOLS_ENABLED

#include "../gsheetimporter.h"
#include "editor/plugins/editor_plugin.h"


class GSheetImporterEditorPlugin : public EditorPlugin {
    GDCLASS(GSheetImporterEditorPlugin, EditorPlugin);

protected:

public:
    inline static GSheetImporterEditorPlugin* singleton = nullptr;

    static GSheetImporterEditorPlugin* get_singleton();

    void FindImporterScripts();

    void add_menu_item(const String &menu_prefix, const String &menu_name, const Ref<GSheetImporter> &script_desired);

    void clicked_menu_item(const String &import_name, const Ref<GSheetImporter> &script_desired);

    Dictionary menu_scripts;
    Vector<String> added_menu_texts;

    GSheetImporterEditorPlugin();

    ~GSheetImporterEditorPlugin();
};

#endif //TOOLS_ENABLED

#endif //GSHEETIMPORTEREDITORPLUGIN_H
