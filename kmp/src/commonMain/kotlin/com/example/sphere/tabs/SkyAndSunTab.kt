package com.example.sphere

import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
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
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp

// -------------------------------------------------------------
// CONTROL TAB: SKY & SUN LIGHTING
// -------------------------------------------------------------
@Composable
fun SkyAndSunTab(
    enableSky: Boolean,
    onToggleSky: (Boolean) -> Unit,
    enableSun: Boolean,
    onToggleSun: (Boolean) -> Unit,
    sunElevation: Float,
    onElevationChange: (Float) -> Unit,
    sunAzimuth: Float,
    onAzimuthChange: (Float) -> Unit,
    turbidity: Float,
    onTurbidityChange: (Float) -> Unit,
    onSelectPreset: (TimeOfDayPreset) -> Unit
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(bottom = 28.dp)
            .testTag("sky_and_sun_tab")
    ) {
        // Toggles Card
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
                            text = "Atmospheric Sky Dome",
                            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                            color = Color.White
                        )
                        Text(
                            text = "Preetham / Hosek-Wilkie Rayleigh scattering",
                            style = MaterialTheme.typography.bodySmall,
                            color = Color(0xFF64748B)
                        )
                    }
                    Switch(
                        checked = enableSky,
                        onCheckedChange = onToggleSky,
                        colors = SwitchDefaults.colors(
                            checkedThumbColor = Color(0xFF38BDF8),
                            checkedTrackColor = Color(0xFF0369A1)
                        )
                    )
                }

                HorizontalDivider(color = Color(0xFF1E293B), modifier = Modifier.padding(vertical = 10.dp))

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column {
                        Text(
                            text = "Sun Disk & Corona Glow",
                            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                            color = Color.White
                        )
                        Text(
                            text = "Direct solar disk, limb darkening & Mie flare",
                            style = MaterialTheme.typography.bodySmall,
                            color = Color(0xFF64748B)
                        )
                    }
                    Switch(
                        checked = enableSun,
                        onCheckedChange = onToggleSun,
                        colors = SwitchDefaults.colors(
                            checkedThumbColor = Color(0xFFFBBF24),
                            checkedTrackColor = Color(0xFFB45309)
                        )
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(16.dp))

        Text(
            text = "Time of Day Solar Presets",
            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
            color = Color.White
        )
        Spacer(modifier = Modifier.height(8.dp))

        Row(
            modifier = Modifier
                .fillMaxWidth()
                .horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            timePresets.forEach { preset ->
                Surface(
                    shape = RoundedCornerShape(10.dp),
                    color = Color(0xFF131722),
                    border = CardDefaults.outlinedCardBorder().copy(
                        brush = Brush.linearGradient(listOf(Color(0xFF1E293B), Color(0xFF334155)))
                    ),
                    modifier = Modifier.clickable { onSelectPreset(preset) }
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 12.dp, vertical = 8.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Icon(
                            imageVector = preset.icon,
                            contentDescription = null,
                            tint = Color(0xFFFBBF24),
                            modifier = Modifier.size(16.dp)
                        )
                        Spacer(modifier = Modifier.width(6.dp))
                        Text(
                            text = preset.name,
                            style = MaterialTheme.typography.labelSmall,
                            color = Color.White,
                            fontWeight = FontWeight.SemiBold
                        )
                    }
                }
            }
        }

        Spacer(modifier = Modifier.height(16.dp))

        // Sun Elevation Slider
        DimensionSlider(
            label = "Sun Elevation (Pitch Angle)",
            value = sunElevation,
            range = -10.0f..90.0f,
            unit = "°",
            accentColor = Color(0xFFFBBF24),
            onValueChange = onElevationChange
        )

        // Sun Azimuth Slider
        DimensionSlider(
            label = "Sun Azimuth (Horizontal Angle)",
            value = sunAzimuth,
            range = 0.0f..360.0f,
            unit = "°",
            accentColor = Color(0xFF38BDF8),
            onValueChange = onAzimuthChange
        )

        // Turbidity / Haze
        DimensionSlider(
            label = "Turbidity (Aerosol Haze Density)",
            value = turbidity,
            range = 1.0f..8.0f,
            unit = "",
            accentColor = Color(0xFFA855F7),
            onValueChange = onTurbidityChange
        )
    }
}
