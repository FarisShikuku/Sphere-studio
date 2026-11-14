package com.example.sphere.trailers.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.TrailerItem

@Composable
fun TrailerVulkanComputeTab(
    trailer: TrailerItem,
    modifier: Modifier = Modifier
) {
    var selectedShaderIndex by remember { mutableIntStateOf(0) }
    val prefix = when (trailer.category.code) {
        "box_van" -> "boxvan"
        "curtainsider" -> "curtain"
        "bulk_hopper" -> "hopper"
        "car_carrier" -> "carcarrier"
        "heavy_haulage" -> "heavy"
        else -> trailer.category.code
    }
    val shaderTabs = listOf("${prefix}_mesh.comp", "${prefix}_albedo.comp", "${prefix}_normal.comp", "gear_mesh.comp")

    val glslCode = when (selectedShaderIndex) {
        0 -> """
// gen/trailers/${trailer.category.moduleFolder}/shaders/${prefix}_mesh.comp
#version 450 core
layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

layout(push_constant) uniform PushConstants {
    vec4 dims; // (${trailer.lengthM}, ${trailer.widthM}, ${trailer.heightM}, 1.0)
    float deckHeight; // ${trailer.deckHeightM}
    uint variant;
} pc;

void main() {
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= 2048u) return;
    // Procedural I-beams, cross-members, & superstructure vertices
}
        """.trimIndent()
        1 -> """
// gen/trailers/${trailer.category.moduleFolder}/shaders/${prefix}_albedo.comp
#version 450 core
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout(binding = 0, rgba8) uniform writeonly image2D outAlbedoImage;
// Output Format: ${trailer.vkAlbedoFormat} (1024x1024)
        """.trimIndent()
        2 -> """
// gen/trailers/${trailer.category.moduleFolder}/shaders/${prefix}_normal.comp
#version 450 core
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout(binding = 0, rgba8) uniform writeonly image2D outNormalImage;
// Output Format: ${trailer.vkNormalFormat} (weld seams, panel ribs, tire tread)
        """.trimIndent()
        else -> """
// gen/trailers/running_gear/shaders/gear_mesh.comp
#version 450 core
layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
// Axle Count: ${trailer.axleConfig.count} | Suspension: ${trailer.suspensionType.displayName}
// Brakes: ${trailer.brakeType.displayName} | Steer Angle: ${trailer.steerAngleDeg} deg
        """.trimIndent()
    }

    Column(
        modifier = modifier
            .fillMaxWidth()
            .padding(14.dp)
            .testTag("trailer_vulkan_compute_tab")
    ) {
        Text(
            "Vulkan Compute Shader Pipeline (gen/trailers/${trailer.category.moduleFolder})",
            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
            color = MaterialTheme.colorScheme.primary
        )
        Spacer(Modifier.height(8.dp))

        Row(
            modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            shaderTabs.forEachIndexed { idx, title ->
                FilterChip(
                    selected = selectedShaderIndex == idx,
                    onClick = { selectedShaderIndex = idx },
                    label = { Text(title, fontSize = 11.sp, fontFamily = FontFamily.Monospace) }
                )
            }
        }

        Spacer(Modifier.height(10.dp))

        Box(
            modifier = Modifier
                .fillMaxWidth()
                .clip(RoundedCornerShape(10.dp))
                .background(Color(0xFF0B0F17))
                .padding(12.dp)
        ) {
            Text(
                text = glslCode,
                color = Color(0xFF93C5FD),
                fontFamily = FontFamily.Monospace,
                fontSize = 11.sp,
                modifier = Modifier.horizontalScroll(rememberScrollState())
            )
        }
    }
}
