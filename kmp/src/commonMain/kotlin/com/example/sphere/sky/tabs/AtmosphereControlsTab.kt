package com.example.sphere

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Switch
import androidx.compose.material3.SwitchDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

// -------------------------------------------------------------
// TAB 0: ATMOSPHERE CONTROLS (PREETHAM / HOSEK-WILKIE)
// -------------------------------------------------------------
@Composable
fun AtmosphereControlsTab(
    turbidity: Float,
    onTurbidityChange: (Float) -> Unit,
    rayleighScale: Float,
    onRayleighChange: (Float) -> Unit,
    mieScale: Float,
    onMieChange: (Float) -> Unit,
    mieAsymmetryG: Float,
    onMieGChange: (Float) -> Unit,
    groundAlbedo: Float,
    onAlbedoChange: (Float) -> Unit,
    ozoneAbsorption: Boolean,
    onOzoneToggle: (Boolean) -> Unit
) {
    Column(verticalArrangement = Arrangement.spacedBy(14.dp)) {
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "Preetham / Hosek-Wilkie Scattering Model",
                    style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                    color = Color.White
                )
                Text(
                    text = "Physical Rayleigh air molecules and Mie aerosol haze distribution",
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFF94A3B8)
                )

                HorizontalDivider(color = Color(0xFF1E293B), modifier = Modifier.padding(vertical = 12.dp))

                SkySliderItem(
                    label = "Aerosol Turbidity (Haze)",
                    value = turbidity,
                    onValueChange = onTurbidityChange,
                    range = 1.0f..10.0f,
                    displayValue = "${(turbidity * 100).toInt() / 100f} T"
                )

                SkySliderItem(
                    label = "Rayleigh Scattering Scale",
                    value = rayleighScale,
                    onValueChange = onRayleighChange,
                    range = 0.2f..3.0f,
                    displayValue = "${(rayleighScale * 100).toInt() / 100f}x"
                )

                SkySliderItem(
                    label = "Mie Scattering Coefficient",
                    value = mieScale,
                    onValueChange = onMieChange,
                    range = 0.2f..3.0f,
                    displayValue = "${(mieScale * 100).toInt() / 100f}x"
                )

                SkySliderItem(
                    label = "Henyey-Greenstein Asymmetry (g)",
                    value = mieAsymmetryG,
                    onValueChange = onMieGChange,
                    range = 0.5f..0.95f,
                    displayValue = "${(mieAsymmetryG * 100).toInt() / 100f}"
                )

                SkySliderItem(
                    label = "Ground Surface Albedo",
                    value = groundAlbedo,
                    onValueChange = onAlbedoChange,
                    range = 0.0f..0.5f,
                    displayValue = "${(groundAlbedo * 100).toInt() / 100f}"
                )

                Row(
                    modifier = Modifier.fillMaxWidth().padding(top = 8.dp),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column {
                        Text("Ozone Layer Absorption (Chappuis Band)", color = Color.White, fontSize = 13.sp)
                        Text("Simulates twilight blue shift at sunset", color = Color(0xFF64748B), fontSize = 11.sp)
                    }
                    Switch(
                        checked = ozoneAbsorption,
                        onCheckedChange = onOzoneToggle,
                        colors = SwitchDefaults.colors(
                            checkedThumbColor = Color(0xFF38BDF8),
                            checkedTrackColor = Color(0xFF0369A1)
                        )
                    )
                }
            }
        }
    }
}
