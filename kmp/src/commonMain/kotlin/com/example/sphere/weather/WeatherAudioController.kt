package com.example.sphere.weather

import android.os.Process
import androidx.compose.runtime.*
import androidx.compose.ui.platform.LocalContext
import com.example.sphere.audio.SphereAudioAssetLoader
import com.example.sphere.audio.SpherePcmAudioTrack
import com.example.sphere.nativebridge.SphereNativeEngine
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive
import kotlinx.coroutines.withContext

class WeatherAudioController internal constructor() {
    var isEnabled by mutableStateOf(false)
        private set
    var isMuted by mutableStateOf(false)
        private set
    var rainLevel by mutableFloatStateOf(0f)
        private set
    var windLevel by mutableFloatStateOf(0f)
        private set
    var thunderLevel by mutableFloatStateOf(0f)
        private set

    internal var activeStreamRequested by mutableStateOf(false)
        private set
    private var lastThunderMs: Long = 0L

    private var curRain = 0f
    private var curSnow = 0f
    private var curWind = 12f
    private var curAzimuth = 0f
    private var curFog = 0f
    private var curStorm = false

    fun updateEnvironment(
        rainIntensity: Float,
        snowIntensity: Float,
        windSpeedKmh: Float,
        windAzimuthDeg: Float,
        fogDensity: Float,
        stormActive: Boolean
    ) {
        curRain = rainIntensity
        curSnow = snowIntensity
        curWind = windSpeedKmh
        curAzimuth = windAzimuthDeg
        curFog = fogDensity
        curStorm = stormActive
        pushToNative()
    }

    fun toggleAudio() {
        setAudioEnabled(!isEnabled)
    }

    fun setAudioEnabled(enable: Boolean) {
        isEnabled = enable
        if (enable) {
            isMuted = false
            activeStreamRequested = true
        } else {
            activeStreamRequested = false
            rainLevel = 0f
            windLevel = 0f
            thunderLevel = 0f
        }
        pushToNative()
    }

    fun toggleMute() {
        isMuted = !isMuted
        pushToNative()
    }

    fun triggerThunder(closeStrike: Boolean = true, autoEnableIfRequested: Boolean = false) {
        if (autoEnableIfRequested && !isEnabled) {
            isEnabled = true
            isMuted = false
            pushToNative()
        }
        if (isEnabled) {
            SphereNativeEngine.triggerWeatherThunder(closeStrike, 1.0f)
            lastThunderMs = System.currentTimeMillis()
            thunderLevel = 1.0f
            activeStreamRequested = true
        }
    }

    private fun pushToNative() {
        SphereNativeEngine.configureWeatherAudio(
            rainIntensity = curRain,
            snowIntensity = curSnow,
            windSpeedKmh = curWind,
            windAzimuthDeg = curAzimuth,
            fogDensity = curFog,
            stormActive = curStorm,
            enabled = isEnabled,
            muted = isMuted
        )
    }

    internal fun applyTelemetry(vals: FloatArray) {
        if (vals.size < 5) return
        rainLevel = vals[1]
        windLevel = vals[2]
        thunderLevel = vals[3]
        val hasAudio = vals[4] > 0.5f
        val recentThunder = (System.currentTimeMillis() - lastThunderMs) < 2800L
        if (!isEnabled && !hasAudio && !recentThunder) {
            activeStreamRequested = false
        }
    }

    internal fun stepJvmFallback() {
        if (!isEnabled) {
            activeStreamRequested = false
            rainLevel = 0f
            windLevel = 0f
            thunderLevel = 0f
            return
        }
        rainLevel = curRain
        windLevel = (curWind / 90f).coerceIn(0f, 1f)
        thunderLevel = if ((System.currentTimeMillis() - lastThunderMs) < 2200L) 0.7f else 0f
    }
}

@Composable
fun rememberWeatherAudioController(
    rainIntensity: Float,
    snowIntensity: Float,
    windSpeedKmh: Float,
    windAzimuthDeg: Float,
    fogDensity: Float,
    stormActive: Boolean
): WeatherAudioController {
    val context = LocalContext.current
    val controller = remember { WeatherAudioController() }

    LaunchedEffect(rainIntensity, snowIntensity, windSpeedKmh, windAzimuthDeg, fogDensity, stormActive) {
        controller.updateEnvironment(
            rainIntensity = rainIntensity,
            snowIntensity = snowIntensity,
            windSpeedKmh = windSpeedKmh,
            windAzimuthDeg = windAzimuthDeg,
            fogDensity = fogDensity,
            stormActive = stormActive
        )
    }

    val isActiveAudio = controller.activeStreamRequested
    LaunchedEffect(isActiveAudio) {
        if (!isActiveAudio) return@LaunchedEffect

        val nativeReady = SphereNativeEngine.isNativeEngineAvailable()
        if (!nativeReady) {
            while (isActive && controller.activeStreamRequested) {
                delay(60L)
                controller.stepJvmFallback()
            }
            return@LaunchedEffect
        }

        withContext(Dispatchers.IO) {
            try {
                Process.setThreadPriority(Process.THREAD_PRIORITY_AUDIO)
            } catch (_: Throwable) {
            }

            SphereAudioAssetLoader.loadFromAssetsIfPresent(context)

            val blockSamples = 1024
            val audioTrack = SpherePcmAudioTrack(
                context = context,
                sampleRate = 44100,
                blockSamples = blockSamples
            )
            val started = audioTrack.start()
            try {
                val pcmBuffer = ShortArray(blockSamples)
                var uiStep = 0
                while (isActive && controller.activeStreamRequested) {
                    val telemetry = SphereNativeEngine.renderWeatherAudioBlock(pcmBuffer, blockSamples)
                    if (started) {
                        audioTrack.writeMonoBlock(pcmBuffer, blockSamples)
                    } else {
                        delay(23L)
                    }
                    uiStep++
                    if (uiStep % 3 == 0) {
                        withContext(Dispatchers.Main) {
                            controller.applyTelemetry(telemetry)
                        }
                    }
                }
            } catch (_: Throwable) {
            } finally {
                audioTrack.stopAndRelease()
            }
        }
    }

    DisposableEffect(Unit) {
        onDispose {
            controller.setAudioEnabled(false)
        }
    }

    return controller
}
