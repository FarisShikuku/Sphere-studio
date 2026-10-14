# Sphere Studio

**Sphere Studio** is a procedural 3D asset generation engine and interactive multiplatform studio for open-world simulation, games, and digital twins. The project combines a high-performance **C++20 / Vulkan 1.3** native procedural asset engine with a modern **Kotlin Multiplatform (KMP)** and **Jetpack Compose** interactive studio.

> **Note on Directory Structure**: Legacy references to the procedural generation directory (`gen/`) are now unified under `native_assets/`. All native C++ pipelines, generators, shaders, and tests reside in `native_assets/`.

---

## Architecture Overview

```
├── app/                  # Android application module (launcher, themes, activity)
├── kmp/                  # Kotlin Multiplatform shared module (Android, iOS, Desktop)
│   ├── src/commonMain/   # Compose Multiplatform Studio UI, scene canvas & state
│   ├── src/androidMain/  # Android platform adapters
│   ├── src/desktopMain/  # Desktop JVM platform adapters
│   └── src/iosMain/      # iOS platform adapters
├── native_assets/        # C++20 & Vulkan 1.3 Procedural Engine (formerly gen/)
│   ├── base/             # Vulkan 1.3 base infrastructure & hardware abstractions
│   ├── src/              # Subsystem implementations (core, vulkan, scene, shared, generators)
│   ├── shaders/          # GLSL 460 shaders (PBR, sky, compute, bindless heaps)
│   ├── meshes/           # Base geometric primitives & reference geometries
│   ├── textures/         # Lookup tables (BRDF, blue noise, sky LUTs)
│   ├── tests/            # Native test suites
│   └── CMakeLists.txt    # Native engine build configuration
├── build.gradle.kts      # Root build configuration
├── settings.gradle.kts   # Gradle project settings
└── metadata.json         # Platform project metadata
```

---

## 1. Native Procedural Engine (`native_assets/`)

The native engine is written in standard **C++20** and targets **Vulkan 1.3** with dynamic rendering, bindless descriptor indexing, and physical simulation capabilities.

