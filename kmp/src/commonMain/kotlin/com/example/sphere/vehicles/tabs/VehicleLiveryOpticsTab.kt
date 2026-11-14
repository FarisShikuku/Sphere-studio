package com.example.sphere.vehicles.tabs

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.outlined.Casino
import androidx.compose.material3.*
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
import com.example.sphere.model.VehicleItem
import com.example.sphere.model.VehicleLiveryPreset
import kotlin.random.Random

@Composable
fun VehicleLiveryOpticsTab(
    vehicle: VehicleItem,
    onVehicleChange: (VehicleItem) -> Unit
) {
    val swatches = listOf(
        Color(0xFF29579E), // Cobalt Fleet
        Color(0xFFC22424), // Highway Red
        Color(0xFF1A7A5C), // Municipal Green
        Color(0xFFE6A314), // Quarry Amber
        Color(0xFFEBEEF5), // Pearl White
        Color(0xFF242B3D), // Midnight Slate
        Color(0xFFFA2E8F), // Matatu Neon Pink
        Color(0xFF12B8D9), // Matatu Cyan
        Color(0xFF8C38EB), // Electric Violet
        Color(0xFF38D15C)  // Matatu Lime
    )

    Column(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 8.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp)
    ) {
        // Livery Preset Selector + Randomize Nganya Graffiti Button
        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
            shape = RoundedCornerShape(14.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(10.dp)) {
                Text("Paint Livery & Cultural Styles", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.Bold)

                Row(
                    modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    VehicleLiveryPreset.entries.forEach { preset ->
                        FilterChip(
                            selected = vehicle.livery == preset,
                            onClick = {
                                onVehicleChange(
                                    vehicle.copy(
                                        livery = preset,
                                        baseColor = preset.baseColor,
                                        accentColor = preset.accentColor,
                                        roughness = preset.roughness,
                                        metallic = preset.metallic
                                    )
                                )
                            },
                            label = { Text(preset.displayName, style = MaterialTheme.typography.labelSmall) },
                            modifier = Modifier.testTag("livery_preset_${preset.name}")
                        )
                    }
                }

                Button(
                    onClick = {
                        val nextSeed = Random.nextInt()
                        val c1 = swatches[((nextSeed ushr 2) and 0x7FFFFFFF) % swatches.size]
                        val c2 = swatches[((nextSeed ushr 8) and 0x7FFFFFFF) % swatches.size].let {
                            if (it == c1) Color(0xFFFACC15) else it
                        }
                        onVehicleChange(
                            vehicle.copy(
                                livery = VehicleLiveryPreset.NAIROBI_NGANYA,
                                randomSeed = nextSeed,
                                baseColor = c1,
                                accentColor = c2
                            )
                        )
                    },
                    modifier = Modifier.fillMaxWidth().defaultMinSize(minHeight = 48.dp).testTag("randomize_livery_button")
                ) {
                    Icon(Icons.Outlined.Casino, contentDescription = null)
                    Spacer(Modifier.width(8.dp))
                    Text("Randomize Nganya / Graffiti Seed")
                }

                Text(
                    text = "Active C++ Seed: 0x${vehicle.randomSeed.toUInt().toString(16).uppercase()}",
                    fontFamily = FontFamily.Monospace,
                    fontSize = 11.sp,
                    color = MaterialTheme.colorScheme.onSurfaceVariant
                )
            }
        }

        // Primary & Accent Swatches
        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
            shape = RoundedCornerShape(14.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(10.dp)) {
                Text("Primary Paint Coat", style = MaterialTheme.typography.labelLarge, fontWeight = FontWeight.SemiBold)
                Row(
                    modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
                    horizontalArrangement = Arrangement.spacedBy(10.dp)
                ) {
                    swatches.forEachIndexed { idx, col ->
                        val selected = vehicle.baseColor == col
                        Surface(
                            shape = CircleShape,
                            border = BorderStroke(if (selected) 3.dp else 1.dp, if (selected) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.outline),
                            modifier = Modifier
                                .size(48.dp)
                                .clip(CircleShape)
                                .clickable { onVehicleChange(vehicle.copy(baseColor = col)) }
                                .testTag("vehicle_base_swatch_$idx")
                        ) {
                            Box(modifier = Modifier.fillMaxSize().background(col))
                        }
                    }
                }

                Text("Accent / Stripe Coat", style = MaterialTheme.typography.labelLarge, fontWeight = FontWeight.SemiBold)
                Row(
                    modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
                    horizontalArrangement = Arrangement.spacedBy(10.dp)
                ) {
                    swatches.forEachIndexed { idx, col ->
                        val selected = vehicle.accentColor == col
                        Surface(
                            shape = CircleShape,
                            border = BorderStroke(if (selected) 3.dp else 1.dp, if (selected) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.outline),
                            modifier = Modifier
                                .size(48.dp)
                                .clip(CircleShape)
                                .clickable { onVehicleChange(vehicle.copy(accentColor = col)) }
                                .testTag("vehicle_accent_swatch_$idx")
                        ) {
                            Box(modifier = Modifier.fillMaxSize().background(col))
                        }
                    }
                }
            }
        }

        // PBR Surface Roughness & Metallic Clearcoat
        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
            shape = RoundedCornerShape(14.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(6.dp)) {
                Text("Automotive Clearcoat PBR", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.Bold)
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Clearcoat Roughness", style = MaterialTheme.typography.bodySmall)
                    Text(String.format("%.2f", vehicle.roughness), fontFamily = FontFamily.Monospace, fontSize = 12.sp)
                }
                Slider(
                    value = vehicle.roughness,
                    onValueChange = { onVehicleChange(vehicle.copy(roughness = it)) },
                    valueRange = 0.08f..0.85f,
                    modifier = Modifier.testTag("slider_vehicle_roughness")
                )

                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Metallic Flake", style = MaterialTheme.typography.bodySmall)
                    Text(String.format("%.2f", vehicle.metallic), fontFamily = FontFamily.Monospace, fontSize = 12.sp)
                }
                Slider(
                    value = vehicle.metallic,
                    onValueChange = { onVehicleChange(vehicle.copy(metallic = it)) },
                    valueRange = 0.0f..1.0f,
                    modifier = Modifier.testTag("slider_vehicle_metallic")
                )
            }
        }
    }
}
