package com.example.sphere.text

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.geometry.CornerRadius
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.*
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.*
import kotlin.math.*

/**
 * GlyphCoreViewportCanvas - Interactive 3D and 2D Vulkan Typography Viewport.
 * Supports:
 *  - Architectural Wall text mounting (Red Brick, Cast Concrete, Dark Ashlar Stone, Cedar Timber)
 *  - Street Signpost mounting (Steel cylinder, Square timber, Cast iron, Highway gantry)
 *  - 3D World Floating Billboard and 2D HUD textoverlay
 *  - Signed Distance Field (SDF) multi-pass rendering (Drop Shadow, Stroke Outline, Crisp Core, Emissive Glow)
 *  - Real-time Font Atlas texture inspector and typographic metric guides
 */
@Composable
fun GlyphCoreViewportCanvas(
    text: String,
    font: FontTypeface,
    renderMode: TextRenderMode,
    alignment: TextAlignment,
    fontSize: Float,
    letterSpacing: Float,
    lineHeight: Float,
    textColor: Color,
    enableOutline: Boolean,
    outlineWidth: Float,
    outlineColor: Color,
    enableShadow: Boolean,
    shadowOffsetX: Float,
    shadowOffsetY: Float,
    shadowSoftness: Float,
    shadowColor: Color,
    smoothing: Float,
    thicknessBias: Float,
    enableGlow: Boolean,
    glowRadius: Float,
    glowColor: Color,
    badgeStyle: TextBadgeStyle,
    showAtlasView: Boolean,
    showMetrics: Boolean,
    rotX: Float,
    rotY: Float,
    zoomFactor: Float,
    onDrag: (Float, Float) -> Unit,
    mountTarget: TextMountTarget = TextMountTarget.WALL,
    wallMaterial: WallMaterialType = WallMaterialType.BRICK_RED,
    postStyle: PostStyleType = PostStyleType.STEEL_CYLINDER,
    textApplicationStyle: TextApplicationStyle = TextApplicationStyle.MOUNTED_PLAQUE,
    mountHeightOffset: Float = 0.0f,
    wallWidthMeters: Float = 3.2f,
    wallHeightMeters: Float = 2.2f,
    postHeightMeters: Float = 2.8f,
    modifier: Modifier = Modifier
) {
    val textMeasurer = rememberTextMeasurer()

    Box(
        modifier = modifier
            .fillMaxSize()
            .background(Color(0xFF0A0C10))
            .pointerInput(Unit) {
                detectDragGestures { change, dragAmount ->
                    change.consume()
                    onDrag(dragAmount.x, dragAmount.y)
                }
            }
    ) {
        Canvas(modifier = Modifier.fillMaxSize().testTag("glyphcore_viewport_canvas")) {
            val width = size.width
            val height = size.height
            val centerX = width / 2f
            val centerY = height / 2f

            if (showAtlasView) {
                // RENDER FONT ATLAS TEXTURE INSPECTOR
                drawFontAtlasInspector(
                    centerX = centerX,
                    centerY = centerY,
                    size = min(width, height) * 0.88f,
                    font = font,
                    textMeasurer = textMeasurer
                )
            } else {
                // 1. RENDER 3D SCENE (GROUND STAGE, WALL OR POST MOUNT)
                val targetAnchor = drawPerspectiveScene(
                    width = width,
                    height = height,
                    rotX = rotX,
                    rotY = rotY,
                    renderMode = renderMode,
                    mountTarget = mountTarget,
                    wallMaterial = wallMaterial,
                    postStyle = postStyle,
                    textApplicationStyle = textApplicationStyle,
                    mountHeightOffset = mountHeightOffset,
                    wallWidthMeters = wallWidthMeters,
                    wallHeightMeters = wallHeightMeters,
                    postHeightMeters = postHeightMeters,
                    zoomFactor = zoomFactor
                )

                // 2. RENDER VULKAN TYPOGRAPHY WITH SDF SIMULATION ONTO THE CHOSEN MOUNT
                drawVulkanTypography(
                    text = text,
                    font = font,
                    renderMode = renderMode,
                    alignment = alignment,
                    fontSize = fontSize * zoomFactor,
                    letterSpacing = letterSpacing,
                    lineHeight = lineHeight,
                    textColor = textColor,
                    enableOutline = enableOutline,
                    outlineWidth = outlineWidth,
                    outlineColor = outlineColor,
                    enableShadow = enableShadow,
                    shadowOffsetX = shadowOffsetX * zoomFactor,
                    shadowOffsetY = shadowOffsetY * zoomFactor,
                    shadowSoftness = shadowSoftness,
                    shadowColor = shadowColor,
                    smoothing = smoothing,
                    thicknessBias = thicknessBias,
                    enableGlow = enableGlow,
                    glowRadius = glowRadius,
                    glowColor = glowColor,
                    badgeStyle = badgeStyle,
                    showMetrics = showMetrics,
                    rotX = rotX,
                    rotY = rotY,
                    zoomFactor = zoomFactor,
                    mountTarget = mountTarget,
                    textApplicationStyle = textApplicationStyle,
                    targetAnchor = targetAnchor,
                    textMeasurer = textMeasurer
                )
            }
        }

        // Live Vulkan Pipeline Telemetry Watermark (Top Left)
        Column(
            modifier = Modifier
                .align(Alignment.TopStart)
                .padding(12.dp)
                .background(Color(0xCC080B10), RoundedCornerShape(8.dp))
                .padding(horizontal = 10.dp, vertical = 6.dp)
        ) {
            Row(verticalAlignment = Alignment.CenterVertically) {
                Box(
                    modifier = Modifier
                        .size(8.dp)
                        .clip(RoundedCornerShape(4.dp))
                        .background(Color(0xFF2BF69A))
                )
                Spacer(Modifier.width(6.dp))
                Text(
                    "VULKAN SDF TEXT // ${mountTarget.name}",
                    style = MaterialTheme.typography.labelSmall.copy(
                        color = Color(0xFF2BF69A),
                        fontWeight = FontWeight.Bold,
                        fontFamily = androidx.compose.ui.text.font.FontFamily.Monospace,
                        fontSize = 10.sp
                    )
                )
            }
            val charCount = text.length
            val quadCount = text.count { !it.isWhitespace() }
            val vertexCount = quadCount * 6
            val triCount = quadCount * 2
            val stagingBytes = vertexCount * 36 // 36 bytes per vertex

            Text(
                "Quads: $quadCount | Vertices: $vertexCount | Tris: $triCount",
                style = MaterialTheme.typography.bodySmall.copy(
                    color = Color(0xFFC0CAD8),
                    fontFamily = androidx.compose.ui.text.font.FontFamily.Monospace,
                    fontSize = 10.sp
                )
            )
            Text(
                "Mount: ${mountTarget.shortName} | Style: ${textApplicationStyle.label.take(16)} | Zoom: ${"%.1f".format(zoomFactor)}x",
                style = MaterialTheme.typography.bodySmall.copy(
                    color = Color(0xFF8895A8),
                    fontFamily = androidx.compose.ui.text.font.FontFamily.Monospace,
                    fontSize = 10.sp
                )
            )
        }

        // View Mode Indicator Badge (Top Right)
        Surface(
            color = Color(0xDD121722),
            shape = RoundedCornerShape(6.dp),
            modifier = Modifier
                .align(Alignment.TopEnd)
                .padding(12.dp)
        ) {
            Row(
                modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Icon(
                    when (mountTarget) {
                        TextMountTarget.WALL -> Icons.Default.Layers
                        TextMountTarget.POST -> Icons.Default.VerticalAlignTop
                        TextMountTarget.FLOATING -> Icons.Default.ViewInAr
                        TextMountTarget.HUD -> Icons.Default.Screenshot
                    },
                    contentDescription = null,
                    tint = if (showAtlasView) Color(0xFFFFA726) else Color(0xFF64B5F6),
                    modifier = Modifier.size(14.dp)
                )
                Spacer(Modifier.width(4.dp))
                Text(
                    if (showAtlasView) "ATLAS VIEW (1024x1024 SDF)" else "MOUNT: ${mountTarget.shortName.uppercase()}",
                    style = MaterialTheme.typography.labelSmall.copy(
                        fontWeight = FontWeight.Bold,
                        color = Color.White,
                        fontSize = 9.sp
                    )
                )
            }
        }

        com.example.sphere.viewport.SceneFpsBadge(
            tag = "glyphcore_scene_fps_badge",
            modifier = Modifier
                .align(Alignment.TopEnd)
                .padding(top = 42.dp, end = 12.dp)
        )
    }
}

