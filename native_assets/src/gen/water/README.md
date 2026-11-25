# Procedural Water Simulation Engine (`gen/water/`)

Full Vulkan 1.3 / C++20 implementation of the real-time 3D **Smoothed Particle Hydrodynamics (SPH)** fluid simulation and **Screen-Space Fluid Rendering (SSFR)** pipeline referenced from [`talvinckb/OpenGL-Water-Simulation`](https://github.com/talvinckb/OpenGL-Water-Simulation).

## Architectural Port: OpenGL 4.6 -> Vulkan 1.3

| OpenGL-Water-Simulation Component | Vulkan 1.3 / Sphere Engine Equivalent |
|---|---|
| `include/FluidSimulator.hpp` & `src/FluidSimulator.cpp` | `gen/water/include/FluidSimulator.hpp` & `src/FluidSimulator.cpp` |
| `include/FluidRenderer.hpp` & `src/FluidRenderer.cpp` | `gen/water/include/FluidRenderer.hpp` & `src/FluidRenderer.cpp` |
| `include/OrbitalCamera.hpp` & `src/OrbitalCamera.cpp` | `gen/water/include/OrbitalCamera.hpp` & `src/OrbitalCamera.cpp` |
| OpenGL SSBOs & `glDispatchCompute` | `VulkanFluidPipeline` (`VkBuffer` storage buffers, `vkCmdDispatch`, `vkCmdPipelineBarrier`) |
| `shaders/compute/*.comp` (GL 4.6) | `gen/water/shaders/compute/*.comp` (Vulkan 1.3 `#version 460`, `set/binding`, `push_constant`) |
| `shaders/graphics/*.vert/.frag` (GL 4.6) | `gen/water/shaders/graphics/*.vert/.frag` (Vulkan 1.3 `#version 460`, attachment samplers) |
| `ImGuiLayer.cpp` Controls | `WaterStudioScreen.kt` + 7 Jetpack Compose Control Tabs |
| [`kentril0/WaterSurfaceRendering`](https://github.com/kentril0/WaterSurfaceRendering) | `WaterSurfaceSimulator` (Tessendorf 2D FFT) + `WaterSurfaceRenderer` (Preetham Sky & Baboud-Décoret Optics) |

## Directory Layout

```text
gen/water/
├── CMakeLists.txt
├── README.md
├── REFERENCES.md
├── include/
│   ├── FluidParams.hpp
│   ├── FluidSimulator.hpp
│   ├── FluidRenderer.hpp
│   ├── WaterSurfaceParams.hpp
│   ├── WaterSurfaceSimulator.hpp
│   ├── WaterSurfaceRenderer.hpp
│   ├── OrbitalCamera.hpp
│   ├── VulkanFluidPipeline.hpp
│   └── WaterSimulationEngine.hpp
├── src/
│   ├── FluidSimulator.cpp
│   ├── FluidRenderer.cpp
│   ├── WaterSurfaceSimulator.cpp
│   ├── WaterSurfaceRenderer.cpp
│   ├── OrbitalCamera.cpp
│   ├── VulkanFluidPipeline.cpp
│   └── WaterSimulationEngine.cpp
└── shaders/
    ├── compute/
    │   ├── build_spatial_hash.comp
    │   ├── bitonic_sort.comp
    │   ├── compute_offsets.comp
    │   ├── sph_predict_positions.comp
    │   ├── sph_density.comp
    │   ├── sph_forces.comp
    │   ├── sph_integrate.comp
    │   └── phillips_fft_spectrum.comp
    └── graphics/
        ├── particle_depth.vert / .frag
        ├── particle_thickness.vert / .frag
        ├── bilateral_blur.vert / .frag
        ├── fluid_composite.vert / .frag
        ├── container_box.vert / .frag
        ├── water_surface.vert / .frag
        └── underwater_terrain.vert / .frag
```
