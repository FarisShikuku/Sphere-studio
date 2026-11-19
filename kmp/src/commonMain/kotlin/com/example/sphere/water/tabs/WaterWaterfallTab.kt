package com.example.sphere.water.tabs

import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Air
import androidx.compose.material.icons.filled.Landscape
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material.icons.filled.Terrain
import androidx.compose.material.icons.filled.Water
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
import com.example.sphere.model.*
import kotlin.math.abs
import kotlin.math.roundToInt
import kotlin.math.sqrt

@Composable
fun WaterWaterfallTab(
    waterfall: WaterWaterfallState,
    physics: WaterPhysicsState,
    onWaterfallChange: (WaterWaterfallState) -> Unit,
    onPhysicsChange: (WaterPhysicsState) -> Unit,
    onApplyWaterfallPreset: (WaterfallCascadePreset) -> Unit,
    onActivateAndRespawnWaterfall: () -> Unit
) {
    val gMag = abs(physics.gravityY).coerceAtLeast(0.2f)
    val dropHeight = (waterfall.cliffHeight - waterfall.plungePoolDepth * 0.5f).coerceAtLeast(0.25f)
    val impactVelMs = sqrt(2f * gMag * dropHeight)
    val fallTimeSec = sqrt(2f * dropHeight / gMag)
    val estDischargeM3s = waterfall.cliffWidth * waterfall.flowVolume * 0.42f

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(horizontal = 14.dp, vertical = 10.dp)
            .testTag("water_waterfall_tab")
    ) {
        // Waterfall Simulation Mode & Presets
        WaterSectionCard(
            title = "Waterfall Over Cliff Simulation",
            subtitle = "Continuous weir discharge flowing over an analytical 3D basalt/granite CliffSDF",
            icon = Icons.Default.Landscape,
            accentColor = Color(0xFF38BDF8)
        ) {
            WaterToggleRow(
                title = "Enable Waterfall & 3D Cliff Geometry",
                subtitle = "Activates rocky cliff ledge, continuous upper river feeder & plunge recirculation",
                checked = waterfall.enabled,
                onCheckedChange = { enabled ->
                    onWaterfallChange(waterfall.copy(enabled = enabled))
                    if (enabled && physics.scenario != WaterSpawnScenarioType.WATERFALL_CLIFF) {
                        onActivateAndRespawnWaterfall()
                    }
                },
                testTag = "waterfall_enable_switch"
            )

            Spacer(Modifier.height(6.dp))

            // Cascade Presets
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState())
                    .padding(bottom = 8.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                waterfallCascadePresetsList.forEach { preset ->
                    FilterChip(
                        selected = waterfall.enabled &&
                            abs(waterfall.cliffHeight - preset.waterfall.cliffHeight) < 0.03f &&
                            waterfall.steppedLedges == preset.waterfall.steppedLedges,
                        onClick = { onApplyWaterfallPreset(preset) },
                        label = { Text("${preset.icon} ${preset.name}") },
                        modifier = Modifier.testTag("waterfall_preset_${preset.name.lowercase().replace(" ", "_")}")
                    )
                }
            }

            // Real-time Hydraulic Telemetry Readout
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(bottom = 8.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                WaterfallMetricChip(
                    label = "Impact v",
                    value = "${"%.2f".format(impactVelMs)} m/s",
                    sub = "√(2gH)",
                    modifier = Modifier.weight(1f)
                )
                WaterfallMetricChip(
                    label = "Discharge Q",
                    value = "${"%.2f".format(estDischargeM3s)} m³/s",
                    sub = "W×Vol flux",
                    modifier = Modifier.weight(1f)
                )
                WaterfallMetricChip(
                    label = "Nappe Time",
                    value = "${"%.2f".format(fallTimeSec)} s",
                    sub = "${waterfall.steppedLedges} ledges",
                    modifier = Modifier.weight(1f)
                )
            }

            FilledTonalButton(
                onClick = onActivateAndRespawnWaterfall,
                modifier = Modifier
                    .fillMaxWidth()
                    .height(48.dp)
                    .testTag("waterfall_respawn_button")
            ) {
                Icon(Icons.Default.Refresh, contentDescription = null, modifier = Modifier.size(18.dp))
                Spacer(Modifier.width(8.dp))
                Text("Respawn Waterfall Cascade", fontWeight = FontWeight.SemiBold)
            }
        }

        // Core Waterfall Dimensions & Flow Volume
        WaterSectionCard(
            title = "Waterfall Dimensions & Volume",
            subtitle = "Height, chute width, upper cliff ledge depth, discharge volume & plunge basin",
            icon = Icons.Default.Water
        ) {
            WaterLabeledSlider(
                label = "Cliff / Waterfall Height (H)",
                value = waterfall.cliffHeight,
                valueRange = 0.60f..2.20f,
                formattedValue = "${"%.2f".format(waterfall.cliffHeight)} m",
                onValueChange = { onWaterfallChange(waterfall.copy(enabled = true, cliffHeight = it)) },
                testTag = "waterfall_height_slider"
            )
            WaterLabeledSlider(
                label = "Cascade / Chute Width (W)",
                value = waterfall.cliffWidth,
                valueRange = 0.45f..2.30f,
                formattedValue = "${"%.2f".format(waterfall.cliffWidth)} m",
                onValueChange = { onWaterfallChange(waterfall.copy(enabled = true, cliffWidth = it)) },
                testTag = "waterfall_width_slider"
            )
            WaterLabeledSlider(
                label = "Cliff Plateau Depth (D)",
                value = waterfall.cliffDepth,
                valueRange = 0.40f..1.65f,
                formattedValue = "${"%.2f".format(waterfall.cliffDepth)} m",
                onValueChange = { onWaterfallChange(waterfall.copy(enabled = true, cliffDepth = it)) },
                testTag = "waterfall_depth_slider"
            )
            WaterLabeledSlider(
                label = "Water Discharge Volume (Q)",
                value = waterfall.flowVolume,
                valueRange = 0.20f..2.50f,
                formattedValue = "${"%.2f".format(waterfall.flowVolume)}x flux",
                onValueChange = { onWaterfallChange(waterfall.copy(enabled = true, flowVolume = it)) },
                testTag = "waterfall_volume_slider"
            )
            WaterLabeledSlider(
                label = "Plunge Pool Depth",
                value = waterfall.plungePoolDepth,
                valueRange = 0.06f..0.80f,
                formattedValue = "${"%.2f".format(waterfall.plungePoolDepth)} m",
                onValueChange = { onWaterfallChange(waterfall.copy(enabled = true, plungePoolDepth = it)) },
                testTag = "waterfall_plunge_pool_depth_slider"
            )
        }

        // Natural Geological Cliff Constraints
        WaterSectionCard(
            title = "Natural Cliff Constraints",
            subtitle = "Terrace ledges, overhang undercut, crest slope, rock friction & gorge walls",
            icon = Icons.Default.Terrain
        ) {
            Text(
                text = "Stepped Rock Terraces (${waterfall.steppedLedges} tiers)",
                style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Medium),
                modifier = Modifier.padding(bottom = 4.dp)
            )
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(bottom = 8.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                listOf(0 to "Sheer", 1 to "1 Tier", 2 to "2 Tiers", 3 to "3 Tiers").forEach { (tiers, label) ->
                    FilterChip(
                        selected = waterfall.steppedLedges == tiers,
                        onClick = { onWaterfallChange(waterfall.copy(enabled = true, steppedLedges = tiers)) },
                        label = { Text(label) },
                        modifier = Modifier
                            .weight(1f)
                            .testTag("waterfall_ledges_$tiers")
                    )
                }
            }

            WaterLabeledSlider(
                label = "Crest Lip Slope Angle",
                value = waterfall.lipSlopeDeg,
                valueRange = 0.0f..42.0f,
                formattedValue = "${waterfall.lipSlopeDeg.roundToInt()}°",
                onValueChange = { onWaterfallChange(waterfall.copy(enabled = true, lipSlopeDeg = it)) },
                testTag = "waterfall_lip_slope_slider"
            )
            WaterLabeledSlider(
                label = "Rock Overhang / Undercut",
                value = waterfall.undercut,
                valueRange = 0.0f..0.45f,
                formattedValue = "${"%.2f".format(waterfall.undercut)} m",
                onValueChange = { onWaterfallChange(waterfall.copy(enabled = true, undercut = it)) },
                testTag = "waterfall_undercut_slider"
            )
            WaterLabeledSlider(
                label = "Rock Bed Roughness & Friction",
                value = waterfall.roughness,
                valueRange = 0.0f..1.0f,
                formattedValue = "${"%.2f".format(waterfall.roughness)}",
                onValueChange = { onWaterfallChange(waterfall.copy(enabled = true, roughness = it)) },
                testTag = "waterfall_roughness_slider"
            )
            WaterLabeledSlider(
                label = "Gorge Side-Channeling Walls",
                value = waterfall.rockChanneling,
                valueRange = 0.0f..1.0f,
                formattedValue = "${"%.2f".format(waterfall.rockChanneling)}",
                onValueChange = { onWaterfallChange(waterfall.copy(enabled = true, rockChanneling = it)) },
                testTag = "waterfall_channeling_slider"
            )
        }

        // Environmental Atmospheric & Hydraulic Constraints
        WaterSectionCard(
            title = "Environmental & Atmospheric Constraints",
            subtitle = "Aerodynamic drag, mist breakup, canyon crosswind & gravitational acceleration",
            icon = Icons.Default.Air
        ) {
            WaterLabeledSlider(
                label = "Air Drag & Cascade Mist Breakup",
                value = waterfall.airDragAndMist,
                valueRange = 0.0f..1.0f,
                formattedValue = "${"%.2f".format(waterfall.airDragAndMist)}",
                onValueChange = { onWaterfallChange(waterfall.copy(enabled = true, airDragAndMist = it)) },
                testTag = "waterfall_air_drag_mist_slider"
            )
            WaterLabeledSlider(
                label = "Canyon Crosswind (Z Lateral Gust)",
                value = physics.windForceZ,
                valueRange = -5.0f..5.0f,
                formattedValue = "${"%.1f".format(physics.windForceZ)} m/s²",
                onValueChange = { onPhysicsChange(physics.copy(windForceZ = it)) },
                testTag = "waterfall_wind_z_slider"
            )
            WaterLabeledSlider(
                label = "Updraft / Tail-Wind (X)",
                value = physics.windForceX,
                valueRange = -5.0f..5.0f,
                formattedValue = "${"%.1f".format(physics.windForceX)} m/s²",
                onValueChange = { onPhysicsChange(physics.copy(windForceX = it)) },
                testTag = "waterfall_wind_x_slider"
            )
            WaterLabeledSlider(
                label = "Gravity Acceleration (g_Y)",
                value = physics.gravityY,
                valueRange = -18.0f..-1.5f,
                formattedValue = "${"%.2f".format(physics.gravityY)} m/s²",
                onValueChange = { onPhysicsChange(physics.copy(gravityY = it)) },
                testTag = "waterfall_gravity_y_slider"
            )
        }

        Spacer(Modifier.height(20.dp))
    }
}

@Composable
private fun WaterfallMetricChip(
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