// -------------------------------------------------------------
// 3D SCENE BACKGROUND, GROUND STAGE, WALL & POST RENDERING
// -------------------------------------------------------------
private fun DrawScope.drawPerspectiveScene(
    width: Float,
    height: Float,
    rotX: Float,
    rotY: Float,
    renderMode: TextRenderMode,
    mountTarget: TextMountTarget,
    wallMaterial: WallMaterialType,
    postStyle: PostStyleType,
    textApplicationStyle: TextApplicationStyle,
    mountHeightOffset: Float,
    wallWidthMeters: Float,
    wallHeightMeters: Float,
    postHeightMeters: Float,
    zoomFactor: Float
): Offset {
    val horizonY = height * 0.52f + (rotX * 1.5f).coerceIn(-100f, 100f)

    // 1. Sky background
    drawRect(
        color = Color(0xFF0E131C),
        topLeft = Offset(0f, 0f),
        size = Size(width, horizonY)
    )

    // Horizon line
    drawLine(
        color = Color(0xFF1E2838),
        start = Offset(0f, horizonY),
        end = Offset(width, horizonY),
        strokeWidth = 1.5f
    )

    // 2. Ground plane with perspective grid
    drawRect(
        color = Color(0xFF090C12),
        topLeft = Offset(0f, horizonY),
        size = Size(width, height - horizonY)
    )

    // Perspective floor lines radiating from vanishing point
    val vanishingX = width * 0.5f - (rotY * 2.0f)
    val gridColor = Color(0x223A4E6C)

    val lineCount = 14
    for (i in -lineCount..lineCount) {
        val groundX = width * 0.5f + i * (width / 16f)
        drawLine(
            color = gridColor,
            start = Offset(vanishingX, horizonY),
            end = Offset(groundX * 1.6f - (width * 0.3f), height),
            strokeWidth = 1.0f
        )
    }

    // Horizontal depth grid rings
    var yStep = horizonY + 12f
    var gap = 12f
    while (yStep < height) {
        drawLine(
            color = gridColor,
            start = Offset(0f, yStep),
            end = Offset(width, yStep),
            strokeWidth = 1.0f
        )
        gap *= 1.42f
        yStep += gap
    }

    // 3. Stage Platform (for 3D modes)
    val stageRadius = min(width, height) * 0.32f
    val stageCenter = Offset(width * 0.5f, horizonY + 75f)

    if (mountTarget != TextMountTarget.HUD && renderMode != TextRenderMode.TEXT_OVERLAY_2D) {
        // Stage shadow
        drawOval(
            color = Color(0x66000000),
            topLeft = Offset(stageCenter.x - stageRadius * 1.05f, stageCenter.y - stageRadius * 0.22f + 14f),
            size = Size(stageRadius * 2.1f, stageRadius * 0.44f)
        )

        // Stage disc
        drawOval(
            color = Color(0xFF161E2C),
            topLeft = Offset(stageCenter.x - stageRadius, stageCenter.y - stageRadius * 0.22f),
            size = Size(stageRadius * 2.0f, stageRadius * 0.44f)
        )
        drawOval(
            color = Color(0xFF2A3D58),
            topLeft = Offset(stageCenter.x - stageRadius, stageCenter.y - stageRadius * 0.22f),
            size = Size(stageRadius * 2.0f, stageRadius * 0.44f),
            style = Stroke(width = 2.0f)
        )

        // Coordinate cross
        drawLine(
            color = Color(0xFF3890F0),
            start = Offset(stageCenter.x - 18f, stageCenter.y),
            end = Offset(stageCenter.x + 18f, stageCenter.y),
            strokeWidth = 2.0f
        )
        drawLine(
            color = Color(0xFFE04030),
            start = Offset(stageCenter.x, stageCenter.y - 10f),
            end = Offset(stageCenter.x, stageCenter.y + 10f),
            strokeWidth = 2.0f
        )
    }

    // 4. RENDER WALL OR POST BASED ON MOUNT TARGET
    return when (mountTarget) {
        TextMountTarget.WALL -> {
            drawArchitecturalWall(
                stageCenter = stageCenter,
                wallMaterial = wallMaterial,
                textApplicationStyle = textApplicationStyle,
                wallWidthMeters = wallWidthMeters,
                wallHeightMeters = wallHeightMeters,
                mountHeightOffset = mountHeightOffset,
                rotX = rotX,
                rotY = rotY,
                zoomFactor = zoomFactor,
                screenWidth = width,
                screenHeight = height
            )
        }
        TextMountTarget.POST -> {
            drawStreetSignpost(
                stageCenter = stageCenter,
                postStyle = postStyle,
                textApplicationStyle = textApplicationStyle,
                postHeightMeters = postHeightMeters,
                mountHeightOffset = mountHeightOffset,
                rotX = rotX,
                rotY = rotY,
                zoomFactor = zoomFactor,
                screenWidth = width,
                screenHeight = height
            )
        }
        TextMountTarget.FLOATING -> {
            // 3D Floating billboard anchor above stage
            Offset(stageCenter.x - (rotY * 0.7f), horizonY - 45f + (rotX * 0.4f))
        }
        TextMountTarget.HUD -> {
            // 2D HUD screen center anchor
            Offset(width * 0.5f, height * 0.42f)
        }
    }
}

