package com.example.sphere

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.Pause
import androidx.compose.material.icons.filled.PlayArrow
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.isActive
import kotlin.random.Random

/**
 * WeatherStudioScreen - Modular Weather Studio Environment Coordinator.
 * Sub-components:
 *  - [ProceduralCloudSynthesizer] (Nubis Perlin-Worley raymarched volumetrics)
 *  - [WeatherViewportCanvas] & [WeatherPrecipitationRenderer]
 *  - [PrecipitationTab], [SnowFrostTab], [CloudsFogTab], [WindStormsTab], [NativeEngineTab]
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun WeatherStudioScreen(
    onNavigateBack: () -> Unit,
    modifier: Modifier = Modifier
) {
    var selectedPreset by remember { mutableStateOf(weatherPresetsList[0]) }
    var selectedTab by remember { mutableIntStateOf(0) }

    // Precipitation parameters
    var rainIntensity by remember { mutableFloatStateOf(selectedPreset.rainIntensity) }
    var dropSpeed by remember { mutableFloatStateOf(16f) }
    var dropletSize by remember { mutableFloatStateOf(1.2f) }
    var splashDensity by remember { mutableFloatStateOf(0.45f) }
    var puddleAccumulation by remember { mutableFloatStateOf(0.35f) }

    // Snow & Frost parameters
    var snowIntensity by remember { mutableFloatStateOf(selectedPreset.snowIntensity) }
    var flakeFlutter by remember { mutableFloatStateOf(1.2f) }
    var frostGlaze by remember { mutableFloatStateOf(0.25f) }
    var groundSnowCover by remember { mutableFloatStateOf(0.0f) }

    // Clouds & Fog parameters
    var cloudType by remember { mutableIntStateOf(selectedPreset.cloudType) }
    var cloudCoverage by remember { mutableFloatStateOf(selectedPreset.cloudCoverage) }
    var cloudDensity by remember { mutableFloatStateOf(0.68f) }
    var silverLiningG by remember { mutableFloatStateOf(0.82f) }
    var powderEffect by remember { mutableFloatStateOf(0.45f) }
    var sunAbsorption by remember { mutableFloatStateOf(0.35f) }
    var detailErosion by remember { mutableFloatStateOf(0.40f) }
    var cloudBaseAltitude by remember { mutableFloatStateOf(1200f) }
    var raymarchSteps by remember { mutableIntStateOf(32) }
    var enableGodRays by remember { mutableStateOf(true) }
    var godRaysIntensity by remember { mutableFloatStateOf(0.65f) }
    var enableCirrus by remember { mutableStateOf(true) }
    var cirrusCoverage by remember { mutableFloatStateOf(0.35f) }
    var billowDepth by remember { mutableFloatStateOf(0.55f) }
    var sunBacklight by remember { mutableFloatStateOf(0.70f) }
    var fogDensity by remember { mutableFloatStateOf(selectedPreset.fogDensity) }
    var groundMist by remember { mutableFloatStateOf(0.15f) }
    var multiScatteringFactor by remember { mutableFloatStateOf(0.55f) }
    var enableBayerJitter by remember { mutableStateOf(true) }

    // Wind & Storms parameters
    var windSpeed by remember { mutableFloatStateOf(selectedPreset.windSpeed) }
    var windAzimuth by remember { mutableFloatStateOf(selectedPreset.windAzimuth) }
    var lightningFrequency by remember { mutableFloatStateOf(if (selectedPreset.isStormy) 0.85f else 0.0f) }
    var isStormActive by remember { mutableStateOf(selectedPreset.isStormy) }
    var lightningFlash by remember { mutableFloatStateOf(0.0f) }
    var lightningSeed by remember { mutableIntStateOf(42) }

    // Simulation animation state
    var isSimulating by remember { mutableStateOf(true) }
    var timeSeconds by remember { mutableFloatStateOf(0f) }

    // Pre-allocated particle buffers
    val rainParticles = remember {
        List(140) {
            floatArrayOf(Random.nextFloat(), Random.nextFloat(), 0.6f + Random.nextFloat() * 0.8f, 10f + Random.nextFloat() * 10f)
        }
    }
    val snowParticles = remember {
        List(110) {
            floatArrayOf(Random.nextFloat(), Random.nextFloat(), Random.nextFloat() * 6.28f, 1.5f + Random.nextFloat() * 2.5f)
        }
    }

    // Procedural AmanSachan1/Meteoros volumetric cloud bitmap
    val cloudImage = remember(
        cloudCoverage,
        cloudDensity,
        cloudType,
        silverLiningG,
        powderEffect,
        sunAbsorption,
        detailErosion,
        isStormActive,
        multiScatteringFactor,
        enableBayerJitter
    ) {
        ProceduralCloudSynthesizer.renderPerspectiveSky(
            width = 240,
            height = 135,
            camYaw = 0f,
            camPitch = 0f,
            timeSeconds = 0f,
            windSpeed = windSpeed,
            windAzimuth = windAzimuth,
            cloudType = cloudType,
            coverage = cloudCoverage,
            density = cloudDensity,
            silverLiningG = silverLiningG,
            powderEffect = powderEffect,
            sunAbsorption = sunAbsorption,
            detailErosion = detailErosion,
            cloudBaseAltitude = cloudBaseAltitude,
            isStorm = isStormActive,
            horizonYFrac = 0.62f,
            multiScatteringFactor = multiScatteringFactor,
            enableBayerJitter = enableBayerJitter
        ).asImageBitmap()
    }

    // Animation frame loop
    LaunchedEffect(isSimulating) {
        if (!isSimulating) return@LaunchedEffect
        var lastTime = withFrameNanos { it }
        while (isActive) {
            withFrameNanos { frameNanos ->
                val dt = ((frameNanos - lastTime) / 1_000_000_000f).coerceIn(0.001f, 0.05f)
                lastTime = frameNanos
                timeSeconds += dt

                // Decay lightning flash
                if (lightningFlash > 0f) {
                    lightningFlash = (lightningFlash - dt * 3.5f).coerceAtLeast(0f)
                } else if (isStormActive && Random.nextFloat() < (lightningFrequency * dt * 0.45f)) {
                    lightningFlash = 1.0f
                    lightningSeed = Random.nextInt(10000)
                }
            }
        }
    }

    fun applyPreset(preset: WeatherPreset) {
        selectedPreset = preset
        rainIntensity = preset.rainIntensity
        snowIntensity = preset.snowIntensity
        cloudCoverage = preset.cloudCoverage
        fogDensity = preset.fogDensity
        windSpeed = preset.windSpeed
        windAzimuth = preset.windAzimuth
        isStormActive = preset.isStormy
        cloudType = preset.cloudType
        lightningFrequency = if (preset.isStormy) 0.85f else 0.0f
        if (preset.snowIntensity > 0.3f) {
            groundSnowCover = 0.85f
            frostGlaze = 0.65f
        } else {
            groundSnowCover = 0.0f
            frostGlaze = 0.15f
        }
    }

    val tabTitles = listOf("Precipitation", "Snow & Frost", "Clouds & Fog", "Wind & Storms", "Vulkan C++")

    Scaffold(
        modifier = modifier.fillMaxSize().testTag("weather_studio_screen"),
        topBar = {
            TopAppBar(
                title = {
                    Column {
                        Text(
                            "Dynamic Weather Studio",
                            style = MaterialTheme.typography.titleMedium.copy(fontWeight = FontWeight.Bold)
                        )
                        Text(
                            "AmanSachan1/Meteoros Volumetric Clouds & Vulkan Engine",
                            style = MaterialTheme.typography.bodySmall.copy(
                                color = MaterialTheme.colorScheme.onSurfaceVariant,
                                fontSize = 11.sp
                            )
                        )
                    }
                },
                navigationIcon = {
                    IconButton(onClick = onNavigateBack, modifier = Modifier.testTag("weather_back_button")) {
                        Icon(Icons.AutoMirrored.Filled.ArrowBack, contentDescription = "Back")
                    }
                },
                actions = {
                    IconButton(onClick = { isSimulating = !isSimulating }) {
                        Icon(
                            if (isSimulating) Icons.Default.Pause else Icons.Default.PlayArrow,
                            contentDescription = if (isSimulating) "Pause Simulation" else "Resume Simulation",
                            tint = if (isSimulating) MaterialTheme.colorScheme.primary else Color.Gray
                        )
                    }
                },
                colors = TopAppBarDefaults.topAppBarColors(
                    containerColor = MaterialTheme.colorScheme.surface
                )
            )
        }
    ) { innerPadding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(innerPadding)
        ) {
            // Preset Chips Row
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .background(MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.35f))
                    .horizontalScroll(rememberScrollState())
                    .padding(horizontal = 12.dp, vertical = 8.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    "PRESETS:",
                    style = MaterialTheme.typography.labelSmall.copy(
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary
                    )
                )
                weatherPresetsList.forEach { preset ->
                    val isSelected = preset.name == selectedPreset.name
                    FilterChip(
                        selected = isSelected,
                        onClick = { applyPreset(preset) },
                        label = {
                            Text(
                                "${preset.icon} ${preset.name}",
                                style = MaterialTheme.typography.labelMedium
                            )
                        },
                        colors = FilterChipDefaults.filterChipColors(
                            selectedContainerColor = MaterialTheme.colorScheme.primaryContainer,
                            selectedLabelColor = MaterialTheme.colorScheme.onPrimaryContainer
                        )
                    )
                }
            }

            // Photorealistic Weather Viewport Canvas
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(270.dp)
                    .background(Color.Black)
            ) {
                Canvas(
                    modifier = Modifier.fillMaxSize().testTag("weather_viewport_canvas")
                ) {
                    val w = size.width
                    val h = size.height
                    val horizonY = h * 0.62f

                    drawWeatherAtmosphere(
                        w = w,
                        h = h,
                        horizonY = horizonY,
                        cloudCoverage = cloudCoverage,
                        fogDensity = fogDensity,
                        isStorm = isStormActive,
                        lightningFlash = lightningFlash,
                        rainIntensity = rainIntensity,
                        snowIntensity = snowIntensity
                    )

                    drawGroundSurface(
                        w = w,
                        h = h,
                        horizonY = horizonY,
                        camPitch = 0f,
                        camYaw = 0f,
                        surfaceMode = SurfaceSceneMode.ENDLESS,
                        stageWidth = 10.0f,
                        stageLength = 10.0f,
                        puddleDepth = puddleAccumulation,
                        snowCover = groundSnowCover,
                        wetness = frostGlaze,
                        cloudCoverage = cloudCoverage,
                        windSpeed = windSpeed,
                        cloudWave = 0.5f,
                        lightningFlash = lightningFlash
                    )

                    drawPhotorealisticCloudscape(
                        w = w,
                        h = h,
                        horizonY = horizonY,
                        cloudImage = cloudImage,
                        coverage = cloudCoverage,
                        density = cloudDensity,
                        enableGodRays = enableGodRays,
                        godRaysIntensity = godRaysIntensity,
                        enableCirrus = enableCirrus,
                        cirrusCoverage = cirrusCoverage,
                        sunBacklight = sunBacklight,
                        camYaw = 0f,
                        camPitch = 0f,
                        windSpeed = windSpeed,
                        cloudWave = 0.5f,
                        timeSeconds = timeSeconds,
                        isStorm = isStormActive,
                        lightningFlash = lightningFlash
                    )

                    if (lightningFlash > 0.1f) {
                        drawProceduralLightning(
                            w = w,
                            h = h,
                            flashIntensity = lightningFlash,
                            seed = lightningSeed,
                            horizonY = horizonY
                        )
                    }

                    if (rainIntensity > 0.05f) {
                        drawRainStreaks(
                            w = w,
                            h = h,
                            particles = rainParticles,
                            intensity = rainIntensity,
                            dropSpeed = dropSpeed,
                            windAngle = windAzimuth,
                            windSpeed = windSpeed,
                            time = timeSeconds,
                            horizonY = horizonY
                        )
                    }

                    if (snowIntensity > 0.05f) {
                        drawSnowFlurries(
                            w = w,
                            h = h,
                            particles = snowParticles,
                            intensity = snowIntensity,
                            flutter = flakeFlutter,
                            windSpeed = windSpeed,
                            time = timeSeconds,
                            horizonY = horizonY
                        )
                    }

                    if (fogDensity > 0.02f || groundMist > 0.02f) {
                        drawFogVolumeOverlay(
                            w = w,
                            h = h,
                            density = fogDensity,
                            mist = groundMist,
                            lightningFlash = lightningFlash,
                            horizonY = horizonY
                        )
                    }
                }

                // Weather HUD Overlay
                Surface(
                    color = Color.Black.copy(alpha = 0.65f),
                    shape = RoundedCornerShape(8.dp),
                    modifier = Modifier
                        .align(Alignment.TopStart)
                        .padding(12.dp)
                ) {
                    Column(modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp)) {
                        Text(
                            "${selectedPreset.icon} ${selectedPreset.name.uppercase()}",
                            color = Color.White,
                            fontWeight = FontWeight.Bold,
                            fontSize = 12.sp,
                            fontFamily = FontFamily.Monospace
                        )
                        val detectedCloudLabel = when (cloudType) {
                            0 -> "🌫 [St] Stratus Overcast Veil"
                            1 -> "🌊 [Sc] Stratocumulus Waves"
                            2 -> "⛅ [Cu] Cumulus Cauliflower"
                            else -> "⛈ [Cb] Cumulonimbus Nimbus"
                        }
                        Text(
                            "Wind: ${"%.1f".format(windSpeed)} m/s • Rain: ${"%.0f".format(rainIntensity * 100)}% • Clouds: ${"%.0f".format(cloudCoverage * 100)}%",
                            color = Color.Cyan.copy(alpha = 0.9f),
                            fontSize = 10.sp,
                            fontFamily = FontFamily.Monospace
                        )
                        Text(
                            "Detected: $detectedCloudLabel",
                            color = Color(0xFFA78BFA),
                            fontSize = 10.sp,
                            fontWeight = FontWeight.Bold,
                            fontFamily = FontFamily.Monospace
                        )
                    }
                }
            }

            // Tab Navigation Row
            ScrollableTabRow(
                selectedTabIndex = selectedTab,
                edgePadding = 12.dp,
                modifier = Modifier.fillMaxWidth()
            ) {
                tabTitles.forEachIndexed { index, title ->
                    Tab(
                        selected = selectedTab == index,
                        onClick = { selectedTab = index },
                        text = {
                            Text(
                                title,
                                style = MaterialTheme.typography.labelMedium.copy(
                                    fontWeight = if (selectedTab == index) FontWeight.Bold else FontWeight.Normal
                                )
                            )
                        }
                    )
                }
            }

            // Control Tabs Content
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .weight(1f)
                    .background(MaterialTheme.colorScheme.background)
            ) {
                when (selectedTab) {
                    0 -> PrecipitationTab(
                        rainIntensity = rainIntensity, onRainIntensityChange = { rainIntensity = it },
                        dropSpeed = dropSpeed, onDropSpeedChange = { dropSpeed = it },
                        dropletSize = dropletSize, onDropletSizeChange = { dropletSize = it },
                        splashDensity = splashDensity, onSplashDensityChange = { splashDensity = it },
                        puddleAccumulation = puddleAccumulation, onPuddleAccumulationChange = { puddleAccumulation = it }
                    )
                    1 -> SnowFrostTab(
                        snowIntensity = snowIntensity, onSnowIntensityChange = { snowIntensity = it },
                        flakeFlutter = flakeFlutter, onFlakeFlutterChange = { flakeFlutter = it },
                        frostGlaze = frostGlaze, onFrostGlazeChange = { frostGlaze = it },
                        groundSnowCover = groundSnowCover, onGroundSnowCoverChange = { groundSnowCover = it }
                    )
                    2 -> CloudsFogTab(
                        cloudType = cloudType, onCloudTypeChange = { cloudType = it },
                        cloudCoverage = cloudCoverage, onCloudCoverageChange = { cloudCoverage = it },
                        cloudDensity = cloudDensity, onCloudDensityChange = { cloudDensity = it },
                        silverLiningG = silverLiningG, onSilverLiningGChange = { silverLiningG = it },
                        powderEffect = powderEffect, onPowderEffectChange = { powderEffect = it },
                        sunAbsorption = sunAbsorption, onSunAbsorptionChange = { sunAbsorption = it },
                        detailErosion = detailErosion, onDetailErosionChange = { detailErosion = it },
                        cloudBaseAltitude = cloudBaseAltitude, onCloudBaseAltitudeChange = { cloudBaseAltitude = it },
                        raymarchSteps = raymarchSteps, onRaymarchStepsChange = { raymarchSteps = it },
                        enableGodRays = enableGodRays, onEnableGodRaysChange = { enableGodRays = it },
                        godRaysIntensity = godRaysIntensity, onGodRaysIntensityChange = { godRaysIntensity = it },
                        enableCirrus = enableCirrus, onEnableCirrusChange = { enableCirrus = it },
                        cirrusCoverage = cirrusCoverage, onCirrusCoverageChange = { cirrusCoverage = it },
                        billowDepth = billowDepth, onBillowDepthChange = { billowDepth = it },
                        sunBacklight = sunBacklight, onSunBacklightChange = { sunBacklight = it },
                        fogDensity = fogDensity, onFogDensityChange = { fogDensity = it },
                        groundMist = groundMist, onGroundMistChange = { groundMist = it },
                        multiScatteringFactor = multiScatteringFactor,
                        onMultiScatteringFactorChange = { multiScatteringFactor = it },
                        enableBayerJitter = enableBayerJitter,
                        onEnableBayerJitterChange = { enableBayerJitter = it }
                    )
                    3 -> WindStormsTab(
                        windSpeed = windSpeed, onWindSpeedChange = { windSpeed = it },
                        windAzimuth = windAzimuth, onWindAzimuthChange = { windAzimuth = it },
                        lightningFrequency = lightningFrequency, onLightningFrequencyChange = { lightningFrequency = it },
                        isStormActive = isStormActive, onStormActiveChange = { isStormActive = it },
                        onTriggerStrike = {
                            lightningFlash = 1.0f
                            lightningSeed = Random.nextInt(10000)
                        }
                    )
                    4 -> NativeEngineTab()
                }
            }
        }
    }
}

