package com.example.sphere

import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Check
import androidx.compose.material.icons.outlined.Grading
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
// TAB 4: TONEMAPPING & COLOR LUT
// -------------------------------------------------------------
@Composable
fun TonemapAndLUTTab(
    selectedMode: TonemapMode,
    onModeSelect: (TonemapMode) -> Unit,
    exposureEv: Float,
    onExposureChange: (Float) -> Unit,
    colorTempKelvin: Float,
    onColorTempChange: (Float) -> Unit,
    contrast: Float,
    onContrastChange: (Float) -> Unit
) {
    Column(verticalArrangement = Arrangement.spacedBy(14.dp)) {
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "HDR Tonemapping Operator",
                    style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                    color = Color.White
                )
                Text(
                    text = "Curves for filmic highlight compression & color grading",
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFF94A3B8)
                )

                HorizontalDivider(color = Color(0xFF1E293B), modifier = Modifier.padding(vertical = 12.dp))

                Column(verticalArrangement = Arrangement.spacedBy(8.dp)) {
                    TonemapMode.values().forEach { mode ->
                        val isSelected = selectedMode == mode
                        Surface(
                            shape = RoundedCornerShape(8.dp),
                            color = if (isSelected) Color(0xFF1E293B) else Color(0xFF0F1219),
                            modifier = Modifier
                                .fillMaxWidth()
                                .clickable { onModeSelect(mode) }
                        ) {
                            Row(
                                modifier = Modifier.padding(10.dp),
                                verticalAlignment = Alignment.CenterVertically
                            ) {
                                Icon(
                                    imageVector = if (isSelected) Icons.Default.Check else Icons.Outlined.Grading,
                                    contentDescription = null,
                                    tint = if (isSelected) Color(0xFF38BDF8) else Color(0xFF64748B),
                                    modifier = Modifier.size(16.dp)
                                )
                                Spacer(modifier = Modifier.width(10.dp))
                                Column {
                                    Text(
                                        text = mode.label,
                                        style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.SemiBold),
                                        color = if (isSelected) Color.White else Color(0xFFCBD5E1)
                                    )
                                    Text(
                                        text = mode.desc,
                                        style = MaterialTheme.typography.labelSmall,
                                        color = Color(0xFF64748B)
                                    )
                                }
                            }
                        }
                    }
                }

                HorizontalDivider(color = Color(0xFF1E293B), modifier = Modifier.padding(vertical = 12.dp))

                SkySliderItem(
                    label = "Exposure Compensation (EV)",
                    value = exposureEv,
                    onValueChange = onExposureChange,
                    range = -3.0f..3.0f,
                    displayValue = "${if (exposureEv > 0) "+" else ""}${(exposureEv * 10).toInt() / 10f} EV"
                )

                SkySliderItem(
                    label = "White Point Temperature (Kelvin)",
                    value = colorTempKelvin,
                    onValueChange = onColorTempChange,
                    range = 2500f..9000f,
                    displayValue = "${colorTempKelvin.toInt()} K"
                )

                SkySliderItem(
                    label = "Contrast Curve",
                    value = contrast,
                    onValueChange = onContrastChange,
                    range = 0.8f..1.4f,
                    displayValue = "${(contrast * 100).toInt() / 100f}x"
                )
            }
        }
    }
}
