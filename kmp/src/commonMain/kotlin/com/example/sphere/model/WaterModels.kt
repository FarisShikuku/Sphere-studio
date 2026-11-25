package com.example.sphere.model

import androidx.compose.runtime.Immutable
import androidx.compose.ui.graphics.Color

enum class WaterSpawnScenarioType(
    val ordinalId: Int,
    val label: String,
    val badge: String,
    val description: String
) {
    DAM_BREAK(0, "Dam Break", "Dam", "Left-wall tall fluid column collapse with obstacle impact"),
    DUAL_WAVE(1, "Dual Collision", "Dual", "Opposing fluid walls colliding at tank center"),
    FOUNTAIN_DROP(2, "Plunge Drop", "Drop", "Elevated fluid block dropping onto resting pool"),
    SLOSH_TANK(3, "Slosh Basin", "Slosh", "Tilted high-energy asymmetric fluid block"),
    ZERO_G_ORB(4, "Zero-G Orb", "Orb", "Cohesive suspended sphere bound by surface tension"),
    WATERFALL_CLIFF(5, "Waterfall Cliff", "Cliff", "Continuous cascading river pouring over a 3D rocky cliff face")
}

enum class FluidDebugViewType(
    val ordinalId: Int,
    val label: String,
    val shaderPassName: String
) {
    COMPOSITE(0, "SSFR Composite", "fluid_composite.frag"),
    RAW_DEPTH(1, "Raw Depth", "particle_depth.frag"),
    SMOOTHED_DEPTH(2, "Bilateral Depth", "bilateral_blur.frag"),
    NORMALS(3, "View Normals", "fluid_composite.frag (dFdx/dFdy)"),
    THICKNESS(4, "Thickness", "particle_thickness.frag"),
    PARTICLE_SPHERES(5, "SPH Particles", "particle_depth.vert/frag")
}

enum class WaterInteractionMode(val label: String) {
    ORBIT_CAMERA("Orbit Camera"),
    STIR_FLUID("Stir Fluid")
}

@Immutable
data class WaterPhysicsState(
    val particleCount: Int = 1400,
    val scenario: WaterSpawnScenarioType = WaterSpawnScenarioType.DAM_BREAK,
    val smoothingRadius: Float = 0.30f,
    val targetDensity: Float = 10.5f,
    val pressureMultiplier: Float = 44.0f,
    val nearPressureMultiplier: Float = 16.0f,
    val viscosityStrength: Float = 0.14f,
    val surfaceTension: Float = 0.35f,
    val collisionDamping: Float = 0.62f,
    val gravityX: Float = 0.0f,
    val gravityY: Float = -9.81f,
    val gravityZ: Float = 0.0f,
    val windForceX: Float = 0.0f,
    val windForceZ: Float = 0.0f,
    val timeScale: Float = 1.0f,
    val substeps: Int = 2
)

@Immutable
data class WaterOpticsState(
    val debugMode: FluidDebugViewType = FluidDebugViewType.COMPOSITE,
    val particleVisualRadius: Float = 0.095f,
    val bilateralBlurEnabled: Boolean = true,
    val blurRadiusPx: Int = 5,
    val blurSpatialSigma: Float = 2.8f,
    val blurDepthSigma: Float = 0.085f,
    val waterColor: Color = Color(0.08f, 0.56f, 0.86f),
    val absorbR: Float = 0.72f,
    val absorbG: Float = 0.22f,
    val absorbB: Float = 0.08f,
    val absorptionStrength: Float = 1.85f,
    val indexOfRefraction: Float = 1.333f,
    val refractionStrength: Float = 0.18f,
    val fresnelPower: Float = 4.2f,
    val fresnelBias: Float = 0.02f,
    val specularPower: Float = 180.0f,
    val reflectionIntensity: Float = 0.85f,
    val foamThreshold: Float = 1.65f,
    val causticsIntensity: Float = 0.65f,
    val sunAzimuthDeg: Float = 42.0f,
    val sunElevationDeg: Float = 52.0f
)

