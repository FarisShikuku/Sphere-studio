package com.example.sphere.water

import android.graphics.Bitmap
import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.gestures.detectTransformGestures
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.RotateRight
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.FilterQuality
import androidx.compose.ui.graphics.ImageBitmap
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.layout.onSizeChanged
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.IntSize
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.*
import com.example.sphere.nativebridge.SphereNativeEngine
import com.example.sphere.viewport.SceneFpsBadge
import com.example.sphere.viewport.SceneFpsConfig
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.isActive
import kotlinx.coroutines.withContext
import kotlin.math.roundToInt

private class DoubleBufferedWaterRaster {
    var bufferA: Bitmap? = null
    var bufferB: Bitmap? = null
    var writeToA: Boolean = true
}

@Composable
fun WaterViewportCanvas(
    physics: WaterPhysicsState,
    optics: WaterOpticsState,
    tank: WaterTankState,
    waterfall: WaterWaterfallState,
    camera: WaterCameraState,
    isPlaying: Boolean,
    interactionMode: WaterInteractionMode,
    stepTrigger: Int,
    onToggleInteractionMode: () -> Unit,
    onCameraChange: (WaterCameraState) -> Unit,
    onResetCamera: () -> Unit,
    onTriggerPulse: (Int) -> Unit,
    onStepSingleFrame: () -> Unit,
    onRespawnScenario: () -> Unit,
    onTelemetryUpdate: (WaterTelemetryState) -> Unit,
    modifier: Modifier = Modifier
) {
    var viewportSizePx by remember { mutableStateOf(IntSize(360, 240)) }
    var displayBitmap by remember { mutableStateOf<ImageBitmap?>(null) }
    var measuredFps by remember { mutableIntStateOf(SceneFpsConfig.TARGET_FPS) }
    var lastFrameRenderMs by remember { mutableFloatStateOf(0f) }
    var latestTelemetry by remember { mutableStateOf(WaterTelemetryState()) }

    val rasterBuffers = remember { DoubleBufferedWaterRaster() }

    val currentCamera by rememberUpdatedState(camera)
    val currentTank by rememberUpdatedState(tank)
    val currentInteractionMode by rememberUpdatedState(interactionMode)
    val currentIsPlaying by rememberUpdatedState(isPlaying)

    // Sync C++ FluidPhysicsParams, FluidRenderParams, WaterfallCliff & OrbitalCamera whenever UI state changes
    LaunchedEffect(physics, optics, tank, waterfall, camera) {
        SphereNativeEngine.configureWaterSimulation(
            particleCount = physics.particleCount,
            scenarioOrdinal = physics.scenario.ordinalId,
            smoothingRadius = physics.smoothingRadius,
            targetDensity = physics.targetDensity,
            pressureMultiplier = physics.pressureMultiplier,
            nearPressureMultiplier = physics.nearPressureMultiplier,
            viscosityStrength = physics.viscosityStrength,
            surfaceTension = physics.surfaceTension,
            collisionDamping = physics.collisionDamping,
            gravityX = physics.gravityX,
            gravityY = physics.gravityY,
            gravityZ = physics.gravityZ,
            windX = physics.windForceX,
            windZ = physics.windForceZ,
            timeScale = physics.timeScale,
            substeps = physics.substeps,
            boundsWidth = tank.boundsWidth,
            boundsHeight = tank.boundsHeight,
            boundsDepth = tank.boundsDepth,
            waveMakerEnabled = tank.waveMakerEnabled,
            waveFrequency = tank.waveFrequency,
            waveAmplitude = tank.waveAmplitude,
            obstacleEnabled = tank.obstacleEnabled,
            obstacleX = tank.obstacleX,
            obstacleY = tank.obstacleY,
            obstacleZ = tank.obstacleZ,
            obstacleRadius = tank.obstacleRadius,
            debugModeOrdinal = optics.debugMode.ordinalId,
            particleVisualRadius = optics.particleVisualRadius,
            bilateralBlurEnabled = optics.bilateralBlurEnabled,
            blurRadiusPx = optics.blurRadiusPx,
            blurSpatialSigma = optics.blurSpatialSigma,
            blurDepthSigma = optics.blurDepthSigma,
            waterR = optics.waterColor.red,
            waterG = optics.waterColor.green,
            waterB = optics.waterColor.blue,
            absorbR = optics.absorbR,
            absorbG = optics.absorbG,
            absorbB = optics.absorbB,
            absorbStrength = optics.absorptionStrength,
            indexOfRefraction = optics.indexOfRefraction,
            refractionStrength = optics.refractionStrength,
            fresnelPower = optics.fresnelPower,
            fresnelBias = optics.fresnelBias,
            specularPower = optics.specularPower,
            reflectionIntensity = optics.reflectionIntensity,
            foamThreshold = optics.foamThreshold,
            causticsIntensity = optics.causticsIntensity,
            sunAzimuthDeg = optics.sunAzimuthDeg,
            sunElevationDeg = optics.sunElevationDeg,
            showContainerBox = tank.showContainerBox,
            showFloorGrid = tank.showFloorGrid,
            camYawDeg = camera.yawDeg,
            camPitchDeg = camera.pitchDeg,
            camDistance = camera.distance
        )
        SphereNativeEngine.configureWaterfall(
            enabled = waterfall.enabled || physics.scenario == WaterSpawnScenarioType.WATERFALL_CLIFF,
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
    }

    // Real-time simulation + SSFR render loop (runs when playing, or renders static updates when paused)
    LaunchedEffect(isPlaying, stepTrigger, physics, optics, tank, waterfall, camera) {
        var prevFrameNs = withFrameNanos { it }
        var emaFps = SceneFpsConfig.TARGET_FPS.toFloat()
        var pendingSingleStep = stepTrigger > 0 && !isPlaying

        do {
            val frameNanos = withFrameNanos { it }
            val dt = ((frameNanos - prevFrameNs) / 1_000_000_000f).coerceIn(0.008f, 0.033f)

            if (currentCamera.autoOrbit && currentIsPlaying) {
                onCameraChange(
                    currentCamera.copy(
                        yawDeg = ((currentCamera.yawDeg + dt * 16f + 180f) % 360f) - 180f
                    )
                )
            }

            val aspect = if (viewportSizePx.height > 0) {
                viewportSizePx.width.toFloat() / viewportSizePx.height.toFloat()
            } else {
                1.5f
            }
            val bmpW = 360
            val bmpH = (bmpW / aspect).roundToInt().coerceIn(180, 320)

            val writeA = rasterBuffers.writeToA
            var targetBmp = if (writeA) rasterBuffers.bufferA else rasterBuffers.bufferB
            if (targetBmp == null || targetBmp.width != bmpW || targetBmp.height != bmpH) {
                targetBmp = Bitmap.createBitmap(bmpW, bmpH, Bitmap.Config.ARGB_8888)
                if (writeA) rasterBuffers.bufferA = targetBmp else rasterBuffers.bufferB = targetBmp
            }

            val shouldAdvancePhysics = currentIsPlaying || pendingSingleStep
            val stepDt = if (pendingSingleStep) 0.016f else dt
            pendingSingleStep = false

            val renderStartNs = System.nanoTime()
            val rawTelemetry = withContext(Dispatchers.Default) {
                SphereNativeEngine.stepAndRenderWaterBitmap(
                    bitmap = targetBmp,
                    dtSeconds = stepDt,
                    isPlaying = shouldAdvancePhysics
                )
            }
            val renderEndNs = System.nanoTime()

            lastFrameRenderMs = ((renderEndNs - renderStartNs) / 1_000_000f).coerceAtLeast(0.1f)
            val deltaNs = (frameNanos - prevFrameNs).coerceAtLeast(1_000_000L)
            if (deltaNs < 250_000_000L && currentIsPlaying) {
                val instFps = (1_000_000_000f / deltaNs.toFloat()).coerceIn(1f, SceneFpsConfig.TARGET_FPS.toFloat())
                emaFps = emaFps * 0.82f + instFps * 0.18f
                measuredFps = emaFps.roundToInt().coerceIn(1, SceneFpsConfig.TARGET_FPS)
            }
            prevFrameNs = frameNanos

            if (rawTelemetry.size >= 9) {
                val parsed = WaterTelemetryState(
                    activeParticles = rawTelemetry[0].roundToInt(),
                    occupiedCells = rawTelemetry[1].roundToInt(),
                    hashTableSize = rawTelemetry[2].roundToInt(),
                    physicsStepMs = rawTelemetry[3],
                    renderPassMs = rawTelemetry[4],
                    avgDensity = rawTelemetry[5],
                    maxSpeed = rawTelemetry[6],
                    kineticEnergy = rawTelemetry[7],
                    vulkanGpuActive = rawTelemetry[8] > 0.5f
                )
                latestTelemetry = parsed
                onTelemetryUpdate(parsed)
            }

            displayBitmap = targetBmp.asImageBitmap()
            rasterBuffers.writeToA = !writeA
        } while (isActive && currentIsPlaying)
    }

    Box(
        modifier = modifier
            .fillMaxWidth()
            .background(Color(0xFF070B12))
            .onSizeChanged { sz ->
                if (sz.width > 0 && sz.height > 0) {
                    viewportSizePx = sz
                }
            }
            .pointerInput(interactionMode) {
                if (interactionMode == WaterInteractionMode.ORBIT_CAMERA) {
                    detectTransformGestures { _, pan, zoom, _ ->
                        val c = currentCamera
                        val nextYaw = ((c.yawDeg + pan.x * 0.35f + 180f) % 360f) - 180f
                        val nextPitch = (c.pitchDeg + pan.y * 0.30f).coerceIn(-12f, 82f)
                        val nextDist = if (zoom != 1.0f) {
                            (c.distance / zoom).coerceIn(2.0f, 11.0f)
                        } else {
                            c.distance
                        }
                        onCameraChange(
                            c.copy(
                                yawDeg = nextYaw,
                                pitchDeg = nextPitch,
                                distance = nextDist
                            )
                        )
                    }
                } else {
                    detectDragGestures { change, dragAmount ->
                        change.consume()
                        val w = size.width.toFloat().coerceAtLeast(1f)
                        val h = size.height.toFloat().coerceAtLeast(1f)
                        val normU = (change.position.x / w).coerceIn(0f, 1f)
                        val normV = (change.position.y / h).coerceIn(0f, 1f)
                        SphereNativeEngine.injectWaterImpulse(
                            normU = normU,
                            normV = normV,
                            dragDx = (dragAmount.x / w) * 28f,
                            dragDy = (dragAmount.y / h) * 28f,
                            strength = currentTank.impulseStrength
                        )
                    }
                }
            }
            .testTag("water_3d_viewport")
    ) {
        Canvas(modifier = Modifier.fillMaxSize()) {
            val bmp = displayBitmap
            if (bmp != null) {
                drawImage(
                    image = bmp,
                    dstOffset = IntOffset.Zero,
                    dstSize = IntSize(size.width.roundToInt().coerceAtLeast(1), size.height.roundToInt().coerceAtLeast(1)),
                    filterQuality = FilterQuality.Medium
                )
            }
        }

        // Top-Left SPH + SSFR Telemetry HUD
        Surface(
            color = Color(0xCC090E18),
            shape = RoundedCornerShape(10.dp),
            border = BorderStroke(1.dp, Color(0xFF0EA5E9).copy(alpha = 0.35f)),
            modifier = Modifier
                .align(Alignment.TopStart)
                .padding(10.dp)
                .testTag("water_telemetry_hud")
        ) {
            Column(modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp)) {
                Text(
                    text = "${physics.scenario.label.uppercase()} • ${optics.debugMode.label.uppercase()}",
                    color = Color(0xFF38BDF8),
                    fontSize = 10.sp,
                    fontWeight = FontWeight.Bold,
                    fontFamily = FontFamily.Monospace
                )
                Text(
                    text = "${latestTelemetry.activeParticles} pts • ${latestTelemetry.occupiedCells}/${latestTelemetry.hashTableSize} hash cells",
                    color = Color.White,
                    fontSize = 10.sp,
                    fontFamily = FontFamily.Monospace
                )
                Text(
                    text = "SPH ${"%.1f".format(latestTelemetry.physicsStepMs)}ms • SSFR ${"%.1f".format(latestTelemetry.renderPassMs)}ms • vMax ${"%.1f".format(latestTelemetry.maxSpeed)}m/s",
                    color = Color(0xFF94A3B8),
                    fontSize = 9.sp,
                    fontFamily = FontFamily.Monospace
                )
            }
        }

        // Top-Right FPS Badge, Interaction Mode Chip & Camera Controls
        Column(
            modifier = Modifier
                .align(Alignment.TopEnd)
                .padding(10.dp),
            horizontalAlignment = Alignment.End,
            verticalArrangement = Arrangement.spacedBy(6.dp)
        ) {
            SceneFpsBadge(
                overrideFps = if (isPlaying) measuredFps else null,
                renderTimeMs = lastFrameRenderMs,
                tag = "water_scene_fps_badge"
            )

            Surface(
                onClick = onToggleInteractionMode,
                shape = RoundedCornerShape(16.dp),
                color = if (interactionMode == WaterInteractionMode.STIR_FLUID) {
                    Color(0xDD0369A1)
                } else {
                    Color(0xCC0F172A)
                },
                border = BorderStroke(
                    1.dp,
                    if (interactionMode == WaterInteractionMode.STIR_FLUID) Color(0xFF38BDF8) else Color(0xFF334155)
                ),
                modifier = Modifier
                    .minimumInteractiveComponentSize()
                    .testTag("water_interaction_mode_button")
            ) {
                Row(
                    modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp),
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.spacedBy(5.dp)
                ) {
                    Icon(
                        imageVector = if (interactionMode == WaterInteractionMode.STIR_FLUID) {
                            Icons.Default.Waves
                        } else {
                            Icons.Default.ViewInAr
                        },
                        contentDescription = "Toggle Orbit or Stir Fluid",
                        tint = Color(0xFF38BDF8),
                        modifier = Modifier.size(14.dp)
                    )
                    Text(
                        text = if (interactionMode == WaterInteractionMode.STIR_FLUID) "Drag: Stir" else "Drag: Orbit",
                        color = Color.White,
                        fontSize = 10.sp,
                        fontWeight = FontWeight.Bold,
                        fontFamily = FontFamily.Monospace
                    )
                }
            }

            Surface(
                shape = RoundedCornerShape(16.dp),
                color = Color(0xCC0F172A),
                border = BorderStroke(1.dp, Color(0xFF334155))
            ) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    IconButton(
                        onClick = {
                            onCameraChange(
                                camera.copy(distance = (camera.distance + 0.6f).coerceIn(2.0f, 11.0f))
                            )
                        },
                        modifier = Modifier
                            .size(48.dp)
                            .testTag("water_zoom_out_button")
                    ) {
                        Icon(Icons.Default.Remove, contentDescription = "Zoom Out", tint = Color.White, modifier = Modifier.size(16.dp))
                    }
                    IconButton(
                        onClick = {
                            onCameraChange(
                                camera.copy(distance = (camera.distance - 0.6f).coerceIn(2.0f, 11.0f))
                            )
                        },
                        modifier = Modifier
                            .size(48.dp)
                            .testTag("water_zoom_in_button")
                    ) {
                        Icon(Icons.Default.Add, contentDescription = "Zoom In", tint = Color.White, modifier = Modifier.size(16.dp))
                    }
                    IconButton(
                        onClick = onResetCamera,
                        modifier = Modifier
                            .size(48.dp)
                            .testTag("water_reset_camera_button")
                    ) {
                        Icon(
                            Icons.AutoMirrored.Filled.RotateRight,
                            contentDescription = "Reset Camera",
                            tint = Color(0xFF38BDF8),
                            modifier = Modifier.size(16.dp)
                        )
                    }
                }
            }
        }

        // Bottom Interactive Pulse & Respawn Toolbar
        Row(
            modifier = Modifier
                .align(Alignment.BottomCenter)
                .fillMaxWidth()
                .padding(horizontal = 10.dp, vertical = 8.dp),
            horizontalArrangement = Arrangement.spacedBy(6.dp, Alignment.CenterHorizontally),
            verticalAlignment = Alignment.CenterVertically
        ) {
            WaterViewportActionChip(
                label = "Plunge",
                icon = Icons.Default.WaterDrop,
                tag = "water_pulse_plunge",
                onClick = { onTriggerPulse(0) }
            )
            WaterViewportActionChip(
                label = "Vortex",
                icon = Icons.Default.Cyclone,
                tag = "water_pulse_vortex",
                onClick = { onTriggerPulse(1) }
            )
            WaterViewportActionChip(
                label = "Slosh",
                icon = Icons.Default.Waves,
                tag = "water_pulse_slosh",
                onClick = { onTriggerPulse(2) }
            )
            if (!isPlaying) {
                WaterViewportActionChip(
                    label = "Step",
                    icon = Icons.Default.SkipNext,
                    tag = "water_step_frame",
                    onClick = onStepSingleFrame
                )
            }
            WaterViewportActionChip(
                label = "Respawn",
                icon = Icons.Default.Refresh,
                tag = "water_respawn_button",
                onClick = onRespawnScenario
            )
        }
    }
}

@Composable
private fun WaterViewportActionChip(
    label: String,
    icon: androidx.compose.ui.graphics.vector.ImageVector,
    tag: String,
    onClick: () -> Unit
) {
    Surface(
        onClick = onClick,
        shape = RoundedCornerShape(14.dp),
        color = Color(0xD90B1220),
        border = BorderStroke(1.dp, Color(0xFF0EA5E9).copy(alpha = 0.45f)),
        modifier = Modifier
            .minimumInteractiveComponentSize()
            .testTag(tag)
    ) {
        Row(
            modifier = Modifier.padding(horizontal = 9.dp, vertical = 6.dp),
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.spacedBy(4.dp)
        ) {
            Icon(icon, contentDescription = label, tint = Color(0xFF38BDF8), modifier = Modifier.size(13.dp))
            Text(
                text = label,
                color = Color.White,
                fontSize = 10.sp,
                fontWeight = FontWeight.SemiBold,
                fontFamily = FontFamily.Monospace
            )
        }
    }
}
