// ============================================================================
// Botanical3DSceneRenderer.cpp
// Complete Native C++17 3D Z-Buffered Botanical Tree & Shrub Scene Renderer
// ============================================================================

#include "Botanical3DSceneRenderer.h"
#include "../trees/tree_types/TreeTypeRegistry.h"
#include "../bushes/BushGenerator.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>

namespace vegetation {
namespace scene {

namespace {
constexpr float kPi = 3.14159265358979323846f;
constexpr float kDegToRad = kPi / 180.0f;

inline float clamp01(float v) { return std::clamp(v, 0.0f, 1.0f); }

inline uint32_t hashU32(uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

inline RgbF scaleColor(const RgbF& c, float s) {
    return {clamp01(c.r * s), clamp01(c.g * s), clamp01(c.b * s)};
}

inline RgbF blendColor(const RgbF& a, const RgbF& b, float t) {
    float u = clamp01(t);
    return {a.r + (b.r - a.r) * u, a.g + (b.g - a.g) * u, a.b + (b.b - a.b) * u};
}

// Clip a 3D world segment against the camera near plane z >= minDepth
bool clipSegmentToNearPlane(
    const Vec3& p0, const Vec3& p1,
    const Vec3& camPos, const Vec3& camFwd,
    float minDepth,
    Vec3& out0, Vec3& out1
) {
    float d0 = dot(p0 - camPos, camFwd);
    float d1 = dot(p1 - camPos, camFwd);
    if (d0 < minDepth && d1 < minDepth) return false;
    out0 = p0;
    out1 = p1;
    if (d0 < minDepth) {
        float t = (minDepth - d0) / std::max(1e-5f, d1 - d0);
        out0 = p0 + (p1 - p0) * t;
    } else if (d1 < minDepth) {
        float t = (minDepth - d1) / std::max(1e-5f, d0 - d1);
        out1 = p1 + (p0 - p1) * t;
    }
    return true;
}

void drawRootCollarAnchor(
    DepthFrameBuffer& fb,
    const Vec3& basePos,
    float baseRadius,
    const RgbF& barkColor,
    const Vec3& sunDir,
    bool isWireframe
) {
    if (baseRadius < 0.008f) return;
    const int flareCount = (baseRadius >= 0.08f) ? 5 : 3;
    const float sinkY = -std::clamp(baseRadius * 0.16f, 0.006f, 0.04f);
    const float crownY = std::clamp(baseRadius * 0.72f, 0.025f, 0.36f);
    const float reach = std::clamp(baseRadius * 2.15f, 0.03f, 0.85f);

    for (int i = 0; i < flareCount; ++i) {
        float az = (static_cast<float>(i) / static_cast<float>(flareCount)) * 2.0f * kPi + 0.37f;
        Vec3 rootTop(basePos.x + std::cos(az) * (baseRadius * 0.32f), crownY, basePos.z + std::sin(az) * (baseRadius * 0.32f));
        Vec3 rootFoot(basePos.x + std::cos(az) * reach, sinkY, basePos.z + std::sin(az) * reach);
        fb.drawShadedCylinder3D(
            rootTop,
            rootFoot,
            std::max(0.004f, baseRadius * 0.56f),
            std::max(0.002f, baseRadius * 0.18f),
            scaleColor(barkColor, 0.82f),
            sunDir,
            0.84f,
            isWireframe,
            true
        );
    }
}

void drawPlantGLHullWireframe(
    DepthFrameBuffer& fb,
    const CrownControlParams& hp,
    const Vec3& centerBase
) {
    float botY = centerBase.y + hp.trunkHeight;
    float crownH = std::max(0.5f, hp.crownHeight);
    float rxP = hp.rxPos;
    float rxN = hp.rxNeg;
    float rzP = hp.ryPos;
    float rzN = hp.ryNeg;

    auto evalEllipsoidPoint = [&](float az, float u) -> Vec3 {
        // u in [0, 1] from bottom pole to top apex
        float rNorm = std::sin(kPi * std::pow(clamp01(u), 0.75f));
        float ca = std::cos(az);
        float sa = std::sin(az);
        float rx = (ca >= 0.0f) ? rxP : rxN;
        float rz = (sa >= 0.0f) ? rzP : rzN;
        return {
            centerBase.x + ca * rx * rNorm,
            botY + u * crownH,
            centerBase.z + sa * rz * rNorm
        };
    };

    // 8 meridians
    for (int m = 0; m < 8; ++m) {
        float az = (static_cast<float>(m) / 8.0f) * 2.0f * kPi;
        for (int s = 0; s < 16; ++s) {
            float u0 = static_cast<float>(s) / 16.0f;
            float u1 = static_cast<float>(s + 1) / 16.0f;
            fb.drawAaLine3D(evalEllipsoidPoint(az, u0), evalEllipsoidPoint(az, u1), 1.0f, 38, 224, 182, 0.34f, true);
        }
    }
    // 3 latitude parallels
    const float lats[3] = {0.25f, 0.50f, 0.75f};
    for (float u : lats) {
        for (int s = 0; s < 28; ++s) {
            float a0 = (static_cast<float>(s) / 28.0f) * 2.0f * kPi;
            float a1 = (static_cast<float>(s + 1) / 28.0f) * 2.0f * kPi;
            fb.drawAaLine3D(evalEllipsoidPoint(a0, u), evalEllipsoidPoint(a1, u), 1.0f, 38, 224, 182, 0.28f, true);
        }
    }
}

} // namespace

// ============================================================================
// DepthFrameBuffer Implementation
// ============================================================================

DepthFrameBuffer::DepthFrameBuffer(int w, int h, int32_t* pixels)
    : m_width(w), m_height(h), m_pixels(pixels), m_zbuf(static_cast<size_t>(w) * static_cast<size_t>(h), 1e30f) {
}

void DepthFrameBuffer::setCamera(
    const Vec3& camPos,
    const Vec3& fwd,
    const Vec3& right,
    const Vec3& up,
    float centerX,
    float centerY,
    float fovScale
) {
    m_camPos = camPos;
    m_fwd = fwd;
    m_right = right;
    m_up = up;
    m_centerX = centerX;
    m_centerY = centerY;
    m_fovScale = fovScale;
}

ScreenProj DepthFrameBuffer::project(const Vec3& worldPos, float minDepth) const {
    Vec3 d = worldPos - m_camPos;
    float depth = dot(d, m_fwd);
    if (depth < minDepth) {
        return {0.0f, 0.0f, depth, false};
    }
    float rx = dot(d, m_right);
    float uy = dot(d, m_up);
    float sx = m_centerX + (rx / depth) * m_fovScale;
    float sy = m_centerY - (uy / depth) * m_fovScale;
    return {sx, sy, depth, true};
}

float DepthFrameBuffer::getDepth(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) return 0.0f;
    return m_zbuf[static_cast<size_t>(y) * m_width + x];
}

void DepthFrameBuffer::forcePixel(int x, int y, float depth, uint8_t r, uint8_t g, uint8_t b) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) return;
    size_t idx = static_cast<size_t>(y) * m_width + x;
    if (depth <= m_zbuf[idx]) {
        m_zbuf[idx] = depth;
        m_pixels[idx] = (0xFF << 24) | (static_cast<int32_t>(r) << 16) | (static_cast<int32_t>(g) << 8) | static_cast<int32_t>(b);
    }
}

