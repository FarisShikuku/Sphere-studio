package com.example.sphere.lighting.tabs

import androidx.compose.foundation.background
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
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.SdfGiParameters

@Composable
fun SdfGiTab(
    params: SdfGiParameters,
    onParamsChange: (SdfGiParameters) -> Unit,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        // Banner explaining PlainRenderer SDF Diffuse GI
        Surface(
            shape = RoundedCornerShape(10.dp),
            color = MaterialTheme.colorScheme.primaryContainer.copy(alpha = 0.45f),
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Icon(
                        Icons.Default.BlurOn,
                        contentDescription = null,
                        tint = MaterialTheme.colorScheme.primary,
                        modifier = Modifier.size(20.dp)
                    )
                    Spacer(Modifier.width(8.dp))
                    Text(
                        "SDF Diffuse Global Illumination",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary
                    )
                }
                Spacer(Modifier.height(4.dp))
                Text(
                    "Referencing Gaukler/PlainRenderer: Meshes are represented as continuous 3D Signed Distance Fields. Compute shaders cone-trace the distance field to produce real-time multi-bounce diffuse indirect illumination and Distance Field Ambient Occlusion (DFAO).",
                    style = MaterialTheme.typography.bodySmall,
                    color = MaterialTheme.colorScheme.onSurfaceVariant
                )
            }
        }

        // Enable / Disable Master Switch
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(horizontal = 16.dp, vertical = 12.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Column {
                    Text("Enable SDF Diffuse GI", style = MaterialTheme.typography.titleMedium, fontWeight = FontWeight.SemiBold)
                    Text("Computes indirect bounce radiance & DFAO in compute shader", style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.outline)
                }
                Switch(
                    checked = params.enabled,
                    onCheckedChange = { onParamsChange(params.copy(enabled = it)) },
                    modifier = Modifier.testTag("sdf_gi_switch")
                )
            }
        }

        // Voxel Grid Resolution
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(16.dp), verticalArrangement = Arrangement.spacedBy(10.dp)) {
                Text("SDF Voxel Resolution (3D Texture)", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.SemiBold)
                Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                    listOf(32, 64, 128).forEach { res ->
                        FilterChip(
                            selected = params.voxelResolution == res,
                            onClick = { onParamsChange(params.copy(voxelResolution = res)) },
                            label = { Text("${res}³ (${res * res * res / 1000}k voxels)", style = MaterialTheme.typography.labelSmall) },
                            modifier = Modifier.testTag("sdf_res_$res")
                        )
                    }
                }
            }
        }

        // Bounce Factor Slider
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("Indirect Bounce Factor", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.SemiBold)
                    Text(
                        "${String.format("%.2f", params.bounceFactor)}x",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary
                    )
                }
                Slider(
                    value = params.bounceFactor,
                    onValueChange = { onParamsChange(params.copy(bounceFactor = it)) },
                    valueRange = 0.0f..2.5f,
                    modifier = Modifier.fillMaxWidth().testTag("sdf_bounce_slider")
                )
                Text(
                    "Scales the intensity of indirect radiance collected by cone-tracing the SDF",
                    style = MaterialTheme.typography.labelSmall,
                    color = MaterialTheme.colorScheme.outline
                )
            }
        }

        // Color Bleeding Boost Slider
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("Color Bleeding Saturation", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.SemiBold)
                    Text(
                        "${String.format("%.2f", params.indirectColorBoost)}x",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.Bold,
                        color = Color(0xFFFF5252)
                    )
                }
                Slider(
                    value = params.indirectColorBoost,
                    onValueChange = { onParamsChange(params.copy(indirectColorBoost = it)) },
                    valueRange = 0.5f..2.5f,
                    modifier = Modifier.fillMaxWidth().testTag("sdf_color_bleed_slider")
                )
                Text(
                    "Accentuates saturated diffuse color transfer from colored Cornell box walls onto floor and cube",
                    style = MaterialTheme.typography.labelSmall,
                    color = MaterialTheme.colorScheme.outline
                )
            }
        }

        // DFAO (Distance Field Ambient Occlusion) Intensity
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("DFAO Intensity", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.SemiBold)
                    Text(
                        "${(params.aoIntensity * 100).toInt()}%",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary
                    )
                }
                Slider(
                    value = params.aoIntensity,
                    onValueChange = { onParamsChange(params.copy(aoIntensity = it)) },
                    valueRange = 0.0f..1.5f,
                    modifier = Modifier.fillMaxWidth().testTag("sdf_dfao_slider")
                )
                Text(
                    "Ambient occlusion derived directly from SDF step distance along surface normal",
                    style = MaterialTheme.typography.labelSmall,
                    color = MaterialTheme.colorScheme.outline
                )
            }
        }

        // Cone Tracing Angle Slider
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("Cone Tracing Aperture", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.SemiBold)
                    Text(
                        "${params.coneTracingAngleDeg.toInt()}°",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary
                    )
                }
                Slider(
                    value = params.coneTracingAngleDeg,
                    onValueChange = { onParamsChange(params.copy(coneTracingAngleDeg = it)) },
                    valueRange = 15f..75f,
                    modifier = Modifier.fillMaxWidth().testTag("sdf_cone_angle_slider")
                )
                Text(
                    "Angular width of diffuse sampling cones marched through continuous distance field",
                    style = MaterialTheme.typography.labelSmall,
                    color = MaterialTheme.colorScheme.outline
                )
            }
        }
    }
}
