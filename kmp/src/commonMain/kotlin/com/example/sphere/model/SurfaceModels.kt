package com.example.sphere.model

import androidx.compose.ui.graphics.Color

enum class SurfaceCategory(val label: String) {
    ARCHITECTURAL("Architectural & Walls"),
    NATURE("Nature & Landscapes"),
    TRANSPORTATION("Vehicles & Transport"),
    INTERIOR("Interiors & Flooring"),
    INFRASTRUCTURE("Pathways & Roads")
}

enum class SurfacePatternType(val label: String, val description: String) {
    HOMOGENEOUS("Uniform Coating", "Single material substrate with uniform coverage"),
    BRICK_BOND_MASONRY("Brick & Mortar", "Masonry blocks arranged in stretcher bond with recessed mortar joints"),
    BRICK_HERRINGBONE("Herringbone Brick", "45-degree interlocking terracotta bricks with sandy cement mortar joints"),
    GRASS_PAVER_FOUR_LEAF("Four-Leaf Clover Paver", "Perforated concrete paver grid with four-leaf clover lawn grass insets"),
    GRASS_PAVER_PICKET_SQUARE("Picket & Square Paver", "Permeable octagonal concrete pavers with square turf lawn insets"),
    ASPHALT_AGGREGATE_MACADAM("Asphalt & Black Aggregate", "Dense bitumen asphalt with embedded black basalt aggregate and micro-pores"),
    PAVER_INTERLOCKING_GEOMETRIC("Interlocking Pavers", "Geometric interlocking pavers (V-paver, bowtie, rectangle) with sand joints"),
    STONE_RUBBLE_FLAGSTONE("Flagstone Rubble", "Natural flagstone slabs with irregular mortar seams"),
    GRANITE_SETTS_COBBLE("Granite Setts", "Hand-dressed rectangular granite cobblestones in staggered courses"),
    TERRAZZO_MOSAIC_CHIPS("Terrazzo & Inlay", "Polished composite terrazzo with mineral marble chips and geometric inlays"),
    CHEVRON_PARQUET_WOOD("Chevron Parquet", "Angle-cut hardwood parquet blocks in classic chevron pattern"),
    ROOF_SHINGLE_SCALLOP("Roof Shingles & Tiles", "Overlapping hexagonal bitumen shingles or curved Roman terracotta tiles"),
    METAL_CORRUGATED_RIBS("Corrugated & Ribbed Metal", "Profiled metal cladding sheets with linear flutes and shadow ribs"),
    METAL_DIAMOND_TREAD("Diamond Tread Plate", "High-traction metal plate with raised directional diamond lugs"),
    ORGANIC_SOIL_LOAM("Organic Soil & Earth", "Damp rich loam with scattered mineral pebbles and decomposed foliage"),
    TURF_BLADE_ORGANIC("Lawn Turf", "Live grass blades, root thatch layer, and underlying soil substrate"),
    FOLIAGE_SOIL_SPLATMAP("Earth & Foliage", "Organic loam substrate supporting scattered micro-foliage and pebbles"),
    DUAL_LAYER_CLEARCOAT("Multi-Layer Paint", "Base metal + color pigment basecoat + high-gloss clearcoat reflection"),
    PLANK_GRAIN_FLOOR("Wood Planks", "Timber grain planks with polyurethane varnish and joinery seams"),
    TILED_GROUT_GRID("Ceramic Tiles", "Glazed vitrified tiles surrounded by porous cementitious grout lines"),
    ASPHALT_ROAD_MARKING("Pathway & Road", "Bitumen aggregate asphalt with reflective painted traffic markings")
}

data class SurfaceLayer(
    val role: String,               // "Primary Substrate", "Joint / Mortar", "Top Clearcoat", "Substrate Soil"
    val material: PbrMaterial,      // The pure physical material used in this layer
    val coveragePercent: Float,     // Percentage of total surface area (0 - 100%)
    val isEnabled: Boolean = true   // Can be toggled on/off to isolate layers
)

