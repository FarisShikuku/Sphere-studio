# Procedural Trailers & Haulage Shared Library (`gen/trailers/`)

Dedicated procedural semi-trailer, chassis, running gear, and heavy haulage engine for Sphere Engine 3D, architected alongside `gen/cargo/` after [Sascha Willems' Vulkan Samples](https://github.com/SaschaWillems/Vulkan) and [KhronosGroup/Vulkan-Samples](https://github.com/KhronosGroup/Vulkan-Samples).

## Overview
- **Zero Static Assets**: Synthesizes I-beam chassis rails, tank shells, logging bunks, translucent PVC curtains, hydraulic car-carrier ramps, converter dollies, and shared multi-axle running gear via C++20 parametric geometry and GLSL compute shaders (`*_mesh.comp`, `*_albedo.comp`, `*_normal.comp`, `*_orm.comp`).
- **Cargo & Truck Physics Integration**:
  - `TrailerDef` exposes `empty_weight_kg`, `max_payload_kg`, `hitch_type`, `axle_count`, `kingpin_offset_m`, and `suspension_type`.
  - `ITrailer::LoadCargo(cargo, quantity)` couples directly with `sphere::cargo::CargoRegistry` to compute Gross Vehicle Weight (GVW), deck load distribution, and air/leaf suspension compression.

## Usage
```cpp
// 1. Spawn the trailer
auto myTrailer = TrailerRegistry::Get(TrailerID::CURTAINSIDER_XL);

// 2. Load the cargo (from the Cargo Registry)
auto cargo = CargoRegistry::Get().GetCargo(CargoID::CARDBOARD_BOX);
myTrailer->LoadCargo(cargo, /*quantity=*/500);

// 3. Query GVW & attach to truck fifth-wheel
float totalMassKg = myTrailer->GetTotalMassKg();
```
