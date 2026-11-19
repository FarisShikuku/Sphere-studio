package com.example.sphere.vehicles

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
import androidx.compose.material.icons.outlined.Dashboard
import androidx.compose.material.icons.outlined.DirectionsCar
import androidx.compose.material.icons.outlined.MeetingRoom
import androidx.compose.material.icons.outlined.SwapHoriz
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
import com.example.sphere.model.VehicleItem
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

private data class VehicleRenderKey(
    val bmpW: Int,
    val bmpH: Int,
    val vehicleId: Int,
    val rotX: Float,
    val rotY: Float,
    val zoom: Float,
    val liveryOrdinal: Int,
    val randomSeed: Int,
    val baseArgb: Int,
    val accentArgb: Int,
    val roughness: Float,
    val metallic: Float,
    val wheelbaseM: Float,
    val fifthWheelOffsetM: Float,
    val actionRatio: Float,
    val doorOpenRatio: Float,
    val steerAngleDeg: Float,
    val headlightsOn: Boolean,
    val hasRoofRack: Boolean,
    val hasExhaustStacks: Boolean,
    val hitchTrailer: Boolean,
    val trailerId: Int,
    val showLoadedCargo: Boolean,
    val cargoId: Int,
    val cargoQty: Int,
    val isRhd: Boolean,
    val insideCabinView: Boolean
)

private class DoubleBufferedVehicleRaster {
    var bufferA: Bitmap? = null
    var bufferB: Bitmap? = null
    var writeToA: Boolean = true
}

