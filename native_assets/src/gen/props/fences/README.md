# Fences Props Engine

Architectural and perimeter fence generators covering modern tempered glass, aluminum fin, laser-cut Corten, gabion stone, ribbed concrete, louver blade, living botanical wall, bamboo & steel, stainless cable-rail, and corrugated Corten systems.

## Shaders & Profiles
- `shaders/include/fence_common.glsl`: Shared fence bay repetition and metal finish helpers.
- `shaders/*.comp`: GPU compute kernels for all 10 modern fence archetypes.
- `textures/spec/fences_spec.h`, `textures/fence_material.glsl`, `textures/fence_pbr_atlas.comp`: PBR material constants and procedural surface atlas generation.
- `meshes/profiles/modern_fence_profile.h`, `meshes/spec/fences_mesh_spec.h`: Cross-section dimensions and vertex/LOD budgets.
