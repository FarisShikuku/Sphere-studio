package com.example.sphere

import androidx.compose.animation.core.LinearEasing
import androidx.compose.animation.core.RepeatMode
import androidx.compose.animation.core.animateFloat
import androidx.compose.animation.core.infiniteRepeatable
import androidx.compose.animation.core.rememberInfiniteTransition
import androidx.compose.animation.core.tween
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.AutoAwesome
import androidx.compose.material.icons.filled.CheckCircle
import androidx.compose.material.icons.filled.Cloud
import androidx.compose.material.icons.filled.Code
import androidx.compose.material.icons.filled.GridOn
import androidx.compose.material.icons.filled.Menu
import androidx.compose.material.icons.filled.PlayArrow
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material.icons.filled.Stop
import androidx.compose.material.icons.filled.ViewInAr
import androidx.compose.material.icons.filled.WbSunny
import androidx.compose.material.icons.outlined.Agriculture
import androidx.compose.material.icons.outlined.Apartment
import androidx.compose.material.icons.outlined.Cloud
import androidx.compose.material.icons.outlined.DirectionsCar
import androidx.compose.material.icons.outlined.Forest
import androidx.compose.material.icons.outlined.GraphicEq
import androidx.compose.material.icons.outlined.Landscape
import androidx.compose.material.icons.outlined.Layers
import androidx.compose.material.icons.outlined.Palette
import androidx.compose.material.icons.outlined.SquareFoot
import androidx.compose.material.icons.outlined.Texture
import androidx.compose.material.icons.outlined.Water
import androidx.compose.material.icons.outlined.WbSunny
import androidx.compose.material.icons.outlined.Widgets
import androidx.compose.material3.Badge
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.DrawerValue
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.FilterChip
import androidx.compose.material3.FilterChipDefaults
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalDrawerSheet
import androidx.compose.material3.ModalNavigationDrawer
import androidx.compose.material3.NavigationDrawerItem
import androidx.compose.material3.NavigationDrawerItemDefaults
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Slider
import androidx.compose.material3.SliderDefaults
import androidx.compose.material3.Surface
import androidx.compose.material3.Switch
import androidx.compose.material3.SwitchDefaults
import androidx.compose.material3.Tab
import androidx.compose.material3.TabRow
import androidx.compose.material3.TabRowDefaults
import androidx.compose.material3.TabRowDefaults.tabIndicatorOffset
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.material3.rememberDrawerState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.PathEffect
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.launch
import kotlin.math.cos
import kotlin.math.sin
import kotlin.math.sqrt

data class AssetCategory(
    val id: String,
    val title: String,
    val subtitle: String,
    val icon: ImageVector,
    val description: String
)

val availableAssetCategories = listOf(
    AssetCategory("test", "Test Screen", "Calibration Cube & Scene Viewport", Icons.Default.ViewInAr, "Interactive 3D geometry, dimensional controls & surface PBR patterns"),
    AssetCategory("water", "Water Bodies", "FFT Ocean, Lakes & Rivers", Icons.Outlined.Water, "Tessendorf FFT ocean spectrum, river splines & shore foam"),
    AssetCategory("roads", "Roads & Pathways", "Splines, Markings & States", Icons.Outlined.Agriculture, "Procedural camber, asphalt PBR, sign SDFs & 10 surface states"),
    AssetCategory("vegetation", "Vegetation & Foliage", "L-System Trees, Grass & Biomes", Icons.Outlined.Forest, "L-system branching, blade compute simulation & species rules"),
    AssetCategory("surfaces", "PBR Surfaces", "Materials, Decals & Detail", Icons.Outlined.Texture, "Multi-layer PBR textures, micro-detail noise & Voronoi"),
    AssetCategory("buildings", "Buildings & Architecture", "Modular Facades & Kits", Icons.Outlined.Apartment, "Grammar-based building generator, socket attachment & floorplans"),
    AssetCategory("weather", "Weather Systems", "Rain, Snow & Clouds", Icons.Outlined.Cloud, "Volumetric froxel fog, raymarched clouds & lightning paths"),
    AssetCategory("terrain", "Terrain & Landscapes", "Erosion, Cliffs & Caves", Icons.Outlined.Landscape, "Hydraulic erosion, marching-cubes voxel caves & splatmaps"),
    AssetCategory("vehicles", "Vehicles & Transports", "Cars, Trucks & Parts", Icons.Outlined.DirectionsCar, "Silhouette extrusion, wheel assemblies & emergency lightbars"),
    AssetCategory("props", "World Props & Utility", "Power, Solar & Street Furniture", Icons.Outlined.Widgets, "Wind turbines, solar PV trackers, catenary wires & transformers"),
    AssetCategory("sky", "Atmosphere & Sky", "Preetham / Hosek-Wilkie", Icons.Outlined.WbSunny, "Atmospheric scattering LUT, star catalogs & solar disk"),
    AssetCategory("particles", "GPU Particle Systems", "VFX & Compute Simulations", Icons.Outlined.Layers, "Sort compute, smoke curl noise, spark physics & flame sheets"),
    AssetCategory("audio", "Procedural Audio", "Synthesis & Ambience", Icons.Outlined.GraphicEq, "Real-time engine oscillator, footsteps & surface acoustics")
)

enum class SurfacePattern(val label: String) {
    SOLID("Solid PBR"),
    GRID("Wire Grid"),
    CHECKER("Checkerboard"),
    VORONOI("Voronoi Cells"),
    RIPPLE("Wave Ripple"),
    HEXAGON("Hex Mesh")
}

data class ColorOption(val name: String, val color: Color)

val paletteColors = listOf(
    ColorOption("Vulkan Cyan", Color(0xFF06B6D4)),
    ColorOption("Neon Emerald", Color(0xFF10B981)),
    ColorOption("Ruby Red", Color(0xFFF43F5E)),
    ColorOption("Sunset Amber", Color(0xFFF59E0B)),
    ColorOption("Cosmic Violet", Color(0xFF8B5CF6)),
    ColorOption("Studio White", Color(0xFFE2E8F0)),
    ColorOption("Obsidian Grey", Color(0xFF475569))
)

// Preset time-of-day solar states
data class TimeOfDayPreset(
    val name: String,
    val elevation: Float,
    val azimuth: Float,
    val turbidity: Float,
    val icon: ImageVector
)

