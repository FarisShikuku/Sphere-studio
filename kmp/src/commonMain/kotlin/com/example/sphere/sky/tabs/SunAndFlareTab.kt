package com.example.sphere

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp

// -------------------------------------------------------------
// TAB 1: SUN & FLARE CONTROLS
// -------------------------------------------------------------
@Composable
fun SunAndFlareTab(
    sunElevation: Float,
    onElevationChange: (Float) -> Unit,
    sunAzimuth: Float,
    onAzimuthChange: (Float) -> Unit,
    sunIntensityLux: Float,
    onIntensityChange: (Float) -> Unit,
    angularRadius: Float,
    onAngularRadiusChange: (Float) -> Unit,
    limbDarkening: Float,
    onLimbDarkeningChange: (Float) -> Unit,
    coronaFlare: Float,
    onCoronaFlareChange: (Float) -> Unit
) {
    Column(verticalArrangement = Arrangement.spacedBy(14.dp)) {
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "Solar Orb & Corona Optics",
                    style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                    color = Color.White
                )
                Text(
                    text = "Analytic solar disk with Eddy & Pierce limb darkening and Mie flare",
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFF94A3B8)
                )

                HorizontalDivider(color = Color(0xFF1E293B), modifier = Modifier.padding(vertical = 12.dp))

                SkySliderItem(
                    label = "Solar Elevation (Altitude)",
                    value = sunElevation,
                    onValueChange = onElevationChange,
                    range = -15f..90f,
                    displayValue = "${(sunElevation * 10).toInt() / 10f}°"
                )

                SkySliderItem(
                    label = "Solar Azimuth",
                    value = sunAzimuth,
                    onValueChange = onAzimuthChange,
                    range = 0f..360f,
                    displayValue = "${(sunAzimuth * 10).toInt() / 10f}°"
                )

                SkySliderItem(
                    label = "Direct Solar Flux (LUX)",
                    value = sunIntensityLux,
                    onValueChange = onIntensityChange,
                    range = 5f..50f,
                    displayValue = "${sunIntensityLux.toInt()} kLux"
                )

                SkySliderItem(
                    label = "Angular Diameter",
                    value = angularRadius,
                    onValueChange = onAngularRadiusChange,
                    range = 0.02f..0.08f,
                    displayValue = "${(angularRadius * 1000).toInt() / 1000f} rad"
                )

                SkySliderItem(
                    label = "Limb Darkening Factor",
                    value = limbDarkening,
                    onValueChange = onLimbDarkeningChange,
                    range = 0.0f..1.0f,
                    displayValue = "${(limbDarkening * 100).toInt() / 100f}"
                )

                SkySliderItem(
                    label = "Mie Corona Flare Intensity",
                    value = coronaFlare,
                    onValueChange = onCoronaFlareChange,
                    range = 0.0f..1.0f,
                    displayValue = "${(coronaFlare * 100).toInt() / 100f}"
                )
            }
        }
    }
}
