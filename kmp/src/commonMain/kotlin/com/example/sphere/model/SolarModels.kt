package com.example.sphere

import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Cloud
import androidx.compose.material.icons.filled.WbSunny
import androidx.compose.material.icons.outlined.WbSunny
import androidx.compose.ui.graphics.vector.ImageVector

// -------------------------------------------------------------
// SOLAR STATE & TIME-OF-DAY PRESETS
// -------------------------------------------------------------
data class TimeOfDayPreset(
    val name: String,
    val elevation: Float,
    val azimuth: Float,
    val turbidity: Float,
    val icon: ImageVector
)

val timePresets = listOf(
    TimeOfDayPreset("Noon", 65.0f, 180.0f, 2.2f, Icons.Default.WbSunny),
    TimeOfDayPreset("Golden Hour", 16.0f, 240.0f, 3.4f, Icons.Outlined.WbSunny),
    TimeOfDayPreset("Sunset", 5.0f, 260.0f, 4.8f, Icons.Outlined.WbSunny),
    TimeOfDayPreset("Sunrise", 8.0f, 85.0f, 3.0f, Icons.Outlined.WbSunny),
    TimeOfDayPreset("Twilight", -4.0f, 280.0f, 2.5f, Icons.Default.Cloud)
)
