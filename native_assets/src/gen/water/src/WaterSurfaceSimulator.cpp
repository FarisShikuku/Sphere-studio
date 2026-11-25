#include "WaterSurfaceSimulator.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>

namespace sphere::water {

namespace {
constexpr float kPi = 3.14159265358979323846f;
constexpr float kTwoPi = 6.2831853071795864769f;
constexpr float kGravity = 9.81f;

inline int32_t ClampPow2Resolution(int32_t n) {
    if (n <= 32) return 32;
    if (n <= 64) return 64;
    return 128;
}

inline float Hash2D(int32_t ix, int32_t iz) {
    uint32_t h = static_cast<uint32_t>(ix) * 374761393u + static_cast<uint32_t>(iz) * 668265263u;
    h = (h ^ (h >> 13u)) * 1274126177u;
    h ^= (h >> 16u);
    return static_cast<float>(h & 0x00FFFFFFu) / 16777216.0f;
}

inline float FiniteOr(float v, float fallback) {
    return std::isfinite(v) ? v : fallback;
}

inline float ValueNoise2D(float x, float z) {
    if (!std::isfinite(x) || !std::isfinite(z)) return 0.0f;
    float cx = std::clamp(x, -1.0e6f, 1.0e6f);
    float cz = std::clamp(z, -1.0e6f, 1.0e6f);
    int32_t ix = static_cast<int32_t>(std::floor(cx));
    int32_t iz = static_cast<int32_t>(std::floor(cz));
    float fx = cx - static_cast<float>(ix);
    float fz = cz - static_cast<float>(iz);
    float ux = fx * fx * (3.0f - 2.0f * fx);
    float uz = fz * fz * (3.0f - 2.0f * fz);

    float v00 = Hash2D(ix, iz);
    float v10 = Hash2D(ix + 1, iz);
    float v01 = Hash2D(ix, iz + 1);
    float v11 = Hash2D(ix + 1, iz + 1);

    float a = v00 + (v10 - v00) * ux;
    float b = v01 + (v11 - v01) * ux;
    return (a + (b - a) * uz) * 2.0f - 1.0f;
}

inline float DeterministicUniform(uint32_t& state) {
    state ^= state << 13u;
    state ^= state >> 17u;
    state ^= state << 5u;
    return std::max(1e-6f, static_cast<float>(state & 0x00FFFFFFu) / 16777216.0f);
}

inline std::complex<float> BoxMullerComplex(uint32_t& rng) {
    float u1 = DeterministicUniform(rng);
    float u2 = DeterministicUniform(rng);
    float mag = std::sqrt(-2.0f * std::log(u1));
    float ang = kTwoPi * u2;
    return std::complex<float>(mag * std::cos(ang), mag * std::sin(ang));
}
} // namespace

void WaterSurfaceSimulator::Configure(const WaterSurfaceParams& params) {
    WaterSurfaceParams sanitized = params;
    sanitized.gridResolution = ClampPow2Resolution(params.gridResolution);
    sanitized.patchSizeM = std::clamp(FiniteOr(params.patchSizeM, 48.0f), 8.0f, 256.0f);
    sanitized.windSpeedMs = std::clamp(FiniteOr(params.windSpeedMs, 14.0f), 1.0f, 36.0f);
    sanitized.windDirectionDeg = std::clamp(FiniteOr(params.windDirectionDeg, 35.0f), -360.0f, 360.0f);
    sanitized.amplitudeScale = std::clamp(FiniteOr(params.amplitudeScale, 0.45f), 0.05f, 1.80f);
    sanitized.choppinessLambda = std::clamp(FiniteOr(params.choppinessLambda, 1.25f), 0.0f, 2.50f);
    sanitized.smallWaveCutoff = std::clamp(FiniteOr(params.smallWaveCutoff, 0.08f), 0.01f, 0.45f);
    sanitized.timeScale = std::clamp(FiniteOr(params.timeScale, 1.0f), 0.05f, 3.0f);
    sanitized.waterDepthM = std::clamp(FiniteOr(params.waterDepthM, 6.5f), 1.0f, 24.0f);
    sanitized.seabedRoughness = std::clamp(FiniteOr(params.seabedRoughness, 1.8f), 0.0f, 5.5f);
    sanitized.seabedScale = std::clamp(FiniteOr(params.seabedScale, 0.14f), 0.02f, 0.45f);
    sanitized.sunAzimuthDeg = std::clamp(FiniteOr(params.sunAzimuthDeg, 42.0f), -360.0f, 360.0f);
    sanitized.sunElevationDeg = std::clamp(FiniteOr(params.sunElevationDeg, 34.0f), 1.0f, 89.0f);
    sanitized.skyTurbidity = std::clamp(FiniteOr(params.skyTurbidity, 2.6f), 1.8f, 8.5f);
    sanitized.indexOfRefraction = std::clamp(FiniteOr(params.indexOfRefraction, 1.333f), 1.0f, 1.75f);
    sanitized.absorptionStrength = std::clamp(FiniteOr(params.absorptionStrength, 0.55f), 0.05f, 3.0f);
    sanitized.scatterStrength = std::clamp(FiniteOr(params.scatterStrength, 0.48f), 0.0f, 1.8f);
    sanitized.foamThreshold = std::clamp(FiniteOr(params.foamThreshold, 0.35f), -0.5f, 1.2f);
    sanitized.reflectionStrength = std::clamp(FiniteOr(params.reflectionStrength, 1.0f), 0.0f, 1.6f);
    sanitized.cutoutWidthM = std::clamp(FiniteOr(params.cutoutWidthM, 10.0f), 2.0f, 64.0f);
    sanitized.cutoutHeightM = std::clamp(FiniteOr(params.cutoutHeightM, 10.0f), 2.0f, 64.0f);
    sanitized.cutoutDepthM = std::clamp(FiniteOr(params.cutoutDepthM, 10.0f), 2.0f, 64.0f);

    bool needsSpectrumRebuild = m_SpectrumDirty ||
        (sanitized.gridResolution != m_Params.gridResolution) ||
        (std::abs(sanitized.patchSizeM - m_Params.patchSizeM) > 1e-3f) ||
        (std::abs(sanitized.windSpeedMs - m_Params.windSpeedMs) > 1e-3f) ||
        (std::abs(sanitized.windDirectionDeg - m_Params.windDirectionDeg) > 1e-2f) ||
        (std::abs(sanitized.amplitudeScale - m_Params.amplitudeScale) > 1e-4f) ||
        (std::abs(sanitized.smallWaveCutoff - m_Params.smallWaveCutoff) > 1e-4f);

    m_Params = sanitized;
    if (needsSpectrumRebuild) {
        RebuildBaseSpectrum();
        ComputeFftMaps();
    }
}

void WaterSurfaceSimulator::Reset(const WaterSurfaceParams& params) {
    m_ElapsedTime = 0.0f;
    m_Ripples.clear();
    m_SpectrumDirty = true;
    Configure(params);
}

void WaterSurfaceSimulator::RebuildBaseSpectrum() {
    const int32_t N = m_Params.gridResolution;
    const size_t total = static_cast<size_t>(N) * static_cast<size_t>(N);
    if (m_AllocatedN != N) {
        m_AllocatedN = N;
        m_H0.resize(total);
        m_H0Conj.resize(total);
        m_Omega.resize(total);
        m_FftGrid0.resize(total);
        m_FftGrid1.resize(total);
        m_FftGrid2.resize(total);
        m_FftGrid3.resize(total);
        m_DisplacementMap.resize(total);
        m_NormalMap.resize(total);
        m_JacobianMap.resize(total);
    }

    const float L = m_Params.patchSizeM;
    const float dk = kTwoPi / L;
    const float Lw = (m_Params.windSpeedMs * m_Params.windSpeedMs) / kGravity;
    const float Lw2 = std::max(0.1f, Lw * Lw);
    const float l2 = m_Params.smallWaveCutoff * m_Params.smallWaveCutoff;
    // Phillips constant scaled with spectral measure (dk^2) so RMS height is grid/patch invariant
    const float A = 0.00055f * m_Params.amplitudeScale;
    const float windRad = m_Params.windDirectionDeg * (kPi / 180.0f);
    const float wx = std::cos(windRad);
    const float wz = std::sin(windRad);

    auto phillipsSpectrum = [&](float kx, float kz) -> float {
        float k2 = kx * kx + kz * kz;
        if (k2 < 1e-8f) return 0.0f;
        float k4 = k2 * k2;
        float kLen = std::sqrt(k2);
        float kDotW = (kx / kLen) * wx + (kz / kLen) * wz;
        float ph = A * (std::exp(-1.0f / (k2 * Lw2)) / k4) * (kDotW * kDotW);
        // Suppress waves travelling opposite the wind gently & damp tiny capillary ripples
        if (kDotW < 0.0f) ph *= 0.18f;
        ph *= std::exp(-k2 * l2);
        return ph;
    };

    uint32_t rng = 0x243F6A88u;
    std::vector<std::complex<float>> gauss(total);
    for (size_t i = 0; i < total; ++i) {
        gauss[i] = BoxMullerComplex(rng);
    }

    for (int32_t iz = 0; iz < N; ++iz) {
        int32_t nz = (iz < N / 2) ? iz : (iz - N);
        float kz = static_cast<float>(nz) * dk;
        for (int32_t ix = 0; ix < N; ++ix) {
            int32_t nx = (ix < N / 2) ? ix : (ix - N);
            float kx = static_cast<float>(nx) * dk;
            size_t idx = static_cast<size_t>(iz * N + ix);

            float kLen = std::sqrt(kx * kx + kz * kz);
            m_Omega[idx] = std::sqrt(kGravity * kLen);

            float ph = phillipsSpectrum(kx, kz);
            float scale = std::sqrt(ph * 0.5f) * dk;
            m_H0[idx] = gauss[idx] * scale;
        }
    }

    // Store h0*(-k) directly for fast Hermitian time evolution
    for (int32_t iz = 0; iz < N; ++iz) {
        int32_t izNeg = (N - iz) & (N - 1);
        for (int32_t ix = 0; ix < N; ++ix) {
            int32_t ixNeg = (N - ix) & (N - 1);
            size_t idx = static_cast<size_t>(iz * N + ix);
            size_t idxNeg = static_cast<size_t>(izNeg * N + ixNeg);
            m_H0Conj[idx] = std::conj(m_H0[idxNeg]);
        }
    }

    m_SpectrumDirty = false;
}

void WaterSurfaceSimulator::Fft1D(std::vector<std::complex<float>>& data, bool inverse) const {
    const size_t N = data.size();
    // Bit-reversal permutation
    size_t j = 0;
    for (size_t i = 1; i < N; ++i) {
        size_t bit = N >> 1u;
        for (; j & bit; bit >>= 1u) {
            j ^= bit;
        }
        j ^= bit;
        if (i < j) {
            std::swap(data[i], data[j]);
        }
    }

    const float sign = inverse ? 1.0f : -1.0f;
    for (size_t len = 2; len <= N; len <<= 1u) {
        float ang = sign * kTwoPi / static_cast<float>(len);
        std::complex<float> wLen(std::cos(ang), std::sin(ang));
        for (size_t i = 0; i < N; i += len) {
            std::complex<float> w(1.0f, 0.0f);
            size_t half = len >> 1u;
            for (size_t k = 0; k < half; ++k) {
                std::complex<float> u = data[i + k];
                std::complex<float> v = data[i + k + half] * w;
                data[i + k] = u + v;
                data[i + k + half] = u - v;
                w *= wLen;
            }
        }
    }
}

void WaterSurfaceSimulator::Fft2D(std::vector<std::complex<float>>& grid, int32_t N, bool inverse) const {
    std::vector<std::complex<float>> line(static_cast<size_t>(N));
    // Rows
    for (int32_t r = 0; r < N; ++r) {
        size_t rowOff = static_cast<size_t>(r * N);
        for (int32_t c = 0; c < N; ++c) {
            line[static_cast<size_t>(c)] = grid[rowOff + static_cast<size_t>(c)];
        }
        Fft1D(line, inverse);
        for (int32_t c = 0; c < N; ++c) {
            grid[rowOff + static_cast<size_t>(c)] = line[static_cast<size_t>(c)];
        }
    }
    // Columns
    for (int32_t c = 0; c < N; ++c) {
        for (int32_t r = 0; r < N; ++r) {
            line[static_cast<size_t>(r)] = grid[static_cast<size_t>(r * N + c)];
        }
        Fft1D(line, inverse);
        for (int32_t r = 0; r < N; ++r) {
            grid[static_cast<size_t>(r * N + c)] = line[static_cast<size_t>(r)];
        }
    }
}

void WaterSurfaceSimulator::ComputeFftMaps() {
    auto t0 = std::chrono::steady_clock::now();

    const int32_t N = m_Params.gridResolution;
    const float L = m_Params.patchSizeM;
    const float dk = kTwoPi / L;
    const float t = m_ElapsedTime;
    const float lambda = m_Params.choppinessLambda;

    // Pack pairs of Hermitian spectra into complex FFT grids: F = A + i * B -> IFFT(F) = a + i * b
    for (int32_t iz = 0; iz < N; ++iz) {
        int32_t nz = (iz < N / 2) ? iz : (iz - N);
        float kz = static_cast<float>(nz) * dk;
        for (int32_t ix = 0; ix < N; ++ix) {
            int32_t nx = (ix < N / 2) ? ix : (ix - N);
            float kx = static_cast<float>(nx) * dk;
            size_t idx = static_cast<size_t>(iz * N + ix);

            float wt = m_Omega[idx] * t;
            std::complex<float> ePos(std::cos(wt), std::sin(wt));
            std::complex<float> eNeg(ePos.real(), -ePos.imag());
            std::complex<float> hkt = m_H0[idx] * ePos + m_H0Conj[idx] * eNeg;

            float kLen = std::sqrt(kx * kx + kz * kz);
            float invK = (kLen > 1e-6f) ? (1.0f / kLen) : 0.0f;

            // D_x = -i * (kx/k) * h, D_z = -i * (kz/k) * h
            std::complex<float> minusI(0.0f, -1.0f);
            std::complex<float> dxK = minusI * (kx * invK) * hkt;
            std::complex<float> dzK = minusI * (kz * invK) * hkt;

            // Slopes S_x = i * kx * h, S_z = i * kz * h
            std::complex<float> plusI(0.0f, 1.0f);
            std::complex<float> sxK = plusI * kx * hkt;
            std::complex<float> szK = plusI * kz * hkt;

            // Displacement derivatives for Jacobian: dDx/dx, dDz/dz, dDx/dz
            std::complex<float> jxxK = (kx * kx * invK) * hkt;
            std::complex<float> jzzK = (kz * kz * invK) * hkt;
            std::complex<float> jxzK = (kx * kz * invK) * hkt;

            m_FftGrid0[idx] = hkt + plusI * dxK;
            m_FftGrid1[idx] = dzK + plusI * sxK;
            m_FftGrid2[idx] = szK + plusI * jxxK;
            m_FftGrid3[idx] = jzzK + plusI * jxzK;
        }
    }

    Fft2D(m_FftGrid0, N, true);
    Fft2D(m_FftGrid1, N, true);
    Fft2D(m_FftGrid2, N, true);
    Fft2D(m_FftGrid3, N, true);

    double sumSqH = 0.0;
    float minY = 1e6f;
    float maxY = -1e6f;
    const size_t total = static_cast<size_t>(N) * static_cast<size_t>(N);

    for (size_t idx = 0; idx < total; ++idx) {
        float hy = m_FftGrid0[idx].real();
        float dx = m_FftGrid0[idx].imag();
        float dz = m_FftGrid1[idx].real();
        float sx = m_FftGrid1[idx].imag();
        float sz = m_FftGrid2[idx].real();
        float jxx = m_FftGrid2[idx].imag();
        float jzz = m_FftGrid3[idx].real();
        float jxz = m_FftGrid3[idx].imag();

        m_DisplacementMap[idx] = Vec3(dx * lambda, hy, dz * lambda);
        m_NormalMap[idx] = Vec3(-sx, 1.0f, -sz).Normalized();

        float j = (1.0f + lambda * jxx) * (1.0f + lambda * jzz) - (lambda * jxz) * (lambda * jxz);
        m_JacobianMap[idx] = j;

        sumSqH += static_cast<double>(hy) * static_cast<double>(hy);
        minY = std::min(minY, hy);
        maxY = std::max(maxY, hy);
    }

    float rms = std::sqrt(static_cast<float>(sumSqH / static_cast<double>(std::max<size_t>(1, total))));
    m_SignificantWaveHeightM = rms * 4.0f;
    m_PeakCrestM = maxY;
    m_MinWaveY = minY - 0.15f;
    m_MaxWaveY = maxY + 0.15f;

    auto t1 = std::chrono::steady_clock::now();
    m_LastFftMs = std::chrono::duration<float, std::milli>(t1 - t0).count();
}

void WaterSurfaceSimulator::Step(float dtSeconds) {
    float dt = std::clamp(dtSeconds, 0.001f, 0.05f) * m_Params.timeScale;
    m_ElapsedTime += dt;

    // Prune expired interactive wave ripples
    m_Ripples.erase(
        std::remove_if(m_Ripples.begin(), m_Ripples.end(), [&](const RippleDisturbance& r) {
            return (m_ElapsedTime - r.startTime) > 6.5f;
        }),
        m_Ripples.end()
    );

    if (m_SpectrumDirty || m_AllocatedN != m_Params.gridResolution) {
        RebuildBaseSpectrum();
    }
    ComputeFftMaps();
}

void WaterSurfaceSimulator::InjectDisturbance(float worldX, float worldZ, float strength) {
    if (m_Ripples.size() >= 12) {
        m_Ripples.erase(m_Ripples.begin());
    }
    m_Ripples.push_back(RippleDisturbance{
        worldX,
        worldZ,
        std::clamp(strength * 0.24f, 0.08f, 0.95f),
        m_ElapsedTime,
        2.8f,
        0.58f
    });
}

void WaterSurfaceSimulator::TriggerPulse(int32_t pulseType, float strength) {
    if (pulseType == 0) {
        // Plunge impact splash ring at patch center
        InjectDisturbance(0.0f, 0.0f, strength * 1.4f);
    } else if (pulseType == 1) {
        // Dual counter-propagating swell train
        InjectDisturbance(-3.5f, 1.8f, strength * 1.1f);
        InjectDisturbance(3.5f, -1.8f, strength * 1.1f);
    } else {
        // Wind gust cross-chop wave packet
        InjectDisturbance(-5.0f, -2.5f, strength * 1.3f);
        InjectDisturbance(2.0f, 3.5f, strength * 0.9f);
    }
}

Vec3 WaterSurfaceSimulator::SampleDisplacement(float worldX, float worldZ) const {
    if (m_DisplacementMap.empty() || m_AllocatedN <= 0 || !std::isfinite(worldX) || !std::isfinite(worldZ)) {
        return Vec3(0.0f, 0.0f, 0.0f);
    }

    const int32_t N = m_AllocatedN;
    const float invL = 1.0f / std::max(1.0f, m_Params.patchSizeM);
    float u = (worldX * invL);
    float v = (worldZ * invL);
    u = u - std::floor(u);
    v = v - std::floor(v);

    float gx = u * static_cast<float>(N);
    float gz = v * static_cast<float>(N);
    int32_t x0 = static_cast<int32_t>(gx) & (N - 1);
    int32_t z0 = static_cast<int32_t>(gz) & (N - 1);
    int32_t x1 = (x0 + 1) & (N - 1);
    int32_t z1 = (z0 + 1) & (N - 1);
    float fx = gx - std::floor(gx);
    float fz = gz - std::floor(gz);

    const Vec3& d00 = m_DisplacementMap[static_cast<size_t>(z0 * N + x0)];
    const Vec3& d10 = m_DisplacementMap[static_cast<size_t>(z0 * N + x1)];
    const Vec3& d01 = m_DisplacementMap[static_cast<size_t>(z1 * N + x0)];
    const Vec3& d11 = m_DisplacementMap[static_cast<size_t>(z1 * N + x1)];

    Vec3 res = Vec3::Lerp(Vec3::Lerp(d00, d10, fx), Vec3::Lerp(d01, d11, fx), fz);

    // Add interactive radial ripples from user touch / splash triggers
    for (const auto& rip : m_Ripples) {
        float age = m_ElapsedTime - rip.startTime;
        if (age < 0.0f) continue;
        float dx = worldX - rip.x;
        float dz = worldZ - rip.z;
        float r = std::sqrt(dx * dx + dz * dz);
        float front = age * 4.2f;
        float band = std::exp(-0.18f * (r - front) * (r - front));
        float env = std::exp(-rip.decayRate * age) * band / (1.0f + r * 0.22f);
        res.y += std::cos((r - front) * (kTwoPi / rip.wavelength)) * rip.amplitude * env;
    }

    return res;
}

void WaterSurfaceSimulator::SampleNormalAndJacobian(
    float worldX,
    float worldZ,
    Vec3& outNormal,
    float& outJacobian
) const {
    if (m_NormalMap.empty() || m_AllocatedN <= 0 || !std::isfinite(worldX) || !std::isfinite(worldZ)) {
        outNormal = Vec3(0.0f, 1.0f, 0.0f);
        outJacobian = 1.0f;
        return;
    }

    const int32_t N = m_AllocatedN;
    const float invL = 1.0f / std::max(1.0f, m_Params.patchSizeM);
    float u = (worldX * invL);
    float v = (worldZ * invL);
    u = u - std::floor(u);
    v = v - std::floor(v);

    float gx = u * static_cast<float>(N);
    float gz = v * static_cast<float>(N);
    int32_t x0 = static_cast<int32_t>(gx) & (N - 1);
    int32_t z0 = static_cast<int32_t>(gz) & (N - 1);
    int32_t x1 = (x0 + 1) & (N - 1);
    int32_t z1 = (z0 + 1) & (N - 1);
    float fx = gx - std::floor(gx);
    float fz = gz - std::floor(gz);

    const Vec3& n00 = m_NormalMap[static_cast<size_t>(z0 * N + x0)];
    const Vec3& n10 = m_NormalMap[static_cast<size_t>(z0 * N + x1)];
    const Vec3& n01 = m_NormalMap[static_cast<size_t>(z1 * N + x0)];
    const Vec3& n11 = m_NormalMap[static_cast<size_t>(z1 * N + x1)];
    Vec3 n = Vec3::Lerp(Vec3::Lerp(n00, n10, fx), Vec3::Lerp(n01, n11, fx), fz);

    float j00 = m_JacobianMap[static_cast<size_t>(z0 * N + x0)];
    float j10 = m_JacobianMap[static_cast<size_t>(z0 * N + x1)];
    float j01 = m_JacobianMap[static_cast<size_t>(z1 * N + x0)];
    float j11 = m_JacobianMap[static_cast<size_t>(z1 * N + x1)];
    float jTop = j00 + (j10 - j00) * fx;
    float jBot = j01 + (j11 - j01) * fx;
    outJacobian = jTop + (jBot - jTop) * fz;

    for (const auto& rip : m_Ripples) {
        float age = m_ElapsedTime - rip.startTime;
        if (age < 0.0f) continue;
        float dx = worldX - rip.x;
        float dz = worldZ - rip.z;
        float r = std::sqrt(dx * dx + dz * dz);
        if (r < 0.05f) continue;
        float front = age * 4.2f;
        float band = std::exp(-0.18f * (r - front) * (r - front));
        float env = std::exp(-rip.decayRate * age) * band / (1.0f + r * 0.22f);
        float kRip = kTwoPi / rip.wavelength;
        float dHdr = -std::sin((r - front) * kRip) * kRip * rip.amplitude * env;
        n.x -= dHdr * (dx / r);
        n.z -= dHdr * (dz / r);
    }

    outNormal = n.Normalized();
}

float WaterSurfaceSimulator::SampleSeabedHeight(float worldX, float worldZ) const {
    float s = m_Params.seabedScale;
    float fbm = ValueNoise2D(worldX * s, worldZ * s) * 0.62f +
                ValueNoise2D(worldX * s * 2.15f + 13.7f, worldZ * s * 2.15f - 7.3f) * 0.28f +
                ValueNoise2D(worldX * s * 4.60f - 4.1f, worldZ * s * 4.60f + 19.2f) * 0.10f;
    // Gently shallow near positive X/Z shoal and deeper toward open ocean
    float shoalBias = std::sin(worldX * 0.08f) * std::cos(worldZ * 0.08f) * 0.35f;
    float y = -m_Params.waterDepthM + (fbm + shoalBias) * m_Params.seabedRoughness;
    return std::min(y, -0.45f);
}

Vec3 WaterSurfaceSimulator::EvaluatePreethamSky(
    const Vec3& rayDir,
    const Vec3& sunDir,
    float turbidity
) {
    float cosTheta = std::clamp(rayDir.y, 0.02f, 1.0f);
    float cosGamma = std::clamp(Vec3::Dot(rayDir, sunDir), -1.0f, 1.0f);
    float gamma = std::acos(cosGamma);
    float sunZenith = std::acos(std::clamp(sunDir.y, 0.02f, 1.0f));
    float t = std::clamp(turbidity, 1.8f, 8.5f);

    // Preetham Perez luminance distribution coefficients
    float A_Y = 0.1787f * t - 1.4630f;
    float B_Y = -0.3554f * t + 0.4275f;
    float C_Y = -0.0227f * t + 5.3251f;
    float D_Y = 0.1206f * t - 2.5771f;
    float E_Y = -0.0670f * t + 0.3703f;

    auto perez = [](float cosT, float gam, float cosG, float A, float B, float C, float D, float E) {
        return (1.0f + A * std::exp(B / cosT)) *
               (1.0f + C * std::exp(D * gam) + E * cosG * cosG);
    };

    float F_dir = perez(cosTheta, gamma, cosGamma, A_Y, B_Y, C_Y, D_Y, E_Y);
    float F_zen = perez(1.0f, sunZenith, std::max(0.0f, sunDir.y), A_Y, B_Y, C_Y, D_Y, E_Y);
    float relLum = std::clamp(F_dir / std::max(0.25f, F_zen), 0.12f, 3.2f);

    float horizonFactor = std::pow(1.0f - std::clamp(rayDir.y, 0.0f, 1.0f), 2.8f);
    float sunWarmth = std::clamp(1.0f - sunDir.y * 1.15f, 0.0f, 1.0f);

    Vec3 zenithBlue(0.14f, 0.38f, 0.78f);
    Vec3 horizonCool(0.56f, 0.74f, 0.90f);
    Vec3 horizonWarm(0.96f, 0.58f, 0.34f);
    Vec3 horizonCol = Vec3::Lerp(horizonCool, horizonWarm, sunWarmth * (0.45f + 0.55f * (t - 1.8f) / 6.7f));

    Vec3 sky = Vec3::Lerp(zenithBlue, horizonCol, horizonFactor) * (0.65f + 0.45f * relLum);

    // Preetham forward Mie scattering halo & sun disc
    float sunCos = std::max(0.0f, cosGamma);
    float mieHalo = std::pow(sunCos, 10.0f) * (0.30f + 0.35f * sunWarmth);
    float sunDisc = std::pow(sunCos, 320.0f) * 3.8f;
    Vec3 sunTint = Vec3::Lerp(Vec3(1.0f, 0.95f, 0.88f), Vec3(1.0f, 0.64f, 0.28f), sunWarmth);
    sky += sunTint * (mieHalo + sunDisc);

    if (rayDir.y < 0.0f) {
        sky = Vec3::Lerp(horizonCol * 0.55f, Vec3(0.06f, 0.09f, 0.14f), std::clamp(-rayDir.y * 3.5f, 0.0f, 1.0f));
    }
    return sky;
}

} // namespace sphere::water
