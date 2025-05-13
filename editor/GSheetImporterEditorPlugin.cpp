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

void GSheetImporterEditorPlugin::FindImporterScripts() {
    if (waitin_for_delayed_add_importers < 300) {
        waitin_for_delayed_add_importers++;
        return;
    } else {
        const Callable my_callable = callable_mp(this,&GSheetImporterEditorPlugin::FindImporterScripts);
        if (SceneTree::get_singleton()->is_connected(SNAME("process_frame"),my_callable))
            SceneTree::get_singleton()->disconnect(SNAME("process_frame"),my_callable);
    }
    create_importer_script_template();
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

void GSheetImporterEditorPlugin::create_importer_script_template() {
    String template_path = EditorPaths::get_singleton()->get_script_templates_dir().path_join("GSheetImporter/GSheetImporter.gd");
    if (FileAccess::exists(template_path)) {
        // print_line("Template already exists");
        return;
    }

    if (!DirAccess::dir_exists_absolute(EditorPaths::get_singleton()->get_script_templates_dir().path_join("GSheetImporter")) && !DirAccess::make_dir_recursive_absolute(EditorPaths::get_singleton()->get_script_templates_dir().path_join("GSheetImporter"))) {
        // print_error(vformat("Can't create directory: '%s'.", EditorPaths::get_singleton()->get_script_templates_dir().path_join("GSheetImporter")));
        return;
    }
    
    Ref<FileAccess> file = FileAccess::open(template_path, FileAccess::WRITE);
	if(file.is_null())
    {
        // print_error(vformat("Can't open file to write: '%s'.", String(template_path)));
        return;
    }
    file->store_string(R"(
extends GSheetImporter

func get_sheet_id()->String:
	return "" # Sheet ID goes here, example: 1plp6NPRKkAcwGaXvJjQUeS6n7TPIZQ6CA297Dvx3qhU

func get_sheet_name()->String:
	return "Sheet 1" # Here you put the sheet name

func get_resource_folder()->String:
	return "res://resources/upgrades" # Here you put the folder where the resources will be saved

func on_imported_data(import_type: String, data: Array):
	start_import_progress_bar(data.size(), "importing_data","Importing %d data" % data.size())

	var amount_attempted:int = 0
	var amount_imported:int = 0
	for entry in data:
		amount_attempted += 1
		if import_data(entry):
			amount_imported+=1
		step_import_progress_bar(amount_attempted, "%d Parsed, %d Imported" % [amount_attempted, amount_imported])
	end_import_progress_bar()

	print("Imported %d %s" % [amount_imported, get_resource_folder()])
	
	if Engine.is_editor_hint():
		var editor_interface = Engine.get_singleton("EditorInterface")
		if editor_interface != null:
			var resource_filesystem = editor_interface.get_resource_filesystem()
			if resource_filesystem != null:
				resource_filesystem.scan()

func import_data(data: Dictionary) -> bool:
    # Here you put the column that stores the filename of the resource
    var resource_name:String = str(data.get("resource_name",""))
	if resource_name.is_empty():
		return false
	resource_name = sanitize_filename(resource_name)
	var import_resource = Resource.new() # Here you put the actual resource type you want to import
	for x in import_resource.get_property_list():
		if x[&"usage"] & PROPERTY_USAGE_EDITOR != 0 and x[&"name"] != "script" and not x[&"name"].begins_with("resource_"):
			if data.has(x[&"name"]):
				var value = data[x[&"name"]]
				if value is String:
					if not value.is_empty() and value.begins_with("\"") and value.ends_with("\""):
							value = JSON.parse_string(value)
					if value.begins_with("res://") and ResourceLoader.exists(value):
						value = ResourceLoader.load(value)
					else:
						if x[&"hint"] == 2 and not x[&"hint_string"].is_empty():
							var hint_string_dict = JSON.parse_string("{\"%s}" % (x[&"hint_string"] as String).replace(",",",\"").replace(":","\":"))
							if (hint_string_dict.keys() as Array).has(value):
								value = hint_string_dict[value]
						else:
							if value == "null":
								continue

				import_resource.set(x[&"name"],value)
	return save_new_or_update_resource("%s/%s.tres" % [get_resource_folder(), resource_name], import_resource)
)");
    // print_line("Template created");
    file->close();
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
