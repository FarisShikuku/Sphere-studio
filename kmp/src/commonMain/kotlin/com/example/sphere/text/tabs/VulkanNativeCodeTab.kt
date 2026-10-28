package com.example.sphere.text.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Code
import androidx.compose.material.icons.filled.DataObject
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.TextRenderMode

@Composable
fun VulkanNativeCodeTab(
    renderMode: TextRenderMode,
    smoothing: Float,
    thicknessBias: Float,
    enableOutline: Boolean,
    outlineWidth: Float,
    enableShadow: Boolean,
    shadowOffsetX: Float,
    shadowOffsetY: Float,
    enableGlow: Boolean,
    glowRadius: Float,
    modifier: Modifier = Modifier
) {
    var selectedSnippet by remember { mutableIntStateOf(0) }
    val snippetTitles = listOf("Vulkan Pipeline C++", "GLSL SDF Fragment Shader", "Quad Vertex Staging")

    Column(
        modifier = modifier
            .fillMaxSize()
            .padding(14.dp)
    ) {
        Text(
            "SASCHA WILLEMS VULKAN ARCHITECTURE",
            style = MaterialTheme.typography.labelSmall.copy(
                fontWeight = FontWeight.Bold,
                color = MaterialTheme.colorScheme.primary,
                letterSpacing = 1.sp
            )
        )
        Spacer(Modifier.height(8.dp))

        // Code Selector Chips
        SingleChoiceSegmentedButtonRow(modifier = Modifier.fillMaxWidth()) {
            snippetTitles.forEachIndexed { index, title ->
                SegmentedButton(
                    selected = selectedSnippet == index,
                    onClick = { selectedSnippet = index },
                    shape = SegmentedButtonDefaults.itemShape(index = index, count = snippetTitles.size),
                    icon = {}
                ) {
                    Text(
                        title,
                        fontSize = 11.sp,
                        fontWeight = if (selectedSnippet == index) FontWeight.Bold else FontWeight.Normal,
                        maxLines = 1
                    )
                }
            }
        }

        Spacer(Modifier.height(10.dp))

        val codeContent = when (selectedSnippet) {
            0 -> """
// Sascha Willems Vulkan Text Rendering Pipeline Setup
// Reference: SaschaWillems/Vulkan/examples/distancefieldfonts/
void VulkanTextPipeline::preparePipeline(VkRenderPass renderPass) {
    // 1. Blend State: Alpha Blending Enabled for Crisp Text Overlays
    VkPipelineColorBlendAttachmentState blendAttachmentState{};
    blendAttachmentState.blendEnable = VK_TRUE;
    blendAttachmentState.colorWriteMask = 
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

    // 2. Push Constants: Dynamic uniform updates per frame
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(TextPushConstants); // 144 bytes

    // 3. Pipeline Layout with Font Atlas Sampler (Descriptor Set 0)
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);

    // 4. Depth Stencil State
    VkPipelineDepthStencilStateCreateInfo depthStencilState{};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ${if (renderMode == TextRenderMode.TEXT_OVERLAY_2D) "depthStencilState.depthTestEnable = VK_FALSE;\ndepthStencilState.depthWriteEnable = VK_FALSE;" else "depthStencilState.depthTestEnable = VK_TRUE;\ndepthStencilState.depthWriteEnable = VK_FALSE;\ndepthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;"}

    // 5. Build Graphics Pipeline Object
    VkGraphicsPipelineCreateInfo pipelineCI{};
    pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCI.layout = pipelineLayout;
    pipelineCI.renderPass = renderPass;
    vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCI, nullptr, &pipeline);
}
""".trimIndent()

            1 -> """
#version 450
// Sascha Willems Signed Distance Field Fragment Shader
// Reflecting active viewport slider parameters
layout (binding = 0) uniform sampler2D samplerFontAtlas;

layout (push_constant) uniform PushConstants {
    mat4 projection;
    vec4 textColor;
    vec4 outlineColor;
    vec4 shadowColor;
    vec2 shadowOffset;      // [${"%.3f".format(shadowOffsetX * 0.001f)}, ${"%.3f".format(-shadowOffsetY * 0.001f)}]
    float outlineWidth;     // ${"%.3f".format(outlineWidth)}
    float outlineEdge;      // ${"%.3f".format(0.5f - thicknessBias)}
    float smoothing;        // ${"%.3f".format(smoothing)}
    float thicknessBias;    // ${"%.3f".format(thicknessBias)}
    float glowRadius;       // ${"%.3f".format(glowRadius)}
    vec4 glowColor;
    uint renderFlags;       // Flags: Outline=${enableOutline}, Shadow=${enableShadow}, Glow=${enableGlow}
} push;

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec2 inShadowUV;
layout (location = 0) out vec4 outFragColor;

void main() {
    // 1. Fetch distance value (0.5 = exact glyph boundary contour)
    float distance = texture(samplerFontAtlas, inUV).r;
    float edge = push.outlineEdge;
    float smoothDelta = max(push.smoothing, fwidth(distance) * 0.5);

    // 2. Core vector text alpha via smoothstep
    float textAlpha = smoothstep(edge - smoothDelta, edge + smoothDelta, distance);
    vec4 finalColor = inColor;

    ${if (enableShadow) """// 3. Drop Shadow pass
    float shadowDist = texture(samplerFontAtlas, inShadowUV).r;
    float shadowAlpha = smoothstep(edge - push.smoothing * 2.5, edge + push.smoothing * 2.5, shadowDist) * push.shadowColor.a;
    finalColor = mix(vec4(push.shadowColor.rgb, shadowAlpha), finalColor, textAlpha);""" else "// Drop shadow disabled"}

    ${if (enableOutline) """// 4. Distance field outline stroke
    float outlineThreshold = edge - push.outlineWidth;
    float outlineAlpha = smoothstep(outlineThreshold - smoothDelta, outlineThreshold + smoothDelta, distance);
    vec4 stroke = vec4(push.outlineColor.rgb, push.outlineColor.a * outlineAlpha);
    finalColor = mix(stroke, inColor, textAlpha);
    finalColor.a = max(textAlpha, outlineAlpha * push.outlineColor.a);""" else "// Outline disabled\n    finalColor.a *= textAlpha;"}

    if (finalColor.a < 0.01) discard;
    outFragColor = finalColor;
}
""".trimIndent()

            else -> """
// Sascha Willems Dynamic Quad Staging Buffer Update
// Maps host memory and commits 6 vertices per non-whitespace character
void VulkanTextOverlay::updateVertexBuffer() {
    VkDeviceSize bufferSize = vertices.size() * sizeof(TextVertex);

    // 1. Map staging memory
    void* mappedMemory = nullptr;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &mappedMemory);
    
    // 2. High-speed contiguous memcpy
    std::memcpy(mappedMemory, vertices.data(), bufferSize);
    
    // 3. Flush mapped memory range if host-coherent is not supported
    VkMappedMemoryRange memoryRange{};
    memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    memoryRange.memory = stagingBufferMemory;
    memoryRange.size = bufferSize;
    vkFlushMappedMemoryRanges(device, 1, &memoryRange);
    vkUnmapMemory(device, stagingBufferMemory);

    // 4. Asynchronously copy staging buffer to device-local vertex buffer
    VkBufferCopy copyRegion{};
    copyRegion.size = bufferSize;
    vkCmdCopyBuffer(copyCommandBuffer, stagingBuffer, deviceLocalVertexBuffer, 1, &copyRegion);
}
""".trimIndent()
        }

        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF090D14), RoundedCornerShape(8.dp))
                .padding(10.dp)
        ) {
            Column(
                modifier = Modifier
                    .fillMaxSize()
                    .verticalScroll(rememberScrollState())
                    .horizontalScroll(rememberScrollState())
            ) {
                Text(
                    text = codeContent,
                    style = MaterialTheme.typography.bodySmall.copy(
                        color = Color(0xFFD4E2F4),
                        fontFamily = FontFamily.Monospace,
                        fontSize = 10.sp,
                        lineHeight = 15.sp
                    )
                )
            }
        }
    }
}
