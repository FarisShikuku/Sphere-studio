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
import androidx.activity.compose.BackHandler
import androidx.compose.material.icons.automirrored.filled.ArrowBack
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
import androidx.compose.material3.NavigationBar
import androidx.compose.material3.NavigationBarItem
import androidx.compose.material3.NavigationBarItemDefaults
import androidx.compose.material3.NavigationDrawerItem
import androidx.compose.material3.NavigationDrawerItemDefaults
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Slider
import androidx.compose.material3.SliderDefaults
import androidx.compose.material3.Surface
import androidx.compose.material3.Switch
import androidx.compose.material3.SwitchDefaults
import androidx.compose.material3.ScrollableTabRow
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
import androidx.compose.ui.text.style.TextAlign
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

// Shadow Mapping Filtering Quality (Referencing SaschaWillems/Vulkan shadowmapping)
enum class ShadowQuality(
    val label: String,
    val shortLabel: String,
    val kernelDesc: String,
    val pcfRange: Int
) {
    OFF("Shadows Disabled", "Off", "Direct sun lighting only", 0),
    HARD("Hard Shadow (1-Tap)", "Hard", "Raw depth test (aliased edges)", 0),
    PCF_3X3("PCF 3×3 (Medium)", "PCF 3×3", "9-tap bilinear PCF filter", 1),
    PCF_5X5("PCF 5×5 (Sascha Willems)", "PCF 5×5", "25-tap smooth penumbra kernel", 2),
    CSM_4("Cascaded Shadow Maps", "CSM 4-Split", "4 frustum depth cascade splits", 2)
}

// 2D Monotone Chain Convex Hull algorithm for projected ground shadows
fun computeConvexHull2D(points: List<Vec3D>): List<Vec3D> {
    if (points.size <= 3) return points
    val sorted = points.sortedWith(compareBy({ it.x }, { it.z }))
    val lower = ArrayList<Vec3D>(points.size)
    for (p in sorted) {
        while (lower.size >= 2 && cross2D(lower[lower.size - 2], lower[lower.size - 1], p) <= 0f) {
            lower.removeAt(lower.size - 1)
        }
        lower.add(p)
    }
    val upper = ArrayList<Vec3D>(points.size)
    for (i in sorted.indices.reversed()) {
        val p = sorted[i]
        while (upper.size >= 2 && cross2D(upper[upper.size - 2], upper[upper.size - 1], p) <= 0f) {
            upper.removeAt(upper.size - 1)
        }
        upper.add(p)
    }
    if (lower.isNotEmpty()) lower.removeAt(lower.size - 1)
    if (upper.isNotEmpty()) upper.removeAt(upper.size - 1)
    return lower + upper
}

