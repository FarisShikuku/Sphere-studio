package com.example.sphere

import androidx.compose.animation.core.LinearEasing
import androidx.compose.animation.core.RepeatMode
import androidx.compose.animation.core.animateFloat
import androidx.compose.animation.core.infiniteRepeatable
import androidx.compose.animation.core.rememberInfiniteTransition
import androidx.compose.animation.core.tween
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.AcUnit
import androidx.compose.material.icons.filled.Air
import androidx.compose.material.icons.filled.Cloud
import androidx.compose.material.icons.filled.Code
import androidx.compose.material.icons.filled.FilterDrama
import androidx.compose.material.icons.filled.FlashOn
import androidx.compose.material.icons.filled.Grain
import androidx.compose.material.icons.filled.Pause
import androidx.compose.material.icons.filled.PlayArrow
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material.icons.filled.WaterDrop
import androidx.compose.material.icons.filled.WbSunny
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ScrollableTabRow
import androidx.compose.material3.Slider
import androidx.compose.material3.SliderDefaults
import androidx.compose.material3.Surface
import androidx.compose.material3.Switch
import androidx.compose.material3.SwitchDefaults
import androidx.compose.material3.Tab
import androidx.compose.material3.TabRowDefaults
import androidx.compose.material3.TabRowDefaults.tabIndicatorOffset
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.delay
import kotlin.math.PI
import kotlin.math.abs
import kotlin.math.cos
import kotlin.math.sin
import kotlin.random.Random

data class WeatherPreset(
    val name: String,
    val icon: String,
    val rainIntensity: Float,
    val snowIntensity: Float,
    val cloudCoverage: Float,
    val fogDensity: Float,
    val windSpeed: Float,
    val windAzimuth: Float,
    val isStormy: Boolean
)

val weatherPresetsList = listOf(
    WeatherPreset("Clear", "☀", 0.0f, 0.0f, 0.10f, 0.01f, 6.0f, 45f, false),
    WeatherPreset("Overcast", "☁", 0.0f, 0.0f, 0.85f, 0.08f, 12.0f, 60f, false),
    WeatherPreset("Rain", "🌧", 0.55f, 0.0f, 0.90f, 0.15f, 18.0f, 225f, false),
    WeatherPreset("Downpour", "⛈", 0.95f, 0.0f, 0.98f, 0.25f, 36.0f, 240f, true),
    WeatherPreset("Snow", "❄", 0.0f, 0.50f, 0.75f, 0.18f, 10.0f, 90f, false),
    WeatherPreset("Blizzard", "🌨", 0.0f, 1.0f, 1.0f, 0.55f, 55.0f, 315f, true),
    WeatherPreset("Dense Fog", "🌫", 0.05f, 0.0f, 0.70f, 0.85f, 3.0f, 0f, false),
    WeatherPreset("Thunderstorm", "⚡", 0.88f, 0.0f, 0.98f, 0.30f, 44.0f, 270f, true)
)

