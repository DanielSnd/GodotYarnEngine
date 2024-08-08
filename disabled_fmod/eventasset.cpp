//
// Created by Daniel on 2024-08-07.
//

#ifdef NO_FMOD
#include "eventasset.h"

void EventAsset::set_event_ref(Dictionary event_description) {

}

Dictionary EventAsset::get_event_description() const {
	return Dictionary{};
}

void EventAsset::set_event_ref_from_description_ref(Dictionary event_description) {

}


void EventAsset::play_one_shot_attached(Node *node) {
}

void EventAsset::stop_looped(Node *node) {
}


void EventAsset::play_looped_attached(Node *node) {

}

void EventAsset::play_looped(Node *node, const Variant &position = Variant()) {
}

void EventAsset::play_one_shot(const Variant &position) {
}

void EventAsset::play_one_shot_volume(float volume,const Variant &position) {
}

void EventAsset::set_3d(bool _is_3d)
{
	this->is_3d = _is_3d;
}

bool EventAsset::get_3d() const
{
	return is_3d;
}

void EventAsset::set_oneshot(bool _is_oneshot)
{
	this->is_oneshot = _is_oneshot;
}

bool EventAsset::get_oneshot() const
{
	return is_oneshot;
}

void EventAsset::set_is_snapshot(bool _is_snapshot)
{
	this->is_snapshot = _is_snapshot;
}

bool EventAsset::get_is_snapshot() const
{
	return is_snapshot;
}

void EventAsset::set_min_distance(float _min_distance)
{
	this->min_distance = _min_distance;
}

float EventAsset::get_min_distance() const
{
	return min_distance;
}

void EventAsset::set_max_distance(float _max_distance)
{
	this->max_distance = _max_distance;
}

float EventAsset::get_max_distance() const
{
	return max_distance;
}

void EventAsset::set_parameters(const Dictionary& _parameters)
{
	this->parameters = _parameters;
}

Dictionary EventAsset::get_parameters() const
{
	return parameters;
}

bool EventAsset::has_parameter(const String& parameter_guid) const
{
	return parameters.has(parameter_guid);
}

void FMODAsset::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_guid", "guid"), &FMODAsset::set_guid);
	ClassDB::bind_method(D_METHOD("get_guid"), &FMODAsset::get_guid);

	ClassDB::bind_method(D_METHOD("set_fmod_path", "path"), &FMODAsset::set_fmod_path);
	ClassDB::bind_method(D_METHOD("get_fmod_path"), &FMODAsset::get_fmod_path);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "fmod_path", PropertyHint::PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),
			"set_fmod_path", "get_fmod_path");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "guid", PropertyHint::PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),

			"set_guid", "get_guid");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "name", PropertyHint::PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),
			"set_name", "get_name");
}

void FMODAsset::set_guid(const String& p_guid) {
	guid = p_guid;
}

String FMODAsset::get_guid() const {
	return guid;
}
String FMODAsset::get_fmod_path() {
	return fmod_path;
}

void FMODAsset::set_fmod_path(const String &_path) {
	fmod_path = _path;
}

void FMODGuidDictionary::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_guid_dictionary", "guid_dictionary"), &FMODGuidDictionary::set_guid_dictionary);
	ClassDB::bind_method(D_METHOD("get_guid_dictionary"), &FMODGuidDictionary::get_guid_dictionary);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "guid_dictionary", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),
			"set_guid_dictionary", "get_guid_dictionary");
}

Dictionary FMODGuidDictionary::get_guid_dictionary() const {
	return guid_dictionary;
}

void FMODGuidDictionary::set_guid_dictionary(const Dictionary &p_guid_dictionary) {
	guid_dictionary = p_guid_dictionary;
}

void BusAsset::_bind_methods() {
}

void BusAsset::set_bus_ref(const Dictionary bus) {
}

void VCAAsset::_bind_methods() {
}

void VCAAsset::set_vca_ref(const Dictionary vca) {
}

