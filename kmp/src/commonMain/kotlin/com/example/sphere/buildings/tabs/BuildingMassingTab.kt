package com.example.sphere.buildings.tabs

import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import com.example.sphere.model.BuildingCornerType
import com.example.sphere.model.BuildingStepMode
import com.example.sphere.model.BuildingStudioCategory
import com.example.sphere.model.BuildingStudioState
import com.example.sphere.model.BuildingVolumeShape
import kotlin.math.roundToInt

@Composable
fun BuildingMassingTab(
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
        // Parametric Volume Shapes (only procedural volume shapes for Massing domain, or show quick info for architectural presets)
        if (state.domainOrdinal == 0) {
            SectionHeader("Volume Massing Shape (27 Forms)", "ProceduralBuildingGenerator")
            Row(
                modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                BuildingVolumeShape.entries.forEach { shape ->
                    FilterChip(
                        selected = state.shape == shape,
                        onClick = { onStateChange(state.copy(shape = shape)) },
                        label = { Text(shape.displayName) },
                        modifier = Modifier.testTag("building_shape_chip_${shape.ordinalId}")
                    )
                }
            }

            SectionHeader("Corner Profile & Edge Treatment", "Chamfer, Fillet & Reveals")
            Row(
                modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                BuildingCornerType.entries.forEach { corner ->
                    FilterChip(
                        selected = state.cornerType == corner,
                        onClick = { onStateChange(state.copy(cornerType = corner)) },
                        label = { Text(corner.displayName) },
                        modifier = Modifier.testTag("building_corner_chip_${corner.ordinalId}")
                    )
                }
            }
            if (state.cornerType != BuildingCornerType.SHARP_90) {
                LabeledFloatSlider(
                    label = "Corner Profile Size",
                    valueText = "${String.format("%.1f", state.cornerSize)} m",
                    value = state.cornerSize,
                    range = 0.4f..6.0f,
                    tag = "slider_building_corner_size",
                    onValueChange = { onStateChange(state.copy(cornerSize = it)) }
                )
            }

            SectionHeader("Vertical Step-Back & Taper", "Setback Tiers & Cantilevering")
            Row(
                modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                BuildingStepMode.entries.forEach { mode ->
                    FilterChip(
                        selected = state.stepMode == mode,
                        onClick = { onStateChange(state.copy(stepMode = mode)) },
                        label = { Text(mode.displayName) },
                        modifier = Modifier.testTag("building_step_chip_${mode.ordinalId}")
                    )
                }
            }
            if (state.stepMode != BuildingStepMode.UNIFORM_STRAIGHT) {
                LabeledFloatSlider(
                    label = "Setback Tiers",
                    valueText = "${state.stepTiers} tiers",
                    value = state.stepTiers.toFloat(),
                    range = 2f..6f,
                    tag = "slider_building_step_tiers",
                    onValueChange = { onStateChange(state.copy(stepTiers = it.roundToInt().coerceIn(2, 6))) }
                )
                LabeledFloatSlider(
                    label = "Top Taper Ratio",
                    valueText = "${(state.topTaperRatio * 100).roundToInt()}%",
                    value = state.topTaperRatio,
                    range = 0.25f..0.95f,
                    tag = "slider_building_taper_ratio",
                    onValueChange = { onStateChange(state.copy(topTaperRatio = it)) }
                )
                LabeledFloatSlider(
                    label = "Tier Shrink Factor",
                    valueText = "${(state.stepShrinkFactor * 100).roundToInt()}%",
                    value = state.stepShrinkFactor,
                    range = 0.05f..0.38f,
                    tag = "slider_building_shrink_factor",
                    onValueChange = { onStateChange(state.copy(stepShrinkFactor = it)) }
                )
                LabeledSwitchRow(
                    title = "Terrace Railing & Decks",
                    subtitle = "Parapet balustrades on stepped tiers",
                    checked = state.stepTerraceRailing,
                    tag = "switch_step_terrace_railing",
                    onCheckedChange = { onStateChange(state.copy(stepTerraceRailing = it)) }
                )
            }
        }

        // Dimensions Card (Applies to all building domains)
        SectionHeader("Dimensions & Floor Stacking", "${state.activeCategory.displayName} Envelope")
        ElevatedCard(shape = RoundedCornerShape(14.dp)) {
            Column(
                modifier = Modifier.padding(14.dp),
                verticalArrangement = Arrangement.spacedBy(10.dp)
            ) {
                LabeledFloatSlider(
                    label = "Stories / Floors",
                    valueText = "${state.floorCount} floors",
                    value = state.floorCount.toFloat(),
                    range = 1f..45f,
                    tag = "slider_building_floors",
                    onValueChange = { onStateChange(state.copy(floorCount = it.roundToInt().coerceIn(1, 45))) }
                )
                LabeledFloatSlider(
                    label = "Floor-to-Floor Height",
                    valueText = "${String.format("%.2f", state.floorHeight)} m",
                    value = state.floorHeight,
                    range = 2.4f..6.5f,
                    tag = "slider_building_floor_height",
                    onValueChange = { onStateChange(state.copy(floorHeight = it)) }
                )
                LabeledFloatSlider(
                    label = "Footprint Width",
                    valueText = "${String.format("%.1f", state.width)} m",
                    value = state.width,
                    range = 4.0f..60.0f,
                    tag = "slider_building_width",
                    onValueChange = { onStateChange(state.copy(width = it)) }
                )
                LabeledFloatSlider(
                    label = "Footprint Depth",
                    valueText = "${String.format("%.1f", state.depth)} m",
                    value = state.depth,
                    range = 4.0f..60.0f,
                    tag = "slider_building_depth",
                    onValueChange = { onStateChange(state.copy(depth = it)) }
                )
            }
        }

        // Domain-Specific Structural Options
        DomainSpecificControlsCard(state = state, onStateChange = onStateChange)
    }
}

