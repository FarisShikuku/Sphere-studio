package com.example.sphere

import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.FilterQuality
import androidx.compose.ui.graphics.ImageBitmap
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.graphics.drawscope.clipRect
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.IntSize
import kotlin.math.PI
import kotlin.math.abs
import kotlin.math.cos
import kotlin.math.sin
import kotlin.random.Random

fun DrawScope.drawWeatherAtmosphere(
    w: Float,
    h: Float,
    horizonY: Float,
    cloudCoverage: Float,
    fogDensity: Float,
    isStorm: Boolean,
    lightningFlash: Float,
    rainIntensity: Float,
    snowIntensity: Float
) {
    val (topSky, midSky, horizonSky) = when {
        lightningFlash > 0.2f -> {
            Triple(Color(0xFFCBD5E1), Color(0xFFE2E8F0), Color(0xFFF8FAFC))
        }
        isStorm -> {
            Triple(Color(0xFF080D1A), Color(0xFF131D31), Color(0xFF1E293B))
        }
        snowIntensity > 0.3f -> {
            Triple(Color(0xFF1E293B), Color(0xFF334155), Color(0xFF64748B))
        }
        rainIntensity > 0.4f -> {
            Triple(Color(0xFF0F172A), Color(0xFF1E293B), Color(0xFF334155))
        }
        fogDensity > 0.5f -> {
            Triple(Color(0xFF334155), Color(0xFF64748B), Color(0xFF94A3B8))
        }
        cloudCoverage > 0.7f -> {
            Triple(Color(0xFF1A2639), Color(0xFF2D3E56), Color(0xFF475569))
        }
        else -> {
            Triple(Color(0xFF0D47A1), Color(0xFF0284C7), Color(0xFF90CAF9))
        }
    }

    drawRect(
        brush = Brush.verticalGradient(
            colors = listOf(topSky, midSky, horizonSky),
            startY = 0f,
            endY = horizonY
        ),
        size = Size(w, horizonY)
    )
}