data class CompositeSurface(
    val id: String,
    val name: String,
    val category: SurfaceCategory,
    val description: String,
    val patternType: SurfacePatternType,
    val layers: List<SurfaceLayer>,
    val macroTileMeters: Float = 1.0f,
    val displacementScaleMeters: Float = 0.02f,
    val surfaceMode: MaterialSurfaceMode = MaterialSurfaceMode.SURFACE_ONLY,
    val assetPath: String? = null
)

private fun getMaterial(id: String): PbrMaterial =
    defaultPbrMaterials.firstOrNull { it.id == id }
        ?: defaultPbrMaterials.firstOrNull { it.id == "rusticated_red_brick" && id == "brick_rusticated" }
        ?: defaultPbrMaterials.firstOrNull { it.id == "mondo_grass" && id == "grass_surface" }
        ?: defaultPbrMaterials.first()

val defaultCompositeSurfaces = listOf(
    // 1. Rusticated Red Brick Wall
    CompositeSurface(
        id = "brick_wall",
        name = "Rusticated Red Brick Wall",
        category = SurfaceCategory.ARCHITECTURAL,
        description = "Composite masonry: rough clay stretcher bricks bound with recessed sandy mortar joints.",
        patternType = SurfacePatternType.BRICK_BOND_MASONRY,
        macroTileMeters = 0.90f,
        displacementScaleMeters = 0.028f,
        assetPath = "src/shared/materials/brick/textures/rusticated_red_brick.ktx2",
        layers = listOf(
            SurfaceLayer("Clay Brick Blocks", getMaterial("rusticated_red_brick"), 86f),
            SurfaceLayer("Cement Mortar Joints", getMaterial("concrete_polished").copy(name = "Sandy Cement Mortar", baseColor = Color(0xFFAFAFA8), roughness = 0.92f), 14f)
        )
    ),

    // 2. Pilotage Herringbone Brick (User Actual Shared Image 1)
    CompositeSurface(
        id = "pilotage_brick_herringbone",
        name = "Pilotage Herringbone Brick Surface (734mm)",
        category = SurfaceCategory.ARCHITECTURAL,
        description = "45-degree herringbone paving: rustic terracotta pavers with sandy cement joint mortar.",
        patternType = SurfacePatternType.BRICK_HERRINGBONE,
        macroTileMeters = 0.734f,
        displacementScaleMeters = 0.018f,
        assetPath = "src/shared/materials/brick/textures/pilotage_brick_herringbone.ktx2",
        layers = listOf(
            SurfaceLayer("Terracotta Herringbone Pavers", getMaterial("pilotage_brick_herringbone"), 88f),
            SurfaceLayer("Recessed Sand Mortar", getMaterial("concrete_polished").copy(name = "Sand Mortar Bed", baseColor = Color(0xFFC0B298), roughness = 0.94f), 12f)
        )
    ),

    // 3. Mondo Grass Four Leaf Clover Paver (User Actual Shared Image 2)
    CompositeSurface(
        id = "mondo_grass_four_leaf",
        name = "Mondo Grass Four-Leaf Clover Paver (1004mm)",
        category = SurfaceCategory.NATURE,
        description = "Perforated concrete paver grid with four-leaf clover apertures holding lush lawn grass.",
        patternType = SurfacePatternType.GRASS_PAVER_FOUR_LEAF,
        macroTileMeters = 1.004f,
        displacementScaleMeters = 0.015f,
        assetPath = "src/shared/materials/grass_surface/textures/mondo_grass_four_leaf.ktx2",
        layers = listOf(
            SurfaceLayer("Concrete Paver Grid", getMaterial("concrete_polished").copy(name = "Light Grey Concrete Frame", baseColor = Color(0xFFC6C8CA), roughness = 0.82f), 58f),
            SurfaceLayer("Four-Leaf Lawn Grass", getMaterial("mondo_grass_four_leaf"), 42f)
        )
    ),

    // 4. Mondo Grass Picket & Square Paver (User Actual Shared Image 3)
    CompositeSurface(
        id = "mondo_grass_picket_and_square",
        name = "Mondo Grass Picket & Square Paver (810mm)",
        category = SurfaceCategory.NATURE,
        description = "Permeable octagonal concrete pavers with square turf inserts for storm water drainage.",
        patternType = SurfacePatternType.GRASS_PAVER_PICKET_SQUARE,
        macroTileMeters = 0.81f,
        displacementScaleMeters = 0.014f,
        assetPath = "src/shared/materials/grass_surface/textures/mondo_grass_picket_and_square.ktx2",
        layers = listOf(
            SurfaceLayer("Octagonal Picket Pavers", getMaterial("concrete_polished").copy(name = "Concrete Picket Pavers", baseColor = Color(0xFFCCD0D2), roughness = 0.80f), 62f),
            SurfaceLayer("Square Lawn Grass Inserts", getMaterial("mondo_grass_picket_and_square"), 38f)
        )
    ),

    // 5. Thin Asphalt Black Stone Aggregate (User Screenshot Selected Surface)
    CompositeSurface(
        id = "asphalt_black_stone_aggregate",
        name = "Thin Asphalt Black Stone Aggregate (450mm)",
        category = SurfaceCategory.INFRASTRUCTURE,
        description = "Fine-graded bitumen asphalt with embedded black basalt stone chips and high-friction microtexture.",
        patternType = SurfacePatternType.ASPHALT_AGGREGATE_MACADAM,
        macroTileMeters = 0.45f,
        displacementScaleMeters = 0.016f,
        assetPath = "src/shared/materials/infrastructure/textures/asphalt_black_stone_aggregate.ktx2",
        layers = listOf(
            SurfaceLayer("Bituminous Asphalt Matrix", getMaterial("asphalt_black_stone_aggregate"), 78f),
            SurfaceLayer("Black Basalt Chips", getMaterial("concrete_polished").copy(name = "Basalt Stone Aggregate", baseColor = Color(0xFF1E2022), roughness = 0.86f), 22f)
        )
    ),

    // 6. Mondo Grass Lawn Turf
    CompositeSurface(
        id = "grass_turf",
        name = "Mondo Grass Lawn Surface",
        category = SurfaceCategory.NATURE,
        description = "Composite ground surface: high-density green grass blades mixed with yellow root thatch.",
        patternType = SurfacePatternType.TURF_BLADE_ORGANIC,
        macroTileMeters = 0.30f,
        displacementScaleMeters = 0.012f,
        assetPath = "src/shared/materials/grass_surface/textures/mondo_grass.ktx2",
        layers = listOf(
            SurfaceLayer("Living Grass Blades", getMaterial("mondo_grass"), 72f),
            SurfaceLayer("Thatch Root Layer", getMaterial("earth_foliage").copy(name = "Dry Yellow Thatch", baseColor = Color(0xFF887D42), roughness = 0.85f), 18f),
            SurfaceLayer("Soil Topsoil", getMaterial("earth_foliage").copy(name = "Organic Topsoil", baseColor = Color(0xFF382A1A), roughness = 0.95f), 10f)
        )
    ),

    // 7. Earth with Foliage Surface
    CompositeSurface(
        id = "earth_foliage",
        name = "Earth with Foliage Surface",
        category = SurfaceCategory.NATURE,
        description = "Composite terrain: nutrient-rich soil substrate supporting creeping broadleaf foliage and stone aggregate.",
        patternType = SurfacePatternType.FOLIAGE_SOIL_SPLATMAP,
        macroTileMeters = 0.70f,
        displacementScaleMeters = 0.032f,
        assetPath = "src/shared/materials/dirt/textures/earth_foliage.ktx2",
        layers = listOf(
            SurfaceLayer("Soil Substrate", getMaterial("earth_foliage").copy(name = "Moist Organic Loam", baseColor = Color(0xFF4A3B2B), roughness = 0.92f), 55f),
            SurfaceLayer("Creeping Ivy Foliage", getMaterial("mondo_grass").copy(name = "Broadleaf Micro-Foliage", baseColor = Color(0xFF5B7828), subsurface = 0.5f), 35f),
            SurfaceLayer("Basalt Pebbles", getMaterial("concrete_polished").copy(name = "Basalt Stone Aggregate", baseColor = Color(0xFF6B6E72), roughness = 0.6f), 10f)
        )
    ),

    // 8. Basalt V-Pavers Geometric
    CompositeSurface(
        id = "basalt_v_pavers",
        name = "Basalt V Pavers Geometric (1594mm)",
        category = SurfaceCategory.INTERIOR,
        description = "Directional chevron V-shaped volcanic basalt pavers with chamfered drainage joints.",
        patternType = SurfacePatternType.PAVER_INTERLOCKING_GEOMETRIC,
        macroTileMeters = 1.594f,
        displacementScaleMeters = 0.025f,
        assetPath = "src/shared/materials/flooring/textures/basalt_v_pavers.ktx2",
        layers = listOf(
            SurfaceLayer("Basalt V-Blocks", getMaterial("basalt_v_pavers"), 90f),
            SurfaceLayer("Joint Sand", getMaterial("concrete_polished").copy(name = "Polymeric Joint Sand", baseColor = Color(0xFF454648), roughness = 0.95f), 10f)
        )
    ),

    // 9. Natural Flagstone Rubble
    CompositeSurface(
        id = "flagstone_rubble",
        name = "Natural Flagstone Rubble (1948mm)",
        category = SurfaceCategory.INTERIOR,
        description = "Irregular split-face flagstone slabs with randomized mortar seams and rustic texture.",
        patternType = SurfacePatternType.STONE_RUBBLE_FLAGSTONE,
        macroTileMeters = 1.948f,
        displacementScaleMeters = 0.035f,
        assetPath = "src/shared/materials/flooring/textures/flagstone_rubble.ktx2",
        layers = listOf(
            SurfaceLayer("Quarried Flagstone", getMaterial("flagstone_rubble"), 85f),
            SurfaceLayer("Mortar Joint Bed", getMaterial("concrete_polished").copy(name = "Recessed Cement Grout", baseColor = Color(0xFF9E9A92), roughness = 0.92f), 15f)
        )
    ),

    // 10. Galletta Terrazzo Mosaic
    CompositeSurface(
        id = "galletta_terrazzo",
        name = "Galletta Terrazzo Mosaic (1350mm)",
        category = SurfaceCategory.INTERIOR,
        description = "Cast terrazzo surface: honed marble and quartz aggregate chips in an ivory matrix.",
        patternType = SurfacePatternType.TERRAZZO_MOSAIC_CHIPS,
        macroTileMeters = 1.35f,
        displacementScaleMeters = 0.008f,
        assetPath = "src/shared/materials/flooring/textures/galletta_terrazzo.ktx2",
        layers = listOf(
            SurfaceLayer("Honed Terrazzo Matrix", getMaterial("galletta_terrazzo"), 85f),
            SurfaceLayer("Marble Chips Flecks", getMaterial("concrete_polished").copy(name = "Cararra & Amber Chips", baseColor = Color(0xFFE8E0D2), roughness = 0.18f), 15f)
        )
    ),

    // 11. Wenge Chevron Parquet Flooring
    CompositeSurface(
        id = "wenge_chevron_wood",
        name = "Wenge Chevron Parquet Flooring (1996mm)",
        category = SurfaceCategory.INTERIOR,
        description = "Exotic dark wenge timber parquet blocks arranged in 45-degree chevron herringbone.",
        patternType = SurfacePatternType.CHEVRON_PARQUET_WOOD,
        macroTileMeters = 1.996f,
        displacementScaleMeters = 0.010f,
        assetPath = "src/shared/materials/flooring/textures/wenge_chevron_wood.ktx2",
        layers = listOf(
            SurfaceLayer("Wenge Hardwood", getMaterial("wenge_chevron_wood"), 92f),
            SurfaceLayer("Polyurethane Satin Lacquer", getMaterial("glass_frosted").copy(name = "Protective Lacquer", roughness = 0.22f), 100f),
            SurfaceLayer("Chamfer Bevel Seams", getMaterial("concrete_polished").copy(name = "Shadow Seams", baseColor = Color(0xFF1E1715), roughness = 0.98f), 8f)
        )
    ),

    // 12. Bitumen Hexagonal Roof Shingles
    CompositeSurface(
        id = "bitumen_hexagonal_shingles",
        name = "Bitumen Hexagonal Roof Shingles (2040mm)",
        category = SurfaceCategory.INFRASTRUCTURE,
        description = "Overlapping multi-tab hexagonal bitumen asphalt shingles with mineral granule coating.",
        patternType = SurfacePatternType.ROOF_SHINGLE_SCALLOP,
        macroTileMeters = 2.04f,
        displacementScaleMeters = 0.018f,
        assetPath = "src/shared/materials/infrastructure/textures/bitumen_hexagonal_shingles.ktx2",
        layers = listOf(
            SurfaceLayer("Hexagonal Shingle Tabs", getMaterial("bitumen_hexagonal_shingles"), 88f),
            SurfaceLayer("Underlay Felt Shadow", getMaterial("asphalt_black_stone_aggregate").copy(name = "Bitumen Underlayment", baseColor = Color(0xFF242628), roughness = 0.95f), 12f)
        )
    ),

    // 13. Box Profile Stainless Steel Cladding
    CompositeSurface(
        id = "box_profile_stainless_steel",
        name = "Box Profile Stainless Steel (2500mm)",
        category = SurfaceCategory.ARCHITECTURAL,
        description = "Engineered trapezoidal box profile architectural wall cladding in brushed stainless steel.",
        patternType = SurfacePatternType.METAL_CORRUGATED_RIBS,
        macroTileMeters = 2.50f,
        displacementScaleMeters = 0.035f,
        assetPath = "src/shared/materials/metals/textures/box_profile_stainless_steel.ktx2",
        layers = listOf(
            SurfaceLayer("Stainless Steel Face", getMaterial("box_profile_stainless_steel"), 90f),
            SurfaceLayer("Recessed Rib Shadows", getMaterial("box_profile_stainless_steel").copy(name = "Rib Valley Shadow", roughness = 0.45f), 10f)
        )
    ),

    // 14. Single Bar Diamond Tread Aluminium
    CompositeSurface(
        id = "single_bar_diamond_tread_aluminium",
        name = "Single Bar Diamond Tread Aluminium (3000mm)",
        category = SurfaceCategory.ARCHITECTURAL,
        description = "Industrial safety diamond plate with raised directional embossed aluminium lugs.",
        patternType = SurfacePatternType.METAL_DIAMOND_TREAD,
        macroTileMeters = 3.00f,
        displacementScaleMeters = 0.020f,
        assetPath = "src/shared/materials/metals/textures/single_bar_diamond_tread_aluminium.ktx2",
        layers = listOf(
            SurfaceLayer("Aluminium Base Plate", getMaterial("single_bar_diamond_tread_aluminium"), 82f),
            SurfaceLayer("Embossed Diamond Lugs", getMaterial("single_bar_diamond_tread_aluminium").copy(name = "Raised Traction Lugs", roughness = 0.18f), 18f)
        )
    ),

    // 15. Rich Organic Loam Soil
    CompositeSurface(
        id = "soil_loam_700",
        name = "Rich Organic Loam Soil (700mm)",
        category = SurfaceCategory.NATURE,
        description = "Damp nutrient-rich organic loam soil with humus aggregate and micro-gravel.",
        patternType = SurfacePatternType.ORGANIC_SOIL_LOAM,
        macroTileMeters = 0.70f,
        displacementScaleMeters = 0.030f,
        assetPath = "src/shared/materials/dirt/textures/soil_loam_700.ktx2",
        layers = listOf(
            SurfaceLayer("Organic Loam Topsoil", getMaterial("soil_loam_700"), 85f),
            SurfaceLayer("Micro Pebbles & Gravel", getMaterial("concrete_polished").copy(name = "Silica Gravel", baseColor = Color(0xFF7E7262), roughness = 0.75f), 15f)
        )
    ),

    // 16. Varnished Oak Wood Flooring
    CompositeSurface(
        id = "wood_floor",
        name = "Varnished Oak Wood Flooring",
        category = SurfaceCategory.INTERIOR,
        description = "Composite floor: solid oak wood planks with natural grain, tongue-and-groove shadow bevels, and satin protective lacquer.",
        patternType = SurfacePatternType.PLANK_GRAIN_FLOOR,
        macroTileMeters = 1.20f,
        displacementScaleMeters = 0.006f,
        layers = listOf(
            SurfaceLayer("Oak Timber Planks", getMaterial("skin_dermal").copy(name = "Natural White Oak Grain", baseColor = Color(0xFFB88E5E), roughness = 0.45f, ior = 1.53f, subsurface = 0.15f), 92f),
            SurfaceLayer("Satin Polyurethane Varnish", getMaterial("glass_frosted").copy(name = "Satin Protective Varnish", roughness = 0.15f, ior = 1.49f), 100f),
            SurfaceLayer("Joinery Bevel Gaps", getMaterial("concrete_polished").copy(name = "Bevel Shadow Recess", baseColor = Color(0xFF3A2814), roughness = 0.95f), 8f)
        )
    ),

    // 17. Glazed Ceramic Floor Tiles
    CompositeSurface(
        id = "ceramic_tile_floor",
        name = "Glazed Ceramic Floor Tiles",
        category = SurfaceCategory.ARCHITECTURAL,
        description = "Composite floor surface: vitrified ceramic glazed tiles surrounded by anti-microbial cementitious grout lines.",
        patternType = SurfacePatternType.TILED_GROUT_GRID,
        macroTileMeters = 0.60f,
        displacementScaleMeters = 0.008f,
        layers = listOf(
            SurfaceLayer("Vitrified Ceramic Glaze", getMaterial("concrete_polished").copy(name = "White Porcelain Glaze", baseColor = Color(0xFFF0F2F4), roughness = 0.08f, ior = 1.52f), 92f),
            SurfaceLayer("Epoxy Grout Joint", getMaterial("concrete_polished").copy(name = "Light Grey Grout", baseColor = Color(0xFF9FA4A8), roughness = 0.88f), 8f)
        )
    ),

    // 18. Urban Asphalt Pathway
    CompositeSurface(
        id = "pathway_road",
        name = "Urban Pathway & Asphalt Surface",
        category = SurfaceCategory.INFRASTRUCTURE,
        description = "Composite road surface: bitumen aggregate asphalt base marked with reflective traffic lines and rainwater puddles.",
        patternType = SurfacePatternType.ASPHALT_ROAD_MARKING,
        macroTileMeters = 3.0f,
        displacementScaleMeters = 0.015f,
        layers = listOf(
            SurfaceLayer("Bitumen Asphalt Base", getMaterial("concrete_polished").copy(name = "Porous Bitumen Aggregate", baseColor = Color(0xFF282B30), roughness = 0.82f), 85f),
            SurfaceLayer("Reflective Road Line", getMaterial("concrete_polished").copy(name = "Reflective Road Marking Paint", baseColor = Color(0xFFF5CF22), roughness = 0.35f), 8f),
            SurfaceLayer("Rainwater Puddle Sheen", getMaterial("water_surface"), 7f)
        )
    )
)

