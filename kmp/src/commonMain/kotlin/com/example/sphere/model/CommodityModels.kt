package com.example.sphere.model

import androidx.compose.ui.graphics.Color

enum class CommodityCategoryType(val label: String, val code: String) {
    BAGGED_GOODS("Bagged Goods", "bagged_goods"),
    BOXES_CRATES("Boxes & Crates", "boxes_crates"),
    LIQUID_BULK("Liquid Bulk", "liquid_bulk"),
    RAW_MATERIALS("Raw Materials", "raw_materials"),
    CONTAINERS_BOTTLES("Containers & Bottles", "containers_bottles"),
    SPECIALTY_INDUSTRIAL("Specialty Industrial", "specialty_industrial"),
    PERISHABLE_GOODS("Perishable Goods", "perishable_goods");

    val displayName: String get() = label
}

enum class StackingMode(val label: String, val multiplier: Int) {
    SINGLE_UNIT("Single Unit (1x)", 1),
    PALLET_STACK("Euro Pallet (40x)", 40),
    WAREHOUSE_RACK("Rack Tier (120x)", 120),
    TRUCK_BED("Flatbed Row (240x)", 240)
}

enum class StencilStampType(val displayName: String) {
    NONE("No Stamp"),
    BARCODE_GS1("GS1 Barcode"),
    WARNING_HAZARD("Hazard Sign"),
    NET_WEIGHT("Net Weight"),
    EPAL_EUR("EPAL / EUR"),
    BATCH_LOT("Batch / Lot")
}

data class CommodityItem(
    val id: Int,
    val name: String,
    val category: CommodityCategoryType,
    val nominalWeightKg: Float,
    val volumeM3: Float,
    val lengthM: Float,
    val widthM: Float,
    val heightM: Float,
    val baseColor: Color,
    val secondaryColor: Color,
    val roughness: Float,
    val metallic: Float,
    val opacity: Float = 1.0f,
    val opacityBias: Float = 0.0f,
    val wrinkleCreaseDepth: Float = 0.35f,
    val wearAndTear: Float = 0.15f,
    val defaultLabel: String,
    val showStencil: Boolean = true,
    val stencilLabel: String = defaultLabel,
    val stampType: StencilStampType = StencilStampType.NET_WEIGHT,
    val stencilColor: Color = Color(0xFF1E1E1E),
    val stencilScale: Float = 1.0f,
    val vkAlbedoFormat: String = "VK_FORMAT_BC7_SRGB_BLOCK",
    val vkNormalFormat: String = "VK_FORMAT_BC5_UNORM_BLOCK",
    val vkOrmFormat: String = "VK_FORMAT_BC7_UNORM_BLOCK",
    val description: String
) {
    val weightKg: Float get() = nominalWeightKg
    val densityKgM3: Float get() = if (volumeM3 > 0.0001f) nominalWeightKg / volumeM3 else 500f
    val maxStackLoadKg: Float get() = nominalWeightKg * 20f
}

fun createDefaultCommoditiesCatalog(): List<CommodityItem> = defaultCommodityItems

