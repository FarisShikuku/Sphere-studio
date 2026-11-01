package com.example.sphere

import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.GridOn
import androidx.compose.material.icons.filled.Layers
import androidx.compose.material.icons.filled.ViewInAr
import androidx.compose.material.icons.filled.Water
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

/**
 * VulkanStageTab - Controls for the shared 3D Vulkan Scene Stage inside Sky Studio.
 * Allows switching between Ground/Stage modes (Flat Piece, Endless, Water Body, Broken Slab, None),
 * adjusting stage dimensions, and configuring calibration geometry under atmospheric lighting.
 */
@Composable
fun VulkanStageTab(
    surfaceMode: SurfaceSceneMode,
    onSurfaceModeChange: (SurfaceSceneMode) -> Unit,
    stageWidth: Float,
    onStageWidthChange: (Float) -> Unit,
    stageLength: Float,
    onStageLengthChange: (Float) -> Unit,
    stageHeight: Float,
    onStageHeightChange: (Float) -> Unit,
    showGrid: Boolean,
    onShowGridChange: (Boolean) -> Unit,
    showCalibrationItem: Boolean,
    onShowCalibrationItemChange: (Boolean) -> Unit,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier.fillMaxWidth(),
        verticalArrangement = Arrangement.spacedBy(14.dp)
    ) {
        // 1. Surface Scene Mode Selector Card
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column {
                        Text(
                            text = "Shared Vulkan Scene Stage",
                            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                            color = Color.White
                        )
                        Text(
                            text = "3D ground plane receiving celestial lighting & shadows",
                            style = MaterialTheme.typography.labelSmall,
                            color = Color(0xFF94A3B8)
                        )
                    }

                    Icon(
                        imageVector = Icons.Default.Layers,
                        contentDescription = null,
                        tint = MaterialTheme.colorScheme.primary,
                        modifier = Modifier.size(20.dp)
                    )
                }

                Spacer(modifier = Modifier.height(12.dp))

                // Mode Chips
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .horizontalScroll(rememberScrollState()),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    SurfaceSceneMode.entries.forEach { mode ->
                        val isSelected = mode == surfaceMode
                        FilterChip(
                            selected = isSelected,
                            onClick = { onSurfaceModeChange(mode) },
                            label = {
                                Text(
                                    text = mode.shortLabel,
                                    fontSize = 11.sp,
                                    fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal
                                )
                            },
                            colors = FilterChipDefaults.filterChipColors(
                                selectedContainerColor = MaterialTheme.colorScheme.primary,
                                selectedLabelColor = MaterialTheme.colorScheme.onPrimary,
                                containerColor = Color(0xFF1E293B),
                                labelColor = Color(0xFFE2E8F0)
                            ),
                            modifier = Modifier.testTag("stage_mode_chip_${mode.name.lowercase()}")
                        )
                    }
                }

                Spacer(modifier = Modifier.height(8.dp))

                Text(
                    text = surfaceMode.description,
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFF38BDF8),
                    fontSize = 11.sp
                )
            }
        }

        // 2. Calibration Item & Grid Toggles Card
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "Stage Overlays & Objects",
                    style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                    color = Color.White
                )

                Spacer(modifier = Modifier.height(10.dp))

                // Calibration Object Toggle
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column(modifier = Modifier.weight(1f)) {
                        Text(
                            text = "3D Calibration Geometry",
                            style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.SemiBold),
                            color = Color.White
                        )
                        Text(
                            text = "Places reference cube casting dynamic sun shadow",
                            style = MaterialTheme.typography.labelSmall,
                            color = Color(0xFF94A3B8),
                            fontSize = 10.sp
                        )
                    }
                    Switch(
                        checked = showCalibrationItem,
                        onCheckedChange = onShowCalibrationItemChange,
                        colors = SwitchDefaults.colors(
                            checkedThumbColor = Color(0xFF38BDF8),
                            checkedTrackColor = Color(0xFF0284C7),
                            uncheckedThumbColor = Color(0xFF94A3B8),
                            uncheckedTrackColor = Color(0xFF1E293B)
                        ),
                        modifier = Modifier.testTag("switch_calibration_item")
                    )
                }

                HorizontalDivider(modifier = Modifier.padding(vertical = 10.dp), color = Color(0xFF1E293B))

                // Stage Grid Toggle
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column(modifier = Modifier.weight(1f)) {
                        Text(
                            text = "Vulkan Perspective Grid",
                            style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.SemiBold),
                            color = Color.White
                        )
                        Text(
                            text = "Metric grid lines aligned with stage coordinate system",
                            style = MaterialTheme.typography.labelSmall,
                            color = Color(0xFF94A3B8),
                            fontSize = 10.sp
                        )
                    }
                    Switch(
                        checked = showGrid,
                        onCheckedChange = onShowGridChange,
                        colors = SwitchDefaults.colors(
                            checkedThumbColor = Color(0xFF38BDF8),
                            checkedTrackColor = Color(0xFF0284C7),
                            uncheckedThumbColor = Color(0xFF94A3B8),
                            uncheckedTrackColor = Color(0xFF1E293B)
                        ),
                        modifier = Modifier.testTag("switch_stage_grid")
                    )
                }
            }
        }

        // 3. Stage Dimension Sliders (only active when surfaceMode is FLAT_PIECE, WATER_BODY, or BROKEN_SLAB)
        if (surfaceMode == SurfaceSceneMode.FLAT_PIECE ||
            surfaceMode == SurfaceSceneMode.WATER_BODY ||
            surfaceMode == SurfaceSceneMode.BROKEN_SLAB
        ) {
            Card(
                colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
                shape = RoundedCornerShape(12.dp),
                modifier = Modifier.fillMaxWidth()
            ) {
                Column(modifier = Modifier.padding(14.dp)) {
                    Text(
                        text = "Stage Dimensions (Meters)",
                        style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                        color = Color.White
                    )

                    Spacer(modifier = Modifier.height(10.dp))

                    SkySliderItem(
                        label = "Stage Width",
                        value = stageWidth,
                        onValueChange = onStageWidthChange,
                        range = 1.5f..15.0f,
                        displayValue = "${(stageWidth * 10).toInt() / 10f} m"
                    )

                    Spacer(modifier = Modifier.height(8.dp))

                    SkySliderItem(
                        label = "Stage Length",
                        value = stageLength,
                        onValueChange = onStageLengthChange,
                        range = 1.5f..15.0f,
                        displayValue = "${(stageLength * 10).toInt() / 10f} m"
                    )

                    Spacer(modifier = Modifier.height(8.dp))

                    SkySliderItem(
                        label = "Stage Height / Thickness",
                        value = stageHeight,
                        onValueChange = onStageHeightChange,
                        range = 0.05f..1.2f,
                        displayValue = "${(stageHeight * 100).toInt() / 100f} m"
                    )
                }
            }
        }
    }
}
