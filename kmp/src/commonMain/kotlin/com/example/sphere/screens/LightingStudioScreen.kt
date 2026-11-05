package com.example.sphere.screens

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
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
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import com.example.sphere.lighting.LightingViewportCanvas
import com.example.sphere.lighting.tabs.*
import com.example.sphere.model.*

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun LightingStudioScreen(
    onNavigateBack: () -> Unit,
    modifier: Modifier = Modifier
) {
    var lightParams by remember {
        mutableStateOf(
            DEFAULT_LIGHTING_SCENARIOS.first { it.id == LightScenarioPresetId.TACTICAL_FLASHLIGHT }.params
        )
    }
    var sdfGiParams by remember { mutableStateOf(SdfGiParameters()) }
    var atmosphereParams by remember { mutableStateOf(AtmosphereParameters()) }
    var cascadedShadowParams by remember { mutableStateOf(CascadedShadowParameters()) }
    var debugViewMode by remember { mutableStateOf(PipelineDebugViewMode.FINAL_LIT) }

    var selectedScenarioId by remember { mutableStateOf(LightScenarioPresetId.TACTICAL_FLASHLIGHT.name) }
    var selectedMesh by remember { mutableStateOf(TargetLightingMesh.BOX_CUBE) }
    var selectedTab by remember { mutableStateOf(0) }

    val tabs = listOf("Optics", "SDF GI", "Atmosphere", "Shadows", "3D Gizmo", "Scenarios", "Vulkan C++")

    Scaffold(
        topBar = {
            TopAppBar(
                title = {
                    Column {
                        Text("Lighting Studio", style = MaterialTheme.typography.titleMedium, fontWeight = FontWeight.Bold)
                        Text(
                            "PlainRenderer Vulkan Engine (SDF Diffuse GI, Atmosphere, CSM)",
                            style = MaterialTheme.typography.labelSmall,
                            color = MaterialTheme.colorScheme.primary
                        )
                    }
                },
                navigationIcon = {
                    IconButton(onClick = onNavigateBack, modifier = Modifier.testTag("lighting_back_button")) {
                        Icon(Icons.AutoMirrored.Filled.ArrowBack, contentDescription = "Back")
                    }
                },
                actions = {
                    IconButton(
                        onClick = {
                            val preset = DEFAULT_LIGHTING_SCENARIOS.first { it.id == LightScenarioPresetId.TACTICAL_FLASHLIGHT }
                            lightParams = preset.params
                            sdfGiParams = preset.sdfGi
                            atmosphereParams = preset.atmosphere
                            selectedScenarioId = preset.id.name
                            debugViewMode = PipelineDebugViewMode.FINAL_LIT
                        },
                        modifier = Modifier.testTag("lighting_reset_button")
                    ) {
                        Icon(Icons.Default.RestartAlt, contentDescription = "Reset Light")
                    }
                },
                colors = TopAppBarDefaults.topAppBarColors(
                    containerColor = MaterialTheme.colorScheme.surface
                )
            )
        },
        modifier = modifier
    ) { innerPadding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(innerPadding)
        ) {
            // Quick Selectors Bar (Type, Target Mesh, Pipeline Debug View)
            Surface(
                color = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.35f),
                modifier = Modifier.fillMaxWidth()
            ) {
                Column(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(horizontal = 12.dp, vertical = 6.dp),
                    verticalArrangement = Arrangement.spacedBy(6.dp)
                ) {
                    // Light Type Quick Selector
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .horizontalScroll(rememberScrollState()),
                        horizontalArrangement = Arrangement.spacedBy(6.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text("Type:", style = MaterialTheme.typography.labelSmall, fontWeight = FontWeight.Bold)
                        LightSourceType.values().forEach { type ->
                            FilterChip(
                                selected = lightParams.type == type,
                                onClick = {
                                    lightParams = lightParams.copy(type = type)
                                },
                                label = {
                                    Text(
                                        text = when (type) {
                                            LightSourceType.SPOT_FLASHLIGHT -> "🔦 Flashlight"
                                            LightSourceType.POINT_BULB -> "💡 Bulb"
                                            LightSourceType.AREA_SOFTBOX -> "◻️ Area"
                                            LightSourceType.DIRECTIONAL_SUN -> "☀️ Sun"
                                        },
                                        style = MaterialTheme.typography.labelSmall
                                    )
                                },
                                modifier = Modifier.height(32.dp).testTag("chip_type_${type.name}")
                            )
                        }
                    }

                    // Target Mesh Quick Selector
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .horizontalScroll(rememberScrollState()),
                        horizontalArrangement = Arrangement.spacedBy(6.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text("Mesh:", style = MaterialTheme.typography.labelSmall, fontWeight = FontWeight.Bold)
                        TargetLightingMesh.values().forEach { mesh ->
                            FilterChip(
                                selected = selectedMesh == mesh,
                                onClick = { selectedMesh = mesh },
                                label = { Text(mesh.displayName, style = MaterialTheme.typography.labelSmall) },
                                modifier = Modifier.height(32.dp).testTag("chip_mesh_${mesh.name}")
                            )
                        }
                    }

                    // Multi-Pass Debug View Selector (PlainRenderer passes)
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .horizontalScroll(rememberScrollState()),
                        horizontalArrangement = Arrangement.spacedBy(6.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text("Pass:", style = MaterialTheme.typography.labelSmall, fontWeight = FontWeight.Bold)
                        PipelineDebugViewMode.values().forEach { mode ->
                            FilterChip(
                                selected = debugViewMode == mode,
                                onClick = { debugViewMode = mode },
                                label = { Text(mode.displayName, style = MaterialTheme.typography.labelSmall) },
                                modifier = Modifier.height(32.dp).testTag("chip_pass_${mode.name}")
                            )
                        }
                    }
                }
            }

            // 3D Viewport Canvas
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(230.dp)
                    .padding(horizontal = 12.dp, vertical = 6.dp)
                    .clip(RoundedCornerShape(12.dp))
                    .background(Color(0xFF0F141A))
            ) {
                LightingViewportCanvas(
                    targetMesh = selectedMesh,
                    lightParams = lightParams,
                    debugViewMode = debugViewMode,
                    sdfGiParams = sdfGiParams,
                    atmosphereParams = atmosphereParams,
                    cascadedShadowParams = cascadedShadowParams,
                    modifier = Modifier.fillMaxSize()
                )

                // Overlay Info Badge
                Surface(
                    shape = RoundedCornerShape(6.dp),
                    color = Color.Black.copy(alpha = 0.70f),
                    modifier = Modifier
                        .align(Alignment.BottomEnd)
                        .padding(8.dp)
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                        horizontalArrangement = Arrangement.spacedBy(6.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Box(
                            modifier = Modifier
                                .size(8.dp)
                                .clip(RoundedCornerShape(4.dp))
                                .background(lightParams.effectiveColor)
                        )
                        Text(
                            text = "${lightParams.type.displayName} • ${lightParams.intensityLumens.toInt()} lm • ${lightParams.colorTemperatureKelvin.toInt()}K",
                            color = Color.White,
                            style = MaterialTheme.typography.labelSmall
                        )
                    }
                }
            }

            // Scrollable Tabs Row
            ScrollableTabRow(
                selectedTabIndex = selectedTab,
                edgePadding = 12.dp,
                modifier = Modifier.fillMaxWidth().testTag("lighting_tabs_row")
            ) {
                tabs.forEachIndexed { index, title ->
                    Tab(
                        selected = selectedTab == index,
                        onClick = { selectedTab = index },
                        text = { Text(title, style = MaterialTheme.typography.labelMedium) },
                        modifier = Modifier.testTag("studio_tab_$index")
                    )
                }
            }

            // Active Tab Content
            Box(modifier = Modifier.weight(1f)) {
                when (selectedTab) {
                    0 -> LightOpticsControlTab(
                        params = lightParams,
                        onParamsChange = { lightParams = it }
                    )
                    1 -> SdfGiTab(
                        params = sdfGiParams,
                        onParamsChange = { sdfGiParams = it }
                    )
                    2 -> AtmosphereSkyTab(
                        params = atmosphereParams,
                        onParamsChange = { atmosphereParams = it }
                    )
                    3 -> CascadedShadowsTab(
                        params = cascadedShadowParams,
                        onParamsChange = { cascadedShadowParams = it }
                    )
                    4 -> LightGizmoPlacementTab(
                        params = lightParams,
                        onParamsChange = { lightParams = it }
                    )
                    5 -> LightScenariosTab(
                        currentPresetId = selectedScenarioId,
                        onSelectPreset = { preset ->
                            selectedScenarioId = preset.id.name
                            lightParams = preset.params
                            sdfGiParams = preset.sdfGi
                            atmosphereParams = preset.atmosphere
                        }
                    )
                    6 -> LightVulkanCodeTab(
                        params = lightParams
                    )
                }
            }
        }
    }
}
