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
    val steerAngleDeg: Float,
    val headlightsOn: Boolean,
    val hasRoofRack: Boolean,
    val hasExhaustStacks: Boolean,
    val hitchTrailer: Boolean,
    val trailerId: Int,
    val showLoadedCargo: Boolean,
    val cargoId: Int,
    val cargoQty: Int
)

private class NativeVehicleRasterBuffer {
    var bitmap: Bitmap? = null
    var imageBitmap: ImageBitmap? = null
    var lastKey: VehicleRenderKey? = null
    var stats: IntArray = intArrayOf(0, 0, 0, 0)
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
    hitchTrailerEnabled: Boolean,
    hitchedTrailer: TrailerItem?,
    showLoadedCargo: Boolean,
    loadedCargo: CommodityItem?,
    cargoQuantity: Int,
    modifier: Modifier = Modifier
) {
    var showTelemetryInfo by remember { mutableStateOf(false) }
    var meshStats by remember { mutableStateOf(intArrayOf(0, 0, 0, 0)) }
    val rasterBuffer = remember { NativeVehicleRasterBuffer() }

    val effectiveHitch = item.canHitchTrailer && hitchTrailerEnabled && hitchedTrailer != null
    val passengerMass = if (item.passengerCapacity > 2) item.passengerCapacity * 72f else 0f
    val trailerMass = if (effectiveHitch && hitchedTrailer != null) hitchedTrailer.emptyWeightKg else 0f
    val cargoMass = if (effectiveHitch && showLoadedCargo && loadedCargo != null) {
        loadedCargo.nominalWeightKg * cargoQuantity
    } else 0f
    val combinedGvw = item.kerbWeightKg + passengerMass + trailerMass + cargoMass

    Box(
        modifier = modifier
            .fillMaxSize()
            .background(Color(0xFF070A10))
            .pointerInput(Unit) {
                detectTransformGestures { _, pan, zoom, _ ->
                    onDrag(pan.x, pan.y)
                    if (zoom != 1.0f) {
                        onZoomChange((zoomFactor * zoom).coerceIn(0.55f, 2.8f))
                    }
                }
            }
            .testTag("vehicles_3d_viewport")
    ) {
        Canvas(modifier = Modifier.fillMaxSize()) {
            val wPx = size.width.roundToInt().coerceAtLeast(1)
            val hPx = size.height.roundToInt().coerceAtLeast(1)

            val maxDim = max(wPx, hPx)
            val resScale = if (maxDim > 720) 720f / maxDim.toFloat() else 1f
            val bmpW = (wPx * resScale).roundToInt().coerceAtLeast(64)
            val bmpH = (hPx * resScale).roundToInt().coerceAtLeast(64)

            val key = VehicleRenderKey(
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
                steerAngleDeg = item.steerAngleDeg,
                headlightsOn = item.headlightsOn,
                hasRoofRack = item.hasRoofRack,
                hasExhaustStacks = item.hasExhaustStacks,
                hitchTrailer = effectiveHitch,
                trailerId = hitchedTrailer?.id ?: 0,
                showLoadedCargo = effectiveHitch && showLoadedCargo,
                cargoId = loadedCargo?.id ?: 0,
                cargoQty = cargoQuantity
            )

            var bmp = rasterBuffer.bitmap
            if (bmp == null || bmp.width != bmpW || bmp.height != bmpH) {
                bmp = Bitmap.createBitmap(bmpW, bmpH, Bitmap.Config.ARGB_8888)
                rasterBuffer.bitmap = bmp
                rasterBuffer.lastKey = null
            }

            if (rasterBuffer.lastKey != key) {
                val stats = SphereNativeEngine.renderVehicleBitmap(
                    bitmap = bmp,
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
                    steerAngleDeg = key.steerAngleDeg,
                    headlightsOn = key.headlightsOn,
                    hasRoofRack = key.hasRoofRack,
                    hasExhaustStacks = key.hasExhaustStacks,
                    hitchTrailer = key.hitchTrailer,
                    trailerId = key.trailerId,
                    showLoadedCargo = key.showLoadedCargo,
                    cargoId = key.cargoId,
                    cargoQty = key.cargoQty
                )
                rasterBuffer.lastKey = key
                rasterBuffer.stats = stats
                rasterBuffer.imageBitmap = bmp.asImageBitmap()
                if (stats.size >= 4 && (stats[2] != meshStats[2] || stats[3] != meshStats[3])) {
                    meshStats = stats
                }
            }

            val img = rasterBuffer.imageBitmap
            if (img != null) {
                drawImage(
                    image = img,
                    dstOffset = IntOffset.Zero,
                    dstSize = IntSize(wPx, hPx),
                    filterQuality = FilterQuality.Medium
                )
            }
        }

        // Top Floating Action Bar: Telemetry Toggle + Zoom & Reset
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
                    .defaultMinSize(minHeight = 36.dp)
                    .testTag("vehicle_toggle_specs_button")
            ) {
                Icon(Icons.Default.Info, contentDescription = "Toggle Vehicle Telemetry", modifier = Modifier.size(16.dp))
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
                    color = if (effectiveHitch) Color(0xFFFACC15) else Color.White,
                    fontFamily = FontFamily.Monospace,
                    fontSize = 10.sp
                )
            }
        }
    }
}
