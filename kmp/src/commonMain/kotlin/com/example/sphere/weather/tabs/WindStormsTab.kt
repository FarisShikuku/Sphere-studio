package com.example.sphere

import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.FlashOn
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
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

import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll

@Composable
fun WindStormsTab(
    windSpeed: Float,
    onWindSpeedChange: (Float) -> Unit,
    windAzimuth: Float,
    onWindAzimuthChange: (Float) -> Unit,
    lightningFrequency: Float,
    onLightningFrequencyChange: (Float) -> Unit,
    isStormActive: Boolean,
    onStormActiveChange: (Boolean) -> Unit,
    onTriggerStrike: () -> Unit
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(bottom = 28.dp)
    ) {
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.padding(bottom = 12.dp)
        ) {
        Column(modifier = Modifier.padding(14.dp)) {
            Text(
                text = "Wind Field & Severe Storms",
                style = MaterialTheme.typography.titleMedium,
                color = Color(0xFFFBBF24),
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(10.dp))

            SliderRow(
                label = "Wind Velocity",
                value = windSpeed,
                onValueChange = onWindSpeedChange,
                valueRange = 0.0f..90.0f,
                displayValue = "${windSpeed.toInt()} km/h"
            )
            SliderRow(
                label = "Wind Direction Angle",
                value = windAzimuth,
                onValueChange = onWindAzimuthChange,
                valueRange = 0.0f..360.0f,
                displayValue = "${windAzimuth.toInt()}°"
            )
            SliderRow(
                label = "Lightning Discharge Rate",
                value = lightningFrequency,
                onValueChange = onLightningFrequencyChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(lightningFrequency * 100).toInt()}%"
            )

            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(vertical = 8.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = "Severe Thunderstorm Mode",
                    style = MaterialTheme.typography.bodyMedium,
                    color = Color.White
                )
                Switch(
                    checked = isStormActive,
                    onCheckedChange = onStormActiveChange,
                    colors = SwitchDefaults.colors(checkedThumbColor = Color(0xFFFBBF24))
                )
            }

            Surface(
                shape = RoundedCornerShape(8.dp),
                color = Color(0xFF1E293B),
                modifier = Modifier
                    .fillMaxWidth()
                    .clickable { onTriggerStrike() }
                    .padding(top = 6.dp)
            ) {
                Row(
                    modifier = Modifier.padding(vertical = 10.dp, horizontal = 14.dp),
                    horizontalArrangement = Arrangement.Center,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Icon(
                        imageVector = Icons.Default.FlashOn,
                        contentDescription = "Trigger Strike",
                        tint = Color(0xFFFBBF24),
                        modifier = Modifier.size(16.dp)
                    )
                    Spacer(modifier = Modifier.width(8.dp))
                    Text(
                        text = "Trigger Lightning Bolt Strike",
                        fontSize = 12.sp,
                        fontWeight = FontWeight.Bold,
                        color = Color.White
                    )
                }
            }
        }
    }
}
}
