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
    stageMode: Int = 0
) {
    var isSurfaceReady by remember { mutableStateOf(false) }

    LaunchedEffect(
        isSurfaceReady,
        lightPosX, lightPosY, lightPosZ,
        intensity, colorR, colorG, colorB,
        roughness, metallic,
        objectWidth, objectHeight, objectDepth,
        stageMode
    ) {
        if (!isSurfaceReady) return@LaunchedEffect
        while (isActive) {
            withFrameNanos {
                if (SphereNativeEngine.isVulkanInitialized()) {
                    SphereNativeEngine.renderVulkanFrame(
                        lightPosX, lightPosY, lightPosZ,
                        intensity, colorR, colorG, colorB,
                        roughness, metallic,
                        objectWidth, objectHeight, objectDepth,
                        stageMode
                    )
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
                        val w = width.coerceAtLeast(640)
                        val h = height.coerceAtLeast(480)
                        val initialized = SphereNativeEngine.createVulkanSurface(surfaceHolder.surface, w, h)
                        if (initialized) {
                            SphereNativeEngine.renderVulkanFrame(
                                lightPosX, lightPosY, lightPosZ,
                                intensity, colorR, colorG, colorB,
                                roughness, metallic,
                                objectWidth, objectHeight, objectDepth,
                                stageMode
                            )
                            isSurfaceReady = true
                        }
                    }

                    override fun surfaceChanged(surfaceHolder: SurfaceHolder, format: Int, w: Int, h: Int) {
                        SphereNativeEngine.resizeVulkanSurface(w, h)
                        if (SphereNativeEngine.isVulkanInitialized()) {
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
                        SphereNativeEngine.destroyVulkanSurface()
                    }
                })
            }
        },
        update = {
            if (SphereNativeEngine.isVulkanInitialized()) {
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
