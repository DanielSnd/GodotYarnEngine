//
// Created by Daniel on 2024-01-22.
//

#ifndef CATBUTTONSEDITORPLUGIN_H
#define CATBUTTONSEDITORPLUGIN_H

#ifdef TOOLS_ENABLED
#include "editor/editor_plugin.h"
#include "editor/editor_inspector.h"
#include "scene/gui/separator.h"
#include "scene/gui/button.h"
#include "scene/gui/line_edit.h"
#include "../yarnengine.h"

namespace yarnengine {
        class CatButtonsInspectorPlugin;

        class CatButtonsEditorPlugin : public EditorPlugin {
        GDCLASS(CatButtonsEditorPlugin, EditorPlugin);

        protected:
            Ref<CatButtonsInspectorPlugin> inspector_plugin;
        public:
                inline static CatButtonsEditorPlugin* singleton = nullptr;
            CatButtonsEditorPlugin();
            ~CatButtonsEditorPlugin();
    };


class YarnEditorFunctionButton : public HBoxContainer {
        GDCLASS(YarnEditorFunctionButton, HBoxContainer)
protected:
        static void _bind_methods();
public:
        String method_name = "";
        LineEdit* args = nullptr;
        Control* spacer= nullptr;
        Control* spacer2= nullptr;
        Button* button= nullptr;
        Object* object= nullptr;
        bool use_arg = false;
        ScriptInstance* old_reference_1 = nullptr;
        ScriptInstance* old_reference_2 = nullptr;
        ScriptInstance* old_reference_3 = nullptr;
        virtual void actually_initialize();
        virtual void on_button_pressed();

        virtual void setup_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide);

        YarnEditorFunctionButton();
};


class YarnEditorCategory : public YarnEditorFunctionButton {
        GDCLASS(YarnEditorCategory, YarnEditorFunctionButton)

public:
        void actually_initialize() override;
        void setup_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide) override;
};

class YarnEditorSeparator : public HSeparator {
        GDCLASS(YarnEditorSeparator, HSeparator)
        public:
                YarnEditorSeparator();
};

class CatButtonsInspectorPlugin : public EditorInspectorPlugin {
        GDCLASS(CatButtonsInspectorPlugin, EditorInspectorPlugin)
public:
        bool can_handle(Object *p_object) override {
                return true;
        }
        bool parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide) override {
                if (p_path.contains("_btn_")) {
                        YarnEditorFunctionButton* spawned_new_control = memnew(YarnEditorFunctionButton);
                        add_custom_control(spawned_new_control);
                        spawned_new_control->setup_property(p_object,p_type,p_path,p_hint,p_hint_text,p_usage,p_wide);
                        spawned_new_control->actually_initialize();
                        return true;
                }
                if (p_path.contains("_c_")) {
                        auto spawned_new_control = memnew(YarnEditorCategory);
                        add_custom_control(spawned_new_control);
                        spawned_new_control->setup_property(p_object,p_type,p_path,p_hint,p_hint_text,p_usage,p_wide);
                        spawned_new_control->actually_initialize();
                        return true;
                }
                if (p_path.contains("_sep_")) {
                        add_custom_control(memnew(YarnEditorSeparator));
                        return true;
                }
             return false;
        }
};

}

#endif //TOOLS ENABLED
#endif //CATBUTTONSEDITORPLUGIN_H