val defaultCommodityItems = listOf(
    // 1. Bagged Goods
    CommodityItem(
        id = 0,
        name = "Sugar Bag 50kg (Woven PP)",
        category = CommodityCategoryType.BAGGED_GOODS,
        nominalWeightKg = 50.0f,
        volumeM3 = 0.082f,
        lengthM = 0.82f, widthM = 0.48f, heightM = 0.22f,
        baseColor = Color(0xFFF1F3F5),
        secondaryColor = Color(0xFF1E40AF),
        roughness = 0.65f, metallic = 0.0f,
        defaultLabel = "PURE WHITE SUGAR 50KG",
        description = "White woven polypropylene polymer sack with blue branded stencil text and stitched mouth."
    ),
    CommodityItem(
        id = 1,
        name = "Maize Sack 100kg (Bulk PP)",
        category = CommodityCategoryType.BAGGED_GOODS,
        nominalWeightKg = 100.0f,
        volumeM3 = 0.165f,
        lengthM = 1.05f, widthM = 0.58f, heightM = 0.32f,
        baseColor = Color(0xFFE8D7A5),
        secondaryColor = Color(0xFF166534),
        roughness = 0.72f, metallic = 0.0f,
        defaultLabel = "GRADE A YELLOW MAIZE 100KG",
        description = "Heavy bulk woven grain bag holding yellow maize corn with agricultural green marking."
    ),
    CommodityItem(
        id = 2,
        name = "Cement Bag 50kg (Kraft)",
        category = CommodityCategoryType.BAGGED_GOODS,
        nominalWeightKg = 50.0f,
        volumeM3 = 0.045f,
        lengthM = 0.65f, widthM = 0.44f, heightM = 0.16f,
        baseColor = Color(0xFFC7BBA8),
        secondaryColor = Color(0xFFB91C1C),
        roughness = 0.85f, metallic = 0.0f,
        defaultLabel = "PORTLAND CEMENT 42.5N",
        description = "Multiwall kraft paper bag with valve mouth and construction grade red cement branding."
    ),
    CommodityItem(
        id = 3,
        name = "Fertilizer 25kg (Laminated)",
        category = CommodityCategoryType.BAGGED_GOODS,
        nominalWeightKg = 25.0f,
        volumeM3 = 0.035f,
        lengthM = 0.62f, widthM = 0.38f, heightM = 0.15f,
        baseColor = Color(0xFF22C55E),
        secondaryColor = Color(0xFFFEF08A),
        roughness = 0.42f, metallic = 0.0f,
        defaultLabel = "BIO-AGRI NPK 17-17-17",
        description = "Laminated plastic moisture-barrier fertilizer bag with vibrant farm agronomy graphics."
    ),
    CommodityItem(
        id = 4,
        name = "Basmati Rice 25kg (Tight Weave)",
        category = CommodityCategoryType.BAGGED_GOODS,
        nominalWeightKg = 25.0f,
        volumeM3 = 0.038f,
        lengthM = 0.58f, widthM = 0.36f, heightM = 0.18f,
        baseColor = Color(0xFFFAF7F2),
        secondaryColor = Color(0xFFDC2626),
        roughness = 0.60f, metallic = 0.0f,
        defaultLabel = "SUPER BASMATI RICE 25KG",
        description = "Fine-weave food-grade woven sack with red royal crest and woven handle seam."
    ),
    CommodityItem(
        id = 5,
        name = "Coffee Bean 60kg (Jute Sack)",
        category = CommodityCategoryType.BAGGED_GOODS,
        nominalWeightKg = 60.0f,
        volumeM3 = 0.118f,
        lengthM = 0.88f, widthM = 0.52f, heightM = 0.26f,
        baseColor = Color(0xFFA07850),
        secondaryColor = Color(0xFF2B1810),
        roughness = 0.92f, metallic = 0.0f,
        defaultLabel = "ORIGIN ARABICA GRADE 1 - 60KG",
        description = "Traditional coarse brown jute hessian sack with black stencil country of origin."
    ),
    CommodityItem(
        id = 6,
        name = "Tea Leaf 50kg (Foil Kraft)",
        category = CommodityCategoryType.BAGGED_GOODS,
        nominalWeightKg = 50.0f,
        volumeM3 = 0.086f,
        lengthM = 0.78f, widthM = 0.46f, heightM = 0.24f,
        baseColor = Color(0xFFB09875),
        secondaryColor = Color(0xFF1E3A2B),
        roughness = 0.80f, metallic = 0.0f,
        defaultLabel = "EXPORT BLACK TEA CTC 50KG",
        description = "Heavy kraft paper sack with aluminum foil barrier lining for moisture-sealed tea leaves."
    ),

    // 2. Boxes & Crates
    CommodityItem(
        id = 7,
        name = "Cardboard Shipping Carton",
        category = CommodityCategoryType.BOXES_CRATES,
        nominalWeightKg = 12.5f,
        volumeM3 = 0.084f,
        lengthM = 0.60f, widthM = 0.40f, heightM = 0.35f,
        baseColor = Color(0xFFB89468),
        secondaryColor = Color(0xFF262626),
        roughness = 0.82f, metallic = 0.0f,
        defaultLabel = "FRAGILE - THIS WAY UP",
        description = "Corrugated Kraft flute carton with brown sealing tape and shipping barcode sticker."
    ),
    CommodityItem(
        id = 8,
        name = "Slatted Wooden Crate",
        category = CommodityCategoryType.BOXES_CRATES,
        nominalWeightKg = 45.0f,
        volumeM3 = 0.520f,
        lengthM = 1.00f, widthM = 0.80f, heightM = 0.65f,
        baseColor = Color(0xFFC7A274),
        secondaryColor = Color(0xFF333333),
        roughness = 0.75f, metallic = 0.05f,
        defaultLabel = "HEAVY MACHINERY PART",
        description = "Pine timber slatted shipping crate reinforced with diagonal bracing and steel corner bands."
    ),
    CommodityItem(
        id = 9,
        name = "Vented HDPE Produce Crate",
        category = CommodityCategoryType.BOXES_CRATES,
        nominalWeightKg = 18.0f,
        volumeM3 = 0.052f,
        lengthM = 0.50f, widthM = 0.35f, heightM = 0.30f,
        baseColor = Color(0xFF1E40AF),
        secondaryColor = Color(0xFFFFFFFF),
        roughness = 0.35f, metallic = 0.0f,
        defaultLabel = "AGRI-LOGISTICS 40L",
        description = "Injection-molded high-density polyethylene crate with vented sidewalls for agricultural harvest."
    ),
    CommodityItem(
        id = 10,
        name = "EUR-1 Wooden Exchange Pallet",
        category = CommodityCategoryType.BOXES_CRATES,
        nominalWeightKg = 25.0f,
        volumeM3 = 0.138f,
        lengthM = 1.20f, widthM = 0.80f, heightM = 0.144f,
        baseColor = Color(0xFFB39268),
        secondaryColor = Color(0xFF1A1A1A),
        roughness = 0.88f, metallic = 0.0f,
        defaultLabel = "EPAL EUR 1200x800",
        description = "Standard European Four-Way logistics exchange pallet with branded corner blocks."
    ),
    CommodityItem(
        id = 11,
        name = "Stackable Milk Crate",
        category = CommodityCategoryType.BOXES_CRATES,
        nominalWeightKg = 16.0f,
        volumeM3 = 0.040f,
        lengthM = 0.38f, widthM = 0.38f, heightM = 0.28f,
        baseColor = Color(0xFFDC2626),
        secondaryColor = Color(0xFFFFFFFF),
        roughness = 0.40f, metallic = 0.0f,
        defaultLabel = "DAIRY FRESH CRATE",
        description = "Interlocking stackable solid-rib plastic crate engineered for bottled milk crates."
    ),

    // 3. Liquid Bulk
    CommodityItem(
        id = 12,
        name = "Potable Water Tanker Module",
        category = CommodityCategoryType.LIQUID_BULK,
        nominalWeightKg = 8600.0f,
        volumeM3 = 8.000f,
        lengthM = 4.50f, widthM = 1.80f, heightM = 1.80f,
        baseColor = Color(0xFFD1D5DB),
        secondaryColor = Color(0xFF0284C7),
        roughness = 0.28f, metallic = 0.75f,
        defaultLabel = "DRINKING WATER ONLY",
        description = "Cylindrical stainless steel bulk liquid vessel with baffled compartments and access manhole."
    ),
    CommodityItem(
        id = 13,
        name = "Steel Chemical Drum 200L",
        category = CommodityCategoryType.LIQUID_BULK,
        nominalWeightKg = 215.0f,
        volumeM3 = 0.200f,
        lengthM = 0.58f, widthM = 0.58f, heightM = 0.88f,
        baseColor = Color(0xFF1D4ED8),
        secondaryColor = Color(0xFFF59E0B),
        roughness = 0.35f, metallic = 0.85f,
        defaultLabel = "UN 1A1/Y1.4/250",
        description = "Standard 55-gallon closed-head steel drum with dual rolling hoop ribs and hazard placard."
    ),
    CommodityItem(
        id = 14,
        name = "Polyethylene Jerrican 20L",
        category = CommodityCategoryType.LIQUID_BULK,
        nominalWeightKg = 22.5f,
        volumeM3 = 0.020f,
        lengthM = 0.35f, widthM = 0.20f, heightM = 0.47f,
        baseColor = Color(0xFFFACC15),
        secondaryColor = Color(0xFF1F2937),
        roughness = 0.38f, metallic = 0.0f,
        defaultLabel = "DIESEL FUEL 20L",
        description = "High-visibility molded plastic liquid canister with integrated triple handle and breather spout."
    ),
    CommodityItem(
        id = 15,
        name = "Stainless Dairy Churn 40L",
        category = CommodityCategoryType.LIQUID_BULK,
        nominalWeightKg = 45.0f,
        volumeM3 = 0.040f,
        lengthM = 0.36f, widthM = 0.36f, heightM = 0.62f,
        baseColor = Color(0xFFE5E7EB),
        secondaryColor = Color(0xFF4B5563),
        roughness = 0.20f, metallic = 0.95f,
        defaultLabel = "GRADE A RAW DAIRY",
        description = "Seamless sanitary milk can with mushroom lid and twin drop wire handles."
    ),
    CommodityItem(
        id = 16,
        name = "Pressurized LPG Cylinder 15kg",
        category = CommodityCategoryType.LIQUID_BULK,
        nominalWeightKg = 28.5f,
        volumeM3 = 0.035f,
        lengthM = 0.32f, widthM = 0.32f, heightM = 0.68f,
        baseColor = Color(0xFFDC2626),
        secondaryColor = Color(0xFFFFFFFF),
        roughness = 0.42f, metallic = 0.70f,
        defaultLabel = "FLAMMABLE GAS 2.1",
        description = "Deep-drawn welded steel liquefied petroleum bottle with collar shroud and brass shutoff valve."
    ),

    // 4. Raw Materials
    CommodityItem(
        id = 17,
        name = "Oak Timber Tree Log",
        category = CommodityCategoryType.RAW_MATERIALS,
        nominalWeightKg = 420.0f,
        volumeM3 = 0.380f,
        lengthM = 2.40f, widthM = 0.45f, heightM = 0.45f,
        baseColor = Color(0xFF78563A),
        secondaryColor = Color(0xFFD4B483),
        roughness = 0.88f, metallic = 0.0f,
        defaultLabel = "HARDWOOD LOG - OAK",
        description = "Natural harvested oak log with deeply furrowed outer bark and concentric growth ring ends."
    ),
    CommodityItem(
        id = 18,
        name = "Milled Timber Lumber (Pack)",
        category = CommodityCategoryType.RAW_MATERIALS,
        nominalWeightKg = 210.0f,
        volumeM3 = 0.720f,
        lengthM = 3.00f, widthM = 0.60f, heightM = 0.40f,
        baseColor = Color(0xFFD1B48C),
        secondaryColor = Color(0xFF1A1A1A),
        roughness = 0.72f, metallic = 0.0f,
        defaultLabel = "KILN-DRIED C24 PINE",
        description = "Strapped bundle of planed construction lumber boards ready for framing and carpentry."
    ),
    CommodityItem(
        id = 19,
        name = "Deformed Steel Rebar Bundle",
        category = CommodityCategoryType.RAW_MATERIALS,
        nominalWeightKg = 850.0f,
        volumeM3 = 0.110f,
        lengthM = 3.50f, widthM = 0.35f, heightM = 0.35f,
        baseColor = Color(0xFF4B5563),
        secondaryColor = Color(0xFFEA580C),
        roughness = 0.58f, metallic = 0.92f,
        defaultLabel = "HIGH YIELD T16 REBAR",
        description = "Heavy bundle of high-tensile hot-rolled ribbed reinforcing steel bars for reinforced concrete."
    ),
    CommodityItem(
        id = 20,
        name = "Anthracite Bulk Coal Mound",
        category = CommodityCategoryType.RAW_MATERIALS,
        nominalWeightKg = 1200.0f,
        volumeM3 = 1.250f,
        lengthM = 1.80f, widthM = 1.80f, heightM = 0.75f,
        baseColor = Color(0xFF18181B),
        secondaryColor = Color(0xFF09090B),
        roughness = 0.65f, metallic = 0.18f,
        defaultLabel = "BULK ANTHRACITE COAL",
        description = "Volumetric heap of graded metallurgical lump coal with glossy fractured mineral facets."
    ),
    CommodityItem(
        id = 21,
        name = "Crushed Granite Gravel Pile",
        category = CommodityCategoryType.RAW_MATERIALS,
        nominalWeightKg = 1450.0f,
        volumeM3 = 0.950f,
        lengthM = 1.80f, widthM = 1.80f, heightM = 0.70f,
        baseColor = Color(0xFFA1A1AA),
        secondaryColor = Color(0xFF71717A),
        roughness = 0.92f, metallic = 0.02f,
        defaultLabel = "CRUSHED AGGREGATE 20MM",
        description = "Conical storage mound of angular granite road-base gravel aggregate."
    ),

    // 5. Containers & Bottles
    CommodityItem(
        id = 22,
        name = "Amber Glass Beer Bottle (330ml)",
        category = CommodityCategoryType.CONTAINERS_BOTTLES,
        nominalWeightKg = 0.58f,
        volumeM3 = 0.0006f,
        lengthM = 0.062f, widthM = 0.062f, heightM = 0.228f,
        baseColor = Color(0xFF5A3614),
        secondaryColor = Color(0xFFEAB308),
        roughness = 0.08f, metallic = 0.0f, opacity = 0.35f,
        defaultLabel = "PREMIUM CRAFT PILSNER 330ML",
        description = "Amber glass bottle with crimped tin crown cap and gold embossed foil wrap label."
    ),
    CommodityItem(
        id = 23,
        name = "PET Soda Bottle 2.0L",
        category = CommodityCategoryType.CONTAINERS_BOTTLES,
        nominalWeightKg = 2.08f,
        volumeM3 = 0.0022f,
        lengthM = 0.108f, widthM = 0.108f, heightM = 0.325f,
        baseColor = Color(0xFF260D07),
        secondaryColor = Color(0xFFDC2626),
        roughness = 0.12f, metallic = 0.0f, opacity = 0.45f,
        defaultLabel = "SPARKLING COLA 2L",
        description = "Fluted clear PET carbonated beverage bottle with dark caramel liquid and red wrap label."
    ),
    CommodityItem(
        id = 24,
        name = "Spring Water PET Bottle 500ml",
        category = CommodityCategoryType.CONTAINERS_BOTTLES,
        nominalWeightKg = 0.52f,
        volumeM3 = 0.0006f,
        lengthM = 0.065f, widthM = 0.065f, heightM = 0.210f,
        baseColor = Color(0xFFE0F2FE),
        secondaryColor = Color(0xFF0284C7),
        roughness = 0.05f, metallic = 0.0f, opacity = 0.18f,
        defaultLabel = "NATURAL SPRING WATER 500ML",
        description = "Clear ribbed plastic mineral water bottle with tamper-evident sky blue screw cap."
    ),
    CommodityItem(
        id = 25,
        name = "Glass Food Preserve Jar 500ml",
        category = CommodityCategoryType.CONTAINERS_BOTTLES,
        nominalWeightKg = 0.75f,
        volumeM3 = 0.0007f,
        lengthM = 0.088f, widthM = 0.088f, heightM = 0.135f,
        baseColor = Color(0xFFF0FDF4),
        secondaryColor = Color(0xFF16A34A),
        roughness = 0.06f, metallic = 0.0f, opacity = 0.25f,
        defaultLabel = "ORGANIC HONEY PRESERVE 500G",
        description = "Wide-mouth soda-lime glass jar with twist-off enameled tinplate lug cap."
    ),
    CommodityItem(
        id = 26,
        name = "Sanitary Steel Tin Can 400g",
        category = CommodityCategoryType.CONTAINERS_BOTTLES,
        nominalWeightKg = 0.46f,
        volumeM3 = 0.0005f,
        lengthM = 0.075f, widthM = 0.075f, heightM = 0.112f,
        baseColor = Color(0xFFE5E7EB),
        secondaryColor = Color(0xFFB91C1C),
        roughness = 0.30f, metallic = 0.90f,
        defaultLabel = "WHOLE PEELED TOMATOES 400G",
        description = "Cylindrical bead-expanded tinplate food can with double-seamed end covers."
    ),

    // 6. Specialty Industrial
    CommodityItem(
        id = 27,
        name = "Industrial Portland Cement 50kg",
        category = CommodityCategoryType.SPECIALTY_INDUSTRIAL,
        nominalWeightKg = 50.0f,
        volumeM3 = 0.045f,
        lengthM = 0.65f, widthM = 0.44f, heightM = 0.16f,
        baseColor = Color(0xFFBDB2A1),
        secondaryColor = Color(0xFF1E293B),
        roughness = 0.85f, metallic = 0.0f,
        defaultLabel = "PORTLAND COMPOSITE 52.5R",
        description = "Rapid-hardening high-early-strength civil engineering cement in heavy-duty poly-ply sack."
    ),
    CommodityItem(
        id = 28,
        name = "Structural Steel Pipe 6m",
        category = CommodityCategoryType.SPECIALTY_INDUSTRIAL,
        nominalWeightKg = 185.0f,
        volumeM3 = 0.228f,
        lengthM = 6.00f, widthM = 0.22f, heightM = 0.22f,
        baseColor = Color(0xFF374151),
        secondaryColor = Color(0xFFFACC15),
        roughness = 0.42f, metallic = 0.95f,
        defaultLabel = "SCH 40 CARBON PIPE 6M",
        description = "6-meter seamless structural pipeline casing with protective yellow polymer end closures."
    ),
    CommodityItem(
        id = 29,
        name = "Banded Brick Pallet (400 Units)",
        category = CommodityCategoryType.SPECIALTY_INDUSTRIAL,
        nominalWeightKg = 1150.0f,
        volumeM3 = 0.912f,
        lengthM = 1.20f, widthM = 0.80f, heightM = 0.95f,
        baseColor = Color(0xFFB91C1C),
        secondaryColor = Color(0xFFF1F5F9),
        roughness = 0.88f, metallic = 0.0f,
        defaultLabel = "FIRED CLAY BRICKS - 400 PCS",
        description = "Pallet cube of interlocking terracotta masonry bricks wrapped in tensioned shrinkwrap."
    ),
    CommodityItem(
        id = 30,
        name = "Commercial Truck Tyre Stack (4x)",
        category = CommodityCategoryType.SPECIALTY_INDUSTRIAL,
        nominalWeightKg = 240.0f,
        volumeM3 = 1.400f,
        lengthM = 1.08f, widthM = 1.08f, heightM = 1.20f,
        baseColor = Color(0xFF18181B),
        secondaryColor = Color(0xFFEAB308),
        roughness = 0.75f, metallic = 0.02f,
        defaultLabel = "ALL-POSITION RADIAL 295/80R22.5",
        description = "Stack of four heavy commercial radial truck tyres with aggressive directional siped tread."
    ),

    // 7. Perishable Goods
    CommodityItem(
        id = 31,
        name = "Vine Harvest Tomatoes Crate",
        category = CommodityCategoryType.PERISHABLE_GOODS,
        nominalWeightKg = 14.0f,
        volumeM3 = 0.044f,
        lengthM = 0.50f, widthM = 0.35f, heightM = 0.25f,
        baseColor = Color(0xFFEF4444),
        secondaryColor = Color(0xFF16A34A),
        roughness = 0.25f, metallic = 0.0f,
        defaultLabel = "FRESH HARVEST TOMATOES",
        description = "Slotted farm harvest crate packed with glossy vine-ripened tomatoes and green calyxes."
    ),
    CommodityItem(
        id = 32,
        name = "Russet Potato Mesh Bag 25kg",
        category = CommodityCategoryType.PERISHABLE_GOODS,
        nominalWeightKg = 25.0f,
        volumeM3 = 0.060f,
        lengthM = 0.65f, widthM = 0.42f, heightM = 0.22f,
        baseColor = Color(0xFFB49774),
        secondaryColor = Color(0xFFDC2626),
        roughness = 0.78f, metallic = 0.0f,
        defaultLabel = "WASHED RUSSET POTATOES 25KG",
        description = "Ventilated knitted red poly-mesh bag containing washed russet potato tubers."
    ),
    CommodityItem(
        id = 33,
        name = "Iced Wild Salmon Seafood Crate",
        category = CommodityCategoryType.PERISHABLE_GOODS,
        nominalWeightKg = 22.0f,
        volumeM3 = 0.048f,
        lengthM = 0.60f, widthM = 0.40f, heightM = 0.20f,
        baseColor = Color(0xFFF87171),
        secondaryColor = Color(0xFFF0F9FF),
        roughness = 0.18f, metallic = 0.0f,
        defaultLabel = "FRESH WILD-CAUGHT SALMON",
        description = "Cold-chain distribution box with chilled salmon fillets nested in crushed crystalline flake ice."
    ),
    CommodityItem(
        id = 34,
        name = "Refrigerated Beef Primal Carton",
        category = CommodityCategoryType.PERISHABLE_GOODS,
        nominalWeightKg = 28.0f,
        volumeM3 = 0.058f,
        lengthM = 0.55f, widthM = 0.38f, heightM = 0.28f,
        baseColor = Color(0xFF991B1B),
        secondaryColor = Color(0xFFFFFFFF),
        roughness = 0.45f, metallic = 0.0f,
        defaultLabel = "CHILLED BEEF PRIMAL VAC-PACK",
        description = "Heavy insulated wax-impregnated meat carton holding vacuum-sealed primal beef cuts."
    )
)
