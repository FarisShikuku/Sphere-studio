package com.example.sphere.lighting.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Code
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
import com.example.sphere.model.LightSourceParameters

@Composable
fun LightVulkanCodeTab(
    params: LightSourceParameters,
    modifier: Modifier = Modifier
) {
    var selectedFileIndex by remember { mutableStateOf(0) }
    val fileNames = listOf(
        "VulkanPipeline.cpp",
        "SdfGiSystem.cpp",
        "Atmosphere.cpp",
        "CascadedShadows.cpp",
        "PlainMesh.h",
        "sdf_diffuse_gi.comp",
        "plain_pbr.frag",
        "sky_atmosphere.frag"
    )

    Column(
        modifier = modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp)
    ) {
        ScrollableTabRow(
            selectedTabIndex = selectedFileIndex,
            edgePadding = 0.dp,
            modifier = Modifier.fillMaxWidth()
        ) {
            fileNames.forEachIndexed { index, name ->
                Tab(
                    selected = selectedFileIndex == index,
                    onClick = { selectedFileIndex = index },
                    text = { Text(name, style = MaterialTheme.typography.labelSmall) },
                    modifier = Modifier.testTag("code_tab_$index")
                )
            }
        }

        val codeSnippet = when (selectedFileIndex) {
            0 -> """
// native_assets/src/lighting/VulkanLightPipeline.cpp
// PlainRenderer Master Multi-Pass Vulkan Render Coordinator

void VulkanLightPipeline::ExecuteFrame(uint32_t width, uint32_t height) {
    // Pass 1: Cascaded Shadow Map Pass (4 splits CSM)
    m_cascadedShadows.SetLightDirection(m_atmosphere.GetSunDirection());
    
    // Pass 2: Deferred G-Buffer (Albedo, Normal, Material, Velocity)
    
    // Pass 3: SDF Diffuse GI Compute Pass (vkCmdDispatch 3D volume raymarch)
    m_sdfGi.SetDebugViewMode(static_cast<int>(m_debugView));
    
    // Pass 4: Physical Atmosphere & Skybox (Rayleigh + Mie single scattering)
    
    // Pass 5: Deferred PBR Lighting Resolve (Direct + Shadows + SDF Indirect GI + Sky)
    
    // Pass 6: TAA Resolve Pass (16-sample Halton subpixel jitter accumulation)
    m_taa.SetResolution(width, height);
    m_taa.AdvanceFrame();
}
            """.trimIndent()

            1 -> """
// native_assets/src/lighting/SdfGiSystem.cpp
// Continuous 3D Signed Distance Field Diffuse GI Cone-Tracing

Vec3 SdfGiSystem::EvaluateIndirectDiffuse(const Vec3& surfacePos, const Vec3& normal, const Vec3& surfaceAlbedo) const {
    // PlainRenderer Cone-Tracing: march multiple diffuse cones along hemisphere
    constexpr int kConeCount = 6;
    Vec3 accumulatedRadiance{0.0f, 0.0f, 0.0f};
    float totalWeight = 0.0f;
    float startOffset = 0.08f; // Normal bias to escape starting surface

    for (int i = 0; i < kConeCount; ++i) {
        Vec3 worldConeDir = ComputeHemisphereConeDirection(normal, i);
        float cosTheta = std::max(0.0f, Vec3::Dot(worldConeDir, normal));
        if (cosTheta <= 0.0f) continue;

        float t = startOffset;
        for (int step = 0; step < 16; ++step) {
            Vec3 samplePoint = surfacePos + worldConeDir * t;
            float dist = SampleDistance(samplePoint);

            if (dist < 0.04f) {
                // Hit surface in distance field: gather bouncing albedo color
                Vec3 bounceAlbedo = QuerySurfaceAlbedo(samplePoint);
                accumulatedRadiance += bounceAlbedo * cosTheta;
                break;
            }
            t += std::max(dist * m_stepSizeFactor, 0.05f);
            if (t > m_maxRayDistance) break;
        }
        totalWeight += cosTheta;
    }
    return (accumulatedRadiance / totalWeight) * m_bounceFactor * m_indirectColorBoost;
}
            """.trimIndent()

            2 -> """
// native_assets/src/lighting/Atmosphere.cpp
// Physically Based Sky & Sun Illuminance (Preetham / Bruneton)

Vec3 Atmosphere::ComputeSunTransmittance() const {
    // Air mass calculation via Kasten-Young formula
    float zenithAngle = std::max(0.0f, 90.0f - m_sunElevationDeg);
    float zenithRad = zenithAngle * 3.14159265f / 180.0f;
    float cosZ = std::max(0.01f, std::cos(zenithRad));
    float airMass = 1.0f / (cosZ + 0.50572f * std::pow(96.07995f - zenithAngle, -1.6364f));

    // Optical depths for Rayleigh (beta_R), Mie (beta_M), and Ozone (beta_O)
    Vec3 betaR = m_rayleighCoeffs * m_rayleighScaleHeight;
    Vec3 betaM = m_mieCoeffs * m_mieScaleHeight * (m_turbidity * 0.4f);
    Vec3 betaO = m_ozoneCoeffs * 4000.0f;

    Vec3 opticalDepthTotal = (betaR + betaM + betaO) * airMass;
    return Vec3{ std::exp(-opticalDepthTotal.x), std::exp(-opticalDepthTotal.y), std::exp(-opticalDepthTotal.z) };
}
            """.trimIndent()

            3 -> """
// native_assets/src/lighting/CascadedShadows.cpp
// 4-Split Directional Cascaded Shadow Maps with Texel Stabilization

void CascadedShadows::UpdateCascades() {
    for (size_t i = 0; i < kCascadeCount; ++i) {
        float p = static_cast<float>(i + 1) / static_cast<float>(kCascadeCount);
        float logSplit = m_cameraNear * std::pow(m_cameraFar / m_cameraNear, p);
        float uniformSplit = m_cameraNear + (m_cameraFar - m_cameraNear) * p;
        m_cascadeSplits[i] = m_splitLambda * logSplit + (1.0f - m_splitLambda) * uniformSplit;
    }

    // Sub-texel grid stabilization: snap frustum center to texel units
    float shadowMapSize = static_cast<float>(m_shadowMapResolution);
    float texelSize = (2.0f * sphereRadius) / shadowMapSize;
    centerLightSpace.x = std::floor(centerLightSpace.x / texelSize) * texelSize;
    centerLightSpace.y = std::floor(centerLightSpace.y / texelSize) * texelSize;
    m_cascadeMatrices[cascadeIdx] = lightProj * lightView;
}
            """.trimIndent()

            4 -> """
// native_assets/src/lighting/PlainMeshFormat.h
// PlainRenderer Custom Binary Format: Mesh + Embedded 3D Signed Distance Field

struct PlainHeader {
    char magic[8];             // "PLAIN_SDF\0"
    uint32_t version;          // 1
    uint32_t vertexCount;
    uint32_t indexCount;
    uint32_t sdfDimX;          // SDF voxel resolution (e.g. 64)
    uint32_t sdfDimY;          // SDF voxel resolution (e.g. 64)
    uint32_t sdfDimZ;          // SDF voxel resolution (e.g. 64)
    float boundsMin[3];        // World AABB bounds min
    float boundsMax[3];        // World AABB bounds max
    float voxelSpacing;        // Grid cell pitch in meters
};
            """.trimIndent()

            5 -> """
// native_assets/shaders/glsl/sdf_diffuse_gi.comp
// Compute shader dispatching 3D SDF Cone-Tracing for Indirect Diffuse GI

#version 450
#include "include/sdf_gi_common.glsl"

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(set = 0, binding = 0) uniform SdfGiUBO { GpuSdfGiData u_sdfGi; };
layout(set = 0, binding = 1) uniform sampler3D u_sdfTexture;
layout(set = 0, binding = 2) uniform sampler3D u_albedoTexture;
layout(set = 0, binding = 5, rgba16f) uniform writeonly image2D u_indirectGiOutput;

void main() {
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
    vec3 normal = normalize(texture(u_gbufferWorldNormal, uv).xyz);
    vec3 worldPos = texture(u_gbufferWorldPos, uv).xyz;

    float dfao = evaluateSdfAmbientOcclusion(u_sdfTexture, u_sdfGi, worldPos, normal);
    vec3 indirectDiffuse = coneTraceIndirectDiffuse(u_sdfTexture, u_albedoTexture, u_sdfGi, worldPos, normal);

    imageStore(u_indirectGiOutput, pixelCoord, vec4(indirectDiffuse, dfao));
}
            """.trimIndent()

            6 -> """
// native_assets/shaders/glsl/plain_renderer_pbr.frag
// PlainRenderer Deferred PBR Lighting: Direct + Shadows + SDF Diffuse GI + Sky

void main() {
    // 1. Direct Illumination (Punctual or Sun)
    vec3 directLit = evaluateLightIllumination(u_primaryLight, worldPos, normal, V, albedo, roughness, metallic);
    
    // 2. Cascaded Shadow Attenuation (4-Cascade PCF 5x5)
    float shadow = sampleCascadeShadowPCF(u_shadowMapArray, u_cascadedShadows, worldPos, normal, viewDist);
    directLit *= shadow;

    // 3. Indirect Diffuse GI (SDF Cone-Traced Bounce)
    vec3 indirectDiffuse = (indirectGiAndAo.rgb + skyAmbient) * albedo * dfao;

    // 4. Final Combined Radiance
    outFinalColor = vec4(directLit + indirectDiffuse, 1.0);
}
            """.trimIndent()

            else -> """
// native_assets/shaders/glsl/sky_atmosphere.frag
// Physically Based Single-Scattering Atmosphere Skybox

#version 450
#include "include/atmosphere_common.glsl"

layout(location = 0) in vec3 inViewRay;
layout(location = 0) out vec4 outSkyRadiance;

void main() {
    vec3 dir = normalize(inViewRay);
    vec3 radiance = evaluateSkyRadiance(u_atmosphere, dir);
    outSkyRadiance = vec4(radiance / (radiance + vec3(1.0)), 1.0);
}
            """.trimIndent()
        }

        Surface(
            shape = RoundedCornerShape(8.dp),
            color = Color(0xFF0F141C),
            modifier = Modifier
                .fillMaxWidth()
                .weight(1f)
        ) {
            Box(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(12.dp)
                    .verticalScroll(rememberScrollState())
                    .horizontalScroll(rememberScrollState())
            ) {
                Text(
                    text = codeSnippet,
                    fontFamily = FontFamily.Monospace,
                    fontSize = 11.sp,
                    color = Color(0xFFCFD8DC),
                    lineHeight = 16.sp
                )
            }
        }
    }
}
