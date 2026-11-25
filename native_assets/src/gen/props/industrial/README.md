# Industrial Props Engine

Procedural warehouse, logistics storage, and fluid distribution infrastructure assets.

## Components
- **CrateGen**: Parametric wooden shipping crates with diagonal cross-braces, reinforced corner brackets, and pine/hardwood styles.
- **BarrelGen**: Steel industrial oil drums, hazardous chemical barrels with bung plugs, and bulging wooden whiskey casks.
- **PalletGen**: EUR-1 Euro pallets, American GMA pallets, and heavy industrial wooden skids with fork entry chamfers.
- **PipeGen**: Flanged high-pressure process piping, 90-degree spool elbows, and T-junctions with manual handwheel valves.

## Shaders & Profiles
- `crate.comp`: Box facet slicing, slat relief generation, and corner bracket extrusion.
- `barrel.comp`: Parabolic belly bulge profile, rolling hoop rib lofting, and chime rim generation.
- `pallet.comp`: Stringer board alignment, bottom skids, and top deck slat spacing.
- `pipe.comp`: Cylindrical extrusion, radial flange bolt rings, and handwheel torus synthesis.
- `crate_profile.h`: Slat thickness and corner bracket dimensions.
- `barrel_profile.h`: Belly expansion curvature and rolling hoop spacing.
- `pallet_profile.h`: Standardized EUR and GMA block and deck profiles.
- `pipe_profile.h`: Flange thickness, bolt-circle layout, and 90-degree elbow arc profile.
