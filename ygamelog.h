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
#include "yarnengine.h"

class YGameLog : public RefCounted {
    GDCLASS(YGameLog, RefCounted);

protected:
    static YGameLog* singleton;
    static void _bind_methods();


public:
    static YGameLog *get_singleton();

    bool output_logging_on = false;
    void SetupOutputLogging();

    Vector<String> logged;
    Vector<Variant> loggedVariants;

    int get_log_count() const {return static_cast<int>(logged.size());}
    Vector<String> get_logged() const {return logged;}
    void set_logged(const Vector<String> &p_logged) {logged = p_logged;}
    void log_message(const String &new_message) { logged.push_back(new_message); }

    void clear_log() { logged.clear(); }

    int get_variant_log_count() const {return static_cast<int>(loggedVariants.size());}
    Vector<Variant> get_logged_variants() const {return loggedVariants;}
    void set_logged_variants(const Vector<Variant> &p_logged) {loggedVariants = p_logged;}
    void log_variant(const Variant &new_message) { loggedVariants.push_back(new_message); }

    void clear_variant_log() { loggedVariants.clear(); }


    YGameLog() {
        singleton = this;
    }

    ~YGameLog() {
        logged.clear();
        loggedVariants.clear();
        if(singleton != nullptr && singleton == this) {
            singleton = nullptr;
        }
    }

};


class YLogger : public Logger {

public:
    virtual void logv(const char *p_format, va_list p_list, bool p_err) override _PRINTF_FORMAT_ATTRIBUTE_2_0;

    void logf_error_custom(const char *p_format, ...) _PRINTF_FORMAT_ATTRIBUTE_2_3;
    bool last_pushed_was_warning=false;
    bool last_pushed_was_error=false;
    virtual void log_error(const char *p_function, const char *p_file, int p_line, const char *p_code, const char *p_rationale, bool p_editor_notify, ErrorType p_type = ERR_ERROR, const Vector<Ref<ScriptBacktrace>> &p_script_backtraces = {}) override;
    virtual ~YLogger() {}
};


#endif //YGAMELOG_H
