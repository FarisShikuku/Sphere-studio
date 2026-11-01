package com.example.sphere.model

import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Rect
import androidx.compose.ui.graphics.Color
import kotlin.math.abs
import kotlin.math.cos
import kotlin.math.sin
import kotlin.math.PI

/**
 * World Meteorological Organization (WMO) Cloud Genera & Species Classification
 */
enum class CloudGenus(
    val displayName: String,
    val symbolCode: String,
    val categoryTier: String,
    val commonName: String,
    val baseAltitudeMeters: Int,
    val topAltitudeMeters: Int,
    val badgeColor: Color,
    val description: String,
    val typicalPrecipitation: String,
    val opticalCharacter: String,
    val iconEmoji: String
) {
    CUMULONIMBUS(
        displayName = "Cumulonimbus",
        symbolCode = "Cb",
        categoryTier = "Vertical Development (High Threat)",
        commonName = "Anvil Thunderhead / Nimbus",
        baseAltitudeMeters = 800,
        topAltitudeMeters = 13500,
        badgeColor = Color(0xFFEF4444), // Bright red/crimson alert
        description = "Monolithic thunderstorm colossus with fibrous glaciated incus anvil top and turbulent dark boiling updraft bases.",
        typicalPrecipitation = "Heavy downpour, hail, severe squalls, cloud-to-ground lightning",
        opticalCharacter = "Extremely opaque (τ > 50). Intense self-shadowing and virga curtain.",
        iconEmoji = "⛈"
    ),
    CUMULUS(
        displayName = "Cumulus",
        symbolCode = "Cu",
        categoryTier = "Low-Level (Convective)",
        commonName = "Fair Weather Cauliflower",
        baseAltitudeMeters = 1200,
        topAltitudeMeters = 3800,
        badgeColor = Color(0xFF10B981), // Emerald green
        description = "Isolated fluffy billows with sharp, brilliant white cauliflower domes and nearly horizontal, shaded condensation bases.",
        typicalPrecipitation = "None (Cumulus humilis) to brief localized showers (Cumulus congestus)",
        opticalCharacter = "Brilliant forward solar silver-lining; moderate opacity (τ ~ 15-30).",
        iconEmoji = "⛅"
    ),
    STRATUS(
        displayName = "Stratus",
        symbolCode = "St",
        categoryTier = "Low-Level (Stratiform)",
        commonName = "Uniform Overcast Fog-Sheet",
        baseAltitudeMeters = 300,
        topAltitudeMeters = 1200,
        badgeColor = Color(0xFF64748B), // Slate gray
        description = "Continuous gray, diffuse cloud layer often resembling lifted fog, obscuring hilltops and landscape contours.",
        typicalPrecipitation = "Fine mist, freezing drizzle, or light snow grains",
        opticalCharacter = "Uniform diffuse scattering, softens or completely hides the solar disk.",
        iconEmoji = "🌫"
    ),
    STRATOCUMULUS(
        displayName = "Stratocumulus",
        symbolCode = "Sc",
        categoryTier = "Low-Level (Stratiform)",
        commonName = "Rolling Lumpy Blanket",
        baseAltitudeMeters = 700,
        topAltitudeMeters = 2400,
        badgeColor = Color(0xFF8B5CF6), // Purple
        description = "Broad patches, rolls, or continuous wavy sheets of grayish-white clouds with dark shaded underside undulations.",
        typicalPrecipitation = "Light rain or snow showers, virga trails",
        opticalCharacter = "Variable optical thickness (τ ~ 10-25) with distinct light/dark banding.",
        iconEmoji = "🌊"
    ),
    CIRRUS(
        displayName = "Cirrus",
        symbolCode = "Ci",
        categoryTier = "High-Level (Ice Crystals)",
        commonName = "Feathery Mare's Tails",
        baseAltitudeMeters = 7500,
        topAltitudeMeters = 12500,
        badgeColor = Color(0xFF38BDF8), // Cyan sky
        description = "Detached, delicate fibrous white filaments and tufted silky bands bent into hooked claws by high-altitude jetstreams.",
        typicalPrecipitation = "None reaches surface (sublimating high-altitude virga)",
        opticalCharacter = "High optical translucency (τ ~ 0.2-2.0); casts faint or no shadow.",
        iconEmoji = "🪶"
    ),
    ALTOCUMULUS(
        displayName = "Altocumulus",
        symbolCode = "Ac",
        categoryTier = "Mid-Level (Mixed Phase)",
        commonName = "Mackerel Sky Ripples",
        baseAltitudeMeters = 2800,
        topAltitudeMeters = 5500,
        badgeColor = Color(0xFFF59E0B), // Amber
        description = "Rippled sheets or parallel cellular rolls resembling fleece or fish scales, composed of supercooled liquid droplets.",
        typicalPrecipitation = "Virga streaks; indicator of approaching frontal instability",
        opticalCharacter = "Produces colorful solar corona and iridescent diffraction rings.",
        iconEmoji = "☁"
    ),
    NIMBOSTRATUS(
        displayName = "Nimbostratus",
        symbolCode = "Ns",
        categoryTier = "Multi-Level / Nimbus",
        commonName = "Continuous Dark Rain Veil",
        baseAltitudeMeters = 600,
        topAltitudeMeters = 4500,
        badgeColor = Color(0xFF0284C7), // Deep ocean blue
        description = "Thick, dark gray overcast layer diffuse with falling precipitation; blotted sun with ragged pannus fractus clouds beneath.",
        typicalPrecipitation = "Continuous moderate-to-heavy steady rain or snowfall",
        opticalCharacter = "High extinction (τ ~ 35-70); solar disk completely extinguished.",
        iconEmoji = "🌧"
    ),
    CIRROCUMULUS(
        displayName = "Cirrocumulus",
        symbolCode = "Cc",
        categoryTier = "High-Level (Ice Crystals)",
        commonName = "High Rippled Cloudlet Field",
        baseAltitudeMeters = 6500,
        topAltitudeMeters = 11000,
        badgeColor = Color(0xFFA7F3D0), // Mint ice
        description = "Thin white sheet of very small, tiny rippled grains or ripples without individual shading, resembling sand ripples.",
        typicalPrecipitation = "None",
        opticalCharacter = "Very thin (τ < 1.0); shimmering refraction in sunlight.",
        iconEmoji = "✨"
    )
}

