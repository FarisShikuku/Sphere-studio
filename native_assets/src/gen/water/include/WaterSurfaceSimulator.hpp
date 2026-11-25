#pragma once

#include "WaterSurfaceParams.hpp"
#include <complex>
#include <cstdint>
#include <vector>

namespace sphere::water {

struct RippleDisturbance {
    float x = 0.0f;
    float z = 0.0f;
    float amplitude = 0.0f;
    float startTime = 0.0f;
    float wavelength = 2.6f;
    float decayRate = 0.75f;
};

/**
 * WaterSurfaceSimulator
 *
 * CPU/GPU-mirrored Tessendorf ocean wave synthesis adapted from kentril0/WaterSurfaceRendering.
 * - Generates base Phillips spectrum h0(k) and h0*(-k)
 * - Evolves frequency spectrum h(k, t) with deep-water dispersion omega(k) = sqrt(g * k)
 * - Runs radix-2 Cooley-Tukey 2D Inverse FFT to update 2D Displacement Map and Normal/Jacobian Map
 * - Evaluates Value-Noise underwater terrain heightfield & Preetham analytic atmospheric sky
 */
class WaterSurfaceSimulator {
public:
    WaterSurfaceSimulator() = default;

    void Configure(const WaterSurfaceParams& params);
    void Reset(const WaterSurfaceParams& params);
    void Step(float dtSeconds);

    void InjectDisturbance(float worldX, float worldZ, float strength);
    void TriggerPulse(int32_t pulseType, float strength);

    // Tileable bilinear sampling of generated 2D Displacement Map (dx*lambda, dy, dz*lambda)
    Vec3 SampleDisplacement(float worldX, float worldZ) const;

    // Tileable bilinear sampling of generated 2D Normal Map (nx, ny, nz) and Jacobian foam factor
    void SampleNormalAndJacobian(float worldX, float worldZ, Vec3& outNormal, float& outJacobian) const;

    // Procedural Value-Noise underwater ground heightfield (below y = -waterDepthM)
    float SampleSeabedHeight(float worldX, float worldZ) const;

    // Preetham analytic daytime/sunset sky dome color
    static Vec3 EvaluatePreethamSky(const Vec3& rayDir, const Vec3& sunDir, float turbidity);

    const WaterSurfaceParams& GetParams() const { return m_Params; }
    float GetElapsedTime() const { return m_ElapsedTime; }
    float GetLastFftMs() const { return m_LastFftMs; }
    float GetSignificantWaveHeightM() const { return m_SignificantWaveHeightM; }
    float GetPeakCrestM() const { return m_PeakCrestM; }
    float GetMinWaveY() const { return m_MinWaveY; }
    float GetMaxWaveY() const { return m_MaxWaveY; }

    const std::vector<Vec3>& GetDisplacementMap() const { return m_DisplacementMap; }
    const std::vector<Vec3>& GetNormalMap() const { return m_NormalMap; }
    const std::vector<float>& GetJacobianMap() const { return m_JacobianMap; }

private:
    void RebuildBaseSpectrum();
    void ComputeFftMaps();
    void Fft1D(std::vector<std::complex<float>>& data, bool inverse) const;
    void Fft2D(std::vector<std::complex<float>>& grid, int32_t N, bool inverse) const;

    WaterSurfaceParams m_Params{};
    bool m_SpectrumDirty = true;
    float m_ElapsedTime = 0.0f;
    float m_LastFftMs = 0.0f;
    float m_SignificantWaveHeightM = 0.0f;
    float m_PeakCrestM = 0.0f;
    float m_MinWaveY = -1.5f;
    float m_MaxWaveY = 1.5f;

    int32_t m_AllocatedN = 0;
    std::vector<std::complex<float>> m_H0;       // N * N
    std::vector<std::complex<float>> m_H0Conj;   // N * N (stored at -k)
    std::vector<float> m_Omega;                  // N * N dispersion frequencies

    // Scratch complex 2D FFT grids (N * N each)
    std::vector<std::complex<float>> m_FftGrid0;
    std::vector<std::complex<float>> m_FftGrid1;
    std::vector<std::complex<float>> m_FftGrid2;
    std::vector<std::complex<float>> m_FftGrid3;

    // 2D Output Textures (sampled by WaterSurfaceRenderer)
    std::vector<Vec3> m_DisplacementMap;         // (lambda*dx, height, lambda*dz)
    std::vector<Vec3> m_NormalMap;               // normalized surface normal
    std::vector<float> m_JacobianMap;            // folding Jacobian determinant

    std::vector<RippleDisturbance> m_Ripples;
};

} // namespace sphere::water
