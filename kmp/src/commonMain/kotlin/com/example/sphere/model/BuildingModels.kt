package com.example.sphere.model

import androidx.compose.ui.graphics.Color

enum class BuildingStudioCategory(
    val displayName: String,
    val defaultDomainOrdinal: Int,
    val subtitle: String
) {
    MASSING("Massing", 0, "27 Parametric Volumes"),
    RESIDENTIAL("Residential", 1, "53 Houses, Villas & Towers"),
    COMMERCIAL("Commercial", 2, "36 Towers, Malls & Stations"),
    INDUSTRIAL("Industrial", 5, "14 Warehouses & Plants"),
    CIVIC("Civic", 6, "20 Stadiums, Churches & Civic"),
    INTERIORS("Interiors", 7, "5 Cutaway Rooms & Workshops"),
    PROPS("Rooftop Props", 8, "3 HVAC, Chimneys & Masts"),
    SIGNAGE("Signage", 9, "3 Billboards & Neon Signs"),
    DAMAGE("Damage", 10, "3 Breaches & Rubble Piles"),
    LOD("City LOD", 11, "3 City Blocks & Impostors")
}

enum class BuildingVolumeShape(val ordinalId: Int, val displayName: String) {
    SQUARE(0, "Square Tower"),
    RECTANGLE(1, "Rectangle Slab"),
    CUBE(2, "Pure Cube"),
    OVAL(3, "Elliptical Oval"),
    SEMI_OVAL(4, "Semi-Oval Crescent"),
    CYLINDER(5, "Cylindrical Drum"),
    TRIANGLE_SEMI_CIRCLE(6, "Triangle + Semi-Circle"),
    TRIANGULAR_FLATIRON(7, "Triangular Flatiron"),
    L_SHAPED(8, "L-Shaped Wing"),
    OCTAGON(9, "Octagonal Prism"),
    HEXAGON(10, "Hexagonal Prism"),
    STEPPED_ZIGGURAT(11, "Stepped Ziggurat"),
    STAR_TRI_POINT_Y(12, "3-Point Y-Star"),
    STAR_FOUR_POINT_CROSS(13, "4-Point Cruciform"),
    STAR_FIVE_POINT_APEX(14, "5-Point Star Apex"),
    TWISTED_HELIX(15, "Twisted Helix Tower"),
    TORUS_RING_PAVILION(16, "Torus Ring Pavilion"),
    CANTILEVER_OVERHANG(17, "Cantilever Overhang"),
    CONICAL_PYRAMID(18, "Conical Pyramid"),
    U_SHAPED_COURTYARD(19, "U-Shaped Courtyard"),
    H_SHAPED_TWIN_WING(20, "H-Shaped Twin Wing"),
    CURVED_WAVE_RIBBON(21, "Curved Wave Ribbon"),
    DIAGRID_EXOSKELETON(22, "Diagrid Exoskeleton"),
    MOBIUS_ARCH_PORTAL(23, "Mobius Arch Portal"),
    STEPPED_SAIL_MARINE(24, "Stepped Marine Sail"),
    INTERLOCKING_JENGA(25, "Interlocking Cantilevers"),
    FACETED_CRYSTALLINE_GEM(26, "Faceted Crystalline Gem")
}

enum class BuildingCornerType(val ordinalId: Int, val displayName: String) {
    SHARP_90(0, "Sharp 90°"),
    CHAMFER_45(1, "Chamfer 45°"),
    ROUNDED_FILLET(2, "Rounded Fillet"),
    INSET_NOTCH(3, "Inset Notch"),
    STEPPED_DOUBLE_NOTCH(4, "Stepped Double Notch"),
    CORNER_COLUMN(5, "Corner Column"),
    GLAZED_SLOT_REVEAL(6, "Glazed Slot Reveal")
}

enum class BuildingStepMode(val ordinalId: Int, val displayName: String) {
    UNIFORM_STRAIGHT(0, "Uniform Straight"),
    STEPPED_BLOCKS(1, "Stepped Setback Blocks"),
    CONTINUOUS_TAPER(2, "Continuous Taper"),
    CONCAVE_CURVED_TAPER(3, "Concave Curved Taper"),
    EXPANDING_CANTILEVER(4, "Expanding Cantilever")
}

