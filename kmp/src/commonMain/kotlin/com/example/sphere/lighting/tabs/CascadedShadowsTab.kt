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
import com.example.sphere.model.CascadedShadowParameters

@Composable
fun CascadedShadowsTab(
    params: CascadedShadowParameters,
    onParamsChange: (CascadedShadowParameters) -> Unit,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        // Banner explaining Cascaded Shadow Maps
        Surface(
            shape = RoundedCornerShape(10.dp),
            color = MaterialTheme.colorScheme.tertiaryContainer.copy(alpha = 0.45f),
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Icon(
                        Icons.Default.Layers,
                        contentDescription = null,
                        tint = MaterialTheme.colorScheme.tertiary,
                        modifier = Modifier.size(20.dp)
                    )
                    Spacer(Modifier.width(8.dp))
                    Text(
                        "Directional Cascaded Shadow Maps (CSM)",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.tertiary
                    )
                }
                Spacer(Modifier.height(4.dp))
                Text(
                    "Referencing Gaukler/PlainRenderer and SaschaWillems/Vulkan: 4 view-frustum splits with practical logarithmic-linear blend, sub-texel stabilization to eliminate edge swimming, and 5x5 PCF soft penumbra filtering.",
                    style = MaterialTheme.typography.bodySmall,
                    color = MaterialTheme.colorScheme.onSurfaceVariant
                )
            }
        }

        // Enable Shadows Master Switch
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
                    Text("Cast Shadows", style = MaterialTheme.typography.titleMedium, fontWeight = FontWeight.SemiBold)
                    Text("Renders shadow map depth pass in Vulkan pipeline", style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.outline)
                }
                Switch(
                    checked = params.enabled,
                    onCheckedChange = { onParamsChange(params.copy(enabled = it)) },
                    modifier = Modifier.testTag("csm_enable_switch")
                )
            }
        }

        // Shadow Map Resolution
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(16.dp), verticalArrangement = Arrangement.spacedBy(10.dp)) {
                Text("Texture 2D Array Resolution", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.SemiBold)
                Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                    listOf(1024, 2048, 4096).forEach { res ->
                        FilterChip(
                            selected = params.shadowMapResolution == res,
                            onClick = { onParamsChange(params.copy(shadowMapResolution = res)) },
                            label = { Text("${res}x${res} (x4)", style = MaterialTheme.typography.labelSmall) },
                            modifier = Modifier.testTag("csm_res_$res")
                        )
                    }
                }
            }
        }

        // 4-Split Cascade Depths Display
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(16.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                Text("Frustum Cascade Split Distances", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.SemiBold)
                val splitLabels = listOf(
                    Triple("Cascade 0 (Near detail):", "${params.splitDistances[0].toInt()}m", Color(0xFFE53935)),
                    Triple("Cascade 1 (Mid-range):", "${params.splitDistances[1].toInt()}m", Color(0xFF43A047)),
                    Triple("Cascade 2 (Far stage):", "${params.splitDistances[2].toInt()}m", Color(0xFF1E88E5)),
                    Triple("Cascade 3 (Horizon):", "${params.splitDistances[3].toInt()}m", Color(0xFFFDD835))
                )
                splitLabels.forEach { (label, value, col) ->
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Row(verticalAlignment = Alignment.CenterVertically) {
                            Box(modifier = Modifier.size(8.dp).clip(RoundedCornerShape(4.dp)).background(col))
                            Spacer(Modifier.width(8.dp))
                            Text(label, style = MaterialTheme.typography.bodySmall)
                        }
                        Text(value, style = MaterialTheme.typography.bodySmall, fontWeight = FontWeight.Bold, color = col)
                    }
                }
            }
        }

        // Split Lambda (Logarithmic vs Uniform) Slider
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("Split Scheme (Lambda)", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.SemiBold)
                    Text(
                        "${(params.splitLambda * 100).toInt()}% Log",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary
                    )
                }
                Slider(
                    value = params.splitLambda,
                    onValueChange = {
                        val pNear = 0.1f
                        val pFar = 100f
                        val splits = (1..4).map { i ->
                            val p = i / 4.0f
                            val logSplit = pNear * kotlin.math.exp(p * kotlin.math.ln(pFar / pNear))
                            val uniformSplit = pNear + (pFar - pNear) * p
                            it * logSplit + (1.0f - it) * uniformSplit
                        }
                        onParamsChange(params.copy(splitLambda = it, splitDistances = splits))
                    },
                    valueRange = 0.0f..1.0f,
                    modifier = Modifier.fillMaxWidth().testTag("csm_lambda_slider")
                )
                Text(
                    "Practical split blend: 0% = Uniform linear, 100% = Pure logarithmic (favoring near camera detail)",
                    style = MaterialTheme.typography.labelSmall,
                    color = MaterialTheme.colorScheme.outline
                )
            }
        }

        // PCF Filter Softness Slider
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("PCF Filter Softness", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.SemiBold)
                    val kernelName = when {
                        params.pcfKernelSize <= 1.2f -> "3x3 Kernel"
                        params.pcfKernelSize <= 2.2f -> "5x5 Kernel"
                        else -> "7x7 Poisson"
                    }
                    Text(
                        kernelName,
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary
                    )
                }
                Slider(
                    value = params.pcfKernelSize,
                    onValueChange = { onParamsChange(params.copy(pcfKernelSize = it)) },
                    valueRange = 1.0f..3.0f,
                    steps = 1,
                    modifier = Modifier.fillMaxWidth().testTag("csm_pcf_slider")
                )
                Text(
                    "Percentage Closer Filtering kernel size for soft penumbra shadow borders",
                    style = MaterialTheme.typography.labelSmall,
                    color = MaterialTheme.colorScheme.outline
                )
            }
        }

        // Sub-Texel Grid Stabilization Switch
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
                Column(modifier = Modifier.weight(1f)) {
                    Text("Sub-Texel Grid Snapping", style = MaterialTheme.typography.titleMedium, fontWeight = FontWeight.SemiBold)
                    Text("Snaps light-space ortho projection to texel grid to stop edge shimmering", style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.outline)
                }
                Switch(
                    checked = params.subTexelStabilization,
                    onCheckedChange = { onParamsChange(params.copy(subTexelStabilization = it)) },
                    modifier = Modifier.testTag("csm_stabilization_switch")
                )
            }
        }
    }
}
