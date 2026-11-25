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
import androidx.compose.material.icons.filled.Verified
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
import com.example.sphere.texture.rememberDecodedKtx2Texture

@Composable
fun MaterialVulkanSpecTab(
    material: PbrMaterial,
    modifier: Modifier = Modifier
) {
    val decodedKtx2 = rememberDecodedKtx2Texture(material.assetPath)
    val kvd = decodedKtx2?.info?.kvdMetadata.orEmpty()

    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(14.dp)
    ) {
        if (decodedKtx2 != null) {
            Card(
                modifier = Modifier.testTag("ktx2_metadata_card"),
                shape = RoundedCornerShape(12.dp),
                colors = CardDefaults.cardColors(containerColor = Color(0xFF0F172A)),
                border = BorderStroke(1.dp, Color(0xFF10B981).copy(alpha = 0.45f))
            ) {
                Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                    Row(verticalAlignment = Alignment.CenterVertically) {
                        Icon(Icons.Default.Verified, contentDescription = null, tint = Color(0xFF10B981), modifier = Modifier.size(20.dp))
                        Spacer(Modifier.width(8.dp))
                        Text(
                            "Khronos KTX 2.0 Container & Filename Metadata",
                            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold, color = Color.White)
                        )
                    }
                    KtxSpecRow("Asset Container", decodedKtx2.assetName, Color(0xFF38BDF8))
                    KtxSpecRow("VkFormat", "${decodedKtx2.info.vkFormatName} (${decodedKtx2.info.vkFormat})", Color(0xFF34D399))
                    kvd["KTXsourceFile"]?.let { KtxSpecRow("Source File", it, Color(0xFFCBD5E1)) }
                    kvd["KTXtileScaleMm"]?.let { KtxSpecRow("Filename Tile Scale", "$it mm (${material.tileScaleMeters} m)", Color(0xFFFBBF24)) }
                    kvd["KTXroadType"]?.let { KtxSpecRow("Road Type", it, Color(0xFFFBBF24)) }
                    kvd["KTXlaneCount"]?.let { lanes ->
                        val formattedLanes = if (lanes.contains("Lane", ignoreCase = true)) lanes else if (lanes == "1") "1 Lane" else "$lanes Lanes"
                        KtxSpecRow("Lanes", formattedLanes, Color(0xFF38BDF8))
                    }
                    kvd["KTXroadWidthM"]?.let { KtxSpecRow("Road Width", "$it m", Color(0xFF34D399)) }
                    kvd["KTXmarkings"]?.let { KtxSpecRow("Markings", it, Color(0xFFCBD5E1)) }
                    kvd["KTXmaterialSource"]?.let { KtxSpecRow("Publisher", it, Color(0xFFA78BFA)) }
                    kvd["KTXmaterialCategory"]?.let { KtxSpecRow("Category", it, Color(0xFFCBD5E1)) }
                    kvd["KTXmaterialId"]?.let { KtxSpecRow("Material ID", it, Color(0xFFCBD5E1)) }
                }
            }
        }

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
                    Text("Albedo Texture (${material.albedoFormat})", style = MaterialTheme.typography.bodySmall, color = Color(0xFF94A3B8))
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

                val resW = decodedKtx2?.info?.pixelWidth ?: material.resolution
                val resH = decodedKtx2?.info?.pixelHeight ?: material.resolution
                val mipCount = decodedKtx2?.info?.levelCount ?: 12
                val vramLabel = decodedKtx2?.let {
                    val kb = it.info.level0ByteLength / 1024.0
                    "${"%.1f".format(kb)} KB Level-0 (${it.info.levelCount} mips)"
                } ?: "10.66 MB (Mipmapped)"

                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Resolution", style = MaterialTheme.typography.bodySmall, color = Color(0xFF94A3B8))
                    Text("$resW x $resH px", fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold, color = Color.White, fontSize = 12.sp)
                }
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Mipmap Chain Count", style = MaterialTheme.typography.bodySmall, color = Color(0xFF94A3B8))
                    Text("$mipCount Levels (to 1x1)", fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold, color = Color.White, fontSize = 12.sp)
                }
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Texture Payload", style = MaterialTheme.typography.bodySmall, color = Color(0xFF94A3B8))
                    Text(vramLabel, fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold, color = Color(0xFF4ADE80), fontSize = 12.sp)
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

@Composable
private fun KtxSpecRow(
    label: String,
    value: String,
    valueColor: Color
) {
    Row(
        modifier = Modifier.fillMaxWidth(),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        Text(
            label,
            style = MaterialTheme.typography.bodySmall,
            color = Color(0xFF94A3B8)
        )
        Spacer(Modifier.width(12.dp))
        Text(
            value,
            fontFamily = FontFamily.Monospace,
            fontWeight = FontWeight.Bold,
            color = valueColor,
            fontSize = 11.sp,
            maxLines = 1
        )
    }
}
