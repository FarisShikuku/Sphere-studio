#!/usr/bin/env python3
import os
import sys
import glob
import re
import shutil
import subprocess
from concurrent.futures import ProcessPoolExecutor

sys.path.insert(0, "native_assets/scripts")
import convert_to_ktx2

TEXTURE_SPECS = [
    # (id, filename_pattern, name, category, roughness, metallic, ior, normalScale, tileScaleMeters, displacementDepthMeters, defaultBaseHex)
    ("box_profile_stainless_steel", "box-profile-stainless-steel", "Box Profile Stainless Steel (2500mm)", "Metals", 0.25, 0.95, 2.50, 1.30, 2.50, 0.035, "0xFFD4D8DC"),
    ("square_profile_zinc_cladding", "square-profile-zinc-cladding", "Square Profile Zinc Cladding (1500mm)", "Metals", 0.35, 0.90, 2.10, 1.20, 1.50, 0.025, "0xFF8E959B"),
    ("corrugated_aluminium", "corrugated-aluminium", "Corrugated Aluminium Sheet (3000mm)", "Metals", 0.30, 0.95, 2.30, 1.35, 3.00, 0.038, "0xFFD8DCDF"),
    ("single_bar_diamond_tread_aluminium", "single-bar-diamond-tread-aluminium", "Single Bar Diamond Tread Aluminium (3000mm)", "Metals", 0.28, 0.95, 2.35, 1.40, 3.00, 0.020, "0xFFC2C7CC"),
    ("four_bar_diamond_tread_aluminium", "four-bar-diamond-tread-aluminium", "Four Bar Diamond Tread Aluminium (3000mm)", "Metals", 0.28, 0.95, 2.35, 1.40, 3.00, 0.020, "0xFFC4C8CD"),
    ("corrugated_patinated_copper", "corrugated-patinated-copper", "Corrugated Patinated Copper (3000mm)", "Metals", 0.52, 0.45, 1.88, 1.30, 3.00, 0.035, "0xFF5BA389"),
    ("corrugated_rusted_iron", "corrugated-rusted-iron", "Corrugated Rusted Iron (800mm)", "Metals", 0.85, 0.60, 2.10, 1.45, 0.80, 0.032, "0xFF8B4B2D"),
    ("corrugated_stainless_steel", "corrugated-stainless-steel", "Corrugated Stainless Steel (2500mm)", "Metals", 0.26, 0.95, 2.50, 1.35, 2.50, 0.038, "0xFFD0D4D8"),
    ("rusted_iron_plate", "rusted-iron-800-mm", "Weathered Rusted Iron Plate (800mm)", "Metals", 0.88, 0.55, 2.10, 1.40, 0.80, 0.024, "0xFF783E22"),

    # Masonry & Bricks
    ("rusticated_red_brick", "rusticated-red-brick", "Rusticated Red Brick (900mm)", "Masonry", 0.85, 0.0, 1.53, 1.25, 0.90, 0.028, "0xFFC0593E"),
    ("metro_tile_white", "metro-tile-stretcher", "Metro Tile Stretcher White (1230mm)", "Masonry", 0.12, 0.0, 1.54, 1.10, 1.23, 0.012, "0xFFE5E7EB"),
    ("roughcast_render_tan", "roughcast-render--tan", "Roughcast Render Tan (500mm)", "Masonry", 0.92, 0.0, 1.52, 1.40, 0.50, 0.022, "0xFFC7B186"),
    ("smoky_brick_grey", "smoky-brick-staggered", "Smoky Brick Staggered Grey (1020mm)", "Masonry", 0.82, 0.0, 1.54, 1.20, 1.02, 0.018, "0xFF7D7F82"),
    ("pilotage_brick_red", "pilotage-staggered", "Pilotage Brick Staggered Red (1230mm)", "Masonry", 0.84, 0.0, 1.53, 1.20, 1.23, 0.016, "0xFFB85942"),
    ("pilotage_brick_herringbone", "pilotage-herringbone", "Pilotage Herringbone Brick (734mm)", "Masonry", 0.84, 0.0, 1.53, 1.25, 0.734, 0.018, "0xFFB2543D"),
    ("k_slip_magenta_brick", "k-slip-cut-face", "K-Slip Cut Face Gillespie Magenta Brick (675mm)", "Masonry", 0.86, 0.0, 1.54, 1.15, 0.675, 0.014, "0xFF8A5364"),
    ("porcelain_brick_matte_white", "white-matte-porcelain-brick", "White Matte Porcelain Brick (765mm)", "Masonry", 0.35, 0.0, 1.52, 0.95, 0.765, 0.010, "0xFFE8E8E6"),
    ("burnt_clay_bricks_13", "burnt-clay-bricks-13", "Burnt Clay Bricks #13 Stretcher (1265mm)", "Masonry", 0.86, 0.0, 1.53, 1.30, 1.265, 0.022, "0xFFA86348"),
    ("buff_flemish_brick", "buff-flemish", "Buff Flemish Facing Brick (675mm)", "Masonry", 0.84, 0.0, 1.54, 1.20, 0.675, 0.018, "0xFFCDB78C"),
    ("dragfaced_brick_stretcher", "dragfaced-brick-stretcher", "Dragfaced Brick Stretcher (900mm)", "Masonry", 0.88, 0.0, 1.53, 1.35, 0.90, 0.024, "0xFFB4573C"),
    ("cream_blend_facing_brick", "cream-blend-facing-brick", "Cream Blend Facing Brick (1320mm)", "Masonry", 0.82, 0.0, 1.53, 1.20, 1.32, 0.018, "0xFFD4BFA4"),
    ("even_drag_brick_flemish", "even-drag-brick-flemish", "Even Drag Brick Flemish (1410mm)", "Masonry", 0.85, 0.0, 1.53, 1.25, 1.41, 0.020, "0xFFB95A40"),
    ("even_drag_brick_stretcher", "even-drag-brick-stretcher", "Even Drag Brick Stretcher (1575mm)", "Masonry", 0.85, 0.0, 1.53, 1.25, 1.575, 0.020, "0xFFBB5D43"),
    ("industrial_brick_common", "industrial-brick-common", "Industrial Brick Common Bond (940mm)", "Masonry", 0.87, 0.0, 1.53, 1.30, 0.94, 0.025, "0xFFA25741"),
    ("crazing_tile_variable_hexagon", "crazing-tile-variable-hexagon", "Crazing Tile Variable Hexagon (1904mm)", "Masonry", 0.18, 0.0, 1.55, 1.05, 1.904, 0.012, "0xFFECECEB"),
    ("grey_victorian_tile_stack", "grey-victorian-tile-stack", "Grey Victorian Encaustic Tile (912mm)", "Masonry", 0.28, 0.0, 1.54, 1.10, 0.912, 0.010, "0xFF8E9094"),

    # Flooring, Pavers & Stone
    ("basalt_v_pavers", "basalt-v-pavers", "Basalt V Pavers Geometric (1594mm)", "Flooring", 0.78, 0.0, 1.56, 1.25, 1.594, 0.025, "0xFF5A5C5E"),
    ("basalt_bowtie_pavers", "basalt-bowtie-pavers", "Basalt Bowtie Interlocking Pavers (1597mm)", "Flooring", 0.76, 0.0, 1.56, 1.20, 1.597, 0.022, "0xFF626466"),
    ("concrete_interlocking_pavers", "concrete-interlocking-rectangle", "Concrete Interlocking Pavers (1597mm)", "Flooring", 0.74, 0.0, 1.53, 1.20, 1.597, 0.020, "0xFF9E9FA1"),
    ("concrete_block_pavers_aggregate", "concrete-block-pavers-with-exposed-aggregates", "Concrete Block Pavers Exposed Aggregate (808mm)", "Flooring", 0.82, 0.0, 1.53, 1.30, 0.808, 0.022, "0xFF8C8E90"),
    ("concrete_pavers_stack", "concrete-pavers-stack", "Concrete Pavers Stack Bond (3200mm)", "Flooring", 0.75, 0.0, 1.53, 1.15, 3.20, 0.018, "0xFF9A9C9E"),
    ("concrete_v_pavers", "concrete-v-pavers", "Concrete V-Pavers Geometric (2410mm)", "Flooring", 0.76, 0.0, 1.53, 1.20, 2.41, 0.022, "0xFF94979A"),
    ("grey_concrete_block_paving_staggered", "grey-concrete-block-paving-with-exposed-aggregate", "Grey Concrete Block Paving Staggered (1228mm)", "Flooring", 0.80, 0.0, 1.53, 1.25, 1.228, 0.020, "0xFF888A8C"),
    ("dark_scottish_granite_setts", "dark-scottish-granite-setts", "Dark Scottish Granite Setts (1212mm)", "Flooring", 0.75, 0.0, 1.58, 1.35, 1.212, 0.028, "0xFF485A6C"),
    ("scottish_light_grey_granite_setts", "scottish-light-grey-granite-setts", "Scottish Light Grey Granite Setts (1760mm)", "Flooring", 0.76, 0.0, 1.58, 1.30, 1.76, 0.026, "0xFFA4AAB0"),
    ("scottish_red_granite_staggered", "scottish-red-granite-staggered", "Scottish Red Granite Staggered (1240mm)", "Flooring", 0.78, 0.0, 1.57, 1.30, 1.24, 0.025, "0xFFB57062"),
    ("red_granite_staggered", "red-granite-staggered", "Red Granite Staggered Setts (1248mm)", "Flooring", 0.78, 0.0, 1.57, 1.30, 1.248, 0.025, "0xFFA86456"),
    ("flagstone_rubble", "flagstone-rubble", "Natural Flagstone Rubble (1948mm)", "Flooring", 0.82, 0.0, 1.56, 1.40, 1.948, 0.035, "0xFF7E7D7A"),
    ("larvikite_stack", "larvikite-stack", "Larvikite Blue Pearl Granite Stack (1612mm)", "Flooring", 0.35, 0.0, 1.62, 1.10, 1.612, 0.015, "0xFF46505C"),
    ("galletta_terrazzo", "galletta-terrazzo", "Galletta Terrazzo Mosaic (1350mm)", "Flooring", 0.20, 0.0, 1.56, 0.80, 1.35, 0.008, "0xFFE2DDD7"),
    ("granite_crazy_paving", "granite-crazy-paving", "Granite Crazy Paving Flagstones (1045mm)", "Flooring", 0.80, 0.0, 1.57, 1.35, 1.045, 0.030, "0xFF828384"),
    ("granite_herringbone", "granite-herringbone", "Granite Herringbone Pavers (1822mm)", "Flooring", 0.75, 0.0, 1.57, 1.25, 1.822, 0.024, "0xFF888B8E"),
    ("granite_rounded_rubble", "granite-rounded-rubble", "Granite Rounded Rubble Stone (749mm)", "Flooring", 0.86, 0.0, 1.57, 1.45, 0.749, 0.040, "0xFF6C6964"),
    ("flamed_granite_paving", "flamed-granite-paving", "Flamed Granite Paving Long Slabs (1208mm)", "Flooring", 0.70, 0.0, 1.57, 1.20, 1.208, 0.018, "0xFF8A8C8E"),
    ("drill_marked_granite_drystone", "drill-marked-granite-drystone", "Drill Marked Granite Drystone (1118mm)", "Flooring", 0.88, 0.0, 1.58, 1.50, 1.118, 0.045, "0xFF7B7D7E"),
    ("sofia_marble_star_hexagon", "sofia-marble-star-and-hexagon", "Sofia Marble Star & Hexagon Inlay (1527mm)", "Flooring", 0.16, 0.0, 1.56, 0.90, 1.527, 0.010, "0xFFECE4D8"),
    ("oak_flooring_3275", "oak-3275-mm", "European Oak Hardwood Planks (3275mm)", "Flooring", 0.38, 0.0, 1.50, 0.90, 3.275, 0.012, "0xFF9E7247"),
    ("wenge_chevron_wood", "wenge-chevron", "Wenge Chevron Parquet Flooring (1996mm)", "Flooring", 0.32, 0.0, 1.51, 0.95, 1.996, 0.010, "0xFF3A2E2A"),

    # Nature & Grounds
    ("mondo_grass", "mondo-grass-300-mm", "Mondo Grass Ground Cover (300mm)", "Nature", 0.72, 0.0, 1.45, 0.85, 0.30, 0.012, "0xFF426E2D"),
    ("mondo_grass_four_leaf", "mondo-grass-four-leaf", "Mondo Grass Four Leaf Clump (1004mm)", "Nature", 0.70, 0.0, 1.45, 0.90, 1.004, 0.015, "0xFF4B7A32"),
    ("mondo_grass_picket_and_square", "mondo-grass-picket-and-square", "Mondo Grass Picket & Square (810mm)", "Nature", 0.72, 0.0, 1.45, 0.90, 0.81, 0.014, "0xFF477230"),
    ("sandy_artificial_grass", "sandy-artificial-grass", "Sandy Artificial Turf Grass (1498mm)", "Nature", 0.65, 0.0, 1.46, 0.80, 1.498, 0.010, "0xFF688A3E"),
    ("grass_meadow_1000", "grass-1000-mm", "Lush Meadow Grass Turf (1000mm)", "Nature", 0.68, 0.0, 1.45, 0.90, 1.00, 0.016, "0xFF747848"),
    ("grass_patch_path", "grass-patch-path", "Grass Patch Stone Stepping Path (1000mm)", "Nature", 0.75, 0.0, 1.48, 1.10, 1.00, 0.022, "0xFF586F3E"),
    ("perennial_ryegrass", "perennial-ryegrass", "Perennial Ryegrass Lawn (350mm)", "Nature", 0.66, 0.0, 1.45, 0.85, 0.35, 0.014, "0xFF468428"),
    ("moss_ground_cover", "moss-300-mm", "Forest Cushion Moss (300mm)", "Nature", 0.82, 0.0, 1.44, 1.15, 0.30, 0.020, "0xFF436528"),
    ("brown_mud_leaves", "brown-mud-leaves", "Brown Mud with Forest Leaves (1300mm)", "Nature", 0.88, 0.0, 1.49, 1.30, 1.30, 0.035, "0xFF655433"),
    ("coast_sand_800", "coast-sand-800-mm", "Coast Sand & Shell Aggregate (800mm)", "Nature", 0.88, 0.0, 1.54, 1.30, 0.80, 0.024, "0xFF83745D"),
    ("soil_loam_700", "soil-700-mm", "Rich Organic Loam Soil (700mm)", "Nature", 0.92, 0.0, 1.50, 1.35, 0.70, 0.030, "0xFF5E4B37"),
    ("earth_foliage", "earth-with-foliage", "Earth with Foliage (700mm)", "Nature", 0.90, 0.0, 1.50, 1.15, 0.70, 0.032, "0xFF64533C"),

    # Infrastructure & Roofing
    ("asphalt_black_stone_aggregate", "asphalt-with-embedded-black-stone", "Thin Asphalt Black Stone Aggregate (450mm)", "Infrastructure", 0.88, 0.0, 1.53, 1.30, 0.45, 0.016, "0xFF2D2E30"),
    ("crushed_stone_asphalt", "crushed-stone-asphalt", "Crushed Stone Asphalt Staggered (2423mm)", "Infrastructure", 0.86, 0.0, 1.53, 1.35, 2.423, 0.024, "0xFF525458"),
    ("terracotta_roman_roof_tile", "terracotta-roman-roof-tile", "Terracotta Roman Roof Tiles (1180mm)", "Infrastructure", 0.78, 0.0, 1.54, 1.40, 1.18, 0.040, "0xFFC46A4F"),
    ("bitumen_hexagonal_shingles", "bitumen-hexagonal-shingles", "Bitumen Hexagonal Roof Shingles (2040mm)", "Infrastructure", 0.85, 0.0, 1.52, 1.25, 2.04, 0.018, "0xFF424446"),
    ("bitumen_roof_shingles", "bitumen-roof-shingles", "Bitumen Architectural Roof Shingles (1412mm)", "Infrastructure", 0.85, 0.0, 1.52, 1.25, 1.412, 0.018, "0xFF484A4D"),
    ("rough_felt_hexagonal_shingles", "rough-felt-hexagonal-shingles", "Rough Felt Hexagonal Shingles (2040mm)", "Infrastructure", 0.88, 0.0, 1.52, 1.30, 2.04, 0.020, "0xFF4D4E50"),

    # Roads & Highways
    ("road_highway_two_lane", "asphalt_texture_seamless_by_rfalworth", "Highway Two-Lane (Yellow Edge & White Dash)", "Roads", 0.84, 0.0, 1.53, 1.30, 7.40, 0.018, "0xFF272520"),
    ("road_two_lane_white_dash", "road_variety_texture_tileable_by_hhh316", "Two-Lane Paved Highway (White Shoulders)", "Roads", 0.88, 0.0, 1.53, 1.25, 7.20, 0.018, "0xFF77716B"),
    ("road_double_yellow_line", "cumdyl_by_slc_world", "Urban Two-Way Road (Double Yellow Center)", "Roads", 0.89, 0.0, 1.53, 1.38, 6.80, 0.022, "0xFF514E4B"),
    ("road_highway_yellow_edge", "dbmwluryl_by_slc_world", "Multi-Lane Highway Segment (White Dash & Yellow Edge)", "Roads", 0.90, 0.0, 1.53, 1.42, 7.50, 0.024, "0xFF4E4C4A"),
    ("road_cracked_urban_unmarked", "cua_by_slc_world", "Cracked Urban Asphalt (Unmarked Lane)", "Roads", 0.91, 0.0, 1.53, 1.40, 4.00, 0.022, "0xFF4D4C4B"),
]