@Composable
fun WeatherStudioScreen(
    modifier: Modifier = Modifier
) {
    // -----------------------------------------------------------------
    // Simulation Parameters
    // -----------------------------------------------------------------
    var currentPresetName by remember { mutableStateOf("Rain") }
    var isSimPlaying by remember { mutableStateOf(true) }

    // Rain params
    var rainIntensity by remember { mutableFloatStateOf(0.60f) }
    var dropSpeed by remember { mutableFloatStateOf(18.0f) }
    var dropletSize by remember { mutableFloatStateOf(1.2f) }
    var splashDensity by remember { mutableFloatStateOf(0.70f) }
    var puddleAccumulation by remember { mutableFloatStateOf(0.48f) }

    // Snow params
    var snowIntensity by remember { mutableFloatStateOf(0.0f) }
    var flakeFlutter by remember { mutableFloatStateOf(0.60f) }
    var frostGlaze by remember { mutableFloatStateOf(0.20f) }
    var groundSnowCover by remember { mutableFloatStateOf(0.0f) }

    // Cloud & Fog params
    var cloudCoverage by remember { mutableFloatStateOf(0.88f) }
    var cloudAltitude by remember { mutableFloatStateOf(1800f) }
    var cloudDensity by remember { mutableFloatStateOf(0.75f) }
    var fogDensity by remember { mutableFloatStateOf(0.18f) }
    var groundMist by remember { mutableFloatStateOf(0.30f) }

    // Wind & Storm params
    var windSpeed by remember { mutableFloatStateOf(22.0f) }
    var windAzimuth by remember { mutableFloatStateOf(225.0f) }
    var lightningFrequency by remember { mutableFloatStateOf(0.35f) }
    var stormTurbulence by remember { mutableFloatStateOf(0.50f) }
    var isStormActive by remember { mutableStateOf(false) }

    // Camera orbit controls
    var camYaw by remember { mutableFloatStateOf(145.0f) }
    var camPitch by remember { mutableFloatStateOf(18.0f) }

    // Active bottom tab
    var activeTab by remember { mutableIntStateOf(0) }

    // Animation & Lightning dynamics
    var timeSeconds by remember { mutableFloatStateOf(0.0f) }
    var lightningFlash by remember { mutableFloatStateOf(0.0f) }
    var lightningSeed by remember { mutableIntStateOf(42) }

    // Precipitation particle state
    val rainParticles = remember {
        List(140) {
            floatArrayOf(
                Random.nextFloat(), // x normalized [0..1]
                Random.nextFloat(), // y normalized [0..1]
                0.4f + Random.nextFloat() * 0.6f, // length
                0.7f + Random.nextFloat() * 0.6f  // speed variance
            )
        }
    }

    val snowParticles = remember {
        List(90) {
            floatArrayOf(
                Random.nextFloat(), // x normalized [0..1]
                Random.nextFloat(), // y normalized [0..1]
                Random.nextFloat() * 6.28f, // phase
                1.5f + Random.nextFloat() * 2.5f // size
            )
        }
    }

    // Simulation loop
    LaunchedEffect(isSimPlaying) {
        var lastStrike = 0f
        while (isSimPlaying) {
            delay(16) // ~60 fps
            timeSeconds += 0.016f

            // Trigger lightning during storms or high lightning frequency
            if ((isStormActive || lightningFrequency > 0.6f) && timeSeconds - lastStrike > (5.0f - lightningFrequency * 4.0f)) {
                lightningFlash = 1.0f
                lightningSeed = Random.nextInt(1000)
                lastStrike = timeSeconds
            }

            if (lightningFlash > 0.0f) {
                lightningFlash = (lightningFlash - 0.05f).coerceAtLeast(0.0f)
            }
        }
    }

    val infiniteTransition = rememberInfiniteTransition(label = "weather_anim")
    val cloudWave by infiniteTransition.animateFloat(
        initialValue = 0f,
        targetValue = 6.28318f,
        animationSpec = infiniteRepeatable(
            animation = tween(durationMillis = 6000, easing = LinearEasing),
            repeatMode = RepeatMode.Restart
        ),
        label = "cloud_wave"
    )

    Column(
        modifier = modifier
            .fillMaxSize()
            .background(Color(0xFF090B10))
            .testTag("weather_studio_screen")
    ) {
        // =============================================================
        // 1. TOP VIEWPORT: 3D PROCEDURAL WEATHER SIMULATION (weight 1.08f)
        // =============================================================
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .weight(1.08f)
                .background(Color(0xFF05070B))
                .pointerInput(Unit) {
                    detectDragGestures { change, dragAmount ->
                        change.consume()
                        camYaw = (camYaw - dragAmount.x * 0.35f + 360f) % 360f
                        camPitch = (camPitch + dragAmount.y * 0.25f).coerceIn(-15f, 75f)
                    }
                }
                .testTag("weather_viewport")
        ) {
            Canvas(modifier = Modifier.fillMaxSize()) {
                val w = size.width
                val h = size.height

                // 1. Dynamic Atmosphere & Sky Gradient
                drawWeatherAtmosphere(
                    w = w,
                    h = h,
                    cloudCoverage = cloudCoverage,
                    fogDensity = fogDensity,
                    isStorm = isStormActive,
                    lightningFlash = lightningFlash,
                    rainIntensity = rainIntensity,
                    snowIntensity = snowIntensity
                )

                // 2. Horizon and Perspective Ground Plane with Puddles
                drawGroundSurface(
                    w = w,
                    h = h,
                    camPitch = camPitch,
                    camYaw = camYaw,
                    puddleDepth = puddleAccumulation,
                    snowCover = groundSnowCover,
                    wetness = (rainIntensity * 0.8f + puddleAccumulation * 0.2f).coerceIn(0f, 1f),
                    lightningFlash = lightningFlash
                )

                // 3. Volumetric Cloud Layer Bands
                drawVolumetricCloudLayers(
                    w = w,
                    h = h,
                    coverage = cloudCoverage,
                    density = cloudDensity,
                    cloudWave = cloudWave,
                    windSpeed = windSpeed,
                    isStorm = isStormActive,
                    lightningFlash = lightningFlash
                )

                // 4. Procedural Lightning Bolt Arc
                if (lightningFlash > 0.15f) {
                    drawProceduralLightning(
                        w = w,
                        h = h,
                        flashIntensity = lightningFlash,
                        seed = lightningSeed
                    )
                }

                // 5. Rain Particle Streaks and Ground Splashes
                if (rainIntensity > 0.05f) {
                    drawRainStreaks(
                        w = w,
                        h = h,
                        particles = rainParticles,
                        intensity = rainIntensity,
                        dropSpeed = dropSpeed,
                        windAngle = windAzimuth,
                        windSpeed = windSpeed,
                        time = timeSeconds
                    )
                }

                // 6. Snow Particle Flurries
                if (snowIntensity > 0.05f) {
                    drawSnowFlurries(
                        w = w,
                        h = h,
                        particles = snowParticles,
                        intensity = snowIntensity,
                        flutter = flakeFlutter,
                        windSpeed = windSpeed,
                        time = timeSeconds
                    )
                }

                // 7. Volumetric Froxel Fog Inscattering
                if (fogDensity > 0.05f) {
                    drawFogVolumeOverlay(
                        w = w,
                        h = h,
                        density = fogDensity,
                        mist = groundMist,
                        lightningFlash = lightningFlash
                    )
                }
            }

            // ---------------------------------------------------------
            // Top HUD Overlay: Badges and Live Telemetry
            // ---------------------------------------------------------
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(10.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                // Weather State Badge
                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = Color(0xFF0F172A).copy(alpha = 0.88f),
                    border = CardDefaults.outlinedCardBorder().copy(
                        brush = Brush.linearGradient(listOf(Color(0xFF1E293B), Color(0xFF0284C7)))
                    )
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        val iconVector = when {
                            snowIntensity > 0.4f -> Icons.Default.AcUnit
                            isStormActive -> Icons.Default.FlashOn
                            rainIntensity > 0.4f -> Icons.Default.WaterDrop
                            fogDensity > 0.4f -> Icons.Default.FilterDrama
                            cloudCoverage > 0.6f -> Icons.Default.Cloud
                            else -> Icons.Default.WbSunny
                        }
                        Icon(
                            imageVector = iconVector,
                            contentDescription = currentPresetName,
                            tint = when {
                                isStormActive -> Color(0xFFFBBF24)
                                snowIntensity > 0.3f -> Color(0xFF93C5FD)
                                rainIntensity > 0.3f -> Color(0xFF38BDF8)
                                else -> Color(0xFFF59E0B)
                            },
                            modifier = Modifier.size(16.dp)
                        )
                        Spacer(modifier = Modifier.width(6.dp))
                        Text(
                            text = currentPresetName.uppercase(),
                            fontSize = 11.sp,
                            fontWeight = FontWeight.Bold,
                            color = Color.White
                        )
                        Text(
                            text = " • ${(rainIntensity * 45.0f + snowIntensity * 25.0f).toInt()} mm/h",
                            fontSize = 10.sp,
                            color = Color(0xFF94A3B8)
                        )
                    }
                }

                // Viewport Controls: Play/Pause, Reset View
                Row(verticalAlignment = Alignment.CenterVertically) {
                    IconButton(
                        onClick = { isSimPlaying = !isSimPlaying },
                        modifier = Modifier
                            .size(32.dp)
                            .background(
                                if (isSimPlaying) Color(0xFF0284C7).copy(alpha = 0.85f)
                                else Color(0xFF334155).copy(alpha = 0.85f),
                                CircleShape
                            )
                            .testTag("toggle_weather_sim_button")
                    ) {
                        Icon(
                            imageVector = if (isSimPlaying) Icons.Default.Pause else Icons.Default.PlayArrow,
                            contentDescription = "Play/Pause weather sim",
                            tint = Color.White,
                            modifier = Modifier.size(16.dp)
                        )
                    }

                    Spacer(modifier = Modifier.width(6.dp))

                    IconButton(
                        onClick = {
                            camPitch = 18.0f
                            camYaw = 145.0f
                        },
                        modifier = Modifier
                            .size(32.dp)
                            .background(Color(0xFF1E293B).copy(alpha = 0.85f), CircleShape)
                            .testTag("reset_weather_camera_button")
                    ) {
                        Icon(
                            imageVector = Icons.Default.Refresh,
                            contentDescription = "Center View",
                            tint = Color(0xFF94A3B8),
                            modifier = Modifier.size(16.dp)
                        )
                    }
                }
            }

            // ---------------------------------------------------------
            // Bottom Overlays: Presets Bar inside Viewport
            // ---------------------------------------------------------
            Row(
                modifier = Modifier
                    .align(Alignment.BottomCenter)
                    .fillMaxWidth()
                    .padding(horizontal = 8.dp, vertical = 6.dp)
                    .horizontalScroll(rememberScrollState()),
                horizontalArrangement = Arrangement.spacedBy(6.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                weatherPresetsList.forEach { preset ->
                    val isSelected = currentPresetName == preset.name
                    Surface(
                        shape = RoundedCornerShape(8.dp),
                        color = if (isSelected)
                            Color(0xFF0284C7).copy(alpha = 0.85f)
                        else
                            Color(0xCC10141E),
                        border = CardDefaults.outlinedCardBorder().copy(
                            brush = Brush.linearGradient(
                                if (isSelected) listOf(Color(0xFF38BDF8), Color(0xFF0284C7))
                                else listOf(Color(0xFF1E293B), Color(0xFF334155))
                            )
                        ),
                        modifier = Modifier
                            .clickable {
                                currentPresetName = preset.name
                                rainIntensity = preset.rainIntensity
                                snowIntensity = preset.snowIntensity
                                cloudCoverage = preset.cloudCoverage
                                fogDensity = preset.fogDensity
                                windSpeed = preset.windSpeed
                                windAzimuth = preset.windAzimuth
                                isStormActive = preset.isStormy
                                puddleAccumulation = (preset.rainIntensity * 0.75f).coerceIn(0f, 1f)
                                groundSnowCover = (preset.snowIntensity * 0.85f).coerceIn(0f, 1f)
                            }
                            .testTag("preset_chip_${preset.name.lowercase()}")
                    ) {
                        Row(
                            modifier = Modifier.padding(horizontal = 10.dp, vertical = 5.dp),
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Text(text = preset.icon, fontSize = 13.sp)
                            Spacer(modifier = Modifier.width(5.dp))
                            Text(
                                text = preset.name,
                                fontSize = 11.sp,
                                fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal,
                                color = if (isSelected) Color.White else Color(0xFFCBD5E1)
                            )
                        }
                    }
                }
            }
        }

        // =============================================================
        // 2. HORIZONTAL DIVIDER (1.dp)
        // =============================================================
        HorizontalDivider(
            thickness = 1.dp,
            color = Color(0xFF1E293B)
        )

        // =============================================================
        // 3. BOTTOM CONTROLS PANEL (weight 0.92f)
        // =============================================================
        Surface(
            modifier = Modifier
                .fillMaxWidth()
                .weight(0.92f),
            color = Color(0xFF0F121A)
        ) {
            Column(modifier = Modifier.fillMaxSize()) {
                ScrollableTabRow(
                    selectedTabIndex = activeTab,
                    containerColor = Color(0xFF131722),
                    contentColor = Color.White,
                    edgePadding = 12.dp,
                    indicator = { tabPositions ->
                        TabRowDefaults.SecondaryIndicator(
                            Modifier.tabIndicatorOffset(tabPositions[activeTab]),
                            color = Color(0xFF38BDF8),
                            height = 3.dp
                        )
                    },
                    modifier = Modifier.fillMaxWidth().testTag("weather_tabs_row")
                ) {
                    val tabs = listOf(
                        "Precipitation",
                        "Snow & Frost",
                        "Clouds & Fog",
                        "Wind & Storms",
                        "Native C++ Core"
                    )
                    tabs.forEachIndexed { index, title ->
                        Tab(
                            selected = activeTab == index,
                            onClick = { activeTab = index },
                            text = {
                                Text(
                                    text = title,
                                    fontSize = 12.sp,
                                    fontWeight = if (activeTab == index) FontWeight.Bold else FontWeight.Normal
                                )
                            }
                        )
                    }
                }

                // Tab Content Body
                Column(
                    modifier = Modifier
                        .fillMaxSize()
                        .verticalScroll(rememberScrollState())
                        .padding(16.dp)
                ) {
                    when (activeTab) {
                        0 -> PrecipitationTab(
                            rainIntensity = rainIntensity,
                            onRainIntensityChange = { rainIntensity = it },
                            dropSpeed = dropSpeed,
                            onDropSpeedChange = { dropSpeed = it },
                            dropletSize = dropletSize,
                            onDropletSizeChange = { dropletSize = it },
                            splashDensity = splashDensity,
                            onSplashDensityChange = { splashDensity = it },
                            puddleAccumulation = puddleAccumulation,
                            onPuddleAccumulationChange = { puddleAccumulation = it }
                        )
                        1 -> SnowFrostTab(
                            snowIntensity = snowIntensity,
                            onSnowIntensityChange = { snowIntensity = it },
                            flakeFlutter = flakeFlutter,
                            onFlakeFlutterChange = { flakeFlutter = it },
                            frostGlaze = frostGlaze,
                            onFrostGlazeChange = { frostGlaze = it },
                            groundSnowCover = groundSnowCover,
                            onGroundSnowCoverChange = { groundSnowCover = it }
                        )
                        2 -> CloudsFogTab(
                            cloudCoverage = cloudCoverage,
                            onCloudCoverageChange = { cloudCoverage = it },
                            cloudAltitude = cloudAltitude,
                            onCloudAltitudeChange = { cloudAltitude = it },
                            cloudDensity = cloudDensity,
                            onCloudDensityChange = { cloudDensity = it },
                            fogDensity = fogDensity,
                            onFogDensityChange = { fogDensity = it },
                            groundMist = groundMist,
                            onGroundMistChange = { groundMist = it }
                        )
                        3 -> WindStormsTab(
                            windSpeed = windSpeed,
                            onWindSpeedChange = { windSpeed = it },
                            windAzimuth = windAzimuth,
                            onWindAzimuthChange = { windAzimuth = it },
                            lightningFrequency = lightningFrequency,
                            onLightningFrequencyChange = { lightningFrequency = it },
                            isStormActive = isStormActive,
                            onStormActiveChange = { isStormActive = it },
                            onTriggerStrike = {
                                lightningFlash = 1.0f
                                lightningSeed = Random.nextInt(1000)
                            }
                        )
                        4 -> NativeEngineTab()
                    }
                }
            }
        }
    }
}

