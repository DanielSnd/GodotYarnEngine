//
// Created by Daniel on 2024-06-14.
//

#include "ybone_attachment_3d.h"


void YBoneAttachment3D::_validate_property(PropertyInfo &p_property) const {
	if (p_property.name == "bone_name") {
		// Because it is a constant function, we cannot use the _get_skeleton_3d function.
		const Skeleton3D *parent = nullptr;
		if (use_external_skeleton) {
			if (external_skeleton_node_cache.is_valid()) {
				parent = Object::cast_to<Skeleton3D>(ObjectDB::get_instance(external_skeleton_node_cache));
			}
		} else {
			parent = Object::cast_to<Skeleton3D>(get_parent());
		}

		if (parent) {
			String names;
			for (int i = 0; i < parent->get_bone_count(); i++) {
				if (i > 0) {
					names += ",";
				}
				names += parent->get_bone_name(i);
			}

			p_property.hint = PROPERTY_HINT_ENUM;
			p_property.hint_string = names;
		} else {
			p_property.hint = PROPERTY_HINT_NONE;
			p_property.hint_string = "";
		}
	}
}

bool YBoneAttachment3D::_set(const StringName &p_path, const Variant &p_value) {
	if (p_path == SNAME("use_external_skeleton")) {
		set_use_external_skeleton(p_value);
	} else if (p_path == SNAME("external_skeleton")) {
		set_external_skeleton(p_value);
	}

	return true;
}

bool YBoneAttachment3D::_get(const StringName &p_path, Variant &r_ret) const {
	if (p_path == SNAME("use_external_skeleton")) {
		r_ret = get_use_external_skeleton();
	} else if (p_path == SNAME("external_skeleton")) {
		r_ret = get_external_skeleton();
	}

	return true;
}

