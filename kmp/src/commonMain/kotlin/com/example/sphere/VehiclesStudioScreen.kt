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
import com.example.sphere.model.CabDriveSide
import com.example.sphere.model.VehicleCategoryType
import com.example.sphere.model.VehicleItem
import com.example.sphere.model.createDefaultCommoditiesCatalog
import com.example.sphere.model.createDefaultTrailersCatalog
import com.example.sphere.model.createDefaultVehiclesCatalog
import com.example.sphere.nativebridge.SphereNativeEngine
import com.example.sphere.vehicles.VehicleDriveConsole
import com.example.sphere.vehicles.VehicleStreamingStats
import com.example.sphere.vehicles.VehiclesViewportCanvas
import com.example.sphere.vehicles.computeFallbackGridCellX
import com.example.sphere.vehicles.rememberVehicleAudioController
import com.example.sphere.vehicles.tabs.VehicleComponentsTab
import com.example.sphere.vehicles.tabs.VehicleHitchPowertrainTab
import com.example.sphere.vehicles.tabs.VehicleLiveryOpticsTab
import com.example.sphere.vehicles.tabs.VehicleRegistryCppTab
import com.example.sphere.vehicles.tabs.VehicleVulkanComputeTab
import kotlinx.coroutines.isActive
import kotlin.math.abs
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

    var insideCabinView by remember { mutableStateOf(false) }
    var cabinRotX by remember { mutableFloatStateOf(14f) }
    var cabinRotY by remember { mutableFloatStateOf(14f) }
    var cabinZoom by remember { mutableFloatStateOf(1.0f) }
    var isAutoRotating by remember { mutableStateOf(false) }

    val filteredItems by remember(catalog, selectedCategory) {
        derivedStateOf { catalog.filter { it.category == selectedCategory } }
    }
    val currentVehicle = catalog.firstOrNull { it.id == selectedVehicleId }
        ?: filteredItems.firstOrNull()
        ?: catalog.first()

    val effectiveHitchForStats = currentVehicle.canHitchTrailer && hitchTrailerEnabled
    val combinedLoadKg = remember(currentVehicle, effectiveHitchForStats, selectedTrailer, showLoadedCargo, selectedCargo, cargoQuantity) {
        val paxMass = if (currentVehicle.passengerCapacity > 2) currentVehicle.passengerCapacity * 72f else 0f
        val trMass = if (effectiveHitchForStats) selectedTrailer.emptyWeightKg else 0f
        val cgMass = if (effectiveHitchForStats && showLoadedCargo) selectedCargo.nominalWeightKg * cargoQuantity else 0f
        currentVehicle.kerbWeightKg + paxMass + trMass + cgMass
    }
    val audioController = rememberVehicleAudioController(
        vehicle = currentVehicle,
        combinedLoadKg = combinedLoadKg
    )
    val currentGridCellX = computeFallbackGridCellX(audioController.distanceM)
    val streamingStats = remember(
        currentVehicle.id,
        effectiveHitchForStats,
        selectedTrailer.id,
        showLoadedCargo,
        selectedCargo.id,
        cargoQuantity,
        currentGridCellX
    ) {
        val raw = SphereNativeEngine.getVehicleStreamingStats(
            vehicleId = currentVehicle.id,
            hitchTrailer = effectiveHitchForStats,
            trailerId = selectedTrailer.id,
            showLoadedCargo = effectiveHitchForStats && showLoadedCargo,
            cargoId = selectedCargo.id,
            cargoQty = cargoQuantity,
            distanceDrivenM = audioController.distanceM
        )
        if (raw.size >= 7) {
            VehicleStreamingStats(
                gridCellX = raw[0],
                gridCellZ = raw[1],
                loadedRingCells = raw[2],
                heroMeshletCount = raw[3],
                maxTrisPerMeshlet = raw[4],
                hlodProxyBatches = raw[5],
                hlodCulledTris = raw[6]
            )
        } else {
            VehicleStreamingStats()
        }
    }

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
                                    if (!firstInCat.isTruck) {
                                        insideCabinView = false
                                    }
                                }
                            },
                            label = { Text(category.displayName, style = MaterialTheme.typography.bodySmall) },
                            modifier = Modifier.testTag("vehicle_filter_chip_${category.name}")
                        )
                    }
                }

                // Vehicle Variant Selector (triggers smooth ModalBottomSheet instead of nested-scroll Popup DropdownMenu)
                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(horizontal = 16.dp, vertical = 4.dp)
                ) {
                    Surface(
                        onClick = { isDropdownExpanded = true },
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
                }

                if (isDropdownExpanded) {
                    val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true)
                    ModalBottomSheet(
                        onDismissRequest = { isDropdownExpanded = false },
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
                                        text = selectedCategory.displayName,
                                        style = MaterialTheme.typography.titleMedium,
                                        fontWeight = FontWeight.Bold
                                    )
                                    Text(
                                        text = "Select procedural C++ model (${filteredItems.size} variants)",
                                        style = MaterialTheme.typography.bodySmall,
                                        color = MaterialTheme.colorScheme.onSurfaceVariant
                                    )
                                }
                                IconButton(
                                    onClick = { isDropdownExpanded = false },
                                    modifier = Modifier.testTag("vehicle_sheet_close_button")
                                ) {
                                    Icon(Icons.Outlined.Close, contentDescription = "Close vehicle list")
                                }
                            }

                            LazyColumn(
                                modifier = Modifier
                                    .fillMaxWidth()
                                    .heightIn(max = 420.dp),
                                verticalArrangement = Arrangement.spacedBy(8.dp),
                                contentPadding = PaddingValues(bottom = 24.dp)
                            ) {
                                items(filteredItems, key = { it.id }) { item ->
                                    val isCurrent = item.id == currentVehicle.id
                                    Surface(
                                        onClick = {
                                            selectedVehicleId = item.id
                                            isDropdownExpanded = false
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
                                            .testTag("vehicle_menu_item_${item.id}")
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
                                                    text = "${item.kerbWeightKg.roundToInt()} kg • ${item.enginePowerHp.roundToInt()} HP • ${item.driveConfigLabel}",
                                                    style = MaterialTheme.typography.bodySmall,
                                                    color = MaterialTheme.colorScheme.onSurfaceVariant
                                                )
                                            }
                                            if (isCurrent) {
                                                Spacer(modifier = Modifier.width(8.dp))
                                                Icon(
                                                    imageVector = Icons.Outlined.Check,
                                                    contentDescription = "Selected",
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

                // 3D Native C++ Procedural Vehicle Viewport Canvas
                val activeCabinMode = insideCabinView && currentVehicle.isTruck
                LaunchedEffect(isAutoRotating, activeCabinMode) {
                    if (!isAutoRotating) return@LaunchedEffect
                    var prevFrameNs = 0L
                    while (isActive) {
                        withFrameNanos { frameNs ->
                            if (prevFrameNs != 0L) {
                                val dtSec = ((frameNs - prevFrameNs) / 1_000_000_000f).coerceIn(0f, 0.05f)
                                if (activeCabinMode) {
                                    var nextYaw = (cabinRotY - dtSec * 36f) % 360f
                                    if (nextYaw >= 180f) nextYaw -= 360f
                                    if (nextYaw < -180f) nextYaw += 360f
                                    cabinRotY = nextYaw
                                } else {
                                    rotY = (rotY + dtSec * 42f) % 360f
                                }
                            }
                            prevFrameNs = frameNs
                        }
                    }
                }
                val toggleCabinMode: (Boolean) -> Unit = { enable ->
                    if (enable && currentVehicle.isTruck) {
                        insideCabinView = true
                        cabinRotX = 16f
                        cabinRotY = if (currentVehicle.cabDriveSide == CabDriveSide.RHD) -16f else 16f
                        cabinZoom = 1.0f
                    } else {
                        insideCabinView = false
                    }
                }
                val toggleCabSide: (CabDriveSide) -> Unit = { side ->
                    catalog = catalog.map {
                        if (it.id == currentVehicle.id) it.copy(cabDriveSide = side) else it
                    }
                    if (abs(cabinRotY) < 35f) {
                        cabinRotY = if (side == CabDriveSide.RHD) -16f else 16f
                    }
                }

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
                        rotX = if (activeCabinMode) cabinRotX else rotX,
                        rotY = if (activeCabinMode) cabinRotY else rotY,
                        zoomFactor = if (activeCabinMode) cabinZoom else zoomFactor,
                        onDrag = { dx, dy ->
                            if (activeCabinMode) {
                                // Unrestricted 360-degree human head turn (wrapped to [-180, 180)) + full vertical tilt
                                var nextYaw = (cabinRotY - dx * 0.40f) % 360f
                                if (nextYaw >= 180f) nextYaw -= 360f
                                if (nextYaw < -180f) nextYaw += 360f
                                cabinRotY = nextYaw
                                cabinRotX = (cabinRotX - dy * 0.32f).coerceIn(-75f, 75f)
                            } else {
                                rotY = (rotY + dx * 0.45f) % 360f
                                rotX = (rotX + dy * 0.45f).coerceIn(-85f, 85f)
                            }
                        },
                        onZoomChange = { nextZoom ->
                            if (activeCabinMode) {
                                cabinZoom = nextZoom
                            } else {
                                zoomFactor = nextZoom
                            }
                        },
                        onResetView = {
                            if (activeCabinMode) {
                                cabinRotX = 16f
                                cabinRotY = if (currentVehicle.cabDriveSide == CabDriveSide.RHD) -16f else 16f
                                cabinZoom = 1.0f
                            } else {
                                rotX = 15f
                                rotY = 35f
                                zoomFactor = 1.0f
                            }
                        },
                        onToggleDoor = {
                            val nextRatio = if (currentVehicle.doorOpenRatio > 0.05f) 0f else 1f
                            catalog = catalog.map {
                                if (it.id == currentVehicle.id) it.copy(doorOpenRatio = nextRatio) else it
                            }
                            audioController.triggerDoorPneumatic()
                        },
                        hitchTrailerEnabled = hitchTrailerEnabled,
                        hitchedTrailer = selectedTrailer,
                        showLoadedCargo = showLoadedCargo,
                        loadedCargo = selectedCargo,
                        cargoQuantity = cargoQuantity,
                        insideCabinView = activeCabinMode,
                        onToggleCabinView = { toggleCabinMode(!activeCabinMode) },
                        onToggleCabDriveSide = {
                            val nextSide = if (currentVehicle.cabDriveSide == CabDriveSide.LHD) CabDriveSide.RHD else CabDriveSide.LHD
                            toggleCabSide(nextSide)
                        },
                        isAutoRotating = isAutoRotating,
                        onToggleAutoRotate = { isAutoRotating = !isAutoRotating },
                        modifier = Modifier.fillMaxSize()
                    )
                }

                Spacer(modifier = Modifier.height(10.dp))

                VehicleDriveConsole(
                    vehicle = currentVehicle,
                    audioController = audioController,
                    streamingStats = streamingStats
                )

                Spacer(modifier = Modifier.height(10.dp))

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
                    val prevDoorOpen = currentVehicle.doorOpenRatio > 0.05f
                    val nextDoorOpen = updated.doorOpenRatio > 0.05f
                    if (prevDoorOpen != nextDoorOpen) {
                        audioController.triggerDoorPneumatic()
                    }
                    catalog = catalog.map { if (it.id == updated.id) updated else it }
                }

                when (selectedTabIndex) {
                    0 -> VehicleHitchPowertrainTab(
                        vehicle = currentVehicle,
                        onVehicleChange = onVehicleUpdate,
                        insideCabinView = activeCabinMode,
                        onToggleInsideCabinView = toggleCabinMode,
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
                        vehicle = currentVehicle,
                        streamingStats = streamingStats
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