void DepthFrameBuffer::blendPixel(int x, int y, float depth, uint8_t r, uint8_t g, uint8_t b, float alpha, bool writeDepth) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height || alpha <= 0.01f) return;
    size_t idx = static_cast<size_t>(y) * m_width + x;
    if (depth > m_zbuf[idx]) return;

    if (alpha >= 0.985f) {
        m_pixels[idx] = (0xFF << 24) | (static_cast<int32_t>(r) << 16) | (static_cast<int32_t>(g) << 8) | static_cast<int32_t>(b);
        if (writeDepth) m_zbuf[idx] = depth;
        return;
    }

    int32_t dst = m_pixels[idx];
    uint8_t dr = static_cast<uint8_t>((dst >> 16) & 0xFF);
    uint8_t dg = static_cast<uint8_t>((dst >> 8) & 0xFF);
    uint8_t db = static_cast<uint8_t>(dst & 0xFF);

    float a = clamp01(alpha);
    float ia = 1.0f - a;
    uint8_t outR = static_cast<uint8_t>(std::clamp(r * a + dr * ia + 0.5f, 0.0f, 255.0f));
    uint8_t outG = static_cast<uint8_t>(std::clamp(g * a + dg * ia + 0.5f, 0.0f, 255.0f));
    uint8_t outB = static_cast<uint8_t>(std::clamp(b * a + db * ia + 0.5f, 0.0f, 255.0f));

    m_pixels[idx] = (0xFF << 24) | (static_cast<int32_t>(outR) << 16) | (static_cast<int32_t>(outG) << 8) | static_cast<int32_t>(outB);
    if (writeDepth && a > 0.65f) {
        m_zbuf[idx] = depth;
    }
}

void DepthFrameBuffer::fillStudioGroundPlane(
    const Vec3& sunDir,
    float canopyRadius,
    float rootRadius,
    float plantHeight,
    bool isShrubTheme
) {
    const float wF = static_cast<float>(m_width);
    const float hF = static_cast<float>(m_height);
    const float invFov = 1.0f / std::max(1.0f, m_fovScale);

    // Clean dark studio backdrop (#08111A AbyssNavy) with subtle top-to-bottom depth tone (no sky, sun, or horizon cut)
    const RgbF studioTop{8.0f / 255.0f, 16.0f / 255.0f, 26.0f / 255.0f};
    const RgbF studioBot{10.0f / 255.0f, 20.0f / 255.0f, 30.0f / 255.0f};

    const float safeCanopyR = std::max(1.2f, canopyRadius);
    const float safeRootR = std::max(0.18f, rootRadius);
    const float camOriginDist = std::hypot(m_camPos.x, m_camPos.z);
    const float fadeRadius = std::max(safeCanopyR * 2.8f, camOriginDist * 1.35f + safeCanopyR);
    const float invFadeRadSq = 1.0f / (fadeRadius * fadeRadius);

    // Offset directional canopy cast shadow opposite sun azimuth on ground y = 0
    const float sunHorizLen = std::max(0.12f, std::hypot(sunDir.x, sunDir.z));
    const float castScale = std::clamp(plantHeight * 0.16f / std::max(0.30f, sunDir.y), 0.18f, safeCanopyR * 0.85f);
    const float shadowCx = -(sunDir.x / sunHorizLen) * castScale;
    const float shadowCz = -(sunDir.z / sunHorizLen) * castScale;
    const float castInvRadSq = 1.0f / std::max(0.35f, (safeCanopyR * 0.82f) * (safeCanopyR * 0.82f));
    const float rootAoInvRadSq = 1.0f / std::max(0.06f, (safeRootR * 4.4f) * (safeRootR * 4.4f));

    // Sunlit soil & mossy turf base tones
    const float sunLift = 0.72f + 0.34f * std::max(0.15f, sunDir.y);
    const RgbF soilNear = isShrubTheme
        ? RgbF{0.105f * sunLift, 0.145f * sunLift, 0.088f * sunLift}
        : RgbF{0.095f * sunLift, 0.132f * sunLift, 0.078f * sunLift};
    const RgbF soilOuter = isShrubTheme
        ? RgbF{0.068f * sunLift, 0.098f * sunLift, 0.062f * sunLift}
        : RgbF{0.062f * sunLift, 0.088f * sunLift, 0.056f * sunLift};

    for (int py = 0; py < m_height; ++py) {
        const float vFrac = static_cast<float>(py) / std::max(1.0f, hF);
        const RgbF bg = blendColor(studioTop, studioBot, vFrac);
        const float ndcY = (m_centerY - (static_cast<float>(py) + 0.5f)) * invFov;

        const Vec3 rowBase = m_fwd + m_up * ndcY;
        const size_t rowOff = static_cast<size_t>(py) * static_cast<size_t>(m_width);

        for (int px = 0; px < m_width; ++px) {
            const float ndcX = (static_cast<float>(px) + 0.5f - m_centerX) * invFov;
            const Vec3 ray = rowBase + m_right * ndcX;

            if (ray.y >= -0.008f || m_camPos.y <= 0.001f) {
                m_pixels[rowOff + px] =
                    (0xFF << 24) | (toByte(bg.r) << 16) | (toByte(bg.g) << 8) | toByte(bg.b);
                continue;
            }

            // Ray intersects ground plane y = 0; because m_fwd, m_right, m_up are orthonormal,
            // forward view depth equals t * dot(ray, m_fwd) == t!
            const float t = -m_camPos.y / ray.y;
            if (t <= 0.10f || t > 180.0f) {
                m_pixels[rowOff + px] =
                    (0xFF << 24) | (toByte(bg.r) << 16) | (toByte(bg.g) << 8) | toByte(bg.b);
                continue;
            }

            const float wx = m_camPos.x + t * ray.x;
            const float wz = m_camPos.z + t * ray.z;
            const float distOriginSq = wx * wx + wz * wz;

            // Smooth exponential falloff into dark studio background (no sharp horizon line)
            const float slopeSoft = clamp01((-ray.y - 0.008f) * 6.5f);
            const float radialFade = std::exp(-distOriginSq * invFadeRadSq);
            const float groundVis = slopeSoft * radialFade;

            if (groundVis <= 0.006f) {
                m_pixels[rowOff + px] =
                    (0xFF << 24) | (toByte(bg.r) << 16) | (toByte(bg.g) << 8) | toByte(bg.b);
                continue;
            }

            // Organic earth & moss micro-texture (zero grid lines or geometric rings)
            const float macroVar = 0.5f + 0.5f * std::sin(wx * 1.45f + std::cos(wz * 1.15f) * 1.4f);
            const float grain = 0.88f + 0.14f * std::sin(wx * 6.4f + wz * 4.7f) * std::cos(wx * 4.3f - wz * 7.1f);
            const uint32_t cellH = hashU32(
                static_cast<uint32_t>(static_cast<int32_t>(std::floor(wx * 14.0f)) * 73856093) ^
                static_cast<uint32_t>(static_cast<int32_t>(std::floor(wz * 14.0f)) * 19349663)
            );
            const float pebble = 0.94f + 0.12f * (static_cast<float>(cellH & 0xFFu) / 255.0f);

            const float nearMix = clamp01(1.0f - std::sqrt(distOriginSq) / (safeCanopyR * 1.55f));
            RgbF earth = blendColor(soilOuter, soilNear, 0.35f * macroVar + 0.65f * nearMix);
            earth = scaleColor(earth, grain * pebble);

            // Crisp root-collar ambient occlusion right around trunk/stem entry + soft canopy shadow
            const float rootAo = 1.0f - 0.64f * std::exp(-distOriginSq * rootAoInvRadSq);
            const float sdx = wx - shadowCx;
            const float sdz = wz - shadowCz;
            const float canopyShadow = 1.0f - 0.42f * std::exp(-(sdx * sdx + sdz * sdz) * castInvRadSq);
            earth = scaleColor(earth, rootAo * canopyShadow);

            const RgbF finalCol = blendColor(bg, earth, groundVis);
            m_pixels[rowOff + px] =
                (0xFF << 24) | (toByte(finalCol.r) << 16) | (toByte(finalCol.g) << 8) | toByte(finalCol.b);

            // Commit ground plane depth inside visible earth footprint so roots intersect solidly
            if (groundVis > 0.18f) {
                m_zbuf[rowOff + px] = t;
            }
        }
    }
}

