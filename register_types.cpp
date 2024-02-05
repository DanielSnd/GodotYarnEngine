#include "register_types.h"

#include "core/object/class_db.h"
#include "yarnengine.h"
#include "core/config/engine.h"
#include "editor/AOBakeEditorPlugin.h"
#include "editor/CatButtonsEditorPlugin.h"

// This is your singleton reference.
static YarnEngine* YarnEnginePtr;

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
 	ClassDB::register_class<YarnEngine>();
 	ClassDB::register_class<AOBakeableMeshInstance>();

	// Initialize your singleton.
	YarnEnginePtr = memnew(YarnEngine);

	// Bind your singleton.
	Engine::get_singleton()->add_singleton(Engine::Singleton("YarnEngine", YarnEngine::get_singleton()));
}

void uninitialize_yarnengine_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
 			return;
    }

	Engine::get_singleton()->remove_singleton("YarnEngine");
	if (YarnEnginePtr != nullptr) {
		memdelete(YarnEnginePtr);
		YarnEnginePtr = nullptr;
	}
   // Nothing to do here in this example.
}