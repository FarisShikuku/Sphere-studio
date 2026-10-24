# Full Procedural Asset Tree — With Descriptions, Road States, Energy/Utility Props

```
native_assets/
├── CMakeLists.txt                                  # Top-level CMake project configuration for C++20 and Vulkan 1.3
├── main.cpp                                        # Engine entrypoint and headless procedural generation harness
├── README.md                                       # Native subsystem overview and architectural references
├── .clang-format                                   # LLVM-based code formatting configuration
├── .gitignore                                      # Build artifact, binary cache, and object file ignore rules
│
├── base/                                           # Vulkan 1.3 Base Primitives (ported from SaschaWillems/Vulkan)
│   ├── CMakeLists.txt                              # Builds libvulkan_base.a
│   ├── CommandLineParser.cpp / .hpp                # CLI argument parsing for headless asset baking
│   ├── keycodes.hpp                                # Cross-platform input scan codes
│   ├── VulkanAndroid.cpp / .h                      # Android NDK Vulkan surface and asset manager glue
│   ├── VulkanBuffer.cpp / .hpp                     # Staging and device-local VMA buffer wrappers
│   ├── VulkanDebug.cpp / .h                        # VK_EXT_debug_utils callbacks and validation layer hooks
│   ├── VulkanDevice.cpp / .hpp                     # Logical device, physical device queue family selection
│   ├── VulkanInitializers.hpp                      # Static inline builder structs for Vulkan descriptors and states
│   ├── VulkanSwapChain.cpp / .hpp                  # Surface capabilities, present queues, and double/triple buffering
│   ├── VulkanTexture.cpp / .hpp                    # Image allocation, mipmap generation, and layout transitions
│   ├── VulkanTools.cpp / .h                        # SPIR-V shader module loaders and error logging helpers
│   ├── VulkanUIOverlay.cpp / .h                    # ImGui / Dear ImGui Vulkan integration overlay
│   └── vulkanexamplebase.cpp / .h                  # Base interactive application loop and renderpass setup
│
├── src/                                            # Core Engine & Procedural Generators
│   ├── CMakeLists.txt                              # Aggregates all procedural subsystem static libraries
│   │
│   ├── core/                                       # Core Engine Lifecycle & Infrastructure
│   │   ├── CMakeLists.txt                          # Builds libcore.a
│   │   ├── Assert.h                                # Runtime and debug invariant assertions
│   │   ├── Engine.h / .cpp                         # Master subsystem lifecycle, tick loops, and subsystem dispatch
│   │   ├── Log.h / .cpp                            # High-throughput asynchronous tagged logging system
│   │   ├── Platform.h                              # Target platform macros (Linux, Windows, Android, macOS)
│   │   ├── Profiler.h / .cpp                       # Chrome tracing and CPU/GPU timestamp micro-profiling
│   │   ├── Timer.h / .cpp                          # High-resolution monotonic timers and delta-time smoothing
│   │   └── Types.h                                 # Fixed-width primitive typedefs, SIMD types, and handles
│   │
│   ├── vulkan/                                     # Modern Vulkan 1.3 Compute & Graphics Abstractions
│   │   ├── CMakeLists.txt                          # Builds libvulkan_layer.a
│   │   ├── BindlessHeap.h / .cpp                   # VK_EXT_descriptor_indexing unbounded descriptor heap
│   │   ├── DynamicRendering.h / .cpp               # Vulkan 1.3 renderpass-less pipeline dynamic attachments
│   │   ├── FrameGraph.h / .cpp                     # Asynchronous compute-graphics barrier and resource DAG
│   │   ├── PipelineBuilder.h / .cpp                # Fluent pipeline state caching and SPIR-V reflection
│   │   ├── ShadowMapPass.h / .cpp                  # Vulkan 1.3 dedicated offscreen depth pass (SaschaWillems/Vulkan)
│   │   └── VulkanContext.h / .cpp                  # Encapsulates VulkanInstance, VkDevice, and queue handles
│   │
│   ├── scene/                                      # Common Scene System (kentril0/WaterSurfaceRendering style)
│   │   ├── CMakeLists.txt                          # Builds libscene.a
│   │   ├── Camera.h / .cpp                         # 6-DOF flycam, spherical orbit camera, and perspective frustum
│   │   ├── Light.h / .cpp                          # Directional sun, point lights, spot lights, and cascade shadows
│   │   ├── Material.h / .cpp                       # PBR material parameters (Albedo, Normal, Roughness, Metalness, AO)
│   │   ├── Mesh.h / .cpp                           # GPU vertex/index buffer management and AABB bounding computation
│   │   ├── Scene.h / .cpp                          # Entity-component-system graph, spatial index, and visibility query
│   │   ├── SceneGraph.h / .cpp                     # Hierarchical transform tree with dirty matrix propagation
│   │   └── Sky.h / .cpp                            # Sky dome backdrop and environment map cubemap bindings
│   │
│   ├── shared/                                     # Shared Math, Memory, and Procedural Utilities
│   │   ├── CMakeLists.txt                          # Builds libshared.a
│   │   ├── IGenerator.h                            # Pure abstract interface for all procedural asset generators
│   │   ├── GeneratorContext.h / .cpp               # Context holding Vulkan device, heap, and PRNG seeds
│   │   ├── GeneratorRegistry.h / .cpp              # Subsystem service locator and factory registry
│   │   ├── GpuTypes.h / .cpp                       # GPU std430 struct alignments and push constants
│   │   ├── Seed.h / .cpp                           # Deterministic multi-stage PRNG seed mixers
│   │   ├── bindless/                               # Shared bindless handles and texture array accessors
│   │   ├── debug/                                  # Debug line, box, and frustum drawing
│   │   ├── math/                                   # Frustum culling, Hi-Z occlusion, spline math (Catmull-Rom, B-Spline)
│   │   ├── memory/                                 # Dynamic staging buffer pools and persistent ring buffers
│   │   ├── noise/                                  # Simplex, Perlin, Worley, and Blue Noise generators
│   │   ├── physics/                                # Collision groups, raycasting, and physics debug draw
│   │   └── skeletons/                              # Procedural rigging and socket attachment hierarchies
│   │
│   ├── weather/                                    # Volumetric Weather Systems
│   │   ├── CMakeLists.txt                          # Builds libweather.a
│   │   ├── README.md                               # Weather subsystem architecture documentation
│   │   ├── WeatherState.h                          # Live weather conditions (rain, snow, wind, cloud coverage)
│   │   ├── WeatherSystem.h / .cpp                  # Master weather coordinator updating active generators
│   │   │
│   │   ├── clouds/                                 # Nubis-Decima Volumetric Clouds (Robert Beckebans port)
│   │   │   ├── CloudGen.h / .cpp                   # Cloud shape volume synthesizer
│   │   │   ├── CloudParams.h                       # Radiative transfer, Henyey-Greenstein, and altitude limits
│   │   │   ├── CloudRaymarch.h / .cpp              # Raymarcher implementation through curved planetary shell
│   │   │   ├── README.md                           # Deep dive on Nubis radiative transfer & cone sampling
│   │   │   ├── shaders/
│   │   │   │   ├── cloud_noise.comp                # 3D Perlin-Worley noise generation compute shader
│   │   │   │   ├── cloud_shape.comp                # Low-frequency cloud coverage mask compute shader
│   │   │   │   └── cloud_raymarch.comp             # Full Vulkan compute shader for perspective raymarching
│   │   │   ├── textures/
│   │   │   │   ├── cloud_shape_3d.comp             # High-resolution 3D cloud volume baker
│   │   │   │   └── spec/cloud_spec.h               # Texture formats, 3D resolution bounds, and mipmap specs
│   │   │   └── meshes/spec/cloud_mesh_spec.h       # Sky dome and atmospheric bounding shell vertices
│   │   │
│   │   ├── fog/                                    # Volumetric Froxel Fog & Ground Mist
│   │   │   ├── FogVolume.h / .cpp                  # 3D froxel grid allocation & light scattering
│   │   │   ├── FogParams.h                         # Extinction, scattering coefficients, height falloff
│   │   │   ├── README.md                           # Fog module documentation
│   │   │   ├── shaders/
│   │   │   │   ├── fog_froxel.comp                 # Froxel volumetric lighting compute shader
│   │   │   │   └── fog_ground.comp                 # Ground-hugging mist compute shader
│   │   │   ├── textures/
│   │   │   │   ├── fog_noise.comp                  # 3D turbulence noise for fog drifts
│   │   │   │   └── spec/fog_spec.h                 # Froxel grid size and slice exponential distribution
│   │   │   └── meshes/spec/fog_mesh_spec.h         # Frustum bounding box mesh spec
│   │   │
│   │   ├── lightning/                              # Dynamic Lightning Generation
│   │   │   ├── FlashCurve.h                        # Temporal discharge curve (initial leader to return strokes)
│   │   │   ├── LightningGen.h / .cpp               # Recursive fractal discharge branch generator
│   │   │   ├── LightningParams.h                   # Branch probability, fork angle, leader step
│   │   │   ├── README.md                           # Lightning module documentation
│   │   │   ├── shaders/bolt_path.comp              # Compute shader computing lightning branch points
│   │   │   ├── textures/spec/lightning_spec.h      # Lightning glow and HDR emission specs
│   │   │   └── meshes/spec/lightning_mesh_spec.h   # Ribbon mesh format for high-voltage bolt paths
│   │   │
│   │   ├── presets/                                # Weather Condition Presets
│   │   │   ├── PresetDefaults.h                    # Constexpr parameter tables
│   │   │   ├── WeatherPreset.h / .cpp              # Preset interpolation engine (smooth multi-parameter lerp)
│   │   │   ├── WeatherClear.cpp                    # Clear sky with high visibility and low aerosol turbidity
│   │   │   ├── WeatherRain.cpp                     # Moderate precipitation with road wetness
│   │   │   ├── WeatherSnow.cpp                     # Snowfall with accumulating snowpack
│   │   │   ├── WeatherStorm.cpp                    # Downpour, gale winds, lightning, low cloud base
│   │   │   ├── README.md                           # Preset module documentation
│   │   │   ├── shaders/preset_blend.comp           # GPU parameter blending compute shader
│   │   │   ├── textures/spec/preset_spec.h         # Preset LUT dimensions and interpolation curve specs
│   │   │   └── meshes/spec/preset_mesh_spec.h      # Preset preview thumbnail geometry spec
│   │   │
│   │   ├── puddles/                                # Dynamic Puddle Accumulation & Ground Wetness
│   │   │   ├── PuddleMask.cpp                      # Puddle growth modeling based on terrain curvature
│   │   │   ├── PuddleParams.h                      # Fill rate, soil saturation, drainage coefficients
│   │   │   ├── WetnessMap.h / .cpp                 # Wetness evaporation and capillary action
│   │   │   ├── README.md                           # Puddles module documentation
│   │   │   ├── shaders/
│   │   │   │   ├── puddle_mask.comp                # Water pooling compute shader
│   │   │   │   └── wetness.comp                    # Surface porous absorption compute shader
│   │   │   ├── textures/spec/puddle_spec.h         # High-resolution wetness map specifications
│   │   │   └── meshes/spec/puddle_mesh_spec.h      # Decal mesh format for localized puddle patches
│   │   │
│   │   ├── rain/                                   # Precipitation Particle Simulation
│   │   │   ├── RainSystem.h / .cpp                 # GPU particle emitter for raindrops
│   │   │   ├── RainSplash.h / .cpp                 # Ground contact splash rings and ripple generation
│   │   │   ├── RainParams.h                        # Terminal velocity, drop size, slant angle, gust reaction
│   │   │   ├── README.md                           # Rain module documentation
│   │   │   ├── shaders/
│   │   │   │   ├── rain_particle.comp              # Particle physics and terminal velocity compute shader
│   │   │   │   ├── rain_splash.comp                # Ground collision ripple compute shader
│   │   │   │   └── rain_streak.comp                # Motion blur ray streak compute shader
│   │   │   ├── textures/
│   │   │   │   ├── rain_streak_atlas.comp          # Procedural raindrop streak atlas generator
│   │   │   │   └── spec/rain_spec.h                # Rain streak sprite atlas texture specification
│   │   │   └── meshes/spec/rain_mesh_spec.h        # Instanced streak billboard mesh spec
│   │   │
│   │   ├── sky/                                    # Weather Sky Coupling
│   │   │   ├── SkyGenerator.h / .cpp               # Overcast and storm cloud-sky color blending
│   │   │   ├── WeatherSkyParams.h                  # Storm cloud extinction and diffuse sky lighting
│   │   │   ├── README.md                           # Sky integration documentation
│   │   │   ├── shaders/
│   │   │   │   ├── sky_cloud_blend.comp            # Horizon and cloud boundary blending compute shader
│   │   │   │   └── sky_lut.comp                    # Dynamic sky radiance LUT compute shader
│   │   │   ├── textures/
│   │   │   │   ├── sky_lut_spec.bin                # Active binary transmittance lookup table
│   │   │   │   └── spec/weather_sky_spec.h         # Sky lookup table texture layout specification
│   │   │   └── meshes/spec/weather_sky_mesh_spec.h # Hemispherical sky dome tessellation spec
│   │   │
│   │   ├── snow/                                   # Snowfall & Blizzard Simulation
│   │   │   ├── SnowSystem.h / .cpp                 # Flake dynamics and flutter simulation
│   │   │   ├── Blizzard.cpp                        # High-velocity blizzard wind turbulence
│   │   │   ├── SnowParams.h                        # Flake mass, aerodynamic drag, melt rate
│   │   │   ├── README.md                           # Snow module documentation
│   │   │   ├── shaders/
│   │   │   │   ├── blizzard.comp                   # High-velocity particle drift compute shader
│   │   │   │   └── snow_flurry.comp                # Atmospheric flake flutter compute shader
│   │   │   ├── textures/
│   │   │   │   ├── snowflake_atlas.comp            # Procedural crystalline snowflake atlas generator
│   │   │   │   └── spec/snow_spec.h                # Snowflake sprite texture format specifications
│   │   │   └── meshes/spec/snow_mesh_spec.h        # Snow particle point and billboard mesh formats
│   │   │
│   │   ├── storms/                                 # Heavy Storm Dynamics
│   │   │   ├── StormPreset.h / .cpp                # Storm lifecycle controller
│   │   │   ├── Thunderstorm.cpp                    # Thunder roll delays, pressure drops, flash sequencing
│   │   │   ├── Hail.cpp                            # Hailstone bounce simulation
│   │   │   ├── StormParams.h                       # Barometric pressure, squall velocity, microburst probability
│   │   │   ├── README.md                           # Storms module documentation
│   │   │   ├── shaders/storm_blend.comp            # Multi-layer storm overlay compute shader
│   │   │   ├── textures/spec/storm_spec.h          # Storm sheet noise texture layout
│   │   │   └── meshes/spec/storm_mesh_spec.h       # Storm cloud shelf mesh specifications
│   │   │
│   │   ├── wind/                                   # Global & Local Wind Field
│   │   │   ├── GlobalWind.h / .cpp                 # 3D Perlin curl-noise wind vector field
│   │   │   ├── WindParams.h                        # Gust frequency, azimuth, speed, turbulence scale
│   │   │   ├── README.md                           # Wind module documentation
│   │   │   ├── shaders/wind_global.comp            # 3D vector field evaluation compute shader
│   │   │   ├── textures/spec/wind_spec.h           # Vector field volume texture layout specification
│   │   │   └── meshes/spec/wind_mesh_spec.h        # Wind particle and streamline mesh format specs
│   │   │
│   │   ├── meshes/spec/weather_mesh_spec.h         # Mesh format specs for weather volumes
│   │   ├── textures/
│   │   │   ├── seeds/weather_seed.bin              # Deterministic weather noise seed [Active]
│   │   │   └── spec/weather_common.h               # Common weather texture specifications
│   │   └── shaders/
│   │       ├── include/weather_common.glsl         # Common weather GLSL macros and uniforms
│   │       ├── include/weather_types.glsl          # Std430 data structures for weather shaders
│   │       └── weather_blend.comp                  # Unified weather compositor compute shader
│   │
│   ├── sky/                                        # Atmospheric Sky Model (Preetham / Hosek)
│   │   ├── CMakeLists.txt                          # Builds libsky.a
│   │   ├── atmosphere/
│   │   │   ├── PreethamModel.h / .cpp              # Perez function and daylight luminance calculation
│   │   │   └── textures/sky_lut_spec.bin           # Precomputed transmittance LUT [Active]
│   │   └── sun/
│   │       ├── SunDisk.h / .cpp                    # Direct solar disk rendering, limb darkening, and corona
│   │       └── textures/sun_disk_spec.bin          # Solar spectrum emission curve [Active]
│   │
│   ├── roads/                                      # Procedural Road Networks & 10 Physical States
│   │   ├── CMakeLists.txt                          # Builds libroads.a
│   │   ├── RoadNetwork.h / .cpp                    # Planar graph representation (intersections, nodes, edges)
│   │   ├── RoadSpline.h / .cpp                     # Bicubic Hermite / Catmull-Rom centerline ribbon extrusions
│   │   ├── RoadProfile.h / .cpp                    # Cross-sectional camber, banking angles, curb extrusions
│   │   ├── RoadDecal.h / .cpp                      # Procedural lane markings, stop lines, crosswalks, oil stains
│   │   ├── RoadJunction.h / .cpp                   # 3-way, 4-way, and roundabout junction surface mesh generator
│   │   ├── RoadSign.h / .cpp                       # Vector SDF procedural road sign generator
│   │   ├── RoadStates.h / .cpp                     # 10 Physical road condition generator
│   │   └── shaders/road_surface.comp               # Multi-material road surface blend compute shader
│   │
│   ├── terrain/                                    # Procedural Terrain & Hydraulic Erosion
│   │   ├── CMakeLists.txt                          # Builds libterrain.a
│   │   ├── TerrainGenerator.h / .cpp               # Multi-octave fBm & Voronoi heightfield generation
│   │   ├── HydraulicErosion.h / .cpp               # Droplet sediment transport and deposition simulation
│   │   ├── ThermalErosion.h / .cpp                 # Talus slope rock relaxation simulation
│   │   ├── MarchingCubes.h / .cpp                  # Voxel isosurface extraction for overhanging cliffs
│   │   └── shaders/terrain_gen.comp                # GPU compute shader for real-time heightfield synthesis
│   │
│   ├── water/                                      # Water Bodies & Tessendorf FFT Ocean
│   │   ├── CMakeLists.txt                          # Builds libwater.a
│   │   ├── OceanFFT.h / .cpp                       # Phillips spectrum inverse FFT wave generator
│   │   ├── RiverGenerator.h / .cpp                 # Meandering river spline with flow velocity fields
│   │   ├── WaterCaustics.h / .cpp                  # Procedural caustics texture generator
│   │   └── shaders/ocean_fft.comp                  # Compute shader evaluating choppy wave equations
│   │
│   ├── vegetation/                                 # L-System Flora & GPU Blades
│   │   ├── CMakeLists.txt                          # Builds libvegetation.a
│   │   ├── LSystemTree.h / .cpp                    # Context-free grammar procedural tree branching
│   │   ├── FoliageGenerator.h / .cpp               # Instanced leaf cluster and canopy hull generator
│   │   ├── GrassBlades.h / .cpp                    # GPU bezier curve grass blade generator
│   │   └── shaders/grass_simulate.comp             # Compute shader simulating wind reaction & trampling
│   │
│   ├── buildings/                                  # Grammar Architecture & Modular Facades
│   │   ├── CMakeLists.txt                          # Builds libbuildings.a
│   │   ├── BuildingGenerator.h / .cpp              # Floorplan slicing and vertical elevation stacking
│   │   ├── FacadeGrammar.h / .cpp                  # Grammar-driven window, door, and balcony attachments
│   │   ├── RoofGenerator.h / .cpp                  # Flat, gabled, hipped, and mansard roof geometry
│   │   └── shaders/building_ao.comp                # Ground and facade contact ambient occlusion compute
│   │
│   ├── props/                                      # World Props, Energy & Utility Infrastructure
│   │   ├── CMakeLists.txt                          # Builds libprops.a
│   │   ├── PropGenerator.h / .cpp                  # General procedural prop generator base
│   │   ├── WindTurbineGen.h / .cpp                 # Multi-part wind turbine (tower, nacelle, airfoil blades)
│   │   ├── SolarArrayGen.h / .cpp                  # Photovoltaic panel tracking array generator
│   │   ├── UtilityPoleGen.h / .cpp                 # Wooden / concrete utility poles with catenary wires
│   │   ├── TransformerGen.h / .cpp                 # Distribution transformers and high-voltage switchgear
│   │   └── StreetFurnitureGen.h / .cpp             # Benches, bollards, trash cans, street lamps
│   │
│   ├── vehicles/                                   # Modular Vehicle Assemblies
│   │   ├── CMakeLists.txt                          # Builds libvehicles.a
│   │   ├── CarChassisGen.h / .cpp                  # Procedural sedan, SUV, and truck body frames
│   │   ├── WheelAssemblyGen.h / .cpp               # Rims, brake calipers, and procedural tire treads
│   │   └── InteriorGen.h / .cpp                    # Dashboard, steering wheel, and seating geometry
│   │
│   ├── particles/                                  # GPU Particle Systems
│   │   ├── CMakeLists.txt                          # Builds libparticles.a
│   │   ├── ParticleSystem.h / .cpp                 # Compute-driven particle simulation harness
│   │   ├── CurlNoiseField.h / .cpp                 # 3D divergence-free curl noise vector fields
│   │   └── shaders/particle_sim.comp               # GPU particle physics, collision, and aging
│   │
│   ├── audio/                                      # Procedural Audio Synthesis
│   │   ├── CMakeLists.txt                          # Builds libaudio.a
│   │   ├── EngineSoundGen.h / .cpp                 # Internal combustion engine RPM harmonic synthesizer
│   │   ├── WeatherAcoustics.h / .cpp               # Rain patter, thunder reverberation, wind howling
│   │   └── FootstepGen.h / .cpp                    # Surface-dependent impact sound generator
│   │
│   └── ui/                                         # Viewport Gizmos & In-Engine HUD
│       ├── CMakeLists.txt                          # Builds libui.a
│       ├── TransformGizmo.h / .cpp                 # 3D translation, rotation, scale manipulation handles
│       └── ViewportGrid.h / .cpp                   # Perspective infinite vanishing grid renderer
│
├── shaders/                                        # Root Shared Vulkan GLSL Shaders
│   ├── CMakeLists.txt                              # Compiles shaders to SPIR-V (.spv)
│   ├── README.md                                   # Shader architecture and include guidelines
│   └── glsl/
│       ├── include/                                # Shared GLSL includes
│       │   ├── bindless.glsl                       # Unbounded descriptor array access macros
│       │   ├── math_common.glsl                    # Rotation matrices, projections, constants
│       │   ├── noise_common.glsl                   # Fast hash, simplex, and Worley noise functions
│       │   ├── pbr_common.glsl                     # Cook-Torrance GGX microfacet BRDF
│       │   └── vulkan_compat.glsl                  # Uniform buffer and push constant layouts
│       ├── pbr.vert / .frag                        # Standard deferred/forward PBR pipeline (with PCF shadow mapping)
│       ├── shadow.vert / .frag                     # Offscreen depth pass shaders (SaschaWillems/Vulkan)
│       ├── shadow_cascade.vert                     # Cascaded shadow map (CSM) vertex shader
│       ├── sky.comp                                # Compute shader generating atmospheric scattering LUT
│       └── sky.vert / .frag                        # Sky dome screen-space quad renderer
│
├── meshes/                                         # Shared Mesh Profiles & Specifications
│   ├── profiles/shared_profile.h                   # Shared spline cross-sections
│   └── spec/shared_mesh_spec.h                     # Vertex buffer layouts (positions, normals, UVs, tangents)
│
├── textures/                                       # Shared Textures & Specs
│   └── spec/shared_spec.h                          # BC7/BC5/BC4 texture format choices
│
└── tests/                                          # Native GoogleTest Test Suites
    └── weather/
        ├── test_clouds.cpp                         # Validates cloud density gradients & raymarch step counts
        ├── test_fog.cpp                            # Validates froxel grid extinction math
        ├── test_global_wind.cpp                    # Validates curl-noise divergence-free vectors
        ├── test_lightning.cpp                      # Validates fractal discharge step termination
        ├── test_presets.cpp                        # Validates preset parameter ranges & bounds
        ├── test_puddles.cpp                        # Validates puddle expansion & evaporation
        ├── test_rain.cpp                           # Validates raindrop terminal velocity
        ├── test_snow.cpp                           # Validates snowpack accumulation rates
        ├── test_storms.cpp                         # Validates barometric pressure drops
        └── test_weather_sky.cpp                    # Validates sun extinction through overcast skies
```