private fun cross2D(o: Vec3D, a: Vec3D, b: Vec3D): Float {
    return (a.x - o.x) * (b.z - o.z) - (a.z - o.z) * (b.x - o.x)
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

    // Surface Scene Mode & Stage Dimensions
    var surfaceMode by remember { mutableStateOf(SurfaceSceneMode.FLAT_PIECE) }
    var stageWidth by remember { mutableFloatStateOf(10.0f) }
    var stageLength by remember { mutableFloatStateOf(10.0f) }
    var stageHeight by remember { mutableFloatStateOf(0.35f) }

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

    // Shadow Mapping Controls (Sascha Willems / Vulkan architecture)
    var enableShadows by remember { mutableStateOf(true) }
    var shadowQuality by remember { mutableStateOf(ShadowQuality.PCF_5X5) }
    var shadowIntensity by remember { mutableFloatStateOf(0.72f) }
    var shadowSoftness by remember { mutableFloatStateOf(1.8f) }
    var depthBias by remember { mutableFloatStateOf(0.005f) }
    var showShadowMapDebug by remember { mutableStateOf(false) }

    // Control tabs: 0: Dimensions, 1: Sky & Sun, 2: Shadows (Vulkan), 3: Stage & Surface, 4: Native Core
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
                                Row(
                                    modifier = Modifier.fillMaxWidth(),
                                    horizontalArrangement = Arrangement.SpaceBetween,
                                    verticalAlignment = Alignment.CenterVertically
                                ) {
                                    Column(modifier = Modifier.weight(1f)) {
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
                                    if (category.id != "test" && category.id != "sky" && category.id != "weather") {
                                        Surface(
                                            shape = RoundedCornerShape(4.dp),
                                            color = Color(0xFF1E293B)
                                        ) {
                                            Text(
                                                text = "EMPTY",
                                                fontSize = 9.sp,
                                                color = Color(0xFF64748B),
                                                fontWeight = FontWeight.Bold,
                                                modifier = Modifier.padding(horizontal = 5.dp, vertical = 2.dp)
                                            )
                                        }
                                    } else if (category.id == "sky" || category.id == "weather") {
                                        Surface(
                                            shape = RoundedCornerShape(4.dp),
                                            color = Color(0xFF0369A1).copy(alpha = 0.4f)
                                        ) {
                                            Text(
                                                text = "ACTIVE",
                                                fontSize = 9.sp,
                                                color = Color(0xFF38BDF8),
                                                fontWeight = FontWeight.Bold,
                                                modifier = Modifier.padding(horizontal = 5.dp, vertical = 2.dp)
                                            )
                                        }
                                    }
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
                                    color = when (selectedCategory.id) {
                                        "sky" -> Color(0xFF0369A1).copy(alpha = 0.4f)
                                        "test" -> Color(0xFF1E293B)
                                        else -> Color(0xFF334155).copy(alpha = 0.5f)
                                    }
                                ) {
                                    Text(
                                        text = when (selectedCategory.id) {
                                            "sky" -> "Atmosphere & Sky"
                                            "test" -> "Test Screen"
                                            else -> "Empty Directory"
                                        },
                                        style = MaterialTheme.typography.labelSmall,
                                        color = when (selectedCategory.id) {
                                            "sky" -> Color(0xFF38BDF8)
                                            "test" -> Color(0xFF10B981)
                                            else -> Color(0xFF94A3B8)
                                        },
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
                        if (selectedCategory.id == "test") {
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
                        } else {
                            IconButton(
                                onClick = { selectedCategory = availableAssetCategories[0] },
                                modifier = Modifier.testTag("back_to_test_button")
                            ) {
                                Icon(
                                    imageVector = Icons.AutoMirrored.Filled.ArrowBack,
                                    contentDescription = "Back to Test Screen",
                                    tint = Color.White
                                )
                            }
                        }
                    },
                    actions = {
                        if (selectedCategory.id != "test") {
                            IconButton(
                                onClick = { coroutineScope.launch { drawerState.open() } },
                                modifier = Modifier.testTag("menu_action_button")
                            ) {
                                Icon(
                                    imageVector = Icons.Default.Menu,
                                    contentDescription = "Asset Menu",
                                    tint = Color(0xFF94A3B8)
                                )
                            }
                        }
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
            BackHandler(enabled = selectedCategory.id != "test") {
                selectedCategory = availableAssetCategories[0]
            }

            when (selectedCategory.id) {
                "sky" -> {
                    SkyStudioScreen(modifier = Modifier.padding(innerPadding))
                }
                "weather" -> {
                    WeatherStudioScreen(modifier = Modifier.padding(innerPadding))
                }
                "test" -> {
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
                    val itemMaxDim = maxOf(cubeWidth * cubeScale, cubeDepth * cubeScale)
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
                        surfaceMode = surfaceMode,
                        stageWidth = stageWidth,
                        stageLength = stageLength,
                        stageHeight = stageHeight,
                        enableShadows = enableShadows,
                        shadowQuality = shadowQuality,
                        shadowIntensity = shadowIntensity,
                        shadowSoftness = shadowSoftness,
                        depthBias = depthBias,
                        showShadowMapDebug = showShadowMapDebug,
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
                            // Quick Shadow Quality Toggle Pill Button (Sascha Willems reference)
                            Surface(
                                shape = RoundedCornerShape(8.dp),
                                color = if (enableShadows && shadowQuality != ShadowQuality.OFF) Color(0xFF4F46E5).copy(alpha = 0.35f) else Color(0xFF131722).copy(alpha = 0.85f),
                                border = CardDefaults.outlinedCardBorder().copy(
                                    brush = Brush.linearGradient(listOf(Color(0xFF1E293B), Color(0xFF334155)))
                                ),
                                modifier = Modifier
                                    .clickable {
                                        val qualities = listOf(ShadowQuality.PCF_5X5, ShadowQuality.PCF_3X3, ShadowQuality.HARD, ShadowQuality.OFF)
                                        val curIdx = qualities.indexOf(shadowQuality)
                                        val nextQuality = qualities[(curIdx + 1) % qualities.size]
                                        shadowQuality = nextQuality
                                        enableShadows = nextQuality != ShadowQuality.OFF
                                    }
                                    .padding(end = 6.dp)
                                    .testTag("quick_shadow_pill")
                            ) {
                                Text(
                                    text = "Shadow: ${shadowQuality.shortLabel}",
                                    style = MaterialTheme.typography.labelSmall,
                                    color = if (enableShadows && shadowQuality != ShadowQuality.OFF) Color(0xFFA5B4FC) else Color(0xFF94A3B8),
                                    fontWeight = FontWeight.Bold,
                                    modifier = Modifier.padding(horizontal = 8.dp, vertical = 6.dp)
                                )
                            }

                            // Surface Mode Quick Toggle Pill Button
                            Surface(
                                shape = RoundedCornerShape(8.dp),
                                color = Color(0xFF131722).copy(alpha = 0.85f),
                                border = CardDefaults.outlinedCardBorder().copy(
                                    brush = Brush.linearGradient(listOf(Color(0xFF1E293B), Color(0xFF334155)))
                                ),
                                modifier = Modifier
                                    .clickable {
                                        val modes = SurfaceSceneMode.values()
                                        val nextIdx = (modes.indexOf(surfaceMode) + 1) % modes.size
                                        surfaceMode = modes[nextIdx]
                                        if (surfaceMode == SurfaceSceneMode.WATER_BODY) {
                                            stageWidth = 10.0f
                                            stageLength = 10.0f
                                        } else if (surfaceMode == SurfaceSceneMode.FLAT_PIECE) {
                                            if (itemMaxDim <= 1.25f) {
                                                stageWidth = 3.0f
                                                stageLength = 3.0f
                                            } else {
                                                stageWidth = 10.0f
                                                stageLength = 10.0f
                                            }
                                        }
                                    }
                                    .padding(end = 6.dp)
                                    .testTag("quick_surface_mode_pill")
                            ) {
                                Text(
                                    text = "${surfaceMode.icon} ${surfaceMode.shortLabel}" +
                                        if (surfaceMode == SurfaceSceneMode.FLAT_PIECE || surfaceMode == SurfaceSceneMode.WATER_BODY)
                                            " (${stageWidth.toInt()}×${stageLength.toInt()}m)" else "",
                                    style = MaterialTheme.typography.labelSmall,
                                    color = Color(0xFF38BDF8),
                                    fontWeight = FontWeight.Bold,
                                    modifier = Modifier.padding(horizontal = 8.dp, vertical = 6.dp)
                                )
                            }

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

                // 2. BOTTOM CONTROLS PANEL (Dimensions, Sky & Sun, Shadows, Stage & Surface, Native Core)
                Surface(
                    modifier = Modifier
                        .fillMaxWidth()
                        .weight(0.92f),
                    color = Color(0xFF0F121A)
                ) {
                    Column(modifier = Modifier.fillMaxSize()) {
                        ScrollableTabRow(
                            selectedTabIndex = selectedTab,
                            containerColor = Color(0xFF131722),
                            contentColor = Color.White,
                            edgePadding = 12.dp,
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
                                text = { Text("Shadows", fontSize = 12.sp, fontWeight = FontWeight.SemiBold) },
                                icon = { Icon(Icons.Default.Cloud, contentDescription = null, modifier = Modifier.size(16.dp)) }
                            )
                            Tab(
                                selected = selectedTab == 3,
                                onClick = { selectedTab = 3 },
                                text = { Text("Stage & Surface", fontSize = 12.sp, fontWeight = FontWeight.SemiBold) },
                                icon = { Icon(Icons.Outlined.Texture, contentDescription = null, modifier = Modifier.size(16.dp)) }
                            )
                            Tab(
                                selected = selectedTab == 4,
                                onClick = { selectedTab = 4 },
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
                            val activeItemSize = maxOf(cubeWidth * cubeScale, cubeDepth * cubeScale)
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
                                3 -> SurfaceAndStageTab(
                                    surfaceMode = surfaceMode,
                                    onSurfaceModeSelect = { newMode ->
                                        surfaceMode = newMode
                                        if (newMode == SurfaceSceneMode.WATER_BODY) {
                                            stageWidth = 10.0f
                                            stageLength = 10.0f
                                        } else if (newMode == SurfaceSceneMode.FLAT_PIECE) {
                                            if (activeItemSize <= 1.25f) {
                                                stageWidth = 3.0f
                                                stageLength = 3.0f
                                            } else {
                                                stageWidth = 10.0f
                                                stageLength = 10.0f
                                            }
                                        }
                                    },
                                    stageWidth = stageWidth,
                                    onStageWidthChange = { stageWidth = it },
                                    stageLength = stageLength,
                                    onStageLengthChange = { stageLength = it },
                                    stageHeight = stageHeight,
                                    onStageHeightChange = { stageHeight = it },
                                    itemSize = activeItemSize,
                                    onApplyAutoFit = {
                                        if (activeItemSize <= 1.25f) {
                                            stageWidth = 3.0f
                                            stageLength = 3.0f
                                        } else {
                                            val autoVal = ((activeItemSize * 2.5f).coerceIn(3.0f, 35.0f) * 2).toInt() / 2f
                                            stageWidth = autoVal
                                            stageLength = autoVal
                                        }
                                    },
                                    selectedColor = selectedColor,
                                    onColorSelect = { selectedColor = it },
                                    selectedPattern = selectedPattern,
                                    onPatternSelect = { selectedPattern = it }
                                )
                                4 -> NativeStructureTab()
                            }
                        }
                    }
                }
            }
        }
        else -> {
            EmptyAssetScreen(
                    category = selectedCategory,
                    onBackToTest = { selectedCategory = availableAssetCategories[0] },
                    onNavigateToSky = { selectedCategory = availableAssetCategories.first { it.id == "sky" } },
                    onNavigateToWeather = { selectedCategory = availableAssetCategories.first { it.id == "weather" } },
                    modifier = Modifier.padding(innerPadding)
                )
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
// CONTROL TAB 2: SHADOWS & PCF (SASCHA WILLEMS VULKAN ARCHITECTURE)
// -------------------------------------------------------------
@Composable
fun ShadowsControlTab(
    enableShadows: Boolean,
    onToggleShadows: (Boolean) -> Unit,
    shadowQuality: ShadowQuality,
    onShadowQualitySelect: (ShadowQuality) -> Unit,
    shadowIntensity: Float,
    onShadowIntensityChange: (Float) -> Unit,
    shadowSoftness: Float,
    onShadowSoftnessChange: (Float) -> Unit,
    depthBias: Float,
    onDepthBiasChange: (Float) -> Unit,
    showShadowMapDebug: Boolean,
    onToggleShadowMapDebug: (Boolean) -> Unit,
    sunElevation: Float,
    sunAzimuth: Float
) {
    Column {
        // 1. Master Toggles Card
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
                    Column(modifier = Modifier.weight(1f)) {
                        Row(verticalAlignment = Alignment.CenterVertically) {
                            Text(
                                text = "Directional Shadows",
                                style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                                color = Color.White
                            )
                            Spacer(modifier = Modifier.width(6.dp))
                            Surface(
                                shape = RoundedCornerShape(4.dp),
                                color = Color(0xFF4F46E5).copy(alpha = 0.25f)
                            ) {
                                Text(
                                    text = "Vulkan 1.3",
                                    style = MaterialTheme.typography.labelSmall,
                                    color = Color(0xFFA5B4FC),
                                    fontSize = 10.sp,
                                    modifier = Modifier.padding(horizontal = 4.dp, vertical = 2.dp)
                                )
                            }
                        }
                        Text(
                            text = "Offscreen depth pass & light projection (SaschaWillems/Vulkan)",
                            style = MaterialTheme.typography.bodySmall,
                            color = Color(0xFF94A3B8)
                        )
                    }
                    Switch(
                        checked = enableShadows,
                        onCheckedChange = onToggleShadows,
                        colors = SwitchDefaults.colors(
                            checkedThumbColor = Color(0xFFA5B4FC),
                            checkedTrackColor = Color(0xFF4F46E5)
                        )
                    )
                }

                HorizontalDivider(color = Color(0xFF1E293B), modifier = Modifier.padding(vertical = 10.dp))

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column(modifier = Modifier.weight(1f)) {
                        Text(
                            text = "Light-Space Depth HUD",
                            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                            color = Color.White
                        )
                        Text(
                            text = "Mini viewport preview of offscreen depth buffer",
                            style = MaterialTheme.typography.bodySmall,
                            color = Color(0xFF64748B)
                        )
                    }
                    Switch(
                        checked = showShadowMapDebug,
                        onCheckedChange = onToggleShadowMapDebug,
                        colors = SwitchDefaults.colors(
                            checkedThumbColor = Color(0xFF38BDF8),
                            checkedTrackColor = Color(0xFF0284C7)
                        )
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(14.dp))

        // 2. Filter Quality Selector (Hard, PCF 3x3, PCF 5x5, CSM)
        Text(
            text = "Shadow Filter Kernel (Sascha Willems)",
            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
            color = Color.White
        )
        Text(
            text = "Percentage Closer Filtering (PCF) mitigates jagged aliasing by sampling multi-tap depth grids",
            style = MaterialTheme.typography.bodySmall,
            color = Color(0xFF94A3B8)
        )

        Spacer(modifier = Modifier.height(8.dp))

        val qualities = listOf(ShadowQuality.HARD, ShadowQuality.PCF_3X3, ShadowQuality.PCF_5X5, ShadowQuality.CSM_4)
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            qualities.forEach { q ->
                val isSelected = shadowQuality == q && enableShadows
                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = if (isSelected) Color(0xFF4F46E5).copy(alpha = 0.35f) else Color(0xFF131722),
                    border = CardDefaults.outlinedCardBorder().copy(
                        brush = Brush.linearGradient(
                            if (isSelected) listOf(Color(0xFF818CF8), Color(0xFF6366F1))
                            else listOf(Color(0xFF1E293B), Color(0xFF334155))
                        )
                    ),
                    modifier = Modifier
                        .weight(1f)
                        .clickable {
                            onShadowQualitySelect(q)
                            if (!enableShadows) onToggleShadows(true)
                        }
                ) {
                    Column(
                        modifier = Modifier.padding(horizontal = 6.dp, vertical = 8.dp),
                        horizontalAlignment = Alignment.CenterHorizontally
                    ) {
                        Text(
                            text = q.shortLabel,
                            style = MaterialTheme.typography.labelSmall,
                            fontWeight = FontWeight.Bold,
                            color = if (isSelected) Color(0xFFA5B4FC) else Color(0xFFCBD5E1),
                            textAlign = TextAlign.Center
                        )
                        Text(
                            text = when (q) {
                                ShadowQuality.HARD -> "1-Tap"
                                ShadowQuality.PCF_3X3 -> "9-Tap"
                                ShadowQuality.PCF_5X5 -> "25-Tap"
                                ShadowQuality.CSM_4 -> "4-Cascade"
                                ShadowQuality.OFF -> "0-Tap"
                            },
                            style = MaterialTheme.typography.labelSmall,
                            fontSize = 9.sp,
                            color = if (isSelected) Color(0xFFC7D2FE) else Color(0xFF64748B)
                        )
                    }
                }
            }
        }

        Spacer(modifier = Modifier.height(14.dp))

        // 3. Shadow Optics & Depth Bias Controls
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                // Shadow Darkness
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text(
                        text = "Shadow Intensity (Opacity)",
                        style = MaterialTheme.typography.bodySmall,
                        color = Color(0xFF94A3B8)
                    )
                    Text(
                        text = "${(shadowIntensity * 100).toInt()}%",
                        style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Bold),
                        color = Color(0xFFA5B4FC),
                        fontFamily = FontFamily.Monospace
                    )
                }
                Slider(
                    value = shadowIntensity,
                    onValueChange = onShadowIntensityChange,
                    valueRange = 0.1f..1.0f,
                    colors = SliderDefaults.colors(
                        thumbColor = Color(0xFFA5B4FC),
                        activeTrackColor = Color(0xFF6366F1),
                        inactiveTrackColor = Color(0xFF1E293B)
                    ),
                    modifier = Modifier.height(30.dp)
                )

                Spacer(modifier = Modifier.height(10.dp))

                // PCF Softness
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text(
                        text = "Penumbra Softness (Filter Radius)",
                        style = MaterialTheme.typography.bodySmall,
                        color = Color(0xFF94A3B8)
                    )
                    Text(
                        text = "${(shadowSoftness * 10).toInt() / 10f}x",
                        style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Bold),
                        color = Color(0xFFA5B4FC),
                        fontFamily = FontFamily.Monospace
                    )
                }
                Slider(
                    value = shadowSoftness,
                    onValueChange = onShadowSoftnessChange,
                    valueRange = 0.5f..4.0f,
                    colors = SliderDefaults.colors(
                        thumbColor = Color(0xFFA5B4FC),
                        activeTrackColor = Color(0xFF6366F1),
                        inactiveTrackColor = Color(0xFF1E293B)
                    ),
                    modifier = Modifier.height(30.dp)
                )

                Spacer(modifier = Modifier.height(10.dp))

                // Depth Bias
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Column {
                        Text(
                            text = "Depth Bias (vkCmdSetDepthBias)",
                            style = MaterialTheme.typography.bodySmall,
                            color = Color(0xFF94A3B8)
                        )
                        Text(
                            text = "Prevents shadow acne & Peter Panning artifacts",
                            style = MaterialTheme.typography.labelSmall,
                            fontSize = 10.sp,
                            color = Color(0xFF64748B)
                        )
                    }
                    Text(
                        text = "${(depthBias * 1000).toInt() / 1000f}",
                        style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Bold),
                        color = Color(0xFFA5B4FC),
                        fontFamily = FontFamily.Monospace
                    )
                }
                Slider(
                    value = depthBias,
                    onValueChange = onDepthBiasChange,
                    valueRange = 0.001f..0.025f,
                    colors = SliderDefaults.colors(
                        thumbColor = Color(0xFFA5B4FC),
                        activeTrackColor = Color(0xFF6366F1),
                        inactiveTrackColor = Color(0xFF1E293B)
                    ),
                    modifier = Modifier.height(30.dp)
                )
            }
        }

        Spacer(modifier = Modifier.height(14.dp))

        // 4. Live Light Camera Depth Buffer Card
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "Light-Space Viewport & Depth Buffer",
                    style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                    color = Color.White
                )
                Text(
                    text = "Orthographic projection from current sun position (El: ${sunElevation.toInt()}°, Az: ${sunAzimuth.toInt()}°)",
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFF64748B)
                )

                Spacer(modifier = Modifier.height(8.dp))

                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .height(110.dp)
                        .background(Color(0xFF080B12), RoundedCornerShape(8.dp))
                        .border(1.dp, Color(0xFF1E293B), RoundedCornerShape(8.dp)),
                    contentAlignment = Alignment.Center
                ) {
                    Canvas(modifier = Modifier.fillMaxSize()) {
                        val cw = size.width
                        val ch = size.height
                        val ccx = cw / 2f
                        val ccy = ch / 2f

                        // Draw orthographic grid
                        for (i in -4..4) {
                            drawLine(
                                color = Color(0xFF1E293B).copy(alpha = 0.4f),
                                start = Offset(ccx + i * 28f, 0f),
                                end = Offset(ccx + i * 28f, ch),
                                strokeWidth = 1f
                            )
                            drawLine(
                                color = Color(0xFF1E293B).copy(alpha = 0.4f),
                                start = Offset(0f, ccy + i * 20f),
                                end = Offset(cw, ccy + i * 20f),
                                strokeWidth = 1f
                            )
                        }

                        // Depth projection of cube from sun angle
                        val radEl = Math.toRadians(sunElevation.toDouble()).toFloat()
                        val radAz = Math.toRadians(sunAzimuth.toDouble()).toFloat()
                        val lx = cos(radEl) * sin(radAz)
                        val ly = sin(radEl)
                        val lz = cos(radEl) * cos(radAz)

                        // Draw light-space occluder footprint with depth gradient
                        val rectW = 60f
                        val rectH = 45f
                        drawRoundRect(
                            brush = Brush.verticalGradient(
                                colors = listOf(Color(0xFFE2E8F0), Color(0xFF94A3B8), Color(0xFF475569))
                            ),
                            topLeft = Offset(ccx - rectW / 2f, ccy - rectH / 2f),
                            size = Size(rectW, rectH),
                            cornerRadius = androidx.compose.ui.geometry.CornerRadius(4f, 4f)
                        )
                        drawRoundRect(
                            color = Color(0xFF818CF8),
                            topLeft = Offset(ccx - rectW / 2f, ccy - rectH / 2f),
                            size = Size(rectW, rectH),
                            cornerRadius = androidx.compose.ui.geometry.CornerRadius(4f, 4f),
                            style = Stroke(width = 1.5f)
                        )

                        // Light ray directional vector arrow
                        val arrowLen = 32f
                        val endPt = Offset(ccx - lx * arrowLen, ccy + lz * arrowLen)
                        drawLine(Color(0xFFFBBF24), Offset(ccx, ccy), endPt, strokeWidth = 2f)
                        drawCircle(Color(0xFFFBBF24), radius = 3.5f, center = endPt)
                    }

                    // Depth Map Details Overlay
                    Column(
                        modifier = Modifier
                            .align(Alignment.BottomStart)
                            .padding(8.dp)
                    ) {
                        Text(
                            text = "VK_FORMAT_D32_SFLOAT • 2048×2048",
                            style = MaterialTheme.typography.labelSmall,
                            color = Color(0xFFA5B4FC),
                            fontSize = 9.sp,
                            fontFamily = FontFamily.Monospace
                        )
                        Text(
                            text = "CompareOp: VK_COMPARE_OP_LESS_OR_EQUAL",
                            style = MaterialTheme.typography.labelSmall,
                            color = Color(0xFF64748B),
                            fontSize = 8.sp,
                            fontFamily = FontFamily.Monospace
                        )
                    }
                }
            }
        }
    }
}

