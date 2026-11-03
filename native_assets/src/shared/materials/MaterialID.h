#pragma once
#include <cstdint>
#include <string>
namespace sphere {
enum class MaterialID : uint32_t {
    // Architectural & Masonry
    BRICK_RED = 0,
    BRICK_RUSTICATED,
    CONCRETE_RAW,
    CONCRETE_POLISHED,
    TILE_CERAMIC,
    TERRACOTTA_TILE,
    STONE_MARBLE,
    STONE_GRANITE,
    // Nature & Landscapes (Surface vs Geometry)
    GRASS_SURFACE,       // Flat 2D ground surface texture (no geometry blades)
    GRASS_3D_BLADES,     // Procedural 3D instanced blade geometry
    EARTH_FOLIAGE,       // Soil with scattered foliage micro-vegetation
    DIRT_DRY,
    MUD_SLICK,
    GRAVEL_CRUSHED,
    SAND_DUNE,
    BARK_OAK,
    FOLIAGE_LEAF,
    // Metals & Industrial
    METAL_STEEL,
    METAL_BRUSHED,
    METAL_RUSTED,
    RUBBER_SMOOTH,
    PLASTIC_HARD,
    PAINT_GLOSS,
    ASPHALT_WET,
    // Optical & Refractive (Dielectric / Refraction)
    GLASS_CLEAR,
    GLASS_FROSTED,
    ICE_GLACIAL,
    WATER_SURFACE,
    // Organics & Emissives
    LEATHER_GRAINED,
    SKIN_HUMAN,
    EMISSIVE_NEON,
    COUNT
};
enum class SurfaceMode : uint32_t {
    SURFACE_FLAT_TEXTURE = 0, // Flat 2D PBR surface with normal/parallax mapping
    GEOMETRY_3D_INSTANCED = 1  // True 3D procedural or instanced geometry (blades, voxels)
};
const char* GetMaterialName(MaterialID id);
const char* GetMaterialCategory(MaterialID id);
} // namespace spher