// ---------------------------------------------------------------------
// CANVAS DRAWING EXTENSIONS
// ---------------------------------------------------------------------

private fun DrawScope.drawWeatherAtmosphere(
    w: Float,
    h: Float,
    cloudCoverage: Float,
    fogDensity: Float,
    isStorm: Boolean,
    lightningFlash: Float,
    rainIntensity: Float,
    snowIntensity: Float
) {
    val horizonY = h * 0.58f

    val (topSky, horizonSky) = when {
        lightningFlash > 0.2f -> {
            Color(0xFFCBD5E1) to Color(0xFFF1F5F9)
        }
        isStorm -> {
            Color(0xFF0B101D) to Color(0xFF1E293B)
        }
        snowIntensity > 0.3f -> {
            Color(0xFF1E293B) to Color(0xFF64748B)
        }
        rainIntensity > 0.4f -> {
            Color(0xFF0F172A) to Color(0xFF334155)
        }
        fogDensity > 0.5f -> {
            Color(0xFF334155) to Color(0xFF94A3B8)
        }
        cloudCoverage > 0.7f -> {
            Color(0xFF1E293B) to Color(0xFF475569)
        }
        else -> {
            Color(0xFF0284C7) to Color(0xFF7DD3FC)
        }
    }

    drawRect(
        brush = Brush.verticalGradient(
            colors = listOf(topSky, horizonSky),
            startY = 0f,
            endY = horizonY
        ),
        size = Size(w, horizonY)
    )
}

