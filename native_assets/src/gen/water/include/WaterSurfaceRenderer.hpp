#pragma once

#include "OrbitalCamera.hpp"
#include "WaterSurfaceParams.hpp"
#include "WaterSurfaceSimulator.hpp"
#include <cstdint>

namespace sphere::water {

/**
 * WaterSurfaceRenderer
 *
 * Vulkan-mirrored displaced-grid & ray-heightfield water surface renderer implementing
 * kentril0/WaterSurfaceRendering optics:
 *  - Preetham atmospheric sky dome & directional sun reflection
 *  - Bilinear-interpolated 2D FFT Displacement Map + Normal/Jacobian Map
 *  - Snell's law refraction to Value-Noise underwater seabed terrain
 *  - Beer-Lambert spectral absorption, animated seabed caustics & crest subsurface scattering
 *  - Schlick Fresnel mixing + Jacobian breaking-wave foam + 6 debug visualization views
 */
class WaterSurfaceRenderer {
public:
    WaterSurfaceRenderer() = default;

    void Render(
        uint32_t* outPixelsRGBA8888,
        int32_t stridePixels,
        uint32_t width,
        uint32_t height,
        const WaterSurfaceSimulator& simulator,
        const OrbitalCamera& camera
    );

    float GetLastRenderMs() const { return m_LastRenderMs; }

private:
    float RaymarchWaterHeightfield(
        const Vec3& rayOrigin,
        const Vec3& rayDir,
        const WaterSurfaceSimulator& simulator,
        float minWaveY,
        float maxWaveY,
        float maxDist,
        Vec3& outHitPos
    ) const;

    float RaymarchSeabed(
        const Vec3& rayOrigin,
        const Vec3& rayDir,
        const WaterSurfaceSimulator& simulator,
        Vec3& outHitPos
    ) const;

    Vec3 ShadeCutawayPixel(
        const Vec3& rayOrigin,
        const Vec3& rayDir,
        const WaterSurfaceSimulator& simulator,
        const Vec3& sunDir,
        const Vec3& skyCol,
        float minWaveY,
        float maxWaveY
    ) const;

    void RenderDisplacedGridOverlay(
        uint32_t* outPixelsRGBA8888,
        int32_t stridePixels,
        uint32_t width,
        uint32_t height,
        const WaterSurfaceSimulator& simulator,
        const OrbitalCamera& camera
    ) const;

    float m_LastRenderMs = 0.0f;
};

} // namespace sphere::water
