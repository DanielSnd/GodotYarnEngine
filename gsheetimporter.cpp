//
// Created by Daniel on 2024-06-02.
//

#include "gsheetimporter.h"

#include "yarnengine.h"
#include "core/io/json.h"
#if TOOLS_ENABLED
#include "editor/GSheetImporterEditorPlugin.h"
#endif

void GSheetImporter::_bind_methods() {
    ADD_SIGNAL(MethodInfo("imported_data", PropertyInfo(Variant::STRING, "import_type"), PropertyInfo(Variant::ARRAY, "data")));
    ClassDB::bind_method(D_METHOD("request_import"), &GSheetImporter::request_import);
    GDVIRTUAL_BIND(get_sheet_id)
    GDVIRTUAL_BIND(get_sheet_name)
    GDVIRTUAL_BIND(on_imported_data,"import_type","data")
}

void GSheetImporter::request_import() {
    String sheet_name = "";
    GDVIRTUAL_CALL(get_sheet_name,sheet_name);
    // print_line(vformat("Requested Import %s" ,sheet_name));

    Vector<String> p_headers;
    p_headers.push_back("Content-Type: application/json");
    p_headers.push_back("X-DataSource-Auth: true");
    auto desired_url = get_spreadsheet_url();
    last_importing_type = sheet_name;
    auto error = http_request->request(desired_url,p_headers);
    //print("Requesting ",desired_url, " headers ",headers)
    if (error != OK)
        ERR_PRINT(vformat("An error %s occurred in the HTTP request. Url: %s Headers %s",error,desired_url, p_headers));
}

String GSheetImporter::get_string_from_utf8(const PackedByteArray &p_instance) {
    String s;
    if (p_instance.size() > 0) {
        const uint8_t *r = p_instance.ptr();
        s.parse_utf8((const char *)r, p_instance.size());
    }
    return s;
}

void GSheetImporter::on_import_completed(int p_status, int p_code, const PackedStringArray &p_headers,
    const PackedByteArray &p_data) {
    String data_csv = get_string_from_utf8(p_data);
    //print_line("data_csv_was: ",data_csv);
    int pos = data_csv.find("\"version\":");
    if(pos != -1) {
         data_csv = data_csv.substr(pos-1, -1).strip_edges();
    }
        //String("{\"version\":") + string_from_utf8.substr(string_from_utf8.find("\"version\":")+1, -1);
    // print_line("data_csv_after: ",data_csv);
    Array columnNames;
    Array entries;

    const Dictionary parsedjson = JSON::parse_string(data_csv);
    // print_line(vformat("Parsed json %s",parsedjson));
    const Dictionary parsed_table = parsedjson["table"];

    // print_line(vformat("Parsed table %s",parsed_table));
    const Array jsonArray = parsed_table["rows"];

    // print_line(vformat("Parsed json array %s",jsonArray));
    const Array jsonColsArray = parsed_table["cols"];

    // print_line(vformat("Parsed json cols array %s",jsonColsArray));

    for (int i = 0; i<jsonColsArray.size(); i++) {
        const Dictionary col_dict = jsonColsArray[i];
        String line = col_dict["label"];
        // print_line(vformat("i %d line %s",i,line));
        if (line.is_empty() || line.length() <= 1){
            columnNames.append(Variant(i));
        } else if (line.length() > 1) {
            if (line.find(" ") != -1) {
                line = line.replace(" ", "");
            }
            columnNames.append(line);
            if (columnNames[0].operator String() == "0" && columnNames[1].operator String() == "1" && columnNames[2].operator String() == "2") {
                columnNames.clear();
            }
        }
    }


    // print_line(vformat("Column names %s",columnNames));

    for (int i = 0; i<jsonArray.size(); i++) {
        const Dictionary json_line_disct = jsonArray[i];
        Array line = json_line_disct["c"];
        if (columnNames.size()<=0) {
            // IS COLUMN
            for (int index=0; index < line.size(); index++) {
                const Dictionary line_index = line[index];
                // print_line(vformat("Is column. Line index  %s",line_index));
                if (line_index.is_empty() || (line_index.has("v") && line_index["v"].get_type() != Variant::NIL)) break;
                String column_name = line_index["v"];
                if (column_name.find(" ") != -1) {
                    column_name = column_name.replace(" ", "");
                }
                columnNames.push_back(column_name);
            }
        } else {
            // IS ROW
            Dictionary entryDict;
            int amount_parse = MIN(line.size(),columnNames.size());
            // print_line(vformat("IS ROW amount parse %d",amount_parse));
            for (int index=0; index<amount_parse; index++) {
                const Dictionary line_index = line[index];
                // print_line(vformat("Row Line index %s not empty? %s has v ? %s", line_index,!line_index.is_empty(), line_index.has("v")));
                if (!line_index.is_empty() && line_index.has("v")) {
                    Variant found_result = line_index.get("v",Variant{});

                    // print_line(vformat("Found result %s. is null? %s",found_result,found_result.is_null()));
                    if (found_result.get_type() != Variant::NIL)
                        entryDict[String(columnNames[index])] = line_index["v"];
                }
            }
            if (!entryDict.is_empty()) {
                entries.append(entryDict);
            }
        }
    }
    // emit_signal("imported_data", data_type, entries);
    String sheet_name = "";
    GDVIRTUAL_CALL(get_sheet_name,sheet_name);
    GDVIRTUAL_CALL(on_imported_data,sheet_name,entries);

    if (http_request != nullptr) {
        http_request->queue_free();
        http_request = nullptr;
    }

    http_request = memnew(HTTPRequest);
    if (!Engine::get_singleton()->is_editor_hint()) {
            YEngine::get_singleton()->add_child(http_request);
    } else {
#if TOOLS_ENABLED
        GSheetImporterEditorPlugin::get_singleton()->call_deferred("add_child",http_request);
#endif
    }
    http_request->connect("request_completed", callable_mp(this,&GSheetImporter::on_import_completed));

//
// #if TOOLS_ENABLED
//     if (GSheetImporterEditorPlugin::get_singleton() != nullptr) {
//         if(GSheetImporterEditorPlugin::get_singleton()->currently_importing.has(this)) {
//             GSheetImporterEditorPlugin::get_singleton()->currently_importing.erase(this);
//         }
//     }
// #endif

}