private fun DrawScope.drawGroundSurface(
    w: Float,
    h: Float,
    camPitch: Float,
    camYaw: Float,
    puddleDepth: Float,
    snowCover: Float,
    wetness: Float,
    lightningFlash: Float
) {
    val horizonY = h * 0.58f

    val baseGroundColor = when {
        lightningFlash > 0.2f -> Color(0xFF475569)
        snowCover > 0.4f -> Color(0xFFCBD5E1)
        wetness > 0.5f -> Color(0xFF0B0F19)
        else -> Color(0xFF131B2A)
    }

    // Ground fill
    drawRect(
        brush = Brush.verticalGradient(
            colors = listOf(baseGroundColor.copy(alpha = 0.95f), baseGroundColor),
            startY = horizonY,
            endY = h
        ),
        topLeft = Offset(0f, horizonY),
        size = Size(w, h - horizonY)
    )

    // Perspective terrain grid lines
    val gridColor = if (snowCover > 0.4f) Color(0xFF94A3B8).copy(alpha = 0.3f)
    else Color(0xFF1E293B).copy(alpha = 0.6f)

    for (i in 0..12) {
        val frac = i / 12f
        val py = horizonY + (h - horizonY) * (frac * frac)
        drawLine(
            color = gridColor,
            start = Offset(0f, py),
            end = Offset(w, py),
            strokeWidth = 1f
        )
    }

    // Perspective vanishing rays
    val centerX = w * 0.5f + (camYaw - 180f) * 0.8f
    for (i in -8..8) {
        val gx = centerX + i * (w * 0.12f)
        drawLine(
            color = gridColor.copy(alpha = 0.35f),
            start = Offset(centerX, horizonY),
            end = Offset(gx, h),
            strokeWidth = 1f
        )
    }

    // Puddle specular reflections on ground
    if (puddleDepth > 0.05f) {
        val puddleColor = if (lightningFlash > 0.2f) Color(0xFFE2E8F0) else Color(0xFF1E3A5F).copy(alpha = 0.65f)
        drawOval(
            color = puddleColor,
            topLeft = Offset(w * 0.22f, horizonY + 28f),
            size = Size(w * 0.26f * puddleDepth, 22f * puddleDepth)
        )
        drawOval(
            color = puddleColor,
            topLeft = Offset(w * 0.58f, horizonY + 54f),
            size = Size(w * 0.32f * puddleDepth, 32f * puddleDepth)
        )
        drawOval(
            color = puddleColor,
            topLeft = Offset(w * 0.36f, horizonY + 95f),
            size = Size(w * 0.42f * puddleDepth, 38f * puddleDepth)
        )
    }
}

