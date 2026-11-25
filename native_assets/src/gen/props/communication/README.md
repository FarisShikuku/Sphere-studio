# Communication Props Engine

Procedural microwave, radio transmission, and satellite telecommunications props.

## Components
- **RadioMastGen**: Four-legged structural steel lattice tower with alternating red/white FAA aviation hazard bands and lightning arrestors.
- **SatelliteDishGen**: High-gain parabolic dish antenna with focal feedhorn assembly, Az-El gimbal mounts, and counterweights.

## Shaders & Profiles
- `radio_mast.comp`: Truss extrusion and diagonal cross-lacing compute kernel.
- `satellite_dish.comp`: Paraboloid surface generation and rim feed horn projection.
- `radio_mast_profile.h`: Structural L-angle and gusset plate section geometry.
- `satellite_dish_profile.h`: True parabolic curve sample functions.
