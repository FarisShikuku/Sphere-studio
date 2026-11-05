package com.example.sphere

import androidx.activity.compose.BackHandler
import androidx.compose.animation.AnimatedVisibility
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch

/**
 * SphereStudioApp - Top-level App Scaffold and Navigation Coordinator.
 *
 * Modular architecture:
 *  - Models: [GeometryModels], [ShadowModels], [SolarModels], [AssetCategory]
 *  - Viewport: [NativeVulkanSurfaceHost] (Native C++20 Vulkan 1.3 WSI Swapchain via ANativeWindow)
 *  - Control Tabs: [DimensionsControlTab], [SurfaceAndStageTab], [ShadowsControlTab], [SkyAndSunTab], [NativeStructureTab]
 *  - Studios: [SkyStudioScreen], [WeatherStudioScreen], [EmptyAssetScreen]
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SphereStudioApp() {
    val drawerState = rememberDrawerState(initialValue = DrawerValue.Closed)
    val coroutineScope = rememberCoroutineScope()

    // Active Category Navigation (0 = "test" / Primary 3D Studio)
    var selectedCategoryIndex by remember { mutableIntStateOf(0) }
    val currentCategory = availableAssetCategories[selectedCategoryIndex]

    val lightingIndex = remember { availableAssetCategories.indexOfFirst { it.id == "lighting" }.coerceAtLeast(0) }
    val skyIndex = remember { availableAssetCategories.indexOfFirst { it.id == "sky" }.coerceAtLeast(0) }
    val weatherIndex = remember { availableAssetCategories.indexOfFirst { it.id == "weather" }.coerceAtLeast(0) }
    val surfacesIndex = remember { availableAssetCategories.indexOfFirst { it.id == "surfaces" }.coerceAtLeast(0) }
    val textIndex = remember { availableAssetCategories.indexOfFirst { it.id == "text" }.coerceAtLeast(0) }

    // Handle system back navigation to return to primary test studio
    BackHandler(enabled = selectedCategoryIndex != 0 || drawerState.isOpen) {
        if (drawerState.isOpen) {
            coroutineScope.launch { drawerState.close() }
        } else {
            selectedCategoryIndex = 0
        }
    }

    // Geometry Dimensions (Metric meters: default 1.2m cube)
    var width by remember { mutableFloatStateOf(1.2f) }
    var height by remember { mutableFloatStateOf(1.2f) }
    var depth by remember { mutableFloatStateOf(1.2f) }
    var scale by remember { mutableFloatStateOf(1.0f) }

    // Stage Auto-Fitting by Default
    var autoFitStage by remember { mutableStateOf(true) }

    // Surface & Material
    var activeColor by remember { mutableStateOf(paletteColors[0]) }
    var selectedPattern by remember { mutableStateOf(SurfacePattern.SOLID) }
    var surfaceMode by remember { mutableStateOf(SurfaceSceneMode.FLAT_PIECE) }
    var manualStageWidth by remember { mutableFloatStateOf(3.5f) }
    var manualStageLength by remember { mutableFloatStateOf(3.5f) }
    var stageHeight by remember { mutableFloatStateOf(0.25f) }

    // Computed stage size: auto-fit to active item footprint by default
    val effectiveItemSize = maxOf(width, height, depth) * scale
    val effectiveStageWidth = if (autoFitStage) {
        if (effectiveItemSize <= 1.25f) 3.0f else (effectiveItemSize * 2.6f).coerceIn(3.0f, 35.0f)
    } else manualStageWidth

    val effectiveStageLength = if (autoFitStage) {
        if (effectiveItemSize <= 1.25f) 3.0f else (effectiveItemSize * 2.6f).coerceIn(3.0f, 35.0f)
    } else manualStageLength

    // Sky & Sun
    var enableSky by remember { mutableStateOf(true) }
    var enableSun by remember { mutableStateOf(true) }
    var sunAzimuth by remember { mutableFloatStateOf(45f) }
    var sunElevation by remember { mutableFloatStateOf(40f) }
    var skyTurbidity by remember { mutableFloatStateOf(2.5f) }

    // Sascha Willems Vulkan Shadow Mapping
    var enableShadows by remember { mutableStateOf(true) }
    var shadowQuality by remember { mutableStateOf(ShadowQuality.PCF_5X5) }
    var shadowIntensity by remember { mutableFloatStateOf(0.72f) }
    var shadowSoftness by remember { mutableFloatStateOf(1.8f) }
    var depthBias by remember { mutableFloatStateOf(0.005f) }
    var showShadowMapDebug by remember { mutableStateOf(false) }

    // Viewport Rotation & Animation
    var rotX by remember { mutableFloatStateOf(25f) }
    var rotY by remember { mutableFloatStateOf(-35f) }
    var autoRotate by remember { mutableStateOf(true) }

    LaunchedEffect(autoRotate) {
        if (!autoRotate) return@LaunchedEffect
        while (isActive) {
            delay(16)
            rotY = (rotY + 0.35f) % 360f
        }
    }

    // Studio Control Tab selection
    var selectedControlTab by remember { mutableIntStateOf(0) }
    val tabTitles = listOf("Dimensions", "Surface & Stage", "Vulkan Shadows", "Sky & Sun", "Native C++")

    ModalNavigationDrawer(
        drawerState = drawerState,
        drawerContent = {
            ModalDrawerSheet(modifier = Modifier.width(300.dp)) {
                Spacer(Modifier.height(16.dp))
                Row(
                    modifier = Modifier.padding(horizontal = 20.dp, vertical = 12.dp),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Icon(
                        Icons.Default.Layers,
                        contentDescription = null,
                        tint = MaterialTheme.colorScheme.primary,
                        modifier = Modifier.size(28.dp)
                    )
                    Spacer(Modifier.width(12.dp))
                    Column {
                        Text(
                            "Sphere Engine 3D",
                            style = MaterialTheme.typography.titleMedium.copy(fontWeight = FontWeight.Bold)
                        )
                        Text(
                            "Asset & Studio Library",
                            style = MaterialTheme.typography.bodySmall.copy(color = MaterialTheme.colorScheme.onSurfaceVariant)
                        )
                    }
                }
                HorizontalDivider(modifier = Modifier.padding(vertical = 8.dp))

                Column(
                    modifier = Modifier
                        .fillMaxWidth()
                        .verticalScroll(rememberScrollState())
                        .padding(horizontal = 12.dp)
                ) {
                    availableAssetCategories.forEachIndexed { index, category ->
                        val isSelected = selectedCategoryIndex == index
                        NavigationDrawerItem(
                            icon = { Icon(category.icon, contentDescription = null) },
                            label = {
                                Column {
                                    Text(
                                        category.title,
                                        fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal
                                    )
                                    Text(
                                        category.subtitle,
                                        style = MaterialTheme.typography.labelSmall.copy(
                                            color = MaterialTheme.colorScheme.onSurfaceVariant,
                                            fontSize = 10.sp
                                        )
                                    )
                                }
                            },
                            selected = isSelected,
                            onClick = {
                                selectedCategoryIndex = index
                                coroutineScope.launch { drawerState.close() }
                            },
                            modifier = Modifier
                                .padding(vertical = 4.dp)
                                .testTag("nav_item_${category.id}")
                        )
                    }
                }
            }
        }
    ) {
        when (currentCategory.id) {
            "sky" -> {
                SkyStudioScreen(
                    onNavigateBack = {
                        selectedCategoryIndex = 0
                    },
                    modifier = Modifier.fillMaxSize()
                )
            }
            "weather" -> {
                WeatherStudioScreen(
                    onNavigateBack = {
                        selectedCategoryIndex = 0
                    }
                )
            }
            "test" -> {
                // Primary 3D Viewport with Sascha Willems Cascaded Shadows & Stages
                Scaffold(
                    topBar = {
                        TopAppBar(
                            title = {
                                Column {
                                    Text(
                                        "Sphere 3D Studio",
                                        style = MaterialTheme.typography.titleMedium.copy(fontWeight = FontWeight.Bold)
                                    )
                                    Text(
                                        "Vulkan Cascaded Shadows & Stage Synthesis",
                                        style = MaterialTheme.typography.bodySmall.copy(
                                            color = MaterialTheme.colorScheme.onSurfaceVariant,
                                            fontSize = 11.sp
                                        )
                                    )
                                }
                            },
                            navigationIcon = {
                                IconButton(
                                    onClick = { coroutineScope.launch { drawerState.open() } },
                                    modifier = Modifier.testTag("app_menu_button")
                                ) {
                                    Icon(Icons.Default.Menu, contentDescription = "Open Drawer")
                                }
                            },
                            actions = {
                                IconButton(
                                    onClick = { autoRotate = !autoRotate },
                                    modifier = Modifier.testTag("auto_rotate_button")
                                ) {
                                    Icon(
                                        if (autoRotate) Icons.Default.RotateRight else Icons.Default.Pause,
                                        contentDescription = "Toggle Auto Rotation",
                                        tint = if (autoRotate) MaterialTheme.colorScheme.primary else Color.Gray
                                    )
                                }
                                IconButton(
                                    onClick = {
                                        width = 1.2f
                                        height = 1.2f
                                        depth = 1.2f
                                        scale = 1.0f
                                        sunAzimuth = 45f
                                        sunElevation = 40f
                                        rotX = 25f
                                        rotY = -35f
                                        surfaceMode = SurfaceSceneMode.FLAT_PIECE
                                        autoFitStage = true
                                    },
                                    modifier = Modifier.testTag("reset_defaults_button")
                                ) {
                                    Icon(Icons.Default.RestartAlt, contentDescription = "Reset Settings")
                                }
                            },
                            colors = TopAppBarDefaults.topAppBarColors(
                                containerColor = MaterialTheme.colorScheme.surface
                            )
                        )
                    }
                ) { innerPadding ->
                    Column(
                        modifier = Modifier
                            .fillMaxSize()
                            .padding(innerPadding)
                    ) {
                        // Native C++ Vulkan 1.3 Scene Viewport
                        Box(
                            modifier = Modifier
                                .fillMaxWidth()
                                .weight(1.15f)
                                .background(Color(0xFF0F1218))
                        ) {
                            val stageModeId = when (surfaceMode) {
                                SurfaceSceneMode.FLAT_PIECE -> 0
                                SurfaceSceneMode.ENDLESS -> 1
                                SurfaceSceneMode.WATER_BODY -> 2
                                SurfaceSceneMode.BROKEN_SLAB -> 3
                                SurfaceSceneMode.NONE -> 4
                            }
                            com.example.sphere.viewport.NativeVulkanSurfaceHost(
                                modifier = Modifier.fillMaxSize(),
                                lightPosX = 0.0f,
                                lightPosY = 2.5f,
                                lightPosZ = 1.8f,
                                intensity = 2500.0f,
                                colorR = activeColor.color.red,
                                colorG = activeColor.color.green,
                                colorB = activeColor.color.blue,
                                roughness = 0.45f,
                                metallic = 0.0f,
                                objectWidth = width * scale,
                                objectHeight = height * scale,
                                objectDepth = depth * scale,
                                stageMode = stageModeId
                            )
                        }

                        // Tab Navigation Row
                        ScrollableTabRow(
                            selectedTabIndex = selectedControlTab,
                            edgePadding = 12.dp,
                            modifier = Modifier.fillMaxWidth()
                        ) {
                            tabTitles.forEachIndexed { index, title ->
                                Tab(
                                    selected = selectedControlTab == index,
                                    onClick = { selectedControlTab = index },
                                    text = {
                                        Text(
                                            title,
                                            style = MaterialTheme.typography.labelMedium.copy(
                                                fontWeight = if (selectedControlTab == index) FontWeight.Bold else FontWeight.Normal
                                            )
                                        )
                                    }
                                )
                            }
                        }

                        // Tab Controls: Fills lower half comfortably with padding and smooth scrolling
                        Box(
                            modifier = Modifier
                                .fillMaxWidth()
                                .weight(1.0f)
                                .background(MaterialTheme.colorScheme.background)
                                .padding(horizontal = 14.dp, vertical = 8.dp)
                        ) {
                            when (selectedControlTab) {
                                0 -> DimensionsControlTab(
                                    width = width,
                                    height = height,
                                    depth = depth,
                                    scale = scale,
                                    isAutoFit = autoFitStage,
                                    onWidthChange = { width = it },
                                    onHeightChange = { height = it },
                                    onDepthChange = { depth = it },
                                    onScaleChange = { scale = it },
                                    onReset = {
                                        width = 1.2f
                                        height = 1.2f
                                        depth = 1.2f
                                        scale = 1.0f
                                        autoFitStage = true
                                    }
                                )
                                1 -> SurfaceAndStageTab(
                                    surfaceMode = surfaceMode,
                                    onSurfaceModeSelect = { surfaceMode = it },
                                    stageWidth = effectiveStageWidth,
                                    onStageWidthChange = {
                                        manualStageWidth = it
                                        autoFitStage = false
                                    },
                                    stageLength = effectiveStageLength,
                                    onStageLengthChange = {
                                        manualStageLength = it
                                        autoFitStage = false
                                    },
                                    stageHeight = stageHeight,
                                    onStageHeightChange = { stageHeight = it },
                                    itemSize = effectiveItemSize,
                                    isAutoFit = autoFitStage,
                                    onApplyAutoFit = {
                                        autoFitStage = true
                                    },
                                    selectedColor = activeColor,
                                    onColorSelect = { activeColor = it },
                                    selectedPattern = selectedPattern,
                                    onPatternSelect = { selectedPattern = it }
                                )
                                2 -> ShadowsControlTab(
                                    enableShadows = enableShadows,
                                    onToggleShadows = { enableShadows = it },
                                    shadowQuality = shadowQuality,
                                    onShadowQualitySelect = { shadowQuality = it },
                                    shadowIntensity = shadowIntensity,
                                    onShadowIntensityChange = { shadowIntensity = it },
                                    shadowSoftness = shadowSoftness,
                                    onShadowSoftnessChange = { shadowSoftness = it },
                                    depthBias = depthBias,
                                    onDepthBiasChange = { depthBias = it },
                                    showShadowMapDebug = showShadowMapDebug,
                                    onToggleShadowMapDebug = { showShadowMapDebug = it },
                                    sunElevation = sunElevation,
                                    sunAzimuth = sunAzimuth
                                )
                                3 -> SkyAndSunTab(
                                    enableSky = enableSky,
                                    onToggleSky = { enableSky = it },
                                    enableSun = enableSun,
                                    onToggleSun = { enableSun = it },
                                    sunElevation = sunElevation,
                                    onElevationChange = { sunElevation = it },
                                    sunAzimuth = sunAzimuth,
                                    onAzimuthChange = { sunAzimuth = it },
                                    turbidity = skyTurbidity,
                                    onTurbidityChange = { skyTurbidity = it },
                                    onSelectPreset = { preset ->
                                        sunElevation = preset.elevation
                                        sunAzimuth = preset.azimuth
                                        skyTurbidity = preset.turbidity
                                    }
                                )
                                4 -> NativeStructureTab()
                            }
                        }
                    }
                }
            }
            "lighting" -> {
                com.example.sphere.screens.LightingStudioScreen(
                    onNavigateBack = {
                        selectedCategoryIndex = 0
                    }
                )
            }
            "surfaces" -> {
                com.example.sphere.screens.MaterialInspectorScreen(
                    onNavigateBack = {
                        selectedCategoryIndex = 0
                    }
                )
            }
            "text" -> {
                GlyphCoreStudioScreen(
                    onNavigateBack = {
                        selectedCategoryIndex = 0
                    }
                )
            }
            else -> {
                EmptyAssetScreen(
                    category = currentCategory,
                    onBackToTest = {
                        selectedCategoryIndex = 0
                    },
                    onNavigateToSky = {
                        selectedCategoryIndex = skyIndex
                    },
                    onNavigateToWeather = {
                        selectedCategoryIndex = weatherIndex
                    },
                    onNavigateToSurfaces = {
                        selectedCategoryIndex = surfacesIndex
                    },
                    onNavigateToText = {
                        selectedCategoryIndex = textIndex
                    }
                )
            }
        }
    }
}