// -------------------------------------------------------------
// CONTROL TAB 3: STAGE & SURFACE (SCENE SURFACE PIECES & PBR)
// -------------------------------------------------------------
@Composable
fun SurfaceAndStageTab(
    surfaceMode: SurfaceSceneMode,
    onSurfaceModeSelect: (SurfaceSceneMode) -> Unit,
    stageWidth: Float,
    onStageWidthChange: (Float) -> Unit,
    stageLength: Float,
    onStageLengthChange: (Float) -> Unit,
    stageHeight: Float,
    onStageHeightChange: (Float) -> Unit,
    itemSize: Float,
    onApplyAutoFit: () -> Unit,
    selectedColor: ColorOption,
    onColorSelect: (ColorOption) -> Unit,
    selectedPattern: SurfacePattern,
    onPatternSelect: (SurfacePattern) -> Unit
) {
    Column {
        // 1. Surface Scene Mode Selector
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                text = "Surface Scene Environment",
                style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                color = Color.White
            )
            Surface(
                shape = RoundedCornerShape(4.dp),
                color = Color(0xFF0369A1).copy(alpha = 0.35f)
            ) {
                Text(
                    text = "${surfaceMode.icon} ${surfaceMode.shortLabel}",
                    style = MaterialTheme.typography.labelSmall,
                    color = Color(0xFF38BDF8),
                    fontWeight = FontWeight.Bold,
                    modifier = Modifier.padding(horizontal = 6.dp, vertical = 2.dp)
                )
            }
        }
        Spacer(modifier = Modifier.height(10.dp))

        // Mode Chips Row
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            SurfaceSceneMode.values().forEach { mode ->
                val isSelected = mode == surfaceMode
                FilterChip(
                    selected = isSelected,
                    onClick = { onSurfaceModeSelect(mode) },
                    label = {
                        Text(
                            text = "${mode.icon} ${mode.title}",
                            fontSize = 12.sp,
                            fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal
                        )
                    },
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
                    ),
                    modifier = Modifier.testTag("surface_mode_chip_${mode.name.lowercase()}")
                )
            }
        }

        Spacer(modifier = Modifier.height(8.dp))

        // Mode Description & Auto-Fit guidance card
        Card(
            modifier = Modifier.fillMaxWidth(),
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            border = CardDefaults.outlinedCardBorder().copy(
                brush = Brush.linearGradient(listOf(Color(0xFF1E293B), Color(0xFF0F172A)))
            )
        ) {
            Column(modifier = Modifier.padding(12.dp)) {
                Text(
                    text = surfaceMode.description,
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFFCBD5E1)
                )
                if (surfaceMode == SurfaceSceneMode.FLAT_PIECE) {
                    Spacer(modifier = Modifier.height(6.dp))
                    Text(
                        text = if (itemSize <= 1.25f)
                            "• Relative Size Rule: Active item is compact (${((itemSize * 100).toInt()) / 100f}m) -> Default 3m × 3m piece"
                        else
                            "• Relative Size Rule: Active item footprint is ${((itemSize * 100).toInt()) / 100f}m -> Default 10m × 10m piece",
                        style = MaterialTheme.typography.labelSmall,
                        color = Color(0xFF38BDF8),
                        fontFamily = FontFamily.Monospace
                    )
                } else if (surfaceMode == SurfaceSceneMode.WATER_BODY) {
                    Spacer(modifier = Modifier.height(6.dp))
                    Text(
                        text = "• Water Body Default: 10m × 10m piece with animated caustics & ripple reflections",
                        style = MaterialTheme.typography.labelSmall,
                        color = Color(0xFF06B6D4),
                        fontFamily = FontFamily.Monospace
                    )
                }
            }
        }

        // 2. Stage Dimensions & Adjustments (When not NONE or ENDLESS)
        if (surfaceMode == SurfaceSceneMode.FLAT_PIECE || surfaceMode == SurfaceSceneMode.WATER_BODY || surfaceMode == SurfaceSceneMode.BROKEN_SLAB) {
            Spacer(modifier = Modifier.height(16.dp))

            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = "Piece Dimensions & Presets",
                    style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                    color = Color.White
                )
                Text(
                    text = "${((stageWidth * 10).toInt()) / 10f}m × ${((stageLength * 10).toInt()) / 10f}m",
                    style = MaterialTheme.typography.labelSmall,
                    color = Color(0xFF38BDF8),
                    fontFamily = FontFamily.Monospace,
                    fontWeight = FontWeight.Bold
                )
            }

            Spacer(modifier = Modifier.height(8.dp))

            // Presets: 3x3m (small item), 10x10m (default piece), 20x20m, Auto-Fit
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState()),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = if (stageWidth == 3.0f && stageLength == 3.0f) Color(0xFF0284C7).copy(alpha = 0.35f) else Color(0xFF1E293B),
                    border = CardDefaults.outlinedCardBorder().copy(
                        brush = Brush.linearGradient(
                            if (stageWidth == 3.0f && stageLength == 3.0f) listOf(Color(0xFF38BDF8), Color(0xFF0284C7))
                            else listOf(Color(0xFF334155), Color(0xFF1E293B))
                        )
                    ),
                    modifier = Modifier
                        .clickable {
                            onStageWidthChange(3.0f)
                            onStageLengthChange(3.0f)
                        }
                        .testTag("preset_3x3m_button")
                ) {
                    Text(
                        text = "3×3m (Small ~1m Item)",
                        style = MaterialTheme.typography.labelSmall,
                        color = if (stageWidth == 3.0f && stageLength == 3.0f) Color(0xFF38BDF8) else Color(0xFFCBD5E1),
                        fontWeight = FontWeight.SemiBold,
                        modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp)
                    )
                }

                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = if (stageWidth == 10.0f && stageLength == 10.0f) Color(0xFF0284C7).copy(alpha = 0.35f) else Color(0xFF1E293B),
                    border = CardDefaults.outlinedCardBorder().copy(
                        brush = Brush.linearGradient(
                            if (stageWidth == 10.0f && stageLength == 10.0f) listOf(Color(0xFF38BDF8), Color(0xFF0284C7))
                            else listOf(Color(0xFF334155), Color(0xFF1E293B))
                        )
                    ),
                    modifier = Modifier
                        .clickable {
                            onStageWidthChange(10.0f)
                            onStageLengthChange(10.0f)
                        }
                        .testTag("preset_10x10m_button")
                ) {
                    Text(
                        text = "10×10m (Default Piece)",
                        style = MaterialTheme.typography.labelSmall,
                        color = if (stageWidth == 10.0f && stageLength == 10.0f) Color(0xFF38BDF8) else Color(0xFFCBD5E1),
                        fontWeight = FontWeight.SemiBold,
                        modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp)
                    )
                }

                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = if (stageWidth == 20.0f && stageLength == 20.0f) Color(0xFF0284C7).copy(alpha = 0.35f) else Color(0xFF1E293B),
                    border = CardDefaults.outlinedCardBorder().copy(
                        brush = Brush.linearGradient(
                            if (stageWidth == 20.0f && stageLength == 20.0f) listOf(Color(0xFF38BDF8), Color(0xFF0284C7))
                            else listOf(Color(0xFF334155), Color(0xFF1E293B))
                        )
                    ),
                    modifier = Modifier
                        .clickable {
                            onStageWidthChange(20.0f)
                            onStageLengthChange(20.0f)
                        }
                        .testTag("preset_20x20m_button")
                ) {
                    Text(
                        text = "20×20m (Large Piece)",
                        style = MaterialTheme.typography.labelSmall,
                        color = if (stageWidth == 20.0f && stageLength == 20.0f) Color(0xFF38BDF8) else Color(0xFFCBD5E1),
                        fontWeight = FontWeight.SemiBold,
                        modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp)
                    )
                }

                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = Color(0xFF0F766E).copy(alpha = 0.4f),
                    border = CardDefaults.outlinedCardBorder().copy(
                        brush = Brush.linearGradient(listOf(Color(0xFF14B8A6), Color(0xFF0F766E)))
                    ),
                    modifier = Modifier
                        .clickable { onApplyAutoFit() }
                        .testTag("auto_fit_stage_button")
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Icon(
                            imageVector = Icons.Default.AutoAwesome,
                            contentDescription = null,
                            tint = Color(0xFF5EEAD4),
                            modifier = Modifier.size(13.dp)
                        )
                        Spacer(modifier = Modifier.width(4.dp))
                        Text(
                            text = "Auto-Fit (${((itemSize * 10).toInt()) / 10f}m)",
                            style = MaterialTheme.typography.labelSmall,
                            color = Color(0xFF5EEAD4),
                            fontWeight = FontWeight.Bold
                        )
                    }
                }
            }

            Spacer(modifier = Modifier.height(12.dp))

            // Width Slider
            Text(
                text = "Piece Width: ${((stageWidth * 10).toInt()) / 10f} m",
                style = MaterialTheme.typography.bodySmall,
                color = Color(0xFFCBD5E1)
            )
            Slider(
                value = stageWidth,
                onValueChange = onStageWidthChange,
                valueRange = 1.0f..50.0f,
                colors = SliderDefaults.colors(
                    thumbColor = Color(0xFF38BDF8),
                    activeTrackColor = Color(0xFF0284C7),
                    inactiveTrackColor = Color(0xFF1E293B)
                ),
                modifier = Modifier.testTag("stage_width_slider")
            )

            // Length Slider
            Text(
                text = "Piece Length: ${((stageLength * 10).toInt()) / 10f} m",
                style = MaterialTheme.typography.bodySmall,
                color = Color(0xFFCBD5E1)
            )
            Slider(
                value = stageLength,
                onValueChange = onStageLengthChange,
                valueRange = 1.0f..50.0f,
                colors = SliderDefaults.colors(
                    thumbColor = Color(0xFF38BDF8),
                    activeTrackColor = Color(0xFF0284C7),
                    inactiveTrackColor = Color(0xFF1E293B)
                ),
                modifier = Modifier.testTag("stage_length_slider")
            )

            // Skirt / Thickness Slider
            Text(
                text = "Platform Thickness: ${((stageHeight * 100).toInt())} cm",
                style = MaterialTheme.typography.bodySmall,
                color = Color(0xFFCBD5E1)
            )
            Slider(
                value = stageHeight,
                onValueChange = onStageHeightChange,
                valueRange = 0.1f..2.5f,
                colors = SliderDefaults.colors(
                    thumbColor = Color(0xFF38BDF8),
                    activeTrackColor = Color(0xFF0284C7),
                    inactiveTrackColor = Color(0xFF1E293B)
                ),
                modifier = Modifier.testTag("stage_height_slider")
            )
        }

        Spacer(modifier = Modifier.height(20.dp))

        // 3. Item Surface Color Palette & Pattern
        Text(
            text = "Item Surface Colour Palette",
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
            text = "Item Procedural PBR Pattern",
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
    surfaceMode: SurfaceSceneMode = SurfaceSceneMode.FLAT_PIECE,
    stageWidth: Float = 10.0f,
    stageLength: Float = 10.0f,
    stageHeight: Float = 0.35f,
    enableShadows: Boolean = true,
    shadowQuality: ShadowQuality = ShadowQuality.PCF_5X5,
    shadowIntensity: Float = 0.72f,
    shadowSoftness: Float = 1.8f,
    depthBias: Float = 0.005f,
    showShadowMapDebug: Boolean = false,
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

        // 3D Camera Projection Setup
        val cameraDist = 4.8f
        val fovFactor = 540f

        fun projectPoint(v: Vec3D): Point2D {
            val x1 = v.x * cosY + v.z * sinY
            val z1 = -v.x * sinY + v.z * cosY

            val y2 = v.y * cosX - z1 * sinX
            val z2 = v.y * sinX + z1 * cosX

            val zView = (z2 + cameraDist).coerceAtLeast(0.1f)
            val px = cx + (x1 / zView) * fovFactor
            val py = cy - (y2 / zView) * fovFactor
            return Point2D(px, py, zView)
        }

        fun transformNormal(rawNorm: Vec3D): Vec3D {
            val nx1 = rawNorm.x * cosY + rawNorm.z * sinY
            val nz1 = -rawNorm.x * sinY + rawNorm.z * cosY
            val ny2 = rawNorm.y * cosX - nz1 * sinX
            val nz2 = rawNorm.y * sinX + nz1 * cosX
            return Vec3D(nx1, ny2, nz2)
        }

        // Horizon position in viewport screen space (shifts with camera pitch rotX)
        val horizonY = cy + 45f + (sinX * 180f)

        // Directional light: Sun or studio light
        val lightDir = if (enableSun) sunDir else Vec3D(0.5f, 0.85f, 0.4f)
        val lightLen = sqrt(lightDir.x * lightDir.x + lightDir.y * lightDir.y + lightDir.z * lightDir.z).coerceAtLeast(0.001f)
        val normLight = Vec3D(lightDir.x / lightLen, lightDir.y / lightLen, lightDir.z / lightLen)
        val sunLightTint = if (enableSun && sunElevation < 20f) Color(0xFFFDBA74) else Color.White

        // 2. BACKGROUND RENDERING (Sky vs Studio Dark vs Void)
        if (enableSky) {
            val zenithColor: Color
            val horizonColor: Color

            if (sunElevation > 20.0f) {
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
                val nightRatio = (-sunElevation / 10.0f).coerceIn(0f, 1f)
                zenithColor = Color(0xFF04060C)
                horizonColor = Color(
                    red = (0.22f * (1f - nightRatio) + 0.05f).coerceIn(0f, 1f),
                    green = (0.15f * (1f - nightRatio) + 0.07f).coerceIn(0f, 1f),
                    blue = (0.35f * (1f - nightRatio) + 0.12f).coerceIn(0f, 1f)
                )
            }

            if (surfaceMode == SurfaceSceneMode.NONE) {
                // VOID (No Surface): Sky seamlessly wraps full viewport with zero horizon cutoff
                drawRect(
                    brush = Brush.verticalGradient(
                        colors = listOf(
                            zenithColor,
                            horizonColor,
                            Color(0xFF0F172A),
                            Color(0xFF050811)
                        )
                    )
                )
            } else {
                // Atmospheric Sky Dome down to horizon
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

                // Below horizon
                if (surfaceMode == SurfaceSceneMode.ENDLESS) {
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
                } else {
                    // Floating stage in atmosphere
                    drawRect(
                        brush = Brush.verticalGradient(
                            colors = listOf(
                                horizonColor.copy(alpha = 0.35f),
                                Color(0xFF0F172A),
                                Color(0xFF090D1A)
                            ),
                            startY = horizonY,
                            endY = h
                        ),
                        topLeft = Offset(0f, horizonY),
                        size = Size(w, (h - horizonY).coerceAtLeast(0f))
                    )
                }
            }

            // Sun Disk & Corona Glow
            if (enableSun && sunElevation > -2.0f) {
                val sunX1 = sunDir.x * cosY + sunDir.z * sinY
                val sunZ1 = -sunDir.x * sinY + sunDir.z * cosY
                val sunY2 = sunDir.y * cosX - sunZ1 * sinX
                val sunZ2 = sunDir.y * sinX + sunZ1 * cosX

                if (sunZ2 > -0.2f) {
                    val sunProjDist = 600f
                    val sunPx = cx + (sunX1 / (sunZ2 + 2.5f)) * sunProjDist
                    val sunPy = cy - (sunY2 / (sunZ2 + 2.5f)) * sunProjDist
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

        // 3. STAGE / SURFACE GEOMETRY RENDERING & DIRECTIONAL SHADOWS
        val hw = width * 0.5f
        val hh = height * 0.5f
        val hd = depth * 0.5f
        val yGround = -hh

        // Sascha Willems Vulkan Directional Shadow Mapping Projection
        val castShadowsActive = enableShadows && enableSun && shadowQuality != ShadowQuality.OFF && sunElevation > -2.0f
        val sunElevationClamped = sunElevation.coerceAtLeast(1.5f)
        val elRadClamped = Math.toRadians(sunElevationClamped.toDouble()).toFloat()
        val lightY = sin(elRadClamped).coerceAtLeast(0.04f)
        val lightX = cos(elRadClamped) * sin(azRad)
        val lightZ = cos(elRadClamped) * cos(azRad)

        // Helper: Cast 3D directional shadow of the item onto a receiving plane
        fun drawItemDirectionalShadow(
            receiverY: Float,
            baseShadowColor: Color = Color.Black,
            intensityScale: Float = 1.0f
        ) {
            if (!castShadowsActive) return

            val effIntensity = (shadowIntensity * intensityScale).coerceIn(0f, 1f)
            val tRay = ((yGround + height) - receiverY) / lightY
            val clampedTRay = tRay.coerceIn(0.1f, 18.0f)

            // 4 Base Vertices on receiver plane
            val b0 = Vec3D(-hw, receiverY, -hd)
            val b1 = Vec3D(hw, receiverY, -hd)
            val b2 = Vec3D(hw, receiverY, hd)
            val b3 = Vec3D(-hw, receiverY, hd)

            // 4 Top Vertices projected along -L onto receiver plane
            val p0 = Vec3D(-hw - clampedTRay * lightX, receiverY, -hd - clampedTRay * lightZ)
            val p1 = Vec3D(hw - clampedTRay * lightX, receiverY, -hd - clampedTRay * lightZ)
            val p2 = Vec3D(hw - clampedTRay * lightX, receiverY, hd - clampedTRay * lightZ)
            val p3 = Vec3D(-hw - clampedTRay * lightX, receiverY, hd - clampedTRay * lightZ)

            val rawHull = computeConvexHull2D(listOf(b0, b1, b2, b3, p0, p1, p2, p3))
            if (rawHull.size < 3) return

            // Penumbra spread radius based on ray distance and softness setting (Sascha Willems PCF)
            val penumbraDist = clampedTRay * 0.05f * shadowSoftness

            // PCF Multi-tap rendering
            when (shadowQuality) {
                ShadowQuality.PCF_5X5, ShadowQuality.CSM_4 -> {
                    // Outer Penumbra Shell (smooth falloff)
                    val outerHull = rawHull.map { pt ->
                        val isProjected = pt.x !in (-hw - 0.01f)..(hw + 0.01f) || pt.z !in (-hd - 0.01f)..(hd + 0.01f)
                        if (isProjected) {
                            Vec3D(pt.x - lightX * penumbraDist * 0.4f, pt.y, pt.z - lightZ * penumbraDist * 0.4f)
                        } else pt
                    }
                    val outerPath = Path().apply {
                        val s0 = projectPoint(outerHull[0])
                        moveTo(s0.x, s0.y)
                        for (i in 1 until outerHull.size) {
                            val si = projectPoint(outerHull[i])
                            lineTo(si.x, si.y)
                        }
                        close()
                    }
                    drawPath(outerPath, color = baseShadowColor.copy(alpha = effIntensity * 0.22f))

                    // Intermediate Penumbra Shell
                    val midHull = rawHull.map { pt ->
                        val isProjected = pt.x !in (-hw - 0.01f)..(hw + 0.01f) || pt.z !in (-hd - 0.01f)..(hd + 0.01f)
                        if (isProjected) {
                            Vec3D(pt.x - lightX * penumbraDist * 0.18f, pt.y, pt.z - lightZ * penumbraDist * 0.18f)
                        } else pt
                    }
                    val midPath = Path().apply {
                        val s0 = projectPoint(midHull[0])
                        moveTo(s0.x, s0.y)
                        for (i in 1 until midHull.size) {
                            val si = projectPoint(midHull[i])
                            lineTo(si.x, si.y)
                        }
                        close()
                    }
                    drawPath(midPath, color = baseShadowColor.copy(alpha = effIntensity * 0.45f))

                    // Core Umbra
                    val corePath = Path().apply {
                        val s0 = projectPoint(rawHull[0])
                        moveTo(s0.x, s0.y)
                        for (i in 1 until rawHull.size) {
                            val si = projectPoint(rawHull[i])
                            lineTo(si.x, si.y)
                        }
                        close()
                    }
                    drawPath(corePath, color = baseShadowColor.copy(alpha = effIntensity * 0.88f))
                }
                ShadowQuality.PCF_3X3 -> {
                    val outerHull = rawHull.map { pt ->
                        val isProjected = pt.x !in (-hw - 0.01f)..(hw + 0.01f) || pt.z !in (-hd - 0.01f)..(hd + 0.01f)
                        if (isProjected) {
                            Vec3D(pt.x - lightX * penumbraDist * 0.25f, pt.y, pt.z - lightZ * penumbraDist * 0.25f)
                        } else pt
                    }
                    val outerPath = Path().apply {
                        val s0 = projectPoint(outerHull[0])
                        moveTo(s0.x, s0.y)
                        for (i in 1 until outerHull.size) {
                            val si = projectPoint(outerHull[i])
                            lineTo(si.x, si.y)
                        }
                        close()
                    }
                    drawPath(outerPath, color = baseShadowColor.copy(alpha = effIntensity * 0.35f))

                    val corePath = Path().apply {
                        val s0 = projectPoint(rawHull[0])
                        moveTo(s0.x, s0.y)
                        for (i in 1 until rawHull.size) {
                            val si = projectPoint(rawHull[i])
                            lineTo(si.x, si.y)
                        }
                        close()
                    }
                    drawPath(corePath, color = baseShadowColor.copy(alpha = effIntensity * 0.85f))
                }
                ShadowQuality.HARD -> {
                    val corePath = Path().apply {
                        val s0 = projectPoint(rawHull[0])
                        moveTo(s0.x, s0.y)
                        for (i in 1 until rawHull.size) {
                            val si = projectPoint(rawHull[i])
                            lineTo(si.x, si.y)
                        }
                        close()
                    }
                    drawPath(corePath, color = baseShadowColor.copy(alpha = effIntensity * 0.92f))
                }
                ShadowQuality.OFF -> {}
            }

            // Contact Ambient Occlusion (prevents Peter Panning at base contact)
            val c0 = projectPoint(Vec3D(-hw * 1.05f, receiverY + 0.005f, -hd * 1.05f))
            val c1 = projectPoint(Vec3D(hw * 1.05f, receiverY + 0.005f, -hd * 1.05f))
            val c2 = projectPoint(Vec3D(hw * 1.05f, receiverY + 0.005f, hd * 1.05f))
            val c3 = projectPoint(Vec3D(-hw * 1.05f, receiverY + 0.005f, hd * 1.05f))
            val contactPath = Path().apply {
                moveTo(c0.x, c0.y); lineTo(c1.x, c1.y); lineTo(c2.x, c2.y); lineTo(c3.x, c3.y); close()
            }
            drawPath(contactPath, color = baseShadowColor.copy(alpha = (effIntensity * 1.15f).coerceAtMost(0.96f)))
        }

        when (surfaceMode) {
            SurfaceSceneMode.NONE -> {
                // No surface: Draw floating reference rings if dark mode
                if (!enableSky) {
                    val refRadius = (maxOf(hw, hd) * 1.8f).coerceAtLeast(1.4f)
                    val numPts = 32
                    val refPoints = (0..numPts).map { i ->
                        val ang = (i.toFloat() / numPts) * 2f * 3.14159265f
                        projectPoint(Vec3D(refRadius * cos(ang), yGround, refRadius * sin(ang)))
                    }
                    val refPath = Path().apply {
                        moveTo(refPoints[0].x, refPoints[0].y)
                        for (pt in refPoints.drop(1)) lineTo(pt.x, pt.y)
                        close()
                    }
                    drawPath(
                        path = refPath,
                        color = Color(0xFF38BDF8).copy(alpha = 0.18f),
                        style = Stroke(width = 1f, pathEffect = PathEffect.dashPathEffect(floatArrayOf(5f, 5f), 0f))
                    )
                }
            }

            SurfaceSceneMode.ENDLESS -> {
                // Infinite ground grid and vanishing lines receding to horizon
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

                // True Directional Ground Shadow projected from light source
                drawItemDirectionalShadow(receiverY = yGround, baseShadowColor = Color.Black)
            }

            SurfaceSceneMode.FLAT_PIECE -> {
                // Bounded, adjustable rectangular platform (default 10×10m, or 3×3m for ~1m items)
                val sw = stageWidth * 0.5f
                val sd = stageLength * 0.5f
                val sh = stageHeight
                val yTop = yGround
                val yBtm = yGround - sh

                val pT0 = projectPoint(Vec3D(-sw, yTop, -sd)) // Front Left
                val pT1 = projectPoint(Vec3D(sw, yTop, -sd))  // Front Right
                val pT2 = projectPoint(Vec3D(sw, yTop, sd))   // Back Right
                val pT3 = projectPoint(Vec3D(-sw, yTop, sd))  // Back Left

                val pB0 = projectPoint(Vec3D(-sw, yBtm, -sd))
                val pB1 = projectPoint(Vec3D(sw, yBtm, -sd))
                val pB2 = projectPoint(Vec3D(sw, yBtm, sd))
                val pB3 = projectPoint(Vec3D(-sw, yBtm, sd))

                // Directional drop shadow of platform on void below (along -L)
                if (castShadowsActive) {
                    val tPlat = (sh + 0.6f) / lightY
                    val clPlat = tPlat.coerceIn(0.2f, 16.0f)
                    val platHull = computeConvexHull2D(listOf(
                        Vec3D(-sw, yBtm - 0.4f, -sd),
                        Vec3D(sw, yBtm - 0.4f, -sd),
                        Vec3D(sw, yBtm - 0.4f, sd),
                        Vec3D(-sw, yBtm - 0.4f, sd),
                        Vec3D(-sw - clPlat * lightX, yBtm - 0.4f, -sd - clPlat * lightZ),
                        Vec3D(sw - clPlat * lightX, yBtm - 0.4f, -sd - clPlat * lightZ),
                        Vec3D(sw - clPlat * lightX, yBtm - 0.4f, sd - clPlat * lightZ),
                        Vec3D(-sw - clPlat * lightX, yBtm - 0.4f, sd - clPlat * lightZ)
                    ))
                    if (platHull.size >= 3) {
                        val platShadowPath = Path().apply {
                            val s0 = projectPoint(platHull[0])
                            moveTo(s0.x, s0.y)
                            for (i in 1 until platHull.size) {
                                val si = projectPoint(platHull[i])
                                lineTo(si.x, si.y)
                            }
                            close()
                        }
                        drawPath(platShadowPath, color = Color.Black.copy(alpha = (shadowIntensity * 0.45f).coerceIn(0f, 0.65f)))
                    }
                }

                // Side Skirts: Front, Right, Back, Left
                val sideFaces = listOf(
                    Triple(listOf(pT0, pT1, pB1, pB0), Vec3D(0f, 0f, -1f), "Front"),
                    Triple(listOf(pT1, pT2, pB2, pB1), Vec3D(1f, 0f, 0f), "Right"),
                    Triple(listOf(pT2, pT3, pB3, pB2), Vec3D(0f, 0f, 1f), "Back"),
                    Triple(listOf(pT3, pT0, pB0, pB3), Vec3D(-1f, 0f, 0f), "Left")
                )

                for ((pts, normal, _) in sideFaces) {
                    val rotNorm = transformNormal(normal)
                    if (rotNorm.z > -0.05f) { // Face is visible
                        val dot = (rotNorm.x * normLight.x + rotNorm.y * normLight.y + rotNorm.z * normLight.z).coerceAtLeast(0f)
                        val skirtShade = (0.25f + 0.55f * dot).coerceIn(0.15f, 0.85f)
                        val skirtColor = Color(
                            red = (0.12f * sunLightTint.red * skirtShade).coerceIn(0f, 1f),
                            green = (0.16f * sunLightTint.green * skirtShade).coerceIn(0f, 1f),
                            blue = (0.24f * sunLightTint.blue * skirtShade).coerceIn(0f, 1f),
                            alpha = 0.98f
                        )
                        val skirtPath = Path().apply {
                            moveTo(pts[0].x, pts[0].y)
                            lineTo(pts[1].x, pts[1].y)
                            lineTo(pts[2].x, pts[2].y)
                            lineTo(pts[3].x, pts[3].y)
                            close()
                        }
                        drawPath(path = skirtPath, color = skirtColor)
                        drawPath(path = skirtPath, color = Color(0xFF1E293B), style = Stroke(width = 1f))
                    }
                }

                // Top Face Quad
                val topQuadPath = Path().apply {
                    moveTo(pT0.x, pT0.y)
                    lineTo(pT1.x, pT1.y)
                    lineTo(pT2.x, pT2.y)
                    lineTo(pT3.x, pT3.y)
                    close()
                }

                // Platform Top Fill (Sleek slate PBR tone)
                val topNormal = transformNormal(Vec3D(0f, 1f, 0f))
                val topDot = (topNormal.x * normLight.x + topNormal.y * normLight.y + topNormal.z * normLight.z).coerceAtLeast(0f)
                val topShade = (0.35f + 0.65f * topDot).coerceIn(0.25f, 1.0f)
                val platformTopColor = Color(
                    red = (0.13f * sunLightTint.red * topShade).coerceIn(0f, 1f),
                    green = (0.17f * sunLightTint.green * topShade).coerceIn(0f, 1f),
                    blue = (0.25f * sunLightTint.blue * topShade).coerceIn(0f, 1f),
                    alpha = 0.98f
                )
                drawPath(path = topQuadPath, color = platformTopColor)

                // 1-Meter Perspective Grid lines on the platform
                if (showGrid) {
                    val stepSize = if (stageWidth <= 4.0f && stageLength <= 4.0f) 0.5f else 1.0f
                    var gx = -sw + stepSize
                    while (gx < sw) {
                        val pA = projectPoint(Vec3D(gx, yTop, -sd))
                        val pB = projectPoint(Vec3D(gx, yTop, sd))
                        val isCenter = kotlin.math.abs(gx) < 0.1f
                        drawLine(
                            color = if (isCenter) Color(0xFF38BDF8).copy(alpha = 0.7f) else Color(0xFF38BDF8).copy(alpha = 0.22f),
                            start = Offset(pA.x, pA.y),
                            end = Offset(pB.x, pB.y),
                            strokeWidth = if (isCenter) 1.8f else 1.0f
                        )
                        gx += stepSize
                    }

                    var gz = -sd + stepSize
                    while (gz < sd) {
                        val pC = projectPoint(Vec3D(-sw, yTop, gz))
                        val pD = projectPoint(Vec3D(sw, yTop, gz))
                        val isCenter = kotlin.math.abs(gz) < 0.1f
                        drawLine(
                            color = if (isCenter) Color(0xFF38BDF8).copy(alpha = 0.7f) else Color(0xFF38BDF8).copy(alpha = 0.22f),
                            start = Offset(pC.x, pC.y),
                            end = Offset(pD.x, pD.y),
                            strokeWidth = if (isCenter) 1.8f else 1.0f
                        )
                        gz += stepSize
                    }
                }

                // Perimeter Bevel Highlight
                drawPath(
                    path = topQuadPath,
                    color = Color(0xFF38BDF8).copy(alpha = 0.65f),
                    style = Stroke(width = 2.0f)
                )

                // Corner registration markers
                listOf(pT0, pT1, pT2, pT3).forEach { pt ->
                    drawCircle(Color(0xFF38BDF8), radius = 3.5f, center = Offset(pt.x, pt.y))
                }

                // True Directional Item Shadow on Platform
                drawItemDirectionalShadow(receiverY = yTop, baseShadowColor = Color.Black)
            }

            SurfaceSceneMode.WATER_BODY -> {
                // Bounded 10×10m default water surface piece
                val sw = stageWidth * 0.5f
                val sd = stageLength * 0.5f
                val sh = stageHeight
                val yTop = yGround
                val yBtm = yGround - sh

                val pT0 = projectPoint(Vec3D(-sw, yTop, -sd))
                val pT1 = projectPoint(Vec3D(sw, yTop, -sd))
                val pT2 = projectPoint(Vec3D(sw, yTop, sd))
                val pT3 = projectPoint(Vec3D(-sw, yTop, sd))

                val pB0 = projectPoint(Vec3D(-sw, yBtm, -sd))
                val pB1 = projectPoint(Vec3D(sw, yBtm, -sd))
                val pB2 = projectPoint(Vec3D(sw, yBtm, sd))
                val pB3 = projectPoint(Vec3D(-sw, yBtm, sd))

                // Underwater basin glass edges
                val basinFaces = listOf(
                    listOf(pT0, pT1, pB1, pB0),
                    listOf(pT1, pT2, pB2, pB1),
                    listOf(pT2, pT3, pB3, pB2),
                    listOf(pT3, pT0, pB0, pB3)
                )
                for (pts in basinFaces) {
                    val p = Path().apply {
                        moveTo(pts[0].x, pts[0].y); lineTo(pts[1].x, pts[1].y); lineTo(pts[2].x, pts[2].y); lineTo(pts[3].x, pts[3].y); close()
                    }
                    drawPath(path = p, color = Color(0xDD075985))
                    drawPath(path = p, color = Color(0xFF0284C7).copy(alpha = 0.7f), style = Stroke(width = 1.2f))
                }

                // Top Water Quad
                val waterPath = Path().apply {
                    moveTo(pT0.x, pT0.y); lineTo(pT1.x, pT1.y); lineTo(pT2.x, pT2.y); lineTo(pT3.x, pT3.y); close()
                }
                drawPath(
                    path = waterPath,
                    brush = Brush.linearGradient(
                        colors = listOf(Color(0xFA0284C7), Color(0xFA0369A1), Color(0xFA0C4A6E)),
                        start = Offset(pT0.x, pT0.y),
                        end = Offset(pT2.x, pT2.y)
                    )
                )

                // Directional Shadow on Water Surface (with aquatic light absorption tinting)
                drawItemDirectionalShadow(receiverY = yTop, baseShadowColor = Color(0xFF031422), intensityScale = 0.85f)

                // Animated Water Ripples in perspective
                for (r in 1..6) {
                    val rz = -sd + (sd * 2f) * (r / 7f)
                    val rA = projectPoint(Vec3D(-sw * 0.9f, yTop, rz))
                    val rB = projectPoint(Vec3D(sw * 0.9f, yTop, rz))
                    val rMid = projectPoint(Vec3D(0f, yTop, rz))
                    val ripplePath = Path().apply {
                        moveTo(rA.x, rA.y)
                        quadraticTo(rMid.x, rMid.y - 3f, rB.x, rB.y)
                    }
                    drawLine(
                        color = Color(0xFFE0F2FE).copy(alpha = 0.35f),
                        start = Offset(rA.x, rA.y),
                        end = Offset(rB.x, rB.y),
                        strokeWidth = 1.2f
                    )
                }

                // Perimeter white-cyan water foam
                drawPath(
                    path = waterPath,
                    color = Color(0xFFBAE6FD).copy(alpha = 0.85f),
                    style = Stroke(width = 2.5f)
                )

                // Item contact water foam ring
                val cW0 = projectPoint(Vec3D(-hw * 1.15f, yTop, -hd * 1.15f))
                val cW1 = projectPoint(Vec3D(hw * 1.15f, yTop, -hd * 1.15f))
                val cW2 = projectPoint(Vec3D(hw * 1.15f, yTop, hd * 1.15f))
                val cW3 = projectPoint(Vec3D(-hw * 1.15f, yTop, hd * 1.15f))
                val foamPath = Path().apply {
                    moveTo(cW0.x, cW0.y); lineTo(cW1.x, cW1.y); lineTo(cW2.x, cW2.y); lineTo(cW3.x, cW3.y); close()
                }
                drawPath(path = foamPath, color = Color(0xFFE0F2FE).copy(alpha = 0.85f), style = Stroke(width = 2f))
                drawPath(path = foamPath, color = Color(0x66082F49))
            }

            SurfaceSceneMode.BROKEN_SLAB -> {
                // Fractured floating rock diorama piece with chiseled cliff edges
                val sw = stageWidth * 0.5f
                val sd = stageLength * 0.5f
                val sh = stageHeight * 1.4f
                val yTop = yGround
                val yBtm = yGround - sh

                val radii = floatArrayOf(
                    1.0f, 0.85f, 1.08f, 0.78f, 1.12f, 0.90f,
                    1.04f, 0.80f, 1.10f, 0.86f, 1.05f, 0.88f
                )
                val numPts = radii.size
                val topPts = mutableListOf<Point2D>()
                val btmPts = mutableListOf<Point2D>()

                for (k in 0 until numPts) {
                    val angle = (k.toFloat() / numPts) * 2f * 3.14159265f
                    val r = sw * radii[k]
                    val fx = r * cos(angle)
                    val fz = (r * (sd / sw)) * sin(angle)
                    topPts.add(projectPoint(Vec3D(fx, yTop, fz)))
                    btmPts.add(projectPoint(Vec3D(fx * 0.72f, yBtm, fz * 0.72f)))
                }

                // Drop shadow on void
                val btmCenter = projectPoint(Vec3D(0f, yBtm - 0.5f, 0f))
                drawCircle(
                    brush = Brush.radialGradient(
                        colors = listOf(Color.Black.copy(alpha = 0.5f), Color.Transparent),
                        center = Offset(btmCenter.x, btmCenter.y),
                        radius = (sw * 45f).coerceAtLeast(20f)
                    ),
                    radius = (sw * 45f).coerceAtLeast(20f),
                    center = Offset(btmCenter.x, btmCenter.y)
                )

                // Cliff side facets
                for (k in 0 until numPts) {
                    val next = (k + 1) % numPts
                    val cliffQuad = Path().apply {
                        moveTo(topPts[k].x, topPts[k].y)
                        lineTo(topPts[next].x, topPts[next].y)
                        lineTo(btmPts[next].x, btmPts[next].y)
                        lineTo(btmPts[k].x, btmPts[k].y)
                        close()
                    }
                    val facetShade = if (k % 2 == 0) 0.35f else 0.22f
                    drawPath(
                        path = cliffQuad,
                        color = Color(
                            red = (0.20f * sunLightTint.red * facetShade).coerceIn(0f, 1f),
                            green = (0.24f * sunLightTint.green * facetShade).coerceIn(0f, 1f),
                            blue = (0.32f * sunLightTint.blue * facetShade).coerceIn(0f, 1f),
                            alpha = 0.98f
                        )
                    )
                    drawPath(path = cliffQuad, color = Color(0xFF1E293B), style = Stroke(width = 1f))
                }

                // Top Weathered Rock Slab
                val topRockPath = Path().apply {
                    moveTo(topPts[0].x, topPts[0].y)
                    for (pt in topPts.drop(1)) lineTo(pt.x, pt.y)
                    close()
                }
                drawPath(path = topRockPath, color = Color(0xFF293548))
                drawPath(path = topRockPath, color = Color(0xFF64748B), style = Stroke(width = 1.5f))

                // Fracture cracks radiating from center
                val centerPt = projectPoint(Vec3D(0f, yTop, 0f))
                for (k in listOf(1, 4, 7, 10)) {
                    drawLine(
                        color = Color(0xFF0F172A).copy(alpha = 0.6f),
                        start = Offset(centerPt.x, centerPt.y),
                        end = Offset(topPts[k].x, topPts[k].y),
                        strokeWidth = 1.2f
                    )
                }

                // Floating rock fragments suspended underneath
                val frag1 = projectPoint(Vec3D(sw * 0.7f, yBtm - 0.25f, sd * 0.5f))
                val frag2 = projectPoint(Vec3D(-sw * 0.65f, yBtm - 0.35f, -sd * 0.4f))
                drawCircle(Color(0xFF334155), radius = 5f, center = Offset(frag1.x, frag1.y))
                drawCircle(Color(0xFF1E293B), radius = 4f, center = Offset(frag2.x, frag2.y))

                // True Directional Item Shadow on Weathered Rock
                drawItemDirectionalShadow(receiverY = yTop, baseShadowColor = Color(0xFF090E17))
            }
        }

        // 4. 3D CUBE PROJECTION & RENDERING
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

        val projected = rawVertices.map { projectPoint(it) }

        val facesData = listOf(
            Triple(listOf(0, 3, 2, 1), Vec3D(0f, 0f, -1f), 0), // Front
            Triple(listOf(7, 4, 5, 6), Vec3D(0f, 0f, 1f), 1),  // Back
            Triple(listOf(4, 7, 3, 0), Vec3D(0f, 1f, 0f), 2),  // Top
            Triple(listOf(1, 2, 6, 5), Vec3D(0f, -1f, 0f), 3), // Bottom
            Triple(listOf(3, 7, 6, 2), Vec3D(1f, 0f, 0f), 4),  // Right
            Triple(listOf(4, 0, 1, 5), Vec3D(-1f, 0f, 0f), 5)  // Left
        )

        val quads = mutableListOf<FaceQuad>()

        for ((indices, rawNorm, faceIdx) in facesData) {
            val p0 = projected[indices[0]]
            val p1 = projected[indices[1]]
            val p2 = projected[indices[2]]
            val p3 = projected[indices[3]]

            val rotNorm = transformNormal(rawNorm)
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
            val diffuse = if (castShadowsActive && dot <= 0.001f) {
                0.0f // In full directional self-shadow (facing away from light source)
            } else {
                0.68f * dot
            }
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

        // Light-Space Depth Map HUD (Sascha Willems diagnostic inspection)
        if (showShadowMapDebug && castShadowsActive) {
            val hudW = 120f
            val hudH = 92f
            val hudX = w - hudW - 14f
            val hudY = 56f

            // Frame
            drawRoundRect(
                color = Color(0xFF070A11).copy(alpha = 0.92f),
                topLeft = Offset(hudX, hudY),
                size = Size(hudW, hudH),
                cornerRadius = androidx.compose.ui.geometry.CornerRadius(6f, 6f)
            )
            drawRoundRect(
                color = Color(0xFF6366F1).copy(alpha = 0.80f),
                topLeft = Offset(hudX, hudY),
                size = Size(hudW, hudH),
                cornerRadius = androidx.compose.ui.geometry.CornerRadius(6f, 6f),
                style = Stroke(width = 1.2f)
            )

            // Orthographic light view projection of cube
            val lccx = hudX + hudW / 2f
            val lccy = hudY + hudH / 2f + 4f
            drawRoundRect(
                brush = Brush.verticalGradient(
                    colors = listOf(Color(0xFFE2E8F0), Color(0xFF64748B), Color(0xFF1E293B))
                ),
                topLeft = Offset(lccx - 24f, lccy - 18f),
                size = Size(48f, 36f),
                cornerRadius = androidx.compose.ui.geometry.CornerRadius(3f, 3f)
            )
            drawRoundRect(
                color = Color(0xFFA5B4FC),
                topLeft = Offset(lccx - 24f, lccy - 18f),
                size = Size(48f, 36f),
                cornerRadius = androidx.compose.ui.geometry.CornerRadius(3f, 3f),
                style = Stroke(width = 1f)
            )

            // Light ray directional vector arrow
            val arrowLen = 22f
            val endPt = Offset(lccx - lightX * arrowLen, lccy + lightZ * arrowLen)
            drawLine(Color(0xFFFBBF24), Offset(lccx, lccy), endPt, strokeWidth = 1.8f)
            drawCircle(Color(0xFFFBBF24), radius = 2.5f, center = endPt)
        }
    }
}

// -------------------------------------------------------------
// EMPTY ASSET GENERATOR SCREEN (STRICTLY EMPTY STATE)
// -------------------------------------------------------------
@Composable
fun EmptyAssetScreen(
    category: AssetCategory,
    onBackToTest: () -> Unit,
    onNavigateToSky: () -> Unit,
    onNavigateToWeather: () -> Unit,
    modifier: Modifier = Modifier
) {
    var selectedEmptyTab by remember { mutableIntStateOf(0) }

    Column(
        modifier = modifier
            .fillMaxSize()
            .background(Color(0xFF0B0D13))
            .testTag("empty_asset_screen_${category.id}")
    ) {
        // 1. TOP VIEWPORT: ISOMETRIC BLUEPRINT LATTICE & EMPTY ASSET CONTAINER (weight 1.08f)
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .weight(1.08f)
                .background(Color(0xFF07090E))
                .testTag("empty_asset_viewport")
        ) {
            Canvas(modifier = Modifier.fillMaxSize()) {
                val w = size.width
                val h = size.height
                val center = Offset(w / 2f, h / 2f)

                // Background gradient
                drawRect(
                    brush = Brush.radialGradient(
                        colors = listOf(Color(0xFF0F172A), Color(0xFF04060A)),
                        center = center,
                        radius = w * 0.75f
                    )
                )

                // Isometric coordinate lattice grid
                val step = 32f
                val gridColor = Color(0xFF1E293B).copy(alpha = 0.45f)
                val accentGrid = Color(0xFF334155).copy(alpha = 0.7f)

                var x = 0f
                while (x < w) {
                    drawLine(if ((x / step).toInt() % 4 == 0) accentGrid else gridColor, Offset(x, 0f), Offset(x, h), strokeWidth = 1f)
                    x += step
                }
                var y = 0f
                while (y < h) {
                    drawLine(if ((y / step).toInt() % 4 == 0) accentGrid else gridColor, Offset(0f, y), Offset(w, y), strokeWidth = 1f)
                    y += step
                }

                // Empty bounding box wireframe
                val boxRadius = minOf(w, h) * 0.28f
                val dashEffect = PathEffect.dashPathEffect(floatArrayOf(12f, 8f), 0f)

                // Top diamond
                val topPath = Path().apply {
                    moveTo(center.x, center.y - boxRadius * 0.8f)
                    lineTo(center.x + boxRadius, center.y - boxRadius * 0.2f)
                    lineTo(center.x, center.y + boxRadius * 0.4f)
                    lineTo(center.x - boxRadius, center.y - boxRadius * 0.2f)
                    close()
                }
                drawPath(topPath, color = Color(0xFF38BDF8).copy(alpha = 0.35f), style = Stroke(width = 1.5f, pathEffect = dashEffect))

                // Vertical posts
                val postColor = Color(0xFF64748B).copy(alpha = 0.4f)
                drawLine(postColor, Offset(center.x, center.y - boxRadius * 0.8f), Offset(center.x, center.y - boxRadius * 0.1f), strokeWidth = 1.5f)
                drawLine(postColor, Offset(center.x + boxRadius, center.y - boxRadius * 0.2f), Offset(center.x + boxRadius, center.y + boxRadius * 0.5f), strokeWidth = 1.5f)
                drawLine(postColor, Offset(center.x - boxRadius, center.y - boxRadius * 0.2f), Offset(center.x - boxRadius, center.y + boxRadius * 0.5f), strokeWidth = 1.5f)
                drawLine(postColor, Offset(center.x, center.y + boxRadius * 0.4f), Offset(center.x, center.y + boxRadius * 1.1f), strokeWidth = 1.5f)

                // Bottom diamond
                val botPath = Path().apply {
                    moveTo(center.x, center.y - boxRadius * 0.1f)
                    lineTo(center.x + boxRadius, center.y + boxRadius * 0.5f)
                    lineTo(center.x, center.y + boxRadius * 1.1f)
                    lineTo(center.x - boxRadius, center.y + boxRadius * 0.5f)
                    close()
                }
                drawPath(botPath, color = Color(0xFF475569).copy(alpha = 0.3f), style = Stroke(width = 1.5f, pathEffect = dashEffect))

                // Center crosshair
                drawLine(Color(0xFFEF4444).copy(alpha = 0.6f), Offset(center.x - 24f, center.y), Offset(center.x + 24f, center.y), strokeWidth = 2f)
                drawLine(Color(0xFF10B981).copy(alpha = 0.6f), Offset(center.x, center.y - 24f), Offset(center.x, center.y + 24f), strokeWidth = 2f)
            }

            // Floating Top HUD
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(12.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = Color(0xCC0F172A),
                    border = CardDefaults.outlinedCardBorder()
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Icon(
                            imageVector = category.icon,
                            contentDescription = null,
                            tint = Color(0xFF94A3B8),
                            modifier = Modifier.size(14.dp)
                        )
                        Spacer(modifier = Modifier.width(6.dp))
                        Text(
                            text = "native_assets/src/${category.id}/",
                            style = MaterialTheme.typography.labelSmall,
                            color = Color(0xFF94A3B8),
                            fontFamily = FontFamily.Monospace
                        )
                    }
                }

                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = Color(0xCC1E293B)
                ) {
                    Text(
                        text = "EMPTY DIRECTORY",
                        style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold),
                        color = Color(0xFFF59E0B),
                        modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                        fontFamily = FontFamily.Monospace
                    )
                }
            }

            // Center Blueprint Label Overlay
            Card(
                colors = CardDefaults.cardColors(containerColor = Color(0xDD0D111A)),
                shape = RoundedCornerShape(12.dp),
                border = CardDefaults.outlinedCardBorder().copy(
                    brush = Brush.linearGradient(listOf(Color(0xFF1E293B), Color(0xFF334155)))
                ),
                modifier = Modifier
                    .align(Alignment.Center)
                    .padding(24.dp)
            ) {
                Column(
                    modifier = Modifier.padding(16.dp),
                    horizontalAlignment = Alignment.CenterHorizontally
                ) {
                    Icon(
                        imageVector = category.icon,
                        contentDescription = null,
                        tint = Color(0xFF64748B),
                        modifier = Modifier.size(36.dp)
                    )
                    Spacer(modifier = Modifier.height(8.dp))
                    Text(
                        text = "${category.title} Generator",
                        style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                        color = Color.White
                    )
                    Spacer(modifier = Modifier.height(4.dp))
                    Text(
                        text = "Directory strictly empty — awaiting resource definitions",
                        style = MaterialTheme.typography.bodySmall,
                        color = Color(0xFF94A3B8),
                        textAlign = TextAlign.Center
                    )
                }
            }
        }

        HorizontalDivider(color = Color(0xFF1E293B), thickness = 1.dp)

        // 2. BOTTOM CONTROLS PANEL: SAME PROPORTIONS AS TEST SCREEN (weight 0.92f)
        Surface(
            modifier = Modifier
                .fillMaxWidth()
                .weight(0.92f),
            color = Color(0xFF0F121A)
        ) {
            Column(modifier = Modifier.fillMaxSize()) {
                TabRow(
                    selectedTabIndex = selectedEmptyTab,
                    containerColor = Color(0xFF131722),
                    contentColor = Color.White,
                    indicator = { tabPositions ->
                        TabRowDefaults.SecondaryIndicator(
                            Modifier.tabIndicatorOffset(tabPositions[selectedEmptyTab]),
                            color = Color(0xFF94A3B8),
                            height = 3.dp
                        )
                    }
                ) {
                    Tab(
                        selected = selectedEmptyTab == 0,
                        onClick = { selectedEmptyTab = 0 },
                        text = { Text("Directory Status", fontSize = 12.sp, fontWeight = FontWeight.SemiBold) }
                    )
                    Tab(
                        selected = selectedEmptyTab == 1,
                        onClick = { selectedEmptyTab = 1 },
                        text = { Text("README Specs", fontSize = 12.sp, fontWeight = FontWeight.SemiBold) }
                    )
                    Tab(
                        selected = selectedEmptyTab == 2,
                        onClick = { selectedEmptyTab = 2 },
                        text = { Text("Active Modules", fontSize = 12.sp, fontWeight = FontWeight.SemiBold) }
                    )
                }

                // Tab Body
                Column(
                    modifier = Modifier
                        .fillMaxSize()
                        .verticalScroll(rememberScrollState())
                        .padding(16.dp)
                ) {
                    when (selectedEmptyTab) {
                        0 -> {
                            Card(
                                colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
                                shape = RoundedCornerShape(12.dp)
                            ) {
                                Column(modifier = Modifier.padding(16.dp)) {
                                    Row(
                                        modifier = Modifier.fillMaxWidth(),
                                        horizontalArrangement = Arrangement.SpaceBetween,
                                        verticalAlignment = Alignment.CenterVertically
                                    ) {
                                        Text(
                                            text = "Empty Directory Status",
                                            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                                            color = Color.White
                                        )
                                        Badge(containerColor = Color(0xFF451A03), contentColor = Color(0xFFFBBF24)) {
                                            Text("Awaiting Assets", fontSize = 10.sp, fontWeight = FontWeight.Bold)
                                        }
                                    }
                                    Spacer(modifier = Modifier.height(8.dp))
                                    Text(
                                        text = "As strictly directed, 'native_assets/src/${category.id}/' is kept empty in .gitignore until complete resource definitions and mesh/shader pipelines are provisioned.",
                                        style = MaterialTheme.typography.bodySmall,
                                        color = Color(0xFF94A3B8)
                                    )
                                    Spacer(modifier = Modifier.height(14.dp))
                                    Surface(
                                        shape = RoundedCornerShape(8.dp),
                                        color = Color(0xFF090B10),
                                        border = CardDefaults.outlinedCardBorder()
                                    ) {
                                        Column(modifier = Modifier.padding(12.dp)) {
                                            Text(
                                                text = "• Location: native_assets/src/${category.id}/\n• Status: Excluded in .gitignore (Empty)\n• Active Implemented Subsystem: Atmosphere & Sky (sky/)\n• Calibration Scene: Test Screen (3D Cube & PBR)",
                                                style = MaterialTheme.typography.labelSmall,
                                                color = Color(0xFFCBD5E1),
                                                fontFamily = FontFamily.Monospace,
                                                lineHeight = 18.sp
                                            )
                                        }
                                    }
                                }
                            }
                        }
                        1 -> {
                            Card(
                                colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
                                shape = RoundedCornerShape(12.dp)
                            ) {
                                Column(modifier = Modifier.padding(16.dp)) {
                                    Text(
                                        text = "Subsystem Architectural Role",
                                        style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                                        color = Color.White
                                    )
                                    Spacer(modifier = Modifier.height(8.dp))
                                    Text(
                                        text = category.description,
                                        style = MaterialTheme.typography.bodyMedium,
                                        color = Color(0xFFE2E8F0)
                                    )
                                    Spacer(modifier = Modifier.height(12.dp))
                                    Text(
                                        text = "Reference: Full Procedural Asset Tree in README.md",
                                        style = MaterialTheme.typography.labelSmall,
                                        color = Color(0xFF64748B)
                                    )
                                }
                            }
                        }
                        2 -> {
                            Card(
                                colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
                                shape = RoundedCornerShape(12.dp)
                            ) {
                                Column(modifier = Modifier.padding(16.dp)) {
                                    Text(
                                        text = "Implemented & Active Modules",
                                        style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                                        color = Color.White
                                    )
                                    Spacer(modifier = Modifier.height(8.dp))
                                    Text(
                                        text = "You can navigate to the fully implemented modules via the drawer or buttons below:",
                                        style = MaterialTheme.typography.bodySmall,
                                        color = Color(0xFF94A3B8)
                                    )
                                    Spacer(modifier = Modifier.height(14.dp))
                                    Row(
                                        modifier = Modifier.fillMaxWidth(),
                                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                                    ) {
                                        Surface(
                                            shape = RoundedCornerShape(8.dp),
                                            color = Color(0xFF0369A1).copy(alpha = 0.35f),
                                            border = CardDefaults.outlinedCardBorder().copy(brush = Brush.linearGradient(listOf(Color(0xFF0284C7), Color(0xFF38BDF8)))),
                                            modifier = Modifier.weight(1f).clickable { onNavigateToSky() }
                                        ) {
                                            Row(
                                                modifier = Modifier.padding(10.dp),
                                                verticalAlignment = Alignment.CenterVertically,
                                                horizontalArrangement = Arrangement.Center
                                            ) {
                                                Icon(Icons.Outlined.WbSunny, contentDescription = null, tint = Color(0xFF38BDF8), modifier = Modifier.size(16.dp))
                                                Spacer(modifier = Modifier.width(6.dp))
                                                Text("Sky", color = Color.White, style = MaterialTheme.typography.labelMedium, fontWeight = FontWeight.Bold)
                                            }
                                        }

                                        Surface(
                                            shape = RoundedCornerShape(8.dp),
                                            color = Color(0xFF0891B2).copy(alpha = 0.35f),
                                            border = CardDefaults.outlinedCardBorder().copy(brush = Brush.linearGradient(listOf(Color(0xFF06B6D4), Color(0xFF38BDF8)))),
                                            modifier = Modifier.weight(1f).clickable { onNavigateToWeather() }
                                        ) {
                                            Row(
                                                modifier = Modifier.padding(10.dp),
                                                verticalAlignment = Alignment.CenterVertically,
                                                horizontalArrangement = Arrangement.Center
                                            ) {
                                                Icon(Icons.Outlined.Cloud, contentDescription = null, tint = Color(0xFF38BDF8), modifier = Modifier.size(16.dp))
                                                Spacer(modifier = Modifier.width(6.dp))
                                                Text("Weather", color = Color.White, style = MaterialTheme.typography.labelMedium, fontWeight = FontWeight.Bold)
                                            }
                                        }

                                        Surface(
                                            shape = RoundedCornerShape(8.dp),
                                            color = Color(0xFF1E2638),
                                            border = CardDefaults.outlinedCardBorder(),
                                            modifier = Modifier.weight(1f).clickable { onBackToTest() }
                                        ) {
                                            Row(
                                                modifier = Modifier.padding(10.dp),
                                                verticalAlignment = Alignment.CenterVertically,
                                                horizontalArrangement = Arrangement.Center
                                            ) {
                                                Icon(Icons.Default.ViewInAr, contentDescription = null, tint = MaterialTheme.colorScheme.primary, modifier = Modifier.size(16.dp))
                                                Spacer(modifier = Modifier.width(6.dp))
                                                Text("Test", color = Color.White, style = MaterialTheme.typography.labelMedium, fontWeight = FontWeight.Bold)
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

