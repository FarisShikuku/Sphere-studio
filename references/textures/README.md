# Reference Surface Textures (Design Documentation)

This folder contains high-resolution photographic reference plates used as target visual benchmarks for the Sphere Engine procedural compute shaders (`.comp`) and PBR material synthesis.

These are **design reference documents only** and are not bundled into the runtime application APK/AAB to ensure zero asset bloat and preserve pure procedural generation.

---

### Reference Plates Inventory

1. **`rusticated_red_brick.jpg`**
   - **Target Material**: `MaterialID::BRICK_RUSTICATED` / `SurfaceDef::brick_wall_surface`
   - **Scale**: 900 mm stretcher bond repeat
   - **Brick Dimensions**: 215 mm width × 65 mm height
   - **Mortar Joint**: 10 mm width, ~6.5 mm recessed relief
   - **Characteristics**: Hand-made clay brick texture, weathered crumbly edges, light lime efflorescence staining, sandy porous mortar.

2. **`mondo_grass.jpg`**
   - **Target Material**: `MaterialID::GRASS_SURFACE` / `SurfaceDef::grass_turf_surface`
   - **Scale**: 300 mm lawn turf repeat
   - **Characteristics**: Flat surface-only ground turf; dense chlorophyll blade clusters, root thatch undertones, zero-geometry overhead for broad landscapes.

3. **`earth_foliage.jpg`**
   - **Target Material**: `MaterialID::EARTH_FOLIAGE` / `SurfaceDef::earth_foliage_surface`
   - **Scale**: 700 mm terrain ground repeat
   - **Characteristics**: Moist organic loam topsoil substrate, creeping broadleaf clover and ivy micro-vegetation, small basalt pebbles, decayed organic matter.
