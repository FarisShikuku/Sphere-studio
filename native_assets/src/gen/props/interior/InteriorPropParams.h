#pragma once

#include "DeskGen.h"
#include "ChairGen.h"
#include "ShelfGen.h"

namespace procedural::props {

struct InteriorPropParams {
    DeskParams desk;
    ChairParams chair;
    ShelfParams shelf;
};

} // namespace procedural::props
