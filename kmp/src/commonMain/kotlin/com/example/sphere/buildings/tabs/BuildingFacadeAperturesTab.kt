package com.example.sphere.buildings.tabs

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.outlined.Check
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import com.example.sphere.model.BuildingBalconyType
import com.example.sphere.model.BuildingDoorType
import com.example.sphere.model.BuildingStudioState
import com.example.sphere.model.BuildingWallMaterial
import com.example.sphere.model.BuildingWindowType
import com.example.sphere.model.buildingWallTintPresets
import kotlin.math.roundToInt

@Composable
fun BuildingFacadeAperturesTab(
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
        SectionHeader("Facade Cladding Material (16 PBR Slots)", "Wall & Curtain Wall Envelope")
        Row(
            modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            BuildingWallMaterial.entries.forEach { wall ->
                val isSelected = state.wallMaterial == wall
                FilterChip(
                    selected = isSelected,
                    onClick = { onStateChange(state.copy(wallMaterial = wall)) },
                    leadingIcon = {
                        Box(
                            modifier = Modifier
                                .size(12.dp)
                                .clip(CircleShape)
                                .background(wall.swatchColor)
                        )
                    },
                    label = { Text(wall.displayName) },
                    modifier = Modifier.testTag("building_wall_chip_${wall.ordinalId}")
                )
            }
        }

        // Residential / Stucco Wall Tint Palette
        SectionHeader("Stucco & Masonry Tint", "Applies to residential & stucco surfaces")
        Row(
            modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(10.dp)
        ) {
            buildingWallTintPresets.forEachIndexed { idx, swatch ->
                val selected = state.wallColor == swatch.color
                Surface(
                    onClick = { onStateChange(state.copy(wallColor = swatch.color)) },
                    shape = RoundedCornerShape(12.dp),
                    color = MaterialTheme.colorScheme.surfaceContainerLow,
                    border = BorderStroke(
                        width = if (selected) 2.dp else 1.dp,
                        color = if (selected) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.outlineVariant
                    ),
                    modifier = Modifier
                        .defaultMinSize(minHeight = 48.dp)
                        .testTag("building_wall_tint_$idx")
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 10.dp, vertical = 8.dp),
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.spacedBy(6.dp)
                    ) {
                        Box(
                            modifier = Modifier
                                .size(18.dp)
                                .clip(CircleShape)
                                .background(swatch.color)
                        )
                        Text(swatch.name, style = MaterialTheme.typography.labelMedium)
                        if (selected) {
                            Icon(
                                Icons.Outlined.Check,
                                contentDescription = "Selected tint",
                                modifier = Modifier.size(15.dp),
                                tint = MaterialTheme.colorScheme.primary
                            )
                        }
                    }
                }
            }
        }

        // Windows (All 23 WindowTypes)
        SectionHeader("Window Style (23 Architectural Profiles)", "Frames, mullions, arches & sills")
        Row(
            modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            BuildingWindowType.entries.forEach { win ->
                FilterChip(
                    selected = state.windowType == win,
                    onClick = { onStateChange(state.copy(windowType = win)) },
                    label = { Text(win.displayName) },
                    modifier = Modifier.testTag("building_window_chip_${win.ordinalId}")
                )
            }
        }

        ElevatedCard(shape = RoundedCornerShape(14.dp)) {
            Column(
                modifier = Modifier.padding(14.dp),
                verticalArrangement = Arrangement.spacedBy(10.dp)
            ) {
                LabeledFloatSlider(
                    label = "Windows per Facade",
                    valueText = "${state.windowsPerFacade} bays",
                    value = state.windowsPerFacade.toFloat(),
                    range = 0f..14f,
                    tag = "slider_building_win_count",
                    onValueChange = { onStateChange(state.copy(windowsPerFacade = it.roundToInt().coerceIn(0, 14))) }
                )
                LabeledFloatSlider(
                    label = "Window Width",
                    valueText = "${String.format("%.2f", state.windowWidth)} m",
                    value = state.windowWidth,
                    range = 0.5f..4.2f,
                    tag = "slider_building_win_width",
                    onValueChange = { onStateChange(state.copy(windowWidth = it)) }
                )
                LabeledFloatSlider(
                    label = "Window Height",
                    valueText = "${String.format("%.2f", state.windowHeight)} m",
                    value = state.windowHeight,
                    range = 0.6f..3.6f,
                    tag = "slider_building_win_height",
                    onValueChange = { onStateChange(state.copy(windowHeight = it)) }
                )
                LabeledFloatSlider(
                    label = "Sill Height",
                    valueText = "${String.format("%.2f", state.windowSillHeight)} m",
                    value = state.windowSillHeight,
                    range = 0.0f..1.8f,
                    tag = "slider_building_win_sill",
                    onValueChange = { onStateChange(state.copy(windowSillHeight = it)) }
                )
                LabeledSwitchRow(
                    title = "Window Shutters",
                    subtitle = "Timber flanking shutters on openings",
                    checked = state.hasWindowShutters,
                    tag = "switch_building_shutters",
                    onCheckedChange = { onStateChange(state.copy(hasWindowShutters = it)) }
                )
                LabeledSwitchRow(
                    title = "Solar Brise-Soleil Louvres",
                    subtitle = "External horizontal facade shading fins",
                    checked = state.hasSunLouvres,
                    tag = "switch_building_louvres",
                    onCheckedChange = { onStateChange(state.copy(hasSunLouvres = it)) }
                )
            }
        }

        // Doors & Balconies
        SectionHeader("Entrance Doors & Balconies", "Portals, canopies, ledges & railings")
        Row(
            modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            BuildingDoorType.entries.forEach { door ->
                FilterChip(
                    selected = state.doorType == door,
                    onClick = { onStateChange(state.copy(doorType = door)) },
                    label = { Text(door.displayName) },
                    modifier = Modifier.testTag("building_door_chip_${door.ordinalId}")
                )
            }
        }

        Row(
            modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            BuildingBalconyType.entries.forEach { balc ->
                FilterChip(
                    selected = state.balconyType == balc,
                    onClick = {
                        onStateChange(
                            state.copy(
                                balconyType = balc,
                                hasBalconies = balc != BuildingBalconyType.NONE
                            )
                        )
                    },
                    label = { Text("Balcony: ${balc.displayName}") },
                    modifier = Modifier.testTag("building_balcony_chip_${balc.ordinalId}")
                )
            }
        }

        ElevatedCard(shape = RoundedCornerShape(14.dp)) {
            Column(
                modifier = Modifier.padding(14.dp),
                verticalArrangement = Arrangement.spacedBy(10.dp)
            ) {
                if (state.balconyType != BuildingBalconyType.NONE || state.hasBalconies) {
                    LabeledFloatSlider(
                        label = "Balcony Cantilever Depth",
                        valueText = "${String.format("%.2f", state.balconyDepth)} m",
                        value = state.balconyDepth,
                        range = 0.6f..3.2f,
                        tag = "slider_building_balcony_depth",
                        onValueChange = { onStateChange(state.copy(balconyDepth = it)) }
                    )
                }
                LabeledSwitchRow(
                    title = "Entrance Canopy",
                    subtitle = "Projecting rain & sun shelter over main door",
                    checked = state.hasEntranceCanopy,
                    tag = "switch_building_canopy",
                    onCheckedChange = { onStateChange(state.copy(hasEntranceCanopy = it)) }
                )
                LabeledSwitchRow(
                    title = "Classical Cornice Belt",
                    subtitle = "Top-edge perimeter stone moulding",
                    checked = state.hasCornice,
                    tag = "switch_building_cornice",
                    onCheckedChange = { onStateChange(state.copy(hasCornice = it)) }
                )
            }
        }
    }
}
