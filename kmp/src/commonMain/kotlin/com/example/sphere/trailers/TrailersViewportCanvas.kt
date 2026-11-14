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

private class NativeTrailerRasterBuffer {
    var bitmap: Bitmap? = null
    var imageBitmap: ImageBitmap? = null
    var lastKey: TrailerRenderKey? = null
    var stats: IntArray = intArrayOf(0, 0)
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
    modifier: Modifier = Modifier
) {
    var showTelemetryInfo by remember { mutableStateOf(false) }
    var meshStats by remember { mutableStateOf(intArrayOf(0, 0)) }
    val rasterBuffer = remember { NativeTrailerRasterBuffer() }

    val payloadMassKg = if (showLoadedCargo && loadedCargo != null) {
        loadedCargo.nominalWeightKg * cargoQuantity
    } else 0f

    Box(
        modifier = modifier
            .fillMaxSize()
            .background(Color(0xFF070A10))
            .pointerInput(Unit) {
                detectTransformGestures { _, pan, zoom, _ ->
                    onDrag(pan.x, pan.y)
                    if (zoom != 1.0f) {
                        onZoomChange((zoomFactor * zoom).coerceIn(0.55f, 2.6f))
                    }
                }
            }
            .testTag("trailers_3d_viewport")
    ) {
        Canvas(modifier = Modifier.fillMaxSize()) {
            val wPx = size.width.roundToInt().coerceAtLeast(1)
            val hPx = size.height.roundToInt().coerceAtLeast(1)

            val maxDim = max(wPx, hPx)
            val resScale = if (maxDim > 720) 720f / maxDim.toFloat() else 1f
            val bmpW = (wPx * resScale).roundToInt().coerceAtLeast(64)
            val bmpH = (hPx * resScale).roundToInt().coerceAtLeast(64)

            val key = TrailerRenderKey(
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

            var bmp = rasterBuffer.bitmap
            if (bmp == null || bmp.width != bmpW || bmp.height != bmpH) {
                bmp = Bitmap.createBitmap(bmpW, bmpH, Bitmap.Config.ARGB_8888)
                rasterBuffer.bitmap = bmp
                rasterBuffer.lastKey = null
            }

            if (rasterBuffer.lastKey != key) {
                val stats = SphereNativeEngine.renderTrailerBitmap(
                    bitmap = bmp,
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
                rasterBuffer.lastKey = key
                rasterBuffer.stats = stats
                rasterBuffer.imageBitmap = bmp.asImageBitmap()
                if (stats[0] != meshStats[0] || stats[1] != meshStats[1]) {
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