/**
 * A detected cloud formation in celestial coordinates (azimuth, elevation).
 */
data class DetectedCloudFormation(
    val id: String,
    val genus: CloudGenus,
    val speciesVariant: String,
    val azimuthDegrees: Float,       // [0..360] horizontal celestial direction
    val elevationDegrees: Float,     // [0..90] altitude angle above horizon
    val angularWidthDegrees: Float,  // Horizontal span across field of view
    val angularHeightDegrees: Float, // Vertical span across field of view
    val baseAltitudeMeters: Float,
    val topAltitudeMeters: Float,
    val opticalDepthTau: Float,
    val confidenceScore: Float,      // [0..1] e.g. 0.982 = 98.2%
    val radarReflectivityDbz: Float, // Weather radar echo e.g. 48 dBZ
    val dropletWaterGramsPerM3: Float,
    val iceCrystalFraction: Float,   // [0..1] 1.0 = glaciated ice
    val convectiveUpdraftMs: Float,  // Updraft velocity e.g. 14.5 m/s
    val detectionTimestamp: String = "LIVE TRACK"
)

/**
 * Projected 2D bounding box and viewport metadata for rendering on canvas
 */
data class ProjectedCloudDetection(
    val formation: DetectedCloudFormation,
    val screenRect: Rect,
    val centerOffset: Offset,
    val isVisibleInFrustum: Boolean,
    val distanceCenterNorm: Float // 0 = dead center, >1 = peripheral
)

/**
 * Atmospheric catalog of default cloud formations across the 360-degree sky dome
 */
