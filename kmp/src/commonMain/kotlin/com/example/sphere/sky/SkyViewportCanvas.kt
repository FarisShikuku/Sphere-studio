package com.example.sphere

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.platform.testTag
import kotlin.math.PI
import kotlin.math.abs
import kotlin.math.cos
import kotlin.math.max
import kotlin.math.min
import kotlin.math.sin

@Composable
fun SkyViewportCanvas(
    modifier: Modifier = Modifier,
    sunElevation: Float,
    sunAzimuth: Float,
    sunAngularRadius: Float,
    coronaFlareIntensity: Float,
    moonPhase: Float,
    moonElevation: Float,
    moonAzimuth: Float,
    earthshine: Float,
    starCount: Int,
    twinkleSpeed: Float,
    twinkleCycle: Float,
    camYaw: Float,
    camPitch: Float,
    onDrag: (Float, Float) -> Unit
) {
    Canvas(
        modifier = modifier
            .fillMaxSize()
            .testTag("sky_viewport_canvas")
            .pointerInput(Unit) {
                detectDragGestures { change, dragAmount ->
                    change.consume()
                    onDrag(dragAmount.x, dragAmount.y)
                }
            }
    ) {
        val w = size.width
        val h = size.height

        // 1. Preetham / Hosek-Wilkie Rayleigh & Mie Sky Gradient Computation
        val (zenithColor, horizonColor) = when {
            sunElevation > 20f -> {
                Color(0xFF0F3A7A) to Color(0xFF6BA5E7)
            }
            sunElevation in 5f..20f -> {
                Color(0xFF1E3A8A) to Color(0xFF93C5FD)
            }
            sunElevation in 0f..5f -> {
                Color(0xFF312E81) to Color(0xFFFB923C)
            }
            sunElevation in -6f..0f -> {
                Color(0xFF0F172A) to Color(0xFFC2185B)
            }
            else -> {
                Color(0xFF030712) to Color(0xFF0B1329)
            }
        }

        val skyBrush = Brush.verticalGradient(
            colors = listOf(zenithColor, horizonColor),
            startY = 0f,
            endY = h * 0.85f
        )
        drawRect(brush = skyBrush)

        // 2. Night Starfield Rendering
        val starDarkness = (1f - (sunElevation + 6f) / 16f).coerceIn(0f, 1f)
        if (starDarkness > 0.05f) {
            val starSeed = 42
            val displayStars = min(starCount, 300)
            for (i in 0 until displayStars) {
                val starRa = ((i * 137.508f + starSeed) % 360f)
                val starDec = -10f + ((i * 53.1f) % 95f)

                val dYaw = (starRa - camYaw + 540f) % 360f - 180f
                val dPitch = starDec - camPitch

                if (abs(dYaw) < 80f && dPitch in -30f..80f) {
                    val sx = center.x + (dYaw / 80f) * (w / 2f)
                    val sy = center.y - (dPitch / 80f) * (h / 2f)

                    val twinkleFactor = 0.65f + 0.35f * sin(twinkleCycle * twinkleSpeed + (i * 1.7f))
                    val starAlpha = starDarkness * twinkleFactor * ((displayStars - i * 0.5f) / displayStars)

                    val starColor = when (i % 5) {
                        0 -> Color(0xFF90CAF9)
                        1 -> Color(0xFFFFE082)
                        2 -> Color(0xFFFFAB91)
                        else -> Color.White
                    }

                    val starSize = if (i % 12 == 0) 2.6f else 1.5f
                    drawCircle(
                        color = starColor.copy(alpha = starAlpha.coerceIn(0f, 1f)),
                        radius = starSize,
                        center = Offset(sx, sy)
                    )
                }
            }
        }

        // 3. Solar Disk & Corona Flare
        val sunDYaw = (sunAzimuth - camYaw + 540f) % 360f - 180f
        val sunDPitch = sunElevation - camPitch

        if (abs(sunDYaw) < 90f && sunDPitch in -30f..90f && sunElevation > -10f) {
            val sunX = center.x + (sunDYaw / 80f) * (w / 2f)
            val sunY = center.y - (sunDPitch / 80f) * (h / 2f)
            val sunCenter = Offset(sunX, sunY)

            val coronaRadius = 45f + coronaFlareIntensity * 35f
            drawCircle(
                brush = Brush.radialGradient(
                    colors = listOf(
                        Color(0xFFFFECB3).copy(alpha = 0.55f * coronaFlareIntensity),
                        Color(0xFFFF9800).copy(alpha = 0.20f * coronaFlareIntensity),
                        Color.Transparent
                    ),
                    center = sunCenter,
                    radius = coronaRadius * 2f
                ),
                radius = coronaRadius * 2f,
                center = sunCenter
            )

            val diskRadius = max(8f, sunAngularRadius * 380f)
            drawCircle(
                brush = Brush.radialGradient(
                    colors = listOf(
                        Color(0xFFFFFFFF),
                        Color(0xFFFFEE58),
                        Color(0xFFFF9800)
                    ),
                    center = sunCenter,
                    radius = diskRadius
                ),
                radius = diskRadius,
                center = sunCenter
            )
        }

        // 4. Lunar Orb & Phase Rendering
        val moonDYaw = (moonAzimuth - camYaw + 540f) % 360f - 180f
        val moonDPitch = moonElevation - camPitch

        if (abs(moonDYaw) < 90f && moonDPitch in -30f..90f && moonElevation > -10f) {
            val moonX = center.x + (moonDYaw / 80f) * (w / 2f)
            val moonY = center.y - (moonDPitch / 80f) * (h / 2f)
            val moonCenter = Offset(moonX, moonY)
            val moonRadius = 14f

            drawCircle(
                color = Color(0xFF64748B).copy(alpha = earthshine * 10f),
                radius = moonRadius * 1.5f,
                center = moonCenter
            )

            drawCircle(
                color = Color(0xFF1E293B),
                radius = moonRadius,
                center = moonCenter
            )

            val phaseFactor = (1f - cos(moonPhase * 2f * PI.toFloat())) * 0.5f
            val moonLitColor = Color(0xFFE2E8F0)
            drawCircle(
                color = moonLitColor.copy(alpha = phaseFactor.coerceIn(0.2f, 1f)),
                radius = moonRadius * phaseFactor.coerceAtLeast(0.3f),
                center = moonCenter
            )
        }

        // 5. Horizon Ground Silhouette & Compass
        val horizonY = center.y + (camPitch / 80f) * (h / 2f)
        drawRect(
            color = Color(0xFF07090E),
            topLeft = Offset(0f, horizonY),
            size = Size(w, h - horizonY)
        )

        drawLine(
            color = Color(0xFF1E293B),
            start = Offset(0f, horizonY),
            end = Offset(w, horizonY),
            strokeWidth = 1.5f
        )

        val cardinalPoints = listOf("N" to 0f, "NE" to 45f, "E" to 90f, "SE" to 135f, "S" to 180f, "SW" to 225f, "W" to 270f, "NW" to 315f)
        for ((_, az) in cardinalPoints) {
            val dAz = (az - camYaw + 540f) % 360f - 180f
            if (abs(dAz) < 70f) {
                val cx = center.x + (dAz / 80f) * (w / 2f)
                drawLine(
                    color = Color(0xFF475569),
                    start = Offset(cx, horizonY - 4f),
                    end = Offset(cx, horizonY + 4f),
                    strokeWidth = 1.5f
                )
            }
        }
    }
}
