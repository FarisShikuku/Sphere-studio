package com.example.sphere.tabs

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.CheckCircle
import androidx.compose.material.icons.filled.Info
import androidx.compose.material.icons.filled.Layers
import androidx.compose.material.icons.filled.Visibility
import androidx.compose.material.icons.filled.VisibilityOff
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.CompositeSurface
import com.example.sphere.model.PbrMaterial
import com.example.sphere.model.SurfaceLayer

@Composable
fun SurfaceCompositeLayersTab(
    surface: CompositeSurface,
    onSurfaceChange: (CompositeSurface) -> Unit,
    onDrillDownMaterial: (PbrMaterial) -> Unit,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        // Explanatory Banner: Material vs Surface
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = Color(0xFF0F172A)),
            border = BorderStroke(1.dp, Color(0xFF1E293B))
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(6.dp)) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Icon(Icons.Default.Layers, contentDescription = null, tint = Color(0xFF38BDF8), modifier = Modifier.size(20.dp))
                    Spacer(Modifier.width(8.dp))
                    Text(
                        "Surface vs Material Architecture",
                        style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold, color = Color.White)
                    )
                }
                Text(
                    "• Material: A single homogeneous physical matter (e.g. Pure Clay, Cement, Iron, Chlorophyll) with atomic physical constants (IOR, Roughness, SSS).\n" +
                    "• Surface: A complex heterogeneous macro-structure assembled from multiple materials blended via spatial masks (e.g. Brick Blocks + Mortar Joints; Car Paint = Metal + Pigment + Clearcoat).",
                    style = MaterialTheme.typography.bodySmall.copy(color = Color(0xFF94A3B8), lineHeight = 18.sp)
                )
            }
        }

        // Composite Layers Breakdown
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                "Constituent Material Layers (${surface.layers.size})",
                style = MaterialTheme.typography.labelLarge.copy(fontWeight = FontWeight.Bold, color = MaterialTheme.colorScheme.primary)
            )
            Surface(
                color = MaterialTheme.colorScheme.primaryContainer,
                shape = RoundedCornerShape(6.dp)
            ) {
                Text(
                    surface.patternType.label,
                    modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                    style = MaterialTheme.typography.labelSmall.copy(color = MaterialTheme.colorScheme.onPrimaryContainer, fontWeight = FontWeight.Bold)
                )
            }
        }

        // Layer Cards
        surface.layers.forEachIndexed { index, layer ->
            Card(
                shape = RoundedCornerShape(10.dp),
                colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.45f)),
                border = BorderStroke(1.dp, if (layer.isEnabled) MaterialTheme.colorScheme.primary.copy(alpha = 0.4f) else MaterialTheme.colorScheme.outlineVariant)
            ) {
                Column(modifier = Modifier.padding(12.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.SpaceBetween
                    ) {
                        Row(verticalAlignment = Alignment.CenterVertically) {
                            Box(
                                modifier = Modifier
                                    .size(20.dp)
                                    .clip(CircleShape)
                                    .background(layer.material.baseColor)
                            )
                            Spacer(Modifier.width(10.dp))
                            Column {
                                Text(
                                    layer.role,
                                    style = MaterialTheme.typography.labelMedium.copy(fontWeight = FontWeight.Bold)
                                )
                                Text(
                                    "${layer.material.name} (${layer.coveragePercent.toInt()}%)",
                                    style = MaterialTheme.typography.bodySmall.copy(color = MaterialTheme.colorScheme.onSurfaceVariant)
                                )
                            }
                        }

                        Row(verticalAlignment = Alignment.CenterVertically) {
                            IconButton(
                                onClick = {
                                    val updatedLayers = surface.layers.toMutableList()
                                    updatedLayers[index] = layer.copy(isEnabled = !layer.isEnabled)
                                    onSurfaceChange(surface.copy(layers = updatedLayers))
                                },
                                modifier = Modifier.size(32.dp).testTag("toggle_layer_$index")
                            ) {
                                Icon(
                                    if (layer.isEnabled) Icons.Default.Visibility else Icons.Default.VisibilityOff,
                                    contentDescription = "Toggle Layer",
                                    tint = if (layer.isEnabled) MaterialTheme.colorScheme.primary else Color.Gray,
                                    modifier = Modifier.size(18.dp)
                                )
                            }

                            FilledTonalButton(
                                onClick = { onDrillDownMaterial(layer.material) },
                                modifier = Modifier.height(32.dp).testTag("inspect_material_$index"),
                                contentPadding = PaddingValues(horizontal = 10.dp, vertical = 0.dp)
                            ) {
                                Text("Inspect", fontSize = 11.sp)
                            }
                        }
                    }

                    // Atomic properties summary badges
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(6.dp)
                    ) {
                        Surface(
                            color = Color(0xFF1E293B),
                            shape = RoundedCornerShape(4.dp)
                        ) {
                            Text(
                                "IOR ${"%.3f".format(layer.material.ior)}",
                                modifier = Modifier.padding(horizontal = 6.dp, vertical = 2.dp),
                                style = MaterialTheme.typography.labelSmall.copy(fontFamily = FontFamily.Monospace, color = Color(0xFFFFB74D), fontSize = 10.sp)
                            )
                        }
                        Surface(
                            color = Color(0xFF1E293B),
                            shape = RoundedCornerShape(4.dp)
                        ) {
                            Text(
                                "Rough ${"%.2f".format(layer.material.roughness)}",
                                modifier = Modifier.padding(horizontal = 6.dp, vertical = 2.dp),
                                style = MaterialTheme.typography.labelSmall.copy(fontFamily = FontFamily.Monospace, color = Color(0xFF64B5F6), fontSize = 10.sp)
                            )
                        }
                        if (layer.material.metallic > 0.05f) {
                            Surface(color = Color(0xFF1E293B), shape = RoundedCornerShape(4.dp)) {
                                Text(
                                    "Metal ${(layer.material.metallic * 100).toInt()}%",
                                    modifier = Modifier.padding(horizontal = 6.dp, vertical = 2.dp),
                                    style = MaterialTheme.typography.labelSmall.copy(fontFamily = FontFamily.Monospace, color = Color(0xFF81C784), fontSize = 10.sp)
                                )
                            }
                        }
                        if (layer.material.subsurface > 0.05f) {
                            Surface(color = Color(0xFF1E293B), shape = RoundedCornerShape(4.dp)) {
                                Text(
                                    "SSS ${(layer.material.subsurface * 100).toInt()}%",
                                    modifier = Modifier.padding(horizontal = 6.dp, vertical = 2.dp),
                                    style = MaterialTheme.typography.labelSmall.copy(fontFamily = FontFamily.Monospace, color = Color(0xFFFF8A80), fontSize = 10.sp)
                                )
                            }
                        }
                    }
                }
            }
        }

        // Macro Spatial Controls
        Text(
            "Macro Assembly Scale & Displacement",
            style = MaterialTheme.typography.labelLarge.copy(fontWeight = FontWeight.Bold, color = MaterialTheme.colorScheme.primary)
        )

        Column {
            Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                Text("Surface Macro UV Tiling", style = MaterialTheme.typography.bodyMedium)
                Text("${"%.2f".format(surface.macroTileMeters)} m", style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold))
            }
            Slider(
                value = surface.macroTileMeters,
                onValueChange = { onSurfaceChange(surface.copy(macroTileMeters = it)) },
                valueRange = 0.2f..4.0f,
                modifier = Modifier.testTag("slider_surface_macro_tile")
            )
        }

        Column {
            Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                Text("Inter-Layer Displacement Relief", style = MaterialTheme.typography.bodyMedium)
                Text("${"%.1f".format(surface.displacementScaleMeters * 100f)} cm", style = MaterialTheme.typography.bodyMedium.copy(fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold))
            }
            Slider(
                value = surface.displacementScaleMeters,
                onValueChange = { onSurfaceChange(surface.copy(displacementScaleMeters = it)) },
                valueRange = 0.0f..0.06f,
                modifier = Modifier.testTag("slider_surface_displacement")
            )
        }
    }
}
