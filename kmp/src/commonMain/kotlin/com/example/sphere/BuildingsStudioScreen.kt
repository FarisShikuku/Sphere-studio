package com.example.sphere

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.outlined.ArrowBack
import androidx.compose.material.icons.filled.ArrowDropDown
import androidx.compose.material.icons.outlined.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.draw.rotate
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import com.example.sphere.buildings.BuildingMeshStats
import com.example.sphere.buildings.BuildingsViewportCanvas
import com.example.sphere.buildings.tabs.BuildingFacadeAperturesTab
import com.example.sphere.buildings.tabs.BuildingMassingTab
import com.example.sphere.buildings.tabs.BuildingRoofInteriorTab
import com.example.sphere.buildings.tabs.BuildingShadersDiagnosticsTab
import com.example.sphere.model.BuildingStudioCategory
import com.example.sphere.model.BuildingStudioState
import com.example.sphere.model.applyPreset
import com.example.sphere.model.createBuildingsCatalog
import kotlinx.coroutines.isActive

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun BuildingsStudioScreen(
    onNavigateBack: () -> Unit,
    modifier: Modifier = Modifier
) {
    val catalog = remember { createBuildingsCatalog() }
    var studioState by remember {
        mutableStateOf(
            BuildingStudioState().applyPreset(catalog.first())
        )
    }

    val filteredPresets by remember(catalog, studioState.activeCategory) {
        derivedStateOf { catalog.filter { it.category == studioState.activeCategory } }
    }
    val currentPreset = catalog.firstOrNull { it.id == studioState.activePresetId }
        ?: filteredPresets.firstOrNull()
        ?: catalog.first()

    var rotX by remember { mutableFloatStateOf(20f) }
    var rotY by remember { mutableFloatStateOf(-35f) }
    var zoomFactor by remember { mutableFloatStateOf(1.0f) }
    var isAutoRotating by remember { mutableStateOf(false) }
    var isVariantSheetOpen by remember { mutableStateOf(false) }
    var selectedTabIndex by remember { mutableIntStateOf(0) }
    var latestStats by remember { mutableStateOf(BuildingMeshStats()) }

    val tabTitles = listOf(
        "Massing" to Icons.Outlined.Apartment,
        "Facade & Doors" to Icons.Outlined.Window,
        "Roof & Optics" to Icons.Outlined.Roofing,
        "Shaders & C++" to Icons.Outlined.Terminal
    )

    LaunchedEffect(isAutoRotating) {
        if (!isAutoRotating) return@LaunchedEffect
        var prevFrameNs = 0L
        while (isActive) {
            withFrameNanos { frameNs ->
                if (prevFrameNs != 0L) {
                    val dtSec = ((frameNs - prevFrameNs) / 1_000_000_000f).coerceIn(0f, 0.05f)
                    rotY = (rotY + dtSec * 36f) % 360f
                }
                prevFrameNs = frameNs
            }
        }
    }

    Scaffold(
        topBar = {
            TopAppBar(
                title = {
                    Column {
                        Text(
                            text = "Buildings & Architecture",
                            style = MaterialTheme.typography.titleMedium,
                            fontWeight = FontWeight.Bold
                        )
                        Text(
                            text = "12 Procedural C++ Generators • 180+ Architectural Forms (gen/buildings)",
                            style = MaterialTheme.typography.bodySmall,
                            color = MaterialTheme.colorScheme.onSurfaceVariant,
                            maxLines = 1,
                            overflow = TextOverflow.Ellipsis
                        )
                    }
                },
                navigationIcon = {
                    IconButton(
                        onClick = onNavigateBack,
                        modifier = Modifier.testTag("buildings_back_button")
                    ) {
                        Icon(Icons.AutoMirrored.Outlined.ArrowBack, contentDescription = "Navigate back")
                    }
                },
                actions = {
                    IconButton(
                        onClick = {
                            rotX = 20f
                            rotY = -35f
                            zoomFactor = 1.0f
                            studioState = BuildingStudioState().applyPreset(currentPreset)
                        },
                        modifier = Modifier.testTag("buildings_reset_button")
                    ) {
                        Icon(Icons.Outlined.RestartAlt, contentDescription = "Reset Building Parameters")
                    }
                },
                colors = TopAppBarDefaults.topAppBarColors(
                    containerColor = MaterialTheme.colorScheme.surface
                )
            )
        },
        modifier = modifier.fillMaxSize()
    ) { innerPadding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(innerPadding)
                .verticalScroll(rememberScrollState()),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .widthIn(max = 720.dp)
            ) {
                // 10 Building Domain Filter Chips
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .horizontalScroll(rememberScrollState())
                        .padding(horizontal = 16.dp, vertical = 8.dp),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    BuildingStudioCategory.entries.forEach { category ->
                        val isSelected = studioState.activeCategory == category
                        FilterChip(
                            selected = isSelected,
                            onClick = {
                                val firstInCat = catalog.firstOrNull { it.category == category }
                                if (firstInCat != null) {
                                    studioState = studioState.applyPreset(firstInCat)
                                }
                            },
                            label = { Text(category.displayName, style = MaterialTheme.typography.bodySmall) },
                            modifier = Modifier.testTag("building_category_chip_${category.name}")
                        )
                    }
                }

                // Variant Selector Card (triggers ModalBottomSheet)
                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(horizontal = 16.dp, vertical = 4.dp)
                ) {
                    Surface(
                        onClick = { isVariantSheetOpen = true },
                        shape = RoundedCornerShape(12.dp),
                        color = MaterialTheme.colorScheme.surfaceContainerLow,
                        border = BorderStroke(
                            width = if (isVariantSheetOpen) 2.dp else 1.dp,
                            color = if (isVariantSheetOpen) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.outline
                        ),
                        modifier = Modifier
                            .fillMaxWidth()
                            .defaultMinSize(minHeight = 56.dp)
                            .testTag("dropdown_building_selector")
                    ) {
                        Row(
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(horizontal = 14.dp, vertical = 10.dp),
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Icon(
                                imageVector = Icons.Outlined.Apartment,
                                contentDescription = null,
                                tint = MaterialTheme.colorScheme.primary
                            )
                            Spacer(modifier = Modifier.width(12.dp))
                            Column(modifier = Modifier.weight(1f)) {
                                Text(
                                    text = "Select ${studioState.activeCategory.displayName} Variant (${filteredPresets.size} in domain)",
                                    style = MaterialTheme.typography.labelSmall,
                                    color = MaterialTheme.colorScheme.onSurfaceVariant
                                )
                                Text(
                                    text = currentPreset.name,
                                    style = MaterialTheme.typography.bodyLarge,
                                    fontWeight = FontWeight.SemiBold,
                                    maxLines = 1,
                                    overflow = TextOverflow.Ellipsis
                                )
                            }
                            Icon(
                                imageVector = Icons.Filled.ArrowDropDown,
                                contentDescription = "Expand building variant list",
                                tint = MaterialTheme.colorScheme.onSurfaceVariant,
                                modifier = Modifier.rotate(if (isVariantSheetOpen) 180f else 0f)
                            )
                        }
                    }
                }

                if (isVariantSheetOpen) {
                    val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true)
                    ModalBottomSheet(
                        onDismissRequest = { isVariantSheetOpen = false },
                        sheetState = sheetState,
                        containerColor = MaterialTheme.colorScheme.surfaceContainerLow
                    ) {
                        Column(
                            modifier = Modifier
                                .fillMaxWidth()
                                .navigationBarsPadding()
                                .padding(horizontal = 16.dp)
                        ) {
                            Row(
                                modifier = Modifier
                                    .fillMaxWidth()
                                    .padding(bottom = 12.dp),
                                horizontalArrangement = Arrangement.SpaceBetween,
                                verticalAlignment = Alignment.CenterVertically
                            ) {
                                Column(modifier = Modifier.weight(1f)) {
                                    Text(
                                        text = studioState.activeCategory.displayName,
                                        style = MaterialTheme.typography.titleMedium,
                                        fontWeight = FontWeight.Bold
                                    )
                                    Text(
                                        text = "${studioState.activeCategory.subtitle} • Select C++ generator variant",
                                        style = MaterialTheme.typography.bodySmall,
                                        color = MaterialTheme.colorScheme.onSurfaceVariant
                                    )
                                }
                                IconButton(
                                    onClick = { isVariantSheetOpen = false },
                                    modifier = Modifier.testTag("building_sheet_close_button")
                                ) {
                                    Icon(Icons.Outlined.Close, contentDescription = "Close building variant list")
                                }
                            }

                            LazyColumn(
                                modifier = Modifier
                                    .fillMaxWidth()
                                    .heightIn(max = 420.dp),
                                verticalArrangement = Arrangement.spacedBy(8.dp),
                                contentPadding = PaddingValues(bottom = 24.dp)
                            ) {
                                items(filteredPresets, key = { it.id }) { item ->
                                    val isCurrent = item.id == currentPreset.id
                                    Surface(
                                        onClick = {
                                            studioState = studioState.applyPreset(item)
                                            isVariantSheetOpen = false
                                        },
                                        shape = RoundedCornerShape(12.dp),
                                        color = if (isCurrent) {
                                            MaterialTheme.colorScheme.primaryContainer.copy(alpha = 0.55f)
                                        } else {
                                            MaterialTheme.colorScheme.surfaceContainer
                                        },
                                        border = BorderStroke(
                                            width = if (isCurrent) 1.5.dp else 1.dp,
                                            color = if (isCurrent) {
                                                MaterialTheme.colorScheme.primary
                                            } else {
                                                MaterialTheme.colorScheme.outlineVariant
                                            }
                                        ),
                                        modifier = Modifier
                                            .fillMaxWidth()
                                            .defaultMinSize(minHeight = 56.dp)
                                            .testTag("building_menu_item_${item.id}")
                                    ) {
                                        Row(
                                            modifier = Modifier
                                                .fillMaxWidth()
                                                .padding(horizontal = 14.dp, vertical = 12.dp),
                                            verticalAlignment = Alignment.CenterVertically
                                        ) {
                                            Column(modifier = Modifier.weight(1f)) {
                                                Text(
                                                    text = item.name,
                                                    style = MaterialTheme.typography.bodyLarge,
                                                    fontWeight = if (isCurrent) FontWeight.Bold else FontWeight.SemiBold,
                                                    color = if (isCurrent) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.onSurface
                                                )
                                                Spacer(modifier = Modifier.height(2.dp))
                                                Text(
                                                    text = "${item.subtitle} • ${item.defaultFloors}F (${item.defaultWidth.toInt()}×${item.defaultDepth.toInt()}m)",
                                                    style = MaterialTheme.typography.bodySmall,
                                                    color = MaterialTheme.colorScheme.onSurfaceVariant
                                                )
                                            }
                                            if (isCurrent) {
                                                Spacer(modifier = Modifier.width(8.dp))
                                                Icon(
                                                    imageVector = Icons.Outlined.Check,
                                                    contentDescription = "Selected variant",
                                                    tint = MaterialTheme.colorScheme.primary,
                                                    modifier = Modifier.size(20.dp)
                                                )
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Spacer(modifier = Modifier.height(10.dp))

                // 3D Native C++ Building Viewport Canvas
                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .height(360.dp)
                        .padding(horizontal = 16.dp)
                        .clip(RoundedCornerShape(18.dp))
                        .testTag("buildings_canvas_container")
                ) {
                    BuildingsViewportCanvas(
                        preset = currentPreset,
                        state = studioState,
                        rotX = rotX,
                        rotY = rotY,
                        zoomFactor = zoomFactor,
                        onDrag = { dx, dy ->
                            rotY = (rotY + dx * 0.45f) % 360f
                            rotX = (rotX + dy * 0.45f).coerceIn(-85f, 85f)
                        },
                        onZoomChange = { zoomFactor = it },
                        onResetView = {
                            rotX = 20f
                            rotY = -35f
                            zoomFactor = 1.0f
                        },
                        onToggleWireframe = {
                            studioState = studioState.copy(wireframe = !studioState.wireframe)
                        },
                        onToggleShowInside = {
                            studioState = studioState.copy(showInside = !studioState.showInside)
                        },
                        isAutoRotating = isAutoRotating,
                        onToggleAutoRotate = { isAutoRotating = !isAutoRotating },
                        onStatsUpdated = { latestStats = it },
                        modifier = Modifier.fillMaxSize()
                    )
                }

                Spacer(modifier = Modifier.height(10.dp))

                // Parameter Sub-Studio Tabs
                ScrollableTabRow(
                    selectedTabIndex = selectedTabIndex,
                    edgePadding = 16.dp,
                    modifier = Modifier.fillMaxWidth().testTag("buildings_tab_row")
                ) {
                    tabTitles.forEachIndexed { index, (title, icon) ->
                        Tab(
                            selected = selectedTabIndex == index,
                            onClick = { selectedTabIndex = index },
                            text = { Text(title) },
                            icon = { Icon(imageVector = icon, contentDescription = null, modifier = Modifier.size(18.dp)) },
                            modifier = Modifier.testTag("building_tab_$index")
                        )
                    }
                }

                when (selectedTabIndex) {
                    0 -> BuildingMassingTab(
                        state = studioState,
                        onStateChange = { studioState = it }
                    )
                    1 -> BuildingFacadeAperturesTab(
                        state = studioState,
                        onStateChange = { studioState = it }
                    )
                    2 -> BuildingRoofInteriorTab(
                        state = studioState,
                        onStateChange = { studioState = it }
                    )
                    3 -> BuildingShadersDiagnosticsTab(
                        preset = currentPreset,
                        state = studioState,
                        stats = latestStats
                    )
                }

                Spacer(modifier = Modifier.height(32.dp))
            }
        }
    }
}