private fun DrawScope.drawVolumetricCloudLayers(
    w: Float,
    h: Float,
    coverage: Float,
    density: Float,
    cloudWave: Float,
    windSpeed: Float,
    isStorm: Boolean,
    lightningFlash: Float
) {
    if (coverage < 0.1f) return
    val baseCloudColor = when {
        lightningFlash > 0.2f -> Color(0xFFE2E8F0)
        isStorm -> Color(0xFF1E293B)
        else -> Color(0xFF475569)
    }

    val numClouds = (coverage * 8).toInt().coerceAtLeast(2)
    for (i in 0 until numClouds) {
        val offsetFactor = sin(cloudWave + i * 1.2f)
        val cx = (w * (i + 0.5f) / numClouds + offsetFactor * 30f) % w
        val cy = h * 0.14f + (i % 3) * 22f
        val radiusX = (w * 0.22f + i * 15f) * coverage
        val radiusY = 32f * density

        drawOval(
            color = baseCloudColor.copy(alpha = 0.45f * density),
            topLeft = Offset(cx - radiusX, cy - radiusY),
            size = Size(radiusX * 2f, radiusY * 2f)
        )
    }
}

private fun DrawScope.drawProceduralLightning(
    w: Float,
    h: Float,
    flashIntensity: Float,
    seed: Int
) {
    val rng = Random(seed)
    val startX = w * (0.3f + rng.nextFloat() * 0.4f)
    var curX = startX
    var curY = h * 0.08f
    val targetY = h * 0.58f

    val boltColor = Color(0xFFF8FAFC).copy(alpha = flashIntensity)
    val glowColor = Color(0xFF60A5FA).copy(alpha = flashIntensity * 0.6f)

    val path = Path()
    path.moveTo(curX, curY)

    val steps = 14
    val stepY = (targetY - curY) / steps

    for (s in 0 until steps) {
        curX += (rng.nextFloat() - 0.5f) * 45f
        curY += stepY
        path.lineTo(curX, curY)
    }

    // Outer glow
    drawPath(
        path = path,
        color = glowColor,
        style = Stroke(width = 7f, cap = StrokeCap.Round)
    )
    // Core bolt
    drawPath(
        path = path,
        color = boltColor,
        style = Stroke(width = 2.5f, cap = StrokeCap.Round)
    )
}

