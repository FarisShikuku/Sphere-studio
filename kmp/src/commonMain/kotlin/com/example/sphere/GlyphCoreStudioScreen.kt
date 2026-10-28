package com.example.sphere

import androidx.activity.compose.BackHandler
import androidx.compose.animation.AnimatedVisibility
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.*
import com.example.sphere.text.GlyphCoreViewportCanvas
import com.example.sphere.text.tabs.*
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive

/**
 * GlyphCoreStudioScreen - Proprietary Vulkan SDF Typography & TextOverlay Studio.
 *
 * Implements the architecture from Sascha Willems Vulkan textoverlay and distancefieldfonts:
 *  - Signed Distance Field (SDF) rasterization with smoothstep subpixel antialiasing
 *  - Dynamic outline width, color, and stroke dilation
 *  - Secondary distance field drop shadow with offset and feathering
 *  - 2D HUD Screen Overlay vs 3D World Space Billboard rendering
 *  - 1024x1024 SDF Font Atlas inspection and typographic metrics
 *  - Live C++ Vulkan pipeline code and GLSL shader code preview
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun GlyphCoreStudioScreen(
    onNavigateBack: () -> Unit,
    modifier: Modifier = Modifier
) {
    BackHandler {
        onNavigateBack()
    }

    var selectedPreset by remember { mutableStateOf(defaultTextPresets[0]) }
    var selectedTab by remember { mutableIntStateOf(0) }
    val tabTitles = listOf("Typography", "SDF & Shading", "3D & Billboard", "Font Atlas", "Vulkan C++")

    // Typography State
    var textContent by remember { mutableStateOf(selectedPreset.text) }
    var selectedFont by remember { mutableStateOf(selectedPreset.font) }
    var renderMode by remember { mutableStateOf(selectedPreset.renderMode) }
    var alignment by remember { mutableStateOf(selectedPreset.alignment) }
    var fontSize by remember { mutableFloatStateOf(selectedPreset.fontSize) }
    var letterSpacing by remember { mutableFloatStateOf(selectedPreset.letterSpacing) }
    var lineHeight by remember { mutableFloatStateOf(selectedPreset.lineHeight) }
    var textColor by remember { mutableStateOf(selectedPreset.textColor) }

    // SDF & Shading State
    var smoothing by remember { mutableFloatStateOf(selectedPreset.smoothing) }
    var thicknessBias by remember { mutableFloatStateOf(selectedPreset.thicknessBias) }
    var enableOutline by remember { mutableStateOf(selectedPreset.enableOutline) }
    var outlineWidth by remember { mutableFloatStateOf(selectedPreset.outlineWidth) }
    var outlineColor by remember { mutableStateOf(selectedPreset.outlineColor) }
    var enableShadow by remember { mutableStateOf(selectedPreset.enableShadow) }
    var shadowOffsetX by remember { mutableFloatStateOf(selectedPreset.shadowOffsetX) }
    var shadowOffsetY by remember { mutableFloatStateOf(selectedPreset.shadowOffsetY) }
    var shadowSoftness by remember { mutableFloatStateOf(selectedPreset.shadowSoftness) }
    var shadowColor by remember { mutableStateOf(selectedPreset.shadowColor) }
    var enableGlow by remember { mutableStateOf(selectedPreset.enableGlow) }
    var glowRadius by remember { mutableFloatStateOf(selectedPreset.glowRadius) }
    var glowColor by remember { mutableStateOf(selectedPreset.glowColor) }

    // 3D & World Billboard State
    var zoomFactor by remember { mutableFloatStateOf(1.0f) }
    var rotX by remember { mutableFloatStateOf(15f) }
    var rotY by remember { mutableFloatStateOf(-20f) }
    var autoRotate by remember { mutableStateOf(false) }
    var badgeStyle by remember { mutableStateOf(selectedPreset.badgeStyle) }

    // Atlas & Metrics State
    var showAtlasView by remember { mutableStateOf(false) }
    var showMetrics by remember { mutableStateOf(false) }

    // Auto-orbit camera loop
    LaunchedEffect(autoRotate) {
        if (!autoRotate) return@LaunchedEffect
        while (isActive) {
            delay(16)
            rotY = (rotY + 0.4f) % 360f
        }
    }

    Scaffold(
        topBar = {
            TopAppBar(
                title = {
                    Column {
                        Text(
                            "GlyphCore Studio",
                            style = MaterialTheme.typography.titleMedium.copy(fontWeight = FontWeight.Bold)
                        )
                        Text(
                            "Vulkan SDF Typography & TextOverlay Engine",
                            style = MaterialTheme.typography.bodySmall.copy(
                                color = MaterialTheme.colorScheme.onSurfaceVariant,
                                fontSize = 11.sp
                            )
                        )
                    }
                },
                navigationIcon = {
                    IconButton(
                        onClick = onNavigateBack,
                        modifier = Modifier.testTag("glyphcore_back_button")
                    ) {
                        Icon(Icons.AutoMirrored.Filled.ArrowBack, contentDescription = "Back to Scene")
                    }
                },
                actions = {
                    IconButton(
                        onClick = { showAtlasView = !showAtlasView },
                        modifier = Modifier.testTag("toggle_atlas_header_button")
                    ) {
                        Icon(
                            if (showAtlasView) Icons.Default.ViewInAr else Icons.Default.GridOn,
                            contentDescription = "Toggle Atlas View",
                            tint = if (showAtlasView) Color(0xFFFFA726) else MaterialTheme.colorScheme.onSurface
                        )
                    }
                    IconButton(
                        onClick = {
                            // Reset to active preset defaults
                            textContent = selectedPreset.text
                            selectedFont = selectedPreset.font
                            renderMode = selectedPreset.renderMode
                            alignment = selectedPreset.alignment
                            fontSize = selectedPreset.fontSize
                            letterSpacing = selectedPreset.letterSpacing
                            lineHeight = selectedPreset.lineHeight
                            textColor = selectedPreset.textColor
                            smoothing = selectedPreset.smoothing
                            thicknessBias = selectedPreset.thicknessBias
                            enableOutline = selectedPreset.enableOutline
                            outlineWidth = selectedPreset.outlineWidth
                            outlineColor = selectedPreset.outlineColor
                            enableShadow = selectedPreset.enableShadow
                            shadowOffsetX = selectedPreset.shadowOffsetX
                            shadowOffsetY = selectedPreset.shadowOffsetY
                            shadowSoftness = selectedPreset.shadowSoftness
                            shadowColor = selectedPreset.shadowColor
                            enableGlow = selectedPreset.enableGlow
                            glowRadius = selectedPreset.glowRadius
                            glowColor = selectedPreset.glowColor
                            badgeStyle = selectedPreset.badgeStyle
                            zoomFactor = 1.0f
                            rotX = 15f
                            rotY = -20f
                        },
                        modifier = Modifier.testTag("reset_preset_button")
                    ) {
                        Icon(Icons.Default.RestartAlt, contentDescription = "Reset Settings")
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
        ) {
            // VIEWPORT AREA
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .weight(1.15f)
                    .background(Color(0xFF0A0C10))
            ) {
                GlyphCoreViewportCanvas(
                    text = textContent,
                    font = selectedFont,
                    renderMode = renderMode,
                    alignment = alignment,
                    fontSize = fontSize,
                    letterSpacing = letterSpacing,
                    lineHeight = lineHeight,
                    textColor = textColor,
                    enableOutline = enableOutline,
                    outlineWidth = outlineWidth,
                    outlineColor = outlineColor,
                    enableShadow = enableShadow,
                    shadowOffsetX = shadowOffsetX,
                    shadowOffsetY = shadowOffsetY,
                    shadowSoftness = shadowSoftness,
                    shadowColor = shadowColor,
                    smoothing = smoothing,
                    thicknessBias = thicknessBias,
                    enableGlow = enableGlow,
                    glowRadius = glowRadius,
                    glowColor = glowColor,
                    badgeStyle = badgeStyle,
                    showAtlasView = showAtlasView,
                    showMetrics = showMetrics,
                    rotX = rotX,
                    rotY = rotY,
                    zoomFactor = zoomFactor,
                    onDrag = { dx, dy ->
                        rotY += dx * 0.5f
                        rotX = (rotX - dy * 0.5f).coerceIn(-45f, 75f)
                    }
                )
            }

            // CONTROLS TAB ROW
            ScrollableTabRow(
                selectedTabIndex = selectedTab,
                edgePadding = 12.dp,
                modifier = Modifier.fillMaxWidth()
            ) {
                tabTitles.forEachIndexed { index, title ->
                    Tab(
                        selected = selectedTab == index,
                        onClick = { selectedTab = index },
                        text = {
                            Text(
                                title,
                                style = MaterialTheme.typography.labelMedium.copy(
                                    fontWeight = if (selectedTab == index) FontWeight.Bold else FontWeight.Normal
                                )
                            )
                        }
                    )
                }
            }

            // CONTROLS CONTENT AREA
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .weight(1.0f)
                    .background(MaterialTheme.colorScheme.background)
            ) {
                when (selectedTab) {
                    0 -> TypographyContentTab(
                        text = textContent,
                        onTextChange = { textContent = it },
                        font = selectedFont,
                        onFontChange = { selectedFont = it },
                        fontSize = fontSize,
                        onFontSizeChange = { fontSize = it },
                        letterSpacing = letterSpacing,
                        onLetterSpacingChange = { letterSpacing = it },
                        lineHeight = lineHeight,
                        onLineHeightChange = { lineHeight = it },
                        alignment = alignment,
                        onAlignmentChange = { alignment = it },
                        textColor = textColor,
                        onTextColorChange = { textColor = it },
                        availablePresets = defaultTextPresets,
                        onSelectPreset = { preset ->
                            selectedPreset = preset
                            textContent = preset.text
                            selectedFont = preset.font
                            renderMode = preset.renderMode
                            alignment = preset.alignment
                            fontSize = preset.fontSize
                            letterSpacing = preset.letterSpacing
                            lineHeight = preset.lineHeight
                            textColor = preset.textColor
                            smoothing = preset.smoothing
                            thicknessBias = preset.thicknessBias
                            enableOutline = preset.enableOutline
                            outlineWidth = preset.outlineWidth
                            outlineColor = preset.outlineColor
                            enableShadow = preset.enableShadow
                            shadowOffsetX = preset.shadowOffsetX
                            shadowOffsetY = preset.shadowOffsetY
                            shadowSoftness = preset.shadowSoftness
                            shadowColor = preset.shadowColor
                            enableGlow = preset.enableGlow
                            glowRadius = preset.glowRadius
                            glowColor = preset.glowColor
                            badgeStyle = preset.badgeStyle
                        }
                    )
                    1 -> SdfShadingTab(
                        smoothing = smoothing,
                        onSmoothingChange = { smoothing = it },
                        thicknessBias = thicknessBias,
                        onThicknessBiasChange = { thicknessBias = it },
                        enableOutline = enableOutline,
                        onToggleOutline = { enableOutline = it },
                        outlineWidth = outlineWidth,
                        onOutlineWidthChange = { outlineWidth = it },
                        outlineColor = outlineColor,
                        onOutlineColorChange = { outlineColor = it },
                        enableShadow = enableShadow,
                        onToggleShadow = { enableShadow = it },
                        shadowOffsetX = shadowOffsetX,
                        onShadowOffsetXChange = { shadowOffsetX = it },
                        shadowOffsetY = shadowOffsetY,
                        onShadowOffsetYChange = { shadowOffsetY = it },
                        shadowSoftness = shadowSoftness,
                        onShadowSoftnessChange = { shadowSoftness = it },
                        shadowColor = shadowColor,
                        onShadowColorChange = { shadowColor = it },
                        enableGlow = enableGlow,
                        onToggleGlow = { enableGlow = it },
                        glowRadius = glowRadius,
                        onGlowRadiusChange = { glowRadius = it },
                        glowColor = glowColor,
                        onGlowColorChange = { glowColor = it }
                    )
                    2 -> WorldAndBillboardTab(
                        renderMode = renderMode,
                        onRenderModeChange = { renderMode = it },
                        zoomFactor = zoomFactor,
                        onZoomFactorChange = { zoomFactor = it },
                        rotX = rotX,
                        onRotXChange = { rotX = it },
                        rotY = rotY,
                        onRotYChange = { rotY = it },
                        autoRotate = autoRotate,
                        onToggleAutoRotate = { autoRotate = it },
                        badgeStyle = badgeStyle,
                        onBadgeStyleChange = { badgeStyle = it },
                        onResetCamera = {
                            rotX = 15f
                            rotY = -20f
                            zoomFactor = 1.0f
                        }
                    )
                    3 -> FontAtlasMetricsTab(
                        font = selectedFont,
                        showAtlasView = showAtlasView,
                        onToggleAtlasView = { showAtlasView = it },
                        showMetrics = showMetrics,
                        onToggleMetrics = { showMetrics = it }
                    )
                    4 -> VulkanNativeCodeTab(
                        renderMode = renderMode,
                        smoothing = smoothing,
                        thicknessBias = thicknessBias,
                        enableOutline = enableOutline,
                        outlineWidth = outlineWidth,
                        enableShadow = enableShadow,
                        shadowOffsetX = shadowOffsetX,
                        shadowOffsetY = shadowOffsetY,
                        enableGlow = enableGlow,
                        glowRadius = glowRadius
                    )
                }
            }
        }
    }
}
