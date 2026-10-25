package com.example.sphere

import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.Stroke
import kotlin.math.PI
import kotlin.math.cos
import kotlin.math.sin
import kotlin.random.Random

/**
 * Precipitation, Snow, Lightning, and Fog Volume overlay renderers
 * for the Weather Simulation Studio.
 */

fun DrawScope.drawProceduralLightning(
    w: Float,
    h: Float,
    flashIntensity: Float,
    seed: Int,
    horizonY: Float
) {
    val rng = Random(seed)
    val startX = w * (0.3f + rng.nextFloat() * 0.4f)
    var curX = startX
    var curY = horizonY * 0.12f
    val targetY = horizonY

    val boltColor = Color(0xFFF8FAFC).copy(alpha = flashIntensity)
    val glowColor = Color(0xFF60A5FA).copy(alpha = flashIntensity * 0.6f)

    val path = Path()
    path.moveTo(curX, curY)

    val steps = 14
    val stepY = (targetY - curY) / steps

    for (s in 0 until steps) {
        curX += (rng.nextFloat() - 0.5f) * 45f
        curY += stepY
        path.lineTo(curX, curY)
    }

    drawPath(
        path = path,
        color = glowColor,
        style = Stroke(width = 7f, cap = StrokeCap.Round)
    )
    drawPath(
        path = path,
        color = boltColor,
        style = Stroke(width = 2.5f, cap = StrokeCap.Round)
    )
}

fun DrawScope.drawRainStreaks(
    w: Float,
    h: Float,
    particles: List<FloatArray>,
    intensity: Float,
    dropSpeed: Float,
    windAngle: Float,
    windSpeed: Float,
    time: Float,
    horizonY: Float
) {
    val rad = windAngle * PI.toFloat() / 180f
    val windDX = cos(rad) * (windSpeed / 12f)
    val count = (particles.size * intensity).toInt().coerceAtLeast(10)
    val rainColor = Color(0xFF93C5FD).copy(alpha = 0.65f)

    for (i in 0 until count) {
        val p = particles[i]
        val speed = p[3] * (dropSpeed / 16f)
        val y = ((p[1] + time * speed) % 1.0f) * h
        val x = ((p[0] + (y / h) * windDX * 0.05f) % 1.0f) * w
        val len = 12f * p[2]

        drawLine(
            color = rainColor,
            start = Offset(x, y),
            end = Offset(x + windDX * 1.5f, y + len),
            strokeWidth = 1.2f,
            cap = StrokeCap.Round
        )

        if (y > horizonY && y < horizonY + 20f && (i % 3 == 0)) {
            drawCircle(
                color = Color(0xFFBFDBFE).copy(alpha = 0.5f),
                radius = 3.5f,
                center = Offset(x, horizonY + 6f),
                style = Stroke(width = 1f)
            )
        }
    }
}

fun DrawScope.drawSnowFlurries(
    w: Float,
    h: Float,
    particles: List<FloatArray>,
    intensity: Float,
    flutter: Float,
    windSpeed: Float,
    time: Float,
    horizonY: Float
) {
    val count = (particles.size * intensity).toInt().coerceAtLeast(10)
    val flakeColor = Color(0xFFF8FAFC).copy(alpha = 0.85f)

    for (i in 0 until count) {
        val p = particles[i]
        val phase = p[2] + time * 3f
        val flutterX = sin(phase) * flutter * 8f
        val y = ((p[1] + time * 0.08f) % 1.0f) * h
        val x = ((p[0] + flutterX / w + (windSpeed / 100f) * (y / h)) % 1.0f) * w

        drawCircle(
            color = flakeColor,
            radius = p[3],
            center = Offset(x, y)
        )
    }
}

fun DrawScope.drawFogVolumeOverlay(
    w: Float,
    h: Float,
    density: Float,
    mist: Float,
    lightningFlash: Float,
    horizonY: Float
) {
    val fogColor = if (lightningFlash > 0.2f) Color(0xFFE2E8F0) else Color(0xFF64748B)
    val fogTop = (horizonY - 20f).coerceAtLeast(0f)
    drawRect(
        brush = Brush.verticalGradient(
            colors = listOf(
                fogColor.copy(alpha = 0.05f * density),
                fogColor.copy(alpha = 0.55f * density + 0.25f * mist)
            ),
            startY = fogTop,
            endY = h
        ),
        topLeft = Offset(0f, fogTop),
        size = Size(w, h - fogTop)
    )
}
