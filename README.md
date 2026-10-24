# Sphere Studio — Comprehensive Procedural 3D World Engine & Multiplatform Studio

[![Android Build](https://img.shields.io/badge/Android-Gradle%208.7-3DDC84.svg?logo=android&logoColor=white)](https://developer.android.com/)
[![Kotlin Multiplatform](https://img.shields.io/badge/Kotlin%20Multiplatform-2.0.0-7F52FF.svg?logo=kotlin&logoColor=white)](https://www.jetbrains.com/kotlin-multiplatform/)
[![Jetpack Compose](https://img.shields.io/badge/Jetpack%20Compose-M3%201.3.0-4285F4.svg?logo=jetpackcompose&logoColor=white)](https://developer.android.com/jetpack/compose)
[![Vulkan](https://img.shields.io/badge/Vulkan-1.3%20Compute%20%26%20Graphics-ED1B24.svg?logo=vulkan&logoColor=white)](https://www.vulkan.org/)
[![C++](https://img.shields.io/badge/C%2B%2B-20%20Standards%20Compliant-00599C.svg?logo=c%2B%2B&logoColor=white)](https://en.cppreference.com/w/cpp/20)

**Sphere Studio** is a real-time procedural 3D asset generation engine, world simulator, and multiplatform CAD/viewport studio. It combines high-performance **C++20 and Vulkan 1.3 compute pipelines** with a modern **Kotlin Multiplatform (KMP) & Jetpack Compose** interface. The platform generates, simulates, inspects, and visualizes everything needed for procedural open-world rendering: atmospheric scattering, photorealistic volumetric cloudscapes, road networks with 10 physical surface states, modular architecture, L-system vegetation, FFT ocean bodies, hydraulic erosion terrain, procedural audio, and customizable 3D stage platforms.

---

## Table of Contents

1. [Executive Summary & Core Highlights](#executive-summary--core-highlights)
2. [Full Project Directory Tree](#full-project-directory-tree)
3. [Subsystem Architecture & Capabilities](#subsystem-architecture--capabilities)
   - [3D Viewport & Surface Scene Modes](#1-3d-viewport--surface-scene-modes)
   - [Volumetric Clouds Subsystem (Robert Beckebans Nubis-Decima Port)](#2-volumetric-clouds-subsystem-robert-beckebans-nubis-decima-port)
   - [Atmospheric Sky & Solar Corona (Preetham / Hosek-Wilkie Model)](#3-atmospheric-sky--solar-corona-preetham--hosek-wilkie-model)
   - [Road Networks & 10 Physical Surface States](#4-road-networks--10-physical-surface-states)
   - [Vegetation & Biomes (L-System & GPU Blades)](#5-vegetation--biomes-l-system--gpu-blades)
   - [PBR Materials, Buildings & Modular Facades](#6-pbr-materials-buildings--modular-facades)
   - [Terrain & Hydraulic Erosion](#7-terrain--hydraulic-erosion)
   - [Water Bodies & Tessendorf FFT Ocean](#8-water-bodies--tessendorf-fft-ocean)
   - [Energy, Utility & World Props](#9-energy-utility--world-props)
   - [GPU Particles & Procedural Audio](#10-gpu-particles--procedural-audio)
4. [Generated Assets & Binary Manifest Registry](#generated-assets--binary-manifest-registry)
5. [Build, Test & Deployment Guide](#build-test--deployment-guide)

---

## Executive Summary & Core Highlights

- **Dual-Engine Architecture**:
  - **Native C++20 / Vulkan 1.3 Core (`native_assets/`)**: Ported and adapted from [SaschaWillems/Vulkan](https://github.com/SaschaWillems/Vulkan), [kentril0/WaterSurfaceRendering](https://github.com/kentril0/WaterSurfaceRendering), and [RobertBeckebans/OpenGL-VolumetricClouds](https://github.com/RobertBeckebans/OpenGL-VolumetricClouds). Employs `VK_EXT_descriptor_indexing` bindless heaps, dynamic rendering, compute shaders, and memory-mapped staging pools.
  - **Kotlin Multiplatform Studio (`kmp/` + `app/`)**: Responsive UI built with Material Design 3 (M3), featuring real-time 3D software rasterization/raymarching Canvas viewports, orbit camera controllers, full procedural parameter editors, and multiplatform support for Android, Desktop (JVM), and iOS.
- **Robert Beckebans Volumetric Clouds**:
  - Replaced outdated 2D billboard strips with full 3D perspective raymarched cloudscapes inside a curved planetary atmosphere shell ($R_{inner}=705\text{km}$, $R_{outer}=722\text{km}$).
  - Features Beer's law extinction, Beer-Powder sugar scattering, dual-lobe Henyey-Greenstein forward silver lining, volumetric crepuscular light shafts (god rays), and high-altitude cirrus ice crystal layers.
- **5 Surface Scene Modes & Relative Sizing**:
  - **Endless Ground (`ENDLESS`)**: Infinite horizon ground plane with perspective vanishing grid lines and moving cloud shadows.
  - **No Surface / Void (`NONE`)**: Complete floating freedom in 3D sky/space with zero horizon cutoff for flight simulators and space inspection.
  - **Flat Piece (`FLAT_PIECE`)**: Bounded, adjustable ground slab (10m × 10m default). Automatically adjusts to **3m × 3m** for small ~1m items, with custom dimension sliders (1m to 50m) and 1-meter perspective grid lines.
  - **Water Piece (`WATER_BODY`)**: 10m × 10m default bounded water basin with animated wave ripples, perimeter foam, and aquatic depth gradient.
  - **Broken Slab (`BROKEN_SLAB`)**: Fractured floating rock diorama piece with chiseled cliff facets, rock strata, and hovering rock fragments.

---

## Full Project Directory Tree

Every file in the repository is documented below with its role and architectural function:

```
sphere-studio/
├── .env.example                                            # Template for API keys and environment variables
├── .gitignore                                              # Git ignore rules for build caches, binaries, and IDE files
├── build.gradle.kts                                        # Root Gradle build script configuring plugins and repositories
├── gradle.properties                                       # JVM memory, AndroidX, and Kotlin compiler flags
├── metadata.json                                           # AI Studio platform application metadata and identity
├── README.md                                               # Master project documentation (this document)
├── settings.gradle.kts                                     # Multi-project module configuration (app, kmp)
│
├── gradle/                                                 # Gradle Wrapper & Dependency Catalog
│   ├── libs.versions.toml                                  # Centralized version catalog for AndroidX, Compose, Coroutines
│   └── wrapper/
│       └── gradle-wrapper.properties                       # Gradle 8.7 distribution specification
│
├── app/                                                    # Android Application Module
│   ├── build.gradle.kts                                    # Android application build configuration and dependencies
│   ├── proguard-rules.pro                                  # ProGuard / R8 code shrinking and obfuscation rules
│   └── src/
│       ├── main/
│       │   ├── AndroidManifest.xml                         # Android manifest: permissions, activities, orientations
│       │   ├── java/com/example/
│       │   │   ├── MainActivity.kt                         # Main Activity host: initializes enableEdgeToEdge & Compose UI
│       │   │   └── ui/theme/
│       │   │       ├── Color.kt                            # Material 3 color palettes (Dark, Light, Brand Accents)
│       │   │       ├── Theme.kt                            # App-wide Material 3 theme wrapper and dynamic color support
│       │   │       └── Type.kt                             # Material 3 typography definitions
│       │   └── res/
│       │       ├── drawable/                               # Adaptive icon vectors (background, foreground)
│       │       ├── mipmap-anydpi-v26/                      # Adaptive icon manifests (ic_launcher, ic_launcher_round)
│       │       ├── mipmap-hdpi/ .. xxxhdpi/                # Raster launcher icons across standard device densities
│       │       ├── values/
│       │       │   ├── colors.xml                          # XML color definitions
│       │       │   ├── strings.xml                         # Strings catalog (app_name: "Sphere Studio")
│       │       │   └── themes.xml                          # Base Android system themes
│       │       └── xml/
│       │           ├── backup_rules.xml                    # Android 12+ backup configuration
│       │           └── data_extraction_rules.xml           # Android 12+ data extraction configuration
│       │
│       ├── androidTest/java/com/example/
│       │   └── ExampleInstrumentedTest.kt                  # Instrumented test stub
│       └── test/java/com/example/
│           ├── ExampleRobolectricTest.kt                   # Robolectric JVM unit tests validating CUJs
│           └── ExampleUnitTest.kt                          # Standard JUnit4 JVM tests
│
├── kmp/                                                    # Kotlin Multiplatform (KMP) Studio Module
│   ├── build.gradle.kts                                    # KMP build script: targets Android, JVM Desktop, and iOS
│   ├── README.md                                           # KMP module overview and architecture documentation
│   └── src/
│       ├── commonMain/kotlin/com/example/sphere/
│       │   ├── Platform.kt                                 # Multiplatform expect declarations
│       │   ├── SphereStudioApp.kt                          # Master Application Shell:
│       │   │                                               #   - 3D Calibration Viewport with Orbit Camera
│       │   │                                               #   - 5 Surface Scene Modes (Endless, Void, Flat, Water, Broken)
│       │   │                                               #   - Responsive relative sizing (10x10m, 3x3m, Auto-Fit)
│       │   │                                               #   - Dimensional sliders (Width, Height, Depth, Scale)
│       │   │                                               #   - Preetham / Hosek-Wilkie atmospheric sky dome & sun disk
│       │   │                                               #   - PBR Procedural Surface Patterns (Solid, Grid, Checker, Ripple, Hex)
│       │   │                                               #   - Native Engine Architecture inspector & 13 category navigation
│       │   ├── WeatherStudioScreen.kt                      # Volumetric Weather & Cloud Simulator:
│       │   │                                               #   - ProceduralCloudSynthesizer (Robert Beckebans Nubis-Decima port)
│       │   │                                               #   - True 3D perspective raymarching through planetary shell
│       │   │                                               #   - Henyey-Greenstein forward solar silver lining
│       │   │                                               #   - Crepuscular light rays (god rays) streaming through cloud gaps
│       │   │                                               #   - High-altitude wispy cirrus ice crystal layer
│       │   │                                               #   - Dynamic lightning bolt generation & thunder flashes
│       │   │                                               #   - Rain streaks, ground splash impacts & puddle accumulation
│       │   │                                               #   - Snow flutter, blizzard drifts & ground snowpack
│       │   │                                               #   - 5 Surface Scene Modes in weather environment
│       │   │                                               #   - 8 dynamic weather presets (Clear, Overcast, Rain, Downpour, etc.)
│       │   ├── SkyStudioScreen.kt                          # Atmospheric Sky & Solar Simulator:
│       │   │                                               #   - Solar elevation, azimuth, and time-of-day presets
│       │   │                                               #   - Aerosol turbidity, ozone absorption & Perez parameters
│       │   │                                               #   - ACES and Reinhard tone-mapping preview
│       │   ├── SphereStudioWelcomeScreen.kt                # Landing hub & onboarding showcase
│       │   └── SurfaceSceneMode.kt                         # Surface Scene Mode enum (ENDLESS, NONE, FLAT_PIECE, WATER_BODY, BROKEN_SLAB)
│       │
│       ├── androidMain/kotlin/com/example/sphere/
│       │   └── Platform.android.kt                         # Android-specific actual platform bindings
│       ├── desktopMain/kotlin/com/example/sphere/
│       │   └── Platform.desktop.kt                         # JVM desktop-specific actual platform bindings
│       └── iosMain/kotlin/com/example/sphere/
│           └── Platform.ios.kt                             # iOS-specific actual platform bindings
│
├── native_assets/                                          # Native C++20 / Vulkan 1.3 Procedural Engine Workspace
│   ├── CMakeLists.txt                                      # Root CMake configuration compiling base, core, scene, shaders, tests
│   ├── main.cpp                                            # Native headless / benchmark CLI application entry point
│   ├── README.md                                           # Subsystem breakdown and architecture documentation
│   │
│   ├── base/                                               # Vulkan 1.3 Base Primitives (ported from SaschaWillems/Vulkan)
│   │   ├── CMakeLists.txt                                  # Builds libbase.a
│   │   ├── Entrypoints.h                                   # Function pointer loaders (vkGetInstanceProcAddr)
│   │   ├── VulkanBuffer.hpp / .cpp                         # Host-visible, device-local, and staging buffer management
│   │   ├── VulkanDebug.hpp / .cpp                          # Validation layer messenger and debug labels
│   │   ├── VulkanDevice.hpp / .cpp                         # Physical device querying, logical device, queue families
│   │   ├── VulkanInitializers.hpp                          # Struct initializer helper functions
│   │   ├── VulkanSwapChain.hpp / .cpp                      # Surface formats, present modes, swapchain image views
│   │   ├── VulkanTexture.hpp / .cpp                        # VkImage, layout barriers, samplers, mipmap generators
│   │   └── VulkanTools.hpp / .cpp                          # Error parsing, format checkers, file loaders
│   │
│   ├── src/                                                # Engine Core and Subsystem Modules
│   │   ├── core/                                           # Core Lifecycle & Utilities (kentril0 src/core/ pattern)
│   │   │   ├── CMakeLists.txt                              # Builds libcore.a
│   │   │   ├── Application.h / .cpp                        # Engine application lifecycle, main loop, event pumping
│   │   │   ├── Assert.h                                    # ASSERT and VERIFY diagnostic macros
│   │   │   ├── Base.h                                      # Platform architecture definitions and common types
│   │   │   ├── Log.h / .cpp                                # Formatted logging system (Debug, Info, Warn, Error)
│   │   │   ├── Profile.h / .cpp                            # High-resolution CPU/GPU microsecond scope profiler
│   │   │   ├── Timer.h                                     # High-precision monotonic timer
│   │   │   └── Timestep.h                                  # Frame delta-time wrapper
│   │   │
│   │   ├── vulkan/                                         # Modern Vulkan 1.3 Wrappers (kentril0 src/vulkan/ pattern)
│   │   │   ├── CMakeLists.txt                              # Builds libvulkan_wrap.a
│   │   │   ├── BindlessHeap.h / .cpp                       # Unbounded descriptor indexing (VK_EXT_descriptor_indexing)
│   │   │   ├── Buffer.h / .cpp                             # RAII VkBuffer with memory barrier helpers
│   │   │   ├── CommandBuffer.h / .cpp                      # Scoped command buffer recording and submission
│   │   │   ├── CommandPool.h / .cpp                        # Thread-safe command pool allocation
│   │   │   ├── Descriptors.h / .cpp                        # Descriptor set layout cache and pool allocator
│   │   │   ├── Device.h / .cpp                             # Device management and queue synchronizations
│   │   │   ├── FrameGraph.h / .cpp                         # Render-graph dependency pass scheduler
│   │   │   ├── Pipeline.h / .cpp                           # Compute and graphics pipeline abstraction
│   │   │   ├── PipelineBuilder.h / .cpp                    # Fluent builder for Vulkan 1.3 Dynamic Rendering pipelines
│   │   │   ├── RenderPass.h / .cpp                         # Subpass dependencies and attachment configurations
│   │   │   ├── Shader.h / .cpp                             # SPIR-V binary module compiler and reflection
│   │   │   ├── SwapChain.h / .cpp                          # Presentation engine wrapper
│   │   │   └── Texture.h / .cpp                            # RAII texture with anisotropic filtering samplers
│   │   │
│   │   ├── scene/                                          # Scene Graph & World Representation
│   │   │   ├── CMakeLists.txt                              # Builds libscene.a
│   │   │   ├── AABB.h / .cpp                               # Axis-aligned bounding box frustum culling
│   │   │   ├── Camera.h / .cpp                             # Perspective & orthographic projection matrices
│   │   │   ├── CameraController.h / .cpp                   # Orbit, fly, and walk camera controllers
│   │   │   ├── Light.h / .cpp                              # Directional, point, and spot light sources
│   │   │   ├── Material.h / .cpp                           # PBR material descriptors (albedo, normal, roughness, metallic)
│   │   │   ├── MaterialRegistry.h / .cpp                   # Global material table with bindless texture indices
│   │   │   ├── Mesh.h / .cpp                               # Vertex and index buffer abstractions with GPU uploads
│   │   │   ├── Meshlet.h / .cpp                            # Meshlet clustering for VK_EXT_mesh_shader pipelines
│   │   │   ├── Scene.h / .cpp                              # Common Scene root container
│   │   │   ├── SceneNode.h / .cpp                          # Transform hierarchy and component attachments
│   │   │   ├── SkyModel.h / .cpp                           # Sky model hook (Preetham / Hosek atmospheric evaluation)
│   │   │   ├── Transform.h / .cpp                          # TRS matrices with cached world transforms
│   │   │   └── WorldState.h                                # Global time, solar vector, wind field, and seed state
│   │   │
│   │   ├── shared/                                         # Cross-Subsystem Common Interfaces & Math
│   │   │   ├── CMakeLists.txt                              # Builds libshared.a
│   │   │   ├── IGenerator.h                                # Standard procedural generator lifecycle interface
│   │   │   ├── GeneratorContext.h / .cpp                   # Context holding Vulkan device, heap, and PRNG seeds
│   │   │   ├── GeneratorRegistry.h / .cpp                  # Subsystem service locator and factory registry
│   │   │   ├── GpuTypes.h / .cpp                           # GPU std430 struct alignments and push constants
│   │   │   ├── Seed.h / .cpp                               # Deterministic multi-stage PRNG seed mixers
│   │   │   ├── bindless/                                   # Shared bindless handles
│   │   │   ├── debug/                                      # Debug line, box, and frustum drawing
│   │   │   ├── math/                                       # Frustum culling, Hi-Z occlusion, spline math
│   │   │   ├── memory/                                     # Dynamic staging buffer pools
│   │   │   ├── noise/                                      # Simplex, Perlin, Worley, and Blue Noise generators
│   │   │   ├── physics/                                    # Collision groups, raycasting, and physics debug draw
│   │   │   └── skeletons/                                  # Procedural rigging and socket attachment hierarchies
│   │   │
│   │   ├── weather/                                        # Volumetric Weather Systems
│   │   │   ├── CMakeLists.txt                              # Builds libweather.a
│   │   │   ├── README.md                                   # Weather subsystem architecture documentation
│   │   │   ├── WeatherState.h                              # Live weather conditions (rain, snow, wind, cloud coverage)
│   │   │   ├── WeatherSystem.h / .cpp                      # Master weather coordinator
│   │   │   ├── clouds/                                     # Nubis-Decima Volumetric Clouds (Robert Beckebans port)
│   │   │   │   ├── CloudGen.h / .cpp                       # Cloud shape volume synthesizer
│   │   │   │   ├── CloudParams.h                           # Radiative transfer, Henyey-Greenstein, and altitude limits
│   │   │   │   ├── CloudRaymarch.h / .cpp                  # Raymarcher implementation through curved planetary shell
│   │   │   │   ├── README.md                               # Deep dive on Nubis radiative transfer & cone sampling
│   │   │   │   ├── shaders/
│   │   │   │   │   ├── cloud_noise.comp                    # 3D Perlin-Worley noise generation
│   │   │   │   │   ├── cloud_shape.comp                    # Low-frequency cloud coverage mask compute
│   │   │   │   │   └── cloud_raymarch.comp                 # Full Vulkan compute shader for perspective raymarching
│   │   │   │   ├── textures/                               # Cloud 3D texture generators & format specs
│   │   │   │   └── meshes/                                 # Sky dome mesh specifications
│   │   │   ├── fog/                                        # Volumetric Froxel Fog & Ground Mist
│   │   │   │   ├── FogVolume.h / .cpp                      # 3D froxel grid allocation & light scattering
│   │   │   │   ├── FogParams.h                             # Extinction, scattering coefficients, height falloff
│   │   │   │   ├── README.md                               # Fog module documentation
│   │   │   │   └── shaders/fog_froxel.comp, fog_ground.comp
│   │   │   ├── lightning/                                  # Dynamic Lightning Generation
│   │   │   │   ├── FlashCurve.h                            # Temporal discharge curve
│   │   │   │   ├── LightningGen.h / .cpp                   # Recursive fractal discharge branch generator
│   │   │   │   ├── LightningParams.h                       # Branch probability, fork angle, leader step
│   │   │   │   ├── README.md                               # Lightning module documentation
│   │   │   │   └── shaders/bolt_path.comp
│   │   │   ├── presets/                                    # Weather Condition Presets
│   │   │   │   ├── PresetDefaults.h                        # Constexpr parameter tables
│   │   │   │   ├── WeatherPreset.h / .cpp                  # Preset interpolation engine
│   │   │   │   ├── WeatherClear.cpp                        # Clear sky with high visibility
│   │   │   │   ├── WeatherRain.cpp                         # Moderate precipitation with road wetness
│   │   │   │   ├── WeatherSnow.cpp                         # Snowfall with accumulating snowpack
│   │   │   │   ├── WeatherStorm.cpp                        # Downpour, gale winds, lightning
│   │   │   │   ├── README.md                               # Preset module documentation
│   │   │   │   └── shaders/preset_blend.comp
│   │   │   ├── puddles/                                    # Dynamic Puddle Accumulation & Ground Wetness
│   │   │   │   ├── PuddleMask.cpp, PuddleParams.h          # Puddle growth modeling
│   │   │   │   ├── WetnessMap.h / .cpp                     # Wetness evaporation and capillary action
│   │   │   │   ├── README.md                               # Puddles module documentation
│   │   │   │   └── shaders/puddle_mask.comp, wetness.comp
│   │   │   ├── rain/                                       # Precipitation Particle Simulation
│   │   │   │   ├── RainSystem.h / .cpp                     # GPU particle emitter for raindrops
│   │   │   │   ├── RainSplash.h / .cpp                     # Ground contact splash rings
│   │   │   │   ├── RainParams.h                            # Terminal velocity, drop size, slant angle
│   │   │   │   ├── README.md                               # Rain module documentation
│   │   │   │   └── shaders/rain_particle.comp, rain_splash.comp, rain_streak.comp
│   │   │   ├── sky/                                        # Weather Sky Coupling
│   │   │   │   ├── SkyGenerator.h / .cpp                   # Overcast and storm cloud-sky color blending
│   │   │   │   ├── WeatherSkyParams.h                      # Storm cloud extinction and diffuse sky lighting
│   │   │   │   ├── README.md                               # Sky integration documentation
│   │   │   │   └── shaders/sky_cloud_blend.comp, sky_lut.comp
│   │   │   ├── snow/                                       # Snowfall & Blizzard Simulation
│   │   │   │   ├── SnowSystem.h / .cpp                     # Flake dynamics and flutter simulation
│   │   │   │   ├── Blizzard.cpp, SnowParams.h              # High-velocity blizzard wind turbulence
│   │   │   │   ├── README.md                               # Snow module documentation
│   │   │   │   └── shaders/blizzard.comp, snow_flurry.comp
│   │   │   ├── storms/                                     # Heavy Storm Dynamics
│   │   │   │   ├── StormPreset.h / .cpp                    # Storm lifecycle controller
│   │   │   │   ├── Thunderstorm.cpp, Hail.cpp              # Hailstone bounce simulation
│   │   │   │   ├── StormParams.h                           # Barometric pressure, squall velocity
│   │   │   │   ├── README.md                               # Storms module documentation
│   │   │   │   └── shaders/storm_blend.comp
│   │   │   ├── wind/                                       # Global & Local Wind Field
│   │   │   │   ├── GlobalWind.h / .cpp                     # 3D Perlin curl-noise wind vector field
│   │   │   │   ├── WindParams.h                            # Gust frequency, azimuth, speed
│   │   │   │   ├── README.md                               # Wind module documentation
│   │   │   │   └── shaders/wind_global.comp
│   │   │   ├── meshes/spec/weather_mesh_spec.h             # Mesh format specs for weather volumes
│   │   │   ├── textures/seeds/weather_seed.bin             # Deterministic weather noise seed [Active]
│   │   │   └── shaders/                                    # Shared weather compute shaders
│   │   │       ├── include/weather_common.glsl, weather_types.glsl
│   │   │       └── weather_blend.comp
│   │   │
│   │   └── sky/                                            # Atmospheric Sky Model (Preetham / Hosek)
│   │       ├── CMakeLists.txt                              # Builds libsky.a
│   │       ├── atmosphere/
│   │       │   ├── PreethamModel.h / .cpp                  # Perez function and daylight luminance calculation
│   │       │   └── textures/sky_lut_spec.bin               # Precomputed transmittance LUT [Active]
│   │       └── sun/
│   │           ├── SunDisk.h / .cpp                        # Direct solar disk rendering, limb darkening, and corona
│   │           └── textures/sun_disk_spec.bin              # Solar spectrum emission curve [Active]
│   │
│   ├── shaders/                                            # Root Shared Vulkan GLSL Shaders
│   │   ├── CMakeLists.txt                                  # Compiles shaders to SPIR-V (.spv)
│   │   ├── README.md                                       # Shader architecture and include guidelines
│   │   └── glsl/
│   │       ├── include/                                    # Shared GLSL includes
│   │       │   ├── bindless.glsl                           # Unbounded descriptor array access macros
│   │       │   ├── math_common.glsl                        # Rotation matrices, projections, constants
│   │       │   ├── noise_common.glsl                       # Fast hash, simplex, and Worley noise functions
│   │       │   ├── pbr_common.glsl                         # Cook-Torrance GGX microfacet BRDF
│   │       │   └── vulkan_compat.glsl                      # Uniform buffer and push constant layouts
│   │       ├── pbr.vert / .frag                            # Standard deferred/forward PBR pipeline
│   │       ├── sky.comp                                    # Compute shader generating atmospheric scattering LUT
│   │       └── sky.vert / .frag                            # Sky dome screen-space quad renderer
│   │
│   ├── meshes/                                             # Shared Mesh Profiles & Specifications
│   │   ├── profiles/shared_profile.h                       # Shared spline cross-sections
│   │   └── spec/shared_mesh_spec.h                         # Vertex buffer layouts (positions, normals, UVs, tangents)
│   │
│   ├── textures/                                           # Shared Textures & Specs
│   │   └── spec/shared_spec.h                              # BC7/BC5/BC4 texture format choices
│   │
│   └── tests/                                              # Native GoogleTest Test Suites
│       └── weather/
│           ├── test_clouds.cpp                             # Validates cloud density gradients & Raymarch step counts
│           ├── test_fog.cpp                                # Validates froxel grid extinction math
│           ├── test_global_wind.cpp                        # Validates curl-noise divergence-free vectors
│           ├── test_lightning.cpp                          # Validates fractal discharge step termination
│           ├── test_presets.cpp                            # Validates preset parameter ranges & bounds
│           ├── test_puddles.cpp                            # Validates puddle expansion & evaporation
│           ├── test_rain.cpp                               # Validates raindrop terminal velocity
│           ├── test_snow.cpp                               # Validates snowpack accumulation rates
│           ├── test_storms.cpp                             # Validates barometric pressure drops
│           └── test_weather_sky.cpp                        # Validates sun extinction through overcast skies
│
├── tools/                                                  # Development & Code Generation Tools
│   └── generate_weather_and_sky.py                         # Generates deterministic binary seeds, LUTs, and shaders
│
└── public/                                                 # Public Assets & Screenshots
    └── assets/aistudio/
        ├── screenshot_1_92476bce.png                       # Reference screenshot 1: Cloud structure analysis
        └── screenshot_2_058116a2.png                       # Reference screenshot 2: Volumetric view angle verification
```

---

## Subsystem Architecture & Capabilities

### 1. 3D Viewport & Surface Scene Modes

Sphere Studio features a 3D perspective viewport supporting five distinct surface environments designed for various inspection and simulation scenarios:

| Surface Mode | Identifier | Default Dimensions | Key Capabilities & Visual Presentation |
| :--- | :---: | :---: | :--- |
| **Endless Surface** | `ENDLESS` | $\infty$ (Infinite) | Horizon ground plane receding to the true vanishing point with perspective grid, distance fog, and moving volumetric cloud shadows. |
| **No Surface (Void)** | `NONE` | $0\text{m} \times 0\text{m}$ | Eliminates ground and horizon obstruction. Atmospheric scattering continues across the entire 360° viewport, ideal for mid-air cloud flights and spacecraft/satellite asset inspection. |
| **Flat Piece** | `FLAT_PIECE` | $10\text{m} \times 10\text{m}$ / $3\text{m} \times 3\text{m}$ | Bounded, adjustable ground slab. Features 3D extruded side skirts with directional lighting, 1-meter perspective grid lines, corner registration brackets, and perimeter bevels. |
| **Water Piece** | `WATER_BODY` | $10\text{m} \times 10\text{m}$ | Bounded aquatic basin with deep-ocean cyan gradient, 6 perspective sinusoidal wave ripples, perimeter foam, submerged glass walls, and a contact foam ring around the resting item. |
| **Broken Slab** | `BROKEN_SLAB` | $8\text{m} \times 8\text{m}$ | Fractured floating rock diorama piece with 12 irregular radial vertices, vertical chiseled cliff facets with rock strata, weathered stone top surface with fracture veins, and hovering stone fragments. |

#### Relative Sizing Logic
When switching assets or adjusting the item scale:
- If the item's max horizontal dimension is $\le 1.25\text{m}$ (~1m item), the **Flat Piece** automatically configures to **$3\text{m} \times 3\text{m}$**.
- If the item is larger ($> 1.25\text{m}$), the platform configures to **$10\text{m} \times 10\text{m}$**.
- The user can fine-tune Width and Length from **$1.0\text{m}$ to $50.0\text{m}$** and Skirt Thickness from **$10\text{cm}$ to $2.5\text{m}$**, or tap the **Auto-Fit** button to size the stage proportionally to the asset footprint.

---

### 2. Directional Shadow Mapping & PCF Subsystem (Sascha Willems Reference Architecture)

Directly referencing the official [SaschaWillems/Vulkan/shadowmapping](https://github.com/SaschaWillems/Vulkan/tree/master/examples/shadowmapping) pipeline, the engine implements true directional shadow mapping with Percentage Closer Filtering (PCF), contact ambient occlusion, and dynamic depth bias:

- **Vulkan 1.3 Offscreen Depth Pass (`ShadowMapPass`)**:
  - Dedicated depth render pass writing to a $2048 \times 2048$ `VK_FORMAT_D32_SFLOAT` attachment with `VK_ATTACHMENT_LOAD_OP_CLEAR` and `VK_ATTACHMENT_STORE_OP_STORE`.
  - Depth-only vertex shader (`shadow.vert`) and alpha-tested cutout fragment shader (`shadow.frag`).
  - Sampler configured with `VK_FILTER_LINEAR`, `VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER` (border color $1.0, 1.0, 1.0, 1.0$), and `VK_COMPARE_OP_LESS_OR_EQUAL` for hardware shadow tests.
- **Light-Space Projection Matrix ($M_{\text{light}}$)**:
  - Tight orthographic frustum $P_{\text{ortho}}$ enclosing the scene radius centered on the model:
    $$M_{\text{light}} = P_{\text{ortho}} \times V_{\text{lookAt}}(\vec{P}_{\text{center}} + 2R \cdot \hat{L}_{\text{sun}}, \vec{P}_{\text{center}}, \hat{Up})$$
  - Biased clip-space transform mapping $[-1, 1]$ XY to $[0, 1]$ texture coordinates:
    $$\text{biasMat} = \begin{bmatrix} 0.5 & 0.0 & 0.0 & 0.0 \\ 0.0 & 0.5 & 0.0 & 0.0 \\ 0.0 & 0.0 & 1.0 & 0.0 \\ 0.5 & 0.5 & 0.0 & 1.0 \end{bmatrix}$$
- **Percentage Closer Filtering (PCF) & Soft Penumbra**:
  - Evaluates multi-tap jittered sampling around the projective shadow coordinate:
    - **Hard (1-Tap)**: Raw depth test (aliased edges).
    - **PCF 3×3**: 9-tap kernel for medium softness.
    - **PCF 5×5 (Sascha Willems Standard)**: 25-tap kernel producing smooth, physically realistic penumbra falloff.
    - **Cascaded Shadow Maps (CSM 4-Split)**: 4-depth split frustum for large open-world scenes.
- **Dynamic Slope-Scaled Depth Bias (`vkCmdSetDepthBias`)**:
  - Eliminates "Shadow Acne" while preventing "Peter Panning" detachment artifacts:
    $$\text{bias} = \max(\text{bias}_{\text{const}} \cdot (1.0 - \hat{N} \cdot \hat{L}), 0.1 \cdot \text{bias}_{\text{const}})$$
- **Real-Time 3D Projection in Viewport**:
  - Models cast dynamic directional shadows onto all 5 surface modes (`FLAT_PIECE`, `ENDLESS`, `WATER_BODY`, `BROKEN_SLAB`, `NONE`).
  - Evaluates ray intersection $t = \frac{V_y - y_{\text{plane}}}{L_y}$ and computes the 2D Monotone Chain convex hull of base and projected vertices.
  - Floating platforms cast broad directional drop shadows onto the atmospheric haze below.
  - Self-shadowing on model faces when $\hat{N} \cdot \hat{L} \le 0$.
  - Live Light-Space Depth Map HUD inspects the offscreen depth buffer in real time.

---

### 3. Volumetric Clouds Subsystem (Robert Beckebans Nubis-Decima Port)

Adapted from [Robert Beckebans's OpenGL-VolumetricClouds](https://github.com/RobertBeckebans/OpenGL-VolumetricClouds), the cloudscape engine implements true 3D perspective raymarching across a curved planetary atmosphere:

- **Dual-Sphere Planetary Geometry**:
  $$\text{Earth Radius} = 6,371,000\,\text{m}, \quad R_{\text{inner}} = 705,000\,\text{m}, \quad R_{\text{outer}} = 722,000\,\text{m}$$
  Raymarching executes strictly within this 17-kilometer atmospheric shell.
- **Nubis Density Gradients**:
  - `Stratus`: Low, thin horizontal layers $(0.0, 0.10, 0.20, 0.30)$.
  - `Stratocumulus`: Broad rolling sheets $(0.02, 0.20, 0.48, 0.625)$.
  - `Cumulus`: Towering billowy cauliflowers with dark, flat condensation bases $(0.00, 0.1625, 0.88, 0.98)$.
- **Optical Radiative Transfer**:
  - **Beer's Law Extinction**: $T(s) = \exp(-\tau(s))$, governing primary line-of-sight and sun shadow ray extinction.
  - **Beer-Powder Sugar Approximation**: $P(d) = 1.0 - \exp(-2d)$, simulating multiple forward scattering in dense droplet clusters.
  - **Dual-Lobe Henyey-Greenstein Phase**: Forward peak ($g_1 = 0.82$) producing the intense solar "Silver Lining" halo, combined with backward scattering ($g_2 = -0.20$).
  - **Cone-Stepped Light Sampling**: 6-vector jittered cone march toward the sun for soft volumetric self-shadowing without expensive multi-pass shadowmaps.
- **Atmospheric Effects**:
  - **Crepuscular Rays (God Rays)**: Volumetric sun shafts piercing cloud gaps.
  - **Cirrus Ice Crystals**: High-altitude wispy ice crystal layer ($>10,000\text{m}$).

---

### 3. Atmospheric Sky & Solar Corona (Preetham / Hosek-Wilkie Model)

- **Daylight Spectrum**: Physically evaluated using the Perez all-weather sky model. Shifts dynamically based on solar elevation, azimuth, and aerosol turbidity ($\tau \in [1.0, 10.0]$).
- **Golden Hour & Twilight**: Transitions from daytime cerulean to sunset amber/crimson and deep astronomical twilight.
- **Solar Disk & Corona**: Modeled with non-linear solar limb darkening and Mie forward halo bloom.
- **Tonemapping**: Implements fitted **ACES filmic** and **Extended Reinhard** luminance curves.

---

### 4. Road Networks & 10 Physical Surface States

- **Graph & Spline Modeling**: Centerline ribbons with procedural camber, banking angles, curb extrusions, and multi-lane dividers.
- **Modular Road Kits**: Overpasses, footbridges, tunnel segments with interior lighting, 4-way intersections, and roundabouts.
- **Vector Road Signs**: Evaluated dynamically using high-resolution Signed Distance Fields (SDF) for stops, yields, and speed limits.
- **10 Surface States**:
  1. `Muddy`: Deep wheel ruts, clay displacement, and puddle blending.
  2. `Rocky`: Unpaved aggregate, stone scatter, and dust factors.
  3. `Bumpy`: Washboard sinusoidal undulation and road chatter.
  4. `Sandy`: Wind-blown ripple dunes and loose sand sink rates.
  5. `Gravel`: Angular crushed stone distribution and loose aggregate.
  6. `Snowy`: Packed wheel ruts, powder snow depth, and salt coverage.
  7. `Icy`: Glazed black ice reflection, micro-fissures, and zero traction.
  8. `Wet`: Thin water film reflection, mirror blur, and tire spray.
  9. `Dry Cracked`: Desiccation thermal cracks and asphalt joint degradation.
  10. `Potholed`: Advanced cavitation, asphalt spalling, and sub-base exposure.

---

### 5. Vegetation & Biomes (L-System & GPU Blades)

- **Grammar-Driven Trees**: Procedural branching via Lindenmayer systems (Oak, Pine, Palm, Birch) with automatic LOD decimation and 8-angle billboard impostor baking.
- **GPU Grass Blade Simulation**: Individual bezier curve blades responding dynamically to wind gusts, vehicle wheel paths, and footstep trampling.
- **Biome Classification**: Rule tables distributing flora across Temperate, Desert, Dense Forest, and Alpine Tundra biomes.

---

### 6. PBR Materials, Buildings & Modular Facades

- **Multi-Layer PBR Materials**: Micro-detail noise, Voronoi cell boundaries, height displacement, roughness, and metalness maps for concrete, weathered metal, timber, and ceramic tiles.
- **Grammar-Based Architecture**: Modular building generator attaching wall sockets, windows, entry doors, commercial storefronts, fire escapes, and rooftop HVAC units.

---

### 7. Terrain & Hydraulic Erosion

- **Fractal Noise**: Multi-octave fractional Brownian motion (fBm) with ridged multifractal peaks.
- **Hydraulic & Thermal Erosion**: Simulates particle drop trajectories, sediment transport, deposition, and talus scree slopes.
- **Voxel Marching Cubes**: Seamless overhanging cliffs, sea arches, and volumetric subterranean cave networks.

---

### 8. Water Bodies & Tessendorf FFT Ocean

- **Phillips Ocean Spectrum**: Evaluated via inverse Fast Fourier Transform (FFT) on compute shaders, generating realistic choppy wave peaks, capillary ripples, and whitecap foam.
- **Inland Water Bodies**: Meandering river splines with cross-sectional velocity profiles, tranquil mountain lakes, and cascading waterfalls.
- **Optical Water Properties**: Depth absorption ($e^{-\alpha z}$), caustic refraction patterns, and shore foam contact rings.

---

### 9. Energy, Utility & World Props

- **Renewable Energy Infrastructure**:
  - Multi-part wind turbines (hollow steel towers, nacelles, pitch-controlled airfoil blades) with rotation simulation.
  - Solar photovoltaic tracking arrays that orient dynamically to the sun vector.
- **Electrical & Utility Distribution**:
  - Wooden and concrete utility poles with catenary wire physics calculation.
  - Pole-mounted distribution transformers, electrical meters, and high-voltage substation switchgear.

---

### 10. GPU Particles & Procedural Audio

- **Compute VFX**: GPU bitonic sorting, curl-noise vector fields, and physics simulation for sparks, smoke plumes, fire sheets, and blizzard flurries.
- **Real-Time Procedural Audio**: Band-limited oscillator synthesis for internal combustion engine RPMs, dynamic rain droplet acoustics, wind howl resonance, and surface-specific footstep impacts.

---

## Generated Assets & Binary Manifest Registry

All procedural generators in `native_assets` utilize deterministic binary seed blocks, precomputed lookup tables (LUTs), serialized topological graphs, vector SDF glyphs, and alpha stencil masks:

| Relative File Path | Format | Subsystem | Functional Role | Status |
| :--- | :---: | :--- | :--- | :---: |
| `native_assets/textures/spec/shared_spec.h` | Header | Root Shared | Texture format specification | **Active** |
| `native_assets/meshes/profiles/shared_profile.h` | Header | Root Shared | Shared spline extrusion profiles | **Active** |
| `native_assets/meshes/spec/shared_mesh_spec.h` | Header | Root Shared | Vertex buffer descriptor specifications | **Active** |
| `native_assets/src/weather/textures/seeds/weather_seed.bin` | Binary Seed | Weather Core | Deterministic cloud and rain seed | **Active** |
| `native_assets/src/weather/sky/textures/sky_lut_spec.bin` | Binary LUT | Weather / Sky | Atmospheric scattering transmittance LUT | **Active** |
| `native_assets/src/sky/atmosphere/textures/sky_lut_spec.bin` | Binary LUT | Sky Core | Preetham multi-scattering transmittance | **Active** |
| `native_assets/src/sky/sun/textures/sun_disk_spec.bin` | Binary LUT | Solar Core | Solar limb darkening & corona emission | **Active** |

---

## Build, Test & Deployment Guide

### Prerequisites
- **Android SDK**: Compile SDK `35`, Min SDK `24`, Target SDK `35`
- **Gradle**: Version `8.7` with Android Gradle Plugin (AGP) `8.6.0`
- **Kotlin**: Version `2.0.0` with Compose Multiplatform
- **C++ Toolchain**: C++20 compliant compiler (`clang++` $\ge 16$ or `g++` $\ge 13$), CMake $\ge 3.22$
- **Vulkan SDK**: Version `1.3.250` or newer with GLSL-to-SPIR-V compiler (`glslangValidator` or `glslc`)

### Building the Android & KMP Application
To build and verify the complete application:

```bash
# Verify applet compilation
gradle :app:assembleDebug

# Run JVM unit & Robolectric tests
gradle :app:testDebugUnitTest

# Run KMP common and desktop tasks
gradle :kmp:desktopTest
```

### Building the Native C++20 / Vulkan Engine
To build the native procedural engine and execute the GoogleTest suite:

```bash
cd native_assets
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Run weather and volumetric cloud test suite
ctest --output-on-failure
```

---

## License & Attribution

- **Robert Beckebans**: [OpenGL-VolumetricClouds](https://github.com/RobertBeckebans/OpenGL-VolumetricClouds) (Nubis-Decima volumetric cloud model).
- **Sascha Willems**: [Vulkan](https://github.com/SaschaWillems/Vulkan) (Vulkan 1.3 base infrastructure & allocators).
- **kentril0**: [WaterSurfaceRendering](https://github.com/kentril0/WaterSurfaceRendering) (Common Scene architecture & Preetham atmospheric model).
- **Sphere Studio Engine Team**: Procedural road states, modular architecture, L-System vegetation, and Kotlin Multiplatform integration.
