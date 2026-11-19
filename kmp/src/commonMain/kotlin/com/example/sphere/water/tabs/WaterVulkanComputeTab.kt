package com.example.sphere.water.tabs

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Memory
import androidx.compose.material.icons.filled.Speed
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
import com.example.sphere.model.WaterPhysicsState
import com.example.sphere.model.WaterTelemetryState

private data class VulkanPassSpec(
    val order: String,
    val shaderFile: String,
    val stageType: String,
    val summary: String,
    val bindingSummary: String
)

private val sphComputePasses = listOf(
    VulkanPassSpec(
        "C1", "shaders/compute/build_spatial_hash.comp", "COMPUTE 256",
        "Hashes predicted particle positions into 3D cell keys (prime XOR hash modulo power-of-2 table).",
        "SSBO[1] PredictedPos -> SSBO[4] SpatialIndices"
    ),
    VulkanPassSpec(
        "C2", "shaders/compute/bitonic_sort.comp", "COMPUTE 256",
        "Parallel bitonic merge sort over (key, particleIndex) pairs in O(N log² N) passes.",
        "SSBO[4] SpatialIndices (PushConst: groupWidth, stepIndex)"
    ),
    VulkanPassSpec(
        "C3", "shaders/compute/compute_offsets.comp", "COMPUTE 256",
        "Records first occurrence offset of each spatial hash cell for O(1) 27-neighbor lookup.",
        "SSBO[4] SpatialIndices -> SSBO[5] SpatialOffsets"
    ),
    VulkanPassSpec(
        "C4", "shaders/compute/sph_predict_positions.comp", "COMPUTE 256",
        "Applies gravity, wind shear & lookahead position x* = x + v·(1/120) for stability.",
        "SSBO[0] Pos + SSBO[2] Vel -> SSBO[1] Predicted"
    ),
    VulkanPassSpec(
        "C5", "shaders/compute/sph_density.comp", "COMPUTE 256",
        "Evaluates 27-cell neighborhood Poly6 density ρ and near-density ρ_near kernels.",
        "SSBO[1,4,5] -> SSBO[3] Densities (vec2)"
    ),
    VulkanPassSpec(
        "C6", "shaders/compute/sph_forces.comp", "COMPUTE 256",
        "Computes Spiky Pow2/Pow3 pressure gradient, viscosity Laplacian & Poly6 surface tension.",
        "SSBO[1,2,3,4,5] -> SSBO[2] Velocities"
    ),
    VulkanPassSpec(
        "C7", "shaders/compute/sph_integrate.comp", "COMPUTE 256",
        "Symplectic Euler position update + tank AABB, wave-maker & sphere obstacle collision.",
        "SSBO[0,2] -> SSBO[0] Pos, SSBO[2] Vel"
    )
)

private val ssfrGraphicsPasses = listOf(
    VulkanPassSpec(
        "G1", "shaders/graphics/particle_depth.vert/.frag", "GRAPHICS",
        "Screen-aligned quad sphere impostors writing linear view-space depth (gl_FragDepth).",
        "Depth32F Attachment + UBO[0]"
    ),
    VulkanPassSpec(
        "G2", "shaders/graphics/particle_thickness.vert/.frag", "GRAPHICS",
        "Additive R16F radial thickness splatting for volumetric Beer-Lambert path length.",
        "Additive Blend R16F Attachment"
    ),
    VulkanPassSpec(
        "G3", "shaders/graphics/bilateral_blur.vert/.frag", "GRAPHICS x2",
        "Separable H/V depth-aware Gaussian filter smoothing particle seams while keeping sharp edges.",
        "Sampler2D DepthTex -> SmoothedDepth"
    ),
    VulkanPassSpec(
        "G4", "shaders/graphics/fluid_composite.vert/.frag", "GRAPHICS",
        "Reconstructs view-space normal, computes Beer-Lambert tint, Snell refraction, Fresnel & foam.",
        "SmoothedDepth + Thickness + SceneBG"
    ),
    VulkanPassSpec(
        "G5", "shaders/graphics/container_box.vert/.frag", "GRAPHICS",
        "Renders 12-edge wireframe tank bounds & wave-maker wall marker.",
        "LineList Topology + UBO[0]"
    )
)