void DepthFrameBuffer::drawAaLine3D(
    const Vec3& p0,
    const Vec3& p1,
    float thicknessPx,
    uint8_t r, uint8_t g, uint8_t b, float alpha,
    bool writeDepth,
    float depthBias
) {
    Vec3 c0, c1;
    if (!clipSegmentToNearPlane(p0, p1, m_camPos, m_fwd, 0.22f, c0, c1)) return;

    ScreenProj s0 = project(c0, 0.18f);
    ScreenProj s1 = project(c1, 0.18f);
    if (!s0.valid || !s1.valid) return;

    float dx = s1.x - s0.x;
    float dy = s1.y - s0.y;
    float lenSq = dx * dx + dy * dy;
    float halfW = std::max(0.45f, thicknessPx * 0.5f);
    float pad = halfW + 1.2f;

    int minX = std::max(0, static_cast<int>(std::floor(std::min(s0.x, s1.x) - pad)));
    int maxX = std::min(m_width - 1, static_cast<int>(std::ceil(std::max(s0.x, s1.x) + pad)));
    int minY = std::max(0, static_cast<int>(std::floor(std::min(s0.y, s1.y) - pad)));
    int maxY = std::min(m_height - 1, static_cast<int>(std::ceil(std::max(s0.y, s1.y) + pad)));
    if (minX > maxX || minY > maxY) return;

    float invZ0 = 1.0f / s0.depth;
    float invZ1 = 1.0f / s1.depth;
    float invLenSq = (lenSq > 1e-4f) ? (1.0f / lenSq) : 0.0f;

    for (int py = minY; py <= maxY; ++py) {
        float y = static_cast<float>(py) + 0.5f;
        for (int px = minX; px <= maxX; ++px) {
            float x = static_cast<float>(px) + 0.5f;
            float t = (invLenSq > 0.0f) ? std::clamp(((x - s0.x) * dx + (y - s0.y) * dy) * invLenSq, 0.0f, 1.0f) : 0.0f;
            float cx = s0.x + dx * t;
            float cy = s0.y + dy * t;
            float dist = std::hypot(x - cx, y - cy);
            if (dist >= halfW + 0.9f) continue;

            float cov = std::clamp((halfW + 0.9f - dist) / 1.1f, 0.0f, 1.0f) * alpha;
            float z = 1.0f / lerp(invZ0, invZ1, t) + depthBias;
            blendPixel(px, py, z, r, g, b, cov, writeDepth);
        }
    }
}

void DepthFrameBuffer::drawShadedCylinder3D(
    const Vec3& p0,
    const Vec3& p1,
    float r0,
    float r1,
    const RgbF& barkBase,
    const Vec3& sunDir,
    float aoFactor,
    bool isWireframe,
    bool drawJointCap
) {
    Vec3 c0, c1;
    if (!clipSegmentToNearPlane(p0, p1, m_camPos, m_fwd, 0.25f, c0, c1)) return;

    ScreenProj s0 = project(c0, 0.20f);
    ScreenProj s1 = project(c1, 0.20f);
    if (!s0.valid || !s1.valid) return;

    if (isWireframe) {
        drawAaLine3D(c0, c1, 1.2f, toByte(barkBase.r * 1.4f), toByte(barkBase.g * 1.4f), toByte(barkBase.b * 1.4f), 0.72f, true);
        return;
    }

    float pxR0 = std::clamp((r0 * m_fovScale) / s0.depth, 0.75f, 95.0f);
    float pxR1 = std::clamp((r1 * m_fovScale) / s1.depth, 0.55f, 95.0f);

    float dx = s1.x - s0.x;
    float dy = s1.y - s0.y;
    float lenSq = dx * dx + dy * dy;
    float lenPx = std::sqrt(lenSq);
    float maxR = std::max(pxR0, pxR1);
    float pad = maxR + 1.5f;

    int minX = std::max(0, static_cast<int>(std::floor(std::min(s0.x, s1.x) - pad)));
    int maxX = std::min(m_width - 1, static_cast<int>(std::ceil(std::max(s0.x, s1.x) + pad)));
    int minY = std::max(0, static_cast<int>(std::floor(std::min(s0.y, s1.y) - pad)));
    int maxY = std::min(m_height - 1, static_cast<int>(std::ceil(std::max(s0.y, s1.y) + pad)));
    if (minX > maxX || minY > maxY) return;

    // 2D perpendicular unit vector across screen axis
    float nx2 = (lenPx > 1e-3f) ? (-dy / lenPx) : 1.0f;
    float ny2 = (lenPx > 1e-3f) ? ( dx / lenPx) : 0.0f;

    // World lateral axis mapped onto screen perpendicular
    Vec3 worldPerp = (m_right * nx2 - m_up * ny2).normalized();
    Vec3 towardCam = (m_fwd * -1.0f).normalized();

    float invZ0 = 1.0f / s0.depth;
    float invZ1 = 1.0f / s1.depth;
    float invLenSq = (lenSq > 1e-4f) ? (1.0f / lenSq) : 0.0f;

    for (int py = minY; py <= maxY; ++py) {
        float y = static_cast<float>(py) + 0.5f;
        for (int px = minX; px <= maxX; ++px) {
            float x = static_cast<float>(px) + 0.5f;

            float t = (invLenSq > 0.0f) ? ((x - s0.x) * dx + (y - s0.y) * dy) * invLenSq : 0.0f;
            float tClamped = drawJointCap ? std::clamp(t, 0.0f, 1.0f) : t;
            if (!drawJointCap && (t < -0.02f || t > 1.02f)) continue;

            float cx = s0.x + dx * tClamped;
            float cy = s0.y + dy * tClamped;
            float rAtT = lerp(pxR0, pxR1, std::clamp(tClamped, 0.0f, 1.0f));

            float vx = x - cx;
            float vy = y - cy;
            float signedPerp = vx * nx2 + vy * ny2;
            float dist = std::hypot(vx, vy);

            if (dist >= rAtT + 0.9f) continue;

            float z = 1.0f / lerp(invZ0, invZ1, std::clamp(tClamped, 0.0f, 1.0f));
            // Quick z cull before shading
            if (z > m_zbuf[static_cast<size_t>(py) * m_width + px] + 0.01f) continue;

            float cov = std::clamp((rAtT + 0.85f - dist) / 1.05f, 0.0f, 1.0f);
            float uCyl = std::clamp(signedPerp / std::max(0.5f, rAtT), -0.96f, 0.96f);
            float zCyl = std::sqrt(std::max(0.04f, 1.0f - uCyl * uCyl));

            Vec3 surfNorm = (worldPerp * uCyl + towardCam * zCyl).normalized();
            float sunDot = std::max(0.0f, dot(surfNorm, sunDir));
            // Subtle warm bark wrap + cool shadow ambience + furrow longitudinal texture
            float barkFurrow = 0.94f + 0.08f * std::sin(uCyl * 11.0f + tClamped * 6.0f);
            float light = (0.34f + 0.66f * sunDot) * aoFactor * barkFurrow;

            RgbF lit = {
                clamp01(barkBase.r * light + 0.03f * sunDot),
                clamp01(barkBase.g * light + 0.02f * sunDot),
                clamp01(barkBase.b * light + 0.01f * sunDot)
            };

            if (cov >= 0.92f) {
                forcePixel(px, py, z - rAtT * 0.002f * zCyl, toByte(lit.r), toByte(lit.g), toByte(lit.b));
            } else {
                blendPixel(px, py, z, toByte(lit.r), toByte(lit.g), toByte(lit.b), cov, false);
            }
        }
    }
}

