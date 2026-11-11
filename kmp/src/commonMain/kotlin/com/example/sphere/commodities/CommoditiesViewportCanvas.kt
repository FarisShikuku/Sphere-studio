package com.example.sphere.commodities

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.CornerRadius
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.*
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.CommodityCategoryType
import com.example.sphere.model.CommodityItem
import com.example.sphere.model.StackingMode
import kotlin.math.*

@Composable
fun CommoditiesViewportCanvas(
    item: CommodityItem,
    stackingMode: StackingMode,
    rotX: Float,
    rotY: Float,
    zoomFactor: Float,
    onDrag: (Float, Float) -> Unit,
    showStraps: Boolean,
    showShrinkwrap: Boolean,
    showPalletBase: Boolean,
    modifier: Modifier = Modifier
) {
    Box(
        modifier = modifier
            .fillMaxSize()
            .background(Color(0xFF07090E))
            .pointerInput(Unit) {
                detectDragGestures { change, dragAmount ->
                    change.consume()
                    onDrag(dragAmount.x, dragAmount.y)
                }
            }
            .testTag("commodities_3d_viewport")
    ) {
        Canvas(modifier = Modifier.fillMaxSize()) {
            val w = size.width
            val h = size.height
            val center = Offset(w / 2f, h / 2f)

            // Studio background radial vignette
            drawRect(
                brush = Brush.radialGradient(
                    colors = listOf(Color(0xFF131A2A), Color(0xFF04060A)),
                    center = center,
                    radius = w * 0.85f
                )
            )

            // Perspective Setup
            val radX = rotX * (PI.toFloat() / 180f)
            val radY = rotY * (PI.toFloat() / 180f)
            val cosY = cos(radY)
            val sinY = sin(radY)
            val cosX = cos(radX)
            val sinX = sin(radX)

            val baseScale = (min(w, h) * 0.28f) * zoomFactor

            // Studio Ground Shadow Disc
            val shadowY = center.y + baseScale * 0.85f
            drawOval(
                brush = Brush.radialGradient(
                    colors = listOf(Color(0xFF010409).copy(alpha = 0.88f), Color(0x00010409)),
                    center = Offset(center.x, shadowY),
                    radius = baseScale * 1.6f
                ),
                topLeft = Offset(center.x - baseScale * 1.6f, shadowY - baseScale * 0.45f),
                size = Size(baseScale * 3.2f, baseScale * 0.9f)
            )

            // Optional Pallet Base
            if (showPalletBase || stackingMode != StackingMode.SINGLE_UNIT) {
                drawProceduralPallet(
                    center = Offset(center.x, shadowY - baseScale * 0.12f),
                    scale = baseScale * 1.15f,
                    cosY = cosY, sinY = sinY, cosX = cosX, sinX = sinX
                )
            }

            // Draw Commodity Shape based on Category and StackingMode
            val totalUnits = stackingMode.multiplier
            val rows = if (totalUnits > 1) 2 else 1
            val cols = if (totalUnits > 1) 2 else 1
            val tiers = if (totalUnits > 1) min(4, totalUnits / 4) else 1

            for (tier in 0 until tiers) {
                val tierOffsetY = -tier * (baseScale * 0.35f)
                for (r in 0 until rows) {
                    for (c in 0 until cols) {
                        val gridOffsetX = (c - (cols - 1) * 0.5f) * (baseScale * 0.45f)
                        val gridOffsetZ = (r - (rows - 1) * 0.5f) * (baseScale * 0.45f)

                        // 3D Rotation of grid position
                        val gx = gridOffsetX * cosY - gridOffsetZ * sinY
                        val gz = gridOffsetX * sinY + gridOffsetZ * cosY
                        val itemPos = Offset(center.x + gx, center.y + tierOffsetY + gz * sinX * 0.5f)

                        drawCommodityShape(
                            item = item,
                            pos = itemPos,
                            scale = baseScale * (if (totalUnits > 1) 0.65f else 1.0f),
                            cosY = cosY, sinY = sinY, cosX = cosX, sinX = sinX
                        )
                    }
                }
            }

            // Strapping / Shrinkwrap Visual Overlays
            if (showShrinkwrap && totalUnits > 1) {
                drawRect(
                    brush = Brush.linearGradient(
                        colors = listOf(Color.White.copy(alpha = 0.12f), Color.White.copy(alpha = 0.04f)),
                        start = Offset(center.x - baseScale, center.y - baseScale * 0.8f),
                        end = Offset(center.x + baseScale, center.y + baseScale * 0.8f)
                    ),
                    topLeft = Offset(center.x - baseScale * 0.85f, center.y - baseScale * 0.75f),
                    size = Size(baseScale * 1.7f, baseScale * 1.4f)
                )
            }

            if (showStraps && totalUnits > 1) {
                // High-tensile yellow polyester strapping lines
                val strapBrush = Brush.linearGradient(listOf(Color(0xFFEAB308), Color(0xFFCA8A04)))
                drawLine(
                    brush = strapBrush,
                    start = Offset(center.x - baseScale * 0.5f, center.y - baseScale * 0.7f),
                    end = Offset(center.x - baseScale * 0.5f, center.y + baseScale * 0.6f),
                    strokeWidth = 6f
                )
                drawLine(
                    brush = strapBrush,
                    start = Offset(center.x + baseScale * 0.5f, center.y - baseScale * 0.7f),
                    end = Offset(center.x + baseScale * 0.5f, center.y + baseScale * 0.6f),
                    strokeWidth = 6f
                )
            }
        }

        // Top-Left Telemetry HUD: Physical Mass, Volume, Density & Stacking
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xDD0D111A)),
            shape = RoundedCornerShape(10.dp),
            border = CardDefaults.outlinedCardBorder().copy(
                brush = Brush.linearGradient(listOf(Color(0xFF1E293B), Color(0xFF334155)))
            ),
            modifier = Modifier
                .align(Alignment.TopStart)
                .padding(10.dp)
        ) {
            Column(modifier = Modifier.padding(10.dp)) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Box(
                        modifier = Modifier
                            .size(8.dp)
                            .background(Color(0xFF22C55E), RoundedCornerShape(4.dp))
                    )
                    Spacer(Modifier.width(6.dp))
                    Text(
                        "PHYSICAL MECHANICS",
                        style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold),
                        color = Color(0xFF38BDF8),
                        letterSpacing = 1.sp
                    )
                }
                Spacer(Modifier.height(4.dp))
                val totalMassKg = item.nominalWeightKg * stackingMode.multiplier
                val totalVolM3 = item.volumeM3 * stackingMode.multiplier
                val density = if (item.volumeM3 > 0.0001f) (item.nominalWeightKg / item.volumeM3).roundToInt() else 625

                Text("• Mass: ${totalMassKg.roundToInt()} kg (${if (totalMassKg >= 1000f) String.format("%.2f t", totalMassKg / 1000f) else "${item.nominalWeightKg} kg/ea"})",
                    style = MaterialTheme.typography.labelSmall, color = Color(0xFFE2E8F0), fontFamily = FontFamily.Monospace)
                Text("• Volume: ${String.format("%.3f", totalVolM3)} m³",
                    style = MaterialTheme.typography.labelSmall, color = Color(0xFF94A3B8), fontFamily = FontFamily.Monospace)
                Text("• Density: $density kg/m³",
                    style = MaterialTheme.typography.labelSmall, color = Color(0xFF94A3B8), fontFamily = FontFamily.Monospace)
                Text("• Dims: ${item.lengthM}m × ${item.widthM}m × ${item.heightM}m",
                    style = MaterialTheme.typography.labelSmall, color = Color(0xFF94A3B8), fontFamily = FontFamily.Monospace)
                Text("• Load: ${stackingMode.label}",
                    style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold), color = Color(0xFFFBBF24), fontFamily = FontFamily.Monospace)
            }
        }

        // Top-Right Telemetry HUD: Vulkan Pipeline, BC7/BC5 Compression & Descriptors
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xDD0D111A)),
            shape = RoundedCornerShape(10.dp),
            border = CardDefaults.outlinedCardBorder().copy(
                brush = Brush.linearGradient(listOf(Color(0xFF1E293B), Color(0xFF334155)))
            ),
            modifier = Modifier
                .align(Alignment.TopEnd)
                .padding(10.dp)
        ) {
            Column(modifier = Modifier.padding(10.dp)) {
                Text(
                    "VULKAN PIPELINE",
                    style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold),
                    color = Color(0xFFF472B6),
                    letterSpacing = 1.sp
                )
                Spacer(Modifier.height(4.dp))
                Text("• Albedo: ${item.vkAlbedoFormat}",
                    style = MaterialTheme.typography.labelSmall, color = Color(0xFFE2E8F0), fontFamily = FontFamily.Monospace, fontSize = 9.sp)
                Text("• Normal: ${item.vkNormalFormat}",
                    style = MaterialTheme.typography.labelSmall, color = Color(0xFF94A3B8), fontFamily = FontFamily.Monospace, fontSize = 9.sp)
                Text("• ORM: ${item.vkOrmFormat}",
                    style = MaterialTheme.typography.labelSmall, color = Color(0xFF94A3B8), fontFamily = FontFamily.Monospace, fontSize = 9.sp)
                Text("• Compute: 1024×1024 (16×16)",
                    style = MaterialTheme.typography.labelSmall, color = Color(0xFF38BDF8), fontFamily = FontFamily.Monospace, fontSize = 9.sp)
                Text("• Mesh: C++20 ICargo::GenerateMesh()",
                    style = MaterialTheme.typography.labelSmall, color = Color(0xFF4ADE80), fontFamily = FontFamily.Monospace, fontSize = 9.sp)
            }
        }

        // Bottom Stencil Branding Badge
        Surface(
            shape = RoundedCornerShape(8.dp),
            color = Color(0xCC0F172A),
            border = CardDefaults.outlinedCardBorder(),
            modifier = Modifier
                .align(Alignment.BottomCenter)
                .padding(bottom = 12.dp)
        ) {
            Row(
                modifier = Modifier.padding(horizontal = 14.dp, vertical = 6.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    "STENCIL LABEL: ",
                    style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold),
                    color = Color(0xFF94A3B8)
                )
                Text(
                    item.defaultLabel,
                    style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold),
                    color = Color.White,
                    fontFamily = FontFamily.Monospace
                )
            }
        }
    }
}

