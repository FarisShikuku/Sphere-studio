package com.example.sphere.text.tabs

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyRow
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Check
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

private val strokeColors = listOf(
    Color(0xFF0A0E17) to "Obsidian Black",
    Color(0xFF1E2838) to "Slate Navy",
    Color(0xFF032238) to "Deep Cyan",
    Color(0xFF321A04) to "Deep Amber",
    Color(0xFF280A0A) to "Deep Maroon",
    Color(0xFFFFFFFF) to "Pure White"
)

private val glowPalette = listOf(
    Color(0xFF29DCFF) to "Neon Cyan",
    Color(0xFF2BF69A) to "Phosphor Green",
    Color(0xFFFF4838) to "Alert Red",
    Color(0xFFFFD56B) to "Solar Gold",
    Color(0xFFBA68C8) to "Cyber Violet"
)

@Composable
fun SdfShadingTab(
    smoothing: Float,
    onSmoothingChange: (Float) -> Unit,
    thicknessBias: Float,
    onThicknessBiasChange: (Float) -> Unit,
    enableOutline: Boolean,
    onToggleOutline: (Boolean) -> Unit,
    outlineWidth: Float,
    onOutlineWidthChange: (Float) -> Unit,
    outlineColor: Color,
    onOutlineColorChange: (Color) -> Unit,
    enableShadow: Boolean,
    onToggleShadow: (Boolean) -> Unit,
    shadowOffsetX: Float,
    onShadowOffsetXChange: (Float) -> Unit,
    shadowOffsetY: Float,
    onShadowOffsetYChange: (Float) -> Unit,
    shadowSoftness: Float,
    onShadowSoftnessChange: (Float) -> Unit,
    shadowColor: Color,
    onShadowColorChange: (Color) -> Unit,
    enableGlow: Boolean,
    onToggleGlow: (Boolean) -> Unit,
    glowRadius: Float,
    onGlowRadiusChange: (Float) -> Unit,
    glowColor: Color,
    onGlowColorChange: (Color) -> Unit,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(14.dp)
    ) {
        // SASCHA WILLEMS SDF CORE CONTROLS
        Text(
            "SASCHA WILLEMS SDF CORE METRICS",
            style = MaterialTheme.typography.labelSmall.copy(
                fontWeight = FontWeight.Bold,
                color = MaterialTheme.colorScheme.primary,
                letterSpacing = 1.sp
            )
        )
        Spacer(Modifier.height(8.dp))

        // Subpixel Antialiasing Smoothing
        Text(
            "Edge Smoothing (smoothstep delta): ${"%.3f".format(smoothing)}",
            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Medium)
        )
        Slider(
            value = smoothing,
            onValueChange = onSmoothingChange,
            valueRange = 0.010f..0.120f,
            modifier = Modifier.testTag("sdf_smoothing_slider")
        )

        Spacer(Modifier.height(8.dp))

        // Character Dilation / Thickness Bias
        Text(
            "Font Thickness Bias (Dilation): ${"%.2f".format(thicknessBias)}",
            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Medium)
        )
        Slider(
            value = thicknessBias,
            onValueChange = onThicknessBiasChange,
            valueRange = -0.15f..0.25f,
            modifier = Modifier.testTag("sdf_thickness_slider")
        )

        Spacer(Modifier.height(14.dp))
        HorizontalDivider(color = MaterialTheme.colorScheme.surfaceVariant)
        Spacer(Modifier.height(14.dp))

        // OUTLINE STROKE SECTION
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Column {
                Text(
                    "OUTLINE STROKE",
                    style = MaterialTheme.typography.labelSmall.copy(
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary,
                        letterSpacing = 1.sp
                    )
                )
                Text(
                    "Shader dual-threshold distance field stroke",
                    style = MaterialTheme.typography.bodySmall.copy(color = MaterialTheme.colorScheme.onSurfaceVariant)
                )
            }
            Switch(
                checked = enableOutline,
                onCheckedChange = onToggleOutline,
                modifier = Modifier.testTag("toggle_outline_switch")
            )
        }

        AnimatedVisibility(visible = enableOutline) {
            Column(modifier = Modifier.padding(top = 10.dp)) {
                Text(
                    "Outline Width: ${"%.2f".format(outlineWidth)}",
                    style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Medium)
                )
                Slider(
                    value = outlineWidth,
                    onValueChange = onOutlineWidthChange,
                    valueRange = 0.02f..0.45f,
                    modifier = Modifier.testTag("outline_width_slider")
                )

                Spacer(Modifier.height(8.dp))
                Text(
                    "Outline Color:",
                    style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Medium)
                )
                Spacer(Modifier.height(6.dp))
                LazyRow(horizontalArrangement = Arrangement.spacedBy(10.dp)) {
                    items(strokeColors) { (col, name) ->
                        val isSelected = outlineColor == col
                        Box(
                            modifier = Modifier
                                .size(32.dp)
                                .clip(CircleShape)
                                .background(col)
                                .clickable { onOutlineColorChange(col) }
                                .then(
                                    if (isSelected) Modifier.border(3.dp, MaterialTheme.colorScheme.primary, CircleShape)
                                    else Modifier.border(1.dp, Color.Gray, CircleShape)
                                ),
                            contentAlignment = Alignment.Center
                        ) {
                            if (isSelected) {
                                Icon(
                                    Icons.Default.Check,
                                    contentDescription = name,
                                    tint = if (col == Color.White) Color.Black else Color.White,
                                    modifier = Modifier.size(16.dp)
                                )
                            }
                        }
                    }
                }
            }
        }

        Spacer(Modifier.height(14.dp))
        HorizontalDivider(color = MaterialTheme.colorScheme.surfaceVariant)
        Spacer(Modifier.height(14.dp))

        // DROP SHADOW SECTION
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Column {
                Text(
                    "DROP SHADOW",
                    style = MaterialTheme.typography.labelSmall.copy(
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary,
                        letterSpacing = 1.sp
                    )
                )
                Text(
                    "UV-offset secondary distance field evaluation",
                    style = MaterialTheme.typography.bodySmall.copy(color = MaterialTheme.colorScheme.onSurfaceVariant)
                )
            }
            Switch(
                checked = enableShadow,
                onCheckedChange = onToggleShadow,
                modifier = Modifier.testTag("toggle_shadow_switch")
            )
        }

        AnimatedVisibility(visible = enableShadow) {
            Column(modifier = Modifier.padding(top = 10.dp)) {
                Text(
                    "Shadow Offset X: ${shadowOffsetX.toInt()} px",
                    style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Medium)
                )
                Slider(
                    value = shadowOffsetX,
                    onValueChange = onShadowOffsetXChange,
                    valueRange = -10f..10f,
                    modifier = Modifier.testTag("shadow_offset_x_slider")
                )

                Spacer(Modifier.height(6.dp))

                Text(
                    "Shadow Offset Y: ${shadowOffsetY.toInt()} px",
                    style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Medium)
                )
                Slider(
                    value = shadowOffsetY,
                    onValueChange = onShadowOffsetYChange,
                    valueRange = -10f..10f,
                    modifier = Modifier.testTag("shadow_offset_y_slider")
                )

                Spacer(Modifier.height(6.dp))

                Text(
                    "Shadow Blur / Softness: ${"%.2f".format(shadowSoftness)}",
                    style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Medium)
                )
                Slider(
                    value = shadowSoftness,
                    onValueChange = onShadowSoftnessChange,
                    valueRange = 0.05f..0.90f,
                    modifier = Modifier.testTag("shadow_softness_slider")
                )
            }
        }

        Spacer(Modifier.height(14.dp))
        HorizontalDivider(color = MaterialTheme.colorScheme.surfaceVariant)
        Spacer(Modifier.height(14.dp))

        // EMISSIVE GLOW & BLOOM SECTION
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Column {
                Text(
                    "EMISSIVE GLOW & BLOOM",
                    style = MaterialTheme.typography.labelSmall.copy(
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary,
                        letterSpacing = 1.sp
                    )
                )
                Text(
                    "Radial cyber glow and bloom halo",
                    style = MaterialTheme.typography.bodySmall.copy(color = MaterialTheme.colorScheme.onSurfaceVariant)
                )
            }
            Switch(
                checked = enableGlow,
                onCheckedChange = onToggleGlow,
                modifier = Modifier.testTag("toggle_glow_switch")
            )
        }

        AnimatedVisibility(visible = enableGlow) {
            Column(modifier = Modifier.padding(top = 10.dp)) {
                Text(
                    "Glow Radius: ${"%.2f".format(glowRadius)}",
                    style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Medium)
                )
                Slider(
                    value = glowRadius,
                    onValueChange = onGlowRadiusChange,
                    valueRange = 0.10f..0.80f,
                    modifier = Modifier.testTag("glow_radius_slider")
                )

                Spacer(Modifier.height(8.dp))
                Text(
                    "Glow Tint:",
                    style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Medium)
                )
                Spacer(Modifier.height(6.dp))
                LazyRow(horizontalArrangement = Arrangement.spacedBy(10.dp)) {
                    items(glowPalette) { (col, name) ->
                        val isSelected = glowColor == col
                        Box(
                            modifier = Modifier
                                .size(32.dp)
                                .clip(CircleShape)
                                .background(col)
                                .clickable { onGlowColorChange(col) }
                                .then(
                                    if (isSelected) Modifier.border(3.dp, MaterialTheme.colorScheme.onSurface, CircleShape)
                                    else Modifier.border(1.dp, Color.Gray, CircleShape)
                                ),
                            contentAlignment = Alignment.Center
                        ) {
                            if (isSelected) {
                                Icon(
                                    Icons.Default.Check,
                                    contentDescription = name,
                                    tint = Color.Black,
                                    modifier = Modifier.size(16.dp)
                                )
                            }
                        }
                    }
                }
            }
        }
    }
}
