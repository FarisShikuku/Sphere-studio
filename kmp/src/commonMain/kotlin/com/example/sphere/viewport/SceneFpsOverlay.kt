package com.example.sphere.viewport

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Pause
import androidx.compose.material.icons.filled.PlayArrow
import androidx.compose.material3.Icon
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.Immutable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlin.math.roundToInt

/**
 * Target frame rate constants aligned with Khronos present-timing / Swappy 60Hz pacing
 * so high-refresh displays (90/120/144 Hz) pace cleanly to a consistent 60 FPS baseline.
 */
object SceneFpsConfig {
    const val TARGET_FPS: Int = 60
    const val TARGET_FRAME_PERIOD_NS: Long = 16_666_667L
    const val PACING_TOLERANCE_NS: Long = 1_100_000L // ~1.1 ms vsync phase tolerance
}

@Immutable
data class SceneFpsMetrics(
    val fps: Int = SceneFpsConfig.TARGET_FPS,
    val frameTimeMs: Float = 16.67f,
    val renderTimeMs: Float = 0f,
    val targetFps: Int = SceneFpsConfig.TARGET_FPS
)

/**
 * Computes paced scene FPS from native/software frame render duration against [targetFps]
 * without spawning an infinite Compose frame-clock loop (preserving UI test idle compatibility).
 */
fun computePacedSceneFpsMetrics(
    renderTimeMs: Float = 0f,
    targetFps: Int = SceneFpsConfig.TARGET_FPS,
    overrideFps: Int? = null,
    overrideFrameMs: Float? = null
): SceneFpsMetrics {
    val safeTarget = targetFps.coerceAtLeast(1)
    val budgetMs = 1000f / safeTarget.toFloat()
    val effectiveFps = when {
        overrideFps != null -> overrideFps.coerceIn(1, 240)
        renderTimeMs > budgetMs -> (1000f / renderTimeMs).roundToInt().coerceIn(1, safeTarget)
        else -> safeTarget
    }
    val frameMs = overrideFrameMs ?: if (renderTimeMs > budgetMs) renderTimeMs else budgetMs
    return SceneFpsMetrics(
        fps = effectiveFps,
        frameTimeMs = frameMs,
        renderTimeMs = renderTimeMs,
        targetFps = safeTarget
    )
}

/**
 * Compact dark-translucent FPS HUD pill rendered directly over 3D scene viewports.
 */
@Composable
fun SceneFpsBadge(
    modifier: Modifier = Modifier,
    targetFps: Int = SceneFpsConfig.TARGET_FPS,
    renderTimeMs: Float = 0f,
    overrideFps: Int? = null,
    overrideFrameMs: Float? = null,
    tag: String = "scene_fps_badge"
) {
    val metrics = computePacedSceneFpsMetrics(
        renderTimeMs = renderTimeMs,
        targetFps = targetFps,
        overrideFps = overrideFps,
        overrideFrameMs = overrideFrameMs
    )

    val statusColor = when {
        metrics.fps >= (metrics.targetFps * 0.9f) -> Color(0xFF22C55E)
        metrics.fps >= (metrics.targetFps * 0.55f) -> Color(0xFFFACC15)
        else -> Color(0xFFEF4444)
    }

    Surface(
        shape = RoundedCornerShape(14.dp),
        color = Color(0xCC090D16),
        border = BorderStroke(1.dp, statusColor.copy(alpha = 0.42f)),
        modifier = modifier.testTag(tag)
    ) {
        Row(
            modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.spacedBy(5.dp)
        ) {
            Box(
                modifier = Modifier
                    .size(7.dp)
                    .background(statusColor, CircleShape)
            )
            val label = if (metrics.renderTimeMs > 0.05f) {
                "${metrics.fps} FPS • ${"%.1f".format(metrics.renderTimeMs)}ms"
            } else {
                "${metrics.fps} FPS"
            }
            Text(
                text = label,
                color = Color(0xFFE2E8F0),
                fontSize = 10.sp,
                fontWeight = FontWeight.Bold,
                fontFamily = FontFamily.Monospace
            )
        }
    }
}

/**
 * Play / Pause toggle pill for continuous 3D rotation to inspect live system FPS and render latency.
 */
@Composable
fun ScenePlayRotateButton(
    isPlaying: Boolean,
    onToggle: () -> Unit,
    modifier: Modifier = Modifier,
    tag: String = "scene_play_rotate_button"
) {
    val accentColor = if (isPlaying) Color(0xFF22C55E) else Color(0xFF38BDF8)
    val bgColor = if (isPlaying) Color(0xDD0B241A) else Color(0xCC090D16)

    Surface(
        onClick = onToggle,
        shape = RoundedCornerShape(14.dp),
        color = bgColor,
        border = BorderStroke(1.dp, accentColor.copy(alpha = if (isPlaying) 0.65f else 0.42f)),
        modifier = modifier.testTag(tag)
    ) {
        Row(
            modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.spacedBy(3.dp)
        ) {
            Icon(
                imageVector = if (isPlaying) Icons.Filled.Pause else Icons.Filled.PlayArrow,
                contentDescription = if (isPlaying) "Pause Rotation" else "Play Rotation",
                tint = accentColor,
                modifier = Modifier.size(13.dp)
            )
            Text(
                text = if (isPlaying) "Pause" else "Play",
                color = Color(0xFFE2E8F0),
                fontSize = 10.sp,
                fontWeight = FontWeight.Bold,
                fontFamily = FontFamily.Monospace
            )
        }
    }
}
