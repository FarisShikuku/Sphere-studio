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
        assetPath = "file:///android_asset/rusticated_red_brick.jpg",
        layers = listOf(
            SurfaceLayer("Clay Brick Blocks", defaultPbrMaterials.first { it.id == "brick_rusticated" }, 86f),
            SurfaceLayer("Cement Mortar Joints", defaultPbrMaterials.first { it.id == "concrete_polished" }.copy(name = "Sandy Cement Mortar", baseColor = Color(0xFFAFAFA8), roughness = 0.92f), 14f)
        )
    ),
    // 2. Mondo Grass Lawn Turf
    CompositeSurface(
        id = "grass_turf",
        name = "Mondo Grass Lawn Surface",
        category = SurfaceCategory.NATURE,
        description = "Composite ground surface: high-density green grass blades mixed with yellow root thatch and dark organic loam.",
        patternType = SurfacePatternType.TURF_BLADE_ORGANIC,
        macroTileMeters = 0.30f,
        displacementScaleMeters = 0.012f,
        assetPath = "file:///android_asset/mondo_grass.jpg",
        layers = listOf(
            SurfaceLayer("Living Grass Blades", defaultPbrMaterials.first { it.id == "grass_surface" }, 72f),
            SurfaceLayer("Thatch Root Layer", defaultPbrMaterials.first { it.id == "earth_foliage" }.copy(name = "Dry Yellow Thatch", baseColor = Color(0xFF887D42), roughness = 0.85f), 18f),
            SurfaceLayer("Soil Topsoil", defaultPbrMaterials.first { it.id == "earth_foliage" }.copy(name = "Organic Topsoil", baseColor = Color(0xFF382A1A), roughness = 0.95f), 10f)
        )
    ),
    // 3. Earth with Foliage Surface
    CompositeSurface(
        id = "earth_foliage",
        name = "Earth with Foliage Surface",
        category = SurfaceCategory.NATURE,
        description = "Composite terrain: nutrient-rich soil substrate supporting creeping broadleaf foliage and stone aggregate.",
        patternType = SurfacePatternType.FOLIAGE_SOIL_SPLATMAP,
        macroTileMeters = 0.70f,
        displacementScaleMeters = 0.032f,
        assetPath = "file:///android_asset/earth_foliage.jpg",
        layers = listOf(
            SurfaceLayer("Soil Substrate", defaultPbrMaterials.first { it.id == "earth_foliage" }.copy(name = "Moist Organic Loam", baseColor = Color(0xFF4A3B2B), roughness = 0.92f), 55f),
            SurfaceLayer("Creeping Ivy Foliage", defaultPbrMaterials.first { it.id == "grass_surface" }.copy(name = "Broadleaf Micro-Foliage", baseColor = Color(0xFF5B7828), subsurface = 0.5f), 35f),
            SurfaceLayer("Basalt Pebbles", defaultPbrMaterials.first { it.id == "concrete_polished" }.copy(name = "Basalt Stone Aggregate", baseColor = Color(0xFF6B6E72), roughness = 0.6f), 10f)
        )
    ),
    // 4. Automotive Paint Surface (User example: vehicle metallic black/white glossy surface)
    CompositeSurface(
        id = "car_paint_black",
        name = "Metallic Black Automotive Surface",
        category = SurfaceCategory.TRANSPORTATION,
        description = "Multi-layer vehicle finish: steel body + metallic black basecoat with aluminum flakes + high-index polyurethane clearcoat.",
        patternType = SurfacePatternType.DUAL_LAYER_CLEARCOAT,
        macroTileMeters = 2.0f,
        displacementScaleMeters = 0.001f,
        layers = listOf(
            SurfaceLayer("Clearcoat Top Layer", defaultPbrMaterials.first { it.id == "glass_clear" }.copy(name = "Polyurethane Clearcoat", roughness = 0.02f, ior = 1.50f, transmission = 0.95f), 100f),
            SurfaceLayer("Metallic Pigment Base", defaultPbrMaterials.first { it.id == "metal_brushed" }.copy(name = "Jet Black Metallic Pigment", baseColor = Color(0xFF141618), roughness = 0.35f, metallic = 0.85f), 100f),
            SurfaceLayer("Steel Body Substrate", defaultPbrMaterials.first { it.id == "metal_brushed" }.copy(name = "Cold-Rolled Steel", baseColor = Color(0xFF888C90), metallic = 1.0f), 100f)
        )
    ),
    // 5. Wood Plank Floor Surface (User example: wood floor)
    CompositeSurface(
        id = "wood_floor",
        name = "Varnished Oak Wood Flooring",
        category = SurfaceCategory.INTERIOR,
        description = "Composite floor: solid oak wood planks with natural grain, tongue-and-groove shadow bevels, and satin protective lacquer.",
        patternType = SurfacePatternType.PLANK_GRAIN_FLOOR,
        macroTileMeters = 1.20f,
        displacementScaleMeters = 0.006f,
        layers = listOf(
            SurfaceLayer("Oak Timber Planks", defaultPbrMaterials.first { it.id == "skin_dermal" }.copy(name = "Natural White Oak Grain", baseColor = Color(0xFFB88E5E), roughness = 0.45f, ior = 1.53f, subsurface = 0.15f), 92f),
            SurfaceLayer("Satin Polyurethane Varnish", defaultPbrMaterials.first { it.id == "glass_frosted" }.copy(name = "Satin Protective Varnish", roughness = 0.15f, ior = 1.49f), 100f),
            SurfaceLayer("Joinery Bevel Gaps", defaultPbrMaterials.first { it.id == "concrete_polished" }.copy(name = "Bevel Shadow Recess", baseColor = Color(0xFF3A2814), roughness = 0.95f), 8f)
        )
    ),
    // 6. Ceramic Floor Tiles (User example: tiles house)
    CompositeSurface(
        id = "ceramic_tile_floor",
        name = "Glazed Ceramic Floor Tiles",
        category = SurfaceCategory.ARCHITECTURAL,
        description = "Composite floor surface: vitrified ceramic glazed tiles surrounded by anti-microbial cementitious grout lines.",
        patternType = SurfacePatternType.TILED_GROUT_GRID,
        macroTileMeters = 0.60f,
        displacementScaleMeters = 0.008f,
        layers = listOf(
            SurfaceLayer("Vitrified Ceramic Glaze", defaultPbrMaterials.first { it.id == "concrete_polished" }.copy(name = "White Porcelain Glaze", baseColor = Color(0xFFF0F2F4), roughness = 0.08f, ior = 1.52f), 92f),
            SurfaceLayer("Epoxy Grout Joint", defaultPbrMaterials.first { it.id == "concrete_polished" }.copy(name = "Light Grey Grout", baseColor = Color(0xFF9FA4A8), roughness = 0.88f), 8f)
        )
    ),
    // 7. Urban Asphalt Pathway (User example: pathways)
    CompositeSurface(
        id = "pathway_road",
        name = "Urban Pathway & Asphalt Surface",
        category = SurfaceCategory.INFRASTRUCTURE,
        description = "Composite road surface: bitumen aggregate asphalt base marked with reflective traffic lines and rainwater puddles.",
        patternType = SurfacePatternType.ASPHALT_ROAD_MARKING,
        macroTileMeters = 3.0f,
        displacementScaleMeters = 0.015f,
        layers = listOf(
            SurfaceLayer("Bitumen Asphalt Base", defaultPbrMaterials.first { it.id == "concrete_polished" }.copy(name = "Porous Bitumen Aggregate", baseColor = Color(0xFF282B30), roughness = 0.82f), 85f),
            SurfaceLayer("Reflective Road Line", defaultPbrMaterials.first { it.id == "concrete_polished" }.copy(name = "Reflective Road Marking Paint", baseColor = Color(0xFFF5CF22), roughness = 0.35f), 8f),
            SurfaceLayer("Rainwater Puddle Sheen", defaultPbrMaterials.first { it.id == "water_surface" }, 7f)
        )
    )
)
