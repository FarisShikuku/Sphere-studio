package com.example.sphere.model

import androidx.compose.ui.graphics.Color
import kotlin.math.*

/**
 * Punctual, Area, and Directional light classifications matching PlainRenderer.
 */
enum class LightSourceType(
    val label: String,
    val shortName: String,
    val description: String,
    val vulkanTypeId: Float
) {
    SPOT_FLASHLIGHT("🔦 Spot / Flashlight", "Flashlight", "Directional cone beam with inner core, penumbra falloff, and volumetric beam", 2.0f),
    POINT_BULB("💡 Point / Filament Bulb", "Bulb", "Omnidirectional 360° point light with inverse-square physical distance falloff", 1.0f),
    AREA_SOFTBOX("⬛ Rectangular Area Light", "Area Light", "Architectural rectangular softbox panel providing broad diffuse highlights", 3.0f),
    DIRECTIONAL_SUN("☀️ Directional Sun", "Sun", "Collimated parallel rays modeling natural sunlight and solar elevation", 0.0f);

    val displayName: String get() = shortName

    companion object {
        val SPOT get() = SPOT_FLASHLIGHT
        val POINT get() = POINT_BULB
        val AREA get() = AREA_SOFTBOX
        val DIRECTIONAL get() = DIRECTIONAL_SUN
    }
}

enum class TargetLightingMesh(val label: String, val description: String) {
    CALIBRATION_CUBE("3D Test Cube", "Photometric 1.2m calibration cube with coordinate crosshairs"),
    ARCHITECTURAL_WALL("Architectural Wall", "Vertical brick / concrete wall slab evaluating surface grazing light"),
    GROUND_SLAB("Ground Slab", "Horizontal 3D floor slab evaluating light footprint and falloff rings"),
    PBR_SPHERE("PBR Sphere", "Curved microfacet sphere showing specular highlight lobes and Fresnel rims");

    val displayName: String get() = label

    companion object {
        val BOX_CUBE get() = CALIBRATION_CUBE
        val WALL get() = ARCHITECTURAL_WALL
        val FLOOR get() = GROUND_SLAB
        val SPHERE get() = PBR_SPHERE
    }
}

/**
 * PlainRenderer Multi-Pass Pipeline Debug Views
 */
enum class PipelineDebugViewMode(val label: String, val shortName: String, val description: String) {
    FINAL_LIT("Combined PBR Lit", "Final Lit", "Complete composite: Direct Lights + Cascaded Shadows + SDF Diffuse GI + Atmosphere Sky"),
    DIRECT_LIGHT_ONLY("Direct Light Only", "Direct Only", "Isolated primary light contribution without indirect bounces or ambient sky"),
    INDIRECT_GI_ONLY("SDF GI Indirect Bounce", "GI Bounce", "Pure indirect diffuse radiance gathered via 3D SDF cone tracing (Color Bleeding)"),
    DISTANCE_FIELD_VOXELS("SDF Distance Field", "SDF Voxels", "Continuous signed distance field isosurfaces and voxel density grid"),
    SHADOW_CASCADES_DEBUG("Shadow Cascades (CSM)", "Cascades", "Color-coded 4-split shadow frustums (Red=C0, Green=C1, Blue=C2, Yellow=C3)"),
    ATMOSPHERE_SKY_ONLY("Atmospheric Sky", "Sky Only", "Single-scattering Rayleigh/Mie sky radiance and solar disk"),
    DFAO_AMBIENT_OCCLUSION("Distance Field AO", "DFAO", "Contact shadowing and ambient occlusion computed from SDF step distances");

    val displayName: String get() = shortName
}

/**
 * Signed Distance Field Diffuse GI parameters referencing Gaukler/PlainRenderer.
 */
data class SdfGiParameters(
    val enabled: Boolean = true,
    val bounceFactor: Float = 1.0f,
    val aoIntensity: Float = 0.85f,
    val coneTracingAngleDeg: Float = 45.0f,
    val maxRayDistanceMeters: Float = 12.0f,
    val indirectColorBoost: Float = 1.35f,
    val voxelResolution: Int = 64, // 32, 64, or 128
    val multiBounceApprox: Boolean = true
) {
    val voxelGridDisplay: String get() = "${voxelResolution}x${voxelResolution}x${voxelResolution}"
}

