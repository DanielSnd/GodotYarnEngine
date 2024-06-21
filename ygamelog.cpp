//
// Created by Daniel on 2024-03-19.
//

#include "ygamelog.h"

Ref<YGameLog> YGameLog::singleton;

void YGameLog::_bind_methods() {
    ClassDB::bind_method(D_METHOD("enable_output_logging"), &YGameLog::SetupOutputLogging);
    ADD_SIGNAL(MethodInfo("system_log_printed", PropertyInfo(Variant::STRING, "printed")));
    ADD_SIGNAL(MethodInfo("system_error_printed", PropertyInfo(Variant::STRING, "printed")));
    ADD_SIGNAL(MethodInfo("system_warning_printed", PropertyInfo(Variant::STRING, "printed")));

    ClassDB::bind_method(D_METHOD("get_log_count"), &YGameLog::get_log_count);
    ClassDB::bind_method(D_METHOD("get_logged"), &YGameLog::get_logged);
    ClassDB::bind_method(D_METHOD("set_logged", "logged"), &YGameLog::set_logged);
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "logged"), "set_logged", "get_logged");

    ClassDB::bind_method(D_METHOD("log_message", "new_message"), &YGameLog::log_message);
    ClassDB::bind_method(D_METHOD("clear_log"), &YGameLog::clear_log);

    ADD_SIGNAL(MethodInfo("on_message_logged", PropertyInfo(Variant::STRING, "message")));
}

YGameLog *YGameLog::get_singleton() {
    return *singleton;
}

void YGameLog::SetupOutputLogging() {
    if (!output_logging_on) {
        output_logging_on=true;
        OS::get_singleton()->add_extra_logger(memnew(YLogger));
    }
}

void YLogger::logv(const char *p_format, va_list p_list, bool p_err) {
    if (!should_log(p_err)) {
        return;
    }
    const int static_buf_size = 512;
    char static_buf[static_buf_size];
    char *buf = static_buf;
    va_list list_copy;
    va_copy(list_copy, p_list);
    int len = vsnprintf(buf, static_buf_size, p_format, p_list);
    if (len >= static_buf_size) {
        buf = (char *)memalloc(len + 1);
        vsnprintf(buf, len + 1, p_format, list_copy);
    }
    va_end(list_copy);
    if(last_pushed_was_warning) {
        YGameLog::get_singleton()->emit_signal("system_warning_printed",String::utf8(buf));
    }else if(last_pushed_was_error) {
        YGameLog::get_singleton()->emit_signal("system_error_printed",String::utf8(buf));
    }else {
        YGameLog::get_singleton()->emit_signal("system_log_printed",String::utf8(buf));
    }

    if (len >= static_buf_size) {
        memfree(buf);
    }
    last_pushed_was_warning=false;
    last_pushed_was_error=false;
}

void YLogger::logf_error_custom(const char *p_format, ...) {
    va_list argp;
    va_start(argp, p_format);

    logv(p_format, argp, false);

    last_pushed_was_error=false;
    last_pushed_was_warning=false;
    va_end(argp);
}

void YLogger::log_error(const char *p_function, const char *p_file, int p_line, const char *p_code,
                        const char *p_rationale, bool p_editor_notify, ErrorType p_type) {
    // if (!should_log(true)) {
    //     return;
    // }

    const char *err_type = "ERROR";
    bool do_push_error=false;
    switch (p_type) {
        case ERR_ERROR:
            err_type = "ERROR";
            do_push_error=true;
            last_pushed_was_error=true;
            last_pushed_was_warning=false;
        break;
        case ERR_WARNING:
            err_type = "WARNING";
            do_push_error=true;
            last_pushed_was_warning=true;
            last_pushed_was_error=false;
        break;
        case ERR_SCRIPT:
            err_type = "SCRIPT ERROR";
            last_pushed_was_error=true;
            last_pushed_was_warning=false;
            do_push_error=true;
        break;
        case ERR_SHADER:
            err_type = "SHADER ERROR";
        break;
        default:
            err_type = "ERROR";
            do_push_error=true;
            last_pushed_was_error=true;
            last_pushed_was_warning=false;
        break;
    }

    const char *err_details;
    if (p_rationale && *p_rationale) {
        err_details = p_rationale;
    } else {
        err_details = p_code;
    }

    if(do_push_error){
        bool temp_was_error = last_pushed_was_error;
        bool temp_was_warning = last_pushed_was_warning;
        logf_error_custom("%s: %s\n", err_type, err_details);
        last_pushed_was_warning = temp_was_warning;
        last_pushed_was_error = temp_was_error;
        logf_error_custom("   at: %s (%s:%i)\n", p_function, p_file, p_line);
    }
}
