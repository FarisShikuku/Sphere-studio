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
import com.example.sphere.model.FontTypeface
import com.example.sphere.model.TextAlignment
import com.example.sphere.model.TextBadgeStyle
import com.example.sphere.model.TextRenderMode
import kotlin.math.*

/**
 * GlyphCoreViewportCanvas - Interactive 3D and 2D Vulkan Typography Viewport.
 * Simulates:
 *  - Sascha Willems dynamic text overlay with vertex staging buffer
 *  - Signed Distance Field (SDF) multi-pass rendering (Drop Shadow, Stroke Outline, Crisp Core, Emissive Glow)
 *  - 3D in-world camera billboard projection and orbit controls
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
                // RENDER 3D SCENE & PERSPECTIVE GRID
                drawPerspectiveScene(
                    width = width,
                    height = height,
                    rotX = rotX,
                    rotY = rotY,
                    renderMode = renderMode
                )

                // RENDER VULKAN TYPOGRAPHY WITH SDF SIMULATION
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
                    textMeasurer = textMeasurer
                )
            }
        }

        // Live Vulkan Pipeline Telemetry Watermark (Top Left)
        Column(
            modifier = Modifier
                .align(Alignment.TopStart)
                .padding(12.dp)
                .background(Color(0xBB080B10), RoundedCornerShape(8.dp))
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
                    "VULKAN TEXT PIPELINE // ACTIVE",
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
                "Staging: ${stagingBytes} B | Mode: ${renderMode.label} | Zoom: ${"%.1f".format(zoomFactor)}x",
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
                    if (showAtlasView) Icons.Default.GridOn else Icons.Default.ViewInAr,
                    contentDescription = null,
                    tint = if (showAtlasView) Color(0xFFFFA726) else Color(0xFF64B5F6),
                    modifier = Modifier.size(14.dp)
                )
                Spacer(Modifier.width(4.dp))
                Text(
                    if (showAtlasView) "ATLAS VIEW (1024x1024 SDF)" else "3D VIEWPORT",
                    style = MaterialTheme.typography.labelSmall.copy(
                        fontWeight = FontWeight.Bold,
                        color = Color.White,
                        fontSize = 9.sp
                    )
                )
            }
        }
    }
}

// -------------------------------------------------------------
// 3D SCENE BACKGROUND & PERSPECTIVE GRID
// -------------------------------------------------------------
private fun DrawScope.drawPerspectiveScene(
    width: Float,
    height: Float,
    rotX: Float,
    rotY: Float,
    renderMode: TextRenderMode
) {
    val horizonY = height * 0.52f + (rotX * 1.5f).coerceIn(-100f, 100f)

    // Sky gradient
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

    // Ground plane with perspective grid
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

    // 3D Center Stage Platform (when in 3D Billboard or Decal mode)
    if (renderMode != TextRenderMode.TEXT_OVERLAY_2D) {
        val stageRadius = min(width, height) * 0.28f
        val stageCenter = Offset(width * 0.5f, horizonY + 65f)

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

        // Small coordinate origin cross
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
    textMeasurer: TextMeasurer
) {
    val width = size.width
    val height = size.height

    // Calculate position based on render mode:
    // 2D HUD: Upper-center or centered
    // 3D Billboard: Anchored above stage with 3D perspective shift
    val baseAnchorY = if (renderMode == TextRenderMode.TEXT_OVERLAY_2D) {
        height * 0.42f
    } else {
        // 3D Billboard perspective anchor
        val horizonY = height * 0.52f + (rotX * 1.5f).coerceIn(-100f, 100f)
        horizonY - 40f + (rotX * 0.4f)
    }

    val baseAnchorX = if (renderMode == TextRenderMode.TEXT_OVERLAY_2D) {
        width * 0.5f
    } else {
        width * 0.5f - (rotY * 0.8f)
    }

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
        TextAlignment.LEFT -> baseAnchorX - textWidth * 0.5f
        TextAlignment.CENTER -> baseAnchorX - textWidth * 0.5f
        TextAlignment.RIGHT -> baseAnchorX - textWidth * 0.5f
    }
    val textTop = baseAnchorY - textHeight * 0.5f

    // 1. RENDER BACKGROUND PLAQUE / BADGE
    if (badgeStyle != TextBadgeStyle.NONE) {
        val padH = 22f * zoomFactor
        val padV = 16f * zoomFactor
        val badgeLeft = textLeft - padH
        val badgeTop = textTop - padV
        val badgeW = textWidth + padH * 2f
        val badgeH = textHeight + padV * 2f

        when (badgeStyle) {
            TextBadgeStyle.GLASS_DARK -> {
                // Frosted dark glass
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
                // Cyberpunk neon bracket plate
                drawRoundRect(
                    color = Color(0xEE050E18),
                    topLeft = Offset(badgeLeft, badgeTop),
                    size = Size(badgeW, badgeH),
                    cornerRadius = CornerRadius(6f * zoomFactor, 6f * zoomFactor)
                )
                // Outer glow stroke
                drawRoundRect(
                    color = Color(0x6629DCFF),
                    topLeft = Offset(badgeLeft - 2f, badgeTop - 2f),
                    size = Size(badgeW + 4f, badgeH + 4f),
                    cornerRadius = CornerRadius(8f * zoomFactor, 8f * zoomFactor),
                    style = Stroke(width = 3.5f)
                )
                // Crisp inner stroke
                drawRoundRect(
                    color = Color(0xFF29DCFF),
                    topLeft = Offset(badgeLeft, badgeTop),
                    size = Size(badgeW, badgeH),
                    cornerRadius = CornerRadius(6f * zoomFactor, 6f * zoomFactor),
                    style = Stroke(width = 1.5f)
                )
            }
            TextBadgeStyle.SOLID_SLATE -> {
                // Solid high-contrast slate
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
                // Dark background + avionics corner brackets
                drawRoundRect(
                    color = Color(0xBB08100C),
                    topLeft = Offset(badgeLeft, badgeTop),
                    size = Size(badgeW, badgeH),
                    cornerRadius = CornerRadius(4f, 4f)
                )
                val bLen = 14f * zoomFactor
                val bColor = Color(0xFF2BF69A)
                val bStroke = 2.0f

                // Top-Left
                drawLine(bColor, Offset(badgeLeft, badgeTop + bLen), Offset(badgeLeft, badgeTop), bStroke)
                drawLine(bColor, Offset(badgeLeft, badgeTop), Offset(badgeLeft + bLen, badgeTop), bStroke)
                // Top-Right
                drawLine(bColor, Offset(badgeLeft + badgeW - bLen, badgeTop), Offset(badgeLeft + badgeW, badgeTop), bStroke)
                drawLine(bColor, Offset(badgeLeft + badgeW, badgeTop), Offset(badgeLeft + badgeW, badgeTop + bLen), bStroke)
                // Bottom-Left
                drawLine(bColor, Offset(badgeLeft, badgeTop + badgeH - bLen), Offset(badgeLeft, badgeTop + badgeH), bStroke)
                drawLine(bColor, Offset(badgeLeft, badgeTop + badgeH), Offset(badgeLeft + bLen, badgeTop + badgeH), bStroke)
                // Bottom-Right
                drawLine(bColor, Offset(badgeLeft + badgeW - bLen, badgeTop + badgeH), Offset(badgeLeft + badgeW, badgeTop + badgeH), bStroke)
                drawLine(bColor, Offset(badgeLeft + badgeW, badgeTop + badgeH), Offset(badgeLeft + badgeW, badgeTop + badgeH - bLen), bStroke)
            }
            TextBadgeStyle.NONE -> {}
        }
    }

    // 2. RENDER EMISSIVE GLOW PASS (If enabled)
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

    // 3. RENDER DROP SHADOW PASS (Sascha Willems distancefieldfonts drop shadow)
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

    // 4. RENDER OUTLINE STROKE PASS (Sascha Willems SDF outline width simulation)
    if (enableOutline && outlineWidth > 0.01f) {
        val strokeDist = (outlineWidth * 16f * zoomFactor).coerceAtLeast(1.0f)
        // 8-directional stroke dilation
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

    // 5. RENDER FOREGROUND TEXT PASS
    drawText(
        textLayoutResult = measuredText,
        color = textColor,
        topLeft = Offset(textLeft, textTop)
    )

    // 6. RENDER TYPOGRAPHIC METRIC GUIDES (If enabled)
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

        // Draw line for each text line baseline
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
        // Vertical lines
        drawLine(gridColor, Offset(left + p, top), Offset(left + p, top + size), 1f)
        // Horizontal lines
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

        // Glyph character
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

    // Outer border
    drawRect(
        color = Color(0xFF4A72A4),
        topLeft = Offset(left, top),
        size = Size(size, size),
        style = Stroke(width = 2.0f)
    )
}
