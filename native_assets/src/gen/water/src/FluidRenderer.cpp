#include "FluidRenderer.hpp"
#include "CliffGeometry.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>

namespace sphere::water {

namespace {
constexpr float kPi = 3.14159265358979323846f;
constexpr float kSkyDepth = 120.0f;

inline uint32_t PackRGBA8888(float r, float g, float b) {
    auto toU8 = [](float c) -> uint8_t {
        float mapped = c / (1.0f + c * 0.35f); // Soft Reinhard highlight compression
        int iv = static_cast<int>(std::clamp(mapped, 0.0f, 1.0f) * 255.0f + 0.5f);
        return static_cast<uint8_t>(std::clamp(iv, 0, 255));
    };
    return (255u << 24) |
           (static_cast<uint32_t>(toU8(b)) << 16) |
           (static_cast<uint32_t>(toU8(g)) << 8) |
           static_cast<uint32_t>(toU8(r));
}

inline Vec3 ComputeSunDir(float azimuthDeg, float elevationDeg) {
    float az = azimuthDeg * (kPi / 180.0f);
    float el = elevationDeg * (kPi / 180.0f);
    float cosE = std::cos(el);
    return Vec3(cosE * std::sin(az), std::sin(el), cosE * std::cos(az)).Normalized();
}

inline Vec3 SampleSkyColor(const Vec3& dir, const Vec3& sunDir) {
    float up = std::clamp(dir.y * 0.5f + 0.5f, 0.0f, 1.0f);
    Vec3 horizon(0.46f, 0.65f, 0.85f);
    Vec3 zenith(0.06f, 0.16f, 0.38f);
    Vec3 ground(0.05f, 0.07f, 0.11f);

    Vec3 sky = (dir.y >= 0.0f)
        ? Vec3::Lerp(horizon, zenith, std::pow(std::clamp(dir.y, 0.0f, 1.0f), 0.55f))
        : Vec3::Lerp(horizon * 0.45f, ground, std::clamp(-dir.y * 2.2f, 0.0f, 1.0f));

    float sunAmt = std::max(0.0f, Vec3::Dot(dir, sunDir));
    sky += Vec3(1.0f, 0.92f, 0.78f) * std::pow(sunAmt, 72.0f) * 1.35f;
    sky += Vec3(0.45f, 0.55f, 0.65f) * std::pow(sunAmt, 8.0f) * 0.25f;
    (void)up;
    return sky;
}

inline void DrawLineRGBA(
    uint32_t* pixels,
    int32_t stride,
    int32_t w,
    int32_t h,
    float x0, float y0,
    float x1, float y1,
    uint8_t r, uint8_t g, uint8_t b, float alpha
) {
    if (!pixels || w <= 0 || h <= 0 || stride < w) return;
    if (!std::isfinite(x0) || !std::isfinite(y0) || !std::isfinite(x1) || !std::isfinite(y1) || !std::isfinite(alpha)) return;
    float dx = x1 - x0;
    float dy = y1 - y0;
    float maxSpan = std::max(std::abs(dx), std::abs(dy));
    if (!std::isfinite(maxSpan) || maxSpan > 4000.0f) return;
    int steps = static_cast<int>(std::ceil(maxSpan));
    if (steps <= 0 || steps > 4000) return;

    float invSteps = 1.0f / static_cast<float>(steps);
    float clampedAlpha = std::clamp(alpha, 0.0f, 1.0f);
    float invA = 1.0f - clampedAlpha;
    for (int s = 0; s <= steps; ++s) {
        int px = static_cast<int>(x0 + dx * (static_cast<float>(s) * invSteps));
        int py = static_cast<int>(y0 + dy * (static_cast<float>(s) * invSteps));
        if (px >= 0 && px < w && py >= 0 && py < h) {
            uint32_t& dst = pixels[static_cast<size_t>(py) * static_cast<size_t>(stride) + static_cast<size_t>(px)];
            float dr = static_cast<float>(dst & 0xFFu);
            float dg = static_cast<float>((dst >> 8) & 0xFFu);
            float db = static_cast<float>((dst >> 16) & 0xFFu);
            uint8_t nr = static_cast<uint8_t>(std::clamp(r * clampedAlpha + dr * invA, 0.0f, 255.0f));
            uint8_t ng = static_cast<uint8_t>(std::clamp(g * clampedAlpha + dg * invA, 0.0f, 255.0f));
            uint8_t nb = static_cast<uint8_t>(std::clamp(b * clampedAlpha + db * invA, 0.0f, 255.0f));
            dst = (255u << 24) | (static_cast<uint32_t>(nb) << 16) | (static_cast<uint32_t>(ng) << 8) | nr;
        }
    }
}
} // namespace

void FluidRenderer::ResizeBuffers(uint32_t width, uint32_t height) {
    if (m_Width == width && m_Height == height) return;
    m_Width = width;
    m_Height = height;
    const size_t total = static_cast<size_t>(width) * static_cast<size_t>(height);
    m_SceneColorRGB.resize(total * 3);
    m_SceneDepth.resize(total);
    m_RawDepth.resize(total);
    m_BlurTempDepth.resize(total);
    m_SmoothedDepth.resize(total);
    m_Thickness.resize(total);
    m_SpeedBuffer.resize(total);
    m_SphereShadeBuffer.resize(total);
}

void FluidRenderer::Render(
    uint32_t* outPixelsRGBA8888,
    int32_t stridePixels,
    uint32_t width,
    uint32_t height,
    const FluidSimulator& simulator,
    const OrbitalCamera& camera,
    const FluidRenderParams& renderParams
) {
    if (!outPixelsRGBA8888 || width == 0 || height == 0 || stridePixels < static_cast<int32_t>(width)) return;

    auto t0 = std::chrono::steady_clock::now();

    ResizeBuffers(width, height);
    RenderBackgroundAndScene(width, height, simulator, camera, renderParams);
    RenderParticleDepthAndThickness(width, height, simulator, camera, renderParams);

    if (renderParams.bilateralBlurEnabled && renderParams.debugMode != FluidDebugViewMode::RAW_DEPTH &&
        renderParams.debugMode != FluidDebugViewMode::PARTICLE_SPHERES) {
        ApplyBilateralBlur(width, height, renderParams);
    } else {
        m_SmoothedDepth = m_RawDepth;
    }

    CompositeFluidSurface(outPixelsRGBA8888, stridePixels, width, height, simulator, camera, renderParams);

    if (renderParams.showContainerBox) {
        RenderContainerWireframe(outPixelsRGBA8888, stridePixels, width, height, simulator, camera, renderParams);
    }

    auto t1 = std::chrono::steady_clock::now();
    m_LastRenderMs = std::chrono::duration<float, std::milli>(t1 - t0).count();
}

void FluidRenderer::RenderBackgroundAndScene(
    uint32_t width,
    uint32_t height,
    const FluidSimulator& simulator,
    const OrbitalCamera& camera,
    const FluidRenderParams& renderParams
) {
    const auto& phys = simulator.GetParams();
    const Vec3 sunDir = ComputeSunDir(renderParams.sunAzimuthDeg, renderParams.sunElevationDeg);
    const Vec3 camPos = camera.GetPosition();
    const Vec3 camFwd = camera.GetForward();
    const Vec3 camRight = camera.GetRight();
    const Vec3 camUp = camera.GetUp();
    const float tanHalf = camera.GetTanHalfFov();
    const float aspect = camera.GetAspect();

    const float halfW = phys.boundsWidth * 0.5f;
    const float halfD = phys.boundsDepth * 0.5f;
    const float time = simulator.GetElapsedTime();

    const Vec3 obsCenter(phys.obstacleX, phys.obstacleY, phys.obstacleZ);
    const float obsRad = std::max(0.15f, phys.obstacleRadius);
    const float obsRadSq = obsRad * obsRad;

    const float invW = 1.0f / static_cast<float>(width);
    const float invH = 1.0f / static_cast<float>(height);

    for (uint32_t y = 0; y < height; ++y) {
        float ndcY = (1.0f - (static_cast<float>(y) + 0.5f) * invH * 2.0f) * tanHalf;
        for (uint32_t x = 0; x < width; ++x) {
            float ndcX = ((static_cast<float>(x) + 0.5f) * invW * 2.0f - 1.0f) * aspect * tanHalf;
            Vec3 rayDir = (camFwd + camRight * ndcX + camUp * ndcY).Normalized();

            Vec3 col = SampleSkyColor(rayDir, sunDir);
            float bestT = kSkyDepth;

            // Raycast tank floor y = 0.0 (and extended studio ground y = -0.02)
            if (rayDir.y < -1e-4f) {
                float tFloor = -camPos.y / rayDir.y;
                if (tFloor > 0.05f && tFloor < bestT) {
                    Vec3 hit = camPos + rayDir * tFloor;
                    bool insideTank = (std::abs(hit.x) <= halfW + 0.02f && std::abs(hit.z) <= halfD + 0.02f);
                    if (insideTank && std::isfinite(hit.x) && std::isfinite(hit.z)) {
                        bestT = tFloor;
                        int cx = static_cast<int>(std::floor(std::clamp(hit.x * 2.2f, -1.0e6f, 1.0e6f)));
                        int cz = static_cast<int>(std::floor(std::clamp(hit.z * 2.2f, -1.0e6f, 1.0e6f)));
                        bool checker = ((cx + cz) & 1) == 0;
                        Vec3 baseTile = checker ? Vec3(0.24f, 0.30f, 0.37f) : Vec3(0.15f, 0.20f, 0.26f);

                        // Animated caustic pattern on pool floor
                        float c1 = std::sin(hit.x * 6.5f + time * 2.2f) * std::cos(hit.z * 6.5f - time * 1.7f);
                        float c2 = std::sin((hit.x + hit.z) * 4.8f - time * 1.4f);
                        float caustic = std::pow(std::max(0.0f, (c1 + c2) * 0.5f), 2.0f) * renderParams.causticsIntensity;

                        float ndotl = std::max(0.25f, sunDir.y);
                        col = baseTile * ndotl + Vec3(0.22f, 0.48f, 0.62f) * caustic;
                    } else if (renderParams.showFloorGrid && std::abs(hit.x) < 9.0f && std::abs(hit.z) < 9.0f) {
                        bestT = tFloor;
                        float gx = std::abs(hit.x - std::round(hit.x));
                        float gz = std::abs(hit.z - std::round(hit.z));
                        float line = (gx < 0.025f || gz < 0.025f) ? 1.0f : 0.0f;
                        float fade = std::exp(-tFloor * 0.14f);
                        Vec3 studioFloor(0.06f, 0.08f, 0.12f);
                        Vec3 gridLine(0.16f, 0.26f, 0.38f);
                        col = Vec3::Lerp(col, Vec3::Lerp(studioFloor, gridLine, line * 0.55f), fade);
                    }
                }
            }

            // Raycast submerged sphere obstacle
            if (phys.obstacleEnabled) {
                Vec3 oc = camPos - obsCenter;
                float b = Vec3::Dot(oc, rayDir);
                float c = oc.LengthSq() - obsRadSq;
                float disc = b * b - c;
                if (disc > 0.0f) {
                    float tHit = -b - std::sqrt(disc);
                    if (tHit > 0.05f && tHit < bestT) {
                        bestT = tHit;
                        Vec3 hitPos = camPos + rayDir * tHit;
                        Vec3 n = (hitPos - obsCenter).Normalized();
                        float ndotl = std::max(0.0f, Vec3::Dot(n, sunDir));
                        float rim = std::pow(1.0f - std::max(0.0f, Vec3::Dot(n, -rayDir)), 3.0f);
                        Vec3 albedo(0.86f, 0.42f, 0.22f); // Coral-orange calibration sphere
                        col = albedo * (0.22f + 0.78f * ndotl) + Vec3(0.25f, 0.45f, 0.65f) * rim * 0.4f;
                    }
                }
            }

            // Raycast 3D Waterfall Cliff (layered basalt/granite + moss + wet sheen)
            if (phys.waterfallEnabled || phys.scenario == WaterSpawnScenario::WATERFALL_CLIFF) {
                Vec3 bMin, bMax;
                GetCliffBoundsAABB(phys, bMin, bMax);

                float tNear = 0.05f;
                float tFar = bestT;
                bool hitBox = true;
                for (int axis = 0; axis < 3; ++axis) {
                    float o = (axis == 0) ? camPos.x : ((axis == 1) ? camPos.y : camPos.z);
                    float d = (axis == 0) ? rayDir.x : ((axis == 1) ? rayDir.y : rayDir.z);
                    float mn = (axis == 0) ? bMin.x : ((axis == 1) ? bMin.y : bMin.z);
                    float mx = (axis == 0) ? bMax.x : ((axis == 1) ? bMax.y : bMax.z);
                    if (std::abs(d) < 1e-6f) {
                        if (o < mn || o > mx) { hitBox = false; break; }
                    } else {
                        float invD = 1.0f / d;
                        float t1 = (mn - o) * invD;
                        float t2 = (mx - o) * invD;
                        if (t1 > t2) std::swap(t1, t2);
                        tNear = std::max(tNear, t1);
                        tFar = std::min(tFar, t2);
                        if (tNear > tFar) { hitBox = false; break; }
                    }
                }

                if (hitBox) {
                    float tRay = tNear;
                    for (int step = 0; step < 26; ++step) {
                        Vec3 p = camPos + rayDir * tRay;
                        float dist = CliffSDF(p, phys);
                        if (dist < 0.009f) {
                            bestT = tRay;
                            Vec3 n = CliffNormal(p, phys);
                            float strata = 0.5f + 0.5f * std::sin(p.y * 14.0f + std::sin(p.z * 3.5f) * 1.4f);
                            float grain = 0.5f + 0.5f * std::cos(p.x * 9.0f + p.z * 11.0f);
                            Vec3 rockDark(0.16f, 0.15f, 0.17f);
                            Vec3 rockLight(0.32f, 0.29f, 0.27f);
                            Vec3 rockCol = Vec3::Lerp(rockDark, rockLight, strata * 0.65f + grain * 0.35f);

                            // Moss on upper plateaus and canyon rims
                            float mossMask = std::clamp((n.y - 0.58f) * 2.4f, 0.0f, 1.0f);
                            Vec3 mossCol(0.14f, 0.34f, 0.16f);
                            rockCol = Vec3::Lerp(rockCol, mossCol, mossMask * 0.55f);

                            // Wet rock darkening along the waterfall chute and splash base
                            float chuteHalfZ = std::clamp(phys.waterfallCliffWidth * 0.5f, 0.22f, halfD - 0.04f);
                            float inChute = (std::abs(p.z) <= chuteHalfZ + 0.14f) ? 1.0f : 0.0f;
                            float nearPool = std::clamp(1.0f - p.y / std::max(0.25f, phys.plungePoolDepth + 0.22f), 0.0f, 1.0f);
                            float wetness = std::max(inChute * 0.72f, nearPool * 0.80f);
                            rockCol *= (1.0f - wetness * 0.42f);

                            float ndotl = std::max(0.10f, Vec3::Dot(n, sunDir));
                            float undercutAO = std::clamp(0.42f + 0.58f * std::max(0.0f, n.x * 0.4f + n.y * 0.6f + 0.3f), 0.35f, 1.0f);
                            Vec3 halfV = (sunDir - rayDir).Normalized();
                            float wetSpec = std::pow(std::max(0.0f, Vec3::Dot(n, halfV)), 48.0f) * wetness * 0.55f;

                            col = rockCol * (0.24f + 0.76f * ndotl) * undercutAO + Vec3(0.75f, 0.86f, 0.96f) * wetSpec;
                            break;
                        }
                        tRay += std::max(0.015f, dist * 0.85f);
                        if (tRay > tFar) break;
                    }
                }
            }

            size_t idx = static_cast<size_t>(y) * width + x;
            m_SceneDepth[idx] = bestT * Vec3::Dot(rayDir, camFwd);
            m_SceneColorRGB[idx * 3 + 0] = col.x;
            m_SceneColorRGB[idx * 3 + 1] = col.y;
            m_SceneColorRGB[idx * 3 + 2] = col.z;
        }
    }
}

void FluidRenderer::RenderParticleDepthAndThickness(
    uint32_t width,
    uint32_t height,
    const FluidSimulator& simulator,
    const OrbitalCamera& camera,
    const FluidRenderParams& renderParams
) {
    const size_t total = static_cast<size_t>(width) * height;
    std::fill(m_RawDepth.begin(), m_RawDepth.end(), kSkyDepth);
    std::fill(m_Thickness.begin(), m_Thickness.end(), 0.0f);
    std::fill(m_SpeedBuffer.begin(), m_SpeedBuffer.end(), 0.0f);
    std::fill(m_SphereShadeBuffer.begin(), m_SphereShadeBuffer.end(), 0.0f);

    const auto& positions = simulator.GetPositions();
    const auto& velocities = simulator.GetVelocities();
    const size_t n = positions.size();

    const Mat4& view = camera.GetViewMatrix();
    const float tanHalf = camera.GetTanHalfFov();
    const float aspect = camera.GetAspect();
    const float focalY = (0.5f * static_cast<float>(height)) / tanHalf;
    const float focalX = (0.5f * static_cast<float>(width)) / (aspect * tanHalf);
    const float halfW = 0.5f * static_cast<float>(width);
    const float halfH = 0.5f * static_cast<float>(height);

    const float rWorld = std::clamp(renderParams.particleVisualRadius, 0.03f, 0.25f);
    const float thicknessRadius = rWorld * 1.25f;
    const Vec3 sunDir = ComputeSunDir(renderParams.sunAzimuthDeg, renderParams.sunElevationDeg);

    // Transform sun direction to view space for PARTICLE_SPHERES debug shading
    Vec4 sunView4 = view.TransformVector(Vec4(sunDir, 0.0f));
    Vec3 sunView = Vec3(sunView4.x, sunView4.y, sunView4.z).Normalized();

    for (size_t i = 0; i < n; ++i) {
        Vec4 vp = view.TransformVector(Vec4(positions[i], 1.0f));
        float viewZ = -vp.z; // Positive distance forward along camera look axis
        if (viewZ < 0.25f || viewZ > 45.0f) continue;

        float sx = halfW + (vp.x / viewZ) * focalX;
        float sy = halfH - (vp.y / viewZ) * focalY;
        if (!std::isfinite(sx) || !std::isfinite(sy)) continue;

        float radPx = (thicknessRadius / viewZ) * focalY;
        if (!std::isfinite(radPx) || radPx < 1.0f) continue;

        float fMinX = std::clamp(std::floor(sx - radPx), 0.0f, static_cast<float>(width));
        float fMaxX = std::clamp(std::ceil(sx + radPx), -1.0f, static_cast<float>(width) - 1.0f);
        float fMinY = std::clamp(std::floor(sy - radPx), 0.0f, static_cast<float>(height));
        float fMaxY = std::clamp(std::ceil(sy + radPx), -1.0f, static_cast<float>(height) - 1.0f);
        int minX = static_cast<int>(fMinX);
        int maxX = static_cast<int>(fMaxX);
        int minY = static_cast<int>(fMinY);
        int maxY = static_cast<int>(fMaxY);
        if (minX > maxX || minY > maxY) continue;

        float depthRadPx = (rWorld / viewZ) * focalY;
        float invDepthRad = 1.0f / std::max(depthRadPx, 0.5f);
        float invThickRad = 1.0f / std::max(radPx, 0.5f);
        float speed = velocities[i].Length();

        for (int py = minY; py <= maxY; ++py) {
            float dy = (static_cast<float>(py) + 0.5f) - sy;
            for (int px = minX; px <= maxX; ++px) {
                float dx = (static_cast<float>(px) + 0.5f) - sx;

                // Additive fluid thickness pass (slightly wider kernel for smooth absorption)
                float uxT = dx * invThickRad;
                float uyT = -dy * invThickRad;
                float r2T = uxT * uxT + uyT * uyT;
                if (r2T < 1.0f) {
                    size_t pidx = static_cast<size_t>(py) * width + static_cast<size_t>(px);
                    float chord = std::sqrt(1.0f - r2T);
                    m_Thickness[pidx] += chord * 0.135f;

                    // Ray-sphere impostor depth pass
                    float ux = dx * invDepthRad;
                    float uy = -dy * invDepthRad;
                    float r2 = ux * ux + uy * uy;
                    if (r2 < 1.0f) {
                        float nz = std::sqrt(1.0f - r2);
                        float fragDepth = viewZ - nz * rWorld;
                        if (fragDepth > 0.1f && fragDepth < m_RawDepth[pidx] && fragDepth < m_SceneDepth[pidx]) {
                            m_RawDepth[pidx] = fragDepth;
                            m_SpeedBuffer[pidx] = speed;
                            Vec3 sphereN(ux, uy, nz);
                            m_SphereShadeBuffer[pidx] = 0.25f + 0.75f * std::max(0.0f, Vec3::Dot(sphereN, sunView));
                        }
                    }
                }
            }
        }
    }
}

void FluidRenderer::ApplyBilateralBlur(
    uint32_t width,
    uint32_t height,
    const FluidRenderParams& renderParams
) {
    const int radius = std::clamp(renderParams.blurRadiusPx, 1, 10);
    const float spatialSigma = std::max(0.5f, renderParams.blurSpatialSigma);
    const float depthSigma = std::max(0.01f, renderParams.blurDepthSigma);

    const float invTwoSpatialSq = 1.0f / (2.0f * spatialSigma * spatialSigma);
    const float invTwoDepthSq = 1.0f / (2.0f * depthSigma * depthSigma);

    float spatialKernel[21];
    for (int i = -radius; i <= radius; ++i) {
        spatialKernel[i + radius] = std::exp(-static_cast<float>(i * i) * invTwoSpatialSq);
    }

    const int w = static_cast<int>(width);
    const int h = static_cast<int>(height);

    // Horizontal pass: m_RawDepth -> m_BlurTempDepth
    for (int y = 0; y < h; ++y) {
        const size_t rowOffset = static_cast<size_t>(y) * width;
        for (int x = 0; x < w; ++x) {
            float centerD = m_RawDepth[rowOffset + x];
            if (centerD >= 90.0f) {
                m_BlurTempDepth[rowOffset + x] = centerD;
                continue;
            }

            float sum = 0.0f;
            float wSum = 0.0f;
            int xStart = std::max(0, x - radius);
            int xEnd = std::min(w - 1, x + radius);

            for (int sx = xStart; sx <= xEnd; ++sx) {
                float sD = m_RawDepth[rowOffset + sx];
                if (sD >= 90.0f) continue;

                float dDiff = sD - centerD;
                float weight = spatialKernel[(sx - x) + radius] * std::exp(-(dDiff * dDiff) * invTwoDepthSq);
                sum += sD * weight;
                wSum += weight;
            }
            m_BlurTempDepth[rowOffset + x] = (wSum > 1e-6f) ? (sum / wSum) : centerD;
        }
    }

    // Vertical pass: m_BlurTempDepth -> m_SmoothedDepth
    for (int y = 0; y < h; ++y) {
        int yStart = std::max(0, y - radius);
        int yEnd = std::min(h - 1, y + radius);
        for (int x = 0; x < w; ++x) {
            size_t centerIdx = static_cast<size_t>(y) * width + x;
            float centerD = m_BlurTempDepth[centerIdx];
            if (centerD >= 90.0f) {
                m_SmoothedDepth[centerIdx] = centerD;
                continue;
            }

            float sum = 0.0f;
            float wSum = 0.0f;
            for (int sy = yStart; sy <= yEnd; ++sy) {
                float sD = m_BlurTempDepth[static_cast<size_t>(sy) * width + x];
                if (sD >= 90.0f) continue;

                float dDiff = sD - centerD;
                float weight = spatialKernel[(sy - y) + radius] * std::exp(-(dDiff * dDiff) * invTwoDepthSq);
                sum += sD * weight;
                wSum += weight;
            }
            m_SmoothedDepth[centerIdx] = (wSum > 1e-6f) ? (sum / wSum) : centerD;
        }
    }
}

void FluidRenderer::CompositeFluidSurface(
    uint32_t* outPixelsRGBA8888,
    int32_t stridePixels,
    uint32_t width,
    uint32_t height,
    const FluidSimulator& simulator,
    const OrbitalCamera& camera,
    const FluidRenderParams& renderParams
) {
    const float tanHalf = camera.GetTanHalfFov();
    const float aspect = camera.GetAspect();
    const float invW = 1.0f / static_cast<float>(width);
    const float invH = 1.0f / static_cast<float>(height);

    const Vec3 sunDir = ComputeSunDir(renderParams.sunAzimuthDeg, renderParams.sunElevationDeg);
    const Vec3 camRight = camera.GetRight();
    const Vec3 camUp = camera.GetUp();
    const Vec3 camFwd = camera.GetForward();

    auto reconstructViewPos = [&](int px, int py, float d) -> Vec3 {
        float ndcX = ((static_cast<float>(px) + 0.5f) * invW * 2.0f - 1.0f) * aspect * tanHalf;
        float ndcY = (1.0f - (static_cast<float>(py) + 0.5f) * invH * 2.0f) * tanHalf;
        return Vec3(ndcX * d, ndcY * d, -d);
    };

    const Vec3 waterTint(renderParams.waterColorR, renderParams.waterColorG, renderParams.waterColorB);
    const Vec3 absorbCoeff(renderParams.absorptionR, renderParams.absorptionG, renderParams.absorptionB);
    const float absorbStrength = renderParams.absorptionStrength;
    const float refractStrength = renderParams.refractionStrength;
    const float fresnelBias = renderParams.fresnelBias;
    const float fresnelPow = renderParams.fresnelPower;
    const float specPow = renderParams.specularPower;
    const float reflIntensity = renderParams.reflectionIntensity;
    const float foamThreshold = renderParams.foamThreshold;

    const int w = static_cast<int>(width);
    const int h = static_cast<int>(height);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            size_t idx = static_cast<size_t>(y) * width + x;
            float depth = m_SmoothedDepth[idx];

            if (depth >= 90.0f) {
                outPixelsRGBA8888[y * stridePixels + x] = PackRGBA8888(
                    m_SceneColorRGB[idx * 3 + 0],
                    m_SceneColorRGB[idx * 3 + 1],
                    m_SceneColorRGB[idx * 3 + 2]
                );
                continue;
            }

            if (renderParams.debugMode == FluidDebugViewMode::PARTICLE_SPHERES) {
                float shade = m_SphereShadeBuffer[idx];
                outPixelsRGBA8888[y * stridePixels + x] = PackRGBA8888(
                    waterTint.x * shade * 1.2f,
                    waterTint.y * shade * 1.2f,
                    waterTint.z * shade * 1.2f
                );
                continue;
            }

            if (renderParams.debugMode == FluidDebugViewMode::RAW_DEPTH ||
                renderParams.debugMode == FluidDebugViewMode::SMOOTHED_DEPTH) {
                float vis = std::clamp(1.0f - (depth - 1.2f) / 6.5f, 0.05f, 1.0f);
                outPixelsRGBA8888[y * stridePixels + x] = PackRGBA8888(vis, vis, vis);
                continue;
            }

            float thickness = m_Thickness[idx];
            if (renderParams.debugMode == FluidDebugViewMode::THICKNESS) {
                float tVis = std::clamp(thickness * 0.45f, 0.0f, 1.0f);
                outPixelsRGBA8888[y * stridePixels + x] = PackRGBA8888(tVis * 0.35f, tVis * 0.82f, tVis);
                continue;
            }

            // Reconstruct screen-space view normal using edge-preserving finite differences
            Vec3 viewPos = reconstructViewPos(x, y, depth);

            float dR = (x + 1 < w) ? m_SmoothedDepth[idx + 1] : kSkyDepth;
            float dL = (x - 1 >= 0) ? m_SmoothedDepth[idx - 1] : kSkyDepth;
            float dU = (y - 1 >= 0) ? m_SmoothedDepth[idx - width] : kSkyDepth;
            float dD = (y + 1 < h) ? m_SmoothedDepth[idx + width] : kSkyDepth;

            Vec3 ddx(0.01f, 0.0f, 0.0f);
            if (dR < 90.0f && dL < 90.0f) {
                ddx = (std::abs(dR - depth) < std::abs(depth - dL))
                    ? (reconstructViewPos(x + 1, y, dR) - viewPos)
                    : (viewPos - reconstructViewPos(x - 1, y, dL));
            } else if (dR < 90.0f) {
                ddx = reconstructViewPos(x + 1, y, dR) - viewPos;
            } else if (dL < 90.0f) {
                ddx = viewPos - reconstructViewPos(x - 1, y, dL);
            }

            Vec3 ddy(0.0f, 0.01f, 0.0f);
            if (dU < 90.0f && dD < 90.0f) {
                ddy = (std::abs(dU - depth) < std::abs(depth - dD))
                    ? (reconstructViewPos(x, y - 1, dU) - viewPos)
                    : (viewPos - reconstructViewPos(x, y + 1, dD));
            } else if (dU < 90.0f) {
                ddy = reconstructViewPos(x, y - 1, dU) - viewPos;
            } else if (dD < 90.0f) {
                ddy = viewPos - reconstructViewPos(x, y + 1, dD);
            }

            Vec3 viewNormal = Vec3::Cross(ddx, ddy).Normalized();
            if (viewNormal.z < 0.0f) viewNormal = -viewNormal;
            if (viewNormal.LengthSq() < 0.1f) viewNormal = Vec3(0.0f, 0.0f, 1.0f);

            if (renderParams.debugMode == FluidDebugViewMode::NORMALS) {
                outPixelsRGBA8888[y * stridePixels + x] = PackRGBA8888(
                    viewNormal.x * 0.5f + 0.5f,
                    viewNormal.y * 0.5f + 0.5f,
                    viewNormal.z * 0.5f + 0.5f
                );
                continue;
            }

            // World-space normal & view vector
            Vec3 worldNormal = (camRight * viewNormal.x + camUp * viewNormal.y - camFwd * viewNormal.z).Normalized();
            Vec3 viewDir = (-viewPos).Normalized();
            Vec3 worldViewDir = (camRight * viewDir.x + camUp * viewDir.y - camFwd * viewDir.z).Normalized();

            // 1. Snell Refraction sampling scene background
            float iorOffset = (renderParams.indexOfRefraction - 1.0f) * 3.0f;
            float offX = viewNormal.x * refractStrength * iorOffset * static_cast<float>(w) * 0.35f;
            float offY = viewNormal.y * refractStrength * iorOffset * static_cast<float>(h) * 0.35f;
            int dxPx = std::isfinite(offX) ? static_cast<int>(std::clamp(offX, -static_cast<float>(w), static_cast<float>(w))) : 0;
            int dyPx = std::isfinite(offY) ? static_cast<int>(std::clamp(offY, -static_cast<float>(h), static_cast<float>(h))) : 0;
            int refX = std::clamp(x + dxPx, 0, w - 1);
            int refY = std::clamp(y - dyPx, 0, h - 1);
            size_t refIdx = static_cast<size_t>(refY) * width + refX;
            Vec3 bgSample(
                m_SceneColorRGB[refIdx * 3 + 0],
                m_SceneColorRGB[refIdx * 3 + 1],
                m_SceneColorRGB[refIdx * 3 + 2]
            );

            // 2. Beer-Lambert Optical Absorption
            float optDepth = thickness * absorbStrength;
            Vec3 beer(
                std::exp(-absorbCoeff.x * optDepth),
                std::exp(-absorbCoeff.y * optDepth),
                std::exp(-absorbCoeff.z * optDepth)
            );
            Vec3 transmitted(
                bgSample.x * beer.x + waterTint.x * (1.0f - beer.x) * 0.65f,
                bgSample.y * beer.y + waterTint.y * (1.0f - beer.y) * 0.65f,
                bgSample.z * beer.z + waterTint.z * (1.0f - beer.z) * 0.65f
            );

            // 3. Schlick Fresnel + Skybox Reflection
            float ndotv = std::clamp(Vec3::Dot(viewNormal, viewDir), 0.0f, 1.0f);
            float fresnel = fresnelBias + (1.0f - fresnelBias) * std::pow(1.0f - ndotv, fresnelPow);
            Vec3 reflDir = (worldNormal * (2.0f * Vec3::Dot(worldViewDir, worldNormal)) - worldViewDir).Normalized();
            Vec3 skyRefl = SampleSkyColor(reflDir, sunDir) * reflIntensity;

            // 4. Sun Specular Highlight
            Vec3 halfVec = (sunDir + worldViewDir).Normalized();
            float spec = std::pow(std::max(0.0f, Vec3::Dot(worldNormal, halfVec)), specPow);
            float ndotl = std::max(0.0f, Vec3::Dot(worldNormal, sunDir));

            // 5. Velocity / Crest Foam
            float speed = m_SpeedBuffer[idx];
            float foamAmt = std::clamp((speed - foamThreshold) * 0.32f, 0.0f, 0.72f);

            Vec3 surfaceCol = Vec3::Lerp(transmitted * (0.72f + 0.35f * ndotl), skyRefl, std::clamp(fresnel, 0.0f, 0.94f));
            surfaceCol += Vec3(1.0f, 0.96f, 0.90f) * spec * 1.15f;
            surfaceCol = Vec3::Lerp(surfaceCol, Vec3(0.92f, 0.97f, 1.0f), foamAmt);

            outPixelsRGBA8888[y * stridePixels + x] = PackRGBA8888(surfaceCol.x, surfaceCol.y, surfaceCol.z);
        }
    }
    (void)simulator;
}