private fun DrawScope.drawRainStreaks(
    w: Float,
    h: Float,
    particles: List<FloatArray>,
    intensity: Float,
    dropSpeed: Float,
    windAngle: Float,
    windSpeed: Float,
    time: Float
) {
    val rad = windAngle * PI.toFloat() / 180f
    val windDX = cos(rad) * (windSpeed / 12f)
    val count = (particles.size * intensity).toInt().coerceAtLeast(10)
    val rainColor = Color(0xFF93C5FD).copy(alpha = 0.65f)

    for (i in 0 until count) {
        val p = particles[i]
        val speed = p[3] * (dropSpeed / 16f)
        val y = ((p[1] + time * speed) % 1.0f) * h
        val x = ((p[0] + (y / h) * windDX * 0.05f) % 1.0f) * w
        val len = 12f * p[2]

        drawLine(
            color = rainColor,
            start = Offset(x, y),
            end = Offset(x + windDX * 1.5f, y + len),
            strokeWidth = 1.2f,
            cap = StrokeCap.Round
        )

        // Ground splash ring
        if (y > h * 0.56f && y < h * 0.60f && (i % 3 == 0)) {
            drawCircle(
                color = Color(0xFFBFDBFE).copy(alpha = 0.5f),
                radius = 3.5f,
                center = Offset(x, h * 0.58f),
                style = Stroke(width = 1f)
            )
        }
    }
}

private fun DrawScope.drawSnowFlurries(
    w: Float,
    h: Float,
    particles: List<FloatArray>,
    intensity: Float,
    flutter: Float,
    windSpeed: Float,
    time: Float
) {
    val count = (particles.size * intensity).toInt().coerceAtLeast(10)
    val flakeColor = Color(0xFFF8FAFC).copy(alpha = 0.85f)

    for (i in 0 until count) {
        val p = particles[i]
        val phase = p[2] + time * 3f
        val flutterX = sin(phase) * flutter * 8f
        val y = ((p[1] + time * 0.08f) % 1.0f) * h
        val x = ((p[0] + flutterX / w + (windSpeed / 100f) * (y / h)) % 1.0f) * w

        drawCircle(
            color = flakeColor,
            radius = p[3],
            center = Offset(x, y)
        )
    }
}

private fun DrawScope.drawFogVolumeOverlay(
    w: Float,
    h: Float,
    density: Float,
    mist: Float,
    lightningFlash: Float
) {
    val fogColor = if (lightningFlash > 0.2f) Color(0xFFE2E8F0) else Color(0xFF64748B)
    drawRect(
        brush = Brush.verticalGradient(
            colors = listOf(
                fogColor.copy(alpha = 0.05f * density),
                fogColor.copy(alpha = 0.55f * density + 0.25f * mist)
            ),
            startY = h * 0.40f,
            endY = h
        ),
        topLeft = Offset(0f, h * 0.40f),
        size = Size(w, h * 0.60f)
    )
}

// ---------------------------------------------------------------------
// TAB CONTENTS
// ---------------------------------------------------------------------

@Composable
private fun PrecipitationTab(
    rainIntensity: Float,
    onRainIntensityChange: (Float) -> Unit,
    dropSpeed: Float,
    onDropSpeedChange: (Float) -> Unit,
    dropletSize: Float,
    onDropletSizeChange: (Float) -> Unit,
    splashDensity: Float,
    onSplashDensityChange: (Float) -> Unit,
    puddleAccumulation: Float,
    onPuddleAccumulationChange: (Float) -> Unit
) {
    Card(
        colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
        shape = RoundedCornerShape(12.dp),
        modifier = Modifier.padding(bottom = 12.dp)
    ) {
        Column(modifier = Modifier.padding(14.dp)) {
            Text(
                text = "Precipitation Dynamics",
                style = MaterialTheme.typography.titleMedium,
                color = Color(0xFF38BDF8),
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(10.dp))

            SliderRow(
                label = "Rain Intensity",
                value = rainIntensity,
                onValueChange = onRainIntensityChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(rainIntensity * 100).toInt()}%"
            )
            SliderRow(
                label = "Terminal Drop Velocity",
                value = dropSpeed,
                onValueChange = onDropSpeedChange,
                valueRange = 8.0f..32.0f,
                displayValue = "${dropSpeed.toInt()} m/s"
            )
            SliderRow(
                label = "Droplet Size (Volume)",
                value = dropletSize,
                onValueChange = onDropletSizeChange,
                valueRange = 0.5f..3.0f,
                displayValue = "${(dropletSize * 10).toInt() / 10f} mm"
            )
            SliderRow(
                label = "Ground Splash Impact",
                value = splashDensity,
                onValueChange = onSplashDensityChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(splashDensity * 100).toInt()}%"
            )
            SliderRow(
                label = "Puddle Water Accumulation",
                value = puddleAccumulation,
                onValueChange = onPuddleAccumulationChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(puddleAccumulation * 100).toInt()}%"
            )
        }
    }
}

