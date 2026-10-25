package com.example.sphere

import androidx.compose.animation.core.LinearEasing
import androidx.compose.animation.core.RepeatMode
import androidx.compose.animation.core.animateFloat
import androidx.compose.animation.core.infiniteRepeatable
import androidx.compose.animation.core.rememberInfiniteTransition
import androidx.compose.animation.core.tween
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.NightsStay
import androidx.compose.material.icons.filled.Pause
import androidx.compose.material.icons.filled.PlayArrow
import androidx.compose.material.icons.filled.Refresh
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
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.delay
import kotlin.math.PI
import kotlin.math.abs
import kotlin.math.sin

@Composable
fun SkyStudioScreen(
    modifier: Modifier = Modifier
) {
    // ----------------- TIME & CELESTIAL STATE -----------------
    var timeOfDayHours by remember { mutableFloatStateOf(14.5f) }
    var isCyclePlaying by remember { mutableStateOf(false) }

    // Atmosphere parameters
    var turbidity by remember { mutableFloatStateOf(2.5f) }
    var rayleighScale by remember { mutableFloatStateOf(1.0f) }
    var mieScale by remember { mutableFloatStateOf(1.0f) }
    var mieAsymmetryG by remember { mutableFloatStateOf(0.76f) }
    var groundAlbedo by remember { mutableFloatStateOf(0.12f) }
    var ozoneAbsorption by remember { mutableStateOf(true) }

    // Solar disk parameters
    var sunElevation by remember { mutableFloatStateOf(38.0f) }
    var sunAzimuth by remember { mutableFloatStateOf(152.0f) }
    var sunIntensityLux by remember { mutableFloatStateOf(25.0f) }
    var sunAngularRadius by remember { mutableFloatStateOf(0.045f) }
    var limbDarkening by remember { mutableFloatStateOf(0.60f) }
    var coronaFlareIntensity by remember { mutableFloatStateOf(0.85f) }

    // Starfield parameters
    var starCount by remember { mutableIntStateOf(1200) }
    var twinkleSpeed by remember { mutableFloatStateOf(1.2f) }
    var magnitudeCutoff by remember { mutableFloatStateOf(6.2f) }
    var milkyWayIntensity by remember { mutableFloatStateOf(0.40f) }

    // Lunar parameters
    var moonPhase by remember { mutableFloatStateOf(0.50f) }
    var moonElevation by remember { mutableFloatStateOf(35.0f) }
    var moonAzimuth by remember { mutableFloatStateOf(35.0f) }
    var earthshine by remember { mutableFloatStateOf(0.025f) }
    var moonAlbedo by remember { mutableFloatStateOf(0.12f) }

    // Tonemapping & LUT
    var tonemapMode by remember { mutableStateOf(TonemapMode.ACES) }
    var exposureEv by remember { mutableFloatStateOf(0.0f) }
    var colorTempKelvin by remember { mutableFloatStateOf(5800f) }
    var contrast by remember { mutableFloatStateOf(1.05f) }

    // Camera view direction
    var camPitch by remember { mutableFloatStateOf(15.0f) }
    var camYaw by remember { mutableFloatStateOf(150.0f) }

    // Tab state: 0: Atmosphere, 1: Sun & Corona, 2: Stars, 3: Moon, 4: Tonemap, 5: Native Core
    var activeTab by remember { mutableIntStateOf(0) }

    // Auto diurnal cycle progression
    LaunchedEffect(isCyclePlaying) {
        while (isCyclePlaying) {
            delay(50)
            timeOfDayHours = (timeOfDayHours + 0.05f) % 24.0f
            val sunAngle = (timeOfDayHours - 6.0f) / 12.0f * PI.toFloat()
            sunElevation = sin(sunAngle) * 65.0f
            sunAzimuth = (timeOfDayHours / 24.0f * 360.0f + 90.0f) % 360.0f
            moonElevation = -sunElevation
            moonAzimuth = (sunAzimuth + 180.0f) % 360.0f
        }
    }

    // Twinkling animation loop
    val infiniteTransition = rememberInfiniteTransition(label = "sky_twinkle")
    val twinkleCycle by infiniteTransition.animateFloat(
        initialValue = 0f,
        targetValue = 6.28318f,
        animationSpec = infiniteRepeatable(
            animation = tween(durationMillis = 2400, easing = LinearEasing),
            repeatMode = RepeatMode.Restart
        ),
        label = "twinkle_phase"
    )

    Column(
        modifier = modifier
            .fillMaxSize()
            .background(Color(0xFF090B10))
            .testTag("sky_studio_screen")
    ) {
        // -------------------------------------------------------------
        // 1. SKY DOME PANORAMIC VIEWPORT
        // -------------------------------------------------------------
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .weight(1.08f)
                .background(Color(0xFF05070B))
                .testTag("sky_dome_viewport")
        ) {
            SkyViewportCanvas(
                sunElevation = sunElevation,
                sunAzimuth = sunAzimuth,
                sunAngularRadius = sunAngularRadius,
                coronaFlareIntensity = coronaFlareIntensity,
                moonPhase = moonPhase,
                moonElevation = moonElevation,
                moonAzimuth = moonAzimuth,
                earthshine = earthshine,
                starCount = starCount,
                twinkleSpeed = twinkleSpeed,
                twinkleCycle = twinkleCycle,
                camYaw = camYaw,
                camPitch = camPitch,
                onDrag = { dx, dy ->
                    camYaw = (camYaw - dx * 0.35f + 360f) % 360f
                    camPitch = (camPitch + dy * 0.25f).coerceIn(-15f, 85f)
                }
            )

            // Viewport Top HUD: Time & Orientation Badges
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(10.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = Color(0xCC0F172A),
                    border = CardDefaults.outlinedCardBorder()
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Icon(
                            imageVector = if (sunElevation > 0f) Icons.Default.WbSunny else Icons.Default.NightsStay,
                            contentDescription = null,
                            tint = if (sunElevation > 0f) Color(0xFFF59E0B) else Color(0xFF818CF8),
                            modifier = Modifier.size(14.dp)
                        )
                        Spacer(modifier = Modifier.width(6.dp))
                        val hrs = timeOfDayHours.toInt()
                        val mins = ((timeOfDayHours - hrs) * 60).toInt()
                        Text(
                            text = "${hrs.toString().padStart(2, '0')}:${mins.toString().padStart(2, '0')} UTC",
                            style = MaterialTheme.typography.labelMedium.copy(fontWeight = FontWeight.Bold),
                            color = Color.White,
                            fontFamily = FontFamily.Monospace
                        )
                    }
                }

                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = Color(0xCC0F172A)
                ) {
                    Text(
                        text = "Sun El: ${(sunElevation * 10).toInt() / 10f}° | Cam Yaw: ${camYaw.toInt()}°",
                        style = MaterialTheme.typography.labelSmall,
                        color = Color(0xFF94A3B8),
                        modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                        fontFamily = FontFamily.Monospace
                    )
                }

                Row(verticalAlignment = Alignment.CenterVertically) {
                    IconButton(
                        onClick = { isCyclePlaying = !isCyclePlaying },
                        modifier = Modifier
                            .size(32.dp)
                            .background(
                                if (isCyclePlaying) Color(0xFF0F766E).copy(alpha = 0.85f) else Color(0xFF1E293B).copy(alpha = 0.85f),
                                CircleShape
                            )
                            .testTag("toggle_diurnal_cycle_button")
                    ) {
                        Icon(
                            imageVector = if (isCyclePlaying) Icons.Default.Pause else Icons.Default.PlayArrow,
                            contentDescription = "Play/Pause diurnal cycle",
                            tint = Color.White,
                            modifier = Modifier.size(16.dp)
                        )
                    }

                    Spacer(modifier = Modifier.width(6.dp))

                    IconButton(
                        onClick = {
                            camPitch = 15.0f
                            camYaw = 150.0f
                        },
                        modifier = Modifier
                            .size(32.dp)
                            .background(Color(0xFF1E293B).copy(alpha = 0.85f), CircleShape)
                            .testTag("reset_sky_camera_button")
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

            // Bottom Overlays inside Viewport: Floating Presets Bar
            Row(
                modifier = Modifier
                    .align(Alignment.BottomCenter)
                    .fillMaxWidth()
                    .padding(horizontal = 8.dp, vertical = 6.dp)
                    .horizontalScroll(rememberScrollState()),
                horizontalArrangement = Arrangement.spacedBy(6.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                skyPresetsList.forEach { preset ->
                    Surface(
                        shape = RoundedCornerShape(8.dp),
                        color = if (abs(timeOfDayHours - preset.timeHours) < 1.0f)
                            Color(0xFF0284C7).copy(alpha = 0.75f)
                        else
                            Color(0xCC10141E),
                        border = CardDefaults.outlinedCardBorder().copy(
                            brush = Brush.linearGradient(listOf(Color(0xFF1E293B), Color(0xFF334155)))
                        ),
                        modifier = Modifier.clickable {
                            timeOfDayHours = preset.timeHours
                            sunElevation = preset.elevation
                            sunAzimuth = preset.azimuth
                            turbidity = preset.turbidity
                            moonElevation = -preset.elevation
                            moonAzimuth = (preset.azimuth + 180f) % 360f
                        }
                    ) {
                        Row(
                            modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Icon(
                                imageVector = preset.icon,
                                contentDescription = null,
                                tint = if (abs(timeOfDayHours - preset.timeHours) < 1.0f) Color.White else Color(0xFF94A3B8),
                                modifier = Modifier.size(12.dp)
                            )
                            Spacer(modifier = Modifier.width(4.dp))
                            Text(
                                text = preset.name,
                                style = MaterialTheme.typography.labelSmall,
                                color = if (abs(timeOfDayHours - preset.timeHours) < 1.0f) Color.White else Color(0xFFCBD5E1),
                                fontSize = 10.sp,
                                fontWeight = FontWeight.SemiBold
                            )
                        }
                    }
                }
            }
        }

        HorizontalDivider(color = Color(0xFF1E293B), thickness = 1.dp)

        // -------------------------------------------------------------
        // 2. BOTTOM CONTROLS PANEL
        // -------------------------------------------------------------
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
                    modifier = Modifier.fillMaxWidth().testTag("sky_tabs_row")
                ) {
                    val tabTitles = listOf("Atmosphere", "Sun & Flare", "Stars & Cosmos", "Moon & Phases", "Tonemap & LUT", "Native C++ Core")
                    tabTitles.forEachIndexed { index, title ->
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
                        0 -> {
                            Card(
                                colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
                                shape = RoundedCornerShape(12.dp),
                                modifier = Modifier.padding(bottom = 12.dp)
                            ) {
                                Column(modifier = Modifier.padding(14.dp)) {
                                    Row(
                                        modifier = Modifier.fillMaxWidth(),
                                        horizontalArrangement = Arrangement.SpaceBetween,
                                        verticalAlignment = Alignment.CenterVertically
                                    ) {
                                        Text(
                                            text = "24-Hour Diurnal Orbit",
                                            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                                            color = Color.White
                                        )
                                        Text(
                                            text = "${(timeOfDayHours * 10).toInt() / 10f} h",
                                            style = MaterialTheme.typography.labelSmall,
                                            color = Color(0xFF38BDF8),
                                            fontFamily = FontFamily.Monospace
                                        )
                                    }
                                    Slider(
                                        value = timeOfDayHours,
                                        onValueChange = {
                                            timeOfDayHours = it
                                            val sunAngle = (it - 6.0f) / 12.0f * PI.toFloat()
                                            sunElevation = sin(sunAngle) * 65.0f
                                            sunAzimuth = (it / 24.0f * 360.0f + 90.0f) % 360.0f
                                            moonElevation = -sunElevation
                                            moonAzimuth = (sunAzimuth + 180.0f) % 360.0f
                                        },
                                        valueRange = 0f..24f,
                                        colors = SliderDefaults.colors(
                                            thumbColor = Color(0xFF38BDF8),
                                            activeTrackColor = Color(0xFF0284C7),
                                            inactiveTrackColor = Color(0xFF1E293B)
                                        ),
                                        modifier = Modifier.fillMaxWidth().testTag("time_of_day_slider")
                                    )
                                }
                            }

                            AtmosphereControlsTab(
                                turbidity = turbidity,
                                onTurbidityChange = { turbidity = it },
                                rayleighScale = rayleighScale,
                                onRayleighChange = { rayleighScale = it },
                                mieScale = mieScale,
                                onMieChange = { mieScale = it },
                                mieAsymmetryG = mieAsymmetryG,
                                onMieGChange = { mieAsymmetryG = it },
                                groundAlbedo = groundAlbedo,
                                onAlbedoChange = { groundAlbedo = it },
                                ozoneAbsorption = ozoneAbsorption,
                                onOzoneToggle = { ozoneAbsorption = it }
                            )
                        }
                        1 -> SunAndFlareTab(
                            sunElevation = sunElevation,
                            onElevationChange = { sunElevation = it },
                            sunAzimuth = sunAzimuth,
                            onAzimuthChange = { sunAzimuth = it },
                            sunIntensityLux = sunIntensityLux,
                            onIntensityChange = { sunIntensityLux = it },
                            angularRadius = sunAngularRadius,
                            onAngularRadiusChange = { sunAngularRadius = it },
                            limbDarkening = limbDarkening,
                            onLimbDarkeningChange = { limbDarkening = it },
                            coronaFlare = coronaFlareIntensity,
                            onCoronaFlareChange = { coronaFlareIntensity = it }
                        )
                        2 -> StarsAndCosmosTab(
                            starCount = starCount,
                            onStarCountChange = { starCount = it },
                            twinkleSpeed = twinkleSpeed,
                            onTwinkleSpeedChange = { twinkleSpeed = it },
                            magnitudeCutoff = magnitudeCutoff,
                            onMagnitudeCutoffChange = { magnitudeCutoff = it },
                            milkyWayIntensity = milkyWayIntensity,
                            onMilkyWayChange = { milkyWayIntensity = it }
                        )
                        3 -> MoonAndPhasesTab(
                            moonPhase = moonPhase,
                            onMoonPhaseChange = { moonPhase = it },
                            moonElevation = moonElevation,
                            onMoonElevationChange = { moonElevation = it },
                            moonAzimuth = moonAzimuth,
                            onMoonAzimuthChange = { moonAzimuth = it },
                            earthshine = earthshine,
                            onEarthshineChange = { earthshine = it },
                            moonAlbedo = moonAlbedo,
                            onMoonAlbedoChange = { moonAlbedo = it }
                        )
                        4 -> TonemapAndLUTTab(
                            selectedMode = tonemapMode,
                            onModeSelect = { tonemapMode = it },
                            exposureEv = exposureEv,
                            onExposureChange = { exposureEv = it },
                            colorTempKelvin = colorTempKelvin,
                            onColorTempChange = { colorTempKelvin = it },
                            contrast = contrast,
                            onContrastChange = { contrast = it }
                        )
                        5 -> SkyNativeCodeViewerTab()
                    }
                }
            }
        }
    }
}