void ParameterAsset::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_parameter_description", "parameter_description"),
		&ParameterAsset::set_parameter_description);
	ClassDB::bind_method(D_METHOD("get_parameter_description"), &ParameterAsset::get_parameter_description);
	ClassDB::bind_method(D_METHOD("set_labels", "labels"), &ParameterAsset::set_labels);
	ClassDB::bind_method(D_METHOD("get_labels"), &ParameterAsset::get_labels);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "parameter_description", PropertyHint::PROPERTY_HINT_RESOURCE_TYPE,
						 "FMOD_STUDIO_PARAMETER_DESCRIPTION", PROPERTY_USAGE_DEFAULT),
			"set_parameter_description", "get_parameter_description");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "labels"), "set_labels", "get_labels");
}

void ParameterAsset::set_parameter_ref(const Dictionary parameter_description) {

}

void ParameterAsset::set_parameter_description(const Dictionary parameter_description) {

}

Dictionary ParameterAsset::get_parameter_description() const {
	return Dictionary{};
}

void ParameterAsset::set_labels(const Array &p_labels) {
	labels = p_labels;
}

Array ParameterAsset::get_labels() const {
	return labels;
}

void BankAsset::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_file_path", "file_path"), &BankAsset::set_file_path);
	ClassDB::bind_method(D_METHOD("get_file_path"), &BankAsset::get_file_path);
	ClassDB::bind_method(D_METHOD("set_modified_time", "modified_time"), &BankAsset::set_modified_time);
	ClassDB::bind_method(D_METHOD("get_modified_time"), &BankAsset::get_modified_time);

	ADD_PROPERTY(
			PropertyInfo(Variant::STRING, "file_path", PropertyHint::PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),
			"set_file_path", "get_file_path");

	ADD_PROPERTY(
			PropertyInfo(Variant::STRING, "modified_time", PropertyHint::PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),
			"set_modified_time", "get_modified_time");
}

void BankAsset::set_bank_file_info(const Dictionary &file_bank_info) {

}

void BankAsset::set_bank_ref(const Dictionary bank) {

}

void BankAsset::set_file_path(const String &p_file_path) {
	file_path = p_file_path;
}

String BankAsset::get_file_path() const {
	return file_path;
}

void BankAsset::set_modified_time(int p_modified_time) {
	modified_time = p_modified_time;
}

int BankAsset::get_modified_time() const {
	return modified_time;
}

void EventAsset::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_3d", "is_3d"), &EventAsset::set_3d);
	ClassDB::bind_method(D_METHOD("get_3d"), &EventAsset::get_3d);
	ClassDB::bind_method(D_METHOD("set_oneshot", "is_oneshot"), &EventAsset::set_oneshot);
	ClassDB::bind_method(D_METHOD("get_oneshot"), &EventAsset::get_oneshot);
	ClassDB::bind_method(D_METHOD("set_is_snapshot", "is_snapshot"), &EventAsset::set_is_snapshot);
	ClassDB::bind_method(D_METHOD("get_is_snapshot"), &EventAsset::get_is_snapshot);
	ClassDB::bind_method(D_METHOD("set_min_distance", "min_distance"), &EventAsset::set_min_distance);
	ClassDB::bind_method(D_METHOD("get_min_distance"), &EventAsset::get_min_distance);
	ClassDB::bind_method(D_METHOD("set_max_distance", "max_distance"), &EventAsset::set_max_distance);
	ClassDB::bind_method(D_METHOD("get_max_distance"), &EventAsset::get_max_distance);
	ClassDB::bind_method(D_METHOD("set_parameters", "parameters"), &EventAsset::set_parameters);
	ClassDB::bind_method(D_METHOD("get_parameters"), &EventAsset::get_parameters);

	ClassDB::bind_method(D_METHOD("play_looped_attached","node"), &EventAsset::play_looped_attached);
	ClassDB::bind_method(D_METHOD("play_looped","node","position"), &EventAsset::play_looped);
	ClassDB::bind_method(D_METHOD("stop_looped","node"), &EventAsset::stop_looped);

	ClassDB::bind_method(D_METHOD("play_one_shot","position"), &EventAsset::play_one_shot,DEFVAL(Variant{}));
	ClassDB::bind_method(D_METHOD("play_one_shot_volume","volume","position"), &EventAsset::play_one_shot_volume,DEFVAL(Variant{}));
	ClassDB::bind_method(D_METHOD("play_one_shot_attached","node"), &EventAsset::play_one_shot_attached);
	ClassDB::bind_method(D_METHOD("set_event_ref_from_description", "description"),
			&EventAsset::set_event_ref_from_description_ref);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_3d", PropertyHint::PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),
			"set_3d", "get_3d");
	ADD_PROPERTY(
			PropertyInfo(Variant::BOOL, "is_oneshot", PropertyHint::PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),
			"set_oneshot", "get_oneshot");
	ADD_PROPERTY(
			PropertyInfo(Variant::BOOL, "is_snapshot", PropertyHint::PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),
			"set_is_snapshot", "get_is_snapshot");
	ADD_PROPERTY(
			PropertyInfo(Variant::FLOAT, "min_distance", PropertyHint::PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),
			"set_min_distance", "get_min_distance");
	ADD_PROPERTY(
			PropertyInfo(Variant::FLOAT, "max_distance", PropertyHint::PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),
			"set_max_distance", "get_max_distance");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "parameters", PropertyHint::PROPERTY_HINT_ARRAY_TYPE,
						 "24/17:ParameterAsset", PROPERTY_USAGE_DEFAULT),
			"set_parameters", "get_parameters");
}

