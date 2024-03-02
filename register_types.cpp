#include "register_types.h"

#include "core/object/class_db.h"
#include "yarnengine.h"
#include "core/config/engine.h"
#ifdef TOOLS_ENABLED
#include "editor/AOBakeEditorPlugin.h"
#include "editor/CatButtonsEditorPlugin.h"
#endif
#include "AOBakeableMeshInstance.h"
#include "yarnsave.h"
#include "yarntime.h"
// This is your singleton reference.
static YEngine* YEnginePtr;
static Ref<YSave> yarn_save_ref;
static Ref<YTime> yarn_time_ref;
void initialize_yarnengine_module(ModuleInitializationLevel p_level) {
#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		ClassDB::register_class<yarnengine::CatButtonsEditorPlugin>();
		ClassDB::register_class<yarnengine::CatButtonsInspectorPlugin>();
		ClassDB::register_class<yarnengine::YarnEditorFunctionButton>();
		ClassDB::register_class<yarnengine::YarnEditorCategory>();
		ClassDB::register_class<yarnengine::YarnEditorSeparator>();
		EditorPlugins::add_by_type<yarnengine::CatButtonsEditorPlugin>();

		ClassDB::register_class<AOBakeEditorPlugin>();
		EditorPlugins::add_by_type<AOBakeEditorPlugin>();
	}
#endif

 	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
 			return;
	}

	ClassDB::register_class<YTime>();
	ClassDB::register_class<YSave>();
 	ClassDB::register_class<YEngine>();
 	ClassDB::register_class<AOBakeableMeshInstance>();

	// Initialize your singleton.
	YEnginePtr = memnew(YEngine);
	yarn_save_ref.instantiate();
	YSave::set_singleton(yarn_save_ref);

	yarn_time_ref.instantiate();
	YTime::set_singleton(yarn_time_ref);

	// Bind your singleton.
	Engine::get_singleton()->add_singleton(Engine::Singleton("YEngine", YEngine::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("YSave", YSave::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("YTime", YTime::get_singleton()));
}

void uninitialize_yarnengine_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
 			return;
    }
	if (YEnginePtr != nullptr) {
		memdelete(YEnginePtr);
	}
	Engine::get_singleton()->remove_singleton("YEngine");
	Engine::get_singleton()->remove_singleton("YSave");
	Engine::get_singleton()->remove_singleton("YTime");
	YEnginePtr = nullptr;
	yarn_save_ref.unref();
	yarn_time_ref.unref();
   // Nothing to do here in this example.
}