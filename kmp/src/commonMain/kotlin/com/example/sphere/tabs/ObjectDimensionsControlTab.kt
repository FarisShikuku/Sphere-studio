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

    // Base metric dimensions for 1.0x scale
    val (baseW, baseH, baseD) = when (shape) {
        PreviewMeshShape.CUBE -> Triple(1.92f, 1.92f, 1.92f)
        PreviewMeshShape.WALL -> Triple(1.85f, 1.45f, 0.34f)
        PreviewMeshShape.PLANE -> Triple(3.40f, 0.22f, 3.40f)
        PreviewMeshShape.SPHERE -> Triple(1.60f, 1.60f, 1.60f)
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
                            text = "CALCULATED UV TILING:",
                            color = Color(0xFF94A3B8),
                            fontSize = 10.sp,
                            fontWeight = FontWeight.Bold,
                            fontFamily = FontFamily.Monospace
                        )
                        Text(
                            text = "U: ${"%.2f".format(tilingU)}x  •  V: ${"%.2f".format(tilingV)}x",
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
                        label = { Text("Standard 1.85m×1.45m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 1.5f && heightScale == 1.6f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(1.5f); onHeightScaleChange(1.6f); onDepthScaleChange(1.0f) },
                        label = { Text("High Facade 2.8m×2.3m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 1.2f && heightScale == 0.7f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(1.2f); onHeightScaleChange(0.7f); onDepthScaleChange(1.3f) },
                        label = { Text("Garden Parapet 2.2m×1.0m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 2.0f && heightScale == 1.8f && depthScale == 1.8f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(2.0f); onHeightScaleChange(1.8f); onDepthScaleChange(1.8f) },
                        label = { Text("Retaining Wall 3.7m×2.6m", fontSize = 11.sp) }
                    )
                }
                PreviewMeshShape.CUBE -> {
                    FilterChip(
                        selected = (widthScale == 1.0f && heightScale == 1.0f && depthScale == 1.0f && scale == 1.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(1.0f); onHeightScaleChange(1.0f); onDepthScaleChange(1.0f) },
                        label = { Text("Standard 1.0m Cube", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (scale == 1.6f && widthScale == 1.0f && heightScale == 1.0f),
                        onClick = { onScaleChange(1.6f); onWidthScaleChange(1.0f); onHeightScaleChange(1.0f); onDepthScaleChange(1.0f) },
                        label = { Text("Large 1.6m Block", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 0.6f && heightScale == 1.8f && depthScale == 0.6f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(0.6f); onHeightScaleChange(1.8f); onDepthScaleChange(0.6f) },
                        label = { Text("Pillar Column (0.6m×1.8m)", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 1.8f && heightScale == 0.5f && depthScale == 1.4f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(1.8f); onHeightScaleChange(0.5f); onDepthScaleChange(1.4f) },
                        label = { Text("Display Plinth", fontSize = 11.sp) }
                    )
                }
                PreviewMeshShape.PLANE -> {
                    FilterChip(
                        selected = (widthScale == 1.0f && depthScale == 1.0f && scale == 1.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(1.0f); onHeightScaleChange(1.0f); onDepthScaleChange(1.0f) },
                        label = { Text("Room Floor 3.4m×3.4m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 1.6f && depthScale == 1.6f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(1.6f); onHeightScaleChange(1.0f); onDepthScaleChange(1.6f) },
                        label = { Text("Grand Hall 5.4m×5.4m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 0.6f && depthScale == 1.8f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(0.6f); onHeightScaleChange(1.0f); onDepthScaleChange(1.8f) },
                        label = { Text("Walkway / Path 2.0m×6.0m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (widthScale == 1.4f && depthScale == 1.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(1.4f); onHeightScaleChange(1.0f); onDepthScaleChange(1.0f) },
                        label = { Text("Patio Slab 4.8m×3.4m", fontSize = 11.sp) }
                    )
                }
                PreviewMeshShape.SPHERE -> {
                    FilterChip(
                        selected = (scale == 0.75f),
                        onClick = { onScaleChange(0.75f); onWidthScaleChange(1.0f); onHeightScaleChange(1.0f); onDepthScaleChange(1.0f) },
                        label = { Text("Compact Ø 1.2m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (scale == 1.0f),
                        onClick = { onScaleChange(1.0f); onWidthScaleChange(1.0f); onHeightScaleChange(1.0f); onDepthScaleChange(1.0f) },
                        label = { Text("Standard Ø 1.6m", fontSize = 11.sp) }
                    )
                    FilterChip(
                        selected = (scale == 1.45f),
                        onClick = { onScaleChange(1.45f); onWidthScaleChange(1.0f); onHeightScaleChange(1.0f); onDepthScaleChange(1.0f) },
                        label = { Text("Monumental Ø 2.3m", fontSize = 11.sp) }
                    )
                }
            }
        }

        Spacer(Modifier.height(4.dp))

        // 4. Parametric Dimension Sliders
        Text(
            text = "Fine Parametric Controls",
            style = MaterialTheme.typography.labelMedium.copy(fontWeight = FontWeight.Bold)
        )

        // Uniform Scale
        DimensionSlider(
            label = "Overall Proportional Scale",
            value = scale,
            range = 0.40f..2.50f,
            unit = "x",
            accentColor = Color(0xFFF59E0B),
            onValueChange = onScaleChange
        )

        // Width (X)
        DimensionSlider(
            label = when (shape) {
                PreviewMeshShape.WALL -> "Wall Width (X-Span)"
                PreviewMeshShape.PLANE -> "Floor Width (X-Span)"
                PreviewMeshShape.SPHERE -> "Sphere Radius / Width"
                else -> "Width (X-Axis)"
            },
            value = widthScale,
            range = 0.40f..2.50f,
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
                value = heightScale,
                range = 0.35f..2.50f,
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
                    PreviewMeshShape.PLANE -> "Floor Length (Z-Depth)"
                    else -> "Depth (Z-Axis)"
                },
                value = depthScale,
                range = 0.35f..2.50f,
                unit = if (shape == PreviewMeshShape.WALL) "x  (${(actualD * 1000).toInt()}mm)" else "x  (${"%.2f".format(actualD)}m)",
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
                    Text("${"%.2f".format(footprintArea)} m²", style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Bold, fontFamily = FontFamily.Monospace))
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
