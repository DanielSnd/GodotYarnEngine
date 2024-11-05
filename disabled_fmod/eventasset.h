//
// Created by Daniel on 2024-08-07.
//

#ifndef EVENTASSET_H
#define EVENTASSET_H
#ifdef NO_FMOD
#include "core/io/resource.h"
#include "scene/main/node.h"
#include "core/object/ref_counted.h"
#include "core/io/dir_access.h"

class FMODAsset : public Resource
{
    GDCLASS(FMODAsset, Resource);

protected:
    static void _bind_methods();

public:
    void set_guid(const String& guid);
    String get_guid() const;
    String get_fmod_path();

    void set_fmod_path(const String &_path);

private:
    String fmod_path{};
    String guid{};
};

class FMODGuidDictionary : public Resource
{
    GDCLASS(FMODGuidDictionary, Resource);

protected:
    static void _bind_methods();

public:
    Dictionary get_guid_dictionary() const;
    void set_guid_dictionary(const Dictionary &p_guid_dictionary);
    Dictionary guid_dictionary;

};


class BusAsset : public FMODAsset
{
    GDCLASS(BusAsset, FMODAsset);

protected:
    static void _bind_methods();

public:
    void set_bus_ref( Dictionary bus);
};

class VCAAsset : public FMODAsset
{
    GDCLASS(VCAAsset, FMODAsset);

protected:
    static void _bind_methods();

public:
    void set_vca_ref( Dictionary vca);
};

class ParameterAsset : public FMODAsset
{
    GDCLASS(ParameterAsset, FMODAsset);

protected:
    static void _bind_methods();

private:
    Array labels;

public:
    void set_parameter_ref(const Dictionary parameter_description);

    void set_parameter_description(const Dictionary parameter_description);
    Dictionary get_parameter_description() const;

    void set_labels(const Array& labels);
    Array get_labels() const;
};

class BankAsset : public FMODAsset
{
    GDCLASS(BankAsset, FMODAsset);

protected:
    static void _bind_methods();

public:
    void set_bank_file_info(const Dictionary& file_bank_info);
    void set_bank_ref(Dictionary bank);
    void set_file_path(const String& file_path);
    String get_file_path() const;
    void set_modified_time(int modified_time);
    int get_modified_time() const;

private:
    String file_path{};
    int modified_time{};
};

class EventAsset : public FMODAsset {
    GDCLASS(EventAsset, FMODAsset);

protected:
    static void _bind_methods();

public:
    void set_event_ref(Dictionary event_description);
    Dictionary get_event_description() const;

    void set_event_ref_from_description_ref(Dictionary event_description);

    void play_one_shot_attached(Node *node);
    void stop_looped(Node *node);
    void play_looped_attached(Node *node);

    void play_looped(Node *node, const Variant &position);

    void play_one_shot(const Variant &position);

    void play_one_shot_volume(float volume, const Variant &position);

    void set_3d(bool is_3d);
    bool get_3d() const;

    void set_oneshot(bool is_oneshot);
    bool get_oneshot() const;

    void set_is_snapshot(bool is_snapshot);
    bool get_is_snapshot() const;

    void set_min_distance(float min_distance);
    float get_min_distance() const;

    void set_max_distance(float max_distance);
    float get_max_distance() const;

    void set_parameters(const Dictionary& parameters);
    Dictionary get_parameters() const;

    bool has_parameter(const String& parameter_guid) const;

private:
    bool is_3d{};
    bool is_oneshot{};
    bool is_snapshot{};
    float min_distance{};
    float max_distance{};
    Dictionary parameters;
};

namespace FmodTypes {
    class FMOD_STUDIO_PARAMETER_ID : public Resource
    {
        GDCLASS(FMOD_STUDIO_PARAMETER_ID, Resource);

    protected:
        static void _bind_methods();

    private:
        unsigned int data1{};
        unsigned int data2{};

