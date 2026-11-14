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
import com.example.sphere.model.VehicleCategoryType
import com.example.sphere.model.VehicleItem
import com.example.sphere.model.createDefaultCommoditiesCatalog
import com.example.sphere.model.createDefaultTrailersCatalog
import com.example.sphere.model.createDefaultVehiclesCatalog
import com.example.sphere.vehicles.VehiclesViewportCanvas
import com.example.sphere.vehicles.tabs.VehicleComponentsTab
import com.example.sphere.vehicles.tabs.VehicleHitchPowertrainTab
import com.example.sphere.vehicles.tabs.VehicleLiveryOpticsTab
import com.example.sphere.vehicles.tabs.VehicleRegistryCppTab
import com.example.sphere.vehicles.tabs.VehicleVulkanComputeTab
import kotlin.math.roundToInt

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun VehiclesStudioScreen(
    onNavigateBack: () -> Unit,
    modifier: Modifier = Modifier
) {
    var catalog by remember { mutableStateOf(createDefaultVehiclesCatalog()) }
    var selectedCategory by remember { mutableStateOf(VehicleCategoryType.LORRY_RIGID) }
    var selectedVehicleId by remember { mutableIntStateOf(0) }

    val trailerCatalog = remember { createDefaultTrailersCatalog() }
    var selectedTrailer by remember { mutableStateOf(trailerCatalog.first()) }
    var hitchTrailerEnabled by remember { mutableStateOf(true) }

    val cargoCatalog = remember { createDefaultCommoditiesCatalog() }
    var selectedCargo by remember { mutableStateOf(cargoCatalog[7]) }
    var showLoadedCargo by remember { mutableStateOf(false) }
    var cargoQuantity by remember { mutableIntStateOf(120) }

    var rotX by remember { mutableFloatStateOf(15f) }
    var rotY by remember { mutableFloatStateOf(35f) }
    var zoomFactor by remember { mutableFloatStateOf(1.0f) }

    val filteredItems by remember(catalog, selectedCategory) {
        derivedStateOf { catalog.filter { it.category == selectedCategory } }
    }
    val currentVehicle = catalog.firstOrNull { it.id == selectedVehicleId }
        ?: filteredItems.firstOrNull()
        ?: catalog.first()

    var selectedTabIndex by remember { mutableIntStateOf(0) }
    var isDropdownExpanded by remember { mutableStateOf(false) }

    val tabTitles = listOf(
        "Hitch & Powertrain" to Icons.Outlined.RvHookup,
        "Livery & Optics" to Icons.Outlined.Palette,
        "Shared Parts" to Icons.Outlined.Build,
        "Vulkan Shaders" to Icons.Outlined.Terminal,
        "C++ Registry" to Icons.Outlined.Tune
    )

    Scaffold(
        topBar = {
            TopAppBar(
                title = {
                    Column {
                        Text(
                            text = "Lorries & Buses Studio",
                            style = MaterialTheme.typography.titleMedium,
                            fontWeight = FontWeight.Bold
                        )
                        Text(
                            text = "Procedural Vulkan Heavy Trucks, Tractors & Coaches (gen/vehicles)",
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
                        modifier = Modifier.testTag("vehicles_back_button")
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
                .verticalScroll(rememberScrollState()),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .widthIn(max = 720.dp)
            ) {
                // 5 Vehicle Category Filter Chips
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .horizontalScroll(rememberScrollState())
                        .padding(horizontal = 16.dp, vertical = 8.dp),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    VehicleCategoryType.entries.forEach { category ->
                        val isSelected = selectedCategory == category
                        FilterChip(
                            selected = isSelected,
                            onClick = {
                                selectedCategory = category
                                isDropdownExpanded = false
                                val firstInCat = catalog.firstOrNull { it.category == category }
                                if (firstInCat != null) {
                                    selectedVehicleId = firstInCat.id
                                }
                            },
                            label = { Text(category.displayName, style = MaterialTheme.typography.bodySmall) },
                            modifier = Modifier.testTag("vehicle_filter_chip_${category.name}")
                        )
                    }
                }

                // Vehicle Variant Selector Dropdown
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
                            .testTag("dropdown_vehicle_selector")
                    ) {
                        Row(
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(horizontal = 14.dp, vertical = 10.dp),
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Icon(
                                imageVector = Icons.Outlined.DirectionsBus,
                                contentDescription = null,
                                tint = MaterialTheme.colorScheme.primary
                            )
                            Spacer(modifier = Modifier.width(12.dp))
                            Column(modifier = Modifier.weight(1f)) {
                                Text(
                                    text = "Select Vehicle Variant (${filteredItems.size} in category)",
                                    style = MaterialTheme.typography.labelSmall,
                                    color = if (isDropdownExpanded) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.onSurfaceVariant
                                )
                                Text(
                                    text = currentVehicle.name,
                                    style = MaterialTheme.typography.bodyLarge,
                                    fontWeight = FontWeight.SemiBold,
                                    maxLines = 1,
                                    overflow = TextOverflow.Ellipsis
                                )
                            }
                            Icon(
                                imageVector = Icons.Filled.ArrowDropDown,
                                contentDescription = "Expand vehicle list",
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
                                val isCurrent = item.id == currentVehicle.id
                                DropdownMenuItem(
                                    text = {
                                        Column {
                                            Text(
                                                item.name,
                                                fontWeight = if (isCurrent) FontWeight.Bold else FontWeight.SemiBold,
                                                color = if (isCurrent) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.onSurface
                                            )
                                            Text(
                                                "${item.kerbWeightKg.roundToInt()} kg • ${item.enginePowerHp.roundToInt()} HP • ${item.driveConfigLabel}",
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
                                        selectedVehicleId = item.id
                                        isDropdownExpanded = false
                                    },
                                    modifier = Modifier.testTag("vehicle_menu_item_${item.id}")
                                )
                            }
                        }
                    }
                }

                Spacer(modifier = Modifier.height(10.dp))

                // 3D Native C++ Procedural Vehicle Viewport Canvas
                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .height(360.dp)
                        .padding(horizontal = 16.dp)
                        .clip(RoundedCornerShape(18.dp))
                        .testTag("vehicles_canvas_container")
                ) {
                    VehiclesViewportCanvas(
                        item = currentVehicle,
                        rotX = rotX,
                        rotY = rotY,
                        zoomFactor = zoomFactor,
                        onDrag = { dx, dy ->
                            rotY = (rotY + dx * 0.45f) % 360f
                            rotX = (rotX + dy * 0.45f).coerceIn(-85f, 85f)
                        },
                        onZoomChange = { zoomFactor = it },
                        onResetView = {
                            rotX = 15f
                            rotY = 35f
                            zoomFactor = 1.0f
                        },
                        hitchTrailerEnabled = hitchTrailerEnabled,
                        hitchedTrailer = selectedTrailer,
                        showLoadedCargo = showLoadedCargo,
                        loadedCargo = selectedCargo,
                        cargoQuantity = cargoQuantity,
                        modifier = Modifier.fillMaxSize()
                    )
                }

                Spacer(modifier = Modifier.height(14.dp))

                // Sub-Studio Navigation Tabs
                ScrollableTabRow(
                    selectedTabIndex = selectedTabIndex,
                    edgePadding = 16.dp,
                    modifier = Modifier.fillMaxWidth().testTag("vehicles_tab_row")
                ) {
                    tabTitles.forEachIndexed { index, (title, icon) ->
                        Tab(
                            selected = selectedTabIndex == index,
                            onClick = { selectedTabIndex = index },
                            text = { Text(title) },
                            icon = { Icon(imageVector = icon, contentDescription = null, modifier = Modifier.size(18.dp)) },
                            modifier = Modifier.testTag("vehicle_tab_$index")
                        )
                    }
                }

                val onVehicleUpdate: (VehicleItem) -> Unit = { updated ->
                    catalog = catalog.map { if (it.id == updated.id) updated else it }
                }

                when (selectedTabIndex) {
                    0 -> VehicleHitchPowertrainTab(
                        vehicle = currentVehicle,
                        onVehicleChange = onVehicleUpdate,
                        trailerCatalog = trailerCatalog,
                        selectedTrailer = selectedTrailer,
                        onSelectTrailer = { selectedTrailer = it },
                        hitchTrailerEnabled = hitchTrailerEnabled,
                        onToggleHitchTrailer = { hitchTrailerEnabled = it },
                        cargoCatalog = cargoCatalog,
                        selectedCargo = selectedCargo,
                        onSelectCargo = { selectedCargo = it },
                        showLoadedCargo = showLoadedCargo,
                        onToggleLoadedCargo = { showLoadedCargo = it },
                        cargoQuantity = cargoQuantity,
                        onCargoQuantityChange = { cargoQuantity = it }
                    )
                    1 -> VehicleLiveryOpticsTab(
                        vehicle = currentVehicle,
                        onVehicleChange = onVehicleUpdate
                    )
                    2 -> VehicleComponentsTab(
                        vehicle = currentVehicle
                    )
                    3 -> VehicleVulkanComputeTab(
                        vehicle = currentVehicle
                    )
                    4 -> VehicleRegistryCppTab(
                        vehicle = currentVehicle,
                        hitchEnabled = hitchTrailerEnabled,
                        hitchedTrailer = selectedTrailer,
                        loadedCargo = selectedCargo,
                        cargoQuantity = cargoQuantity
                    )
                }

                Spacer(modifier = Modifier.height(32.dp))
            }
        }
    }
}