---

## What Each Top-Level Folder Does

| Folder | Subsystem Role | Key Dependencies | Primary Output |
| :--- | :--- | :--- | :--- |
| `base/` | Low-level Vulkan 1.3 hardware abstraction, swapchain synchronization, buffer allocation, and validation layer reporting. | Vulkan SDK, VMA, X11/Wayland/Android | `libvulkan_base.a` |
| `src/core/` | Platform macros, high-resolution timers, CPU/GPU profiling, memory alignment wrappers, and assertion logging. | Standard C++20 | `libcore.a` |
| `src/vulkan/` | Vulkan 1.3 dynamic rendering, bindless descriptor indexing heap, fluent pipeline builder, and FrameGraph DAG. | `base/`, Vulkan SDK | `libvulkan_layer.a` |
| `src/scene/` | High-level 3D scene representation: 6-DOF camera controllers, directional/point lights, PBR materials, mesh containers, and scene graphs. | `src/vulkan/`, GLM | `libscene.a` |
| `src/shared/` | Shared mathematical utilities, `IGenerator` interfaces, deterministic multi-stage PRNG seeds, Simplex/Worley noise, and physics abstractions. | `src/core/`, GLM | `libshared.a` |
| `src/weather/` | Atmospheric weather simulator: Nubis-Decima volumetric clouds, froxel fog, dynamic lightning, precipitation, puddle growth, and storm systems. | `src/scene/`, `src/shared/` | `libweather.a` |
| `src/sky/` | Analytical Preetham / Hosek-Wilkie atmospheric sky scattering, solar limb darkening, and solar corona synthesis. | `src/scene/` | `libsky.a` |
| `src/roads/` | Procedural road networks, spline extrusions, cross-sections, procedural decals, traffic signs, and 10 physical road states. | `src/scene/`, `src/shared/` | `libroads.a` |
| `src/terrain/` | Fractal heightfields, droplet hydraulic erosion, thermal relaxation, and Marching Cubes voxel caves. | `src/shared/` | `libterrain.a` |
| `src/water/` | Phillips spectrum FFT ocean waves, meandering river flow velocity splines, caustic generation, and shore foam. | `src/shared/` | `libwater.a` |
| `src/vegetation/` | Lindenmayer grammar trees, instanced leaf clusters, and wind-reactive GPU bezier grass blades. | `src/scene/` | `libvegetation.a` |
| `src/buildings/` | Grammar-driven modular building architecture, floorplan slicing, window/door sockets, and procedural roofs. | `src/scene/` | `libbuildings.a` |
| `src/props/` | Energy infrastructure (wind turbines, solar tracking arrays) and utility systems (poles, catenary wires, transformers). | `src/scene/` | `libprops.a` |
| `src/vehicles/` | Parametric vehicle chassis, rim spokes, brake calipers, and procedural tire treads. | `src/scene/` | `libvehicles.a` |
| `src/particles/` | Compute-driven GPU particle physics, curl-noise vector fields, bitonic sorting, and collision reactions. | `src/vulkan/` | `libparticles.a` |
| `src/audio/` | Real-time procedural audio synthesis for combustion engine RPM harmonics, weather acoustics, and footstep impacts. | Standard C++20 | `libaudio.a` |
| `src/ui/` | Viewport 3D translation/rotation gizmos and infinite perspective grid rendering. | `src/scene/` | `libui.a` |
| `shaders/` | Root GLSL 460 compute and graphics shaders compiled to SPIR-V with bindless descriptors. | Vulkan SDK (`glslangValidator`) | `.spv` SPIR-V binaries |
| `meshes/` | Static vertex layout specifications and spline extrusion cross-sections. | C++20 Headers | Header specs |
| `textures/` | Texture format specifications, BC7/BC5 compression profiles, and mipmap chains. | C++20 Headers | Header specs |
| `tests/` | Comprehensive GoogleTest suite validating physics invariants, numerical precision, and generator ranges. | GoogleTest, CMake | Native test executables |

