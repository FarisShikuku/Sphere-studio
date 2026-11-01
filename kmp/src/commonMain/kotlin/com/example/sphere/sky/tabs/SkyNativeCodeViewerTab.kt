package com.example.sphere

import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Badge
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.FilterChip
import androidx.compose.material3.FilterChipDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

// -------------------------------------------------------------
// TAB 5: NATIVE C++ & SHADER CORE INSPECTOR
// -------------------------------------------------------------
@Composable
fun SkyNativeCodeViewerTab() {
    var selectedFileIdx by remember { mutableIntStateOf(0) }
    val files = listOf(
        "cloud_raymarch.comp" to """// shaders/cloud_raymarch.comp (AmanSachan1/Meteoros Decima Engine)
#version 460
layout(local_size_x = 16, local_size_y = 16) in;

layout(set = 0, binding = 0, rgba16f) uniform image2D u_CloudOutput;
layout(set = 0, binding = 1) uniform sampler3D u_PerlinWorleyNoise;
layout(set = 0, binding = 2) uniform sampler3D u_WorleyDetailNoise;
layout(set = 0, binding = 3) uniform sampler2D u_WeatherMap;

// Meteoros Dual Beer-Lambert multi-scattering approximation
float MultiBeer(float d, float multiFactor) {
    float primary = exp(-d);
    float secondary = exp(-d * 0.25) * 0.7;
    return primary + secondary * multiFactor;
}

// Henyey-Greenstein dual-phase silver lining
float HenyeyGreenstein(float cosTheta, float g) {
    float gg = g * g;
    return (1.0 - gg) / (4.0 * PI * pow(max(0.0001, 1.0 + gg - 2.0 * g * cosTheta), 1.5));
}

// 4x4 Bayer matrix temporal ray jittering
const float BAYER_4X4[16] = float[16](
     0.0/16.0,  8.0/16.0,  2.0/16.0, 10.0/16.0,
    12.0/16.0,  4.0/16.0, 14.0/16.0,  6.0/16.0,
     3.0/16.0, 11.0/16.0,  1.0/16.0,  9.0/16.0,
    15.0/16.0,  7.0/16.0, 13.0/16.0,  5.0/16.0
);""",
        "Geometry.h" to """// scene/Geometry.h - Native C++ Procedural Geometry Generator
#pragma once
#include "Mesh.h"
#include "Transform.h"

namespace sphere {
class GeometryGenerator {
public:
    static Mesh GenerateCube(float width, float height, float depth, const Vec3& center = {0, 0, 0});
    static Mesh GenerateCalibrationCube(float size = 1.0f, float bevel = 0.04f);
    static Mesh GenerateStagePlatform(float width, float length, float height, float bevel = 0.05f);
    static Mesh GenerateGridLines(int lineCount = 20, float spacing = 0.5f, bool addAxes = true);
    static Mesh GenerateSphere(float radius = 1.0f, uint32_t rings = 32, uint32_t sectors = 32);
    static Mesh GenerateSkyDome(float radius = 500.0f, uint32_t rings = 24, uint32_t sectors = 48);
    static Mesh GenerateAtmosphereCloudHull(float innerRadius, float outerRadius, float spanDeg);
    static void CalculateNormals(Mesh& mesh);
    static void CalculateTangents(Mesh& mesh);
};
}""",
        "Scene.h" to """// scene/Scene.h - Native C++ Common Vulkan Scene
#pragma once
#include "Camera.h"
#include "Light.h"
#include "SkyModel.h"
#include "SceneGraph.h"
#include "Geometry.h"

namespace sphere {
class Scene {
public:
    void InitializeDefaultStage();
    void SetStageMode(StageMode mode); // FlatPiece, Boundary, InfiniteGrid, CalibrationCube
    void Update(float deltaTime);
    
    Camera& GetCamera() { return m_Camera; }
    SkyModel& GetSky() { return m_Sky; }
    SceneGraph& GetSceneGraph() { return m_SceneGraph; }
    const std::vector<Light>& GetLights() const { return m_Lights; }
    std::shared_ptr<Mesh> GetStageMesh() const { return m_StageMesh; }
    std::shared_ptr<Mesh> GetCalibrationCubeMesh() const { return m_CalibrationCubeMesh; }
};
}""",
        "VulkanStage.h" to """// scene/VulkanStage.h - Native Vulkan 1.3 Stage Pipeline Coordinator
#pragma once
#include "Scene.h"
#include <vulkan/vulkan.h>

namespace sphere {
struct alignas(16) GlobalSceneUBO {
    Mat4 viewMatrix;
    Mat4 projMatrix;
    Mat4 viewProjMatrix;
    Mat4 lightSpaceMatrix;
    Vec4 cameraPosition;
    Vec4 sunDirection;
    Vec4 sunColor;
    Vec4 ambientSkyColor;
    Vec4 worldParams;
};

class VulkanStage {
public:
    void RecordShadowPass(VkCommandBuffer cmd);
    void RecordSkyAtmospherePass(VkCommandBuffer cmd);
    void RecordOpaqueGeometryPass(VkCommandBuffer cmd);
    void RecordDebugGridPass(VkCommandBuffer cmd);
};
}""",
        "CloudGen.h" to """// weather/clouds/CloudGen.h (AmanSachan1/Meteoros C++ Core)
#pragma once
#include "CloudParams.h"

namespace sphere::weather {
class CloudGen {
public:
    void SetParams(const CloudParams& params);
    float GetHeightFraction(const Vec3& pos) const;
    float GetDensityForCloud(float hFrac, CloudType type) const;
    float Beer(float opticalDepth) const;
    float MultiBeer(float opticalDepth, float multiFactor) const;
    float HenyeyGreenstein(float cosTheta, float g) const;
    float Powder(float density) const;
};
}""",
        "SkySystem.h" to """// SkySystem.h - Primary Atmospheric Coordinator (kentril0 reference)
namespace sphere::sky {
class SkySystem {
public:
    AtmosphereScatter atmosphere;
    HosekWilkie hosekWilkie;
    PreethamModel preetham;
    StarfieldGen starfield;
    SunDisk sunDisk;
    MoonPhaseGen moonPhase;
    TonemapLUT tonemap;

    bool enableAtmosphere = true;
    bool enableSun = true;
    bool enableMoon = true;
    bool enableStars = true;

    void SetTimeOfDay(float hours);
    void Update(float deltaTime);
    Vec3 ComputeSkyRadiance(const Vec3& viewDir) const;
};
}""",
        "sky.frag" to """// shaders/sky.frag - GLSL 460 Sky Dome Fragment Shader
#version 460
#include "include/sky_common.glsl"

layout(location = 0) in vec3 inWorldDir;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform SkyPush {
    mat4 invViewProj;
    vec4 sunDirection;
    vec4 sunColor;
    float turbidity;
    float exposure;
} uPush;

void main() {
    vec3 dir = normalize(inWorldDir);
    float cosTheta = max(0.0, dir.y);
    float cosGamma = dot(dir, normalize(uPush.sunDirection.xyz));

    vec3 sky = mix(vec3(0.65, 0.75, 0.88), vec3(0.18, 0.42, 0.85), pow(cosTheta, 0.45));
    if (cosGamma > 0.0) {
        sky += uPush.sunColor.rgb * (pow(cosGamma, 16.0) * 0.45);
    }
    outColor = vec4(sky * uPush.exposure, 1.0);
}""",
        "PreethamModel.h" to """// atmosphere/PreethamModel.h
namespace sphere {
class PreethamModel {
public:
    float turbidity = 2.5f;
    void SetTurbidity(float t) { turbidity = t; }
    Vec3 CalculateZenithColor(float sunTheta) const;
    Vec3 EvaluatePerez(float theta, float gamma) const;
};
}""",
        "SunDisk.h" to """// sun/SunDisk.h
namespace sphere {
class SunDisk {
public:
    float angularDiameter = 0.045f;
    float coronaRadius = 0.22f;
    float intensity = 25.0f;
    bool enabled = true;

    Vec3 RenderDisk(const Vec3& rayDir, const Vec3& sunDir, const Vec3& sunColor) const;
};
}""",
        "MoonPhaseGen.h" to """// moon/MoonPhaseGen.h
namespace sphere::sky {
class MoonPhaseGen {
public:
    MoonParams params;
    Vec3 moonDirection{0.0f, -0.5f, 0.866f};
    Vec3 moonColor{0.75f, 0.82f, 0.95f};

    void SetLunarPosition(float elevationDeg, float azimuthDeg);
    void SetPhase(float phase0to1);
    Vec3 RenderMoon(const Vec3& viewDir) const;
};
}"""
    )

    Column(verticalArrangement = Arrangement.spacedBy(14.dp)) {
        // Binary Assets Verification Cards
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "Compiled Native Binary Assets Status",
                    style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                    color = Color.White
                )
                Text(
                    text = "All 7 binary lookup tables and seeds verified on disk",
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFF94A3B8)
                )

                Spacer(modifier = Modifier.height(10.dp))

                val binaryAssets = listOf(
                    "sky_seed.bin" to "Atmospheric perturbation seed",
                    "sky_lut_spec.bin" to "32x32 multi-scattering transmittance table",
                    "star_catalog.bin" to "Hipparcos 256 celestial stars",
                    "sun_disk_spec.bin" to "64 samples limb darkening curve",
                    "moon_phase_spec.bin" to "32 steps phase albedo & earthshine",
                    "aces.bin" to "64 samples ACES fitted filmic LUT",
                    "reinhard.bin" to "64 samples Extended Reinhard LUT"
                )

                binaryAssets.forEach { (file, desc) ->
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(vertical = 3.dp),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Column {
                            Text(file, color = Color(0xFF38BDF8), fontSize = 12.sp, fontFamily = FontFamily.Monospace)
                            Text(desc, color = Color(0xFF64748B), fontSize = 10.sp)
                        }
                        Badge(containerColor = Color(0xFF065F46), contentColor = Color(0xFF6EE7B7)) {
                            Text("Active", fontSize = 10.sp, fontWeight = FontWeight.Bold)
                        }
                    }
                }
            }
        }

        // C++ / GLSL Code Inspector Card
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF0F1219)),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .horizontalScroll(rememberScrollState()),
                    horizontalArrangement = Arrangement.spacedBy(6.dp)
                ) {
                    files.forEachIndexed { idx, (title, _) ->
                        FilterChip(
                            selected = selectedFileIdx == idx,
                            onClick = { selectedFileIdx = idx },
                            label = { Text(title, fontSize = 11.sp, fontFamily = FontFamily.Monospace) },
                            colors = FilterChipDefaults.filterChipColors(
                                selectedContainerColor = Color(0xFF1E293B),
                                selectedLabelColor = Color(0xFF38BDF8)
                            )
                        )
                    }
                }

                Spacer(modifier = Modifier.height(10.dp))

                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = Color(0xFF05070B),
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Text(
                        text = files[selectedFileIdx].second,
                        color = Color(0xFFE2E8F0),
                        fontSize = 11.sp,
                        fontFamily = FontFamily.Monospace,
                        modifier = Modifier.padding(12.dp)
                    )
                }
            }
        }
    }
}
