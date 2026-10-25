package com.example.sphere

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.DarkMode
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp

// -------------------------------------------------------------
// TAB 3: MOON & LUNAR PHASES
// -------------------------------------------------------------
@Composable
fun MoonAndPhasesTab(
    moonPhase: Float,
    onMoonPhaseChange: (Float) -> Unit,
    moonElevation: Float,
    onMoonElevationChange: (Float) -> Unit,
    moonAzimuth: Float,
    onMoonAzimuthChange: (Float) -> Unit,
    earthshine: Float,
    onEarthshineChange: (Float) -> Unit,
    moonAlbedo: Float,
    onMoonAlbedoChange: (Float) -> Unit
) {
    val phaseName = when {
        moonPhase in 0.0f..0.06f || moonPhase in 0.94f..1.0f -> "New Moon (0%)"
        moonPhase in 0.06f..0.22f -> "Waxing Crescent (${(moonPhase * 200f).toInt()}%)"
        moonPhase in 0.22f..0.28f -> "First Quarter (50%)"
        moonPhase in 0.28f..0.47f -> "Waxing Gibbous (${((moonPhase - 0.25f) * 200f + 50f).toInt()}%)"
        moonPhase in 0.47f..0.53f -> "Full Moon (100%)"
        moonPhase in 0.53f..0.72f -> "Waning Gibbous (${(100f - (moonPhase - 0.5f) * 200f).toInt()}%)"
        moonPhase in 0.72f..0.78f -> "Third Quarter (50%)"
        else -> "Waning Crescent (${((1.0f - moonPhase) * 200f).toInt()}%)"
    }

    Column(verticalArrangement = Arrangement.spacedBy(14.dp)) {
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column {
                        Text(
                            text = "Lunar Phase Geometry",
                            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                            color = Color.White
                        )
                        Text(
                            text = phaseName,
                            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.SemiBold),
                            color = Color(0xFF38BDF8)
                        )
                    }
                    Surface(
                        shape = CircleShape,
                        color = Color(0xFF1E293B),
                        modifier = Modifier.size(36.dp)
                    ) {
                        Box(contentAlignment = Alignment.Center) {
                            Icon(
                                imageVector = Icons.Default.DarkMode,
                                contentDescription = null,
                                tint = Color(0xFF94A3B8),
                                modifier = Modifier.size(20.dp)
                            )
                        }
                    }
                }

                HorizontalDivider(color = Color(0xFF1E293B), modifier = Modifier.padding(vertical = 12.dp))

                SkySliderItem(
                    label = "Orbital Phase (New -> Full -> New)",
                    value = moonPhase,
                    onValueChange = onMoonPhaseChange,
                    range = 0.0f..1.0f,
                    displayValue = "${(moonPhase * 100).toInt() / 100f}"
                )

                SkySliderItem(
                    label = "Moon Elevation",
                    value = moonElevation,
                    onValueChange = onMoonElevationChange,
                    range = -30f..90f,
                    displayValue = "${(moonElevation * 10).toInt() / 10f}°"
                )

                SkySliderItem(
                    label = "Moon Azimuth",
                    value = moonAzimuth,
                    onValueChange = onMoonAzimuthChange,
                    range = 0f..360f,
                    displayValue = "${(moonAzimuth * 10).toInt() / 10f}°"
                )

                SkySliderItem(
                    label = "Earthshine Intensity",
                    value = earthshine,
                    onValueChange = onEarthshineChange,
                    range = 0.005f..0.08f,
                    displayValue = "${(earthshine * 1000).toInt() / 1000f}"
                )

                SkySliderItem(
                    label = "Geometric Albedo",
                    value = moonAlbedo,
                    onValueChange = onMoonAlbedoChange,
                    range = 0.05f..0.30f,
                    displayValue = "${(moonAlbedo * 100).toInt() / 100f}"
                )
            }
        }
    }
}