val timePresets = listOf(
    TimeOfDayPreset("Noon", 65.0f, 180.0f, 2.2f, Icons.Default.WbSunny),
    TimeOfDayPreset("Golden Hour", 16.0f, 240.0f, 3.4f, Icons.Outlined.WbSunny),
    TimeOfDayPreset("Sunset", 5.0f, 260.0f, 4.8f, Icons.Outlined.WbSunny),
    TimeOfDayPreset("Sunrise", 8.0f, 85.0f, 3.0f, Icons.Outlined.WbSunny),
    TimeOfDayPreset("Twilight", -4.0f, 280.0f, 2.5f, Icons.Default.Cloud)
)

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SphereStudioApp() {
    var selectedCategory by remember { mutableStateOf(availableAssetCategories[0]) } // Defaults to Test Screen
    val drawerState = rememberDrawerState(initialValue = DrawerValue.Closed)
    val coroutineScope = rememberCoroutineScope()

    // 3D Cube Dimensions
    var cubeWidth by remember { mutableFloatStateOf(1.6f) }
    var cubeHeight by remember { mutableFloatStateOf(1.6f) }
    var cubeDepth by remember { mutableFloatStateOf(1.6f) }
    var cubeScale by remember { mutableFloatStateOf(1.0f) }

    // Color & Surface Pattern
    var selectedColor by remember { mutableStateOf(paletteColors[0]) }
    var selectedPattern by remember { mutableStateOf(SurfacePattern.SOLID) }

    // Sky & Sun (Atmosphere) Controls (referencing kentril0/WaterSurfaceRendering SkyModel)
    var enableSky by remember { mutableStateOf(true) }
    var enableSun by remember { mutableStateOf(true) }
    var sunElevation by remember { mutableFloatStateOf(32.0f) } // degrees
    var sunAzimuth by remember { mutableFloatStateOf(145.0f) }   // degrees
    var turbidity by remember { mutableFloatStateOf(2.5f) }      // aerosol density

    // Camera & Viewport
    var camRotX by remember { mutableFloatStateOf(24f) }
    var camRotY by remember { mutableFloatStateOf(-35f) }
    var autoSpin by remember { mutableStateOf(false) }
    var showGrid by remember { mutableStateOf(true) }

    // Control tabs: 0: Dimensions, 1: Sky & Sun, 2: Surface & Color, 3: Native Structure
    var selectedTab by remember { mutableIntStateOf(0) }

    // Auto-spin animation
    val infiniteTransition = rememberInfiniteTransition(label = "spin_loop")
    val spinAngle by infiniteTransition.animateFloat(
        initialValue = 0f,
        targetValue = 360f,
        animationSpec = infiniteRepeatable(
            animation = tween(durationMillis = 14000, easing = LinearEasing),
            repeatMode = RepeatMode.Restart
        ),
        label = "spin_angle"
    )

    val currentYaw = if (autoSpin) (camRotY + spinAngle) % 360f else camRotY

    ModalNavigationDrawer(
        drawerState = drawerState,
        gesturesEnabled = true,
        drawerContent = {
            ModalDrawerSheet(
                modifier = Modifier
                    .width(320.dp)
                    .testTag("asset_navigation_drawer"),
                drawerContainerColor = Color(0xFF131722),
                drawerContentColor = Color(0xFFE2E8F0)
            ) {
                // Drawer Header
                Column(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(24.dp)
                ) {
                    Row(verticalAlignment = Alignment.CenterVertically) {
                        Surface(
                            shape = CircleShape,
                            color = MaterialTheme.colorScheme.primary.copy(alpha = 0.2f),
                            modifier = Modifier.size(44.dp)
                        ) {
                            Box(contentAlignment = Alignment.Center) {
                                Icon(
                                    imageVector = Icons.Default.ViewInAr,
                                    contentDescription = "Studio Emblem",
                                    tint = MaterialTheme.colorScheme.primary,
                                    modifier = Modifier.size(24.dp)
                                )
                            }
                        }
                        Spacer(modifier = Modifier.width(12.dp))
                        Column {
                            Text(
                                text = "Sphere Studio",
                                style = MaterialTheme.typography.titleMedium.copy(
                                    fontWeight = FontWeight.Bold,
                                    letterSpacing = 0.5.sp
                                ),
                                color = Color.White
                            )
                            Text(
                                text = "Native C++ / Vulkan 1.3 Viewport",
                                style = MaterialTheme.typography.bodySmall,
                                color = Color(0xFF94A3B8)
                            )
                        }
                    }
                }

                HorizontalDivider(color = Color(0xFF242938))

                Text(
                    text = "SCENES & PROCEDURAL GENERATORS",
                    style = MaterialTheme.typography.labelSmall.copy(
                        fontWeight = FontWeight.SemiBold,
                        letterSpacing = 1.2.sp
                    ),
                    color = Color(0xFF64748B),
                    modifier = Modifier.padding(horizontal = 24.dp, vertical = 14.dp)
                )

                LazyColumn(
                    modifier = Modifier
                        .fillMaxWidth()
                        .weight(1f)
                        .padding(horizontal = 12.dp)
                ) {
                    items(availableAssetCategories) { category ->
                        val isSelected = category.id == selectedCategory.id
                        NavigationDrawerItem(
                            icon = {
                                Icon(
                                    imageVector = category.icon,
                                    contentDescription = category.title,
                                    tint = if (isSelected) MaterialTheme.colorScheme.primary else Color(0xFF94A3B8)
                                )
                            },
                            label = {
                                Column {
                                    Text(
                                        text = category.title,
                                        style = MaterialTheme.typography.bodyMedium.copy(
                                            fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal
                                        ),
                                        color = if (isSelected) Color.White else Color(0xFFCBD5E1)
                                    )
                                    Text(
                                        text = category.subtitle,
                                        style = MaterialTheme.typography.labelSmall,
                                        color = if (isSelected) MaterialTheme.colorScheme.primary.copy(alpha = 0.85f) else Color(0xFF64748B),
                                        maxLines = 1
                                    )
                                }
                            },
                            selected = isSelected,
                            onClick = {
                                selectedCategory = category
                                coroutineScope.launch { drawerState.close() }
                            },
                            modifier = Modifier
                                .padding(vertical = 2.dp)
                                .testTag("drawer_item_${category.id}"),
                            colors = NavigationDrawerItemDefaults.colors(
                                selectedContainerColor = Color(0xFF1E2638),
                                unselectedContainerColor = Color.Transparent
                            ),
                            shape = RoundedCornerShape(10.dp)
                        )
                    }
                }

                HorizontalDivider(color = Color(0xFF242938))

                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(16.dp),
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text(
                        text = "Native Engine Core",
                        style = MaterialTheme.typography.labelSmall,
                        color = Color(0xFF64748B),
                        fontFamily = FontFamily.Monospace
                    )
                    Badge(
                        containerColor = Color(0xFF0F766E),
                        contentColor = Color(0xFF5EEAD4)
                    ) {
                        Text("Online", fontSize = 10.sp, fontWeight = FontWeight.Bold)
                    }
                }
            }
        }
    ) {
        Scaffold(
            topBar = {
                TopAppBar(
                    title = {
                        Column {
                            Row(verticalAlignment = Alignment.CenterVertically) {
                                Text(
                                    text = selectedCategory.title,
                                    style = MaterialTheme.typography.titleLarge.copy(
                                        fontWeight = FontWeight.Bold
                                    ),
                                    color = Color.White,
                                    modifier = Modifier.testTag("active_asset_title")
                                )
                                Spacer(modifier = Modifier.width(8.dp))
                                Surface(
                                    shape = RoundedCornerShape(4.dp),
                                    color = Color(0xFF1E293B)
                                ) {
                                    Text(
                                        text = "Common Scene",
                                        style = MaterialTheme.typography.labelSmall,
                                        color = Color(0xFF38BDF8),
                                        modifier = Modifier.padding(horizontal = 6.dp, vertical = 2.dp)
                                    )
                                }
                            }
                            Text(
                                text = selectedCategory.subtitle,
                                style = MaterialTheme.typography.bodySmall,
                                color = Color(0xFF94A3B8)
                            )
                        }
                    },
                    navigationIcon = {
                        IconButton(
                            onClick = { coroutineScope.launch { drawerState.open() } },
                            modifier = Modifier.testTag("hamburger_menu_button")
                        ) {
                            Icon(
                                imageVector = Icons.Default.Menu,
                                contentDescription = "Open Asset Menu",
                                tint = Color.White
                            )
                        }
                    },
                    actions = {
                        IconButton(
                            onClick = {
                                camRotX = 24f
                                camRotY = -35f
                                cubeWidth = 1.6f
                                cubeHeight = 1.6f
                                cubeDepth = 1.6f
                                cubeScale = 1.0f
                                sunElevation = 32.0f
                                sunAzimuth = 145.0f
                                autoSpin = false
                                enableSky = true
                                enableSun = true
                            },
                            modifier = Modifier.testTag("reset_view_button")
                        ) {
                            Icon(
                                imageVector = Icons.Default.Refresh,
                                contentDescription = "Reset Scene & Atmosphere",
                                tint = Color(0xFF94A3B8)
                            )
                        }
                    },
                    colors = TopAppBarDefaults.topAppBarColors(
                        containerColor = Color(0xFF0F1219)
                    )
                )
            },
            containerColor = Color(0xFF0B0D13)
        ) { innerPadding ->
            Column(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(innerPadding)
                    .background(Color(0xFF0B0D13))
            ) {
                // 1. TOP VIEWPORT: 3D COMMON SCENE WITH CUBE, SKY, SUN & HORIZON
                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .weight(1.08f)
                        .background(Color(0xFF0B0D13))
                ) {
                    SceneCubeViewport(
                        modifier = Modifier.fillMaxSize(),
                        width = cubeWidth * cubeScale,
                        height = cubeHeight * cubeScale,
                        depth = cubeDepth * cubeScale,
                        baseColor = selectedColor.color,
                        pattern = selectedPattern,
                        rotX = camRotX,
                        rotY = currentYaw,
                        showGrid = showGrid,
                        enableSky = enableSky,
                        enableSun = enableSun,
                        sunElevation = sunElevation,
                        sunAzimuth = sunAzimuth,
                        turbidity = turbidity,
                        onDrag = { dx, dy ->
                            if (!autoSpin) {
                                camRotY = (camRotY + dx * 0.45f) % 360f
                            }
                            camRotX = (camRotX - dy * 0.45f).coerceIn(-85f, 85f)
                        }
                    )

                    // Top Viewport Floating Badges & Controls
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(12.dp),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Surface(
                            shape = RoundedCornerShape(8.dp),
                            color = Color(0xFF131722).copy(alpha = 0.85f),
                            border = CardDefaults.outlinedCardBorder().copy(
                                brush = Brush.linearGradient(listOf(Color(0xFF1E293B), Color(0xFF0F172A)))
                            )
                        ) {
                            Row(
                                modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                                verticalAlignment = Alignment.CenterVertically
                            ) {
                                if (enableSun) {
                                    Icon(
                                        imageVector = Icons.Default.WbSunny,
                                        contentDescription = null,
                                        tint = Color(0xFFFBBF24),
                                        modifier = Modifier.size(14.dp)
                                    )
                                    Spacer(modifier = Modifier.width(4.dp))
                                }
                                Text(
                                    text = if (enableSky) "Sky Active • Sun ${sunElevation.toInt()}°" else "Studio Dark • Sky Off",
                                    style = MaterialTheme.typography.labelSmall,
                                    color = if (enableSky) Color(0xFF38BDF8) else Color(0xFF94A3B8),
                                    fontFamily = FontFamily.Monospace
                                )
                            }
                        }

                        // On-scene quick action toggles
                        Row(verticalAlignment = Alignment.CenterVertically) {
                            // Sky Toggle Pill Button
                            Surface(
                                shape = RoundedCornerShape(8.dp),
                                color = if (enableSky) Color(0xFF0284C7).copy(alpha = 0.35f) else Color(0xFF131722).copy(alpha = 0.85f),
                                border = CardDefaults.outlinedCardBorder().copy(
                                    brush = Brush.linearGradient(listOf(Color(0xFF1E293B), Color(0xFF334155)))
                                ),
                                modifier = Modifier
                                    .clickable { enableSky = !enableSky }
                                    .padding(end = 6.dp)
                            ) {
                                Text(
                                    text = if (enableSky) "Sky: ON" else "Sky: OFF",
                                    style = MaterialTheme.typography.labelSmall,
                                    color = if (enableSky) Color(0xFF38BDF8) else Color(0xFF94A3B8),
                                    fontWeight = FontWeight.Bold,
                                    modifier = Modifier.padding(horizontal = 8.dp, vertical = 6.dp)
                                )
                            }

                            // Sun Toggle Pill Button
                            Surface(
                                shape = RoundedCornerShape(8.dp),
                                color = if (enableSun) Color(0xFFF59E0B).copy(alpha = 0.3f) else Color(0xFF131722).copy(alpha = 0.85f),
                                border = CardDefaults.outlinedCardBorder().copy(
                                    brush = Brush.linearGradient(listOf(Color(0xFF1E293B), Color(0xFF334155)))
                                ),
                                modifier = Modifier
                                    .clickable { enableSun = !enableSun }
                                    .padding(end = 6.dp)
                            ) {
                                Text(
                                    text = if (enableSun) "Sun: ON" else "Sun: OFF",
                                    style = MaterialTheme.typography.labelSmall,
                                    color = if (enableSun) Color(0xFFFBBF24) else Color(0xFF94A3B8),
                                    fontWeight = FontWeight.Bold,
                                    modifier = Modifier.padding(horizontal = 8.dp, vertical = 6.dp)
                                )
                            }

                            // Auto Spin
                            IconButton(
                                onClick = { autoSpin = !autoSpin },
                                modifier = Modifier
                                    .size(32.dp)
                                    .background(if (autoSpin) Color(0xFF0284C7).copy(alpha = 0.3f) else Color(0xFF131722).copy(alpha = 0.85f), CircleShape)
                                    .border(1.dp, Color(0xFF1E293B), CircleShape)
                            ) {
                                Icon(
                                    imageVector = if (autoSpin) Icons.Default.Stop else Icons.Default.PlayArrow,
                                    contentDescription = "Toggle Auto-Spin",
                                    tint = if (autoSpin) Color(0xFF38BDF8) else Color(0xFF94A3B8),
                                    modifier = Modifier.size(15.dp)
                                )
                            }
                        }
                    }
                }

                HorizontalDivider(color = Color(0xFF1E293B), thickness = 1.dp)

                // 2. BOTTOM CONTROLS PANEL (Dimensions, Sky & Sun, Color & Surface, Native Structure)
                Surface(
                    modifier = Modifier
                        .fillMaxWidth()
                        .weight(0.92f),
                    color = Color(0xFF0F121A)
                ) {
                    Column(modifier = Modifier.fillMaxSize()) {
                        TabRow(
                            selectedTabIndex = selectedTab,
                            containerColor = Color(0xFF131722),
                            contentColor = Color.White,
                            indicator = { tabPositions ->
                                TabRowDefaults.SecondaryIndicator(
                                    Modifier.tabIndicatorOffset(tabPositions[selectedTab]),
                                    color = MaterialTheme.colorScheme.primary,
                                    height = 3.dp
                                )
                            }
                        ) {
                            Tab(
                                selected = selectedTab == 0,
                                onClick = { selectedTab = 0 },
                                text = { Text("Dimensions", fontSize = 12.sp, fontWeight = FontWeight.SemiBold) },
                                icon = { Icon(Icons.Outlined.SquareFoot, contentDescription = null, modifier = Modifier.size(16.dp)) }
                            )
                            Tab(
                                selected = selectedTab == 1,
                                onClick = { selectedTab = 1 },
                                text = { Text("Sky & Sun", fontSize = 12.sp, fontWeight = FontWeight.SemiBold) },
                                icon = { Icon(Icons.Outlined.WbSunny, contentDescription = null, modifier = Modifier.size(16.dp)) }
                            )
                            Tab(
                                selected = selectedTab == 2,
                                onClick = { selectedTab = 2 },
                                text = { Text("Surface", fontSize = 12.sp, fontWeight = FontWeight.SemiBold) },
                                icon = { Icon(Icons.Outlined.Texture, contentDescription = null, modifier = Modifier.size(16.dp)) }
                            )
                            Tab(
                                selected = selectedTab == 3,
                                onClick = { selectedTab = 3 },
                                text = { Text("Native Core", fontSize = 12.sp, fontWeight = FontWeight.SemiBold) },
                                icon = { Icon(Icons.Default.Code, contentDescription = null, modifier = Modifier.size(16.dp)) }
                            )
                        }

                        // Tab Body
                        Column(
                            modifier = Modifier
                                .fillMaxSize()
                                .verticalScroll(rememberScrollState())
                                .padding(16.dp)
                        ) {
                            when (selectedTab) {
                                0 -> DimensionsControlTab(
                                    width = cubeWidth,
                                    height = cubeHeight,
                                    depth = cubeDepth,
                                    scale = cubeScale,
                                    onWidthChange = { cubeWidth = it },
                                    onHeightChange = { cubeHeight = it },
                                    onDepthChange = { cubeDepth = it },
                                    onScaleChange = { cubeScale = it },
                                    onReset = {
                                        cubeWidth = 1.6f
                                        cubeHeight = 1.6f
                                        cubeDepth = 1.6f
                                        cubeScale = 1.0f
                                    }
                                )
                                1 -> SkyAndSunTab(
                                    enableSky = enableSky,
                                    onToggleSky = { enableSky = it },
                                    enableSun = enableSun,
                                    onToggleSun = { enableSun = it },
                                    sunElevation = sunElevation,
                                    onElevationChange = { sunElevation = it },
                                    sunAzimuth = sunAzimuth,
                                    onAzimuthChange = { sunAzimuth = it },
                                    turbidity = turbidity,
                                    onTurbidityChange = { turbidity = it },
                                    onSelectPreset = { preset ->
                                        sunElevation = preset.elevation
                                        sunAzimuth = preset.azimuth
                                        turbidity = preset.turbidity
                                        enableSky = true
                                        enableSun = preset.elevation >= 0f
                                    }
                                )
                                2 -> SurfaceAndColorTab(
                                    selectedColor = selectedColor,
                                    onColorSelect = { selectedColor = it },
                                    selectedPattern = selectedPattern,
                                    onPatternSelect = { selectedPattern = it }
                                )
                                3 -> NativeStructureTab()
                            }
                        }
                    }
                }
            }
        }
    }
}

