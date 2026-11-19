package com.example.sphere.trailers

import android.graphics.Bitmap
import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.fadeIn
import androidx.compose.animation.fadeOut
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.gestures.detectTransformGestures
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.RotateRight
import androidx.compose.material.icons.filled.Add
import androidx.compose.material.icons.filled.Info
import androidx.compose.material.icons.filled.Remove
import androidx.compose.material.icons.outlined.Close
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.FilterQuality
import androidx.compose.ui.graphics.ImageBitmap
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.layout.onSizeChanged
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.IntSize
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.CommodityItem
import com.example.sphere.model.TrailerItem
import com.example.sphere.nativebridge.SphereNativeEngine
import com.example.sphere.viewport.SceneFpsBadge
import com.example.sphere.viewport.SceneFpsConfig
import com.example.sphere.viewport.ScenePlayRotateButton
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.conflate
import kotlinx.coroutines.flow.distinctUntilChanged
import kotlinx.coroutines.flow.filterNotNull
import kotlinx.coroutines.withContext
import kotlin.math.max
import kotlin.math.roundToInt

private data class TrailerRenderKey(
    val bmpW: Int,
    val bmpH: Int,
    val trailerId: Int,
    val rotX: Float,
    val rotY: Float,
    val zoom: Float,
    val landingGearExtended: Boolean,
    val showLoadedCargo: Boolean,
    val cargoId: Int,
    val cargoQty: Int,
    val curtainOpenRatio: Float,
    val hydraulicLiftRatio: Float,
    val curtainOpacity: Float,
    val baseArgb: Int,
    val secondaryArgb: Int,
    val roughness: Float,
    val metallic: Float,
    val axleCount: Int,
    val suspensionOrdinal: Int,
    val brakeOrdinal: Int,
    val hasLiftAxle: Boolean,
    val liftAxleRaised: Boolean,
    val hasSteerableAxle: Boolean,
    val steerAngleDeg: Float
)

private class DoubleBufferedTrailerRaster {
    var bufferA: Bitmap? = null
    var bufferB: Bitmap? = null
    var writeToA: Boolean = true
}

