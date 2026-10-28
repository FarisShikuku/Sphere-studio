#pragma once
#include "../materials/MaterialID.h"
#include "../materials/MaterialDef.h"
#include <string>
#include <vector>

namespace sphere {

enum class SurfacePatternType : uint32_t {
    HOMOGENEOUS = 0,        // Single material coating (e.g. smooth car sheet)
    BRICK_BOND_MASONRY,     // Masonry bricks + recessed mortar joints
    TURF_BLADE_ORGANIC,     // Living grass blades + thatch + underlying soil
    FOLIAGE_SOIL_SPLATMAP,  // Loam substrate + micro-foliage ivy + pebble aggregate
    DUAL_LAYER_CLEARCOAT,   // Conductive metallic base + pigment + smooth dielectric clearcoat
    PLANK_GRAIN_FLOOR,      // Wood grain boards + lacquer finish + joinery gaps
    TILED_GROUT_GRID,       // Vitrified ceramic tiles + porous cementitious grout
    ASPHALT_ROAD_MARKING    // Porous aggregate bitumen + reflective painted markings
};

/**
 * A Surface is a complex composite macro-structure composed of one or more
 * atomic physical Materials blended through spatial masks, layers, and geometry.
 */
struct SurfaceDef {
    std::string id;
    std::string name;
    std::string category;
    std::string description;

    // Component Materials
    MaterialID primaryMaterial = MaterialID::BRICK_RUSTICATED;    // Main body substrate
    MaterialID secondaryMaterial = MaterialID::CONCRETE_RAW;      // Inlay, mortar, or sub-layer
    MaterialID tertiaryMaterial = MaterialID::DIRT_DRY;           // Weathering, soil, or over-layer

    SurfacePatternType pattern = SurfacePatternType::BRICK_BOND_MASONRY;
    SurfaceMode surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;

    // Spatial & Layering parameters
    float primaryRatio = 0.85f;        // Blend weight for primary material
    float secondaryRatio = 0.15f;      // Blend weight for secondary (e.g. 15% mortar)
    float macroTileSizeMeters = 1.0f;  // UV repeat scale in meters
    float surfaceRoughnessBias = 0.0f; // Global weathering roughness offset
    float displacementScale = 0.02f;   // Physical height difference between layers

    // Optional baked asset reference
    std::string assetPath;
};

} // namespace sphere