fun DrawScope.drawGroundSurface(
    w: Float,
    h: Float,
    horizonY: Float,
    camPitch: Float,
    camYaw: Float,
    surfaceMode: SurfaceSceneMode = SurfaceSceneMode.ENDLESS,
    stageWidth: Float = 10.0f,
    stageLength: Float = 10.0f,
    puddleDepth: Float,
    snowCover: Float,
    wetness: Float,
    cloudCoverage: Float,
    windSpeed: Float,
    cloudWave: Float,
    lightningFlash: Float,
    showGrid: Boolean = false
) {
    if (surfaceMode == SurfaceSceneMode.NONE) {
        return
    }

    val baseGroundColor = when {
        lightningFlash > 0.2f -> Color(0xFF475569)
        snowCover > 0.4f -> Color(0xFFCBD5E1)
        wetness > 0.5f -> Color(0xFF0B0F19)
        else -> Color(0xFF131B2A)
    }

    if (surfaceMode == SurfaceSceneMode.ENDLESS) {
        drawRect(
            brush = Brush.verticalGradient(
                colors = listOf(baseGroundColor.copy(alpha = 0.95f), baseGroundColor),
                startY = horizonY,
                endY = h
            ),
            topLeft = Offset(0f, horizonY),
            size = Size(w, (h - horizonY).coerceAtLeast(0f))
        )

        if (showGrid) {
            val gridColor = if (snowCover > 0.4f) Color(0xFF94A3B8).copy(alpha = 0.3f)
            else Color(0xFF1E293B).copy(alpha = 0.6f)

            for (i in 0..12) {
                val frac = i / 12f
                val py = horizonY + (h - horizonY) * (frac * frac)
                drawLine(
                    color = gridColor,
                    start = Offset(0f, py),
                    end = Offset(w, py),
                    strokeWidth = 1f
                )
            }

            val centerX = w * 0.5f + sin((camYaw - 180f) * (PI.toFloat() / 180f)) * (w * 0.5f)
            for (i in -8..8) {
                val gx = centerX + i * (w * 0.12f)
                drawLine(
                    color = gridColor.copy(alpha = 0.35f),
                    start = Offset(centerX, horizonY),
                    end = Offset(gx, h),
                    strokeWidth = 1f
                )
            }
        }

        if (cloudCoverage > 0.15f) {
            val shadowShift = (cloudWave * 15f + windSpeed * 2f) % (w * 0.8f)
            val shadowAlpha = (cloudCoverage * 0.35f).coerceIn(0.08f, 0.45f)
            drawOval(
                brush = Brush.radialGradient(
                    colors = listOf(Color.Black.copy(alpha = shadowAlpha), Color.Transparent),
                    center = Offset(w * 0.35f + shadowShift * 0.6f, horizonY + (h - horizonY) * 0.45f),
                    radius = w * 0.32f
                ),
                topLeft = Offset(w * 0.15f + shadowShift * 0.6f, horizonY + (h - horizonY) * 0.25f),
                size = Size(w * 0.45f, (h - horizonY) * 0.55f)
            )
            drawOval(
                brush = Brush.radialGradient(
                    colors = listOf(Color.Black.copy(alpha = shadowAlpha * 0.85f), Color.Transparent),
                    center = Offset(w * 0.72f - shadowShift * 0.4f, horizonY + (h - horizonY) * 0.65f),
                    radius = w * 0.28f
                ),
                topLeft = Offset(w * 0.55f - shadowShift * 0.4f, horizonY + (h - horizonY) * 0.45f),
                size = Size(w * 0.40f, (h - horizonY) * 0.48f)
            )
        }

        if (puddleDepth > 0.05f) {
            val puddleColor = if (lightningFlash > 0.2f) Color(0xFFE2E8F0) else Color(0xFF1E3A5F).copy(alpha = 0.65f)
            drawOval(
                color = puddleColor,
                topLeft = Offset(w * 0.22f, horizonY + 28f),
                size = Size(w * 0.26f * puddleDepth, 22f * puddleDepth)
            )
            drawOval(
                color = puddleColor,
                topLeft = Offset(w * 0.58f, horizonY + 54f),
                size = Size(w * 0.32f * puddleDepth, 32f * puddleDepth)
            )
            drawOval(
                color = puddleColor,
                topLeft = Offset(w * 0.36f, horizonY + 95f),
                size = Size(w * 0.42f * puddleDepth, 38f * puddleDepth)
            )
        }
    } else {
        val stageCenterY = horizonY + (h - horizonY) * 0.45f
        val stageHalfW = (w * 0.36f * (stageWidth / 10.0f)).coerceIn(w * 0.18f, w * 0.48f)
        val stageHalfH = (h * 0.18f * (stageLength / 10.0f)).coerceIn(h * 0.09f, h * 0.28f)
        val stageThickness = 28f

        val topP0 = Offset(w * 0.5f - stageHalfW, stageCenterY)
        val topP1 = Offset(w * 0.5f, stageCenterY - stageHalfH)
        val topP2 = Offset(w * 0.5f + stageHalfW, stageCenterY)
        val topP3 = Offset(w * 0.5f, stageCenterY + stageHalfH)

        val btmP0 = Offset(topP0.x, topP0.y + stageThickness)
        val btmP2 = Offset(topP2.x, topP2.y + stageThickness)
        val btmP3 = Offset(topP3.x, topP3.y + stageThickness)

        drawOval(
            brush = Brush.radialGradient(
                colors = listOf(Color.Black.copy(alpha = 0.55f), Color.Transparent),
                center = Offset(w * 0.5f, stageCenterY + stageHalfH + 35f),
                radius = stageHalfW * 0.9f
            ),
            topLeft = Offset(w * 0.5f - stageHalfW * 0.9f, stageCenterY + stageHalfH + 15f),
            size = Size(stageHalfW * 1.8f, stageHalfH * 1.4f)
        )

        when (surfaceMode) {
            SurfaceSceneMode.WATER_BODY -> {
                val basinPath = Path().apply {
                    moveTo(topP0.x, topP0.y); lineTo(topP3.x, topP3.y); lineTo(topP2.x, topP2.y)
                    lineTo(btmP2.x, btmP2.y); lineTo(btmP3.x, btmP3.y); lineTo(btmP0.x, btmP0.y); close()
                }
                drawPath(basinPath, color = Color(0xDD075985))

                val waterPath = Path().apply {
                    moveTo(topP0.x, topP0.y); lineTo(topP1.x, topP1.y); lineTo(topP2.x, topP2.y); lineTo(topP3.x, topP3.y); close()
                }
                drawPath(
                    waterPath,
                    brush = Brush.linearGradient(
                        colors = listOf(Color(0xFA0284C7), Color(0xFA0369A1), Color(0xFA0C4A6E)),
                        start = topP1,
                        end = topP3
                    )
                )
                for (k in 1..4) {
                    val rippleY = stageCenterY - stageHalfH * 0.5f + (k * 18f)
                    drawOval(
                        color = Color(0xFFE0F2FE).copy(alpha = 0.45f),
                        topLeft = Offset(w * 0.5f - stageHalfW * 0.6f * (k / 4f), rippleY),
                        size = Size(stageHalfW * 1.2f * (k / 4f), 12f * (k / 4f)),
                        style = Stroke(width = 1.2f)
                    )
                }
                drawPath(waterPath, color = Color(0xFFBAE6FD).copy(alpha = 0.85f), style = Stroke(width = 2.5f))
            }
            SurfaceSceneMode.BROKEN_SLAB -> {
                val skirtPath = Path().apply {
                    moveTo(topP0.x, topP0.y); lineTo(topP3.x + 8f, topP3.y - 4f); lineTo(topP2.x, topP2.y)
                    lineTo(btmP2.x - 12f, btmP2.y + 12f); lineTo(btmP3.x, btmP3.y + 16f); lineTo(btmP0.x + 10f, btmP0.y + 14f); close()
                }
                drawPath(skirtPath, color = Color(0xFF1E293B))
                drawPath(skirtPath, color = Color(0xFF334155), style = Stroke(width = 1.5f))

                val topRock = Path().apply {
                    moveTo(topP0.x, topP0.y); lineTo(topP1.x + 12f, topP1.y + 6f); lineTo(topP2.x, topP2.y)
                    lineTo(topP3.x - 8f, topP3.y - 4f); close()
                }
                drawPath(topRock, color = if (snowCover > 0.4f) Color(0xFFCBD5E1) else Color(0xFF293548))
                drawPath(topRock, color = Color(0xFF64748B), style = Stroke(width = 1.8f))
            }
            else -> {
                val skirtPath = Path().apply {
                    moveTo(topP0.x, topP0.y); lineTo(topP3.x, topP3.y); lineTo(topP2.x, topP2.y)
                    lineTo(btmP2.x, btmP2.y); lineTo(btmP3.x, btmP3.y); lineTo(btmP0.x, btmP0.y); close()
                }
                drawPath(skirtPath, color = Color(0xFF131B2A))
                drawPath(skirtPath, color = Color(0xFF1E293B), style = Stroke(width = 1f))

                val topSlab = Path().apply {
                    moveTo(topP0.x, topP0.y); lineTo(topP1.x, topP1.y); lineTo(topP2.x, topP2.y); lineTo(topP3.x, topP3.y); close()
                }
                val slabFillColor = when {
                    snowCover > 0.4f -> Color(0xFFCBD5E1)
                    wetness > 0.5f -> Color(0xFF111726)
                    else -> Color(0xFF1E2638)
                }
                drawPath(topSlab, color = slabFillColor)

                if (showGrid) {
                    for (g in 1..4) {
                        val f = g / 5f
                        val pA = Offset(topP0.x + (topP1.x - topP0.x) * f, topP0.y + (topP1.y - topP0.y) * f)
                        val pB = Offset(topP3.x + (topP2.x - topP3.x) * f, topP3.y + (topP2.y - topP3.y) * f)
                        drawLine(Color(0xFF38BDF8).copy(alpha = 0.25f), pA, pB, strokeWidth = 1f)

                        val pC = Offset(topP1.x + (topP2.x - topP1.x) * f, topP1.y + (topP2.y - topP1.y) * f)
                        val pD = Offset(topP0.x + (topP3.x - topP0.x) * f, topP0.y + (topP3.y - topP0.y) * f)
                        drawLine(Color(0xFF38BDF8).copy(alpha = 0.25f), pC, pD, strokeWidth = 1f)
                    }

                    drawPath(topSlab, color = Color(0xFF38BDF8).copy(alpha = 0.65f), style = Stroke(width = 2f))
                }

                if (puddleDepth > 0.05f) {
                    val pColor = if (lightningFlash > 0.2f) Color(0xFFE2E8F0) else Color(0xFF1E3A5F).copy(alpha = 0.75f)
                    drawOval(
                        color = pColor,
                        topLeft = Offset(w * 0.5f - stageHalfW * 0.35f, stageCenterY - 10f),
                        size = Size(stageHalfW * 0.7f * puddleDepth, 24f * puddleDepth)
                    )
                }
            }
        }
    }
}

