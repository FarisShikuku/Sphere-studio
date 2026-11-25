package com.example.sphere.nativebridge

import android.graphics.Bitmap

/**
 * SphereNativeEngine
 *
 * Kotlin Multiplatform JNI bridge to the native C++20 / Vulkan 1.3 core engine
 * based on Sascha Willems' Vulkan samples (https://github.com/SaschaWillems/Vulkan)
 * and PlainRenderer multi-pass lighting architecture.
 *
 * All 3D procedural geometry (CargoRegistry, TrailerRegistry), volumetric cloud raymarching
 * (CloudRaymarch), Cook-Torrance PBR shading, and scene rasterization execute strictly in C++.
 */
object SphereNativeEngine {

    private var isLibraryLoaded = false

    init {
        try {
            System.loadLibrary("SphereStudioEngine")
            isLibraryLoaded = true
        } catch (_: UnsatisfiedLinkError) {
            isLibraryLoaded = false
        }
    }

    fun isNativeEngineAvailable(): Boolean = isLibraryLoaded

    // -----------------------------------------------------------------------
    // Core Engine Lifecycle & KTX2 Texture Bindless Registry
    // -----------------------------------------------------------------------

    fun initEngine(): Boolean {
        return if (isLibraryLoaded) nativeInitEngine() else false
    }

    fun getEngineVersion(): String {
        return if (isLibraryLoaded) {
            nativeGetEngineVersion()
        } else {
            "SphereStudioEngine 1.3 (C++20 Native Core)"
        }
    }

    fun getRegisteredTextureCount(): Int {
        return if (isLibraryLoaded) nativeGetRegisteredTextureCount() else 71
    }

    fun getTextureKtx2Path(index: Int): String {
        return if (isLibraryLoaded) nativeGetTextureKtx2Path(index) else ""
    }

    // -----------------------------------------------------------------------
    // Physical Optics & Lighting Calculations (C++20 GGX / Cook-Torrance Only)
    // -----------------------------------------------------------------------

    fun evaluateCookTorrance(
        roughness: Float,
        metallic: Float,
        NdotL: Float,
        NdotV: Float,
        NdotH: Float,
        VdotH: Float
    ): Float {
        return if (isLibraryLoaded) {
            nativeEvaluateCookTorrance(roughness, metallic, NdotL, NdotV, NdotH, VdotH)
        } else {
            0f
        }
    }

    fun evaluateSpotAttenuation(cosAngle: Float, cosInner: Float, cosOuter: Float, exponent: Float): Float {
        return if (isLibraryLoaded) {
            nativeEvaluateSpotAttenuation(cosAngle, cosInner, cosOuter, exponent)
        } else {
            0f
        }
    }