def find_source_file(pattern):
    matches = glob.glob(f"public/assets/aistudio/*{pattern}*.*")
    # Exclude screenshots
    matches = [m for m in matches if not os.path.basename(m).startswith("screenshot")]
    if not matches:
        return None
    return sorted(matches)[0]

def convert_single_texture(item):
    tex_id, pattern, name, category, roughness, metallic, ior, normalScale, tileScaleMeters, displacementDepthMeters, defaultBaseHex = item
    src_path = find_source_file(pattern)
    if not src_path:
        print(f"[WARN] No source file found for pattern: {pattern}")
        return None
    
    out_ktx2_textures = f"textures/{tex_id}.ktx2"
    out_ktx2_native = f"native_assets/textures/{tex_id}.ktx2"
    
    # Check if already generated and valid
    if os.path.exists(out_ktx2_textures) and os.path.getsize(out_ktx2_textures) > 1000:
        if not os.path.exists(out_ktx2_native):
            shutil.copy2(out_ktx2_textures, out_ktx2_native)
        return (tex_id, out_ktx2_textures, src_path)
    
    os.makedirs("textures", exist_ok=True)
    os.makedirs("native_assets/textures", exist_ok=True)
    
    convert_to_ktx2.convert_image_to_ktx2(src_path, out_ktx2_textures, max_dim=1024, is_srgb=True)
    shutil.copy2(out_ktx2_textures, out_ktx2_native)
    return (tex_id, out_ktx2_textures, src_path)

def main():
    print(f"Total texture specs to process: {len(TEXTURE_SPECS)}")
    
    # Clean app/src/main/assets/ if any textures exist there to avoid mergeDebugAssets collision
    if os.path.exists("app/src/main/assets"):
        for f in os.listdir("app/src/main/assets"):
            p = os.path.join("app/src/main/assets", f)
            if os.path.isfile(p):
                os.remove(p)
                print(f"Removed redundant asset from app/src/main/assets: {f}")
                
    # Run conversion in parallel
    with ProcessPoolExecutor(max_workers=4) as executor:
        results = list(executor.map(convert_single_texture, TEXTURE_SPECS))
        
    print(f"Processed {len([r for r in results if r])} textures successfully.")

if __name__ == "__main__":
    main()
