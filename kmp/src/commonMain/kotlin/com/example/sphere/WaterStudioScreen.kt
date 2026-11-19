package com.example.sphere

import androidx.activity.compose.BackHandler
import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.Pause
import androidx.compose.material.icons.filled.PlayArrow
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material.icons.filled.RestartAlt
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.*
import com.example.sphere.nativebridge.SphereNativeEngine
import com.example.sphere.water.WaterViewportCanvas
import com.example.sphere.water.tabs.*

/**
 * WaterStudioScreen - Real-Time 3D SPH Fluid Simulation, Waterfall Cliff Dynamics,
 * & Screen-Space Fluid Rendering (SSFR) on Sphere Studio's Vulkan 1.3 C++20 core.
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun WaterStudioScreen(
    onNavigateBack: () -> Unit,
    modifier: Modifier = Modifier
) {
    BackHandler { onNavigateBack() }

    var selectedPreset by remember { mutableStateOf(waterPresetsList[0]) }
    var physics by remember { mutableStateOf(selectedPreset.physics) }
    var optics by remember { mutableStateOf(selectedPreset.optics) }
    var tank by remember { mutableStateOf(selectedPreset.tank) }
    var waterfall by remember { mutableStateOf(selectedPreset.waterfall) }
    var camera by remember { mutableStateOf(WaterCameraState()) }
    var telemetry by remember { mutableStateOf(WaterTelemetryState()) }

    var isPlaying by remember { mutableStateOf(true) }
    var interactionMode by remember { mutableStateOf(WaterInteractionMode.ORBIT_CAMERA) }
    var stepTrigger by remember { mutableIntStateOf(0) }
    var selectedTab by remember { mutableIntStateOf(0) }

    val tabTitles = listOf("SPH Physics", "Waterfall", "SSFR Optics", "Tank & Camera", "Vulkan Compute", "C++ Registry")

    fun syncAndRespawnWaterfall(nextWaterfall: WaterWaterfallState = waterfall.copy(enabled = true)) {
        waterfall = nextWaterfall
        val nextPhysics = physics.copy(
            scenario = WaterSpawnScenarioType.WATERFALL_CLIFF,
            particleCount = physics.particleCount.coerceAtLeast(1536)
        )
        physics = nextPhysics
        tank = tank.copy(
            boundsWidth = maxOf(tank.boundsWidth, 3.4f),
            boundsHeight = maxOf(tank.boundsHeight, 2.5f),
            boundsDepth = maxOf(tank.boundsDepth, 2.4f),
            obstacleEnabled = false,
            waveMakerEnabled = false
        )
        SphereNativeEngine.configureWaterfall(
            enabled = nextWaterfall.enabled,
            cliffHeight = nextWaterfall.cliffHeight,
            cliffWidth = nextWaterfall.cliffWidth,
            cliffDepth = nextWaterfall.cliffDepth,
            flowVolume = nextWaterfall.flowVolume,
            lipSlopeDeg = nextWaterfall.lipSlopeDeg,
            undercut = nextWaterfall.undercut,
            steppedLedges = nextWaterfall.steppedLedges,
            roughness = nextWaterfall.roughness,
            rockChanneling = nextWaterfall.rockChanneling,
            airDragAndMist = nextWaterfall.airDragAndMist,
            plungePoolDepth = nextWaterfall.plungePoolDepth
        )
        SphereNativeEngine.resetWaterSimulation(
            scenarioOrdinal = WaterSpawnScenarioType.WATERFALL_CLIFF.ordinalId,
            particleCount = nextPhysics.particleCount
        )
    }

    fun applyPreset(preset: WaterPreset) {
        selectedPreset = preset
        physics = preset.physics
        optics = preset.optics
        tank = preset.tank
        waterfall = preset.waterfall
        SphereNativeEngine.configureWaterfall(
            enabled = preset.waterfall.enabled,
            cliffHeight = preset.waterfall.cliffHeight,
            cliffWidth = preset.waterfall.cliffWidth,
            cliffDepth = preset.waterfall.cliffDepth,
            flowVolume = preset.waterfall.flowVolume,
            lipSlopeDeg = preset.waterfall.lipSlopeDeg,
            undercut = preset.waterfall.undercut,
            steppedLedges = preset.waterfall.steppedLedges,
            roughness = preset.waterfall.roughness,
            rockChanneling = preset.waterfall.rockChanneling,
            airDragAndMist = preset.waterfall.airDragAndMist,
            plungePoolDepth = preset.waterfall.plungePoolDepth
        )
        SphereNativeEngine.resetWaterSimulation(
            scenarioOrdinal = preset.physics.scenario.ordinalId,
            particleCount = preset.physics.particleCount
        )
    }

    Scaffold(
        modifier = modifier
            .fillMaxSize()
            .testTag("water_studio_screen"),
        topBar = {
            TopAppBar(
                title = {
                    Column {
                        Text(
                            "Vulkan Water Simulation",
                            style = MaterialTheme.typography.titleMedium.copy(fontWeight = FontWeight.Bold)
                        )
                        Text(
                            "3D SPH Hydrodynamics, Cliff Waterfall & SSFR",
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
                        modifier = Modifier.testTag("water_back_button")
                    ) {
                        Icon(Icons.AutoMirrored.Filled.ArrowBack, contentDescription = "Back")
                    }
                },
                actions = {
                    IconButton(
                        onClick = { isPlaying = !isPlaying },
                        modifier = Modifier.testTag("water_play_pause_button")
                    ) {
                        Icon(
                            imageVector = if (isPlaying) Icons.Default.Pause else Icons.Default.PlayArrow,
                            contentDescription = if (isPlaying) "Pause Fluid Simulation" else "Play Fluid Simulation",
                            tint = if (isPlaying) MaterialTheme.colorScheme.primary else Color.Gray
                        )
                    }
                    IconButton(
                        onClick = {
                            SphereNativeEngine.resetWaterSimulation(
                                scenarioOrdinal = physics.scenario.ordinalId,
                                particleCount = physics.particleCount
                            )
                        },
                        modifier = Modifier.testTag("water_top_respawn_button")
                    ) {
                        Icon(Icons.Default.Refresh, contentDescription = "Respawn Fluid Particles")
                    }
                    IconButton(
                        onClick = {
                            camera = WaterCameraState()
                            applyPreset(waterPresetsList[0])
                        },
                        modifier = Modifier.testTag("water_reset_all_button")
                    ) {
                        Icon(Icons.Default.RestartAlt, contentDescription = "Reset Water Defaults")
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
            // Fluid Presets Chip Strip
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .background(MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.35f))
                    .horizontalScroll(rememberScrollState())
                    .padding(horizontal = 12.dp, vertical = 6.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = "PRESETS:",
                    style = MaterialTheme.typography.labelSmall.copy(
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary
                    )
                )
                waterPresetsList.forEach { preset ->
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
                        modifier = Modifier.testTag("water_preset_${preset.name.lowercase().replace(" ", "_")}")
                    )
                }
            }

            // Real-Time 3D SPH + SSFR Viewport
            WaterViewportCanvas(
                physics = physics,
                optics = optics,
                tank = tank,
                waterfall = waterfall,
                camera = camera,
                isPlaying = isPlaying,
                interactionMode = interactionMode,
                stepTrigger = stepTrigger,
                onToggleInteractionMode = {
                    interactionMode = if (interactionMode == WaterInteractionMode.ORBIT_CAMERA) {
                        WaterInteractionMode.STIR_FLUID
                    } else {
                        WaterInteractionMode.ORBIT_CAMERA
                    }
                },
                onCameraChange = { camera = it },
                onResetCamera = { camera = WaterCameraState() },
                onTriggerPulse = { pulseType ->
                    SphereNativeEngine.triggerWaterPulse(pulseType, tank.impulseStrength)
                },
                onStepSingleFrame = { stepTrigger += 1 },
                onRespawnScenario = {
                    SphereNativeEngine.resetWaterSimulation(
                        scenarioOrdinal = physics.scenario.ordinalId,
                        particleCount = physics.particleCount
                    )
                },
                onTelemetryUpdate = { telemetry = it },
                modifier = Modifier
                    .fillMaxWidth()
                    .height(270.dp)
            )

            // Studio Control Tabs
            ScrollableTabRow(
                selectedTabIndex = selectedTab,
                edgePadding = 12.dp,
                modifier = Modifier.fillMaxWidth()
            ) {
                tabTitles.forEachIndexed { index, title ->
                    Tab(
                        selected = selectedTab == index,
                        onClick = {
                            selectedTab = index
                            if (index == 1 && !waterfall.enabled) {
                                syncAndRespawnWaterfall(waterfall.copy(enabled = true))
                            }
                        },
                        modifier = Modifier.testTag("water_tab_$index"),
                        text = {
                            Text(
                                text = title,
                                style = MaterialTheme.typography.labelMedium.copy(
                                    fontWeight = if (selectedTab == index) FontWeight.Bold else FontWeight.Normal
                                )
                            )
                        }
                    )
                }
            }

            // Tab Content
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .weight(1f)
                    .background(MaterialTheme.colorScheme.background)
            ) {
                when (selectedTab) {
                    0 -> WaterPhysicsTab(
                        physics = physics,
                        onPhysicsChange = { physics = it },
                        onRespawnWithScenario = { scenario, count ->
                            val isCliff = (scenario == WaterSpawnScenarioType.WATERFALL_CLIFF)
                            waterfall = waterfall.copy(enabled = isCliff)
                            physics = physics.copy(scenario = scenario, particleCount = count)
                            if (isCliff) {
                                syncAndRespawnWaterfall(waterfall)
                            } else {
                                SphereNativeEngine.configureWaterfall(
                                    enabled = false,
                                    cliffHeight = waterfall.cliffHeight,
                                    cliffWidth = waterfall.cliffWidth,
                                    cliffDepth = waterfall.cliffDepth,
                                    flowVolume = waterfall.flowVolume,
                                    lipSlopeDeg = waterfall.lipSlopeDeg,
                                    undercut = waterfall.undercut,
                                    steppedLedges = waterfall.steppedLedges,
                                    roughness = waterfall.roughness,
                                    rockChanneling = waterfall.rockChanneling,
                                    airDragAndMist = waterfall.airDragAndMist,
                                    plungePoolDepth = waterfall.plungePoolDepth
                                )
                                SphereNativeEngine.resetWaterSimulation(scenario.ordinalId, count)
                            }
                        }
                    )
                    1 -> WaterWaterfallTab(
                        waterfall = waterfall,
                        physics = physics,
                        onWaterfallChange = { nextWf ->
                            waterfall = nextWf
                            if (nextWf.enabled && physics.scenario != WaterSpawnScenarioType.WATERFALL_CLIFF) {
                                syncAndRespawnWaterfall(nextWf)
                            }
                        },
                        onPhysicsChange = { physics = it },
                        onApplyWaterfallPreset = { preset ->
                            physics = physics.copy(
                                windForceX = preset.windX,
                                windForceZ = preset.windZ,
                                gravityY = preset.gravityY
                            )
                            optics = optics.copy(foamThreshold = preset.foamThreshold)
                            syncAndRespawnWaterfall(preset.waterfall)
                        },
                        onActivateAndRespawnWaterfall = {
                            syncAndRespawnWaterfall(waterfall.copy(enabled = true))
                        }
                    )
                    2 -> WaterOpticsTab(
                        optics = optics,
                        onOpticsChange = { optics = it }
                    )
                    3 -> WaterTankTab(
                        tank = tank,
                        camera = camera,
                        onTankChange = { tank = it },
                        onCameraChange = { camera = it },
                        onTriggerPulse = { pulseType ->
                            SphereNativeEngine.triggerWaterPulse(pulseType, tank.impulseStrength)
                        }
                    )
                    4 -> WaterVulkanComputeTab(
                        physics = physics,
                        telemetry = telemetry
                    )
                    5 -> WaterRegistryCppTab()
                }
            }
        }
    }
}
