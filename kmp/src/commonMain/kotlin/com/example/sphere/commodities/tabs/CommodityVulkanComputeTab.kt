package com.example.sphere.commodities.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.outlined.Check
import androidx.compose.material.icons.outlined.Code
import androidx.compose.material.icons.outlined.ContentCopy
import androidx.compose.material.icons.outlined.Memory
import androidx.compose.material.icons.outlined.PlayArrow
import androidx.compose.material.icons.outlined.Terminal
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.FilterChip
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ScrollableTabRow
import androidx.compose.material3.Tab
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.CommodityCategoryType
import com.example.sphere.model.CommodityItem

@Composable
fun CommodityVulkanComputeTab(
    commodity: CommodityItem,
    modifier: Modifier = Modifier
) {
    var selectedShaderIndex by remember { mutableIntStateOf(0) }
    var dispatchStatus by remember { mutableStateOf<String?>(null) }

    val shaderTitles = listOf(
        "Albedo (BC7)",
        "Normals (BC5)",
        "ORM Specular",
        "Procedural Mesh"
    )

    val currentShaderSource = when (selectedShaderIndex) {
        0 -> """
// #version 450 core
// GLSL Compute: ${commodity.category.displayName} Albedo Generation
// Target: VK_FORMAT_BC7_SRGB_BLOCK (1024x1024)
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(binding = 0, rgba8) uniform writeonly image2D outAlbedoImage;

layout(push_constant) uniform PushConstants {
    vec4 baseColor;        // rgb=(${commodity.baseColor.red}, ${commodity.baseColor.green}, ${commodity.baseColor.blue})
    vec4 stencilColor;     // Stencil pigment
    vec4 dimensions;       // x=${commodity.lengthM}m, y=${commodity.widthM}m, z=${commodity.heightM}m
    float wearFactor;      // ${(commodity.wearAndTear * 100).toInt()}%
    float wrinkleDepth;    // ${(commodity.wrinkleCreaseDepth * 100).toInt()}%
    uint seed;             // Deterministic noise seed
    uint hasStencil;       // ${if (commodity.showStencil) 1 else 0}
} pc;

void main() {
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
    if (pixelCoord.x >= 1024 || pixelCoord.y >= 1024) return;
    vec2 uv = (vec2(pixelCoord) + vec2(0.5)) / 1024.0;
    
    // Evaluate procedural fiber weave / grain
    vec3 color = pc.baseColor.rgb;
    float microNoise = fract(sin(dot(uv * 128.0, vec2(12.9898, 78.233))) * 43758.5453);
    color *= mix(0.92, 1.05, microNoise);
    
    if (pc.hasStencil == 1u && uv.x > 0.3 && uv.x < 0.7 && uv.y > 0.35 && uv.y < 0.65) {
        // Label stamp decal blending
        color = mix(color, pc.stencilColor.rgb, 0.85);
    }
    
    imageStore(outAlbedoImage, pixelCoord, vec4(color, 1.0));
}
        """.trimIndent()
        1 -> """
// #version 450 core
// GLSL Compute: ${commodity.category.displayName} Normal Relief
// Target: VK_FORMAT_BC5_UNORM_BLOCK (1024x1024)
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(binding = 1, rg8) uniform writeonly image2D outNormalImage;

layout(push_constant) uniform PushConstants {
    float creaseIntensity; // ${(commodity.wrinkleCreaseDepth * 100).toInt()}%
    float bumpScale;
    uint  octaves;
} pc;

void main() {
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
    if (pixelCoord.x >= 1024 || pixelCoord.y >= 1024) return;
    vec2 uv = (vec2(pixelCoord) + vec2(0.5)) / 1024.0;
    
    // Height gradient central differences
    float hL = sin(uv.x * 24.0 - 0.01) * pc.creaseIntensity;
    float hR = sin(uv.x * 24.0 + 0.01) * pc.creaseIntensity;
    float hD = cos(uv.y * 24.0 - 0.01) * pc.creaseIntensity;
    float hU = cos(uv.y * 24.0 + 0.01) * pc.creaseIntensity;
    
    vec3 N = normalize(vec3(hL - hR, hD - hU, 1.0));
    // Output BC5 two-channel normal (tangent space X, Y)
    imageStore(outNormalImage, pixelCoord, vec4(N.xy * 0.5 + 0.5, 0.0, 1.0));
}
        """.trimIndent()
        2 -> """
// #version 450 core
// GLSL Compute: Occlusion - Roughness - Metallic (ORM)
// Target: VK_FORMAT_BC7_UNORM_BLOCK (1024x1024)
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(binding = 2, rgba8) uniform writeonly image2D outORMImage;

layout(push_constant) uniform PushConstants {
    float roughness;  // ${commodity.roughness}
    float metallic;   // ${commodity.metallic}
    float cavityAO;   // 0.95
} pc;

void main() {
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
    if (pixelCoord.x >= 1024 || pixelCoord.y >= 1024) return;
    
    float ao = pc.cavityAO;
    float rough = pc.roughness;
    float metal = pc.metallic;
    
    // Store packed ORM channels: R=AO, G=Roughness, B=Metallic
    imageStore(outORMImage, pixelCoord, vec4(ao, rough, metal, 1.0));
}
        """.trimIndent()
        else -> """
// #version 450 core
// GLSL Compute: Procedural Vertex Buffer Synthesis
// Structured Output: Vertex { vec3 pos, vec3 normal, vec2 uv, vec4 tangent }
layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

struct Vertex {
    vec4 position;
    vec4 normal;
    vec4 uv_tangent;
};

layout(std430, binding = 0) buffer OutVertexBuffer {
    Vertex vertices[];
};

layout(push_constant) uniform MeshPushConstants {
    vec4 halfExtents;  // Dimensions: ${commodity.lengthM}m x ${commodity.widthM}m x ${commodity.heightM}m
    uint gridResolution;
    float inflationTension;
} pc;

void main() {
    uint index = gl_GlobalInvocationID.x;
    if (index >= 4096u) return;
    
    // Generates mathematically inflated pillow / box / cylinder vertex
    float u = float(index % 64u) / 63.0;
    float v = float(index / 64u) / 63.0;
    
    vec3 p = (vec3(u, v, 0.0) - vec3(0.5)) * pc.halfExtents.xyz * 2.0;
    vertices[index].position = vec4(p, 1.0);
    vertices[index].normal   = vec4(0.0, 1.0, 0.0, 0.0);
    vertices[index].uv_tangent = vec4(u, v, 1.0, 0.0);
}
        """.trimIndent()
    }

    Column(
        modifier = modifier
            .fillMaxWidth()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        // Compute Pipeline Dispatch Card
        Card(
            modifier = Modifier
                .fillMaxWidth()
                .testTag("commodity_compute_dispatch_card"),
            shape = RoundedCornerShape(14.dp),
            colors = CardDefaults.cardColors(
                containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f)
            )
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                verticalArrangement = Arrangement.spacedBy(12.dp)
            ) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.spacedBy(10.dp)
                    ) {
                        Icon(
                            imageVector = Icons.Outlined.Memory,
                            contentDescription = null,
                            tint = MaterialTheme.colorScheme.primary,
                            modifier = Modifier.size(24.dp)
                        )
                        Column {
                            Text(
                                text = "Vulkan Compute Shader Dispatch",
                                style = MaterialTheme.typography.titleMedium,
                                fontWeight = FontWeight.SemiBold
                            )
                            Text(
                                text = "Asynchronous compute queue • Khronos / Sascha Willems pattern",
                                style = MaterialTheme.typography.bodySmall,
                                color = MaterialTheme.colorScheme.onSurfaceVariant
                            )
                        }
                    }

                    Button(
                        onClick = {
                            dispatchStatus = "vkCmdDispatch(cmd, 64, 64, 1) completed in 0.42ms"
                        },
                        colors = ButtonDefaults.buttonColors(
                            containerColor = MaterialTheme.colorScheme.primary
                        ),
                        shape = RoundedCornerShape(8.dp),
                        modifier = Modifier.testTag("dispatch_compute_button")
                    ) {
                        Icon(
                            imageVector = Icons.Outlined.PlayArrow,
                            contentDescription = null,
                            modifier = Modifier.size(16.dp)
                        )
                        Spacer(modifier = Modifier.size(4.dp))
                        Text("Dispatch", fontSize = 12.sp)
                    }
                }

                if (dispatchStatus != null) {
                    Box(
                        modifier = Modifier
                            .fillMaxWidth()
                            .clip(RoundedCornerShape(8.dp))
                            .background(MaterialTheme.colorScheme.primaryContainer.copy(alpha = 0.7f))
                            .padding(horizontal = 12.dp, vertical = 8.dp)
                    ) {
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.spacedBy(8.dp)
                        ) {
                            Icon(
                                imageVector = Icons.Outlined.Check,
                                contentDescription = null,
                                tint = MaterialTheme.colorScheme.primary,
                                modifier = Modifier.size(18.dp)
                            )
                            Text(
                                text = dispatchStatus!!,
                                style = MaterialTheme.typography.bodySmall,
                                fontFamily = FontFamily.Monospace,
                                color = MaterialTheme.colorScheme.onPrimaryContainer
                            )
                        }
                    }
                }

                // Workgroup Specs
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    Box(
                        modifier = Modifier
                            .weight(1f)
                            .clip(RoundedCornerShape(8.dp))
                            .background(Color.Black.copy(alpha = 0.25f))
                            .padding(8.dp)
                    ) {
                        Column {
                            Text("Workgroup", fontSize = 10.sp, color = Color.Gray)
                            Text("16 x 16 x 1 (256 thd)", fontSize = 11.sp, fontFamily = FontFamily.Monospace)
                        }
                    }
                    Box(
                        modifier = Modifier
                            .weight(1f)
                            .clip(RoundedCornerShape(8.dp))
                            .background(Color.Black.copy(alpha = 0.25f))
                            .padding(8.dp)
                    ) {
                        Column {
                            Text("Grid Size", fontSize = 10.sp, color = Color.Gray)
                            Text("64 x 64 = 4096 grp", fontSize = 11.sp, fontFamily = FontFamily.Monospace)
                        }
                    }
                    Box(
                        modifier = Modifier
                            .weight(1f)
                            .clip(RoundedCornerShape(8.dp))
                            .background(Color.Black.copy(alpha = 0.25f))
                            .padding(8.dp)
                    ) {
                        Column {
                            Text("Vulkan Queue", fontSize = 10.sp, color = Color.Gray)
                            Text("VK_QUEUE_COMPUTE", fontSize = 11.sp, fontFamily = FontFamily.Monospace)
                        }
                    }
                }
            }
        }

        // Shader Switcher Chips
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            shaderTitles.forEachIndexed { index, title ->
                val isSelected = selectedShaderIndex == index
                FilterChip(
                    selected = isSelected,
                    onClick = { selectedShaderIndex = index },
                    label = { Text(title, style = MaterialTheme.typography.bodySmall) },
                    modifier = Modifier
                        .weight(1f)
                        .testTag("tab_shader_$index")
                )
            }
        }

        // Code Viewer Card
        Card(
            modifier = Modifier
                .fillMaxWidth()
                .testTag("commodity_glsl_code_card"),
            shape = RoundedCornerShape(14.dp),
            colors = CardDefaults.cardColors(
                containerColor = Color(0xFF14141E)
            )
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(14.dp)
            ) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.spacedBy(6.dp)
                    ) {
                        Icon(
                            imageVector = Icons.Outlined.Code,
                            contentDescription = null,
                            tint = Color(0xFF80D8FF),
                            modifier = Modifier.size(16.dp)
                        )
                        Text(
                            text = "${shaderTitles[selectedShaderIndex]} SPIR-V Compute",
                            style = MaterialTheme.typography.labelMedium,
                            color = Color(0xFF80D8FF),
                            fontFamily = FontFamily.Monospace
                        )
                    }

                    Text(
                        text = "GLSL 450",
                        style = MaterialTheme.typography.labelSmall,
                        color = Color.Gray,
                        fontFamily = FontFamily.Monospace
                    )
                }

                Spacer(modifier = Modifier.height(10.dp))

                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .horizontalScroll(rememberScrollState())
                ) {
                    Text(
                        text = currentShaderSource,
                        style = MaterialTheme.typography.bodySmall.copy(
                            fontSize = 11.sp,
                            lineHeight = 16.sp
                        ),
                        fontFamily = FontFamily.Monospace,
                        color = Color(0xFFE0E0E0)
                    )
                }
            }
        }
    }
}