@Composable
private fun DomainSpecificControlsCard(
    state: BuildingStudioState,
    onStateChange: (BuildingStudioState) -> Unit
) {
    val category = state.activeCategory
    if (category == BuildingStudioCategory.MASSING) return

    SectionHeader("Domain Structural Features", category.subtitle)
    ElevatedCard(shape = RoundedCornerShape(14.dp)) {
        Column(
            modifier = Modifier.padding(14.dp),
            verticalArrangement = Arrangement.spacedBy(10.dp)
        ) {
            when (category) {
                BuildingStudioCategory.RESIDENTIAL -> {
                    LabeledSwitchRow(
                        title = "Attached Garage Bay",
                        subtitle = "Include side garage wing when supported",
                        checked = state.detailFlagA,
                        tag = "switch_res_garage",
                        onCheckedChange = { onStateChange(state.copy(detailFlagA = it)) }
                    )
                }
                BuildingStudioCategory.COMMERCIAL -> {
                    LabeledSwitchRow(
                        title = "Tower Setback Tiers",
                        subtitle = "Enable podium & upper mechanical setbacks",
                        checked = state.detailFlagA,
                        tag = "switch_com_setbacks",
                        onCheckedChange = { onStateChange(state.copy(detailFlagA = it)) }
                    )
                    LabeledSwitchRow(
                        title = "Double-Height Atrium",
                        subtitle = "Glazed public entrance lobby",
                        checked = state.detailFlagB,
                        tag = "switch_com_atrium",
                        onCheckedChange = { onStateChange(state.copy(detailFlagB = it)) }
                    )
                }
                BuildingStudioCategory.INDUSTRIAL -> {
                    LabeledFloatSlider(
                        label = "Loading Docks / Bays",
                        valueText = "${state.detailCount} bays",
                        value = state.detailCount.toFloat(),
                        range = 1f..8f,
                        tag = "slider_ind_bays",
                        onValueChange = { onStateChange(state.copy(detailCount = it.roundToInt().coerceIn(1, 8))) }
                    )
                    LabeledSwitchRow(
                        title = "Interior Mezzanine Catwalks",
                        subtitle = "Steel service catwalks & guardrails",
                        checked = state.detailFlagA,
                        tag = "switch_ind_catwalks",
                        onCheckedChange = { onStateChange(state.copy(detailFlagA = it)) }
                    )
                    LabeledSwitchRow(
                        title = "Northlight Sawtooth Roof",
                        subtitle = "Stepped clerestory industrial skylights",
                        checked = state.detailFlagB,
                        tag = "switch_ind_sawtooth",
                        onCheckedChange = { onStateChange(state.copy(detailFlagB = it)) }
                    )
                }
                BuildingStudioCategory.CIVIC -> {
                    LabeledSwitchRow(
                        title = "Clock / Control Tower",
                        subtitle = "Civic spire, bell tower, or ATC cab",
                        checked = state.detailFlagA,
                        tag = "switch_civ_tower",
                        onCheckedChange = { onStateChange(state.copy(detailFlagA = it)) }
                    )
                    LabeledSwitchRow(
                        title = "Rooftop Helipad",
                        subtitle = "Hospital & civic H-pad deck marking",
                        checked = state.detailFlagB,
                        tag = "switch_civ_helipad",
                        onCheckedChange = { onStateChange(state.copy(detailFlagB = it)) }
                    )
                    LabeledFloatSlider(
                        label = "Portico Columns / Bus Bays",
                        valueText = "${state.detailCount}",
                        value = state.detailCount.toFloat(),
                        range = 2f..10f,
                        tag = "slider_civ_bays",
                        onValueChange = { onStateChange(state.copy(detailCount = it.roundToInt().coerceIn(2, 10))) }
                    )
                }
                BuildingStudioCategory.INTERIORS -> {
                    LabeledSwitchRow(
                        title = "Interior Doors & Thresholds",
                        subtitle = "Partition frames and swing panels",
                        checked = state.detailFlagA,
                        tag = "switch_int_doors",
                        onCheckedChange = { onStateChange(state.copy(detailFlagA = it)) }
                    )
                    LabeledSwitchRow(
                        title = "Furniture & Equipment Fitout",
                        subtitle = "Desks, seating, racking, or vehicle lifts",
                        checked = state.detailFlagB,
                        tag = "switch_int_furniture",
                        onCheckedChange = { onStateChange(state.copy(detailFlagB = it)) }
                    )
                }
                BuildingStudioCategory.PROPS -> {
                    LabeledFloatSlider(
                        label = "Attachment Scale",
                        valueText = "${String.format("%.2f", state.detailIntensity)}×",
                        value = state.detailIntensity,
                        range = 0.5f..3.0f,
                        tag = "slider_prop_scale",
                        onValueChange = { onStateChange(state.copy(detailIntensity = it)) }
                    )
                    LabeledSwitchRow(
                        title = "Cluster Multiple Units",
                        subtitle = "Array rooftop HVAC chillers or masts",
                        checked = state.detailFlagA,
                        tag = "switch_prop_cluster",
                        onCheckedChange = { onStateChange(state.copy(detailFlagA = it)) }
                    )
                    if (state.detailFlagA) {
                        LabeledFloatSlider(
                            label = "Cluster Count",
                            valueText = "${state.detailCount} units",
                            value = state.detailCount.toFloat(),
                            range = 1f..6f,
                            tag = "slider_prop_count",
                            onValueChange = { onStateChange(state.copy(detailCount = it.roundToInt().coerceIn(1, 6))) }
                        )
                    }
                }
                BuildingStudioCategory.SIGNAGE -> {
                    LabeledSwitchRow(
                        title = "Overhead Spotlights",
                        subtitle = "Floodlight trusses illuminating fascia",
                        checked = state.detailFlagA,
                        tag = "switch_sign_spotlights",
                        onCheckedChange = { onStateChange(state.copy(detailFlagA = it)) }
                    )
                    LabeledSwitchRow(
                        title = "Maintenance Catwalk",
                        subtitle = "Steel service walkway beneath sign",
                        checked = state.detailFlagB,
                        tag = "switch_sign_catwalk",
                        onCheckedChange = { onStateChange(state.copy(detailFlagB = it)) }
                    )
                }
                BuildingStudioCategory.DAMAGE -> {
                    LabeledFloatSlider(
                        label = "Damage Severity",
                        valueText = "${(state.detailIntensity * 100).roundToInt()}%",
                        value = state.detailIntensity,
                        range = 0.1f..1.0f,
                        tag = "slider_damage_severity",
                        onValueChange = { onStateChange(state.copy(detailIntensity = it)) }
                    )
                    LabeledSwitchRow(
                        title = "Expose Twisted Rebar",
                        subtitle = "Generate fractured steel reinforcement rods",
                        checked = state.detailFlagA,
                        tag = "switch_damage_rebar",
                        onCheckedChange = { onStateChange(state.copy(detailFlagA = it)) }
                    )
                    LabeledFloatSlider(
                        label = "Debris Rubble Chunks",
                        valueText = "${state.detailCount} chunks",
                        value = state.detailCount.toFloat(),
                        range = 6f..54f,
                        tag = "slider_damage_chunks",
                        onValueChange = { onStateChange(state.copy(detailCount = it.roundToInt().coerceIn(6, 54))) }
                    )
                }
                BuildingStudioCategory.LOD -> {
                    LabeledFloatSlider(
                        label = "Block Grid Density",
                        valueText = "${state.detailCount} × ${state.detailCount}",
                        value = state.detailCount.toFloat(),
                        range = 1f..6f,
                        tag = "slider_lod_density",
                        onValueChange = { onStateChange(state.copy(detailCount = it.roundToInt().coerceIn(1, 6))) }
                    )
                }
                BuildingStudioCategory.MASSING -> {}
            }
        }
    }
}

