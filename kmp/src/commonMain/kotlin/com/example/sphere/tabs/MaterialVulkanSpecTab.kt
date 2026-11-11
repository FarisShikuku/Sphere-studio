package com.example.sphere.tabs

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Memory
import androidx.compose.material.icons.filled.Storage
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
import com.example.sphere.model.PbrMaterial

@Composable
fun MaterialVulkanSpecTab(
    material: PbrMaterial,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(14.dp)
    ) {
        // Vulkan 1.3 Descriptor Indexing & Bindless Heap Card
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = Color(0xFF0F172A)),
            border = BorderStroke(1.dp, Color(0xFF1E293B))
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(10.dp)) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Icon(Icons.Default.Storage, contentDescription = null, tint = Color(0xFF38BDF8), modifier = Modifier.size(20.dp))
                    Spacer(Modifier.width(8.dp))
                    Text(
                        "Bindless Heap Texture Slots (VK_EXT_descriptor_indexing)",
                        style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold, color = Color.White)
                    )
                }

                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Albedo Texture (BC7 sRGB)", style = MaterialTheme.typography.bodySmall, color = Color(0xFF94A3B8))
                    Text("Slot #${material.bindlessAlbedoSlot}", fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold, color = Color(0xFF38BDF8), fontSize = 12.sp)
                }
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Normal Map (BC5 Tangent RG)", style = MaterialTheme.typography.bodySmall, color = Color(0xFF94A3B8))
                    Text("Slot #${material.bindlessNormalSlot}", fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold, color = Color(0xFF818CF8), fontSize = 12.sp)
                }
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("ORM Map (BC7 AO/Rough/Metal)", style = MaterialTheme.typography.bodySmall, color = Color(0xFF94A3B8))
                    Text("Slot #${material.bindlessOrmSlot}", fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold, color = Color(0xFF34D399), fontSize = 12.sp)
                }
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Displacement Height (BC4)", style = MaterialTheme.typography.bodySmall, color = Color(0xFF94A3B8))
                    Text("Slot #${material.bindlessHeightSlot}", fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold, color = Color(0xFFFBBF24), fontSize = 12.sp)
                }
            }
        }

        // Texture Formats & Memory Footprint Card
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = Color(0xFF0F172A)),
            border = BorderStroke(1.dp, Color(0xFF1E293B))
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(10.dp)) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Icon(Icons.Default.Memory, contentDescription = null, tint = Color(0xFFA78BFA), modifier = Modifier.size(20.dp))
                    Spacer(Modifier.width(8.dp))
                    Text(
                        "VRAM Texture Memory & Resolution",
                        style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold, color = Color.White)
                    )
                }

                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Resolution", style = MaterialTheme.typography.bodySmall, color = Color(0xFF94A3B8))
                    Text("${material.resolution} x ${material.resolution} px", fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold, color = Color.White, fontSize = 12.sp)
                }
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Mipmap Chain Count", style = MaterialTheme.typography.bodySmall, color = Color(0xFF94A3B8))
                    Text("12 Levels (to 1x1)", fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold, color = Color.White, fontSize = 12.sp)
                }
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Total Compressed VRAM", style = MaterialTheme.typography.bodySmall, color = Color(0xFF94A3B8))
                    Text("10.66 MB (Mipmapped)", fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold, color = Color(0xFF4ADE80), fontSize = 12.sp)
                }
            }
        }

        // Vulkan C++ Native Pipeline Architecture Card
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = Color(0xFF0F172A)),
            border = BorderStroke(1.dp, Color(0xFF38BDF8).copy(alpha = 0.35f))
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Icon(Icons.Default.Memory, contentDescription = null, tint = Color(0xFF38BDF8), modifier = Modifier.size(20.dp))
                    Spacer(Modifier.width(8.dp))
                    Text(
                        "Native Vulkan C++ Pipeline (SaschaWillems / Khronos)",
                        style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold, color = Color.White)
                    )
                }
                Text(
                    "PBR (Cook-Torrance GGX microfacet BRDF, D_GGX, V_Smith, F_Schlick) is exclusively evaluated by the native Vulkan C++ engine and GLSL shaders (native_assets/shaders/glsl/pbr.frag, pbr.vert).",
                    style = MaterialTheme.typography.bodySmall.copy(color = Color(0xFFCBD5E1), fontSize = 11.sp)
                )
                Surface(
                    color = Color.Black.copy(alpha = 0.45f),
                    shape = RoundedCornerShape(6.dp),
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Column(modifier = Modifier.padding(10.dp), verticalArrangement = Arrangement.spacedBy(4.dp)) {
                        Text(
                            "PHYSICAL INVARIANCE SPECIFICATION:",
                            color = Color(0xFF38BDF8),
                            fontSize = 10.sp,
                            fontWeight = FontWeight.Bold,
                            fontFamily = FontFamily.Monospace
                        )
                        Text(
                            "• Camera View Scale (Zoom): Alters camera projection / viewport FOV; zero effect on physical geometry or PBR material properties.",
                            color = Color(0xFF94A3B8),
                            fontSize = 10.sp,
                            fontFamily = FontFamily.Monospace
                        )
                        Text(
                            "• Metric Dimension (Measurements): Increases physical area and volume. Physical PBR properties (albedo, roughness, metallic, tile scale) remain constant in world space.",
                            color = Color(0xFF94A3B8),
                            fontSize = 10.sp,
                            fontFamily = FontFamily.Monospace
                        )
                        Text(
                            "• True UV Tiling: A 1m surface with 4 tiles (2x2) yields 16 tiles (4x4) at 4m² (2m x 2m), preserving constant physical component dimensions.",
                            color = Color(0xFF4ADE80),
                            fontSize = 10.sp,
                            fontFamily = FontFamily.Monospace
                        )
                    }
                }
            }
        }
    }
}
