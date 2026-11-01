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
// CONTROL TAB 3: NATIVE STRUCTURE & SCENE GEOMETRY INSPECTOR
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
                text = "Native C++ Scene & Geometry Core",
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
                        text = "Vulkan 1.3 C++ Active",
                        style = MaterialTheme.typography.labelSmall,
                        color = Color(0xFF6EE7B7),
                        fontWeight = FontWeight.Bold
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(10.dp))

        val nativeFiles = listOf(
            Triple("Procedural Geometry Core", "src/scene/Geometry.cpp", "C++ PBR cube, calibration cube, stage plane, infinite grid, and sky dome generators"),
            Triple("Common Vulkan Scene", "src/scene/Scene.cpp", "Unified root scene holding stage modes (Flat/Boundary/Grid/Cube), lights, camera & assets"),
            Triple("Hierarchical Scene Graph", "src/scene/SceneGraph.cpp", "C++ parent-child transform hierarchy, dirty matrix caching, and visibility traversal"),
            Triple("Vulkan Stage Pipeline", "src/scene/VulkanStage.cpp", "Multi-pass Vulkan 1.3 coordinator (shadows, atmosphere, PBR, debug grid & post-fx)"),
            Triple("Transform & 3D Math", "src/scene/Transform.cpp", "Column-major Mat4, Quat, Vec3 vector arithmetic and Vulkan clip projection"),
            Triple("Mesh & Tangent Calculation", "src/scene/Mesh.cpp", "Gram-Schmidt tangent space, smooth area-weighted normal synthesis, AABB bounds"),
            Triple("Atmospheric Sky & Sun Core", "src/scene/SkyModel.cpp", "Preetham / Hosek-Wilkie scattering model & solar extinction calculation"),
            Triple("Camera & 6-DOF Orbit", "src/scene/Camera.cpp", "Perspective projection, reverse-Z depth & spherical orbit controller"),
            Triple("Bindless Heap Manager", "src/vulkan/BindlessHeap.cpp", "VK_EXT_descriptor_indexing unbounded descriptor heap for textures & buffers"),
            Triple("Decima Raymarching Compute", "src/weather/clouds/CloudRaymarch.cpp", "Meteoros multi-scattering dual Beer-Lambert and Henyey-Greenstein solver")
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
