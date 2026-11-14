package com.example.sphere

import android.graphics.Bitmap
import androidx.compose.ui.geometry.Offset
import com.example.sphere.nativebridge.SphereNativeEngine
import kotlin.math.PI
import kotlin.math.cos
import kotlin.math.sin
import kotlin.math.tan

// =====================================================================
// NATIVE C++ / VULKAN VOLUMETRIC CLOUD RAYMARCHER INTERFACE
// =====================================================================
object ProceduralCloudSynthesizer {

    /**
     * Renders a true 3D perspective volumetric cloudscape raymarched strictly in native C++.
     */
    @Suppress("UNUSED_PARAMETER")
    fun renderPerspectiveSky(
        width: Int = 240,
        height: Int = 135,
        camYaw: Float,
        camPitch: Float,
        timeSeconds: Float,
        windSpeed: Float,
        windAzimuth: Float,
        cloudType: Int,
        coverage: Float,
        density: Float,
        silverLiningG: Float,
        powderEffect: Float,
        sunAbsorption: Float,
        detailErosion: Float,
        cloudBaseAltitude: Float,
        isStorm: Boolean,
        horizonYFrac: Float,
        multiScatteringFactor: Float = 0.50f,
        enableBayerJitter: Boolean = true,
        sunAzimuth: Float = 225.0f,
        sunElevation: Float = 38.0f
    ): Bitmap {
        val bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
        val pixels = IntArray(width * height)

        val nativeSucceeded = SphereNativeEngine.renderVolumetricClouds(
            outPixels = pixels,
            width = width,
            height = height,
            camYaw = camYaw,
            camPitch = camPitch,
            sunAzimuth = sunAzimuth,
            sunElevation = sunElevation,
            cloudType = cloudType,
            coverage = coverage,
            density = density,
            time = timeSeconds
        )

        if (nativeSucceeded) {
            bitmap.setPixels(pixels, 0, width, 0, 0, width, height)
        }
        return bitmap
    }

    /**
     * Projects 3D celestial sun vector onto the screen using camera pitch and yaw.
     */
    fun projectSunToScreen(
        w: Float,
        h: Float,
        camYaw: Float,
        camPitch: Float,
        sunAzimuth: Float = 225.0f,
        sunElevation: Float = 38.0f
    ): Offset? {
        val fovH = 75.0f * (PI.toFloat() / 180f)
        val aspect = w / h
        val tanH = tan(fovH * 0.5f)
        val tanV = tanH / aspect

        val pitchRad = camPitch * (PI.toFloat() / 180f)
        val yawRad = camYaw * (PI.toFloat() / 180f)

        val sunAzRad = sunAzimuth * (PI.toFloat() / 180f)
        val sunElRad = sunElevation * (PI.toFloat() / 180f)
        val sx = cos(sunElRad) * sin(sunAzRad)
        val sy = sin(sunElRad)
        val sz = cos(sunElRad) * cos(sunAzRad)

        val cosY = cos(yawRad)
        val sinY = sin(yawRad)
        val sx1 = sx * cosY - sz * sinY
        val sy1 = sy
        val sz1 = sx * sinY + sz * cosY

        val cosP = cos(pitchRad)
        val sinP = sin(pitchRad)
        val sx2 = sx1
        val sy2 = sy1 * cosP - sz1 * sinP
        val sz2 = sy1 * sinP + sz1 * cosP

        if (sz2 <= 0.05f) return null

        val normX = sx2 / (sz2 * tanH)
        val normY = sy2 / (sz2 * tanV)

        val screenX = (normX * 0.5f + 0.5f) * w
        val screenY = (0.5f - normY * 0.5f) * h
        return Offset(screenX, screenY)
    }
}
