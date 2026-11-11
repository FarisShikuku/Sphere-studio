package com.example.sphere

import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.AutoAwesome
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

// -------------------------------------------------------------
// CONTROL TAB 1: DIMENSIONS & SCALING
// -------------------------------------------------------------
@Composable
fun DimensionsControlTab(
    width: Float,
    height: Float,
    depth: Float,
    scale: Float,
    isAutoFit: Boolean = true,
    onWidthChange: (Float) -> Unit,
    onHeightChange: (Float) -> Unit,
    onDepthChange: (Float) -> Unit,
    onScaleChange: (Float) -> Unit,
    onReset: () -> Unit
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(bottom = 20.dp)
            .testTag("dimensions_control_tab")
    ) {
        // Top Header with Reset
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Column {
                Text(
                    text = "Geometry Dimensions",
                    style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                    color = Color.White
                )
                Text(
                    text = "Parametric Metric Bounds",
                    style = MaterialTheme.typography.bodySmall.copy(
                        color = MaterialTheme.colorScheme.onSurfaceVariant,
                        fontSize = 11.sp
                    )
                )
            }
            Surface(
                shape = RoundedCornerShape(6.dp),
                color = Color(0xFF1E293B),
                modifier = Modifier
                    .clickable { onReset() }
                    .testTag("reset_dimensions_button")
            ) {
                Text(
                    text = "Reset Default",
                    style = MaterialTheme.typography.labelSmall,
                    color = Color(0xFF38BDF8),
                    fontWeight = FontWeight.SemiBold,
                    modifier = Modifier.padding(horizontal = 10.dp, vertical = 5.dp)
                )
            }
        }

        Spacer(modifier = Modifier.height(10.dp))

        // Quick Presets Row
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            FilterChip(
                selected = (width == 1.0f && height == 1.0f && depth == 1.0f),
                onClick = {
                    onWidthChange(1.0f)
                    onHeightChange(1.0f)
                    onDepthChange(1.0f)
                },
                label = { Text("1.0m Cube", fontSize = 11.sp) }
            )
            FilterChip(
                selected = (width == 1.6f && height == 1.6f && depth == 1.6f),
                onClick = {
                    onWidthChange(1.6f)
                    onHeightChange(1.6f)
                    onDepthChange(1.6f)
                },
                label = { Text("1.6m Studio", fontSize = 11.sp) }
            )
            FilterChip(
                selected = (width == 0.8f && height == 2.4f && depth == 0.8f),
                onClick = {
                    onWidthChange(0.8f)
                    onHeightChange(2.4f)
                    onDepthChange(0.8f)
                },
                label = { Text("2.4m Pillar", fontSize = 11.sp) }
            )
            FilterChip(
                selected = (width == 2.4f && height == 0.6f && depth == 1.2f),
                onClick = {
                    onWidthChange(2.4f)
                    onHeightChange(0.6f)
                    onDepthChange(1.2f)
                },
                label = { Text("Low Slab", fontSize = 11.sp) }
            )
            FilterChip(
                selected = (width == 100.0f && height == 100.0f && depth == 100.0f),
                onClick = {
                    onWidthChange(100.0f)
                    onHeightChange(100.0f)
                    onDepthChange(100.0f)
                },
                label = { Text("100m Complex", fontSize = 11.sp) }
            )
            FilterChip(
                selected = (width == 120.0f && height == 25.0f && depth == 120.0f),
                onClick = {
                    onWidthChange(120.0f)
                    onHeightChange(25.0f)
                    onDepthChange(120.0f)
                },
                label = { Text("120m Superfield", fontSize = 11.sp) }
            )
        }

        Spacer(modifier = Modifier.height(10.dp))

        // Live Dimension Sliders (allowing measurements to exceed 100 meters)
        DimensionSlider(
            label = "Width (X-Axis)",
            value = width.coerceIn(0.4f, 150.0f),
            range = 0.4f..150.0f,
            unit = "m",
            accentColor = Color(0xFFEF4444),
            onValueChange = onWidthChange
        )

        DimensionSlider(
            label = "Height (Y-Axis)",
            value = height.coerceIn(0.4f, 150.0f),
            range = 0.4f..150.0f,
            unit = "m",
            accentColor = Color(0xFF10B981),
            onValueChange = onHeightChange
        )

        DimensionSlider(
            label = "Depth (Z-Axis)",
            value = depth.coerceIn(0.4f, 150.0f),
            range = 0.4f..150.0f,
            unit = "m",
            accentColor = Color(0xFF3B82F6),
            onValueChange = onDepthChange
        )

        DimensionSlider(
            label = "Overall Scale",
            value = scale.coerceIn(0.1f, 10.0f),
            range = 0.1f..10.0f,
            unit = "x",
            accentColor = Color(0xFFF59E0B),
            onValueChange = onScaleChange
        )

        Spacer(modifier = Modifier.height(12.dp))

        // Live Metric Card
        Card(
            modifier = Modifier.fillMaxWidth(),
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(10.dp)
        ) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(horizontal = 14.dp, vertical = 10.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Column {
                    Text(
                        text = "Footprint: ${"%.2f".format(width * depth * scale * scale)} m²  •  Vol: ${"%.2f".format(width * height * depth * scale * scale * scale)} m³",
                        style = MaterialTheme.typography.bodySmall,
                        color = Color(0xFFCBD5E1),
                        fontFamily = FontFamily.Monospace
                    )
                    Text(
                        text = "Bounding Extent: ${"%.2f".format(maxOf(width, height, depth) * scale)}m max",
                        style = MaterialTheme.typography.labelSmall,
                        color = Color(0xFF94A3B8)
                    )
                }

                if (isAutoFit) {
                    Surface(
                        shape = RoundedCornerShape(6.dp),
                        color = Color(0xFF0F766E).copy(alpha = 0.35f)
                    ) {
                        Row(
                            modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Icon(
                                Icons.Default.AutoAwesome,
                                contentDescription = null,
                                tint = Color(0xFF5EEAD4),
                                modifier = Modifier.size(12.dp)
                            )
                            Spacer(Modifier.width(4.dp))
                            Text(
                                "Auto-Fit: ON",
                                color = Color(0xFF5EEAD4),
                                fontSize = 10.sp,
                                fontWeight = FontWeight.Bold
                            )
                        }
                    }
                }
            }
        }
    }
}

@Composable
fun DimensionSlider(
    label: String,
    value: Float,
    range: ClosedFloatingPointRange<Float>,
    unit: String,
    accentColor: Color,
    onValueChange: (Float) -> Unit
) {
    Column(modifier = Modifier.padding(vertical = 3.dp)) {
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween
        ) {
            Text(
                text = label,
                style = MaterialTheme.typography.bodySmall,
                color = Color(0xFFCBD5E1)
            )
            Text(
                text = "${"%.2f".format(value)} $unit",
                style = MaterialTheme.typography.labelSmall,
                color = accentColor,
                fontFamily = FontFamily.Monospace,
                fontWeight = FontWeight.Bold
            )
        }
        Slider(
            value = value,
            onValueChange = onValueChange,
            valueRange = range,
            colors = SliderDefaults.colors(
                thumbColor = accentColor,
                activeTrackColor = accentColor,
                inactiveTrackColor = Color(0xFF1E293B)
            ),
            modifier = Modifier.height(28.dp)
        )
    }
}
