# Sphere Studio — Root Shaders

## Sharing Rule (MANDATORY)
Files in this root `shaders/` directory are **shared across 2 or more subsystems**. Category-specific shaders (e.g. road camber, FFT ocean, tree L-systems) reside directly inside their respective subsystem folder under `src/`.

## Descriptor Set Layout Standards (Vulkan 1.3)
- **Set 0 (`bindless.glsl`)**: Bindless unbounded arrays of samplers and storage buffers (`VK_EXT_descriptor_indexing`).
- **Set 1 (`Scene Uniforms`)**: Camera matrices (View, Proj, ViewProj, inverse matrices), global lighting, exposure, atmosphere settings.
- **Set 2 (`Material Table`)**: Material parameter buffers (base color, roughness, metallic, texture indices).
- **Set 3 (`Per-Draw / Instance`)**: Transform, bounding box, instance ID (accessed via Push Constants or Storage Buffer).

## Included GLSL Libraries
- `vulkan_compat.glsl`: Vulkan 1.3 extensions (nonuniform qualifiers, scalar block layout, 64-bit int).
- `bindless.glsl`: Bindless texture indexing helpers (`SampleBindless2D`, `SampleBindlessLod2D`, `SampleBindlessCube`).
- `math_common.glsl`: Linear algebra, rotation, unpacking, TBN construction, and linear depth helpers.
- `noise_common.glsl`: Hash functions, 2D Value Noise, FBM (Fractional Brownian Motion), and Worley/Cellular noise.
- `atmosphere_common.glsl`: Preetham / Hosek-Wilkie Rayleigh and Mie scattering models with optical airmass transmittance.
- `pbr_common.glsl`: Complete Cook-Torrance specular (GGX normal distribution, Smith correlated geometric visibility, Schlick Fresnel) and Lambertian diffuse BRDF.

## Pipeline Shaders
- `pbr.vert` / `pbr.frag`: Standard deferred / forward PBR pipeline with bindless textures, normal mapping, and ORM parameters.
- `shadow.vert` / `shadow.frag`: Directional sun shadow mapping with alpha testing / cutout support.
- `sky.vert` / `sky.frag`: Screen-space atmospheric sky dome rendering with sun disk and corona.
- `sky.comp`: Compute shader precomputing atmospheric single scattering Look-Up Tables (LUT).
- `cull.comp`: GPU frustum and occlusion culling generating indirect draw commands.
- `meshlet.comp`: Meshlet cluster culling (cone backface and bounding sphere checks).
- `debug/debug_line.vert` / `debug/debug_line.frag`: World-space colored debug lines (bounding boxes, raycasts, normals).
- `debug/debug_text.frag`: SDF font atlas debug text overlay.

