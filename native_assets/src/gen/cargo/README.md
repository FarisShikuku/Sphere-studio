# Procedural Cargo & Commodities Shared Library (`gen/cargo/`)

Dedicated procedural goods and physical commodities engine for Sphere Engine 3D, designed after the Vulkan architecture demonstrated in [Sascha Willems' Vulkan Samples](https://github.com/SaschaWillems/Vulkan) and [KhronosGroup/Vulkan-Samples](https://github.com/KhronosGroup/Vulkan-Samples).

Instead of static surface materials (like brick or plaster), this module proceduralizes 3D physical goods (bags, crates, liquids, logs, bottles, industrial pallets, perishables) so they can be loaded onto vehicles, placed on warehouse racks, stacked on Euro pallets, or stocked on retail shelves without heavy 3D asset bundles.

---

## 1. Architectural Highlights

1. **Mesh + Texture in One (`ICargo.h`)**:
   Physical goods require procedural geometry (e.g. inflated pillow sack, lathe bottle contour, ribbed steel drum) alongside PBR textures (Albedo, Normal, ORM). `ICargo` orchestrates both.

2. **Infinitely Variable Deterministic Seeds**:
   Each category reads uint32 seeds from `seeds/*_seed.bin` to ensure that every pallet of goods has organic wrinkles, varied branding wear, or micro-dents while remaining fully deterministic.

3. **Memory & GPU Efficient**:
   Instead of shipping large static 4K texture packs, compact Vulkan compute pipelines (`bag_albedo.comp`, `drum_normal.comp`) generate BC7/BC5 compressed textures in milliseconds.

4. **Physics & Logistics Integration**:
   `CargoDef` exposes realistic mass (`weight_kg`), volume (`volume_m3`), density (`density_kg_m3`), and stack capacity (`max_stack_load_kg`) so vehicle suspensions and physics engines accurately simulate payload weight.

5. **Imported Subsystems**:
   As required, low-level typography, stenciling, and billboard rendering are imported from `shared/text/TextDef.h`, and scene graph / mesh management is imported from `scene/Mesh.h` and `scene/Scene.h`.

---

## 2. Directory Structure

```text
gen/cargo/
├── CMakeLists.txt                             # Builds libcargo.a
├── README.md                                  # Registry architecture documentation
│
├── library/                                   # Central Registry & Interfaces
│   ├── include/CargoID.h                      # CargoID enum (35+ commodities)
│   ├── include/CargoDef.h                     # CargoDef physical struct & VkFormat specs
│   ├── include/CargoRegistry.h                # Singleton: Register(), Get(), RequestMesh(), RequestImage()
│   ├── include/ICargo.h                       # Base interface for cargo generators
│   └── src/                                   # Implementation files
│
├── bagged_goods/                              # Bags (Sugar, Maize, Cement, Fertilizer, Rice, Coffee, Tea)
├── boxes_crates/                              # Boxes (Cardboard, Wooden Crate, Plastic Crate, Pallet, Milk Crate)
├── liquid_bulk/                               # Liquids (Water Tanker, 200L Drum, Jerrican, Milk Churn, Gas Cylinder)
├── raw_materials/                             # Raw (Oak Log, Timber Plank, Steel Rebar, Coal Pile, Gravel Pile)
├── containers_bottles/                        # Containers (Beer Bottle, Soda Bottle, Water Bottle, Jar, Tin Can)
├── specialty_industrial/                      # Specialty (Cement Bag, 6m Steel Pipe, Brick Pallet, Tyre Stack)
└── perishable_goods/                          # Perishables (Tomato Crate, Potato Bag, Fish Crate, Meat Box)
```

---

## 3. How to Query and Use Commodities in C++

```cpp
#include "gen/cargo/library/include/CargoRegistry.h"

// Retrieve cargo definition and physical properties
auto sugarBagDef = sphere::cargo::CargoRegistry::Get().GetDef(sphere::cargo::CargoID::SUGAR_BAG_50KG);
float payloadMass = sugarBagDef.weight_kg * 40.0f; // 40 bags = 2,000 kg

// Request procedural mesh geometry
std::shared_ptr<sphere::Mesh> bagMesh = sphere::cargo::CargoRegistry::Get().RequestMesh(
    sphere::cargo::CargoID::SUGAR_BAG_50KG, /*lod=*/0
);

// Add to scene graph node
truckBedNode->AttachMesh(bagMesh);
```

---

## 4. How to Add a New Commodity

1. Add the enum value to `library/include/CargoID.h`.
2. Add the human readable name and category mapping to `library/src/CargoID.cpp`.
3. Add a variant configuration struct in the appropriate module's `Variants.h` (e.g. `BaggedGoodsVariants.h`).
4. Implement procedural shape parameters in `*Cargo.cpp`.
5. Rebuild `cargo` shared library via CMake.
