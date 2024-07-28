//
// Created by Daniel on 2024-06-02.
//

#ifndef GSHEETIMPORTER_H
#define GSHEETIMPORTER_H
#include "core/object/ref_counted.h"
#include "scene/main/http_request.h"


class GSheetImporter : public RefCounted {
    GDCLASS(GSheetImporter, RefCounted);

protected:
    static void _bind_methods();

public:

    typedef void (*ImportBeginFunc)(int, const String &, const String &);
    typedef void (*ImportStepFunc)(int, const String &);
    typedef void (*ImportEndFunc)();

    HTTPRequest *http_request = nullptr;

    String last_importing_type;
    void request_import();

    String get_string_from_utf8(const PackedByteArray &p_instance);

    void on_import_completed(int p_status, int p_code, const PackedStringArray &p_headers, const PackedByteArray &p_data);

    void save_resource_if_different(const String &resource_path, const Ref<Resource> &resource_saving);

    void handle_import_on_gdscript(const String &sheet_name, const Array &entries);
    int str_hash(String s);

    String get_spreadsheet_url();

    void ensure_http_request_is_child();

    static ImportBeginFunc import_begin_function;
    static ImportStepFunc import_step_function;
    static ImportEndFunc import_end_function;

    GDVIRTUAL2(on_imported_data, String, Array)
    GDVIRTUAL0RC(String,get_sheet_id)
    GDVIRTUAL0RC(String,get_sheet_name)
    void start_import_progress_bar(int p_steps, const String &p_task_name, const String &p_task_description);
    void step_import_progress_bar(int p_step, const String &p_step_description);
    void end_import_progress_bar();

    GSheetImporter();
    ~GSheetImporter();
};


#endif //GSHEETIMPORTER_H