void ProjectCache::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_banks", "banks"), &ProjectCache::set_banks);
	ClassDB::bind_method(D_METHOD("get_banks"), &ProjectCache::get_banks);
	ClassDB::bind_method(D_METHOD("set_events", "events"), &ProjectCache::set_events);
	ClassDB::bind_method(D_METHOD("get_events"), &ProjectCache::get_events);
	ClassDB::bind_method(D_METHOD("set_snapshots", "snapshots"), &ProjectCache::set_snapshots);
	ClassDB::bind_method(D_METHOD("get_snapshots"), &ProjectCache::get_snapshots);
	ClassDB::bind_method(D_METHOD("set_busses", "busses"), &ProjectCache::set_busses);
	ClassDB::bind_method(D_METHOD("get_busses"), &ProjectCache::get_busses);
	ClassDB::bind_method(D_METHOD("set_vcas", "vcas"), &ProjectCache::set_vcas);
	ClassDB::bind_method(D_METHOD("get_vcas"), &ProjectCache::get_vcas);
	ClassDB::bind_method(D_METHOD("set_parameters", "parameters"), &ProjectCache::set_parameters);
	ClassDB::bind_method(D_METHOD("get_parameters"), &ProjectCache::get_parameters);
	ClassDB::bind_method(D_METHOD("set_guid_to_path", "guid_to_path"), &ProjectCache::set_guid_to_path);
	ClassDB::bind_method(D_METHOD("get_guid_to_path"), &ProjectCache::get_guid_to_path);
	ClassDB::bind_method(D_METHOD("set_bank_tree", "bank_tree"), &ProjectCache::set_bank_tree);
	ClassDB::bind_method(D_METHOD("get_bank_tree"), &ProjectCache::get_bank_tree);
	ClassDB::bind_method(D_METHOD("set_event_tree", "event_tree"), &ProjectCache::set_event_tree);
	ClassDB::bind_method(D_METHOD("get_event_tree"), &ProjectCache::get_event_tree);
	ClassDB::bind_method(D_METHOD("set_snapshot_tree", "snapshot_tree"), &ProjectCache::set_snapshot_tree);
	ClassDB::bind_method(D_METHOD("get_snapshot_tree"), &ProjectCache::get_snapshot_tree);
	ClassDB::bind_method(D_METHOD("set_bus_tree", "bus_tree"), &ProjectCache::set_bus_tree);
	ClassDB::bind_method(D_METHOD("get_bus_tree"), &ProjectCache::get_bus_tree);
	ClassDB::bind_method(D_METHOD("set_vca_tree", "vca_tree"), &ProjectCache::set_vca_tree);
	ClassDB::bind_method(D_METHOD("get_vca_tree"), &ProjectCache::get_vca_tree);
	ClassDB::bind_method(D_METHOD("set_parameter_tree", "parameter_tree"), &ProjectCache::set_parameter_tree);
	ClassDB::bind_method(D_METHOD("get_parameter_tree"), &ProjectCache::get_parameter_tree);

	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "banks", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),
			"set_banks", "get_banks");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "events", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),
			"set_events", "get_events");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "snapshots", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),
			"set_snapshots", "get_snapshots");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "vcas", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT), "set_vcas",
			"get_vcas");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "busses", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),
			"set_busses", "get_busses");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "parameters", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),
	"set_parameters", "get_parameters");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "guid_to_path", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT),
			"set_guid_to_path", "get_guid_to_path");

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "bank_tree", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE),
			"set_bank_tree", "get_bank_tree");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "event_tree", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE),
			"set_event_tree", "get_event_tree");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "snapshot_tree", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE),
			"set_snapshot_tree", "get_snapshot_tree");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "vca_tree", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE),
			"set_vca_tree", "get_vca_tree");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "bus_tree", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE),
			"set_bus_tree", "get_bus_tree");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "parameter_tree", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE),
			"set_parameter_tree", "get_parameter_tree");
}

