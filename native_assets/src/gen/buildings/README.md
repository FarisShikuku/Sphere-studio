# Procedural Buildings Subsystem

Modular architectural geometry engine covering residential, commercial, industrial, civic, interior, rooftop prop, signage, structural damage, and LOD impostor domains.

## Architecture
- **`BuildingSystem` / `ProceduralBuildingGenerator`**: Synthesizes 27 architectural volume shapes, 7 corner profiles, 23 window systems, 10 door systems, 22 roof profiles, and 16 facade cladding materials.
- **Shaders (`shaders/*.comp`, `shaders/*.glsl`, `shaders/*.vert/.frag`)**: GLSL 450 compute passes for facade atlases, window grids, corrugated industrial bays, neon emissives, Voronoi damage masks, and octahedral LOD impostors.
- **Textures (`textures/spec/*_spec.h`, `textures/*.comp`)**: PBR roughness, metallic, IOR, and atlas resolution specifications.
- **Meshes (`meshes/spec/*_mesh_spec.h`, `meshes/profiles/*.h`)**: Architectural cross-sections, LOD tiers, and vertex budget specifications.