@Immutable
data class WaterTankState(
    val boundsWidth: Float = 2.8f,
    val boundsHeight: Float = 2.2f,
    val boundsDepth: Float = 2.4f,
    val waveMakerEnabled: Boolean = false,
    val waveFrequency: Float = 1.15f,
    val waveAmplitude: Float = 0.38f,
    val obstacleEnabled: Boolean = true,
    val obstacleX: Float = 0.25f,
    val obstacleY: Float = 0.42f,
    val obstacleZ: Float = 0.0f,
    val obstacleRadius: Float = 0.38f,
    val showContainerBox: Boolean = true,
    val showFloorGrid: Boolean = true,
    val impulseStrength: Float = 2.8f
)

@Immutable
data class WaterWaterfallState(
    val enabled: Boolean = false,
    val cliffHeight: Float = 1.48f,
    val cliffWidth: Float = 1.25f,
    val cliffDepth: Float = 0.92f,
    val flowVolume: Float = 1.15f,
    val lipSlopeDeg: Float = 12.0f,
    val undercut: Float = 0.18f,
    val steppedLedges: Int = 1,
    val roughness: Float = 0.38f,
    val rockChanneling: Float = 0.80f,
    val airDragAndMist: Float = 0.28f,
    val plungePoolDepth: Float = 0.32f
)

@Immutable
data class WaterfallCascadePreset(
    val name: String,
    val icon: String,
    val description: String,
    val waterfall: WaterWaterfallState,
    val windX: Float = 0.0f,
    val windZ: Float = 0.0f,
    val gravityY: Float = -9.81f,
    val foamThreshold: Float = 1.45f
)

val waterfallCascadePresetsList = listOf(
    WaterfallCascadePreset(
        name = "Sheer Canyon Fall",
        icon = "🏞️",
        description = "Tall vertical granite drop plunging straight into a deep turquoise pool",
        waterfall = WaterWaterfallState(
            enabled = true,
            cliffHeight = 1.72f,
            cliffWidth = 1.15f,
            cliffDepth = 0.95f,
            flowVolume = 1.25f,
            lipSlopeDeg = 8.0f,
            undercut = 0.24f,
            steppedLedges = 0,
            roughness = 0.28f,
            rockChanneling = 0.88f,
            airDragAndMist = 0.26f,
            plungePoolDepth = 0.40f
        ),
        foamThreshold = 1.45f
    ),
    WaterfallCascadePreset(
        name = "Multi-Tier Cascade",
        icon = "🪨",
        description = "3 stepped basalt rock terraces breaking the torrent into frothy ribbons",
        waterfall = WaterWaterfallState(
            enabled = true,
            cliffHeight = 1.65f,
            cliffWidth = 1.45f,
            cliffDepth = 0.85f,
            flowVolume = 1.35f,
            lipSlopeDeg = 16.0f,
            undercut = 0.08f,
            steppedLedges = 3,
            roughness = 0.62f,
            rockChanneling = 0.72f,
            airDragAndMist = 0.32f,
            plungePoolDepth = 0.28f
        ),
        foamThreshold = 1.25f
    ),
    WaterfallCascadePreset(
        name = "Bridal Veil Mist",
        icon = "🌫️",
        description = "Wide low-volume curtain catching crosswind & dispersing into fine mist",
        waterfall = WaterWaterfallState(
            enabled = true,
            cliffHeight = 1.85f,
            cliffWidth = 1.95f,
            cliffDepth = 0.80f,
            flowVolume = 0.65f,
            lipSlopeDeg = 4.0f,
            undercut = 0.32f,
            steppedLedges = 0,
            roughness = 0.22f,
            rockChanneling = 0.55f,
            airDragAndMist = 0.78f,
            plungePoolDepth = 0.24f
        ),
        windX = 1.1f,
        windZ = 1.4f,
        foamThreshold = 1.15f
    ),
    WaterfallCascadePreset(
        name = "Alpine Flash Flood",
        icon = "🌊",
        description = "High-discharge chute torrent roaring down a steep sloped canyon lip",
        waterfall = WaterWaterfallState(
            enabled = true,
            cliffHeight = 1.42f,
            cliffWidth = 1.05f,
            cliffDepth = 1.15f,
            flowVolume = 2.20f,
            lipSlopeDeg = 28.0f,
            undercut = 0.12f,
            steppedLedges = 1,
            roughness = 0.50f,
            rockChanneling = 1.0f,
            airDragAndMist = 0.36f,
            plungePoolDepth = 0.48f
        ),
        foamThreshold = 1.10f
    ),
    WaterfallCascadePreset(
        name = "Undercut Cave Fall",
        icon = "🕳️",
        description = "Deeply recessed basalt overhang creating a free-hanging water curtain",
        waterfall = WaterWaterfallState(
            enabled = true,
            cliffHeight = 1.56f,
            cliffWidth = 1.32f,
            cliffDepth = 1.05f,
            flowVolume = 1.40f,
            lipSlopeDeg = 6.0f,
            undercut = 0.42f,
            steppedLedges = 0,
            roughness = 0.34f,
            rockChanneling = 0.82f,
            airDragAndMist = 0.22f,
            plungePoolDepth = 0.35f
        ),
        foamThreshold = 1.40f
    )
)

