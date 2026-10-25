package com.example.sphere

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
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
// TAB 2: STARS & COSMOS CONTROLS
// -------------------------------------------------------------
@Composable
fun StarsAndCosmosTab(
    starCount: Int,
    onStarCountChange: (Int) -> Unit,
    twinkleSpeed: Float,
    onTwinkleSpeedChange: (Float) -> Unit,
    magnitudeCutoff: Float,
    onMagnitudeCutoffChange: (Float) -> Unit,
    milkyWayIntensity: Float,
    onMilkyWayChange: (Float) -> Unit
) {
    Column(verticalArrangement = Arrangement.spacedBy(14.dp)) {
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "Hipparcos Procedural Starfield & Scintillation",
                    style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                    color = Color.White
                )
                Text(
                    text = "Celestial sphere distribution with atmospheric turbulence twinkling",
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFF94A3B8)
                )

                HorizontalDivider(color = Color(0xFF1E293B), modifier = Modifier.padding(vertical = 12.dp))

                SkySliderItem(
                    label = "Rendered Star Count",
                    value = starCount.toFloat(),
                    onValueChange = { onStarCountChange(it.toInt()) },
                    range = 256f..3000f,
                    displayValue = "$starCount Stars"
                )

                SkySliderItem(
                    label = "Atmospheric Twinkling Speed",
                    value = twinkleSpeed,
                    onValueChange = onTwinkleSpeedChange,
                    range = 0.0f..4.0f,
                    displayValue = "${(twinkleSpeed * 100).toInt() / 100f}x"
                )

                SkySliderItem(
                    label = "Apparent Magnitude Cutoff",
                    value = magnitudeCutoff,
                    onValueChange = onMagnitudeCutoffChange,
                    range = 2.0f..7.5f,
                    displayValue = "${(magnitudeCutoff * 10).toInt() / 10f} Mag"
                )

                SkySliderItem(
                    label = "Milky Way Band Radiance",
                    value = milkyWayIntensity,
                    onValueChange = onMilkyWayChange,
                    range = 0.0f..1.0f,
                    displayValue = "${(milkyWayIntensity * 100).toInt() / 100f}"
                )
            }
        }
    }
}