// ---------------------------------------------------------------------------
// 3D Procedural Commodity Render Helpers
// ---------------------------------------------------------------------------

private fun DrawScope.drawProceduralPallet(
    center: Offset,
    scale: Float,
    cosY: Float, sinY: Float, cosX: Float, sinX: Float
) {
    val palletColor = Color(0xFF8D6E46)
    val darkWood = Color(0xFF5A4428)

    // Bottom skids (3 runners)
    for (i in -1..1) {
        val ox = i * (scale * 0.42f)
        drawRect(
            color = darkWood,
            topLeft = Offset(center.x + ox - scale * 0.08f, center.y + scale * 0.15f),
            size = Size(scale * 0.16f, scale * 0.06f)
        )
    }

    // Top deck boards (5 slats)
    for (i in -2..2) {
        val oy = i * (scale * 0.08f)
        drawRoundRect(
            color = palletColor,
            topLeft = Offset(center.x - scale * 0.55f, center.y + oy),
            size = Size(scale * 1.1f, scale * 0.06f),
            cornerRadius = CornerRadius(4f, 4f)
        )
    }
}

private fun DrawScope.drawCommodityShape(
    item: CommodityItem,
    pos: Offset,
    scale: Float,
    cosY: Float, sinY: Float, cosX: Float, sinX: Float
) {
    when (item.category) {
        CommodityCategoryType.BAGGED_GOODS -> {
            drawProceduralBag(item, pos, scale, cosY, sinY, cosX, sinX)
        }
        CommodityCategoryType.BOXES_CRATES -> {
            drawProceduralBox(item, pos, scale, cosY, sinY, cosX, sinX)
        }
        CommodityCategoryType.LIQUID_BULK -> {
            drawProceduralDrum(item, pos, scale, cosY, sinY, cosX, sinX)
        }
        CommodityCategoryType.RAW_MATERIALS -> {
            drawProceduralRaw(item, pos, scale, cosY, sinY, cosX, sinX)
        }
        CommodityCategoryType.CONTAINERS_BOTTLES -> {
            drawProceduralBottle(item, pos, scale, cosY, sinY, cosX, sinX)
        }
        CommodityCategoryType.SPECIALTY_INDUSTRIAL -> {
            drawProceduralSpecialty(item, pos, scale, cosY, sinY, cosX, sinX)
        }
        CommodityCategoryType.PERISHABLE_GOODS -> {
            drawProceduralPerishable(item, pos, scale, cosY, sinY, cosX, sinX)
        }
    }
}

