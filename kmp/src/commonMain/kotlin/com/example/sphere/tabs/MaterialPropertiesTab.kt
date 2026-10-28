package com.example.sphere.tabs

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Info
import androidx.compose.material.icons.filled.Layers
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
import com.example.sphere.model.MaterialSurfaceMode
import com.example.sphere.model.PbrMaterial

@Composable
fun MaterialPropertiesTab(
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
        // Surface Representation Mode (Surface Grass vs 3D Blades)
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

                Spacer(Modifier.height(12.dp))
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
                                .testTag("surface_mode_${mode.name.lowercase()}"),
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

        // Base PBR Controls
        Text(
            "Microfacet Roughness & Metalness",
            style = MaterialTheme.typography.labelLarge.copy(fontWeight = FontWeight.Bold, color = MaterialTheme.colorScheme.primary)
        )

        // Roughness Slider
        Column {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text("Roughness (GGX Microfacet)", style = MaterialTheme.typography.bodyMedium)
                Text(
                    "%.2f".format(material.roughness),
                    style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold)
                )
            }
            Slider(
                value = material.roughness,
                onValueChange = { onMaterialChange(material.copy(roughness = it)) },
                valueRange = 0.0f..1.0f,
                modifier = Modifier.testTag("slider_roughness")
            )
        }

        // Metalness Slider
        Column {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text("Metallic Conductor Factor", style = MaterialTheme.typography.bodyMedium)
                Text(
                    "%.2f".format(material.metallic),
                    style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold)
                )
            }
            Slider(
                value = material.metallic,
                onValueChange = { onMaterialChange(material.copy(metallic = it)) },
                valueRange = 0.0f..1.0f,
                modifier = Modifier.testTag("slider_metallic")
            )
        }

        // Normal Perturbation Scale
        Column {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text("Normal Map Perturbation", style = MaterialTheme.typography.bodyMedium)
                Text(
                    "%.2f x".format(material.normalScale),
                    style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold)
                )
            }
            Slider(
                value = material.normalScale,
                onValueChange = { onMaterialChange(material.copy(normalScale = it)) },
                valueRange = 0.0f..3.0f,
                modifier = Modifier.testTag("slider_normal_scale")
            )
        }

        // Parallax Displacement Depth
        Column {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text("Height Displacement Depth", style = MaterialTheme.typography.bodyMedium)
                Text(
                    "${"%.1f".format(material.displacementDepthMeters * 100f)} cm",
                    style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold)
                )
            }
            Slider(
                value = material.displacementDepthMeters,
                onValueChange = { onMaterialChange(material.copy(displacementDepthMeters = it)) },
                valueRange = 0.0f..0.10f,
                modifier = Modifier.testTag("slider_displacement")
            )
        }

        // UV Tile Scale
        Column {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text("World-Space UV Tile Scale", style = MaterialTheme.typography.bodyMedium)
                Text(
                    "${"%.2f".format(material.tileScaleMeters)} m",
                    style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold)
                )
            }
            Slider(
                value = material.tileScaleMeters,
                onValueChange = { onMaterialChange(material.copy(tileScaleMeters = it)) },
                valueRange = 0.1f..3.0f,
                modifier = Modifier.testTag("slider_tile_scale")
            )
        }
    }
}