void DepthFrameBuffer::drawShadedSphere3D(
    const Vec3& center,
    float worldRadius,
    const RgbF& baseColor,
    const Vec3& sunDir,
    float specStrength,
    float minPx,
    float maxPx
) {
    ScreenProj s = project(center, 0.25f);
    if (!s.valid) return;

    float radPx = std::clamp((worldRadius * m_fovScale) / s.depth, minPx, maxPx);
    float pad = radPx + 1.2f;

    int minX = std::max(0, static_cast<int>(std::floor(s.x - pad)));
    int maxX = std::min(m_width - 1, static_cast<int>(std::ceil(s.x + pad)));
    int minY = std::max(0, static_cast<int>(std::floor(s.y - pad)));
    int maxY = std::min(m_height - 1, static_cast<int>(std::ceil(s.y + pad)));
    if (minX > maxX || minY > maxY) return;

    Vec3 towardCam = (m_fwd * -1.0f).normalized();
    Vec3 halfVec = (sunDir + towardCam).normalized();

    for (int py = minY; py <= maxY; ++py) {
        float y = static_cast<float>(py) + 0.5f;
        float ny = (s.y - y) / radPx;
        for (int px = minX; px <= maxX; ++px) {
            float x = static_cast<float>(px) + 0.5f;
            float nx = (x - s.x) / radPx;
            float r2 = nx * nx + ny * ny;
            float distPx = std::sqrt(r2) * radPx;
            if (distPx >= radPx + 0.9f) continue;

            float nz = std::sqrt(std::max(0.0f, 1.0f - std::min(0.99f, r2)));
            float z = s.depth - worldRadius * nz * 0.5f;
            if (z > m_zbuf[static_cast<size_t>(py) * m_width + px] + 0.008f) continue;

            Vec3 n = (m_right * nx + m_up * ny + towardCam * nz).normalized();
            float diff = std::max(0.0f, dot(n, sunDir));
            float spec = std::pow(std::max(0.0f, dot(n, halfVec)), 18.0f) * specStrength;
            float light = 0.36f + 0.68f * diff;

            RgbF lit = {
                clamp01(baseColor.r * light + spec),
                clamp01(baseColor.g * light + spec),
                clamp01(baseColor.b * light + spec * 0.9f)
            };

            float cov = std::clamp((radPx + 0.85f - distPx) / 1.05f, 0.0f, 1.0f);
            if (cov >= 0.92f) {
                forcePixel(px, py, z, toByte(lit.r), toByte(lit.g), toByte(lit.b));
            } else {
                blendPixel(px, py, z, toByte(lit.r), toByte(lit.g), toByte(lit.b), cov, false);
            }
        }
    }
}

void DepthFrameBuffer::drawGroundShadowEllipse(
    const Vec3& rootPos,
    float worldRadiusX,
    float worldRadiusZ
) {
    ScreenProj s = project(rootPos, 0.30f);
    if (!s.valid) return;

    float radX = std::clamp((worldRadiusX * m_fovScale) / s.depth, 14.0f, 140.0f);
    float radY = std::clamp(radX * 0.38f, 6.0f, 56.0f);

    int minX = std::max(0, static_cast<int>(std::floor(s.x - radX)));
    int maxX = std::min(m_width - 1, static_cast<int>(std::ceil(s.x + radX)));
    int minY = std::max(0, static_cast<int>(std::floor(s.y - radY)));
    int maxY = std::min(m_height - 1, static_cast<int>(std::ceil(s.y + radY)));
    if (minX > maxX || minY > maxY) return;

    for (int py = minY; py <= maxY; ++py) {
        float dy = (static_cast<float>(py) + 0.5f - s.y) / radY;
        for (int px = minX; px <= maxX; ++px) {
            float dx = (static_cast<float>(px) + 0.5f - s.x) / radX;
            float r2 = dx * dx + dy * dy;
            if (r2 >= 1.0f) continue;
            float alpha = std::pow(1.0f - r2, 1.6f) * 0.42f;
            blendPixel(px, py, s.depth + 0.04f, 2, 10, 5, alpha, false);
        }
    }
}

// ============================================================================
// Helper: Build Complete Orthonormal Leaf Frames from Synthesized Leaves
// ============================================================================

static void buildLeafRenderItemsFromTreeLeaves(
    const std::vector<TreeLeafData>& srcLeaves,
    float treeHeightMeters,
    int leafMorphologyId,
    bool isCloseup,
    std::vector<LeafRenderItem>& outItems
) {
    outItems.clear();
    outItems.reserve(srcLeaves.size());

    bool isConifer = (leafMorphologyId <= 3 || leafMorphologyId == 21);

    for (size_t i = 0; i < srcLeaves.size(); ++i) {
        const auto& l = srcLeaves[i];
        Vec3 M = l.normal.normalized();

        // Petiole proportional strictly to leaf blade (banana paddle attaches flush at petiole tip)
        bool isBananaBlade = (leafMorphologyId == 14);
        bool isGiant = (l.size >= 0.55f) || isBananaBlade || leafMorphologyId == 17 || leafMorphologyId == 18;
        float leafLen = l.size * (isGiant ? 1.0f : (isCloseup ? 1.12f : 1.05f));
        float petioleLen = isConifer ? 0.008f : (isBananaBlade ? 0.0f : (isGiant ? 0.020f : std::clamp(leafLen * 0.20f, 0.007f, 0.048f)));
        Vec3 bladeBase = l.position + M * petioleLen;

        // Orthonormal basis (S0, N0, M)
        Vec3 upRef = (std::abs(M.y) < 0.94f) ? Vec3(0.0f, 1.0f, 0.0f) : Vec3(1.0f, 0.0f, 0.0f);
        Vec3 s0 = cross(M, upRef).normalized();
        Vec3 n0 = cross(s0, M).normalized();

        // Phyllotaxis natural roll (keeps banana & palm fronds oriented cleanly skyward)
        float pA = std::sin(static_cast<float>(i) * 2.3999632f + l.position.x * 2.71f + l.position.z * 3.13f);
        float pB = std::cos(static_cast<float>(i) * 1.6180339f + l.position.y * 3.77f);
        float roll = isBananaBlade ? (pA * 0.16f + pB * 0.06f) : (isGiant ? (pA * 0.36f + pB * 0.12f) : (pA * 1.10f + pB * 0.40f));
        float cR = std::cos(roll);
        float sR = std::sin(roll);

        Vec3 S = (s0 * cR + n0 * sR).normalized();
        Vec3 N = (n0 * cR - s0 * sR).normalized();
        // Flip adaxial side so it mostly prefers skyward hemisphere
        if (N.y < -0.35f) {
            N = N * -1.0f;
            S = S * -1.0f;
        }

        float sunDot = dot(N, Vec3(0.452f, 0.804f, 0.382f));
        float wrapped = clamp01((sunDot + 0.35f) / 1.35f);
        float heightShade = std::clamp(l.position.y / std::max(1.0f, treeHeightMeters), 0.25f, 1.0f);
        float sunlit = clamp01((wrapped * 0.72f + std::abs(sunDot) * 0.28f) * (0.62f + 0.38f * heightShade));

        LeafRenderItem item;
        item.twigAttach = l.position;
        item.bladeBase = bladeBase;
        item.midribDir = M;
        item.wingDir = S;
        item.normalDir = N;
        item.lengthMeters = leafLen;
        item.petioleMeters = petioleLen;
        item.sunlitFactor = sunlit;
        item.tintVariation = clamp01(0.35f + 0.30f * l.colorG);
        item.indexSeed = static_cast<uint32_t>(i);
        item.isGiantFrond = isGiant;
        item.allowSplits = isBananaBlade ? (i >= srcLeaves.size() / 3) : true;
        outItems.push_back(item);
    }
}

