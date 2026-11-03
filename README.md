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
   - [Common Scene, Hierarchical Scene Graph & Procedural Geometry (C++20 / Vulkan 1.3)](#1-common-scene-hierarchical-scene-graph--procedural-geometry-c20--vulkan-13)
   - [Live Sky Cloud Scanner & Cloud Genera Classification (Meteoros Architecture)](#2-live-sky-cloud-scanner--cloud-genera-classification-meteoros-architecture)
   - [Unified Vulkan 3D Scene Top Bar Across All Studios](#3-unified-vulkan-3d-scene-top-bar-across-all-studios)
   - [3D Viewport & Surface Scene Modes](#4-3d-viewport--surface-scene-modes)
   - [Directional Shadow Mapping & PCF Subsystem (Sascha Willems Reference)](#5-directional-shadow-mapping--pcf-subsystem-sascha-willems-reference-architecture)
   - [Volumetric Clouds Subsystem (AmanSachan1/Meteoros Vulkan Architecture)](#6-volumetric-clouds-subsystem-amansachan1meteoros-vulkan-architecture)
   - [Atmospheric Sky & Solar Corona (Preetham / Hosek-Wilkie Model)](#7-atmospheric-sky--solar-corona-preetham--hosek-wilkie-model)
   - [Road Networks & 10 Physical Surface States](#8-road-networks--10-physical-surface-states)
   - [Vegetation & Biomes (L-System & GPU Blades)](#9-vegetation--biomes-l-system--gpu-blades)
   - [PBR Materials, Architectural Metric Scale & Parametric Sizing](#10-pbr-materials-architectural-metric-scale--parametric-sizing)
   - [KTX2 Texture Compression & 65 Registered Architectural Assets](#11-ktx2-texture-compression--65-registered-architectural-assets)
   - [Vulkan C++ Native Core vs. Kotlin Architecture (Sascha Willems Integration)](#12-vulkan-c-native-core-vs-kotlin-architecture-sascha-willems-integration)
   - [Terrain & Hydraulic Erosion](#13-terrain--hydraulic-erosion)
   - [Water Bodies & Tessendorf FFT Ocean](#14-water-bodies--tessendorf-fft-ocean)
   - [Energy, Utility & World Props](#15-energy-utility--world-props)
   - [GPU Particles & Procedural Audio](#16-gpu-particles--procedural-audio)
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
│       │   ├── SphereStudioApp.kt                          # Master Application Shell & Navigation Host
│       │   ├── SurfaceSceneMode.kt                         # Surface Scene Mode enum (ENDLESS, NONE, FLAT_PIECE, WATER_BODY, BROKEN_SLAB)
│       │   ├── SphereStudioWelcomeScreen.kt                # Landing hub & onboarding showcase
│       │   ├── GlyphCoreStudioScreen.kt                    # SDF vector glyphs & typography studio
│       │   ├── SkyStudioScreen.kt                          # Atmospheric Sky & Solar Simulator with LiDAR Cloud Scanner
│       │   ├── WeatherStudioScreen.kt                      # Volumetric Weather & Nubis Cloud Simulator
│       │   │
│       │   ├── screens/                                    # Top-Level Studio Screens
│       │   │   ├── MaterialInspectorScreen.kt              # Advanced PBR Material, Real-World Scale & Sizing Studio:
│       │   │   │                                           #   - 3D Target Meshes: Calibration Cube, Wall, Ground Slab, Sphere
│       │   │   │                                           #   - Architectural PBR metric scale (mm) with seamless UV tiling
│       │   │   │                                           #   - Real-World Scale vs. Stretch Fit toggle
│       │   │   │                                           #   - Parametric 3-axis dimension controls & scale presets
│       │   │   │                                           #   - 6 Texture channel modes (Full PBR, Albedo, Normal, ORM, Height, Split)
│       │   │   │                                           #   - 4 Category filters (Wall & Masonry, Metals, Flooring, Nature)
│       │   │   └── EmptyAssetScreen.kt                     # Clean state placeholder for unloaded assets
│       │   │
│       │   ├── tabs/                                       # Modular Parameter Control Tabs
│       │   │   ├── ObjectDimensionsControlTab.kt           # Parametric 3-axis dimensions, PBR metric tile scale & UV tiling
│       │   │   ├── DimensionsControlTab.kt                 # Surface stage platform dimensions (width, length, skirt thickness)
│       │   │   ├── MaterialChannelSelectorRow.kt           # Interactive PBR channel mode selector chips
│       │   │   ├── MaterialPropertiesTab.kt                # Roughness, Metallic, IOR, Normal Strength, Height Displacement
│       │   │   ├── MaterialOpticsTab.kt                    # Microfacet GGX distribution, Fresnel F0, Smith geometry
│       │   │   ├── MaterialProceduralDetailTab.kt          # Brick bond patterns, tile grout, mortar offsets, wood grain
│       │   │   ├── MaterialVulkanSpecTab.kt                # Vulkan 1.3 pipeline state, BC7/BC5/BC4 compression specs, KTX2 slots
│       │   │   ├── MaterialShaderTab.kt                    # GLSL pbr.vert / pbr.frag shader inspection
│       │   │   ├── SurfaceCompositeLayersTab.kt            # Multi-layer composite surface stack (e.g. wetness, mud, decals)
│       │   │   ├── ShadowsControlTab.kt                    # PCF shadow map resolution, softness, and depth bias
│       │   │   ├── SkyAndSunTab.kt                         # Solar position, Rayleigh/Mie turbidity, ozone absorption
│       │   │   ├── SurfaceAndStageTab.kt                   # 5 Surface scene modes & procedural ground patterns
│       │   │   └── NativeStructureTab.kt                   # Native C++ engine architecture & memory stats
│       │   │
│       │   ├── viewport/                                   # Real-Time 3D Perspective Viewports & Shaders
│       │   │   ├── MaterialPreviewViewport.kt              # 3D Material Viewport (Cube, Wall, Slab, Sphere) with lighting & blueprint HUD
│       │   │   ├── TextureManager.kt                       # Real-World metric UV tiling shader (BitmapShader TileMode.REPEAT)
│       │   │   ├── SceneCubeViewport.kt                    # Multi-face photometric 3D cube rasterizer
│       │   │   ├── SceneStageRenderer.kt                   # 5 Surface scene stage modes renderer
│       │   │   └── DirectionalShadowRenderer.kt            # Monotone-chain convex hull shadow projector & PCF soft penumbra
│       │   │
│       │   ├── model/                                      # Data Models & Schemas
│       │   │   ├── MaterialModels.kt                       # PbrMaterial models with real-world metric tileScaleMeters
│       │   │   ├── SurfaceModels.kt                        # CompositeSurface models, categories, and layer stacks
│       │   │   ├── GeometryModels.kt                       # Vec3D, Mat4, Vertex, and 3D mesh definitions
│       │   │   ├── ShadowModels.kt                         # Shadow quality, PCF kernels, and depth bias params
│       │   │   ├── SolarModels.kt                          # Solar coordinates, atmospheric parameters, and LUTs
│       │   │   ├── CloudDetectionModels.kt                 # Cloud genera (Cb, Cu, Sc, St, Ci, Ac, Ns) and LiDAR tracking
│       │   │   ├── TextModels.kt                           # SDF glyph metrics, kerning pairs, and font atlases
│       │   │   └── AssetCategory.kt                        # 13 Studio asset category enumerations
│       │   │
│       │   ├── sky/                                        # Sky & Solar Subsystem UI & Viewports
│       │   ├── weather/                                    # Weather & Cloud Subsystem UI & Viewports
│       │   └── text/                                       # Typography & SDF Subsystem UI & Viewports
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
│   │   ├── jni/                                            # JNI C-ABI Bridge between Kotlin & Vulkan C++
│   │   │   └── SphereStudioJni.cpp                         # Native lifecycle, registered KTX2 textures, Cook-Torrance BRDF
│   │   │
│   │   ├── scene/                                          # Scene Graph & World Representation (C++20 / Vulkan 1.3)
│   │   │   ├── CMakeLists.txt                              # Compiles libscene.a (Geometry, SceneGraph, VulkanStage, Scene, Mesh, Transform)
│   │   │   ├── AABB.h / .cpp                               # Axis-aligned bounding box frustum culling & ray intersections
│   │   │   ├── Camera.h / .cpp                             # Perspective & orthographic projection matrices with Vulkan Y-flip
│   │   │   ├── CameraController.h / .cpp                   # Orbit, fly, and walk camera controllers with spherical coordinates
│   │   │   ├── Geometry.h / .cpp                           # Native Procedural Geometry Generators:
│   │   │   │                                               #   - PBR Calibration Cube (1.0m photometric standard)
│   │   │   │                                               #   - Stage Platform with beveled perimeter rims & side skirts
│   │   │   │                                               #   - Infinite ground grid & Reference 3D axes grid (RGB)
│   │   │   │                                               #   - Parametric UV Sphere (smooth lat-long tessellation)
│   │   │   │                                               #   - Inward Sky Dome hemisphere & Curved Meteoros Cloud Volume Hull
│   │   │   │                                               #   - Bounding wireframe AABB box & Procedural Terrain Patch
│   │   │   ├── Light.h / .cpp                              # Directional, point, and spot light sources with cascaded shadow matrices
│   │   │   ├── Material.h / .cpp                           # PBR material descriptors (albedo, normal, roughness, metallic, emissive)
│   │   │   ├── MaterialRegistry.h / .cpp                   # Global material table with bindless texture indices
│   │   │   ├── Mesh.h / .cpp                               # Vertex/Index buffer abstractions, MikkTSpace tangent orthonormalization,
│   │   │   │                                               #   area-weighted vertex normal computation, bounding box calculation
│   │   │   ├── Meshlet.h / .cpp                            # Meshlet clustering for VK_EXT_mesh_shader pipelines
│   │   │   ├── Scene.h / .cpp                              # Common Scene root coordinator:
│   │   │   │                                               #   - 4 Stage Modes (FlatPiece, Boundary, InfiniteGrid, CalibrationCube)
│   │   │   │                                               #   - Cascaded shadow maps, directional sun light, atmospheric sky coupling
│   │   │   ├── SceneGraph.h / .cpp                         # Hierarchical parent-child scene graph with top-down dirty matrix propagation
│   │   │   ├── SceneNode.h / .cpp                          # Scene node entity with component attachments
│   │   │   ├── SkyModel.h / .cpp                           # Sky model hook (Preetham / Hosek atmospheric evaluation)
│   │   │   ├── Transform.h / .cpp                          # 3D Math: column-major Mat4 (Vulkan std430), Vec2/3/4, Quat quaternion,
│   │   │   │                                               #   Euler rotations, TRS composition, Vulkan clip-space projection
│   │   │   ├── VulkanStage.h / .cpp                        # Vulkan 1.3 Multi-Pass Stage Coordinator:
│   │   │   │                                               #   - GlobalSceneUBO layout & MeshPushConstants
│   │   │   │                                               #   - Dynamic rendering passes (Shadow, Sky, Opaque PBR, Stage Grid, Tonemap)
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
│   ├── textures/                                           # Shared Textures & KTX2 Texture Specifications
│   │   └── spec/shared_spec.h                              # 65 registered KTX2 architectural textures with real-world metric dimensions (Architextures standard)
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

### 1. Common Scene, Hierarchical Scene Graph & Procedural Geometry (C++20 / Vulkan 1.3)

All 3D scene architecture, spatial transforms, procedural mesh synthesis, material registry bindings, bounding volume hierarchies, and Vulkan 1.3 pipeline recording are strictly engineered in native C++20 (`native_assets/src/scene/`), ensuring deterministic performance, zero JNI garbage collection overhead, and cross-platform binary alignment. Kotlin Multiplatform and Jetpack Compose function solely as the declarative client UI layer and viewport display bridge.

```
       ┌─────────────────────────────────────────────────────────────┐
       │                   Sphere::Scene (Coordinator)               │
       │   - 4 Stage Modes: FlatPiece, Boundary, Grid, Calibration   │
       │   - Cascaded Sun Lighting & Atmospheric Radiance Sync       │
       └──────────────┬──────────────────────────────┬───────────────┘
                      │                              │
       ┌──────────────▼──────────────┐┌──────────────▼───────────────┐
       │  Sphere::SceneGraph         ││   Sphere::VulkanStage        │
       │  - Parent-Child Hierarchy   ││   - Dynamic Rendering Passes │
       │  - Dirty Transform Updates  ││   - GlobalSceneUBO Push      │
       │  - Frustum & Range Culling  ││   - MeshPushConstants        │
       └──────────────┬──────────────┘└──────────────┬───────────────┘
                      │                              │
       ┌──────────────▼──────────────────────────────▼───────────────┐
       │     Sphere::Geometry & Sphere::Mesh (C++ Generators)        │
       │     - 1m Photometric Calibration Cube                       │
       │     - Stage Platform with Beveled Rims & Side Skirts        │
       │     - Infinite Ground Grid & 3D RGB Reference Axes          │
       │     - Curved Meteoros Atmosphere Shell (705km-722km)        │
       │     - Parametric UV Sphere, Sky Dome, Terrain Patch         │
       └─────────────────────────────────────────────────────────────┘
```

#### Native Procedural Geometry Generators (`Geometry.h` / `Geometry.cpp`)
All geometric assets are generated algorithmically with precise analytical vertex normals, UVs, and orthonormal tangent frames:
- **Photometric 1m Calibration Cube (`CreateCalibrationCube`)**:
  - $1.0\text{m} \times 1.0\text{m} \times 1.0\text{m}$ ground-truth reference standard.
  - Generates 24 distinct vertices with unique face normal directions ($\pm X, \pm Y, \pm Z$) and high-contrast diagnostic UV checkerboards.
  - Used across all studios for verifying photometric exposure, sun elevation shadow casting, and specular reflection behavior.
- **Stage Platform with Beveled Rims (`CreateStagePlatform`)**:
  - Parametric rectangular or square foundation ($W \times L \times H$).
  - Features $45^\circ$ beveled perimeter bevels, vertical extruded side skirts with directional lighting contrast, and bottom chamfers.
- **Infinite Ground Grid (`CreateInfiniteGroundGrid`) & Reference 3D Axes (`CreateReferenceGrid`)**:
  - Metric coordinate grid with colored primary axes: **Red (+X, East)**, **Green (+Y, Zenith/Up)**, **Blue (+Z, South)**.
  - Radial distance attenuation falloff preventing moiré artifacts at vanishing horizons.
- **Parametric UV Sphere (`CreateUvSphere`)**:
  - Tessellated latitude-longitude sphere with welded polar singularities and seamless spherical UV coordinates ($\theta \in [0, \pi]$, $\phi \in [0, 2\pi]$).
- **Meteoros Atmosphere Cloud Volume Hull (`CreateMeteorosCloudVolumeHull`)**:
  - Inward-facing curved double-spherical shell matching the planetary curvature ($R_{\text{inner}} = 705\,\text{km}$, $R_{\text{outer}} = 722\,\text{km}$).
  - Serves as the raymarch entry/exit bounding proxy geometry for the compute cloud synthesizer.
- **Sky Dome Hemisphere (`CreateSkyDome`)**:
  - Inward hemispherical canopy for Preetham/Hosek atmospheric radiance sampling and background projection.
- **Procedural Terrain Patch (`CreateTerrainPatch`)**:
  - Analytical grid mesh with multi-frequency simplex noise height displacement and central-difference normal reconstruction:
    $$\vec{N} = \text{normalize}\left(\begin{bmatrix} -(h_{x+1} - h_{x-1}) / 2\Delta \\ 1.0 \\ -(h_{z+1} - h_{z-1}) / 2\Delta \end{bmatrix}\right)$$

#### Tangent Space Orthonormalization & Normal Synthesis (`Mesh.h` / `Mesh.cpp`)
- Evaluates per-triangle tangents from texture coordinate gradients:
  $$\Delta \vec{P} = \Delta u \, \vec{T} + \Delta v \, \vec{B}$$
- Performs Gram-Schmidt orthonormalization to guarantee strict orthogonal frames for PBR normal mapping:
  $$\vec{T}' = \text{normalize}\left(\vec{T} - (\vec{N} \cdot \vec{T}) \vec{N}\right), \quad \text{handedness } \sigma = \text{sign}\left((\vec{N} \times \vec{T}) \cdot \vec{B}\right)$$

#### Column-Major 3D Math Engine & Camera Subsystem (`Transform.h` / `Camera.h` / `Camera.cpp`)
- **Sascha Willems Vulkan Camera Reference Architecture (`SaschaWillems/Vulkan`)**:
  - Direct port of `SaschaWillems/Vulkan/base/camera.hpp` orbit turntable look-at architecture.
  - **Right-Handed World & Camera Coordinate System**:
    - $+X$ points East/Right, $+Y$ points Zenith/Up, $+Z$ points towards the viewer (South).
    - Camera is placed at $+Z$ (distance $D$) looking along $-Z$ towards the scene origin $(0, 0, 0)$.
    - Points on the front face ($+Z$) have smaller camera view depth ($D - z < D$) and appear closer to the observer.
    - Points on the back face ($-Z$) have larger camera view depth ($D - (-z) > D$) and are drawn behind front faces.
  - **View Transformation**:
    $$V = T(\vec{P}) \times R_x(\text{pitch}) \times R_y(\text{yaw})$$
  - `Mat4` column-major matrix matching Vulkan `std430` buffer layout rules ($m[\text{col} \times 4 + \text{row}]$).
  - Quaternion orientation (`Quat`) with Hamilton multiplication, spherical linear interpolation (slerp), and pitch-yaw-roll conversions.
  - Vulkan clip-space projection with inverted Y-axis to account for Vulkan's inverted NDC coordinate system:
    $$P_{11} = -\frac{1}{\tan(\text{fov}_y / 2)}$$
  - Depth buffer maps to $[0, 1]$ (`GLM_FORCE_DEPTH_ZERO_TO_ONE` standard) with clockwise back-face culling on all 6 cube faces and architectural geometry.

#### Hierarchical Scene Graph (`SceneGraph.h` / `SceneGraph.cpp`)
- Parent-child node relationship tree with top-down dirty matrix propagation:
  $$M_{\text{world}} = M_{\text{parent\_world}} \times M_{\text{local}}$$
- Optimized with dirty-flag caching (`m_worldTransformDirty`) to eliminate redundant matrix multiplications during static frame evaluation.
- Breadth-first and depth-first visitor traversals for frustum culling, shadow pass collection, and render bucket grouping.

#### Vulkan 1.3 Multi-Pass Stage Coordinator (`VulkanStage.h` / `VulkanStage.cpp`)
- Records render commands utilizing Vulkan 1.3 Dynamic Rendering (`vkCmdBeginRendering`):
  1. **Directional Shadow Pass**: Depth-only rasterization into $2048 \times 2048$ `D32_SFLOAT` shadow map.
  2. **Atmospheric Sky Pass**: Inward sky dome radiance projection.
  3. **Opaque PBR Pass**: Stage platform and calibration cube rendering with Cook-Torrance GGX shading.
  4. **Reference Grid Overlay**: Dynamic blend pass for metric distance guides and axes.
  5. **HDR Tonemapping & Post-Process**: ACES filmic or Extended Reinhard tone reproduction.
- Compact push constant layout (`MeshPushConstants`):
  ```cpp
  struct MeshPushConstants {
      Mat4 modelMatrix;
      Vec4 baseColorFactor;
      float metallicFactor;
      float roughnessFactor;
      float specularFactor;
      uint32_t materialFlags;
  };
  ```

---

### 2. Live Sky Cloud Scanner & Cloud Genera Classification (Meteoros Architecture)

Integrated directly into `SkyStudioScreen.kt`, the **Live Sky Cloud Scanner** provides a real-time computer vision and LiDAR telemetry dashboard tracking all major cloud genera classified by the World Meteorological Organization (WMO) and rendered via the AmanSachan1/Meteoros volumetric engine:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ ☉ Live Sky Cloud Scanner                     [LiDAR & CV Tracker ACTIVE]  [ON]│
├─────────────────────────────────────────────────────────────────────────────┤
│ Cloud Types & Genera Filter:                                                │
│ [ All (7) ] [ Cb (1) ] [ Cu (1) ] [ St (1) ] [ Sc (1) ] [ Ci (1) ] [ Ac ]   │
├─────────────────────────────────────────────────────────────────────────────┤
│ ┌────────────────────────────────────────────────────────┐ ┌──────────────┐ │
│ │ [Cb] Cumulonimbus (capillatus incus)                   │ │  F O C U S   │ │
│ │      Anvil Thunderhead / Nimbus Class                  │ │  V I E W     │ │
│ │                                                        │ │              │ │
│ │  Altitude: 1,800m - 12,500m (Troposphere Depth)        │ │  [ ⛶ 1.8x ]  │ │
│ │  Density: 0.94 g/m³  •  Coverage: 88%  •  Turb: Heavy   │ │              │ │
│ │  Phase: Mixed (Supercooled Drops + Hexagonal Ice)      │ │  Align Cam   │ │
│ └────────────────────────────────────────────────────────┘ └──────────────┘ │
└─────────────────────────────────────────────────────────────────────────────┘
```

#### Supported Cloud Genera & Microphysics
1. **Cumulonimbus (`Cb`)**:
   - Monolithic vertical convective thunderheads reaching up to $14,000\,\text{m}$.
   - Exhibits dense anvil cirrus tops (`incus`), heavy optical extinction, dynamic lightning discharge, and strong downdrafts.
2. **Cumulus (`Cu`)**:
   - Crisp, billowy cauliflower heaps with flat horizontal condensation bases ($800\text{m} - 2,200\text{m}$).
   - Forward solar silver lining halo evaluated via dual-lobe Henyey-Greenstein scattering ($g_1 = 0.82$).
3. **Cirrus (`Ci`)**:
   - High-altitude fibrous ice crystal streamers ($7,000\text{m} - 12,000\text{m}$).
   - Optical phase consists of hexagonal ice columns exhibiting $22^\circ$ solar halos and delicate sun ray filtering.
4. **Stratocumulus (`Sc`)**:
   - Broad low-level marine rolling sheets and patchy convective rolls ($600\text{m} - 2,000\text{m}$).
   - Strong multi-scattering radiative transfer preventing dark undersides.
5. **Stratus (`St`)**:
   - Featureless low gray overcast sheet ($200\text{m} - 800\text{m}$) diffusing direct sunlight into uniform ambient illumination.
6. **Altocumulus (`Ac`)**:
   - Mid-level undulating fleece rolls and lenticular wave patches ($2,500\text{m} - 6,000\text{m}$).
7. **Nimbostratus (`Ns`)**:
   - Thick, dark diffuse continuous rain blankets ($500\text{m} - 3,500\text{m}$) accompanied by persistent precipitation and ground wetness.

#### Interactive HUD Features
- **Genera Quick-Filter**: Instant filtering between all detected clouds or isolating specific genera (`Cb`, `Cu`, `St`, `Sc`, `Ci`, `Ac`, `Ns`).
- **Target Tracking & Focus Mode**: Tapping **"Focus View"** immediately engages an animated viewport target lock, smoothly interpolating camera pitch, yaw, and distance to inspect the selected cloud volume with bounding cage wireframes and localized microphysics telemetry.

---

### 3. Unified Vulkan 3D Scene Top Bar Across All Studios

To ensure seamless navigation and contextual awareness across all engine studios (**Atmospheric Sky Studio**, **Volumetric Weather Studio**, **Material Inspector**, and **GlyphCore Studio**), a standardized Vulkan 3D Scene Top Bar is anchored to the top of the viewport:

- **Global Solar & Environmental Telemetry**:
  - **Solar Time**: Live UTC time readout (e.g., `14:30 UTC`) synchronized with astronomical solar progression.
  - **Sun Elevation & Azimuth**: Live angular altitude ($El: 38.0^\circ$) driving shadow projection matrices and Perez daylight luminance.
  - **Camera Yaw**: Real-time orientation angle ($Cam Yaw: 145^\circ$) showing observer azimuth.
  - **Scanner Status Badge**: Indicator pill signaling active LiDAR & computer vision cloud tracking.
- **Interactive Quick-Action Controls**:
  - **Stage Mode Switcher Modal**: Quick-action modal button to seamlessly switch between the 5 surface scene modes (`Flat Piece`, `Endless Ground`, `Void`, `Water Piece`, `Broken Slab`) without leaving the active studio.
  - **Animation Play / Pause**: Toggles physical simulation, wind advection, wave ripple cycles, and precipitation particle updates.
  - **Camera Reset**: Instantly returns orbit camera pitch, yaw, and zoom to optimal framing defaults.

---

### 4. 3D Viewport & Surface Scene Modes

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

### 5. Directional Shadow Mapping & PCF Subsystem (Sascha Willems Reference Architecture)

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

### 6. Volumetric Clouds Subsystem (AmanSachan1/Meteoros Vulkan Architecture)

Adapted and converted from [AmanSachan1/Meteoros](https://github.com/AmanSachan1/Meteoros) (Aman Sachan & Meghana Seshadri, CIS 565 UPenn), based on Guerrilla Games' Decima Engine ('Nubis' SIGGRAPH 2015 & 2017 papers), the cloudscape engine implements high-performance 3D perspective raymarching across a curved planetary atmosphere:

- **Dual-Sphere Planetary Atmosphere Geometry**:
  $$\text{Earth Radius} = 6,371,000\,\text{m}, \quad R_{\text{inner}} = 705,000\,\text{m}, \quad R_{\text{outer}} = 722,000\,\text{m}$$
  Raymarching executes inside this 17-kilometer atmospheric shell with altitude wind shear and advection.
- **Meteoros Cloud Species Gradients**:
  - `Stratus`: Low, thin horizontal layers $(0.0, 0.10, 0.20, 0.30)$.
  - `Stratocumulus`: Broad rolling sheets $(0.02, 0.20, 0.48, 0.625)$.
  - `Cumulus`: Towering billowy cauliflowers with dark, flat condensation bases $(0.00, 0.1625, 0.88, 0.98)$.
  - `Cumulonimbus`: Monolithic anvil thunderclouds spanning full atmospheric depth $(0.01, 0.12, 0.92, 1.00)$.
- **Meteoros Multi-Scattering Optical Radiative Transfer**:
  - **Dual Beer-Lambert Law Approximation**:
    $$E_{\text{multi}} = \exp(-\tau) + \exp(-0.25\tau) \cdot 0.7 \cdot k_{\text{multi}}$$
    Pushes photons deep into thick cloud folds to prevent pitch-black cores and produce luminous cauliflower volume interiors.
  - **Beer-Powder Sugar Approximation**: $P(d) = 1.0 - \exp(-2d)$, simulating multiple forward in-scattering in dense droplet clusters.
  - **Dual-Lobe Henyey-Greenstein Phase**: Forward peak ($g_1 = 0.82$) producing radiant solar "Silver Lining" halos, combined with backward scattering ($g_2 = -0.22$).
  - **Cone-Stepped Light Sampling**: 6-vector jittered cone march toward the sun for soft volumetric self-shadowing without expensive multi-pass shadowmaps.
- **Meteoros Pipelines & Post-Processing**:
  - **Interleaved 4x4 Bayer Ray Jittering**: Offsets initial ray distance per pixel, completely eliminating woodgrain slicing artifacts.
  - **Crepuscular Rays (`cloud_godrays.comp`)**: Volumetric sun shafts piercing cloud gaps via radial screen-space illumination marching with exponential decay.
  - **Temporal Reprojection (`cloud_reproject.comp`)**: 3x3 neighborhood bounding box clamping for anti-aliasing and ghosting rejection.
  - **High Dynamic Range (HDR) & ACES Tone Mapping**: Natural highlight roll-off for solar disk backlighting and bright cloud edges.

---

### 7. Atmospheric Sky & Solar Corona (Preetham / Hosek-Wilkie Model)

- **Daylight Spectrum**: Physically evaluated using the Perez all-weather sky model. Shifts dynamically based on solar elevation, azimuth, and aerosol turbidity ($\tau \in [1.0, 10.0]$).
- **Golden Hour & Twilight**: Transitions from daytime cerulean to sunset amber/crimson and deep astronomical twilight.
- **Solar Disk & Corona**: Modeled with non-linear solar limb darkening and Mie forward halo bloom.
- **Tonemapping**: Implements fitted **ACES filmic** and **Extended Reinhard** luminance curves.

---

### 8. Road Networks & 10 Physical Surface States

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

### 9. Vegetation & Biomes (L-System & GPU Blades)

- **Grammar-Driven Trees**: Procedural branching via Lindenmayer systems (Oak, Pine, Palm, Birch) with automatic LOD decimation and 8-angle billboard impostor baking.
- **GPU Grass Blade Simulation**: Individual bezier curve blades responding dynamically to wind gusts, vehicle wheel paths, and footstep trampling.
- **Biome Classification**: Rule tables distributing flora across Temperate, Desert, Dense Forest, and Alpine Tundra biomes.

---

### 10. PBR Materials, Architectural Metric Scale & Parametric Sizing

The PBR Material & Surface Studio (`MaterialInspectorScreen.kt`) provides a production-grade inspection environment for physically based rendering, architectural materials, real-world metric scale tiling, and parametric object sizing:

#### 3D Target Meshes
- **3D Photometric Test Cube (`CUBE`)**: Standard 3D calibration cube ($1.92\text{m} \times 1.92\text{m} \times 1.92\text{m}$ default) rendered in 3D perspective with corner registration brackets, metric crosshairs, "+Z FRONT / +Y TOP / +X RIGHT" face telemetry, and true Cook-Torrance specular highlight lobes.
- **3D Architectural Wall (`WALL`)**: Vertical wall slab ($1.85\text{m(W)} \times 1.45\text{m(H)} \times 340\text{mm(T)}$ default) with realistic physical thickness, bevelled top coping capstone, foundation curb, and contact drop shadows.
- **3D Ground / Floor Slab (`PLANE`)**: Perspective architectural slab ($2.40\text{m} \times 2.40\text{m} \times 200\text{mm}$ thickness default) with side skirt extrusion and 1-meter reference grid.
- **PBR Microfacet Sphere (`SPHERE`)**: Curved calibration sphere ($\varnothing 1.60\text{m}$ default) evaluating Fresnel rims, microfacet roughness gradients, and Snell's law reflections.

#### Architectural Real-World Metric Scale (Architextures / BIM Standard)
In real-world architectural design (BIM, Architextures, Unreal Engine, 3ds Max), PBR materials have known physical sample dimensions (e.g., standard UK/US stretcher brick pattern: $900\text{mm} \times 748\text{mm}$; rich organic loam: $700\text{mm}$; mondo grass: $300\text{mm}$; flamed granite paving: $1208\text{mm}$).
- **Anti-Distortion Guarantee**: Applying a texture to an object avoids arbitrary single-quad stretching or fit-to-box distortion. Instead, the surface dimensions directly govern UV repetition:
  $$\text{repeat}_U = \frac{\text{Surface Width (m)}}{\text{Material Tile Scale (m)}} \times \text{Multiplier}_U, \quad \text{repeat}_V = \frac{\text{Surface Height (m)}}{\text{Material Tile Scale (m)}} \times \text{Multiplier}_V$$
- **Seamless BitmapShader Tiling**: Implemented via Android `BitmapShader(..., TileMode.REPEAT, TileMode.REPEAT)` mapped to perspective quad polygons via `Matrix.setPolyToPoly` and `setLocalMatrix`. When the user expands a wall from $1.8\text{m}$ to $3.6\text{m}$, the texture seamlessly adds more brick courses and paver units at exact 1:1 metric scale.
- **Sphere Circumference Tiling**: UV repetition around the sphere equator is physically coupled to diameter: $\text{repeat}_U = \frac{\pi \cdot D}{\text{Tile Scale}} \times \text{Multiplier}_U$.
- **Real-World vs. Stretch Fit Toggle**: Instant toggle between physical metric tiling and single-fit stretching.
- **UV Multipliers & Presets**: Quick $1.0\times$, $2.0\times$, and $0.5\times$ presets, with independent parametric $U$ and $V$ sliders.
- **Live Telemetry & Blueprint Plaques**: Each mesh displays live physical dimensions ($mm$/meters) and $(U \times V)$ repetition coordinates directly on 3D blueprint HUD plaques.

#### Parametric 3-Axis Object Sizing Controls (`ObjectDimensionsControlTab.kt`)
- **Independent 3-Axis Sliders**:
  - **Wall**: Width ($0.5\text{m} - 5.0\text{m}$), Height ($0.5\text{m} - 4.0\text{m}$), Thickness ($100\text{mm} - 800\text{mm}$).
  - **Cube**: Width ($0.4\text{m} - 4.0\text{m}$), Height ($0.4\text{m} - 4.0\text{m}$), Depth ($0.4\text{m} - 4.0\text{m}$).
  - **Floor / Slab**: Width ($0.5\text{m} - 6.0\text{m}$), Depth ($0.5\text{m} - 6.0\text{m}$), Slab Thickness ($50\text{mm} - 600\text{mm}$).
  - **Sphere**: Diameter ($\varnothing 0.4\text{m} - 4.0\text{m}$).
- **Global Sizing Presets**: Quick-switch buttons for **Compact (0.7x)**, **Default (1.0x)**, **Expanded (1.4x)**, and **Massive (2.0x)**.
- **Quick Size Toolbar**: Top-anchored expander bar allowing rapid dimension tweaking and real-world scale toggles without leaving the 3D viewport.

#### Channel Modes & Parameter Inspection
- **Channel Modes**:
  1. `Full Composite PBR`: Complete lighting with diffuse albedo, normal perturbation, and specular microfacets.
  2. `Albedo Map`: Unlit diffuse surface reflectance.
  3. `Normal Map`: Tangent-space normal vector field with chromatic normal vectors.
  4. `ORM Map`: Composite channel packed (Red = Ambient Occlusion, Green = Roughness, Blue = Metallic).
  5. `Height / Displacement`: Grayscale elevation / parallax displacement map.
  6. `Split Compare`: Interactive A/B split slider comparing raw albedo against final lit PBR output.
- **Physical Optical Properties**: Roughness ($0.0 - 1.0$), Metallic ($0.0 - 1.0$), Index of Refraction (IOR $1.0 - 3.0$), Normal Strength ($0.0 - 3.0\times$), Height Displacement ($0 - 50\text{mm}$), Ambient Occlusion intensity, and Base Color tinting.

---

### 11. KTX2 Texture Compression & 65 Registered Architectural Assets

Sphere Studio incorporates the **Khronos Texture 2.0 (`.ktx2`)** specification using **Basis Universal supercompression** (UASTC / ETC1S) for GPU memory optimization and multiplatform texture streaming:

#### Basis Universal & Native Transcoding Pipeline
- **Storage & Bandwidth**: Texture file sizes are compressed by $60\% - 80\%$ compared to uncompressed RGBA8, allowing 65 high-resolution architectural PBR texture sets to fit into mobile memory.
- **Zero-CPU GPU Transcoding**: KTX2 textures are transcoded at load time directly into the target GPU's native hardware block compression format:
  - **Desktop / Vulkan PC**: Transcoded to **BC7** (Albedo/ORM) and **BC5** (Normal maps).
  - **Mobile / Android**: Transcoded to **ASTC (4x4 or 6x6)** or **ETC2** / **EAC**.
- **Complete Mipmap Pyramids**: All KTX2 assets contain 10 precomputed mipmap levels with Kaiser filtered downsampling to eliminate texture aliasing at shallow grazing angles.

#### 65 Registered Architectural Assets (`shared_spec.h`)
The native engine registers 65 standard architectural materials across 4 dedicated categories, complete with metric tile dimensions, bindless descriptor slots, and photometric defaults:

| Category | Example Materials | Physical Tile Scale | Bindless Slot Range |
| :--- | :--- | :---: | :---: |
| **Metals & Cladding** | Box Profile Stainless Steel, Square Profile Zinc, Corrugated Aluminium, Diamond Tread Aluminium, Patinated Copper, Weathered Rusted Iron Plate | $800\text{mm} - 3000\text{mm}$ | `1000 - 1080` |
| **Bricks & Masonry** | Rusticated Red Brick, Metro Subway Tile White, Roughcast Render Tan, Smoky Brick Grey, Pilotage Red, Gillespie Magenta, Buff Flemish, Industrial Common Bond | $500\text{mm} - 1575\text{mm}$ | `1090 - 1240` |
| **Stone, Pavers & Slabs** | Grey Victorian Encaustic Tile, Basalt V-Pavers, Basalt Bowtie, Concrete Aggregate Pavers, Dark Scottish Granite Setts, Larvikite Blue Pearl, Galletta Terrazzo, Flamed Granite Slabs, Sofia Marble Star Inlay | $450\text{mm} - 3200\text{mm}$ | `1250 - 1450` |
| **Flooring, Nature & Roof** | European Oak Hardwood Planks, Wenge Chevron Parquet, Mondo Grass Turf, Perennial Ryegrass, Forest Cushion Moss, Rich Organic Loam, Black Stone Asphalt, Terracotta Roman Roof Tiles, Bitumen Hexagonal Shingles | $300\text{mm} - 3275\text{mm}$ | `1460 - 1640` |

---

### 12. Vulkan C++ Native Core vs. Kotlin Architecture (Sascha Willems Integration)

In accordance with modern high-performance Android graphics and the reference architecture from **[SaschaWillems/Vulkan](https://github.com/SaschaWillems/Vulkan)**, responsibilities are cleanly decoupled between C++20 (NDK) and Kotlin Multiplatform:

```
[ User Interaction ]  --->  [ Kotlin Compose UI ]  --->  [ JNI Bridge ]  --->  [ Vulkan C++ ]  --->  [ GPU Shaders ]
(Touch / Sliders)          (MaterialInspectorScreen)      (SphereStudioJni)       (UBO / PushConstants)   (pbr.vert / pbr.frag)
```

#### 1. Why Core Vulkan Belongs in C/C++
Android does not provide an official Java or Kotlin runtime binding for Vulkan (unlike OpenGL ES with `android.opengl.GLES20`). Vulkan on Android is strictly exposed as a native C API via the NDK (`<vulkan/vulkan.h>` and `libvulkan.so`):
- **Vulkan Infrastructure (`native_assets/base/`)**: Adapted directly from Sascha Willems (`VulkanDevice.cpp`, `VulkanSwapChain.cpp`, `VulkanBuffer.cpp`, `VulkanTexture.cpp`, `VulkanDebug.cpp`).
- **Memory & Allocators**: Native host-visible, device-local, and staging buffer management with sub-allocation pools.
- **Dynamic Rendering & Pipelines (`native_assets/src/vulkan/`)**: Vulkan 1.3 dynamic rendering pipelines (`PipelineBuilder.cpp`), removing legacy render pass boilerplate.
- **Bindless Descriptor Heaps**: `VK_EXT_descriptor_indexing` allowing the 65 registered KTX2 textures to reside in unbounded descriptor arrays indexed directly by `mat.albedoTexIndex` in GLSL.

#### 2. Why UI and Parameter Controls Belong in Kotlin (`kt`)
- In Sascha Willems' desktop repository, controls are implemented via **Dear ImGui (`imgui.cpp`)**. On Android, ImGui lacks native accessibility, system font scaling, and smooth mobile touch ergonomics.
- Kotlin Multiplatform & Jetpack Compose (`MaterialInspectorScreen.kt`, `ObjectDimensionsControlTab.kt`) provide responsive Material 3 UI widgets (Sliders, FilterChips, Dropdowns, TabRows).
- Provides instant fallback software/canvas rasterization viewports on devices without hardware Vulkan drivers or in headless JVM unit test environments.

#### 3. JNI Bridge (`SphereStudioJni.cpp`)
Connects the Kotlin Compose UI with the C++ Vulkan pipeline:
- `nativeInitEngine()`: Initializes C++ logging, `GeneratorRegistry`, and KTX2 texture metadata.
- `nativeGetEngineVersion()`: Returns engine branding string (`"SphereStudioEngine 1.3 (Sascha Willems Vulkan Framework / C++20 / NDK r26b)"`).
- `nativeGetRegisteredTextureCount()`: Returns total registered KTX2 textures ($65$).
- `nativeGetTextureId()`, `nativeGetTextureDisplayName()`, `nativeGetTextureBindlessSlot()`: Reflects texture metadata to Kotlin.
- `nativeEvaluateCookTorrance()`: Evaluates the exact C++ microfacet specular BRDF ($D \cdot G \cdot F$) on CPU when cross-validating GPU shader parity.

#### 4. GPU Shaders (GLSL to SPIR-V)
- **`pbr.vert`**: Multiplies vertex position by `push.modelMatrix` (containing the parametric object dimensions $w, h, d$) and outputs light-space coordinates for Sascha Willems shadow mapping.
- **`pbr.frag`**: Multiplies `inTexCoord` by the push constant `uvScale` vector:
  ```glsl
  vec2 uv = inTexCoord * mat.uvScale;
  vec4 albedo = texture(samplerColorMap, uv);
  ```
  The sampler is configured with `VK_SAMPLER_ADDRESS_MODE_REPEAT` to execute seamless physical metric tiling across the 3D surface.

---

### 13. Terrain & Hydraulic Erosion

- **Fractal Noise**: Multi-octave fractional Brownian motion (fBm) with ridged multifractal peaks.
- **Hydraulic & Thermal Erosion**: Simulates particle drop trajectories, sediment transport, deposition, and talus scree slopes.
- **Voxel Marching Cubes**: Seamless overhanging cliffs, sea arches, and volumetric subterranean cave networks.

---

### 14. Water Bodies & Tessendorf FFT Ocean

- **Phillips Ocean Spectrum**: Evaluated via inverse Fast Fourier Transform (FFT) on compute shaders, generating realistic choppy wave peaks, capillary ripples, and whitecap foam.
- **Inland Water Bodies**: Meandering river splines with cross-sectional velocity profiles, tranquil mountain lakes, and cascading waterfalls.
- **Optical Water Properties**: Depth absorption ($e^{-\alpha z}$), caustic refraction patterns, and shore foam contact rings.

---

### 15. Energy, Utility & World Props

- **Renewable Energy Infrastructure**:
  - Multi-part wind turbines (hollow steel towers, nacelles, pitch-controlled airfoil blades) with rotation simulation.
  - Solar photovoltaic tracking arrays that orient dynamically to the sun vector.
- **Electrical & Utility Distribution**:
  - Wooden and concrete utility poles with catenary wire physics calculation.
  - Pole-mounted distribution transformers, electrical meters, and high-voltage substation switchgear.

---

### 16. GPU Particles & Procedural Audio

- **Compute VFX**: GPU bitonic sorting, curl-noise vector fields, and physics simulation for sparks, smoke plumes, fire sheets, and blizzard flurries.
- **Real-Time Procedural Audio**: Band-limited oscillator synthesis for internal combustion engine RPMs, dynamic rain droplet acoustics, wind howl resonance, and surface-specific footstep impacts.

---

## Generated Assets & Binary Manifest Registry

All procedural generators in `native_assets` utilize deterministic binary seed blocks, precomputed lookup tables (LUTs), serialized topological graphs, vector SDF glyphs, and alpha stencil masks:

| Relative File Path | Format | Subsystem | Functional Role | Status |
| :--- | :---: | :--- | :--- | :---: |
| `native_assets/textures/spec/shared_spec.h` | C++ Header | Root Shared | 65 Registered KTX2 Architectural Assets with Real-World Physical Dimensions | **Active** |
| `native_assets/src/jni/SphereStudioJni.cpp` | C++ / JNI | Native Core | JNI Bridge: Lifecycle, KTX2 Texture Registry Reflection & Cook-Torrance BRDF | **Active** |
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

- **Aman Sachan & Meghana Seshadri**: [Meteoros](https://github.com/AmanSachan1/Meteoros) (Vulkan real-time volumetric cloudscape renderer based on Guerrilla Games' Decima / Nubis architecture).
- **Robert Beckebans**: [OpenGL-VolumetricClouds](https://github.com/RobertBeckebans/OpenGL-VolumetricClouds) (Nubis-Decima volumetric cloud equations).
- **Sascha Willems**: [Vulkan](https://github.com/SaschaWillems/Vulkan) (Vulkan 1.3 base infrastructure & allocators).
- **kentril0**: [WaterSurfaceRendering](https://github.com/kentril0/WaterSurfaceRendering) (Common Scene architecture & Preetham atmospheric model).
- **Sphere Studio Engine Team**: Procedural road states, modular architecture, L-System vegetation, and Kotlin Multiplatform integration.
