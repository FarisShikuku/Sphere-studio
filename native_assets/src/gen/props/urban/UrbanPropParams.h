#pragma once

#include "HydrantGen.h"
#include "MailboxGen.h"
#include "BusStopGen.h"
#include "UrbanFenceGen.h"
#include "BollardGen.h"
#include "JerseyBarrierGen.h"

namespace procedural::props {

struct UrbanPropParams {
    HydrantParams hydrant;
    MailboxParams mailbox;
    BusStopParams busStop;
    UrbanFenceParams urbanFence;
    BollardParams bollard;
    JerseyBarrierParams jerseyBarrier;
};

} // namespace procedural::props
