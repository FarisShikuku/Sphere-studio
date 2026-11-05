package com.example.sphere.lighting.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.AtmosphereParameters

@Composable
fun AtmosphereSkyTab(
    params: AtmosphereParameters,
    onParamsChange: (AtmosphereParameters) -> Unit,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        // Banner explaining PlainRenderer Atmosphere
        Surface(
            shape = RoundedCornerShape(10.dp),
            color = MaterialTheme.colorScheme.secondaryContainer.copy(alpha = 0.45f),
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Icon(
                        Icons.Default.WbSunny,
                        contentDescription = null,
                        tint = MaterialTheme.colorScheme.secondary,
                        modifier = Modifier.size(20.dp)
                    )
                    Spacer(Modifier.width(8.dp))
                    Text(
                        "Physically Based Atmosphere & Sky",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.secondary
                    )
                }
                Spacer(Modifier.height(4.dp))
                Text(
                    "Referencing Gaukler/PlainRenderer and Bruneton/Preetham: Simulates Rayleigh scattering (air molecules), Mie aerosol scattering (haze/dust), solar disk luminance, and ozone absorption band for dynamic sun and sky lighting.",
                    style = MaterialTheme.typography.bodySmall,
                    color = MaterialTheme.colorScheme.onSurfaceVariant
                )
            }
        }

        // Live Photometric Readout Box
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(16.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                Text("Computed Photometric Irradiance", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.SemiBold)
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("Direct Sun Illuminance:", style = MaterialTheme.typography.bodySmall)
                    Text(
                        "${params.directSunIlluminanceLux.toInt()} Lux",
                        style = MaterialTheme.typography.bodySmall,
                        fontWeight = FontWeight.Bold,
                        color = Color(0xFFFFA726)
                    )
                }
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("Diffuse Sky Radiance:", style = MaterialTheme.typography.bodySmall)
                    Text(
                        "${params.skyDiffuseIrradianceLux.toInt()} Lux",
                        style = MaterialTheme.typography.bodySmall,
                        fontWeight = FontWeight.Bold,
                        color = Color(0xFF42A5F5)
                    )
                }
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("Optical Air Mass (Kasten-Young):", style = MaterialTheme.typography.bodySmall)
                    val sinElev = kotlin.math.sin(Math.toRadians(kotlin.math.max(1.0, params.sunElevationDeg.toDouble()))).toFloat()
                    val am = (1.0f / kotlin.math.max(0.02f, sinElev))
                    Text(
                        String.format("%.2f", am),
                        style = MaterialTheme.typography.bodySmall,
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary
                    )
                }
            }
        }

        // Sun Elevation Slider
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("Solar Elevation Angle", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.SemiBold)
                    Text(
                        "${params.sunElevationDeg.toInt()}°",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary
                    )
                }
                Slider(
                    value = params.sunElevationDeg,
                    onValueChange = { onParamsChange(params.copy(sunElevationDeg = it)) },
                    valueRange = -5.0f..90.0f,
                    modifier = Modifier.fillMaxWidth().testTag("atmosphere_elevation_slider")
                )
                Text(
                    "Elevation from horizon: 0° is sunset/sunrise, 90° is solar noon directly overhead",
                    style = MaterialTheme.typography.labelSmall,
                    color = MaterialTheme.colorScheme.outline
                )
            }
        }

        // Sun Azimuth Slider
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("Solar Azimuth Angle", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.SemiBold)
                    Text(
                        "${params.sunAzimuthDeg.toInt()}°",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary
                    )
                }
                Slider(
                    value = params.sunAzimuthDeg,
                    onValueChange = { onParamsChange(params.copy(sunAzimuthDeg = it)) },
                    valueRange = 0.0f..360.0f,
                    modifier = Modifier.fillMaxWidth().testTag("atmosphere_azimuth_slider")
                )
                Text(
                    "Compass rotation angle of the Sun around the stage",
                    style = MaterialTheme.typography.labelSmall,
                    color = MaterialTheme.colorScheme.outline
                )
            }
        }

        // Turbidity Slider
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("Atmospheric Turbidity (Haze)", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.SemiBold)
                    Text(
                        String.format("%.2f", params.turbidity),
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.secondary
                    )
                }
                Slider(
                    value = params.turbidity,
                    onValueChange = { onParamsChange(params.copy(turbidity = it)) },
                    valueRange = 1.0f..8.0f,
                    modifier = Modifier.fillMaxWidth().testTag("atmosphere_turbidity_slider")
                )
                Text(
                    "1.0 = Pristine mountain air, 2.5 = Clear sky, 5.0+ = Industrial smog / overcast",
                    style = MaterialTheme.typography.labelSmall,
                    color = MaterialTheme.colorScheme.outline
                )
            }
        }

        // Mie Aerosol Asymmetry (g) Slider
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("Mie Asymmetry Factor (g)", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.SemiBold)
                    Text(
                        String.format("%.2f", params.mieAsymmetry),
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary
                    )
                }
                Slider(
                    value = params.mieAsymmetry,
                    onValueChange = { onParamsChange(params.copy(mieAsymmetry = it)) },
                    valueRange = 0.5f..0.95f,
                    modifier = Modifier.fillMaxWidth().testTag("atmosphere_mie_slider")
                )
                Text(
                    "Forward scattering preference in Cornette-Shanks aerosol phase function",
                    style = MaterialTheme.typography.labelSmall,
                    color = MaterialTheme.colorScheme.outline
                )
            }
        }
    }
}
