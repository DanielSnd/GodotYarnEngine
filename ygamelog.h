//
// Created by Daniel on 2024-03-19.
//

#ifndef YGAMELOG_H
#define YGAMELOG_H
#include "core/config/project_settings.h"
#include "core/io/logger.h"
#include "core/object/ref_counted.h"
#include "core/os/os.h"
#include "core/string/ustring.h"
#include <stdarg.h>

class YGameLog : public RefCounted {
    GDCLASS(YGameLog, RefCounted);

protected:
    static Ref<YGameLog> singleton;
    static void _bind_methods();


public:
    static YGameLog *get_singleton();

    static void set_singleton(const Ref<YGameLog> &ref) {
        singleton = ref;
    }

    bool output_logging_on = false;
    void SetupOutputLogging();

    Vector<String> logged;

    YGameLog() { }

    ~YGameLog() {
        logged.clear();
        if(singleton.is_valid() && singleton == this) {
            singleton.unref();
        }
    }

};


class YLogger : public Logger {

public:
    virtual void logv(const char *p_format, va_list p_list, bool p_err) override _PRINTF_FORMAT_ATTRIBUTE_2_0;

    void logf_error_custom(const char *p_format, ...) _PRINTF_FORMAT_ATTRIBUTE_2_3;
    bool last_pushed_was_warning=false;
    bool last_pushed_was_error=false;
    virtual void log_error(const char *p_function, const char *p_file, int p_line, const char *p_code, const char *p_rationale, bool p_editor_notify, ErrorType p_type = ERR_ERROR) override;
    virtual ~YLogger() {}
};


#endif //YGAMELOG_H
