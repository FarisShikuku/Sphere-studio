package com.example.sphere.text.tabs

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Pause
import androidx.compose.material.icons.filled.PlayArrow
import androidx.compose.material.icons.filled.RestartAlt
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.TextBadgeStyle
import com.example.sphere.model.TextRenderMode

@Composable
fun WorldAndBillboardTab(
    renderMode: TextRenderMode,
    onRenderModeChange: (TextRenderMode) -> Unit,
    zoomFactor: Float,
    onZoomFactorChange: (Float) -> Unit,
    rotX: Float,
    onRotXChange: (Float) -> Unit,
    rotY: Float,
    onRotYChange: (Float) -> Unit,
    autoRotate: Boolean,
    onToggleAutoRotate: (Boolean) -> Unit,
    badgeStyle: TextBadgeStyle,
    onBadgeStyleChange: (TextBadgeStyle) -> Unit,
    onResetCamera: () -> Unit,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(14.dp)
    ) {
        // VULKAN RENDER MODE
        Text(
            "VULKAN RENDER MODE",
            style = MaterialTheme.typography.labelSmall.copy(
                fontWeight = FontWeight.Bold,
                color = MaterialTheme.colorScheme.primary,
                letterSpacing = 1.sp
            )
        )
        Spacer(Modifier.height(8.dp))
        Column(verticalArrangement = Arrangement.spacedBy(6.dp)) {
            TextRenderMode.entries.forEach { mode ->
                val isSelected = renderMode == mode
                Card(
                    onClick = { onRenderModeChange(mode) },
                    colors = CardDefaults.cardColors(
                        containerColor = if (isSelected) MaterialTheme.colorScheme.primaryContainer else MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f)
                    ),
                    modifier = Modifier.fillMaxWidth().testTag("render_mode_${mode.name}")
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 12.dp, vertical = 10.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        RadioButton(selected = isSelected, onClick = { onRenderModeChange(mode) })
                        Spacer(Modifier.width(8.dp))
                        Column {
                            Text(
                                mode.label,
                                style = MaterialTheme.typography.bodyMedium.copy(
                                    fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal
                                )
                            )
                            Text(
                                mode.description,
                                style = MaterialTheme.typography.labelSmall.copy(
                                    color = MaterialTheme.colorScheme.onSurfaceVariant,
                                    fontSize = 10.sp
                                )
                            )
                        }
                    }
                }
            }
        }

        Spacer(Modifier.height(16.dp))
        HorizontalDivider(color = MaterialTheme.colorScheme.surfaceVariant)
        Spacer(Modifier.height(16.dp))

        // SDF ZOOM MAGNIFICATION TEST
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Column {
                Text(
                    "SDF VECTOR ZOOM TEST",
                    style = MaterialTheme.typography.labelSmall.copy(
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary,
                        letterSpacing = 1.sp
                    )
                )
                Text(
                    "Observe zero pixelation even at 5.0x magnification",
                    style = MaterialTheme.typography.bodySmall.copy(color = MaterialTheme.colorScheme.onSurfaceVariant)
                )
            }
            Text(
                "${"%.1f".format(zoomFactor)}x",
                style = MaterialTheme.typography.titleMedium.copy(
                    fontWeight = FontWeight.Bold,
                    color = MaterialTheme.colorScheme.primary
                )
            )
        }
        Slider(
            value = zoomFactor,
            onValueChange = onZoomFactorChange,
            valueRange = 0.6f..5.0f,
            modifier = Modifier.testTag("zoom_factor_slider")
        )

        Spacer(Modifier.height(14.dp))
        HorizontalDivider(color = MaterialTheme.colorScheme.surfaceVariant)
        Spacer(Modifier.height(14.dp))

        // 3D CAMERA & ORBIT ROTATION
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                "3D CAMERA ORBIT CONTROLS",
                style = MaterialTheme.typography.labelSmall.copy(
                    fontWeight = FontWeight.Bold,
                    color = MaterialTheme.colorScheme.primary,
                    letterSpacing = 1.sp
                )
            )
            Row {
                IconButton(
                    onClick = { onToggleAutoRotate(!autoRotate) },
                    modifier = Modifier.testTag("auto_orbit_button")
                ) {
                    Icon(
                        if (autoRotate) Icons.Default.Pause else Icons.Default.PlayArrow,
                        contentDescription = "Toggle Orbit"
                    )
                }
                IconButton(
                    onClick = onResetCamera,
                    modifier = Modifier.testTag("reset_camera_button")
                ) {
                    Icon(Icons.Default.RestartAlt, contentDescription = "Reset Camera")
                }
            }
        }

        Spacer(Modifier.height(6.dp))
        Text(
            "Pitch (Elevation): ${rotX.toInt()}°",
            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Medium)
        )
        Slider(
            value = rotX,
            onValueChange = onRotXChange,
            valueRange = -45f..75f,
            modifier = Modifier.testTag("camera_pitch_slider")
        )

        Spacer(Modifier.height(6.dp))
        Text(
            "Yaw (Azimuth): ${rotY.toInt()}°",
            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Medium)
        )
        Slider(
            value = rotY,
            onValueChange = onRotYChange,
            valueRange = -180f..180f,
            modifier = Modifier.testTag("camera_yaw_slider")
        )

        Spacer(Modifier.height(14.dp))
        HorizontalDivider(color = MaterialTheme.colorScheme.surfaceVariant)
        Spacer(Modifier.height(14.dp))

        // BACKGROUND BADGE / FRAME STYLE
        Text(
            "BACKGROUND PLAQUE / FRAME STYLE",
            style = MaterialTheme.typography.labelSmall.copy(
                fontWeight = FontWeight.Bold,
                color = MaterialTheme.colorScheme.primary,
                letterSpacing = 1.sp
            )
        )
        Spacer(Modifier.height(8.dp))
        Column(verticalArrangement = Arrangement.spacedBy(6.dp)) {
            TextBadgeStyle.entries.forEach { style ->
                val isSelected = badgeStyle == style
                Card(
                    onClick = { onBadgeStyleChange(style) },
                    colors = CardDefaults.cardColors(
                        containerColor = if (isSelected) MaterialTheme.colorScheme.secondaryContainer else MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.4f)
                    ),
                    modifier = Modifier.fillMaxWidth().testTag("badge_style_${style.name}")
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 12.dp, vertical = 8.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        RadioButton(selected = isSelected, onClick = { onBadgeStyleChange(style) })
                        Spacer(Modifier.width(8.dp))
                        Text(
                            style.label,
                            style = MaterialTheme.typography.bodyMedium.copy(
                                fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal
                            )
                        )
                    }
                }
            }
        }
    }
}