@Composable
fun TrailersViewportCanvas(
    item: TrailerItem,
    rotX: Float,
    rotY: Float,
    zoomFactor: Float,
    onDrag: (Float, Float) -> Unit,
    onZoomChange: (Float) -> Unit,
    onResetView: () -> Unit,
    landingGearExtended: Boolean,
    showLoadedCargo: Boolean,
    loadedCargo: CommodityItem?,
    cargoQuantity: Int,
    curtainOpenRatio: Float,
    hydraulicLiftRatio: Float,
    isAutoRotating: Boolean = false,
    onToggleAutoRotate: () -> Unit = {},
    modifier: Modifier = Modifier
) {
    var showTelemetryInfo by remember { mutableStateOf(false) }
    var meshStats by remember { mutableStateOf(intArrayOf(0, 0)) }
    var canvasSizePx by remember { mutableStateOf(IntSize.Zero) }
    var displayImageBitmap by remember { mutableStateOf<ImageBitmap?>(null) }
    var isRendering by remember { mutableStateOf(false) }
    var lastRenderMs by remember { mutableFloatStateOf(0f) }
    var measuredAutoFps by remember { mutableIntStateOf(SceneFpsConfig.TARGET_FPS) }
    val currentAutoRotating by rememberUpdatedState(isAutoRotating)
    val rasterBuffers = remember { DoubleBufferedTrailerRaster() }

    val payloadMassKg = if (showLoadedCargo && loadedCargo != null) {
        loadedCargo.nominalWeightKg * cargoQuantity
    } else 0f

    val currentRenderKey = remember(
        canvasSizePx,
        item,
        rotX,
        rotY,
        zoomFactor,
        landingGearExtended,
        showLoadedCargo,
        loadedCargo?.id,
        cargoQuantity,
        curtainOpenRatio,
        hydraulicLiftRatio
    ) {
        if (canvasSizePx.width <= 0 || canvasSizePx.height <= 0) {
            null
        } else {
            val maxDim = max(canvasSizePx.width, canvasSizePx.height)
            val resScale = if (maxDim > 540) 540f / maxDim.toFloat() else 1f
            val bmpW = (canvasSizePx.width * resScale).roundToInt().coerceAtLeast(64)
            val bmpH = (canvasSizePx.height * resScale).roundToInt().coerceAtLeast(64)
            TrailerRenderKey(
                bmpW = bmpW,
                bmpH = bmpH,
                trailerId = item.id,
                rotX = rotX,
                rotY = rotY,
                zoom = zoomFactor,
                landingGearExtended = landingGearExtended,
                showLoadedCargo = showLoadedCargo,
                cargoId = loadedCargo?.id ?: 0,
                cargoQty = cargoQuantity,
                curtainOpenRatio = curtainOpenRatio,
                hydraulicLiftRatio = hydraulicLiftRatio,
                curtainOpacity = item.curtainOpacity,
                baseArgb = item.baseColor.toArgb(),
                secondaryArgb = item.secondaryColor.toArgb(),
                roughness = item.roughness,
                metallic = item.metallic,
                axleCount = item.axleConfig.count,
                suspensionOrdinal = item.suspensionType.ordinal,
                brakeOrdinal = item.brakeType.ordinal,
                hasLiftAxle = item.hasLiftAxle,
                liftAxleRaised = item.liftAxleRaised,
                hasSteerableAxle = item.hasSteerableAxle,
                steerAngleDeg = item.steerAngleDeg
            )
        }
    }

    val latestKeyState = rememberUpdatedState(currentRenderKey)

    LaunchedEffect(Unit) {
        var prevCommitNs = 0L
        var emaFps = SceneFpsConfig.TARGET_FPS.toFloat()
        snapshotFlow { latestKeyState.value }
            .filterNotNull()
            .distinctUntilChanged()
            .conflate()
            .collect { key ->
                isRendering = true
                val writeA = rasterBuffers.writeToA
                var targetBmp = if (writeA) rasterBuffers.bufferA else rasterBuffers.bufferB
                if (targetBmp == null || targetBmp.width != key.bmpW || targetBmp.height != key.bmpH) {
                    targetBmp = Bitmap.createBitmap(key.bmpW, key.bmpH, Bitmap.Config.ARGB_8888)
                    if (writeA) {
                        rasterBuffers.bufferA = targetBmp
                    } else {
                        rasterBuffers.bufferB = targetBmp
                    }
                }

                val tStartNs = System.nanoTime()
                val stats = withContext(Dispatchers.Default) {
                    SphereNativeEngine.renderTrailerBitmap(
                        bitmap = targetBmp,
                        trailerId = key.trailerId,
                        rotX = key.rotX,
                        rotY = key.rotY,
                        zoom = key.zoom,
                        landingGearExtended = key.landingGearExtended,
                        showLoadedCargo = key.showLoadedCargo,
                        cargoId = key.cargoId,
                        cargoQty = key.cargoQty,
                        curtainOpenRatio = key.curtainOpenRatio,
                        hydraulicLiftRatio = key.hydraulicLiftRatio,
                        curtainOpacity = key.curtainOpacity,
                        baseArgb = key.baseArgb,
                        secondaryArgb = key.secondaryArgb,
                        roughness = key.roughness,
                        metallic = key.metallic,
                        axleCount = key.axleCount,
                        suspensionOrdinal = key.suspensionOrdinal,
                        brakeOrdinal = key.brakeOrdinal,
                        hasLiftAxle = key.hasLiftAxle,
                        liftAxleRaised = key.liftAxleRaised,
                        hasSteerableAxle = key.hasSteerableAxle,
                        steerAngleDeg = key.steerAngleDeg
                    )
                }
                val nowNs = System.nanoTime()
                lastRenderMs = ((nowNs - tStartNs) / 1_000_000f).coerceAtLeast(0.1f)
                if (currentAutoRotating && prevCommitNs != 0L) {
                    val deltaNs = (nowNs - prevCommitNs).coerceAtLeast(1_000_000L)
                    if (deltaNs < 250_000_000L) {
                        val instFps = (1_000_000_000f / deltaNs.toFloat()).coerceIn(1f, SceneFpsConfig.TARGET_FPS.toFloat())
                        emaFps = emaFps * 0.80f + instFps * 0.20f
                        measuredAutoFps = emaFps.roundToInt().coerceIn(1, SceneFpsConfig.TARGET_FPS)
                    } else {
                        emaFps = SceneFpsConfig.TARGET_FPS.toFloat()
                    }
                }
                prevCommitNs = nowNs

                displayImageBitmap = targetBmp.asImageBitmap()
                rasterBuffers.writeToA = !writeA
                if (stats.size >= 2 && (stats[0] != meshStats[0] || stats[1] != meshStats[1])) {
                    meshStats = stats
                }
                isRendering = false
            }
    }

    val currentOnDrag by rememberUpdatedState(onDrag)
    val currentOnZoomChange by rememberUpdatedState(onZoomChange)
    val currentZoomFactor by rememberUpdatedState(zoomFactor)

    Box(
        modifier = modifier
            .fillMaxSize()
            .background(Color(0xFF070A10))
            .onSizeChanged { size ->
                if (size.width > 0 && size.height > 0 && size != canvasSizePx) {
                    canvasSizePx = size
                }
            }
            .pointerInput(Unit) {
                detectTransformGestures { _, pan, zoom, _ ->
                    currentOnDrag(pan.x, pan.y)
                    if (zoom != 1.0f) {
                        currentOnZoomChange((currentZoomFactor * zoom).coerceIn(0.55f, 2.6f))
                    }
                }
            }
            .testTag("trailers_3d_viewport")
    ) {
        Canvas(modifier = Modifier.fillMaxSize()) {
            val wPx = size.width.roundToInt().coerceAtLeast(1)
            val hPx = size.height.roundToInt().coerceAtLeast(1)
            val img = displayImageBitmap
            if (img != null) {
                drawImage(
                    image = img,
                    dstOffset = IntOffset.Zero,
                    dstSize = IntSize(wPx, hPx),
                    filterQuality = FilterQuality.Medium
                )
            }
        }

        if (isRendering && displayImageBitmap == null) {
            CircularProgressIndicator(
                modifier = Modifier
                    .align(Alignment.Center)
                    .size(28.dp),
                strokeWidth = 2.5.dp,
                color = MaterialTheme.colorScheme.primary
            )
        }

        // Top Control Floating Bar: Toggle Specs + Zoom/Reset
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(12.dp),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            FilledTonalButton(
                onClick = { showTelemetryInfo = !showTelemetryInfo },
                shape = RoundedCornerShape(20.dp),
                colors = ButtonDefaults.filledTonalButtonColors(
                    containerColor = if (showTelemetryInfo) MaterialTheme.colorScheme.primaryContainer else Color(0xCC0F172A),
                    contentColor = if (showTelemetryInfo) MaterialTheme.colorScheme.onPrimaryContainer else Color(0xFFE2E8F0)
                ),
                contentPadding = PaddingValues(horizontal = 12.dp, vertical = 6.dp),
                modifier = Modifier
                    .height(34.dp)
                    .testTag("trailer_toggle_specs_button")
            ) {
                Icon(Icons.Default.Info, contentDescription = "Toggle Trailer Specs", modifier = Modifier.size(16.dp))
                Spacer(Modifier.width(6.dp))
                Text(if (showTelemetryInfo) "Hide Specs" else "Show Specs", fontSize = 11.sp, fontWeight = FontWeight.SemiBold)
            }

            Surface(
                shape = RoundedCornerShape(20.dp),
                color = Color(0xCC0F172A),
                border = CardDefaults.outlinedCardBorder()
            ) {
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    modifier = Modifier.padding(horizontal = 4.dp, vertical = 2.dp)
                ) {
                    IconButton(
                        onClick = { onZoomChange((zoomFactor - 0.15f).coerceIn(0.55f, 2.6f)) },
                        modifier = Modifier.size(32.dp).testTag("trailer_zoom_out")
                    ) {
                        Icon(Icons.Default.Remove, contentDescription = "Zoom Out", tint = Color.White, modifier = Modifier.size(16.dp))
                    }
                    Text(
                        "${(zoomFactor * 100).roundToInt()}%",
                        color = Color(0xFF38BDF8),
                        fontSize = 11.sp,
                        fontFamily = FontFamily.Monospace,
                        modifier = Modifier.padding(horizontal = 6.dp)
                    )
                    IconButton(
                        onClick = { onZoomChange((zoomFactor + 0.15f).coerceIn(0.55f, 2.6f)) },
                        modifier = Modifier.size(32.dp).testTag("trailer_zoom_in")
                    ) {
                        Icon(Icons.Default.Add, contentDescription = "Zoom In", tint = Color.White, modifier = Modifier.size(16.dp))
                    }
                    IconButton(
                        onClick = onResetView,
                        modifier = Modifier.size(32.dp).testTag("trailer_reset_view")
                    ) {
                        Icon(Icons.AutoMirrored.Filled.RotateRight, contentDescription = "Reset View", tint = Color(0xFFA5D6A7), modifier = Modifier.size(16.dp))
                    }
                }
            }
        }

        if (!showTelemetryInfo) {
            Row(
                modifier = Modifier
                    .align(Alignment.TopEnd)
                    .padding(top = 56.dp, end = 12.dp),
                verticalAlignment = Alignment.CenterVertically,
                horizontalArrangement = Arrangement.spacedBy(6.dp)
            ) {
                ScenePlayRotateButton(
                    isPlaying = isAutoRotating,
                    onToggle = onToggleAutoRotate,
                    tag = "trailer_play_rotate_button"
                )
                SceneFpsBadge(
                    overrideFps = if (isAutoRotating) measuredAutoFps else null,
                    renderTimeMs = lastRenderMs,
                    tag = "trailer_scene_fps_badge"
                )
            }
        }

        // Toggleable Telemetry Card
        AnimatedVisibility(
            visible = showTelemetryInfo,
            enter = fadeIn(),
            exit = fadeOut(),
            modifier = Modifier
                .align(Alignment.TopCenter)
                .padding(top = 52.dp, start = 14.dp, end = 14.dp)
        ) {
            Card(
                colors = CardDefaults.cardColors(containerColor = Color(0xEE0B101B)),
                shape = RoundedCornerShape(12.dp),
                border = CardDefaults.outlinedCardBorder().copy(
                    brush = Brush.linearGradient(listOf(Color(0xFF2563EB), Color(0xFF06B6D4)))
                ),
                modifier = Modifier.fillMaxWidth().testTag("trailer_telemetry_panel")
            ) {
                Column(modifier = Modifier.padding(12.dp)) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Row(verticalAlignment = Alignment.CenterVertically) {
                            Box(modifier = Modifier.size(8.dp).background(Color(0xFF22C55E), CircleShape))
                            Spacer(Modifier.width(6.dp))
                            Text(
                                "C++ HAULAGE CHASSIS & VULKAN TELEMETRY",
                                style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold),
                                color = Color(0xFF38BDF8),
                                letterSpacing = 1.sp
                            )
                        }
                        IconButton(onClick = { showTelemetryInfo = false }, modifier = Modifier.size(24.dp)) {
                            Icon(Icons.Outlined.Close, contentDescription = "Close", tint = Color.Gray, modifier = Modifier.size(16.dp))
                        }
                    }
                    Spacer(Modifier.height(8.dp))
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(12.dp)) {
                        Column(modifier = Modifier.weight(1f)) {
                            Text("Chassis & Scale", fontWeight = FontWeight.Bold, fontSize = 10.sp, color = Color(0xFF94A3B8))
                            Text("• Empty: ${item.emptyWeightKg.roundToInt()} kg", fontSize = 10.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• Payload: ${payloadMassKg.roundToInt()} / ${item.maxPayloadKg.roundToInt()} kg", fontSize = 10.sp, color = Color(0xFFFACC15), fontFamily = FontFamily.Monospace)
                            Text("• Dims: ${item.lengthM}m × ${item.widthM}m × ${item.heightM}m", fontSize = 10.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• Deck / Clr: ${item.deckHeightM}m / ${String.format("%.2f", item.groundClearanceM)}m", fontSize = 10.sp, color = Color(0xFF38BDF8), fontFamily = FontFamily.Monospace)
                        }
                        Column(modifier = Modifier.weight(1f)) {
                            Text("Running Gear (C++)", fontWeight = FontWeight.Bold, fontSize = 10.sp, color = Color(0xFFF472B6))
                            Text("• Axles: ${item.axleConfig.displayName}", fontSize = 9.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• Tires: ${item.tireSpecLabel}", fontSize = 9.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• Hitch: ${item.hitchType.displayName}", fontSize = 9.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• Mesh: ${meshStats[1]} tris (${meshStats[0]} vtx)", fontSize = 9.sp, color = Color(0xFF22C55E), fontFamily = FontFamily.Monospace)
                        }
                    }
                }
            }
        }

        // Bottom Variant + Cargo Status Pill
        Surface(
            shape = RoundedCornerShape(20.dp),
            color = Color(0xCC0B111E),
            border = CardDefaults.outlinedCardBorder(),
            modifier = Modifier.align(Alignment.BottomCenter).padding(bottom = 8.dp)
        ) {
            Row(
                modifier = Modifier.padding(horizontal = 12.dp, vertical = 4.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    if (showLoadedCargo && loadedCargo != null) "LOADED: ${cargoQuantity}× ${loadedCargo.name}" else "C++ TRAILER: ${item.name}",
                    style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold),
                    color = if (showLoadedCargo && loadedCargo != null) Color(0xFFFACC15) else Color.White,
                    fontFamily = FontFamily.Monospace,
                    fontSize = 10.sp
                )
            }
        }
    }
}
