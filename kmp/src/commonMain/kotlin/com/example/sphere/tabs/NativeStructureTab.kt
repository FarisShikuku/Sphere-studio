package com.example.sphere

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.CheckCircle
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

// -------------------------------------------------------------
// CONTROL TAB 3: NATIVE STRUCTURE INSPECTOR
// -------------------------------------------------------------
@Composable
fun NativeStructureTab() {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(bottom = 28.dp)
    ) {
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                text = "Native Engine Architecture",
                style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                color = Color.White
            )
            Surface(
                shape = RoundedCornerShape(6.dp),
                color = Color(0xFF064E3B)
            ) {
                Row(
                    modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Icon(
                        imageVector = Icons.Default.CheckCircle,
                        contentDescription = null,
                        tint = Color(0xFF34D399),
                        modifier = Modifier.size(12.dp)
                    )
                    Spacer(modifier = Modifier.width(4.dp))
                    Text(
                        text = "Vulkan 1.3 Verified",
                        style = MaterialTheme.typography.labelSmall,
                        color = Color(0xFF6EE7B7),
                        fontWeight = FontWeight.Bold
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(10.dp))

        val nativeFiles = listOf(
            Triple("Sky & Atmosphere Core", "src/scene/SkyModel.cpp", "Preetham / Hosek-Wilkie scattering model & solar extinction"),
            Triple("Sun Disk & Corona", "src/sky/sun/SunDisk.cpp", "Direct solar disk, limb darkening & Mie forward halo"),
            Triple("Atmospheric Perez", "src/sky/atmosphere/PreethamModel.cpp", "All-weather daylight spectrum & zenith luminance"),
            Triple("Common Scene", "src/scene/Scene.cpp", "Root viewport container holding Camera, SkyModel, Light & Nodes"),
            Triple("Camera & Controls", "src/scene/Camera.cpp", "Perspective projection, reverse-Z & orbit controllers"),
            Triple("Vulkan Device & Allocator", "base/VulkanDevice.cpp", "Physical/logical device, queue families & staging buffer pool"),
            Triple("Bindless Heap", "src/vulkan/BindlessHeap.cpp", "VK_EXT_descriptor_indexing unbounded descriptor heap"),
            Triple("Pipeline Builder", "src/vulkan/PipelineBuilder.cpp", "Vulkan 1.3 dynamic rendering graphics & compute pipelines"),
            Triple("Sky Shaders", "shaders/glsl/sky.comp", "GLSL 460 compute shader evaluating atmospheric LUT")
        )

        nativeFiles.forEach { (name, path, desc) ->
            Card(
                colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
                shape = RoundedCornerShape(8.dp),
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(vertical = 4.dp)
            ) {
                Column(modifier = Modifier.padding(12.dp)) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(
                            text = name,
                            style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Bold),
                            color = Color.White
                        )
                        Text(
                            text = path,
                            style = MaterialTheme.typography.labelSmall,
                            color = Color(0xFF38BDF8),
                            fontFamily = FontFamily.Monospace,
                            fontSize = 10.sp
                        )
                    }
                    Spacer(modifier = Modifier.height(2.dp))
                    Text(
                        text = desc,
                        style = MaterialTheme.typography.labelSmall,
                        color = Color(0xFF64748B)
                    )
                }
            }
        }
    }
}