fun DrawScope.drawPhotorealisticCloudscape(
    w: Float,
    h: Float,
    horizonY: Float,
    cloudImage: ImageBitmap,
    coverage: Float,
    density: Float,
    enableGodRays: Boolean,
    godRaysIntensity: Float,
    enableCirrus: Boolean,
    cirrusCoverage: Float,
    sunBacklight: Float,
    camYaw: Float,
    camPitch: Float,
    windSpeed: Float,
    cloudWave: Float,
    timeSeconds: Float,
    isStorm: Boolean,
    lightningFlash: Float
) {
    if (coverage < 0.02f) return

    val sunCenter = ProceduralCloudSynthesizer.projectSunToScreen(
        w = w,
        h = h,
        camYaw = camYaw,
        camPitch = camPitch
    )
    val isSunVisible = sunCenter != null && sunCenter.y < horizonY + 80f && sunCenter.x in (-w * 0.35f)..(w * 1.35f)

    clipRect(left = 0f, top = 0f, right = w, bottom = horizonY) {
        if (isSunVisible && sunCenter != null && !isStorm) {
            val coronaRadius = w * 0.28f
            drawCircle(
                brush = Brush.radialGradient(
                    colors = listOf(
                        Color(0xFFFFFBEB).copy(alpha = 0.60f * sunBacklight),
                        Color(0xFFFFEDD5).copy(alpha = 0.28f * sunBacklight),
                        Color(0xFFBAE6FD).copy(alpha = 0.08f * sunBacklight),
                        Color.Transparent
                    ),
                    center = sunCenter,
                    radius = coronaRadius
                ),
                radius = coronaRadius,
                center = sunCenter
            )

            drawCircle(
                brush = Brush.radialGradient(
                    colors = listOf(
                        Color(0xFFFFFFFF),
                        Color(0xFFFEF08A),
                        Color(0xFFF59E0B)
                    ),
                    center = sunCenter,
                    radius = 24f
                ),
                radius = 24f,
                center = sunCenter
            )
        }

        if (enableGodRays && isSunVisible && sunCenter != null && godRaysIntensity > 0.05f) {
            val rayCount = 14
            val baseAlpha = (godRaysIntensity * (1.0f - abs(coverage - 0.60f) * 0.7f) * 0.40f).coerceIn(0.06f, 0.38f)

            for (r in 0 until rayCount) {
                val rayAngle = (r.toFloat() / rayCount) * 160f - 80f + sin(cloudWave + r) * 4f
                val rad = (rayAngle + 90f) * PI.toFloat() / 180f
                val rayLength = h * 0.90f

                val endX = sunCenter.x + cos(rad) * rayLength
                val endY = sunCenter.y + sin(rad) * rayLength
                val beamWidth = 28f + (r % 3) * 16f

                val rayPath = Path().apply {
                    moveTo(sunCenter.x - beamWidth * 0.2f, sunCenter.y)
                    lineTo(sunCenter.x + beamWidth * 0.2f, sunCenter.y)
                    lineTo(endX + beamWidth * 1.5f, endY)
                    lineTo(endX - beamWidth * 1.5f, endY)
                    close()
                }

                val beamShimmer = 0.75f + 0.25f * sin(timeSeconds * 1.5f + r * 1.8f)
                drawPath(
                    path = rayPath,
                    brush = Brush.linearGradient(
                        colors = listOf(
                            Color(0xFFFFFBEB).copy(alpha = baseAlpha * beamShimmer),
                            Color(0xFFFEF3C7).copy(alpha = baseAlpha * 0.5f * beamShimmer),
                            Color.Transparent
                        ),
                        start = sunCenter,
                        end = Offset(endX, endY)
                    )
                )
            }
        }

        if (enableCirrus && cirrusCoverage > 0.05f) {
            val cirrusAlpha = (cirrusCoverage * 0.50f).coerceIn(0.08f, 0.65f)
            val cirrusDrift = (timeSeconds * 8.0f) % (w * 1.5f)

            for (c in 0..7) {
                val baseY = horizonY * (0.08f + c * 0.05f)
                val curvePath = Path().apply {
                    val startX = -w * 0.2f + (c * w * 0.25f + cirrusDrift) % (w * 1.4f)
                    moveTo(startX, baseY)
                    quadraticTo(
                        startX + w * 0.35f,
                        baseY - 20f + sin(cloudWave + c) * 6f,
                        startX + w * 0.70f,
                        baseY + 12f
                    )
                }

                drawPath(
                    path = curvePath,
                    color = Color.White.copy(alpha = cirrusAlpha * (0.4f + (c % 3) * 0.2f)),
                    style = Stroke(width = 12f + (c % 4) * 8f, cap = StrokeCap.Round)
                )
            }
        }

        val cloudAlpha = if (lightningFlash > 0.2f) 1.0f else (0.95f * density).coerceIn(0.1f, 1.0f)

        drawImage(
            image = cloudImage,
            dstOffset = IntOffset(0, 0),
            dstSize = IntSize(w.toInt(), horizonY.toInt().coerceAtLeast(1)),
            alpha = cloudAlpha,
            filterQuality = FilterQuality.Medium
        )
    }
}
