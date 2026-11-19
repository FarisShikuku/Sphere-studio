package com.example.sphere.commodities

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
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.*
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.layout.onSizeChanged
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.TextMeasurer
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.drawText
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.rememberTextMeasurer
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.IntSize
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.CommodityItem
import com.example.sphere.model.StackingMode
import com.example.sphere.model.StencilStampType
import com.example.sphere.nativebridge.SphereNativeEngine
import com.example.sphere.viewport.SceneFpsBadge
import com.example.sphere.viewport.SceneFpsConfig
import com.example.sphere.viewport.ScenePlayRotateButton
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.conflate
import kotlinx.coroutines.flow.distinctUntilChanged
import kotlinx.coroutines.flow.filterNotNull
import kotlinx.coroutines.withContext
import kotlin.math.*

private data class CargoRenderKey(
    val bmpW: Int,
    val bmpH: Int,
    val cargoId: Int,
    val stackingMode: Int,
    val rotX: Float,
    val rotY: Float,
    val zoom: Float,
    val showPallet: Boolean,
    val showStraps: Boolean,
    val showShrinkwrap: Boolean,
    val primaryArgb: Int,
    val secondaryArgb: Int,
    val roughness: Float,
    val metallic: Float,
    val wear: Float,
    val wrinkle: Float,
    val opacity: Float,
    val lowDetail: Boolean
)

private class DoubleBufferedCargoRaster {
    var bufferA: Bitmap? = null
    var bufferB: Bitmap? = null
    var writeToA: Boolean = true
}

