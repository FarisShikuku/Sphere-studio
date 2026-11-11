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
import androidx.activity.compose.BackHandler
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.Layers
import androidx.compose.material.icons.filled.NightsStay
import androidx.compose.material.icons.filled.Pause
import androidx.compose.material.icons.filled.PlayArrow
import androidx.compose.material.icons.filled.Radar
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material.icons.filled.RestartAlt
import androidx.compose.material.icons.filled.WbSunny
import com.example.sphere.model.CloudDetectionRepository
import com.example.sphere.model.CloudGenus
import com.example.sphere.model.DetectedCloudFormation
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.ScrollableTabRow
import androidx.compose.material3.Slider
import androidx.compose.material3.SliderDefaults
import androidx.compose.material3.Surface
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarDefaults
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

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SkyStudioScreen(
    onNavigateBack: () -> Unit = {},
    modifier: Modifier = Modifier
) {
    BackHandler { onNavigateBack() }

    // ----------------- TIME & CELESTIAL STATE -----------------
    var timeOfDayHours by remember { mutableFloatStateOf(14.5f) }
    var isCyclePlaying by remember { mutableStateOf(false) }

    // Shared Vulkan Scene Stage state (Default NONE for clean panoramic sky dome)
    var surfaceMode by remember { mutableStateOf(SurfaceSceneMode.NONE) }
    var stageWidth by remember { mutableFloatStateOf(3.5f) }
    var stageLength by remember { mutableFloatStateOf(3.5f) }
    var stageHeight by remember { mutableFloatStateOf(0.25f) }
    var showStageGrid by remember { mutableStateOf(false) }
    var showCalibrationItem by remember { mutableStateOf(false) }

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

    // Cloud Detection state
    var isCloudDetectionEnabled by remember { mutableStateOf(false) }
    var selectedFilterGenus by remember { mutableStateOf<CloudGenus?>(null) }
    var selectedFormationId by remember { mutableStateOf<String?>(null) }
    val detectedFormations = remember { CloudDetectionRepository.defaultFormations }

    // Meteoros Volumetric Cloud parameters (AmanSachan1/Meteoros Decima Raymarcher)
    var cloudType by remember { mutableIntStateOf(2) } // 0: Stratus, 1: Stratocumulus, 2: Cumulus, 3: Cumulonimbus
    var cloudCoverage by remember { mutableFloatStateOf(0.65f) }
    var cloudDensity by remember { mutableFloatStateOf(1.0f) }
    var multiScatteringFactor by remember { mutableFloatStateOf(0.50f) }
    var silverLiningG by remember { mutableFloatStateOf(0.82f) }
    var powderEffect by remember { mutableFloatStateOf(0.35f) }
    var sunAbsorption by remember { mutableFloatStateOf(0.65f) }
    var detailErosion by remember { mutableFloatStateOf(0.35f) }
    var enableGodRays by remember { mutableStateOf(true) }
    var godRaysIntensity by remember { mutableFloatStateOf(0.65f) }
    var enableCirrus by remember { mutableStateOf(true) }
    var cirrusCoverage by remember { mutableFloatStateOf(0.35f) }
    var windSpeed by remember { mutableFloatStateOf(18.0f) }
    var windAzimuth by remember { mutableFloatStateOf(240.0f) }
    var enableBayerJitter by remember { mutableStateOf(true) }

    // Tab state: 0: Detected Clouds, 1: Meteoros Volumetrics, 2: Vulkan Stage, 3: Atmosphere, 4: Sun & Flare, 5: Stars, 6: Moon, 7: Tonemap, 8: Native Core
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

    // Volumetric cloudscape rendering via ProceduralCloudSynthesizer (Meteoros raymarcher)
    val cloudImageBitmap = remember(
        camYaw.toInt(),
        camPitch.toInt(),
        cloudType,
        cloudCoverage,
        cloudDensity,
        multiScatteringFactor,
        silverLiningG,
        powderEffect,
        sunAbsorption,
        detailErosion,
        enableBayerJitter,
        sunAzimuth.toInt(),
        sunElevation.toInt(),
        (twinkleCycle * 1.5f).toInt()
    ) {
        val bmp = ProceduralCloudSynthesizer.renderPerspectiveSky(
            width = 240,
            height = 140,
            camYaw = camYaw,
            camPitch = camPitch,
            timeSeconds = twinkleCycle * 4.0f,
            windSpeed = windSpeed,
            windAzimuth = windAzimuth,
            cloudType = cloudType,
            coverage = cloudCoverage,
            density = cloudDensity,
            silverLiningG = silverLiningG,
            powderEffect = powderEffect,
            sunAbsorption = sunAbsorption,
            detailErosion = detailErosion,
            cloudBaseAltitude = 2.0f,
            isStorm = (cloudType == 3 && cloudCoverage > 0.65f),
            horizonYFrac = 0.52f,
            multiScatteringFactor = multiScatteringFactor,
            enableBayerJitter = enableBayerJitter,
            sunAzimuth = sunAzimuth,
            sunElevation = sunElevation
        )
        bmp.asImageBitmap()
    }

    Scaffold(
        modifier = modifier.fillMaxSize().testTag("sky_studio_screen"),
        topBar = {
            TopAppBar(
                title = {
                    Column {
                        Text(
                            "Atmospheric Sky Studio",
                            style = MaterialTheme.typography.titleMedium.copy(fontWeight = FontWeight.Bold)
                        )
                        Text(
                            "Preetham & Hosek-Wilkie Model • Vulkan 3D Scene",
                            style = MaterialTheme.typography.bodySmall.copy(
                                color = MaterialTheme.colorScheme.onSurfaceVariant,
                                fontSize = 11.sp
                            )
                        )
                    }
                },
                navigationIcon = {
                    IconButton(
                        onClick = onNavigateBack,
                        modifier = Modifier.testTag("sky_studio_back_button")
                    ) {
                        Icon(
                            Icons.AutoMirrored.Filled.ArrowBack,
                            contentDescription = "Back to Studio"
                        )
                    }
                },
                actions = {
                    // Quick Stage mode cycle button
                    IconButton(
                        onClick = {
                            val modes = SurfaceSceneMode.entries
                            val nextIdx = (modes.indexOf(surfaceMode) + 1) % modes.size
                            surfaceMode = modes[nextIdx]
                        },
                        modifier = Modifier.testTag("toggle_stage_mode_top_button")
                    ) {
                        Icon(
                            Icons.Default.Layers,
                            contentDescription = "Switch Vulkan Scene Stage",
                            tint = if (surfaceMode != SurfaceSceneMode.NONE) MaterialTheme.colorScheme.primary else Color.Gray
                        )
                    }
                    IconButton(
                        onClick = { isCyclePlaying = !isCyclePlaying },
                        modifier = Modifier.testTag("toggle_cycle_top_button")
                    ) {
                        Icon(
                            imageVector = if (isCyclePlaying) Icons.Default.Pause else Icons.Default.PlayArrow,
                            contentDescription = "Play/Pause diurnal cycle",
                            tint = if (isCyclePlaying) MaterialTheme.colorScheme.primary else Color.White
                        )
                    }
                    IconButton(
                        onClick = {
                            timeOfDayHours = 14.5f
                            sunElevation = 38.0f
                            sunAzimuth = 152.0f
                            camPitch = 15.0f
                            camYaw = 150.0f
                            surfaceMode = SurfaceSceneMode.NONE
                            stageWidth = 3.5f
                            stageLength = 3.5f
                            stageHeight = 0.25f
                            showStageGrid = true
                            showCalibrationItem = false
                            isCloudDetectionEnabled = false
                            selectedFormationId = null
                            selectedFilterGenus = null
                            cloudType = 2
                            cloudCoverage = 0.65f
                            cloudDensity = 1.0f
                            multiScatteringFactor = 0.50f
                            silverLiningG = 0.82f
                            powderEffect = 0.35f
                            sunAbsorption = 0.65f
                            detailErosion = 0.35f
                            enableGodRays = true
                            godRaysIntensity = 0.65f
                            enableCirrus = true
                            cirrusCoverage = 0.35f
                            windSpeed = 18.0f
                            windAzimuth = 240.0f
                            enableBayerJitter = true
                        },
                        modifier = Modifier.testTag("reset_sky_defaults_button")
                    ) {
                        Icon(Icons.Default.RestartAlt, contentDescription = "Reset Defaults")
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
                .background(Color(0xFF090B10))
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
                    },
                    cloudImage = cloudImageBitmap,
                    cloudCoverage = cloudCoverage,
                    cloudDensity = cloudDensity,
                    enableGodRays = enableGodRays,
                    godRaysIntensity = godRaysIntensity,
                    enableCirrus = enableCirrus,
                    cirrusCoverage = cirrusCoverage,
                    timeSeconds = twinkleCycle * 4.0f,
                    surfaceMode = surfaceMode,
                    stageWidth = stageWidth,
                    stageLength = stageLength,
                    stageHeight = stageHeight,
                    showStageGrid = showStageGrid,
                    showCalibrationItem = showCalibrationItem,
                    isDetectionEnabled = isCloudDetectionEnabled,
                    detectedFormations = detectedFormations,
                    selectedFormationId = selectedFormationId,
                    selectedFilterGenus = selectedFilterGenus,
                    onSelectFormation = { formation ->
                        selectedFormationId = formation.id
                        selectedFilterGenus = formation.genus
                        cloudType = when (formation.genus) {
                            CloudGenus.STRATUS -> 0
                            CloudGenus.STRATOCUMULUS -> 1
                            CloudGenus.CUMULUS -> 2
                            CloudGenus.CUMULONIMBUS, CloudGenus.NIMBOSTRATUS -> 3
                            CloudGenus.ALTOCUMULUS, CloudGenus.CIRROCUMULUS -> 4
                            CloudGenus.CIRRUS -> 5
                        }
                        cloudCoverage = (formation.opticalDepthTau / 45.0f).coerceIn(0.50f, 0.85f)
                        cloudDensity = 1.25f
                    }
                )

            // Viewport Top HUD: Time, Orientation & Cloud Scanner Badges
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

                // Cloud Detection Scanner Toggle Pill
                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = if (isCloudDetectionEnabled) Color(0xFF0284C7).copy(alpha = 0.85f) else Color(0xCC0F172A),
                    border = CardDefaults.outlinedCardBorder(),
                    modifier = Modifier
                        .clickable { isCloudDetectionEnabled = !isCloudDetectionEnabled }
                        .testTag("toggle_cloud_detection_hud")
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Icon(
                            imageVector = Icons.Default.Radar,
                            contentDescription = null,
                            tint = if (isCloudDetectionEnabled) Color.White else Color(0xFF64748B),
                            modifier = Modifier.size(13.dp)
                        )
                        Spacer(modifier = Modifier.width(4.dp))
                        Text(
                            text = if (isCloudDetectionEnabled) "SCANNER ON" else "SCANNER OFF",
                            style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold),
                            color = if (isCloudDetectionEnabled) Color.White else Color(0xFF94A3B8),
                            fontFamily = FontFamily.Monospace,
                            fontSize = 9.sp
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

            // Quick Cloud Species Selector Strip (pinned right above presets bar)
            Row(
                modifier = Modifier
                    .align(Alignment.BottomCenter)
                    .fillMaxWidth()
                    .padding(bottom = 38.dp, start = 8.dp, end = 8.dp)
                    .horizontalScroll(rememberScrollState()),
                horizontalArrangement = Arrangement.spacedBy(6.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                detectedFormations.forEach { formation ->
                    val isTargeted = formation.id == selectedFormationId
                    Surface(
                        shape = RoundedCornerShape(16.dp),
                        color = if (isTargeted) formation.genus.badgeColor.copy(alpha = 0.90f) else Color(0xDE0B111D),
                        border = CardDefaults.outlinedCardBorder().copy(
                            brush = Brush.linearGradient(
                                if (isTargeted) listOf(Color.White, formation.genus.badgeColor)
                                else listOf(Color(0xFF334155), Color(0xFF1E293B))
                            )
                        ),
                        modifier = Modifier
                            .clickable {
                                selectedFormationId = formation.id
                                selectedFilterGenus = formation.genus
                                camYaw = formation.azimuthDegrees
                                camPitch = formation.elevationDegrees.coerceIn(0f, 65f)
                                cloudType = when (formation.genus) {
                                    CloudGenus.STRATUS -> 0
                                    CloudGenus.STRATOCUMULUS -> 1
                                    CloudGenus.CUMULUS -> 2
                                    CloudGenus.CUMULONIMBUS, CloudGenus.NIMBOSTRATUS -> 3
                                    CloudGenus.ALTOCUMULUS, CloudGenus.CIRROCUMULUS -> 4
                                    CloudGenus.CIRRUS -> 5
                                }
                                cloudCoverage = (formation.opticalDepthTau / 45.0f).coerceIn(0.55f, 0.85f)
                                cloudDensity = 1.25f
                            }
                            .testTag("quick_cloud_chip_${formation.id}")
                    ) {
                        Row(
                            modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Text(
                                text = formation.genus.iconEmoji,
                                fontSize = 11.sp
                            )
                            Spacer(modifier = Modifier.width(4.dp))
                            Text(
                                text = "${formation.genus.displayName} (${formation.genus.symbolCode})",
                                fontSize = 10.sp,
                                fontWeight = if (isTargeted) FontWeight.Bold else FontWeight.SemiBold,
                                color = if (isTargeted) Color.White else Color(0xFFE2E8F0)
                            )
                        }
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
                    val tabTitles = listOf("Detected Clouds", "Meteoros Volumetrics", "Vulkan Stage", "Atmosphere", "Sun & Flare", "Stars & Cosmos", "Moon & Phases", "Tonemap & LUT", "Native C++ Core")
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
                        0 -> CloudDetectionTab(
                            isDetectionEnabled = isCloudDetectionEnabled,
                            onToggleDetection = { isCloudDetectionEnabled = it },
                            selectedFilterGenus = selectedFilterGenus,
                            onFilterGenusChange = { selectedFilterGenus = it },
                            selectedFormationId = selectedFormationId,
                            onSelectFormation = { formation ->
                                selectedFormationId = formation.id
                                cloudType = when (formation.genus) {
                                    CloudGenus.STRATUS -> 0
                                    CloudGenus.STRATOCUMULUS, CloudGenus.ALTOCUMULUS -> 1
                                    CloudGenus.CUMULONIMBUS, CloudGenus.NIMBOSTRATUS -> 3
                                    else -> 2
                                }
                            },
                            onFocusCamera = { az, el ->
                                camYaw = az
                                camPitch = el.coerceIn(0f, 65f)
                            },
                            cloudCoverage = cloudCoverage,
                            onCloudCoverageChange = { cloudCoverage = it },
                            detectedFormations = detectedFormations
                        )
                        1 -> MeteorosVolumetricsTab(
                            cloudType = cloudType,
                            onCloudTypeChange = { cloudType = it },
                            cloudCoverage = cloudCoverage,
                            onCloudCoverageChange = { cloudCoverage = it },
                            cloudDensity = cloudDensity,
                            onCloudDensityChange = { cloudDensity = it },
                            multiScatteringFactor = multiScatteringFactor,
                            onMultiScatteringFactorChange = { multiScatteringFactor = it },
                            silverLiningG = silverLiningG,
                            onSilverLiningGChange = { silverLiningG = it },
                            powderEffect = powderEffect,
                            onPowderEffectChange = { powderEffect = it },
                            sunAbsorption = sunAbsorption,
                            onSunAbsorptionChange = { sunAbsorption = it },
                            detailErosion = detailErosion,
                            onDetailErosionChange = { detailErosion = it },
                            enableGodRays = enableGodRays,
                            onEnableGodRaysChange = { enableGodRays = it },
                            godRaysIntensity = godRaysIntensity,
                            onGodRaysIntensityChange = { godRaysIntensity = it },
                            enableCirrus = enableCirrus,
                            onEnableCirrusChange = { enableCirrus = it },
                            cirrusCoverage = cirrusCoverage,
                            onCirrusCoverageChange = { cirrusCoverage = it },
                            windSpeed = windSpeed,
                            onWindSpeedChange = { windSpeed = it },
                            windAzimuth = windAzimuth,
                            onWindAzimuthChange = { windAzimuth = it },
                            enableBayerJitter = enableBayerJitter,
                            onEnableBayerJitterChange = { enableBayerJitter = it }
                        )
                        2 -> VulkanStageTab(
                            surfaceMode = surfaceMode,
                            onSurfaceModeChange = { surfaceMode = it },
                            stageWidth = stageWidth,
                            onStageWidthChange = { stageWidth = it },
                            stageLength = stageLength,
                            onStageLengthChange = { stageLength = it },
                            stageHeight = stageHeight,
                            onStageHeightChange = { stageHeight = it },
                            showGrid = showStageGrid,
                            onShowGridChange = { showStageGrid = it },
                            showCalibrationItem = showCalibrationItem,
                            onShowCalibrationItemChange = { showCalibrationItem = it }
                        )
                        3 -> {
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
                        4 -> SunAndFlareTab(
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
                        5 -> StarsAndCosmosTab(
                            starCount = starCount,
                            onStarCountChange = { starCount = it },
                            twinkleSpeed = twinkleSpeed,
                            onTwinkleSpeedChange = { twinkleSpeed = it },
                            magnitudeCutoff = magnitudeCutoff,
                            onMagnitudeCutoffChange = { magnitudeCutoff = it },
                            milkyWayIntensity = milkyWayIntensity,
                            onMilkyWayChange = { milkyWayIntensity = it }
                        )
                        6 -> MoonAndPhasesTab(
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
                        7 -> TonemapAndLUTTab(
                            selectedMode = tonemapMode,
                            onModeSelect = { tonemapMode = it },
                            exposureEv = exposureEv,
                            onExposureChange = { exposureEv = it },
                            colorTempKelvin = colorTempKelvin,
                            onColorTempChange = { colorTempKelvin = it },
                            contrast = contrast,
                            onContrastChange = { contrast = it }
                        )
                        8 -> SkyNativeCodeViewerTab()
                    }
                }
            }
        }
    }
}
}