private fun DrawScope.drawProceduralBag(
    item: CommodityItem,
    pos: Offset,
    scale: Float,
    cosY: Float, sinY: Float, cosX: Float, sinX: Float
) {
    val halfW = scale * 0.55f
    val halfH = scale * 0.38f

    // Puffed pillow shape path
    val path = Path().apply {
        moveTo(pos.x - halfW, pos.y)
        cubicTo(
            pos.x - halfW * 0.9f, pos.y - halfH,
            pos.x + halfW * 0.9f, pos.y - halfH,
            pos.x + halfW, pos.y
        )
        cubicTo(
            pos.x + halfW * 0.9f, pos.y + halfH,
            pos.x - halfW * 0.9f, pos.y + halfH,
            pos.x - halfW, pos.y
        )
        close()
    }

    // Cook-Torrance illuminated gradient
    val lightOffset = Offset(pos.x - halfW * 0.3f, pos.y - halfH * 0.5f)
    drawPath(
        path = path,
        brush = Brush.radialGradient(
            colors = listOf(
                item.baseColor,
                item.baseColor.copy(alpha = 0.92f),
                item.baseColor.copy(alpha = 0.65f)
            ),
            center = lightOffset,
            radius = scale * 0.85f
        )
    )

    // Seam stitching edge line
    drawPath(
        path = path,
        color = item.baseColor.copy(alpha = 0.4f),
        style = Stroke(width = 3f)
    )

    // Fabric fold ripples
    for (i in -2..2) {
        val foldY = pos.y + i * (scale * 0.12f)
        drawLine(
            color = Color.Black.copy(alpha = 0.18f),
            start = Offset(pos.x - halfW * 0.7f, foldY),
            end = Offset(pos.x + halfW * 0.7f, foldY + scale * 0.04f),
            strokeWidth = 2.5f
        )
    }

    // Branding Stencil area
    drawRoundRect(
        color = item.secondaryColor.copy(alpha = 0.85f),
        topLeft = Offset(pos.x - scale * 0.30f, pos.y - scale * 0.12f),
        size = Size(scale * 0.60f, scale * 0.24f),
        cornerRadius = CornerRadius(6f, 6f)
    )
}

