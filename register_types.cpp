#include "register_types.h"

#include "core/object/class_db.h"
#include "yarnengine.h"
#include "core/config/engine.h"
#ifdef TOOLS_ENABLED
#include "editor/AOBakeEditorPlugin.h"
#include "editor/CatButtonsEditorPlugin.h"
#endif
#include "AOBakeableMeshInstance.h"
#include "yarnmenu.h"
#include "yarnphysics.h"
#include "yarnsave.h"
#include "yarntime.h"
// This is your singleton reference.
static YEngine* YEnginePtr;
static Ref<YSave> yarn_save_ref;
static Ref<YPhysics> yarn_physics_ref;
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
	ClassDB::register_class<YPhysics>();
 	ClassDB::register_class<YEngine>();
 	ClassDB::register_class<YMenu>();
 	ClassDB::register_class<AOBakeableMeshInstance>();

	// Initialize your singleton.
	YEnginePtr = memnew(YEngine);
	YEnginePtr->add_setting("application/config/window_name", "", Variant::Type::STRING);

	yarn_save_ref.instantiate();
	YSave::set_singleton(yarn_save_ref);

	yarn_time_ref.instantiate();
	YTime::set_singleton(yarn_time_ref);

	yarn_physics_ref.instantiate();
	YPhysics::set_singleton(yarn_physics_ref);

	// Bind your singleton.
	Engine::get_singleton()->add_singleton(Engine::Singleton("YEngine", YEngine::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("YSave", YSave::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("YTime", YTime::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("YPhysics", YPhysics::get_singleton()));
}

void uninitialize_yarnengine_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
 			return;
    }
	Engine::get_singleton()->remove_singleton("YEngine");
	Engine::get_singleton()->remove_singleton("YSave");
	Engine::get_singleton()->remove_singleton("YTime");
	Engine::get_singleton()->remove_singleton("YPhysics");
	if (yarn_save_ref != nullptr && yarn_save_ref.is_valid())
		yarn_save_ref.unref();
	if (yarn_time_ref != nullptr && yarn_time_ref.is_valid())
		yarn_time_ref.unref();
	if (yarn_physics_ref != nullptr && yarn_physics_ref.is_valid())
		yarn_physics_ref.unref();
	if (YEnginePtr != nullptr) {
		memdelete(YEnginePtr);
	}
	YEnginePtr = nullptr;
   // Nothing to do here in this example.
}