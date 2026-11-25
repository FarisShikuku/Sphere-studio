package com.example.sphere.vehicles

import android.os.Process
import androidx.compose.runtime.*
import androidx.compose.ui.platform.LocalContext
import com.example.sphere.audio.SphereAudioAssetLoader
import com.example.sphere.audio.SpherePcmAudioTrack
import com.example.sphere.model.VehicleItem
import com.example.sphere.nativebridge.SphereNativeEngine
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive
import kotlinx.coroutines.withContext
import kotlin.math.abs
import kotlin.math.floor
import kotlin.math.max
import kotlin.math.min

object VehicleOneShotIds {
    const val AIR_BRAKE_HISS = 0
    const val TRUCK_HORN_HEAVY = 1
    const val BUS_DOOR_PNEUMATIC = 2
    const val MATATU_HORN = 3
    const val GEAR_SHIFT_CLUNK = 4
}

@Immutable
data class VehicleStreamingStats(
    val gridCellX: Int = 0,
    val gridCellZ: Int = 0,
    val loadedRingCells: Int = 9,
    val heroMeshletCount: Int = 0,
    val maxTrisPerMeshlet: Int = 126,
    val hlodProxyBatches: Int = 6,
    val hlodCulledTris: Int = 0
)

class VehicleAudioController internal constructor() {
    var ignitionState by mutableIntStateOf(0) // 0 = OFF, 1 = STARTING, 2 = RUNNING
        private set
    var rpm by mutableFloatStateOf(0f)
        private set
    var speedKmh by mutableFloatStateOf(0f)
        private set
    var gear by mutableIntStateOf(0)
        private set
    var turboBoostBar by mutableFloatStateOf(0f)
        private set
    var distanceM by mutableFloatStateOf(0f)
        private set

    var throttlePressed by mutableStateOf(false)
        private set
    var brakePressed by mutableStateOf(false)
        private set
    var cruiseDrive by mutableStateOf(false)
        private set
    var isMuted by mutableStateOf(false)
        private set

    internal var activeStreamRequested by mutableStateOf(false)
        private set
    private var lastOneShotMs: Long = 0L
    private var maxRpmHint: Float = 2400f
    private var idleRpmHint: Float = 650f

    val isEngineOn: Boolean
        get() = ignitionState != 0

    fun configureVehicle(vehicle: VehicleItem, combinedLoadKg: Float) {
        val catOrd = vehicle.category.ordinal
        idleRpmHint = when (catOrd) {
            1 -> 600f
            0 -> 650f
            2 -> 620f
            3 -> 580f
            else -> 780f
        }
        maxRpmHint = when (catOrd) {
            1 -> 2250f
            0 -> 2450f
            2 -> 2200f
            3 -> 2350f
            else -> 3800f
        }
        SphereNativeEngine.configureVehicleAudio(
            categoryOrdinal = catOrd,
            powerHp = vehicle.enginePowerHp,
            torqueNm = vehicle.engineTorqueNm,
            kerbKg = vehicle.kerbWeightKg,
            combinedLoadKg = combinedLoadKg
        )
    }

    fun toggleIgnition() {
        setIgnition(!isEngineOn)
    }

    fun setIgnition(turnOn: Boolean) {
        SphereNativeEngine.setVehicleIgnition(turnOn)
        if (turnOn) {
            ignitionState = 1
            activeStreamRequested = true
        } else {
            ignitionState = 0
            throttlePressed = false
            cruiseDrive = false
            syncControlsToNative()
            if (!SphereNativeEngine.isNativeEngineAvailable()) {
                rpm = 0f
                gear = 0
                turboBoostBar = 0f
                activeStreamRequested = false
            }
        }
    }

    fun setThrottle(pressed: Boolean) {
        if (throttlePressed == pressed) return
        throttlePressed = pressed
        syncControlsToNative()
        if (!SphereNativeEngine.isNativeEngineAvailable() && pressed && isEngineOn) {
            ignitionState = 2
            rpm = (idleRpmHint + (maxRpmHint - idleRpmHint) * 0.72f)
            speedKmh = max(speedKmh, 28f)
            gear = max(gear, 3)
            turboBoostBar = 1.6f
        }
    }

    fun setBrake(pressed: Boolean) {
        val releasedHeavy = brakePressed && !pressed && speedKmh > 1f
        if (brakePressed == pressed) return
        brakePressed = pressed
        syncControlsToNative()
        if ((pressed && speedKmh > 3.5f) || releasedHeavy) {
            lastOneShotMs = System.currentTimeMillis()
            activeStreamRequested = true
        }
        if (!SphereNativeEngine.isNativeEngineAvailable() && pressed) {
            speedKmh = max(0f, speedKmh - 18f)
            if (speedKmh < 1f) gear = 0
        }
    }

    fun toggleCruise() {
        val next = !cruiseDrive
        if (next && !isEngineOn) {
            setIgnition(true)
        }
        cruiseDrive = next
        syncControlsToNative()
        if (!SphereNativeEngine.isNativeEngineAvailable() && next) {
            ignitionState = 2
            speedKmh = 64f
            gear = 8
            rpm = 1480f
            turboBoostBar = 1.2f
        }
    }

    fun toggleMute() {
        isMuted = !isMuted
        syncControlsToNative()
    }

