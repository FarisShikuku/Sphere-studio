package com.example.sphere

import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Switch
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

@Composable
fun CloudsFogTab(
    cloudType: Int,
    onCloudTypeChange: (Int) -> Unit,
    cloudCoverage: Float,
    onCloudCoverageChange: (Float) -> Unit,
    cloudDensity: Float,
    onCloudDensityChange: (Float) -> Unit,
    silverLiningG: Float,
    onSilverLiningGChange: (Float) -> Unit,
    powderEffect: Float,
    onPowderEffectChange: (Float) -> Unit,
    sunAbsorption: Float,
    onSunAbsorptionChange: (Float) -> Unit,
    detailErosion: Float,
    onDetailErosionChange: (Float) -> Unit,
    cloudBaseAltitude: Float,
    onCloudBaseAltitudeChange: (Float) -> Unit,
    raymarchSteps: Int,
    onRaymarchStepsChange: (Int) -> Unit,
    enableGodRays: Boolean,
    onEnableGodRaysChange: (Boolean) -> Unit,
    godRaysIntensity: Float,
    onGodRaysIntensityChange: (Float) -> Unit,
    enableCirrus: Boolean,
    onEnableCirrusChange: (Boolean) -> Unit,
    cirrusCoverage: Float,
    onCirrusCoverageChange: (Float) -> Unit,
    billowDepth: Float,
    onBillowDepthChange: (Float) -> Unit,
    sunBacklight: Float,
    onSunBacklightChange: (Float) -> Unit,
    fogDensity: Float,
    onFogDensityChange: (Float) -> Unit,
    groundMist: Float,
    onGroundMistChange: (Float) -> Unit,
    multiScatteringFactor: Float = 0.55f,
    onMultiScatteringFactorChange: (Float) -> Unit = {},
    enableBayerJitter: Boolean = true,
    onEnableBayerJitterChange: (Boolean) -> Unit = {}
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(bottom = 28.dp)
    ) {
        // Card 1: Cloud Species Selection (AmanSachan1/Meteoros & Decima Nubis Types)
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.padding(bottom = 12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "Cloud Species & Height Profiles (Meteoros)",
                    style = MaterialTheme.typography.titleMedium,
                    color = Color(0xFFA78BFA),
                    fontWeight = FontWeight.Bold
                )
                Text(
                    text = "Volumetric height density profiles from AmanSachan1/Meteoros Decima model",
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFF94A3B8)
                )
                Spacer(modifier = Modifier.height(10.dp))

                val types = listOf(
                    Triple(0, "Stratus", "Low flat sheet veil"),
                    Triple(1, "Stratocumulus", "Rolling wave blankets"),
                    Triple(2, "Cumulus", "Towering cauliflower heaps"),
                    Triple(3, "Cumulonimbus", "Monolithic anvil storm")
                )

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(6.dp)
                ) {
                    types.forEach { (typeId, title, desc) ->
                        val isSelected = cloudType == typeId
                        Surface(
                            shape = RoundedCornerShape(8.dp),
                            color = if (isSelected) Color(0xFF7C3AED).copy(alpha = 0.45f) else Color(0xFF1E293B),
                            border = CardDefaults.outlinedCardBorder().copy(
                                brush = Brush.linearGradient(
                                    if (isSelected) listOf(Color(0xFFA78BFA), Color(0xFF7C3AED))
                                    else listOf(Color(0xFF334155), Color(0xFF1E293B))
                                )
                            ),
                            modifier = Modifier
                                .weight(1f)
                                .clickable { onCloudTypeChange(typeId) }
                                .testTag("cloud_type_button_$typeId")
                        ) {
                            Column(modifier = Modifier.padding(6.dp)) {
                                Text(
                                    text = title,
                                    fontSize = 10.sp,
                                    fontWeight = FontWeight.Bold,
                                    color = if (isSelected) Color.White else Color(0xFFCBD5E1),
                                    maxLines = 1
                                )
                                Spacer(modifier = Modifier.height(2.dp))
                                Text(
                                    text = desc,
                                    fontSize = 8.5.sp,
                                    color = Color(0xFF94A3B8),
                                    maxLines = 2,
                                    lineHeight = 10.sp
                                )
                            }
                        }
                    }
                }
            }
        }

        // Card 2: Cinematic Atmospheric Volumetrics (Meteoros God-Rays & Anti-Aliasing)
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.padding(bottom = 12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "Meteoros Volumetrics & Crepuscular Rays",
                    style = MaterialTheme.typography.titleMedium,
                    color = Color(0xFFFBBF24),
                    fontWeight = FontWeight.Bold
                )
                Spacer(modifier = Modifier.height(8.dp))

                Row(
                    modifier = Modifier.fillMaxWidth().padding(vertical = 4.dp),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column {
                        Text(text = "Crepuscular God Rays (Light Shafts)", style = MaterialTheme.typography.bodyMedium, color = Color.White)
                        Text(text = "Meteoros radial screen-space sunbeam marching", fontSize = 10.sp, color = Color(0xFF94A3B8))
                    }
                    Switch(checked = enableGodRays, onCheckedChange = onEnableGodRaysChange)
                }

                if (enableGodRays) {
                    SliderRow(
                        label = "God Ray Shaft Beam Intensity",
                        value = godRaysIntensity,
                        onValueChange = onGodRaysIntensityChange,
                        valueRange = 0.0f..1.0f,
                        displayValue = "${(godRaysIntensity * 100).toInt()}%"
                    )
                }

                Row(
                    modifier = Modifier.fillMaxWidth().padding(vertical = 4.dp),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column {
                        Text(text = "4x4 Bayer Interleaved Jitter", style = MaterialTheme.typography.bodyMedium, color = Color.White)
                        Text(text = "Eliminates ray stepping & woodgrain slicing artifacts", fontSize = 10.sp, color = Color(0xFF94A3B8))
                    }
                    Switch(checked = enableBayerJitter, onCheckedChange = onEnableBayerJitterChange)
                }

                Row(
                    modifier = Modifier.fillMaxWidth().padding(vertical = 4.dp),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Text(text = "High-Altitude Cirrus Veil (10km)", style = MaterialTheme.typography.bodyMedium, color = Color.White)
                    Switch(checked = enableCirrus, onCheckedChange = onEnableCirrusChange)
                }

                if (enableCirrus) {
                    SliderRow(
                        label = "Cirrus Ice Plume Density",
                        value = cirrusCoverage,
                        onValueChange = onCirrusCoverageChange,
                        valueRange = 0.0f..1.0f,
                        displayValue = "${(cirrusCoverage * 100).toInt()}%"
                    )
                }

                SliderRow(
                    label = "Solar Disc Backlight & Corona",
                    value = sunBacklight,
                    onValueChange = onSunBacklightChange,
                    valueRange = 0.2f..1.2f,
                    displayValue = "${(sunBacklight * 100).toInt()}%"
                )
            }
        }

        // Card 3: Meteoros Optical Radiative Transfer & Multi-Scattering
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.padding(bottom = 12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "Meteoros Photometric Scattering & Optics",
                    style = MaterialTheme.typography.titleMedium,
                    color = Color(0xFF38BDF8),
                    fontWeight = FontWeight.Bold
                )
                Spacer(modifier = Modifier.height(8.dp))

                SliderRow(
                    label = "Weather Map Fractional Coverage",
                    value = cloudCoverage,
                    onValueChange = onCloudCoverageChange,
                    valueRange = 0.0f..1.0f,
                    displayValue = "${(cloudCoverage * 100).toInt()}%"
                )
                SliderRow(
                    label = "Volumetric Optical Density",
                    value = cloudDensity,
                    onValueChange = onCloudDensityChange,
                    valueRange = 0.1f..1.0f,
                    displayValue = "${(cloudDensity * 100).toInt()}%"
                )
                SliderRow(
                    label = "Dual Beer-Lambert Multi-Scattering",
                    value = multiScatteringFactor,
                    onValueChange = onMultiScatteringFactorChange,
                    valueRange = 0.0f..1.0f,
                    displayValue = "${(multiScatteringFactor * 100).toInt()}%"
                )
                SliderRow(
                    label = "Silver Lining (Henyey-Greenstein g1)",
                    value = silverLiningG,
                    onValueChange = onSilverLiningGChange,
                    valueRange = 0.40f..0.98f,
                    displayValue = "${(silverLiningG * 100).toInt() / 100f}"
                )
                SliderRow(
                    label = "Beer-Powder Sugar Approximation",
                    value = powderEffect,
                    onValueChange = onPowderEffectChange,
                    valueRange = 0.0f..1.0f,
                    displayValue = "${(powderEffect * 100).toInt()}%"
                )
                SliderRow(
                    label = "Sun Cone March Extinction",
                    value = sunAbsorption,
                    onValueChange = onSunAbsorptionChange,
                    valueRange = 0.20f..1.50f,
                    displayValue = "${(sunAbsorption * 100).toInt() / 100f}"
                )
                SliderRow(
                    label = "High-Freq 3D Worley Erosion",
                    value = detailErosion,
                    onValueChange = onDetailErosionChange,
                    valueRange = 0.0f..1.0f,
                    displayValue = "${(detailErosion * 100).toInt()}%"
                )
                SliderRow(
                    label = "Planetary Shell Base Altitude",
                    value = cloudBaseAltitude,
                    onValueChange = onCloudBaseAltitudeChange,
                    valueRange = 3000f..8000f,
                    displayValue = "${cloudBaseAltitude.toInt()} m"
                )
                SliderRow(
                    label = "Raymarch Steps Quality",
                    value = raymarchSteps.toFloat(),
                    onValueChange = { onRaymarchStepsChange(it.toInt()) },
                    valueRange = 16f..64f,
                    displayValue = "$raymarchSteps steps"
                )
                SliderRow(
                    label = "Ground Froxel Fog Density",
                    value = fogDensity,
                    onValueChange = onFogDensityChange,
                    valueRange = 0.0f..1.0f,
                    displayValue = "${(fogDensity * 100).toInt()}%"
                )
                SliderRow(
                    label = "Ground Mist Layer",
                    value = groundMist,
                    onValueChange = onGroundMistChange,
                    valueRange = 0.0f..1.0f,
                    displayValue = "${(groundMist * 100).toInt()}%"
                )
            }
        }

        // Card 4: AmanSachan1/Meteoros Engine Architecture Telemetry Card
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.padding(bottom = 12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "AmanSachan1/Meteoros Pipeline Telemetry",
                    style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                    color = Color(0xFF4ADE80)
                )
                Spacer(modifier = Modifier.height(6.dp))

                val stats = listOf(
                    "Cloud Engine" to "AmanSachan1/Meteoros (Vulkan 1.3 Compute)",
                    "Scattering Model" to "Dual Beer-Lambert Multi-Scattering + Powder Sugar",
                    "Light Scattering" to "Dual-Lobe HG (Forward g1=0.82, Back g2=-0.22)",
                    "Crepuscular God Rays" to if (enableGodRays) "Active (cloud_godrays.comp radial march)" else "Disabled",
                    "Slicing Mitigation" to if (enableBayerJitter) "4x4 Bayer Interleaved Jitter Active" else "Standard Stepping",
                    "Color Space" to "32-bit HDR with ACES Filmic Tone Mapping",
                    "Atmosphere Bounds" to "R_inner = 705,000m, R_outer = 722,000m",
                    "Species Profiles" to "Stratus, Stratocumulus, Cumulus, Cumulonimbus"
                )

                stats.forEach { (label, value) ->
                    Row(
                        modifier = Modifier.fillMaxWidth().padding(vertical = 2.dp),
                        horizontalArrangement = Arrangement.SpaceBetween
                    ) {
                        Text(text = label, fontSize = 10.sp, color = Color(0xFF94A3B8))
                        Text(text = value, fontSize = 10.sp, fontWeight = FontWeight.Bold, color = Color(0xFFE2E8F0))
                    }
                }
            }
        }
    }
}
