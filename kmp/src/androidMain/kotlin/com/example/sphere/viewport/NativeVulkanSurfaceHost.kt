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
 * Direct Android SurfaceView host for Sascha Willems' Vulkan 1.3 C++ swapchain.
 * Employs ANativeWindow and VK_KHR_android_surface to provide 120 FPS hardware-composited
 * rendering directly from the native C++ engine into Android SurfaceFlinger.
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

    // Display VSync-synchronized render loop while surface is active
    LaunchedEffect(isSurfaceReady, lightPosX, lightPosY, lightPosZ, intensity, colorR, colorG, colorB, roughness, metallic, objectWidth, objectHeight, objectDepth, stageMode) {
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
                        try {
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
                        } catch (e: Throwable) {
                            android.util.Log.w("VulkanSurfaceHost", "Native Vulkan surface creation handled: ${e.message}")
                        }
                    }

                    override fun surfaceChanged(surfaceHolder: SurfaceHolder, format: Int, w: Int, h: Int) {
                        try {
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
                        } catch (e: Throwable) {
                            android.util.Log.w("VulkanSurfaceHost", "Native Vulkan resize handled: ${e.message}")
                        }
                    }

                    override fun surfaceDestroyed(surfaceHolder: SurfaceHolder) {
                        isSurfaceReady = false
                        try {
                            SphereNativeEngine.destroyVulkanSurface()
                        } catch (e: Throwable) {
                            android.util.Log.w("VulkanSurfaceHost", "Native Vulkan destroy handled: ${e.message}")
                        }
                    }
                })
            }
        },
        update = {
            try {
                if (SphereNativeEngine.isVulkanInitialized()) {
                    SphereNativeEngine.renderVulkanFrame(
                        lightPosX, lightPosY, lightPosZ,
                        intensity, colorR, colorG, colorB,
                        roughness, metallic,
                        objectWidth, objectHeight, objectDepth,
                        stageMode
                    )
                }
            } catch (e: Throwable) {
                // Safe recovery
            }
        }
    )
}