    fun triggerHorn(isMinibus: Boolean) {
        val id = if (isMinibus) VehicleOneShotIds.MATATU_HORN else VehicleOneShotIds.TRUCK_HORN_HEAVY
        SphereNativeEngine.triggerVehicleOneShot(id, 1.0f)
        lastOneShotMs = System.currentTimeMillis()
        activeStreamRequested = true
    }

    fun triggerDoorPneumatic() {
        SphereNativeEngine.triggerVehicleOneShot(VehicleOneShotIds.BUS_DOOR_PNEUMATIC, 0.95f)
        lastOneShotMs = System.currentTimeMillis()
        activeStreamRequested = true
    }

    private fun syncControlsToNative() {
        SphereNativeEngine.setVehicleAudioControls(
            throttle = if (throttlePressed) 1f else 0f,
            brake = if (brakePressed) 1f else 0f,
            cruiseDrive = cruiseDrive,
            muted = isMuted
        )
    }

    internal fun applyTelemetryArray(vals: FloatArray) {
        if (vals.size < 7) return
        ignitionState = vals[0].toInt()
        rpm = vals[1]
        speedKmh = vals[2]
        gear = vals[3].toInt()
        turboBoostBar = vals[4]
        distanceM = vals[5]
        val hasAudio = vals[6] > 0.5f
        val oneShotRecent = (System.currentTimeMillis() - lastOneShotMs) < 1350L
        if (!hasAudio && !oneShotRecent && !throttlePressed && !cruiseDrive) {
            activeStreamRequested = false
        }
    }

    internal fun stepJvmFallback(dtSec: Float) {
        if (!isEngineOn) {
            speedKmh = max(0f, speedKmh - (4f + if (brakePressed) 32f else 0f) * dtSec)
            rpm = max(0f, rpm - 950f * dtSec)
            if (speedKmh < 0.5f) gear = 0
            distanceM += (speedKmh / 3.6f) * dtSec
            val oneShotRecent = (System.currentTimeMillis() - lastOneShotMs) < 900L
            if (!oneShotRecent && rpm < 40f) {
                activeStreamRequested = false
            }
            return
        }
        ignitionState = 2
        val effThrottle = when {
            throttlePressed -> 1f
            cruiseDrive && !brakePressed -> if (speedKmh < 68f) 0.58f else 0.24f
            else -> 0f
        }
        val accel = effThrottle * 16f - (if (brakePressed) 34f else 0f) - 1.1f - speedKmh * 0.07f
        speedKmh = min(110f, max(0f, speedKmh + accel * dtSec))
        distanceM += (speedKmh / 3.6f) * dtSec
        gear = if (speedKmh < 1f && effThrottle < 0.05f) 0 else min(12, max(1, (speedKmh / 9f).toInt() + 1))
        val targetRpm = if (gear == 0) {
            idleRpmHint + effThrottle * (maxRpmHint - idleRpmHint) * 0.85f
        } else {
            idleRpmHint + (speedKmh % 12f) * 85f + effThrottle * 320f
        }
        rpm += (targetRpm - rpm) * min(1f, 5f * dtSec)
        turboBoostBar = ((rpm - idleRpmHint) / max(400f, maxRpmHint - idleRpmHint)).coerceIn(0f, 1f) *
            (0.25f + 0.75f * effThrottle) * 2.1f
    }

    internal fun snapshotCurrentTelemetry() {
        if (SphereNativeEngine.isNativeEngineAvailable()) {
            applyTelemetryArray(SphereNativeEngine.getVehicleAudioTelemetry())
        }
    }

    val normalizedRpm: Float
        get() = ((rpm - 0f) / max(1000f, maxRpmHint)).coerceIn(0f, 1f)
}

@Composable
fun rememberVehicleAudioController(
    vehicle: VehicleItem,
    combinedLoadKg: Float
): VehicleAudioController {
    val context = LocalContext.current
    val controller = remember { VehicleAudioController() }

    LaunchedEffect(vehicle.id, vehicle.category, vehicle.enginePowerHp, combinedLoadKg) {
        controller.configureVehicle(vehicle, combinedLoadKg)
    }

    val isActiveAudio = controller.activeStreamRequested
    LaunchedEffect(isActiveAudio) {
        if (!isActiveAudio) return@LaunchedEffect

        val nativeReady = SphereNativeEngine.isNativeEngineAvailable()
        if (!nativeReady) {
            while (isActive && controller.activeStreamRequested) {
                delay(40L)
                controller.stepJvmFallback(0.040f)
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
                    val telemetry = SphereNativeEngine.renderVehicleAudioBlock(pcmBuffer, blockSamples)
                    if (started) {
                        audioTrack.writeMonoBlock(pcmBuffer, blockSamples)
                    } else {
                        delay(23L)
                    }
                    uiStep++
                    if (uiStep % 2 == 0) {
                        withContext(Dispatchers.Main) {
                            controller.applyTelemetryArray(telemetry)
                        }
                    }
                }
            } catch (_: Throwable) {
            } finally {
                audioTrack.stopAndRelease()
                withContext(Dispatchers.Main) {
                    controller.snapshotCurrentTelemetry()
                }
            }
        }
    }

    DisposableEffect(Unit) {
        onDispose {
            controller.setIgnition(false)
        }
    }

    return controller
}

fun computeFallbackGridCellX(distanceM: Float): Int = floor(abs(distanceM) / 64f).toInt()