Architectural patterns are derived from:
- [SaschaWillems/Vulkan](https://github.com/SaschaWillems/Vulkan): Device abstraction, swapchain, memory management, and pipeline caching.
- [kentril0/WaterSurfaceRendering](https://github.com/kentril0/WaterSurfaceRendering): Scene graphs, camera controllers, sky models, and bindless descriptors.

### Core Modules (`native_assets/src/`)

- **`core/`**: Engine lifecycle, high-resolution timers, profilers, memory tracking, and logging macros.
- **`vulkan/`**: Vulkan 1.3 modern rendering abstractions:
  - Dynamic Rendering (`VK_KHR_dynamic_rendering` without legacy render passes).
  - Bindless Descriptor Sets (`VkDescriptorSetLayoutBinding` with `VARIABLE_DESCRIPTOR_COUNT`).
  - Pipeline Layout Builder with push constants.
  - FrameGraph resource barrier management.
- **`scene/`**:
  - `Camera.h`/`Camera.cpp`: Perspective camera with frustum extraction and view-projection calculation.
  - `SunSky.h`/`SunSky.cpp`: Preetham/Hosek-Wilkie physical sky radiance and directional sun lighting with live toggles.
  - `Transform.h`, `AABB.h`, `Mesh.h`, `WorldState.h`: Scene graph spatial hierarchy and bounding volumes.

### Shared Algorithms & Mathematical Pipelines (`native_assets/src/shared/`)

- **Hi-Z Occlusion Culling (`math/`)**:
  - `HizBuild.cpp`: Multi-mip downsampling pipeline loop using Vulkan 1.3 `VkImageMemoryBarrier` transitions and `vkCmdBlitImage` reduction.
  - `HizCull.cpp`: Screen-space projected AABB bounding box calculation with multi-LOD depth pyramid testing.
  - `FrustumCull.cpp`: 6-plane frustum extraction and AABB intersection tests.
  - `SplineLib.cpp`: Cubic Hermite and Catmull-Rom spline curves with arc-length parameterization and Frenet-Serret frames.
- **Physics & Collision Detection (`physics/`)**:
  - `PhysicsWorld.cpp`: Rigid body numerical integration with Möller–Trumbore ray-triangle intersections, Kay-Kayfia slab ray-AABB intersections, and ray-sphere tests.
  - `CollisionGroups.cpp`: Layer mask collision filtering (`Static`, `Terrain`, `Dynamic`, `Player`, `Vehicle`).
  - `PhysicsDebug.cpp`: Real-time wireframe collider rendering via `DebugDraw`.
- **Skeletal Kinematics & Skinning (`skeletons/`)**:
  - `SkeletonGen.cpp`: Bone hierarchy forward kinematics and exact 4x4 matrix inversion using Cramer's rule.
  - `RigShared.cpp`: Smooth Laplacian distance-to-bone skinning weights generator with top-4 influence selection and normalization.
- **Tileable Blue Noise (`noise/`)**:
  - `NoiseBlue.cpp`: Void-and-Cluster / Mitchell Best-Candidate algorithm on a toroidal 2D grid with Gaussian energy splatting for high-quality ray-marching and dithering.
- **Debug Drawing (`debug/`)**:
  - `DebugDraw.cpp`: Thread-safe immediate-mode rendering of debug rays, 3D lines, bounding boxes, frustums, and parametric wireframe spheres.
- **Materials & Memory (`materials/`, `memory/`)**:
  - `MaterialDefs.cpp` & `MaterialRegistry.cpp`: Standardized PBR parameter structures and cache.
  - `StreamingPool.cpp`: Ring-buffer staging allocators for low-overhead dynamic vertex and index streaming.
- **Generator Context (`GeneratorContext.cpp`)**:
  - Unified `GeneratorContext` supplying time, wind vectors, precipitation, snow depth, and 32-bit FNV-1a hash seed derivation to all generators.

### Procedural Asset Domain Generators

All asset generators adhere to the uniform `IGenerator` interface and generate parametric 3D meshes, textures, and descriptors:
- `audio/`: Spatial soundscapes, footsteps, engine audio, and ambient weather.
- `buildings/`: Procedural residential, commercial, industrial, civic, and damaged structures with LODs and interiors.
- `particles/`: GPU particle graphs for fire, smoke, sparks, dust, and explosions.
- `props/`: Street furniture, utility poles, signage, interior, and industrial props.
- `road/`: Multi-lane road networks, asphalt wear states (potholed, wet, icy, cracked, muddy), bridges, tunnels, and intersections.
- `sky/`: Atmospheric scattering, sun disc, moon phases, star fields, and weather LUTs.
- `surfaces/`: Procedural PBR materials (concrete, metal, wood, tiles, fabric, glass, decals).
- `terrain/`: Multi-octave heightmaps, splatmaps, cliff geometry, road cuts, and cave systems.
- `ui/`: SDF vector icons, procedural layout atlases, and HUD widgets.
- `vegetation/`: Procedural trees, bushes, grass blades, flowers, crops, and wind sway physics.
- `vehicles/`: Procedural cars, trucks, bikes, emergency vehicles, and mechanical parts.
- `water/`: Dynamic FFT ocean surfaces, rivers, lakes, waterfalls, underwater caustics, and foam.
- `weather/`: Volumetric clouds, rain streaks, snow accumulators, fog, and lightning bolts.

### Shaders (`native_assets/shaders/`)

- `pbr.vert` / `pbr.frag`: Physically-based rendering with Cook-Torrance microfacet specular GGX BRDF and bindless textures.
- `sky.vert` / `sky.frag`: Analytical physical atmosphere scattering with sun disk luminance and Rayleigh/Mie phase functions.
- `hiz_reduce.comp`: Compute shader for hierarchical Z-buffer mip downsampling.
- `cull.comp`: GPU compute indirect draw command occlusion culling.

---

## 2. Interactive Studio UI (`kmp/` & `app/`)

The frontend is built using **Compose Multiplatform** (`kmp/src/commonMain/kotlin/com/example/sphere/`):

- **Real-Time Scene Canvas**:
  - Drag gesture-based interactive 3D orbit camera (pitch and yaw rotation).
  - Perspective grid visualization and dynamic horizon gradient rendering.
  - Dynamic sun & sky toggle: easily enable or disable atmospheric scattering and directional sun radiance to preview assets under varying lighting conditions.
- **Subsystem Structure Verification**:
  - Automated scanner that verifies the directory structure, header conventions, and implementation integrity of all `native_assets/` modules.
- **Domain Generation Panels**:
  - Interactive parameter controls for road generation, vegetation density, terrain heightmaps, surface materials, and weather presets.
  - Random seed generators with live regeneration triggers.

---

## 3. Building & Testing

### Android Application
Build and run the Android app using Gradle:

```bash
# Compile Android Debug APK
gradle :app:assembleDebug

# Run JVM Unit & Robolectric Tests
gradle :app:testDebugUnitTest
```

### Native Assets Engine (`native_assets/`)
To configure and compile the native C++ engine standalone:

```bash
cd native_assets
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

Requirements for standalone native compilation:
- C++20 compliant compiler (GCC 11+, Clang 13+, or MSVC 2019+)
- CMake 3.20+
- Vulkan SDK 1.3+