void YBoneAttachment3D::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::BOOL, "use_external_skeleton", PROPERTY_HINT_NONE, ""));
	if (use_external_skeleton) {
		p_list->push_back(PropertyInfo(Variant::NODE_PATH, "external_skeleton", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Skeleton3D"));
	}
}

PackedStringArray YBoneAttachment3D::get_configuration_warnings() const {
	PackedStringArray warnings = Node3D::get_configuration_warnings();

	if (use_external_skeleton) {
		if (external_skeleton_node_cache.is_null()) {
			warnings.push_back(RTR("External Skeleton3D node not set! Please set a path to an external Skeleton3D node."));
		}
	} else {
		Skeleton3D *parent = Object::cast_to<Skeleton3D>(get_parent());
		if (!parent) {
			warnings.push_back(RTR("Parent node is not a Skeleton3D node! Please use an external Skeleton3D if you intend to use the YBoneAttachment3D without it being a child of a Skeleton3D node."));
		}
	}

	if (bone_idx == -1) {
		warnings.push_back(RTR("YBoneAttachment3D node is not bound to any bones! Please select a bone to attach this node."));
	}

	return warnings;
}

void YBoneAttachment3D::_update_external_skeleton_cache() {
	external_skeleton_node_cache = ObjectID();
	if (has_node(external_skeleton_node)) {
		Node *node = get_node(external_skeleton_node);
		ERR_FAIL_NULL_MSG(node, "Cannot update external skeleton cache: Node cannot be found!");

		// Make sure it's a skeleton3D
		Skeleton3D *sk = Object::cast_to<Skeleton3D>(node);
		ERR_FAIL_NULL_MSG(sk, "Cannot update external skeleton cache: Skeleton3D Nodepath does not point to a Skeleton3D node!");

		external_skeleton_node_cache = node->get_instance_id();
	} else {
		if (external_skeleton_node.is_empty()) {
			YBoneAttachment3D *parent_attachment = Object::cast_to<YBoneAttachment3D>(get_parent());
			if (parent_attachment) {
				parent_attachment->_update_external_skeleton_cache();
				if (parent_attachment->has_node(parent_attachment->external_skeleton_node)) {
					Node *node = parent_attachment->get_node(parent_attachment->external_skeleton_node);
					ERR_FAIL_NULL_MSG(node, "Cannot update external skeleton cache: Parent's Skeleton3D node cannot be found!");

					// Make sure it's a skeleton3D
					Skeleton3D *sk = Object::cast_to<Skeleton3D>(node);
					ERR_FAIL_NULL_MSG(sk, "Cannot update external skeleton cache: Parent Skeleton3D Nodepath does not point to a Skeleton3D node!");

					external_skeleton_node_cache = node->get_instance_id();
					external_skeleton_node = get_path_to(node);
				}
			}
		}
	}
}

void YBoneAttachment3D::_check_bind() {
	Skeleton3D *sk = _get_skeleton3d();

	if (sk && !bound) {
		if (bone_idx <= -1) {
			bone_idx = sk->find_bone(bone_name);
		}
		if (bone_idx != -1) {
			sk->connect(SNAME("skeleton_updated"), callable_mp(this, &YBoneAttachment3D::on_skeleton_update));
			bound = true;
			callable_mp(this, &YBoneAttachment3D::on_skeleton_update);
		}
	}
}

Skeleton3D *YBoneAttachment3D::_get_skeleton3d() {
	if (use_external_skeleton) {
		if (external_skeleton_node_cache.is_valid()) {
			return Object::cast_to<Skeleton3D>(ObjectDB::get_instance(external_skeleton_node_cache));
		} else {
			_update_external_skeleton_cache();
			if (external_skeleton_node_cache.is_valid()) {
				return Object::cast_to<Skeleton3D>(ObjectDB::get_instance(external_skeleton_node_cache));
			}
		}
	} else {
		return Object::cast_to<Skeleton3D>(get_parent());
	}
	return nullptr;
}

void YBoneAttachment3D::_check_unbind() {
	if (bound) {
		Skeleton3D *sk = _get_skeleton3d();

		if (sk) {
			sk->disconnect(SNAME("skeleton_updated"), callable_mp(this, &YBoneAttachment3D::on_skeleton_update));
		}
		bound = false;
	}
}

void YBoneAttachment3D::_transform_changed() {
	if (!is_inside_tree()) {
		return;
	}

	if (override_pose && !overriding) {
		Skeleton3D *sk = _get_skeleton3d();

		ERR_FAIL_NULL_MSG(sk, "Cannot override pose: Skeleton not found!");
		ERR_FAIL_INDEX_MSG(bone_idx, sk->get_bone_count(), "Cannot override pose: Bone index is out of range!");

		Transform3D our_trans = get_transform();
		if (use_external_skeleton) {
			our_trans = sk->get_global_transform().affine_inverse() * get_global_transform();
		}

		overriding = true;
		sk->set_bone_global_pose(bone_idx, our_trans);
		sk->force_update_all_dirty_bones();
	}
	overriding = false;
}

void YBoneAttachment3D::set_bone_name(const String &p_name) {
	bone_name = p_name;
	Skeleton3D *sk = _get_skeleton3d();
	if (sk) {
		set_bone_idx(sk->find_bone(bone_name));
	}
}

String YBoneAttachment3D::get_bone_name() const {
	return bone_name;
}

void YBoneAttachment3D::set_bone_idx(const int &p_idx) {
	if (is_inside_tree()) {
		_check_unbind();
	}

	bone_idx = p_idx;

	Skeleton3D *sk = _get_skeleton3d();
	if (sk) {
		if (bone_idx <= -1 || bone_idx >= sk->get_bone_count()) {
			WARN_PRINT("Bone index out of range! Cannot connect BoneAttachment to node!");
			bone_idx = -1;
		} else {
			bone_name = sk->get_bone_name(bone_idx);
		}
	}

	if (is_inside_tree()) {
		_check_bind();
	}

	notify_property_list_changed();
}

int YBoneAttachment3D::get_bone_idx() const {
	return bone_idx;
}

void YBoneAttachment3D::set_override_pose(bool p_override) {
	override_pose = p_override;
	set_notify_transform(override_pose);
	set_process_internal(override_pose);
	notify_property_list_changed();
}

bool YBoneAttachment3D::get_override_pose() const {
	return override_pose;
}

void YBoneAttachment3D::set_ignore_scale(bool p_ignore_scale) {
	ignore_scale = p_ignore_scale;
}

bool YBoneAttachment3D::get_ignore_scale() const {
	return ignore_scale;
}

void YBoneAttachment3D::set_use_external_skeleton(bool p_use_external) {
	use_external_skeleton = p_use_external;

	if (use_external_skeleton) {
		_check_unbind();
		_update_external_skeleton_cache();
		_check_bind();
		_transform_changed();
	}

	notify_property_list_changed();
}

bool YBoneAttachment3D::get_use_external_skeleton() const {
	return use_external_skeleton;
}

void YBoneAttachment3D::set_external_skeleton(NodePath p_path) {
	external_skeleton_node = p_path;
	_update_external_skeleton_cache();
	notify_property_list_changed();
}

NodePath YBoneAttachment3D::get_external_skeleton() const {
	return external_skeleton_node;
}

void YBoneAttachment3D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			if (use_external_skeleton) {
				_update_external_skeleton_cache();
			}
			_check_bind();
		} break;

		case NOTIFICATION_EXIT_TREE: {
			_check_unbind();
		} break;

		case NOTIFICATION_TRANSFORM_CHANGED: {
			_transform_changed();
		} break;

		case NOTIFICATION_INTERNAL_PROCESS: {
			if (_override_dirty) {
				_override_dirty = false;
			}
		} break;
	}
}

