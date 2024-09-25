#include "tool_constrain_diameter_radius.hpp"
#include "document/document.hpp"
#include "document/entity/entity.hpp"
#include "document/constraint/constraint_diameter_radius.hpp"
#include "util/selection_util.hpp"
#include "util/template_util.hpp"
#include "core/tool_id.hpp"
#include "tool_common_constrain_impl.hpp"

namespace dune3d {

ToolBase::CanBegin ToolConstrainDiameterRadius::can_begin()
{
    auto enp = point_from_selection(get_doc(), m_selection);
    if (!enp)
        return false;
    if (enp->point != 0)
        return false;
    auto &doc = get_doc();
    auto &en = get_entity(enp->entity);

    if (!en.of_type(Entity::Type::ARC_2D, Entity::Type::CIRCLE_2D))
        return false;

    if (any_of(m_tool_id, ToolID::MEASURE_DIAMETER, ToolID::MEASURE_RADIUS))
        return true;

    if (!any_entity_from_current_group(en))
        return false;

    const auto constraint_types = en.get_constraint_types(doc);
    if (set_contains(constraint_types, Constraint::Type::RADIUS, Constraint::Type::DIAMETER))
        return false;

    return true;
}

ToolResponse ToolConstrainDiameterRadius::begin(const ToolArgs &args)
{
    auto enp = point_from_selection(m_core.get_current_document(), m_selection);
    if (!enp.has_value())
        return ToolResponse::end();

    ConstraintDiameterRadius *constraint = nullptr;
    switch (m_tool_id) {
    case ToolID::CONSTRAIN_RADIUS:
    case ToolID::MEASURE_RADIUS:
        constraint = &add_constraint<ConstraintRadius>();
        break;

    case ToolID::CONSTRAIN_DIAMETER:
    case ToolID::MEASURE_DIAMETER:
        constraint = &add_constraint<ConstraintDiameter>();
        break;

    default:
        return ToolResponse::end();
    }

    constraint->m_entity = enp->entity;
    constraint->measure(get_doc());
    constraint->m_measurement = any_of(m_tool_id, ToolID::MEASURE_DIAMETER, ToolID::MEASURE_RADIUS);

    return commit();
}
} // namespace dune3d