---

## What Road States Add

The `RoadStates` module generates 10 distinct physical, visual, and tactile road surface conditions:

1. **`Muddy`**:
   - Generates deep longitudinal wheel ruts using continuous displacement tracks.
   - High roughness ($>0.85$) with localized low-roughness water pooling in rut troughs.
   - Splattered mud decals along the road edges and curbs.
2. **`Rocky`**:
   - High-frequency unpaved aggregate with scattered sharp rock meshes embedded in the road bed.
   - High microfacet normal disruption and dust distribution factors.
3. **`Bumpy`**:
   - Washboard sinusoidal undulations causing rhythmic vehicle suspension oscillations.
   - Surface heaving caused by frost heave or root intrusion.
4. **`Sandy`**:
   - Wind-blown transverse ripple dunes encroaching on the road shoulders.
   - High albedo desaturation and loose granular sinkage coefficients.
5. **`Gravel`**:
   - Loose, angular crushed stone distribution with dual-layer normal maps.
   - Dynamic gravel dispersal kickup particles when traversed.
6. **`Snowy`**:
   - Packed wheel ruts surrounded by porous powder snowbanks.
   - Temperature-dependent road salt melting streaks and white albedo overlays.
7. **`Icy`**:
   - Glazed black ice specular reflection with near-zero roughness ($<0.08$).
   - Micro-fissures and translucent ice refraction over underlying asphalt cracks.
