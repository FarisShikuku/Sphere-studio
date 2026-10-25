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
fun SnowFrostTab(
    snowIntensity: Float,
    onSnowIntensityChange: (Float) -> Unit,
    flakeFlutter: Float,
    onFlakeFlutterChange: (Float) -> Unit,
    frostGlaze: Float,
    onFrostGlazeChange: (Float) -> Unit,
    groundSnowCover: Float,
    onGroundSnowCoverChange: (Float) -> Unit
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
                text = "Snow & Cryosphere",
                style = MaterialTheme.typography.titleMedium,
                color = Color(0xFF93C5FD),
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(10.dp))

            SliderRow(
                label = "Snowfall Precipitation",
                value = snowIntensity,
                onValueChange = onSnowIntensityChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(snowIntensity * 100).toInt()}%"
            )
            SliderRow(
                label = "Flake Flutter Turbulence",
                value = flakeFlutter,
                onValueChange = onFlakeFlutterChange,
                valueRange = 0.1f..1.0f,
                displayValue = "${(flakeFlutter * 100).toInt()}%"
            )
            SliderRow(
                label = "Surface Frost Glaze",
                value = frostGlaze,
                onValueChange = onFrostGlazeChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(frostGlaze * 100).toInt()}%"
            )
            SliderRow(
                label = "Ground Snowpack Accumulation",
                value = groundSnowCover,
                onValueChange = onGroundSnowCoverChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(groundSnowCover * 100).toInt()}%"
            )
        }
    }
}
}