fun getCompositeSurfaceForMaterial(mat: PbrMaterial): CompositeSurface {
    defaultCompositeSurfaces.firstOrNull { it.id == mat.id }?.let { return it }
    defaultCompositeSurfaces.firstOrNull { surf ->
        mat.id.contains(surf.id) || surf.id.contains(mat.id)
    }?.let { return it }

    val pattern = when {
        mat.id.contains("herringbone") -> SurfacePatternType.BRICK_HERRINGBONE
        mat.id.contains("four_leaf") || mat.id.contains("four-leaf") -> SurfacePatternType.GRASS_PAVER_FOUR_LEAF
        mat.id.contains("picket") -> SurfacePatternType.GRASS_PAVER_PICKET_SQUARE
        mat.id.contains("asphalt") || mat.id.contains("macadam") -> SurfacePatternType.ASPHALT_AGGREGATE_MACADAM
        mat.id.contains("chevron") -> SurfacePatternType.CHEVRON_PARQUET_WOOD
        mat.id.contains("paver") || mat.id.contains("bowtie") || mat.id.contains("interlocking") -> SurfacePatternType.PAVER_INTERLOCKING_GEOMETRIC
        mat.id.contains("granite") && mat.id.contains("sett") -> SurfacePatternType.GRANITE_SETTS_COBBLE
        mat.id.contains("flagstone") || mat.id.contains("rubble") || mat.id.contains("crazy") -> SurfacePatternType.STONE_RUBBLE_FLAGSTONE
        mat.id.contains("terrazzo") || mat.id.contains("mosaic") || mat.id.contains("inlay") -> SurfacePatternType.TERRAZZO_MOSAIC_CHIPS
        mat.id.contains("shingle") || mat.id.contains("roof") || mat.id.contains("felt") -> SurfacePatternType.ROOF_SHINGLE_SCALLOP
        mat.id.contains("corrugated") || mat.id.contains("profile") || mat.id.contains("cladding") -> SurfacePatternType.METAL_CORRUGATED_RIBS
        mat.id.contains("tread") || mat.id.contains("diamond") -> SurfacePatternType.METAL_DIAMOND_TREAD
        mat.id.contains("soil") || mat.id.contains("mud") || mat.id.contains("earth") -> SurfacePatternType.ORGANIC_SOIL_LOAM
        mat.id.contains("grass") || mat.id.contains("turf") || mat.id.contains("ryegrass") || mat.id.contains("moss") -> SurfacePatternType.TURF_BLADE_ORGANIC
        mat.id.contains("brick") -> SurfacePatternType.BRICK_BOND_MASONRY
        mat.id.contains("tile") -> SurfacePatternType.TILED_GROUT_GRID
        mat.id.contains("wood") || mat.id.contains("oak") || mat.id.contains("plank") -> SurfacePatternType.PLANK_GRAIN_FLOOR
        mat.category == "Metals" -> SurfacePatternType.DUAL_LAYER_CLEARCOAT
        else -> SurfacePatternType.HOMOGENEOUS
    }

    val cat = when (mat.category) {
        "Masonry" -> SurfaceCategory.ARCHITECTURAL
        "Metals" -> SurfaceCategory.ARCHITECTURAL
        "Flooring" -> SurfaceCategory.INTERIOR
        "Nature", "Organics" -> SurfaceCategory.NATURE
        "Infrastructure" -> SurfaceCategory.INFRASTRUCTURE
        "Transportation" -> SurfaceCategory.TRANSPORTATION
        else -> SurfaceCategory.ARCHITECTURAL
    }

    return CompositeSurface(
        id = mat.id,
        name = mat.name,
        category = cat,
        description = mat.description,
        patternType = pattern,
        macroTileMeters = mat.tileScaleMeters,
        displacementScaleMeters = mat.displacementDepthMeters,
        surfaceMode = mat.surfaceMode,
        assetPath = mat.assetPath,
        layers = listOf(
            SurfaceLayer("Primary Face Material", mat, 88f),
            SurfaceLayer("Substrate / Joint Bedding", mat.copy(name = "${mat.name} Joint / Mortar", roughness = (mat.roughness + 0.15f).coerceAtMost(1f)), 12f)
        )
    )
}