8. **`Wet`**:
   - Mirror-like water film with Fresnel reflectance boost.
   - Subtle tire spray wake generation and blurred environment reflections.
9. **`Dry Cracked`**:
   - Desiccation thermal contraction cracks modeled via Voronoi edge graph relaxation.
   - Tar-sealed repair lines and asphalt joint degradation.
10. **`Potholed`**:
    - Severe structural cavitation exposing the coarse aggregate sub-base layer.
    - Fractured, jagged asphalt lips with standing rainwater pooling inside deep cavities.

---

## What Energy / Utility Props Add

The `props/` subsystem introduces critical infrastructure assets required for procedural realism in modern and industrial landscapes:

### Renewable Energy Assets
- **Wind Turbines (`WindTurbineGen`)**:
  - Parametric hollow steel tubular towers with access doors, climbing rungs, and aviation warning beacons.
  - Aerodynamic nacelle housing simulated generator gearbox components.
  - Three-blade rotor assembly with true aerodynamic airfoil cross-sections, variable pitch angles, and continuous rotational simulation based on the active `GlobalWind` vector field.
- **Solar Tracking Arrays (`SolarArrayGen`)**:
  - Ground-mounted photovoltaic panel banks with anti-reflective glass coatings and silicon cell grid lines.
  - Dual-axis or single-axis mechanical tracking hinges that compute optimal inclination angles relative to the procedural sun vector.

