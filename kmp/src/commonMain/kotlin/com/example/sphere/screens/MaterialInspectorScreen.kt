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
import androidx.compose.ui.geometry.Offset
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
    ALL("All Textures", "All 74 Khronos KTX2 PBR materials"),
    WALL("🧱 Masonry", "Bricks, blocks, tiles, mortar"),
    METALS("✨ Metals", "Steel, zinc, aluminium, copper"),
    FLOORING("🌲 Flooring & Stone", "Oak planks, chevron parquet, pavers, flagstone, terrazzo"),
    NATURE("🌿 Nature & Turf", "Lawn grass, four-leaf clover, picket paver, soil loam"),
    INFRASTRUCTURE("🏗️ Roads & Roofing", "Thin asphalt aggregate, macadam, bitumen shingles, tiles"),
    OPTICAL("💎 Optical Glass", "Architectural glass, frosted glass, water, ice")
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

    // Real-World Physical PBR Scaling & UV Tiling (Architextures / BIM standard)
    var useRealWorldScale by remember { mutableStateOf(true) }
    var uvTilingMultiplierX by remember { mutableFloatStateOf(1.0f) }
    var uvTilingMultiplierY by remember { mutableFloatStateOf(1.0f) }

    // Material & Surface State
    var selectedMaterialIndex by remember { mutableIntStateOf(0) }
    var currentMaterial by remember(selectedMaterialIndex) {
        mutableStateOf(defaultPbrMaterials[selectedMaterialIndex])
    }

    var currentSurface by remember(currentMaterial) {
        mutableStateOf(getCompositeSurfaceForMaterial(currentMaterial))
    }

    var selectedCategoryFilter by remember { mutableStateOf(MaterialCategoryFilter.ALL) }
    var selectedChannel by remember { mutableStateOf(MaterialChannelMode.COMPOSITE_PBR) }
    var selectedTab by remember { mutableIntStateOf(0) }
    var showWireframe by remember { mutableStateOf(false) }

    // Filtered materials based on category
    val filteredMaterials = remember(selectedCategoryFilter) {
        when (selectedCategoryFilter) {
            MaterialCategoryFilter.ALL -> defaultPbrMaterials
            MaterialCategoryFilter.WALL -> defaultPbrMaterials.filter { it.category == "Masonry" }
            MaterialCategoryFilter.METALS -> defaultPbrMaterials.filter { it.category == "Metals" }
            MaterialCategoryFilter.FLOORING -> defaultPbrMaterials.filter { it.category == "Flooring" }
            MaterialCategoryFilter.NATURE -> defaultPbrMaterials.filter { it.category == "Nature" || it.category == "Organics" }
            MaterialCategoryFilter.INFRASTRUCTURE -> defaultPbrMaterials.filter { it.category == "Infrastructure" }
            MaterialCategoryFilter.OPTICAL -> defaultPbrMaterials.filter { it.category == "Refractive" }
        }
    }

    val tabTitles = listOf("📐 Dimensions", "🎨 Texture Library", "⚙️ PBR Parameters", "🔬 Micro-Details", "🗺️ Channel Maps", "⚡ Vulkan C++")

    Scaffold(
        topBar = {
            TopAppBar(
                title = {
                    Text(
                        "PBR Material Studio",
                        style = MaterialTheme.typography.titleMedium.copy(fontWeight = FontWeight.Bold)
                    )
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
                            currentSurface = getCompositeSurfaceForMaterial(currentMaterial)
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
            // 1. Sleek Compact Target 3D Mesh Selector
            Surface(
                color = MaterialTheme.colorScheme.surfaceContainerHigh,
                modifier = Modifier.fillMaxWidth()
            ) {
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(horizontal = 12.dp, vertical = 6.dp),
                    horizontalArrangement = Arrangement.spacedBy(6.dp)
                ) {
                    PreviewMeshShape.entries.forEach { shapeOption ->
                        val isSelected = previewShape == shapeOption
                        FilterChip(
                            selected = isSelected,
                            onClick = { previewShape = shapeOption },
                            modifier = Modifier
                                .weight(1f)
                                .testTag("select_shape_${shapeOption.id}"),
                            leadingIcon = {
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
                            },
                            label = {
                                Text(
                                    shapeOption.label,
                                    fontSize = 11.sp,
                                    fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal,
                                    maxLines = 1
                                )
                            },
                            colors = FilterChipDefaults.filterChipColors(
                                selectedContainerColor = MaterialTheme.colorScheme.primaryContainer,
                                selectedLabelColor = MaterialTheme.colorScheme.onPrimaryContainer,
                                selectedLeadingIconColor = MaterialTheme.colorScheme.primary
                            )
                        )
                    }
                }
            }

            // 2A. PINNED Category Filter Bar (Fixed above materials so it never scrolls away)
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState())
                    .padding(horizontal = 12.dp, vertical = 3.dp),
                horizontalArrangement = Arrangement.spacedBy(6.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                MaterialCategoryFilter.entries.forEach { cat ->
                    val count = when (cat) {
                        MaterialCategoryFilter.ALL -> defaultPbrMaterials.size
                        MaterialCategoryFilter.WALL -> defaultPbrMaterials.count { it.category == "Masonry" }
                        MaterialCategoryFilter.METALS -> defaultPbrMaterials.count { it.category == "Metals" }
                        MaterialCategoryFilter.FLOORING -> defaultPbrMaterials.count { it.category == "Flooring" }
                        MaterialCategoryFilter.NATURE -> defaultPbrMaterials.count { it.category == "Nature" || it.category == "Organics" }
                        MaterialCategoryFilter.INFRASTRUCTURE -> defaultPbrMaterials.count { it.category == "Infrastructure" }
                        MaterialCategoryFilter.OPTICAL -> defaultPbrMaterials.count { it.category == "Refractive" }
                    }
                    val isCatSelected = selectedCategoryFilter == cat
                    FilterChip(
                        selected = isCatSelected,
                        onClick = { selectedCategoryFilter = cat },
                        label = {
                            Text(
                                "${cat.label} ($count)",
                                fontSize = 11.sp,
                                fontWeight = if (isCatSelected) FontWeight.Bold else FontWeight.Normal
                            )
                        },
                        modifier = Modifier.testTag("filter_cat_${cat.name.lowercase()}"),
                        colors = FilterChipDefaults.filterChipColors(
                            selectedContainerColor = MaterialTheme.colorScheme.secondaryContainer,
                            selectedLabelColor = MaterialTheme.colorScheme.onSecondaryContainer
                        )
                    )
                }
            }

            // 2B. Scrollable Material Chips Row
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState())
                    .padding(horizontal = 12.dp, vertical = 2.dp),
                horizontalArrangement = Arrangement.spacedBy(6.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                filteredMaterials.forEach { mat ->
                    val isMatSelected = currentMaterial.id == mat.id
                    FilterChip(
                        selected = isMatSelected,
                        onClick = {
                            currentMaterial = mat
                            selectedMaterialIndex = defaultPbrMaterials.indexOfFirst { it.id == mat.id }.coerceAtLeast(0)
                            currentSurface = getCompositeSurfaceForMaterial(mat)
                        },
                        leadingIcon = {
                            Box(
                                modifier = Modifier
                                    .size(10.dp)
                                    .clip(CircleShape)
                                    .background(mat.baseColor)
                            )
                        },
                        label = {
                            Text(
                                mat.name,
                                fontSize = 11.sp,
                                fontWeight = if (isMatSelected) FontWeight.Bold else FontWeight.Normal
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

            // 3. Texture Channel Selector (Full PBR, Albedo, Normal, ORM, Height)
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
                showWireframe = showWireframe,
                onToggleWireframe = { showWireframe = it },
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
    var searchQuery by remember { mutableStateOf("") }

    val filteredList = remember(searchQuery, materials) {
        if (searchQuery.isBlank()) {
            materials
        } else {
            materials.filter {
                it.name.contains(searchQuery, ignoreCase = true) ||
                it.category.contains(searchQuery, ignoreCase = true) ||
                it.id.contains(searchQuery, ignoreCase = true)
            }
        }
    }

    // Curated showcase of textures converted to Khronos KTX 2.0 (specifically user-shared images)
    val convertedShowcaseIds = listOf(
        "pilotage_brick_herringbone",
        "mondo_grass_four_leaf",
        "mondo_grass_picket_and_square",
        "asphalt_black_stone_aggregate"
    )
    val showcaseMaterials = remember {
        defaultPbrMaterials.filter { it.id in convertedShowcaseIds }
    }

    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(12.dp),
        verticalArrangement = Arrangement.spacedBy(10.dp)
    ) {
        // Search & Filter Box
        OutlinedTextField(
            value = searchQuery,
            onValueChange = { searchQuery = it },
            modifier = Modifier
                .fillMaxWidth()
                .testTag("texture_search_input"),
            placeholder = { Text("Search 74 KTX2 PBR textures (e.g. herringbone, grass, paver)...", fontSize = 12.sp) },
            leadingIcon = { Icon(Icons.Default.Search, contentDescription = "Search", modifier = Modifier.size(18.dp)) },
            trailingIcon = {
                if (searchQuery.isNotEmpty()) {
                    IconButton(onClick = { searchQuery = "" }) {
                        Icon(Icons.Default.Clear, contentDescription = "Clear", modifier = Modifier.size(16.dp))
                    }
                }
            },
            singleLine = true,
            shape = RoundedCornerShape(10.dp)
        )

        // -----------------------------------------------------------------
        // USER ACTUAL SHARED IMAGES -> KTX 2.0 CONVERSION SHOWCASE
        // -----------------------------------------------------------------
        Card(
            modifier = Modifier
                .fillMaxWidth()
                .testTag("ktx2_conversion_showcase_card"),
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(
                containerColor = MaterialTheme.colorScheme.surfaceContainerHighest.copy(alpha = 0.70f)
            ),
            border = BorderStroke(1.5.dp, MaterialTheme.colorScheme.primary.copy(alpha = 0.50f))
        ) {
            Column(
                modifier = Modifier.padding(12.dp),
                verticalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    Icon(
                        Icons.Default.Verified,
                        contentDescription = null,
                        tint = MaterialTheme.colorScheme.primary,
                        modifier = Modifier.size(20.dp)
                    )
                    Column {
                        Text(
                            "Actual Images ➔ Khronos KTX 2.0 GPU Containers",
                            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold)
                        )
                        Text(
                            "Direct Vulkan 1.3 GPU pipeline • BC7/VK_FORMAT_R8G8B8A8_SRGB • 10 Mipmap levels",
                            style = MaterialTheme.typography.labelSmall.copy(
                                color = MaterialTheme.colorScheme.primary,
                                fontSize = 10.sp
                            )
                        )
                    }
                }

                showcaseMaterials.forEach { showMat ->
                    val isSelected = showMat.id == activeMaterial.id
                    Surface(
                        modifier = Modifier
                            .fillMaxWidth()
                            .clickable { onSelectMaterial(showMat) }
                            .testTag("showcase_item_${showMat.id}"),
                        shape = RoundedCornerShape(8.dp),
                        color = if (isSelected) MaterialTheme.colorScheme.primaryContainer.copy(alpha = 0.40f) else MaterialTheme.colorScheme.surface.copy(alpha = 0.60f),
                        border = BorderStroke(
                            1.dp,
                            if (isSelected) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.outlineVariant.copy(alpha = 0.4f)
                        )
                    ) {
                        Row(
                            modifier = Modifier.padding(8.dp),
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            MaterialSwatchThumbnail(mat = showMat, isSelected = isSelected)

                            Spacer(Modifier.width(10.dp))

                            Column(modifier = Modifier.weight(1f)) {
                                Text(
                                    showMat.name,
                                    style = MaterialTheme.typography.labelLarge.copy(fontWeight = FontWeight.Bold),
                                    maxLines = 1
                                )
                                Text(
                                    "Format: VK_FORMAT_R8G8B8A8_SRGB (43) • 10 Mips • Slot #${showMat.bindlessAlbedoSlot}",
                                    style = MaterialTheme.typography.labelSmall.copy(
                                        fontFamily = FontFamily.Monospace,
                                        fontSize = 9.sp,
                                        color = Color(0xFF38BDF8)
                                    )
                                )
                                Text(
                                    "Physical Scale: ${(showMat.tileScaleMeters * 1000).toInt()}mm • Architextures BIM",
                                    style = MaterialTheme.typography.labelSmall.copy(
                                        fontSize = 9.sp,
                                        color = MaterialTheme.colorScheme.onSurfaceVariant
                                    )
                                )
                            }

                            Spacer(Modifier.width(6.dp))

                            FilledTonalButton(
                                onClick = { onSelectMaterial(showMat) },
                                colors = ButtonDefaults.filledTonalButtonColors(
                                    containerColor = if (isSelected) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.primaryContainer,
                                    contentColor = if (isSelected) MaterialTheme.colorScheme.onPrimary else MaterialTheme.colorScheme.onPrimaryContainer
                                ),
                                contentPadding = PaddingValues(horizontal = 8.dp, vertical = 4.dp),
                                modifier = Modifier.height(32.dp)
                            ) {
                                Text(if (isSelected) "Applied" else "Apply 3D", fontSize = 10.sp)
                            }
                        }
                    }
                }
            }
        }

        // Section Title: All Textures
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                "All Library Textures (${filteredList.size})",
                style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold)
            )
            Text(
                "Tap to apply to ${targetShape.label}",
                style = MaterialTheme.typography.labelSmall.copy(
                    color = MaterialTheme.colorScheme.onSurfaceVariant,
                    fontSize = 10.sp
                )
            )
        }

        // Full Materials List
        filteredList.forEach { mat ->
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
                    modifier = Modifier.padding(10.dp),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    MaterialSwatchThumbnail(mat = mat, isSelected = isSelected)

                    Spacer(Modifier.width(10.dp))

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
                                "Scale: ${(mat.tileScaleMeters * 1000).toInt()}mm",
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

@Composable
private fun MaterialSwatchThumbnail(
    mat: PbrMaterial,
    isSelected: Boolean,
    modifier: Modifier = Modifier
) {
    Box(
        modifier = modifier
            .size(46.dp)
            .clip(RoundedCornerShape(8.dp))
            .background(mat.baseColor)
    ) {
        androidx.compose.foundation.Canvas(modifier = Modifier.fillMaxSize()) {
            val w = size.width
            val h = size.height
            when {
                mat.id.contains("herringbone") -> {
                    val lineCol = Color.White.copy(alpha = 0.65f)
                    drawLine(lineCol, Offset(0f, h * 0.5f), Offset(w * 0.5f, 0f), strokeWidth = 1.8f)
                    drawLine(lineCol, Offset(w * 0.5f, h), Offset(w, h * 0.5f), strokeWidth = 1.8f)
                    drawLine(lineCol, Offset(w * 0.5f, 0f), Offset(w, h * 0.5f), strokeWidth = 1.8f)
                    drawLine(lineCol, Offset(0f, h * 0.5f), Offset(w * 0.5f, h), strokeWidth = 1.8f)
                }
                mat.id.contains("four_leaf") || mat.id.contains("four-leaf") -> {
                    val gCol = Color(0xFF386622)
                    drawCircle(gCol, radius = 5.5f, center = Offset(w * 0.35f, h * 0.35f))
                    drawCircle(gCol, radius = 5.5f, center = Offset(w * 0.65f, h * 0.35f))
                    drawCircle(gCol, radius = 5.5f, center = Offset(w * 0.35f, h * 0.65f))
                    drawCircle(gCol, radius = 5.5f, center = Offset(w * 0.65f, h * 0.65f))
                }
                mat.id.contains("picket") -> {
                    val frameCol = Color.White.copy(alpha = 0.6f)
                    drawRect(frameCol, topLeft = Offset(4f, 4f), size = androidx.compose.ui.geometry.Size(w - 8f, h - 8f), style = androidx.compose.ui.graphics.drawscope.Stroke(width = 2f))
                    drawRect(Color(0xFF386622), topLeft = Offset(w * 0.3f, h * 0.3f), size = androidx.compose.ui.geometry.Size(w * 0.4f, h * 0.4f))
                }
                mat.id.contains("asphalt") -> {
                    val speck = Color(0xFF141516)
                    val speckLight = Color(0xFF888A8E)
                    drawCircle(speck, radius = 2f, center = Offset(w * 0.25f, h * 0.3f))
                    drawCircle(speckLight, radius = 2f, center = Offset(w * 0.7f, h * 0.25f))
                    drawCircle(speck, radius = 2.5f, center = Offset(w * 0.5f, h * 0.6f))
                    drawCircle(speckLight, radius = 1.8f, center = Offset(w * 0.3f, h * 0.8f))
                    drawCircle(speck, radius = 2.2f, center = Offset(w * 0.8f, h * 0.75f))
                }
                mat.id.contains("brick") -> {
                    val mortar = Color.White.copy(alpha = 0.45f)
                    drawLine(mortar, Offset(0f, h * 0.33f), Offset(w, h * 0.33f), strokeWidth = 1.5f)
                    drawLine(mortar, Offset(0f, h * 0.66f), Offset(w, h * 0.66f), strokeWidth = 1.5f)
                    drawLine(mortar, Offset(w * 0.5f, 0f), Offset(w * 0.5f, h * 0.33f), strokeWidth = 1.5f)
                    drawLine(mortar, Offset(w * 0.25f, h * 0.33f), Offset(w * 0.25f, h * 0.66f), strokeWidth = 1.5f)
                    drawLine(mortar, Offset(w * 0.75f, h * 0.33f), Offset(w * 0.75f, h * 0.66f), strokeWidth = 1.5f)
                }
                mat.id.contains("wood") || mat.id.contains("plank") || mat.id.contains("chevron") -> {
                    val grain = Color.Black.copy(alpha = 0.35f)
                    drawLine(grain, Offset(0f, h * 0.35f), Offset(w, h * 0.35f), strokeWidth = 1.5f)
                    drawLine(grain, Offset(0f, h * 0.70f), Offset(w, h * 0.70f), strokeWidth = 1.5f)
                }
                mat.id.contains("grass") || mat.id.contains("turf") -> {
                    val blade = Color(0xFF68A33A)
                    drawLine(blade, Offset(w * 0.3f, h * 0.8f), Offset(w * 0.35f, h * 0.3f), strokeWidth = 1.8f)
                    drawLine(blade, Offset(w * 0.6f, h * 0.85f), Offset(w * 0.55f, h * 0.25f), strokeWidth = 1.8f)
                }
                mat.category == "Metals" -> {
                    val shine = Color.White.copy(alpha = 0.5f)
                    drawLine(shine, Offset(0f, 0f), Offset(w, h), strokeWidth = 2.2f)
                }
            }
        }

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
}