@Composable
fun VehiclesViewportCanvas(
    item: VehicleItem,
    rotX: Float,
    rotY: Float,
    zoomFactor: Float,
    onDrag: (Float, Float) -> Unit,
    onZoomChange: (Float) -> Unit,
    onResetView: () -> Unit,
    onToggleDoor: () -> Unit,
    hitchTrailerEnabled: Boolean,
    hitchedTrailer: TrailerItem?,
    showLoadedCargo: Boolean,
    loadedCargo: CommodityItem?,
    cargoQuantity: Int,
    insideCabinView: Boolean = false,
    onToggleCabinView: () -> Unit = {},
    onToggleCabDriveSide: () -> Unit = {},
    isAutoRotating: Boolean = false,
    onToggleAutoRotate: () -> Unit = {},
    modifier: Modifier = Modifier
) {
    var showTelemetryInfo by remember { mutableStateOf(false) }
    var meshStats by remember { mutableStateOf(intArrayOf(0, 0, 0, 0)) }
    var canvasSizePx by remember { mutableStateOf(IntSize.Zero) }
    var displayImageBitmap by remember { mutableStateOf<ImageBitmap?>(null) }
    var isRendering by remember { mutableStateOf(false) }
    var lastRenderMs by remember { mutableFloatStateOf(0f) }
    var measuredAutoFps by remember { mutableIntStateOf(SceneFpsConfig.TARGET_FPS) }
    val currentAutoRotating by rememberUpdatedState(isAutoRotating)
    val rasterBuffers = remember { DoubleBufferedVehicleRaster() }

    val effectiveHitch = item.canHitchTrailer && hitchTrailerEnabled && hitchedTrailer != null
    val passengerMass = if (item.passengerCapacity > 2) item.passengerCapacity * 72f else 0f
    val trailerMass = if (effectiveHitch && hitchedTrailer != null) hitchedTrailer.emptyWeightKg else 0f
    val cargoMass = if (effectiveHitch && showLoadedCargo && loadedCargo != null) {
        loadedCargo.nominalWeightKg * cargoQuantity
    } else 0f
    val combinedGvw = item.kerbWeightKg + passengerMass + trailerMass + cargoMass

    val currentRenderKey = remember(
        canvasSizePx,
        item,
        rotX,
        rotY,
        zoomFactor,
        effectiveHitch,
        hitchedTrailer?.id,
        showLoadedCargo,
        loadedCargo?.id,
        cargoQuantity,
        insideCabinView
    ) {
        if (canvasSizePx.width <= 0 || canvasSizePx.height <= 0) {
            null
        } else {
            val maxDim = max(canvasSizePx.width, canvasSizePx.height)
            val resScale = if (maxDim > 540) 540f / maxDim.toFloat() else 1f
            val bmpW = (canvasSizePx.width * resScale).roundToInt().coerceAtLeast(64)
            val bmpH = (canvasSizePx.height * resScale).roundToInt().coerceAtLeast(64)
            val effectiveRhd = item.cabDriveSide.name == "RHD"
            VehicleRenderKey(
                bmpW = bmpW,
                bmpH = bmpH,
                vehicleId = item.id,
                rotX = rotX,
                rotY = rotY,
                zoom = zoomFactor,
                liveryOrdinal = item.livery.ordinal,
                randomSeed = item.randomSeed,
                baseArgb = item.baseColor.toArgb(),
                accentArgb = item.accentColor.toArgb(),
                roughness = item.roughness,
                metallic = item.metallic,
                wheelbaseM = item.wheelbaseM,
                fifthWheelOffsetM = item.fifthWheelOffsetM,
                actionRatio = item.actionRatio,
                doorOpenRatio = item.doorOpenRatio,
                steerAngleDeg = item.steerAngleDeg,
                headlightsOn = item.headlightsOn,
                hasRoofRack = item.hasRoofRack,
                hasExhaustStacks = item.hasExhaustStacks,
                hitchTrailer = effectiveHitch,
                trailerId = hitchedTrailer?.id ?: 0,
                showLoadedCargo = effectiveHitch && showLoadedCargo,
                cargoId = loadedCargo?.id ?: 0,
                cargoQty = cargoQuantity,
                isRhd = effectiveRhd,
                insideCabinView = insideCabinView && item.isTruck
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
                    SphereNativeEngine.renderVehicleBitmap(
                        bitmap = targetBmp,
                        vehicleId = key.vehicleId,
                        rotX = key.rotX,
                        rotY = key.rotY,
                        zoom = key.zoom,
                        liveryOrdinal = key.liveryOrdinal,
                        randomSeed = key.randomSeed,
                        baseArgb = key.baseArgb,
                        accentArgb = key.accentArgb,
                        roughness = key.roughness,
                        metallic = key.metallic,
                        wheelbaseM = key.wheelbaseM,
                        fifthWheelOffsetM = key.fifthWheelOffsetM,
                        actionRatio = key.actionRatio,
                        doorOpenRatio = key.doorOpenRatio,
                        steerAngleDeg = key.steerAngleDeg,
                        headlightsOn = key.headlightsOn,
                        hasRoofRack = key.hasRoofRack,
                        hasExhaustStacks = key.hasExhaustStacks,
                        hitchTrailer = key.hitchTrailer,
                        trailerId = key.trailerId,
                        showLoadedCargo = key.showLoadedCargo,
                        cargoId = key.cargoId,
                        cargoQty = key.cargoQty,
                        isRhd = key.isRhd,
                        insideCabinView = key.insideCabinView
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
                if (stats.size >= 4 && (stats[2] != meshStats[2] || stats[3] != meshStats[3])) {
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
                        currentOnZoomChange((currentZoomFactor * zoom).coerceIn(0.55f, 2.8f))
                    }
                }
            }
            .testTag("vehicles_3d_viewport")
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

        // Top Floating Action Bar: Telemetry Toggle + Zoom & Reset
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(12.dp),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Row(horizontalArrangement = Arrangement.spacedBy(6.dp), verticalAlignment = Alignment.CenterVertically) {
                FilledTonalButton(
                    onClick = { showTelemetryInfo = !showTelemetryInfo },
                    shape = RoundedCornerShape(20.dp),
                    colors = ButtonDefaults.filledTonalButtonColors(
                        containerColor = if (showTelemetryInfo) MaterialTheme.colorScheme.primaryContainer else Color(0xCC0F172A),
                        contentColor = if (showTelemetryInfo) MaterialTheme.colorScheme.onPrimaryContainer else Color(0xFFE2E8F0)
                    ),
                    contentPadding = PaddingValues(horizontal = 9.dp, vertical = 6.dp),
                    modifier = Modifier
                        .defaultMinSize(minHeight = 36.dp)
                        .testTag("vehicle_toggle_specs_button")
                ) {
                    Icon(Icons.Default.Info, contentDescription = "Toggle Vehicle Telemetry", modifier = Modifier.size(15.dp))
                    Spacer(Modifier.width(4.dp))
                    Text(if (showTelemetryInfo) "Specs" else "Specs", fontSize = 11.sp, fontWeight = FontWeight.SemiBold)
                }

                val isDoorOpen = item.doorOpenRatio > 0.05f
                FilledTonalButton(
                    onClick = onToggleDoor,
                    shape = RoundedCornerShape(20.dp),
                    colors = ButtonDefaults.filledTonalButtonColors(
                        containerColor = if (isDoorOpen) MaterialTheme.colorScheme.primary else Color(0xCC0F172A),
                        contentColor = if (isDoorOpen) MaterialTheme.colorScheme.onPrimary else Color(0xFFE2E8F0)
                    ),
                    contentPadding = PaddingValues(horizontal = 9.dp, vertical = 6.dp),
                    modifier = Modifier
                        .defaultMinSize(minHeight = 36.dp)
                        .testTag("vehicle_toggle_door_button")
                ) {
                    Icon(Icons.Outlined.MeetingRoom, contentDescription = "Toggle Vehicle Door", modifier = Modifier.size(15.dp))
                    Spacer(Modifier.width(4.dp))
                    Text(if (isDoorOpen) "Close" else "Door", fontSize = 11.sp, fontWeight = FontWeight.SemiBold)
                }

                if (item.isTruck) {
                    val inCabin = insideCabinView
                    FilledTonalButton(
                        onClick = onToggleCabinView,
                        shape = RoundedCornerShape(20.dp),
                        colors = ButtonDefaults.filledTonalButtonColors(
                            containerColor = if (inCabin) MaterialTheme.colorScheme.tertiary else Color(0xCC0F172A),
                            contentColor = if (inCabin) MaterialTheme.colorScheme.onTertiary else Color(0xFFE2E8F0)
                        ),
                        contentPadding = PaddingValues(horizontal = 9.dp, vertical = 6.dp),
                        modifier = Modifier
                            .defaultMinSize(minHeight = 36.dp)
                            .testTag("vehicle_toggle_cabin_view_button")
                    ) {
                        Icon(
                            imageVector = if (inCabin) Icons.Outlined.DirectionsCar else Icons.Outlined.Dashboard,
                            contentDescription = if (inCabin) "Exit Cabin View" else "Inside Cabin View",
                            modifier = Modifier.size(15.dp)
                        )
                        Spacer(Modifier.width(4.dp))
                        Text(if (inCabin) "Exterior" else "Cabin", fontSize = 11.sp, fontWeight = FontWeight.SemiBold)
                    }
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
                        onClick = { onZoomChange((zoomFactor - 0.15f).coerceIn(0.55f, 2.8f)) },
                        modifier = Modifier.size(34.dp).testTag("vehicle_zoom_out")
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
                        onClick = { onZoomChange((zoomFactor + 0.15f).coerceIn(0.55f, 2.8f)) },
                        modifier = Modifier.size(34.dp).testTag("vehicle_zoom_in")
                    ) {
                        Icon(Icons.Default.Add, contentDescription = "Zoom In", tint = Color.White, modifier = Modifier.size(16.dp))
                    }
                    IconButton(
                        onClick = onResetView,
                        modifier = Modifier.size(34.dp).testTag("vehicle_reset_view")
                    ) {
                        Icon(Icons.AutoMirrored.Filled.RotateRight, contentDescription = "Reset View", tint = Color(0xFFA5D6A7), modifier = Modifier.size(16.dp))
                    }
                }
            }
        }

        // Quick LHD / RHD steering-side badge while inside truck cabin
        if (item.isTruck && insideCabinView && !showTelemetryInfo) {
            FilledTonalButton(
                onClick = onToggleCabDriveSide,
                shape = RoundedCornerShape(18.dp),
                colors = ButtonDefaults.filledTonalButtonColors(
                    containerColor = Color(0xCC0F172A),
                    contentColor = Color(0xFF38BDF8)
                ),
                contentPadding = PaddingValues(horizontal = 10.dp, vertical = 6.dp),
                modifier = Modifier
                    .align(Alignment.TopStart)
                    .padding(start = 12.dp, top = 54.dp)
                    .defaultMinSize(minHeight = 36.dp)
                    .testTag("vehicle_toggle_cab_side_button")
            ) {
                Icon(Icons.Outlined.SwapHoriz, contentDescription = "Switch LHD or RHD Steering Side", modifier = Modifier.size(15.dp))
                Spacer(Modifier.width(5.dp))
                Text("${item.cabDriveSide.shortLabel} Cockpit", fontSize = 11.sp, fontWeight = FontWeight.Bold)
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
                    tag = "vehicle_play_rotate_button"
                )
                SceneFpsBadge(
                    overrideFps = if (isAutoRotating) measuredAutoFps else null,
                    renderTimeMs = lastRenderMs,
                    tag = "vehicle_scene_fps_badge"
                )
            }
        }

        // Toggleable C++ Telemetry Overlay Card
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
                    brush = Brush.linearGradient(listOf(Color(0xFF2563EB), Color(0xFF06B6D4)))
                ),
                modifier = Modifier.fillMaxWidth().testTag("vehicle_telemetry_panel")
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
                                "C++ VEHICLE & POWERTRAIN TELEMETRY",
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
                            Text("Chassis & Metrics", fontWeight = FontWeight.Bold, fontSize = 10.sp, color = Color(0xFF94A3B8))
                            Text("• Kerb: ${item.kerbWeightKg.roundToInt()} kg", fontSize = 10.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• Combined GVW: ${combinedGvw.roundToInt()} kg", fontSize = 10.sp, color = Color(0xFFFACC15), fontFamily = FontFamily.Monospace)
                            Text("• Dims: ${item.lengthM}m × ${item.widthM}m × ${item.heightM}m", fontSize = 10.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• WB / Clr: ${String.format("%.2f", item.wheelbaseM)}m / ${String.format("%.2f", item.groundClearanceM)}m", fontSize = 10.sp, color = Color(0xFF38BDF8), fontFamily = FontFamily.Monospace)
                        }
                        Column(modifier = Modifier.weight(1f)) {
                            Text("Engine & Vulkan (C++)", fontWeight = FontWeight.Bold, fontSize = 10.sp, color = Color(0xFFF472B6))
                            Text("• Drive: ${item.driveConfigLabel}", fontSize = 9.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• Output: ${item.enginePowerHp.roundToInt()} HP / ${item.engineTorqueNm.roundToInt()} Nm", fontSize = 9.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• Pax / Tire: ${item.passengerCapacity} / ${item.tireSpecLabel}", fontSize = 9.sp, color = Color.White, fontFamily = FontFamily.Monospace)
                            Text("• Mesh: ${meshStats[3]} tris (${meshStats[2]} vtx)", fontSize = 9.sp, color = Color(0xFF22C55E), fontFamily = FontFamily.Monospace)
                        }
                    }
                }
            }
        }

        // Bottom Coupled Status Pill
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
                val statusLabel = when {
                    insideCabinView && item.isTruck ->
                        "CABIN (${item.cabDriveSide.shortLabel}) • HEAD ${rotY.roundToInt()}° / ${rotX.roundToInt()}° • Drag 360° Look"
                    effectiveHitch && hitchedTrailer != null && showLoadedCargo && loadedCargo != null ->
                        "RIG: ${item.cppEnumName} + ${hitchedTrailer.name} (${cargoQuantity}× ${loadedCargo.name})"
                    effectiveHitch && hitchedTrailer != null ->
                        "HITCHED: ${item.name} + ${hitchedTrailer.name}"
                    else ->
                        "C++ VEHICLE: ${item.name}"
                }
                Text(
                    text = statusLabel,
                    style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold),
                    color = if (insideCabinView && item.isTruck) Color(0xFF38BDF8) else if (effectiveHitch) Color(0xFFFACC15) else Color.White,
                    fontFamily = FontFamily.Monospace,
                    fontSize = 10.sp
                )
            }
        }
    }
}