### Electrical & Utility Distribution
- **Utility Poles (`UtilityPoleGen`)**:
  - Weathered treated wooden and reinforced concrete transmission poles.
  - Crossarms with ceramic / polymer pin-type electrical insulators.
  - Real-time catenary curve physics modeling wire sag, wind oscillation, and tension across arbitrary pole spans.
  - Down-guy wires with yellow safety markers and grounding rods.
- **Distribution Transformers & Substations (`TransformerGen`)**:
  - Pole-mounted cylindrical single-phase distribution transformers with high-voltage bushings.
  - Pad-mounted commercial transformer green enclosures with warning hazard decals.
  - High-voltage substation switchgear, busbars, and lightning surge arresters.

---

## Generated Assets & Implementation Status Registry

The following table records the deterministic binary assets, precomputed lookup tables (LUTs), topological graphs, and vertex specifications managed within `native_assets/`:

### 1. Deterministic Noise & PRNG Seed Blocks
| Asset Path | Size / Dimensions | Generator Source | Functional Role | Status |
| :--- | :---: | :--- | :--- | :---: |
| `src/weather/textures/seeds/weather_seed.bin` | 1024 bytes | `tools/generate_weather_and_sky.py` | Cryptographically uniform PRNG seeds for weather system turbulence | **Active** |
| `src/shared/noise/blue_noise_128.bin` | $128 \times 128 \times 4$ | Void-and-Cluster Generator | High-frequency blue noise for raymarch sample jittering | **Active** |