private fun DrawScope.drawProceduralBox(
    item: CommodityItem,
    pos: Offset,
    scale: Float,
    cosY: Float, sinY: Float, cosX: Float, sinX: Float
) {
    val hw = scale * 0.52f
    val hh = scale * 0.45f
    val depth = scale * 0.28f

    // Isometric / rotated box 3-faces
    val topFace = Path().apply {
        moveTo(pos.x, pos.y - hh - depth)
        lineTo(pos.x + hw, pos.y - hh)
        lineTo(pos.x, pos.y - hh + depth)
        lineTo(pos.x - hw, pos.y - hh)
        close()
    }
    val frontFace = Path().apply {
        moveTo(pos.x - hw, pos.y - hh)
        lineTo(pos.x, pos.y - hh + depth)
        lineTo(pos.x, pos.y + hh + depth)
        lineTo(pos.x - hw, pos.y + hh)
        close()
    }
    val sideFace = Path().apply {
        moveTo(pos.x, pos.y - hh + depth)
        lineTo(pos.x + hw, pos.y - hh)
        lineTo(pos.x + hw, pos.y + hh)
        lineTo(pos.x, pos.y + hh + depth)
        close()
    }

    drawPath(topFace, item.baseColor.copy(alpha = 1.0f))
    drawPath(frontFace, item.baseColor.copy(alpha = 0.85f))
    drawPath(sideFace, item.baseColor.copy(alpha = 0.70f))

    // Seam & tape
    drawLine(
        color = Color(0xFF785532),
        start = Offset(pos.x - hw * 0.1f, pos.y - hh),
        end = Offset(pos.x + hw * 0.1f, pos.y - hh + depth),
        strokeWidth = 10f
    )

    // Outline edges
    val edgeCol = Color.Black.copy(alpha = 0.35f)
    drawPath(topFace, edgeCol, style = Stroke(width = 2f))
    drawPath(frontFace, edgeCol, style = Stroke(width = 2f))
    drawPath(sideFace, edgeCol, style = Stroke(width = 2f))
}

