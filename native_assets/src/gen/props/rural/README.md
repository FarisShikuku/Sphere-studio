# Rural Props Engine

Procedural countryside farmstead barriers, agricultural feed, and livestock watering infrastructure.

## Components
- **FenceGen**: Country split-rail ranch fences, classic white picket fences with spear points, and multi-rail pasture fences.
- **HayBaleGen**: Giant round rolled straw bales with ground compression flattening and twine wraps, and compact rectangular silage bales.
- **WaterTroughGen**: Galvanized corrugated steel livestock water troughs, carved stone basins, floating brass valves, and procedural water volume.

## Shaders & Profiles
- `fence.comp`: Parametric post repetition, rail mortise-and-tenon intersection, and picket spear profiling.
- `hay_bale.comp`: Contact plane sag deformation, spiral straw winding displacement, and twine hoop banding.
- `water_trough.comp`: Basin wall extrusion, rolled rim lip, float valve rod lofting, and planar water level mesh.
- `fence_profile.h`: Picket width, spear contour, and split-rail taper profile.
- `hay_bale_profile.h`: Cylindrical roll sag and ground contact contour.
- `water_trough_profile.h`: Basin wall thickness, rim bevel, and float ball parameters.
