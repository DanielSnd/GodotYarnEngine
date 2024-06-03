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

    HTTPRequest *http_request = nullptr;

    String last_importing_type;
    void request_import();

    String get_string_from_utf8(const PackedByteArray &p_instance);

    void on_import_completed(int p_status, int p_code, const PackedStringArray &p_headers, const PackedByteArray &p_data);

    int str_hash(String s);

    String get_spreadsheet_url();

    void ensure_http_request_is_child();

    GDVIRTUAL2(on_imported_data, String, Array)
    GDVIRTUAL0RC(String,get_sheet_id)
    GDVIRTUAL0RC(String,get_sheet_name)

    GSheetImporter();
    ~GSheetImporter();
};


#endif //GSHEETIMPORTER_H
