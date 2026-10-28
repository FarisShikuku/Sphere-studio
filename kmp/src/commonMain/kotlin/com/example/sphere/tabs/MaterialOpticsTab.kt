package com.example.sphere.tabs

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
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
import com.example.sphere.model.PbrMaterial

@Composable
fun MaterialOpticsTab(
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
        // Refraction Transmission
        Text(
            "Optical Refraction & Snell's Law",
            style = MaterialTheme.typography.labelLarge.copy(fontWeight = FontWeight.Bold, color = MaterialTheme.colorScheme.primary)
        )

        Column {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text("Refraction Transmission", style = MaterialTheme.typography.bodyMedium)
                Text(
                    "${(material.transmission * 100f).toInt()}%",
                    style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold)
                )
            }
            Slider(
                value = material.transmission,
                onValueChange = { onMaterialChange(material.copy(transmission = it)) },
                valueRange = 0.0f..1.0f,
                modifier = Modifier.testTag("slider_transmission")
            )
        }

        // Index of Refraction (IOR)
        Column {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text("Index of Refraction (IOR η)", style = MaterialTheme.typography.bodyMedium)
                Text(
                    "%.3f".format(material.ior),
                    style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold, color = Color(0xFFFFB74D))
                )
            }
            Slider(
                value = material.ior,
                onValueChange = { onMaterialChange(material.copy(ior = it)) },
                valueRange = 1.0f..2.5f,
                modifier = Modifier.testTag("slider_ior")
            )

            // Quick IOR Presets (Sascha Willems PBR / Raytracing materials)
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.spacedBy(6.dp)
            ) {
                listOf(
                    "Air" to 1.000f,
                    "Ice" to 1.309f,
                    "Water" to 1.333f,
                    "Glass" to 1.520f,
                    "Diamond" to 2.417f
                ).forEach { (presetName, presetIor) ->
                    AssistChip(
                        onClick = { onMaterialChange(material.copy(ior = presetIor)) },
                        label = { Text("$presetName (${presetIor})", fontSize = 11.sp) },
                        modifier = Modifier.testTag("ior_preset_${presetName.lowercase()}")
                    )
                }
            }
        }

        // Chromatic Dispersion (Abbe number)
        Column {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text("Chromatic Dispersion (Abbe Δ)", style = MaterialTheme.typography.bodyMedium)
                Text(
                    "%.3f".format(material.dispersion),
                    style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold)
                )
            }
            Slider(
                value = material.dispersion,
                onValueChange = { onMaterialChange(material.copy(dispersion = it)) },
                valueRange = 0.0f..0.05f,
                modifier = Modifier.testTag("slider_dispersion")
            )
        }

        // Frosted Refraction Roughness
        Column {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text("Frosted Refraction Blur", style = MaterialTheme.typography.bodyMedium)
                Text(
                    "%.2f".format(material.refractionRoughness),
                    style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold)
                )
            }
            Slider(
                value = material.refractionRoughness,
                onValueChange = { onMaterialChange(material.copy(refractionRoughness = it)) },
                valueRange = 0.0f..1.0f,
                modifier = Modifier.testTag("slider_refraction_blur")
            )
        }

        HorizontalDivider(modifier = Modifier.padding(vertical = 4.dp))

        // Subsurface Scattering (SSS)
        Text(
            "Subsurface Scattering (Foliage & Dermal)",
            style = MaterialTheme.typography.labelLarge.copy(fontWeight = FontWeight.Bold, color = MaterialTheme.colorScheme.primary)
        )

        // SSS Factor
        Column {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text("Subsurface Scattering Weight", style = MaterialTheme.typography.bodyMedium)
                Text(
                    "${(material.subsurface * 100f).toInt()}%",
                    style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold)
                )
            }
            Slider(
                value = material.subsurface,
                onValueChange = { onMaterialChange(material.copy(subsurface = it)) },
                valueRange = 0.0f..1.0f,
                modifier = Modifier.testTag("slider_subsurface")
            )
        }

        // SSS Scattering Radius
        Column {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text("Mean Free Path Radius", style = MaterialTheme.typography.bodyMedium)
                Text(
                    "${"%.1f".format(material.subsurfaceRadiusMm)} mm",
                    style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold)
                )
            }
            Slider(
                value = material.subsurfaceRadiusMm,
                onValueChange = { onMaterialChange(material.copy(subsurfaceRadiusMm = it)) },
                valueRange = 0.1f..15.0f,
                modifier = Modifier.testTag("slider_sss_radius")
            )
        }
    }
}