enum class BuildingWindowType(val ordinalId: Int, val displayName: String) {
    MODERN_CASEMENT(0, "Modern Casement"),
    DOUBLE_HUNG_SASH(1, "Double Hung Sash"),
    PICTURE_GLASS(2, "Picture Glass"),
    RIBBON_STRIP(3, "Ribbon Strip"),
    ARCHED_HISTORIC(4, "Arched Historic"),
    CURTAIN_WALL(5, "Curtain Wall Grid"),
    BAY_WINDOW(6, "Extruded Bay Window"),
    CORNER_WRAP(7, "Corner Wrap Glass"),
    CLERESTORY_TRANSOM(8, "Clerestory Transom"),
    ROUND_OCULUS(9, "Round Oculus"),
    LOUVERED_JALOUSIE(10, "Louvered Jalousie"),
    ARCHED_GOTHIC_LANCET(11, "Gothic Lancet Arch"),
    MULTI_PANE_FACTORY(12, "Multi-Pane Factory"),
    DIAMOND_LEAD_GLAZING(13, "Diamond Lead Glazing"),
    PIVOTING_HORIZ_HOPPER(14, "Pivoting Hopper"),
    PIVOTING_VERT_SASH(15, "Pivoting Vertical Sash"),
    OCTAGONAL_BULLSEYE(16, "Octagonal Bullseye"),
    PALLADIAN_TRIPLE_ARCHED(17, "Palladian Triple Arch"),
    TRIANGULAR_APEX_TRANSOM(18, "Triangular Apex"),
    HEXAGONAL_HONEYCOMB(19, "Hexagonal Honeycomb"),
    BRISE_SOLEIL_FINNED(20, "Brise-Soleil Finned"),
    SLIDING_POCKET_SASH(21, "Sliding Pocket Sash"),
    FRENCH_CASEMENT_FULL(22, "French Full-Height")
}

enum class BuildingGlassType(val ordinalId: Int, val displayName: String, val swatchColor: Color) {
    CLEAR_FLOAT(0, "Clear Float", Color(0xFF38BDF8)),
    SOLAR_TINTED_SMOKE(1, "Solar Smoke Tint", Color(0xFF334155)),
    REFLECTIVE_CHROME(2, "Chrome Mirror", Color(0xFF94A3B8)),
    LOW_E_EMERALD(3, "Low-E Emerald", Color(0xFF10B981)),
    BRONZE_ARCH(4, "Bronze Architectural", Color(0xFF92400E)),
    FROSTED_SATIN(5, "Frosted Privacy", Color(0xFFCBD5E1)),
    GOLD_MIRRORED(6, "Gold Mirrored Luxury", Color(0xFFF59E0B)),
    STAINED_ARTISAN(7, "Stained Glass", Color(0xFF8B5CF6)),
    WIRED_SAFETY(8, "Wired Safety", Color(0xFF64748B))
}

enum class BuildingWallMaterial(val ordinalId: Int, val displayName: String, val swatchColor: Color) {
    CONCRETE_BRUTALIST(0, "Brutalist Concrete", Color(0xFFB8BDC4)),
    BRICK_RED_MASONRY(1, "Red Clay Brick", Color(0xFF9E4232)),
    LIMESTONE_PORTLAND(2, "Portland Limestone", Color(0xFFDCD3BA)),
    GLASS_CURTAIN_WALL(3, "Glass Curtain Wall", Color(0xFF3B82F6)),
    WHITE_STUCCO_MODERN(4, "Modern White Stucco", Color(0xFFE8E6DC)),
    WEATHERED_CORTEN(5, "Weathered Corten Steel", Color(0xFF8C3E22)),
    DARK_BASALT_SLATE(6, "Dark Basalt Slate", Color(0xFF2D333B)),
    NATURAL_TIMBER(7, "Natural Timber Siding", Color(0xFF966136)),
    BLACK_ALUM_GRID(8, "Black Aluminum Grid", Color(0xFF1F242B)),
    STRUCTURAL_SILICONE(9, "Structural Silicone Glass", Color(0xFF2563EB)),
    CHAMPAGNE_ALUM(10, "Champagne Aluminum", Color(0xFFC9BA9E)),
    DOUBLE_SKIN_AERO(11, "Double-Skin Aero Glass", Color(0xFF60A5FA)),
    BRONZE_ALUM_FINS(12, "Bronze Aluminum Fins", Color(0xFF705236)),
    GUNMETAL_CASSETTE(13, "Gunmetal Cassette", Color(0xFF4D5560)),
    PERFORATED_ALUM(14, "Perforated Brise-Soleil", Color(0xFFC4CCD6)),
    SPIDER_POINT_GLASS(15, "Spider Point-Fixed Glass", Color(0xFF7DD3FC))
}

