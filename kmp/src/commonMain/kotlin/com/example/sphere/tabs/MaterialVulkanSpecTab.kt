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
                    Text("Compute Dispatch Grid", style = MaterialTheme.typography.bodySmall, color = Color(0xFF94A3B8))
                    Text("[128, 128, 1] groups (16x16)", fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold, color = Color(0xFF38BDF8), fontSize = 12.sp)
                }
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Total Compressed VRAM", style = MaterialTheme.typography.bodySmall, color = Color(0xFF94A3B8))
                    Text("10.66 MB (Mipmapped)", fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold, color = Color(0xFF4ADE80), fontSize = 12.sp)
                }
            }
        }
    }
}
