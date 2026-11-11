#pragma once
#include <vulkan/vulkan.h>
#include <string_view>
#include <array>

namespace sphere::textures {

struct TextureAssetEntry {
    const char* id;
    const char* ktx2Path;
    const char* displayName;
    uint32_t width;
    uint32_t height;
    uint32_t mipLevels;
    VkFormat albedoFormat;
    uint32_t bindlessSlot;
    float defaultRoughness;
    float defaultMetallic;
    float defaultIor;
};

struct TextureFormatSpec {
    VkFormat albedoFormat = VK_FORMAT_R8G8B8A8_SRGB;
    VkFormat normalFormat = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormat ormFormat = VK_FORMAT_BC7_UNORM_BLOCK;
    bool generateMips = true;
};

// Registered Khronos Texture 2.0 (KTX2) architectural assets for Sascha Willems Vulkan 1.3 pipeline (65 textures)
inline constexpr std::array<TextureAssetEntry, 65> kRegisteredTextures = {{
    { "box_profile_stainless_steel", "src/shared/materials/metals/textures/box_profile_stainless_steel.ktx2", "Box Profile Stainless Steel (2500mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1000, 0.25f, 0.95f, 2.50f },
    { "square_profile_zinc_cladding", "src/shared/materials/metals/textures/square_profile_zinc_cladding.ktx2", "Square Profile Zinc Cladding (1500mm)", 628, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1010, 0.35f, 0.90f, 2.10f },
    { "corrugated_aluminium", "src/shared/materials/metals/textures/corrugated_aluminium.ktx2", "Corrugated Aluminium Sheet (3000mm)", 628, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1020, 0.30f, 0.95f, 2.30f },
    { "single_bar_diamond_tread_aluminium", "src/shared/materials/metals/textures/single_bar_diamond_tread_aluminium.ktx2", "Single Bar Diamond Tread Aluminium (3000mm)", 628, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1030, 0.28f, 0.95f, 2.35f },
    { "four_bar_diamond_tread_aluminium", "src/shared/materials/metals/textures/four_bar_diamond_tread_aluminium.ktx2", "Four Bar Diamond Tread Aluminium (3000mm)", 628, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1040, 0.28f, 0.95f, 2.35f },
    { "corrugated_patinated_copper", "src/shared/materials/metals/textures/corrugated_patinated_copper.ktx2", "Corrugated Patinated Copper (3000mm)", 892, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1050, 0.52f, 0.45f, 1.88f },
    { "corrugated_rusted_iron", "src/shared/materials/metals/textures/corrugated_rusted_iron.ktx2", "Corrugated Rusted Iron (800mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1060, 0.85f, 0.60f, 2.10f },
    { "corrugated_stainless_steel", "src/shared/materials/metals/textures/corrugated_stainless_steel.ktx2", "Corrugated Stainless Steel (2500mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1070, 0.26f, 0.95f, 2.50f },
    { "rusted_iron_plate", "src/shared/materials/metals/textures/rusted_iron_plate.ktx2", "Weathered Rusted Iron Plate (800mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1080, 0.88f, 0.55f, 2.10f },
    { "rusticated_red_brick", "src/shared/materials/brick/textures/rusticated_red_brick.ktx2", "Rusticated Red Brick (900mm)", 900, 748, 10, VK_FORMAT_R8G8B8A8_SRGB, 1090, 0.85f, 0.00f, 1.53f },
    { "metro_tile_white", "src/shared/materials/brick/textures/metro_tile_white.ktx2", "Metro Tile Stretcher White (1230mm)", 900, 764, 10, VK_FORMAT_R8G8B8A8_SRGB, 1100, 0.12f, 0.00f, 1.54f },
    { "roughcast_render_tan", "src/shared/materials/brick/textures/roughcast_render_tan.ktx2", "Roughcast Render Tan (500mm)", 900, 600, 10, VK_FORMAT_R8G8B8A8_SRGB, 1110, 0.92f, 0.00f, 1.52f },
    { "smoky_brick_grey", "src/shared/materials/brick/textures/smoky_brick_grey.ktx2", "Smoky Brick Staggered Grey (1020mm)", 900, 632, 10, VK_FORMAT_R8G8B8A8_SRGB, 1120, 0.82f, 0.00f, 1.54f },
    { "pilotage_brick_red", "src/shared/materials/brick/textures/pilotage_brick_red.ktx2", "Pilotage Brick Staggered Red (1230mm)", 900, 720, 10, VK_FORMAT_R8G8B8A8_SRGB, 1130, 0.84f, 0.00f, 1.53f },
    { "pilotage_brick_herringbone", "src/shared/materials/brick/textures/pilotage_brick_herringbone.ktx2", "Pilotage Herringbone Brick (734mm)", 800, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1140, 0.84f, 0.00f, 1.53f },
    { "k_slip_magenta_brick", "src/shared/materials/brick/textures/k_slip_magenta_brick.ktx2", "K-Slip Cut Face Gillespie Magenta Brick (675mm)", 900, 796, 10, VK_FORMAT_R8G8B8A8_SRGB, 1150, 0.86f, 0.00f, 1.54f },
    { "porcelain_brick_matte_white", "src/shared/materials/brick/textures/porcelain_brick_matte_white.ktx2", "White Matte Porcelain Brick (765mm)", 900, 564, 10, VK_FORMAT_R8G8B8A8_SRGB, 1160, 0.35f, 0.00f, 1.52f },
    { "burnt_clay_bricks_13", "src/shared/materials/brick/textures/burnt_clay_bricks_13.ktx2", "Burnt Clay Bricks #13 Stretcher (1265mm)", 900, 580, 10, VK_FORMAT_R8G8B8A8_SRGB, 1170, 0.86f, 0.00f, 1.53f },
    { "buff_flemish_brick", "src/shared/materials/brick/textures/buff_flemish_brick.ktx2", "Buff Flemish Facing Brick (675mm)", 672, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1180, 0.84f, 0.00f, 1.54f },
    { "dragfaced_brick_stretcher", "src/shared/materials/brick/textures/dragfaced_brick_stretcher.ktx2", "Dragfaced Brick Stretcher (900mm)", 900, 440, 10, VK_FORMAT_R8G8B8A8_SRGB, 1190, 0.88f, 0.00f, 1.53f },
    { "cream_blend_facing_brick", "src/shared/materials/brick/textures/cream_blend_facing_brick.ktx2", "Cream Blend Facing Brick (1320mm)", 900, 476, 10, VK_FORMAT_R8G8B8A8_SRGB, 1200, 0.82f, 0.00f, 1.53f },
    { "even_drag_brick_flemish", "src/shared/materials/brick/textures/even_drag_brick_flemish.ktx2", "Even Drag Brick Flemish (1410mm)", 900, 764, 10, VK_FORMAT_R8G8B8A8_SRGB, 1210, 0.85f, 0.00f, 1.53f },
    { "even_drag_brick_stretcher", "src/shared/materials/brick/textures/even_drag_brick_stretcher.ktx2", "Even Drag Brick Stretcher (1575mm)", 900, 684, 10, VK_FORMAT_R8G8B8A8_SRGB, 1220, 0.85f, 0.00f, 1.53f },
    { "industrial_brick_common", "src/shared/materials/brick/textures/industrial_brick_common.ktx2", "Industrial Brick Common Bond (940mm)", 900, 688, 10, VK_FORMAT_R8G8B8A8_SRGB, 1230, 0.87f, 0.00f, 1.53f },
    { "crazing_tile_variable_hexagon", "src/shared/materials/brick/textures/crazing_tile_variable_hexagon.ktx2", "Crazing Tile Variable Hexagon (1904mm)", 900, 736, 10, VK_FORMAT_R8G8B8A8_SRGB, 1240, 0.18f, 0.00f, 1.55f },
    { "grey_victorian_tile_stack", "src/shared/materials/brick/textures/grey_victorian_tile_stack.ktx2", "Grey Victorian Encaustic Tile (912mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1250, 0.28f, 0.00f, 1.54f },
    { "basalt_v_pavers", "src/shared/materials/flooring/textures/basalt_v_pavers.ktx2", "Basalt V Pavers Geometric (1594mm)", 692, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1260, 0.78f, 0.00f, 1.56f },
    { "basalt_bowtie_pavers", "src/shared/materials/flooring/textures/basalt_bowtie_pavers.ktx2", "Basalt Bowtie Interlocking Pavers (1597mm)", 900, 824, 10, VK_FORMAT_R8G8B8A8_SRGB, 1270, 0.76f, 0.00f, 1.56f },
    { "concrete_interlocking_pavers", "src/shared/materials/flooring/textures/concrete_interlocking_pavers.ktx2", "Concrete Interlocking Pavers (1597mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1280, 0.74f, 0.00f, 1.53f },
    { "concrete_block_pavers_aggregate", "src/shared/materials/flooring/textures/concrete_block_pavers_aggregate.ktx2", "Concrete Block Pavers Exposed Aggregate (808mm)", 900, 680, 10, VK_FORMAT_R8G8B8A8_SRGB, 1290, 0.82f, 0.00f, 1.53f },
    { "concrete_pavers_stack", "src/shared/materials/flooring/textures/concrete_pavers_stack.ktx2", "Concrete Pavers Stack Bond (3200mm)", 844, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1300, 0.75f, 0.00f, 1.53f },
    { "concrete_v_pavers", "src/shared/materials/flooring/textures/concrete_v_pavers.ktx2", "Concrete V-Pavers Geometric (2410mm)", 900, 776, 10, VK_FORMAT_R8G8B8A8_SRGB, 1310, 0.76f, 0.00f, 1.53f },
    { "grey_concrete_block_paving_staggered", "src/shared/materials/flooring/textures/grey_concrete_block_paving_staggered.ktx2", "Grey Concrete Block Paving Staggered (1228mm)", 900, 688, 10, VK_FORMAT_R8G8B8A8_SRGB, 1320, 0.80f, 0.00f, 1.53f },
    { "dark_scottish_granite_setts", "src/shared/materials/flooring/textures/dark_scottish_granite_setts.ktx2", "Dark Scottish Granite Setts (1212mm)", 900, 464, 10, VK_FORMAT_R8G8B8A8_SRGB, 1330, 0.75f, 0.00f, 1.58f },
    { "scottish_light_grey_granite_setts", "src/shared/materials/flooring/textures/scottish_light_grey_granite_setts.ktx2", "Scottish Light Grey Granite Setts (1760mm)", 900, 560, 10, VK_FORMAT_R8G8B8A8_SRGB, 1340, 0.76f, 0.00f, 1.58f },
    { "scottish_red_granite_staggered", "src/shared/materials/flooring/textures/scottish_red_granite_staggered.ktx2", "Scottish Red Granite Staggered (1240mm)", 900, 696, 10, VK_FORMAT_R8G8B8A8_SRGB, 1350, 0.78f, 0.00f, 1.57f },
    { "red_granite_staggered", "src/shared/materials/flooring/textures/red_granite_staggered.ktx2", "Red Granite Staggered Setts (1248mm)", 900, 700, 10, VK_FORMAT_R8G8B8A8_SRGB, 1360, 0.78f, 0.00f, 1.57f },
    { "flagstone_rubble", "src/shared/materials/flooring/textures/flagstone_rubble.ktx2", "Natural Flagstone Rubble (1948mm)", 900, 648, 10, VK_FORMAT_R8G8B8A8_SRGB, 1370, 0.82f, 0.00f, 1.56f },
    { "larvikite_stack", "src/shared/materials/flooring/textures/larvikite_stack.ktx2", "Larvikite Blue Pearl Granite Stack (1612mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1380, 0.35f, 0.00f, 1.62f },
    { "galletta_terrazzo", "src/shared/materials/flooring/textures/galletta_terrazzo.ktx2", "Galletta Terrazzo Mosaic (1350mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1390, 0.20f, 0.00f, 1.56f },
    { "granite_crazy_paving", "src/shared/materials/flooring/textures/granite_crazy_paving.ktx2", "Granite Crazy Paving Flagstones (1045mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1400, 0.80f, 0.00f, 1.57f },
    { "granite_herringbone", "src/shared/materials/flooring/textures/granite_herringbone.ktx2", "Granite Herringbone Pavers (1822mm)", 664, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1410, 0.75f, 0.00f, 1.57f },
    { "granite_rounded_rubble", "src/shared/materials/flooring/textures/granite_rounded_rubble.ktx2", "Granite Rounded Rubble Stone (749mm)", 900, 816, 10, VK_FORMAT_R8G8B8A8_SRGB, 1420, 0.86f, 0.00f, 1.57f },
    { "flamed_granite_paving", "src/shared/materials/flooring/textures/flamed_granite_paving.ktx2", "Flamed Granite Paving Long Slabs (1208mm)", 900, 676, 10, VK_FORMAT_R8G8B8A8_SRGB, 1430, 0.70f, 0.00f, 1.57f },
    { "drill_marked_granite_drystone", "src/shared/materials/flooring/textures/drill_marked_granite_drystone.ktx2", "Drill Marked Granite Drystone (1118mm)", 900, 688, 10, VK_FORMAT_R8G8B8A8_SRGB, 1440, 0.88f, 0.00f, 1.58f },
    { "sofia_marble_star_hexagon", "src/shared/materials/flooring/textures/sofia_marble_star_hexagon.ktx2", "Sofia Marble Star & Hexagon Inlay (1527mm)", 776, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1450, 0.16f, 0.00f, 1.56f },
    { "oak_flooring_3275", "src/shared/materials/flooring/textures/oak_flooring_3275.ktx2", "European Oak Hardwood Planks (3275mm)", 900, 704, 10, VK_FORMAT_R8G8B8A8_SRGB, 1460, 0.38f, 0.00f, 1.50f },
    { "wenge_chevron_wood", "src/shared/materials/flooring/textures/wenge_chevron_wood.ktx2", "Wenge Chevron Parquet Flooring (1996mm)", 900, 656, 10, VK_FORMAT_R8G8B8A8_SRGB, 1470, 0.32f, 0.00f, 1.51f },
    { "mondo_grass", "src/shared/materials/grass_surface/textures/mondo_grass.ktx2", "Mondo Grass Ground Cover (300mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1480, 0.72f, 0.00f, 1.45f },
    { "mondo_grass_four_leaf", "src/shared/materials/grass_surface/textures/mondo_grass_four_leaf.ktx2", "Mondo Grass Four Leaf Clump (1004mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1490, 0.70f, 0.00f, 1.45f },
    { "mondo_grass_picket_and_square", "src/shared/materials/grass_surface/textures/mondo_grass_picket_and_square.ktx2", "Mondo Grass Picket & Square (810mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1500, 0.72f, 0.00f, 1.45f },
    { "sandy_artificial_grass", "src/shared/materials/grass_surface/textures/sandy_artificial_grass.ktx2", "Sandy Artificial Turf Grass (1498mm)", 692, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1510, 0.65f, 0.00f, 1.46f },
    { "grass_meadow_1000", "src/shared/materials/grass_surface/textures/grass_meadow_1000.ktx2", "Lush Meadow Grass Turf (1000mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1520, 0.68f, 0.00f, 1.45f },
    { "grass_patch_path", "src/shared/materials/grass_surface/textures/grass_patch_path.ktx2", "Grass Patch Stone Stepping Path (1000mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1530, 0.75f, 0.00f, 1.48f },
    { "perennial_ryegrass", "src/shared/materials/grass_surface/textures/perennial_ryegrass.ktx2", "Perennial Ryegrass Lawn (350mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1540, 0.66f, 0.00f, 1.45f },
    { "moss_ground_cover", "src/shared/materials/grass_surface/textures/moss_ground_cover.ktx2", "Forest Cushion Moss (300mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1550, 0.82f, 0.00f, 1.44f },
    { "brown_mud_leaves", "src/shared/materials/dirt/textures/brown_mud_leaves.ktx2", "Brown Mud with Forest Leaves (1300mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1560, 0.88f, 0.00f, 1.49f },
    { "soil_loam_700", "src/shared/materials/dirt/textures/soil_loam_700.ktx2", "Rich Organic Loam Soil (700mm)", 900, 860, 10, VK_FORMAT_R8G8B8A8_SRGB, 1570, 0.92f, 0.00f, 1.50f },
    { "earth_foliage", "src/shared/materials/dirt/textures/earth_foliage.ktx2", "Earth with Foliage (700mm)", 900, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1580, 0.90f, 0.00f, 1.50f },
    { "asphalt_black_stone_aggregate", "src/shared/materials/infrastructure/textures/asphalt_black_stone_aggregate.ktx2", "Thin Asphalt Black Stone Aggregate (450mm)", 900, 600, 10, VK_FORMAT_R8G8B8A8_SRGB, 1590, 0.88f, 0.00f, 1.53f },
    { "crushed_stone_asphalt", "src/shared/materials/infrastructure/textures/crushed_stone_asphalt.ktx2", "Crushed Stone Asphalt Staggered (2423mm)", 900, 776, 10, VK_FORMAT_R8G8B8A8_SRGB, 1600, 0.86f, 0.00f, 1.53f },
    { "terracotta_roman_roof_tile", "src/shared/materials/infrastructure/textures/terracotta_roman_roof_tile.ktx2", "Terracotta Roman Roof Tiles (1180mm)", 900, 884, 10, VK_FORMAT_R8G8B8A8_SRGB, 1610, 0.78f, 0.00f, 1.54f },
    { "bitumen_hexagonal_shingles", "src/shared/materials/infrastructure/textures/bitumen_hexagonal_shingles.ktx2", "Bitumen Hexagonal Roof Shingles (2040mm)", 900, 776, 10, VK_FORMAT_R8G8B8A8_SRGB, 1620, 0.85f, 0.00f, 1.52f },
    { "bitumen_roof_shingles", "src/shared/materials/infrastructure/textures/bitumen_roof_shingles.ktx2", "Bitumen Architectural Roof Shingles (1412mm)", 812, 900, 10, VK_FORMAT_R8G8B8A8_SRGB, 1630, 0.85f, 0.00f, 1.52f },
    { "rough_felt_hexagonal_shingles", "src/shared/materials/infrastructure/textures/rough_felt_hexagonal_shingles.ktx2", "Rough Felt Hexagonal Shingles (2040mm)", 900, 776, 10, VK_FORMAT_R8G8B8A8_SRGB, 1640, 0.88f, 0.00f, 1.52f }
}};

} // namespace sphere::textures
