//
// Created by Daniel on 2024-06-02.
//

#ifndef GSHEETIMPORTEREDITORPLUGIN_H
#define GSHEETIMPORTEREDITORPLUGIN_H

#ifdef TOOLS_ENABLED

#include "../gsheetimporter.h"
#include "editor/plugins/editor_plugin.h"
#include "editor/editor_node.h"
#include "editor/editor_paths.h"

struct EditorProgress;
class GSheetImporterEditorPlugin : public EditorPlugin {
    GDCLASS(GSheetImporterEditorPlugin, EditorPlugin);

    int waitin_for_delayed_add_importers = 0;
    static EditorProgress *tmp_progress;
    static void import_func_begin(int p_steps, const String &p_task_name, const String &p_task_description);
    static void import_func_step(int p_step, const String &p_description);
    static void import_func_end();
protected:
    void _notification(int p_what);

public:
    inline static GSheetImporterEditorPlugin* singleton = nullptr;

    static GSheetImporterEditorPlugin* get_singleton();

    void FindImporterScripts();

    void DelayedLookForImporterScripts();

    void add_menu_item(const String &menu_prefix, const String &menu_name, const Ref<GSheetImporter> &script_desired, const String &script_path);

    void clicked_menu_item(const String &import_name, const Ref<GSheetImporter> &script_desired, const String &script_path);

    void create_importer_script_template();

    Dictionary menu_scripts;
    Vector<String> added_menu_texts;

    GSheetImporterEditorPlugin();

    ~GSheetImporterEditorPlugin();
};

#endif //TOOLS_ENABLED

#endif //GSHEETIMPORTEREDITORPLUGIN_H