@Immutable
data class WaterCameraState(
    val yawDeg: Float = -34.0f,
    val pitchDeg: Float = 22.0f,
    val distance: Float = 4.6f,
    val autoOrbit: Boolean = false
)

@Immutable
data class WaterTelemetryState(
    val activeParticles: Int = 1400,
    val occupiedCells: Int = 0,
    val hashTableSize: Int = 4096,
    val physicsStepMs: Float = 0f,
    val renderPassMs: Float = 0f,
    val avgDensity: Float = 10.5f,
    val maxSpeed: Float = 0f,
    val kineticEnergy: Float = 0f,
    val vulkanGpuActive: Boolean = false
)

@Immutable
data class WaterPreset(
    val name: String,
    val icon: String,
    val description: String,
    val physics: WaterPhysicsState,
    val optics: WaterOpticsState,
    val tank: WaterTankState,
    val waterfall: WaterWaterfallState = WaterWaterfallState(enabled = false)
)

val waterPresetsList = listOf(
    WaterPreset(
        name = "Tropical Lagoon",
        icon = "🏝️",
        description = "Clear turquoise dam break with crisp caustics & Beer-Lambert absorption",
        physics = WaterPhysicsState(
            particleCount = 1400,
            scenario = WaterSpawnScenarioType.DAM_BREAK,
            viscosityStrength = 0.12f,
            surfaceTension = 0.32f,
            gravityY = -9.81f
        ),
        optics = WaterOpticsState(
            waterColor = Color(0.06f, 0.68f, 0.88f),
            absorbR = 0.82f,
            absorbG = 0.16f,
            absorbB = 0.06f,
            absorptionStrength = 1.75f,
            causticsIntensity = 0.80f,
            sunElevationDeg = 58f
        ),
        tank = WaterTankState(
            waveMakerEnabled = false,
            obstacleEnabled = true
        )
    ),
    WaterPreset(
        name = "Cliff Waterfall",
        icon = "🏞️",
        description = "Continuous cascade plunging over a rocky basalt cliff with plunge pool foam",
        physics = WaterPhysicsState(
            particleCount = 1600,
            scenario = WaterSpawnScenarioType.WATERFALL_CLIFF,
            viscosityStrength = 0.10f,
            surfaceTension = 0.25f,
            gravityY = -9.81f
        ),
        optics = WaterOpticsState(
            waterColor = Color(0.10f, 0.64f, 0.88f),
            absorbR = 0.72f,
            absorbG = 0.20f,
            absorbB = 0.07f,
            absorptionStrength = 1.65f,
            foamThreshold = 1.35f,
            causticsIntensity = 0.75f,
            sunAzimuthDeg = 35f,
            sunElevationDeg = 54f
        ),
        tank = WaterTankState(
            boundsWidth = 3.4f,
            boundsHeight = 2.5f,
            boundsDepth = 2.5f,
            waveMakerEnabled = false,
            obstacleEnabled = false
        ),
        waterfall = WaterWaterfallState(enabled = true)
    ),
    WaterPreset(
        name = "Stormy Wave Tank",
        icon = "🌊",
        description = "Oscillating wave paddle + dual opposing collision & whitecap foam",
        physics = WaterPhysicsState(
            particleCount = 1600,
            scenario = WaterSpawnScenarioType.DUAL_WAVE,
            viscosityStrength = 0.10f,
            surfaceTension = 0.28f,
            windForceX = 1.6f,
            gravityY = -9.81f
        ),
        optics = WaterOpticsState(
            waterColor = Color(0.05f, 0.35f, 0.58f),
            absorbR = 0.78f,
            absorbG = 0.42f,
            absorbB = 0.18f,
            absorptionStrength = 2.35f,
            foamThreshold = 1.25f,
            reflectionIntensity = 0.95f,
            sunElevationDeg = 32f
        ),
        tank = WaterTankState(
            boundsWidth = 3.2f,
            waveMakerEnabled = true,
            waveFrequency = 1.35f,
            waveAmplitude = 0.48f,
            obstacleEnabled = true,
            obstacleX = 0.0f,
            obstacleY = 0.44f
        )
    ),
    WaterPreset(
        name = "Crystal Pool",
        icon = "💎",
        description = "High-clarity plunge drop with strong Snell refraction & low absorption",
        physics = WaterPhysicsState(
            particleCount = 1400,
            scenario = WaterSpawnScenarioType.FOUNTAIN_DROP,
            viscosityStrength = 0.08f,
            surfaceTension = 0.42f,
            gravityY = -9.81f
        ),
        optics = WaterOpticsState(
            waterColor = Color(0.18f, 0.78f, 0.96f),
            absorbR = 0.42f,
            absorbG = 0.10f,
            absorbB = 0.04f,
            absorptionStrength = 0.95f,
            indexOfRefraction = 1.333f,
            refractionStrength = 0.26f,
            specularPower = 240f,
            causticsIntensity = 0.90f
        ),
        tank = WaterTankState(
            waveMakerEnabled = false,
            obstacleEnabled = true,
            obstacleX = 0.0f,
            obstacleY = 0.36f,
            obstacleRadius = 0.42f
        )
    ),
    WaterPreset(
        name = "Viscous Honey",
        icon = "🍯",
        description = "High-viscosity non-Newtonian amber fluid slosh with thick absorption",
        physics = WaterPhysicsState(
            particleCount = 1200,
            scenario = WaterSpawnScenarioType.SLOSH_TANK,
            viscosityStrength = 1.45f,
            surfaceTension = 0.85f,
            pressureMultiplier = 36f,
            collisionDamping = 0.45f,
            gravityY = -9.81f
        ),
        optics = WaterOpticsState(
            waterColor = Color(0.88f, 0.56f, 0.08f),
            absorbR = 0.14f,
            absorbG = 0.48f,
            absorbB = 0.92f,
            absorptionStrength = 2.40f,
            indexOfRefraction = 1.49f,
            foamThreshold = 3.2f,
            specularPower = 120f
        ),
        tank = WaterTankState(
            waveMakerEnabled = false,
            obstacleEnabled = false
        )
    ),
    WaterPreset(
        name = "Zero-G Mercury",
        icon = "🪐",
        description = "Microgravity cohesive liquid orb dominated by Poly6 surface tension",
        physics = WaterPhysicsState(
            particleCount = 1300,
            scenario = WaterSpawnScenarioType.ZERO_G_ORB,
            smoothingRadius = 0.34f,
            viscosityStrength = 0.22f,
            surfaceTension = 1.55f,
            gravityY = 0.0f,
            collisionDamping = 0.85f
        ),
        optics = WaterOpticsState(
            waterColor = Color(0.76f, 0.82f, 0.90f),
            absorbR = 0.25f,
            absorbG = 0.25f,
            absorbB = 0.22f,
            absorptionStrength = 1.45f,
            fresnelPower = 2.0f,
            fresnelBias = 0.38f,
            reflectionIntensity = 1.25f,
            specularPower = 260f,
            foamThreshold = 3.0f
        ),
        tank = WaterTankState(
            waveMakerEnabled = false,
            obstacleEnabled = false
        )
    )
)

