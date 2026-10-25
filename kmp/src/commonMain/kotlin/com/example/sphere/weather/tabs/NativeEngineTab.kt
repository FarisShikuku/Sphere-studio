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
                text = "Converted Robert Beckebans Vulkan Modules",
                style = MaterialTheme.typography.titleMedium,
                color = Color(0xFF4ADE80),
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(10.dp))

            val modules = listOf(
                Triple("CloudRaymarch.cpp", "native_assets/src/weather/clouds/CloudRaymarch.cpp", "Beckebans planetary shell raymarcher & 6-cone light march"),
                Triple("CloudGen.cpp", "native_assets/src/weather/clouds/CloudGen.cpp", "Nubis Stratus/Cumulus gradients, Beer's law & powder effect"),
                Triple("cloud_raymarch.comp", "native_assets/src/weather/clouds/shaders/cloud_raymarch.comp", "Vulkan 1.3 compute shader converted from volumetric_clouds.frag"),
                Triple("CloudParams.h", "native_assets/src/weather/clouds/CloudParams.h", "Dual-lobe HG parameters, earth radius & optical scattering"),
                Triple("test_clouds.cpp", "native_assets/tests/weather/test_clouds.cpp", "Complete test suite validating Beckebans cloud physics (PASS)"),
                Triple("WeatherSystem.cpp", "native_assets/src/weather/WeatherSystem.cpp", "Precipitation, wetness integration & world state sync"),
                Triple("rain_particle.comp", "native_assets/src/weather/rain/shaders/rain_particle.comp", "GPU compute 65,536 rain particles with wind deflection"),
                Triple("fog_froxel.comp", "native_assets/src/weather/fog/shaders/fog_froxel.comp", "160x90x64 3D froxel volume grid with Mie inscattering")
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
