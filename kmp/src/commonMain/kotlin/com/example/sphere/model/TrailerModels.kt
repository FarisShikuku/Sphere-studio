package com.example.sphere.model

import androidx.compose.ui.graphics.Color

enum class TrailerCategoryType(val label: String, val code: String, val moduleFolder: String) {
    FLATBED("Flatbeds", "flatbed", "flatbed"),
    TANKER("Tankers", "tanker", "tanker"),
    BOX_VAN("Box Vans", "box_van", "box_van"),
    CHASSIS("Container Chassis", "chassis", "chassis"),
    LOGGING("Logging", "logging", "logging"),
    CURTAINSIDER("Curtainsiders", "curtainsider", "curtainsider"),
    BULK_HOPPER("Bulk Hoppers", "bulk_hopper", "bulk_hopper"),
    CAR_CARRIER("Car Carriers", "car_carrier", "car_carrier"),
    HEAVY_HAULAGE("Heavy Haulage", "heavy_haulage", "heavy_haulage"),
    DOLLY("Dollies", "dolly", "dolly"),
    RECREATIONAL("Recreational", "recreational", "recreational"),
    SPECIALTY("Specialty", "specialty", "specialty");

    val displayName: String get() = label
}

enum class TrailerHitchType(val displayName: String) {
    FIFTH_WHEEL_KINGPIN("Fifth-Wheel Kingpin"),
    PINTLE_RING_DOLLY("Pintle Ring / Drawbar"),
    BALL_HITCH_50MM("50mm Tow Ball Hitch"),
    DETACHABLE_GOOSENECK("Detachable Gooseneck")
}

enum class AxleConfigType(val displayName: String, val count: Int) {
    SINGLE("Single (1x)", 1),
    TANDEM("Tandem (2x)", 2),
    TRIDEM("Tridem (3x)", 3),
    QUAD("Quad (4x)", 4),
    MODULAR_6("Modular (6x)", 6)
}

enum class SuspensionType(val displayName: String) {
    AIR_SUSPENSION("Air Suspension"),
    LEAF_SPRING("Leaf Spring"),
    HYDRAULIC_MODULAR("Hydraulic Beam")
}

enum class BrakeSystemType(val displayName: String) {
    DISC_ABS("Ventilated Disc + ABS"),
    DRUM_SCAM("Heavy S-Cam Drum")
}

data class TrailerItem(
    val id: Int,
    val name: String,
    val category: TrailerCategoryType,
    val emptyWeightKg: Float,
    val maxPayloadKg: Float,
    val volumeM3: Float,
    val lengthM: Float,
    val widthM: Float,
    val heightM: Float,
    val deckHeightM: Float,
    val hitchType: TrailerHitchType = TrailerHitchType.FIFTH_WHEEL_KINGPIN,
    val axleConfig: AxleConfigType = AxleConfigType.TRIDEM,
    val suspensionType: SuspensionType = SuspensionType.AIR_SUSPENSION,
    val brakeType: BrakeSystemType = BrakeSystemType.DISC_ABS,
    val hasLiftAxle: Boolean = false,
    val liftAxleRaised: Boolean = false,
    val hasSteerableAxle: Boolean = false,
    val steerAngleDeg: Float = 0f,
    val baseColor: Color,
    val secondaryColor: Color,
    val roughness: Float = 0.45f,
    val metallic: Float = 0.55f,
    val curtainOpacity: Float = 1.0f,
    val wearAndTear: Float = 0.12f,
    val stencilLabel: String,
    val showStencil: Boolean = true,
    val vkAlbedoFormat: String = "VK_FORMAT_BC7_SRGB_BLOCK",
    val vkNormalFormat: String = "VK_FORMAT_BC5_UNORM_BLOCK",
    val vkOrmFormat: String = "VK_FORMAT_BC7_UNORM_BLOCK",
    val description: String
) {
    val gvwRatingKg: Float get() = emptyWeightKg + maxPayloadKg

    val wheelRadiusM: Float
        get() = when {
            category == TrailerCategoryType.RECREATIONAL -> 0.345f // 205/75R15 (0.69m OD)
            category == TrailerCategoryType.CAR_CARRIER -> 0.385f  // 235/75R17.5 (0.77m OD)
            category == TrailerCategoryType.HEAVY_HAULAGE || deckHeightM <= 1.02f -> 0.395f // 245/70R17.5 (0.79m OD)
            deckHeightM < 1.22f -> 0.42f // 315/45R22.5 mega-liner (0.84m OD)
            else -> 0.525f // 385/65R22.5 standard semi (1.05m OD)
        }

    val tireWidthM: Float
        get() = when {
            category == TrailerCategoryType.RECREATIONAL -> 0.215f
            category == TrailerCategoryType.HEAVY_HAULAGE -> 0.44f
            wheelRadiusM <= 0.42f -> 0.315f
            else -> 0.385f
        }

    val axleSpacingM: Float
        get() = when {
            category == TrailerCategoryType.RECREATIONAL -> 0.88f
            category == TrailerCategoryType.HEAVY_HAULAGE -> 1.25f
            else -> 1.36f
        }

    val groundClearanceM: Float
        get() = (wheelRadiusM - 0.08f).coerceAtLeast(0.24f)

    val tireSpecLabel: String
        get() = when {
            category == TrailerCategoryType.RECREATIONAL -> "205/75R15 (Ø 0.69m)"
            category == TrailerCategoryType.CAR_CARRIER -> "235/75R17.5 (Ø 0.77m)"
            category == TrailerCategoryType.HEAVY_HAULAGE || deckHeightM <= 1.02f -> "245/70R17.5 Dual (Ø 0.79m)"
            deckHeightM < 1.22f -> "315/45R22.5 Low (Ø 0.84m)"
            else -> "385/65R22.5 Super (Ø 1.05m)"
        }
}