@Composable
fun WaterVulkanComputeTab(
    physics: WaterPhysicsState,
    telemetry: WaterTelemetryState
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(horizontal = 14.dp, vertical = 10.dp)
            .testTag("water_vulkan_compute_tab")
    ) {
        // Live Vulkan Telemetry Metrics
        WaterSectionCard(
            title = "Live Vulkan 1.3 Compute & SSFR Telemetry",
            subtitle = "Real-time frame statistics from WaterSimulationEngine & VulkanFluidPipeline",
            icon = Icons.Default.Speed,
            accentColor = Color(0xFF38BDF8)
        ) {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                TelemetryMetricChip(
                    label = "SPH Step",
                    value = "${"%.2f".format(telemetry.physicsStepMs)} ms",
                    sub = "${physics.substeps} substeps",
                    modifier = Modifier.weight(1f)
                )
                TelemetryMetricChip(
                    label = "SSFR Render",
                    value = "${"%.2f".format(telemetry.renderPassMs)} ms",
                    sub = "4-pass composite",
                    modifier = Modifier.weight(1f)
                )
                TelemetryMetricChip(
                    label = "Hash Grid",
                    value = "${telemetry.occupiedCells}/${telemetry.hashTableSize}",
                    sub = "${telemetry.activeParticles} pts",
                    modifier = Modifier.weight(1f)
                )
            }
            Spacer(Modifier.height(8.dp))
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                TelemetryMetricChip(
                    label = "Avg Density",
                    value = "${"%.2f".format(telemetry.avgDensity)}",
                    sub = "Target ${"%.1f".format(physics.targetDensity)}",
                    modifier = Modifier.weight(1f)
                )
                TelemetryMetricChip(
                    label = "Max Speed",
                    value = "${"%.2f".format(telemetry.maxSpeed)} m/s",
                    sub = "KE ${"%.1f".format(telemetry.kineticEnergy)} J",
                    modifier = Modifier.weight(1f)
                )
                TelemetryMetricChip(
                    label = "GPU Backend",
                    value = if (telemetry.vulkanGpuActive) "VK HW" else "VK+CPU",
                    sub = "VK_API_1_3",
                    modifier = Modifier.weight(1f)
                )
            }
        }

        // Compute Pipeline Stages
        WaterSectionCard(
            title = "Vulkan 1.3 Compute Passes (SPH Solver)",
            subtitle = "7 compute shader dispatches ported from talvinckb/OpenGL-Water-Simulation",
            icon = Icons.Default.Memory
        ) {
            sphComputePasses.forEach { pass ->
                VulkanPassCard(pass, badgeColor = Color(0xFF0EA5E9))
            }
        }

        // Graphics Pipeline Stages
        WaterSectionCard(
            title = "Vulkan 1.3 Graphics Passes (SSFR)",
            subtitle = "5 screen-space fluid rendering & tank wireframe passes",
            icon = Icons.Default.Memory,
            accentColor = Color(0xFF22C55E)
        ) {
            ssfrGraphicsPasses.forEach { pass ->
                VulkanPassCard(pass, badgeColor = Color(0xFF22C55E))
            }
        }
        Spacer(Modifier.height(20.dp))
    }
}

@Composable
private fun TelemetryMetricChip(
    label: String,
    value: String,
    sub: String,
    modifier: Modifier = Modifier
) {
    Surface(
        shape = RoundedCornerShape(10.dp),
        color = Color(0xFF0F172A),
        modifier = modifier
    ) {
        Column(modifier = Modifier.padding(horizontal = 8.dp, vertical = 8.dp)) {
            Text(label, fontSize = 10.sp, color = Color(0xFF94A3B8))
            Text(
                value,
                fontSize = 12.sp,
                fontWeight = FontWeight.Bold,
                fontFamily = FontFamily.Monospace,
                color = Color(0xFF38BDF8)
            )
            Text(sub, fontSize = 9.sp, fontFamily = FontFamily.Monospace, color = Color(0xFF64748B))
        }
    }
}

@Composable
private fun VulkanPassCard(pass: VulkanPassSpec, badgeColor: Color) {
    Surface(
        shape = RoundedCornerShape(10.dp),
        color = Color(0xFF0F172A),
        modifier = Modifier
            .fillMaxWidth()
            .padding(vertical = 4.dp)
    ) {
        Column(modifier = Modifier.padding(10.dp)) {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = "${pass.order} • ${pass.shaderFile}",
                    fontSize = 11.sp,
                    fontWeight = FontWeight.Bold,
                    fontFamily = FontFamily.Monospace,
                    color = Color.White,
                    modifier = Modifier.weight(1f)
                )
                Surface(
                    shape = RoundedCornerShape(4.dp),
                    color = badgeColor.copy(alpha = 0.2f)
                ) {
                    Text(
                        text = pass.stageType,
                        fontSize = 9.sp,
                        fontWeight = FontWeight.Bold,
                        fontFamily = FontFamily.Monospace,
                        color = badgeColor,
                        modifier = Modifier.padding(horizontal = 5.dp, vertical = 2.dp)
                    )
                }
            }
            Spacer(Modifier.height(3.dp))
            Text(pass.summary, fontSize = 11.sp, color = Color(0xFFCBD5E1))
            Text(
                text = pass.bindingSummary,
                fontSize = 9.sp,
                fontFamily = FontFamily.Monospace,
                color = Color(0xFF64748B),
                modifier = Modifier.padding(top = 2.dp)
            )
        }
    }
}