@Composable
internal fun SectionHeader(title: String, subtitle: String) {
    Column {
        Text(
            text = title,
            style = MaterialTheme.typography.titleSmall,
            fontWeight = FontWeight.Bold
        )
        Text(
            text = subtitle,
            style = MaterialTheme.typography.bodySmall,
            color = MaterialTheme.colorScheme.onSurfaceVariant
        )
    }
}

@Composable
internal fun LabeledFloatSlider(
    label: String,
    valueText: String,
    value: Float,
    range: ClosedFloatingPointRange<Float>,
    tag: String,
    onValueChange: (Float) -> Unit
) {
    Column(modifier = Modifier.fillMaxWidth()) {
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(label, style = MaterialTheme.typography.bodyMedium, fontWeight = FontWeight.Medium)
            Text(
                valueText,
                style = MaterialTheme.typography.labelMedium,
                color = MaterialTheme.colorScheme.primary,
                fontWeight = FontWeight.Bold
            )
        }
        Slider(
            value = value.coerceIn(range.start, range.endInclusive),
            onValueChange = onValueChange,
            valueRange = range,
            modifier = Modifier.fillMaxWidth().testTag(tag)
        )
    }
}

@Composable
internal fun LabeledSwitchRow(
    title: String,
    subtitle: String,
    checked: Boolean,
    tag: String,
    onCheckedChange: (Boolean) -> Unit
) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .defaultMinSize(minHeight = 48.dp),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        Column(modifier = Modifier.weight(1f).padding(end = 12.dp)) {
            Text(title, style = MaterialTheme.typography.bodyMedium, fontWeight = FontWeight.Medium)
            Text(subtitle, style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
        }
        Switch(
            checked = checked,
            onCheckedChange = onCheckedChange,
            modifier = Modifier.testTag(tag)
        )
    }
}
