package com.example.sphere

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
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

import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll

@Composable
fun NativeEngineTab() {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(bottom = 28.dp)
    ) {
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.padding(bottom = 12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "AmanSachan1/Meteoros Vulkan Architecture",
                    style = MaterialTheme.typography.titleMedium,
                    color = Color(0xFF4ADE80),
                    fontWeight = FontWeight.Bold
                )
                Text(
                    text = "High-performance Vulkan 1.3 / C++20 compute pipelines from AmanSachan1/Meteoros",
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFF94A3B8)
                )
                Spacer(modifier = Modifier.height(10.dp))

                val modules = listOf(
                    Triple("cloud_raymarch.comp", "native_assets/src/weather/clouds/shaders/cloud_raymarch.comp", "Meteoros Vulkan compute shader with Dual Beer-Lambert multi-scattering & 4x4 Bayer jitter"),
                    Triple("cloud_godrays.comp", "native_assets/src/weather/clouds/shaders/cloud_godrays.comp", "Meteoros crepuscular volumetric god-rays post-processing with exponential decay"),
                    Triple("cloud_reproject.comp", "native_assets/src/weather/clouds/shaders/cloud_reproject.comp", "Meteoros temporal reprojection & 3x3 neighborhood bounding-box clamping"),
                    Triple("CloudRaymarch.cpp", "native_assets/src/weather/clouds/CloudRaymarch.cpp", "Meteoros planetary shell raymarcher & 6-cone light march with god-ray occlusion map"),
                    Triple("CloudGen.cpp", "native_assets/src/weather/clouds/CloudGen.cpp", "Nubis Stratus, Stratocumulus, Cumulus & Cumulonimbus gradients with MultiBeer law"),
                    Triple("CloudParams.h", "native_assets/src/weather/clouds/CloudParams.h", "Dual-lobe HG parameters (g1=0.82, g2=-0.22), MultiBeer factor & ACES tone mapping"),
                    Triple("test_clouds.cpp", "native_assets/tests/weather/test_clouds.cpp", "Complete test suite validating AmanSachan1/Meteoros cloud physics (PASS)"),
                    Triple("WeatherSystem.cpp", "native_assets/src/weather/WeatherSystem.cpp", "Precipitation, wetness integration & world state sync"),
                    Triple("rain_particle.comp", "native_assets/src/weather/rain/shaders/rain_particle.comp", "GPU compute 65,536 rain particles with wind deflection"),
                    Triple("fog_froxel.comp", "native_assets/src/weather/fog/shaders/fog_froxel.comp", "160x90x64 3D froxel volume grid with Mie inscattering"),
                    Triple("WeatherAudioSystem.cpp", "native_assets/src/gen/audio/weather/src/WeatherAudioSystem.cpp", "44.1kHz procedural rain, wind gusts, snow shimmer & RAM thunder PCM"),
                    Triple("WeatherAudioMixer.cpp", "native_assets/src/gen/audio/weather/src/WeatherAudioMixer.cpp", "Soft-clipped zero-latency weather audio PCM16 mixer"),
                    Triple("rain_granular.comp", "native_assets/src/gen/audio/weather/shaders/rain_granular.comp", "Vulkan compute procedural rain sheet & droplet splash synth"),
                    Triple("wind_thunder_osc.comp", "native_assets/src/gen/audio/weather/shaders/wind_thunder_osc.comp", "Vulkan compute sweeping gale wind, fog bed & thunder synthesis")
                )

                modules.forEach { (name, path, desc) ->
                    Surface(
                        shape = RoundedCornerShape(8.dp),
                        color = Color(0xFF0F172A),
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(vertical = 4.dp)
                    ) {
                        Column(modifier = Modifier.padding(10.dp)) {
                            Row(
                                modifier = Modifier.fillMaxWidth(),
                                horizontalArrangement = Arrangement.SpaceBetween,
                                verticalAlignment = Alignment.CenterVertically
                            ) {
                                Text(
                                    text = name,
                                    fontSize = 12.sp,
                                    fontWeight = FontWeight.Bold,
                                    color = Color(0xFF38BDF8)
                                )
                                Surface(
                                    shape = RoundedCornerShape(4.dp),
                                    color = Color(0xFF166534).copy(alpha = 0.5f)
                                ) {
                                    Text(
                                        text = "VULKAN 1.3",
                                        fontSize = 9.sp,
                                        fontWeight = FontWeight.Bold,
                                        color = Color(0xFF4ADE80),
                                        modifier = Modifier.padding(horizontal = 4.dp, vertical = 2.dp)
                                    )
                                }
                            }
                            Text(
                                text = path,
                                fontSize = 10.sp,
                                fontFamily = FontFamily.Monospace,
                                color = Color(0xFF64748B)
                            )
                            Spacer(modifier = Modifier.height(2.dp))
                            Text(
                                text = desc,
                                fontSize = 11.sp,
                                color = Color(0xFFCBD5E1)
                            )
                        }
                    }
                }
            }
        }
    }
}
