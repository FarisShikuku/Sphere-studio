# Urban Props Engine

Procedural municipal city street hardware, public transit shelters, and postal fixtures.

## Components
- **HydrantGen**: Cast-iron municipal fire hydrants with threaded nozzle outlets, chain attachments, pentagonal operating nuts, and bonnet domes.
- **MailboxGen**: US street blue collection boxes with hopper drop doors, British red circular pillar boxes, and suburban post-mounted mailboxes with indicator flags.
- **BusStopGen**: Public transit bus stops with tubular structural steel frames, tempered glass panels, integrated benches, timetable totem posts, and curved canopies.

## Shaders & Profiles
- `hydrant.comp`: Stepped barrel lofting, outlet nozzle branching, bonnet dome revolution, and nut extrusion.
- `mailbox.comp`: Prismatic box fillet beveling, hopper chute sweep, and cylindrical leg pedestals.
- `bus_stop.comp`: Extruded tubular steel columns, cantilevered roof purlins, and glass panel planar synthesis.
- `hydrant_profile.h`: Bonnet dome curvature, flange lip, and nozzle specs.
- `mailbox_profile.h`: Domed cap profile and mail drop slot dimensions.
- `bus_stop_profile.h`: Curved canopy arc profile and glass mullion layout.