void YBoneAttachment3D::on_skeleton_update() {
	if (updating) {
		return;
	}
	updating = true;
	if (bone_idx >= 0) {
		Skeleton3D *sk = _get_skeleton3d();
		if (sk) {
			if (!override_pose) {
				if (ignore_scale) {
					if (use_external_skeleton) {
						const auto found_transform = sk->get_global_transform() * sk->get_bone_global_pose(bone_idx);
						set_global_basis(found_transform.basis.get_rotation_quaternion());
						set_global_position(found_transform.get_origin());
					} else {
						const auto found_transform = sk->get_bone_global_pose(bone_idx);
						set_global_basis(found_transform.basis.get_rotation_quaternion());
						set_global_position(found_transform.get_origin());
					}
				} else {
					if (use_external_skeleton) {
						set_global_transform(sk->get_global_transform() * sk->get_bone_global_pose(bone_idx));
					} else {
						set_transform(sk->get_bone_global_pose(bone_idx));
					}
				}
			} else {
				if (!_override_dirty) {
					_transform_changed();
					_override_dirty = true;
				}
			}
		}
	}
	updating = false;
}
#ifdef TOOLS_ENABLED
void YBoneAttachment3D::notify_skeleton_bones_renamed(Node *p_base_scene, Skeleton3D *p_skeleton, Dictionary p_rename_map) {
	const Skeleton3D *parent = nullptr;
	if (use_external_skeleton) {
		if (external_skeleton_node_cache.is_valid()) {
			parent = Object::cast_to<Skeleton3D>(ObjectDB::get_instance(external_skeleton_node_cache));
		}
	} else {
		parent = Object::cast_to<Skeleton3D>(get_parent());
	}
	if (parent && parent == p_skeleton) {
		StringName bn = p_rename_map[bone_name];
		if (bn) {
			set_bone_name(bn);
		}
	}
}

void YBoneAttachment3D::notify_rebind_required() {
	// Ensures bindings are properly updated after a scene reload.
	_check_unbind();
	if (use_external_skeleton) {
		_update_external_skeleton_cache();
	}
	bone_idx = -1;
	_check_bind();
}
#endif // TOOLS_ENABLED

YBoneAttachment3D::YBoneAttachment3D() {
}

void YBoneAttachment3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_bone_name", "bone_name"), &YBoneAttachment3D::set_bone_name);
	ClassDB::bind_method(D_METHOD("get_bone_name"), &YBoneAttachment3D::get_bone_name);

	ClassDB::bind_method(D_METHOD("set_bone_idx", "bone_idx"), &YBoneAttachment3D::set_bone_idx);
	ClassDB::bind_method(D_METHOD("get_bone_idx"), &YBoneAttachment3D::get_bone_idx);

	ClassDB::bind_method(D_METHOD("on_skeleton_update"), &YBoneAttachment3D::on_skeleton_update);

	ClassDB::bind_method(D_METHOD("set_override_pose", "override_pose"), &YBoneAttachment3D::set_override_pose);
	ClassDB::bind_method(D_METHOD("get_override_pose"), &YBoneAttachment3D::get_override_pose);

	ClassDB::bind_method(D_METHOD("set_ignore_scale", "ignore_scale"), &YBoneAttachment3D::set_ignore_scale);
	ClassDB::bind_method(D_METHOD("get_ignore_scale"), &YBoneAttachment3D::get_ignore_scale);

	ClassDB::bind_method(D_METHOD("set_use_external_skeleton", "use_external_skeleton"), &YBoneAttachment3D::set_use_external_skeleton);
	ClassDB::bind_method(D_METHOD("get_use_external_skeleton"), &YBoneAttachment3D::get_use_external_skeleton);
	ClassDB::bind_method(D_METHOD("set_external_skeleton", "external_skeleton"), &YBoneAttachment3D::set_external_skeleton);
	ClassDB::bind_method(D_METHOD("get_external_skeleton"), &YBoneAttachment3D::get_external_skeleton);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "bone_name"), "set_bone_name", "get_bone_name");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bone_idx"), "set_bone_idx", "get_bone_idx");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "override_pose"), "set_override_pose", "get_override_pose");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "ignore_scale"), "set_ignore_scale", "get_ignore_scale");
}
