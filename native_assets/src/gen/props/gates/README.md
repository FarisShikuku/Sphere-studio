# Gates Props Engine

Procedural estate, toll, railway, garden, and 10 modern architectural gate generators with smart hardware toggles, finish presets, and multi-leaf geometry.

## Shaders & Profiles
- `shaders/include/gate_common.glsl`: Shared gate parameters, metal/wood finish lookup, and vertex stream structs.
- `shaders/*.comp`: GLSL 450 compute shaders for all legacy and modern gate archetypes.
- `textures/spec/gates_spec.h`, `textures/gate_finish.glsl`, `textures/gate_pbr_atlas.comp`: PBR material constants and procedural texture atlas pass.
- `meshes/profiles/gate_profile.h`, `meshes/spec/gates_mesh_spec.h`: Cross-section dimensions and vertex/LOD budgets.
