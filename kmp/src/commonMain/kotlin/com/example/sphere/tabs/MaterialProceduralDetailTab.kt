package com.example.sphere.tabs

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Grass
import androidx.compose.material.icons.filled.Landscape
import androidx.compose.material.icons.filled.Layers
import androidx.compose.material.icons.filled.ViewInAr
import androidx.compose.material.icons.filled.Widgets
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
import com.example.sphere.model.MaterialSurfaceMode
import com.example.sphere.model.PbrMaterial

@Composable
fun MaterialProceduralDetailTab(
    material: PbrMaterial,
    onMaterialChange: (PbrMaterial) -> Unit,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        // Surface Representation Toggle Card (Surface Grass vs 3D Grass / Surface Brick vs Voxel)
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f)),
            border = BorderStroke(1.dp, MaterialTheme.colorScheme.outlineVariant)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Icon(
                        imageVector = if (material.surfaceMode == MaterialSurfaceMode.SURFACE_ONLY) Icons.Default.Layers else Icons.Default.ViewInAr,
                        contentDescription = null,
                        tint = MaterialTheme.colorScheme.primary,
                        modifier = Modifier.size(20.dp)
                    )
                    Spacer(Modifier.width(8.dp))
                    Text(
                        "Surface Representation Mode",
                        style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold)
                    )
                }

                Spacer(Modifier.height(6.dp))
                Text(
                    text = material.surfaceMode.description,
                    style = MaterialTheme.typography.bodySmall.copy(color = MaterialTheme.colorScheme.onSurfaceVariant)
                )

                Spacer(Modifier.height(10.dp))
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    MaterialSurfaceMode.entries.forEach { mode ->
                        val isSelected = material.surfaceMode == mode
                        FilledTonalButton(
                            onClick = { onMaterialChange(material.copy(surfaceMode = mode)) },
                            modifier = Modifier
                                .weight(1f)
                                .testTag("procedural_mode_${mode.name.lowercase()}"),
                            colors = ButtonDefaults.filledTonalButtonColors(
                                containerColor = if (isSelected) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.surface,
                                contentColor = if (isSelected) MaterialTheme.colorScheme.onPrimary else MaterialTheme.colorScheme.onSurface
                            )
                        ) {
                            Text(mode.label, fontSize = 12.sp, fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal)
                        }
                    }
                }
            }
        }

        // Category-Specific Procedural Generators
        when {
            material.category == "Masonry" -> {
                // Procedural Brick & Mortar Parameters
                Text(
                    "Procedural Masonry & Mortar Spec",
                    style = MaterialTheme.typography.labelLarge.copy(fontWeight = FontWeight.Bold, color = MaterialTheme.colorScheme.primary)
                )

                Column {
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("Mortar Joint Width", style = MaterialTheme.typography.bodyMedium)
                        Text("${material.mortarWidthMm.toInt()} mm", style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold))
                    }
                    Slider(
                        value = material.mortarWidthMm,
                        onValueChange = { onMaterialChange(material.copy(mortarWidthMm = it)) },
                        valueRange = 4.0f..25.0f,
                        modifier = Modifier.testTag("slider_mortar_width")
                    )
                }

                Column {
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("Mortar Recess Depth", style = MaterialTheme.typography.bodyMedium)
                        Text("${"%.1f".format(material.mortarDepthMm)} mm", style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold))
                    }
                    Slider(
                        value = material.mortarDepthMm,
                        onValueChange = { onMaterialChange(material.copy(mortarDepthMm = it)) },
                        valueRange = 0.0f..15.0f,
                        modifier = Modifier.testTag("slider_mortar_depth")
                    )
                }

                Column {
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("Rustication & Crumbly Edge Erosion", style = MaterialTheme.typography.bodyMedium)
                        Text("${(material.brickRustication * 100f).toInt()}%", style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold))
                    }
                    Slider(
                        value = material.brickRustication,
                        onValueChange = { onMaterialChange(material.copy(brickRustication = it)) },
                        valueRange = 0.0f..1.0f,
                        modifier = Modifier.testTag("slider_brick_rustication")
                    )
                }

                Column {
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("Salt Efflorescence Staining", style = MaterialTheme.typography.bodyMedium)
                        Text("${(material.efflorescence * 100f).toInt()}%", style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold))
                    }
                    Slider(
                        value = material.efflorescence,
                        onValueChange = { onMaterialChange(material.copy(efflorescence = it)) },
                        valueRange = 0.0f..1.0f,
                        modifier = Modifier.testTag("slider_efflorescence")
                    )
                }
            }

            material.category == "Nature" && material.id.contains("grass") -> {
                // Procedural Grass Parameters (Surface vs 3D Blades)
                Text(
                    "Procedural Turf & Blade Synthesis",
                    style = MaterialTheme.typography.labelLarge.copy(fontWeight = FontWeight.Bold, color = MaterialTheme.colorScheme.primary)
                )

                Column {
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("Blade Density", style = MaterialTheme.typography.bodyMedium)
                        Text("${material.bladeDensity.toInt()} blades/m²", style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold))
                    }
                    Slider(
                        value = material.bladeDensity,
                        onValueChange = { onMaterialChange(material.copy(bladeDensity = it)) },
                        valueRange = 500f..5000f,
                        modifier = Modifier.testTag("slider_blade_density")
                    )
                }

                Column {
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("Blade Length", style = MaterialTheme.typography.bodyMedium)
                        Text("${material.bladeLengthMm.toInt()} mm", style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold))
                    }
                    Slider(
                        value = material.bladeLengthMm,
                        onValueChange = { onMaterialChange(material.copy(bladeLengthMm = it)) },
                        valueRange = 15f..120f,
                        modifier = Modifier.testTag("slider_blade_length")
                    )
                }

                Column {
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("Wind Sway & Curl Amplitude", style = MaterialTheme.typography.bodyMedium)
                        Text("${(material.windSway * 100f).toInt()}%", style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold))
                    }
                    Slider(
                        value = material.windSway,
                        onValueChange = { onMaterialChange(material.copy(windSway = it)) },
                        valueRange = 0.0f..1.0f,
                        modifier = Modifier.testTag("slider_wind_sway")
                    )
                }

                Column {
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("Thatch Root Layer Ratio", style = MaterialTheme.typography.bodyMedium)
                        Text("${(material.thatchRatio * 100f).toInt()}%", style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold))
                    }
                    Slider(
                        value = material.thatchRatio,
                        onValueChange = { onMaterialChange(material.copy(thatchRatio = it)) },
                        valueRange = 0.0f..0.5f,
                        modifier = Modifier.testTag("slider_thatch")
                    )
                }
            }

            material.category == "Nature" -> {
                // Procedural Soil & Earth Foliage Parameters
                Text(
                    "Procedural Topsoil & Foliage Matrix",
                    style = MaterialTheme.typography.labelLarge.copy(fontWeight = FontWeight.Bold, color = MaterialTheme.colorScheme.primary)
                )

                Column {
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("Micro-Foliage Ground Coverage", style = MaterialTheme.typography.bodyMedium)
                        Text("${material.foliageCoveragePct.toInt()}%", style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold))
                    }
                    Slider(
                        value = material.foliageCoveragePct,
                        onValueChange = { onMaterialChange(material.copy(foliageCoveragePct = it)) },
                        valueRange = 0.0f..100.0f,
                        modifier = Modifier.testTag("slider_foliage_coverage")
                    )
                }

                Column {
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("Soil Moisture Content", style = MaterialTheme.typography.bodyMedium)
                        Text("${(material.soilMoisture * 100f).toInt()}%", style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold))
                    }
                    Slider(
                        value = material.soilMoisture,
                        onValueChange = { onMaterialChange(material.copy(soilMoisture = it)) },
                        valueRange = 0.0f..1.0f,
                        modifier = Modifier.testTag("slider_soil_moisture")
                    )
                }

                Column {
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("Pebble & Aggregate Density", style = MaterialTheme.typography.bodyMedium)
                        Text("${material.pebbleCount.toInt()} / m²", style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold))
                    }
                    Slider(
                        value = material.pebbleCount,
                        onValueChange = { onMaterialChange(material.copy(pebbleCount = it)) },
                        valueRange = 0.0f..400.0f,
                        modifier = Modifier.testTag("slider_pebble_count")
                    )
                }
            }
        }
    }
}
