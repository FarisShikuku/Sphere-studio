package com.example.sphere

data class WeatherPreset(
    val name: String,
    val icon: String,
    val rainIntensity: Float,
    val snowIntensity: Float,
    val cloudCoverage: Float,
    val fogDensity: Float,
    val windSpeed: Float,
    val windAzimuth: Float,
    val isStormy: Boolean,
    val cloudType: Int = 2 // 0=Stratus, 1=Stratocumulus, 2=Cumulus, 3=Cumulonimbus
)

val weatherPresetsList = listOf(
    WeatherPreset("Clear", "☀", 0.0f, 0.0f, 0.18f, 0.01f, 6.0f, 45f, false, cloudType = 2),
    WeatherPreset("Overcast", "☁", 0.0f, 0.0f, 0.88f, 0.08f, 12.0f, 60f, false, cloudType = 1),
    WeatherPreset("Rain", "🌧", 0.55f, 0.0f, 0.90f, 0.15f, 18.0f, 225f, false, cloudType = 1),
    WeatherPreset("Downpour", "⛈", 0.95f, 0.0f, 0.98f, 0.25f, 36.0f, 240f, true, cloudType = 3),
    WeatherPreset("Snow", "❄", 0.0f, 0.50f, 0.75f, 0.18f, 10.0f, 90f, false, cloudType = 1),
    WeatherPreset("Blizzard", "🌨", 0.0f, 1.0f, 1.0f, 0.55f, 55.0f, 315f, true, cloudType = 0),
    WeatherPreset("Dense Fog", "🌫", 0.05f, 0.0f, 0.70f, 0.85f, 3.0f, 0f, false, cloudType = 0),
    WeatherPreset("Thunderstorm", "⚡", 0.88f, 0.0f, 0.98f, 0.30f, 44.0f, 270f, true, cloudType = 3)
)
