package com.example.sphere.commodities.tabs

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.AllInbox
import androidx.compose.material.icons.filled.Scale
import androidx.compose.material.icons.filled.ViewInAr
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
import com.example.sphere.model.CommodityItem
import com.example.sphere.model.StackingMode
import kotlin.math.roundToInt

@Composable
fun CommodityLogisticsTab(
    item: CommodityItem,
    weightKg: Float,
    onWeightChange: (Float) -> Unit,
    lengthM: Float,
    onLengthChange: (Float) -> Unit,
    widthM: Float,
    onWidthChange: (Float) -> Unit,
    heightM: Float,
    onHeightChange: (Float) -> Unit,
    stackingMode: StackingMode,
    onStackingModeChange: (StackingMode) -> Unit,
    showStraps: Boolean,
    onToggleStraps: (Boolean) -> Unit,
    showShrinkwrap: Boolean,
    onToggleShrinkwrap: (Boolean) -> Unit,
    showPalletBase: Boolean,
    onTogglePalletBase: (Boolean) -> Unit,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier
            .fillMaxWidth()
            .padding(14.dp)
            .testTag("commodity_logistics_tab")
    ) {
        // 1. Stacking Arrangement & Load Carrier
        Text(
            "Stacking Array & Logistics Unit",
            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
            color = MaterialTheme.colorScheme.primary
        )
        Spacer(Modifier.height(8.dp))

        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            StackingMode.entries.forEach { mode ->
                val isSelected = stackingMode == mode
                FilledTonalButton(
                    onClick = { onStackingModeChange(mode) },
                    modifier = Modifier
                        .weight(1f)
                        .testTag("stack_mode_${mode.name.lowercase()}"),
                    colors = ButtonDefaults.filledTonalButtonColors(
                        containerColor = if (isSelected) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.surfaceVariant,
                        contentColor = if (isSelected) MaterialTheme.colorScheme.onPrimary else MaterialTheme.colorScheme.onSurfaceVariant
                    ),
                    contentPadding = PaddingValues(horizontal = 4.dp, vertical = 6.dp)
                ) {
                    Text(
                        mode.label.substringBefore(" ("),
                        fontSize = 11.sp,
                        fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal
                    )
                }
            }
        }

        Spacer(Modifier.height(14.dp))

        // Packaging & Restraints (Straps, Shrinkwrap, Pallet)
        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
            shape = RoundedCornerShape(10.dp)
        ) {
            Column(modifier = Modifier.padding(12.dp)) {
                Text(
                    "Packaging Restraints & Load Securing",
                    style = MaterialTheme.typography.labelMedium.copy(fontWeight = FontWeight.Bold),
                    color = Color.White
                )
                Spacer(Modifier.height(8.dp))

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Text("EUR-1 Wooden Pallet Base", style = MaterialTheme.typography.bodySmall)
                    Switch(
                        checked = showPalletBase,
                        onCheckedChange = onTogglePalletBase,
                        modifier = Modifier.testTag("toggle_pallet_base")
                    )
                }

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Text("High-Tensile Polyester Banding Straps", style = MaterialTheme.typography.bodySmall)
                    Switch(
                        checked = showStraps,
                        onCheckedChange = onToggleStraps,
                        modifier = Modifier.testTag("toggle_banding_straps")
                    )
                }

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Text("Stretch Film Shrinkwrap Sheen", style = MaterialTheme.typography.bodySmall)
                    Switch(
                        checked = showShrinkwrap,
                        onCheckedChange = onToggleShrinkwrap,
                        modifier = Modifier.testTag("toggle_shrinkwrap")
                    )
                }
            }
        }

        Spacer(Modifier.height(16.dp))

        // 2. Unit Weight & Mass Properties
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Row(verticalAlignment = Alignment.CenterVertically) {
                Icon(Icons.Default.Scale, contentDescription = null, tint = MaterialTheme.colorScheme.primary, modifier = Modifier.size(16.dp))
                Spacer(Modifier.width(6.dp))
                Text("Unit Weight (Mass):", style = MaterialTheme.typography.labelMedium.copy(fontWeight = FontWeight.Bold))
            }
            Text(
                "${String.format("%.1f", weightKg)} kg",
                style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold, color = MaterialTheme.colorScheme.primary),
                fontFamily = FontFamily.Monospace
            )
        }
        Slider(
            value = weightKg,
            onValueChange = onWeightChange,
            valueRange = 0.2f..1500.0f,
            modifier = Modifier.fillMaxWidth().testTag("weight_slider")
        )

        Spacer(Modifier.height(10.dp))

        // 3. Physical Dimensions (Length, Width, Height)
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween
        ) {
            Text("Length (X-axis):", style = MaterialTheme.typography.labelSmall)
            Text("${String.format("%.2f", lengthM)} m", style = MaterialTheme.typography.labelSmall, fontFamily = FontFamily.Monospace)
        }
        Slider(
            value = lengthM,
            onValueChange = onLengthChange,
            valueRange = 0.05f..6.0f,
            modifier = Modifier.fillMaxWidth().testTag("length_slider")
        )

        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween
        ) {
            Text("Width (Z-axis):", style = MaterialTheme.typography.labelSmall)
            Text("${String.format("%.2f", widthM)} m", style = MaterialTheme.typography.labelSmall, fontFamily = FontFamily.Monospace)
        }
        Slider(
            value = widthM,
            onValueChange = onWidthChange,
            valueRange = 0.05f..3.0f,
            modifier = Modifier.fillMaxWidth().testTag("width_slider")
        )

        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween
        ) {
            Text("Height (Y-axis):", style = MaterialTheme.typography.labelSmall)
            Text("${String.format("%.2f", heightM)} m", style = MaterialTheme.typography.labelSmall, fontFamily = FontFamily.Monospace)
        }
        Slider(
            value = heightM,
            onValueChange = onHeightChange,
            valueRange = 0.05f..3.0f,
            modifier = Modifier.fillMaxWidth().testTag("height_slider")
        )

        Spacer(Modifier.height(10.dp))

        // Total Payload summary badge
        val totalWeight = weightKg * stackingMode.multiplier
        val totalVol = lengthM * widthM * heightM * stackingMode.multiplier
        Surface(
            shape = RoundedCornerShape(8.dp),
            color = Color(0xFF1E293B),
            border = CardDefaults.outlinedCardBorder()
        ) {
            Column(modifier = Modifier.padding(12.dp)) {
                Text(
                    "TOTAL FREIGHT SUMMARY",
                    style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold),
                    color = Color(0xFF38BDF8)
                )
                Spacer(Modifier.height(4.dp))
                Text(
                    "• Total Net Payload: ${totalWeight.roundToInt()} kg (${String.format("%.2f", totalWeight / 1000f)} tonnes)\n" +
                    "• Total Freight Cube: ${String.format("%.3f", totalVol)} m³\n" +
                    "• Unit Count: ${stackingMode.multiplier} units\n" +
                    "• Vehicle Compatibility: Flatbed Trailer / Container 20ft / Warehouse Pallet Rack",
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFFCBD5E1),
                    lineHeight = 18.sp
                )
            }
        }
    }
}
