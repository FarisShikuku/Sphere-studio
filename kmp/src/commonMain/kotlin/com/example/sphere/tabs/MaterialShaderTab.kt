package com.example.sphere.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Code
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.PbrMaterial

@Composable
fun MaterialShaderTab(
    material: PbrMaterial,
    modifier: Modifier = Modifier
) {
    val glslCode = """
// Sascha Willems Vulkan PBR / Raytracing Shader Reference
// Model: Cook-Torrance Microfacet BRDF + Snell's Law Refraction
#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(set = 0, binding = 0) uniform SceneUBO {
    mat4 view;
    mat4 proj;
    vec4 cameraPos;
    vec4 lightPos;
} ubo;

layout(set = 1, binding = 0) uniform sampler2D bindlessTextures[];

struct PbrMaterial {
    vec4  baseColor;       // vec4(${material.baseColor.red.format(2)}, ${material.baseColor.green.format(2)}, ${material.baseColor.blue.format(2)}, ${material.baseColor.alpha.format(2)})
    float roughness;       // ${material.roughness.format(2)}
    float metallic;        // ${material.metallic.format(2)}
    float ior;             // ${material.ior.format(3)}
    float transmission;    // ${material.transmission.format(2)}
    float dispersion;      // ${material.dispersion.format(3)}
    float subsurface;      // ${material.subsurface.format(2)}
};

// 1. Normal Distribution Function (Trowbridge-Reitz GGX)
float D_GGX(float NdotH, float roughness) {
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = (NdotH * NdotH * (alpha2 - 1.0) + 1.0);
    return alpha2 / (3.14159265 * denom * denom);
}

// 2. Geometric Shadowing (Smith GGX correlated)
float G_SchlickSmithGGX(float NdotL, float NdotV, float roughness) {
    float k = ((roughness + 1.0) * (roughness + 1.0)) / 8.0;
    float GL = NdotL / (NdotL * (1.0 - k) + k);
    float GV = NdotV / (NdotV * (1.0 - k) + k);
    return GL * GV;
}

// 3. Fresnel-Schlick with Snell Refraction
vec3 F_Schlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// 4. Snell's Law Refraction & Chromatic Dispersion
vec3 EvaluateRefraction(vec3 I, vec3 N, float ior, float dispersion) {
    float etaR = 1.0 / (ior - dispersion);
    float etaG = 1.0 / ior;
    float etaB = 1.0 / (ior + dispersion);
    
    vec3 refractR = refract(I, N, etaR);
    vec3 refractG = refract(I, N, etaG);
    vec3 refractB = refract(I, N, etaB);
    return vec3(refractR.r, refractG.g, refractB.b);
}
""".trimIndent()

    Column(
        modifier = modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(10.dp)
    ) {
        Row(
            modifier = Modifier.fillMaxWidth(),
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.SpaceBetween
        ) {
            Row(verticalAlignment = Alignment.CenterVertically) {
                Icon(Icons.Default.Code, contentDescription = null, tint = MaterialTheme.colorScheme.primary)
                Spacer(Modifier.width(8.dp))
                Text(
                    "SPIR-V / GLSL PBR Pipeline",
                    style = MaterialTheme.typography.titleSmall
                )
            }
            Surface(
                color = MaterialTheme.colorScheme.primaryContainer,
                shape = RoundedCornerShape(6.dp)
            ) {
                Text(
                    "Sascha Willems PBR",
                    modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                    style = MaterialTheme.typography.labelSmall.copy(color = MaterialTheme.colorScheme.onPrimaryContainer)
                )
            }
        }

        Box(
            modifier = Modifier
                .fillMaxSize()
                .clip(RoundedCornerShape(8.dp))
                .background(Color(0xFF0D1117))
                .padding(12.dp)
                .testTag("pbr_glsl_code_box")
        ) {
            Text(
                text = glslCode,
                fontFamily = FontFamily.Monospace,
                fontSize = 11.sp,
                color = Color(0xFFC9D1D9),
                modifier = Modifier
                    .fillMaxSize()
                    .verticalScroll(rememberScrollState())
                    .horizontalScroll(rememberScrollState())
            )
        }
    }
}

private fun Float.format(digits: Int): String = "%.${digits}f".format(this)