    fun evaluateDistanceAttenuation(distance: Float, range: Float): Float {
        return if (isLibraryLoaded) {
            nativeEvaluateDistanceAttenuation(distance, range)
        } else {
            0f
        }
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

    fun isVulkanGpuAvailable(): Boolean {
        return if (isLibraryLoaded) {
            try {
                nativeIsVulkanGpuAvailable()
            } catch (_: Throwable) {
                false
            }
        } else {
            false
        }
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
    private external fun nativeIsVulkanGpuAvailable(): Boolean

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
        return if (isLibraryLoaded) nativeGetCargoCommoditiesCount() else 51
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
    private external fun nativeGetCargoWeight(id: Int): Float
    private external fun nativeGetCargoCategory(id: Int): String
    private external fun nativeGetCargoVolume(id: Int): Float
    private external fun nativeGetCargoDimensions(id: Int): FloatArray?
    private external fun nativeRequestCargoMeshStats(id: Int, lod: Int): IntArray?

    // -----------------------------------------------------------------------
    // Procedural Trailers & Haulage Registry Bridge
    // -----------------------------------------------------------------------

    fun getTrailersCount(): Int {
        return if (isLibraryLoaded) nativeGetTrailersCount() else 51
    }

    fun getTrailerName(id: Int): String {
        return if (isLibraryLoaded) nativeGetTrailerName(id) else "Standard 40ft Flatbed"
    }

    fun getTrailerCategory(id: Int): String {
        return if (isLibraryLoaded) nativeGetTrailerCategory(id) else "Flatbeds"
    }

    fun getTrailerSpecs(id: Int): FloatArray {
        return if (isLibraryLoaded) {
            nativeGetTrailerSpecs(id) ?: floatArrayOf(6200f, 32000f, 13.6f, 2.55f, 1.55f, 1.35f)
        } else {
            floatArrayOf(6200f, 32000f, 13.6f, 2.55f, 1.55f, 1.35f)
        }
    }

    fun requestTrailerMeshStats(id: Int, lod: Int = 0): IntArray {
        return if (isLibraryLoaded) nativeRequestTrailerMeshStats(id, lod) ?: intArrayOf(1200, 1800) else intArrayOf(1200, 1800)
    }

    fun renderCargoBitmap(
        bitmap: Bitmap,
        cargoId: Int,
        stackingMode: Int,
        rotX: Float,
        rotY: Float,
        zoom: Float,
        showPallet: Boolean,
        showStraps: Boolean,
        showShrinkwrap: Boolean,
        primaryArgb: Int,
        secondaryArgb: Int,
        roughness: Float,
        metallic: Float,
        wear: Float,
        wrinkle: Float,
        opacity: Float,
        lowDetail: Boolean
    ): IntArray {
        return if (isLibraryLoaded) {
            nativeRenderCargoBitmap(
                bitmap, cargoId, stackingMode, rotX, rotY, zoom,
                showPallet, showStraps, showShrinkwrap,
                primaryArgb, secondaryArgb, roughness, metallic, wear, wrinkle, opacity, lowDetail
            ) ?: intArrayOf(0, 0)
        } else {
            intArrayOf(0, 0)
        }
    }

    fun renderTrailerBitmap(
        bitmap: Bitmap,
        trailerId: Int,
        rotX: Float,
        rotY: Float,
        zoom: Float,
        landingGearExtended: Boolean,
        showLoadedCargo: Boolean,
        cargoId: Int,
        cargoQty: Int,
        curtainOpenRatio: Float,
        hydraulicLiftRatio: Float,
        curtainOpacity: Float,
        baseArgb: Int,
        secondaryArgb: Int,
        roughness: Float,
        metallic: Float,
        axleCount: Int,
        suspensionOrdinal: Int,
        brakeOrdinal: Int,
        hasLiftAxle: Boolean,
        liftAxleRaised: Boolean,
        hasSteerableAxle: Boolean,
        steerAngleDeg: Float
    ): IntArray {
        return if (isLibraryLoaded) {
            nativeRenderTrailerBitmap(
                bitmap, trailerId, rotX, rotY, zoom,
                landingGearExtended, showLoadedCargo, cargoId, cargoQty,
                curtainOpenRatio, hydraulicLiftRatio, curtainOpacity,
                baseArgb, secondaryArgb, roughness, metallic,
                axleCount, suspensionOrdinal, brakeOrdinal,
                hasLiftAxle, liftAxleRaised, hasSteerableAxle, steerAngleDeg
            ) ?: intArrayOf(0, 0)
        } else {
            intArrayOf(0, 0)
        }
    }

    private external fun nativeGetTrailersCount(): Int
    private external fun nativeGetTrailerName(id: Int): String
    private external fun nativeGetTrailerCategory(id: Int): String
    private external fun nativeGetTrailerSpecs(id: Int): FloatArray?
    private external fun nativeRequestTrailerMeshStats(id: Int, lod: Int): IntArray?

    private external fun nativeRenderCargoBitmap(
        bitmap: Bitmap,
        cargoId: Int,
        stackingMode: Int,
        rotX: Float,
        rotY: Float,
        zoom: Float,
        showPallet: Boolean,
        showStraps: Boolean,
        showShrinkwrap: Boolean,
        primaryArgb: Int,
        secondaryArgb: Int,
        roughness: Float,
        metallic: Float,
        wear: Float,
        wrinkle: Float,
        opacity: Float,
        lowDetail: Boolean
    ): IntArray?

    private external fun nativeRenderTrailerBitmap(
        bitmap: Bitmap,
        trailerId: Int,
        rotX: Float,
        rotY: Float,
        zoom: Float,
        landingGearExtended: Boolean,
        showLoadedCargo: Boolean,
        cargoId: Int,
        cargoQty: Int,
        curtainOpenRatio: Float,
        hydraulicLiftRatio: Float,
        curtainOpacity: Float,
        baseArgb: Int,
        secondaryArgb: Int,
        roughness: Float,
        metallic: Float,
        axleCount: Int,
        suspensionOrdinal: Int,
        brakeOrdinal: Int,
        hasLiftAxle: Boolean,
        liftAxleRaised: Boolean,
        hasSteerableAxle: Boolean,
        steerAngleDeg: Float
    ): IntArray?

    // -----------------------------------------------------------------------
    // Procedural Vehicles & Transports Registry Bridge (Lorries & Buses)
    // -----------------------------------------------------------------------

    fun getVehiclesCount(): Int {
        return if (isLibraryLoaded) nativeGetVehiclesCount() else 51
    }

    fun getVehicleName(id: Int): String {
        return if (isLibraryLoaded) nativeGetVehicleName(id) else "Rigid Box Delivery Lorry (18T)"
    }

    fun getVehicleCategory(id: Int): String {
        return if (isLibraryLoaded) nativeGetVehicleCategory(id) else "Rigid Lorries"
    }

    fun getVehicleSpecs(id: Int): FloatArray {
        return if (isLibraryLoaded) {
            nativeGetVehicleSpecs(id) ?: floatArrayOf(7250f, 18000f, 4.8f, 0f, 1150f, 290f, 2f, 8.2f, 2.5f, 3.45f)
        } else {
            floatArrayOf(7250f, 18000f, 4.8f, 0f, 1150f, 290f, 2f, 8.2f, 2.5f, 3.45f)
        }
    }

    fun renderVehicleBitmap(
        bitmap: Bitmap,
        vehicleId: Int,
        rotX: Float,
        rotY: Float,
        zoom: Float,
        liveryOrdinal: Int,
        randomSeed: Int,
        baseArgb: Int,
        accentArgb: Int,
        roughness: Float,
        metallic: Float,
        wheelbaseM: Float,
        fifthWheelOffsetM: Float,
        actionRatio: Float,
        doorOpenRatio: Float,
        steerAngleDeg: Float,
        headlightsOn: Boolean,
        hasRoofRack: Boolean,
        hasExhaustStacks: Boolean,
        hitchTrailer: Boolean,
        trailerId: Int,
        showLoadedCargo: Boolean,
        cargoId: Int,
        cargoQty: Int,
        isRhd: Boolean = false,
        insideCabinView: Boolean = false
    ): IntArray {
        return if (isLibraryLoaded) {
            nativeRenderVehicleBitmap(
                bitmap, vehicleId, rotX, rotY, zoom,
                liveryOrdinal, randomSeed, baseArgb, accentArgb,
                roughness, metallic, wheelbaseM, fifthWheelOffsetM,
                actionRatio, doorOpenRatio, steerAngleDeg, headlightsOn, hasRoofRack, hasExhaustStacks,
                hitchTrailer, trailerId, showLoadedCargo, cargoId, cargoQty,
                isRhd, insideCabinView
            ) ?: intArrayOf(0, 0, 0, 0)
        } else {
            intArrayOf(0, 0, 0, 0)
        }
    }

    private external fun nativeGetVehiclesCount(): Int
    private external fun nativeGetVehicleName(id: Int): String
    private external fun nativeGetVehicleCategory(id: Int): String
    private external fun nativeGetVehicleSpecs(id: Int): FloatArray?

    private external fun nativeRenderVehicleBitmap(
        bitmap: Bitmap,
        vehicleId: Int,
        rotX: Float,
        rotY: Float,
        zoom: Float,
        liveryOrdinal: Int,
        randomSeed: Int,
        baseArgb: Int,
        accentArgb: Int,
        roughness: Float,
        metallic: Float,
        wheelbaseM: Float,
        fifthWheelOffsetM: Float,
        actionRatio: Float,
        doorOpenRatio: Float,
        steerAngleDeg: Float,
        headlightsOn: Boolean,
        hasRoofRack: Boolean,
        hasExhaustStacks: Boolean,
        hitchTrailer: Boolean,
        trailerId: Int,
        showLoadedCargo: Boolean,
        cargoId: Int,
        cargoQty: Int,
        isRhd: Boolean,
        insideCabinView: Boolean
    ): IntArray?

    // -----------------------------------------------------------------------
    // Procedural Vehicle Audio & Spatial Meshlet Streaming Bridge
    // -----------------------------------------------------------------------

    fun configureVehicleAudio(
        categoryOrdinal: Int,
        powerHp: Float,
        torqueNm: Float,
        kerbKg: Float,
        combinedLoadKg: Float
    ) {
        if (isLibraryLoaded) {
            nativeConfigureVehicleAudio(categoryOrdinal, powerHp, torqueNm, kerbKg, combinedLoadKg)
        }
    }

    fun setVehicleIgnition(turnOn: Boolean) {
        if (isLibraryLoaded) {
            nativeSetVehicleIgnition(turnOn)
        }
    }

    fun setVehicleAudioControls(
        throttle: Float,
        brake: Float,
        cruiseDrive: Boolean,
        muted: Boolean
    ) {
        if (isLibraryLoaded) {
            nativeSetVehicleAudioControls(throttle, brake, cruiseDrive, muted)
        }
    }

    fun triggerVehicleOneShot(oneShotId: Int, gain: Float = 1.0f) {
        if (isLibraryLoaded) {
            nativeTriggerVehicleOneShot(oneShotId, gain)
        }
    }

    fun renderVehicleAudioBlock(outPcmArray: ShortArray, sampleCount: Int): FloatArray {
        return if (isLibraryLoaded) {
            nativeRenderVehicleAudioBlock(outPcmArray, sampleCount)
                ?: floatArrayOf(0f, 0f, 0f, 0f, 0f, 0f, 0f)
        } else {
            floatArrayOf(0f, 0f, 0f, 0f, 0f, 0f, 0f)
        }
    }

    fun getVehicleAudioTelemetry(): FloatArray {
        return if (isLibraryLoaded) {
            nativeGetVehicleAudioTelemetry() ?: floatArrayOf(0f, 0f, 0f, 0f, 0f, 0f, 0f)
        } else {
            floatArrayOf(0f, 0f, 0f, 0f, 0f, 0f, 0f)
        }
    }

    fun getVehicleStreamingStats(
        vehicleId: Int,
        hitchTrailer: Boolean,
        trailerId: Int,
        showLoadedCargo: Boolean,
        cargoId: Int,
        cargoQty: Int,
        distanceDrivenM: Float
    ): IntArray {
        return if (isLibraryLoaded) {
            nativeGetVehicleStreamingStats(
                vehicleId,
                hitchTrailer,
                trailerId,
                showLoadedCargo,
                cargoId,
                cargoQty,
                distanceDrivenM
            ) ?: intArrayOf(0, 0, 9, 0, 126, 6, 0)
        } else {
            val gridX = kotlin.math.floor(distanceDrivenM / 64f).toInt()
            intArrayOf(gridX, 0, 9, if (hitchTrailer) 28 else 16, 126, 6, if (hitchTrailer) 2576 else 1472)
        }
    }

    private external fun nativeConfigureVehicleAudio(
        categoryOrdinal: Int,
        powerHp: Float,
        torqueNm: Float,
        kerbKg: Float,
        combinedLoadKg: Float
    )
    private external fun nativeSetVehicleIgnition(turnOn: Boolean)
    private external fun nativeSetVehicleAudioControls(
        throttle: Float,
        brake: Float,
        cruiseDrive: Boolean,
        muted: Boolean
    )
    private external fun nativeTriggerVehicleOneShot(oneShotId: Int, gain: Float)
    private external fun nativeRenderVehicleAudioBlock(outPcmArray: ShortArray, sampleCount: Int): FloatArray?
    private external fun nativeGetVehicleAudioTelemetry(): FloatArray?
    private external fun nativeGetVehicleStreamingStats(
        vehicleId: Int,
        hitchTrailer: Boolean,
        trailerId: Int,
        showLoadedCargo: Boolean,
        cargoId: Int,
        cargoQty: Int,
        distanceDrivenM: Float
    ): IntArray?

    // -----------------------------------------------------------------------
    // Procedural Weather Audio Bridge (gen/audio/weather)
    // -----------------------------------------------------------------------

    fun configureWeatherAudio(
        rainIntensity: Float,
        snowIntensity: Float,
        windSpeedKmh: Float,
        windAzimuthDeg: Float,
        fogDensity: Float,
        stormActive: Boolean,
        enabled: Boolean,
        muted: Boolean
    ) {
        if (isLibraryLoaded) {
            nativeConfigureWeatherAudio(
                rainIntensity,
                snowIntensity,
                windSpeedKmh,
                windAzimuthDeg,
                fogDensity,
                stormActive,
                enabled,
                muted
            )
        }
    }

    fun triggerWeatherThunder(closeStrike: Boolean, gain: Float = 1.0f) {
        if (isLibraryLoaded) {
            nativeTriggerWeatherThunder(closeStrike, gain)
        }
    }

    fun renderWeatherAudioBlock(outPcmArray: ShortArray, sampleCount: Int): FloatArray {
        return if (isLibraryLoaded) {
            nativeRenderWeatherAudioBlock(outPcmArray, sampleCount)
                ?: floatArrayOf(0f, 0f, 0f, 0f, 0f)
        } else {
            floatArrayOf(0f, 0f, 0f, 0f, 0f)
        }
    }

    fun getWeatherAudioTelemetry(): FloatArray {
        return if (isLibraryLoaded) {
            nativeGetWeatherAudioTelemetry() ?: floatArrayOf(0f, 0f, 0f, 0f, 0f)
        } else {
            floatArrayOf(0f, 0f, 0f, 0f, 0f)
        }
    }

    object AudioSampleSlots {
        const val AIR_BRAKE = 0
        const val TRUCK_HORN = 1
        const val BUS_DOOR = 2
        const val MATATU_HORN = 3
        const val GEAR_SHIFT = 4
        const val TRUCK_ENGINE = 5
        const val RAIN_LOOP = 10
        const val WIND_LOOP = 11
        const val THUNDER_CLOSE = 12
        const val THUNDER_DISTANT = 13
    }

    fun uploadCustomAudioSample(slotId: Int, pcm16: ShortArray, sampleRate: Int) {
        if (isLibraryLoaded && pcm16.isNotEmpty() && sampleRate > 0) {
            nativeUploadCustomAudioSample(slotId, pcm16, sampleRate)
        }
    }

    private external fun nativeConfigureWeatherAudio(
        rainIntensity: Float,
        snowIntensity: Float,
        windSpeedKmh: Float,
        windAzimuthDeg: Float,
        fogDensity: Float,
        stormActive: Boolean,
        enabled: Boolean,
        muted: Boolean
    )
    private external fun nativeTriggerWeatherThunder(closeStrike: Boolean, gain: Float)
    private external fun nativeRenderWeatherAudioBlock(outPcmArray: ShortArray, sampleCount: Int): FloatArray?
    private external fun nativeGetWeatherAudioTelemetry(): FloatArray?
    private external fun nativeUploadCustomAudioSample(slotId: Int, pcm16: ShortArray, sampleRate: Int)

    // -----------------------------------------------------------------------
    // Vulkan SPH & Screen-Space Fluid Rendering Bridge (gen/water)
    // -----------------------------------------------------------------------

    fun configureWaterSimulation(
        particleCount: Int,
        scenarioOrdinal: Int,
        smoothingRadius: Float,
        targetDensity: Float,
        pressureMultiplier: Float,
        nearPressureMultiplier: Float,
        viscosityStrength: Float,
        surfaceTension: Float,
        collisionDamping: Float,
        gravityX: Float,
        gravityY: Float,
        gravityZ: Float,
        windX: Float,
        windZ: Float,
        timeScale: Float,
        substeps: Int,
        boundsWidth: Float,
        boundsHeight: Float,
        boundsDepth: Float,
        waveMakerEnabled: Boolean,
        waveFrequency: Float,
        waveAmplitude: Float,
        obstacleEnabled: Boolean,
        obstacleX: Float,
        obstacleY: Float,
        obstacleZ: Float,
        obstacleRadius: Float,
        debugModeOrdinal: Int,
        particleVisualRadius: Float,
        bilateralBlurEnabled: Boolean,
        blurRadiusPx: Int,
        blurSpatialSigma: Float,
        blurDepthSigma: Float,
        waterR: Float,
        waterG: Float,
        waterB: Float,
        absorbR: Float,
        absorbG: Float,
        absorbB: Float,
        absorbStrength: Float,
        indexOfRefraction: Float,
        refractionStrength: Float,
        fresnelPower: Float,
        fresnelBias: Float,
        specularPower: Float,
        reflectionIntensity: Float,
        foamThreshold: Float,
        causticsIntensity: Float,
        sunAzimuthDeg: Float,
        sunElevationDeg: Float,
        showContainerBox: Boolean,
        showFloorGrid: Boolean,
        camYawDeg: Float,
        camPitchDeg: Float,
        camDistance: Float
    ) {
        if (isLibraryLoaded) {
            nativeConfigureWaterSimulation(
                particleCount, scenarioOrdinal, smoothingRadius, targetDensity,
                pressureMultiplier, nearPressureMultiplier, viscosityStrength, surfaceTension,
                collisionDamping, gravityX, gravityY, gravityZ, windX, windZ,
                timeScale, substeps, boundsWidth, boundsHeight, boundsDepth,
                waveMakerEnabled, waveFrequency, waveAmplitude,
                obstacleEnabled, obstacleX, obstacleY, obstacleZ, obstacleRadius,
                debugModeOrdinal, particleVisualRadius, bilateralBlurEnabled,
                blurRadiusPx, blurSpatialSigma, blurDepthSigma,
                waterR, waterG, waterB, absorbR, absorbG, absorbB, absorbStrength,
                indexOfRefraction, refractionStrength, fresnelPower, fresnelBias,
                specularPower, reflectionIntensity, foamThreshold, causticsIntensity,
                sunAzimuthDeg, sunElevationDeg, showContainerBox, showFloorGrid,
                camYawDeg, camPitchDeg, camDistance
            )
        }
    }

    fun resetWaterSimulation(scenarioOrdinal: Int, particleCount: Int) {
        if (isLibraryLoaded) {
            nativeResetWaterSimulation(scenarioOrdinal, particleCount)
        }
    }

    fun configureWaterfall(
        enabled: Boolean,
        cliffHeight: Float,
        cliffWidth: Float,
        cliffDepth: Float,
        flowVolume: Float,
        lipSlopeDeg: Float,
        undercut: Float,
        steppedLedges: Int,
        roughness: Float,
        rockChanneling: Float,
        airDragAndMist: Float,
        plungePoolDepth: Float
    ) {
        if (isLibraryLoaded) {
            nativeConfigureWaterfall(
                enabled,
                cliffHeight,
                cliffWidth,
                cliffDepth,
                flowVolume,
                lipSlopeDeg,
                undercut,
                steppedLedges,
                roughness,
                rockChanneling,
                airDragAndMist,
                plungePoolDepth
            )
        }
    }

    fun injectWaterImpulse(normU: Float, normV: Float, dragDx: Float, dragDy: Float, strength: Float) {
        if (isLibraryLoaded) {
            nativeInjectWaterImpulse(normU, normV, dragDx, dragDy, strength)
        }
    }

    fun triggerWaterPulse(pulseType: Int, strength: Float = 1.0f) {
        if (isLibraryLoaded) {
            nativeTriggerWaterPulse(pulseType, strength)
        }
    }

    fun stepAndRenderWaterBitmap(bitmap: Bitmap, dtSeconds: Float, isPlaying: Boolean): FloatArray {
        return if (isLibraryLoaded) {
            nativeStepAndRenderWaterBitmap(bitmap, dtSeconds, isPlaying)
                ?: floatArrayOf(0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f)
        } else {
            floatArrayOf(0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f)
        }
    }

    fun configureWaterSurface(
        enabled: Boolean,
        gridResolution: Int,
        patchSizeM: Float,
        windSpeedMs: Float,
        windDirectionDeg: Float,
        amplitudeScale: Float,
        choppinessLambda: Float,
        smallWaveCutoff: Float,
        timeScale: Float,
        waterDepthM: Float,
        seabedRoughness: Float,
        seabedScale: Float,
        sunAzimuthDeg: Float,
        sunElevationDeg: Float,
        skyTurbidity: Float,
        indexOfRefraction: Float,
        absorptionStrength: Float,
        scatterStrength: Float,
        foamThreshold: Float,
        reflectionStrength: Float,
        debugModeOrdinal: Int,
        cutoutEnabled: Boolean,
        cutoutWidthM: Float,
        cutoutHeightM: Float,
        cutoutDepthM: Float
    ) {
        if (isLibraryLoaded) {
            nativeConfigureWaterSurface(
                enabled,
                gridResolution,
                patchSizeM,
                windSpeedMs,
                windDirectionDeg,
                amplitudeScale,
                choppinessLambda,
                smallWaveCutoff,
                timeScale,
                waterDepthM,
                seabedRoughness,
                seabedScale,
                sunAzimuthDeg,
                sunElevationDeg,
                skyTurbidity,
                indexOfRefraction,
                absorptionStrength,
                scatterStrength,
                foamThreshold,
                reflectionStrength,
                debugModeOrdinal,
                cutoutEnabled,
                cutoutWidthM,
                cutoutHeightM,
                cutoutDepthM
            )
        }
    }

    private external fun nativeConfigureWaterSimulation(
        particleCount: Int,
        scenarioOrdinal: Int,
        smoothingRadius: Float,
        targetDensity: Float,
        pressureMultiplier: Float,
        nearPressureMultiplier: Float,
        viscosityStrength: Float,
        surfaceTension: Float,
        collisionDamping: Float,
        gravityX: Float,
        gravityY: Float,
        gravityZ: Float,
        windX: Float,
        windZ: Float,
        timeScale: Float,
        substeps: Int,
        boundsWidth: Float,
        boundsHeight: Float,
        boundsDepth: Float,
        waveMakerEnabled: Boolean,
        waveFrequency: Float,
        waveAmplitude: Float,
        obstacleEnabled: Boolean,
        obstacleX: Float,
        obstacleY: Float,
        obstacleZ: Float,
        obstacleRadius: Float,
        debugModeOrdinal: Int,
        particleVisualRadius: Float,
        bilateralBlurEnabled: Boolean,
        blurRadiusPx: Int,
        blurSpatialSigma: Float,
        blurDepthSigma: Float,
        waterR: Float,
        waterG: Float,
        waterB: Float,
        absorbR: Float,
        absorbG: Float,
        absorbB: Float,
        absorbStrength: Float,
        indexOfRefraction: Float,
        refractionStrength: Float,
        fresnelPower: Float,
        fresnelBias: Float,
        specularPower: Float,
        reflectionIntensity: Float,
        foamThreshold: Float,
        causticsIntensity: Float,
        sunAzimuthDeg: Float,
        sunElevationDeg: Float,
        showContainerBox: Boolean,
        showFloorGrid: Boolean,
        camYawDeg: Float,
        camPitchDeg: Float,
        camDistance: Float
    )
    private external fun nativeResetWaterSimulation(scenarioOrdinal: Int, particleCount: Int)
    private external fun nativeConfigureWaterfall(
        enabled: Boolean,
        cliffHeight: Float,
        cliffWidth: Float,
        cliffDepth: Float,
        flowVolume: Float,
        lipSlopeDeg: Float,
        undercut: Float,
        steppedLedges: Int,
        roughness: Float,
        rockChanneling: Float,
        airDragAndMist: Float,
        plungePoolDepth: Float
    )
    private external fun nativeInjectWaterImpulse(normU: Float, normV: Float, dragDx: Float, dragDy: Float, strength: Float)
    private external fun nativeTriggerWaterPulse(pulseType: Int, strength: Float)
    private external fun nativeStepAndRenderWaterBitmap(bitmap: Bitmap, dtSeconds: Float, isPlaying: Boolean): FloatArray?
    private external fun nativeConfigureWaterSurface(
        enabled: Boolean,
        gridResolution: Int,
        patchSizeM: Float,
        windSpeedMs: Float,
        windDirectionDeg: Float,
        amplitudeScale: Float,
        choppinessLambda: Float,
        smallWaveCutoff: Float,
        timeScale: Float,
        waterDepthM: Float,
        seabedRoughness: Float,
        seabedScale: Float,
        sunAzimuthDeg: Float,
        sunElevationDeg: Float,
        skyTurbidity: Float,
        indexOfRefraction: Float,
        absorptionStrength: Float,
        scatterStrength: Float,
        foamThreshold: Float,
        reflectionStrength: Float,
        debugModeOrdinal: Int,
        cutoutEnabled: Boolean,
        cutoutWidthM: Float,
        cutoutHeightM: Float,
        cutoutDepthM: Float
    )

    // -----------------------------------------------------------------------
    // Procedural Buildings & Architecture Bridge (gen/buildings)
    // -----------------------------------------------------------------------

    fun getBuildingDiagnostics(): String {
        return if (isLibraryLoaded) {
            nativeGetBuildingDiagnostics()
        } else {
            "BuildingSystem: 12 generators registered (Massing, Residential, Commercial, Industrial, Civic, Interiors, Props, Signage, Damage, LOD)"
        }
    }

    fun renderBuildingBitmap(
        bitmap: Bitmap,
        domainOrdinal: Int,
        subTypeOrdinal: Int,
        rotX: Float,
        rotY: Float,
        zoom: Float,
        shapeOrdinal: Int,
        cornerOrdinal: Int,
        cornerSize: Float,
        stepModeOrdinal: Int,
        stepTiers: Int,
        topTaperRatio: Float,
        stepShrinkFactor: Float,
        stepTerraceRailing: Boolean,
        floorCount: Int,
        floorHeight: Float,
        width: Float,
        depth: Float,
        cornerRadius: Float,
        windowsPerFacade: Int,
        windowTypeOrdinal: Int,
        windowWidth: Float,
        windowHeight: Float,
        windowSillHeight: Float,
        glassTypeOrdinal: Int,
        windowIor: Float,
        wallMaterialOrdinal: Int,
        wallColorArgb: Int,
        doorTypeOrdinal: Int,
        doorWidth: Float,
        doorHeight: Float,
        hasEntranceCanopy: Boolean,
        roofTypeOrdinal: Int,
        roofHeight: Float,
        roofOverhang: Float,
        showRooftopClutter: Boolean,
        balconyTypeOrdinal: Int,
        hasBalconies: Boolean,
        balconyDepth: Float,
        hasSunLouvres: Boolean,
        hasFireEscape: Boolean,
        hasCornice: Boolean,
        hasGroundPillars: Boolean,
        pillarCount: Int,
        hasWindowShutters: Boolean,
        showInside: Boolean,
        interiorDesign: Int,
        detailIntensity: Float,
        detailCount: Int,
        detailFlagA: Boolean,
        detailFlagB: Boolean,
        wireframe: Boolean
    ): FloatArray {
        return if (isLibraryLoaded) {
            nativeRenderBuildingBitmap(
                bitmap, domainOrdinal, subTypeOrdinal, rotX, rotY, zoom,
                shapeOrdinal, cornerOrdinal, cornerSize, stepModeOrdinal, stepTiers,
                topTaperRatio, stepShrinkFactor, stepTerraceRailing,
                floorCount, floorHeight, width, depth, cornerRadius,
                windowsPerFacade, windowTypeOrdinal, windowWidth, windowHeight, windowSillHeight,
                glassTypeOrdinal, windowIor, wallMaterialOrdinal, wallColorArgb,
                doorTypeOrdinal, doorWidth, doorHeight, hasEntranceCanopy,
                roofTypeOrdinal, roofHeight, roofOverhang, showRooftopClutter,
                balconyTypeOrdinal, hasBalconies, balconyDepth,
                hasSunLouvres, hasFireEscape, hasCornice, hasGroundPillars, pillarCount,
                hasWindowShutters, showInside, interiorDesign,
                detailIntensity, detailCount, detailFlagA, detailFlagB, wireframe
            ) ?: floatArrayOf(0f, 0f, 0f, 0f, 0f, 0f)
        } else {
            floatArrayOf(0f, 0f, 0f, 0f, 0f, 0f)
        }
    }

    private external fun nativeGetBuildingDiagnostics(): String

    private external fun nativeRenderBuildingBitmap(
        bitmap: Bitmap,
        domainOrdinal: Int,
        subTypeOrdinal: Int,
        rotX: Float,
        rotY: Float,
        zoom: Float,
        shapeOrdinal: Int,
        cornerOrdinal: Int,
        cornerSize: Float,
        stepModeOrdinal: Int,
        stepTiers: Int,
        topTaperRatio: Float,
        stepShrinkFactor: Float,
        stepTerraceRailing: Boolean,
        floorCount: Int,
        floorHeight: Float,
        width: Float,
        depth: Float,
        cornerRadius: Float,
        windowsPerFacade: Int,
        windowTypeOrdinal: Int,
        windowWidth: Float,
        windowHeight: Float,
        windowSillHeight: Float,
        glassTypeOrdinal: Int,
        windowIor: Float,
        wallMaterialOrdinal: Int,
        wallColorArgb: Int,
        doorTypeOrdinal: Int,
        doorWidth: Float,
        doorHeight: Float,
        hasEntranceCanopy: Boolean,
        roofTypeOrdinal: Int,
        roofHeight: Float,
        roofOverhang: Float,
        showRooftopClutter: Boolean,
        balconyTypeOrdinal: Int,
        hasBalconies: Boolean,
        balconyDepth: Float,
        hasSunLouvres: Boolean,
        hasFireEscape: Boolean,
        hasCornice: Boolean,
        hasGroundPillars: Boolean,
        pillarCount: Int,
        hasWindowShutters: Boolean,
        showInside: Boolean,
        interiorDesign: Int,
        detailIntensity: Float,
        detailCount: Int,
        detailFlagA: Boolean,
        detailFlagB: Boolean,
        wireframe: Boolean
    ): FloatArray?
}