// -------------------------------------------------------------
// ARCHITECTURAL WALL RENDERING
// -------------------------------------------------------------
private fun DrawScope.drawArchitecturalWall(
    stageCenter: Offset,
    wallMaterial: WallMaterialType,
    textApplicationStyle: TextApplicationStyle,
    wallWidthMeters: Float,
    wallHeightMeters: Float,
    mountHeightOffset: Float,
    rotX: Float,
    rotY: Float,
    zoomFactor: Float,
    screenWidth: Float,
    screenHeight: Float
): Offset {
    val wallBaseY = stageCenter.y + 8f
    val baseWallW = (screenWidth * 0.65f * zoomFactor * (wallWidthMeters / 3.2f)).coerceIn(240f, screenWidth * 0.94f)
    val baseWallH = (screenHeight * 0.38f * zoomFactor * (wallHeightMeters / 2.2f)).coerceIn(160f, screenHeight * 0.72f)
    val wallTopY = wallBaseY - baseWallH
    val wallCenterX = stageCenter.x - (rotY * 0.7f)
    val wallLeft = wallCenterX - baseWallW * 0.5f
    val wallRight = wallLeft + baseWallW

    // 1. Wall Floor Contact Shadow
    drawOval(
        color = Color(0x77000000),
        topLeft = Offset(wallLeft - 18f, wallBaseY - 14f),
        size = Size(baseWallW + 36f, 32f)
    )

    // 2. Concrete Base Foundation Curb
    val curbH = 14f * zoomFactor
    drawRect(
        color = Color(0xFF38404E),
        topLeft = Offset(wallLeft - 6f, wallBaseY - curbH),
        size = Size(baseWallW + 12f, curbH)
    )
    drawLine(
        color = Color(0xFF58667C),
        start = Offset(wallLeft - 6f, wallBaseY - curbH),
        end = Offset(wallRight + 6f, wallBaseY - curbH),
        strokeWidth = 2.0f
    )

    // 3. 3D Side Thickness Face (Perspective depth based on rotY)
    val depthX = rotY * 0.35f
    if (abs(depthX) > 2f) {
        val sidePath = Path()
        if (depthX > 0) {
            // Looking from right, left side visible
            sidePath.moveTo(wallLeft, wallTopY)
            sidePath.lineTo(wallLeft - depthX, wallTopY + 8f)
            sidePath.lineTo(wallLeft - depthX, wallBaseY - curbH + 8f)
            sidePath.lineTo(wallLeft, wallBaseY - curbH)
            sidePath.close()
            drawPath(sidePath, color = wallMaterial.primaryColor.copy(alpha = 0.65f))
            drawPath(sidePath, color = Color(0x66000000))
        } else {
            // Looking from left, right side visible
            sidePath.moveTo(wallRight, wallTopY)
            sidePath.lineTo(wallRight - depthX, wallTopY + 8f)
            sidePath.lineTo(wallRight - depthX, wallBaseY - curbH + 8f)
            sidePath.lineTo(wallRight, wallBaseY - curbH)
            sidePath.close()
            drawPath(sidePath, color = wallMaterial.primaryColor.copy(alpha = 0.65f))
            drawPath(sidePath, color = Color(0x66000000))
        }
    }

    // 4. Front Wall Facade Fill
    val facadeRect = Size(baseWallW, baseWallH - curbH)
    val facadeTopLeft = Offset(wallLeft, wallTopY)
    drawRect(
        color = wallMaterial.primaryColor,
        topLeft = facadeTopLeft,
        size = facadeRect
    )

    // 5. Material Specific Architectural Facade Patterns
    when (wallMaterial) {
        WallMaterialType.BRICK_RED -> {
            // Red clay bricks with stretcher bond courses
            val courseCount = 14
            val courseH = facadeRect.height / courseCount
            val brickW = courseH * 2.5f

            for (c in 0..courseCount) {
                val cy = wallTopY + c * courseH
                // Horizontal mortar line
                drawLine(
                    color = wallMaterial.mortarColor,
                    start = Offset(wallLeft, cy),
                    end = Offset(wallRight, cy),
                    strokeWidth = 1.8f
                )
                // Staggered vertical joints
                val isStaggered = c % 2 == 1
                val startX = if (isStaggered) wallLeft + brickW * 0.5f else wallLeft
                var bx = startX
                while (bx < wallRight) {
                    drawLine(
                        color = wallMaterial.mortarColor,
                        start = Offset(bx, cy),
                        end = Offset(bx, cy + courseH),
                        strokeWidth = 1.5f
                    )
                    // Subtle brick highlight
                    if ((c + (bx / brickW).toInt()) % 3 == 0) {
                        drawRect(
                            color = wallMaterial.accentColor.copy(alpha = 0.25f),
                            topLeft = Offset(bx + 2f, cy + 2f),
                            size = Size(brickW - 4f, courseH - 4f)
                        )
                    }
                    bx += brickW
                }
            }
        }
        WallMaterialType.CONCRETE_MODERN -> {
            // Formwork architectural concrete with tie-holes
            val cols = 4
            val rows = 3
            val colW = facadeRect.width / cols
            val rowH = facadeRect.height / rows

            // Expansion seams
            for (r in 1 until rows) {
                val ry = wallTopY + r * rowH
                drawLine(Color(0xFF2B323D), Offset(wallLeft, ry), Offset(wallRight, ry), 2.5f)
                drawLine(Color(0xFF758296), Offset(wallLeft, ry + 1.5f), Offset(wallRight, ry + 1.5f), 1.0f)
            }
            for (c in 1 until cols) {
                val cx = wallLeft + c * colW
                drawLine(Color(0xFF2B323D), Offset(cx, wallTopY), Offset(cx, wallBaseY - curbH), 2.5f)
                drawLine(Color(0xFF758296), Offset(cx + 1.5f, wallTopY), Offset(cx + 1.5f, wallBaseY - curbH), 1.0f)
            }
            // Tie-rod circular indentations
            for (r in 0 until rows) {
                for (c in 0 until cols) {
                    val tieX = wallLeft + c * colW + 16f
                    val tieY = wallTopY + r * rowH + 16f
                    drawCircle(Color(0xFF202630), 4f * zoomFactor, Offset(tieX, tieY))
                    drawCircle(Color(0xFF6E7A8C), 2f * zoomFactor, Offset(tieX, tieY))
                }
            }
        }
        WallMaterialType.DARK_STONE -> {
            // Ashlar large stone blocks
            val rows = 6
            val rowH = facadeRect.height / rows
            val blockW = rowH * 2.2f
            for (r in 0..rows) {
                val ry = wallTopY + r * rowH
                drawLine(wallMaterial.mortarColor, Offset(wallLeft, ry), Offset(wallRight, ry), 2.5f)
                val isStaggered = r % 2 == 1
                var bx = if (isStaggered) wallLeft + blockW * 0.5f else wallLeft
                while (bx < wallRight) {
                    drawLine(wallMaterial.mortarColor, Offset(bx, ry), Offset(bx, ry + rowH), 2.5f)
                    bx += blockW
                }
            }
        }
        WallMaterialType.TIMBER_PLANK -> {
            // Horizontal cedar siding planks
            val planks = 16
            val plankH = facadeRect.height / planks
            for (p in 0..planks) {
                val py = wallTopY + p * plankH
                drawLine(wallMaterial.mortarColor, Offset(wallLeft, py), Offset(wallRight, py), 2.0f)
                // Wood grain stripe
                drawLine(wallMaterial.accentColor.copy(alpha = 0.35f), Offset(wallLeft, py + plankH * 0.45f), Offset(wallRight, py + plankH * 0.45f), 1.0f)
            }
        }
    }

    // 6. Top Stone Coping Capstone (Overhang with drop shadow)
    val capstoneH = 16f * zoomFactor
    val capstoneOverhang = 12f * zoomFactor
    // Under-capstone drop shadow
    drawRect(
        color = Color(0x77000000),
        topLeft = Offset(wallLeft - capstoneOverhang, wallTopY),
        size = Size(baseWallW + capstoneOverhang * 2f, 8f)
    )
    // Capstone slab
    drawRect(
        color = Color(0xFFB0BAC8),
        topLeft = Offset(wallLeft - capstoneOverhang, wallTopY - capstoneH),
        size = Size(baseWallW + capstoneOverhang * 2f, capstoneH)
    )
    // Bevel highlight
    drawLine(
        color = Color(0xFFE2EAF4),
        start = Offset(wallLeft - capstoneOverhang, wallTopY - capstoneH),
        end = Offset(wallRight + capstoneOverhang, wallTopY - capstoneH),
        strokeWidth = 2.0f
    )

    // 7. Text Anchor Center
    val textAnchorY = (wallTopY + (wallBaseY - curbH)) * 0.5f + (mountHeightOffset * 90f * zoomFactor)
    val textAnchorX = wallCenterX

    // 8. If Signboard Plaque is chosen, draw the mounting plate behind the text anchor
    if (textApplicationStyle == TextApplicationStyle.MOUNTED_PLAQUE) {
        val plaqueW = (baseWallW * 0.70f).coerceIn(180f, baseWallW - 20f)
        val plaqueH = (baseWallH * 0.48f).coerceIn(100f, baseWallH - 24f)
        val plaqueLeft = textAnchorX - plaqueW * 0.5f
        val plaqueTop = textAnchorY - plaqueH * 0.5f

        // Plaque Drop Shadow onto wall
        drawRoundRect(
            color = Color(0x88000000),
            topLeft = Offset(plaqueLeft + 4f, plaqueTop + 6f),
            size = Size(plaqueW, plaqueH),
            cornerRadius = CornerRadius(8f, 8f)
        )
        // Main Plaque Plate (Dark architectural bronze/slate)
        drawRoundRect(
            color = Color(0xF2121720),
            topLeft = Offset(plaqueLeft, plaqueTop),
            size = Size(plaqueW, plaqueH),
            cornerRadius = CornerRadius(8f, 8f)
        )
        // Brushed Brass / Chrome Border
        drawRoundRect(
            color = Color(0xFFD4AF37),
            topLeft = Offset(plaqueLeft, plaqueTop),
            size = Size(plaqueW, plaqueH),
            cornerRadius = CornerRadius(8f, 8f),
            style = Stroke(width = 2.5f)
        )
        // 4 Corner Fixing Bolts with metallic specular dots
        val boltMargin = 12f * zoomFactor
        val boltRadius = 4f * zoomFactor
        val boltPositions = listOf(
            Offset(plaqueLeft + boltMargin, plaqueTop + boltMargin),
            Offset(plaqueLeft + plaqueW - boltMargin, plaqueTop + boltMargin),
            Offset(plaqueLeft + boltMargin, plaqueTop + plaqueH - boltMargin),
            Offset(plaqueLeft + plaqueW - boltMargin, plaqueTop + plaqueH - boltMargin)
        )
        for (b in boltPositions) {
            drawCircle(Color(0xFF22262E), boltRadius, b)
            drawCircle(Color(0xFFF0E0A0), boltRadius * 0.6f, b)
            drawCircle(Color(0xFFFFFFFF), boltRadius * 0.25f, Offset(b.x - 1f, b.y - 1f))
        }
    }

    return Offset(textAnchorX, textAnchorY)
}