fun createDefaultTrailersCatalog(): List<TrailerItem> = defaultTrailerItems

val defaultTrailerItems = listOf(
    // 1. Flatbeds (0..4)
    TrailerItem(0, "Standard 40ft Flatbed", TrailerCategoryType.FLATBED, 6200f, 32000f, 82f, 13.6f, 2.55f, 1.55f, 1.35f,
        hasLiftAxle = true, baseColor = Color(0xFF374151), secondaryColor = Color(0xFFA87643), stencilLabel = "40FT FLATBED",
        description = "Twin I-beam longitudinal chassis with cross-members, hardwood deck planks, and twist-lock pockets."),
    TrailerItem(1, "Extendable Telescopic Flatbed", TrailerCategoryType.FLATBED, 7800f, 36000f, 98f, 16.5f, 2.55f, 1.55f, 1.35f,
        hasSteerableAxle = true, baseColor = Color(0xFFB91C1C), secondaryColor = Color(0xFF9E6B3B), stencilLabel = "EXTENDABLE 16.5M",
        description = "Telescopic sliding backbone frame for long steel beams, precast trusses, and wind tower sections."),
    TrailerItem(2, "Step-Deck Drop Frame Flatbed", TrailerCategoryType.FLATBED, 6900f, 34000f, 88f, 13.6f, 2.55f, 1.55f, 0.98f,
        baseColor = Color(0xFF1D4ED8), secondaryColor = Color(0xFF9E6B3B), stencilLabel = "STEP-DECK",
        description = "Upper kingpin neck stepping down to a 0.98m low-profile deck for tall industrial machinery."),
    TrailerItem(3, "Deep Drop-Deck Flatbed", TrailerCategoryType.FLATBED, 7400f, 38000f, 94f, 14.2f, 2.55f, 1.55f, 0.78f,
        hasSteerableAxle = true, baseColor = Color(0xFF1E293B), secondaryColor = Color(0xFFF59E0B), stencilLabel = "DROP-DECK",
        description = "Deep-cranked drop center deck maximizing vertical overhead bridge clearance."),
    TrailerItem(4, "Self-Loading Crane Sideloader", TrailerCategoryType.FLATBED, 9400f, 30000f, 84f, 13.8f, 2.55f, 2.95f, 1.32f,
        baseColor = Color(0xFFD97706), secondaryColor = Color(0xFF1E293B), stencilLabel = "SIDELOADER 36T",
        description = "Integrated front and rear hydraulic swing cranes for autonomous ground-to-deck container transfer."),

    // 2. Tankers (5..10)
    TrailerItem(5, "Potable Water Tanker 30kL", TrailerCategoryType.TANKER, 7100f, 30000f, 30f, 12.2f, 2.50f, 3.65f, 1.25f,
        roughness = 0.24f, metallic = 0.65f, baseColor = Color(0xFF2563EB), secondaryColor = Color(0xFFE2E8F0), stencilLabel = "POTABLE WATER 30KL",
        description = "Baffled cylindrical water tank with top manway catwalk, dome bulkheads, and gravity discharge manifold."),
    TrailerItem(6, "Multi-Compartment Fuel Tanker", TrailerCategoryType.TANKER, 7600f, 34000f, 42f, 13.2f, 2.55f, 3.72f, 1.25f,
        hasLiftAxle = true, roughness = 0.18f, metallic = 0.92f, baseColor = Color(0xFFE2E8F0), secondaryColor = Color(0xFFDC2626), stencilLabel = "UN1203 PETROL",
        description = "5-compartment elliptical aluminum shell with ADR vapor recovery piping and bottom API loading valves."),
    TrailerItem(7, "Insulated Stainless Milk Tanker", TrailerCategoryType.TANKER, 7400f, 29000f, 32f, 12.4f, 2.50f, 3.65f, 1.25f,
        roughness = 0.14f, metallic = 0.95f, baseColor = Color(0xFFF1F5F9), secondaryColor = Color(0xFF1D4ED8), stencilLabel = "GRADE-A DAIRY",
        description = "Mirror-polished sanitary 316L stainless tank with thermal foam jacket and CIP spray manifolds."),
    TrailerItem(8, "Corrosive Acid Chemical Tanker", TrailerCategoryType.TANKER, 8200f, 31000f, 34f, 12.6f, 2.50f, 3.62f, 1.25f,
        roughness = 0.22f, metallic = 0.88f, baseColor = Color(0xFFCBD5E1), secondaryColor = Color(0xFFF59E0B), stencilLabel = "ADR CORROSIVE 8",
        description = "External structural ring-stiffened chemical barrel with rollover coaming protection."),
    TrailerItem(9, "Pressurized LPG Gas Tanker", TrailerCategoryType.TANKER, 10400f, 24000f, 48f, 12.8f, 2.50f, 3.80f, 1.25f,
        roughness = 0.28f, metallic = 0.35f, baseColor = Color(0xFFF8FAFC), secondaryColor = Color(0xFFDC2626), stencilLabel = "LPG UN1075 18 BAR",
        description = "Heavy thick-wall pressure vessel with hemispherical domed ends and sun-shield white coating."),
    TrailerItem(10, "Pressurized Dry Powder Tanker", TrailerCategoryType.TANKER, 7900f, 32000f, 38f, 12.9f, 2.55f, 3.85f, 1.25f,
        roughness = 0.26f, metallic = 0.78f, baseColor = Color(0xFFD1D5DB), secondaryColor = Color(0xFF059669), stencilLabel = "SILO BULK 2.0 BAR",
        description = "Pneumatic fluidized-bed powder tanker with lower aeration cones and compressed air discharge."),

    // 3. Box Vans (11..15)
    TrailerItem(11, "53ft Dry Freight Box Van", TrailerCategoryType.BOX_VAN, 6600f, 29000f, 110f, 16.15f, 2.59f, 4.11f, 1.25f,
        axleConfig = AxleConfigType.TANDEM, roughness = 0.38f, metallic = 0.25f, baseColor = Color(0xFFF1F5F9), secondaryColor = Color(0xFF1E40AF), stencilLabel = "53FT DRY VAN",
        description = "Riveted aluminum dry freight box van with aerodynamic underbody side skirts and rear roll-up door."),
    TrailerItem(12, "Refrigerated Reefer Van", TrailerCategoryType.BOX_VAN, 7800f, 27500f, 102f, 16.15f, 2.59f, 4.11f, 1.25f,
        axleConfig = AxleConfigType.TANDEM, roughness = 0.32f, metallic = 0.22f, baseColor = Color(0xFFF8FAFC), secondaryColor = Color(0xFF0284C7), stencilLabel = "COLD CHAIN -25C",
        description = "Polyurethane-insulated FRP box van with front diesel-electric Thermo refrigeration unit."),
    TrailerItem(13, "Double-Stack Parcel Express Van", TrailerCategoryType.BOX_VAN, 5800f, 24000f, 88f, 13.6f, 2.55f, 4.00f, 1.25f,
        axleConfig = AxleConfigType.TANDEM, roughness = 0.40f, metallic = 0.20f, baseColor = Color(0xFFF59E0B), secondaryColor = Color(0xFFB91C1C), stencilLabel = "EXPRESS PARCEL",
        description = "High-visibility parcel logistics van with captive internal shelving tracks and rear step bumper."),
    TrailerItem(14, "Drop-Frame Household Moving Van", TrailerCategoryType.BOX_VAN, 6900f, 23000f, 105f, 14.6f, 2.55f, 4.10f, 0.92f,
        axleConfig = AxleConfigType.TANDEM, roughness = 0.42f, metallic = 0.20f, baseColor = Color(0xFFE5E7EB), secondaryColor = Color(0xFF15803D), stencilLabel = "TRANSIT MOVERS",
        description = "Low-deck drop-frame furniture van offering cavernous interior cube and curbside double doors."),
    TrailerItem(15, "Double-Deck High-Cube Van", TrailerCategoryType.BOX_VAN, 8300f, 28000f, 128f, 13.6f, 2.55f, 4.35f, 1.20f,
        axleConfig = AxleConfigType.TRIDEM, roughness = 0.36f, metallic = 0.30f, baseColor = Color(0xFF1E3A5F), secondaryColor = Color(0xFFF8FAFC), stencilLabel = "DOUBLE-DECK 4.35M",
        description = "4.35m tall dual-level UK/EU mega-trailer accommodating 52 Euro pallets across two decks."),

    // 4. Container Chassis (16..20)
    TrailerItem(16, "20ft ISO Container Chassis", TrailerCategoryType.CHASSIS, 3400f, 30480f, 33.2f, 7.20f, 2.48f, 1.32f, 1.20f,
        axleConfig = AxleConfigType.TANDEM, metallic = 0.72f, baseColor = Color(0xFF334155), secondaryColor = Color(0xFFF59E0B), stencilLabel = "20FT ISO CHASSIS",
        description = "Compact 20ft skeletal frame engineered for dense port container drayage with 4 ISO twist-locks."),
    TrailerItem(17, "40ft Tri-Axle Container Chassis", TrailerCategoryType.CHASSIS, 4800f, 34000f, 67.7f, 12.4f, 2.48f, 1.32f, 1.20f,
        axleConfig = AxleConfigType.TRIDEM, metallic = 0.72f, baseColor = Color(0xFF1F2937), secondaryColor = Color(0xFFFACC15), stencilLabel = "40FT INTERMODAL",
        description = "Standard intermodal skeletal container carrier with front and rear twist-lock bolsters."),
    TrailerItem(18, "Gooseneck High-Cube Chassis", TrailerCategoryType.CHASSIS, 4600f, 33000f, 76.4f, 12.6f, 2.48f, 1.25f, 1.15f,
        axleConfig = AxleConfigType.TRIDEM, metallic = 0.72f, baseColor = Color(0xFF1D4ED8), secondaryColor = Color(0xFFFACC15), stencilLabel = "GOOSENECK 9'6\"",
        description = "Stepped gooseneck front frame cradling 9'6\" high-cube ISO containers below legal 4.0m bridge height."),
    TrailerItem(19, "Extendable Slider Container Chassis", TrailerCategoryType.CHASSIS, 5200f, 34000f, 86.0f, 13.6f, 2.48f, 1.32f, 1.20f,
        axleConfig = AxleConfigType.TRIDEM, metallic = 0.72f, baseColor = Color(0xFFB91C1C), secondaryColor = Color(0xFFFACC15), stencilLabel = "SLIDER 20-45FT",
        description = "Pneumatically locking telescopic rear bogie accommodating 20ft, 40ft, and 45ft containers."),
    TrailerItem(20, "Hydraulic Tipping Container Chassis", TrailerCategoryType.CHASSIS, 5900f, 32000f, 33.2f, 8.40f, 2.50f, 1.45f, 1.28f,
        axleConfig = AxleConfigType.TRIDEM, metallic = 0.72f, baseColor = Color(0xFFD97706), secondaryColor = Color(0xFF1F2937), stencilLabel = "TIPPING BULK 20FT",
        description = "Front-mounted multi-stage telescopic hydraulic ram elevating 20ft liner containers up to 48 degrees."),

    // 5. Logging (21..23)
    TrailerItem(21, "Steerable Forestry Pole Trailer", TrailerCategoryType.LOGGING, 4800f, 36000f, 62f, 13.8f, 2.55f, 3.85f, 1.28f,
        axleConfig = AxleConfigType.TRIDEM, suspensionType = SuspensionType.LEAF_SPRING, hasSteerableAxle = true,
        baseColor = Color(0xFFEA580C), secondaryColor = Color(0xFFFACC15), stencilLabel = "TIMBER POLE",
        description = "Heavy backbone pole frame with twin sliding bolsters and self-steering rear bogie for forest tracks."),
    TrailerItem(22, "Heavy Bolster Logging Bunk Trailer", TrailerCategoryType.LOGGING, 6400f, 38000f, 68f, 13.6f, 2.55f, 3.95f, 1.28f,
        axleConfig = AxleConfigType.TRIDEM, suspensionType = SuspensionType.LEAF_SPRING, hasSteerableAxle = true,
        baseColor = Color(0xFFB91C1C), secondaryColor = Color(0xFFF59E0B), stencilLabel = "4-BUNK LOGGING",
        description = "4 heavy-duty steel bolsters with vertical containment stakes and rear hydraulic log loader crane."),
    TrailerItem(23, "Telescopic Long-Log Timber Trailer", TrailerCategoryType.LOGGING, 7100f, 40000f, 82f, 16.2f, 2.55f, 3.95f, 1.28f,
        axleConfig = AxleConfigType.TRIDEM, suspensionType = SuspensionType.LEAF_SPRING, hasSteerableAxle = true,
        baseColor = Color(0xFF15803D), secondaryColor = Color(0xFFF59E0B), stencilLabel = "LONG-LOG FORESTRY",
        description = "Extendable 5-bunk logging carrier equipped for 16m full-length Douglas fir and oak trunks."),

    // 6. Curtainsiders (24..27)
    TrailerItem(24, "Standard EU Tautliner Curtainsider", TrailerCategoryType.CURTAINSIDER, 6400f, 32000f, 92f, 13.6f, 2.55f, 4.00f, 1.25f,
        axleConfig = AxleConfigType.TRIDEM, hasLiftAxle = true, roughness = 0.62f, metallic = 0.14f, curtainOpacity = 1.0f,
        baseColor = Color(0xFF1D4ED8), secondaryColor = Color(0xFFF8FAFC), stencilLabel = "EURO FREIGHT LINE",
        description = "Heavy reinforced opaque PVC side curtains with sliding aluminum posts and ratchet strap buckles."),
    TrailerItem(25, "EN 12642-XL Certified Curtainsider", TrailerCategoryType.CURTAINSIDER, 6650f, 32500f, 94f, 13.6f, 2.55f, 4.05f, 1.25f,
        axleConfig = AxleConfigType.TRIDEM, hasLiftAxle = true, roughness = 0.60f, metallic = 0.15f, curtainOpacity = 1.0f,
        baseColor = Color(0xFFDC2626), secondaryColor = Color(0xFFFACC15), stencilLabel = "CODE XL CARGO",
        description = "Code-XL load-securing tautliner with integrated steel-belt curtains and reinforced bulkhead."),
    TrailerItem(26, "Double-Deck Lifting Curtainsider", TrailerCategoryType.CURTAINSIDER, 8400f, 30000f, 124f, 13.6f, 2.55f, 4.38f, 1.20f,
        axleConfig = AxleConfigType.TRIDEM, roughness = 0.62f, metallic = 0.16f, curtainOpacity = 1.0f,
        baseColor = Color(0xFF0F766E), secondaryColor = Color(0xFFF1F5F9), stencilLabel = "4.38M MEGA DECK",
        description = "Hydraulically raised internal second deck supporting dual-layer palletized retail goods."),
    TrailerItem(27, "Mega-Cube Side-Loading Curtainsider", TrailerCategoryType.CURTAINSIDER, 6900f, 31500f, 100f, 13.6f, 2.55f, 4.15f, 1.12f,
        axleConfig = AxleConfigType.TRIDEM, roughness = 0.58f, metallic = 0.18f, curtainOpacity = 1.0f,
        baseColor = Color(0xFF334155), secondaryColor = Color(0xFF38BDF8), stencilLabel = "MEGA CUBE 100M3",
        description = "Low-profile mega-volume tautliner with elevating roof header for tall forklift side-loading."),

    // 7. Bulk Hoppers (28..31)
    TrailerItem(28, "Twin-Hopper Grain Bulker", TrailerCategoryType.BULK_HOPPER, 5600f, 34000f, 55f, 12.8f, 2.55f, 3.65f, 1.25f,
        baseColor = Color(0xFFE2E8F0), secondaryColor = Color(0xFF15803D), stencilLabel = "AGRI GRAIN 55M3",
        description = "Lightweight aluminum V-bottom agricultural grain hopper with roll-over tarp and twin chutes."),
    TrailerItem(29, "Pneumatic Cement Silo Hopper", TrailerCategoryType.BULK_HOPPER, 6100f, 35000f, 38f, 11.8f, 2.55f, 3.85f, 1.25f,
        baseColor = Color(0xFFF1F5F9), secondaryColor = Color(0xFFEA580C), stencilLabel = "CEM-SILO 38M3",
        description = "Pressurized multi-cone dry bulk silo discharging Portland cement via 2-bar pneumatic hoses."),
    TrailerItem(30, "End-Dump Aggregate Tipper", TrailerCategoryType.BULK_HOPPER, 6800f, 36000f, 32f, 10.6f, 2.55f, 3.35f, 1.25f,
        baseColor = Color(0xFFD97706), secondaryColor = Color(0xFF1E293B), stencilLabel = "HARDOX TIPPER",
        description = "Abrasion-resistant Hardox steel half-pipe dump body for crushed gravel, sand, and asphalt."),
    TrailerItem(31, "Quarry Side-Tipper Tub", TrailerCategoryType.BULK_HOPPER, 7200f, 38000f, 35f, 11.4f, 2.55f, 3.25f, 1.25f,
        baseColor = Color(0xFFB91C1C), secondaryColor = Color(0xFF1E293B), stencilLabel = "SIDE-TIPPER 42T",
        description = "Twin-cylinder lateral dumping tub engineered for road-train mining and quarry aggregate runs."),

    // 8. Car Carriers (32..35)
    TrailerItem(32, "Single-Deck 3-Car Transporter", TrailerCategoryType.CAR_CARRIER, 4900f, 12000f, 38f, 12.2f, 2.52f, 2.25f, 0.92f,
        axleConfig = AxleConfigType.TANDEM, baseColor = Color(0xFF1D4ED8), secondaryColor = Color(0xFFE2E8F0), stencilLabel = "3-CAR OPEN",
        description = "Low-angle galvanized wheel-track hauler with slide-out rear loading ramps and chocks."),
    TrailerItem(33, "Hydraulic 8-Car Double-Deck Stinger", TrailerCategoryType.CAR_CARRIER, 9200f, 22000f, 88f, 16.4f, 2.55f, 4.10f, 0.92f,
        axleConfig = AxleConfigType.TANDEM, baseColor = Color(0xFFDC2626), secondaryColor = Color(0xFFE2E8F0), stencilLabel = "8-CAR STINGER",
        description = "Articulated multi-ram upper and lower perforated decks carrying up to 8 passenger vehicles."),
    TrailerItem(34, "Open Steel Track Car Hauler", TrailerCategoryType.CAR_CARRIER, 6800f, 19000f, 74f, 14.5f, 2.55f, 3.85f, 0.92f,
        axleConfig = AxleConfigType.TANDEM, baseColor = Color(0xFF15803D), secondaryColor = Color(0xFFE2E8F0), stencilLabel = "6-CAR TRACK",
        description = "Twin-deck high-tensile steel car transporter with serrated wheel-tie tracks."),
    TrailerItem(35, "Enclosed Exotic Motorsport Carrier", TrailerCategoryType.CAR_CARRIER, 8800f, 20000f, 96f, 16.15f, 2.59f, 4.15f, 0.92f,
        axleConfig = AxleConfigType.TANDEM, baseColor = Color(0xFF111827), secondaryColor = Color(0xFFF59E0B), stencilLabel = "MOTORSPORT PRO",
        description = "Weather-sealed luxury automotive transporter with hydraulic rear tailgate lift."),

    // 9. Heavy Haulage (36..39)
    TrailerItem(36, "Detachable Gooseneck 60T Lowboy", TrailerCategoryType.HEAVY_HAULAGE, 13500f, 65000f, 65f, 16.2f, 3.00f, 1.55f, 0.58f,
        hitchType = TrailerHitchType.DETACHABLE_GOOSENECK, axleConfig = AxleConfigType.QUAD, suspensionType = SuspensionType.HYDRAULIC_MODULAR,
        hasSteerableAxle = true, baseColor = Color(0xFFD97706), secondaryColor = Color(0xFF1E293B), stencilLabel = "60T DETACHABLE LOWBOY",
        description = "Hydraulic RGN gooseneck detaches at front so excavators and tracked dozers drive straight into well."),
    TrailerItem(37, "12-Axle Modular Heavy Platform", TrailerCategoryType.HEAVY_HAULAGE, 26000f, 180000f, 110f, 18.5f, 3.20f, 1.45f, 1.05f,
        hitchType = TrailerHitchType.DETACHABLE_GOOSENECK, axleConfig = AxleConfigType.MODULAR_6, suspensionType = SuspensionType.HYDRAULIC_MODULAR,
        hasSteerableAxle = true, baseColor = Color(0xFFB91C1C), secondaryColor = Color(0xFF1E293B), stencilLabel = "SPMT MODULAR 180T",
        description = "Self-equalizing hydraulic pendular axles with synchronized all-wheel steering for power transformers."),
    TrailerItem(38, "Wind Turbine Blade Lifter Trailer", TrailerCategoryType.HEAVY_HAULAGE, 15800f, 45000f, 120f, 22.0f, 2.85f, 2.40f, 1.15f,
        hitchType = TrailerHitchType.DETACHABLE_GOOSENECK, axleConfig = AxleConfigType.QUAD, suspensionType = SuspensionType.HYDRAULIC_MODULAR,
        hasSteerableAxle = true, baseColor = Color(0xFF1D4ED8), secondaryColor = Color(0xFFF8FAFC), stencilLabel = "WIND BLADE ADAPTER",
        description = "Telescopic spine trailer with hydraulic root-clamp pitching wind blades up to 60 degrees."),
    TrailerItem(39, "TBM Shield Tunnel Segment Hauler", TrailerCategoryType.HEAVY_HAULAGE, 19200f, 120000f, 92f, 17.2f, 3.10f, 1.75f, 0.72f,
        hitchType = TrailerHitchType.DETACHABLE_GOOSENECK, axleConfig = AxleConfigType.MODULAR_6, suspensionType = SuspensionType.HYDRAULIC_MODULAR,
        hasSteerableAxle = true, baseColor = Color(0xFF334155), secondaryColor = Color(0xFFF59E0B), stencilLabel = "TBM HEAVY 120T",
        description = "Ultra-reinforced twin box-girder spine hauling tunnel boring shields and bridge girders."),

    // 10. Dollies (40..42)
    TrailerItem(40, "Twin-Axle Road-Train Converter Dolly", TrailerCategoryType.DOLLY, 2650f, 22000f, 2f, 4.30f, 2.48f, 1.35f, 1.24f,
        hitchType = TrailerHitchType.PINTLE_RING_DOLLY, axleConfig = AxleConfigType.TANDEM,
        baseColor = Color(0xFFDC2626), secondaryColor = Color(0xFF1F2937), stencilLabel = "ROAD TRAIN DOLLY",
        description = "Heavy hinged A-frame drawbar with fifth-wheel turntable linking B-double & road-train trailers."),
    TrailerItem(41, "Single-Axle A-Frame Converter Dolly", TrailerCategoryType.DOLLY, 1550f, 11000f, 1.5f, 3.60f, 2.45f, 1.30f, 1.24f,
        hitchType = TrailerHitchType.PINTLE_RING_DOLLY, axleConfig = AxleConfigType.SINGLE,
        baseColor = Color(0xFF1D4ED8), secondaryColor = Color(0xFF1F2937), stencilLabel = "A-DOLLY 11T",
        description = "Lightweight single-axle converter dolly for twin-28ft parcel pup trailer combinations."),
    TrailerItem(42, "Heavy Haulage Tandem Jeep Dolly", TrailerCategoryType.DOLLY, 4200f, 36000f, 3f, 5.20f, 2.75f, 1.38f, 1.24f,
        hitchType = TrailerHitchType.FIFTH_WHEEL_KINGPIN, axleConfig = AxleConfigType.TANDEM,
        baseColor = Color(0xFFD97706), secondaryColor = Color(0xFF1F2937), stencilLabel = "HEAVY JEEP 36T",
        description = "Load-distributing fifth-wheel jeep booster spreading oversize gooseneck pin loads across two extra axles."),

    // 11. Recreational (43..46)
    TrailerItem(43, "Galvanized Keel-Roller Boat Trailer", TrailerCategoryType.RECREATIONAL, 680f, 2800f, 10f, 7.40f, 2.25f, 1.35f, 0.65f,
        hitchType = TrailerHitchType.BALL_HITCH_50MM, axleConfig = AxleConfigType.TANDEM, suspensionType = SuspensionType.LEAF_SPRING,
        baseColor = Color(0xFFCBD5E1), secondaryColor = Color(0xFFF59E0B), stencilLabel = "MARINE ROLLER",
        description = "Hot-dip galvanized marine V-chassis with bow winch post, polyurethane keel rollers, and bunk skids."),
    TrailerItem(44, "Dual-Quad Utility ATV Mesh Trailer", TrailerCategoryType.RECREATIONAL, 420f, 1800f, 6f, 4.60f, 2.10f, 1.15f, 0.60f,
        hitchType = TrailerHitchType.BALL_HITCH_50MM, axleConfig = AxleConfigType.SINGLE, suspensionType = SuspensionType.LEAF_SPRING,
        baseColor = Color(0xFF1E293B), secondaryColor = Color(0xFFF59E0B), stencilLabel = "ATV UTILITY",
        description = "Lightweight tubular steel side-rail trailer with folding expanded-mesh loading tailgate."),
    TrailerItem(45, "Expedition Off-Road Camper Trailer", TrailerCategoryType.RECREATIONAL, 1650f, 950f, 18f, 6.20f, 2.20f, 2.55f, 0.72f,
        hitchType = TrailerHitchType.BALL_HITCH_50MM, axleConfig = AxleConfigType.SINGLE, suspensionType = SuspensionType.LEAF_SPRING,
        baseColor = Color(0xFFD6C7A9), secondaryColor = Color(0xFF1E293B), stencilLabel = "OVERLAND CAMPER",
        description = "Independent trailing-arm expedition caravan with rooftop solar rack and articulated hitch."),
    TrailerItem(46, "Tandem Box General Utility Trailer", TrailerCategoryType.RECREATIONAL, 580f, 2500f, 8f, 5.20f, 2.15f, 1.25f, 0.62f,
        hitchType = TrailerHitchType.BALL_HITCH_50MM, axleConfig = AxleConfigType.TANDEM, suspensionType = SuspensionType.LEAF_SPRING,
        baseColor = Color(0xFF1D4ED8), secondaryColor = Color(0xFFCBD5E1), stencilLabel = "UTILITY 2500KG",
        description = "All-purpose galvanized tandem utility trailer for landscaping equipment, ATVs, and building supplies."),

    // 12. Specialty (47..50)
    TrailerItem(47, "Triple-Deck Ventilated Livestock Hauler", TrailerCategoryType.SPECIALTY, 8800f, 26000f, 94f, 14.8f, 2.55f, 4.15f, 1.25f,
        axleConfig = AxleConfigType.TRIDEM, baseColor = Color(0xFFE2E8F0), secondaryColor = Color(0xFF334155), stencilLabel = "LIVESTOCK 3-DECK",
        description = "Perforated punched aluminum side walls providing continuous airflow and internal sorting gates."),
    TrailerItem(48, "Municipal Vacuum Waste Tanker", TrailerCategoryType.SPECIALTY, 8400f, 28000f, 26f, 11.8f, 2.52f, 3.65f, 1.25f,
        axleConfig = AxleConfigType.TRIDEM, baseColor = Color(0xFF15803D), secondaryColor = Color(0xFFF59E0B), stencilLabel = "VACUUM TANKER",
        description = "Heavy-wall tipping vacuum sludge vessel with hydraulic rear door clamps and suction boom."),
    TrailerItem(49, "Heavy Tactical Tank Transporter", TrailerCategoryType.SPECIALTY, 16400f, 70000f, 78f, 16.8f, 3.30f, 1.75f, 0.98f,
        hitchType = TrailerHitchType.DETACHABLE_GOOSENECK, axleConfig = AxleConfigType.MODULAR_6, suspensionType = SuspensionType.HYDRAULIC_MODULAR,
        baseColor = Color(0xFF4B5320), secondaryColor = Color(0xFF292E18), stencilLabel = "HET M1000 70T",
        description = "Military heavy equipment transporter with folding rear loading ramps and armored winch deck."),
    TrailerItem(50, "Armored Escort Security Trailer", TrailerCategoryType.SPECIALTY, 9600f, 18000f, 58f, 12.2f, 2.55f, 3.65f, 1.25f,
        axleConfig = AxleConfigType.TANDEM, baseColor = Color(0xFF334155), secondaryColor = Color(0xFFB91C1C), stencilLabel = "ARMORED CLASS B6",
        description = "Ballistic-plated security vault trailer with interlocking airlock doors and reinforced roof.")
)