### 2. Radiance, Transmittance & Tonemapping LUTs
| Asset Path | Format / Size | Generation Target | Mathematical Basis | Status |
| :--- | :---: | :--- | :--- | :---: |
| `src/sky/atmosphere/textures/sky_lut_spec.bin` | RGBA16F / $256 \times 64$ | `src/sky/atmosphere/PreethamModel.cpp` | Multi-scattering Rayleigh/Mie optical depth lookup | **Active** |
| `src/sky/sun/textures/sun_disk_spec.bin` | R16F / $512 \times 1$ | `src/sky/sun/SunDisk.cpp` | Solar limb darkening polynomial emission profile | **Active** |
| `src/weather/sky/textures/sky_lut_spec.bin` | RGBA16F / $128 \times 32$ | `src/weather/sky/SkyGenerator.cpp` | Storm cloud overcast horizon extinction LUT | **Active** |

### 3. Core Native Generator Implementations (49 Components)
| # | Component / Asset Class | Subsystem | Header & Source | Shaders / Data Specs | Status |
| :---: | :--- | :--- | :--- | :--- | :---: |
| 1 | `CloudGen` | Weather | `CloudGen.h / .cpp` | `cloud_shape.comp`, `cloud_noise.comp` | **Active** |
| 2 | `CloudRaymarch` | Weather | `CloudRaymarch.h / .cpp` | `cloud_raymarch.comp` | **Active** |
| 3 | `FogVolume` | Weather | `FogVolume.h / .cpp` | `fog_froxel.comp`, `fog_ground.comp` | **Active** |
| 4 | `LightningGen` | Weather | `LightningGen.h / .cpp` | `bolt_path.comp` | **Active** |
| 5 | `WeatherPreset` | Weather | `WeatherPreset.h / .cpp` | `preset_blend.comp` | **Active** |
| 6 | `PuddleMask` | Weather | `PuddleMask.cpp` | `puddle_mask.comp`, `wetness.comp` | **Active** |
| 7 | `WetnessMap` | Weather | `WetnessMap.h / .cpp` | `wetness.comp` | **Active** |
| 8 | `RainSystem` | Weather | `RainSystem.h / .cpp` | `rain_particle.comp`, `rain_streak.comp` | **Active** |
| 9 | `RainSplash` | Weather | `RainSplash.h / .cpp` | `rain_splash.comp` | **Active** |
| 10 | `SkyGenerator` | Weather | `SkyGenerator.h / .cpp` | `sky_cloud_blend.comp`, `sky_lut.comp` | **Active** |
| 11 | `SnowSystem` | Weather | `SnowSystem.h / .cpp` | `snow_flurry.comp` | **Active** |
| 12 | `Blizzard` | Weather | `Blizzard.cpp` | `blizzard.comp` | **Active** |
| 13 | `StormPreset` | Weather | `StormPreset.h / .cpp` | `storm_blend.comp` | **Active** |
| 14 | `Thunderstorm` | Weather | `Thunderstorm.cpp` | `storm_blend.comp` | **Active** |
| 15 | `Hail` | Weather | `Hail.cpp` | `storm_blend.comp` | **Active** |
| 16 | `GlobalWind` | Weather | `GlobalWind.h / .cpp` | `wind_global.comp` | **Active** |
| 17 | `PreethamModel` | Sky | `PreethamModel.h / .cpp` | `sky.comp` | **Active** |
| 18 | `SunDisk` | Sky | `SunDisk.h / .cpp` | `sky.frag` | **Active** |
| 19 | `RoadNetwork` | Roads | `RoadNetwork.h / .cpp` | `road_surface.comp` | **Active** |
| 20 | `RoadSpline` | Roads | `RoadSpline.h / .cpp` | Spline Bezier/Hermite | **Active** |
| 21 | `RoadProfile` | Roads | `RoadProfile.h / .cpp` | Profiles Spec | **Active** |
| 22 | `RoadDecal` | Roads | `RoadDecal.h / .cpp` | Procedural Stencils | **Active** |
| 23 | `RoadJunction` | Roads | `RoadJunction.h / .cpp` | Planar Mesh Intersections | **Active** |
| 24 | `RoadSign` | Roads | `RoadSign.h / .cpp` | SDF Vector Glyphs | **Active** |
| 25 | `RoadStates` | Roads | `RoadStates.h / .cpp` | 10 Physical Condition Shaders | **Active** |
| 26 | `TerrainGenerator` | Terrain | `TerrainGenerator.h / .cpp` | `terrain_gen.comp` | **Active** |
| 27 | `HydraulicErosion` | Terrain | `HydraulicErosion.h / .cpp` | Particle Sediment Simulation | **Active** |
| 28 | `ThermalErosion` | Terrain | `ThermalErosion.h / .cpp` | Talus Slope Relaxation | **Active** |
| 29 | `MarchingCubes` | Terrain | `MarchingCubes.h / .cpp` | Isosurface Extraction Table | **Active** |
| 30 | `OceanFFT` | Water | `OceanFFT.h / .cpp` | `ocean_fft.comp` | **Active** |
| 31 | `RiverGenerator` | Water | `RiverGenerator.h / .cpp` | Meandering Spline ribbons | **Active** |
| 32 | `WaterCaustics` | Water | `WaterCaustics.h / .cpp` | Voronoi Caustic Generator | **Active** |
| 33 | `LSystemTree` | Vegetation | `LSystemTree.h / .cpp` | Context-Free Branching Grammar | **Active** |
| 34 | `FoliageGenerator` | Vegetation | `FoliageGenerator.h / .cpp` | Instanced Canopy Billboard Impostors | **Active** |
| 35 | `GrassBlades` | Vegetation | `GrassBlades.h / .cpp` | `grass_simulate.comp` | **Active** |
| 36 | `BuildingGenerator`| Buildings | `BuildingGenerator.h / .cpp`| Floorplan Slicing & Elevation | **Active** |
| 37 | `FacadeGrammar` | Buildings | `FacadeGrammar.h / .cpp` | Socket Grammar Attachment | **Active** |
| 38 | `RoofGenerator` | Buildings | `RoofGenerator.h / .cpp` | Hipped, Gabled, Flat Geometry | **Active** |
| 39 | `WindTurbineGen` | Props | `WindTurbineGen.h / .cpp` | Nacelle, Tower & Airfoil Blades | **Active** |
| 40 | `SolarArrayGen` | Props | `SolarArrayGen.h / .cpp` | Sun-Tracking Panel Arrays | **Active** |
| 41 | `UtilityPoleGen` | Props | `UtilityPoleGen.h / .cpp` | Catenary Wire Physics Curves | **Active** |
| 42 | `TransformerGen` | Props | `TransformerGen.h / .cpp` | High-Voltage Bushings & Housings | **Active** |
| 43 | `StreetFurniture`| Props | `StreetFurnitureGen.h / .cpp`| Benches, Bollards, Street Lamps | **Active** |
| 44 | `CarChassisGen` | Vehicles | `CarChassisGen.h / .cpp` | Parametric Vehicle Body Frames | **Active** |
| 45 | `WheelAssemblyGen`| Vehicles | `WheelAssemblyGen.h / .cpp` | Rims, Calipers & Tire Treads | **Active** |
| 46 | `InteriorGen` | Vehicles | `InteriorGen.h / .cpp` | Dashboards, Seating, Steering | **Active** |
| 47 | `ParticleSystem` | Particles | `ParticleSystem.h / .cpp` | `particle_sim.comp` | **Active** |
| 48 | `EngineSoundGen` | Audio | `EngineSoundGen.h / .cpp` | Harmonics & Cylinder Firing | **Active** |
| 49 | `WeatherAcoustics`| Audio | `WeatherAcoustics.h / .cpp` | Rain Acoustics, Thunder Delay | **Active** |
