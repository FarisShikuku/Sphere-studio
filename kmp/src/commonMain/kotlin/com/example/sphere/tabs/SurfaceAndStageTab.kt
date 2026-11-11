package com.example.sphere

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.AutoAwesome
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.FilterChip
import androidx.compose.material3.FilterChipDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Slider
import androidx.compose.material3.SliderDefaults
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

// -------------------------------------------------------------
// CONTROL TAB 3: STAGE & SURFACE (SCENE SURFACE PIECES & PBR)
// -------------------------------------------------------------
@Composable
fun SurfaceAndStageTab(
    surfaceMode: SurfaceSceneMode,
    onSurfaceModeSelect: (SurfaceSceneMode) -> Unit,
    stageWidth: Float,
    onStageWidthChange: (Float) -> Unit,
    stageLength: Float,
    onStageLengthChange: (Float) -> Unit,
    stageHeight: Float,
    onStageHeightChange: (Float) -> Unit,
    itemSize: Float,
    isAutoFit: Boolean = true,
    onApplyAutoFit: () -> Unit,
    selectedColor: ColorOption,
    onColorSelect: (ColorOption) -> Unit,
    selectedPattern: SurfacePattern,
    onPatternSelect: (SurfacePattern) -> Unit
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(bottom = 28.dp)
            .testTag("surface_and_stage_tab")
    ) {
        // 1. Surface Scene Mode Selector
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                text = "Surface Scene Environment",
                style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                color = Color.White
            )
            Surface(
                shape = RoundedCornerShape(4.dp),
                color = Color(0xFF0369A1).copy(alpha = 0.35f)
            ) {
                Text(
                    text = "${surfaceMode.icon} ${surfaceMode.shortLabel}",
                    style = MaterialTheme.typography.labelSmall,
                    color = Color(0xFF38BDF8),
                    fontWeight = FontWeight.Bold,
                    modifier = Modifier.padding(horizontal = 6.dp, vertical = 2.dp)
                )
            }
        }
        Spacer(modifier = Modifier.height(10.dp))

        // Mode Chips Row
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            SurfaceSceneMode.values().forEach { mode ->
                val isSelected = mode == surfaceMode
                FilterChip(
                    selected = isSelected,
                    onClick = { onSurfaceModeSelect(mode) },
                    label = {
                        Text(
                            text = "${mode.icon} ${mode.title}",
                            fontSize = 12.sp,
                            fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal
                        )
                    },
                    colors = FilterChipDefaults.filterChipColors(
                        selectedContainerColor = Color(0xFF0284C7).copy(alpha = 0.35f),
                        selectedLabelColor = Color(0xFF38BDF8),
                        containerColor = Color(0xFF131722),
                        labelColor = Color(0xFF94A3B8)
                    ),
                    border = FilterChipDefaults.filterChipBorder(
                        borderColor = if (isSelected) Color(0xFF0284C7) else Color(0xFF1E293B),
                        selectedBorderColor = Color(0xFF38BDF8),
                        enabled = true,
                        selected = isSelected
                    ),
                    modifier = Modifier.testTag("surface_mode_chip_${mode.name.lowercase()}")
                )
            }
        }

        Spacer(modifier = Modifier.height(8.dp))

        // Mode Description & Auto-Fit guidance card
        Card(
            modifier = Modifier.fillMaxWidth(),
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            border = CardDefaults.outlinedCardBorder().copy(
                brush = Brush.linearGradient(listOf(Color(0xFF1E293B), Color(0xFF0F172A)))
            )
        ) {
            Column(modifier = Modifier.padding(12.dp)) {
                Text(
                    text = surfaceMode.description,
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFFCBD5E1)
                )
                if (surfaceMode == SurfaceSceneMode.FLAT_PIECE) {
                    Spacer(modifier = Modifier.height(6.dp))
                    Text(
                        text = if (itemSize <= 1.25f)
                            "• Relative Size Rule: Active item is compact (${((itemSize * 100).toInt()) / 100f}m) -> Default 3m × 3m piece"
                        else
                            "• Relative Size Rule: Active item footprint is ${((itemSize * 100).toInt()) / 100f}m -> Default 10m × 10m piece",
                        style = MaterialTheme.typography.labelSmall,
                        color = Color(0xFF38BDF8),
                        fontFamily = FontFamily.Monospace
                    )
                } else if (surfaceMode == SurfaceSceneMode.WATER_BODY) {
                    Spacer(modifier = Modifier.height(6.dp))
                    Text(
                        text = "• Water Body Default: 10m × 10m piece with animated caustics & ripple reflections",
                        style = MaterialTheme.typography.labelSmall,
                        color = Color(0xFF06B6D4),
                        fontFamily = FontFamily.Monospace
                    )
                }
            }
        }

        // 2. Stage Dimensions & Adjustments (When not NONE or ENDLESS)
        if (surfaceMode == SurfaceSceneMode.FLAT_PIECE || surfaceMode == SurfaceSceneMode.WATER_BODY || surfaceMode == SurfaceSceneMode.BROKEN_SLAB) {
            Spacer(modifier = Modifier.height(16.dp))

            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = "Piece Dimensions & Presets",
                    style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                    color = Color.White
                )
                Text(
                    text = "${((stageWidth * 10).toInt()) / 10f}m × ${((stageLength * 10).toInt()) / 10f}m",
                    style = MaterialTheme.typography.labelSmall,
                    color = Color(0xFF38BDF8),
                    fontFamily = FontFamily.Monospace,
                    fontWeight = FontWeight.Bold
                )
            }

            Spacer(modifier = Modifier.height(8.dp))

            // Presets: 3x3m (small item), 10x10m (default piece), 20x20m, Auto-Fit
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState()),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = if (stageWidth == 3.0f && stageLength == 3.0f) Color(0xFF0284C7).copy(alpha = 0.35f) else Color(0xFF1E293B),
                    border = CardDefaults.outlinedCardBorder().copy(
                        brush = Brush.linearGradient(
                            if (stageWidth == 3.0f && stageLength == 3.0f) listOf(Color(0xFF38BDF8), Color(0xFF0284C7))
                            else listOf(Color(0xFF334155), Color(0xFF1E293B))
                        )
                    ),
                    modifier = Modifier
                        .clickable {
                            onStageWidthChange(3.0f)
                            onStageLengthChange(3.0f)
                        }
                        .testTag("preset_3x3m_button")
                ) {
                    Text(
                        text = "3×3m (Small ~1m Item)",
                        style = MaterialTheme.typography.labelSmall,
                        color = if (stageWidth == 3.0f && stageLength == 3.0f) Color(0xFF38BDF8) else Color(0xFFCBD5E1),
                        fontWeight = FontWeight.SemiBold,
                        modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp)
                    )
                }

                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = if (stageWidth == 10.0f && stageLength == 10.0f) Color(0xFF0284C7).copy(alpha = 0.35f) else Color(0xFF1E293B),
                    border = CardDefaults.outlinedCardBorder().copy(
                        brush = Brush.linearGradient(
                            if (stageWidth == 10.0f && stageLength == 10.0f) listOf(Color(0xFF38BDF8), Color(0xFF0284C7))
                            else listOf(Color(0xFF334155), Color(0xFF1E293B))
                        )
                    ),
                    modifier = Modifier
                        .clickable {
                            onStageWidthChange(10.0f)
                            onStageLengthChange(10.0f)
                        }
                        .testTag("preset_10x10m_button")
                ) {
                    Text(
                        text = "10×10m (Default Piece)",
                        style = MaterialTheme.typography.labelSmall,
                        color = if (stageWidth == 10.0f && stageLength == 10.0f) Color(0xFF38BDF8) else Color(0xFFCBD5E1),
                        fontWeight = FontWeight.SemiBold,
                        modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp)
                    )
                }

                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = if (stageWidth == 20.0f && stageLength == 20.0f) Color(0xFF0284C7).copy(alpha = 0.35f) else Color(0xFF1E293B),
                    border = CardDefaults.outlinedCardBorder().copy(
                        brush = Brush.linearGradient(
                            if (stageWidth == 20.0f && stageLength == 20.0f) listOf(Color(0xFF38BDF8), Color(0xFF0284C7))
                            else listOf(Color(0xFF334155), Color(0xFF1E293B))
                        )
                    ),
                    modifier = Modifier
                        .clickable {
                            onStageWidthChange(20.0f)
                            onStageLengthChange(20.0f)
                        }
                        .testTag("preset_20x20m_button")
                ) {
                    Text(
                        text = "20×20m (Large Piece)",
                        style = MaterialTheme.typography.labelSmall,
                        color = if (stageWidth == 20.0f && stageLength == 20.0f) Color(0xFF38BDF8) else Color(0xFFCBD5E1),
                        fontWeight = FontWeight.SemiBold,
                        modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp)
                    )
                }

                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = if (stageWidth == 100.0f && stageLength == 100.0f) Color(0xFF0284C7).copy(alpha = 0.35f) else Color(0xFF1E293B),
                    border = CardDefaults.outlinedCardBorder().copy(
                        brush = Brush.linearGradient(
                            if (stageWidth == 100.0f && stageLength == 100.0f) listOf(Color(0xFF38BDF8), Color(0xFF0284C7))
                            else listOf(Color(0xFF334155), Color(0xFF1E293B))
                        )
                    ),
                    modifier = Modifier
                        .clickable {
                            onStageWidthChange(100.0f)
                            onStageLengthChange(100.0f)
                        }
                        .testTag("preset_100x100m_button")
                ) {
                    Text(
                        text = "100×100m (Mega Stage)",
                        style = MaterialTheme.typography.labelSmall,
                        color = if (stageWidth == 100.0f && stageLength == 100.0f) Color(0xFF38BDF8) else Color(0xFFCBD5E1),
                        fontWeight = FontWeight.SemiBold,
                        modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp)
                    )
                }

                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = if (isAutoFit) Color(0xFF0D9488) else Color(0xFF0F766E).copy(alpha = 0.4f),
                    border = CardDefaults.outlinedCardBorder().copy(
                        brush = Brush.linearGradient(
                            if (isAutoFit) listOf(Color(0xFF2DD4BF), Color(0xFF0D9488))
                            else listOf(Color(0xFF14B8A6), Color(0xFF0F766E))
                        )
                    ),
                    modifier = Modifier
                        .clickable { onApplyAutoFit() }
                        .testTag("auto_fit_stage_button")
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Icon(
                            imageVector = Icons.Default.AutoAwesome,
                            contentDescription = null,
                            tint = if (isAutoFit) Color.White else Color(0xFF5EEAD4),
                            modifier = Modifier.size(13.dp)
                        )
                        Spacer(modifier = Modifier.width(4.dp))
                        Text(
                            text = if (isAutoFit) "Auto-Fit: ON (${"%.1f".format(stageWidth)}m)" else "Auto-Fit (${"%.1f".format(itemSize * 2.8f)}m)",
                            style = MaterialTheme.typography.labelSmall,
                            color = if (isAutoFit) Color.White else Color(0xFF5EEAD4),
                            fontWeight = FontWeight.Bold
                        )
                    }
                }
            }

            Spacer(modifier = Modifier.height(12.dp))

            // Width Slider (allowing to exceed 100 meters)
            Text(
                text = "Piece Width: ${((stageWidth * 10).toInt()) / 10f} m",
                style = MaterialTheme.typography.bodySmall,
                color = Color(0xFFCBD5E1)
            )
            Slider(
                value = stageWidth,
                onValueChange = onStageWidthChange,
                valueRange = 1.0f..150.0f,
                colors = SliderDefaults.colors(
                    thumbColor = Color(0xFF38BDF8),
                    activeTrackColor = Color(0xFF0284C7),
                    inactiveTrackColor = Color(0xFF1E293B)
                ),
                modifier = Modifier.testTag("stage_width_slider")
            )

            // Length Slider (allowing to exceed 100 meters)
            Text(
                text = "Piece Length: ${((stageLength * 10).toInt()) / 10f} m",
                style = MaterialTheme.typography.bodySmall,
                color = Color(0xFFCBD5E1)
            )
            Slider(
                value = stageLength,
                onValueChange = onStageLengthChange,
                valueRange = 1.0f..150.0f,
                colors = SliderDefaults.colors(
                    thumbColor = Color(0xFF38BDF8),
                    activeTrackColor = Color(0xFF0284C7),
                    inactiveTrackColor = Color(0xFF1E293B)
                ),
                modifier = Modifier.testTag("stage_length_slider")
            )

            // Skirt / Thickness Slider
            Text(
                text = "Platform Thickness: ${((stageHeight * 100).toInt())} cm",
                style = MaterialTheme.typography.bodySmall,
                color = Color(0xFFCBD5E1)
            )
            Slider(
                value = stageHeight,
                onValueChange = onStageHeightChange,
                valueRange = 0.1f..2.5f,
                colors = SliderDefaults.colors(
                    thumbColor = Color(0xFF38BDF8),
                    activeTrackColor = Color(0xFF0284C7),
                    inactiveTrackColor = Color(0xFF1E293B)
                ),
                modifier = Modifier.testTag("stage_height_slider")
            )
        }

        Spacer(modifier = Modifier.height(20.dp))

        // 3. Item Surface Color Palette & Pattern
        Text(
            text = "Item Surface Colour Palette",
            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
            color = Color.White
        )
        Spacer(modifier = Modifier.height(10.dp))

        Row(
            modifier = Modifier
                .fillMaxWidth()
                .horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(10.dp)
        ) {
            paletteColors.forEach { opt ->
                val isSelected = opt.name == selectedColor.name
                Column(
                    horizontalAlignment = Alignment.CenterHorizontally,
                    modifier = Modifier
                        .clickable { onColorSelect(opt) }
                        .padding(2.dp)
                ) {
                    Box(
                        modifier = Modifier
                            .size(38.dp)
                            .clip(CircleShape)
                            .background(opt.color)
                            .border(
                                width = if (isSelected) 3.dp else 1.dp,
                                color = if (isSelected) Color.White else Color(0xFF334155),
                                shape = CircleShape
                            )
                    )
                    Spacer(modifier = Modifier.height(4.dp))
                    Text(
                        text = opt.name.split(" ").last(),
                        style = MaterialTheme.typography.labelSmall,
                        color = if (isSelected) Color.White else Color(0xFF64748B),
                        fontSize = 10.sp
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(20.dp))

        Text(
            text = "Item Procedural PBR Pattern",
            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
            color = Color.White
        )
        Spacer(modifier = Modifier.height(10.dp))

        Row(
            modifier = Modifier
                .fillMaxWidth()
                .horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            SurfacePattern.values().forEach { pattern ->
                val isSelected = pattern == selectedPattern
                FilterChip(
                    selected = isSelected,
                    onClick = { onPatternSelect(pattern) },
                    label = { Text(pattern.label, fontSize = 12.sp) },
                    colors = FilterChipDefaults.filterChipColors(
                        selectedContainerColor = Color(0xFF0284C7).copy(alpha = 0.35f),
                        selectedLabelColor = Color(0xFF38BDF8),
                        containerColor = Color(0xFF131722),
                        labelColor = Color(0xFF94A3B8)
                    ),
                    border = FilterChipDefaults.filterChipBorder(
                        borderColor = if (isSelected) Color(0xFF0284C7) else Color(0xFF1E293B),
                        selectedBorderColor = Color(0xFF38BDF8),
                        enabled = true,
                        selected = isSelected
                    )
                )
            }
        }
    }
}
