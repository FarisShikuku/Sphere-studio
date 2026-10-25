package com.example.sphere

import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.NightsStay
import androidx.compose.material.icons.filled.WbSunny
import androidx.compose.material.icons.outlined.BrightnessMedium
import androidx.compose.material.icons.outlined.WbCloudy
import androidx.compose.material.icons.outlined.WbSunny
import androidx.compose.material.icons.outlined.WbTwilight
import androidx.compose.ui.graphics.vector.ImageVector

data class SkyPreset(
    val name: String,
    val timeHours: Float,
    val elevation: Float,
    val azimuth: Float,
    val turbidity: Float,
    val icon: ImageVector
)

val skyPresetsList = listOf(
    SkyPreset("Dawn", 6.0f, 4.0f, 85.0f, 2.8f, Icons.Outlined.WbTwilight),
    SkyPreset("Morning", 9.0f, 32.0f, 115.0f, 2.2f, Icons.Outlined.WbSunny),
    SkyPreset("High Noon", 12.0f, 68.0f, 180.0f, 2.0f, Icons.Default.WbSunny),
    SkyPreset("Golden Hour", 17.5f, 14.0f, 245.0f, 3.6f, Icons.Outlined.BrightnessMedium),
    SkyPreset("Sunset", 19.2f, 2.0f, 268.0f, 4.8f, Icons.Outlined.WbTwilight),
    SkyPreset("Twilight", 20.5f, -8.0f, 285.0f, 2.5f, Icons.Outlined.WbCloudy),
    SkyPreset("Midnight", 0.0f, -50.0f, 0.0f, 2.0f, Icons.Default.NightsStay)
)

enum class TonemapMode(val label: String, val desc: String) {
    ACES("ACES Filmic", "Academy standard HDR S-curve with natural highlight roll-off"),
    REINHARD("Extended Reinhard", "Luminance-preserving tone mapping with white point clamp"),
    FILMIC_HEJL("Filmic Hejl", "Jim Hejl filmic curve with automatic gamma 2.2 correction"),
    AGX("AgX Natural", "Advanced wide-gamut preservation curve"),
    LINEAR("Linear Raw", "Direct normalized HDR pass-through")
}
