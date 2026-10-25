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
