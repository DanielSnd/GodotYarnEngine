//
// Created by Daniel on 2024-08-07.
//

#ifndef EVENTASSET_H
#define EVENTASSET_H
#include "core/io/resource.h"

class EventAsset : public Resource {
    GDCLASS(EventAsset, Resource);

protected:
    static void _bind_methods();

public:
    void set_event_ref(Variant event_description);
    Variant get_event_description() const;

    void set_event_ref_from_description_ref(Variant event_description);

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

public:
    void set_guid(const String& guid);
    String get_guid() const;
    String get_fmod_path();

    void set_fmod_path(const String &_path);

private:
    String fmod_path{};
    String guid{};
    bool is_3d{};
    bool is_oneshot{};
    bool is_snapshot{};
    float min_distance{};
    float max_distance{};
    Dictionary parameters;
};



#endif //EVENTASSET_H
