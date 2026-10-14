# Sphere Studio — Native Procedural Engine

High-performance C++20 and Vulkan 1.3 standards-compliant engine for procedural 3D world assets.
Referencing architectural patterns from:
- [SaschaWillems/Vulkan](https://github.com/SaschaWillems/Vulkan): Vulkan base infrastructure, memory allocators, device abstraction, swapchain.
- [kentril0/WaterSurfaceRendering](https://github.com/kentril0/WaterSurfaceRendering): Common scene management, camera controllers, sky model, bindless descriptors.

## Subsystems
- **`base/`**: Vulkan 1.3 base primitives ported from SaschaWillems (buffers, textures, swapchain, debug utils).
- **`src/core/`**: Engine lifecycle, timers, profilers, platform macros, and assertions.
- **`src/vulkan/`**: Vulkan 1.3 abstractions (Dynamic Rendering, Bindless Heap, Pipeline Builder, FrameGraph).
- **`src/scene/`**: Common Scene where all procedural assets are viewed (Camera, Lighting, Sky, SceneGraph, Materials).
- **`src/shared/`**: Generator interface (`IGenerator`), Seed mixers, Noise library, Spline math, GPU alignments.
- **`shaders/`**: Vulkan GLSL 460 shaders with bindless descriptors and PBR BRDF.\n