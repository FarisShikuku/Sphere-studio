package com.example.sphere.lighting.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Brightness5
import androidx.compose.material.icons.filled.FlashlightOn
import androidx.compose.material.icons.filled.Lightbulb
import androidx.compose.material.icons.filled.Waves
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import com.example.sphere.model.LightSourceParameters
import com.example.sphere.model.LightSourceType
import kotlin.math.abs
import kotlin.math.max
import kotlin.math.min
import kotlin.math.roundToInt

@Composable
fun LightOpticsControlTab(
    params: LightSourceParameters,
    onParamsChange: (LightSourceParameters) -> Unit,
    modifier: Modifier = Modifier
) {
    val scrollState = rememberScrollState()

    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(scrollState)
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        // Physical Intensity (PlainRenderer Punctual/Area formula)
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                        Icon(Icons.Default.Brightness5, contentDescription = null, tint = MaterialTheme.colorScheme.primary)
                        Text(
                            text = "Luminous Flux / Intensity",
                            style = MaterialTheme.typography.titleSmall,
                            fontWeight = FontWeight.Bold
                        )
                    }
                    Text(
                        text = "${params.intensityLumens.roundToInt()} lm",
                        style = MaterialTheme.typography.bodyMedium,
                        fontWeight = FontWeight.SemiBold,
                        color = MaterialTheme.colorScheme.primary
                    )
                }

                Slider(
                    value = params.intensityLumens,
                    onValueChange = { onParamsChange(params.copy(intensityLumens = it)) },
                    valueRange = 50f..10000f,
                    modifier = Modifier.fillMaxWidth()
                )

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("50 lm (Dim candle)", style = MaterialTheme.typography.labelSmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
                    Text("1200 lm (Standard bulb)", style = MaterialTheme.typography.labelSmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
                    Text("10000 lm (Arc lamp)", style = MaterialTheme.typography.labelSmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
                }
            }
        }

        // Color Temperature (Kelvin) & Custom Hue
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(10.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                        Icon(Icons.Default.Lightbulb, contentDescription = null, tint = MaterialTheme.colorScheme.primary)
                        Text(
                            text = "Blackbody Color (Kelvin)",
                            style = MaterialTheme.typography.titleSmall,
                            fontWeight = FontWeight.Bold
                        )
                    }
                    Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                        Box(
                            modifier = Modifier
                                .size(24.dp)
                                .clip(CircleShape)
                                .background(params.effectiveColor)
                                .border(1.dp, MaterialTheme.colorScheme.outline, CircleShape)
                        )
                        Text(
                            text = "${params.colorKelvin.roundToInt()} K",
                            style = MaterialTheme.typography.bodyMedium,
                            fontWeight = FontWeight.SemiBold
                        )
                    }
                }

                Slider(
                    value = params.colorKelvin,
                    onValueChange = { onParamsChange(params.withColorKelvin(it)) },
                    valueRange = 1800f..9500f,
                    modifier = Modifier.fillMaxWidth()
                )

                // Kelvin presets
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    FilterChip(
                        selected = abs(params.colorKelvin - 2200f) < 100f,
                        onClick = { onParamsChange(params.withColorKelvin(2200f)) },
                        label = { Text("2200K Candle", style = MaterialTheme.typography.labelSmall) }
                    )
                    FilterChip(
                        selected = abs(params.colorKelvin - 3200f) < 100f,
                        onClick = { onParamsChange(params.withColorKelvin(3200f)) },
                        label = { Text("3200K Tungsten", style = MaterialTheme.typography.labelSmall) }
                    )
                    FilterChip(
                        selected = abs(params.colorKelvin - 5500f) < 100f,
                        onClick = { onParamsChange(params.withColorKelvin(5500f)) },
                        label = { Text("5500K Daylight", style = MaterialTheme.typography.labelSmall) }
                    )
                    FilterChip(
                        selected = abs(params.colorKelvin - 7500f) < 100f,
                        onClick = { onParamsChange(params.withColorKelvin(7500f)) },
                        label = { Text("7500K Overcast", style = MaterialTheme.typography.labelSmall) }
                    )
                }
            }
        }

        // Distance Range & PlainRenderer Attenuation Radius
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Text(
                        text = "Attenuation Range (Smooth Windowing)",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.Bold
                    )
                    Text(
                        text = "${(params.rangeMeters * 10f).roundToInt() / 10f} m",
                        style = MaterialTheme.typography.bodyMedium,
                        fontWeight = FontWeight.SemiBold,
                        color = MaterialTheme.colorScheme.primary
                    )
                }
                Slider(
                    value = params.rangeMeters,
                    onValueChange = { onParamsChange(params.withRange(it)) },
                    valueRange = 2f..30f,
                    modifier = Modifier.fillMaxWidth()
                )
                Text(
                    text = "Formula: (1 / max(d², 0.01)) * clamp(1 - (d / range)⁴, 0, 1)²",
                    style = MaterialTheme.typography.labelSmall,
                    fontFamily = FontFamily.Monospace,
                    color = MaterialTheme.colorScheme.onSurfaceVariant
                )
            }
        }

        // Spot Light Cone Optics (when type is SPOT_FLASHLIGHT)
        if (params.type == LightSourceType.SPOT_FLASHLIGHT) {
            Card(
                shape = RoundedCornerShape(12.dp),
                colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.primaryContainer.copy(alpha = 0.3f))
            ) {
                Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(10.dp)) {
                    Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                        Icon(Icons.Default.FlashlightOn, contentDescription = null, tint = MaterialTheme.colorScheme.primary)
                        Text(
                            text = "Flashlight Beam Angles (Inner / Outer Cone)",
                            style = MaterialTheme.typography.titleSmall,
                            fontWeight = FontWeight.Bold
                        )
                    }

                    // Inner Angle
                    Column(verticalArrangement = Arrangement.spacedBy(4.dp)) {
                        Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                            Text("Inner Cone (Hotspot):", style = MaterialTheme.typography.bodySmall)
                            Text("${params.spotInnerDeg.roundToInt()}°", style = MaterialTheme.typography.bodySmall, fontWeight = FontWeight.Bold)
                        }
                        Slider(
                            value = params.spotInnerDeg,
                            onValueChange = {
                                val inner = it
                                val outer = max(inner + 2f, params.spotOuterDeg)
                                onParamsChange(params.withSpotAngles(inner, outer))
                            },
                            valueRange = 5f..60f,
                            modifier = Modifier.fillMaxWidth()
                        )
                    }

                    // Outer Angle
                    Column(verticalArrangement = Arrangement.spacedBy(4.dp)) {
                        Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                            Text("Outer Cone (Penumbra):", style = MaterialTheme.typography.bodySmall)
                            Text("${params.spotOuterDeg.roundToInt()}°", style = MaterialTheme.typography.bodySmall, fontWeight = FontWeight.Bold)
                        }
                        Slider(
                            value = params.spotOuterDeg,
                            onValueChange = {
                                val outer = it
                                val inner = min(outer - 2f, params.spotInnerDeg)
                                onParamsChange(params.withSpotAngles(inner, outer))
                            },
                            valueRange = 8f..85f,
                            modifier = Modifier.fillMaxWidth()
                        )
                    }
                }
            }
        }

        // Area Light Dimensions (when type is AREA_SOFTBOX)
        if (params.type == LightSourceType.AREA_SOFTBOX) {
            Card(
                shape = RoundedCornerShape(12.dp),
                colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.tertiaryContainer.copy(alpha = 0.3f))
            ) {
                Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(10.dp)) {
                    Text(
                        text = "Area Light Dimensions (Rectangular Softbox)",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.Bold
                    )

                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(16.dp)) {
                        Column(modifier = Modifier.weight(1f)) {
                            Text("Emitter Width: ${(params.areaWidthMeters * 10f).roundToInt() / 10f} m", style = MaterialTheme.typography.bodySmall)
                            Slider(
                                value = params.areaWidthMeters,
                                onValueChange = { onParamsChange(params.withAreaDimensions(it, params.areaHeightMeters)) },
                                valueRange = 0.2f..4.0f
                            )
                        }
                        Column(modifier = Modifier.weight(1f)) {
                            Text("Emitter Height: ${(params.areaHeightMeters * 10f).roundToInt() / 10f} m", style = MaterialTheme.typography.bodySmall)
                            Slider(
                                value = params.areaHeightMeters,
                                onValueChange = { onParamsChange(params.withAreaDimensions(params.areaWidthMeters, it)) },
                                valueRange = 0.2f..4.0f
                            )
                        }
                    }
                }
            }
        }

        // Atmosphere / Volumetric Beam Haze
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                        Icon(Icons.Default.Waves, contentDescription = null, tint = MaterialTheme.colorScheme.primary)
                        Text(
                            text = "Volumetric Beam Haze",
                            style = MaterialTheme.typography.titleSmall,
                            fontWeight = FontWeight.Bold
                        )
                    }
                    Switch(
                        checked = params.volumetricHaze,
                        onCheckedChange = { onParamsChange(params.withVolumetric(it)) }
                    )
                }

                if (params.volumetricHaze) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween
                    ) {
                        Text("Scattering density:", style = MaterialTheme.typography.bodySmall)
                        Text("${(params.volumetricIntensity * 100f).roundToInt()}%", style = MaterialTheme.typography.bodySmall, fontWeight = FontWeight.Bold)
                    }
                    Slider(
                        value = params.volumetricIntensity,
                        onValueChange = { onParamsChange(params.withVolumetric(true, it)) },
                        valueRange = 0.05f..1.0f,
                        modifier = Modifier.fillMaxWidth()
                    )
                }
            }
        }
    }
}