private fun DrawScope.drawProceduralDrum(
    item: CommodityItem,
    pos: Offset,
    scale: Float,
    cosY: Float, sinY: Float, cosX: Float, sinX: Float
) {
    val r = scale * 0.36f
    val halfH = scale * 0.52f

    // Cylindrical drum body
    val body = Path().apply {
        moveTo(pos.x - r, pos.y - halfH)
        lineTo(pos.x + r, pos.y - halfH)
        lineTo(pos.x + r, pos.y + halfH)
        lineTo(pos.x - r, pos.y + halfH)
        close()
    }

    drawPath(
        path = body,
        brush = Brush.linearGradient(
            colors = listOf(item.baseColor.copy(alpha = 0.6f), item.baseColor, item.baseColor.copy(alpha = 0.5f)),
            start = Offset(pos.x - r, pos.y),
            end = Offset(pos.x + r, pos.y)
        )
    )

    // Top chime lid
    drawOval(
        color = item.baseColor.copy(alpha = 0.95f),
        topLeft = Offset(pos.x - r, pos.y - halfH - r * 0.25f),
        size = Size(r * 2f, r * 0.5f)
    )

    // Rolling hoop ribs at 1/3 and 2/3
    val ribY1 = pos.y - halfH * 0.35f
    val ribY2 = pos.y + halfH * 0.35f
    drawLine(Color.White.copy(alpha = 0.45f), Offset(pos.x - r, ribY1), Offset(pos.x + r, ribY1), strokeWidth = 5f)
    drawLine(Color.White.copy(alpha = 0.45f), Offset(pos.x - r, ribY2), Offset(pos.x + r, ribY2), strokeWidth = 5f)

    // Hazard Diamond Placard
    val diamond = Path().apply {
        val dy = pos.y
        moveTo(pos.x, dy - scale * 0.12f)
        lineTo(pos.x + scale * 0.12f, dy)
        lineTo(pos.x, dy + scale * 0.12f)
        lineTo(pos.x - scale * 0.12f, dy)
        close()
    }
    drawPath(diamond, item.secondaryColor)
}

private fun DrawScope.drawProceduralRaw(
    item: CommodityItem,
    pos: Offset,
    scale: Float,
    cosY: Float, sinY: Float, cosX: Float, sinX: Float
) {
    if (item.id == 20 || item.id == 21) {
        // Conical aggregate pile (coal / gravel)
        val pile = Path().apply {
            moveTo(pos.x - scale * 0.65f, pos.y + scale * 0.35f)
            lineTo(pos.x, pos.y - scale * 0.45f)
            lineTo(pos.x + scale * 0.65f, pos.y + scale * 0.35f)
            close()
        }
        drawPath(
            pile,
            brush = Brush.radialGradient(
                colors = listOf(item.baseColor, item.baseColor.copy(alpha = 0.7f)),
                center = Offset(pos.x, pos.y - scale * 0.2f),
                radius = scale * 0.6f
            )
        )
    } else {
        // Tree Log or Timber bundle
        val r = scale * 0.22f
        val len = scale * 0.85f

        // Bark trunk cylinder
        drawRoundRect(
            brush = Brush.linearGradient(
                colors = listOf(item.baseColor.copy(alpha = 0.7f), item.baseColor, item.baseColor.copy(alpha = 0.6f)),
                start = Offset(pos.x, pos.y - r),
                end = Offset(pos.x, pos.y + r)
            ),
            topLeft = Offset(pos.x - len * 0.5f, pos.y - r),
            size = Size(len, r * 2f),
            cornerRadius = CornerRadius(6f, 6f)
        )

        // Sawn end sapwood rings
        drawOval(
            color = item.secondaryColor,
            topLeft = Offset(pos.x + len * 0.5f - r * 0.3f, pos.y - r),
            size = Size(r * 0.6f, r * 2f)
        )
    }
}