enum class BuildingDoorType(val ordinalId: Int, val displayName: String) {
    NONE(0, "No Entrance Door"),
    SINGLE_MODERN(1, "Single Modern"),
    DOUBLE_GLASS_COMMERCIAL(2, "Double Glass Lobby"),
    ARCHED_WOODEN(3, "Arched Wooden"),
    INDUSTRIAL_ROLL_UP(4, "Industrial Roll-Up"),
    REVOLVING_DOOR(5, "Revolving Carousel"),
    AUTOMATIC_SLIDING(6, "Automatic Sliding"),
    FRENCH_DOUBLE_PATIO(7, "French Double Patio"),
    CANOPIED_GRAND_PORTAL(8, "Canopied Grand Portal"),
    SECURITY_GATE_GRILLE(9, "Security Gate Grille")
}

enum class BuildingRoofType(val ordinalId: Int, val displayName: String) {
    FLAT_PARAPET(0, "Flat Parapet"),
    GABLE(1, "Pitched Gable"),
    HIP(2, "Hip Roof"),
    MANSARD(3, "French Mansard"),
    DOME(4, "Hemispherical Dome"),
    NONE_FLAT(5, "Flat Deck Slab"),
    SHED_MONOPITCH(6, "Monopitch Shed"),
    SAWTOOTH_INDUSTRIAL(7, "Sawtooth Northlight"),
    PYRAMIDAL_APEX(8, "Pyramidal Apex"),
    BARREL_VAULT(9, "Barrel Vault"),
    GAMBREL_DUTCH(10, "Dutch Gambrel"),
    GREEN_ROOF_TERRACE(11, "Green Roof Terrace"),
    BUTTERFLY(12, "Butterfly V-Roof"),
    CROSS_GABLE_DORMER(13, "Cross-Gable Dormers"),
    DUTCH_HIP_CUPOLA(14, "Dutch Hip + Cupola"),
    PAGODA_CURVED_EAVES(15, "Pagoda Curved Eaves"),
    GEODESIC_SKYLIGHT(16, "Geodesic Skylight Crown"),
    SPLIT_CLERESTORY(17, "Split Clerestory Pitch"),
    HYPERBOLIC_PARABOLOID(18, "Hyperbolic Paraboloid"),
    CONICAL_SPIRE_LANTERN(19, "Conical Spire Lantern"),
    DIAGRID_SAIL_CANOPY(20, "Diagrid Sail Canopy"),
    STEPPED_ZIGGURAT_CROWN(21, "Stepped Ziggurat Crown")
}

enum class BuildingBalconyType(val ordinalId: Int, val displayName: String) {
    NONE(0, "None"),
    CANTILEVERED_GLASS(1, "Cantilevered Glass"),
    WROUGHT_IRON(2, "Wrought Iron"),
    CONCRETE_LEDGE(3, "Concrete Ledge")
}

data class BuildingPresetItem(
    val id: String,
    val name: String,
    val subtitle: String,
    val category: BuildingStudioCategory,
    val domainOrdinal: Int,
    val subTypeOrdinal: Int,
    val defaultFloors: Int,
    val defaultFloorHeight: Float,
    val defaultWidth: Float,
    val defaultDepth: Float,
    val defaultShape: BuildingVolumeShape = BuildingVolumeShape.RECTANGLE,
    val defaultWall: BuildingWallMaterial = BuildingWallMaterial.CONCRETE_BRUTALIST,
    val defaultRoof: BuildingRoofType = BuildingRoofType.FLAT_PARAPET,
    val defaultWindow: BuildingWindowType = BuildingWindowType.MODERN_CASEMENT,
    val defaultGlass: BuildingGlassType = BuildingGlassType.CLEAR_FLOAT,
    val defaultDoor: BuildingDoorType = BuildingDoorType.DOUBLE_GLASS_COMMERCIAL,
    val defaultDetailCount: Int = 3,
    val defaultFlagA: Boolean = true,
    val defaultFlagB: Boolean = true
)

data class BuildingWallTintPreset(val name: String, val color: Color)

