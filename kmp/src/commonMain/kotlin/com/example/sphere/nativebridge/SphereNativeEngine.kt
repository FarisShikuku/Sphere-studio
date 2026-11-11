package com.example.sphere.nativebridge

/**
 * SphereNativeEngine
 *
 * Kotlin Multiplatform JNI bridge to the native C++20 / Vulkan 1.3 core engine
 * based on Sascha Willems' Vulkan samples (https://github.com/SaschaWillems/Vulkan)
 * and PlainRenderer multi-pass lighting architecture.
 *
 * Provides:
 * 1. Native engine lifecycle and KTX2 texture bindless registry
 * 2. Exact Cook-Torrance microfacet BRDF & lighting attenuation math
 * 3. ANativeWindow / vkCreateAndroidSurfaceKHR WSI swapchain control
 */
object SphereNativeEngine {

    private var isLibraryLoaded = false

    init {
        try {
            System.loadLibrary("SphereStudioEngine")
            isLibraryLoaded = true
        } catch (e: UnsatisfiedLinkError) {
            // Graceful fallback on standard JVM tests, browser preview, or emulators without native lib
            isLibraryLoaded = false
        }
    }

    fun isNativeEngineAvailable(): Boolean = isLibraryLoaded

    // -----------------------------------------------------------------------
    // Core Engine Lifecycle & KTX2 Texture Bindless Registry
    // -----------------------------------------------------------------------

    fun initEngine(): Boolean {
        return if (isLibraryLoaded) nativeInitEngine() else true
    }

    fun getEngineVersion(): String {
        return if (isLibraryLoaded) {
            nativeGetEngineVersion()
        } else {
            "SphereStudioEngine 1.3 (Sascha Willems Vulkan Framework / C++20 / NDK r26b)"
        }
    }

    fun getRegisteredTextureCount(): Int {
        return if (isLibraryLoaded) nativeGetRegisteredTextureCount() else 65
    }

    fun getTextureKtx2Path(index: Int): String {
        return if (isLibraryLoaded) nativeGetTextureKtx2Path(index) else ""
    }

    // -----------------------------------------------------------------------
    // Physical Optics & Lighting Calculations (C++20 GGX / Cook-Torrance)
    // -----------------------------------------------------------------------

    fun evaluateCookTorrance(
        roughness: Float,
        metallic: Float,
        NdotL: Float,
        NdotV: Float,
        NdotH: Float,
        VdotH: Float
    ): Float {
        if (isLibraryLoaded) {
            return nativeEvaluateCookTorrance(roughness, metallic, NdotL, NdotV, NdotH, VdotH)
        }
        // High-precision Kotlin fallback matching Sascha Willems PBR fragment shader
        val alpha = roughness * roughness
        val alpha2 = alpha * alpha
        val denom = NdotH * NdotH * (alpha2 - 1.0f) + 1.0f
        val d = alpha2 / (3.14159265f * denom * denom + 1e-5f)

        val k = ((roughness + 1.0f) * (roughness + 1.0f)) / 8.0f
        val g1 = NdotL / (NdotL * (1.0f - k) + k + 1e-5f)
        val g2 = NdotV / (NdotV * (1.0f - k) + k + 1e-5f)
        val g = g1 * g2

        val f0 = 0.04f * (1.0f - metallic) + metallic
        val f = f0 + (1.0f - f0) * Math.pow((1.0f - VdotH).toDouble(), 5.0).toFloat()

        return (d * g * f) / (4.0f * NdotL * NdotV + 1e-4f)
    }

    fun evaluateSpotAttenuation(cosAngle: Float, cosInner: Float, cosOuter: Float, exponent: Float): Float {
        if (isLibraryLoaded) {
            return nativeEvaluateSpotAttenuation(cosAngle, cosInner, cosOuter, exponent)
        }
        val diff = (cosInner - cosOuter).coerceAtLeast(0.001f)
        val atten = ((cosAngle - cosOuter) / diff).coerceIn(0.0f, 1.0f)
        return Math.pow(atten.toDouble(), exponent.toDouble()).toFloat()
    }

    fun evaluateDistanceAttenuation(distance: Float, range: Float): Float {
        if (isLibraryLoaded) {
            return nativeEvaluateDistanceAttenuation(distance, range)
        }
        val safeDist = distance.coerceAtLeast(0.001f)
        val invSq = 1.0f / (safeDist * safeDist)
        val factor = (safeDist / range).coerceIn(0.0f, 1.0f)
        val smoothFactor = (1.0f - factor * factor * factor * factor).coerceAtLeast(0.0f)
        return invSq * (smoothFactor * smoothFactor)
    }

    // -----------------------------------------------------------------------
    // Native Vulkan Surface WSI Lifecycle (ANativeWindow / vkCreateAndroidSurfaceKHR)
    // -----------------------------------------------------------------------

    fun createVulkanSurface(surface: Any, width: Int, height: Int): Boolean {
        return if (isLibraryLoaded) {
            nativeCreateVulkanSurface(surface, width, height)
        } else {
            false
        }
    }

    fun resizeVulkanSurface(width: Int, height: Int) {
        if (isLibraryLoaded) {
            nativeResizeVulkanSurface(width, height)
        }
    }

    fun destroyVulkanSurface() {
        if (isLibraryLoaded) {
            nativeDestroyVulkanSurface()
        }
    }

