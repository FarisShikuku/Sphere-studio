package com.example.sphere.water.tabs

import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.GridOn
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material.icons.filled.Terrain
import androidx.compose.material.icons.filled.ViewInAr
import androidx.compose.material.icons.filled.Waves
import androidx.compose.material.icons.filled.WbSunny
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.WaterSurfaceDebugViewType
import com.example.sphere.model.WaterSurfacePreset
import com.example.sphere.model.WaterSurfaceState
import com.example.sphere.model.WaterTelemetryState
import com.example.sphere.model.waterCutoutSizePresets
import com.example.sphere.model.waterSurfacePresetsList
import kotlin.math.abs
import kotlin.math.roundToInt

@Composable
fun WaterSurfaceTab(
    surface: WaterSurfaceState,
    telemetry: WaterTelemetryState,
    onSurfaceChange: (WaterSurfaceState) -> Unit,
    onApplyPreset: (WaterSurfacePreset) -> Unit,
    onResetSurface: () -> Unit
) {
    val peakWindWaveL = (surface.windSpeedMs * surface.windSpeedMs) / 9.81f
    val gridSpacingM = surface.patchSizeM / surface.gridResolution.coerceAtLeast(32)

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(horizontal = 14.dp, vertical = 10.dp)
            .testTag("water_surface_tab")
    ) {
        // Tessendorf FFT Ocean Surface Master Control & Presets
        WaterSectionCard(
            title = "Water Surface (Tessendorf 2D FFT)",
            subtitle = "Ported from kentril0/WaterSurfaceRendering: Phillips spectrum, Preetham sky & Baboud-Décoret optics",
            icon = Icons.Default.Waves,
            accentColor = Color(0xFF38BDF8)
        ) {
            WaterToggleRow(
                title = "Enable 2D FFT Ocean Surface Mode",
                subtitle = "Switches viewport between 3D SPH Fluid/Waterfall and GPU-mirrored FFT Water Surface",
                checked = surface.enabled,
                onCheckedChange = { onSurfaceChange(surface.copy(enabled = it)) },
                testTag = "water_surface_enable_switch"
            )

            Spacer(Modifier.height(6.dp))

            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState())
                    .padding(bottom = 8.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                waterSurfacePresetsList.forEach { preset ->
                    val isSelected = surface.enabled &&
                        abs(surface.windSpeedMs - preset.state.windSpeedMs) < 0.2f &&
                        abs(surface.patchSizeM - preset.state.patchSizeM) < 1.0f &&
                        surface.gridResolution == preset.state.gridResolution
                    FilterChip(
                        selected = isSelected,
                        onClick = { onApplyPreset(preset) },
                        label = { Text("${preset.icon} ${preset.name}") },
                        modifier = Modifier.testTag(
                            "water_surface_preset_${preset.name.lowercase().replace(" ", "_")}"
                        )
                    )
                }
            }

            // Live Ocean Surface Metrics
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(bottom = 8.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                SurfaceMetricChip(
                    label = "Sig Wave H_s",
                    value = "${"%.2f".format(if (surface.enabled) telemetry.avgDensity else 0f)} m",
                    sub = "Peak ${"%.2f".format(if (surface.enabled) telemetry.maxSpeed else 0f)} m",
                    modifier = Modifier.weight(1f)
                )
                SurfaceMetricChip(
                    label = "FFT Grid",
                    value = "${surface.gridResolution}×${surface.gridResolution}",
                    sub = "Δx ${"%.2f".format(gridSpacingM)} m",
                    modifier = Modifier.weight(1f)
                )
                SurfaceMetricChip(
                    label = "Max λ_w",
                    value = "${"%.1f".format(peakWindWaveL)} m",
                    sub = "V²/g fetch",
                    modifier = Modifier.weight(1f)
                )
            }

            FilledTonalButton(
                onClick = onResetSurface,
                modifier = Modifier
                    .fillMaxWidth()
                    .height(48.dp)
                    .testTag("water_surface_reset_button")
            ) {
                Icon(Icons.Default.Refresh, contentDescription = null, modifier = Modifier.size(18.dp))
                Spacer(Modifier.width(8.dp))
                Text("Reset FFT Wave Field & Seabed", fontWeight = FontWeight.SemiBold)
            }
        }

        // Compact 3D Ocean Cutaway Block (e.g. 10m x 10m x 10m section) for Clear Control Inspection
        val cutoutVolumeM3 = surface.cutoutWidthM * surface.cutoutHeightM * surface.cutoutDepthM
        WaterSectionCard(
            title = "Ocean Cutaway Block (W × H × D)",
            subtitle = "Isolate a small 3D ocean slice (e.g. 10×10×10 m) to inspect waves, underwater column, seabed & control performance",
            icon = Icons.Default.ViewInAr,
            accentColor = Color(0xFF06B6D4)
        ) {
            WaterToggleRow(
                title = "Small Ocean Cutaway Block",
                subtitle = "Renders bounded W×H×D cross-section with translucent water sides & rock seabed",
                checked = surface.cutoutEnabled,
                onCheckedChange = { enabled ->
                    onSurfaceChange(surface.copy(enabled = true, cutoutEnabled = enabled))
                },
                testTag = "water_surface_cutout_switch"
            )

            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState())
                    .padding(vertical = 6.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                waterCutoutSizePresets.forEach { sizePreset ->
                    val isSizeSelected = surface.cutoutEnabled &&
                        abs(surface.cutoutWidthM - sizePreset.widthM) < 0.2f &&
                        abs(surface.cutoutHeightM - sizePreset.heightM) < 0.2f &&
                        abs(surface.cutoutDepthM - sizePreset.depthM) < 0.2f
                    val tagKey = "${sizePreset.widthM.roundToInt()}x${sizePreset.heightM.roundToInt()}x${sizePreset.depthM.roundToInt()}"
                    FilterChip(
                        selected = isSizeSelected,
                        onClick = {
                            onSurfaceChange(
                                surface.copy(
                                    enabled = true,
                                    cutoutEnabled = true,
                                    cutoutWidthM = sizePreset.widthM,
                                    cutoutHeightM = sizePreset.heightM,
                                    cutoutDepthM = sizePreset.depthM
                                )
                            )
                        },
                        label = { Text(sizePreset.label) },
                        modifier = Modifier.testTag("water_surface_cutout_chip_$tagKey")
                    )
                }
            }

            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(bottom = 8.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                SurfaceMetricChip(
                    label = "Cut Volume",
                    value = "${cutoutVolumeM3.roundToInt()} m³",
                    sub = "${surface.cutoutWidthM.roundToInt()}×${surface.cutoutHeightM.roundToInt()}×${surface.cutoutDepthM.roundToInt()} m",
                    modifier = Modifier.weight(1f)
                )
                SurfaceMetricChip(
                    label = "FFT Step",
                    value = "${"%.2f".format(if (surface.enabled) telemetry.physicsStepMs else 0f)} ms",
                    sub = "${surface.gridResolution}×${surface.gridResolution} IFFT",
                    modifier = Modifier.weight(1f)
                )
                SurfaceMetricChip(
                    label = "Surf Render",
                    value = "${"%.1f".format(if (surface.enabled) telemetry.renderPassMs else 0f)} ms",
                    sub = if (surface.cutoutEnabled) "Cutaway Rays" else "Full Ocean",
                    modifier = Modifier.weight(1f)
                )
            }

            WaterLabeledSlider(
                label = "Cut Width (X)",
                value = surface.cutoutWidthM,
                valueRange = 3.0f..32.0f,
                formattedValue = "${"%.1f".format(surface.cutoutWidthM)} m",
                onValueChange = {
                    onSurfaceChange(surface.copy(enabled = true, cutoutEnabled = true, cutoutWidthM = it))
                },
                testTag = "water_surface_cutout_width_slider"
            )
            WaterLabeledSlider(
                label = "Cut Height / Column (Y)",
                value = surface.cutoutHeightM,
                valueRange = 3.0f..24.0f,
                formattedValue = "${"%.1f".format(surface.cutoutHeightM)} m",
                onValueChange = {
                    onSurfaceChange(surface.copy(enabled = true, cutoutEnabled = true, cutoutHeightM = it))
                },
                testTag = "water_surface_cutout_height_slider"
            )
            WaterLabeledSlider(
                label = "Cut Depth (Z)",
                value = surface.cutoutDepthM,
                valueRange = 3.0f..32.0f,
                formattedValue = "${"%.1f".format(surface.cutoutDepthM)} m",
                onValueChange = {
                    onSurfaceChange(surface.copy(enabled = true, cutoutEnabled = true, cutoutDepthM = it))
                },
                testTag = "water_surface_cutout_depth_slider"
            )
        }

        // Texture & Shader Debug Modes (Displacement, Normal, Seabed, Fresnel, Wireframe)
        WaterSectionCard(
            title = "Water Surface Shader & Texture Views",
            subtitle = "Inspect 2D FFT Displacement Map, Normal/Jacobian Map, Underwater Terrain & Wireframe Grid",
            icon = Icons.Default.GridOn,
            accentColor = Color(0xFF22D3EE)
        ) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState()),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                WaterSurfaceDebugViewType.entries.forEach { mode ->
                    FilterChip(
                        selected = surface.debugMode == mode,
                        onClick = { onSurfaceChange(surface.copy(enabled = true, debugMode = mode)) },
                        label = { Text(mode.label) },
                        modifier = Modifier.testTag("water_surface_debug_${mode.name.lowercase()}")
                    )
                }
            }
            Text(
                text = "Shader Stage: ${surface.debugMode.shaderPassName}",
                style = MaterialTheme.typography.bodySmall,
                color = MaterialTheme.colorScheme.primary,
                modifier = Modifier.padding(top = 6.dp)
            )

            Spacer(Modifier.height(8.dp))
            Text(
                text = "FFT Texture Resolution (${surface.gridResolution}×${surface.gridResolution})",
                style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Medium),
                modifier = Modifier.padding(bottom = 4.dp)
            )
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                listOf(32, 64, 128).forEach { res ->
                    FilterChip(
                        selected = surface.gridResolution == res,
                        onClick = { onSurfaceChange(surface.copy(enabled = true, gridResolution = res)) },
                        label = { Text("${res}×${res}") },
                        modifier = Modifier
                            .weight(1f)
                            .testTag("water_surface_fft_res_$res")
                    )
                }
            }
        }

        // Tessendorf Phillips Spectrum & Choppiness Controls
        WaterSectionCard(
            title = "Phillips Spectrum & Choppy Waves (FFT)",
            subtitle = "Controls wind energy V, tile patch length L, wave amplitude A & horizontal pinch λ",
            icon = Icons.Default.Waves
        ) {
            WaterLabeledSlider(
                label = "Wind Speed (V)",
                value = surface.windSpeedMs,
                valueRange = 2.0f..32.0f,
                formattedValue = "${"%.1f".format(surface.windSpeedMs)} m/s",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, windSpeedMs = it)) },
                testTag = "water_surface_wind_speed_slider"
            )
            WaterLabeledSlider(
                label = "Wind Azimuth Angle",
                value = surface.windDirectionDeg,
                valueRange = -180.0f..180.0f,
                formattedValue = "${surface.windDirectionDeg.roundToInt()}°",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, windDirectionDeg = it)) },
                testTag = "water_surface_wind_dir_slider"
            )
            WaterLabeledSlider(
                label = "Ocean Patch Size (L)",
                value = surface.patchSizeM,
                valueRange = 8.0f..180.0f,
                formattedValue = "${surface.patchSizeM.roundToInt()} m",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, patchSizeM = it)) },
                testTag = "water_surface_patch_size_slider"
            )
            WaterLabeledSlider(
                label = "Phillips Amplitude Scale (A)",
                value = surface.amplitudeScale,
                valueRange = 0.10f..1.40f,
                formattedValue = "${"%.2f".format(surface.amplitudeScale)}",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, amplitudeScale = it)) },
                testTag = "water_surface_amplitude_slider"
            )
            WaterLabeledSlider(
                label = "Horizontal Choppiness (λ)",
                value = surface.choppinessLambda,
                valueRange = 0.0f..2.30f,
                formattedValue = "${"%.2f".format(surface.choppinessLambda)}",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, choppinessLambda = it)) },
                testTag = "water_surface_choppiness_slider"
            )
            WaterLabeledSlider(
                label = "Capillary Wave Cutoff (l)",
                value = surface.smallWaveCutoff,
                valueRange = 0.01f..0.35f,
                formattedValue = "${"%.2f".format(surface.smallWaveCutoff)} m",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, smallWaveCutoff = it)) },
                testTag = "water_surface_cutoff_slider"
            )
            WaterLabeledSlider(
                label = "Simulation Time Scale",
                value = surface.timeScale,
                valueRange = 0.15f..2.20f,
                formattedValue = "${"%.2f".format(surface.timeScale)}×",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, timeScale = it)) },
                testTag = "water_surface_timescale_slider"
            )
        }

        // Preetham Sky, Refraction, Beer-Lambert Absorption & Subsurface Scattering
        WaterSectionCard(
            title = "Preetham Sky & Baboud–Décoret Optics",
            subtitle = "Atmospheric scattering, Snell refraction IOR, depth absorption, SSS & crest foam",
            icon = Icons.Default.WbSunny
        ) {
            WaterLabeledSlider(
                label = "Sun Elevation",
                value = surface.sunElevationDeg,
                valueRange = 6.0f..84.0f,
                formattedValue = "${surface.sunElevationDeg.roundToInt()}°",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, sunElevationDeg = it)) },
                testTag = "water_surface_sun_elev_slider"
            )
            WaterLabeledSlider(
                label = "Sun Azimuth",
                value = surface.sunAzimuthDeg,
                valueRange = -180.0f..180.0f,
                formattedValue = "${surface.sunAzimuthDeg.roundToInt()}°",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, sunAzimuthDeg = it)) },
                testTag = "water_surface_sun_az_slider"
            )
            WaterLabeledSlider(
                label = "Preetham Sky Turbidity (T)",
                value = surface.skyTurbidity,
                valueRange = 1.8f..8.0f,
                formattedValue = "${"%.1f".format(surface.skyTurbidity)}",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, skyTurbidity = it)) },
                testTag = "water_surface_turbidity_slider"
            )
            WaterLabeledSlider(
                label = "Refractive Index (IOR)",
                value = surface.indexOfRefraction,
                valueRange = 1.05f..1.65f,
                formattedValue = "${"%.3f".format(surface.indexOfRefraction)}",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, indexOfRefraction = it)) },
                testTag = "water_surface_ior_slider"
            )
            WaterLabeledSlider(
                label = "Beer-Lambert Depth Absorption",
                value = surface.absorptionStrength,
                valueRange = 0.08f..2.20f,
                formattedValue = "${"%.2f".format(surface.absorptionStrength)}",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, absorptionStrength = it)) },
                testTag = "water_surface_absorption_slider"
            )
            WaterLabeledSlider(
                label = "Wave Crest Subsurface Scatter",
                value = surface.scatterStrength,
                valueRange = 0.0f..1.40f,
                formattedValue = "${"%.2f".format(surface.scatterStrength)}",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, scatterStrength = it)) },
                testTag = "water_surface_scatter_slider"
            )
            WaterLabeledSlider(
                label = "Jacobian Crest Foam Threshold",
                value = surface.foamThreshold,
                valueRange = -0.35f..1.10f,
                formattedValue = "${"%.2f".format(surface.foamThreshold)}",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, foamThreshold = it)) },
                testTag = "water_surface_foam_slider"
            )
            WaterLabeledSlider(
                label = "Sky Fresnel Reflection Strength",
                value = surface.reflectionStrength,
                valueRange = 0.0f..1.50f,
                formattedValue = "${"%.2f".format(surface.reflectionStrength)}×",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, reflectionStrength = it)) },
                testTag = "water_surface_reflection_slider"
            )
        }

        // Value-Noise Underwater Terrain
        WaterSectionCard(
            title = "Value-Noise Underwater Terrain",
            subtitle = "Procedural sandbar/trench seabed intersected by refracted rays",
            icon = Icons.Default.Terrain
        ) {
            WaterLabeledSlider(
                label = "Mean Basin Depth",
                value = surface.waterDepthM,
                valueRange = 1.5f..20.0f,
                formattedValue = "${"%.1f".format(surface.waterDepthM)} m",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, waterDepthM = it)) },
                testTag = "water_surface_depth_slider"
            )
            WaterLabeledSlider(
                label = "Seabed Value-Noise Relief",
                value = surface.seabedRoughness,
                valueRange = 0.0f..4.5f,
                formattedValue = "${"%.2f".format(surface.seabedRoughness)} m",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, seabedRoughness = it)) },
                testTag = "water_surface_seabed_roughness_slider"
            )
            WaterLabeledSlider(
                label = "Seabed Noise Frequency",
                value = surface.seabedScale,
                valueRange = 0.04f..0.38f,
                formattedValue = "${"%.2f".format(surface.seabedScale)}",
                onValueChange = { onSurfaceChange(surface.copy(enabled = true, seabedScale = it)) },
                testTag = "water_surface_seabed_scale_slider"
            )
        }

        Spacer(Modifier.height(20.dp))
    }
}

@Composable
private fun SurfaceMetricChip(
    label: String,
    value: String,
    sub: String,
    modifier: Modifier = Modifier
) {
    Surface(
        shape = RoundedCornerShape(10.dp),
        color = Color(0xFF0F172A),
        modifier = modifier
    ) {
        Column(modifier = Modifier.padding(horizontal = 8.dp, vertical = 7.dp)) {
            Text(label, fontSize = 10.sp, color = Color(0xFF94A3B8))
            Text(
                value,
                fontSize = 11.sp,
                fontWeight = FontWeight.Bold,
                fontFamily = FontFamily.Monospace,
                color = Color(0xFF38BDF8)
            )
            Text(sub, fontSize = 9.sp, fontFamily = FontFamily.Monospace, color = Color(0xFF64748B))
        }
    }
}
