//
// Created by Daniel on 2024-06-02.
//

#include "GSheetImporterEditorPlugin.h"

#ifdef TOOLS_ENABLED
#include "core/io/dir_access.h"

EditorProgress *GSheetImporterEditorPlugin::tmp_progress = nullptr;

void GSheetImporterEditorPlugin::import_func_begin(int p_steps, const String &p_task_name, const String &p_task_description) {
	if (tmp_progress != nullptr) {
	    memdelete(tmp_progress);
	    tmp_progress = nullptr;
	}
    tmp_progress = memnew(EditorProgress(p_task_name, p_task_description, p_steps));
}

void GSheetImporterEditorPlugin::import_func_step(int p_step, const String &p_description) {
    if (tmp_progress != nullptr) {
        tmp_progress->step(p_description, p_step, false);
    }
}

void GSheetImporterEditorPlugin::import_func_end() {
    if (tmp_progress != nullptr) {
        memdelete(tmp_progress);
        tmp_progress = nullptr;
    }
}

GSheetImporterEditorPlugin * GSheetImporterEditorPlugin::get_singleton() {
    return singleton;
}

void GSheetImporterEditorPlugin::FindImporterScripts() {
    if (waitin_for_delayed_add_importers < 500) {
        waitin_for_delayed_add_importers++;
        return;
    } else {
        const Callable my_callable = callable_mp(this,&GSheetImporterEditorPlugin::FindImporterScripts);
        if (SceneTree::get_singleton()->is_connected(SNAME("process_frame"),my_callable))
            SceneTree::get_singleton()->disconnect(SNAME("process_frame"),my_callable);
    }
    auto _dir_exists = DirAccess::exists("res://importers/");
    if (!_dir_exists)
        return;

    Ref<DirAccess> dir = DirAccess::open("res://importers/");
    dir->list_dir_begin();
    String file_name = dir->_get_next();
    while (file_name != "") {
        if (!dir->current_is_dir() && file_name.get_extension() == "gd") {
            //print_line("Attempt to load script",file_name);
            const String file_path = vformat("res://importers/%s",file_name);
            Ref<Script> gdScript = ResourceLoader::load(file_path, "Script");
            if (gdScript.is_valid()) {
                if(gdScript->has_method("on_imported_data") && gdScript->has_method("get_sheet_name") && gdScript->has_method("get_sheet_id")) {
                    Ref<GSheetImporter> gsheet_importer;
                    gsheet_importer.instantiate();
                    auto temp_script_instance = gdScript->instance_create(gsheet_importer.ptr());
                    if (temp_script_instance != nullptr) {
                        gsheet_importer->set_script_instance(temp_script_instance);
                        String get_sheet_name = temp_script_instance->call("get_sheet_name");
                        //print_line("get_sheet_name ",get_sheet_name);
                        if (!menu_scripts.has(get_sheet_name) && !get_sheet_name.is_empty()) {
                            add_menu_item("Import ",get_sheet_name, gsheet_importer, file_path);
                        }
                    }
                } else if (gdScript->has_method("on_menu_clicked") && gdScript->has_method("get_menu_name")) {
                    Ref<GSheetImporter> gsheet_importer;
                    gsheet_importer.instantiate();
                    auto temp_script_instance = gdScript->instance_create(gsheet_importer.ptr());
                    if (temp_script_instance != nullptr) {
                        gsheet_importer->set_script_instance(temp_script_instance);
                        String get_menu_name = temp_script_instance->call("get_menu_name");
                        if (!menu_scripts.has(get_menu_name) && !get_menu_name.is_empty()) {
                            add_menu_item("",get_menu_name, gsheet_importer, file_path);
                        }
                    }
                } else {
                    print_error(vformat("Script res://importers/%s is missing 'on_imported_data(import_type: String, data: Array)' method",file_name));
                }
            } else {
                print_error(vformat("Failed to load importer script res://importers/%s",file_name));
            }
        }
        file_name = dir->_get_next();
    }
}

void GSheetImporterEditorPlugin::DelayedLookForImporterScripts() {
    SceneTree::get_singleton()->connect(SNAME("process_frame"),callable_mp(this,&GSheetImporterEditorPlugin::FindImporterScripts));
}

void GSheetImporterEditorPlugin::add_menu_item(const String &menu_prefix, const String &menu_name, const Ref<GSheetImporter> &script_desired, const String &
                                               script_path) {
    menu_scripts[menu_name] = script_desired;
    added_menu_texts.push_back(vformat("%s%s",menu_prefix, menu_name));
    add_tool_menu_item(vformat("%s%s",menu_prefix, menu_name),callable_mp(this,&GSheetImporterEditorPlugin::clicked_menu_item).bind(menu_name,script_desired, script_path));
}

void GSheetImporterEditorPlugin::clicked_menu_item(const String &import_name, const Ref<GSheetImporter> &script_desired, const String &script_path) {
    //add_tool_menu_item(vformat("Import %s",import_name),callable_mp());
    // print_line(vformat("Is ref valid? %s",script_desired.is_valid()));
    if (script_desired.is_null() || !script_desired.is_valid()) {

    }
    if (script_desired.is_valid()) {
        if (script_desired->has_method("on_imported_data"))
            script_desired->request_import();
        else if (script_desired->has_method("on_menu_clicked")) {
            script_desired->call("on_menu_clicked");
        }
    }
}


void GSheetImporterEditorPlugin::_notification(int p_what) {
    if (p_what == NOTIFICATION_POSTINITIALIZE && Engine::get_singleton()->is_editor_hint()) {
        callable_mp(this,&GSheetImporterEditorPlugin::DelayedLookForImporterScripts).call_deferred();
    }
}

GSheetImporterEditorPlugin::GSheetImporterEditorPlugin() {
    singleton = this;
    waitin_for_delayed_add_importers = 0;
    GSheetImporter::import_begin_function = import_func_begin;
    GSheetImporter::import_step_function = import_func_step;
    GSheetImporter::import_end_function = import_func_end;
}

GSheetImporterEditorPlugin::~GSheetImporterEditorPlugin() {
    if (!added_menu_texts.is_empty()) {
        for (const String& key: added_menu_texts) {
            EditorPlugin::remove_tool_menu_item(key);
        }
    }
    menu_scripts.clear();
    added_menu_texts.clear();
    if(singleton != nullptr && singleton == this) {
        //if (created_menu_item)
          //  EditorPlugin::remove_tool_menu_item("Create BakeableMeshInstance");
        singleton = nullptr;
    }
}
#endif
