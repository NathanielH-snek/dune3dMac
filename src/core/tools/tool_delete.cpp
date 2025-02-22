#include "tool_delete.hpp"
#include "document/document.hpp"
#include "document/entity/entity.hpp"
#include "document/entity/ientity_delete_point.hpp"
#include "document/constraint/constraint.hpp"
#include "document/group/group.hpp"
#include "tool_common_impl.hpp"
#include "editor/editor_interface.hpp"
#include "editor/buffer.hpp"
#include <iostream>
#include "core/tool_id.hpp"

namespace dune3d {

ToolBase::CanBegin ToolDelete::can_begin()
{
    if (m_tool_id == ToolID::CUT && !Buffer::can_create(get_doc(), m_selection))
        return false;

    for (const auto &sr : m_selection) {
        if (sr.type == SelectableRef::Type::ENTITY) {
            auto &entity = get_entity(sr.item);
            if (entity.can_delete(get_doc()))
                return true;
        }
        else if (sr.type == SelectableRef::Type::CONSTRAINT) {
            return true;
        }
    }
    return false;
}

ToolResponse ToolDelete::begin(const ToolArgs &args)
{
    auto &doc = get_doc();

    ItemsToDelete items_to_delete;
    const Group *first_anchor_group = nullptr;
    std::set<EntityAndPoint> deleted_anchors;

    for (auto &sr : m_selection) {
        if (sr.type == SelectableRef::Type::ENTITY) {
            auto &en = doc.get_entity(sr.item);
            if (!en.can_delete(doc))
                continue;
            if (auto en_del = dynamic_cast<IEntityDeletePoint *>(&en)) {
                if (en_del->delete_point(sr.point)) {
                    deleted_anchors.insert(sr.get_entity_and_point());
                    doc.accumulate_first_group(first_anchor_group, en.m_group);
                }
                else {
                    items_to_delete.entities.insert(sr.item);
                }
            }
            else {
                items_to_delete.entities.insert(sr.item);
            }
        }
        else if (sr.type == SelectableRef::Type::CONSTRAINT)
            items_to_delete.constraints.insert(sr.item);
    }
    ItemsToDelete selected_items = items_to_delete;

    for (const auto &[uu, constr] : doc.m_constraints) {
        std::set<EntityAndPoint> isect;
        std::ranges::set_intersection(constr->get_referenced_entities_and_points(), deleted_anchors,
                                      std::inserter(isect, isect.begin()));
        if (isect.size())
            items_to_delete.constraints.insert(uu);
    }

    auto extra_items = doc.get_additional_items_to_delete(items_to_delete);
    items_to_delete.append(extra_items);

    if (m_tool_id == ToolID::CUT) {
        auto buffer = Buffer::create(get_doc(), m_selection, Buffer::Operation::CUT);
        if (buffer)
            m_intf.set_buffer(std::move(buffer));
    }

    m_intf.show_delete_items_popup(selected_items, items_to_delete);

    doc.delete_items(items_to_delete);

    if (first_anchor_group) {
        doc.set_group_solve_pending(first_anchor_group->m_uuid);
    }


    return ToolResponse::commit();
}


ToolResponse ToolDelete::update(const ToolArgs &args)
{
    return ToolResponse();
}
} // namespace dune3d
