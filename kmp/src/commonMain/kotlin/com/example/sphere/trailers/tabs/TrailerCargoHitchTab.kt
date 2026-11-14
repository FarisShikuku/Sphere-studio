package com.example.sphere.trailers.tabs

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowDropDown
import androidx.compose.material.icons.outlined.Check
import androidx.compose.material.icons.outlined.Inventory2
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.rotate
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.CommodityItem
import com.example.sphere.model.TrailerHitchType
import com.example.sphere.model.TrailerItem
import kotlin.math.roundToInt

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun TrailerCargoHitchTab(
    trailer: TrailerItem,
    onTrailerChange: (TrailerItem) -> Unit,
    cargoCatalog: List<CommodityItem>,
    selectedCargo: CommodityItem,
    onSelectCargo: (CommodityItem) -> Unit,
    showLoadedCargo: Boolean,
    onToggleLoadedCargo: (Boolean) -> Unit,
    cargoQuantity: Int,
    onCargoQuantityChange: (Int) -> Unit,
    landingGearExtended: Boolean,
    onToggleLandingGear: (Boolean) -> Unit,
    curtainOpenRatio: Float,
    onCurtainOpenRatioChange: (Float) -> Unit,
    hydraulicLiftRatio: Float,
    onHydraulicLiftRatioChange: (Float) -> Unit,
    modifier: Modifier = Modifier
) {
    var cargoDropdownExpanded by remember { mutableStateOf(false) }
    val payloadKg = if (showLoadedCargo) selectedCargo.nominalWeightKg * cargoQuantity else 0f
    val payloadUtilization = (payloadKg / trailer.maxPayloadKg.coerceAtLeast(1f)).coerceIn(0f, 1.25f)

    Column(
        modifier = modifier
            .fillMaxWidth()
            .padding(14.dp)
            .testTag("trailer_cargo_hitch_tab")
    ) {
        Text(
            "CargoRegistry Payload Integration & Hitching",
            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
            color = MaterialTheme.colorScheme.primary
        )
        Spacer(Modifier.height(8.dp))

        // Cargo Integration Card
        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(12.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column(modifier = Modifier.weight(1f)) {
                        Text("Load Cargo via CargoRegistry::Get()", fontWeight = FontWeight.Bold, fontSize = 13.sp)
                        Text(
                            "Simulates myTrailer->LoadCargo(cargo, quantity)",
                            style = MaterialTheme.typography.bodySmall,
                            color = MaterialTheme.colorScheme.onSurfaceVariant
                        )
                    }
                    Switch(
                        checked = showLoadedCargo,
                        onCheckedChange = onToggleLoadedCargo,
                        modifier = Modifier.testTag("switch_load_cargo")
                    )
                }

                Spacer(Modifier.height(10.dp))

                Box(modifier = Modifier.fillMaxWidth()) {
                    Surface(
                        onClick = { cargoDropdownExpanded = !cargoDropdownExpanded },
                        shape = RoundedCornerShape(10.dp),
                        color = MaterialTheme.colorScheme.surface,
                        border = BorderStroke(
                            width = if (cargoDropdownExpanded) 2.dp else 1.dp,
                            color = if (cargoDropdownExpanded) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.outline
                        ),
                        modifier = Modifier
                            .fillMaxWidth()
                            .defaultMinSize(minHeight = 52.dp)
                            .testTag("dropdown_trailer_cargo")
                    ) {
                        Row(
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(horizontal = 12.dp, vertical = 8.dp),
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Icon(
                                imageVector = Icons.Outlined.Inventory2,
                                contentDescription = null,
                                tint = MaterialTheme.colorScheme.primary
                            )
                            Spacer(Modifier.width(10.dp))
                            Column(modifier = Modifier.weight(1f)) {
                                Text(
                                    text = "Cargo Commodity (${cargoCatalog.size} available)",
                                    style = MaterialTheme.typography.labelSmall,
                                    color = if (cargoDropdownExpanded) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.onSurfaceVariant
                                )
                                Text(
                                    text = "${selectedCargo.name} (${selectedCargo.nominalWeightKg.roundToInt()} kg)",
                                    style = MaterialTheme.typography.bodyMedium,
                                    fontWeight = FontWeight.SemiBold,
                                    maxLines = 1,
                                    overflow = TextOverflow.Ellipsis
                                )
                            }
                            Icon(
                                imageVector = Icons.Filled.ArrowDropDown,
                                contentDescription = "Expand cargo list",
                                tint = MaterialTheme.colorScheme.onSurfaceVariant,
                                modifier = Modifier.rotate(if (cargoDropdownExpanded) 180f else 0f)
                            )
                        }
                    }

                    DropdownMenu(
                        expanded = cargoDropdownExpanded,
                        onDismissRequest = { cargoDropdownExpanded = false },
                        modifier = Modifier
                            .widthIn(min = 260.dp, max = 360.dp)
                            .heightIn(max = 320.dp)
                    ) {
                        cargoCatalog.forEach { c ->
                            key(c.id) {
                                val isCur = c.id == selectedCargo.id
                                DropdownMenuItem(
                                    text = {
                                        Column {
                                            Text(
                                                c.name,
                                                fontSize = 12.sp,
                                                fontWeight = if (isCur) FontWeight.Bold else FontWeight.Medium,
                                                color = if (isCur) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.onSurface
                                            )
                                            Text(
                                                "${c.category.displayName} • ${c.nominalWeightKg.roundToInt()} kg",
                                                fontSize = 10.sp,
                                                color = MaterialTheme.colorScheme.onSurfaceVariant
                                            )
                                        }
                                    },
                                    trailingIcon = if (isCur) {
                                        { Icon(Icons.Outlined.Check, contentDescription = "Selected", tint = MaterialTheme.colorScheme.primary, modifier = Modifier.size(16.dp)) }
                                    } else null,
                                    onClick = {
                                        onSelectCargo(c)
                                        cargoDropdownExpanded = false
                                    }
                                )
                            }
                        }
                    }
                }

                Spacer(Modifier.height(10.dp))

                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Cargo Units (Quantity)", style = MaterialTheme.typography.labelMedium)
                    Text("$cargoQuantity units • ${payloadKg.roundToInt()} kg", fontFamily = FontFamily.Monospace, fontSize = 11.sp, color = MaterialTheme.colorScheme.primary)
                }
                Slider(
                    value = cargoQuantity.toFloat(),
                    onValueChange = { onCargoQuantityChange(it.roundToInt()) },
                    valueRange = 1f..500f
                )

                LinearProgressIndicator(
                    progress = { payloadUtilization.coerceIn(0f, 1f) },
                    color = if (payloadKg > trailer.maxPayloadKg) Color(0xFFEF4444) else MaterialTheme.colorScheme.primary,
                    modifier = Modifier.fillMaxWidth().height(6.dp)
                )
                Spacer(Modifier.height(4.dp))
                Text(
                    "Payload Capacity: ${payloadKg.roundToInt()} / ${trailer.maxPayloadKg.roundToInt()} kg (${(payloadUtilization * 100).roundToInt()}%)",
                    fontSize = 11.sp,
                    color = if (payloadKg > trailer.maxPayloadKg) Color(0xFFF87171) else MaterialTheme.colorScheme.onSurfaceVariant
                )
            }
        }

        Spacer(Modifier.height(14.dp))

        // Hitch & Mechanical Actuators
        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(12.dp)) {
                Text("Hitch Coupler & Mechanical Actuators", fontWeight = FontWeight.Bold, fontSize = 13.sp)
                Spacer(Modifier.height(8.dp))

                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(6.dp)) {
                    TrailerHitchType.entries.forEach { h ->
                        val active = trailer.hitchType == h
                        FilterChip(
                            selected = active,
                            onClick = { onTrailerChange(trailer.copy(hitchType = h)) },
                            label = { Text(h.displayName.substringBefore(" "), fontSize = 10.sp) }
                        )
                    }
                }

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Text("Extend Telescopic Landing Gear Legs", style = MaterialTheme.typography.bodySmall)
                    Switch(checked = landingGearExtended, onCheckedChange = onToggleLandingGear)
                }

                Spacer(Modifier.height(6.dp))
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Curtain Rollback / Side Access", style = MaterialTheme.typography.bodySmall)
                    Text("${(curtainOpenRatio * 100).roundToInt()}%", fontFamily = FontFamily.Monospace, fontSize = 11.sp)
                }
                Slider(value = curtainOpenRatio, onValueChange = onCurtainOpenRatioChange, valueRange = 0f..0.85f)

                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Hydraulic Deck / Tipper Ram Lift", style = MaterialTheme.typography.bodySmall)
                    Text("${(hydraulicLiftRatio * 100).roundToInt()}%", fontFamily = FontFamily.Monospace, fontSize = 11.sp)
                }
                Slider(value = hydraulicLiftRatio, onValueChange = onHydraulicLiftRatioChange, valueRange = 0f..1.0f)
            }
        }

        Spacer(Modifier.height(14.dp))

        // Dimensions
        Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
            Text("Trailer Length (m)", style = MaterialTheme.typography.labelMedium)
            Text("${String.format("%.2f", trailer.lengthM)} m", fontFamily = FontFamily.Monospace, fontSize = 12.sp)
        }
        Slider(
            value = trailer.lengthM,
            onValueChange = { onTrailerChange(trailer.copy(lengthM = it)) },
            valueRange = 3.5f..22.5f
        )

        Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
            Text("Empty Tare Weight (kg)", style = MaterialTheme.typography.labelMedium)
            Text("${trailer.emptyWeightKg.roundToInt()} kg", fontFamily = FontFamily.Monospace, fontSize = 12.sp)
        }
        Slider(
            value = trailer.emptyWeightKg,
            onValueChange = { onTrailerChange(trailer.copy(emptyWeightKg = it)) },
            valueRange = 400f..28000f
        )
    }
}