/**
 * Physically Based Sky & Sun parameters referencing Gaukler/PlainRenderer.
 */
data class AtmosphereParameters(
    val sunAzimuthDeg: Float = 145.0f,
    val sunElevationDeg: Float = 38.0f,
    val turbidity: Float = 2.45f,          // 1.0 = pristine air, 2.5 = clear continental, 6.0+ = hazy
    val mieAsymmetry: Float = 0.76f,       // Henyey-Greenstein g factor
    val solarIlluminanceLux: Float = 120000.0f,
    val groundAlbedo: Color = Color(0xFF262626)
) {
    // Computed direct solar illuminance at surface (in Lux)
    val directSunIlluminanceLux: Float
        get() {
            val sinElev = sin(Math.toRadians(max(0.0, sunElevationDeg.toDouble()))).toFloat()
            val trans = exp(-0.18f * turbidity / max(0.05f, sinElev))
            return solarIlluminanceLux * sinElev * trans
        }

    // Sky diffuse irradiance (in Lux)
    val skyDiffuseIrradianceLux: Float
        get() {
            val sinElev = sin(Math.toRadians(max(0.0, sunElevationDeg.toDouble()))).toFloat()
            return solarIlluminanceLux * 0.16f * sinElev.pow(0.7f)
        }
}

/**
 * Directional Cascaded Shadow Map (CSM) parameters referencing Gaukler/PlainRenderer.
 */
data class CascadedShadowParameters(
    val enabled: Boolean = true,
    val cascadeCount: Int = 4,
    val splitLambda: Float = 0.75f,        // 0.0 = uniform, 1.0 = logarithmic
    val shadowMapResolution: Int = 2048,
    val pcfKernelSize: Float = 2.0f,       // 1.0 = 3x3, 2.0 = 5x5, 3.0 = 7x7
    val normalBias: Float = 0.005f,
    val depthBias: Float = 0.0015f,
    val subTexelStabilization: Boolean = true,
    val splitDistances: List<Float> = listOf(5.0f, 15.0f, 40.0f, 100.0f)
)

/**
 * Temporal Anti-Aliasing (TAA) parameters referencing Gaukler/PlainRenderer.
 */
data class TaaParameters(
    val enabled: Boolean = true,
    val historyWeight: Float = 0.92f,
    val jitterPhases: Int = 16,
    val neighborhoodClamping: Boolean = true
)

/**
 * Real-time Vulkan Pipeline Telemetry
 */
data class PipelineTelemetryStats(
    val activePassesCount: Int = 6,
    val shadowPassMs: Float = 0.35f,
    val gbufferPassMs: Float = 0.42f,
    val sdfGiComputeMs: Float = 1.15f,
    val atmosphereSkyMs: Float = 0.22f,
    val deferredLightingMs: Float = 0.58f,
    val taaResolveMs: Float = 0.28f,
    val totalFrameMs: Float = 3.00f,
    val frameRateFps: Int = 60
)