private fun DrawScope.drawProceduralBottle(
    item: CommodityItem,
    pos: Offset,
    scale: Float,
    cosY: Float, sinY: Float, cosX: Float, sinX: Float
) {
    val r = scale * 0.18f
    val halfH = scale * 0.55f

    // Bottle body
    val bottle = Path().apply {
        moveTo(pos.x - r, pos.y + halfH)
        lineTo(pos.x - r, pos.y - halfH * 0.2f)
        lineTo(pos.x - r * 0.4f, pos.y - halfH * 0.65f)
        lineTo(pos.x - r * 0.4f, pos.y - halfH)
        lineTo(pos.x + r * 0.4f, pos.y - halfH)
        lineTo(pos.x + r * 0.4f, pos.y - halfH * 0.65f)
        lineTo(pos.x + r, pos.y - halfH * 0.2f)
        lineTo(pos.x + r, pos.y + halfH)
        close()
    }

    drawPath(
        path = bottle,
        brush = Brush.linearGradient(
            colors = listOf(
                item.baseColor.copy(alpha = item.opacity * 0.6f),
                item.baseColor.copy(alpha = item.opacity),
                item.baseColor.copy(alpha = item.opacity * 0.5f)
            ),
            start = Offset(pos.x - r, pos.y),
            end = Offset(pos.x + r, pos.y)
        )
    )

    // Body Label
    drawRect(
        color = item.secondaryColor,
        topLeft = Offset(pos.x - r * 0.9f, pos.y - halfH * 0.1f),
        size = Size(r * 1.8f, halfH * 0.5f)
    )

    // Crown Cap
    drawRect(
        color = Color(0xFFE5E7EB),
        topLeft = Offset(pos.x - r * 0.45f, pos.y - halfH - 4f),
        size = Size(r * 0.9f, 8f)
    )
}

private fun DrawScope.drawProceduralSpecialty(
    item: CommodityItem,
    pos: Offset,
    scale: Float,
    cosY: Float, sinY: Float, cosX: Float, sinX: Float
) {
    if (item.id == 30) {
        // Tyre stack (4 stacked toruses)
        val tyreR = scale * 0.42f
        for (i in 0..3) {
            val ty = pos.y + (i - 1.5f) * (scale * 0.22f)
            drawOval(
                color = item.baseColor,
                topLeft = Offset(pos.x - tyreR, ty - scale * 0.08f),
                size = Size(tyreR * 2f, scale * 0.16f)
            )
            // Inner rim hole
            drawOval(
                color = Color(0xFF090A0F),
                topLeft = Offset(pos.x - tyreR * 0.5f, ty - scale * 0.04f),
                size = Size(tyreR, scale * 0.08f)
            )
        }
    } else {
        // Steel pipe or brick pallet
        drawProceduralBox(item, pos, scale, cosY, sinY, cosX, sinX)
    }
}

private fun DrawScope.drawProceduralPerishable(
    item: CommodityItem,
    pos: Offset,
    scale: Float,
    cosY: Float, sinY: Float, cosX: Float, sinX: Float
) {
    // Plastic crate container
    val hw = scale * 0.52f
    val hh = scale * 0.32f
    drawRect(
        color = Color(0xFF1E293B),
        topLeft = Offset(pos.x - hw, pos.y - hh * 0.5f),
        size = Size(hw * 2f, hh)
    )

    // Clustered fresh spherical produce
    val produceR = scale * 0.09f
    for (ix in -2..2) {
        for (iz in -1..1) {
            val px = pos.x + ix * (produceR * 1.8f)
            val py = pos.y - hh * 0.5f + iz * (produceR * 1.2f)
            drawCircle(
                brush = Brush.radialGradient(
                    colors = listOf(Color.White.copy(alpha = 0.5f), item.baseColor),
                    center = Offset(px - produceR * 0.3f, py - produceR * 0.3f),
                    radius = produceR
                ),
                radius = produceR,
                center = Offset(px, py)
            )
        }
    }
}
