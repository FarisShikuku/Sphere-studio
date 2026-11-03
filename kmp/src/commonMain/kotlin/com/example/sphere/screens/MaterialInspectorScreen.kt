package com.example.sphere.screens

import androidx.activity.compose.BackHandler
import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyRow
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.*
import com.example.sphere.tabs.MaterialChannelSelectorRow
import com.example.sphere.tabs.MaterialOpticsTab
import com.example.sphere.tabs.MaterialProceduralDetailTab
import com.example.sphere.tabs.MaterialPropertiesTab
import com.example.sphere.tabs.MaterialShaderTab
import com.example.sphere.tabs.MaterialVulkanSpecTab
import com.example.sphere.tabs.ObjectDimensionsControlTab
import com.example.sphere.tabs.SurfaceCompositeLayersTab
import com.example.sphere.viewport.MaterialPreviewViewport
import com.example.sphere.viewport.PreviewMeshShape
import kotlin.math.*

enum class MaterialCategoryFilter(val label: String, val iconDescription: String) {
    ALL("All Textures", "All procedural PBR materials"),
    WALL("🧱 Wall & Masonry", "Bricks, concrete, tiles, mortar"),
    METALS("✨ Metals & PBR", "Gold, brushed steel, rusted iron, copper"),
    FLOORING("🌲 Flooring & Stone", "Oak planks, marble, bitumen asphalt"),
    NATURE("🌿 Nature & Organic", "Lawn turf, moist earth loam, skin")
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MaterialInspectorScreen(
    onNavigateBack: () -> Unit,
    modifier: Modifier = Modifier
) {
    BackHandler { onNavigateBack() }

    // Selected Target Object: Default to 3D Test Cube as requested
    var previewShape by remember { mutableStateOf(PreviewMeshShape.CUBE) }

    // Dimensions & Size Modification State for Wall, Box, Floor, Sphere
    var objectScale by remember { mutableFloatStateOf(1.0f) }
    var objectWidthScale by remember { mutableFloatStateOf(1.0f) }
    var objectHeightScale by remember { mutableFloatStateOf(1.0f) }
    var objectDepthScale by remember { mutableFloatStateOf(1.0f) }
    var isQuickSizeBarExpanded by remember { mutableStateOf(false) }

    // Real-World Physical PBR Scaling & UV Tiling (Architextures / BIM standard)
    var useRealWorldScale by remember { mutableStateOf(true) }
    var uvTilingMultiplierX by remember { mutableFloatStateOf(1.0f) }
    var uvTilingMultiplierY by remember { mutableFloatStateOf(1.0f) }

    // Material & Surface State
    var selectedMaterialIndex by remember { mutableIntStateOf(0) }
    var currentMaterial by remember(selectedMaterialIndex) {
        mutableStateOf(defaultPbrMaterials[selectedMaterialIndex])
    }

    var selectedSurfaceIndex by remember { mutableIntStateOf(0) }
    var currentSurface by remember(selectedSurfaceIndex) {
        mutableStateOf(defaultCompositeSurfaces[selectedSurfaceIndex])
    }

    var selectedCategoryFilter by remember { mutableStateOf(MaterialCategoryFilter.ALL) }
    var selectedChannel by remember { mutableStateOf(MaterialChannelMode.COMPOSITE_PBR) }
    var selectedTab by remember { mutableIntStateOf(0) }

    // Filtered materials based on category
    val filteredMaterials = remember(selectedCategoryFilter) {
        when (selectedCategoryFilter) {
            MaterialCategoryFilter.ALL -> defaultPbrMaterials
            MaterialCategoryFilter.WALL -> defaultPbrMaterials.filter { it.category == "Masonry" }
            MaterialCategoryFilter.METALS -> defaultPbrMaterials.filter { it.category == "Metals" }
            MaterialCategoryFilter.FLOORING -> defaultPbrMaterials.filter { it.category == "Flooring" || it.category == "Infrastructure" }
            MaterialCategoryFilter.NATURE -> defaultPbrMaterials.filter { it.category == "Nature" || it.category == "Organics" || it.category == "Refractive" }
        }
    }

    val tabTitles = listOf("📐 Dimensions", "🎨 Texture Library", "⚙️ PBR Parameters", "🔬 Micro-Details", "🗺️ Channel Maps", "⚡ Vulkan C++")

    Scaffold(
        topBar = {
            TopAppBar(
                title = {
                    Column {
                        Row(verticalAlignment = Alignment.CenterVertically) {
                            Text(
                                "PBR Material & Surface Studio",
                                style = MaterialTheme.typography.titleMedium.copy(fontWeight = FontWeight.Bold)
                            )
                            Spacer(Modifier.width(8.dp))
                            Surface(
                                color = MaterialTheme.colorScheme.primaryContainer,
                                shape = RoundedCornerShape(4.dp)
                            ) {
                                Text(
                                    "14:30 UTC",
                                    modifier = Modifier.padding(horizontal = 5.dp, vertical = 2.dp),
                                    style = MaterialTheme.typography.labelSmall.copy(
                                        fontFamily = FontFamily.Monospace,
                                        fontSize = 10.sp,
                                        fontWeight = FontWeight.Bold,
                                        color = MaterialTheme.colorScheme.onPrimaryContainer
                                    )
                                )
                            }
                        }
                        Text(
                            "Target: ${previewShape.label} • Active: ${currentMaterial.name}",
                            style = MaterialTheme.typography.bodySmall.copy(
                                color = MaterialTheme.colorScheme.onSurfaceVariant,
                                fontSize = 11.sp
                            )
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
                            currentMaterial = defaultPbrMaterials[selectedMaterialIndex]
                            currentSurface = defaultCompositeSurfaces[selectedSurfaceIndex]
                            selectedChannel = MaterialChannelMode.COMPOSITE_PBR
                            objectScale = 1.0f
                            objectWidthScale = 1.0f
                            objectHeightScale = 1.0f
                            objectDepthScale = 1.0f
                            useRealWorldScale = true
                            uvTilingMultiplierX = 1.0f
                            uvTilingMultiplierY = 1.0f
                        },
                        modifier = Modifier.testTag("reset_material_preset_button")
                    ) {
                        Icon(Icons.Default.Refresh, contentDescription = "Reset Preset")
                    }
                },
                colors = TopAppBarDefaults.topAppBarColors(
                    containerColor = MaterialTheme.colorScheme.surface
                )
            )
        },
        modifier = modifier.testTag("material_inspector_screen")
    ) { innerPadding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(innerPadding)
        ) {
            // 1. Target Object Selector Bar (Cube, Wall, Sphere, Slab)
            Surface(
                color = MaterialTheme.colorScheme.surfaceContainerHigh,
                modifier = Modifier.fillMaxWidth()
            ) {
                Column(modifier = Modifier.padding(horizontal = 12.dp, vertical = 6.dp)) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Row(verticalAlignment = Alignment.CenterVertically) {
                            Icon(
                                Icons.Default.ViewInAr,
                                contentDescription = null,
                                tint = MaterialTheme.colorScheme.primary,
                                modifier = Modifier.size(16.dp)
                            )
                            Spacer(Modifier.width(6.dp))
                            Text(
                                "Target 3D Object:",
                                style = MaterialTheme.typography.labelMedium.copy(fontWeight = FontWeight.Bold)
                            )
                        }

                        Text(
                            "Tap object, then apply any texture below",
                            style = MaterialTheme.typography.labelSmall.copy(
                                color = MaterialTheme.colorScheme.onSurfaceVariant,
                                fontSize = 10.sp
                            )
                        )
                    }

                    Spacer(Modifier.height(4.dp))

                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        PreviewMeshShape.entries.forEach { shapeOption ->
                            val isSelected = previewShape == shapeOption
                            FilledTonalButton(
                                onClick = { previewShape = shapeOption },
                                modifier = Modifier.weight(1f).testTag("select_shape_${shapeOption.id}"),
                                colors = ButtonDefaults.filledTonalButtonColors(
                                    containerColor = if (isSelected) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.surfaceContainerHighest,
                                    contentColor = if (isSelected) MaterialTheme.colorScheme.onPrimary else MaterialTheme.colorScheme.onSurfaceVariant
                                ),
                                contentPadding = PaddingValues(horizontal = 4.dp, vertical = 6.dp)
                            ) {
                                Icon(
                                    imageVector = when (shapeOption) {
                                        PreviewMeshShape.CUBE -> Icons.Default.ViewInAr
                                        PreviewMeshShape.WALL -> Icons.Default.Layers
                                        PreviewMeshShape.SPHERE -> Icons.Default.Circle
                                        PreviewMeshShape.PLANE -> Icons.Default.SquareFoot
                                    },
                                    contentDescription = null,
                                    modifier = Modifier.size(14.dp)
                                )
                                Spacer(Modifier.width(4.dp))
                                Text(
                                    shapeOption.label,
                                    fontSize = 11.sp,
                                    fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal
                                )
                            }
                        }
                    }

                    // Quick Size & Dimensions Toolbar
                    Spacer(Modifier.height(6.dp))
                    Surface(
                        color = MaterialTheme.colorScheme.surfaceContainerHighest.copy(alpha = 0.65f),
                        shape = RoundedCornerShape(8.dp),
                        modifier = Modifier.fillMaxWidth().testTag("quick_size_toolbar")
                    ) {
                        Column(modifier = Modifier.padding(horizontal = 8.dp, vertical = 6.dp)) {
                            Row(
                                modifier = Modifier.fillMaxWidth(),
                                horizontalArrangement = Arrangement.SpaceBetween,
                                verticalAlignment = Alignment.CenterVertically
                            ) {
                                Row(
                                    verticalAlignment = Alignment.CenterVertically,
                                    modifier = Modifier.weight(1f)
                                ) {
                                    Icon(
                                        Icons.Default.Straighten,
                                        contentDescription = null,
                                        tint = Color(0xFFF59E0B),
                                        modifier = Modifier.size(14.dp)
                                    )
                                    Spacer(Modifier.width(5.dp))
                                    Text(
                                        text = when (previewShape) {
                                            PreviewMeshShape.WALL -> "Wall: ${(1.85f * objectScale * objectWidthScale * 10).roundToInt() / 10f}m(W) × ${(1.45f * objectScale * objectHeightScale * 10).roundToInt() / 10f}m(H) × ${(0.34f * objectScale * objectDepthScale * 1000).toInt()}mm(T)"
                                            PreviewMeshShape.PLANE -> "Floor: ${(2.40f * objectScale * objectWidthScale * 10).roundToInt() / 10f}m × ${(2.40f * objectScale * objectDepthScale * 10).roundToInt() / 10f}m (Slab: ${(0.20f * objectScale * objectHeightScale * 1000).toInt()}mm)"
                                            PreviewMeshShape.CUBE -> "Box: ${(1.92f * objectScale * objectWidthScale * 10).roundToInt() / 10f}m × ${(1.92f * objectScale * objectHeightScale * 10).roundToInt() / 10f}m × ${(1.92f * objectScale * objectDepthScale * 10).roundToInt() / 10f}m"
                                            PreviewMeshShape.SPHERE -> "Sphere: Ø ${(1.60f * objectScale * objectWidthScale * 10).roundToInt() / 10f}m"
                                        },
                                        style = MaterialTheme.typography.labelSmall.copy(
                                            fontFamily = FontFamily.Monospace,
                                            fontWeight = FontWeight.Bold,
                                            color = MaterialTheme.colorScheme.onSurface,
                                            fontSize = 10.sp
                                        ),
                                        maxLines = 1
                                    )
                                }

                                Row(verticalAlignment = Alignment.CenterVertically) {
                                    IconButton(
                                        onClick = { objectScale = (objectScale - 0.15f).coerceAtLeast(0.40f) },
                                        modifier = Modifier.size(26.dp).testTag("button_decrease_size")
                                    ) {
                                        Icon(Icons.Default.Remove, contentDescription = "Decrease Size", modifier = Modifier.size(13.dp))
                                    }

                                    Surface(
                                        color = MaterialTheme.colorScheme.primaryContainer,
                                        shape = RoundedCornerShape(4.dp),
                                        modifier = Modifier.clickable { selectedTab = 0 }
                                    ) {
                                        Text(
                                            "${(objectScale * 100).toInt()}%",
                                            modifier = Modifier.padding(horizontal = 6.dp, vertical = 2.dp),
                                            style = MaterialTheme.typography.labelSmall.copy(
                                                fontFamily = FontFamily.Monospace,
                                                fontWeight = FontWeight.Bold,
                                                color = MaterialTheme.colorScheme.onPrimaryContainer,
                                                fontSize = 10.sp
                                            )
                                        )
                                    }

                                    IconButton(
                                        onClick = { objectScale = (objectScale + 0.15f).coerceAtMost(2.50f) },
                                        modifier = Modifier.size(26.dp).testTag("button_increase_size")
                                    ) {
                                        Icon(Icons.Default.Add, contentDescription = "Increase Size", modifier = Modifier.size(13.dp))
                                    }

                                    IconButton(
                                        onClick = { isQuickSizeBarExpanded = !isQuickSizeBarExpanded },
                                        modifier = Modifier.size(26.dp).testTag("toggle_size_sliders")
                                    ) {
                                        Icon(
                                            if (isQuickSizeBarExpanded) Icons.Default.ExpandLess else Icons.Default.Tune,
                                            contentDescription = "Tune Dimensions",
                                            tint = MaterialTheme.colorScheme.primary,
                                            modifier = Modifier.size(14.dp)
                                        )
                                    }
                                }
                            }

                            if (isQuickSizeBarExpanded) {
                                Spacer(Modifier.height(4.dp))
                                Row(
                                    modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
                                    horizontalArrangement = Arrangement.spacedBy(6.dp),
                                    verticalAlignment = Alignment.CenterVertically
                                ) {
                                    FilterChip(
                                        selected = useRealWorldScale,
                                        onClick = { useRealWorldScale = !useRealWorldScale },
                                        label = {
                                            Text(
                                                if (useRealWorldScale) "PBR ${(currentMaterial.tileScaleMeters * 1000).toInt()}mm" else "Stretch Fit",
                                                fontSize = 10.sp,
                                                fontWeight = FontWeight.Bold
                                            )
                                        },
                                        leadingIcon = {
                                            Icon(
                                                if (useRealWorldScale) Icons.Default.GridOn else Icons.Default.FitScreen,
                                                contentDescription = null,
                                                modifier = Modifier.size(11.dp)
                                            )
                                        },
                                        modifier = Modifier.testTag("quick_toggle_real_world_scale")
                                    )
                                    AssistChip(
                                        onClick = {
                                            objectScale = 0.70f
                                            objectWidthScale = 1.0f
                                            objectHeightScale = 1.0f
                                            objectDepthScale = 1.0f
                                        },
                                        label = { Text("Compact 0.7x", fontSize = 10.sp) },
                                        modifier = Modifier.testTag("preset_compact")
                                    )
                                    AssistChip(
                                        onClick = {
                                            objectScale = 1.0f
                                            objectWidthScale = 1.0f
                                            objectHeightScale = 1.0f
                                            objectDepthScale = 1.0f
                                        },
                                        label = { Text("Default 1.0x", fontSize = 10.sp) },
                                        modifier = Modifier.testTag("preset_default")
                                    )
                                    AssistChip(
                                        onClick = {
                                            objectScale = 1.40f
                                            objectWidthScale = 1.0f
                                            objectHeightScale = 1.0f
                                            objectDepthScale = 1.0f
                                        },
                                        label = { Text("Expanded 1.4x", fontSize = 10.sp) },
                                        modifier = Modifier.testTag("preset_expanded")
                                    )
                                    AssistChip(
                                        onClick = {
                                            objectScale = 2.0f
                                            objectWidthScale = 1.0f
                                            objectHeightScale = 1.0f
                                            objectDepthScale = 1.0f
                                        },
                                        label = { Text("Massive 2.0x", fontSize = 10.sp) },
                                        modifier = Modifier.testTag("preset_massive")
                                    )
                                    Button(
                                        onClick = { selectedTab = 0 },
                                        colors = ButtonDefaults.buttonColors(containerColor = MaterialTheme.colorScheme.primary),
                                        contentPadding = PaddingValues(horizontal = 8.dp, vertical = 2.dp),
                                        modifier = Modifier.height(26.dp)
                                    ) {
                                        Icon(Icons.Default.OpenInFull, contentDescription = null, modifier = Modifier.size(11.dp))
                                        Spacer(Modifier.width(3.dp))
                                        Text("All Parametric Sliders", fontSize = 10.sp)
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // 2. Texture Category Filter & Quick Material Selection
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState())
                    .padding(horizontal = 12.dp, vertical = 3.dp),
                horizontalArrangement = Arrangement.spacedBy(6.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                MaterialCategoryFilter.entries.forEach { cat ->
                    val isSelected = selectedCategoryFilter == cat
                    FilterChip(
                        selected = isSelected,
                        onClick = { selectedCategoryFilter = cat },
                        label = { Text(cat.label, fontSize = 11.sp, fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal) },
                        modifier = Modifier.testTag("filter_cat_${cat.name.lowercase()}"),
                        colors = FilterChipDefaults.filterChipColors(
                            selectedContainerColor = MaterialTheme.colorScheme.secondaryContainer,
                            selectedLabelColor = MaterialTheme.colorScheme.onSecondaryContainer
                        )
                    )
                }
            }

            // 3. Compact Horizontal Texture Quick-Chips
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState())
                    .padding(horizontal = 12.dp, vertical = 2.dp),
                horizontalArrangement = Arrangement.spacedBy(6.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                filteredMaterials.forEach { mat ->
                    val isSelected = currentMaterial.id == mat.id
                    FilterChip(
                        selected = isSelected,
                        onClick = {
                            currentMaterial = mat
                            selectedMaterialIndex = defaultPbrMaterials.indexOfFirst { it.id == mat.id }.coerceAtLeast(0)

                            val matchingSurface = defaultCompositeSurfaces.find { surf ->
                                surf.id.contains(mat.id) || mat.id.contains(surf.id) || surf.category.name == mat.category.uppercase()
                            }
                            if (matchingSurface != null) {
                                currentSurface = matchingSurface
                                selectedSurfaceIndex = defaultCompositeSurfaces.indexOf(matchingSurface)
                            }
                        },
                        leadingIcon = {
                            Box(
                                modifier = Modifier
                                    .size(12.dp)
                                    .clip(CircleShape)
                                    .background(mat.baseColor)
                            )
                        },
                        label = {
                            Text(
                                mat.name,
                                fontSize = 11.sp,
                                fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal
                            )
                        },
                        modifier = Modifier.testTag("chip_mat_${mat.id}"),
                        colors = FilterChipDefaults.filterChipColors(
                            selectedContainerColor = MaterialTheme.colorScheme.primaryContainer,
                            selectedLabelColor = MaterialTheme.colorScheme.onPrimaryContainer
                        )
                    )
                }
            }

            // 4. Texture Channel Selector (Full PBR, Albedo, Normal, ORM, Height)
            MaterialChannelSelectorRow(
                selectedChannel = selectedChannel,
                onChannelSelect = { selectedChannel = it }
            )

            // 5. Expansive 3D Viewport with the chosen Object & Applied Texture
            MaterialPreviewViewport(
                material = currentMaterial,
                compositeSurface = currentSurface,
                channelMode = selectedChannel,
                shape = previewShape,
                objectScale = objectScale,
                objectWidthScale = objectWidthScale,
                objectHeightScale = objectHeightScale,
                objectDepthScale = objectDepthScale,
                useRealWorldScale = useRealWorldScale,
                uvTilingMultiplierX = uvTilingMultiplierX,
                uvTilingMultiplierY = uvTilingMultiplierY,
                onScaleChange = { objectScale = it },
                onShapeChange = { previewShape = it },
                modifier = Modifier
                    .fillMaxWidth()
                    .weight(1.30f)
            )

            // 6. Navigation Tabs for In-Depth Customization
            PrimaryScrollableTabRow(
                selectedTabIndex = selectedTab.coerceIn(0, tabTitles.size - 1),
                modifier = Modifier.fillMaxWidth(),
                edgePadding = 8.dp
            ) {
                tabTitles.forEachIndexed { index, title ->
                    Tab(
                        selected = selectedTab == index,
                        onClick = { selectedTab = index },
                        text = {
                            Text(
                                title,
                                fontSize = 11.sp,
                                fontWeight = if (selectedTab == index) FontWeight.Bold else FontWeight.Normal
                            )
                        },
                        modifier = Modifier.testTag("material_tab_$index")
                    )
                }
            }

            // 7. Active Tab Panel
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .weight(1.15f)
            ) {
                when (selectedTab) {
                    0 -> ObjectDimensionsControlTab(
                        shape = previewShape,
                        material = currentMaterial,
                        scale = objectScale,
                        widthScale = objectWidthScale,
                        heightScale = objectHeightScale,
                        depthScale = objectDepthScale,
                        useRealWorldScale = useRealWorldScale,
                        onToggleRealWorldScale = { useRealWorldScale = it },
                        uvTilingMultiplierX = uvTilingMultiplierX,
                        uvTilingMultiplierY = uvTilingMultiplierY,
                        onUvTilingChange = { ux, uy ->
                            uvTilingMultiplierX = ux
                            uvTilingMultiplierY = uy
                        },
                        onScaleChange = { objectScale = it },
                        onWidthScaleChange = { objectWidthScale = it },
                        onHeightScaleChange = { objectHeightScale = it },
                        onDepthScaleChange = { objectDepthScale = it },
                        onReset = {
                            objectScale = 1.0f
                            objectWidthScale = 1.0f
                            objectHeightScale = 1.0f
                            objectDepthScale = 1.0f
                            useRealWorldScale = true
                            uvTilingMultiplierX = 1.0f
                            uvTilingMultiplierY = 1.0f
                        }
                    )
                    1 -> MaterialLibraryBrowserTab(
                        materials = defaultPbrMaterials,
                        activeMaterial = currentMaterial,
                        targetShape = previewShape,
                        onSelectMaterial = { selectedMat ->
                            currentMaterial = selectedMat
                            selectedMaterialIndex = defaultPbrMaterials.indexOfFirst { it.id == selectedMat.id }.coerceAtLeast(0)
                        }
                    )
                    2 -> MaterialPropertiesTab(
                        material = currentMaterial,
                        onMaterialChange = { currentMaterial = it }
                    )
                    3 -> MaterialProceduralDetailTab(
                        material = currentMaterial,
                        onMaterialChange = { currentMaterial = it }
                    )
                    4 -> MaterialVulkanSpecTab(
                        material = currentMaterial
                    )
                    5 -> MaterialShaderTab(
                        material = currentMaterial
                    )
                }
            }
        }
    }
}

// -------------------------------------------------------------
// VISUAL MATERIAL LIBRARY BROWSER TAB
// -------------------------------------------------------------
@Composable
private fun MaterialLibraryBrowserTab(
    materials: List<PbrMaterial>,
    activeMaterial: PbrMaterial,
    targetShape: PreviewMeshShape,
    onSelectMaterial: (PbrMaterial) -> Unit,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(14.dp),
        verticalArrangement = Arrangement.spacedBy(10.dp)
    ) {
        // Quick Apply Notice
        Surface(
            color = MaterialTheme.colorScheme.primaryContainer.copy(alpha = 0.45f),
            shape = RoundedCornerShape(8.dp),
            border = BorderStroke(1.dp, MaterialTheme.colorScheme.primary.copy(alpha = 0.35f))
        ) {
            Row(
                modifier = Modifier.padding(10.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Icon(
                    Icons.Default.TouchApp,
                    contentDescription = null,
                    tint = MaterialTheme.colorScheme.primary,
                    modifier = Modifier.size(18.dp)
                )
                Spacer(Modifier.width(8.dp))
                Text(
                    "Tap any texture card below to apply it to the active ${targetShape.label}.",
                    style = MaterialTheme.typography.bodySmall.copy(
                        fontWeight = FontWeight.Medium,
                        color = MaterialTheme.colorScheme.onSurface
                    )
                )
            }
        }

        materials.forEach { mat ->
            val isSelected = mat.id == activeMaterial.id
            Card(
                modifier = Modifier
                    .fillMaxWidth()
                    .clickable { onSelectMaterial(mat) }
                    .testTag("mat_card_${mat.id}"),
                shape = RoundedCornerShape(10.dp),
                colors = CardDefaults.cardColors(
                    containerColor = if (isSelected) MaterialTheme.colorScheme.primaryContainer.copy(alpha = 0.35f) else MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.45f)
                ),
                border = BorderStroke(
                    1.dp,
                    if (isSelected) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.outlineVariant.copy(alpha = 0.5f)
                )
            ) {
                Row(
                    modifier = Modifier.padding(12.dp),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    // Color / Material Swatch Thumbnail
                    Box(
                        modifier = Modifier
                            .size(46.dp)
                            .clip(RoundedCornerShape(8.dp))
                            .background(mat.baseColor)
                            .padding(2.dp)
                    ) {
                        if (isSelected) {
                            Icon(
                                Icons.Default.CheckCircle,
                                contentDescription = "Active",
                                tint = Color.White,
                                modifier = Modifier
                                    .size(18.dp)
                                    .align(Alignment.Center)
                            )
                        }
                    }

                    Spacer(Modifier.width(12.dp))

                    Column(modifier = Modifier.weight(1f)) {
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.spacedBy(6.dp)
                        ) {
                            Text(
                                mat.name,
                                style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold)
                            )
                            Surface(
                                color = MaterialTheme.colorScheme.secondaryContainer,
                                shape = RoundedCornerShape(4.dp)
                            ) {
                                Text(
                                    mat.category,
                                    modifier = Modifier.padding(horizontal = 4.dp, vertical = 1.dp),
                                    style = MaterialTheme.typography.labelSmall.copy(fontSize = 9.sp)
                                )
                            }
                        }

                        Spacer(Modifier.height(2.dp))

                        Text(
                            mat.description,
                            style = MaterialTheme.typography.bodySmall.copy(
                                color = MaterialTheme.colorScheme.onSurfaceVariant,
                                fontSize = 11.sp
                            ),
                            maxLines = 2
                        )

                        Spacer(Modifier.height(4.dp))

                        // Physical PBR Telemetry Badges
                        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                            Text(
                                "Rough: ${(mat.roughness * 100).toInt()}%",
                                style = MaterialTheme.typography.labelSmall.copy(
                                    fontFamily = FontFamily.Monospace,
                                    color = Color(0xFF38BDF8),
                                    fontSize = 10.sp
                                )
                            )
                            Text(
                                "Metal: ${(mat.metallic * 100).toInt()}%",
                                style = MaterialTheme.typography.labelSmall.copy(
                                    fontFamily = FontFamily.Monospace,
                                    color = Color(0xFFFBBF24),
                                    fontSize = 10.sp
                                )
                            )
                            Text(
                                "IOR: ${mat.ior}",
                                style = MaterialTheme.typography.labelSmall.copy(
                                    fontFamily = FontFamily.Monospace,
                                    color = Color(0xFFA78BFA),
                                    fontSize = 10.sp
                                )
                            )
                        }
                    }

                    Spacer(Modifier.width(8.dp))

                    FilledTonalButton(
                        onClick = { onSelectMaterial(mat) },
                        colors = ButtonDefaults.filledTonalButtonColors(
                            containerColor = if (isSelected) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.surfaceVariant,
                            contentColor = if (isSelected) MaterialTheme.colorScheme.onPrimary else MaterialTheme.colorScheme.onSurfaceVariant
                        ),
                        contentPadding = PaddingValues(horizontal = 10.dp, vertical = 6.dp)
                    ) {
                        Text(if (isSelected) "Applied" else "Apply", fontSize = 11.sp)
                    }
                }
            }
        }
    }
}
