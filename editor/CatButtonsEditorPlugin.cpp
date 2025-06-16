//
// Created by Daniel on 2024-01-22.
//


#ifdef TOOLS_ENABLED

#include "editor/editor_node.h"
#include "CatButtonsEditorPlugin.h"

#include "core/object/script_language.h"

yarnengine::CatButtonsEditorPlugin::CatButtonsEditorPlugin() {
        singleton = this;
        //if (EditorNode::get_singleton() == nullptr || !EditorNode::get_singleton()->is_editor_ready()) return;
        inspector_plugin.instantiate();
        add_inspector_plugin(inspector_plugin);
}

yarnengine::CatButtonsEditorPlugin::~CatButtonsEditorPlugin() {
        if(singleton != nullptr && singleton == this) {
                singleton = nullptr;
        }
        if (!inspector_plugin.is_null() && inspector_plugin.is_valid()) {
                inspector_plugin = nullptr;
        }
}

void yarnengine::YarnEditorFunctionButton::_bind_methods() {
        ClassDB::bind_method(D_METHOD("actually_initialize"), &YarnEditorFunctionButton::actually_initialize);
}

void yarnengine::YarnEditorFunctionButton::actually_initialize() {
        if (button != nullptr) return;
        set_alignment(ALIGNMENT_CENTER);
        set_h_size_flags(SIZE_EXPAND_FILL);
        spacer = memnew(Control);
        add_child(spacer);
        spacer->set_h_size_flags(SIZE_EXPAND_FILL);
        spacer->set_stretch_ratio(use_arg ? 0.33 : 0.25);
        button = memnew(Button);
        add_child(button);
        button->set_text_alignment(HORIZONTAL_ALIGNMENT_CENTER);
        button->set_modulate(Color(1.3,1.5,0.8));
        button->set_h_size_flags(SIZE_EXPAND_FILL);
        button->set_stretch_ratio(0.5);
        button->set_text(method_name.capitalize());
        // button->set_tooltip_text(vformat("Click to call %s",method_name));
        button->connect(SNAME("pressed"),callable_mp(this,&YarnEditorFunctionButton::on_button_pressed));
        button->set_disabled(false);

        if (use_arg) {
                args = memnew(LineEdit);
                add_child(args);
                args->set_h_size_flags(SIZE_EXPAND_FILL);
                args->set_stretch_ratio(0.33f);
                args->set_modulate(Color(1.1,1.1,1.1));
                args->set_placeholder("[ Argument ]");
                args->set_horizontal_alignment(HorizontalAlignment::HORIZONTAL_ALIGNMENT_CENTER);
        } else {
                spacer2 = memnew(Control);
                add_child(spacer2);
                spacer2->set_h_size_flags(SIZE_EXPAND_FILL);
                spacer2->set_stretch_ratio(0.25f);
        }
}

