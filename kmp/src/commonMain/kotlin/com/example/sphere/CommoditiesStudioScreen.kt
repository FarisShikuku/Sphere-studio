package com.example.sphere

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.defaultMinSize
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.widthIn
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.outlined.ArrowBack
import androidx.compose.material.icons.filled.ArrowDropDown
import androidx.compose.material.icons.outlined.Check
import androidx.compose.material.icons.outlined.Inventory2
import androidx.compose.material.icons.outlined.LocalShipping
import androidx.compose.material.icons.outlined.Palette
import androidx.compose.material.icons.outlined.QrCode
import androidx.compose.material.icons.outlined.Terminal
import androidx.compose.material.icons.outlined.Tune
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.FilterChip
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.ScrollableTabRow
import androidx.compose.material3.Surface
import androidx.compose.material3.Tab
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.ui.draw.rotate
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.runtime.Composable
import androidx.compose.runtime.derivedStateOf
import androidx.compose.runtime.getValue
import androidx.compose.runtime.key
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.commodities.CommoditiesViewportCanvas
import com.example.sphere.commodities.tabs.CommodityLogisticsTab
import com.example.sphere.commodities.tabs.CommodityRegistryCppTab
import com.example.sphere.commodities.tabs.CommodityStencilsTab
import com.example.sphere.commodities.tabs.CommoditySurfaceOpticsTab
import com.example.sphere.commodities.tabs.CommodityVulkanComputeTab
import com.example.sphere.model.CommodityCategoryType
import com.example.sphere.model.CommodityItem
import com.example.sphere.model.createDefaultCommoditiesCatalog

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun CommoditiesStudioScreen(
    onNavigateBack: () -> Unit,
    modifier: Modifier = Modifier
) {
    // Complete catalog of procedural commodities
    var catalog by remember { mutableStateOf(createDefaultCommoditiesCatalog()) }
    var selectedCategory by remember { mutableStateOf(CommodityCategoryType.BAGGED_GOODS) }
    var selectedCommodityId by remember { mutableIntStateOf(0) }
    var stackingMode by remember { mutableStateOf(com.example.sphere.model.StackingMode.PALLET_STACK) }
    var rotX by remember { mutableFloatStateOf(20f) }
    var rotY by remember { mutableFloatStateOf(35f) }
    var zoomFactor by remember { mutableFloatStateOf(1.0f) }
    var showStraps by remember { mutableStateOf(true) }
    var showShrinkwrap by remember { mutableStateOf(false) }
    var showPalletBase by remember { mutableStateOf(true) }

    val filteredItems by remember(catalog, selectedCategory) {
        derivedStateOf {
            catalog.filter { it.category == selectedCategory }
        }
    }

    val currentCommodity = catalog.firstOrNull { it.id == selectedCommodityId }
        ?: filteredItems.firstOrNull()
        ?: catalog.first()

    var selectedTabIndex by remember { mutableIntStateOf(0) }
    var isDropdownExpanded by remember { mutableStateOf(false) }

    val tabTitles = listOf(
        "Logistics & Stacking" to Icons.Outlined.LocalShipping,
        "Surface & PBR Optics" to Icons.Outlined.Palette,
        "Stencils & Labels" to Icons.Outlined.QrCode,
        "Vulkan Shaders" to Icons.Outlined.Terminal,
        "C++ Registry" to Icons.Outlined.Tune
    )

    Scaffold(
        topBar = {
            TopAppBar(
                title = {
                    Column {
                        Text(
                            text = "Commodities & Cargo Studio",
                            style = MaterialTheme.typography.titleMedium,
                            fontWeight = FontWeight.Bold
                        )
                        Text(
                            text = "Procedural Vulkan Physical Goods Engine (gen/cargo)",
                            style = MaterialTheme.typography.bodySmall,
                            color = MaterialTheme.colorScheme.onSurfaceVariant
                        )
                    }
                },
                navigationIcon = {
                    IconButton(
                        onClick = onNavigateBack,
                        modifier = Modifier.testTag("commodities_back_button")
                    ) {
                        Icon(
                            imageVector = Icons.AutoMirrored.Outlined.ArrowBack,
                            contentDescription = "Navigate back"
                        )
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
            // Category Filter Chips
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState())
                    .padding(horizontal = 16.dp, vertical = 8.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                CommodityCategoryType.entries.forEach { category ->
                    val isSelected = selectedCategory == category
                    FilterChip(
                        selected = isSelected,
                        onClick = {
                            selectedCategory = category
                            isDropdownExpanded = false
                            val firstInCat = catalog.firstOrNull { it.category == category }
                            if (firstInCat != null) {
                                selectedCommodityId = firstInCat.id
                            }
                        },
                        label = {
                            Text(category.displayName, style = MaterialTheme.typography.bodySmall)
                        },
                        modifier = Modifier.testTag("filter_chip_${category.name}")
                    )
                }
            }

            // Commodity Variant Selector
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
                        .testTag("dropdown_commodity_selector")
                ) {
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(horizontal = 14.dp, vertical = 10.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Icon(
                            imageVector = Icons.Outlined.Inventory2,
                            contentDescription = null,
                            tint = MaterialTheme.colorScheme.primary
                        )
                        Spacer(modifier = Modifier.width(12.dp))
                        Column(modifier = Modifier.weight(1f)) {
                            Text(
                                text = "Select Commodity Variant (${filteredItems.size} in category)",
                                style = MaterialTheme.typography.labelSmall,
                                color = if (isDropdownExpanded) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.onSurfaceVariant
                            )
                            Text(
                                text = currentCommodity.name,
                                style = MaterialTheme.typography.bodyLarge,
                                fontWeight = FontWeight.SemiBold,
                                maxLines = 1,
                                overflow = TextOverflow.Ellipsis
                            )
                        }
                        Icon(
                            imageVector = Icons.Filled.ArrowDropDown,
                            contentDescription = "Expand commodity list",
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
                            val isCurrent = item.id == currentCommodity.id
                            DropdownMenuItem(
                                text = {
                                    Column {
                                        Text(
                                            item.name,
                                            fontWeight = if (isCurrent) FontWeight.Bold else FontWeight.SemiBold,
                                            color = if (isCurrent) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.onSurface
                                        )
                                        Text(
                                            "${item.weightKg} kg • ${item.lengthM}x${item.widthM}x${item.heightM} m",
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
                                    selectedCommodityId = item.id
                                    isDropdownExpanded = false
                                },
                                modifier = Modifier.testTag("menu_item_${item.id}")
                            )
                        }
                    }
                }
            }

            Spacer(modifier = Modifier.height(10.dp))

            // 3D Procedural Viewport Canvas
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(380.dp)
                    .padding(horizontal = 16.dp)
                    .clip(RoundedCornerShape(18.dp))
                    .testTag("commodities_canvas_container")
            ) {
                CommoditiesViewportCanvas(
                    item = currentCommodity,
                    stackingMode = stackingMode,
                    rotX = rotX,
                    rotY = rotY,
                    zoomFactor = zoomFactor,
                    onDrag = { dx, dy ->
                        rotY = (rotY + dx * 0.45f) % 360f
                        rotX = (rotX + dy * 0.45f).coerceIn(-85f, 85f)
                    },
                    onZoomChange = { newZoom ->
                        zoomFactor = newZoom
                    },
                    onResetView = {
                        rotX = 20f
                        rotY = 35f
                        zoomFactor = 1.0f
                    },
                    showStraps = showStraps,
                    showShrinkwrap = showShrinkwrap,
                    showPalletBase = showPalletBase,
                    modifier = Modifier.fillMaxSize()
                )
            }

            Spacer(modifier = Modifier.height(14.dp))

            // Sub-Studio Navigation Tabs
            ScrollableTabRow(
                selectedTabIndex = selectedTabIndex,
                edgePadding = 16.dp,
                modifier = Modifier.fillMaxWidth().testTag("commodities_tab_row")
            ) {
                tabTitles.forEachIndexed { index, (title, icon) ->
                    Tab(
                        selected = selectedTabIndex == index,
                        onClick = { selectedTabIndex = index },
                        text = { Text(title) },
                        icon = { Icon(imageVector = icon, contentDescription = null, modifier = Modifier.size(18.dp)) },
                        modifier = Modifier.testTag("commodity_tab_$index")
                    )
                }
            }

            // Tab Content
            val onCommodityUpdate: (CommodityItem) -> Unit = { updated ->
                catalog = catalog.map { if (it.id == updated.id) updated else it }
            }

            when (selectedTabIndex) {
                0 -> CommodityLogisticsTab(
                    item = currentCommodity,
                    weightKg = currentCommodity.nominalWeightKg,
                    onWeightChange = { onCommodityUpdate(currentCommodity.copy(nominalWeightKg = it)) },
                    lengthM = currentCommodity.lengthM,
                    onLengthChange = { onCommodityUpdate(currentCommodity.copy(lengthM = it)) },
                    widthM = currentCommodity.widthM,
                    onWidthChange = { onCommodityUpdate(currentCommodity.copy(widthM = it)) },
                    heightM = currentCommodity.heightM,
                    onHeightChange = { onCommodityUpdate(currentCommodity.copy(heightM = it)) },
                    stackingMode = stackingMode,
                    onStackingModeChange = { stackingMode = it },
                    showStraps = showStraps,
                    onToggleStraps = { showStraps = it },
                    showShrinkwrap = showShrinkwrap,
                    onToggleShrinkwrap = { showShrinkwrap = it },
                    showPalletBase = showPalletBase,
                    onTogglePalletBase = { showPalletBase = it }
                )
                1 -> CommoditySurfaceOpticsTab(
                    commodity = currentCommodity,
                    onCommodityChange = onCommodityUpdate
                )
                2 -> CommodityStencilsTab(
                    commodity = currentCommodity,
                    onCommodityChange = onCommodityUpdate
                )
                3 -> CommodityVulkanComputeTab(
                    commodity = currentCommodity
                )
                4 -> CommodityRegistryCppTab(
                    commodity = currentCommodity
                )
            }

            Spacer(modifier = Modifier.height(32.dp))
        }
    }
}
