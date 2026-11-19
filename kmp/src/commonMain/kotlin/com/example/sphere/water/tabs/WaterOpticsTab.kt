package com.example.sphere.water.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.BlurOn
import androidx.compose.material.icons.filled.Visibility
import androidx.compose.material.icons.filled.WaterDrop
import androidx.compose.material.icons.filled.WbSunny
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.unit.dp
import com.example.sphere.model.FluidDebugViewType
import com.example.sphere.model.WaterOpticsState
import kotlin.math.roundToInt

private val waterColorSwatches = listOf(
    "Tropical" to Color(0.06f, 0.68f, 0.88f),
    "Cobalt" to Color(0.08f, 0.56f, 0.86f),
    "Deep Sea" to Color(0.04f, 0.28f, 0.52f),
    "Emerald" to Color(0.05f, 0.62f, 0.54f),
    "Amber" to Color(0.88f, 0.56f, 0.08f),
    "Mercury" to Color(0.76f, 0.82f, 0.90f)
)

@Composable
fun WaterOpticsTab(
    optics: WaterOpticsState,
    onOpticsChange: (WaterOpticsState) -> Unit
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(horizontal = 14.dp, vertical = 10.dp)
    ) {
        // SSFR Render Pass Debug Stage Inspector
        WaterSectionCard(
            title = "SSFR Render Pass Debug View",
            subtitle = "Inspect intermediate Screen-Space Fluid Rendering textures",
            icon = Icons.Default.Visibility
        ) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState()),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                FluidDebugViewType.entries.forEach { mode ->
                    FilterChip(
                        selected = optics.debugMode == mode,
                        onClick = { onOpticsChange(optics.copy(debugMode = mode)) },
                        label = { Text(mode.label) },
                        modifier = Modifier.testTag("water_debug_mode_${mode.name.lowercase()}")
                    )
                }
            }
            Text(
                text = "Active Shader: ${optics.debugMode.shaderPassName}",
                style = MaterialTheme.typography.bodySmall,
                color = MaterialTheme.colorScheme.primary,
                modifier = Modifier.padding(top = 6.dp)
            )
        }

        // Depth Splatting & Separable Bilateral Gaussian Blur
        WaterSectionCard(
            title = "Sphere Impostors & Bilateral Depth Smoothing",
            subtitle = "Depth-preserving separable Gaussian filter eliminating particle seams",
            icon = Icons.Default.BlurOn
        ) {
            WaterLabeledSlider(
                label = "Particle Impostor Radius",
                value = optics.particleVisualRadius,
                valueRange = 0.04f..0.18f,
                formattedValue = "${"%.3f".format(optics.particleVisualRadius)} m",
                onValueChange = { onOpticsChange(optics.copy(particleVisualRadius = it)) },
                testTag = "water_particle_radius_slider"
            )

            WaterToggleRow(
                title = "Separable Bilateral Blur",
                subtitle = "Preserves silhouette edges while smoothing interior fluid surface",
                checked = optics.bilateralBlurEnabled,
                onCheckedChange = { onOpticsChange(optics.copy(bilateralBlurEnabled = it)) },
                testTag = "water_bilateral_blur_switch"
            )

            if (optics.bilateralBlurEnabled) {
                WaterLabeledSlider(
                    label = "Blur Kernel Radius",
                    value = optics.blurRadiusPx.toFloat(),
                    valueRange = 1f..9f,
                    steps = 7,
                    formattedValue = "${optics.blurRadiusPx} px",
                    onValueChange = { onOpticsChange(optics.copy(blurRadiusPx = it.roundToInt().coerceIn(1, 9))) },
                    testTag = "water_blur_radius_slider"
                )
                WaterLabeledSlider(
                    label = "Spatial Sigma (σ_spatial)",
                    value = optics.blurSpatialSigma,
                    valueRange = 0.8f..6.0f,
                    formattedValue = "${"%.1f".format(optics.blurSpatialSigma)}",
                    onValueChange = { onOpticsChange(optics.copy(blurSpatialSigma = it)) },
                    testTag = "water_blur_spatial_sigma_slider"
                )
                WaterLabeledSlider(
                    label = "Depth Edge Falloff (σ_depth)",
                    value = optics.blurDepthSigma,
                    valueRange = 0.01f..0.30f,
                    formattedValue = "${"%.3f".format(optics.blurDepthSigma)} m",
                    onValueChange = { onOpticsChange(optics.copy(blurDepthSigma = it)) },
                    testTag = "water_blur_depth_sigma_slider"
                )
            }
        }

        // Beer-Lambert Absorption, Snell Refraction & Schlick Fresnel
        WaterSectionCard(
            title = "Beer-Lambert Optics, Refraction & Fresnel",
            subtitle = "Spectral depth attenuation exp(-σ·thickness), Snell IOR & GGX specular",
            icon = Icons.Default.WaterDrop
        ) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState())
                    .padding(vertical = 6.dp),
                horizontalArrangement = Arrangement.spacedBy(10.dp)
            ) {
                waterColorSwatches.forEach { (name, swatch) ->
                    val isSelected = optics.waterColor == swatch
                    Column(
                        horizontalAlignment = androidx.compose.ui.Alignment.CenterHorizontally
                    ) {
                        Box(
                            modifier = Modifier
                                .size(48.dp)
                                .clip(CircleShape)
                                .background(swatch)
                                .border(
                                    width = if (isSelected) 3.dp else 1.dp,
                                    color = if (isSelected) MaterialTheme.colorScheme.primary else Color.White.copy(alpha = 0.35f),
                                    shape = CircleShape
                                )
                                .clickable { onOpticsChange(optics.copy(waterColor = swatch)) }
                                .testTag("water_swatch_${name.lowercase().replace(" ", "_")}")
                        )
                        Spacer(Modifier.height(2.dp))
                        Text(name, style = MaterialTheme.typography.labelSmall)
                    }
                }
            }

            WaterLabeledSlider(
                label = "Beer-Lambert Absorption Strength",
                value = optics.absorptionStrength,
                valueRange = 0.1f..4.0f,
                formattedValue = "${"%.2f".format(optics.absorptionStrength)}",
                onValueChange = { onOpticsChange(optics.copy(absorptionStrength = it)) },
                testTag = "water_absorption_strength_slider"
            )
            WaterLabeledSlider(
                label = "Red Extinction (σ_R)",
                value = optics.absorbR,
                valueRange = 0.0f..1.5f,
                formattedValue = "${"%.2f".format(optics.absorbR)}",
                onValueChange = { onOpticsChange(optics.copy(absorbR = it)) },
                testTag = "water_absorb_r_slider"
            )
            WaterLabeledSlider(
                label = "Green Extinction (σ_G)",
                value = optics.absorbG,
                valueRange = 0.0f..1.5f,
                formattedValue = "${"%.2f".format(optics.absorbG)}",
                onValueChange = { onOpticsChange(optics.copy(absorbG = it)) },
                testTag = "water_absorb_g_slider"
            )
            WaterLabeledSlider(
                label = "Blue Extinction (σ_B)",
                value = optics.absorbB,
                valueRange = 0.0f..1.5f,
                formattedValue = "${"%.2f".format(optics.absorbB)}",
                onValueChange = { onOpticsChange(optics.copy(absorbB = it)) },
                testTag = "water_absorb_b_slider"
            )
            WaterLabeledSlider(
                label = "Index of Refraction (IOR)",
                value = optics.indexOfRefraction,
                valueRange = 1.00f..1.80f,
                formattedValue = "${"%.3f".format(optics.indexOfRefraction)}",
                onValueChange = { onOpticsChange(optics.copy(indexOfRefraction = it)) },
                testTag = "water_ior_slider"
            )
            WaterLabeledSlider(
                label = "Refraction Distortion Strength",
                value = optics.refractionStrength,
                valueRange = 0.0f..0.45f,
                formattedValue = "${"%.2f".format(optics.refractionStrength)}",
                onValueChange = { onOpticsChange(optics.copy(refractionStrength = it)) },
                testTag = "water_refraction_strength_slider"
            )
            WaterLabeledSlider(
                label = "Schlick Fresnel Power",
                value = optics.fresnelPower,
                valueRange = 1.0f..8.0f,
                formattedValue = "${"%.1f".format(optics.fresnelPower)}",
                onValueChange = { onOpticsChange(optics.copy(fresnelPower = it)) },
                testTag = "water_fresnel_power_slider"
            )
            WaterLabeledSlider(
                label = "Fresnel F₀ Reflectance Bias",
                value = optics.fresnelBias,
                valueRange = 0.0f..0.5f,
                formattedValue = "${"%.2f".format(optics.fresnelBias)}",
                onValueChange = { onOpticsChange(optics.copy(fresnelBias = it)) },
                testTag = "water_fresnel_bias_slider"
            )
            WaterLabeledSlider(
                label = "Sky Reflection Intensity",
                value = optics.reflectionIntensity,
                valueRange = 0.0f..1.6f,
                formattedValue = "${"%.2f".format(optics.reflectionIntensity)}",
                onValueChange = { onOpticsChange(optics.copy(reflectionIntensity = it)) },
                testTag = "water_reflection_intensity_slider"
            )
            WaterLabeledSlider(
                label = "Sun Specular Power",
                value = optics.specularPower,
                valueRange = 16f..320f,
                formattedValue = "${optics.specularPower.roundToInt()}",
                onValueChange = { onOpticsChange(optics.copy(specularPower = it)) },
                testTag = "water_specular_power_slider"
            )
            WaterLabeledSlider(
                label = "Velocity Foam Threshold",
                value = optics.foamThreshold,
                valueRange = 0.5f..4.0f,
                formattedValue = "${"%.2f".format(optics.foamThreshold)} m/s",
                onValueChange = { onOpticsChange(optics.copy(foamThreshold = it)) },
                testTag = "water_foam_threshold_slider"
            )
            WaterLabeledSlider(
                label = "Floor Caustics Intensity",
                value = optics.causticsIntensity,
                valueRange = 0.0f..1.5f,
                formattedValue = "${"%.2f".format(optics.causticsIntensity)}",
                onValueChange = { onOpticsChange(optics.copy(causticsIntensity = it)) },
                testTag = "water_caustics_slider"
            )
        }

        // Sun Direction
        WaterSectionCard(
            title = "Sun Lighting Direction",
            subtitle = "Controls directional specular glint & submerged sphere shading",
            icon = Icons.Default.WbSunny
        ) {
            WaterLabeledSlider(
                label = "Sun Azimuth",
                value = optics.sunAzimuthDeg,
                valueRange = -180f..180f,
                formattedValue = "${optics.sunAzimuthDeg.roundToInt()}°",
                onValueChange = { onOpticsChange(optics.copy(sunAzimuthDeg = it)) },
                testTag = "water_sun_azimuth_slider"
            )
            WaterLabeledSlider(
                label = "Sun Elevation",
                value = optics.sunElevationDeg,
                valueRange = 8f..88f,
                formattedValue = "${optics.sunElevationDeg.roundToInt()}°",
                onValueChange = { onOpticsChange(optics.copy(sunElevationDeg = it)) },
                testTag = "water_sun_elevation_slider"
            )
        }
        Spacer(Modifier.height(20.dp))
    }
}
