package com.example.sphere.vehicles.tabs

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowDropDown
import androidx.compose.material.icons.outlined.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.rotate
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.CabDriveSide
import com.example.sphere.model.CommodityItem
import com.example.sphere.model.TrailerItem
import com.example.sphere.model.VehicleItem
import kotlin.math.roundToInt

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun VehicleHitchPowertrainTab(
    vehicle: VehicleItem,
    onVehicleChange: (VehicleItem) -> Unit,
    insideCabinView: Boolean = false,
    onToggleInsideCabinView: (Boolean) -> Unit = {},
    trailerCatalog: List<TrailerItem>,
    selectedTrailer: TrailerItem,
    onSelectTrailer: (TrailerItem) -> Unit,
    hitchTrailerEnabled: Boolean,
    onToggleHitchTrailer: (Boolean) -> Unit,
    cargoCatalog: List<CommodityItem>,
    selectedCargo: CommodityItem,
    onSelectCargo: (CommodityItem) -> Unit,
    showLoadedCargo: Boolean,
    onToggleLoadedCargo: (Boolean) -> Unit,
    cargoQuantity: Int,
    onCargoQuantityChange: (Int) -> Unit
) {
    var isTrailerDropdownOpen by remember { mutableStateOf(false) }
    var isCargoDropdownOpen by remember { mutableStateOf(false) }

    val compatibleTrailers = remember(trailerCatalog) {
        trailerCatalog.filter { it.hitchType.name.contains("FIFTH") || it.hitchType.name.contains("GOOSENECK") }
    }
    val cargoOptions = remember(cargoCatalog) { cargoCatalog.take(24) }

    val passengerLoad = if (vehicle.passengerCapacity > 2) vehicle.passengerCapacity * 72f else 0f
    val trailerMass = if (vehicle.canHitchTrailer && hitchTrailerEnabled) selectedTrailer.emptyWeightKg else 0f
    val cargoMass = if (vehicle.canHitchTrailer && hitchTrailerEnabled && showLoadedCargo) {
        selectedCargo.nominalWeightKg * cargoQuantity
    } else 0f
    val totalMassKg = vehicle.kerbWeightKg + passengerLoad + trailerMass + cargoMass
    val utilization = (totalMassKg / vehicle.maxGvwKg.coerceAtLeast(1f)).coerceIn(0f, 1.3f)

    Column(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 8.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp)
    ) {
        // 0. Truck Cabin Interior Camera & LHD / RHD Steering Layout
        if (vehicle.isTruck) {
            Card(
                colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
                shape = RoundedCornerShape(14.dp)
            ) {
                Column(
                    modifier = Modifier.padding(14.dp),
                    verticalArrangement = Arrangement.spacedBy(10.dp)
                ) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Column(modifier = Modifier.weight(1f)) {
                            Text(
                                text = when {
                                    vehicle.isScania -> "Scania Next-Gen Cockpit & Cabin View"
                                    vehicle.isVolvo -> "Volvo Globetrotter Cockpit & Cabin View"
                                    else -> "Truck Cabin Interior & Steering Layout"
                                },
                                style = MaterialTheme.typography.titleSmall,
                                fontWeight = FontWeight.Bold
                            )
                            Text(
                                text = when {
                                    vehicle.isScania -> "Full LHD / RHD cockpit (RWD / FWD / AWD driveline compatible): Griffin wheel, hooded gauges, infotainment, cab computer tablet & travel mug."
                                    vehicle.isVolvo -> "Full LHD / RHD cockpit (RWD / FWD / AWD driveline compatible): 3-spoke Volvo wheel, 12-inch digital cluster, quilted bucket seats & ambient ceiling halo."
                                    else -> "First-person driver cockpit view with mirrored LHD / RHD wheel, binnacle gauges & seats (RWD / FWD / AWD chassis compatible)."
                                },
                                style = MaterialTheme.typography.bodySmall,
                                color = MaterialTheme.colorScheme.onSurfaceVariant
                            )
                        }
                        Switch(
                            checked = insideCabinView,
                            onCheckedChange = onToggleInsideCabinView,
                            modifier = Modifier.testTag("switch_vehicle_inside_cabin")
                        )
                    }

                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        CabDriveSide.entries.forEach { side ->
                            val selected = vehicle.cabDriveSide == side
                            FilterChip(
                                selected = selected,
                                onClick = { onVehicleChange(vehicle.copy(cabDriveSide = side)) },
                                label = {
                                    Text(
                                        "${side.shortLabel} (${side.displayName})",
                                        fontSize = 12.sp,
                                        maxLines = 1,
                                        overflow = TextOverflow.Ellipsis
                                    )
                                },
                                modifier = Modifier
                                    .weight(1f)
                                    .testTag("cab_side_${side.name.lowercase()}")
                            )
                        }
                    }
                }
            }
        }

        // 1. Fifth-Wheel Hitching (only active for Articulated Tractor Units, clear explanation otherwise)
        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
            shape = RoundedCornerShape(14.dp)
        ) {
            Column(
                modifier = Modifier.padding(14.dp),
                verticalArrangement = Arrangement.spacedBy(10.dp)
            ) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column(modifier = Modifier.weight(1f)) {
                        Text(
                            text = "Fifth-Wheel Semi-Trailer Hitch",
                            style = MaterialTheme.typography.titleSmall,
                            fontWeight = FontWeight.Bold
                        )
                        Text(
                            text = if (vehicle.canHitchTrailer) {
                                "Simulates vehicle->Hitch(TrailerRegistry::Get()) at offset +${String.format("%.2f", vehicle.fifthWheelOffsetM)}m"
                            } else {
                                "Solo rigid / passenger chassis (fifth-wheel hitch available on Articulated Tractors)"
                            },
                            style = MaterialTheme.typography.bodySmall,
                            color = MaterialTheme.colorScheme.onSurfaceVariant
                        )
                    }
                    Switch(
                        checked = vehicle.canHitchTrailer && hitchTrailerEnabled,
                        onCheckedChange = { if (vehicle.canHitchTrailer) onToggleHitchTrailer(it) },
                        enabled = vehicle.canHitchTrailer,
                        modifier = Modifier.testTag("switch_hitch_trailer")
                    )
                }

                if (vehicle.canHitchTrailer && hitchTrailerEnabled) {
                    Surface(
                        onClick = { isTrailerDropdownOpen = true },
                        shape = RoundedCornerShape(10.dp),
                        color = MaterialTheme.colorScheme.surfaceContainer,
                        border = BorderStroke(1.dp, MaterialTheme.colorScheme.outline),
                        modifier = Modifier
                            .fillMaxWidth()
                            .defaultMinSize(minHeight = 48.dp)
                            .testTag("dropdown_hitched_trailer")
                    ) {
                        Row(
                            modifier = Modifier.padding(horizontal = 12.dp, vertical = 10.dp),
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Icon(Icons.Outlined.RvHookup, contentDescription = null, tint = MaterialTheme.colorScheme.primary)
                            Spacer(Modifier.width(10.dp))
                            Column(modifier = Modifier.weight(1f)) {
                                Text("Coupled Semi-Trailer (${compatibleTrailers.size} compatible)", style = MaterialTheme.typography.labelSmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
                                Text(selectedTrailer.name, fontWeight = FontWeight.SemiBold, maxLines = 1, overflow = TextOverflow.Ellipsis)
                            }
                            Icon(Icons.Filled.ArrowDropDown, contentDescription = "Select trailer", modifier = Modifier.rotate(if (isTrailerDropdownOpen) 180f else 0f))
                        }
                    }

                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Column(modifier = Modifier.weight(1f)) {
                            Text("Load Freight onto Trailer", style = MaterialTheme.typography.bodyMedium, fontWeight = FontWeight.SemiBold)
                            Text("Attaches CargoRegistry::Get() freight onto deck", style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
                        }
                        Switch(
                            checked = showLoadedCargo,
                            onCheckedChange = onToggleLoadedCargo,
                            modifier = Modifier.testTag("switch_vehicle_trailer_cargo")
                        )
                    }

                    if (showLoadedCargo) {
                        Surface(
                            onClick = { isCargoDropdownOpen = true },
                            shape = RoundedCornerShape(10.dp),
                            color = MaterialTheme.colorScheme.surfaceContainer,
                            border = BorderStroke(1.dp, MaterialTheme.colorScheme.outline),
                            modifier = Modifier
                                .fillMaxWidth()
                                .defaultMinSize(minHeight = 48.dp)
                                .testTag("dropdown_vehicle_cargo")
                        ) {
                            Row(
                                modifier = Modifier.padding(horizontal = 12.dp, vertical = 8.dp),
                                verticalAlignment = Alignment.CenterVertically
                            ) {
                                Icon(Icons.Outlined.Inventory2, contentDescription = null, tint = MaterialTheme.colorScheme.secondary)
                                Spacer(Modifier.width(10.dp))
                                Column(modifier = Modifier.weight(1f)) {
                                    Text("Cargo Commodity", style = MaterialTheme.typography.labelSmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
                                    Text("${selectedCargo.name} (${selectedCargo.nominalWeightKg.roundToInt()} kg)", fontWeight = FontWeight.SemiBold, maxLines = 1, overflow = TextOverflow.Ellipsis)
                                }
                                Icon(Icons.Filled.ArrowDropDown, contentDescription = "Select cargo", modifier = Modifier.rotate(if (isCargoDropdownOpen) 180f else 0f))
                            }
                        }

                        Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                            Text("Quantity", style = MaterialTheme.typography.labelMedium)
                            Text("$cargoQuantity units • ${cargoMass.roundToInt()} kg", fontFamily = FontFamily.Monospace, fontSize = 12.sp)
                        }
                        Slider(
                            value = cargoQuantity.toFloat(),
                            onValueChange = { onCargoQuantityChange(it.roundToInt()) },
                            valueRange = 8f..240f,
                            modifier = Modifier.testTag("slider_vehicle_cargo_qty")
                        )
                    }
                }
            }
        }

        // 2. Combined GVW & Powertrain Bar
        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
            shape = RoundedCornerShape(14.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Powertrain & GVW Load", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.Bold)
                    Text(
                        "${totalMassKg.roundToInt()} / ${vehicle.maxGvwKg.roundToInt()} kg",
                        fontFamily = FontFamily.Monospace,
                        fontSize = 12.sp,
                        color = if (utilization > 1.0f) MaterialTheme.colorScheme.error else MaterialTheme.colorScheme.primary
                    )
                }
                LinearProgressIndicator(
                    progress = { utilization.coerceIn(0f, 1f) },
                    modifier = Modifier.fillMaxWidth().height(8.dp)
                )
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("${vehicle.enginePowerHp.roundToInt()} HP • ${vehicle.engineTorqueNm.roundToInt()} Nm", style = MaterialTheme.typography.bodySmall, fontFamily = FontFamily.Monospace)
                    Text("Pax: ${vehicle.passengerCapacity} • Clear: ${String.format("%.2f", vehicle.groundClearanceM)}m", style = MaterialTheme.typography.bodySmall, fontFamily = FontFamily.Monospace)
                }
            }
        }

        // 3. Interactive Mechanical Controls (Action Ratio, Wheelbase, Steering, Headlights)
        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
            shape = RoundedCornerShape(14.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(6.dp)) {
                Text("Chassis Kinematics & Body Action", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.Bold)

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column(modifier = Modifier.weight(1f)) {
                        Text("Passenger / Cab Door", style = MaterialTheme.typography.bodyMedium, fontWeight = FontWeight.SemiBold)
                        Text("Opens door to reveal cabin, seats, steps & cockpit", style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
                    }
                    Switch(
                        checked = vehicle.doorOpenRatio > 0.05f,
                        onCheckedChange = { open -> onVehicleChange(vehicle.copy(doorOpenRatio = if (open) 1f else 0f)) },
                        modifier = Modifier.testTag("switch_vehicle_door_open")
                    )
                }
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Door Open Position", style = MaterialTheme.typography.bodySmall)
                    Text("${(vehicle.doorOpenRatio * 100f).roundToInt()}%", fontFamily = FontFamily.Monospace, fontSize = 12.sp)
                }
                Slider(
                    value = vehicle.doorOpenRatio,
                    onValueChange = { onVehicleChange(vehicle.copy(doorOpenRatio = it)) },
                    valueRange = 0f..1f,
                    modifier = Modifier.testTag("slider_vehicle_door_open")
                )

                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text(vehicle.actionLabel, style = MaterialTheme.typography.bodySmall)
                    Text("${(vehicle.actionRatio * 100f).roundToInt()}%", fontFamily = FontFamily.Monospace, fontSize = 12.sp)
                }
                Slider(
                    value = vehicle.actionRatio,
                    onValueChange = { onVehicleChange(vehicle.copy(actionRatio = it)) },
                    valueRange = 0f..1f,
                    modifier = Modifier.testTag("slider_vehicle_action_ratio")
                )

                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Front Steer Angle", style = MaterialTheme.typography.bodySmall)
                    Text("${vehicle.steerAngleDeg.roundToInt()} deg", fontFamily = FontFamily.Monospace, fontSize = 12.sp)
                }
                Slider(
                    value = vehicle.steerAngleDeg,
                    onValueChange = { onVehicleChange(vehicle.copy(steerAngleDeg = it)) },
                    valueRange = -28f..28f,
                    modifier = Modifier.testTag("slider_vehicle_steer_angle")
                )

                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Wheelbase (m)", style = MaterialTheme.typography.bodySmall)
                    Text("${String.format("%.2f", vehicle.wheelbaseM)} m", fontFamily = FontFamily.Monospace, fontSize = 12.sp)
                }
                val minWb = (vehicle.lengthM * 0.38f).coerceAtLeast(2.6f)
                val maxWb = (vehicle.lengthM * 0.72f).coerceAtLeast(minWb + 0.5f)
                Slider(
                    value = vehicle.wheelbaseM.coerceIn(minWb, maxWb),
                    onValueChange = { onVehicleChange(vehicle.copy(wheelbaseM = it)) },
                    valueRange = minWb..maxWb,
                    modifier = Modifier.testTag("slider_vehicle_wheelbase")
                )

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Text("LED Headlamps & Roof Markers", style = MaterialTheme.typography.bodyMedium)
                    Switch(
                        checked = vehicle.headlightsOn,
                        onCheckedChange = { onVehicleChange(vehicle.copy(headlightsOn = it)) },
                        modifier = Modifier.testTag("switch_vehicle_headlights")
                    )
                }

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Text("Roof Luggage Rack / Rails", style = MaterialTheme.typography.bodyMedium)
                    Switch(
                        checked = vehicle.hasRoofRack,
                        onCheckedChange = { onVehicleChange(vehicle.copy(hasRoofRack = it)) },
                        modifier = Modifier.testTag("switch_vehicle_roof_rack")
                    )
                }

                if (vehicle.canHitchTrailer) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text("Chrome Vertical Exhaust Stacks", style = MaterialTheme.typography.bodyMedium)
                        Switch(
                            checked = vehicle.hasExhaustStacks,
                            onCheckedChange = { onVehicleChange(vehicle.copy(hasExhaustStacks = it)) },
                            modifier = Modifier.testTag("switch_vehicle_exhaust_stacks")
                        )
                    }
                }
            }
        }
    }

    if (isTrailerDropdownOpen) {
        val trailerSheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true)
        ModalBottomSheet(
            onDismissRequest = { isTrailerDropdownOpen = false },
            sheetState = trailerSheetState,
            containerColor = MaterialTheme.colorScheme.surfaceContainerLow
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .navigationBarsPadding()
                    .padding(horizontal = 16.dp)
            ) {
                Row(
                    modifier = Modifier.fillMaxWidth().padding(bottom = 12.dp),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column(modifier = Modifier.weight(1f)) {
                        Text("Select Coupled Semi-Trailer", style = MaterialTheme.typography.titleMedium, fontWeight = FontWeight.Bold)
                        Text("${compatibleTrailers.size} fifth-wheel & gooseneck variants", style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
                    }
                    IconButton(onClick = { isTrailerDropdownOpen = false }) {
                        Icon(Icons.Outlined.Close, contentDescription = "Close trailer selector")
                    }
                }
                LazyColumn(
                    modifier = Modifier.fillMaxWidth().heightIn(max = 420.dp),
                    verticalArrangement = Arrangement.spacedBy(8.dp),
                    contentPadding = PaddingValues(bottom = 24.dp)
                ) {
                    items(compatibleTrailers, key = { it.id }) { tr ->
                        val isCurrent = tr.id == selectedTrailer.id
                        Surface(
                            onClick = {
                                onSelectTrailer(tr)
                                isTrailerDropdownOpen = false
                            },
                            shape = RoundedCornerShape(12.dp),
                            color = if (isCurrent) MaterialTheme.colorScheme.primaryContainer.copy(alpha = 0.55f) else MaterialTheme.colorScheme.surfaceContainer,
                            border = BorderStroke(
                                width = if (isCurrent) 1.5.dp else 1.dp,
                                color = if (isCurrent) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.outlineVariant
                            ),
                            modifier = Modifier
                                .fillMaxWidth()
                                .defaultMinSize(minHeight = 52.dp)
                                .testTag("hitch_trailer_option_${tr.id}")
                        ) {
                            Row(
                                modifier = Modifier.fillMaxWidth().padding(horizontal = 14.dp, vertical = 10.dp),
                                verticalAlignment = Alignment.CenterVertically
                            ) {
                                Column(modifier = Modifier.weight(1f)) {
                                    Text(
                                        tr.name,
                                        fontWeight = if (isCurrent) FontWeight.Bold else FontWeight.SemiBold,
                                        color = if (isCurrent) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.onSurface
                                    )
                                    Text(
                                        "${tr.lengthM}m • Tare ${tr.emptyWeightKg.roundToInt()} kg",
                                        style = MaterialTheme.typography.bodySmall,
                                        color = MaterialTheme.colorScheme.onSurfaceVariant
                                    )
                                }
                                if (isCurrent) {
                                    Icon(Icons.Outlined.Check, contentDescription = "Selected", tint = MaterialTheme.colorScheme.primary, modifier = Modifier.size(18.dp))
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (isCargoDropdownOpen) {
        val cargoSheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true)
        ModalBottomSheet(
            onDismissRequest = { isCargoDropdownOpen = false },
            sheetState = cargoSheetState,
            containerColor = MaterialTheme.colorScheme.surfaceContainerLow
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .navigationBarsPadding()
                    .padding(horizontal = 16.dp)
            ) {
                Row(
                    modifier = Modifier.fillMaxWidth().padding(bottom = 12.dp),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column(modifier = Modifier.weight(1f)) {
                        Text("Select Cargo Commodity", style = MaterialTheme.typography.titleMedium, fontWeight = FontWeight.Bold)
                        Text("CargoRegistry::Get() freight items", style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
                    }
                    IconButton(onClick = { isCargoDropdownOpen = false }) {
                        Icon(Icons.Outlined.Close, contentDescription = "Close cargo selector")
                    }
                }
                LazyColumn(
                    modifier = Modifier.fillMaxWidth().heightIn(max = 420.dp),
                    verticalArrangement = Arrangement.spacedBy(8.dp),
                    contentPadding = PaddingValues(bottom = 24.dp)
                ) {
                    items(cargoOptions, key = { it.id }) { cg ->
                        val isCurrent = cg.id == selectedCargo.id
                        Surface(
                            onClick = {
                                onSelectCargo(cg)
                                isCargoDropdownOpen = false
                            },
                            shape = RoundedCornerShape(12.dp),
                            color = if (isCurrent) MaterialTheme.colorScheme.primaryContainer.copy(alpha = 0.55f) else MaterialTheme.colorScheme.surfaceContainer,
                            border = BorderStroke(
                                width = if (isCurrent) 1.5.dp else 1.dp,
                                color = if (isCurrent) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.outlineVariant
                            ),
                            modifier = Modifier
                                .fillMaxWidth()
                                .defaultMinSize(minHeight = 48.dp)
                                .testTag("hitch_cargo_option_${cg.id}")
                        ) {
                            Row(
                                modifier = Modifier.fillMaxWidth().padding(horizontal = 14.dp, vertical = 10.dp),
                                verticalAlignment = Alignment.CenterVertically
                            ) {
                                Text(
                                    "${cg.name} (${cg.nominalWeightKg.roundToInt()} kg)",
                                    fontWeight = if (isCurrent) FontWeight.Bold else FontWeight.Medium,
                                    color = if (isCurrent) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.onSurface,
                                    modifier = Modifier.weight(1f)
                                )
                                if (isCurrent) {
                                    Icon(Icons.Outlined.Check, contentDescription = "Selected", tint = MaterialTheme.colorScheme.primary, modifier = Modifier.size(18.dp))
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