void yarnengine::YarnEditorFunctionButton::on_button_pressed() {
        int set_used_args = -1;
        int use_int = 0;
        float use_float = 0.0;
        if(use_arg && args != nullptr) {
                auto typed_text = args->get_text();
                if (typed_text.is_valid_int()) {
                        use_int = typed_text.to_int();
                        set_used_args = 0;
                } else if (typed_text.is_valid_float()) {
                        use_float = typed_text.to_float();
                        set_used_args= 1;
                } else {
                        set_used_args = 2;
                }
        } else {
                //print_line("Object ",object->get_class_name()," method desired: [",method_name,"] object has? ",object->has_method(method_name)," object has scriptinstance? ",object->get_script_instance() != nullptr);
                if (ScriptInstance* _script_instance = (object->get_script_instance()); _script_instance != nullptr) {
                        List<Pair<StringName, Variant>> property_states;
                        _script_instance->get_property_state(property_states);

                        object->set_script_instance(_script_instance->get_script()->instance_create(object));
                        _script_instance = object->get_script_instance();
                        for (const auto& property: property_states) {
                                _script_instance->set(property.first,property.second);
                        }

                        Variant ret;
                        Callable::CallError cerr;
                        const StringName new_string_name{method_name};
                        if(set_used_args == -1 || args == nullptr) {
                                const Variant ** emptyargs {};
                                ret = _script_instance->callp(new_string_name, emptyargs, 0, cerr);
                        } else if (set_used_args == 0) {
                                const Variant myVariant {use_int};
                                const Variant* _args[1] = {&myVariant};
                                ret = _script_instance->callp(new_string_name, _args, 1, cerr);
                        }else if (set_used_args == 1) {
                                const Variant myVariant {use_float};
                                const Variant* _args[1] = {&myVariant};
                                ret = _script_instance->callp(new_string_name, _args, 1, cerr);
                        }else if (set_used_args == 2) {
                                const Variant myVariant{args->get_text()};
                                const Variant* _args[1] = {&myVariant};
                                ret = _script_instance->callp(new_string_name, _args, 1, cerr);
                        }
                        //force jumptable
                        if(cerr.error > 0 && cerr.error <=5) {
                                print_line("Error calling ",new_string_name," with error: ",cerr.error);
                        }
                        switch (cerr.error) {
                                case Callable::CallError::CALL_OK:
                                        if (!ret.is_null()) {
                                                print_line("Return value ",ret);
                                        }
                                break;
                                case Callable::CallError::CALL_ERROR_METHOD_NOT_CONST:
                                        if (!ret.is_null()) {
                                                print_line("Return value ",ret);
                                        }
                                break;
                                default:
                                        break;
                        }

                        List<Pair<StringName, Variant>> new_property_states;
                        _script_instance->get_property_state(new_property_states);

                        object->set_script_instance(_script_instance->get_script()->placeholder_instance_create(object));
                        _script_instance = object->get_script_instance();
                        for (const auto& property: new_property_states) {
                                _script_instance->set(property.first,property.second);
                        }
                }
        }
}

void yarnengine::YarnEditorFunctionButton::setup_property(Object *p_object, const Variant::Type p_type,
                                                          const String &p_path, const PropertyHint p_hint, const String &p_hint_text,
                                                          const BitField<PropertyUsageFlags> p_usage, const bool p_wide) {
                object = p_object;
                use_arg = p_type == Variant::Type::STRING;
                //print_line("setup_property ",p_path);
                if (p_path.to_lower().begins_with("members")) {
                        method_name = p_path.trim_prefix("Members/_btn_");
                } else {
                        method_name = p_path.trim_prefix("_btn_");
                }

}

yarnengine::YarnEditorFunctionButton::YarnEditorFunctionButton() {
}

void yarnengine::YarnEditorCategory::actually_initialize() {
        YarnEditorFunctionButton::actually_initialize();
        //button->set_disabled(true);
        button->set_h_size_flags(SIZE_EXPAND_FILL);
        button->set_modulate(Color(1.5,1.5,1.5,0.8));
        //button->add_theme_color_override("font_color_disabled",Color(1.7,1.7,1.7));
        button->set_text(vformat(" ■  %s  ",method_name.capitalize()));
        button->set_focus_mode(FOCUS_NONE);
        button->set_mouse_filter(MOUSE_FILTER_IGNORE);
        button->set_text_alignment(HORIZONTAL_ALIGNMENT_LEFT);
}

void yarnengine::YarnEditorCategory::setup_property(Object *p_object, const Variant::Type p_type, const String &p_path,
                                                    const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage,
                                                    const bool p_wide) {
        YarnEditorFunctionButton::setup_property(p_object, p_type, p_path, p_hint, p_hint_text, p_usage, p_wide);

        method_name = p_path.trim_prefix("_c_");
}

yarnengine::YarnEditorSeparator::YarnEditorSeparator() {
        set_h_size_flags(SIZE_EXPAND_FILL);
        set_focus_mode(FOCUS_NONE);
        set_modulate(Color(1.5,1.5,1.5,1.0));
}

#endif