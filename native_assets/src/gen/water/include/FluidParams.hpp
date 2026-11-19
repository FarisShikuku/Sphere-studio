#pragma once

#include <cstdint>
#include "scene/Transform.h"

namespace sphere::water {

enum class WaterSpawnScenario : int32_t {
    DAM_BREAK = 0,
    DUAL_WAVE = 1,
    FOUNTAIN_DROP = 2,
    SLOSH_TANK = 3,
    ZERO_G_ORB = 4,
    WATERFALL_CLIFF = 5
};

enum class FluidDebugViewMode : int32_t {
    COMPOSITE = 0,
    RAW_DEPTH = 1,
    SMOOTHED_DEPTH = 2,
    NORMALS = 3,
    THICKNESS = 4,
    PARTICLE_SPHERES = 5
};

struct alignas(16) FluidPhysicsParams {
    uint32_t particleCount = 1400;
    float smoothingRadius = 0.30f;
    float targetDensity = 10.5f;
    float pressureMultiplier = 44.0f;
    float nearPressureMultiplier = 16.0f;
    float viscosityStrength = 0.14f;
    float surfaceTension = 0.35f;
    float collisionDamping = 0.62f;

    float gravityX = 0.0f;
    float gravityY = -9.81f;
    float gravityZ = 0.0f;

    float windForceX = 0.0f;
    float windForceZ = 0.0f;

    float timeScale = 1.0f;
    uint32_t substeps = 2;

    // Container domain half-extents (width, height, depth in meters)
    float boundsWidth = 2.8f;
    float boundsHeight = 2.2f;
    float boundsDepth = 2.4f;

    // Wave maker
    bool waveMakerEnabled = false;
    float waveFrequency = 1.15f;
    float waveAmplitude = 0.38f;

    // Submerged sphere obstacle
    bool obstacleEnabled = true;
    float obstacleX = 0.25f;
    float obstacleY = 0.42f;
    float obstacleZ = 0.0f;
    float obstacleRadius = 0.38f;

    // Waterfall & Cliff natural constraint parameters
    bool waterfallEnabled = false;
    float waterfallCliffHeight = 1.48f;
    float waterfallCliffWidth = 1.25f;
    float waterfallCliffDepth = 0.92f;
    float waterfallFlowVolume = 1.15f;
    float cliffLipSlopeDeg = 12.0f;
    float cliffUndercut = 0.18f;
    int32_t cliffSteppedLedges = 1;
    float cliffRoughness = 0.38f;
    float cliffRockChanneling = 0.80f;
    float airDragAndMist = 0.28f;
    float plungePoolDepth = 0.32f;

    WaterSpawnScenario scenario = WaterSpawnScenario::DAM_BREAK;
};

struct alignas(16) FluidRenderParams {
    FluidDebugViewMode debugMode = FluidDebugViewMode::COMPOSITE;

    float particleVisualRadius = 0.095f;
    bool bilateralBlurEnabled = true;
    int32_t blurRadiusPx = 5;
    float blurSpatialSigma = 2.8f;
    float blurDepthSigma = 0.085f;

    // Water Optical Properties (Beer-Lambert + Fresnel + Snell)
    float waterColorR = 0.08f;
    float waterColorG = 0.56f;
    float waterColorB = 0.86f;

    float absorptionR = 0.72f;
    float absorptionG = 0.22f;
    float absorptionB = 0.08f;
    float absorptionStrength = 1.85f;

    float indexOfRefraction = 1.333f;
    float refractionStrength = 0.18f;
    float fresnelPower = 4.2f;
    float fresnelBias = 0.02f;
    float specularPower = 180.0f;
    float reflectionIntensity = 0.85f;
    float foamThreshold = 1.65f;
    float causticsIntensity = 0.65f;

    // Directional Sun & Scene
    float sunAzimuthDeg = 42.0f;
    float sunElevationDeg = 52.0f;
    bool showContainerBox = true;
    bool showFloorGrid = true;
};

// std430-aligned GPU Uniform / PushConstant block for Vulkan 1.3 shaders
struct alignas(16) GpuFluidUniforms {
    Mat4 view;
    Mat4 proj;
    Mat4 invProj;
    Vec4 cameraPosAndTime;       // xyz = camPos, w = elapsedTime
    Vec4 boundsHalfAndRadius;    // xyz = half extents, w = particleRadius
    Vec4 gravityAndDt;           // xyz = gravity, w = dt
    Vec4 sphConstants;           // x = smoothingRadius, y = targetDensity, z = pressureMult, w = nearPressureMult
    Vec4 sphForces;              // x = viscosity, y = surfaceTension, z = damping, w = particleCount
    Vec4 windAndObstacle;        // x = windX, y = windZ, z = waveOffset, w = obstacleEnabled
    Vec4 obstacleSphere;         // xyz = center, w = radius
    Vec4 waterColorAndRefract;   // rgb = waterColor, w = refractionStrength
    Vec4 absorptionAndStrength;  // rgb = absorptionCoeff, w = absorptionStrength
    Vec4 opticsParams;           // x = fresnelPower, y = fresnelBias, z = specularPower, w = reflectionIntensity
    Vec4 sunDirAndFoam;          // xyz = sunDir, w = foamThreshold
    uint32_t screenWidth = 360;
    uint32_t screenHeight = 240;
    int32_t debugMode = 0;
    int32_t blurRadius = 5;
    float blurSpatialSigma = 2.8f;
    float blurDepthSigma = 0.085f;
    float causticsIntensity = 0.65f;
    float ior = 1.333f;
};

struct WaterTelemetry {
    uint32_t activeParticles = 0;
    uint32_t occupiedCells = 0;
    uint32_t hashTableSize = 0;
    float physicsStepMs = 0.0f;
    float renderPassMs = 0.0f;
    float avgDensity = 0.0f;
    float maxSpeed = 0.0f;
    float kineticEnergy = 0.0f;
    bool vulkanGpuActive = false;
};

} // namespace sphere::water