// -------------------------------------------------------------
// STREET SIGNPOST & PILLAR RENDERING
// -------------------------------------------------------------
private fun DrawScope.drawStreetSignpost(
    stageCenter: Offset,
    postStyle: PostStyleType,
    textApplicationStyle: TextApplicationStyle,
    postHeightMeters: Float,
    mountHeightOffset: Float,
    rotX: Float,
    rotY: Float,
    zoomFactor: Float,
    screenWidth: Float,
    screenHeight: Float
): Offset {
    val postBaseY = stageCenter.y + 10f
    val postH = (screenHeight * 0.48f * zoomFactor * (postHeightMeters / 2.8f)).coerceIn(200f, screenHeight * 0.82f)
    val postTopY = postBaseY - postH
    val postCenterX = stageCenter.x - (rotY * 0.6f)
    val poleW = 18f * zoomFactor

    // 1. Pole Ground Shadow
    drawOval(
        color = Color(0x66000000),
        topLeft = Offset(postCenterX - 24f, postBaseY - 10f),
        size = Size(48f, 20f)
    )

    // 2. Baseplate Flange with Anchor Bolts
    drawOval(
        color = Color(0xFF262E3B),
        topLeft = Offset(postCenterX - 22f * zoomFactor, postBaseY - 8f * zoomFactor),
        size = Size(44f * zoomFactor, 16f * zoomFactor)
    )
    drawOval(
        color = Color(0xFF4A5568),
        topLeft = Offset(postCenterX - 22f * zoomFactor, postBaseY - 8f * zoomFactor),
        size = Size(44f * zoomFactor, 16f * zoomFactor),
        style = Stroke(width = 1.5f)
    )
    // 4 Anchor bolt dots
    val boltXOffsets = listOf(-14f, 14f, -7f, 7f)
    for (bx in boltXOffsets) {
        drawCircle(Color(0xFFB0BAC8), 2.5f * zoomFactor, Offset(postCenterX + bx * zoomFactor, postBaseY - 2f))
    }

    // 3. Vertical Post Shaft
    val shaftTopLeft = Offset(postCenterX - poleW * 0.5f, postTopY)
    val shaftSize = Size(poleW, postH)

    when (postStyle) {
        PostStyleType.STEEL_CYLINDER -> {
            // Cylindrical steel post with specular highlight
            drawRect(color = postStyle.poleColor, topLeft = shaftTopLeft, size = shaftSize)
            // Left shadow edge
            drawRect(color = Color(0x55000000), topLeft = shaftTopLeft, size = Size(poleW * 0.28f, postH))
            // Center specular highlight stripe
            drawRect(color = Color(0x55FFFFFF), topLeft = Offset(postCenterX - poleW * 0.1f, postTopY), size = Size(poleW * 0.25f, postH))
        }
        PostStyleType.TIMBER_SQUARE -> {
            // Square cedar post
            drawRect(color = postStyle.poleColor, topLeft = shaftTopLeft, size = shaftSize)
            // Wood grain vertical lines
            drawLine(Color(0x442C1B0D), Offset(postCenterX - 3f, postTopY), Offset(postCenterX - 3f, postBaseY), 1.5f)
            drawLine(Color(0x33FFFFFF), Offset(postCenterX + 4f, postTopY), Offset(postCenterX + 4f, postBaseY), 1.0f)
        }
        PostStyleType.CAST_IRON_BLACK -> {
            // Victorian cast iron post with fluted ridges and base flare
            drawRect(color = postStyle.poleColor, topLeft = shaftTopLeft, size = shaftSize)
            // Fluted vertical grooves
            drawLine(Color(0x88000000), Offset(postCenterX - 4f, postTopY), Offset(postCenterX - 4f, postBaseY), 2.0f)
            drawLine(Color(0x88000000), Offset(postCenterX + 4f, postTopY), Offset(postCenterX + 4f, postBaseY), 2.0f)
            // Decorative ring collar
            val collarY = postTopY + postH * 0.45f
            drawRoundRect(color = Color(0xFF384252), topLeft = Offset(postCenterX - poleW * 0.8f, collarY), size = Size(poleW * 1.6f, 10f * zoomFactor), cornerRadius = CornerRadius(4f, 4f))
        }
        PostStyleType.HIGHWAY_GANTRY -> {
            // Industrial post with yellow/black hazard bands
            drawRect(color = postStyle.poleColor, topLeft = shaftTopLeft, size = shaftSize)
            val bandCount = 8
            val bandH = postH / bandCount
            for (b in 0 until bandCount step 2) {
                drawRect(color = Color(0xFFF59E0B), topLeft = Offset(postCenterX - poleW * 0.5f, postTopY + b * bandH), size = Size(poleW, bandH * 0.6f))
            }
        }
    }

    // 4. Post Top Rain Cap
    drawRoundRect(
        color = Color(0xFFCBD5E1),
        topLeft = Offset(postCenterX - poleW * 0.7f, postTopY - 6f * zoomFactor),
        size = Size(poleW * 1.4f, 8f * zoomFactor),
        cornerRadius = CornerRadius(3f, 3f)
    )

    // 5. Signboard Mounting Bracket Arms & Panel
    val signCenterY = postTopY + postH * 0.32f + (mountHeightOffset * 90f * zoomFactor)
    val signW = (screenWidth * 0.62f * zoomFactor).coerceIn(200f, screenWidth * 0.88f)
    val signH = (signW * 0.42f).coerceIn(80f, 180f * zoomFactor)
    val signLeft = postCenterX - signW * 0.5f
    val signTop = signCenterY - signH * 0.5f

    // Mounting clamp rings encircling the pole
    val clampH = 12f * zoomFactor
    val clampTop1 = signTop + signH * 0.22f
    val clampTop2 = signTop + signH * 0.72f
    drawRoundRect(color = postStyle.bracketColor, topLeft = Offset(postCenterX - poleW * 0.8f, clampTop1), size = Size(poleW * 1.6f, clampH), cornerRadius = CornerRadius(3f, 3f))
    drawRoundRect(color = postStyle.bracketColor, topLeft = Offset(postCenterX - poleW * 0.8f, clampTop2), size = Size(poleW * 1.6f, clampH), cornerRadius = CornerRadius(3f, 3f))

    // Horizontal bracket support arms extending across sign back
    drawRect(color = postStyle.bracketColor, topLeft = Offset(signLeft + 20f, clampTop1 + 2f), size = Size(signW - 40f, clampH - 4f))
    drawRect(color = postStyle.bracketColor, topLeft = Offset(signLeft + 20f, clampTop2 + 2f), size = Size(signW - 40f, clampH - 4f))

    // Signboard Panel Drop Shadow
    drawRoundRect(
        color = Color(0x88000000),
        topLeft = Offset(signLeft + 4f, signTop + 6f),
        size = Size(signW, signH),
        cornerRadius = CornerRadius(10f, 10f)
    )

    // Signboard Panel Face (Green highway, blue municipal, or dark slate)
    drawRoundRect(
        color = postStyle.plateColor,
        topLeft = Offset(signLeft, signTop),
        size = Size(signW, signH),
        cornerRadius = CornerRadius(10f, 10f)
    )

    // Reflective Border Pinstripe (Highway sign standard)
    drawRoundRect(
        color = Color(0xFFFFFFFF),
        topLeft = Offset(signLeft + 5f, signTop + 5f),
        size = Size(signW - 10f, signH - 10f),
        cornerRadius = CornerRadius(7f, 7f),
        style = Stroke(width = 2.0f)
    )

    // Mounting Bolt Rivets on Sign Face
    val rivetMargin = 16f * zoomFactor
    val rivets = listOf(
        Offset(signLeft + rivetMargin, clampTop1 + clampH * 0.5f),
        Offset(signLeft + signW - rivetMargin, clampTop1 + clampH * 0.5f),
        Offset(signLeft + rivetMargin, clampTop2 + clampH * 0.5f),
        Offset(signLeft + signW - rivetMargin, clampTop2 + clampH * 0.5f)
    )
    for (r in rivets) {
        drawCircle(Color(0xFFE2EAF4), 3.5f * zoomFactor, r)
        drawCircle(Color(0xFF2B323D), 1.5f * zoomFactor, r)
    }

    return Offset(postCenterX, signCenterY)
}