val buildingWallTintPresets = listOf(
    BuildingWallTintPreset("Modern Alabaster", Color(0xFFE2E8F0)),
    BuildingWallTintPreset("Warm Limestone", Color(0xFFEADBC8)),
    BuildingWallTintPreset("Terracotta Ochre", Color(0xFFD98A6C)),
    BuildingWallTintPreset("Sage Plaster", Color(0xFFA7C4B5)),
    BuildingWallTintPreset("Nordic Slate", Color(0xFF64748B)),
    BuildingWallTintPreset("Charcoal Obsidian", Color(0xFF27303F))
)

data class BuildingStudioState(
    val activeCategory: BuildingStudioCategory = BuildingStudioCategory.MASSING,
    val activePresetId: String = "massing_rectangle",
    val domainOrdinal: Int = 0,
    val subTypeOrdinal: Int = 0,
    val shape: BuildingVolumeShape = BuildingVolumeShape.STEPPED_SAIL_MARINE,
    val cornerType: BuildingCornerType = BuildingCornerType.CHAMFER_45,
    val cornerSize: Float = 2.0f,
    val stepMode: BuildingStepMode = BuildingStepMode.STEPPED_BLOCKS,
    val stepTiers: Int = 3,
    val topTaperRatio: Float = 0.58f,
    val stepShrinkFactor: Float = 0.18f,
    val stepTerraceRailing: Boolean = true,
    val floorCount: Int = 10,
    val floorHeight: Float = 3.2f,
    val width: Float = 22.0f,
    val depth: Float = 16.0f,
    val cornerRadius: Float = 3.0f,
    val windowsPerFacade: Int = 4,
    val windowType: BuildingWindowType = BuildingWindowType.CURTAIN_WALL,
    val windowWidth: Float = 1.6f,
    val windowHeight: Float = 1.9f,
    val windowSillHeight: Float = 0.7f,
    val glassType: BuildingGlassType = BuildingGlassType.CLEAR_FLOAT,
    val windowIor: Float = 1.52f,
    val wallMaterial: BuildingWallMaterial = BuildingWallMaterial.GLASS_CURTAIN_WALL,
    val wallColor: Color = Color(0xFFE2E8F0),
    val doorType: BuildingDoorType = BuildingDoorType.DOUBLE_GLASS_COMMERCIAL,
    val doorWidth: Float = 2.6f,
    val doorHeight: Float = 2.8f,
    val hasEntranceCanopy: Boolean = true,
    val roofType: BuildingRoofType = BuildingRoofType.FLAT_PARAPET,
    val roofHeight: Float = 2.6f,
    val roofOverhang: Float = 0.6f,
    val showRooftopClutter: Boolean = true,
    val balconyType: BuildingBalconyType = BuildingBalconyType.NONE,
    val hasBalconies: Boolean = false,
    val balconyDepth: Float = 1.1f,
    val hasSunLouvres: Boolean = false,
    val hasFireEscape: Boolean = false,
    val hasCornice: Boolean = true,
    val hasGroundPillars: Boolean = false,
    val pillarCount: Int = 4,
    val hasWindowShutters: Boolean = false,
    val showInside: Boolean = false,
    val interiorDesign: Int = 0,
    val detailIntensity: Float = 0.75f,
    val detailCount: Int = 3,
    val detailFlagA: Boolean = true,
    val detailFlagB: Boolean = true,
    val wireframe: Boolean = false
)

fun BuildingStudioState.applyPreset(preset: BuildingPresetItem): BuildingStudioState {
    return copy(
        activeCategory = preset.category,
        activePresetId = preset.id,
        domainOrdinal = preset.domainOrdinal,
        subTypeOrdinal = preset.subTypeOrdinal,
        shape = preset.defaultShape,
        floorCount = preset.defaultFloors,
        floorHeight = preset.defaultFloorHeight,
        width = preset.defaultWidth,
        depth = preset.defaultDepth,
        wallMaterial = preset.defaultWall,
        roofType = preset.defaultRoof,
        windowType = preset.defaultWindow,
        glassType = preset.defaultGlass,
        doorType = preset.defaultDoor,
        detailCount = preset.defaultDetailCount,
        detailFlagA = preset.defaultFlagA,
        detailFlagB = preset.defaultFlagB,
        showInside = preset.category == BuildingStudioCategory.INTERIORS
    )
}