// -------------------------------------------------------------
// CONTROL TAB 1: SKY & SUN (ATMOSPHERE)
// -------------------------------------------------------------
@Composable
fun SkyAndSunTab(
    enableSky: Boolean,
    onToggleSky: (Boolean) -> Unit,
    enableSun: Boolean,
    onToggleSun: (Boolean) -> Unit,
    sunElevation: Float,
    onElevationChange: (Float) -> Unit,
    sunAzimuth: Float,
    onAzimuthChange: (Float) -> Unit,
    turbidity: Float,
    onTurbidityChange: (Float) -> Unit,
    onSelectPreset: (TimeOfDayPreset) -> Unit
) {
    Column {
        // Toggles Card
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column {
                        Text(
                            text = "Atmospheric Sky Dome",
                            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                            color = Color.White
                        )
                        Text(
                            text = "Preetham / Hosek-Wilkie Rayleigh scattering",
                            style = MaterialTheme.typography.bodySmall,
                            color = Color(0xFF64748B)
                        )
                    }
                    Switch(
                        checked = enableSky,
                        onCheckedChange = onToggleSky,
                        colors = SwitchDefaults.colors(
                            checkedThumbColor = Color(0xFF38BDF8),
                            checkedTrackColor = Color(0xFF0369A1)
                        )
                    )
                }

                HorizontalDivider(color = Color(0xFF1E293B), modifier = Modifier.padding(vertical = 10.dp))

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column {
                        Text(
                            text = "Sun Disk & Corona Glow",
                            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                            color = Color.White
                        )
                        Text(
                            text = "Direct solar disk, limb darkening & Mie flare",
                            style = MaterialTheme.typography.bodySmall,
                            color = Color(0xFF64748B)
                        )
                    }
                    Switch(
                        checked = enableSun,
                        onCheckedChange = onToggleSun,
                        colors = SwitchDefaults.colors(
                            checkedThumbColor = Color(0xFFFBBF24),
                            checkedTrackColor = Color(0xFFB45309)
                        )
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(16.dp))

        Text(
            text = "Time of Day Solar Presets",
            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
            color = Color.White
        )
        Spacer(modifier = Modifier.height(8.dp))

        Row(
            modifier = Modifier
                .fillMaxWidth()
                .horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            timePresets.forEach { preset ->
                Surface(
                    shape = RoundedCornerShape(10.dp),
                    color = Color(0xFF131722),
                    border = CardDefaults.outlinedCardBorder().copy(
                        brush = Brush.linearGradient(listOf(Color(0xFF1E293B), Color(0xFF334155)))
                    ),
                    modifier = Modifier.clickable { onSelectPreset(preset) }
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 12.dp, vertical = 8.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Icon(
                            imageVector = preset.icon,
                            contentDescription = null,
                            tint = Color(0xFFFBBF24),
                            modifier = Modifier.size(16.dp)
                        )
                        Spacer(modifier = Modifier.width(6.dp))
                        Text(
                            text = preset.name,
                            style = MaterialTheme.typography.labelSmall,
                            color = Color.White,
                            fontWeight = FontWeight.SemiBold
                        )
                    }
                }
            }
        }

        Spacer(modifier = Modifier.height(16.dp))

        // Sun Elevation Slider
        DimensionSlider(
            label = "Sun Elevation (Pitch Angle)",
            value = sunElevation,
            range = -10.0f..90.0f,
            unit = "°",
            accentColor = Color(0xFFFBBF24),
            onValueChange = onElevationChange
        )

        // Sun Azimuth Slider
        DimensionSlider(
            label = "Sun Azimuth (Horizontal Angle)",
            value = sunAzimuth,
            range = 0.0f..360.0f,
            unit = "°",
            accentColor = Color(0xFF38BDF8),
            onValueChange = onAzimuthChange
        )

        // Turbidity / Haze
        DimensionSlider(
            label = "Turbidity (Aerosol Haze Density)",
            value = turbidity,
            range = 1.0f..8.0f,
            unit = "",
            accentColor = Color(0xFFA855F7),
            onValueChange = onTurbidityChange
        )
    }
}