// ============================================================================
// Top-Level C++17 Tree Scene Entry Point
// ============================================================================

void renderTreeSceneToArgb(
    const BotanicalTreeRenderParams& params,
    int width,
    int height,
    int32_t* outArgbPixels,
    float* outStats8
) {
    if (!outArgbPixels || width <= 4 || height <= 4) return;
    auto tStart = std::chrono::steady_clock::now();

    // 1. Synthesize 3D Tree Geometry directly in C++
    auto tree = TreeTypeRegistry::createTreeType(static_cast<TreeSpeciesId>(params.speciesId));
    if (!tree) {
        tree = TreeTypeRegistry::createTreeType(TreeSpeciesId::ENGLISH_OAK);
    }

    if (params.heightMeters > 0.5f) tree->setTreeHeight(params.heightMeters);
    if (params.trunkRadiusMeters > 0.01f) tree->setTrunkRadius(params.trunkRadiusMeters);
    if (params.branchCount > 0) tree->setBranchNumber(params.branchCount);
    if (params.leafCount >= 0) tree->setLeafCount(params.leafCount);
    if (params.branchAngleDeg > 5.0f) tree->setBranchAngleDeg(params.branchAngleDeg);
    if (params.leafScale > 0.05f) tree->setLeafScale(params.leafScale);
    tree->setWindEnabled(params.windEnabled);
    tree->setWindSpeedMps(params.windSpeedMps);
    tree->setWindFlexibility(params.windFlexibility);

    std::vector<TreeBranchSegment> branches;
    std::vector<TreeLeafData> rawLeaves;
    tree->buildTreeStructure(branches, rawLeaves, params.seed);

    // Apply C++ aerodynamic wind deflection cleanly across branches and attached leaves
    if (params.windEnabled && params.windSpeedMps > 1e-4f) {
        std::vector<Vec3> defStart(branches.size());
        std::vector<Vec3> defEnd(branches.size());
        for (size_t i = 0; i < branches.size(); ++i) {
            defStart[i] = tree->computeWindDeflection(branches[i].startPos, params.timeSeconds);
            defEnd[i] = tree->computeWindDeflection(branches[i].endPos, params.timeSeconds);
        }
        for (auto& l : rawLeaves) {
            if (l.branchIndex < branches.size()) {
                const auto& b = branches[l.branchIndex];
                Vec3 d = b.endPos - b.startPos;
                float lenSq = d.lengthSq();
                float t = (lenSq > 1e-4f) ? clamp01(dot(l.position - b.startPos, d) / lenSq) : 0.5f;
                l.position += defStart[l.branchIndex] * (1.0f - t) + defEnd[l.branchIndex] * t;
            } else {
                l.position += tree->computeWindDeflection(l.position, params.timeSeconds);
            }
            l.position.y = std::max(0.025f, l.position.y);
        }
        for (size_t i = 0; i < branches.size(); ++i) {
            bool isRootBase = (branches[i].startPos.y <= 0.03f);
            branches[i].startPos += defStart[i];
            if (isRootBase) {
                branches[i].startPos.y = -0.015f; // Anchor basal trunk/stem cleanly into ground plane
            } else {
                branches[i].startPos.y = std::max(0.02f, branches[i].startPos.y);
            }
            branches[i].endPos += defEnd[i];
            branches[i].endPos.y = std::max(0.025f, branches[i].endPos.y);
        }
    } else {
        for (auto& b : branches) {
            if (b.startPos.y <= 0.03f) {
                b.startPos.y = -0.015f;
            }
            if (b.endPos.y < 0.02f) {
                b.endPos.y = 0.02f;
            }
        }
    }

    // 2. Build Real Leaf Frames + Exact 3D AABB across ALL branches & leaves
    bool isCloseup = (params.cameraFocus == 2);
    std::vector<LeafRenderItem> leafItems;
    if (params.showLeaves) {
        buildLeafRenderItemsFromTreeLeaves(rawLeaves, params.heightMeters, params.leafMorphologyId, isCloseup, leafItems);
    }

    Vec3 bMin( 1e9f,  1e9f,  1e9f);
    Vec3 bMax(-1e9f, -1e9f, -1e9f);
    auto expandBox = [&](const Vec3& p, float pad = 0.0f) {
        bMin.x = std::min(bMin.x, p.x - pad);
        bMin.y = std::min(bMin.y, p.y - pad);
        bMin.z = std::min(bMin.z, p.z - pad);
        bMax.x = std::max(bMax.x, p.x + pad);
        bMax.y = std::max(bMax.y, p.y + pad);
        bMax.z = std::max(bMax.z, p.z + pad);
    };

    for (const auto& b : branches) {
        expandBox(b.startPos, b.radiusStart);
        expandBox(b.endPos, b.radiusEnd);
    }
    for (const auto& l : leafItems) {
        expandBox(l.bladeBase, l.lengthMeters * 0.7f);
        expandBox(l.bladeBase + l.midribDir * l.lengthMeters, l.lengthMeters * 0.4f);
    }
    if (branches.empty()) {
        bMin = Vec3(-2.0f, 0.0f, -2.0f);
        bMax = Vec3( 2.0f, params.heightMeters, 2.0f);
    }

    bMin.y = 0.0f;
    float heightSpan = std::max(1.8f, bMax.y - bMin.y);
    float radX = std::max(std::abs(bMin.x), std::abs(bMax.x));
    float radZ = std::max(std::abs(bMin.z), std::abs(bMax.z));
    float maxHorizR = std::max(1.0f, std::hypot(radX, radZ));
    Vec3 boxCenter((bMin.x + bMax.x) * 0.5f, bMin.y + heightSpan * 0.48f, (bMin.z + bMax.z) * 0.5f);

    // 3. Configure Auto-Fit LookAt Camera
    float wF = static_cast<float>(width);
    float hF = static_cast<float>(height);
    float halfFovTan = std::tan(24.0f * kDegToRad);
    float fovScale = (std::min(wF, hF) * 0.50f) / halfFovTan;

    // Fit entire tree from y = 0 ground contact up to crown apex with breathing room above & below
    float fitVert = (heightSpan * 0.54f) / std::max(0.1f, (hF * 0.35f / fovScale));
    float fitHoriz = (maxHorizR * 1.10f) / std::max(0.1f, (wF * 0.45f / fovScale));
    float baseFitDist = std::max({3.4f, fitVert, fitHoriz});

    float userZoom = std::clamp(params.camDistanceMeters / 22.0f, 0.22f, 3.8f);

    Vec3 targetPos = boxCenter;
    float camDist = baseFitDist * userZoom;

    if (params.cameraFocus == 1) {
        // Branching & Bifurcations closeup
        targetPos = Vec3(boxCenter.x * 0.6f, bMin.y + heightSpan * 0.46f, boxCenter.z * 0.6f);
        camDist = std::clamp(baseFitDist * 0.58f * userZoom, 1.5f, 50.0f);
    } else if (params.cameraFocus == 2) {
        // Leaf & Petiole closeup: frame an outer sunlit terminal stem so its tip leaves & acropetal graduation are clearly visible
        if (!leafItems.empty()) {
            size_t bestIdx = 0;
            float bestScore = -1e9f;
            float camAz = params.camAzimuthDeg * kDegToRad;
            Vec3 camDirGuess(std::sin(camAz), 0.35f, std::cos(camAz));
            for (size_t i = 0; i < leafItems.size(); ++i) {
                const auto& li = leafItems[i];
                float tipBonus = 0.0f;
                if (i < rawLeaves.size() && rawLeaves[i].branchIndex < branches.size()) {
                    const auto& b = branches[rawLeaves[i].branchIndex];
                    if (b.recursionDepth >= 2) tipBonus += 1.2f;
                }
                float score = dot(li.bladeBase, camDirGuess) + li.sunlitFactor * 2.0f + tipBonus;
                if (score > bestScore) {
                    bestScore = score;
                    bestIdx = i;
                }
            }
            const auto& focusLeaf = leafItems[bestIdx];
            float twigSpan = 0.0f;
            if (focusLeaf.isGiantFrond) {
                targetPos = focusLeaf.bladeBase + focusLeaf.midribDir * (focusLeaf.lengthMeters * 0.34f);
            } else if (bestIdx < rawLeaves.size() && rawLeaves[bestIdx].branchIndex < branches.size()) {
                const auto& b = branches[rawLeaves[bestIdx].branchIndex];
                twigSpan = (b.endPos - b.startPos).length();
                targetPos = lerp(b.startPos, b.endPos, 0.72f);
            } else {
                targetPos = focusLeaf.bladeBase + focusLeaf.midribDir * (focusLeaf.lengthMeters * 0.5f);
            }
            float focusSize = std::max(0.14f, focusLeaf.lengthMeters);
            camDist = std::clamp(std::max(0.26f, twigSpan * 1.15f + focusSize * 2.6f) * userZoom, 0.48f, 3.8f);
        } else {
            targetPos = Vec3(0.0f, bMin.y + heightSpan * 0.65f, 0.0f);
            camDist = std::clamp(baseFitDist * 0.28f * userZoom, 1.0f, 18.0f);
        }
    } else {
        // Keep ground y=0 root base + contact shadow clearly framed inside lower viewport while crown clears HUD bar
        targetPos.y = bMin.y + heightSpan * 0.48f;
        camDist = std::clamp(baseFitDist * userZoom, 2.8f, 95.0f);
    }

    float azRad = params.camAzimuthDeg * kDegToRad;
    float elDeg = std::clamp(params.camElevationDeg, 8.0f, 75.0f);
    float elRad = elDeg * kDegToRad;

    Vec3 camPos(
        targetPos.x + camDist * std::cos(elRad) * std::sin(azRad),
        std::max(0.65f, targetPos.y + camDist * std::sin(elRad)),
        targetPos.z + camDist * std::cos(elRad) * std::cos(azRad)
    );

    Vec3 fwd = (targetPos - camPos).normalized();
    Vec3 right = Vec3(-fwd.z, 0.0f, fwd.x).normalized();
    Vec3 up = cross(right, fwd).normalized();

    float sunAzRad = params.sunAzimuthDeg * kDegToRad;
    float sunElRad = std::clamp(params.sunElevationDeg, 10.0f, 82.0f) * kDegToRad;
    Vec3 sunDir(
        std::cos(sunElRad) * std::cos(sunAzRad),
        std::sin(sunElRad),
        std::cos(sunElRad) * std::sin(sunAzRad)
    );
    sunDir = sunDir.normalized();

    DepthFrameBuffer fb(width, height, outArgbPixels);
    float centerY = hF * (params.cameraFocus == 0 ? 0.51f : 0.52f);
    fb.setCamera(camPos, fwd, right, up, wF * 0.5f, centerY, fovScale);

    // 4. Fill Dark Studio Backdrop + Grounded Z-Buffered 3D Earth Plane (y = 0)
    float rootBaseRadius = branches.empty() ? std::max(0.15f, params.trunkRadiusMeters) : std::max(0.10f, branches.front().radiusStart);
    fb.fillStudioGroundPlane(sunDir, maxHorizR, rootBaseRadius, heightSpan, false);

    // Grounded root collar flares anchoring woody trunks firmly into y = 0 soil
    // Herbaceous monocots (Banana, Palms, Papaya) emerge cleanly from a smooth basal corm/collar instead of oak root claws
    bool isBananaSpecies = (params.speciesId == 34);
    bool skipWoodyRootClaws = isBananaSpecies || params.speciesId == 36
        || params.leafMorphologyId == 14 || params.leafMorphologyId == 15
        || params.leafMorphologyId == 17 || params.leafMorphologyId == 18;

    if (!skipWoodyRootClaws) {
        for (const auto& seg : branches) {
            if (seg.recursionDepth == 0 && seg.startPos.y <= 0.04f) {
                drawRootCollarAnchor(fb, Vec3(seg.startPos.x, 0.0f, seg.startPos.z), seg.radiusStart, params.barkColor, sunDir, params.showWireframeHull);
            }
        }
    } else if (isBananaSpecies && !branches.empty()) {
        // Subtle smooth fibrous corm base sleeve seated into y = 0 soil (no splayed woody claws)
        const auto& baseSeg = branches.front();
        RgbF cormColor = scaleColor(blendColor(params.barkColor, {0.30f, 0.24f, 0.14f}, 0.55f), 0.84f);
        fb.drawShadedCylinder3D(
            Vec3(baseSeg.startPos.x, -0.025f, baseSeg.startPos.z),
            Vec3(baseSeg.startPos.x, 0.16f, baseSeg.startPos.z),
            std::max(0.05f, baseSeg.radiusStart * 1.12f),
            std::max(0.04f, baseSeg.radiusStart * 0.96f),
            cormColor,
            sunDir,
            0.88f,
            params.showWireframeHull,
            false
        );
    }

    // Optional PlantGL Hull Wireframe
    if (params.showWireframeHull) {
        drawPlantGLHullWireframe(fb, tree->getCrownHullParams(), Vec3(0.0f, 0.0f, 0.0f));
    }

    // 5. Render All Smooth Tapered Shaded Cylindrical Woody Trunks & Herbaceous Pseudostems/Petioles
    for (const auto& seg : branches) {
        float ao = (seg.recursionDepth == 0) ? 1.0f : (seg.recursionDepth == 1 ? 0.94f : 0.88f);
        RgbF segColor = params.barkColor;
        if (isBananaSpecies) {
            if (seg.recursionDepth == 0) {
                segColor = (seg.startPos.y <= 0.28f)
                    ? blendColor(params.barkColor, {0.34f, 0.29f, 0.17f}, 0.38f)
                    : blendColor(params.barkColor, params.crownBaseColor, 0.28f);
            } else {
                segColor = blendColor(params.barkColor, params.crownSunlitColor, 0.48f);
            }
        }
        fb.drawShadedCylinder3D(
            seg.startPos,
            seg.endPos,
            std::max(0.005f, seg.radiusStart),
            std::max(0.004f, seg.radiusEnd),
            segColor,
            sunDir,
            ao,
            params.showWireframeHull,
            true
        );
    }

    // 6. Depth-Sort and Render Real Botanical Leaves & Petioles (Z-Tested + AA)
    float leafAlpha = params.showLeaves ? 1.0f : 0.0f;
    if (params.cameraFocus == 1) leafAlpha *= 0.25f; // Ghost leaves slightly in Branching focus

    if (leafAlpha > 0.02f && !leafItems.empty()) {
        for (auto& li : leafItems) {
            li.depthKey = dot(li.bladeBase - camPos, fwd);
        }
        std::sort(leafItems.begin(), leafItems.end(), [](const LeafRenderItem& a, const LeafRenderItem& b) {
            return a.depthKey > b.depthKey; // Back-to-front for crisp anti-aliased edge blending
        });

        for (const auto& li : leafItems) {
            if (li.depthKey < 0.15f) continue;
            rasterizeBotanicalLeaf(
                fb,
                li,
                params.leafMorphologyId,
                params.crownBaseColor,
                params.crownSunlitColor,
                params.barkColor,
                sunDir,
                leafAlpha,
                isCloseup,
                params.showWireframeHull
            );
        }
    }

    // 7. Render Biologically Accurate Seasonal 3D Blossoms & Trunk/Collar/Twig Fruits
    renderTreeReproductiveOrgans(fb, params, branches, rawLeaves, heightSpan, sunDir, isCloseup);

    if (outStats8) {
        auto tEnd = std::chrono::steady_clock::now();
        float renderMs = std::chrono::duration<float, std::milli>(tEnd - tStart).count();
        outStats8[0] = static_cast<float>(branches.size());
        outStats8[1] = static_cast<float>(rawLeaves.size());
        outStats8[2] = heightSpan;
        outStats8[3] = maxHorizR * 2.0f;
        outStats8[4] = camDist;
        outStats8[5] = params.heightMeters > 0 ? params.heightMeters : heightSpan;
        outStats8[6] = 1.0f;
        outStats8[7] = renderMs;
    }
}

