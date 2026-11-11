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
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.outlined.Check
import androidx.compose.material.icons.outlined.Edit
import androidx.compose.material.icons.outlined.Label
import androidx.compose.material.icons.outlined.QrCode
import androidx.compose.material.icons.outlined.Report
import androidx.compose.material.icons.outlined.Tune
import androidx.compose.material.icons.outlined.Warning
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.FilterChip
import androidx.compose.material3.FilterChipDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Slider
import androidx.compose.material3.SliderDefaults
import androidx.compose.material3.Switch
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
import com.example.sphere.model.StencilStampType

@Composable
fun CommodityStencilsTab(
    commodity: CommodityItem,
    onCommodityChange: (CommodityItem) -> Unit,
    modifier: Modifier = Modifier
) {
    val stencilColors = listOf(
        Color(0xFF1E1E1E) to "Carbon Black",
        Color(0xFFB71C1C) to "Hazard Red",
        Color(0xFF0D47A1) to "Logistics Blue",
        Color(0xFF1B5E20) to "Bio Green",
        Color(0xFFE65100) to "Warning Orange",
        Color(0xFFFFFFFF) to "High-Vis White"
    )

    Column(
        modifier = modifier
            .fillMaxWidth()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        // Stencil Activation Card
        Card(
            modifier = Modifier
                .fillMaxWidth()
                .testTag("commodity_stencil_toggle_card"),
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
                        horizontalArrangement = Arrangement.spacedBy(10.dp)
                    ) {
                        Icon(
                            imageVector = Icons.Outlined.Label,
                            contentDescription = null,
                            tint = MaterialTheme.colorScheme.primary,
                            modifier = Modifier.size(24.dp)
                        )
                        Column {
                            Text(
                                text = "Procedural Stencil Decal",
                                style = MaterialTheme.typography.titleMedium,
                                fontWeight = FontWeight.SemiBold
                            )
                            Text(
                                text = "Imports project typography pipeline (TextDef.h)",
                                style = MaterialTheme.typography.bodySmall,
                                color = MaterialTheme.colorScheme.onSurfaceVariant
                            )
                        }
                    }

                    Switch(
                        checked = commodity.showStencil,
                        onCheckedChange = { onCommodityChange(commodity.copy(showStencil = it)) },
                        modifier = Modifier.testTag("switch_show_stencil")
                    )
                }
            }
        }

        if (commodity.showStencil) {
            // Label Text Field
            Card(
                modifier = Modifier
                    .fillMaxWidth()
                    .testTag("commodity_stencil_text_card"),
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
                    Text(
                        text = "Branding & Commodity Specification Label",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.SemiBold
                    )

                    OutlinedTextField(
                        value = commodity.stencilLabel,
                        onValueChange = { onCommodityChange(commodity.copy(stencilLabel = it)) },
                        label = { Text("Stencil Label / Commodity Stamp") },
                        modifier = Modifier
                            .fillMaxWidth()
                            .testTag("input_stencil_label"),
                        singleLine = true,
                        leadingIcon = {
                            Icon(imageVector = Icons.Outlined.Edit, contentDescription = null)
                        }
                    )

                    Text(
                        text = "Renders directly into BC7_SRGB albedo compute dispatch at UV footprint (u_min, v_min) to (u_max, v_max).",
                        style = MaterialTheme.typography.bodySmall,
                        color = MaterialTheme.colorScheme.onSurfaceVariant
                    )
                }
            }

            // Stamp Type Selection
            Card(
                modifier = Modifier
                    .fillMaxWidth()
                    .testTag("commodity_stamp_type_card"),
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
                    Text(
                        text = "Industrial Stamp & Compliance Mark",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.SemiBold
                    )

                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        StencilStampType.entries.take(3).forEach { stamp ->
                            val isSelected = commodity.stampType == stamp
                            FilterChip(
                                selected = isSelected,
                                onClick = { onCommodityChange(commodity.copy(stampType = stamp)) },
                                label = { Text(stamp.displayName, style = MaterialTheme.typography.bodySmall) },
                                leadingIcon = {
                                    Icon(
                                        imageVector = when (stamp) {
                                            StencilStampType.NONE -> Icons.Outlined.Tune
                                            StencilStampType.BARCODE_GS1 -> Icons.Outlined.QrCode
                                            StencilStampType.WARNING_HAZARD -> Icons.Outlined.Warning
                                            StencilStampType.NET_WEIGHT -> Icons.Outlined.Label
                                            StencilStampType.EPAL_EUR -> Icons.Outlined.Check
                                            StencilStampType.BATCH_LOT -> Icons.Outlined.Report
                                        },
                                        contentDescription = null,
                                        modifier = Modifier.size(16.dp)
                                    )
                                },
                                modifier = Modifier
                                    .weight(1f)
                                    .testTag("chip_stamp_${stamp.name}")
                            )
                        }
                    }

                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        StencilStampType.entries.drop(3).forEach { stamp ->
                            val isSelected = commodity.stampType == stamp
                            FilterChip(
                                selected = isSelected,
                                onClick = { onCommodityChange(commodity.copy(stampType = stamp)) },
                                label = { Text(stamp.displayName, style = MaterialTheme.typography.bodySmall) },
                                leadingIcon = {
                                    Icon(
                                        imageVector = when (stamp) {
                                            StencilStampType.NONE -> Icons.Outlined.Tune
                                            StencilStampType.BARCODE_GS1 -> Icons.Outlined.QrCode
                                            StencilStampType.WARNING_HAZARD -> Icons.Outlined.Warning
                                            StencilStampType.NET_WEIGHT -> Icons.Outlined.Label
                                            StencilStampType.EPAL_EUR -> Icons.Outlined.Check
                                            StencilStampType.BATCH_LOT -> Icons.Outlined.Report
                                        },
                                        contentDescription = null,
                                        modifier = Modifier.size(16.dp)
                                    )
                                },
                                modifier = Modifier
                                    .weight(1f)
                                    .testTag("chip_stamp_${stamp.name}")
                            )
                        }
                    }
                }
            }

            // Stencil Ink Color & Scale Card
            Card(
                modifier = Modifier
                    .fillMaxWidth()
                    .testTag("commodity_stencil_props_card"),
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
                        text = "Stencil Pigment Ink & Scale",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.SemiBold
                    )

                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        stencilColors.forEach { (color, name) ->
                            val isSelected = commodity.stencilColor == color
                            Box(
                                modifier = Modifier
                                    .weight(1f)
                                    .height(38.dp)
                                    .clip(RoundedCornerShape(6.dp))
                                    .background(color)
                                    .border(
                                        width = if (isSelected) 3.dp else 1.dp,
                                        color = if (isSelected) MaterialTheme.colorScheme.primary else Color.Black.copy(alpha = 0.3f),
                                        shape = RoundedCornerShape(6.dp)
                                    )
                                    .clickable {
                                        onCommodityChange(commodity.copy(stencilColor = color))
                                    }
                                    .testTag("stencil_color_${name.replace(" ", "_")}")
                            )
                        }
                    }

                    // Stencil Scale Slider
                    Column(verticalArrangement = Arrangement.spacedBy(4.dp)) {
                        Row(
                            modifier = Modifier.fillMaxWidth(),
                            horizontalArrangement = Arrangement.SpaceBetween,
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Text(
                                text = "Stamp Print Scale",
                                style = MaterialTheme.typography.bodyMedium
                            )
                            Text(
                                text = "${(commodity.stencilScale * 100).toInt()}%",
                                style = MaterialTheme.typography.bodySmall,
                                fontFamily = FontFamily.Monospace,
                                color = MaterialTheme.colorScheme.primary
                            )
                        }
                        Slider(
                            value = commodity.stencilScale,
                            onValueChange = { onCommodityChange(commodity.copy(stencilScale = it)) },
                            valueRange = 0.3f..1.5f,
                            modifier = Modifier.testTag("slider_stencil_scale"),
                            colors = SliderDefaults.colors(
                                thumbColor = MaterialTheme.colorScheme.primary,
                                activeTrackColor = MaterialTheme.colorScheme.primary
                            )
                        )
                    }
                }
            }
        }
    }
}
