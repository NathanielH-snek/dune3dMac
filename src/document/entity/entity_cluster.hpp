#pragma once
#include "entityt.hpp"
#include "ientity_in_workplane_set.hpp"
#include "ientity_movable2d.hpp"
#include "ientity_bounding_box2d.hpp"
#include "ientity_cluster_content_update.hpp"
#include "util/cluster_content.hpp"
#include <glm/glm.hpp>

namespace dune3d {
class EntityCluster : public EntityT<EntityCluster>,
                      public IEntityInWorkplaneSet,
                      public IEntityMovable2D,
                      public IEntityBoundingBox2D,
                      public IEntityClusterContentUpdate {
public:
    explicit EntityCluster(const UUID &uu);
    explicit EntityCluster(const UUID &uu, const json &j);

    static constexpr Type s_type = Type::CLUSTER;
    json serialize() const override;

    bool can_delete(const Document &doc) const override;

    double get_param(unsigned int point, unsigned int axis) const override;
    void set_param(unsigned int point, unsigned int axis, double value) override;

    glm::dvec3 get_point(unsigned int point, const Document &doc) const override;
    bool is_valid_point(unsigned int point) const override;
    glm::dvec2 get_point_in_workplane(unsigned int point) const override;

    glm::dvec2 m_origin = {0, 0};
    double m_scale_x = 1;
    double m_scale_y = 1;
    double m_angle = 0;

    bool m_lock_scale_x = false;
    bool m_lock_scale_y = false;
    bool m_lock_aspect_ratio = false;
    bool m_lock_angle = false;

    std::map<unsigned int, glm::dvec2> m_anchors_transformed;
    std::map<unsigned int, EntityAndPoint> m_anchors;
    std::map<unsigned int, EntityAndPoint> m_anchors_available;

    static constexpr unsigned int s_available_anchor_offset = 1000;

    void add_anchor(unsigned int i, const EntityAndPoint &enp);
    void remove_anchor(unsigned int i);
    void add_available_anchors();
    glm::dvec2 get_anchor_point(const EntityAndPoint &enp) const;

    glm::dvec2 transform(const glm::dvec2 &p) const;

    std::shared_ptr<const ClusterContent> m_content;

    UUID m_wrkpl;
    UUID m_exploded_group;

    void move(const Entity &last, const glm::dvec2 &delta, unsigned int point) override;

    static bool is_supported_entity(const Entity &en);

    std::string get_point_name(unsigned int point) const override;

    const UUID &get_workplane() const override
    {
        return m_wrkpl;
    }

    void set_workplane(const UUID &uu) override
    {
        m_wrkpl = uu;
    }

    const ClusterContent &get_cluster_content() const override
    {
        return *m_content;
    }

    void update_cluster_content_for_new_workplane(const UUID &wrkpl) override;

    std::pair<glm::dvec2, glm::dvec2> get_bbox() const override;

    std::set<UUID> get_referenced_entities() const override;
};

} // namespace dune3d
