package com.example.sphere.buildings.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.unit.dp
import com.example.sphere.model.BuildingGlassType
import com.example.sphere.model.BuildingRoofType
import com.example.sphere.model.BuildingStudioState
import kotlin.math.roundToInt

@Composable
fun BuildingRoofInteriorTab(
    state: BuildingStudioState,
    onStateChange: (BuildingStudioState) -> Unit,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 12.dp),
        verticalArrangement = Arrangement.spacedBy(14.dp)
    ) {
        // Roof Design (All 22 RoofTypes)
        SectionHeader("Roof Crown & Geometry (22 Forms)", "Gables, domes, pagodas, vaults & green terraces")
        Row(
            modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            BuildingRoofType.entries.forEach { roof ->
                FilterChip(
                    selected = state.roofType == roof,
                    onClick = { onStateChange(state.copy(roofType = roof)) },
                    label = { Text(roof.displayName) },
                    modifier = Modifier.testTag("building_roof_chip_${roof.ordinalId}")
                )
            }
        }

        ElevatedCard(shape = RoundedCornerShape(14.dp)) {
            Column(
                modifier = Modifier.padding(14.dp),
                verticalArrangement = Arrangement.spacedBy(10.dp)
            ) {
                LabeledFloatSlider(
                    label = "Roof Crown Height",
                    valueText = "${String.format("%.2f", state.roofHeight)} m",
                    value = state.roofHeight,
                    range = 0.4f..10.0f,
                    tag = "slider_building_roof_height",
                    onValueChange = { onStateChange(state.copy(roofHeight = it)) }
                )
                LabeledFloatSlider(
                    label = "Eave Overhang",
                    valueText = "${String.format("%.2f", state.roofOverhang)} m",
                    value = state.roofOverhang,
                    range = 0.0f..2.6f,
                    tag = "slider_building_roof_overhang",
                    onValueChange = { onStateChange(state.copy(roofOverhang = it)) }
                )
                LabeledSwitchRow(
                    title = "Rooftop HVAC, Water Tanks & Antennas",
                    subtitle = "Attach procedural mechanical clutter via socket points",
                    checked = state.showRooftopClutter,
                    tag = "switch_building_rooftop_clutter",
                    onCheckedChange = { onStateChange(state.copy(showRooftopClutter = it)) }
                )
            }
        }

        // Glazing & Optical IOR
        SectionHeader("Glazing & Schlick Fresnel Optics", "9 architectural glass tints & privacy IOR")
        Row(
            modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            BuildingGlassType.entries.forEach { glass ->
                FilterChip(
                    selected = state.glassType == glass,
                    onClick = { onStateChange(state.copy(glassType = glass)) },
                    leadingIcon = {
                        Box(
                            modifier = Modifier
                                .size(12.dp)
                                .clip(CircleShape)
                                .background(glass.swatchColor)
                        )
                    },
                    label = { Text(glass.displayName) },
                    modifier = Modifier.testTag("building_glass_chip_${glass.ordinalId}")
                )
            }
        }

        ElevatedCard(shape = RoundedCornerShape(14.dp)) {
            Column(
                modifier = Modifier.padding(14.dp),
                verticalArrangement = Arrangement.spacedBy(10.dp)
            ) {
                val opacityDesc = when {
                    state.windowIor < 1.18f -> "Transparent (See-through)"
                    state.windowIor < 1.48f -> "Semi-Reflective Architectural"
                    else -> "Privacy Mirrored (Opaque)"
                }
                LabeledFloatSlider(
                    label = "Window Glass IOR ($opacityDesc)",
                    valueText = String.format("%.2f", state.windowIor),
                    value = state.windowIor,
                    range = 1.00f..2.80f,
                    tag = "slider_building_glass_ior",
                    onValueChange = { onStateChange(state.copy(windowIor = it)) }
                )
            }
        }

        // Structural Circulation & Interior Cutaway
        SectionHeader("Columns, Fire Escapes & Interior Fitout", "Substructure & interior cutaway visualization")
        ElevatedCard(shape = RoundedCornerShape(14.dp)) {
            Column(
                modifier = Modifier.padding(14.dp),
                verticalArrangement = Arrangement.spacedBy(10.dp)
            ) {
                LabeledSwitchRow(
                    title = "Ground Colonnade Pillars",
                    subtitle = "Elevate base volume on structural pilotis",
                    checked = state.hasGroundPillars,
                    tag = "switch_building_pillars",
                    onCheckedChange = { onStateChange(state.copy(hasGroundPillars = it)) }
                )
                if (state.hasGroundPillars) {
                    LabeledFloatSlider(
                        label = "Pillar Column Count",
                        valueText = "${state.pillarCount} columns",
                        value = state.pillarCount.toFloat(),
                        range = 2f..12f,
                        tag = "slider_building_pillar_count",
                        onValueChange = { onStateChange(state.copy(pillarCount = it.roundToInt().coerceIn(2, 12))) }
                    )
                }
                LabeledSwitchRow(
                    title = "Exterior Steel Fire Escape",
                    subtitle = "Zig-zag galvanized stairways & landings",
                    checked = state.hasFireEscape,
                    tag = "switch_building_fire_escape",
                    onCheckedChange = { onStateChange(state.copy(hasFireEscape = it)) }
                )
                LabeledSwitchRow(
                    title = "Show Inside Cutaway (Double-Sided)",
                    subtitle = "Open shell & render interior partitions/racking",
                    checked = state.showInside,
                    tag = "switch_building_show_inside",
                    onCheckedChange = { onStateChange(state.copy(showInside = it)) }
                )
                if (state.showInside || state.domainOrdinal == 7) {
                    Text("Interior Fitout Layout", style = MaterialTheme.typography.labelLarge)
                    Row(
                        modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        val designs = listOf(
                            0 to "Executive / Living",
                            1 to "Open Office Pods",
                            2 to "Logistics Racking",
                            3 to "Repair Service Bay"
                        )
                        designs.forEach { (id, label) ->
                            FilterChip(
                                selected = state.interiorDesign == id,
                                onClick = { onStateChange(state.copy(interiorDesign = id)) },
                                label = { Text(label) },
                                modifier = Modifier.testTag("building_interior_design_$id")
                            )
                        }
                    }
                }
            }
        }
    }
}
