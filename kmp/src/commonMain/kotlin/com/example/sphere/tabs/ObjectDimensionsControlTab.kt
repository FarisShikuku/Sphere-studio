package com.example.sphere.tabs

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.DimensionSlider
import com.example.sphere.model.PbrMaterial
import com.example.sphere.viewport.PreviewMeshShape

@Composable
fun ObjectDimensionsControlTab(
    shape: PreviewMeshShape,
    material: PbrMaterial,
    scale: Float,
    widthScale: Float,
    heightScale: Float,
    depthScale: Float,
    onScaleChange: (Float) -> Unit,
    onWidthScaleChange: (Float) -> Unit,
    onHeightScaleChange: (Float) -> Unit,
    onDepthScaleChange: (Float) -> Unit,
    onReset: () -> Unit,
    useRealWorldScale: Boolean = true,
    onToggleRealWorldScale: ((Boolean) -> Unit)? = null,
    uvTilingMultiplierX: Float = 1.0f,
    uvTilingMultiplierY: Float = 1.0f,
    onUvTilingChange: ((Float, Float) -> Unit)? = null,
    modifier: Modifier = Modifier
) {
    val scrollState = rememberScrollState()

    // Base metric dimensions for 1.0x scale (1.0m standard unit cube)
    val (baseW, baseH, baseD) = when (shape) {
        PreviewMeshShape.CUBE -> Triple(1.0f, 1.0f, 1.0f)
        PreviewMeshShape.WALL -> Triple(2.0f, 1.5f, 0.25f)
        PreviewMeshShape.PLANE -> Triple(2.0f, 0.15f, 2.0f)
        PreviewMeshShape.SPHERE -> Triple(1.0f, 1.0f, 1.0f)
    }

    val actualW = baseW * scale * widthScale
    val actualH = baseH * scale * heightScale
    val actualD = baseD * scale * depthScale

    val footprintArea = actualW * actualD
    val volume = actualW * actualH * actualD

    // Physical texture pattern dimensions from Architextures PBR specification
    val patternM = if (material.tileScaleMeters > 0.05f) material.tileScaleMeters else 1.0f
    val patternMm = (patternM * 1000).toInt()

    val tilingU = if (useRealWorldScale) {
        (actualW / patternM) * uvTilingMultiplierX
    } else {
        uvTilingMultiplierX
    }
    val tilingV = if (useRealWorldScale) {
        (if (shape == PreviewMeshShape.PLANE) actualD else actualH) / patternM * uvTilingMultiplierY
    } else {
        uvTilingMultiplierY
    }

    // Density estimate in kg/m³
    val density = when (material.category) {
        "Metals" -> 7850f
        "Masonry" -> 1950f
        "Flooring", "Infrastructure" -> 2300f
        "Nature", "Organics" -> 1200f
        "Refractive" -> 2500f
        else -> 1800f
    }
    val massKg = volume * density

    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(scrollState)
            .padding(14.dp)
            .testTag("object_dimensions_control_tab"),
        verticalArrangement = Arrangement.spacedBy(10.dp)
    ) {
        // 1. Header Card with Active Shape and Reset Action
        Surface(
            color = MaterialTheme.colorScheme.surfaceContainerHigh,
            shape = RoundedCornerShape(10.dp),
            border = BorderStroke(1.dp, MaterialTheme.colorScheme.outlineVariant.copy(alpha = 0.5f))
        ) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(12.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Surface(
                        color = MaterialTheme.colorScheme.primaryContainer,
                        shape = RoundedCornerShape(8.dp),
                        modifier = Modifier.size(36.dp)
                    ) {
                        Box(contentAlignment = Alignment.Center) {
                            Icon(
                                imageVector = when (shape) {
                                    PreviewMeshShape.CUBE -> Icons.Default.ViewInAr
                                    PreviewMeshShape.WALL -> Icons.Default.Layers
                                    PreviewMeshShape.SPHERE -> Icons.Default.Circle
                                    PreviewMeshShape.PLANE -> Icons.Default.SquareFoot
                                },
                                contentDescription = null,
                                tint = MaterialTheme.colorScheme.onPrimaryContainer,
                                modifier = Modifier.size(20.dp)
                            )
                        }
                    }

                    Spacer(Modifier.width(10.dp))

                    Column {
                        Text(
                            text = "${shape.label} Dimensions",
                            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold)
                        )
                        Text(
                            text = "Parametric Metric Real-Time Scaler",
                            style = MaterialTheme.typography.bodySmall.copy(
                                color = MaterialTheme.colorScheme.onSurfaceVariant,
                                fontSize = 11.sp
                            )
                        )
                    }
                }

                FilledTonalButton(
                    onClick = onReset,
                    contentPadding = PaddingValues(horizontal = 10.dp, vertical = 6.dp),
                    colors = ButtonDefaults.filledTonalButtonColors(
                        containerColor = MaterialTheme.colorScheme.surfaceContainerHighest,
                        contentColor = MaterialTheme.colorScheme.primary
                    ),
                    modifier = Modifier.testTag("reset_object_dimensions_btn")
                ) {
                    Icon(Icons.Default.Refresh, contentDescription = null, modifier = Modifier.size(14.dp))
                    Spacer(Modifier.width(4.dp))
                    Text("Reset 1.0x", fontSize = 11.sp, fontWeight = FontWeight.Bold)
                }
            }
        }

        // 2. Live Dimensional Readout Badge
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF0F172A)),
            shape = RoundedCornerShape(8.dp),
            border = BorderStroke(1.dp, Color(0xFF38BDF8).copy(alpha = 0.35f))
        ) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(horizontal = 12.dp, vertical = 8.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Column {
                    Text(
                        text = "BOUNDING METRICS",
                        color = Color(0xFF94A3B8),
                        fontSize = 9.sp,
                        fontWeight = FontWeight.Bold,
                        fontFamily = FontFamily.Monospace
                    )
                    Text(
                        text = "${"%.2f".format(actualW)}m (W) × ${"%.2f".format(actualH)}m (H) × ${"%.2f".format(actualD)}m (D)",
                        color = Color.White,
                        fontSize = 12.sp,
                        fontWeight = FontWeight.Bold,
                        fontFamily = FontFamily.Monospace
                    )
                }

                Surface(
                    color = Color(0xFF38BDF8).copy(alpha = 0.15f),
                    shape = RoundedCornerShape(6.dp),
                    border = BorderStroke(1.dp, Color(0xFF38BDF8).copy(alpha = 0.4f))
                ) {
                    Text(
                        text = "${(scale * 100).toInt()}% SCALE",
                        color = Color(0xFF38BDF8),
                        fontSize = 11.sp,
                        fontWeight = FontWeight.Bold,
                        fontFamily = FontFamily.Monospace,
                        modifier = Modifier.padding(horizontal = 8.dp, vertical = 3.dp)
                    )
                }
            }
        }

        // 3. ARCHITECTURAL PBR REAL-WORLD SCALE & UV TILING BEHAVIOR
        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerHigh),
            shape = RoundedCornerShape(10.dp),
            border = BorderStroke(1.dp, Color(0xFF10B981).copy(alpha = 0.45f)),
            modifier = Modifier.fillMaxWidth().testTag("real_world_pbr_card")
        ) {
            Column(modifier = Modifier.padding(12.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Row(verticalAlignment = Alignment.CenterVertically) {
                        Surface(
                            color = Color(0xFF10B981).copy(alpha = 0.20f),
                            shape = RoundedCornerShape(6.dp),
                            modifier = Modifier.size(28.dp)
                        ) {
                            Box(contentAlignment = Alignment.Center) {
                                Icon(
                                    Icons.Default.Architecture,
                                    contentDescription = null,
                                    tint = Color(0xFF10B981),
                                    modifier = Modifier.size(16.dp)
                                )
                            }
                        }
                        Spacer(Modifier.width(8.dp))
                        Column {
                            Text(
                                text = "REAL-WORLD PBR SCALE (BIM / Architextures)",
                                style = MaterialTheme.typography.labelSmall.copy(
                                    fontFamily = FontFamily.Monospace,
                                    fontWeight = FontWeight.Bold,
                                    color = Color(0xFF10B981)
                                )
                            )
                            Text(
                                text = "Texture Physical Size: ${patternMm} mm (${"%.2f".format(patternM)} m)",
                                style = MaterialTheme.typography.bodySmall.copy(
                                    fontWeight = FontWeight.Bold,
                                    fontSize = 11.sp
                                )
                            )
                        }
                    }

                    if (onToggleRealWorldScale != null) {
                        Switch(
                            checked = useRealWorldScale,
                            onCheckedChange = onToggleRealWorldScale,
                            modifier = Modifier.testTag("toggle_real_world_scale_switch")
                        )
                    }
                }

                Text(
                    text = if (useRealWorldScale) {
                        "When you increase or modify the size of the ${shape.label}, the texture preserves true-to-life component scale (${patternMm}mm) by tiling more units seamlessly across the surface, rather than stretching or distorting."
                    } else {
                        "Fit Mode: Texture is stretched (0..1 UV) across the object geometry."
                    },
                    style = MaterialTheme.typography.bodySmall.copy(
                        color = MaterialTheme.colorScheme.onSurfaceVariant,
                        fontSize = 10.sp,
                        lineHeight = 14.sp
                    )
                )

                // UV Tiling Telemetry Readout
                Surface(
                    color = Color.Black.copy(alpha = 0.45f),
                    shape = RoundedCornerShape(6.dp),
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(
                            text = "REAPPLICATIONS (U × V):",
                            color = Color(0xFF94A3B8),
                            fontSize = 10.sp,
                            fontWeight = FontWeight.Bold,
                            fontFamily = FontFamily.Monospace
                        )
                        val totalRepeats = tilingU * tilingV
                        Text(
                            text = "${"%.2f".format(tilingU)}× × ${"%.2f".format(tilingV)}× (${"%.1f".format(totalRepeats)} tiles)",
                            color = Color(0xFF38BDF8),
                            fontSize = 11.sp,
                            fontWeight = FontWeight.Bold,
                            fontFamily = FontFamily.Monospace
                        )
                    }
                }

                // Fine UV Tiling Multipliers
                if (onUvTilingChange != null) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        FilterChip(
                            selected = uvTilingMultiplierX == 1.0f && uvTilingMultiplierY == 1.0f,
                            onClick = { onUvTilingChange(1.0f, 1.0f) },
                            label = { Text("1.0x Tiling", fontSize = 10.sp) },
                            modifier = Modifier.weight(1f)
                        )
                        FilterChip(
                            selected = uvTilingMultiplierX == 2.0f && uvTilingMultiplierY == 2.0f,
                            onClick = { onUvTilingChange(2.0f, 2.0f) },
                            label = { Text("2.0x Dense", fontSize = 10.sp) },
                            modifier = Modifier.weight(1f)
                        )
                        FilterChip(
                            selected = uvTilingMultiplierX == 0.5f && uvTilingMultiplierY == 0.5f,
                            onClick = { onUvTilingChange(0.5f, 0.5f) },
                            label = { Text("0.5x Macro", fontSize = 10.sp) },
                            modifier = Modifier.weight(1f)
                        )
                    }
                }
            }
        }

        // 4. Quick Architectural Presets for the current object
        Text(
            text = "Architectural Size Presets",
            style = MaterialTheme.typography.labelMedium.copy(fontWeight = FontWeight.Bold)
        )

        Row(
            modifier = Modifier
                .fillMaxWidth()
                .horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            when (shape) {
                PreviewMeshShape.WALL -> {
                    FilterChip(
                        selected = (widthScale == 1.0f && heightScale == 1.0f && depthScale == 1.0f && scale == 1.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(1.0f); onHeightScaleChange(1.0f); onDepthScaleChange(1.0f) },
                        label = { Text("Standard 2.0m×1.5m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 5.0f && heightScale == 2.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(5.0f); onHeightScaleChange(2.0f); onDepthScaleChange(1.0f) },
                        label = { Text("Facade 10m×3m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 25.0f && heightScale == 4.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(25.0f); onHeightScaleChange(4.0f); onDepthScaleChange(2.0f) },
                        label = { Text("Acoustic Barrier 50m×6m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 50.0f && heightScale == 6.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(50.0f); onHeightScaleChange(6.0f); onDepthScaleChange(3.0f) },
                        label = { Text("Megastructure 100m×9m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 60.0f && heightScale == 8.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(60.0f); onHeightScaleChange(8.0f); onDepthScaleChange(4.0f) },
                        label = { Text("Perimeter Wall 120m×12m", fontSize = 11.sp) }
                    )
                }
                PreviewMeshShape.CUBE -> {
                    FilterChip(
                        selected = (widthScale == 1.0f && heightScale == 1.0f && depthScale == 1.0f && scale == 1.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(1.0f); onHeightScaleChange(1.0f); onDepthScaleChange(1.0f) },
                        label = { Text("Standard 1.0m Cube", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 2.0f && heightScale == 2.0f && depthScale == 2.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(2.0f); onHeightScaleChange(2.0f); onDepthScaleChange(2.0f) },
                        label = { Text("2.0m Solid Cube (8m³)", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 10.0f && heightScale == 10.0f && depthScale == 10.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(10.0f); onHeightScaleChange(10.0f); onDepthScaleChange(10.0f) },
                        label = { Text("Building Block 10m³", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 50.0f && heightScale == 50.0f && depthScale == 50.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(50.0f); onHeightScaleChange(50.0f); onDepthScaleChange(50.0f) },
                        label = { Text("District Block 50m³", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 100.0f && heightScale == 100.0f && depthScale == 100.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(100.0f); onHeightScaleChange(100.0f); onDepthScaleChange(100.0f) },
                        label = { Text("Mega Complex 100m³", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 120.0f && heightScale == 120.0f && depthScale == 120.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(120.0f); onHeightScaleChange(120.0f); onDepthScaleChange(120.0f) },
                        label = { Text("Superstructure 120m³ (>100m)", fontSize = 11.sp) }
                    )
                }
                PreviewMeshShape.PLANE -> {
                    FilterChip(
                        selected = (widthScale == 1.0f && depthScale == 1.0f && scale == 1.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(1.0f); onHeightScaleChange(1.0f); onDepthScaleChange(1.0f) },
                        label = { Text("Standard Slab 2m×2m (4m²)", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 7.4f && depthScale == 14.8f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(7.4f); onHeightScaleChange(1.0f); onDepthScaleChange(14.8f) },
                        label = { Text("2-Lane Road 14.8m×29.6m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 5.0f && depthScale == 5.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(5.0f); onHeightScaleChange(1.0f); onDepthScaleChange(5.0f) },
                        label = { Text("Plaza 10m×10m (100m²)", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 25.0f && depthScale == 25.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(25.0f); onHeightScaleChange(1.0f); onDepthScaleChange(25.0f) },
                        label = { Text("Courtyard 50m×50m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 50.0f && depthScale == 50.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(50.0f); onHeightScaleChange(1.0f); onDepthScaleChange(50.0f) },
                        label = { Text("Urban District 100m×100m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 500.0f && depthScale == 500.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(500.0f); onHeightScaleChange(1.0f); onDepthScaleChange(500.0f) },
                        label = { Text("1km × 1km (1 km²)", fontSize = 11.sp) }
                    )
                }
                PreviewMeshShape.SPHERE -> {
                    FilterChip(
                        selected = (scale == 1.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(1.0f); onHeightScaleChange(1.0f); onDepthScaleChange(1.0f) },
                        label = { Text("Standard Ø 1.0m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 10.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(10.0f); onHeightScaleChange(10.0f); onDepthScaleChange(10.0f) },
                        label = { Text("Dome Ø 10m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 50.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(50.0f); onHeightScaleChange(50.0f); onDepthScaleChange(50.0f) },
                        label = { Text("Biosphere Ø 50m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 100.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(100.0f); onHeightScaleChange(100.0f); onDepthScaleChange(100.0f) },
                        label = { Text("Superdome Ø 100m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 120.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(120.0f); onHeightScaleChange(120.0f); onDepthScaleChange(120.0f) },
                        label = { Text("Megadome Ø 120m (>100m)", fontSize = 11.sp) }
                    )
                }
            }
        }

        Spacer(Modifier.height(4.dp))

        // 4. Parametric Dimension Sliders with support for > 100 meters
        Text(
            text = "Fine Parametric Controls (Scalable to > 100m)",
            style = MaterialTheme.typography.labelMedium.copy(fontWeight = FontWeight.Bold)
        )

        // Uniform Scale
        DimensionSlider(
            label = "Overall Proportional Scale",
            value = scale.coerceIn(0.10f, 10.0f),
            range = 0.10f..10.0f,
            unit = "x",
            accentColor = Color(0xFFF59E0B),
            onValueChange = onScaleChange
        )

        // Width (X)
        DimensionSlider(
            label = when (shape) {
                PreviewMeshShape.WALL -> "Wall Width (X-Span)"
                PreviewMeshShape.PLANE -> "Surface Width (X-Span)"
                PreviewMeshShape.SPHERE -> "Sphere Radius / Width"
                else -> "Width (X-Axis)"
            },
            value = widthScale.coerceIn(0.20f, 500.0f),
            range = 0.20f..500.0f,
            unit = "x  (${"%.2f".format(actualW)}m)",
            accentColor = Color(0xFFEF4444),
            onValueChange = onWidthScaleChange
        )

        // Height (Y)
        if (shape != PreviewMeshShape.SPHERE) {
            DimensionSlider(
                label = when (shape) {
                    PreviewMeshShape.WALL -> "Wall Height (Elevation)"
                    PreviewMeshShape.PLANE -> "Slab Foundation Depth"
                    else -> "Height (Y-Axis)"
                },
                value = heightScale.coerceIn(0.20f, 500.0f),
                range = 0.20f..500.0f,
                unit = "x  (${"%.2f".format(actualH)}m)",
                accentColor = Color(0xFF10B981),
                onValueChange = onHeightScaleChange
            )
        }

        // Depth (Z)
        if (shape == PreviewMeshShape.CUBE || shape == PreviewMeshShape.WALL || shape == PreviewMeshShape.PLANE) {
            DimensionSlider(
                label = when (shape) {
                    PreviewMeshShape.WALL -> "Wall Thickness (Coping & Core)"
                    PreviewMeshShape.PLANE -> "Surface Length (Z-Depth)"
                    else -> "Depth (Z-Axis)"
                },
                value = depthScale.coerceIn(0.20f, 500.0f),
                range = 0.20f..500.0f,
                unit = if (shape == PreviewMeshShape.WALL && actualD < 2.0f) "x  (${(actualD * 1000).toInt()}mm)" else "x  (${"%.2f".format(actualD)}m)",
                accentColor = Color(0xFF3B82F6),
                onValueChange = onDepthScaleChange
            )
        }

        Spacer(Modifier.height(6.dp))

        // 5. Engineering Mass & Volume Card
        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerHigh),
            shape = RoundedCornerShape(10.dp)
        ) {
            Column(modifier = Modifier.padding(12.dp), verticalArrangement = Arrangement.spacedBy(6.dp)) {
                Text(
                    text = "PHYSICAL METRICS & VOLUME CALCULATION",
                    style = MaterialTheme.typography.labelSmall.copy(
                        fontFamily = FontFamily.Monospace,
                        color = MaterialTheme.colorScheme.primary,
                        fontWeight = FontWeight.Bold
                    )
                )

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("Footprint Area:", style = MaterialTheme.typography.bodySmall)
                    val areaStr = if (footprintArea >= 100_000f) {
                        "${"%.0f".format(footprintArea)} m² (${"%.2f".format(footprintArea / 1_000_000f)} km²)"
                    } else {
                        "${"%.2f".format(footprintArea)} m²"
                    }
                    Text(areaStr, style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Bold, fontFamily = FontFamily.Monospace))
                }

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("Total Volume:", style = MaterialTheme.typography.bodySmall)
                    Text("${"%.3f".format(volume)} m³", style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Bold, fontFamily = FontFamily.Monospace))
                }

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("Estimated Physical Mass (${material.category}):", style = MaterialTheme.typography.bodySmall)
                    val massDisplay = if (massKg >= 1000f) "${"%.2f".format(massKg / 1000f)} tonnes" else "${massKg.toInt()} kg"
                    Text(massDisplay, style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Bold, fontFamily = FontFamily.Monospace, color = Color(0xFFF59E0B)))
                }
            }
        }
    }
}