fun createBuildingsCatalog(): List<BuildingPresetItem> {
    val items = mutableListOf<BuildingPresetItem>()

    // 1. Massing & Shapes (All 27 BuildingVolumeShape values)
    BuildingVolumeShape.entries.forEach { shape ->
        items += BuildingPresetItem(
            id = "massing_${shape.name.lowercase()}",
            name = shape.displayName,
            subtitle = "ProceduralBuildingGenerator • Volume #${shape.ordinalId}",
            category = BuildingStudioCategory.MASSING,
            domainOrdinal = 0,
            subTypeOrdinal = shape.ordinalId,
            defaultFloors = if (shape.ordinalId >= 11) 12 else 6,
            defaultFloorHeight = 3.2f,
            defaultWidth = 20f,
            defaultDepth = 16f,
            defaultShape = shape,
            defaultWall = BuildingWallMaterial.GLASS_CURTAIN_WALL,
            defaultRoof = BuildingRoofType.FLAT_PARAPET,
            defaultWindow = BuildingWindowType.CURTAIN_WALL
        )
    }

    // 2. Residential (All 53 ResidentialType values, HouseGen domain = 1)
    val residentialSpecs = listOf(
        0 to "Suburban House" to Triple(2, 14f, 10f),
        1 to "Apartment Block" to Triple(5, 22f, 14f),
        2 to "Mediterranean & Luxury Villa" to Triple(2, 18f, 14f),
        3 to "Rowhouse Brownstone" to Triple(3, 8f, 14f),
        4 to "Craftsman Tudor" to Triple(2, 14f, 11f),
        5 to "Mid-Century Ranch" to Triple(1, 18f, 11f),
        6 to "Residential Skyscraper" to Triple(18, 22f, 20f),
        7 to "Modern Duplex" to Triple(2, 16f, 12f),
        8 to "Country Farmhouse" to Triple(2, 15f, 11f),
        9 to "Residential Garage" to Triple(1, 8f, 7f),
        10 to "Victorian Painted Lady" to Triple(3, 12f, 13f),
        11 to "Mediterranean Villa" to Triple(2, 18f, 14f),
        12 to "A-Frame Alpine Chalet" to Triple(2, 10f, 12f),
        13 to "Tiny House Modular" to Triple(1, 6f, 4f),
        14 to "Cape Cod Cottage" to Triple(2, 12f, 9f),
        15 to "Geodesic Eco Dome" to Triple(1, 12f, 12f),
        16 to "Courtyard Atrium House" to Triple(2, 18f, 18f),
        17 to "Japanese Machiya" to Triple(2, 9f, 14f),
        18 to "Cantilever Glass House" to Triple(2, 18f, 12f),
        19 to "Colonial Four-Square" to Triple(2, 13f, 12f),
        20 to "Minimalist Cubic Villa" to Triple(2, 15f, 12f),
        21 to "Bauhaus Residence" to Triple(2, 16f, 11f),
        22 to "Desert Modernist Pavilion" to Triple(1, 20f, 12f),
        23 to "Brutalist Concrete Home" to Triple(2, 16f, 13f),
        24 to "Japanese Zen Flat House" to Triple(1, 17f, 12f),
        25 to "Shipping Container Home" to Triple(2, 13f, 8f),
        26 to "Green-Roof Eco Villa" to Triple(2, 17f, 12f),
        27 to "Tropical Louvered House" to Triple(2, 16f, 12f),
        28 to "Clerestory Modern Home" to Triple(2, 16f, 11f),
        29 to "Penthouse Terrace House" to Triple(3, 16f, 14f),
        30 to "Tropical Gable Manor" to Triple(2, 20f, 15f),
        31 to "Rooftop Lounge Cube" to Triple(3, 15f, 13f),
        32 to "Parapet Concealed Bungalow" to Triple(1, 17f, 13f),
        33 to "Hip Arched Bungalow" to Triple(1, 16f, 13f),
        34 to "Obsidian Louver Villa" to Triple(2, 18f, 13f),
        35 to "Cantilever V-Strut Mansion" to Triple(3, 22f, 15f),
        36 to "Staggered Terrace Apartments" to Triple(6, 24f, 16f),
        37 to "Halo Canopy Villa" to Triple(2, 19f, 14f),
        38 to "Modern Dual-Box Carport Villa" to Triple(2, 18f, 13f),
        39 to "Taupe Interlocking Frame Villa" to Triple(2, 18f, 13f),
        40 to "Twin-Wing Skybridge Apartments" to Triple(7, 26f, 16f),
        41 to "Corinthian Portico Modern Villa" to Triple(2, 20f, 14f),
        42 to "Stone Atrium Pergola Villa" to Triple(2, 19f, 15f),
        43 to "Diagrid Panel Corner Apartments" to Triple(6, 22f, 16f),
        44 to "Slanted Prow Modern Villa" to Triple(2, 19f, 13f),
        45 to "Timber Band Podium Apartments" to Triple(6, 24f, 16f),
        46 to "Triangular Void Geometric Villa" to Triple(2, 18f, 13f),
        47 to "Brick Pergola Penthouse Villa" to Triple(3, 18f, 14f),
        48 to "Twin-Stair Corner Apartments" to Triple(5, 22f, 15f),
        49 to "Minimalist Ribbon Bungalow" to Triple(1, 18f, 13f),
        50 to "88 Nairobi Condominium (176m)", // placeholder replaced below
        51 to "GTC Residential Tower 1 (145m)",
        52 to "Altura Residential Tower"
    )
    residentialSpecs.forEachIndexed { idx, entry ->
        when (entry) {
            is Pair<*, *> -> {
                val first = entry.first
                if (first is Pair<*, *>) {
                    val subId = first.first as Int
                    val label = first.second as String
                    @Suppress("UNCHECKED_CAST")
                    val dims = entry.second as Triple<Int, Float, Float>
                    items += BuildingPresetItem(
                        id = "res_$subId",
                        name = label,
                        subtitle = "HouseGen • ResidentialType #$subId",
                        category = BuildingStudioCategory.RESIDENTIAL,
                        domainOrdinal = 1,
                        subTypeOrdinal = subId,
                        defaultFloors = dims.first,
                        defaultFloorHeight = 3.2f,
                        defaultWidth = dims.second,
                        defaultDepth = dims.third,
                        defaultWall = if (subId >= 20) BuildingWallMaterial.WHITE_STUCCO_MODERN else BuildingWallMaterial.BRICK_RED_MASONRY,
                        defaultRoof = if (subId >= 20 || subId == 1 || subId == 6) BuildingRoofType.FLAT_PARAPET else BuildingRoofType.GABLE,
                        defaultWindow = BuildingWindowType.DOUBLE_HUNG_SASH,
                        defaultDoor = BuildingDoorType.SINGLE_MODERN
                    )
                } else {
                    val subId = entry.first as Int
                    val label = entry.second as String
                    items += BuildingPresetItem(
                        id = "res_$subId",
                        name = label,
                        subtitle = "HouseGen • Iconic Nairobi Tower #$subId",
                        category = BuildingStudioCategory.RESIDENTIAL,
                        domainOrdinal = 1,
                        subTypeOrdinal = subId,
                        defaultFloors = if (subId == 50) 44 else if (subId == 51) 35 else 28,
                        defaultFloorHeight = 3.6f,
                        defaultWidth = 26f,
                        defaultDepth = 24f,
                        defaultWall = BuildingWallMaterial.GLASS_CURTAIN_WALL,
                        defaultRoof = BuildingRoofType.FLAT_PARAPET,
                        defaultWindow = BuildingWindowType.CURTAIN_WALL,
                        defaultGlass = BuildingGlassType.LOW_E_EMERALD
                    )
                }
            }
        }
    }

    // 3. Commercial (All 36 CommercialType values + PetrolStationGen / CarWashGen)
    val commercialNames = listOf(
        "Office Tower", "Shopping Mall", "Small High-Street Shop", "Petrol Station Forecourt",
        "Automated Car Wash Tunnel", "Mega Shopping Mall", "Supermarket", "Auto Dealership",
        "Fast-Food Drive-Thru", "Grand Hotel", "EV Charging Station", "Service Garage",
        "Multi-Storey Parking Garage", "Self-Storage Facility", "Modular Container Shop", "Coffee Kiosk",
        "Boutique Hotel", "Covered Market Hall", "Open Farmers Market", "Night Market Plaza",
        "Sweeping Ribbon Campus", "Corporate Glass Park", "Eco Terrace Spire", "Glass Pilaster HQ",
        "Diagonal Strut Pavilion", "Britam Tower (200m)", "GTC Office Tower (184m)", "UAP Old Mutual Tower (163m)",
        "Times Tower (140m)", "Prism Tower (133m)", "Le'Mac Tower (126m)", "Teleposta House (120m)",
        "CBK Pension Tower (110m)", "KCB Plaza (105m)", "Upper Hill Chambers", "NSSF Social Security Building"
    )
    commercialNames.forEachIndexed { idx, label ->
        val domainId = when (idx) {
            3 -> 3  // PetrolStationGen
            4 -> 4  // CarWashGen
            else -> 2 // TowerGen
        }
        val stories = when {
            idx >= 25 -> (38 - (idx - 25) * 2).coerceAtLeast(18)
            idx == 0 || idx == 9 || idx == 22 -> 14
            idx == 1 || idx == 5 || idx == 12 -> 4
            else -> 2
        }
        items += BuildingPresetItem(
            id = "com_$idx",
            name = label,
            subtitle = when (domainId) {
                3 -> "PetrolStationGen • Canopy & Pumps"
                4 -> "CarWashGen • Automated Wash Tunnel"
                else -> "TowerGen • CommercialType #$idx"
            },
            category = BuildingStudioCategory.COMMERCIAL,
            domainOrdinal = domainId,
            subTypeOrdinal = idx,
            defaultFloors = stories,
            defaultFloorHeight = 3.5f,
            defaultWidth = if (idx == 5 || idx == 1) 34f else 24f,
            defaultDepth = if (idx == 5 || idx == 1) 26f else 18f,
            defaultWall = BuildingWallMaterial.GLASS_CURTAIN_WALL,
            defaultRoof = BuildingRoofType.FLAT_PARAPET,
            defaultWindow = BuildingWindowType.CURTAIN_WALL,
            defaultGlass = if (idx >= 25) BuildingGlassType.LOW_E_EMERALD else BuildingGlassType.CLEAR_FLOAT
        )
    }

    // 4. Industrial (All 14 IndustrialType values, WarehouseGen domain = 5)
    val industrialNames = listOf(
        "Logistics Warehouse", "Manufacturing Factory", "Agricultural Grain Silo", "Thermal Power Plant",
        "Petrochemical Refinery", "Container Port Terminal", "Heavy Steel Mill", "Semiconductor Cleanroom Fab",
        "High-Voltage Pylon Lines", "Water Treatment Facility", "Hyperscale Data Center", "Aerospace Hangar",
        "Waste-to-Energy Incinerator", "Biopharma Production Plant"
    )
    industrialNames.forEachIndexed { idx, label ->
        items += BuildingPresetItem(
            id = "ind_$idx",
            name = label,
            subtitle = "WarehouseGen • IndustrialType #$idx",
            category = BuildingStudioCategory.INDUSTRIAL,
            domainOrdinal = 5,
            subTypeOrdinal = idx,
            defaultFloors = if (idx == 2 || idx == 8 || idx == 11) 4 else 2,
            defaultFloorHeight = 4.2f,
            defaultWidth = 22f,
            defaultDepth = 32f,
            defaultWall = BuildingWallMaterial.CONCRETE_BRUTALIST,
            defaultRoof = if (idx == 1) BuildingRoofType.SAWTOOTH_INDUSTRIAL else BuildingRoofType.GABLE,
            defaultWindow = BuildingWindowType.MULTI_PANE_FACTORY,
            defaultDoor = BuildingDoorType.INDUSTRIAL_ROLL_UP,
            defaultDetailCount = 3
        )
    }

    // 5. Civic & Public (All 20 CivicType values, CivicGen domain = 6)
    val civicNames = listOf(
        "Public School Campus", "Regional Hospital", "Municipal Town Hall", "Intercity Bus Terminal",
        "Open Athletics Stadium", "International Airport Terminal", "Central Train Station", "Domed Arena Stadium",
        "Baseball Ballpark", "Olympic Cycling Velodrome", "City Fire Station", "Botanical Conservatory Greenhouse",
        "Astronomical Observatory", "Public Library & Museum", "Gothic Cathedral", "Neighborhood Parish Chapel",
        "Modern Worship Sanctuary", "Community Amphitheater", "Kenya Parliament Tower", "KICC Kenyatta Tower"
    )
    civicNames.forEachIndexed { idx, label ->
        items += BuildingPresetItem(
            id = "civ_$idx",
            name = label,
            subtitle = "CivicGen • CivicType #$idx",
            category = BuildingStudioCategory.CIVIC,
            domainOrdinal = 6,
            subTypeOrdinal = idx,
            defaultFloors = when (idx) {
                18, 19 -> 24
                1 -> 6
                14 -> 5
                else -> 3
            },
            defaultFloorHeight = 3.6f,
            defaultWidth = if (idx == 4 || idx == 7 || idx == 8) 38f else 28f,
            defaultDepth = if (idx == 4 || idx == 7 || idx == 8) 32f else 18f,
            defaultWall = BuildingWallMaterial.LIMESTONE_PORTLAND,
            defaultRoof = if (idx == 12 || idx == 7) BuildingRoofType.DOME else BuildingRoofType.MANSARD,
            defaultWindow = BuildingWindowType.ARCHED_HISTORIC,
            defaultDoor = BuildingDoorType.CANOPIED_GRAND_PORTAL
        )
    }

    // 6. Interiors (All 5 RoomType values, RoomGen domain = 7)
    val interiorNames = listOf(
        "Furnished Living Room",
        "Open-Plan Office Floor",
        "Multi-Room Floorplan Cutaway",
        "Logistics Warehouse Interior",
        "Vehicle Repair Workshop Bays"
    )
    interiorNames.forEachIndexed { idx, label ->
        items += BuildingPresetItem(
            id = "int_$idx",
            name = label,
            subtitle = "RoomGen • Cutaway Interior #$idx",
            category = BuildingStudioCategory.INTERIORS,
            domainOrdinal = 7,
            subTypeOrdinal = idx,
            defaultFloors = 1,
            defaultFloorHeight = if (idx >= 3) 4.8f else 3.0f,
            defaultWidth = if (idx >= 3) 18f else 9f,
            defaultDepth = if (idx >= 3) 22f else 11f,
            defaultWall = BuildingWallMaterial.WHITE_STUCCO_MODERN
        )
    }

    // 7. Rooftop Props (All 3 AttachmentType values, PropAttach domain = 8)
    listOf("Commercial HVAC Chiller Unit", "Brick & Flue Chimney Stack", "Telecom Sector Antenna Mast").forEachIndexed { idx, label ->
        items += BuildingPresetItem(
            id = "prop_$idx",
            name = label,
            subtitle = "PropAttach • Socket Asset #$idx",
            category = BuildingStudioCategory.PROPS,
            domainOrdinal = 8,
            subTypeOrdinal = idx,
            defaultFloors = 1,
            defaultFloorHeight = 3.0f,
            defaultWidth = 4f,
            defaultDepth = 4f,
            defaultDetailCount = 3
        )
    }

    // 8. Signage (All 3 SignType values, NeonSignGen domain = 9)
    listOf("Neon Glow Rooftop Sign", "Highway Steel Gantry Billboard", "Storefront Illuminated Fascia").forEachIndexed { idx, label ->
        items += BuildingPresetItem(
            id = "sign_$idx",
            name = label,
            subtitle = "NeonSignGen • Signage #$idx",
            category = BuildingStudioCategory.SIGNAGE,
            domainOrdinal = 9,
            subTypeOrdinal = idx,
            defaultFloors = 2,
            defaultFloorHeight = 3.2f,
            defaultWidth = 10f,
            defaultDepth = 4f
        )
    }

    // 9. Structural Damage (All 3 DamageType values, DamageGen domain = 10)
    listOf("Fractured Cracked Wall", "Collapsed Concrete Rubble Pile", "Explosive Structural Breach").forEachIndexed { idx, label ->
        items += BuildingPresetItem(
            id = "dmg_$idx",
            name = label,
            subtitle = "DamageGen • Destruction #$idx",
            category = BuildingStudioCategory.DAMAGE,
            domainOrdinal = 10,
            subTypeOrdinal = idx,
            defaultFloors = 2,
            defaultFloorHeight = 3.2f,
            defaultWidth = 14f,
            defaultDepth = 8f,
            defaultDetailCount = 24
        )
    }

    // 10. LOD & City Blocks (All 3 ImpostorType values, ImpostorGen domain = 11)
    listOf("Octahedral City Block Grid", "Billboard Impostor Quad", "Baked Facade Card").forEachIndexed { idx, label ->
        items += BuildingPresetItem(
            id = "lod_$idx",
            name = label,
            subtitle = "ImpostorGen • Distant HLOD #$idx",
            category = BuildingStudioCategory.LOD,
            domainOrdinal = 11,
            subTypeOrdinal = idx,
            defaultFloors = 8,
            defaultFloorHeight = 3.5f,
            defaultWidth = 24f,
            defaultDepth = 24f,
            defaultDetailCount = 3
        )
    }

    return items
}
