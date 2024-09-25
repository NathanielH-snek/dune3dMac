#include "tool_constrain_hv.hpp"
#include "document/document.hpp"
#include "document/entity/entity_workplane.hpp"
#include "document/constraint/constraint_hv.hpp"
#include "core/tool_id.hpp"
#include <optional>
#include "util/selection_util.hpp"
#include "util/template_util.hpp"
#include "tool_common_constrain_impl.hpp"

namespace dune3d {

ToolBase::CanBegin ToolConstrainHV::can_begin()
{
    if (!get_workplane_uuid())
        return false;

    auto tp = two_points_from_selection(get_doc(), m_selection);
    if (!tp)
        return false;

    if (!any_entity_from_current_group(tp->get_enps_as_tuple()))
        return false;

    auto wrkpl = get_workplane();
    const auto p1 = wrkpl->project(get_doc().get_point(tp->point1));
    const auto p2 = wrkpl->project(get_doc().get_point(tp->point2));
    const auto v = glm::abs(p2 - p1);
    const auto is_horizontal = v.x > v.y;
    switch (m_tool_id) {
    default:
    case ToolID::CONSTRAIN_HORIZONTAL_AUTO:
    case ToolID::CONSTRAIN_VERTICAL:
        if (!is_horizontal)
            return false;
        break;

    case ToolID::CONSTRAIN_HORIZONTAL:
    case ToolID::CONSTRAIN_VERTICAL_AUTO:
        if (is_horizontal)
            return false;
        break;
        return false;
    }

    if (any_of(m_tool_id, ToolID::CONSTRAIN_HORIZONTAL, ToolID::CONSTRAIN_HORIZONTAL_AUTO))
        return !has_constraint_of_type_in_workplane(tp->get_enps(), Constraint::Type::HORIZONTAL,
                                                    Constraint::Type::VERTICAL, Constraint::Type::SYMMETRIC_HORIZONTAL,
                                                    Constraint::Type::SYMMETRIC_VERTICAL,
                                                    Constraint::Type::POINT_DISTANCE_VERTICAL);
    else
        return !has_constraint_of_type_in_workplane(tp->get_enps(), Constraint::Type::HORIZONTAL,
                                                    Constraint::Type::VERTICAL, Constraint::Type::SYMMETRIC_HORIZONTAL,
                                                    Constraint::Type::SYMMETRIC_VERTICAL,
                                                    Constraint::Type::POINT_DISTANCE_HORIZONTAL);
}


ToolResponse ToolConstrainHV::begin(const ToolArgs &args)
{
    auto tp = two_points_from_selection(get_doc(), m_selection);

    if (!tp.has_value())
        return ToolResponse::end();

    ConstraintHV *constraint = nullptr;
    if (any_of(m_tool_id, ToolID::CONSTRAIN_HORIZONTAL, ToolID::CONSTRAIN_HORIZONTAL_AUTO))
        constraint = &add_constraint<ConstraintHorizontal>();
    else
        constraint = &add_constraint<ConstraintVertical>();
    constraint->m_entity1 = tp->point1;
    constraint->m_entity2 = tp->point2;
    constraint->m_wrkpl = get_workplane_uuid();

    return commit();
}

} // namespace dune3d