// ============================================================================
// Top-Level C++17 Shrub Scene Entry Point (All 26 Shrub Archetypes)
// ============================================================================

void renderShrubSceneToArgb(
    const BotanicalShrubRenderParams& params,
    int width,
    int height,
    int32_t* outArgbPixels,
    float* outStats8
) {
    if (!outArgbPixels || width <= 4 || height <= 4) return;
    auto tStart = std::chrono::steady_clock::now();

    BushArchetype arch = static_cast<BushArchetype>(std::clamp(params.bushArchetypeId, 0, 25));
    BushSynthesisParams bParams = getPredefinedBushParams(arch);
    if (params.radiusMeters > 0.05f) bParams.radiusMeters = params.radiusMeters;
    if (params.heightMeters > 0.05f) bParams.heightMeters = params.heightMeters;
    if (params.stemCount > 0) bParams.stemBranchCount = params.stemCount;
    if (params.clusterDensity > 0) bParams.leafClusterDensity = params.clusterDensity;
    bParams.windFlexibility = params.windFlexibility;
    bParams.hasAccents = params.showAccents;

    BushGenerator generator(bParams);
    ShrubStructureResult res = generator.synthesizeShrubStructure(params.seed);

    // Apply unified C++ wind sway deflection across shrub canes, twigs, leaves & accents
    auto evalWind = [&](const Vec3& p, float weight) -> Vec3 {
        if (params.windFlexibility <= 0.03f) return {0.0f, 0.0f, 0.0f};
        float hFrac = std::clamp(p.y / std::max(0.4f, bParams.heightMeters), 0.0f, 1.4f);
        float amp = hFrac * hFrac * 0.14f * params.windFlexibility * (0.4f + 0.6f * weight);
        float phase = params.timeSeconds * 2.4f + p.x * 1.3f + p.z * 1.1f;
        return {std::sin(phase) * amp, -0.05f * std::abs(std::sin(phase)) * amp, std::cos(phase * 0.78f) * amp * 0.55f};
    };

    for (auto& br : res.branches) {
        bool isBasal = (br.startPos.y <= 0.03f);
        br.startPos += evalWind(br.startPos, br.windWeight * 0.8f);
        br.startPos.y = isBasal ? -0.01f : std::max(0.015f, br.startPos.y);
        br.endPos += evalWind(br.endPos, br.windWeight);
        br.endPos.y = std::max(0.015f, br.endPos.y);
    }
    for (auto& lf : res.leaves) {
        lf.twigAttachment += evalWind(lf.twigAttachment, 1.0f);
        lf.twigAttachment.y = std::max(0.012f, lf.twigAttachment.y);
    }
    for (auto& ac : res.accents) {
        ac.position += evalWind(ac.position, 1.0f);
        ac.position.y = std::max(0.015f, ac.position.y);
    }

    // Build leaf render items + AABB
    bool isCloseup = (params.cameraFocus == 2);
    bool isFernFrond = (params.leafMorphologyId == 17);
    std::vector<LeafRenderItem> leafItems;
    leafItems.reserve(res.leaves.size());

    Vec3 bMin( 1e9f,  1e9f,  1e9f);
    Vec3 bMax(-1e9f, -1e9f, -1e9f);
    auto expandBox = [&](const Vec3& p, float pad = 0.0f) {
        bMin.x = std::min(bMin.x, p.x - pad);
        bMin.y = std::min(bMin.y, p.y - pad);
        bMin.z = std::min(bMin.z, p.z - pad);
        bMax.x = std::max(bMax.x, p.x + pad);
        bMax.y = std::max(bMax.y, p.y + pad);
        bMax.z = std::max(bMax.z, p.z + pad);
    };

    for (const auto& br : res.branches) {
        expandBox(br.startPos, br.radiusBottom);
        expandBox(br.endPos, br.radiusTop);
    }

    for (size_t i = 0; i < res.leaves.size(); ++i) {
        const auto& lf = res.leaves[i];
        Vec3 M = lf.midribDir.normalized();
        float L = std::max(0.045f, lf.sizeMeters);
        float petioleLen = isFernFrond ? 0.009f : std::clamp(L * 0.22f, 0.007f, 0.045f);
        Vec3 bladeBase = lf.twigAttachment + M * petioleLen;

        Vec3 upRef = (std::abs(M.y) < 0.94f) ? Vec3(0.0f, 1.0f, 0.0f) : Vec3(1.0f, 0.0f, 0.0f);
        Vec3 s0 = cross(M, upRef).normalized();
        Vec3 n0 = cross(s0, M).normalized();
        float roll = isFernFrond
            ? std::sin(static_cast<float>(i) * 2.39996f) * 0.28f
            : (std::sin(static_cast<float>(i) * 2.3999632f + lf.twigAttachment.x * 5.3f) * 1.1f);
        float cR = std::cos(roll), sR = std::sin(roll);
        Vec3 S = (s0 * cR + n0 * sR).normalized();
        Vec3 N = (n0 * cR - s0 * sR).normalized();
        if (N.y < -0.35f) {
            N = N * -1.0f;
            S = S * -1.0f;
        }
        float sunDot = dot(N, Vec3(0.452f, 0.804f, 0.382f));
        float sunlit = clamp01(clamp01((sunDot + 0.35f) / 1.35f) * 0.68f + lf.colorTint * 0.32f);

        LeafRenderItem item;
        item.twigAttach = lf.twigAttachment;
        item.bladeBase = bladeBase;
        item.midribDir = M;
        item.wingDir = S;
        item.normalDir = N;
        item.lengthMeters = L;
        item.petioleMeters = petioleLen;
        item.sunlitFactor = sunlit;
        item.tintVariation = lf.colorTint;
        item.indexSeed = static_cast<uint32_t>(i);
        item.isGiantFrond = isFernFrond;
        leafItems.push_back(item);

        expandBox(bladeBase, L * 0.7f);
        expandBox(bladeBase + M * L, L * 0.4f);
    }

    if (res.branches.empty()) {
        bMin = Vec3(-bParams.radiusMeters, 0.0f, -bParams.radiusMeters);
        bMax = Vec3( bParams.radiusMeters, bParams.heightMeters, bParams.radiusMeters);
    }

    bMin.y = 0.0f;
    float heightSpan = std::max(0.5f, bMax.y - bMin.y);
    float maxHorizR = std::max(0.45f, std::hypot(std::max(std::abs(bMin.x), std::abs(bMax.x)), std::max(std::abs(bMin.z), std::abs(bMax.z))));
    Vec3 boxCenter((bMin.x + bMax.x) * 0.5f, bMin.y + heightSpan * 0.46f, (bMin.z + bMax.z) * 0.5f);

    float wF = static_cast<float>(width);
    float hF = static_cast<float>(height);
    float halfFovTan = std::tan(24.0f * kDegToRad);
    float fovScale = (std::min(wF, hF) * 0.52f) / halfFovTan;

    float fitV = (heightSpan * 0.55f) / std::max(0.1f, (hF * 0.35f / fovScale));
    float fitH = (maxHorizR * 1.10f) / std::max(0.1f, (wF * 0.45f / fovScale));
    float baseFitDist = std::max({1.9f, fitV, fitH});
    float userZoom = std::clamp(params.camDistanceMeters / 3.6f, 0.25f, 4.0f);

    Vec3 targetPos = boxCenter;
    float camDist = baseFitDist * userZoom;

    if (params.cameraFocus == 1) {
        targetPos = Vec3(0.0f, bMin.y + heightSpan * 0.42f, 0.0f);
        camDist = std::clamp(baseFitDist * 0.65f * userZoom, 0.8f, 12.0f);
    } else if (params.cameraFocus == 2) {
        if (params.showAccents && !res.accents.empty()) {
            targetPos = res.accents[0].position;
            camDist = std::clamp(1.25f * userZoom, 0.45f, 4.5f);
        } else if (!leafItems.empty()) {
            size_t bestIdx = 0;
            float bestScore = -1e9f;
            float camAz = params.camAzimuthDeg * kDegToRad;
            Vec3 camDirGuess(std::sin(camAz), 0.45f, std::cos(camAz));
            for (size_t i = 0; i < leafItems.size(); ++i) {
                float score = dot(leafItems[i].bladeBase, camDirGuess) + leafItems[i].sunlitFactor * 2.0f;
                if (score > bestScore) {
                    bestScore = score;
                    bestIdx = i;
                }
            }
            if (bestIdx < res.leaves.size() && res.leaves[bestIdx].parentBranchIndex >= 0 &&
                static_cast<size_t>(res.leaves[bestIdx].parentBranchIndex) < res.branches.size()) {
                const auto& b = res.branches[static_cast<size_t>(res.leaves[bestIdx].parentBranchIndex)];
                float twigLen = (b.endPos - b.startPos).length();
                targetPos = lerp(b.startPos, b.endPos, 0.72f);
                camDist = std::clamp(std::max(0.28f, twigLen * 1.4f + leafItems[bestIdx].lengthMeters * 2.4f) * userZoom, 0.42f, 3.2f);
            } else {
                targetPos = leafItems[bestIdx].bladeBase + leafItems[bestIdx].midribDir * (leafItems[bestIdx].lengthMeters * 0.5f);
                camDist = std::clamp(std::max(0.20f, leafItems[bestIdx].lengthMeters) * 4.0f * userZoom, 0.45f, 3.5f);
            }
        }
    } else {
        targetPos.y = bMin.y + heightSpan * 0.46f;
        camDist = std::clamp(baseFitDist * userZoom, 1.0f, 18.0f);
    }

    float azRad = params.camAzimuthDeg * kDegToRad;
    float elDeg = std::clamp(params.camElevationDeg, 8.0f, 75.0f);
    float elRad = elDeg * kDegToRad;
    Vec3 camPos(
        targetPos.x + camDist * std::cos(elRad) * std::sin(azRad),
        std::max(0.28f, targetPos.y + camDist * std::sin(elRad)),
        targetPos.z + camDist * std::cos(elRad) * std::cos(azRad)
    );
    Vec3 fwd = (targetPos - camPos).normalized();
    Vec3 right = Vec3(-fwd.z, 0.0f, fwd.x).normalized();
    Vec3 up = cross(right, fwd).normalized();
    Vec3 sunDir = Vec3(0.55f, 0.78f, 0.30f).normalized();

    DepthFrameBuffer fb(width, height, outArgbPixels);
    fb.setCamera(camPos, fwd, right, up, wF * 0.5f, hF * 0.50f, fovScale);

    float basalRootR = res.branches.empty() ? std::max(0.06f, bParams.radiusMeters * 0.2f) : std::max(0.04f, res.branches.front().radiusBottom * 1.6f);
    fb.fillStudioGroundPlane(sunDir, maxHorizR, basalRootR, heightSpan, true);
    for (const auto& br : res.branches) {
        if (br.order == 0 && br.startPos.y <= 0.03f) {
            drawRootCollarAnchor(fb, Vec3(br.startPos.x, 0.0f, br.startPos.z), br.radiusBottom, params.stemColor, sunDir, params.showWireframe);
        }
    }

    if (params.showWireframe) {
        CrownControlParams hp;
        hp.trunkHeight = 0.05f;
        hp.crownHeight = heightSpan;
        hp.rxPos = maxHorizR;
        hp.rxNeg = maxHorizR;
        hp.ryPos = maxHorizR;
        hp.ryNeg = maxHorizR;
        drawPlantGLHullWireframe(fb, hp, Vec3(0.0f, 0.0f, 0.0f));
    }

    // Canes & woody stems
    bool showBranches = (params.cameraFocus == 1) ? true : params.showBranches;
    if (showBranches) {
        for (const auto& seg : res.branches) {
            float ao = (seg.order == 0) ? 1.0f : (seg.order == 1 ? 0.94f : 0.88f);
            fb.drawShadedCylinder3D(
                seg.startPos,
                seg.endPos,
                std::max(0.004f, seg.radiusBottom),
                std::max(0.003f, seg.radiusTop),
                params.stemColor,
                sunDir,
                ao,
                params.showWireframe,
                true
            );
            if (params.cameraFocus == 1 && (seg.order == 1 || seg.order == 2)) {
                RgbF nodeCol = (seg.order == 1) ? RgbF{1.0f, 0.75f, 0.20f} : RgbF{0.11f, 0.91f, 0.71f};
                fb.drawShadedSphere3D(seg.startPos, std::max(0.015f, seg.radiusBottom * 1.4f), nodeCol, sunDir, 0.4f, 2.2f, 5.0f);
            }
        }
    }

    // Real Botanical Shrub Leaves
    RgbF baseLeaf = scaleColor(params.foliageColor, 0.66f);
    RgbF sunLeaf = {
        clamp01(params.foliageColor.r * 1.25f + 0.08f),
        clamp01(params.foliageColor.g * 1.22f + 0.06f),
        clamp01(params.foliageColor.b * 0.92f)
    };
    float foliageAlpha = (params.cameraFocus == 1) ? 0.18f : 1.0f;

    if (!leafItems.empty()) {
        for (auto& li : leafItems) li.depthKey = dot(li.bladeBase - camPos, fwd);
        std::sort(leafItems.begin(), leafItems.end(), [](const LeafRenderItem& a, const LeafRenderItem& b) {
            return a.depthKey > b.depthKey;
        });
        for (const auto& li : leafItems) {
            if (li.depthKey < 0.12f) continue;
            rasterizeBotanicalLeaf(
                fb, li, params.leafMorphologyId,
                baseLeaf, sunLeaf, params.stemColor,
                sunDir, foliageAlpha, isCloseup, params.showWireframe
            );
        }
    }

    // Shrub Floral, Berry, Bract, Mophead, Spine & Protea Accents
    if (params.showAccents && !res.accents.empty()) {
        for (const auto& ac : res.accents) {
            rasterizeShrubAccent3D(
                fb,
                ac.position,
                ac.dir,
                ac.sizeMeters,
                static_cast<int>(ac.kind),
                ac.phaseOffset,
                params.accentColor,
                params.foliageColor,
                sunDir,
                isCloseup
            );
        }
    }

    if (outStats8) {
        auto tEnd = std::chrono::steady_clock::now();
        float renderMs = std::chrono::duration<float, std::milli>(tEnd - tStart).count();
        outStats8[0] = static_cast<float>(res.caneCount);
        outStats8[1] = static_cast<float>(res.branches.size());
        outStats8[2] = static_cast<float>(res.leaves.size());
        outStats8[3] = static_cast<float>(res.accents.size());
        outStats8[4] = heightSpan;
        outStats8[5] = maxHorizR * 2.0f;
        outStats8[6] = 1.0f;
        outStats8[7] = renderMs;
    }
}

} // namespace scene
} // namespace vegetation