object CloudDetectionRepository {
    val defaultFormations = listOf(
        DetectedCloudFormation(
            id = "CLOUD-CB-01",
            genus = CloudGenus.CUMULONIMBUS,
            speciesVariant = "capillatus incus",
            azimuthDegrees = 145f,
            elevationDegrees = 26f,
            angularWidthDegrees = 38f,
            angularHeightDegrees = 32f,
            baseAltitudeMeters = 900f,
            topAltitudeMeters = 12800f,
            opticalDepthTau = 58.4f,
            confidenceScore = 0.987f,
            radarReflectivityDbz = 52.5f,
            dropletWaterGramsPerM3 = 3.8f,
            iceCrystalFraction = 0.65f,
            convectiveUpdraftMs = 18.2f
        ),
        DetectedCloudFormation(
            id = "CLOUD-CU-02",
            genus = CloudGenus.CUMULUS,
            speciesVariant = "congestus / mediocris",
            azimuthDegrees = 210f,
            elevationDegrees = 34f,
            angularWidthDegrees = 24f,
            angularHeightDegrees = 19f,
            baseAltitudeMeters = 1350f,
            topAltitudeMeters = 3900f,
            opticalDepthTau = 22.1f,
            confidenceScore = 0.964f,
            radarReflectivityDbz = 18.0f,
            dropletWaterGramsPerM3 = 1.4f,
            iceCrystalFraction = 0.05f,
            convectiveUpdraftMs = 4.5f
        ),
        DetectedCloudFormation(
            id = "CLOUD-CI-03",
            genus = CloudGenus.CIRRUS,
            speciesVariant = "fibratus uncinus",
            azimuthDegrees = 85f,
            elevationDegrees = 52f,
            angularWidthDegrees = 32f,
            angularHeightDegrees = 14f,
            baseAltitudeMeters = 8400f,
            topAltitudeMeters = 11800f,
            opticalDepthTau = 1.4f,
            confidenceScore = 0.991f,
            radarReflectivityDbz = -4.0f,
            dropletWaterGramsPerM3 = 0.05f,
            iceCrystalFraction = 1.0f,
            convectiveUpdraftMs = 0.2f
        ),
        DetectedCloudFormation(
            id = "CLOUD-SC-04",
            genus = CloudGenus.STRATOCUMULUS,
            speciesVariant = "stratiformis undulatus",
            azimuthDegrees = 290f,
            elevationDegrees = 18f,
            angularWidthDegrees = 42f,
            angularHeightDegrees = 16f,
            baseAltitudeMeters = 950f,
            topAltitudeMeters = 2100f,
            opticalDepthTau = 16.8f,
            confidenceScore = 0.948f,
            radarReflectivityDbz = 12.0f,
            dropletWaterGramsPerM3 = 0.8f,
            iceCrystalFraction = 0.02f,
            convectiveUpdraftMs = 1.1f
        ),
        DetectedCloudFormation(
            id = "CLOUD-ST-05",
            genus = CloudGenus.STRATUS,
            speciesVariant = "nebulosus",
            azimuthDegrees = 20f,
            elevationDegrees = 12f,
            angularWidthDegrees = 46f,
            angularHeightDegrees = 12f,
            baseAltitudeMeters = 350f,
            topAltitudeMeters = 1100f,
            opticalDepthTau = 19.5f,
            confidenceScore = 0.952f,
            radarReflectivityDbz = 8.5f,
            dropletWaterGramsPerM3 = 0.5f,
            iceCrystalFraction = 0.0f,
            convectiveUpdraftMs = 0.1f
        ),
        DetectedCloudFormation(
            id = "CLOUD-AC-06",
            genus = CloudGenus.ALTOCUMULUS,
            speciesVariant = "floccus / castellanus",
            azimuthDegrees = 180f,
            elevationDegrees = 45f,
            angularWidthDegrees = 28f,
            angularHeightDegrees = 14f,
            baseAltitudeMeters = 3200f,
            topAltitudeMeters = 5100f,
            opticalDepthTau = 8.7f,
            confidenceScore = 0.973f,
            radarReflectivityDbz = 5.0f,
            dropletWaterGramsPerM3 = 0.6f,
            iceCrystalFraction = 0.35f,
            convectiveUpdraftMs = 2.4f
        ),
        DetectedCloudFormation(
            id = "CLOUD-NS-07",
            genus = CloudGenus.NIMBOSTRATUS,
            speciesVariant = "praecipitatio",
            azimuthDegrees = 330f,
            elevationDegrees = 22f,
            angularWidthDegrees = 48f,
            angularHeightDegrees = 22f,
            baseAltitudeMeters = 650f,
            topAltitudeMeters = 4200f,
            opticalDepthTau = 45.2f,
            confidenceScore = 0.979f,
            radarReflectivityDbz = 38.0f,
            dropletWaterGramsPerM3 = 2.6f,
            iceCrystalFraction = 0.40f,
            convectiveUpdraftMs = 3.2f
        )
    )

    /**
     * Projects a celestial formation onto screen viewport space given current camera orientation.
     */
    fun projectFormation(
        formation: DetectedCloudFormation,
        screenWidth: Float,
        screenHeight: Float,
        camYaw: Float,
        camPitch: Float,
        horizontalFovDeg: Float = 80f,
        verticalFovDeg: Float = 60f
    ): ProjectedCloudDetection {
        val dYaw = (formation.azimuthDegrees - camYaw + 540f) % 360f - 180f
        val dPitch = formation.elevationDegrees - camPitch

        val halfW = screenWidth * 0.5f
        val halfH = screenHeight * 0.5f

        val xNorm = (dYaw / (horizontalFovDeg * 0.5f))
        val yNorm = (dPitch / (verticalFovDeg * 0.5f))

        val screenX = halfW + xNorm * halfW
        val screenY = halfH - yNorm * halfH

        val rectWidth = (formation.angularWidthDegrees / horizontalFovDeg) * screenWidth
        val rectHeight = (formation.angularHeightDegrees / verticalFovDeg) * screenHeight

        val left = screenX - rectWidth * 0.5f
        val top = screenY - rectHeight * 0.5f
        val rect = Rect(left, top, left + rectWidth, top + rectHeight)

        val isVisible = abs(dYaw) < (horizontalFovDeg * 0.65f) && abs(dPitch) < (verticalFovDeg * 0.75f)
        val distNorm = kotlin.math.sqrt(xNorm * xNorm + yNorm * yNorm)

        return ProjectedCloudDetection(
            formation = formation,
            screenRect = rect,
            centerOffset = Offset(screenX, screenY),
            isVisibleInFrustum = isVisible,
            distanceCenterNorm = distNorm
        )
    }
}
