#pragma once

#include <cstdint>
#include "scene/Transform.h"

namespace sphere::water {

/**
 * Debug & inspection modes matching kentril0/WaterSurfaceRendering's Vulkan pipeline.
 */
enum class WaterSurfaceDebugMode : int32_t {
    LIT_OPTICS = 0,
    DISPLACEMENT_MAP = 1,
    NORMAL_MAP = 2,
    SEABED_DEPTH = 3,
    FRESNEL_AND_FOAM = 4,
    WIREFRAME_GRID = 5
};

/**
 * WaterSurfaceParams
 *
 * Full parameter set for Tessendorf Phillips-FFT choppy waves, Preetham sky atmosphere,
 * Baboud & Decoret refraction/absorption optics, and value-noise underwater terrain.
 */
struct alignas(16) WaterSurfaceParams {
    bool enabled = false;
    int32_t gridResolution = 64;     // Power of 2: 32, 64, or 128
    float patchSizeM = 64.0f;        // Physical repeating domain size L (meters)
    float windSpeedMs = 14.0f;       // Phillips wind speed V (m/s)
    float windDirectionDeg = 35.0f;  // Wind direction angle (degrees)
    float amplitudeScale = 0.45f;    // Phillips spectrum scale factor
    float choppinessLambda = 1.25f;  // Horizontal displacement pinch factor lambda
    float smallWaveCutoff = 0.08f;   // Capillary wave damping l (meters)
    float timeScale = 1.0f;

    // Underwater Value-Noise Terrain & Basin Depth
    float waterDepthM = 6.5f;        // Mean depth above underwater ground plane (meters)
    float seabedRoughness = 1.8f;    // Amplitude of value-noise seabed terrain (meters)
    float seabedScale = 0.14f;       // Spatial frequency of value-noise terrain

    // Preetham Sky & Baboud-Decoret Water Optics
    float sunAzimuthDeg = 42.0f;
    float sunElevationDeg = 34.0f;
    float skyTurbidity = 2.6f;
    float indexOfRefraction = 1.333f;
    float absorptionStrength = 0.55f;
    float scatterStrength = 0.48f;
    float foamThreshold = 0.35f;
    float reflectionStrength = 1.0f;

    // Compact Ocean Cutaway Block (e.g. 10m x 10m x 10m cross-section block)
    bool cutoutEnabled = false;
    float cutoutWidthM = 10.0f;      // X span in meters
    float cutoutHeightM = 10.0f;     // Vertical block height Y in meters
    float cutoutDepthM = 10.0f;      // Z span in meters

    WaterSurfaceDebugMode debugMode = WaterSurfaceDebugMode::LIT_OPTICS;
};

/**
 * std430-aligned Vulkan UBO matching shaders/graphics/water_surface.vert/.frag
 */
struct alignas(16) GpuWaterSurfaceUniforms {
    Mat4 view;
    Mat4 proj;
    Mat4 invViewProj;
    Vec4 cameraPosAndTime;       // xyz = cameraPos, w = elapsedTime
    Vec4 sunDirAndTurbidity;     // xyz = sunDir, w = Preetham turbidity
    Vec4 patchAndWaveParams;     // x = patchSizeM, y = choppinessLambda, z = waterDepthM, w = seabedRoughness
    Vec4 opticsParams;           // x = ior, y = absorptionStrength, z = scatterStrength, w = foamThreshold
    Vec4 absorptionCoeffAndRefl; // rgb = sigmaRGB, w = reflectionStrength
    Vec4 cutoutBounds;           // x = enabled (0/1), y = widthM, z = heightM, w = depthM
    uint32_t gridResolution = 64;
    int32_t debugMode = 0;
    float seabedScale = 0.14f;
    float pad0 = 0.0f;
};

struct WaterSurfaceTelemetry {
    uint32_t gridVertices = 4096;
    uint32_t fftResolution = 64;
    uint32_t textureTexels = 8192;
    float fftComputeMs = 0.0f;
    float renderMs = 0.0f;
    float significantWaveHeightM = 0.0f;
    float peakCrestM = 0.0f;
    float windSpeedMs = 14.0f;
};

} // namespace sphere::water
