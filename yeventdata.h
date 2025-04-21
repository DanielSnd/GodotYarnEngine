#ifndef YEVENTDATA_H
#define YEVENTDATA_H

#include "core/object/ref_counted.h"
#include "core/templates/hash_map.h"
#include "core/variant/variant.h"
#include "scene/main/node.h"

class YEventData : public RefCounted {
    GDCLASS(YEventData, RefCounted);

private:
    struct RegEventCallbackInstance {
        ObjectID node_inst_id;
        Callable callable;
        int priority;
        RegEventCallbackInstance() : node_inst_id(), callable(), priority(50) {}
        RegEventCallbackInstance(ObjectID node_inst_id, const Callable &p_callable, int p_priority) 
            : node_inst_id(node_inst_id), callable(p_callable), priority(p_priority) {}
    };

    struct RegEventCallback {
        int event_id;
        Vector<RegEventCallbackInstance> callbacks;
        RegEventCallback() : event_id(0) {}
        explicit RegEventCallback(int p_event_id) : event_id(p_event_id) {}
    };

    static HashMap<int, RegEventCallback> reg_event_callbacks;
    static HashMap<ObjectID, int> count_node_callbacks;
    static HashMap<int, String> parameter_names;
    static HashMap<int, String> event_id_names;
    static void set_parameter_name_dictionary(const Dictionary &p_dict);
    static void set_event_id_name_dictionary(const Dictionary &p_dict);

    HashMap<int, Variant> data;
    int event_type;

    static void _node_exiting_tree(Node *p_node);
    static void _clean_invalid_callables(int p_event_id);
    static void _sort_event_callbacks(int p_event_id);
    struct YEventDataPriorityCompare {
		_FORCE_INLINE_ bool operator()(const RegEventCallbackInstance& a, const RegEventCallbackInstance& b) const {
			return a.priority < b.priority;
		}
	};
protected:
    static void _bind_methods();

public:
    YEventData();

    Ref<YEventData> set_event_type(int p_type);
    int get_event_type() const;

    bool has_value(int p_identifier) const;
    Variant get_value(int p_identifier, const Variant &p_default = Variant()) const;
    Ref<YEventData> set_value(int p_identifier, const Variant &p_value);
    Ref<YEventData> increment_value(int p_identifier, const Variant &p_amount);

    bool is_event_type(int p_type) const;
    Dictionary get_data_as_dictionary() const;
    Ref<YEventData> set_data_from_dictionary(const Dictionary &p_dict);
    Ref<YEventData> add_data_from_dictionary(const Dictionary &p_dict);

    Ref<YEventData> emit() const;
    Ref<YEventData> emit_to_node(Node *p_node) const;

    static void register_listener(int p_event_id, const Callable &p_callable, int p_priority = 50);
    static void unregister_listener(int p_event_id, const Callable &p_callable);
    static void register_listener_with_node(Node *p_node, int p_event_id, const Callable &p_callable, int p_priority = 50);
    static void unregister_listener_with_node(Node *p_node, int p_event_id, const Callable &p_callable);
    static void clear_node_callbacks(Node *p_node);
    static Ref<YEventData> create(int with_type, int parameter_one, Variant value_one);

	virtual String to_string() override;

    Ref<YEventData> duplicate() const;
};

#endif // YEVENTDATA_H
