# Street Props Engine

Procedural street furniture, illumination, and urban traffic control fixtures.

## Components
- **LampPostGen**: Parametric street lamps (modern cobra-head, vintage Victorian lantern, LED bar) with curved arm reach and dual-arm variants.
- **TrafficLightGen**: Intersection traffic signals with multi-section housings, visors, mast arms, and pedestrian signal pods.
- **BenchGen**: Public park and urban street benches with slatted wooden seats, cast-iron legs, and backrests.
- **TrashBinGen**: Municipal waste receptacles, wire mesh cans, and covered recycling bins.

## Shaders & Profiles
- `lamp_post.comp`: Mast extrusion, arm lofting, luminaire housing, and emissive lens generation.
- `traffic_light.comp`: Mast arm cantilever, signal head stacking, cowl visor beveling, and colored lens emissives.
- `bench.comp`: Slat array extrusion, leg contour sweep, and armrest curvature.
- `trash_bin.comp`: Slotted cylinder lofting, inner liner volume, and pivot lid generation.
- `lamp_post_profile.h`: Tapered pole sections and outreach arm arc curve evaluator.
- `bench_profile.h`: Ergonomic slat profile and cast-iron frame contour.
- `traffic_light_profile.h`: Signal head section bounds and visor geometry profiles.
