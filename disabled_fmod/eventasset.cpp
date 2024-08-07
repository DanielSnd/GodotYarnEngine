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

void EventAsset::set_guid(const String& p_guid) {
	guid = p_guid;
}

String EventAsset::get_guid() const {
	return guid;
}
String EventAsset::get_fmod_path() {
	return fmod_path;
}

void EventAsset::set_fmod_path(const String &_path) {
	fmod_path = _path;
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

#endif