// -------------------------------------------------------------
// CONTROL TAB 0: DIMENSIONS
// -------------------------------------------------------------
@Composable
fun DimensionsControlTab(
    width: Float,
    height: Float,
    depth: Float,
    scale: Float,
    onWidthChange: (Float) -> Unit,
    onHeightChange: (Float) -> Unit,
    onDepthChange: (Float) -> Unit,
    onScaleChange: (Float) -> Unit,
    onReset: () -> Unit
) {
    Column {
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                text = "Geometry Dimensions",
                style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                color = Color.White
            )
            Surface(
                shape = RoundedCornerShape(6.dp),
                color = Color(0xFF1E293B),
                modifier = Modifier.clickable { onReset() }
            ) {
                Text(
                    text = "Reset Default",
                    style = MaterialTheme.typography.labelSmall,
                    color = Color(0xFF38BDF8),
                    modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp)
                )
            }
        }

        Spacer(modifier = Modifier.height(8.dp))

        DimensionSlider(
            label = "Width (X-Axis)",
            value = width,
            range = 0.4f..3.2f,
            unit = "m",
            accentColor = Color(0xFFEF4444),
            onValueChange = onWidthChange
        )

        DimensionSlider(
            label = "Height (Y-Axis)",
            value = height,
            range = 0.4f..3.2f,
            unit = "m",
            accentColor = Color(0xFF10B981),
            onValueChange = onHeightChange
        )

        DimensionSlider(
            label = "Depth (Z-Axis)",
            value = depth,
            range = 0.4f..3.2f,
            unit = "m",
            accentColor = Color(0xFF3B82F6),
            onValueChange = onDepthChange
        )

        DimensionSlider(
            label = "Overall Scale",
            value = scale,
            range = 0.5f..2.0f,
            unit = "x",
            accentColor = Color(0xFFF59E0B),
            onValueChange = onScaleChange
        )
    }
}

