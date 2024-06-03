#include "register_types.h"

#include "core/object/class_db.h"
#include "yarnengine.h"
#include "core/config/engine.h"
#ifdef TOOLS_ENABLED
#include "editor/AOBakeEditorPlugin.h"
#include "editor/CatButtonsEditorPlugin.h"
#include "editor/AssetPlacer.h"
#include "editor/GSheetImporterEditorPlugin.h"
#endif
#include "AOBakeableMeshInstance.h"
#include "CircleWithConeTexture2D.h"
#include "gsheetimporter.h"
#include "yarnmenu.h"
#include "yarnphysics.h"
#include "yarnsave.h"
#include "yarntime.h"
#include "yboxcontainer.h"
#include "yvisualelement3d.h"
#include "ygamestate.h"
#include "yvisualaction.h"
#include "yspecialpoint3d.h"
#include "ygameaction.h"
#include "ystate.h"
#include "ystateoverride.h"
#include "ygamelog.h"
#include "yscenespawner3d.h"
#include "ytween.h"
#include "ythreader.h"
// This is your singleton reference.
static YEngine* YEnginePtr;
static Ref<YSave> yarn_save_ref;
static Ref<YPhysics> yarn_physics_ref;
static Ref<YGameState> yarn_gamestate_ref;
static Ref<YTime> yarn_time_ref;
static Ref<YGameLog> yarn_game_log_ref;
static Ref<YTween> yarn_tween_ref;

void initialize_yarnengine_module(ModuleInitializationLevel p_level) {
#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		ClassDB::register_class<yarnengine::AssetPlacerPlugin>();
		ClassDB::register_class<yarnengine::CatButtonsEditorPlugin>();
		ClassDB::register_class<yarnengine::CatButtonsInspectorPlugin>();
		ClassDB::register_class<yarnengine::YarnEditorFunctionButton>();
		ClassDB::register_class<yarnengine::YarnEditorCategory>();
		ClassDB::register_class<yarnengine::YarnEditorSeparator>();
		ClassDB::register_class<AOBakeEditorPlugin>();
		ClassDB::register_class<GSheetImporterEditorPlugin>();
		ClassDB::register_class<yarnengine::AssetPanelEditorBottomPanel>();
		EditorPlugins::add_by_type<yarnengine::CatButtonsEditorPlugin>();
		EditorPlugins::add_by_type<yarnengine::AssetPlacerPlugin>();
		EditorPlugins::add_by_type<AOBakeEditorPlugin>();
		EditorPlugins::add_by_type<GSheetImporterEditorPlugin>();
	}
#endif

 	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
 			return;
	}

	ClassDB::register_class<GSheetImporter>();
	ClassDB::register_class<YTime>();
	ClassDB::register_class<YSave>();
	ClassDB::register_class<YPhysics>();
 	ClassDB::register_class<YEngine>();
	ClassDB::register_class<YMenu>();
	ClassDB::register_class<YBoxContainer>();
	ClassDB::register_class<YHBoxContainer>();
	ClassDB::register_class<YVBoxContainer>();
	ClassDB::register_class<YTweenWrap>();
	ClassDB::register_class<YTween>();
	ClassDB::register_class<YSpecialPoint3D>();
	ClassDB::register_class<YVisualElement3D>();
	ClassDB::register_class<YGamePlayer>();
	ClassDB::register_class<YVisualAction>();
	ClassDB::register_class<YActionStep>();
	ClassDB::register_class<YGameAction>();
	ClassDB::register_class<YGameState>();
	ClassDB::register_class<YGameLog>();
	ClassDB::register_class<YState>();
	ClassDB::register_class<YStateMachine>();
	ClassDB::register_class<YStateOverride>();
	ClassDB::register_class<CircleWithConeTexture2D>();
 	ClassDB::register_class<AOBakeableMeshInstance>();
 	ClassDB::register_class<YThreader>();
 	ClassDB::register_class<YThreaderInterrupter>();
 	ClassDB::register_class<YSceneSpawner3D>();

	// Initialize your singleton.
	YEnginePtr = memnew(YEngine);
	YEnginePtr->add_setting("application/config/window_name", "", Variant::Type::STRING);

	yarn_save_ref.instantiate();
	YSave::set_singleton(yarn_save_ref);

	yarn_time_ref.instantiate();
	YTime::set_singleton(yarn_time_ref);

	yarn_physics_ref.instantiate();
	YPhysics::set_singleton(yarn_physics_ref);

	yarn_gamestate_ref.instantiate();
	YGameState::set_singleton(yarn_gamestate_ref);

	yarn_game_log_ref.instantiate();
	YGameLog::set_singleton(yarn_game_log_ref);

	yarn_tween_ref.instantiate();
	YTween::set_singleton(yarn_tween_ref);

	// Bind your singleton.
	Engine::get_singleton()->add_singleton(Engine::Singleton("YEngine", YEngine::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("YSave", YSave::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("YTime", YTime::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("YPhysics", YPhysics::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("YGameState", YGameState::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("YGameLog", YGameLog::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("YTween", YTween::get_singleton()));
}

void uninitialize_yarnengine_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
 			return;
    }
	Engine::get_singleton()->remove_singleton("YEngine");
	Engine::get_singleton()->remove_singleton("YSave");
	Engine::get_singleton()->remove_singleton("YTime");
	Engine::get_singleton()->remove_singleton("YPhysics");
	Engine::get_singleton()->remove_singleton("YGameState");
	Engine::get_singleton()->remove_singleton("YGameLog");
	Engine::get_singleton()->remove_singleton("YTween");
	if (yarn_save_ref != nullptr && yarn_save_ref.is_valid())
		yarn_save_ref.unref();
	if (yarn_time_ref != nullptr && yarn_time_ref.is_valid())
		yarn_time_ref.unref();
	if (yarn_physics_ref != nullptr && yarn_physics_ref.is_valid())
		yarn_physics_ref.unref();
	if (yarn_gamestate_ref != nullptr && yarn_gamestate_ref.is_valid())
		yarn_gamestate_ref.unref();
	if (yarn_game_log_ref != nullptr && yarn_game_log_ref.is_valid())
		yarn_game_log_ref.unref();
	if (yarn_tween_ref != nullptr && yarn_tween_ref.is_valid())
		yarn_tween_ref.unref();
	if (YEnginePtr != nullptr) {
		memdelete(YEnginePtr);
	}
	YEnginePtr = nullptr;
   // Nothing to do here in this example.
}