@Composable
fun CommoditiesViewportCanvas(
    item: CommodityItem,
    stackingMode: StackingMode,
    rotX: Float,
    rotY: Float,
    zoomFactor: Float,
    onDrag: (Float, Float) -> Unit,
    onZoomChange: (Float) -> Unit = {},
    onResetView: () -> Unit = {},
    showStraps: Boolean,
    showShrinkwrap: Boolean,
    showPalletBase: Boolean,
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
    val textMeasurer = rememberTextMeasurer()
    val rasterBuffers = remember { DoubleBufferedCargoRaster() }

    val isMultiUnitStack = stackingMode != StackingMode.SINGLE_UNIT
    val effOpacity = remember(item.opacityBias, item.opacity) {
        if (item.opacityBias <= 0.01f) {
            1.0f
        } else {
            (1.0f - item.opacityBias * 0.6f).coerceIn(0.35f, 1.0f)
        }
    }

    val currentRenderKey = remember(
        canvasSizePx,
        item,
        stackingMode,
        rotX,
        rotY,
        zoomFactor,
        showPalletBase,
        showStraps,
        showShrinkwrap,
        effOpacity,
        isMultiUnitStack
    ) {
        if (canvasSizePx.width <= 0 || canvasSizePx.height <= 0) {
            null
        } else {
            val maxDim = max(canvasSizePx.width, canvasSizePx.height)
            val resScale = if (maxDim > 540) 540f / maxDim.toFloat() else 1f
            val bmpW = (canvasSizePx.width * resScale).roundToInt().coerceAtLeast(64)
            val bmpH = (canvasSizePx.height * resScale).roundToInt().coerceAtLeast(64)
            CargoRenderKey(
                bmpW = bmpW,
                bmpH = bmpH,
                cargoId = item.id,
                stackingMode = stackingMode.ordinal,
                rotX = rotX,
                rotY = rotY,
                zoom = zoomFactor,
                showPallet = showPalletBase,
                showStraps = showStraps,
                showShrinkwrap = showShrinkwrap,
                primaryArgb = item.baseColor.toArgb(),
                secondaryArgb = item.secondaryColor.toArgb(),
                roughness = item.roughness,
                metallic = item.metallic,
                wear = item.wearAndTear,
                wrinkle = item.wrinkleCreaseDepth,
                opacity = effOpacity,
                lowDetail = isMultiUnitStack
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
                    SphereNativeEngine.renderCargoBitmap(
                        bitmap = targetBmp,
                        cargoId = key.cargoId,
                        stackingMode = key.stackingMode,
                        rotX = key.rotX,
                        rotY = key.rotY,
                        zoom = key.zoom,
                        showPallet = key.showPallet,
                        showStraps = key.showStraps,
                        showShrinkwrap = key.showShrinkwrap,
                        primaryArgb = key.primaryArgb,
                        secondaryArgb = key.secondaryArgb,
                        roughness = key.roughness,
                        metallic = key.metallic,
                        wear = key.wear,
                        wrinkle = key.wrinkle,
                        opacity = key.opacity,
                        lowDetail = key.lowDetail
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
            .background(Color(0xFF07090E))
            .onSizeChanged { size ->
                if (size.width > 0 && size.height > 0 && size != canvasSizePx) {
                    canvasSizePx = size
                }
            }
            .pointerInput(Unit) {
                detectTransformGestures { _, pan, zoom, _ ->
                    currentOnDrag(pan.x, pan.y)
                    if (zoom != 1.0f) {
                        val newZoom = (currentZoomFactor * zoom).coerceIn(0.5f, 2.5f)
                        currentOnZoomChange(newZoom)
                    }
                }
            }
            .testTag("commodities_3d_viewport")
    ) {
        Canvas(modifier = Modifier.fillMaxSize()) {
            val w = size.width
            val h = size.height
            val wPx = w.roundToInt().coerceAtLeast(1)
            val hPx = h.roundToInt().coerceAtLeast(1)

            val img = displayImageBitmap
            if (img != null) {
                drawImage(
                    image = img,
                    dstOffset = IntOffset.Zero,
                    dstSize = IntSize(wPx, hPx),
                    filterQuality = FilterQuality.Medium
                )
            }

            // Projected 3D stencil & barcode decal aligned to C++ front-most unit camera projection
            val sceneCenter = Offset(w * 0.5f, h * 0.53f)
            val baseScale = min(w, h) * 0.44f * zoomFactor
            val radX = rotX * (PI.toFloat() / 180f)
            val radY = rotY * (PI.toFloat() / 180f)
            val cosY = cos(radY); val sinY = sin(radY)
            val cosX = cos(radX); val sinX = sin(radX)

            val cols: Int
            val rows: Int
            val tiers: Int
            val unitScaleRatio: Float
            when (stackingMode) {
                StackingMode.SINGLE_UNIT -> { cols = 1; rows = 1; tiers = 1; unitScaleRatio = 1.0f }
                StackingMode.PALLET_STACK -> { cols = 2; rows = 2; tiers = 2; unitScaleRatio = 0.48f }
                StackingMode.WAREHOUSE_RACK -> { cols = 2; rows = 2; tiers = 3; unitScaleRatio = 0.42f }
                StackingMode.TRUCK_BED -> { cols = 3; rows = 2; tiers = 3; unitScaleRatio = 0.36f }
            }

            val cSpanX = item.lengthM.coerceAtLeast(0.04f)
            val cSpanY = item.heightM.coerceAtLeast(0.04f)
            val cSpanZ = item.widthM.coerceAtLeast(0.04f)
            val effectiveScale = (0.85f / maxOf(cSpanX, cSpanY, cSpanZ)) * unitScaleRatio

            val meshW = (cSpanX * effectiveScale).coerceAtLeast(0.20f)
            val meshH = (cSpanY * effectiveScale).coerceAtLeast(0.14f)
            val meshD = (cSpanZ * effectiveScale).coerceAtLeast(0.20f)
            val stepX = meshW * 1.08f
            val stepZ = meshD * 1.08f
            val stepY = meshH * 1.01f

            val hasPallet = (showPalletBase || stackingMode.multiplier > 1) && item.id != 10
            val palletScaleRatio = when (stackingMode) {
                StackingMode.SINGLE_UNIT -> 1.05f
                StackingMode.PALLET_STACK -> 1.25f
                StackingMode.WAREHOUSE_RACK -> 1.32f
                StackingMode.TRUCK_BED -> 1.50f
            }
            val cargoFloorY = if (hasPallet) {
                -0.34f + 0.144f * (0.76f / 1.20f) * palletScaleRatio
            } else {
                -0.34f
            }

            var bestFrontZ = Float.NEGATIVE_INFINITY
            var bestCenter = sceneCenter
            var bestScalePx = baseScale * unitScaleRatio

            for (t in 0 until tiers) {
                val wy = cargoFloorY + meshH * 0.5f + t * stepY
                for (r in 0 until rows) {
                    val wz = (r - (rows - 1) * 0.5f) * stepZ
                    for (c in 0 until cols) {
                        val wx = (c - (cols - 1) * 0.5f) * stepX
                        val rx = wx * cosY - wz * sinY
                        val rz = wx * sinY + wz * cosY
                        val ry = wy * cosX - rz * sinX
                        val fz = wy * sinX + rz * cosX
                        val persp = 3.4f / (3.4f - fz).coerceAtLeast(0.45f)
                        if (fz > bestFrontZ) {
                            bestFrontZ = fz
                            bestCenter = Offset(
                                sceneCenter.x + rx * baseScale * persp,
                                sceneCenter.y - ry * baseScale * persp
                            )
                            bestScalePx = baseScale * unitScaleRatio * persp
                        }
                    }
                }
            }

            drawProjectedStencilOverlay(
                item = item,
                center = bestCenter,
                worldScalePx = bestScalePx,
                rotXDeg = rotX,
                rotYDeg = rotY,
                textMeasurer = textMeasurer
            )
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

        // Top Control Floating Bar: Info Toggle & Zoom & Reset
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
                    .testTag("toggle_telemetry_info_button")
            ) {
                Icon(
                    imageVector = Icons.Default.Info,
                    contentDescription = "Toggle Info",
                    modifier = Modifier.size(16.dp)
                )
                Spacer(Modifier.width(6.dp))
                Text(
                    if (showTelemetryInfo) "Hide Specs" else "Show Specs",
                    fontSize = 11.sp,
                    fontWeight = FontWeight.SemiBold
                )
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
                        onClick = { onZoomChange((zoomFactor - 0.15f).coerceIn(0.5f, 2.5f)) },
                        modifier = Modifier
                            .size(32.dp)
                            .testTag("zoom_out_button")
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
                        onClick = { onZoomChange((zoomFactor + 0.15f).coerceIn(0.5f, 2.5f)) },
                        modifier = Modifier
                            .size(32.dp)
                            .testTag("zoom_in_button")
                    ) {
                        Icon(Icons.Default.Add, contentDescription = "Zoom In", tint = Color.White, modifier = Modifier.size(16.dp))
                    }

                    Spacer(Modifier.width(2.dp))

                    IconButton(
                        onClick = onResetView,
                        modifier = Modifier
                            .size(32.dp)
                            .testTag("reset_view_button")
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
                    tag = "cargo_play_rotate_button"
                )
                SceneFpsBadge(
                    overrideFps = if (isAutoRotating) measuredAutoFps else null,
                    renderTimeMs = lastRenderMs,
                    tag = "commodity_scene_fps_badge"
                )
            }
        }

        // Toggleable Compact Telemetry Overlay (Physical + C++ Vulkan Pipeline)
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
                    brush = Brush.linearGradient(listOf(Color(0xFF2563EB), Color(0xFF0284C7)))
                ),
                modifier = Modifier
                    .fillMaxWidth()
                    .testTag("telemetry_info_panel")
            ) {
                Column(modifier = Modifier.padding(12.dp)) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Row(verticalAlignment = Alignment.CenterVertically) {
                            Box(
                                modifier = Modifier
                                    .size(8.dp)
                                    .background(Color(0xFF22C55E), CircleShape)
                            )
                            Spacer(Modifier.width(6.dp))
                            Text(
                                "C++ CARGO REGISTRY & VULKAN TELEMETRY",
                                style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold),
                                color = Color(0xFF38BDF8),
                                letterSpacing = 1.sp
                            )
                        }

                        IconButton(
                            onClick = { showTelemetryInfo = false },
                            modifier = Modifier.size(24.dp)
                        ) {
                            Icon(Icons.Outlined.Close, contentDescription = "Close", tint = Color.Gray, modifier = Modifier.size(16.dp))
                        }
                    }

                    Spacer(Modifier.height(8.dp))

                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(12.dp)) {
                        Column(modifier = Modifier.weight(1f)) {
                            val totalMassKg = item.nominalWeightKg * stackingMode.multiplier
                            val totalVolM3 = item.volumeM3 * stackingMode.multiplier
                            val density = if (item.volumeM3 > 0.0001f) (item.nominalWeightKg / item.volumeM3).roundToInt() else 625

                            Text("Logistics Specs", fontWeight = FontWeight.Bold, fontSize = 10.sp, color = Color(0xFF94A3B8))
                            Text("• Mass: ${totalMassKg.roundToInt()} kg (${item.nominalWeightKg} kg/ea)", fontSize = 10.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• Volume: ${String.format("%.3f", totalVolM3)} m³", fontSize = 10.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• Density: $density kg/m³", fontSize = 10.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• Dims: ${item.lengthM}m × ${item.widthM}m × ${item.heightM}m", fontSize = 10.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                        }

                        Column(modifier = Modifier.weight(1f)) {
                            Text("Native C++ Pipeline", fontWeight = FontWeight.Bold, fontSize = 10.sp, color = Color(0xFFF472B6))
                            Text("• Albedo: ${item.vkAlbedoFormat}", fontSize = 9.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• Normal: ${item.vkNormalFormat}", fontSize = 9.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• Mesh: ${meshStats[1]} tris (${meshStats[0]} vtx)", fontSize = 9.sp, color = Color(0xFF22C55E), fontFamily = FontFamily.Monospace)
                            Text("• Engine: C++ CargoRegistry", fontSize = 9.sp, color = Color(0xFF38BDF8), fontFamily = FontFamily.Monospace)
                        }
                    }
                }
            }
        }

        // Bottom Product Label Badge
        Surface(
            shape = RoundedCornerShape(20.dp),
            color = Color(0xCC0B111E),
            border = CardDefaults.outlinedCardBorder(),
            modifier = Modifier
                .align(Alignment.BottomCenter)
                .padding(bottom = 8.dp)
        ) {
            Row(
                modifier = Modifier.padding(horizontal = 12.dp, vertical = 4.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    "C++ MESH: ",
                    style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold),
                    color = Color(0xFF38BDF8),
                    fontSize = 10.sp
                )
                Text(
                    item.name,
                    style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold),
                    color = Color.White,
                    fontFamily = FontFamily.Monospace,
                    fontSize = 10.sp
                )
            }
        }
    }
}