// -------------------------------------------------------------
// VULKAN TYPOGRAPHY RENDERING (SDF, OUTLINE, SHADOW & GLOW)
// -------------------------------------------------------------
private fun DrawScope.drawVulkanTypography(
    text: String,
    font: FontTypeface,
    renderMode: TextRenderMode,
    alignment: TextAlignment,
    fontSize: Float,
    letterSpacing: Float,
    lineHeight: Float,
    textColor: Color,
    enableOutline: Boolean,
    outlineWidth: Float,
    outlineColor: Color,
    enableShadow: Boolean,
    shadowOffsetX: Float,
    shadowOffsetY: Float,
    shadowSoftness: Float,
    shadowColor: Color,
    smoothing: Float,
    thicknessBias: Float,
    enableGlow: Boolean,
    glowRadius: Float,
    glowColor: Color,
    badgeStyle: TextBadgeStyle,
    showMetrics: Boolean,
    rotX: Float,
    rotY: Float,
    zoomFactor: Float,
    mountTarget: TextMountTarget,
    textApplicationStyle: TextApplicationStyle,
    targetAnchor: Offset,
    textMeasurer: TextMeasurer
) {
    val composeAlign = when (alignment) {
        TextAlignment.LEFT -> androidx.compose.ui.text.style.TextAlign.Left
        TextAlignment.CENTER -> androidx.compose.ui.text.style.TextAlign.Center
        TextAlignment.RIGHT -> androidx.compose.ui.text.style.TextAlign.Right
    }

    val baseTextStyle = TextStyle(
        fontFamily = font.family,
        fontSize = fontSize.sp,
        fontWeight = if (thicknessBias > 0.05f) FontWeight.Bold else FontWeight.Medium,
        textAlign = composeAlign,
        letterSpacing = (letterSpacing - 1.0f).sp,
        lineHeight = (fontSize * lineHeight).sp
    )

    val measuredText = textMeasurer.measure(
        text = AnnotatedString(text),
        style = baseTextStyle
    )

    val textWidth = measuredText.size.width.toFloat()
    val textHeight = measuredText.size.height.toFloat()

    val textLeft = when (alignment) {
        TextAlignment.LEFT -> targetAnchor.x - textWidth * 0.5f
        TextAlignment.CENTER -> targetAnchor.x - textWidth * 0.5f
        TextAlignment.RIGHT -> targetAnchor.x - textWidth * 0.5f
    }
    val textTop = targetAnchor.y - textHeight * 0.5f

    // 1. RENDER BACKGROUND BADGE / FRAME (When Floating or HUD, or if specifically selected)
    if (badgeStyle != TextBadgeStyle.NONE && mountTarget != TextMountTarget.WALL && mountTarget != TextMountTarget.POST) {
        val padH = 22f * zoomFactor
        val padV = 16f * zoomFactor
        val badgeLeft = textLeft - padH
        val badgeTop = textTop - padV
        val badgeW = textWidth + padH * 2f
        val badgeH = textHeight + padV * 2f

        when (badgeStyle) {
            TextBadgeStyle.GLASS_DARK -> {
                drawRoundRect(
                    color = Color(0xCC0C121D),
                    topLeft = Offset(badgeLeft, badgeTop),
                    size = Size(badgeW, badgeH),
                    cornerRadius = CornerRadius(10f * zoomFactor, 10f * zoomFactor)
                )
                drawRoundRect(
                    color = Color(0x664B6E9C),
                    topLeft = Offset(badgeLeft, badgeTop),
                    size = Size(badgeW, badgeH),
                    cornerRadius = CornerRadius(10f * zoomFactor, 10f * zoomFactor),
                    style = Stroke(width = 1.5f)
                )
            }
            TextBadgeStyle.NEON_CYBER -> {
                drawRoundRect(
                    color = Color(0xEE050E18),
                    topLeft = Offset(badgeLeft, badgeTop),
                    size = Size(badgeW, badgeH),
                    cornerRadius = CornerRadius(6f * zoomFactor, 6f * zoomFactor)
                )
                drawRoundRect(
                    color = Color(0x6629DCFF),
                    topLeft = Offset(badgeLeft - 2f, badgeTop - 2f),
                    size = Size(badgeW + 4f, badgeH + 4f),
                    cornerRadius = CornerRadius(8f * zoomFactor, 8f * zoomFactor),
                    style = Stroke(width = 3.5f)
                )
                drawRoundRect(
                    color = Color(0xFF29DCFF),
                    topLeft = Offset(badgeLeft, badgeTop),
                    size = Size(badgeW, badgeH),
                    cornerRadius = CornerRadius(6f * zoomFactor, 6f * zoomFactor),
                    style = Stroke(width = 1.5f)
                )
            }
            TextBadgeStyle.SOLID_SLATE -> {
                drawRoundRect(
                    color = Color(0xF0181A20),
                    topLeft = Offset(badgeLeft, badgeTop),
                    size = Size(badgeW, badgeH),
                    cornerRadius = CornerRadius(6f * zoomFactor, 6f * zoomFactor)
                )
                drawRoundRect(
                    color = Color(0xFFFFA000),
                    topLeft = Offset(badgeLeft, badgeTop),
                    size = Size(badgeW, badgeH),
                    cornerRadius = CornerRadius(6f * zoomFactor, 6f * zoomFactor),
                    style = Stroke(width = 2.0f)
                )
            }
            TextBadgeStyle.TACTICAL_BRACKETS -> {
                drawRoundRect(
                    color = Color(0xBB08100C),
                    topLeft = Offset(badgeLeft, badgeTop),
                    size = Size(badgeW, badgeH),
                    cornerRadius = CornerRadius(4f, 4f)
                )
                val bLen = 14f * zoomFactor
                val bColor = Color(0xFF2BF69A)
                val bStroke = 2.0f

                drawLine(bColor, Offset(badgeLeft, badgeTop + bLen), Offset(badgeLeft, badgeTop), bStroke)
                drawLine(bColor, Offset(badgeLeft, badgeTop), Offset(badgeLeft + bLen, badgeTop), bStroke)
                drawLine(bColor, Offset(badgeLeft + badgeW - bLen, badgeTop), Offset(badgeLeft + badgeW, badgeTop), bStroke)
                drawLine(bColor, Offset(badgeLeft + badgeW, badgeTop), Offset(badgeLeft + badgeW, badgeTop + bLen), bStroke)
                drawLine(bColor, Offset(badgeLeft, badgeTop + badgeH - bLen), Offset(badgeLeft, badgeTop + badgeH), bStroke)
                drawLine(bColor, Offset(badgeLeft, badgeTop + badgeH), Offset(badgeLeft + bLen, badgeTop + badgeH), bStroke)
                drawLine(bColor, Offset(badgeLeft + badgeW - bLen, badgeTop + badgeH), Offset(badgeLeft + badgeW, badgeTop + badgeH), bStroke)
                drawLine(bColor, Offset(badgeLeft + badgeW, badgeTop + badgeH), Offset(badgeLeft + badgeW, badgeTop + badgeH - bLen), bStroke)
            }
            TextBadgeStyle.NONE -> {}
        }
    }

    // 2. RENDER APPLICATION STYLE SPECIAL EFFECTS (Channel Letters 3D shadow or Neon Wall Glow)
    if (mountTarget == TextMountTarget.WALL || mountTarget == TextMountTarget.POST) {
        when (textApplicationStyle) {
            TextApplicationStyle.CHANNEL_3D_LETTERS -> {
                // Extruded 3D Channel letters: directional drop shadow cast downward-right onto surface
                val extrudeSteps = 4
                for (e in 1..extrudeSteps) {
                    val offX = e * 1.5f * zoomFactor
                    val offY = e * 2.5f * zoomFactor
                    drawText(
                        textLayoutResult = measuredText,
                        color = Color(0x44000000),
                        topLeft = Offset(textLeft + offX, textTop + offY)
                    )
                }
            }
            TextApplicationStyle.NEON_SIGN -> {
                // Radial illumination onto wall/post face behind the neon tubes
                val neonGlowColor = textColor.copy(alpha = 0.55f)
                for (g in 4 downTo 1) {
                    val rad = g * 12f * zoomFactor
                    val offsets = listOf(
                        Offset(-rad, 0f), Offset(rad, 0f),
                        Offset(0f, -rad), Offset(0f, rad),
                        Offset(-rad * 0.7f, -rad * 0.7f),
                        Offset(rad * 0.7f, rad * 0.7f)
                    )
                    for (off in offsets) {
                        drawText(
                            textLayoutResult = measuredText,
                            color = neonGlowColor.copy(alpha = 0.12f),
                            topLeft = Offset(textLeft + off.x, textTop + off.y)
                        )
                    }
                }
            }
            TextApplicationStyle.DIRECT_DECAL -> {
                // Decal conforms directly with subtle surface blend
            }
            TextApplicationStyle.MOUNTED_PLAQUE -> {}
        }
    }

    // 3. RENDER EMISSIVE GLOW PASS (If enabled in shading controls)
    if (enableGlow && glowRadius > 0.05f) {
        val glowPasses = 3
        for (g in glowPasses downTo 1) {
            val spread = g * (glowRadius * 6f * zoomFactor)
            val passColor = glowColor.copy(alpha = (glowColor.alpha / glowPasses) * 0.45f)
            val offsets = listOf(
                Offset(-spread, 0f), Offset(spread, 0f),
                Offset(0f, -spread), Offset(0f, spread),
                Offset(-spread * 0.7f, -spread * 0.7f),
                Offset(spread * 0.7f, spread * 0.7f)
            )
            for (off in offsets) {
                drawText(
                    textLayoutResult = measuredText,
                    color = passColor,
                    topLeft = Offset(textLeft + off.x, textTop + off.y)
                )
            }
        }
    }

    // 4. RENDER DROP SHADOW PASS (Sascha Willems distancefieldfonts drop shadow)
    if (enableShadow) {
        val shadowPasses = if (shadowSoftness > 0.2f) 3 else 1
        for (s in 0 until shadowPasses) {
            val delta = s * (shadowSoftness * 2.5f * zoomFactor)
            val sColor = shadowColor.copy(alpha = shadowColor.alpha / (shadowPasses + 0.5f))
            drawText(
                textLayoutResult = measuredText,
                color = sColor,
                topLeft = Offset(textLeft + shadowOffsetX + delta, textTop + shadowOffsetY + delta)
            )
        }
    }

    // 5. RENDER OUTLINE STROKE PASS (Sascha Willems SDF outline width simulation)
    if (enableOutline && outlineWidth > 0.01f) {
        val strokeDist = (outlineWidth * 16f * zoomFactor).coerceAtLeast(1.0f)
        val strokeOffsets = listOf(
            Offset(-strokeDist, 0f), Offset(strokeDist, 0f),
            Offset(0f, -strokeDist), Offset(0f, strokeDist),
            Offset(-strokeDist * 0.707f, -strokeDist * 0.707f),
            Offset(strokeDist * 0.707f, -strokeDist * 0.707f),
            Offset(-strokeDist * 0.707f, strokeDist * 0.707f),
            Offset(strokeDist * 0.707f, -strokeDist * 0.707f)
        )
        for (off in strokeOffsets) {
            drawText(
                textLayoutResult = measuredText,
                color = outlineColor,
                topLeft = Offset(textLeft + off.x, textTop + off.y)
            )
        }
    }

    // 6. RENDER FOREGROUND TEXT PASS
    drawText(
        textLayoutResult = measuredText,
        color = textColor,
        topLeft = Offset(textLeft, textTop)
    )

    // 7. RENDER TYPOGRAPHIC METRIC GUIDES (If enabled)
    if (showMetrics) {
        val guideColor = Color(0xAAFF5252)
        val baselineColor = Color(0xAA40C4FF)
        val boxColor = Color(0x66FFEB3B)

        // Bounding box
        drawRect(
            color = boxColor,
            topLeft = Offset(textLeft, textTop),
            size = Size(textWidth, textHeight),
            style = Stroke(width = 1.0f)
        )

        for (i in 0 until measuredText.lineCount) {
            val lineBaseline = textTop + measuredText.getLineBaseline(i)
            val lineTop = textTop + measuredText.getLineTop(i)
            val lineBottom = textTop + measuredText.getLineBottom(i)

            // Baseline (Cyan)
            drawLine(
                color = baselineColor,
                start = Offset(textLeft - 10f, lineBaseline),
                end = Offset(textLeft + textWidth + 10f, lineBaseline),
                strokeWidth = 1.0f
            )

            // Ascender top (Red)
            drawLine(
                color = guideColor,
                start = Offset(textLeft - 6f, lineTop),
                end = Offset(textLeft + textWidth + 6f, lineTop),
                strokeWidth = 1.0f
            )

            // Descender bottom (Red)
            drawLine(
                color = guideColor,
                start = Offset(textLeft - 6f, lineBottom),
                end = Offset(textLeft + textWidth + 6f, lineBottom),
                strokeWidth = 1.0f
            )
        }
    }
}

