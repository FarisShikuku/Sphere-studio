package com.example.sphere

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp

import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll

@Composable
fun PrecipitationTab(
    rainIntensity: Float,
    onRainIntensityChange: (Float) -> Unit,
    dropSpeed: Float,
    onDropSpeedChange: (Float) -> Unit,
    dropletSize: Float,
    onDropletSizeChange: (Float) -> Unit,
    splashDensity: Float,
    onSplashDensityChange: (Float) -> Unit,
    puddleAccumulation: Float,
    onPuddleAccumulationChange: (Float) -> Unit
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
                text = "Precipitation Dynamics",
                style = MaterialTheme.typography.titleMedium,
                color = Color(0xFF38BDF8),
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(10.dp))

            SliderRow(
                label = "Rain Intensity",
                value = rainIntensity,
                onValueChange = onRainIntensityChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(rainIntensity * 100).toInt()}%"
            )
            SliderRow(
                label = "Terminal Drop Velocity",
                value = dropSpeed,
                onValueChange = onDropSpeedChange,
                valueRange = 8.0f..32.0f,
                displayValue = "${dropSpeed.toInt()} m/s"
            )
            SliderRow(
                label = "Droplet Size (Volume)",
                value = dropletSize,
                onValueChange = onDropletSizeChange,
                valueRange = 0.5f..3.0f,
                displayValue = "${(dropletSize * 10).toInt() / 10f} mm"
            )
            SliderRow(
                label = "Ground Splash Impact",
                value = splashDensity,
                onValueChange = onSplashDensityChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(splashDensity * 100).toInt()}%"
            )
            SliderRow(
                label = "Puddle Water Accumulation",
                value = puddleAccumulation,
                onValueChange = onPuddleAccumulationChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(puddleAccumulation * 100).toInt()}%"
            )
        }
    }
}
}