enum class WaterSurfaceDebugViewType(
    val ordinalId: Int,
    val label: String,
    val shaderPassName: String
) {
    LIT_OPTICS(0, "Lit Ocean", "water_surface.frag"),
    DISPLACEMENT_MAP(1, "Displacement", "phillips_fft_spectrum.comp (dx,y,dz)"),
    NORMAL_MAP(2, "Normal Map", "water_surface.vert (∇h, Jacobian)"),
    SEABED_DEPTH(3, "Seabed Depth", "underwater_terrain.vert (Value Noise)"),
    FRESNEL_AND_FOAM(4, "Fresnel & Foam", "water_surface.frag (Schlick + J)"),
    WIREFRAME_GRID(5, "Displaced Mesh", "water_surface.vert (N×N grid)")
}

@Immutable
data class WaterSurfaceState(
    val enabled: Boolean = false,
    val gridResolution: Int = 64,
    val patchSizeM: Float = 64.0f,
    val windSpeedMs: Float = 14.0f,
    val windDirectionDeg: Float = 35.0f,
    val amplitudeScale: Float = 0.45f,
    val choppinessLambda: Float = 1.25f,
    val smallWaveCutoff: Float = 0.08f,
    val timeScale: Float = 1.0f,
    val waterDepthM: Float = 6.5f,
    val seabedRoughness: Float = 1.8f,
    val seabedScale: Float = 0.14f,
    val sunAzimuthDeg: Float = 42.0f,
    val sunElevationDeg: Float = 34.0f,
    val skyTurbidity: Float = 2.6f,
    val indexOfRefraction: Float = 1.333f,
    val absorptionStrength: Float = 0.55f,
    val scatterStrength: Float = 0.48f,
    val foamThreshold: Float = 0.35f,
    val reflectionStrength: Float = 1.0f,
    val cutoutEnabled: Boolean = false,
    val cutoutWidthM: Float = 10.0f,
    val cutoutHeightM: Float = 10.0f,
    val cutoutDepthM: Float = 10.0f,
    val debugMode: WaterSurfaceDebugViewType = WaterSurfaceDebugViewType.LIT_OPTICS
)