void ProjectCache::initialize_cache(const Dictionary& project_info) {

	set_banks(project_info["banks"]);
	set_events(project_info["events"]);
	set_snapshots(project_info["snapshots"]);
	set_vcas(project_info["vcas"]);
	set_busses(project_info["busses"]);
	set_parameters(project_info["parameters"]);
	set_guid_to_path(project_info["guid_to_path"]);

	const String cache_dir = "res://FMOD/cache";
	Ref<DirAccess> dir = DirAccess::open(cache_dir);
	if (dir.is_null())
	{
		DirAccess::make_dir_absolute(cache_dir);
		dir = DirAccess::open(cache_dir);
	}

	const String cache_path = "res://FMOD/cache/fmod_project_cache.tres";

	set_path(cache_path, true);
	ResourceSaver::save(this, cache_path);
	print_line(vformat("[FMOD] Cache created in %s",cache_path));
	emit_changed();

	const String resources_path = "res://FMOD/editor/resources";
	Dictionary resource_dirs;
	resource_dirs["events"] = "res://FMOD/editor/resources/events/";
	resource_dirs["snapshots"] = "res://FMOD/editor/resources/snapshots/";
	resource_dirs["busses"] = "res://FMOD/editor/resources/busses/";
	resource_dirs["vcas"] = "res://FMOD/editor/resources/vcas/";
	resource_dirs["parameters"] = "res://FMOD/editor/resources/parameters/";
	Array all_fmod_paths;
	all_fmod_paths.append_array(get_events().keys());
	all_fmod_paths.append_array(get_snapshots().keys());
	all_fmod_paths.append_array(get_busses().keys());
	all_fmod_paths.append_array(get_vcas().keys());
	all_fmod_paths.append_array(get_parameters().keys());
	// print_line(vformat("All fmod paths %s",all_fmod_paths));
	int len = static_cast<int>(resource_dirs.values().size());
	for (int i = 0; i < len; ++i) {
		const String& dir_path = resource_dirs.values()[i];
		Ref<DirAccess> new_dir = DirAccess::open(dir_path);
		if (!new_dir.is_null()) {
			new_dir->list_dir_begin();
			String file_name = new_dir->get_next();
			while (!file_name.is_empty()) {
				//print_line(vformat("filename [%s]. Load path [%s]",file_name,vformat("%s/%s",dir_path,file_name)));
				if (file_name.ends_with(".tres") && (!all_fmod_paths.has(file_name.replace(".tres","")))) {
					Ref<FMODAsset> loaded_asset = ResourceLoader::load(vformat("%s/%s",dir_path,file_name));
					if (loaded_asset.is_valid()) {
						print_line(vformat("[FMOD] FMOD Asset [%s] path [%s] not found in fmod bank",file_name,loaded_asset->get_fmod_path()));
					}
					else {
						print_line(vformat("[FMOD] FMOD Asset [%s] not found in fmod bank",file_name));
					}
				}
				file_name = new_dir->get_next();
			}
		}
	}
}
#endif