int GSheetImporter::str_hash(String s) {
    s = s.to_upper().strip_edges();
    int hash = 5381;

    for (int i=0; i<s.length(); i++) {
        hash = static_cast<int>((((hash & 0x7fffffff) << 5) + hash) + s.unicode_at(i)); // hash * 33 + c
    }

    return static_cast<int>((hash & 0xffffffff) - 0x80000000); // Convert to 32-bit signed integer
}

String GSheetImporter::get_spreadsheet_url() {
    String sheet_id = "";
    GDVIRTUAL_CALL(get_sheet_id,sheet_id);
    if(sheet_id.find(" ") != -1) {
        sheet_id = sheet_id.replace(" ", "%20");
    }
    String sheet_name = "";
    GDVIRTUAL_CALL(get_sheet_name,sheet_name);
    if(sheet_name.find(" ") != -1) {
        sheet_name = sheet_name.replace(" ", "%20");
    }
    return vformat("https://docs.google.com/a/google.com/spreadsheets/d/%s/gviz/tq?tqx=out:json&tq&&sheet=%s&headers=1",sheet_id, sheet_name);
}

void GSheetImporter::ensure_http_request_is_child() {
    if (http_request != nullptr && !http_request->is_inside_tree()) {
        if (!Engine::get_singleton()->is_editor_hint()) {
            if (!YEngine::get_singleton()->is_inside_tree()) {
                YEngine::get_singleton()->connect("initialized",callable_mp(this,&GSheetImporter::ensure_http_request_is_child), CONNECT_ONE_SHOT);
            } else {
                YEngine::get_singleton()->add_child(http_request);
            }
        } else {
#if TOOLS_ENABLED
        GSheetImporterEditorPlugin::get_singleton()->call_deferred("add_child",http_request);
#endif
        }
    }
}

GSheetImporter::GSheetImporter() {
    http_request = memnew(HTTPRequest);
    if (!Engine::get_singleton()->is_editor_hint()) {
        if (!YEngine::get_singleton()->is_inside_tree()) {
            YEngine::get_singleton()->connect("initialized",callable_mp(this,&GSheetImporter::ensure_http_request_is_child), CONNECT_ONE_SHOT);
        } else {
            YEngine::get_singleton()->add_child(http_request);
        }
    } else {
#if TOOLS_ENABLED
        GSheetImporterEditorPlugin::get_singleton()->call_deferred("add_child",http_request);
#endif
    }
    http_request->connect("request_completed", callable_mp(this,&GSheetImporter::on_import_completed));
}

GSheetImporter::~GSheetImporter() {
}