@Composable
fun DimensionSlider(
    label: String,
    value: Float,
    range: ClosedFloatingPointRange<Float>,
    unit: String,
    accentColor: Color,
    onValueChange: (Float) -> Unit
) {
    Column(modifier = Modifier.padding(vertical = 4.dp)) {
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween
        ) {
            Text(
                text = label,
                style = MaterialTheme.typography.bodySmall,
                color = Color(0xFFCBD5E1)
            )
            Text(
                text = "${String.format("%.2f", value)} $unit",
                style = MaterialTheme.typography.labelSmall,
                color = accentColor,
                fontFamily = FontFamily.Monospace,
                fontWeight = FontWeight.Bold
            )
        }
        Slider(
            value = value,
            onValueChange = onValueChange,
            valueRange = range,
            colors = SliderDefaults.colors(
                thumbColor = accentColor,
                activeTrackColor = accentColor,
                inactiveTrackColor = Color(0xFF1E293B)
            ),
            modifier = Modifier.height(30.dp)
        )
    }
}

// -------------------------------------------------------------
// CONTROL TAB 2: SURFACE & COLOR
// -------------------------------------------------------------
@Composable
fun SurfaceAndColorTab(
    selectedColor: ColorOption,
    onColorSelect: (ColorOption) -> Unit,
    selectedPattern: SurfacePattern,
    onPatternSelect: (SurfacePattern) -> Unit
) {
    Column {
        Text(
            text = "Surface Colour Palette",
            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
            color = Color.White
        )
        Spacer(modifier = Modifier.height(10.dp))

        Row(
            modifier = Modifier
                .fillMaxWidth()
                .horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(10.dp)
        ) {
            paletteColors.forEach { opt ->
                val isSelected = opt.name == selectedColor.name
                Column(
                    horizontalAlignment = Alignment.CenterHorizontally,
                    modifier = Modifier
                        .clickable { onColorSelect(opt) }
                        .padding(2.dp)
                ) {
                    Box(
                        modifier = Modifier
                            .size(38.dp)
                            .clip(CircleShape)
                            .background(opt.color)
                            .border(
                                width = if (isSelected) 3.dp else 1.dp,
                                color = if (isSelected) Color.White else Color(0xFF334155),
                                shape = CircleShape
                            )
                    )
                    Spacer(modifier = Modifier.height(4.dp))
                    Text(
                        text = opt.name.split(" ").last(),
                        style = MaterialTheme.typography.labelSmall,
                        color = if (isSelected) Color.White else Color(0xFF64748B),
                        fontSize = 10.sp
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(20.dp))

        Text(
            text = "Surface Procedural Pattern",
            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
            color = Color.White
        )
        Spacer(modifier = Modifier.height(10.dp))

        Row(
            modifier = Modifier
                .fillMaxWidth()
                .horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            SurfacePattern.values().forEach { pattern ->
                val isSelected = pattern == selectedPattern
                FilterChip(
                    selected = isSelected,
                    onClick = { onPatternSelect(pattern) },
                    label = { Text(pattern.label, fontSize = 12.sp) },
                    colors = FilterChipDefaults.filterChipColors(
                        selectedContainerColor = Color(0xFF0284C7).copy(alpha = 0.35f),
                        selectedLabelColor = Color(0xFF38BDF8),
                        containerColor = Color(0xFF131722),
                        labelColor = Color(0xFF94A3B8)
                    ),
                    border = FilterChipDefaults.filterChipBorder(
                        borderColor = if (isSelected) Color(0xFF0284C7) else Color(0xFF1E293B),
                        selectedBorderColor = Color(0xFF38BDF8),
                        enabled = true,
                        selected = isSelected
                    )
                )
            }
        }
    }
}

// -------------------------------------------------------------
// CONTROL TAB 3: NATIVE STRUCTURE INSPECTOR
// -------------------------------------------------------------
@Composable
fun NativeStructureTab() {
    Column {
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                text = "Native Engine Architecture",
                style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                color = Color.White
            )
            Surface(
                shape = RoundedCornerShape(6.dp),
                color = Color(0xFF064E3B)
            ) {
                Row(
                    modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Icon(
                        imageVector = Icons.Default.CheckCircle,
                        contentDescription = null,
                        tint = Color(0xFF34D399),
                        modifier = Modifier.size(12.dp)
                    )
                    Spacer(modifier = Modifier.width(4.dp))
                    Text(
                        text = "Vulkan 1.3 Verified",
                        style = MaterialTheme.typography.labelSmall,
                        color = Color(0xFF6EE7B7),
                        fontWeight = FontWeight.Bold
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(10.dp))

        val nativeFiles = listOf(
            Triple("Sky & Atmosphere Core", "src/scene/SkyModel.cpp", "Preetham / Hosek-Wilkie scattering model & solar extinction"),
            Triple("Sun Disk & Corona", "src/sky/sun/SunDisk.cpp", "Direct solar disk, limb darkening & Mie forward halo"),
            Triple("Atmospheric Perez", "src/sky/atmosphere/PreethamModel.cpp", "All-weather daylight spectrum & zenith luminance"),
            Triple("Common Scene", "src/scene/Scene.cpp", "Root viewport container holding Camera, SkyModel, Light & Nodes"),
            Triple("Camera & Controls", "src/scene/Camera.cpp", "Perspective projection, reverse-Z & orbit controllers"),
            Triple("Vulkan Device & Allocator", "base/VulkanDevice.cpp", "Physical/logical device, queue families & staging buffer pool"),
            Triple("Bindless Heap", "src/vulkan/BindlessHeap.cpp", "VK_EXT_descriptor_indexing unbounded descriptor heap"),
            Triple("Pipeline Builder", "src/vulkan/PipelineBuilder.cpp", "Vulkan 1.3 dynamic rendering graphics & compute pipelines"),
            Triple("Sky Shaders", "shaders/glsl/sky.comp", "GLSL 460 compute shader evaluating atmospheric LUT")
        )

        nativeFiles.forEach { (name, path, desc) ->
            Card(
                colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
                shape = RoundedCornerShape(8.dp),
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(vertical = 4.dp)
            ) {
                Column(modifier = Modifier.padding(12.dp)) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(
                            text = name,
                            style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Bold),
                            color = Color.White
                        )
                        Text(
                            text = path,
                            style = MaterialTheme.typography.labelSmall,
                            color = Color(0xFF38BDF8),
                            fontFamily = FontFamily.Monospace,
                            fontSize = 10.sp
                        )
                    }
                    Spacer(modifier = Modifier.height(2.dp))
                    Text(
                        text = desc,
                        style = MaterialTheme.typography.labelSmall,
                        color = Color(0xFF64748B)
                    )
                }
            }
        }
    }
}

// -------------------------------------------------------------
// 3D PERSPECTIVE CANVAS WITH CUBE, ATMOSPHERIC SKY & SUN
// -------------------------------------------------------------
data class Vec3D(val x: Float, val y: Float, val z: Float)
data class Point2D(val x: Float, val y: Float, val depth: Float)

data class FaceQuad(
    val p0: Point2D,
    val p1: Point2D,
    val p2: Point2D,
    val p3: Point2D,
    val avgDepth: Float,
    val normal: Vec3D,
    val faceIndex: Int
)

@Composable
fun SceneCubeViewport(
    modifier: Modifier = Modifier,
    width: Float,
    height: Float,
    depth: Float,
    baseColor: Color,
    pattern: SurfacePattern,
    rotX: Float,
    rotY: Float,
    showGrid: Boolean,
    enableSky: Boolean,
    enableSun: Boolean,
    sunElevation: Float,
    sunAzimuth: Float,
    turbidity: Float,
    onDrag: (Float, Float) -> Unit
) {
    Canvas(
        modifier = modifier
            .testTag("scene_cube_viewport")
            .pointerInput(Unit) {
                detectDragGestures { change, dragAmount ->
                    change.consume()
                    onDrag(dragAmount.x, dragAmount.y)
                }
            }
    ) {
        val w = size.width
        val h = size.height
        val cx = w / 2f
        val cy = h / 2f - 15f

        // 1. SKY AND HORIZON EVALUATION (Preetham / Hosek atmospheric model)
        val radY = Math.toRadians(rotY.toDouble()).toFloat()
        val radX = Math.toRadians(rotX.toDouble()).toFloat()
        val cosY = cos(radY)
        val sinY = sin(radY)
        val cosX = cos(radX)
        val sinX = sin(radX)

        val elRad = Math.toRadians(sunElevation.toDouble()).toFloat()
        val azRad = Math.toRadians(sunAzimuth.toDouble()).toFloat()

        // 3D Sun direction vector in world space
        val sunDir = Vec3D(
            x = cos(elRad) * sin(azRad),
            y = sin(elRad),
            z = cos(elRad) * cos(azRad)
        )

        // Horizon position in viewport screen space (shifts with camera pitch rotX)
        val horizonY = cy + 45f + (sinX * 180f)

        if (enableSky) {
            // Atmospheric Scattering Gradient
            // Colors shift dynamically from zenith down to horizon, modified by sun elevation & turbidity
            val zenithColor: Color
            val horizonColor: Color

            if (sunElevation > 20.0f) {
                // Daytime brilliant blue atmosphere
                val factor = (sunElevation - 20.0f) / 70.0f
                zenithColor = Color(
                    red = (0.10f + 0.05f * factor).coerceIn(0f, 1f),
                    green = (0.35f + 0.15f * factor).coerceIn(0f, 1f),
                    blue = (0.78f + 0.18f * factor).coerceIn(0f, 1f)
                )
                horizonColor = Color(
                    red = (0.55f + 0.15f * (turbidity / 10f)).coerceIn(0f, 1f),
                    green = (0.70f + 0.15f * (turbidity / 10f)).coerceIn(0f, 1f),
                    blue = (0.85f + 0.10f * (turbidity / 10f)).coerceIn(0f, 1f)
                )
            } else if (sunElevation >= 0.0f) {
                // Golden Hour / Sunset warm atmospheric scattering
                val sunsetRatio = 1.0f - (sunElevation / 20.0f)
                zenithColor = Color(
                    red = 0.18f + 0.15f * sunsetRatio,
                    green = 0.28f + 0.05f * sunsetRatio,
                    blue = 0.65f - 0.25f * sunsetRatio
                )
                horizonColor = Color(
                    red = 0.85f + 0.12f * sunsetRatio,
                    green = 0.52f - 0.12f * sunsetRatio,
                    blue = 0.22f - 0.10f * sunsetRatio
                )
            } else {
                // Dusk / Night celestial indigo
                val nightRatio = (-sunElevation / 10.0f).coerceIn(0f, 1f)
                zenithColor = Color(0xFF04060C)
                horizonColor = Color(
                    red = (0.22f * (1f - nightRatio) + 0.05f).coerceIn(0f, 1f),
                    green = (0.15f * (1f - nightRatio) + 0.07f).coerceIn(0f, 1f),
                    blue = (0.35f * (1f - nightRatio) + 0.12f).coerceIn(0f, 1f)
                )
            }

            // Draw Atmospheric Sky Dome gradient (from top of screen down to horizon)
            drawRect(
                brush = Brush.verticalGradient(
                    colorStops = arrayOf(
                        0.0f to zenithColor,
                        0.75f to horizonColor,
                        1.0f to horizonColor.copy(alpha = 0.9f)
                    ),
                    startY = 0f,
                    endY = horizonY.coerceAtLeast(10f)
                ),
                topLeft = Offset(0f, 0f),
                size = Size(w, horizonY.coerceAtLeast(10f))
            )

            // Ground floor atmospheric haze (below horizon)
            drawRect(
                brush = Brush.verticalGradient(
                    colors = listOf(
                        horizonColor.copy(alpha = 0.45f),
                        Color(0xFF0E121B),
                        Color(0xFF08090D)
                    ),
                    startY = horizonY,
                    endY = h
                ),
                topLeft = Offset(0f, horizonY),
                size = Size(w, (h - horizonY).coerceAtLeast(0f))
            )

            // 2. SUN DISK & ATMOSPHERIC CORONA (projected into 2D viewport)
            if (enableSun && sunElevation > -2.0f) {
                // Transform sun direction by camera rotation
                val sunX1 = sunDir.x * cosY + sunDir.z * sinY
                val sunZ1 = -sunDir.x * sinY + sunDir.z * cosY
                val sunY2 = sunDir.y * cosX - sunZ1 * sinX
                val sunZ2 = sunDir.y * sinX + sunZ1 * cosX

                // If sun is in front of camera
                if (sunZ2 > -0.2f) {
                    val sunProjDist = 600f
                    val sunPx = cx + (sunX1 / (sunZ2 + 2.5f)) * sunProjDist
                    val sunPy = cy - (sunY2 / (sunZ2 + 2.5f)) * sunProjDist

                    // Sun Corona Glow (Radial bloom)
                    val coronaRadius = 140f
                    val sunColorRGB = if (sunElevation < 15f) Color(0xFFF97316) else Color(0xFFFDE047)

                    drawCircle(
                        brush = Brush.radialGradient(
                            colors = listOf(
                                sunColorRGB.copy(alpha = 0.45f),
                                sunColorRGB.copy(alpha = 0.18f),
                                Color.Transparent
                            ),
                            center = Offset(sunPx, sunPy),
                            radius = coronaRadius
                        ),
                        radius = coronaRadius,
                        center = Offset(sunPx, sunPy)
                    )

                    // Core Solar Disk (Bright high-intensity disk)
                    drawCircle(
                        color = Color.White,
                        radius = 18f,
                        center = Offset(sunPx, sunPy)
                    )
                    drawCircle(
                        color = sunColorRGB.copy(alpha = 0.85f),
                        radius = 21f,
                        center = Offset(sunPx, sunPy),
                        style = Stroke(width = 3f)
                    )
                }
            }
        } else {
            // Studio Dark mode background
            drawRect(
                brush = Brush.verticalGradient(
                    colors = listOf(
                        Color(0xFF090B10),
                        Color(0xFF10141F),
                        Color(0xFF080A0E)
                    )
                )
            )
        }

        // 3. PERSPECTIVE GROUND GRID
        if (showGrid) {
            val gridColor = if (enableSky) Color(0xFF38BDF8).copy(alpha = 0.22f) else Color(0xFF1E293B).copy(alpha = 0.4f)
            val gridAxisColor = if (enableSky) Color(0xFF38BDF8).copy(alpha = 0.6f) else Color(0xFF0284C7).copy(alpha = 0.6f)
            val dashEffect = PathEffect.dashPathEffect(floatArrayOf(4f, 4f), 0f)

            val lines = 12
            val spacing = w / (lines + 2)

            for (i in -lines / 2..lines / 2) {
                val offset = i * spacing
                val isCenter = i == 0
                val pY = cy + 130f

                drawLine(
                    color = if (isCenter) gridAxisColor else gridColor,
                    start = Offset(cx + offset * 0.4f, pY - 50f),
                    end = Offset(cx + offset * 2.1f, h + 20f),
                    strokeWidth = if (isCenter) 2f else 1f,
                    pathEffect = if (!isCenter) dashEffect else null
                )
            }

            for (j in 1..6) {
                val lineY = cy + 60f + (j * j * 9f)
                if (lineY <= h) {
                    drawLine(
                        color = gridColor,
                        start = Offset(0f, lineY),
                        end = Offset(w, lineY),
                        strokeWidth = 1f
                    )
                }
            }
        }

        // Soft ground shadow for the cube
        val shadowRadiusX = (width * 58f).coerceAtLeast(30f)
        val shadowRadiusY = (depth * 24f).coerceAtLeast(15f)
        val shadowY = cy + (height * 38f) + 40f

        drawOval(
            brush = Brush.radialGradient(
                colors = listOf(Color.Black.copy(alpha = 0.65f), Color.Transparent),
                center = Offset(cx, shadowY),
                radius = shadowRadiusX
            ),
            topLeft = Offset(cx - shadowRadiusX, shadowY - shadowRadiusY),
            size = Size(shadowRadiusX * 2f, shadowRadiusY * 2f)
        )

        // 4. 3D CUBE PROJECTION
        val hw = width * 0.5f
        val hh = height * 0.5f
        val hd = depth * 0.5f

        val rawVertices = listOf(
            Vec3D(-hw,  hh, -hd), // 0: Top Front Left
            Vec3D(-hw, -hh, -hd), // 1: Bottom Front Left
            Vec3D( hw, -hh, -hd), // 2: Bottom Front Right
            Vec3D( hw,  hh, -hd), // 3: Top Front Right
            Vec3D(-hw,  hh,  hd), // 4: Top Back Left
            Vec3D(-hw, -hh,  hd), // 5: Bottom Back Left
            Vec3D( hw, -hh,  hd), // 6: Bottom Back Right
            Vec3D( hw,  hh,  hd)  // 7: Top Back Right
        )

        val cameraDist = 4.8f
        val fovFactor = 540f

        fun projectPoint(v: Vec3D): Point2D {
            val x1 = v.x * cosY + v.z * sinY
            val z1 = -v.x * sinY + v.z * cosY

            val y2 = v.y * cosX - z1 * sinX
            val z2 = v.y * sinX + z1 * cosX

            val zView = z2 + cameraDist
            val px = cx + (x1 / zView) * fovFactor
            val py = cy - (y2 / zView) * fovFactor
            return Point2D(px, py, zView)
        }

        val projected = rawVertices.map { projectPoint(it) }

        val facesData = listOf(
            Triple(listOf(0, 3, 2, 1), Vec3D(0f, 0f, -1f), 0), // Front
            Triple(listOf(7, 4, 5, 6), Vec3D(0f, 0f, 1f), 1),  // Back
            Triple(listOf(4, 7, 3, 0), Vec3D(0f, 1f, 0f), 2),  // Top
            Triple(listOf(1, 2, 6, 5), Vec3D(0f, -1f, 0f), 3), // Bottom
            Triple(listOf(3, 7, 6, 2), Vec3D(1f, 0f, 0f), 4),  // Right
            Triple(listOf(4, 0, 1, 5), Vec3D(-1f, 0f, 0f), 5)  // Left
        )

        // Directional light: If Sun enabled, use Sun Direction! Otherwise standard studio light.
        val lightDir = if (enableSun) sunDir else Vec3D(0.5f, 0.85f, 0.4f)
        val lightLen = sqrt(lightDir.x * lightDir.x + lightDir.y * lightDir.y + lightDir.z * lightDir.z)
        val normLight = Vec3D(lightDir.x / lightLen, lightDir.y / lightLen, lightDir.z / lightLen)

        // Sun color modulation for lighting
        val sunLightTint = if (enableSun && sunElevation < 20f) Color(0xFFFDBA74) else Color.White

        val quads = mutableListOf<FaceQuad>()

        for ((indices, rawNorm, faceIdx) in facesData) {
            val p0 = projected[indices[0]]
            val p1 = projected[indices[1]]
            val p2 = projected[indices[2]]
            val p3 = projected[indices[3]]

            val nx1 = rawNorm.x * cosY + rawNorm.z * sinY
            val nz1 = -rawNorm.x * sinY + rawNorm.z * cosY
            val ny2 = rawNorm.y * cosX - nz1 * sinX
            val nz2 = rawNorm.y * sinX + nz1 * cosX
            val rotNorm = Vec3D(nx1, ny2, nz2)

            val avgDepth = (p0.depth + p1.depth + p2.depth + p3.depth) / 4f

            quads.add(
                FaceQuad(
                    p0 = p0,
                    p1 = p1,
                    p2 = p2,
                    p3 = p3,
                    avgDepth = avgDepth,
                    normal = rotNorm,
                    faceIndex = faceIdx
                )
            )
        }

        quads.sortByDescending { it.avgDepth }

        for (quad in quads) {
            val dot = (quad.normal.x * normLight.x + quad.normal.y * normLight.y + quad.normal.z * normLight.z)
                .coerceAtLeast(0.0f)
            val ambient = if (enableSky) 0.32f else 0.26f
            val diffuse = 0.68f * dot
            val shade = (ambient + diffuse).coerceIn(0.18f, 1.0f)

            val faceColor = Color(
                red = (baseColor.red * sunLightTint.red * shade).coerceIn(0f, 1f),
                green = (baseColor.green * sunLightTint.green * shade).coerceIn(0f, 1f),
                blue = (baseColor.blue * sunLightTint.blue * shade).coerceIn(0f, 1f),
                alpha = 0.95f
            )

            val path = Path().apply {
                moveTo(quad.p0.x, quad.p0.y)
                lineTo(quad.p1.x, quad.p1.y)
                lineTo(quad.p2.x, quad.p2.y)
                lineTo(quad.p3.x, quad.p3.y)
                close()
            }

            drawPath(path = path, color = faceColor)

            when (pattern) {
                SurfacePattern.GRID -> {
                    val lineColor = Color.White.copy(alpha = 0.35f)
                    val mid01 = Offset((quad.p0.x + quad.p1.x) / 2f, (quad.p0.y + quad.p1.y) / 2f)
                    val mid23 = Offset((quad.p2.x + quad.p3.x) / 2f, (quad.p2.y + quad.p3.y) / 2f)
                    val mid12 = Offset((quad.p1.x + quad.p2.x) / 2f, (quad.p1.y + quad.p2.y) / 2f)
                    val mid30 = Offset((quad.p3.x + quad.p0.x) / 2f, (quad.p3.y + quad.p0.y) / 2f)
                    drawLine(lineColor, mid01, mid23, strokeWidth = 1.2f)
                    drawLine(lineColor, mid12, mid30, strokeWidth = 1.2f)
                }
                SurfacePattern.CHECKER -> {
                    val subPath = Path().apply {
                        val center = Offset(
                            (quad.p0.x + quad.p1.x + quad.p2.x + quad.p3.x) / 4f,
                            (quad.p0.y + quad.p1.y + quad.p2.y + quad.p3.y) / 4f
                        )
                        moveTo(quad.p0.x, quad.p0.y)
                        lineTo(center.x, center.y)
                        lineTo(quad.p1.x, quad.p1.y)
                        close()
                        moveTo(quad.p2.x, quad.p2.y)
                        lineTo(center.x, center.y)
                        lineTo(quad.p3.x, quad.p3.y)
                        close()
                    }
                    drawPath(subPath, color = Color.Black.copy(alpha = 0.22f))
                }
                SurfacePattern.RIPPLE -> {
                    val center = Offset(
                        (quad.p0.x + quad.p1.x + quad.p2.x + quad.p3.x) / 4f,
                        (quad.p0.y + quad.p1.y + quad.p2.y + quad.p3.y) / 4f
                    )
                    drawCircle(Color.White.copy(alpha = 0.25f), 22f, center, style = Stroke(width = 1.5f))
                    drawCircle(Color.White.copy(alpha = 0.15f), 42f, center, style = Stroke(width = 1.5f))
                }
                SurfacePattern.HEXAGON -> {
                    val center = Offset(
                        (quad.p0.x + quad.p1.x + quad.p2.x + quad.p3.x) / 4f,
                        (quad.p0.y + quad.p1.y + quad.p2.y + quad.p3.y) / 4f
                    )
                    val spokeColor = Color.White.copy(alpha = 0.28f)
                    drawLine(spokeColor, center, Offset(quad.p0.x, quad.p0.y), strokeWidth = 1f)
                    drawLine(spokeColor, center, Offset(quad.p1.x, quad.p1.y), strokeWidth = 1f)
                    drawLine(spokeColor, center, Offset(quad.p2.x, quad.p2.y), strokeWidth = 1f)
                    drawLine(spokeColor, center, Offset(quad.p3.x, quad.p3.y), strokeWidth = 1f)
                }
                SurfacePattern.VORONOI -> {
                    val center = Offset(
                        (quad.p0.x + quad.p1.x + quad.p2.x + quad.p3.x) / 4f,
                        (quad.p0.y + quad.p1.y + quad.p2.y + quad.p3.y) / 4f
                    )
                    drawCircle(Color.Black.copy(alpha = 0.3f), 14f, center)
                    drawCircle(Color.White.copy(alpha = 0.4f), 4f, center)
                }
                SurfacePattern.SOLID -> {}
            }

            drawPath(
                path = path,
                color = Color.White.copy(alpha = 0.45f),
                style = Stroke(width = 1.5f)
            )
        }

        // Coordinate axes gizmo at top-left
        val axisOrigin = Offset(52f, 52f)
        val axisLen = 28f

        val xEnd = Offset(axisOrigin.x + (cosY * axisLen), axisOrigin.y + (sinY * sinX * axisLen * 0.5f))
        val zEnd = Offset(axisOrigin.x - (sinY * axisLen), axisOrigin.y + (cosY * sinX * axisLen * 0.5f))
        val yEnd = Offset(axisOrigin.x, axisOrigin.y - (cosX * axisLen))

        drawLine(Color(0xFFEF4444), axisOrigin, xEnd, strokeWidth = 2.5f)
        drawLine(Color(0xFF10B981), axisOrigin, yEnd, strokeWidth = 2.5f)
        drawLine(Color(0xFF3B82F6), axisOrigin, zEnd, strokeWidth = 2.5f)
    }
}
