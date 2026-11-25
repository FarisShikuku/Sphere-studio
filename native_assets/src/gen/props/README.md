# World Props Subsystem

Procedural simulation engine domain for street, industrial, urban, rural, interior, utility, energy, communication, and signage fixtures.

## Architecture
- **`PropSystem`**: Central registry and mesh synthesis dispatcher for all prop types.
- **`PropMeshHelper`**: CPU-side geometry assembler generating flat triangle streams for JNI interop and GPU buffer staging.
- **Compute Shaders (`*.comp`)**: GPU-driven procedural extrusion, lathe, and profiling passes.
- **Texture Specs (`textures/spec/*_spec.h`)**: PBR material roughness, metallic, transmission, and emission properties.
- **Mesh Profiles (`meshes/profiles/*_profile.h`)**: Mathematical curves and geometric cross-sections.

## Sub-Domains
- **`street/`**: Street lamps, traffic signals, park benches, municipal bins.
- **`industrial/`**: Shipping crates, 55-gal drums, Euro/GMA pallets, flanged process pipes.
- **`urban/`**: Fire hydrants, collection mailboxes, transit bus shelters.
- **`rural/`**: Ranch fences, compressed round/square hay bales, water troughs.
- **`interior/`**: Office workstations/desks, ergonomic swivel chairs, tiered bookcases.
- **`utility/`**: Electric posts, pole/pad transformers, smart meters, cobra-head street lights, water towers, telecom towers.
- **`energy/`**: Wind turbines (towers, nacelles, blades), solar panels, tracking arrays, battery banks, substations.
- **`communication/`**: Radio broadcast masts, parabolic satellite dishes.
- **`signage_street/`**: Highway directional info boards, milestone markers, delineator posts.
- **`fences/`**: Modern tempered glass, aluminum fin, laser-cut Corten, gabion stone, ribbed concrete, louver, living wall, bamboo, cable-rail, and corrugated Corten fences.
- **`gates/`**: Estate, toll, railway, garden, cantilevered slat, glass pivot, laser art, bi-fold speed, louver, brutalist portal, optical speedstile, Japanese sukiya, gabion timber, and solar smart gates.