@Composable
private fun SnowFrostTab(
    snowIntensity: Float,
    onSnowIntensityChange: (Float) -> Unit,
    flakeFlutter: Float,
    onFlakeFlutterChange: (Float) -> Unit,
    frostGlaze: Float,
    onFrostGlazeChange: (Float) -> Unit,
    groundSnowCover: Float,
    onGroundSnowCoverChange: (Float) -> Unit
) {
    Card(
        colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
        shape = RoundedCornerShape(12.dp),
        modifier = Modifier.padding(bottom = 12.dp)
    ) {
        Column(modifier = Modifier.padding(14.dp)) {
            Text(
                text = "Snow & Cryosphere",
                style = MaterialTheme.typography.titleMedium,
                color = Color(0xFF93C5FD),
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(10.dp))

            SliderRow(
                label = "Snowfall Precipitation",
                value = snowIntensity,
                onValueChange = onSnowIntensityChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(snowIntensity * 100).toInt()}%"
            )
            SliderRow(
                label = "Flake Flutter Turbulence",
                value = flakeFlutter,
                onValueChange = onFlakeFlutterChange,
                valueRange = 0.1f..1.0f,
                displayValue = "${(flakeFlutter * 100).toInt()}%"
            )
            SliderRow(
                label = "Surface Frost Glaze",
                value = frostGlaze,
                onValueChange = onFrostGlazeChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(frostGlaze * 100).toInt()}%"
            )
            SliderRow(
                label = "Ground Snowpack Accumulation",
                value = groundSnowCover,
                onValueChange = onGroundSnowCoverChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(groundSnowCover * 100).toInt()}%"
            )
        }
    }
}

@Composable
private fun CloudsFogTab(
    cloudCoverage: Float,
    onCloudCoverageChange: (Float) -> Unit,
    cloudAltitude: Float,
    onCloudAltitudeChange: (Float) -> Unit,
    cloudDensity: Float,
    onCloudDensityChange: (Float) -> Unit,
    fogDensity: Float,
    onFogDensityChange: (Float) -> Unit,
    groundMist: Float,
    onGroundMistChange: (Float) -> Unit
) {
    Card(
        colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
        shape = RoundedCornerShape(12.dp),
        modifier = Modifier.padding(bottom = 12.dp)
    ) {
        Column(modifier = Modifier.padding(14.dp)) {
            Text(
                text = "Volumetric Clouds & Fog",
                style = MaterialTheme.typography.titleMedium,
                color = Color(0xFFA78BFA),
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(10.dp))

            SliderRow(
                label = "Cloud Fractional Coverage",
                value = cloudCoverage,
                onValueChange = onCloudCoverageChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(cloudCoverage * 100).toInt()}%"
            )
            SliderRow(
                label = "Cloud Base Altitude",
                value = cloudAltitude,
                onValueChange = onCloudAltitudeChange,
                valueRange = 800f..4000f,
                displayValue = "${cloudAltitude.toInt()} m"
            )
            SliderRow(
                label = "Optical Cloud Density",
                value = cloudDensity,
                onValueChange = onCloudDensityChange,
                valueRange = 0.1f..1.0f,
                displayValue = "${(cloudDensity * 100).toInt()}%"
            )
            SliderRow(
                label = "Froxel Fog Density",
                value = fogDensity,
                onValueChange = onFogDensityChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(fogDensity * 100).toInt()}%"
            )
            SliderRow(
                label = "Ground Mist Layer",
                value = groundMist,
                onValueChange = onGroundMistChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(groundMist * 100).toInt()}%"
            )
        }
    }
}

@Composable
private fun WindStormsTab(
    windSpeed: Float,
    onWindSpeedChange: (Float) -> Unit,
    windAzimuth: Float,
    onWindAzimuthChange: (Float) -> Unit,
    lightningFrequency: Float,
    onLightningFrequencyChange: (Float) -> Unit,
    isStormActive: Boolean,
    onStormActiveChange: (Boolean) -> Unit,
    onTriggerStrike: () -> Unit
) {
    Card(
        colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
        shape = RoundedCornerShape(12.dp),
        modifier = Modifier.padding(bottom = 12.dp)
    ) {
        Column(modifier = Modifier.padding(14.dp)) {
            Text(
                text = "Wind Field & Severe Storms",
                style = MaterialTheme.typography.titleMedium,
                color = Color(0xFFFBBF24),
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(10.dp))

            SliderRow(
                label = "Wind Velocity",
                value = windSpeed,
                onValueChange = onWindSpeedChange,
                valueRange = 0.0f..90.0f,
                displayValue = "${windSpeed.toInt()} km/h"
            )
            SliderRow(
                label = "Wind Direction Angle",
                value = windAzimuth,
                onValueChange = onWindAzimuthChange,
                valueRange = 0.0f..360.0f,
                displayValue = "${windAzimuth.toInt()}°"
            )
            SliderRow(
                label = "Lightning Discharge Rate",
                value = lightningFrequency,
                onValueChange = onLightningFrequencyChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(lightningFrequency * 100).toInt()}%"
            )

            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(vertical = 8.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = "Severe Thunderstorm Mode",
                    style = MaterialTheme.typography.bodyMedium,
                    color = Color.White
                )
                Switch(
                    checked = isStormActive,
                    onCheckedChange = onStormActiveChange,
                    colors = SwitchDefaults.colors(checkedThumbColor = Color(0xFFFBBF24))
                )
            }

            Surface(
                shape = RoundedCornerShape(8.dp),
                color = Color(0xFF1E293B),
                modifier = Modifier
                    .fillMaxWidth()
                    .clickable { onTriggerStrike() }
                    .padding(top = 6.dp)
            ) {
                Row(
                    modifier = Modifier.padding(vertical = 10.dp, horizontal = 14.dp),
                    horizontalArrangement = Arrangement.Center,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Icon(
                        imageVector = Icons.Default.FlashOn,
                        contentDescription = "Trigger Strike",
                        tint = Color(0xFFFBBF24),
                        modifier = Modifier.size(16.dp)
                    )
                    Spacer(modifier = Modifier.width(8.dp))
                    Text(
                        text = "Trigger Lightning Bolt Strike",
                        fontSize = 12.sp,
                        fontWeight = FontWeight.Bold,
                        color = Color.White
                    )
                }
            }
        }
    }
}

