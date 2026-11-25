#pragma once

#include "CrateGen.h"
#include "BarrelGen.h"
#include "PalletGen.h"
#include "PipeGen.h"

namespace procedural::props {

struct IndustrialPropParams {
    CrateParams crate;
    BarrelParams barrel;
    PalletParams pallet;
    PipeParams pipe;
};

} // namespace procedural::props
