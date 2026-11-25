package com.example.sphere.viewport

import android.view.SurfaceHolder
import android.view.SurfaceView
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.viewinterop.AndroidView
import com.example.sphere.nativebridge.SphereNativeEngine
import kotlinx.coroutines.isActive

/**
 * NativeVulkanSurfaceHost
 *
 * Direct Android SurfaceView host for the native C++ VulkanNativeSceneRenderer.
 * All rendering executes exclusively in C++ (Vulkan swapchain on hardware GPU or
 * native ANativeWindow C++ PBR software rasterizer when no /dev/dri/renderD* node exists).
 */
@Composable
fun NativeVulkanSurfaceHost(
    modifier: Modifier = Modifier,
    lightPosX: Float = 0.0f,
    lightPosY: Float = 2.5f,
    lightPosZ: Float = 1.8f,
    intensity: Float = 2500.0f,
    colorR: Float = 1.0f,
    colorG: Float = 0.95f,
    colorB: Float = 0.88f,
    roughness: Float = 0.45f,
    metallic: Float = 0.0f,
    objectWidth: Float = 2.0f,
    objectHeight: Float = 2.0f,
    objectDepth: Float = 2.0f,
    stageMode: Int = 0,
    onFpsUpdated: (Int, Float) -> Unit = { _, _ -> }
) {
    var isSurfaceReady by remember { mutableStateOf(false) }
    var activeHolder by remember { mutableStateOf<SurfaceHolder?>(null) }
    val latestOnFps by rememberUpdatedState(onFpsUpdated)

    DisposableEffect(Unit) {
        onDispose {
            isSurfaceReady = false
            activeHolder = null
            SphereNativeEngine.destroyVulkanSurface()
        }
    }

    LaunchedEffect(
        isSurfaceReady,
        lightPosX, lightPosY, lightPosZ,
        intensity, colorR, colorG, colorB,
        roughness, metallic,
        objectWidth, objectHeight, objectDepth,
        stageMode
    ) {
        if (!isSurfaceReady) return@LaunchedEffect
        val gateThresholdNs = SceneFpsConfig.TARGET_FRAME_PERIOD_NS - SceneFpsConfig.PACING_TOLERANCE_NS
        var lastPacedNanos = 0L
        var windowStartNanos = 0L
        var frameCount = 0
        var lastDrawMs = 0f

        while (isActive && isSurfaceReady) {
            withFrameNanos { frameNanos ->
                if (lastPacedNanos == 0L) {
                    lastPacedNanos = frameNanos
                    windowStartNanos = frameNanos
                }
                if (frameNanos - lastPacedNanos >= gateThresholdNs || frameCount == 0) {
                    lastPacedNanos = frameNanos
                    val holderValid = activeHolder?.surface?.isValid == true
                    if (isSurfaceReady && holderValid && SphereNativeEngine.isVulkanInitialized()) {
                        val t0 = System.nanoTime()
                        SphereNativeEngine.renderVulkanFrame(
                            lightPosX, lightPosY, lightPosZ,
                            intensity, colorR, colorG, colorB,
                            roughness, metallic,
                            objectWidth, objectHeight, objectDepth,
                            stageMode
                        )
                        lastDrawMs = ((System.nanoTime() - t0) / 1_000_000f).coerceAtLeast(0.1f)
                    }
                    frameCount++
                }
                val elapsedNs = frameNanos - windowStartNanos
                if (elapsedNs >= 500_000_000L && frameCount > 0) {
                    val fps = kotlin.math.round(
                        (frameCount.toDouble() * 1_000_000_000.0) / elapsedNs.toDouble()
                    ).toInt().coerceIn(1, SceneFpsConfig.TARGET_FPS)
                    latestOnFps(fps, lastDrawMs)
                    windowStartNanos = frameNanos
                    frameCount = 0
                }
            }
        }
    }

    AndroidView(
        modifier = modifier,
        factory = { context ->
            SurfaceView(context).apply {
                holder.addCallback(object : SurfaceHolder.Callback {
                    override fun surfaceCreated(surfaceHolder: SurfaceHolder) {
                        if (!surfaceHolder.surface.isValid) return
                        activeHolder = surfaceHolder
                        val w = width.coerceAtLeast(640)
                        val h = height.coerceAtLeast(480)
                        val initialized = SphereNativeEngine.createVulkanSurface(surfaceHolder.surface, w, h)
                        if (initialized) {
                            isSurfaceReady = true
                            SphereNativeEngine.renderVulkanFrame(
                                lightPosX, lightPosY, lightPosZ,
                                intensity, colorR, colorG, colorB,
                                roughness, metallic,
                                objectWidth, objectHeight, objectDepth,
                                stageMode
                            )
                        }
                    }

                    override fun surfaceChanged(surfaceHolder: SurfaceHolder, format: Int, w: Int, h: Int) {
                        if (!surfaceHolder.surface.isValid || w <= 0 || h <= 0) return
                        activeHolder = surfaceHolder
                        SphereNativeEngine.resizeVulkanSurface(w, h)
                        if (isSurfaceReady && SphereNativeEngine.isVulkanInitialized()) {
                            SphereNativeEngine.renderVulkanFrame(
                                lightPosX, lightPosY, lightPosZ,
                                intensity, colorR, colorG, colorB,
                                roughness, metallic,
                                objectWidth, objectHeight, objectDepth,
                                stageMode
                            )
                        }
                    }

                    override fun surfaceDestroyed(surfaceHolder: SurfaceHolder) {
                        isSurfaceReady = false
                        activeHolder = null
                        SphereNativeEngine.destroyVulkanSurface()
                    }
                })
            }
        },
        update = {
            if (isSurfaceReady && activeHolder?.surface?.isValid == true && SphereNativeEngine.isVulkanInitialized()) {
                SphereNativeEngine.renderVulkanFrame(
                    lightPosX, lightPosY, lightPosZ,
                    intensity, colorR, colorG, colorB,
                    roughness, metallic,
                    objectWidth, objectHeight, objectDepth,
                    stageMode
                )
            }
        }
    )
}
