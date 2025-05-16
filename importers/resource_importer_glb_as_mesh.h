//
// Created by Daniel on 2024-07-29.
//

#ifndef RESOURCEIMPORTERGLBASMESH_H
#define RESOURCEIMPORTERGLBASMESH_H

#include "core/io/resource_importer.h"
#include "scene/3d/importer_mesh_instance_3d.h"

#ifdef TOOLS_ENABLED
#include "editor/editor_interface.h"
#include "editor/editor_file_system.h"
#endif

#include "modules/gltf/gltf_document.h"
#include "scene/resources/material.h"


class ResourceImporterGLBasMesh : public ResourceImporter {
    GDCLASS(ResourceImporterGLBasMesh, ResourceImporter);

public:
    virtual String get_importer_name() const override;
    virtual String get_visible_name() const override;
    virtual void get_recognized_extensions(List<String> *p_extensions) const override;
    virtual String get_save_extension() const override;
    virtual String get_resource_type() const override;
    virtual int get_format_version() const override;

    virtual int get_preset_count() const override;
    virtual String get_preset_name(int p_idx) const override;

    virtual void get_import_options(const String &p_path, List<ImportOption> *r_options, int p_preset = 0) const override;
    virtual bool get_option_visibility(const String &p_path, const String &p_option, const HashMap<StringName, Variant> &p_options) const override;

    static TypedArray<ImporterMeshInstance3D> find_first_matched_nodes(const Node *checking_node);

    virtual Error import(ResourceUID::ID p_source_id, const String &p_source_file, const String &p_save_path, const HashMap<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files = nullptr, Variant *r_metadata = nullptr) override;

    virtual int get_import_order() const override { return ResourceImporter::IMPORT_ORDER_SCENE; }
    // virtual float get_priority() const { return 1.0; }
    // Threaded import can currently cause deadlocks, see GH-48265.
    virtual bool can_import_threaded() const override { return false; }

    void smooth_normals(Array &p_arrays, float p_angle_threshold, bool p_has_vertex_colors);

    String sanitize_filename(const String &p_filename);

    ResourceImporterGLBasMesh();
    ~ResourceImporterGLBasMesh();
};


#endif //RESOURCEIMPORTERGLBASMESH_H
