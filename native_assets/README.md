# Sphere Studio — Native Procedural Vulkan 1.3 Engine & High-Performance Studio

High-performance C++20, C, and Vulkan 1.3 engine for procedural 3D world assets, textures, and real-time physical simulation.

Referencing architectural patterns from:
- [SaschaWillems/Vulkan](https://github.com/SaschaWillems/Vulkan): Vulkan base infrastructure (`vks::VulkanDevice`, `vks::Buffer`, `vks::Texture`, `vks::Texture2D`, `vks::VulkanSwapChain`, `vks::initializers`), memory allocators, staging pools, directional shadow mapping with PCF, and split-sum PBR BRDF integration.
- [AmanSachan1/Meteoros](https://github.com/AmanSachan1/Meteoros): Real-time volumetric cloudscape renderer based on Guerrilla Games' Decima / Nubis architecture.
- [kentril0/WaterSurfaceRendering](https://github.com/kentril0/WaterSurfaceRendering): Common scene management, camera controllers, sky model, and bindless descriptors.

---

## 1. C++ / C / Vulkan Framework Architecture

All native assets are built strictly with standards-compliant C++20 and C, linking against Vulkan 1.3 (`Vulkan::Vulkan`):

```
native_assets/
├── base/                               # Ported directly from SaschaWillems/Vulkan
│   ├── VulkanDevice.hpp / .cpp         # Physical device selection, logical queues, feature enablement
│   ├── VulkanBuffer.hpp / .cpp         # Staging & device-local buffer allocation, host mapping, cache flush
│   ├── VulkanTexture.hpp / .cpp        # KTX2 loader, staging buffers, mip generation, image view & sampler
│   ├── VulkanSwapChain.hpp / .cpp      # Surface presentation, swapchain recreation, acquire next image
│   ├── VulkanTools.hpp / .cpp          # Memory type filtering, format checks, pipeline barrier transitions
│   ├── VulkanInitializers.hpp          # Sascha Willems Vulkan struct initializers (vks::initializers)
│   └── VulkanDebug.hpp / .cpp          # VK_EXT_debug_utils messenger callbacks
├── src/
│   ├── core/                           # Lifecycle, timers, profilers, platform macros
│   ├── vulkan/                         # Dynamic Rendering, Bindless Heap, Pipeline Builder, FrameGraph
│   ├── scene/                          # Geometry (Calibration Cube, Stage, Meshes), SceneGraph, Transform
│   ├── weather/                        # Volumetric clouds (Meteoros), froxel fog, rain, snow particles
│   ├── sky/                            # Atmospheric scattering (Preetham / Hosek-Wilkie), solar corona
│   └── shared/                         # Generators (IGenerator), seed mixers, noise, GPU memory alignments
├── shaders/                            # GLSL 460 Vulkan Shaders
│   ├── CMakeLists.txt                  # Offline glslc SPIR-V build rules
│   └── glsl/
│       ├── pbr.vert / pbr.frag         # PBR Cook-Torrance with Sascha Willems multi-tap PCF shadow mapping
│       ├── shadow.vert / shadow.frag   # Directional shadow pass depth renderers
│       ├── shadow_cascade.vert         # Cascaded shadow map layer selector
│       ├── sky.vert / sky.frag         # Inward sky dome scattering shader
│       ├── sky.comp                    # Atmospheric transmittance compute pre-pass
│       ├── brdf_lut.comp               # Sascha Willems Split-Sum BRDF 2D integration LUT compute generator
│       ├── texture_processor.comp      # KTX2 Sobel normal & roughness derivation compute filter
│       ├── text_sdf.vert / text_sdf.frag # Signed Distance Field (SDF) glyph rendering
│       └── include/                    # bindless.glsl, pbr_common.glsl, math_common.glsl, vulkan_compat.glsl
└── textures/                           # Khronos Texture 2.0 (KTX2) Assets
    ├── rusticated_red_brick.ktx2       # 900x748, 10 mip levels, VK_FORMAT_R8G8B8A8_SRGB
    ├── mondo_grass.ktx2                # 900x900, 10 mip levels, VK_FORMAT_R8G8B8A8_SRGB
    ├── earth_foliage.ktx2              # 900x900, 10 mip levels, VK_FORMAT_R8G8B8A8_SRGB
    └── spec/                           # Texture specifications & PBR parameter descriptors
```

---

## 2. KTX2 Khronos Texture Pipeline (Zero JPEG / PNG Dependency)

To deliver a high-performance studio without CPU decode bottlenecks:
- Images are converted to **Khronos Texture 2.0 (`.ktx2`)** containers.
- Each `.ktx2` file contains:
  1. **12-byte KTX2 Identifier**: `\xABKTX 20\xBB\r\n\x1A\n`
  2. **Vulkan Format Enum**: `VK_FORMAT_R8G8B8A8_SRGB` (43) or `VK_FORMAT_R8G8B8A8_UNORM` (37)
  3. **Multi-level Mipmap Pyramid**: 10 pre-filtered mip levels down to 1×1 for trilinear and anisotropic texture filtering.
  4. **Khronos DFD (Data Format Descriptor)**: Complete channel layout (R8G8B8A8, BT.709 primaries, sRGB transfer function).
  5. **Direct GPU Staging**: Uploaded to Vulkan `VkImage` using `VkBufferImageCopy` without CPU software decompression.
  6. **Generator Tool**: Included at `native_assets/scripts/convert_to_ktx2.py`.

---

## 3. Shader Pipeline Resources (`comp`, `vert`, `frag`)

| Shader File | Stage | Purpose / Architectural Reference |
|---|---|---|
| `brdf_lut.comp` | Compute | Generates the 2D split-sum integration LUT for image-based lighting (IBL) via Hammersley GGX quasi-Monte Carlo sampling (Sascha Willems PBR pipeline). |
| `sky.comp` | Compute | Real-time atmospheric rayleigh & mie transmittance compute evaluation. |
| `texture_processor.comp` | Compute | GPU Sobel filter for generating tangent-space normal maps and micro-contrast roughness directly from KTX2 albedo textures. |
| `pbr.vert` | Vertex | Transforms vertex positions, normals, tangents (Gram-Schmidt orthonormalized), and calculates light-space coordinates using Sascha Willems Vulkan clip-space bias matrix. |
| `pbr.frag` | Fragment | Evaluates Cook-Torrance specular BRDF ($D$ GGX, $V$ Smith-Correlated, $F$ Schlick) + Lambertian diffuse + directional shadow mapping with Percentage Closer Filtering (PCF). |
| `shadow.vert` / `shadow.frag` | Vertex/Fragment | Renders scene geometry to the directional shadow depth map attachment. |
| `shadow_cascade.vert` | Vertex | Cascaded shadow map render pass for multiple distance splits. |
| `sky.vert` / `sky.frag` | Vertex/Fragment | Renders inward hemisphere sky dome with dynamic sun position and horizon haze. |
| `text_sdf.vert` / `text_sdf.frag` | Vertex/Fragment | High-precision vector text rendering using Signed Distance Fields (SDF). |