enum class LightScenarioPresetId(
    val id: String,
    val title: String,
    val description: String,
    val lightType: LightSourceType
) {
    TACTICAL_FLASHLIGHT(
        "flashlight_tactical",
        "Tactical Flashlight",
        "High-intensity focused spot beam piercing dark shadows with smooth penumbra cutoff and volumetric dust cone",
        LightSourceType.SPOT_FLASHLIGHT
    ),
    WARM_FILAMENT_BULB(
        "warm_filament_bulb",
        "Hanging Filament Bulb",
        "Cozy 2400K tungsten Edison bulb casting radial omnidirectional illumination with inverse-square falloff",
        LightSourceType.POINT_BULB
    ),
    ARCHITECTURAL_SOFTBOX(
        "architectural_softbox",
        "Studio Softbox Panel",
        "1.6m x 1.0m rectangular diffuse emitter producing gentle architectural highlights and broad reflections",
        LightSourceType.AREA_SOFTBOX
    ),
    CORNELL_BOX_GI(
        "cornell_box_gi",
        "Cornell Box (SDF Diffuse GI)",
        "Iconic global illumination benchmark: Red left wall and Blue back wall casting indirect color bleeding onto floor and cube",
        LightSourceType.SPOT_FLASHLIGHT
    ),
    SUNLIT_EXTERIOR(
        "sunlit_exterior",
        "Sunlit Exterior (4x CSM)",
        "Physical Sun with dynamic Rayleigh/Mie atmospheric scattering, 4-split cascaded shadow maps, and sky diffuse fill",
        LightSourceType.DIRECTIONAL_SUN
    ),
    THREE_POINT_STUDIO(
        "three_point_studio",
        "3-Point Studio Rig",
        "Cinematic studio configuration with Key spot, soft ambient Fill, and rim silhouette lighting",
        LightSourceType.SPOT_FLASHLIGHT
    ),
    CYBERPUNK_NEON(
        "cyberpunk_neon",
        "Cyberpunk Neon Cyan",
        "Electric cyan saturated spot beam with heavy atmospheric fog beam scattering and ambient haze",
        LightSourceType.SPOT_FLASHLIGHT
    ),
    STREET_LAMP_POST(
        "street_lamp_post",
        "Sodium Street Luminaire",
        "Warm 2100K high-pressure sodium overhead luminaire directed downward onto stage platform",
        LightSourceType.SPOT_FLASHLIGHT
    )
}

data class LightSourceParameters(
    val type: LightSourceType = LightSourceType.SPOT_FLASHLIGHT,
    val intensityLumens: Float = 3500.0f,
    val colorKelvin: Float = 5500.0f,
    val useKelvin: Boolean = true,
    val customColor: Color = Color(0xFFF4F8FC),
    val posX: Float = 0.0f,
    val posY: Float = 2.4f,
    val posZ: Float = 2.2f,
    val pitchDeg: Float = -35.0f,
    val yawDeg: Float = 0.0f,
    val rangeMeters: Float = 20.0f,
    val bulbRadiusMeters: Float = 0.12f,
    val spotInnerDeg: Float = 16.0f,
    val spotOuterDeg: Float = 34.0f,
    val spotFalloff: Float = 1.0f,
    val areaWidthMeters: Float = 1.4f,
    val areaHeightMeters: Float = 0.9f,
    val volumetricHaze: Boolean = true,
    val volumetricIntensity: Float = 0.70f,
    val castShadows: Boolean = true,
    val shadowSoftness: Float = 1.8f
) {
    // Computed Blackbody or Custom RGB color
    val effectiveColor: Color
        get() = if (useKelvin) kelvinToColor(colorKelvin) else customColor

    // Compatibility accessors for UI sliders
    val colorTemperatureKelvin: Float get() = colorKelvin
    val range: Float get() = rangeMeters
    val spotInnerAngleDeg: Float get() = spotInnerDeg
    val spotOuterAngleDeg: Float get() = spotOuterDeg
    val areaWidth: Float get() = areaWidthMeters
    val areaHeight: Float get() = areaHeightMeters
    val enableVolumetric: Boolean get() = volumetricHaze
    val volumetricDensity: Float get() = volumetricIntensity

    // Direction vector computed from pitch and yaw
    val dirX: Float
        get() {
            val yawRad = Math.toRadians(yawDeg.toDouble())
            val pitchRad = Math.toRadians(pitchDeg.toDouble())
            return (sin(yawRad) * cos(pitchRad)).toFloat()
        }

    val dirY: Float
        get() {
            val pitchRad = Math.toRadians(pitchDeg.toDouble())
            return sin(pitchRad).toFloat()
        }

    val dirZ: Float
        get() {
            val yawRad = Math.toRadians(yawDeg.toDouble())
            val pitchRad = Math.toRadians(pitchDeg.toDouble())
            return (-cos(yawRad) * cos(pitchRad)).toFloat()
        }

    fun withColorKelvin(k: Float): LightSourceParameters = copy(colorKelvin = k)
    fun withRange(r: Float): LightSourceParameters = copy(rangeMeters = r)
    fun withSpotAngles(inner: Float, outer: Float): LightSourceParameters = copy(spotInnerDeg = inner, spotOuterDeg = outer)
    fun withAreaDimensions(w: Float, h: Float): LightSourceParameters = copy(areaWidthMeters = w, areaHeightMeters = h)
    fun withVolumetric(enabled: Boolean, density: Float = volumetricIntensity): LightSourceParameters = copy(volumetricHaze = enabled, volumetricIntensity = density)
    fun withDirection(dx: Float, dy: Float, dz: Float): LightSourceParameters {
        val len = sqrt(dx * dx + dy * dy + dz * dz).coerceAtLeast(0.0001f)
        val nx = dx / len
        val ny = dy / len
        val nz = dz / len
        val newYaw = (Math.toDegrees(atan2(nx.toDouble(), -nz.toDouble())).toFloat() + 360f) % 360f
        val newPitch = Math.toDegrees(asin(ny.toDouble().coerceIn(-1.0, 1.0))).toFloat()
        return copy(yawDeg = newYaw, pitchDeg = newPitch)
    }
}

