package com.example.sphere

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
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
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.AutoAwesome
import androidx.compose.material.icons.filled.Cloud
import androidx.compose.material.icons.filled.FilterDrama
import androidx.compose.material.icons.filled.LightMode
import androidx.compose.material.icons.filled.ScatterPlot
import androidx.compose.material.icons.filled.Speed
import androidx.compose.material.icons.filled.WbSunny
import androidx.compose.material3.Badge
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.FilterChip
import androidx.compose.material3.FilterChipDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Slider
import androidx.compose.material3.SliderDefaults
import androidx.compose.material3.Surface
import androidx.compose.material3.Switch
import androidx.compose.material3.SwitchDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.CloudGenus

/**
 * AmanSachan1/Meteoros Volumetric Cloudscape Controls Panel
 * Based on Guerrilla Games' Decima Engine (Nubis 2015/2017)
 */
@Composable
fun MeteorosVolumetricsTab(
    cloudType: Int,
    onCloudTypeChange: (Int) -> Unit,
    cloudCoverage: Float,
    onCloudCoverageChange: (Float) -> Unit,
    cloudDensity: Float,
    onCloudDensityChange: (Float) -> Unit,
    multiScatteringFactor: Float,
    onMultiScatteringFactorChange: (Float) -> Unit,
    silverLiningG: Float,
    onSilverLiningGChange: (Float) -> Unit,
    powderEffect: Float,
    onPowderEffectChange: (Float) -> Unit,
    sunAbsorption: Float,
    onSunAbsorptionChange: (Float) -> Unit,
    detailErosion: Float,
    onDetailErosionChange: (Float) -> Unit,
    enableGodRays: Boolean,
    onEnableGodRaysChange: (Boolean) -> Unit,
    godRaysIntensity: Float,
    onGodRaysIntensityChange: (Float) -> Unit,
    enableCirrus: Boolean,
    onEnableCirrusChange: (Boolean) -> Unit,
    cirrusCoverage: Float,
    onCirrusCoverageChange: (Float) -> Unit,
    windSpeed: Float,
    onWindSpeedChange: (Float) -> Unit,
    windAzimuth: Float,
    onWindAzimuthChange: (Float) -> Unit,
    enableBayerJitter: Boolean,
    onEnableBayerJitterChange: (Boolean) -> Unit
) {
    Column(
        modifier = Modifier
            .fillMaxWidth()
            .padding(bottom = 24.dp)
            .testTag("meteoros_volumetrics_tab")
    ) {
        // Header Card: Engine info & citations
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.padding(bottom = 12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Row(verticalAlignment = Alignment.CenterVertically) {
                        Icon(
                            imageVector = Icons.Default.FilterDrama,
                            contentDescription = null,
                            tint = Color(0xFF38BDF8),
                            modifier = Modifier.size(20.dp)
                        )
                        Spacer(modifier = Modifier.width(8.dp))
                        Text(
                            text = "Meteoros Volumetric Cloudscape Engine",
                            style = MaterialTheme.typography.titleMedium.copy(fontWeight = FontWeight.Bold),
                            color = Color.White
                        )
                    }
                    Badge(
                        containerColor = Color(0xFF0284C7).copy(alpha = 0.25f),
                        contentColor = Color(0xFF38BDF8)
                    ) {
                        Text("Vulkan / Decima", fontSize = 10.sp, fontWeight = FontWeight.Bold)
                    }
                }
                Spacer(modifier = Modifier.height(4.dp))
                Text(
                    text = "AmanSachan1/Meteoros real-time volumetric raymarching based on Guerrilla Games' Decima Engine (Nubis). Features dual Beer-Lambert multi-scattering, Henyey-Greenstein silver lining, and crepuscular god rays.",
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFF94A3B8),
                    fontSize = 11.sp
                )
            }
        }

        // Section 1: Cloud Species Height Gradient
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.padding(bottom = 12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "Cloud Species Height Profile",
                    style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                    color = Color.White
                )
                Text(
                    text = "Altitude density curve per Decima weather map specification",
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFF64748B),
                    fontSize = 11.sp
                )
                Spacer(modifier = Modifier.height(10.dp))

                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .horizontalScroll(rememberScrollState()),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    val species = listOf(
                        Triple(0, "Stratus (St)", "Flat low blanket"),
                        Triple(1, "Stratocumulus (Sc)", "Rolling waves"),
                        Triple(2, "Cumulus (Cu)", "Cauliflower heaps"),
                        Triple(3, "Cumulonimbus (Cb)", "Anvil thunderhead")
                    )
                    species.forEach { (typeIdx, name, desc) ->
                        val isSelected = cloudType == typeIdx
                        FilterChip(
                            selected = isSelected,
                            onClick = { onCloudTypeChange(typeIdx) },
                            label = {
                                Column(modifier = Modifier.padding(vertical = 2.dp)) {
                                    Text(name, fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal)
                                    Text(desc, fontSize = 9.sp, color = if (isSelected) Color(0xFFBAE6FD) else Color(0xFF64748B))
                                }
                            },
                            colors = FilterChipDefaults.filterChipColors(
                                selectedContainerColor = Color(0xFF0284C7).copy(alpha = 0.85f),
                                selectedLabelColor = Color.White,
                                containerColor = Color(0xFF1E2433),
                                labelColor = Color(0xFFCBD5E1)
                            )
                        )
                    }
                }
            }
        }

        // Section 2: Coverage & Density
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.padding(bottom = 12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "Coverage & Morphology",
                    style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                    color = Color.White
                )
                Spacer(modifier = Modifier.height(8.dp))

                SkySliderItem(
                    label = "Weather Map Coverage",
                    value = cloudCoverage,
                    onValueChange = onCloudCoverageChange,
                    range = 0.0f..1.0f,
                    displayValue = "${(cloudCoverage * 100).toInt()}%"
                )

                SkySliderItem(
                    label = "Optical Density Multiplier",
                    value = cloudDensity,
                    onValueChange = onCloudDensityChange,
                    range = 0.2f..2.5f,
                    displayValue = "${(cloudDensity * 100).toInt() / 100f}x"
                )

                SkySliderItem(
                    label = "Worley Detail Erosion",
                    value = detailErosion,
                    onValueChange = onDetailErosionChange,
                    range = 0.0f..1.0f,
                    displayValue = "${(detailErosion * 100).toInt()}%"
                )
            }
        }

        // Section 3: Radiative Transfer & Scattering (Meteoros MultiBeer + Henyey-Greenstein)
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.padding(bottom = 12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "Volumetric Lighting & Radiative Transfer",
                    style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                    color = Color.White
                )
                Text(
                    text = "Beer-Lambert multi-scattering and Henyey-Greenstein dual-phase silver lining",
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFF64748B),
                    fontSize = 11.sp
                )
                Spacer(modifier = Modifier.height(8.dp))

                SkySliderItem(
                    label = "Dual Beer Multi-Scattering",
                    value = multiScatteringFactor,
                    onValueChange = onMultiScatteringFactorChange,
                    range = 0.0f..1.0f,
                    displayValue = "${(multiScatteringFactor * 100).toInt()}%"
                )

                SkySliderItem(
                    label = "Silver Lining Peak (g₁ Forward)",
                    value = silverLiningG,
                    onValueChange = onSilverLiningGChange,
                    range = 0.50f..0.95f,
                    displayValue = "${(silverLiningG * 1000).toInt() / 1000f}"
                )

                SkySliderItem(
                    label = "Powder Sugar Effect",
                    value = powderEffect,
                    onValueChange = onPowderEffectChange,
                    range = 0.0f..1.0f,
                    displayValue = "${(powderEffect * 100).toInt()}%"
                )

                SkySliderItem(
                    label = "Solar Ray Extinction (Absorption)",
                    value = sunAbsorption,
                    onValueChange = onSunAbsorptionChange,
                    range = 0.1f..1.5f,
                    displayValue = "${(sunAbsorption * 100).toInt() / 100f}"
                )
            }
        }

        // Section 4: Crepuscular Sunbeams (God Rays)
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.padding(bottom = 12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column {
                        Text(
                            text = "Crepuscular God Rays (Sun Shafts)",
                            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                            color = Color.White
                        )
                        Text(
                            text = "Volumetric radial light shaft scattering through cloud openings",
                            style = MaterialTheme.typography.bodySmall,
                            color = Color(0xFF64748B),
                            fontSize = 11.sp
                        )
                    }
                    Switch(
                        checked = enableGodRays,
                        onCheckedChange = onEnableGodRaysChange,
                        colors = SwitchDefaults.colors(
                            checkedThumbColor = Color(0xFFFBBF24),
                            checkedTrackColor = Color(0xFFB45309)
                        )
                    )
                }

                if (enableGodRays) {
                    Spacer(modifier = Modifier.height(8.dp))
                    SkySliderItem(
                        label = "Light Shaft Intensity",
                        value = godRaysIntensity,
                        onValueChange = onGodRaysIntensityChange,
                        range = 0.1f..1.0f,
                        displayValue = "${(godRaysIntensity * 100).toInt()}%"
                    )
                }
            }
        }

        // Section 5: High-Altitude Cirrus & Wind
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.padding(bottom = 12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column {
                        Text(
                            text = "High-Altitude Cirrus Layer",
                            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                            color = Color.White
                        )
                        Text(
                            text = "10km altitude ice crystal filaments with forward halo",
                            style = MaterialTheme.typography.bodySmall,
                            color = Color(0xFF64748B),
                            fontSize = 11.sp
                        )
                    }
                    Switch(
                        checked = enableCirrus,
                        onCheckedChange = onEnableCirrusChange,
                        colors = SwitchDefaults.colors(
                            checkedThumbColor = Color(0xFF38BDF8),
                            checkedTrackColor = Color(0xFF0284C7)
                        )
                    )
                }

                if (enableCirrus) {
                    Spacer(modifier = Modifier.height(8.dp))
                    SkySliderItem(
                        label = "Cirrus Streaks Coverage",
                        value = cirrusCoverage,
                        onValueChange = onCirrusCoverageChange,
                        range = 0.05f..0.85f,
                        displayValue = "${(cirrusCoverage * 100).toInt()}%"
                    )
                }

                Spacer(modifier = Modifier.height(10.dp))
                SkySliderItem(
                    label = "Atmospheric Wind Speed",
                    value = windSpeed,
                    onValueChange = onWindSpeedChange,
                    range = 0.0f..100.0f,
                    displayValue = "${windSpeed.toInt()} km/h"
                )

                SkySliderItem(
                    label = "Wind Drift Azimuth",
                    value = windAzimuth,
                    onValueChange = onWindAzimuthChange,
                    range = 0.0f..360.0f,
                    displayValue = "${windAzimuth.toInt()}°"
                )
            }
        }

        // Section 6: Raymarch Execution & Anti-Aliasing (Bayer Jitter)
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.padding(bottom = 12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column {
                        Text(
                            text = "4x4 Bayer Matrix Ray Jitter",
                            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                            color = Color.White
                        )
                        Text(
                            text = "Interleaved temporal ray dither eliminates woodgrain slicing artifacts",
                            style = MaterialTheme.typography.bodySmall,
                            color = Color(0xFF64748B),
                            fontSize = 11.sp
                        )
                    }
                    Switch(
                        checked = enableBayerJitter,
                        onCheckedChange = onEnableBayerJitterChange,
                        colors = SwitchDefaults.colors(
                            checkedThumbColor = Color(0xFF38BDF8),
                            checkedTrackColor = Color(0xFF0284C7)
                        )
                    )
                }

                Spacer(modifier = Modifier.height(8.dp))
                Surface(
                    color = Color(0xFF0A0D14),
                    shape = RoundedCornerShape(6.dp),
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Column(modifier = Modifier.padding(10.dp)) {
                        Text(
                            text = "PLANETARY ATMOSPHERE SHELL:",
                            fontSize = 10.sp,
                            fontWeight = FontWeight.Bold,
                            color = Color(0xFF38BDF8),
                            fontFamily = FontFamily.Monospace
                        )
                        Text(
                            text = "• Earth Radius: 700,000 m\n• Inner Cloud Altitude: 705,000 m (Base: 5,000 m)\n• Outer Cloud Altitude: 722,000 m (Top: 17,000 m)\n• Color Pipeline: ACES Filmic HDR Tone Mapping",
                            fontSize = 10.sp,
                            color = Color(0xFF94A3B8),
                            fontFamily = FontFamily.Monospace,
                            lineHeight = 16.sp
                        )
                    }
                }
            }
        }
    }
}
