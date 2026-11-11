package com.example.sphere.commodities.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.outlined.AutoAwesome
import androidx.compose.material.icons.outlined.ColorLens
import androidx.compose.material.icons.outlined.Grain
import androidx.compose.material.icons.outlined.Opacity
import androidx.compose.material.icons.outlined.Refresh
import androidx.compose.material.icons.outlined.Texture
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Slider
import androidx.compose.material3.SliderDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.CommodityItem

@Composable
fun CommoditySurfaceOpticsTab(
    commodity: CommodityItem,
    onCommodityChange: (CommodityItem) -> Unit,
    modifier: Modifier = Modifier
) {
    val presetColors = listOf(
        Color(0xFFE5DCC3), // Burlap / Jute
        Color(0xFFF5F5F0), // Paper / Sugar white
        Color(0xFF3E505B), // Slate / Cement
        Color(0xFF2E7D32), // Fertilizer Green
        Color(0xFFC08A3E), // Cardboard / Kraft
        Color(0xFF8D6E63), // Raw Timber Wood
        Color(0xFF1565C0), // Industrial Blue Drum
        Color(0xFFC62828), // Safety Red Can
        Color(0xFF37474F), // Rebar Dark Steel
        Color(0xFF263238)  // Rubber Tyre Black
    )

    Column(
        modifier = modifier
            .fillMaxWidth()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        // Material & PBR Pipeline Card
        Card(
            modifier = Modifier
                .fillMaxWidth()
                .testTag("commodity_pbr_card"),
            shape = RoundedCornerShape(14.dp),
            colors = CardDefaults.cardColors(
                containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f)
            )
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                verticalArrangement = Arrangement.spacedBy(12.dp)
            ) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        Icon(
                            imageVector = Icons.Outlined.AutoAwesome,
                            contentDescription = null,
                            tint = MaterialTheme.colorScheme.primary,
                            modifier = Modifier.size(20.dp)
                        )
                        Text(
                            text = "PBR Material Pipeline",
                            style = MaterialTheme.typography.titleMedium,
                            fontWeight = FontWeight.SemiBold
                        )
                    }

                    IconButton(
                        onClick = {
                            onCommodityChange(
                                commodity.copy(
                                    roughness = 0.72f,
                                    metallic = 0.05f,
                                    opacityBias = 0.0f,
                                    wearAndTear = 0.25f,
                                    wrinkleCreaseDepth = 0.45f
                                )
                            )
                        },
                        modifier = Modifier
                            .size(36.dp)
                            .testTag("reset_optics_button")
                    ) {
                        Icon(
                            imageVector = Icons.Outlined.Refresh,
                            contentDescription = "Reset optics to standard PBR",
                            tint = MaterialTheme.colorScheme.onSurfaceVariant
                        )
                    }
                }

                Text(
                    text = "Vulkan BC7/BC5 dual-texture bindings: albedo diffuse (BC7_SRGB) and normal / micro-creases (BC5_UNORM) evaluated through Disney PBR BRDF with GGX specular.",
                    style = MaterialTheme.typography.bodySmall,
                    color = MaterialTheme.colorScheme.onSurfaceVariant
                )
            }
        }

        // Color Palettes
        Card(
            modifier = Modifier
                .fillMaxWidth()
                .testTag("commodity_tint_card"),
            shape = RoundedCornerShape(14.dp),
            colors = CardDefaults.cardColors(
                containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.4f)
            )
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                verticalArrangement = Arrangement.spacedBy(12.dp)
            ) {
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    Icon(
                        imageVector = Icons.Outlined.ColorLens,
                        contentDescription = null,
                        tint = MaterialTheme.colorScheme.primary,
                        modifier = Modifier.size(20.dp)
                    )
                    Text(
                        text = "Base Albedo Tint",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.SemiBold
                    )
                }

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(10.dp)
                ) {
                    presetColors.take(5).forEach { color ->
                        val isSelected = commodity.baseColor == color
                        Box(
                            modifier = Modifier
                                .weight(1f)
                                .height(44.dp)
                                .clip(RoundedCornerShape(8.dp))
                                .background(color)
                                .border(
                                    width = if (isSelected) 3.dp else 1.dp,
                                    color = if (isSelected) MaterialTheme.colorScheme.primary else Color.Black.copy(alpha = 0.2f),
                                    shape = RoundedCornerShape(8.dp)
                                )
                                .clickable {
                                    onCommodityChange(commodity.copy(baseColor = color))
                                }
                                .testTag("color_tint_${color.value}")
                        )
                    }
                }

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(10.dp)
                ) {
                    presetColors.drop(5).take(5).forEach { color ->
                        val isSelected = commodity.baseColor == color
                        Box(
                            modifier = Modifier
                                .weight(1f)
                                .height(44.dp)
                                .clip(RoundedCornerShape(8.dp))
                                .background(color)
                                .border(
                                    width = if (isSelected) 3.dp else 1.dp,
                                    color = if (isSelected) MaterialTheme.colorScheme.primary else Color.Black.copy(alpha = 0.2f),
                                    shape = RoundedCornerShape(8.dp)
                                )
                                .clickable {
                                    onCommodityChange(commodity.copy(baseColor = color))
                                }
                                .testTag("color_tint_${color.value}")
                        )
                    }
                }
            }
        }

        // Roughness & Metallic Controls
        Card(
            modifier = Modifier
                .fillMaxWidth()
                .testTag("commodity_roughness_metallic_card"),
            shape = RoundedCornerShape(14.dp),
            colors = CardDefaults.cardColors(
                containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.4f)
            )
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                verticalArrangement = Arrangement.spacedBy(16.dp)
            ) {
                // Roughness Slider
                Column(verticalArrangement = Arrangement.spacedBy(4.dp)) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(6.dp)) {
                            Icon(
                                imageVector = Icons.Outlined.Texture,
                                contentDescription = null,
                                tint = MaterialTheme.colorScheme.secondary,
                                modifier = Modifier.size(16.dp)
                            )
                            Text(
                                text = "Surface Roughness",
                                style = MaterialTheme.typography.bodyMedium,
                                fontWeight = FontWeight.Medium
                            )
                        }
                        Text(
                            text = "${(commodity.roughness * 100).toInt()}% (GGX α)",
                            style = MaterialTheme.typography.bodySmall,
                            fontFamily = FontFamily.Monospace,
                            color = MaterialTheme.colorScheme.primary
                        )
                    }
                    Slider(
                        value = commodity.roughness,
                        onValueChange = { onCommodityChange(commodity.copy(roughness = it)) },
                        valueRange = 0.05f..1.0f,
                        modifier = Modifier.testTag("slider_roughness"),
                        colors = SliderDefaults.colors(
                            thumbColor = MaterialTheme.colorScheme.primary,
                            activeTrackColor = MaterialTheme.colorScheme.primary
                        )
                    )
                }

                // Metallic Slider
                Column(verticalArrangement = Arrangement.spacedBy(4.dp)) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(6.dp)) {
                            Icon(
                                imageVector = Icons.Outlined.Grain,
                                contentDescription = null,
                                tint = MaterialTheme.colorScheme.secondary,
                                modifier = Modifier.size(16.dp)
                            )
                            Text(
                                text = "Metallic / Conductance",
                                style = MaterialTheme.typography.bodyMedium,
                                fontWeight = FontWeight.Medium
                            )
                        }
                        Text(
                            text = "${(commodity.metallic * 100).toInt()}%",
                            style = MaterialTheme.typography.bodySmall,
                            fontFamily = FontFamily.Monospace,
                            color = MaterialTheme.colorScheme.primary
                        )
                    }
                    Slider(
                        value = commodity.metallic,
                        onValueChange = { onCommodityChange(commodity.copy(metallic = it)) },
                        valueRange = 0.0f..1.0f,
                        modifier = Modifier.testTag("slider_metallic"),
                        colors = SliderDefaults.colors(
                            thumbColor = MaterialTheme.colorScheme.primary,
                            activeTrackColor = MaterialTheme.colorScheme.primary
                        )
                    )
                }

                // Opacity / Subsurface Bias Slider
                Column(verticalArrangement = Arrangement.spacedBy(4.dp)) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(6.dp)) {
                            Icon(
                                imageVector = Icons.Outlined.Opacity,
                                contentDescription = null,
                                tint = MaterialTheme.colorScheme.secondary,
                                modifier = Modifier.size(16.dp)
                            )
                            Text(
                                text = "Opacity Bias / Glass Refraction",
                                style = MaterialTheme.typography.bodyMedium,
                                fontWeight = FontWeight.Medium
                            )
                        }
                        Text(
                            text = "${(commodity.opacityBias * 100).toInt()}%",
                            style = MaterialTheme.typography.bodySmall,
                            fontFamily = FontFamily.Monospace,
                            color = MaterialTheme.colorScheme.primary
                        )
                    }
                    Slider(
                        value = commodity.opacityBias,
                        onValueChange = { onCommodityChange(commodity.copy(opacityBias = it)) },
                        valueRange = 0.0f..1.0f,
                        modifier = Modifier.testTag("slider_opacity_bias"),
                        colors = SliderDefaults.colors(
                            thumbColor = MaterialTheme.colorScheme.primary,
                            activeTrackColor = MaterialTheme.colorScheme.primary
                        )
                    )
                }
            }
        }

        // Procedural Relief & Wear & Tear
        Card(
            modifier = Modifier
                .fillMaxWidth()
                .testTag("commodity_wrinkle_relief_card"),
            shape = RoundedCornerShape(14.dp),
            colors = CardDefaults.cardColors(
                containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.4f)
            )
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                verticalArrangement = Arrangement.spacedBy(16.dp)
            ) {
                Text(
                    text = "Micro-Displacement & Fabric Normal Relief",
                    style = MaterialTheme.typography.titleSmall,
                    fontWeight = FontWeight.SemiBold
                )

                // Wrinkle & Crease Depth
                Column(verticalArrangement = Arrangement.spacedBy(4.dp)) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(
                            text = "Wrinkles & Corrugation Depth",
                            style = MaterialTheme.typography.bodyMedium
                        )
                        Text(
                            text = "${(commodity.wrinkleCreaseDepth * 100).toInt()}% BC5",
                            style = MaterialTheme.typography.bodySmall,
                            fontFamily = FontFamily.Monospace,
                            color = MaterialTheme.colorScheme.secondary
                        )
                    }
                    Slider(
                        value = commodity.wrinkleCreaseDepth,
                        onValueChange = { onCommodityChange(commodity.copy(wrinkleCreaseDepth = it)) },
                        valueRange = 0.0f..1.0f,
                        modifier = Modifier.testTag("slider_wrinkle_depth"),
                        colors = SliderDefaults.colors(
                            thumbColor = MaterialTheme.colorScheme.secondary,
                            activeTrackColor = MaterialTheme.colorScheme.secondary
                        )
                    )
                }

                // Wear and Tear
                Column(verticalArrangement = Arrangement.spacedBy(4.dp)) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(
                            text = "Operational Wear & Edge Scuffs",
                            style = MaterialTheme.typography.bodyMedium
                        )
                        Text(
                            text = "${(commodity.wearAndTear * 100).toInt()}%",
                            style = MaterialTheme.typography.bodySmall,
                            fontFamily = FontFamily.Monospace,
                            color = MaterialTheme.colorScheme.secondary
                        )
                    }
                    Slider(
                        value = commodity.wearAndTear,
                        onValueChange = { onCommodityChange(commodity.copy(wearAndTear = it)) },
                        valueRange = 0.0f..1.0f,
                        modifier = Modifier.testTag("slider_wear_tear"),
                        colors = SliderDefaults.colors(
                            thumbColor = MaterialTheme.colorScheme.secondary,
                            activeTrackColor = MaterialTheme.colorScheme.secondary
                        )
                    )
                }
            }
        }
    }
}