void FluidRenderer::RenderContainerWireframe(
    uint32_t* outPixelsRGBA8888,
    int32_t stridePixels,
    uint32_t width,
    uint32_t height,
    const FluidSimulator& simulator,
    const OrbitalCamera& camera,
    const FluidRenderParams& renderParams
) {
    (void)renderParams;
    const auto& phys = simulator.GetParams();
    float minX = -phys.boundsWidth * 0.5f;
    float maxX = phys.boundsWidth * 0.5f;
    float minY = 0.0f;
    float maxY = phys.boundsHeight;
    float minZ = -phys.boundsDepth * 0.5f;
    float maxZ = phys.boundsDepth * 0.5f;

    const Mat4& view = camera.GetViewMatrix();
    const float tanHalf = camera.GetTanHalfFov();
    const float aspect = camera.GetAspect();
    const float focalY = (0.5f * static_cast<float>(height)) / tanHalf;
    const float focalX = (0.5f * static_cast<float>(width)) / (aspect * tanHalf);
    const float halfW = 0.5f * static_cast<float>(width);
    const float halfH = 0.5f * static_cast<float>(height);

    auto project = [&](const Vec3& p, float& outX, float& outY) -> bool {
        Vec4 vp = view.TransformVector(Vec4(p, 1.0f));
        float vz = -vp.z;
        if (vz < 0.15f) return false;
        outX = halfW + (vp.x / vz) * focalX;
        outY = halfH - (vp.y / vz) * focalY;
        return true;
    };

    Vec3 corners[8] = {
        {minX, minY, minZ}, {maxX, minY, minZ}, {maxX, minY, maxZ}, {minX, minY, maxZ},
        {minX, maxY, minZ}, {maxX, maxY, minZ}, {maxX, maxY, maxZ}, {minX, maxY, maxZ}
    };

    constexpr int edges[12][2] = {
        {0,1}, {1,2}, {2,3}, {3,0},
        {4,5}, {5,6}, {6,7}, {7,4},
        {0,4}, {1,5}, {2,6}, {3,7}
    };

    for (int e = 0; e < 12; ++e) {
        float x0, y0, x1, y1;
        if (project(corners[edges[e][0]], x0, y0) && project(corners[edges[e][1]], x1, y1)) {
            DrawLineRGBA(outPixelsRGBA8888, stridePixels, static_cast<int32_t>(width), static_cast<int32_t>(height),
                         x0, y0, x1, y1, 148, 210, 255, 0.45f);
        }
    }

    if (phys.waveMakerEnabled && simulator.GetCurrentWaveOffset() > 0.005f) {
        float wx = minX + simulator.GetCurrentWaveOffset();
        Vec3 waveCorners[4] = {
            {wx, minY, minZ}, {wx, minY, maxZ}, {wx, maxY * 0.72f, maxZ}, {wx, maxY * 0.72f, minZ}
        };
        for (int i = 0; i < 4; ++i) {
            float x0, y0, x1, y1;
            if (project(waveCorners[i], x0, y0) && project(waveCorners[(i + 1) % 4], x1, y1)) {
                DrawLineRGBA(outPixelsRGBA8888, stridePixels, static_cast<int32_t>(width), static_cast<int32_t>(height),
                             x0, y0, x1, y1, 56, 224, 184, 0.80f);
            }
        }
    }
}

} // namespace sphere::water
