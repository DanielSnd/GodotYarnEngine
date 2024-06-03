//
// Created by Daniel on 2024-06-02.
//

#include "GSheetImporterEditorPlugin.h"

#include "core/io/dir_access.h"

GSheetImporterEditorPlugin * GSheetImporterEditorPlugin::get_singleton() {
    return singleton;
}

void GSheetImporterEditorPlugin::FindImporterScripts() {
    auto _dir_exists = DirAccess::exists("res://importers/");
    if (!_dir_exists)
        return;

    Ref<DirAccess> dir = DirAccess::open("res://importers/");
    dir->list_dir_begin();
    String file_name = dir->_get_next();
    while (file_name != "") {
        if (!dir->current_is_dir() && file_name.get_extension() == "gd") {
            //print_line("Attempt to load script",file_name);
            Ref<Script> gdScript = ResourceLoader::load(vformat("importers/%s",file_name), "Script");
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
                            add_menu_item(get_sheet_name, gsheet_importer);
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

void GSheetImporterEditorPlugin::add_menu_item(const String &menu_name, const Ref<GSheetImporter> &script_desired) {
    menu_scripts[menu_name] = script_desired;
    add_tool_menu_item(vformat("Import %s",menu_name),callable_mp(this,&GSheetImporterEditorPlugin::clicked_menu_item).bind(menu_name,script_desired));
}

void GSheetImporterEditorPlugin::clicked_menu_item(const String &import_name, const Ref<GSheetImporter> &script_desired) {
    //add_tool_menu_item(vformat("Import %s",import_name),callable_mp());
    // print_line(vformat("Is ref valid? %s",script_desired.is_valid()));
    if (script_desired.is_valid()) {
        script_desired->request_import();
    }
}

GSheetImporterEditorPlugin::GSheetImporterEditorPlugin() {
    singleton = this;
    FindImporterScripts();
}

GSheetImporterEditorPlugin::~GSheetImporterEditorPlugin() {
    if (menu_scripts.is_empty()) {
        for (String key: menu_scripts.keys()) {
            EditorPlugin::remove_tool_menu_item(vformat("Import %s",key));
        }
    }
    menu_scripts.clear();
    if(singleton != nullptr && singleton == this) {
        //if (created_menu_item)
          //  EditorPlugin::remove_tool_menu_item("Create BakeableMeshInstance");
        singleton = nullptr;
    }
}