// -------------------------------------------------------------
// FONT ATLAS TEXTURE MAP INSPECTOR (Sascha Willems SDF Atlas)
// -------------------------------------------------------------
private fun DrawScope.drawFontAtlasInspector(
    centerX: Float,
    centerY: Float,
    size: Float,
    font: FontTypeface,
    textMeasurer: TextMeasurer
) {
    val left = centerX - size * 0.5f
    val top = centerY - size * 0.5f

    // Atlas checkerboard backing
    drawRect(
        color = Color(0xFF141923),
        topLeft = Offset(left, top),
        size = Size(size, size)
    )

    // Draw 16x16 Glyph Grid
    val cells = 16
    val cellSize = size / cells
    val gridColor = Color(0x33446088)

    for (i in 0..cells) {
        val p = i * cellSize
        drawLine(gridColor, Offset(left + p, top), Offset(left + p, top + size), 1f)
        drawLine(gridColor, Offset(left, top + p), Offset(left + size, top + p), 1f)
    }

    // Render Simulated Distance Field Glyphs in Atlas Cells (ASCII 32 - 126)
    val atlasTextStyle = TextStyle(
        fontFamily = font.family,
        fontSize = (cellSize * 0.55f).sp,
        fontWeight = FontWeight.Bold,
        textAlign = androidx.compose.ui.text.style.TextAlign.Center
    )

    for (c in 33..126) {
        val idx = c - 32
        val col = idx % cells
        val row = idx / cells
        val cellLeft = left + col * cellSize
        val cellTop = top + row * cellSize

        // Distance field radial halo simulation
        drawCircle(
            color = Color(0x224888D8),
            radius = cellSize * 0.38f,
            center = Offset(cellLeft + cellSize * 0.5f, cellTop + cellSize * 0.5f)
        )

        val charStr = c.toChar().toString()
        val m = textMeasurer.measure(charStr, atlasTextStyle)
        drawText(
            textLayoutResult = m,
            color = Color(0xFFE2EAF4),
            topLeft = Offset(
                cellLeft + (cellSize - m.size.width) * 0.5f,
                cellTop + (cellSize - m.size.height) * 0.5f
            )
        )
    }

    drawRect(
        color = Color(0xFF4A72A4),
        topLeft = Offset(left, top),
        size = Size(size, size),
        style = Stroke(width = 2.0f)
    )
}