@Immutable
data class WaterCutoutSizePreset(
    val label: String,
    val widthM: Float,
    val heightM: Float,
    val depthM: Float
)

val waterCutoutSizePresets = listOf(
    WaterCutoutSizePreset("6×6×6 m", 6f, 6f, 6f),
    WaterCutoutSizePreset("10×10×10 m", 10f, 10f, 10f),
    WaterCutoutSizePreset("16×10×16 m", 16f, 10f, 16f),
    WaterCutoutSizePreset("24×14×24 m", 24f, 14f, 24f)
)

@Immutable
data class WaterSurfacePreset(
    val name: String,
    val icon: String,
    val description: String,
    val state: WaterSurfaceState
)

val waterSurfacePresetsList = listOf(
    WaterSurfacePreset(
        name = "10m Ocean Cut",
        icon = "🧊",
        description = "Compact 10m×10m×10m cross-section block showing waves, seabed strata & control performance",
        state = WaterSurfaceState(
            enabled = true,
            gridResolution = 64,
            patchSizeM = 22f,
            windSpeedMs = 13.5f,
            windDirectionDeg = 35f,
            amplitudeScale = 0.48f,
            choppinessLambda = 1.35f,
            waterDepthM = 6.2f,
            seabedRoughness = 1.6f,
            seabedScale = 0.22f,
            sunAzimuthDeg = 42f,
            sunElevationDeg = 42f,
            skyTurbidity = 2.4f,
            absorptionStrength = 0.55f,
            scatterStrength = 0.58f,
            foamThreshold = 0.35f,
            cutoutEnabled = true,
            cutoutWidthM = 10f,
            cutoutHeightM = 10f,
            cutoutDepthM = 10f
        )
    ),
    WaterSurfacePreset(
        name = "Trade Wind Swell",
        icon = "🌊",
        description = "Balanced Tessendorf Phillips ocean swells with translucent emerald wave crests",
        state = WaterSurfaceState(
            enabled = true,
            gridResolution = 64,
            patchSizeM = 64f,
            windSpeedMs = 14f,
            windDirectionDeg = 35f,
            amplitudeScale = 0.45f,
            choppinessLambda = 1.25f,
            waterDepthM = 6.5f,
            seabedRoughness = 1.8f,
            sunAzimuthDeg = 42f,
            sunElevationDeg = 34f,
            skyTurbidity = 2.6f,
            absorptionStrength = 0.55f,
            scatterStrength = 0.48f,
            foamThreshold = 0.35f
        )
    ),
    WaterSurfacePreset(
        name = "Tropical Shallows",
        icon = "🏝️",
        description = "Crystal-clear sunlit sandbar seabed with crisp Snell refraction & caustics",
        state = WaterSurfaceState(
            enabled = true,
            gridResolution = 64,
            patchSizeM = 48f,
            windSpeedMs = 8.5f,
            windDirectionDeg = 20f,
            amplitudeScale = 0.32f,
            choppinessLambda = 0.85f,
            waterDepthM = 3.6f,
            seabedRoughness = 1.4f,
            seabedScale = 0.18f,
            sunAzimuthDeg = 55f,
            sunElevationDeg = 58f,
            skyTurbidity = 2.1f,
            absorptionStrength = 0.32f,
            scatterStrength = 0.65f,
            foamThreshold = 0.20f
        )
    ),
    WaterSurfacePreset(
        name = "North Atlantic Gale",
        icon = "⛈️",
        description = "Heavy choppy storm sea with steep trochoidal crests and whitecap foam",
        state = WaterSurfaceState(
            enabled = true,
            gridResolution = 64,
            patchSizeM = 96f,
            windSpeedMs = 24f,
            windDirectionDeg = -45f,
            amplitudeScale = 0.72f,
            choppinessLambda = 1.85f,
            waterDepthM = 14.0f,
            seabedRoughness = 2.4f,
            sunAzimuthDeg = 18f,
            sunElevationDeg = 22f,
            skyTurbidity = 5.4f,
            absorptionStrength = 0.95f,
            scatterStrength = 0.38f,
            foamThreshold = 0.58f
        )
    ),
    WaterSurfacePreset(
        name = "Golden Sunset Glass",
        icon = "🌅",
        description = "Low Preetham solar elevation casting warm specular glitter over gentle ripples",
        state = WaterSurfaceState(
            enabled = true,
            gridResolution = 64,
            patchSizeM = 52f,
            windSpeedMs = 6.2f,
            windDirectionDeg = 12f,
            amplitudeScale = 0.24f,
            choppinessLambda = 0.65f,
            waterDepthM = 8.0f,
            seabedRoughness = 1.2f,
            sunAzimuthDeg = 10f,
            sunElevationDeg = 11f,
            skyTurbidity = 4.6f,
            absorptionStrength = 0.62f,
            scatterStrength = 0.42f,
            foamThreshold = 0.12f,
            reflectionStrength = 1.25f
        )
    ),
    WaterSurfacePreset(
        name = "High-Res Mesh 128",
        icon = "📐",
        description = "High-density 128x128 FFT displaced grid inspection with sharp capillary detail",
        state = WaterSurfaceState(
            enabled = true,
            gridResolution = 128,
            patchSizeM = 72f,
            windSpeedMs = 16.5f,
            windDirectionDeg = 55f,
            amplitudeScale = 0.50f,
            choppinessLambda = 1.45f,
            smallWaveCutoff = 0.04f,
            waterDepthM = 7.5f,
            seabedRoughness = 2.0f,
            sunAzimuthDeg = 48f,
            sunElevationDeg = 40f,
            skyTurbidity = 2.4f,
            debugMode = WaterSurfaceDebugViewType.WIREFRAME_GRID
        )
    )
)
