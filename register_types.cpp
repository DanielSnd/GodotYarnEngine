#include "register_types.h"

#include "core/object/class_db.h"
#include "yarnengine.h"
#include "core/config/engine.h"
#ifdef TOOLS_ENABLED
#include "editor/editor_yarn_3d_tools.h"
#include "editor/AOBakeEditorPlugin.h"
#include "editor/CatButtonsEditorPlugin.h"
#include "editor/AssetPlacer.h"
#include "editor/GSheetImporterEditorPlugin.h"
#endif
#include "AOBakeableMeshInstance.h"
#include "CircleWithConeTexture2D.h"
#include "CombinedTexture2D.h"
#include "gsheetimporter.h"
#include "yarnmenu.h"
#include "yarnphysics.h"
#include "ydir.h"
#include "yarnsave.h"
#include "yarntime.h"
#include "yeventdata.h"
#include "ybone_attachment_3d.h"
#include "yboxcontainer.h"
#include "yboxcontainer3D.h"
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
#include "ypersistentid.h"
#include "ymeshgen.h"
#include "ynavhelper3d.h"
#include "ynavhelper2d.h"
#include "ysaveable3d.h"
#include "importers/resource_importer_glb_as_mesh.h"

#ifdef NO_FMOD
#include "disabled_fmod/eventasset.h"
#endif

// This is your singleton reference.
static Ref<YSave> yarn_save_ref;
static Ref<YTime> yarn_time_ref;
static Ref<YPhysics> yarn_physics_ref;
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
		ClassDB::register_class<EditorYarn3DTools>();
		EditorPlugins::add_by_type<yarnengine::CatButtonsEditorPlugin>();
		EditorPlugins::add_by_type<yarnengine::AssetPlacerPlugin>();
		EditorPlugins::add_by_type<AOBakeEditorPlugin>();
		EditorPlugins::add_by_type<GSheetImporterEditorPlugin>();
	    EditorPlugins::add_by_type<EditorYarn3DTools>();
#ifdef NO_FMOD
		ClassDB::register_class<ProjectCache>();
#endif
	}
#endif

 	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
 			return;
	}

#ifdef TOOLS_ENABLED
	if (Engine::get_singleton()->is_editor_hint()) {
		Ref<ResourceImporterGLBasMesh> ogg_vorbis_importer;
		ogg_vorbis_importer.instantiate();
		ResourceFormatImporter::get_singleton()->add_importer(ogg_vorbis_importer);
	}

	ClassDB::APIType prev_api = ClassDB::get_current_api();
	ClassDB::set_current_api(ClassDB::API_EDITOR);

	GDREGISTER_CLASS(ResourceImporterGLBasMesh);

	ClassDB::set_current_api(prev_api);
#endif

#ifdef NO_FMOD
	ClassDB::register_class<EventAsset>();
	ClassDB::register_class<FMODAsset>();
	ClassDB::register_class<BankAsset>();
	ClassDB::register_class<EventAsset>();
	ClassDB::register_class<BusAsset>();
	ClassDB::register_class<VCAAsset>();
	ClassDB::register_class<ParameterAsset>();
	ClassDB::register_class<FMODGuidDictionary>();
	ClassDB::register_class<FmodTypes::FMOD_STUDIO_PARAMETER_ID>();
	ClassDB::register_class<FmodTypes::FMOD_STUDIO_PARAMETER_DESCRIPTION>();
#endif
	ClassDB::register_class<GSheetImporter>();
	ClassDB::register_class<YTime>();
	ClassDB::register_class<YSave>();
	ClassDB::register_class<YPhysics>();
	ClassDB::register_class<YDir>();
	ClassDB::register_class<YMeshGen>();
 	ClassDB::register_class<YEngine>();
	ClassDB::register_class<YMenu>();
	ClassDB::register_class<YEventData>();
	ClassDB::register_class<YBoxContainer>();
	ClassDB::register_class<YHBoxContainer>();
	ClassDB::register_class<YVBoxContainer>();
	ClassDB::register_class<YTweenWrap>();
	ClassDB::register_class<YTweenJiggle>();
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
	ClassDB::register_class<YNavHelper2D>();
	ClassDB::register_class<YNavHelper3D>();
	ClassDB::register_class<CircleWithConeTexture2D>();
 	ClassDB::register_class<AOBakeableMeshInstance>();
 	ClassDB::register_class<YThreader>();
 	ClassDB::register_class<YThreaderInterrupter>();
 	ClassDB::register_class<YSceneSpawner3D>();
 	ClassDB::register_class<YBoneAttachment3D>();
	ClassDB::register_class<YSaveable3D>();
	ClassDB::register_class<CombinedTexture2D>();
	ClassDB::register_class<YPersistentID>();
	ClassDB::register_class<IntersectResult>();
	ClassDB::register_class<YBoxContainer3D>();
	ClassDB::register_class<YVBoxContainer3D>();
	ClassDB::register_class<YHBoxContainer3D>();

	// Initialize your singleton.

	yarn_save_ref.instantiate();

	yarn_time_ref.instantiate();

	yarn_physics_ref.instantiate();

	yarn_game_log_ref.instantiate();

	yarn_tween_ref.instantiate();

	// Bind your singleton.
	Engine::get_singleton()->add_singleton(Engine::Singleton("YEngine", memnew(YEngine)));
	YEngine::get_singleton()->add_setting("application/config/window_name", "", Variant::Type::STRING);
	YEngine::get_singleton()->add_setting("yengine/cached/next_persistent_id", 0, Variant::Type::INT);
	Engine::get_singleton()->add_singleton(Engine::Singleton("YSave", YSave::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("YTime", YTime::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("YPhysics", YPhysics::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("YGameState", memnew(YGameState)));
	Engine::get_singleton()->add_singleton(Engine::Singleton("YGameLog", YGameLog::get_singleton()));
	Engine::get_singleton()->add_singleton(Engine::Singleton("YTween", YTween::get_singleton()));
}

void uninitialize_yarnengine_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	// Remove singletons first
	if (Engine::get_singleton()->has_singleton("YEngine")) {
		YEngine* engine = YEngine::get_singleton();
		Engine::get_singleton()->remove_singleton("YEngine");
		if (engine != nullptr) {
			memdelete(engine);
		}
		if (Engine::get_singleton()->has_singleton("YGameState")) {
			YGameState* ygamestate = YGameState::get_singleton();
			Engine::get_singleton()->remove_singleton("YGameState");
			if (ygamestate != nullptr) {
				memdelete(ygamestate);
			}
		}
		
		// Remove other singletons...
		Engine::get_singleton()->remove_singleton("YSave");
		Engine::get_singleton()->remove_singleton("YTime");
		Engine::get_singleton()->remove_singleton("YPhysics");
		Engine::get_singleton()->remove_singleton("YGameLog");
		Engine::get_singleton()->remove_singleton("YTween");
	}

	// Cleanup refs
	if (yarn_time_ref.is_valid()) {
		yarn_time_ref = nullptr;
	}	
	if (yarn_save_ref.is_valid()) {
		yarn_save_ref = nullptr;
	}
	if (yarn_physics_ref.is_valid()) {
		yarn_physics_ref = nullptr;
	}
	if (yarn_game_log_ref.is_valid()) {
		yarn_game_log_ref = nullptr;
	}
	if (yarn_tween_ref.is_valid()) {
		yarn_tween_ref = nullptr;
	}
}