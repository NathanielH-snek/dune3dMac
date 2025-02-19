#pragma once

namespace dune3d {

enum class InToolActionID {
    NONE,
    // common
    LMB,
    RMB,
    TOGGLE_CONSTRUCTION,
    TOGGLE_COINCIDENT_CONSTRAINT,
    TOGGLE_HV_CONSTRAINT,
    TOGGLE_TANGENT_CONSTRAINT,

    FLIP_ARC,
    ARC_MODE,

    TOGGLE_ARC,
    TOGGLE_BEZIER,

    CLEAR_EDGES,

    CLEAR_SPINE_ENTITIES,

    N_SIDES_INC,
    N_SIDES_DEC,
    ENTER_N_SIDES,

    TOGGLE_RECTANGLE_MODE,

    CANCEL,
    COMMIT
};

} // namespace dune3d