data class LightScenarioPreset(
    val id: LightScenarioPresetId,
    val name: String,
    val description: String,
    val params: LightSourceParameters,
    val sdfGi: SdfGiParameters = SdfGiParameters(),
    val atmosphere: AtmosphereParameters = AtmosphereParameters()
)

val DEFAULT_LIGHTING_SCENARIOS = listOf(
    LightScenarioPreset(
        id = LightScenarioPresetId.TACTICAL_FLASHLIGHT,
        name = "Tactical Flashlight",
        description = "High-intensity focused spot beam with volumetric haze piercing shadows",
        params = LightSourceParameters(
            type = LightSourceType.SPOT_FLASHLIGHT,
            intensityLumens = 2800f,
            colorKelvin = 6200f,
            posX = -1.2f, posY = 2.4f, posZ = 2.4f,
            pitchDeg = -38f, yawDeg = 24f,
            rangeMeters = 25f,
            spotInnerDeg = 14f, spotOuterDeg = 30f,
            volumetricHaze = true, volumetricIntensity = 0.75f,
            castShadows = true
        )
    ),
    LightScenarioPreset(
        id = LightScenarioPresetId.WARM_FILAMENT_BULB,
        name = "Hanging Filament Bulb",
        description = "Cozy 2400K tungsten Edison bulb with 360° omnidirectional radial decay",
        params = LightSourceParameters(
            type = LightSourceType.POINT_BULB,
            intensityLumens = 1400f,
            colorKelvin = 2400f,
            posX = 0f, posY = 2.2f, posZ = 0.5f,
            rangeMeters = 15f,
            volumetricHaze = false,
            castShadows = true
        )
    ),
    LightScenarioPreset(
        id = LightScenarioPresetId.ARCHITECTURAL_SOFTBOX,
        name = "Studio Softbox Panel",
        description = "1.6m x 1.0m rectangular diffuse emitter producing soft architectural highlights",
        params = LightSourceParameters(
            type = LightSourceType.AREA_SOFTBOX,
            intensityLumens = 4500f,
            colorKelvin = 5400f,
            posX = -1.8f, posY = 2.8f, posZ = 1.8f,
            pitchDeg = -42f, yawDeg = 40f,
            areaWidthMeters = 1.6f, areaHeightMeters = 1.0f,
            rangeMeters = 22f,
            volumetricHaze = false,
            castShadows = true
        )
    ),
    LightScenarioPreset(
        id = LightScenarioPresetId.CORNELL_BOX_GI,
        name = "Cornell Box (SDF Diffuse GI)",
        description = "Iconic Diffuse GI color bleeding: Red left wall and blue back wall scattering indirect light",
        params = LightSourceParameters(
            type = LightSourceType.SPOT_FLASHLIGHT,
            intensityLumens = 3200f,
            colorKelvin = 5800f,
            posX = 0.0f, posY = 3.2f, posZ = 1.2f,
            pitchDeg = -65f, yawDeg = 0f,
            spotInnerDeg = 35f, spotOuterDeg = 65f,
            rangeMeters = 20f,
            volumetricHaze = false,
            castShadows = true
        ),
        sdfGi = SdfGiParameters(
            enabled = true,
            bounceFactor = 1.65f,
            aoIntensity = 0.90f,
            indirectColorBoost = 1.80f,
            voxelResolution = 64
        )
    ),
    LightScenarioPreset(
        id = LightScenarioPresetId.SUNLIT_EXTERIOR,
        name = "Sunlit Exterior (4x CSM)",
        description = "Physical Sun with dynamic Rayleigh/Mie atmosphere and 4-split cascaded shadow maps",
        params = LightSourceParameters(
            type = LightSourceType.DIRECTIONAL_SUN,
            intensityLumens = 110000f,
            colorKelvin = 5600f,
            posX = 5.0f, posY = 8.0f, posZ = 5.0f,
            pitchDeg = -42f, yawDeg = 145f,
            rangeMeters = 100f,
            volumetricHaze = true, volumetricIntensity = 0.35f,
            castShadows = true
        ),
        atmosphere = AtmosphereParameters(
            sunElevationDeg = 42.0f,
            sunAzimuthDeg = 145.0f,
            turbidity = 2.2f
        )
    ),
    LightScenarioPreset(
        id = LightScenarioPresetId.THREE_POINT_STUDIO,
        name = "3-Point Studio Rig",
        description = "Cinematic photographic setup with 5500K daylight key light and soft shadows",
        params = LightSourceParameters(
            type = LightSourceType.SPOT_FLASHLIGHT,
            intensityLumens = 3800f,
            colorKelvin = 5500f,
            posX = 2.0f, posY = 2.6f, posZ = 2.0f,
            pitchDeg = -35f, yawDeg = -45f,
            spotInnerDeg = 28f, spotOuterDeg = 52f,
            rangeMeters = 24f,
            volumetricHaze = false,
            castShadows = true
        )
    ),
    LightScenarioPreset(
        id = LightScenarioPresetId.CYBERPUNK_NEON,
        name = "Cyberpunk Neon Beam",
        description = "Saturated cyan spot light with heavy atmospheric beam scattering",
        params = LightSourceParameters(
            type = LightSourceType.SPOT_FLASHLIGHT,
            intensityLumens = 3200f,
            useKelvin = false,
            customColor = Color(0xFF00E5FF),
            posX = -1.5f, posY = 2.0f, posZ = 2.0f,
            pitchDeg = -30f, yawDeg = 35f,
            spotInnerDeg = 18f, spotOuterDeg = 38f,
            rangeMeters = 24f,
            volumetricHaze = true, volumetricIntensity = 0.90f,
            castShadows = true
        )
    ),
    LightScenarioPreset(
        id = LightScenarioPresetId.STREET_LAMP_POST,
        name = "Sodium Street Luminaire",
        description = "Warm 2100K high-pressure sodium overhead luminaire with broad street pool",
        params = LightSourceParameters(
            type = LightSourceType.SPOT_FLASHLIGHT,
            intensityLumens = 5500f,
            colorKelvin = 2100f,
            posX = 0f, posY = 3.6f, posZ = 0f,
            pitchDeg = -85f, yawDeg = 0f,
            spotInnerDeg = 40f, spotOuterDeg = 68f,
            rangeMeters = 20f,
            volumetricHaze = true, volumetricIntensity = 0.40f,
            castShadows = true
        )
    )
)

fun kelvinToColor(kelvin: Float): Color {
    val temp = (kelvin.coerceIn(1000f, 15000f)) / 100f
    val r: Float
    val g: Float
    val b: Float

    // Red
    r = if (temp <= 66f) {
        1.0f
    } else {
        (329.698727446f * (temp - 60f).pow(-0.1332047592f) / 255f).coerceIn(0f, 1f)
    }

    // Green
    g = if (temp <= 66f) {
        ((99.4708025861f * ln(temp)) - 161.1195681661f) / 255f
    } else {
        (288.1221695283f * (temp - 60f).pow(-0.0755148492f) / 255f)
    }.coerceIn(0f, 1f)

    // Blue
    b = when {
        temp >= 66f -> 1.0f
        temp <= 19f -> 0.0f
        else -> ((138.5177312231f * ln(temp - 10f)) - 305.0447927307f) / 255f
    }.coerceIn(0f, 1f)

    return Color(r, g, b, 1.0f)
}