@Composable
private fun NativeEngineTab() {
    Card(
        colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
        shape = RoundedCornerShape(12.dp),
        modifier = Modifier.padding(bottom = 12.dp)
    ) {
        Column(modifier = Modifier.padding(14.dp)) {
            Text(
                text = "Vulkan 1.3 Weather Compute Modules",
                style = MaterialTheme.typography.titleMedium,
                color = Color(0xFF4ADE80),
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(10.dp))

            val modules = listOf(
                Triple("WeatherSystem.cpp", "native_assets/src/weather/WeatherSystem.cpp", "Precipitation, wetness integration & world state sync"),
                Triple("rain_particle.comp", "native_assets/src/weather/rain/shaders/rain_particle.comp", "GPU compute 65,536 rain particles with wind deflection"),
                Triple("cloud_raymarch.comp", "native_assets/src/weather/clouds/shaders/cloud_raymarch.comp", "Volumetric Worley noise raymarching with dual-phase powder"),
                Triple("fog_froxel.comp", "native_assets/src/weather/fog/shaders/fog_froxel.comp", "160x90x64 3D froxel volume grid with Mie inscattering"),
                Triple("bolt_path.comp", "native_assets/src/weather/lightning/shaders/bolt_path.comp", "Recursive branch generation for procedural lightning arcs"),
                Triple("puddle_mask.comp", "native_assets/src/weather/puddles/shaders/puddle_mask.comp", "Dynamic ground wetness & puddle depression masking"),
                Triple("weather_seed.bin", "native_assets/src/weather/textures/seeds/weather_seed.bin", "256-entry deterministic float noise seed [Status: Active]"),
                Triple("test_weather.cpp", "native_assets/tests/weather/test_rain.cpp", "GoogleTest suites: Rain, Snow, Fog, Clouds, Storms (PASS)")
            )

            modules.forEach { (name, path, desc) ->
                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = Color(0xFF0F172A),
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(vertical = 4.dp)
                ) {
                    Column(modifier = Modifier.padding(10.dp)) {
                        Row(
                            modifier = Modifier.fillMaxWidth(),
                            horizontalArrangement = Arrangement.SpaceBetween,
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Text(
                                text = name,
                                fontSize = 12.sp,
                                fontWeight = FontWeight.Bold,
                                color = Color(0xFF38BDF8)
                            )
                            Surface(
                                shape = RoundedCornerShape(4.dp),
                                color = Color(0xFF166534).copy(alpha = 0.5f)
                            ) {
                                Text(
                                    text = "ACTIVE",
                                    fontSize = 9.sp,
                                    fontWeight = FontWeight.Bold,
                                    color = Color(0xFF4ADE80),
                                    modifier = Modifier.padding(horizontal = 4.dp, vertical = 2.dp)
                                )
                            }
                        }
                        Text(
                            text = path,
                            fontSize = 10.sp,
                            fontFamily = FontFamily.Monospace,
                            color = Color(0xFF64748B)
                        )
                        Spacer(modifier = Modifier.height(2.dp))
                        Text(
                            text = desc,
                            fontSize = 11.sp,
                            color = Color(0xFFCBD5E1)
                        )
                    }
                }
            }
        }
    }
}

@Composable
private fun SliderRow(
    label: String,
    value: Float,
    onValueChange: (Float) -> Unit,
    valueRange: ClosedFloatingPointRange<Float>,
    displayValue: String
) {
    Column(modifier = Modifier.padding(vertical = 4.dp)) {
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween
        ) {
            Text(text = label, style = MaterialTheme.typography.bodySmall, color = Color(0xFFCBD5E1))
            Text(text = displayValue, style = MaterialTheme.typography.bodySmall, fontWeight = FontWeight.Bold, color = Color(0xFF38BDF8))
        }
        Slider(
            value = value,
            onValueChange = onValueChange,
            valueRange = valueRange,
            colors = SliderDefaults.colors(
                thumbColor = Color(0xFF38BDF8),
                activeTrackColor = Color(0xFF0284C7),
                inactiveTrackColor = Color(0xFF1E293B)
            )
        )
    }
}
