#include "MaterialDef.h"

namespace sphere {

const char* GetMaterialName(MaterialID id) {
    switch (id) {
        case MaterialID::BRICK_RED: return "Red Clay Brick";
        case MaterialID::BRICK_RUSTICATED: return "Rusticated Red Brick (900mm)";
        case MaterialID::CONCRETE_RAW: return "Raw Cast Concrete";
        case MaterialID::CONCRETE_POLISHED: return "Polished Architectural Concrete";
        case MaterialID::TILE_CERAMIC: return "Glazed Ceramic Tile";
        case MaterialID::TERRACOTTA_TILE: return "Terracotta Roof Tile";
        case MaterialID::STONE_MARBLE: return "Polished Carrara Marble";
        case MaterialID::STONE_GRANITE: return "Rough Cut Granite";
        case MaterialID::GRASS_SURFACE: return "Mondo Grass (Surface Only)";
        case MaterialID::GRASS_3D_BLADES: return "Mondo Grass (3D Instanced Blades)";
        case MaterialID::EARTH_FOLIAGE: return "Earth with Foliage (700mm)";
        case MaterialID::DIRT_DRY: return "Compacted Dry Soil";
        case MaterialID::MUD_SLICK: return "Wet Clay Mud";
        case MaterialID::GRAVEL_CRUSHED: return "Crushed Basalt Gravel";
        case MaterialID::SAND_DUNE: return "Fine Desert Sand";
        case MaterialID::BARK_OAK: return "Rough Oak Bark";
        case MaterialID::FOLIAGE_LEAF: return "Deciduous Leaf Foliage";
        case MaterialID::METAL_STEEL: return "Structural Steel";
        case MaterialID::METAL_BRUSHED: return "Brushed Anodized Aluminum";
        case MaterialID::METAL_RUSTED: return "Corroded Weathered Iron";
        case MaterialID::RUBBER_SMOOTH: return "Vulcanized Smooth Rubber";
        case MaterialID::PLASTIC_HARD: return "Hard Matte ABS Plastic";
        case MaterialID::PAINT_GLOSS: return "Automotive Gloss Clearcoat";
        case MaterialID::ASPHALT_WET: return "Rain-Slicked Asphalt";
        case MaterialID::GLASS_CLEAR: return "Refractive Architectural Glass";
        case MaterialID::GLASS_FROSTED: return "Frosted Diffuse Glass";
        case MaterialID::ICE_GLACIAL: return "Subsurface Glacial Ice";
        case MaterialID::WATER_SURFACE: return "Ocean & Pool Water Surface";
        case MaterialID::LEATHER_GRAINED: return "Full-Grain Saddle Leather";
        case MaterialID::SKIN_HUMAN: return "Subsurface Dermal Skin";
        case MaterialID::EMISSIVE_NEON: return "High-Output Neon Emissive";
        default: return "Custom PBR Material";
    }
}

const char* GetMaterialCategory(MaterialID id) {
    switch (id) {
        case MaterialID::BRICK_RED:
        case MaterialID::BRICK_RUSTICATED:
        case MaterialID::CONCRETE_RAW:
        case MaterialID::CONCRETE_POLISHED:
        case MaterialID::TILE_CERAMIC:
        case MaterialID::TERRACOTTA_TILE:
        case MaterialID::STONE_MARBLE:
        case MaterialID::STONE_GRANITE:
            return "Masonry & Architecture";

        case MaterialID::GRASS_SURFACE:
        case MaterialID::GRASS_3D_BLADES:
        case MaterialID::EARTH_FOLIAGE:
        case MaterialID::DIRT_DRY:
        case MaterialID::MUD_SLICK:
        case MaterialID::GRAVEL_CRUSHED:
        case MaterialID::SAND_DUNE:
        case MaterialID::BARK_OAK:
        case MaterialID::FOLIAGE_LEAF:
            return "Nature & Ground Surfaces";

        case MaterialID::METAL_STEEL:
        case MaterialID::METAL_BRUSHED:
        case MaterialID::METAL_RUSTED:
        case MaterialID::RUBBER_SMOOTH:
        case MaterialID::PLASTIC_HARD:
        case MaterialID::PAINT_GLOSS:
        case MaterialID::ASPHALT_WET:
            return "Metals & Polymers";

        case MaterialID::GLASS_CLEAR:
        case MaterialID::GLASS_FROSTED:
        case MaterialID::ICE_GLACIAL:
        case MaterialID::WATER_SURFACE:
            return "Optical & Refractive";

        default:
            return "Specialized & Organics";
    }
}

} // namespace sphere
