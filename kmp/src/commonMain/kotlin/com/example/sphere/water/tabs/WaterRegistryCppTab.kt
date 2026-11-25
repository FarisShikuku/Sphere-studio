package com.example.sphere.water.tabs

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Code
import androidx.compose.material.icons.filled.Folder
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

@Composable
fun WaterRegistryCppTab() {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(horizontal = 14.dp, vertical = 10.dp)
            .testTag("water_registry_cpp_tab")
    ) {
        WaterSectionCard(
            title = "gen/water Repository Layout (C++20 / Vulkan 1.3)",
            subtitle = "Mirrors talvinckb/OpenGL-Water-Simulation structure ported to Sphere Studio Vulkan",
            icon = Icons.Default.Folder,
            accentColor = Color(0xFF4ADE80)
        ) {
            val treeLines = """
                native_assets/src/gen/water/
                ├── CMakeLists.txt
                ├── README.md & REFERENCES.md
                ├── include/
                │   ├── FluidParams.hpp
                │   ├── OrbitalCamera.hpp
                │   ├── FluidSimulator.hpp
                │   ├── FluidRenderer.hpp
                │   ├── WaterSurfaceParams.hpp
                │   ├── WaterSurfaceSimulator.hpp
                │   ├── WaterSurfaceRenderer.hpp
                │   ├── VulkanFluidPipeline.hpp
                │   └── WaterSimulationEngine.hpp
                ├── src/
                │   ├── OrbitalCamera.cpp
                │   ├── FluidSimulator.cpp
                │   ├── FluidRenderer.cpp
                │   ├── WaterSurfaceSimulator.cpp
                │   ├── WaterSurfaceRenderer.cpp
                │   ├── VulkanFluidPipeline.cpp
                │   └── WaterSimulationEngine.cpp
                └── shaders/
                    ├── compute/ (7 SPH + phillips_fft_spectrum.comp)
                    └── graphics/ (10 SSFR + water_surface + underwater_terrain)
            """.trimIndent()

            Surface(
                shape = RoundedCornerShape(10.dp),
                color = Color(0xFF0B111E),
                modifier = Modifier.fillMaxWidth()
            ) {
                Text(
                    text = treeLines,
                    fontSize = 10.sp,
                    lineHeight = 15.sp,
                    fontFamily = FontFamily.Monospace,
                    color = Color(0xFFE2E8F0),
                    modifier = Modifier.padding(10.dp)
                )
            }
        }

        WaterSectionCard(
            title = "std430 GPU Buffers & Kernel Specifications",
            subtitle = "Aligned Vulkan SSBO / UBO memory contracts & SPH smoothing kernels",
            icon = Icons.Default.Code,
            accentColor = Color(0xFF38BDF8)
        ) {
            val specs = listOf(
                Triple(
                    "Set 0 • Binding 0..5 (Storage Buffers)",
                    "Positions[], Predicted[], Velocities[], Densities(vec2)[], SpatialIndices(uvec2)[], SpatialOffsets(uint)[]",
                    "Updated across C1–C7 compute passes with VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT barriers"
                ),
                Triple(
                    "Set 1 • Binding 0 (GpuFluidUniforms UBO)",
                    "mat4 view, proj, invProj + cameraPos, boundsHalf, sphConstants, sphForces, obstacleSphere, waterColor, absorption, opticsParams",
                    "16-byte std430 aligned in FluidParams.hpp and shared across compute & graphics layouts"
                ),
                Triple(
                    "SPH Smoothing Kernels (3D)",
                    "SpikyPow2: 15/(2πh⁵)·(h-r)² | SpikyPow3: 15/(πh⁶)·(h-r)³ | Poly6: 315/(64πh⁹)·(h²-r²)³",
                    "Eliminates particle clustering via double-density relaxation (Clavet et al.)"
                ),
                Triple(
                    "GeneratorRegistry Integration",
                    "sphere::water::WaterSimulationEngine (IGenerator)",
                    "Registered as \"water\" in GeneratorRegistry.cpp and linked via CMake target 'water'"
                )
            )

            specs.forEach { (title, layout, desc) ->
                Surface(
                    shape = RoundedCornerShape(10.dp),
                    color = Color(0xFF0F172A),
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(vertical = 4.dp)
                ) {
                    Column(modifier = Modifier.padding(10.dp)) {
                        Text(
                            text = title,
                            fontSize = 11.sp,
                            fontWeight = FontWeight.Bold,
                            color = Color(0xFF38BDF8)
                        )
                        Text(
                            text = layout,
                            fontSize = 10.sp,
                            fontFamily = FontFamily.Monospace,
                            color = Color(0xFF4ADE80),
                            modifier = Modifier.padding(vertical = 2.dp)
                        )
                        Text(
                            text = desc,
                            fontSize = 10.sp,
                            color = Color(0xFFCBD5E1)
                        )
                    }
                }
            }
        }
        Spacer(Modifier.height(20.dp))
    }
}