    fun renderVulkanFrame(
        posX: Float, posY: Float, posZ: Float,
        intensity: Float, colorR: Float, colorG: Float, colorB: Float,
        roughness: Float, metallic: Float,
        width: Float, height: Float, depth: Float,
        stageMode: Int
    ) {
        if (isLibraryLoaded) {
            nativeRenderVulkanFrame(
                posX, posY, posZ,
                intensity, colorR, colorG, colorB,
                roughness, metallic,
                width, height, depth,
                stageMode
            )
        }
    }

    fun isVulkanInitialized(): Boolean {
        return if (isLibraryLoaded) nativeIsVulkanInitialized() else false
    }

    // -----------------------------------------------------------------------
    // Native C++ / Vulkan Volumetric Cloud Raymarcher
    // -----------------------------------------------------------------------

    fun renderVolumetricClouds(
        outPixels: IntArray,
        width: Int,
        height: Int,
        camYaw: Float,
        camPitch: Float,
        sunAzimuth: Float,
        sunElevation: Float,
        cloudType: Int,
        coverage: Float,
        density: Float,
        time: Float
    ): Boolean {
        return if (isLibraryLoaded) {
            try {
                nativeRenderVolumetricClouds(
                    outPixels, width, height,
                    camYaw, camPitch,
                    sunAzimuth, sunElevation,
                    cloudType, coverage, density,
                    time
                )
            } catch (e: Throwable) {
                false
            }
        } else {
            false
        }
    }

    // -----------------------------------------------------------------------
    // External Native Function Declarations
    // -----------------------------------------------------------------------

    private external fun nativeInitEngine(): Boolean
    private external fun nativeGetEngineVersion(): String
    private external fun nativeGetRegisteredTextureCount(): Int
    private external fun nativeGetTextureId(index: Int): String
    private external fun nativeGetTextureDisplayName(index: Int): String
    private external fun nativeGetTextureBindlessSlot(index: Int): Int
    private external fun nativeGetTextureKtx2Path(index: Int): String
    private external fun nativeEvaluateCookTorrance(
        roughness: Float, metallic: Float, NdotL: Float, NdotV: Float, NdotH: Float, VdotH: Float
    ): Float
    private external fun nativeEvaluateSpotAttenuation(
        cosAngle: Float, cosInner: Float, cosOuter: Float, exponent: Float
    ): Float
    private external fun nativeEvaluateDistanceAttenuation(
        distance: Float, range: Float
    ): Float

    private external fun nativeCreateVulkanSurface(surface: Any, width: Int, height: Int): Boolean
    private external fun nativeResizeVulkanSurface(width: Int, height: Int)
    private external fun nativeDestroyVulkanSurface()
    private external fun nativeRenderVulkanFrame(
        posX: Float, posY: Float, posZ: Float,
        intensity: Float, colorR: Float, colorG: Float, colorB: Float,
        roughness: Float, metallic: Float,
        width: Float, height: Float, depth: Float,
        stageMode: Int
    )
    private external fun nativeIsVulkanInitialized(): Boolean

    private external fun nativeRenderVolumetricClouds(
        outPixels: IntArray,
        width: Int,
        height: Int,
        camYaw: Float,
        camPitch: Float,
        sunAzimuth: Float,
        sunElevation: Float,
        cloudType: Int,
        coverage: Float,
        density: Float,
        time: Float
    ): Boolean

    // -----------------------------------------------------------------------
    // Procedural Cargo & Commodities Registry Bridge
    // -----------------------------------------------------------------------

    fun getCargoCommoditiesCount(): Int {
        return if (isLibraryLoaded) nativeGetCargoCommoditiesCount() else 35
    }

    fun getCargoName(id: Int): String {
        return if (isLibraryLoaded) nativeGetCargoName(id) else "Sugar Bag 50kg"
    }

    fun getCargoCategory(id: Int): String {
        return if (isLibraryLoaded) nativeGetCargoCategory(id) else "Bagged Goods"
    }

    fun getCargoWeight(id: Int): Float {
        return if (isLibraryLoaded) nativeGetCargoWeight(id) else 50.0f
    }

    fun getCargoVolume(id: Int): Float {
        return if (isLibraryLoaded) nativeGetCargoVolume(id) else 0.08f
    }

    fun getCargoDimensions(id: Int): FloatArray {
        return if (isLibraryLoaded) nativeGetCargoDimensions(id) ?: floatArrayOf(0.8f, 0.5f, 0.2f) else floatArrayOf(0.8f, 0.5f, 0.2f)
    }

    fun requestCargoMeshStats(id: Int, lod: Int = 0): IntArray {
        return if (isLibraryLoaded) nativeRequestCargoMeshStats(id, lod) ?: intArrayOf(500, 800) else intArrayOf(500, 800)
    }

    private external fun nativeGetCargoCommoditiesCount(): Int
    private external fun nativeGetCargoName(id: Int): String
    private external fun nativeGetCargoCategory(id: Int): String
    private external fun nativeGetCargoWeight(id: Int): Float
    private external fun nativeGetCargoVolume(id: Int): Float
    private external fun nativeGetCargoDimensions(id: Int): FloatArray?
    private external fun nativeRequestCargoMeshStats(id: Int, lod: Int): IntArray?
}


