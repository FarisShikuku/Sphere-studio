package com.example.sphere.screens

import androidx.activity.compose.BackHandler
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.Layers
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material.icons.filled.Science
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.CompositeSurface
import com.example.sphere.model.MaterialChannelMode
import com.example.sphere.model.PbrMaterial
import com.example.sphere.model.defaultCompositeSurfaces
import com.example.sphere.model.defaultPbrMaterials
import com.example.sphere.tabs.MaterialChannelSelectorRow
import com.example.sphere.tabs.MaterialOpticsTab
import com.example.sphere.tabs.MaterialProceduralDetailTab
import com.example.sphere.tabs.MaterialPropertiesTab
import com.example.sphere.tabs.MaterialShaderTab
import com.example.sphere.tabs.MaterialVulkanSpecTab
import com.example.sphere.tabs.SurfaceCompositeLayersTab
import com.example.sphere.viewport.MaterialPreviewViewport
import com.example.sphere.viewport.PreviewMeshShape

enum class InspectorMode(val label: String, val subtitle: String) {
    SURFACES("Composite Surfaces", "Heterogeneous macro-structures (Brick Wall, Car Paint, Wood Floor)"),
    MATERIALS("Pure Materials", "Homogeneous atomic physical matter (Iron, Clay, Chlorophyll, Glass)")
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MaterialInspectorScreen(
    onNavigateBack: () -> Unit,
    modifier: Modifier = Modifier
) {
    BackHandler { onNavigateBack() }

    var inspectorMode by remember { mutableStateOf(InspectorMode.SURFACES) }

    // Surface State
    var selectedSurfaceIndex by remember { mutableIntStateOf(0) }
    var currentSurface by remember(selectedSurfaceIndex) {
        mutableStateOf(defaultCompositeSurfaces[selectedSurfaceIndex])
    }

    // Material State
    var selectedMaterialIndex by remember { mutableIntStateOf(0) }
    var currentMaterial by remember(selectedMaterialIndex) {
        mutableStateOf(defaultPbrMaterials[selectedMaterialIndex])
    }

    var previewShape by remember { mutableStateOf(PreviewMeshShape.PLANE) }
    var selectedChannel by remember { mutableStateOf(MaterialChannelMode.COMPOSITE_PBR) }
    var selectedTab by remember { mutableIntStateOf(0) }

    val surfaceTabTitles = listOf("Composite Layers", "PBR Channels", "Vulkan Specs")
    val materialTabTitles = listOf("Physical Constants", "Procedural Details", "Refraction & SSS", "Vulkan Shader")

    Scaffold(
        topBar = {
            TopAppBar(
                title = {
                    Column {
                        Text(
                            if (inspectorMode == InspectorMode.SURFACES) "Surface & Assembly Inspector" else "Pure Material Physics",
                            style = MaterialTheme.typography.titleMedium.copy(fontWeight = FontWeight.Bold)
                        )
                        Text(
                            if (inspectorMode == InspectorMode.SURFACES) "${currentSurface.name} • ${currentSurface.category.label}" else "${currentMaterial.name} • ${currentMaterial.category}",
                            style = MaterialTheme.typography.bodySmall.copy(color = MaterialTheme.colorScheme.onSurfaceVariant, fontSize = 11.sp)
                        )
                    }
                },
                navigationIcon = {
                    IconButton(
                        onClick = onNavigateBack,
                        modifier = Modifier.testTag("material_inspector_back_button")
                    ) {
                        Icon(Icons.AutoMirrored.Filled.ArrowBack, contentDescription = "Back to Studio")
                    }
                },
                actions = {
                    IconButton(
                        onClick = {
                            if (inspectorMode == InspectorMode.SURFACES) {
                                currentSurface = defaultCompositeSurfaces[selectedSurfaceIndex]
                            } else {
                                currentMaterial = defaultPbrMaterials[selectedMaterialIndex]
                            }
                            selectedChannel = MaterialChannelMode.COMPOSITE_PBR
                        },
                        modifier = Modifier.testTag("reset_preset_button")
                    ) {
                        Icon(Icons.Default.Refresh, contentDescription = "Reset Preset")
                    }
                }
            )
        },
        modifier = modifier.testTag("material_inspector_screen")
    ) { innerPadding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(innerPadding)
        ) {
            // 1. High-Level Mode Switcher: Composite Surface vs Pure Material
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(horizontal = 12.dp, vertical = 6.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                InspectorMode.entries.forEach { mode ->
                    val isSelected = mode == inspectorMode
                    FilledTonalButton(
                        onClick = {
                            inspectorMode = mode
                            selectedTab = 0
                        },
                        modifier = Modifier.weight(1f).testTag("mode_${mode.name.lowercase()}"),
                        colors = ButtonDefaults.filledTonalButtonColors(
                            containerColor = if (isSelected) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.surfaceVariant,
                            contentColor = if (isSelected) MaterialTheme.colorScheme.onPrimary else MaterialTheme.colorScheme.onSurfaceVariant
                        )
                    ) {
                        Icon(
                            if (mode == InspectorMode.SURFACES) Icons.Default.Layers else Icons.Default.Science,
                            contentDescription = null,
                            modifier = Modifier.size(16.dp)
                        )
                        Spacer(Modifier.width(6.dp))
                        Text(mode.label, fontSize = 11.sp, fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal)
                    }
                }
            }

            // 2. Horizontal Item Selector Chips
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState())
                    .padding(horizontal = 12.dp, vertical = 4.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                if (inspectorMode == InspectorMode.SURFACES) {
                    defaultCompositeSurfaces.forEachIndexed { index, surf ->
                        val isSelected = selectedSurfaceIndex == index
                        FilterChip(
                            selected = isSelected,
                            onClick = {
                                selectedSurfaceIndex = index
                                currentSurface = surf
                                previewShape = PreviewMeshShape.PLANE
                            },
                            label = { Text(surf.name, fontSize = 12.sp) },
                            modifier = Modifier.testTag("chip_surface_${surf.id}"),
                            colors = FilterChipDefaults.filterChipColors(
                                selectedContainerColor = MaterialTheme.colorScheme.primaryContainer,
                                selectedLabelColor = MaterialTheme.colorScheme.onPrimaryContainer
                            )
                        )
                    }
                } else {
                    defaultPbrMaterials.forEachIndexed { index, mat ->
                        val isSelected = selectedMaterialIndex == index
                        FilterChip(
                            selected = isSelected,
                            onClick = {
                                selectedMaterialIndex = index
                                currentMaterial = mat
                            },
                            label = { Text(mat.name, fontSize = 12.sp) },
                            modifier = Modifier.testTag("chip_material_${mat.id}"),
                            colors = FilterChipDefaults.filterChipColors(
                                selectedContainerColor = MaterialTheme.colorScheme.primaryContainer,
                                selectedLabelColor = MaterialTheme.colorScheme.onPrimaryContainer
                            )
                        )
                    }
                }
            }

            // 3. Texture Channel Bar (when in PBR Channels or Material mode)
            MaterialChannelSelectorRow(
                selectedChannel = selectedChannel,
                onChannelSelect = { selectedChannel = it }
            )

            // 4. Viewport
            val activeMat = if (inspectorMode == InspectorMode.SURFACES) {
                currentSurface.layers.firstOrNull { it.isEnabled }?.material ?: defaultPbrMaterials.first()
            } else {
                currentMaterial
            }

            MaterialPreviewViewport(
                material = activeMat,
                compositeSurface = if (inspectorMode == InspectorMode.SURFACES) currentSurface else null,
                channelMode = selectedChannel,
                shape = previewShape,
                onShapeChange = { previewShape = it },
                modifier = Modifier
                    .fillMaxWidth()
                    .weight(1.02f)
            )

            // 5. Tabs
            val activeTabTitles = if (inspectorMode == InspectorMode.SURFACES) surfaceTabTitles else materialTabTitles
            PrimaryScrollableTabRow(
                selectedTabIndex = selectedTab.coerceIn(0, activeTabTitles.size - 1),
                modifier = Modifier.fillMaxWidth(),
                edgePadding = 8.dp
            ) {
                activeTabTitles.forEachIndexed { index, title ->
                    Tab(
                        selected = selectedTab == index,
                        onClick = { selectedTab = index },
                        text = { Text(title, fontSize = 12.sp, fontWeight = if (selectedTab == index) FontWeight.Bold else FontWeight.Normal) },
                        modifier = Modifier.testTag("inspector_tab_$index")
                    )
                }
            }

            // 6. Active Tab Panel
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .weight(1.18f)
            ) {
                if (inspectorMode == InspectorMode.SURFACES) {
                    when (selectedTab) {
                        0 -> SurfaceCompositeLayersTab(
                            surface = currentSurface,
                            onSurfaceChange = { currentSurface = it },
                            onDrillDownMaterial = { drilledMat ->
                                val foundIdx = defaultPbrMaterials.indexOfFirst { it.name == drilledMat.name }
                                if (foundIdx >= 0) {
                                    selectedMaterialIndex = foundIdx
                                    currentMaterial = defaultPbrMaterials[foundIdx]
                                } else {
                                    currentMaterial = drilledMat
                                }
                                inspectorMode = InspectorMode.MATERIALS
                                selectedTab = 0
                            }
                        )
                        1 -> MaterialProceduralDetailTab(
                            material = activeMat,
                            onMaterialChange = { updated ->
                                val updatedLayers = currentSurface.layers.map { layer ->
                                    if (layer.material.id == updated.id) layer.copy(material = updated) else layer
                                }
                                currentSurface = currentSurface.copy(layers = updatedLayers)
                            }
                        )
                        2 -> MaterialVulkanSpecTab(
                            material = activeMat
                        )
                    }
                } else {
                    when (selectedTab) {
                        0 -> MaterialPropertiesTab(
                            material = currentMaterial,
                            onMaterialChange = { currentMaterial = it }
                        )
                        1 -> MaterialProceduralDetailTab(
                            material = currentMaterial,
                            onMaterialChange = { currentMaterial = it }
                        )
                        2 -> MaterialOpticsTab(
                            material = currentMaterial,
                            onMaterialChange = { currentMaterial = it }
                        )
                        3 -> MaterialShaderTab(
                            material = currentMaterial
                        )
                    }
                }
            }
        }
    }
}
