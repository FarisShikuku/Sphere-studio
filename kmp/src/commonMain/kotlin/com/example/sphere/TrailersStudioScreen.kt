package com.example.sphere

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
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
import com.example.sphere.model.TrailerCategoryType
import com.example.sphere.model.TrailerItem
import com.example.sphere.model.createDefaultCommoditiesCatalog
import com.example.sphere.model.createDefaultTrailersCatalog
import com.example.sphere.trailers.TrailersViewportCanvas
import com.example.sphere.trailers.tabs.TrailerCargoHitchTab
import com.example.sphere.trailers.tabs.TrailerRegistryCppTab
import com.example.sphere.trailers.tabs.TrailerRunningGearTab
import com.example.sphere.trailers.tabs.TrailerSurfaceOpticsTab
import com.example.sphere.trailers.tabs.TrailerVulkanComputeTab
import kotlinx.coroutines.isActive
import kotlin.math.roundToInt

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun TrailersStudioScreen(
    onNavigateBack: () -> Unit,
    modifier: Modifier = Modifier
) {
    var catalog by remember { mutableStateOf(createDefaultTrailersCatalog()) }
    var selectedCategory by remember { mutableStateOf(TrailerCategoryType.FLATBED) }
    var selectedTrailerId by remember { mutableIntStateOf(0) }

    val cargoCatalog = remember { createDefaultCommoditiesCatalog() }
    var selectedCargo by remember { mutableStateOf(cargoCatalog[7]) } // Cardboard Box default
    var showLoadedCargo by remember { mutableStateOf(false) }
    var cargoQuantity by remember { mutableIntStateOf(160) }

    var landingGearExtended by remember { mutableStateOf(false) }
    var curtainOpenRatio by remember { mutableFloatStateOf(0f) }
    var hydraulicLiftRatio by remember { mutableFloatStateOf(0f) }

    var rotX by remember { mutableFloatStateOf(16f) }
    var rotY by remember { mutableFloatStateOf(32f) }
    var zoomFactor by remember { mutableFloatStateOf(1.0f) }
    var isAutoRotating by remember { mutableStateOf(false) }

    LaunchedEffect(isAutoRotating) {
        if (!isAutoRotating) return@LaunchedEffect
        var prevFrameNs = 0L
        while (isActive) {
            withFrameNanos { frameNs ->
                if (prevFrameNs != 0L) {
                    val dtSec = ((frameNs - prevFrameNs) / 1_000_000_000f).coerceIn(0f, 0.05f)
                    rotY = (rotY + dtSec * 42f) % 360f
                }
                prevFrameNs = frameNs
            }
        }
    }

    val filteredItems by remember(catalog, selectedCategory) {
        derivedStateOf { catalog.filter { it.category == selectedCategory } }
    }
    val currentTrailer = catalog.firstOrNull { it.id == selectedTrailerId }
        ?: filteredItems.firstOrNull()
        ?: catalog.first()

    var selectedTabIndex by remember { mutableIntStateOf(0) }
    var isDropdownExpanded by remember { mutableStateOf(false) }

    val tabTitles = listOf(
        "Cargo & Hitch" to Icons.Outlined.LocalShipping,
        "Running Gear" to Icons.Outlined.Settings,
        "Optics & Livery" to Icons.Outlined.Palette,
        "Vulkan Shaders" to Icons.Outlined.Terminal,
        "C++ Registry" to Icons.Outlined.Tune
    )

    Scaffold(
        topBar = {
            TopAppBar(
                title = {
                    Column {
                        Text(
                            text = "Trailers & Haulage Studio",
                            style = MaterialTheme.typography.titleMedium,
                            fontWeight = FontWeight.Bold
                        )
                        Text(
                            text = "Procedural Vulkan Semi-Trailers & Running Gear (gen/trailers)",
                            style = MaterialTheme.typography.bodySmall,
                            color = MaterialTheme.colorScheme.onSurfaceVariant
                        )
                    }
                },
                navigationIcon = {
                    IconButton(
                        onClick = onNavigateBack,
                        modifier = Modifier.testTag("trailers_back_button")
                    ) {
                        Icon(Icons.AutoMirrored.Outlined.ArrowBack, contentDescription = "Navigate back")
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
                .verticalScroll(rememberScrollState())
        ) {
            // 12 Trailer Category Filter Chips
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState())
                    .padding(horizontal = 16.dp, vertical = 8.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                TrailerCategoryType.entries.forEach { category ->
                    val isSelected = selectedCategory == category
                    FilterChip(
                        selected = isSelected,
                        onClick = {
                            selectedCategory = category
                            isDropdownExpanded = false
                            val firstInCat = catalog.firstOrNull { it.category == category }
                            if (firstInCat != null) {
                                selectedTrailerId = firstInCat.id
                            }
                        },
                        label = { Text(category.displayName, style = MaterialTheme.typography.bodySmall) },
                        modifier = Modifier.testTag("trailer_filter_chip_${category.name}")
                    )
                }
            }

            // Trailer Variant Selector Dropdown (direct Surface click + DropdownMenu for instant response)
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(horizontal = 16.dp, vertical = 4.dp)
            ) {
                Surface(
                    onClick = { isDropdownExpanded = !isDropdownExpanded },
                    shape = RoundedCornerShape(12.dp),
                    color = MaterialTheme.colorScheme.surfaceContainerLow,
                    border = BorderStroke(
                        width = if (isDropdownExpanded) 2.dp else 1.dp,
                        color = if (isDropdownExpanded) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.outline
                    ),
                    modifier = Modifier
                        .fillMaxWidth()
                        .defaultMinSize(minHeight = 56.dp)
                        .testTag("dropdown_trailer_selector")
                ) {
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(horizontal = 14.dp, vertical = 10.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Icon(
                            imageVector = Icons.Outlined.RvHookup,
                            contentDescription = null,
                            tint = MaterialTheme.colorScheme.primary
                        )
                        Spacer(modifier = Modifier.width(12.dp))
                        Column(modifier = Modifier.weight(1f)) {
                            Text(
                                text = "Select Trailer Variant (${filteredItems.size} in category)",
                                style = MaterialTheme.typography.labelSmall,
                                color = if (isDropdownExpanded) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.onSurfaceVariant
                            )
                            Text(
                                text = currentTrailer.name,
                                style = MaterialTheme.typography.bodyLarge,
                                fontWeight = FontWeight.SemiBold,
                                maxLines = 1,
                                overflow = TextOverflow.Ellipsis
                            )
                        }
                        Icon(
                            imageVector = Icons.Filled.ArrowDropDown,
                            contentDescription = "Expand trailer list",
                            tint = MaterialTheme.colorScheme.onSurfaceVariant,
                            modifier = Modifier.rotate(if (isDropdownExpanded) 180f else 0f)
                        )
                    }
                }

                DropdownMenu(
                    expanded = isDropdownExpanded,
                    onDismissRequest = { isDropdownExpanded = false },
                    modifier = Modifier
                        .widthIn(min = 280.dp, max = 380.dp)
                        .heightIn(max = 360.dp)
                ) {
                    filteredItems.forEach { item ->
                        key(item.id) {
                            val isCurrent = item.id == currentTrailer.id
                            DropdownMenuItem(
                                text = {
                                    Column {
                                        Text(
                                            item.name,
                                            fontWeight = if (isCurrent) FontWeight.Bold else FontWeight.SemiBold,
                                            color = if (isCurrent) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.onSurface
                                        )
                                        Text(
                                            "${item.emptyWeightKg.roundToInt()} kg tare • Max ${(item.maxPayloadKg / 1000f).roundToInt()}T • ${item.axleConfig.displayName}",
                                            style = MaterialTheme.typography.bodySmall,
                                            color = MaterialTheme.colorScheme.onSurfaceVariant
                                        )
                                    }
                                },
                                trailingIcon = if (isCurrent) {
                                    {
                                        Icon(
                                            imageVector = Icons.Outlined.Check,
                                            contentDescription = "Selected",
                                            tint = MaterialTheme.colorScheme.primary,
                                            modifier = Modifier.size(18.dp)
                                        )
                                    }
                                } else null,
                                onClick = {
                                    selectedTrailerId = item.id
                                    isDropdownExpanded = false
                                },
                                modifier = Modifier.testTag("trailer_menu_item_${item.id}")
                            )
                        }
                    }
                }
            }

            Spacer(modifier = Modifier.height(10.dp))

            // 3D Procedural Trailer Viewport Canvas
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(360.dp)
                    .padding(horizontal = 16.dp)
                    .clip(RoundedCornerShape(18.dp))
                    .testTag("trailers_canvas_container")
            ) {
                TrailersViewportCanvas(
                    item = currentTrailer,
                    rotX = rotX,
                    rotY = rotY,
                    zoomFactor = zoomFactor,
                    onDrag = { dx, dy ->
                        rotY = (rotY + dx * 0.45f) % 360f
                        rotX = (rotX + dy * 0.45f).coerceIn(-85f, 85f)
                    },
                    onZoomChange = { zoomFactor = it },
                    onResetView = {
                        rotX = 16f
                        rotY = 32f
                        zoomFactor = 1.0f
                    },
                    landingGearExtended = landingGearExtended,
                    showLoadedCargo = showLoadedCargo,
                    loadedCargo = selectedCargo,
                    cargoQuantity = cargoQuantity,
                    curtainOpenRatio = curtainOpenRatio,
                    hydraulicLiftRatio = hydraulicLiftRatio,
                    isAutoRotating = isAutoRotating,
                    onToggleAutoRotate = { isAutoRotating = !isAutoRotating },
                    modifier = Modifier.fillMaxSize()
                )
            }

            Spacer(modifier = Modifier.height(14.dp))

            // Sub-Studio Navigation Tabs
            ScrollableTabRow(
                selectedTabIndex = selectedTabIndex,
                edgePadding = 16.dp,
                modifier = Modifier.fillMaxWidth().testTag("trailers_tab_row")
            ) {
                tabTitles.forEachIndexed { index, (title, icon) ->
                    Tab(
                        selected = selectedTabIndex == index,
                        onClick = { selectedTabIndex = index },
                        text = { Text(title) },
                        icon = { Icon(imageVector = icon, contentDescription = null, modifier = Modifier.size(18.dp)) },
                        modifier = Modifier.testTag("trailer_tab_$index")
                    )
                }
            }

            val onTrailerUpdate: (TrailerItem) -> Unit = { updated ->
                catalog = catalog.map { if (it.id == updated.id) updated else it }
            }

            when (selectedTabIndex) {
                0 -> TrailerCargoHitchTab(
                    trailer = currentTrailer,
                    onTrailerChange = onTrailerUpdate,
                    cargoCatalog = cargoCatalog,
                    selectedCargo = selectedCargo,
                    onSelectCargo = { selectedCargo = it },
                    showLoadedCargo = showLoadedCargo,
                    onToggleLoadedCargo = { showLoadedCargo = it },
                    cargoQuantity = cargoQuantity,
                    onCargoQuantityChange = { cargoQuantity = it },
                    landingGearExtended = landingGearExtended,
                    onToggleLandingGear = { landingGearExtended = it },
                    curtainOpenRatio = curtainOpenRatio,
                    onCurtainOpenRatioChange = { curtainOpenRatio = it },
                    hydraulicLiftRatio = hydraulicLiftRatio,
                    onHydraulicLiftRatioChange = { hydraulicLiftRatio = it }
                )
                1 -> TrailerRunningGearTab(
                    trailer = currentTrailer,
                    onTrailerChange = onTrailerUpdate
                )
                2 -> TrailerSurfaceOpticsTab(
                    trailer = currentTrailer,
                    onTrailerChange = onTrailerUpdate
                )
                3 -> TrailerVulkanComputeTab(
                    trailer = currentTrailer
                )
                4 -> TrailerRegistryCppTab(
                    trailer = currentTrailer,
                    selectedCargo = selectedCargo,
                    cargoQuantity = cargoQuantity
                )
            }

            Spacer(modifier = Modifier.height(32.dp))
        }
    }
}
