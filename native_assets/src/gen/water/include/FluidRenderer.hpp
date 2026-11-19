#pragma once

#include "FluidParams.hpp"
#include "FluidSimulator.hpp"
#include "OrbitalCamera.hpp"
#include <vector>
#include <cstdint>

namespace sphere::water {

/**
 * FluidRenderer
 *
 * Screen-Space Fluid Rendering (SSFR) pipeline ported from talvinckb/OpenGL-Water-Simulation.
 *
 * Multi-pass pipeline:
 *  Pass 1: Scene Backdrop + Checkerboard Caustic Pool Floor & Container Walls
 *  Pass 2: Particle Impostor Sphere Linear Depth & Thickness Buffers
 *  Pass 3: Separable Depth-Aware Bilateral Gaussian Filter (horizontal + vertical)
 *  Pass 4: Screen-Space Normal Reconstruction + Optical Composite Shading:
 *          - Beer-Lambert RGB light absorption through thickness buffer
 *          - Snell's law refraction distortion of tank floor + caustics
 *          - Schlick Fresnel reflection of procedural sky dome
 *          - GGX sun specular highlights + velocity/density surface foam
 *          - Debug visualization modes (Composite, RawDepth, SmoothDepth, Normals, Thickness, Spheres)
 */
class FluidRenderer {
public:
    FluidRenderer() = default;
    ~FluidRenderer() = default;

    void Render(
        uint32_t* outPixelsRGBA8888,
        int32_t stridePixels,
        uint32_t width,
        uint32_t height,
        const FluidSimulator& simulator,
        const OrbitalCamera& camera,
        const FluidRenderParams& renderParams
    );

    float GetLastRenderMs() const { return m_LastRenderMs; }

private:
    void ResizeBuffers(uint32_t width, uint32_t height);

    void RenderBackgroundAndScene(
        uint32_t width,
        uint32_t height,
        const FluidSimulator& simulator,
        const OrbitalCamera& camera,
        const FluidRenderParams& renderParams
    );

    void RenderParticleDepthAndThickness(
        uint32_t width,
        uint32_t height,
        const FluidSimulator& simulator,
        const OrbitalCamera& camera,
        const FluidRenderParams& renderParams
    );

    void ApplyBilateralBlur(
        uint32_t width,
        uint32_t height,
        const FluidRenderParams& renderParams
    );

    void CompositeFluidSurface(
        uint32_t* outPixelsRGBA8888,
        int32_t stridePixels,
        uint32_t width,
        uint32_t height,
        const FluidSimulator& simulator,
        const OrbitalCamera& camera,
        const FluidRenderParams& renderParams
    );

    void RenderContainerWireframe(
        uint32_t* outPixelsRGBA8888,
        int32_t stridePixels,
        uint32_t width,
        uint32_t height,
        const FluidSimulator& simulator,
        const OrbitalCamera& camera,
        const FluidRenderParams& renderParams
    );

    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
    float m_LastRenderMs = 0.0f;

    std::vector<float> m_SceneColorRGB;     // width * height * 3 (linear RGB backdrop + floor + obstacle)
    std::vector<float> m_SceneDepth;        // width * height (view-space positive depth)
    std::vector<float> m_RawDepth;          // width * height (particle sphere front depth)
    std::vector<float> m_BlurTempDepth;     // width * height (horizontal pass scratch)
    std::vector<float> m_SmoothedDepth;     // width * height (final bilateral smoothed depth)
    std::vector<float> m_Thickness;         // width * height (additive fluid optical thickness)
    std::vector<float> m_SpeedBuffer;       // width * height (weighted particle speed for foam)
    std::vector<float> m_SphereShadeBuffer; // width * height (for PARTICLE_SPHERES debug mode)
};

} // namespace sphere::water
