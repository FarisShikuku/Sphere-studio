package com.example.sphere.buildings

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
import androidx.compose.material.icons.filled.GridOn
import androidx.compose.material.icons.filled.Info
import androidx.compose.material.icons.filled.Remove
import androidx.compose.material.icons.outlined.Close
import androidx.compose.material.icons.outlined.Layers
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
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.IntSize
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.BuildingPresetItem
import com.example.sphere.model.BuildingStudioState
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

private data class BuildingRenderKey(
    val bmpW: Int,
    val bmpH: Int,
    val rotX: Float,
    val rotY: Float,
    val zoom: Float,
    val state: BuildingStudioState
)

data class BuildingMeshStats(
    val vertexCount: Int = 0,
    val triangleCount: Int = 0,
    val totalHeightMeters: Float = 0f,
    val footprintAreaSqM: Float = 0f,
    val grossFloorAreaSqM: Float = 0f,
    val estimatedWindows: Int = 0
)

private class DoubleBufferedBuildingRaster {
    var bufferA: Bitmap? = null
    var bufferB: Bitmap? = null
    var writeToA: Boolean = true
}

@Composable
fun BuildingsViewportCanvas(
    preset: BuildingPresetItem,
    state: BuildingStudioState,
    rotX: Float,
    rotY: Float,
    zoomFactor: Float,
    onDrag: (Float, Float) -> Unit,
    onZoomChange: (Float) -> Unit,
    onResetView: () -> Unit,
    onToggleWireframe: () -> Unit,
    onToggleShowInside: () -> Unit,
    isAutoRotating: Boolean,
    onToggleAutoRotate: () -> Unit,
    onStatsUpdated: (BuildingMeshStats) -> Unit = {},
    modifier: Modifier = Modifier
) {
    var showTelemetryInfo by remember { mutableStateOf(false) }
    var meshStats by remember { mutableStateOf(BuildingMeshStats()) }
    var canvasSizePx by remember { mutableStateOf(IntSize.Zero) }
    var displayImageBitmap by remember { mutableStateOf<ImageBitmap?>(null) }
    var isRendering by remember { mutableStateOf(false) }
    var lastRenderMs by remember { mutableFloatStateOf(0f) }
    var measuredAutoFps by remember { mutableIntStateOf(SceneFpsConfig.TARGET_FPS) }
    val currentAutoRotating by rememberUpdatedState(isAutoRotating)
    val currentOnStatsUpdated by rememberUpdatedState(onStatsUpdated)
    val rasterBuffers = remember { DoubleBufferedBuildingRaster() }

    val currentRenderKey = remember(canvasSizePx, rotX, rotY, zoomFactor, state) {
        if (canvasSizePx.width <= 0 || canvasSizePx.height <= 0) {
            null
        } else {
            val maxDim = max(canvasSizePx.width, canvasSizePx.height)
            val resScale = if (maxDim > 560) 560f / maxDim.toFloat() else 1f
            val bmpW = (canvasSizePx.width * resScale).roundToInt().coerceAtLeast(64)
            val bmpH = (canvasSizePx.height * resScale).roundToInt().coerceAtLeast(64)
            BuildingRenderKey(
                bmpW = bmpW,
                bmpH = bmpH,
                rotX = rotX,
                rotY = rotY,
                zoom = zoomFactor,
                state = state
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
                    if (writeA) rasterBuffers.bufferA = targetBmp else rasterBuffers.bufferB = targetBmp
                }

                val st = key.state
                val tStartNs = System.nanoTime()
                val rawStats = withContext(Dispatchers.Default) {
                    SphereNativeEngine.renderBuildingBitmap(
                        bitmap = targetBmp,
                        domainOrdinal = st.domainOrdinal,
                        subTypeOrdinal = st.subTypeOrdinal,
                        rotX = key.rotX,
                        rotY = key.rotY,
                        zoom = key.zoom,
                        shapeOrdinal = st.shape.ordinalId,
                        cornerOrdinal = st.cornerType.ordinalId,
                        cornerSize = st.cornerSize,
                        stepModeOrdinal = st.stepMode.ordinalId,
                        stepTiers = st.stepTiers,
                        topTaperRatio = st.topTaperRatio,
                        stepShrinkFactor = st.stepShrinkFactor,
                        stepTerraceRailing = st.stepTerraceRailing,
                        floorCount = st.floorCount,
                        floorHeight = st.floorHeight,
                        width = st.width,
                        depth = st.depth,
                        cornerRadius = st.cornerRadius,
                        windowsPerFacade = st.windowsPerFacade,
                        windowTypeOrdinal = st.windowType.ordinalId,
                        windowWidth = st.windowWidth,
                        windowHeight = st.windowHeight,
                        windowSillHeight = st.windowSillHeight,
                        glassTypeOrdinal = st.glassType.ordinalId,
                        windowIor = st.windowIor,
                        wallMaterialOrdinal = st.wallMaterial.ordinalId,
                        wallColorArgb = st.wallColor.toArgb(),
                        doorTypeOrdinal = st.doorType.ordinalId,
                        doorWidth = st.doorWidth,
                        doorHeight = st.doorHeight,
                        hasEntranceCanopy = st.hasEntranceCanopy,
                        roofTypeOrdinal = st.roofType.ordinalId,
                        roofHeight = st.roofHeight,
                        roofOverhang = st.roofOverhang,
                        showRooftopClutter = st.showRooftopClutter,
                        balconyTypeOrdinal = st.balconyType.ordinalId,
                        hasBalconies = st.hasBalconies,
                        balconyDepth = st.balconyDepth,
                        hasSunLouvres = st.hasSunLouvres,
                        hasFireEscape = st.hasFireEscape,
                        hasCornice = st.hasCornice,
                        hasGroundPillars = st.hasGroundPillars,
                        pillarCount = st.pillarCount,
                        hasWindowShutters = st.hasWindowShutters,
                        showInside = st.showInside,
                        interiorDesign = st.interiorDesign,
                        detailIntensity = st.detailIntensity,
                        detailCount = st.detailCount,
                        detailFlagA = st.detailFlagA,
                        detailFlagB = st.detailFlagB,
                        wireframe = st.wireframe
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
                if (rawStats.size >= 6) {
                    val updated = BuildingMeshStats(
                        vertexCount = rawStats[0].roundToInt(),
                        triangleCount = rawStats[1].roundToInt(),
                        totalHeightMeters = rawStats[2],
                        footprintAreaSqM = rawStats[3],
                        grossFloorAreaSqM = rawStats[4],
                        estimatedWindows = rawStats[5].roundToInt()
                    )
                    if (updated != meshStats) {
                        meshStats = updated
                        currentOnStatsUpdated(updated)
                    }
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
                        currentOnZoomChange((currentZoomFactor * zoom).coerceIn(0.45f, 3.4f))
                    }
                }
            }
            .testTag("buildings_3d_viewport")
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

        // Top HUD Actions Row
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(10.dp),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Row(
                horizontalArrangement = Arrangement.spacedBy(6.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                FilledTonalButton(
                    onClick = { showTelemetryInfo = !showTelemetryInfo },
                    shape = RoundedCornerShape(20.dp),
                    colors = ButtonDefaults.filledTonalButtonColors(
                        containerColor = if (showTelemetryInfo) MaterialTheme.colorScheme.primaryContainer else Color(0xCC0F172A),
                        contentColor = if (showTelemetryInfo) MaterialTheme.colorScheme.onPrimaryContainer else Color(0xFFE2E8F0)
                    ),
                    contentPadding = PaddingValues(horizontal = 10.dp, vertical = 6.dp),
                    modifier = Modifier
                        .defaultMinSize(minHeight = 38.dp)
                        .testTag("building_toggle_specs_button")
                ) {
                    Icon(Icons.Default.Info, contentDescription = "Toggle Building Telemetry", modifier = Modifier.size(15.dp))
                    Spacer(Modifier.width(4.dp))
                    Text("Specs", fontSize = 11.sp, fontWeight = FontWeight.SemiBold)
                }

                FilledTonalButton(
                    onClick = onToggleWireframe,
                    shape = RoundedCornerShape(20.dp),
                    colors = ButtonDefaults.filledTonalButtonColors(
                        containerColor = if (state.wireframe) Color(0xFF0284C7) else Color(0xCC0F172A),
                        contentColor = Color.White
                    ),
                    contentPadding = PaddingValues(horizontal = 10.dp, vertical = 6.dp),
                    modifier = Modifier
                        .defaultMinSize(minHeight = 38.dp)
                        .testTag("building_toggle_wireframe_button")
                ) {
                    Icon(Icons.Default.GridOn, contentDescription = "Toggle CAD Wireframe", modifier = Modifier.size(15.dp))
                    Spacer(Modifier.width(4.dp))
                    Text("CAD", fontSize = 11.sp, fontWeight = FontWeight.SemiBold)
                }

                FilledTonalButton(
                    onClick = onToggleShowInside,
                    shape = RoundedCornerShape(20.dp),
                    colors = ButtonDefaults.filledTonalButtonColors(
                        containerColor = if (state.showInside) MaterialTheme.colorScheme.tertiary else Color(0xCC0F172A),
                        contentColor = if (state.showInside) MaterialTheme.colorScheme.onTertiary else Color(0xFFE2E8F0)
                    ),
                    contentPadding = PaddingValues(horizontal = 10.dp, vertical = 6.dp),
                    modifier = Modifier
                        .defaultMinSize(minHeight = 38.dp)
                        .testTag("building_toggle_cutaway_button")
                ) {
                    Icon(Icons.Outlined.Layers, contentDescription = "Toggle Cutaway Interior", modifier = Modifier.size(15.dp))
                    Spacer(Modifier.width(4.dp))
                    Text(if (state.showInside) "Shell" else "Inside", fontSize = 11.sp, fontWeight = FontWeight.SemiBold)
                }
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
                        onClick = { onZoomChange((zoomFactor - 0.15f).coerceIn(0.45f, 3.4f)) },
                        modifier = Modifier.size(34.dp).testTag("building_zoom_out")
                    ) {
                        Icon(Icons.Default.Remove, contentDescription = "Zoom Out", tint = Color.White, modifier = Modifier.size(16.dp))
                    }
                    Text(
                        "${(zoomFactor * 100).roundToInt()}%",
                        color = Color(0xFF38BDF8),
                        fontSize = 11.sp,
                        fontFamily = FontFamily.Monospace,
                        modifier = Modifier.padding(horizontal = 4.dp)
                    )
                    IconButton(
                        onClick = { onZoomChange((zoomFactor + 0.15f).coerceIn(0.45f, 3.4f)) },
                        modifier = Modifier.size(34.dp).testTag("building_zoom_in")
                    ) {
                        Icon(Icons.Default.Add, contentDescription = "Zoom In", tint = Color.White, modifier = Modifier.size(16.dp))
                    }
                    IconButton(
                        onClick = onResetView,
                        modifier = Modifier.size(34.dp).testTag("building_reset_view")
                    ) {
                        Icon(Icons.AutoMirrored.Filled.RotateRight, contentDescription = "Reset Camera View", tint = Color(0xFFA5D6A7), modifier = Modifier.size(16.dp))
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
                    tag = "building_play_rotate_button"
                )
                SceneFpsBadge(
                    overrideFps = if (isAutoRotating) measuredAutoFps else null,
                    renderTimeMs = lastRenderMs,
                    tag = "building_scene_fps_badge"
                )
            }
        }

        // Toggleable C++ Architectural Telemetry Overlay
        AnimatedVisibility(
            visible = showTelemetryInfo,
            enter = fadeIn(),
            exit = fadeOut(),
            modifier = Modifier
                .align(Alignment.TopCenter)
                .padding(top = 54.dp, start = 14.dp, end = 14.dp)
        ) {
            Card(
                colors = CardDefaults.cardColors(containerColor = Color(0xEE0B101B)),
                shape = RoundedCornerShape(12.dp),
                border = CardDefaults.outlinedCardBorder().copy(
                    brush = Brush.linearGradient(listOf(Color(0xFF38BDF8), Color(0xFF818CF8)))
                ),
                modifier = Modifier.fillMaxWidth().testTag("building_telemetry_panel")
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
                                "C++ ARCHITECTURAL TELEMETRY",
                                style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold),
                                color = Color(0xFF38BDF8),
                                letterSpacing = 1.sp
                            )
                        }
                        IconButton(onClick = { showTelemetryInfo = false }, modifier = Modifier.size(28.dp)) {
                            Icon(Icons.Outlined.Close, contentDescription = "Close Telemetry", tint = Color.Gray, modifier = Modifier.size(16.dp))
                        }
                    }
                    Spacer(Modifier.height(8.dp))
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(12.dp)) {
                        Column(modifier = Modifier.weight(1f)) {
                            Text("Dimensions & Area", fontWeight = FontWeight.Bold, fontSize = 10.sp, color = Color(0xFF94A3B8))
                            Text("• Height: ${String.format("%.1f", meshStats.totalHeightMeters)} m", fontSize = 10.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• Footprint: ${meshStats.footprintAreaSqM.roundToInt()} m²", fontSize = 10.sp, color = Color(0xFFFACC15), fontFamily = FontFamily.Monospace)
                            Text("• Gross Area: ${meshStats.grossFloorAreaSqM.roundToInt()} m²", fontSize = 10.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                        }
                        Column(modifier = Modifier.weight(1f)) {
                            Text("Geometry & Glazing", fontWeight = FontWeight.Bold, fontSize = 10.sp, color = Color(0xFFF472B6))
                            Text("• Tris: ${meshStats.triangleCount}", fontSize = 10.sp, color = Color(0xFF22C55E), fontFamily = FontFamily.Monospace)
                            Text("• Vertices: ${meshStats.vertexCount}", fontSize = 10.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• Windows / IOR: ${meshStats.estimatedWindows} / ${String.format("%.2f", state.windowIor)}", fontSize = 10.sp, color = Color(0xFF38BDF8), fontFamily = FontFamily.Monospace)
                        }
                    }
                }
            }
        }

        // Bottom Status Pill
        Surface(
            shape = RoundedCornerShape(20.dp),
            color = Color(0xCC0B111E),
            border = CardDefaults.outlinedCardBorder(),
            modifier = Modifier
                .align(Alignment.BottomCenter)
                .padding(bottom = 8.dp, start = 12.dp, end = 12.dp)
        ) {
            Row(
                modifier = Modifier.padding(horizontal = 12.dp, vertical = 4.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = "${preset.name} • ${String.format("%.1f", meshStats.totalHeightMeters)}m • ${meshStats.triangleCount} tris",
                    style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold),
                    color = if (state.showInside) Color(0xFFFACC15) else Color.White,
                    fontFamily = FontFamily.Monospace,
                    fontSize = 10.sp,
                    maxLines = 1,
                    overflow = TextOverflow.Ellipsis
                )
            }
        }
    }
}