    public:
        void set_data1(const unsigned int& data1);
        unsigned int get_data1() const;
        void set_data2(const unsigned int& data2);
        unsigned int get_data2() const;
    };

    class FMOD_STUDIO_PARAMETER_DESCRIPTION : public Resource
    {
        GDCLASS(FMOD_STUDIO_PARAMETER_DESCRIPTION, Resource);

    protected:
        static void _bind_methods();

    private:
        String name{};
        Ref<FMOD_STUDIO_PARAMETER_ID> id;
        float minimum{};
        float maximum{};
        float defaultvalue{};
        int type{};
        int flags{};
        String guid{};

    public:
        void set_id(const Ref<FMOD_STUDIO_PARAMETER_ID>& id);
        Ref<FMOD_STUDIO_PARAMETER_ID> get_id() const;
        void set_minimum(float minimum);
        float get_minimum() const;
        void set_maximum(float maximum);
        float get_maximum() const;
        void set_default_value(float defaultValue);
        float get_default_value() const;
        void set_type(int type);
        int get_type() const;
        void set_flags(int flags);
        int get_flags() const;
        void set_guid(const String& guid);
        String get_guid() const;
    };
}
class ProjectCache : public Resource
{
    GDCLASS(ProjectCache, Resource);

protected:
    static void _bind_methods();

private:
    Array bank_tree;
    Array event_tree;
    Array snapshot_tree;
    Array bus_tree;
    Array vca_tree;
    Array parameter_tree;

public:
    Dictionary banks;
    Dictionary events;
    Dictionary snapshots;
    Dictionary busses;
    Dictionary vcas;
    Dictionary parameters;
    Dictionary guid_to_path;

    void initialize_cache(const Dictionary& p_project_info);

    void set_banks(const Dictionary& p_banks) {
        banks = p_banks;
    }
    Dictionary get_banks() const
    { return banks; }
    void set_events(const Dictionary& p_events) {
        events = p_events;
    }
    Dictionary get_events() const {
        return events;
    }
    void set_snapshots(const Dictionary& p_snapshots) {
        snapshots = p_snapshots;
    }
    Dictionary get_snapshots() const {
        return snapshots;
    }
    void set_busses(const Dictionary& p_busses) {
        busses = p_busses;
    }
    Dictionary get_busses() const {
        return busses;
    }
    void set_vcas(const Dictionary& p_vcas) {
        vcas = p_vcas;
    }
    Dictionary get_vcas() const {
        return vcas;
    }
    void set_parameters(const Dictionary& p_parameters) {
        parameters = p_parameters;
    }
    Dictionary get_parameters() const {
        return parameters;
    }
    void set_guid_to_path(const Dictionary& p_guid_to_path) {
        guid_to_path = p_guid_to_path;
    }
    Dictionary get_guid_to_path() const {
        return guid_to_path;
    }
    void delete_invalid_event_assets() {

    }
    void set_bank_tree(const Array& p_bank_tree) {
        bank_tree = p_bank_tree;
    }
    Array get_bank_tree() {
        return bank_tree;
    }
    void set_event_tree(const Array& p_event_tree) {
        event_tree = p_event_tree;
    }
    Array get_event_tree() {
        return event_tree;
    }
    void set_snapshot_tree(const Array& p_snapshot_tree) {
        snapshot_tree = p_snapshot_tree;
    }
    Array get_snapshot_tree() {
        return snapshot_tree;
    }
    void set_bus_tree(const Array& p_bus_tree) {
        bus_tree = p_bus_tree;
    }
    Array get_bus_tree() {
        return bus_tree;
    }
    void set_vca_tree(const Array& p_vca_tree) {
        vca_tree = p_vca_tree;
    }
    Array get_vca_tree() {
        return vca_tree;
    }
    void set_parameter_tree(const Array& p_parameter_tree) {
        parameter_tree = p_parameter_tree;
    }
    Array get_parameter_tree() {
        return parameter_tree;
    }
};
#endif

#endif //EVENTASSET_H