private fun DrawScope.drawProjectedStencilOverlay(
    item: CommodityItem,
    center: Offset,
    worldScalePx: Float,
    rotXDeg: Float,
    rotYDeg: Float,
    textMeasurer: TextMeasurer
) {
    if (!item.showStencil || item.stencilLabel.isBlank()) return

    val radX = rotXDeg * (PI.toFloat() / 180f)
    val radY = rotYDeg * (PI.toFloat() / 180f)
    val cosY = cos(radY); val sinY = sin(radY)
    val cosX = cos(radX); val sinX = sin(radX)

    val nz = cosY * cosX
    if (nz < 0.05f) return

    val zAnchor = 0.28f
    val rx = -zAnchor * sinY
    val rz = zAnchor * cosY
    val ry = 0.04f * cosX - rz * sinX
    val fz = 0.04f * sinX + rz * cosX

    val persp = 3.4f / (3.4f - fz).coerceAtLeast(0.5f)
    val px = center.x + rx * worldScalePx * persp
    val py = center.y - ry * worldScalePx * persp

    val textRes = textMeasurer.measure(
        text = item.stencilLabel.take(22),
        style = TextStyle(
            fontSize = (9.5f * item.stencilScale).coerceIn(7f, 15f).sp,
            fontWeight = FontWeight.Bold,
            fontFamily = FontFamily.Monospace,
            color = item.stencilColor.copy(alpha = (nz * 0.95f).coerceIn(0.3f, 0.95f))
        )
    )

    val padX = 10f
    val padY = 5f
    val boxW = textRes.size.width + padX * 2f
    val boxH = textRes.size.height + padY * 2f

    drawRect(
        color = Color.White.copy(alpha = (0.78f * nz).coerceIn(0.25f, 0.85f)),
        topLeft = Offset(px - boxW * 0.5f, py - boxH * 0.5f),
        size = Size(boxW, boxH)
    )
    drawRect(
        color = item.stencilColor.copy(alpha = 0.65f * nz),
        topLeft = Offset(px - boxW * 0.5f, py - boxH * 0.5f),
        size = Size(boxW, boxH),
        style = Stroke(1.5f)
    )
    drawText(
        textLayoutResult = textRes,
        topLeft = Offset(px - textRes.size.width * 0.5f, py - textRes.size.height * 0.5f)
    )

    if (item.stampType == StencilStampType.BARCODE_GS1) {
        val by = py + boxH * 0.5f + 4f
        for (b in 0..9) {
            val bx = px - boxW * 0.3f + b * (boxW * 0.065f)
            drawLine(
                color = item.stencilColor.copy(alpha = 0.8f * nz),
                start = Offset(bx, by),
                end = Offset(bx, by + 8f),
                strokeWidth = if (b % 2 == 0) 2.5f else 1.2f
            )
        }
    }
}
