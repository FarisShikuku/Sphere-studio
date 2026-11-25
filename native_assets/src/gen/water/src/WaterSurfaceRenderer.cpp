#include "WaterSurfaceRenderer.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>

namespace sphere::water {

namespace {
constexpr float kPi = 3.14159265358979323846f;

inline uint32_t PackRgba(float r, float g, float b) {
    auto toU8 = [](float c) -> uint8_t {
        float mapped = c / (1.0f + std::max(0.0f, c) * 0.32f);
        int iv = static_cast<int>(std::clamp(mapped, 0.0f, 1.0f) * 255.0f + 0.5f);
        return static_cast<uint8_t>(std::clamp(iv, 0, 255));
    };
    return (255u << 24) |
           (static_cast<uint32_t>(toU8(b)) << 16) |
           (static_cast<uint32_t>(toU8(g)) << 8) |
           static_cast<uint32_t>(toU8(r));
}

inline Vec3 SunDirFromAngles(float azDeg, float elDeg) {
    float az = azDeg * (kPi / 180.0f);
    float el = elDeg * (kPi / 180.0f);
    float ce = std::cos(el);
    return Vec3(ce * std::sin(az), std::sin(el), ce * std::cos(az)).Normalized();
}

inline Vec3 RefractRay(const Vec3& incident, const Vec3& normal, float eta) {
    float cosI = -Vec3::Dot(normal, incident);
    float sinT2 = eta * eta * (1.0f - cosI * cosI);
    if (sinT2 > 1.0f) {
        return (incident - normal * (2.0f * Vec3::Dot(incident, normal))).Normalized();
    }
    float cosT = std::sqrt(1.0f - sinT2);
    return (incident * eta + normal * (eta * cosI - cosT)).Normalized();
}

inline float SampleWaveElevation(const WaterSurfaceSimulator& simulator, float x, float z) {
    Vec3 disp = simulator.SampleDisplacement(x, z);
    return simulator.SampleDisplacement(x - disp.x * 0.55f, z - disp.z * 0.55f).y;
}

inline float SampleCutawaySeabedY(
    const WaterSurfaceSimulator& simulator,
    float x,
    float z,
    float yBottom
) {
    float minFloor = yBottom + std::min(0.45f, std::abs(yBottom) * 0.16f);
    float raw = simulator.SampleSeabedHeight(x, z);
    return std::clamp(raw, minFloor, -0.30f);
}

inline bool IntersectRayAabb(
    const Vec3& origin,
    const Vec3& dir,
    const Vec3& boxMin,
    const Vec3& boxMax,
    float& outTNear,
    float& outTFar,
    int& outEnterAxis,
    float& outEnterSign
) {
    float tMin = -1e20f;
    float tMax = 1e20f;
    int enterAxis = 1;
    float enterSign = 1.0f;

    auto testAxis = [&](int axis, float o, float d, float bMin, float bMax) -> bool {
        if (std::abs(d) < 1e-6f) {
            return (o >= bMin && o <= bMax);
        }
        float invD = 1.0f / d;
        float t1 = (bMin - o) * invD;
        float t2 = (bMax - o) * invD;
        float s1 = -1.0f;
        if (t1 > t2) {
            std::swap(t1, t2);
            s1 = 1.0f;
        }
        if (t1 > tMin) {
            tMin = t1;
            enterAxis = axis;
            enterSign = s1;
        }
        if (t2 < tMax) {
            tMax = t2;
        }
        return tMin <= tMax;
    };

    if (!testAxis(0, origin.x, dir.x, boxMin.x, boxMax.x)) return false;
    if (!testAxis(1, origin.y, dir.y, boxMin.y, boxMax.y)) return false;
    if (!testAxis(2, origin.z, dir.z, boxMin.z, boxMax.z)) return false;
    if (tMax < 0.02f) return false;

    outTNear = tMin;
    outTFar = tMax;
    outEnterAxis = enterAxis;
    outEnterSign = enterSign;
    return true;
}

inline float RayExitAabbDistance(
    const Vec3& origin,
    const Vec3& dir,
    const Vec3& boxMin,
    const Vec3& boxMax
) {
    float tMax = 1e20f;
    auto checkAxis = [&](float o, float d, float bMin, float bMax) {
        if (std::abs(d) < 1e-6f) return;
        float invD = 1.0f / d;
        float t1 = (bMin - o) * invD;
        float t2 = (bMax - o) * invD;
        tMax = std::min(tMax, std::max(t1, t2));
    };
    checkAxis(origin.x, dir.x, boxMin.x, boxMax.x);
    checkAxis(origin.y, dir.y, boxMin.y, boxMax.y);
    checkAxis(origin.z, dir.z, boxMin.z, boxMax.z);
    return std::max(0.05f, tMax);
}

inline void DrawLineRgba(
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
    if (!std::isfinite(maxSpan) || maxSpan > 2000.0f) return;
    int steps = static_cast<int>(std::ceil(maxSpan));
    if (steps <= 0 || steps > 2000) return;

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

inline bool ProjectWorldToScreen(
    const Vec3& worldPt,
    const Vec3& camPos,
    const Vec3& camFwd,
    const Vec3& camRight,
    const Vec3& camUp,
    float nearPlane,
    float invAspectTanFov,
    float invTanFov,
    float fw,
    float fh,
    Vec2& outScreen
) {
    Vec3 rel = worldPt - camPos;
    float zCam = Vec3::Dot(rel, camFwd);
    if (zCam <= nearPlane) return false;
    float xCam = Vec3::Dot(rel, camRight);
    float yCam = Vec3::Dot(rel, camUp);
    float ndcX = (xCam / zCam) * invAspectTanFov;
    float ndcY = (yCam / zCam) * invTanFov;
    outScreen = Vec2(0.5f * (ndcX + 1.0f) * fw, 0.5f * (1.0f - ndcY) * fh);
    return true;
}

inline Vec3 ShadeStudioBackdrop(
    const Vec3& rayOrigin,
    const Vec3& rayDir,
    const Vec3& skyCol,
    float yFloor,
    float hx,
    float hz
) {
    if (rayDir.y >= -0.01f) {
        return skyCol;
    }
    float tFloor = (yFloor - rayOrigin.y) / rayDir.y;
    if (tFloor <= 0.05f || tFloor > 140.0f) {
        return skyCol;
    }
    Vec3 p = rayOrigin + rayDir * tFloor;
    float distBoxX = std::max(0.0f, std::abs(p.x) - hx);
    float distBoxZ = std::max(0.0f, std::abs(p.z) - hz);
    float dEdge = std::sqrt(distBoxX * distBoxX + distBoxZ * distBoxZ);
    float shadow = 1.0f - 0.52f * std::exp(-dEdge * 0.55f);
    // Subtle dark architectural scale grid every 2.0m under the cutaway ocean block
    float gx = std::abs(std::sin(p.x * (kPi * 0.5f)));
    float gz = std::abs(std::sin(p.z * (kPi * 0.5f)));
    float gridLine = (gx < 0.03f || gz < 0.03f) ? 1.0f : 0.0f;
    float gridFade = std::exp(-tFloor * 0.035f);
    Vec3 floorCol = Vec3(0.045f, 0.068f, 0.105f) * shadow + Vec3(0.04f, 0.14f, 0.22f) * (gridLine * 0.35f * gridFade);
    float horizonFog = std::clamp((tFloor - 25.0f) / 75.0f, 0.0f, 0.85f);
    return Vec3::Lerp(floorCol, skyCol, horizonFog);
}
} // namespace

float WaterSurfaceRenderer::RaymarchWaterHeightfield(
    const Vec3& rayOrigin,
    const Vec3& rayDir,
    const WaterSurfaceSimulator& simulator,
    float minWaveY,
    float maxWaveY,
    float maxDist,
    Vec3& outHitPos
) const {
    if (rayOrigin.y > maxWaveY && rayDir.y >= -1e-4f) {
        return -1.0f;
    }

    float tStart = 0.05f;
    float tEnd = maxDist;

    if (std::abs(rayDir.y) > 1e-4f) {
        float tTop = (maxWaveY - rayOrigin.y) / rayDir.y;
        float tBot = (minWaveY - rayOrigin.y) / rayDir.y;
        if (tTop > tBot) std::swap(tTop, tBot);
        tStart = std::max(tStart, tTop);
        tEnd = std::min(tEnd, tBot);
    }

    if (tStart > tEnd || tStart > maxDist) return -1.0f;

    constexpr int kSteps = 22;
    float dt = std::max(0.02f, (tEnd - tStart) / static_cast<float>(kSteps));
    float prevT = tStart;

    for (int i = 0; i <= kSteps; ++i) {
        float t = tStart + dt * static_cast<float>(i);
        Vec3 p = rayOrigin + rayDir * t;
        float h = SampleWaveElevation(simulator, p.x, p.z);

        if (p.y <= h) {
            float lo = prevT;
            float hi = t;
            for (int b = 0; b < 5; ++b) {
                float mid = 0.5f * (lo + hi);
                Vec3 mp = rayOrigin + rayDir * mid;
                float mh = SampleWaveElevation(simulator, mp.x, mp.z);
                if (mp.y <= mh) {
                    hi = mid;
                } else {
                    lo = mid;
                }
            }
            outHitPos = rayOrigin + rayDir * hi;
            return hi;
        }
        prevT = t;
    }

    if (rayDir.y < -1e-4f) {
        float tPlane = -rayOrigin.y / rayDir.y;
        if (tPlane > 0.05f && tPlane <= maxDist) {
            outHitPos = rayOrigin + rayDir * tPlane;
            return tPlane;
        }
    }
    return -1.0f;
}

float WaterSurfaceRenderer::RaymarchSeabed(
    const Vec3& rayOrigin,
    const Vec3& rayDir,
    const WaterSurfaceSimulator& simulator,
    Vec3& outHitPos
) const {
    const auto& params = simulator.GetParams();
    float minBedY = -params.waterDepthM - params.seabedRoughness * 1.1f;
    float maxBedY = -0.35f;

    if (rayDir.y >= -1e-4f) {
        outHitPos = rayOrigin + rayDir * 24.0f;
        return 24.0f;
    }

    float tMin = std::max(0.05f, (maxBedY - rayOrigin.y) / rayDir.y);
    float tMax = std::min(48.0f, (minBedY - rayOrigin.y) / rayDir.y);
    if (tMax < tMin) tMax = tMin + 12.0f;

    constexpr int kBedSteps = 14;
    float dt = (tMax - tMin) / static_cast<float>(kBedSteps);
    float prevT = tMin;
    for (int i = 0; i <= kBedSteps; ++i) {
        float t = tMin + dt * static_cast<float>(i);
        Vec3 p = rayOrigin + rayDir * t;
        float bedH = simulator.SampleSeabedHeight(p.x, p.z);
        if (p.y <= bedH) {
            float mid = 0.5f * (prevT + t);
            outHitPos = rayOrigin + rayDir * mid;
            outHitPos.y = simulator.SampleSeabedHeight(outHitPos.x, outHitPos.z);
            return mid;
        }
        prevT = t;
    }
    outHitPos = rayOrigin + rayDir * tMax;
    outHitPos.y = simulator.SampleSeabedHeight(outHitPos.x, outHitPos.z);
    return tMax;
}

Vec3 WaterSurfaceRenderer::ShadeCutawayPixel(
    const Vec3& rayOrigin,
    const Vec3& rayDir,
    const WaterSurfaceSimulator& simulator,
    const Vec3& sunDir,
    const Vec3& skyCol,
    float minWaveY,
    float maxWaveY
) const {
    const auto& params = simulator.GetParams();
    const float hx = std::max(1.0f, params.cutoutWidthM * 0.5f);
    const float hz = std::max(1.0f, params.cutoutDepthM * 0.5f);
    const float yBottom = -std::max(1.5f, params.cutoutHeightM);
    const float yTopBound = std::max(0.45f, maxWaveY + 0.12f);

    const Vec3 boxMin(-hx, yBottom, -hz);
    const Vec3 boxMax(hx, yTopBound, hz);

    float tNear = 0.0f;
    float tFar = 0.0f;
    int enterAxis = 1;
    float enterSign = 1.0f;
    if (!IntersectRayAabb(rayOrigin, rayDir, boxMin, boxMax, tNear, tFar, enterAxis, enterSign)) {
        return ShadeStudioBackdrop(rayOrigin, rayDir, skyCol, yBottom - 0.25f, hx, hz);
    }

    const float tEnter = std::max(0.05f, tNear);
    const Vec3 pEnter = rayOrigin + rayDir * tEnter;
    const float time = simulator.GetElapsedTime();
    const float ior = std::max(1.01f, params.indexOfRefraction);
    const float f0 = std::pow((ior - 1.0f) / (ior + 1.0f), 2.0f);
    const float eta = 1.0f / ior;
    const Vec3 sigmaAbsorb(0.46f, 0.13f, 0.048f);

    const float waveAtEnter = SampleWaveElevation(simulator, pEnter.x, pEnter.z);
    const float bedAtEnter = SampleCutawaySeabedY(simulator, pEnter.x, pEnter.z, yBottom);

    bool hitTopWaterSurface = false;
    Vec3 hitPos = pEnter;
    float hitT = tEnter;

    if (pEnter.y > waveAtEnter) {
        // Ray enters bounding volume above the water wave surface: march to intersect undulating top waves
        constexpr int kSteps = 22;
        float spanT = std::max(0.02f, tFar - tEnter);
        float dt = spanT / static_cast<float>(kSteps);
        float prevT = tEnter;
        for (int i = 1; i <= kSteps; ++i) {
            float t = tEnter + dt * static_cast<float>(i);
            Vec3 p = rayOrigin + rayDir * t;
            float h = SampleWaveElevation(simulator, p.x, p.z);
            if (p.y <= h) {
                float lo = prevT;
                float hi = t;
                for (int b = 0; b < 5; ++b) {
                    float mid = 0.5f * (lo + hi);
                    Vec3 mp = rayOrigin + rayDir * mid;
                    float mh = SampleWaveElevation(simulator, mp.x, mp.z);
                    if (mp.y <= mh) hi = mid; else lo = mid;
                }
                hitT = hi;
                hitPos = rayOrigin + rayDir * hi;
                hitTopWaterSurface = true;
                break;
            }
            prevT = t;
        }
        if (!hitTopWaterSurface) {
            return ShadeStudioBackdrop(rayOrigin, rayDir, skyCol, yBottom - 0.25f, hx, hz);
        }
    }

    if (hitTopWaterSurface || (enterAxis == 1 && enterSign > 0.0f && pEnter.y >= bedAtEnter)) {
        // Top water surface hit inside WxD bounds
        Vec3 disp = simulator.SampleDisplacement(hitPos.x, hitPos.z);
        Vec3 normal;
        float jacobian = 1.0f;
        simulator.SampleNormalAndJacobian(hitPos.x - disp.x * 0.55f, hitPos.z - disp.z * 0.55f, normal, jacobian);
        if (Vec3::Dot(normal, -rayDir) < 0.0f) {
            normal = Vec3(normal.x, std::abs(normal.y), normal.z).Normalized();
        }

        Vec3 viewDir = -rayDir;
        float ndotv = std::clamp(Vec3::Dot(normal, viewDir), 0.0f, 1.0f);
        float fresnel = std::clamp(f0 + (1.0f - f0) * std::pow(1.0f - ndotv, 5.0f), 0.0f, 1.0f);
        float foamMask = std::clamp((params.foamThreshold - jacobian) * 2.6f, 0.0f, 1.0f);

        if (params.debugMode == WaterSurfaceDebugMode::DISPLACEMENT_MAP) {
            return Vec3(
                std::clamp(0.5f + disp.x * 0.35f, 0.0f, 1.0f),
                std::clamp(0.5f + disp.y * 0.55f, 0.0f, 1.0f),
                std::clamp(0.5f + disp.z * 0.35f, 0.0f, 1.0f)
            );
        }
        if (params.debugMode == WaterSurfaceDebugMode::NORMAL_MAP) {
            return normal * 0.5f + Vec3(0.5f, 0.5f, 0.5f);
        }
        if (params.debugMode == WaterSurfaceDebugMode::FRESNEL_AND_FOAM) {
            return Vec3(foamMask, fresnel, 0.15f + 0.55f * fresnel);
        }

        // Trace Snell-refracted ray inside the WxHxD ocean block
        Vec3 refractDir = RefractRay(rayDir, normal, eta);
        float tExitBox = RayExitAabbDistance(hitPos, refractDir, boxMin, boxMax);

        // Find seabed intersection along refracted ray inside [0, tExitBox]
        constexpr int kBedSteps = 12;
        float dtBed = tExitBox / static_cast<float>(kBedSteps);
        float bedT = tExitBox;
        bool hitSeabed = false;
        Vec3 bedPos = hitPos + refractDir * tExitBox;
        for (int i = 1; i <= kBedSteps; ++i) {
            float bt = dtBed * static_cast<float>(i);
            Vec3 bp = hitPos + refractDir * bt;
            float bH = SampleCutawaySeabedY(simulator, bp.x, bp.z, yBottom);
            if (bp.y <= bH) {
                bedT = bt;
                bedPos = bp;
                bedPos.y = bH;
                hitSeabed = true;
                break;
            }
        }

        if (params.debugMode == WaterSurfaceDebugMode::SEABED_DEPTH) {
            float depthNorm = std::clamp(bedT / std::max(2.0f, params.cutoutHeightM * 1.15f), 0.0f, 1.0f);
            return Vec3::Lerp(Vec3(0.28f, 0.92f, 0.84f), Vec3(0.03f, 0.12f, 0.32f), depthNorm);
        }

        Vec3 interiorRadiance;
        if (hitSeabed) {
            float hL = SampleCutawaySeabedY(simulator, bedPos.x - 0.2f, bedPos.z, yBottom);
            float hR = SampleCutawaySeabedY(simulator, bedPos.x + 0.2f, bedPos.z, yBottom);
            float hD = SampleCutawaySeabedY(simulator, bedPos.x, bedPos.z - 0.2f, yBottom);
            float hU = SampleCutawaySeabedY(simulator, bedPos.x, bedPos.z + 0.2f, yBottom);
            Vec3 bedNormal = Vec3(hL - hR, 0.4f, hD - hU).Normalized();
            float bedDiffuse = std::max(0.22f, Vec3::Dot(bedNormal, sunDir));
            float sandRipple = 0.5f + 0.5f * std::sin(bedPos.x * 2.4f + std::cos(bedPos.z * 1.9f));
            Vec3 sandAlbedo = Vec3::Lerp(Vec3(0.58f, 0.54f, 0.42f), Vec3(0.76f, 0.72f, 0.56f), sandRipple);

            Vec3 causticN;
            float causticJ = 1.0f;
            simulator.SampleNormalAndJacobian(bedPos.x + sunDir.x * bedT * 0.4f, bedPos.z + sunDir.z * bedT * 0.4f, causticN, causticJ);
            float causticFocus = std::pow(std::clamp(1.25f - causticJ * 0.85f, 0.0f, 1.4f), 2.0f);
            float causticWave = 0.5f + 0.5f * std::sin(bedPos.x * 3.1f + time * 1.7f) * std::cos(bedPos.z * 2.7f - time * 1.3f);
            float causticBoost = (causticFocus * 0.65f + causticWave * 0.35f) * std::exp(-bedT * 0.14f);
            interiorRadiance = sandAlbedo * bedDiffuse + Vec3(0.45f, 0.72f, 0.76f) * causticBoost * 0.60f;
        } else {
            // Refracted ray exited out the transparent side cut wall of the block
            interiorRadiance = ShadeStudioBackdrop(bedPos, refractDir, WaterSurfaceSimulator::EvaluatePreethamSky(refractDir, sunDir, params.skyTurbidity), yBottom - 0.25f, hx, hz) * 0.90f;
        }

        Vec3 opticalEx = sigmaAbsorb * (bedT * params.absorptionStrength);
        Vec3 transmittance(std::exp(-opticalEx.x), std::exp(-opticalEx.y), std::exp(-opticalEx.z));
        Vec3 deepWaterScatter(0.015f, 0.16f, 0.26f);
        Vec3 refractedColor(
            deepWaterScatter.x + (interiorRadiance.x - deepWaterScatter.x) * transmittance.x,
            deepWaterScatter.y + (interiorRadiance.y - deepWaterScatter.y) * transmittance.y,
            deepWaterScatter.z + (interiorRadiance.z - deepWaterScatter.z) * transmittance.z
        );

        float crestFactor = std::clamp((hitPos.y + 0.1f) * 1.2f, 0.0f, 1.0f);
        Vec3 scatterDir = (sunDir + normal * 0.55f).Normalized();
        float sssGlow = std::pow(std::clamp(Vec3::Dot(viewDir, -scatterDir), 0.0f, 1.0f), 3.0f);
        refractedColor += Vec3(0.06f, 0.52f, 0.48f) * (sssGlow * 0.85f + crestFactor * 0.22f) * params.scatterStrength;

        Vec3 reflectDir = (rayDir - normal * (2.0f * Vec3::Dot(rayDir, normal))).Normalized();
        reflectDir.y = std::max(0.02f, reflectDir.y);
        Vec3 reflectedSky = WaterSurfaceSimulator::EvaluatePreethamSky(reflectDir, sunDir, params.skyTurbidity) *
                            params.reflectionStrength;

        Vec3 halfVec = (sunDir + viewDir).Normalized();
        float sunSpec = std::pow(std::max(0.0f, Vec3::Dot(normal, halfVec)), 240.0f) * 2.6f;
        Vec3 waterCol = Vec3::Lerp(refractedColor, reflectedSky, fresnel) + Vec3(1.0f, 0.94f, 0.82f) * sunSpec;

        Vec3 foamCol(0.92f, 0.96f, 0.98f);
        waterCol = Vec3::Lerp(waterCol, foamCol * (0.55f + 0.45f * std::max(0.2f, sunDir.y)), foamMask * 0.88f);

        // Subtle glass-cut rim highlight right along the horizontal boundary of the cut patch
        float edgeDist = std::min(hx - std::abs(hitPos.x), hz - std::abs(hitPos.z));
        if (edgeDist < 0.09f) {
            waterCol = Vec3::Lerp(waterCol, Vec3(0.55f, 0.90f, 1.0f), 0.38f);
        }
        return waterCol;
    }

    // Ray hit a vertical or bottom cutaway face!
    Vec3 wallNormal(0.0f, 0.0f, 0.0f);
    if (enterAxis == 0) wallNormal.x = enterSign;
    else if (enterAxis == 1) wallNormal.y = enterSign;
    else wallNormal.z = enterSign;

    if (pEnter.y > bedAtEnter) {
        // Cross-section through the translucent ocean water column on the side cut wall
        float depthBelowWave = std::max(0.0f, waveAtEnter - pEnter.y);
        if (params.debugMode == WaterSurfaceDebugMode::DISPLACEMENT_MAP) {
            Vec3 disp = simulator.SampleDisplacement(pEnter.x, pEnter.z);
            return Vec3(
                std::clamp(0.45f + disp.x * 0.35f, 0.0f, 1.0f),
                std::clamp(0.45f + disp.y * 0.55f - depthBelowWave * 0.04f, 0.0f, 1.0f),
                std::clamp(0.55f + disp.z * 0.35f, 0.0f, 1.0f)
            );
        }
        if (params.debugMode == WaterSurfaceDebugMode::NORMAL_MAP) {
            return wallNormal * 0.45f + Vec3(0.5f, 0.5f, 0.5f);
        }
        if (params.debugMode == WaterSurfaceDebugMode::SEABED_DEPTH) {
            float depthNorm = std::clamp(depthBelowWave / std::max(1.5f, params.cutoutHeightM), 0.0f, 1.0f);
            return Vec3::Lerp(Vec3(0.28f, 0.92f, 0.84f), Vec3(0.03f, 0.12f, 0.32f), depthNorm);
        }

        Vec3 viewDir = -rayDir;
        Vec3 refractDir = RefractRay(rayDir, wallNormal, eta);
        float tExitWater = RayExitAabbDistance(pEnter, refractDir, boxMin, boxMax);

        constexpr int kSteps = 10;
        float dt = tExitWater / static_cast<float>(kSteps);
        bool hitSeabedInside = false;
        float tTrace = tExitWater;
        Vec3 traceEnd = pEnter + refractDir * tExitWater;
        for (int i = 1; i <= kSteps; ++i) {
            float t = dt * static_cast<float>(i);
            Vec3 p = pEnter + refractDir * t;
            float bH = SampleCutawaySeabedY(simulator, p.x, p.z, yBottom);
            if (p.y <= bH) {
                tTrace = t;
                traceEnd = p;
                traceEnd.y = bH;
                hitSeabedInside = true;
                break;
            }
        }

        Vec3 behindRadiance;
        if (hitSeabedInside) {
            float hL = SampleCutawaySeabedY(simulator, traceEnd.x - 0.2f, traceEnd.z, yBottom);
            float hR = SampleCutawaySeabedY(simulator, traceEnd.x + 0.2f, traceEnd.z, yBottom);
            float hD = SampleCutawaySeabedY(simulator, traceEnd.x, traceEnd.z - 0.2f, yBottom);
            float hU = SampleCutawaySeabedY(simulator, traceEnd.x, traceEnd.z + 0.2f, yBottom);
            Vec3 bN = Vec3(hL - hR, 0.4f, hD - hU).Normalized();
            float bDiff = std::max(0.20f, Vec3::Dot(bN, sunDir));
            Vec3 sand = Vec3(0.66f, 0.62f, 0.48f) * bDiff;
            float cWave = 0.5f + 0.5f * std::sin(traceEnd.x * 3.1f + time * 1.7f) * std::cos(traceEnd.z * 2.7f - time * 1.3f);
            behindRadiance = sand + Vec3(0.32f, 0.56f, 0.64f) * (cWave * std::exp(-tTrace * 0.14f) * 0.35f);
        } else {
            behindRadiance = ShadeStudioBackdrop(
                traceEnd,
                refractDir,
                WaterSurfaceSimulator::EvaluatePreethamSky(refractDir, sunDir, params.skyTurbidity),
                yBottom - 0.25f,
                hx,
                hz
            );
        }

        Vec3 opticalEx = sigmaAbsorb * (tTrace * params.absorptionStrength);
        Vec3 transmittance(std::exp(-opticalEx.x), std::exp(-opticalEx.y), std::exp(-opticalEx.z));
        Vec3 deepScatter(0.015f, 0.16f, 0.26f);
        Vec3 col(
            deepScatter.x + (behindRadiance.x - deepScatter.x) * transmittance.x,
            deepScatter.y + (behindRadiance.y - deepScatter.y) * transmittance.y,
            deepScatter.z + (behindRadiance.z - deepScatter.z) * transmittance.z
        );

        // Volumetric sun shafts & SSS near top wave crest inside cut cross-section
        float shaftWave = 0.5f + 0.5f * std::sin((pEnter.x + pEnter.z) * 2.2f + time * 1.9f);
        float topShaft = std::exp(-depthBelowWave * 0.45f) * (0.28f + 0.42f * shaftWave);
        col += Vec3(0.08f, 0.50f, 0.56f) * topShaft * params.scatterStrength;

        // Subtle glass cut Fresnel reflection + meniscus contour along top wave profile
        float wallNdotV = std::clamp(Vec3::Dot(wallNormal, viewDir), 0.0f, 1.0f);
        float wallFresnel = 0.04f + 0.32f * std::pow(1.0f - wallNdotV, 4.0f);
        Vec3 reflDir = (rayDir - wallNormal * (2.0f * Vec3::Dot(rayDir, wallNormal))).Normalized();
        Vec3 wallSky = WaterSurfaceSimulator::EvaluatePreethamSky(reflDir, sunDir, params.skyTurbidity);
        col = Vec3::Lerp(col, wallSky, wallFresnel * 0.55f * params.reflectionStrength);

        if (depthBelowWave < 0.12f) {
            col = Vec3::Lerp(col, Vec3(0.72f, 0.94f, 1.0f), 0.55f);
        }
        return col;
    }

    // Solid Value-Noise seabed geological strata cross-section slab below bedAtEnter
    float depthInsideRock = std::max(0.0f, bedAtEnter - pEnter.y);
    float strata = 0.5f + 0.5f * std::sin(pEnter.y * 5.2f + std::sin((pEnter.x + pEnter.z) * 1.4f));
    Vec3 sandRock = Vec3::Lerp(Vec3(0.32f, 0.28f, 0.22f), Vec3(0.20f, 0.21f, 0.24f), std::clamp(depthInsideRock * 0.32f, 0.0f, 1.0f));
    Vec3 strataTint = Vec3::Lerp(sandRock * 0.85f, sandRock * 1.15f, strata);
    float wallLit = std::max(0.28f, Vec3::Dot(wallNormal, sunDir) * 0.65f + 0.42f);
    Vec3 rockCol = strataTint * wallLit;
    // Highlight the seabed surface seam
    if (depthInsideRock < 0.12f) {
        rockCol = Vec3::Lerp(rockCol, Vec3(0.68f, 0.64f, 0.50f), 0.45f);
    }
    return rockCol;
}

void WaterSurfaceRenderer::Render(
    uint32_t* outPixelsRGBA8888,
    int32_t stridePixels,
    uint32_t width,
    uint32_t height,
    const WaterSurfaceSimulator& simulator,
    const OrbitalCamera& camera
) {
    if (!outPixelsRGBA8888 || width == 0 || height == 0 || stridePixels < static_cast<int32_t>(width)) return;

    auto t0 = std::chrono::steady_clock::now();

    const auto& params = simulator.GetParams();
    const Vec3 camPos = camera.GetPosition();
    const Vec3 camFwd = camera.GetForward();
    const Vec3 camRight = camera.GetRight();
    const Vec3 camUp = camera.GetUp();
    const float tanHalf = camera.GetTanHalfFov();
    const float aspect = camera.GetAspect();
    const Vec3 sunDir = SunDirFromAngles(params.sunAzimuthDeg, params.sunElevationDeg);
    const float minWaveY = simulator.GetMinWaveY();
    const float maxWaveY = simulator.GetMaxWaveY();
    const float time = simulator.GetElapsedTime();

    const float ior = std::max(1.01f, params.indexOfRefraction);
    const float f0 = std::pow((ior - 1.0f) / (ior + 1.0f), 2.0f);
    const float eta = 1.0f / ior;
    const Vec3 sigmaAbsorb(0.46f, 0.13f, 0.048f);

    const float invW = 1.0f / static_cast<float>(width);
    const float invH = 1.0f / static_cast<float>(height);
    const float maxHorizonDist = 96.0f;

    for (uint32_t y = 0; y < height; ++y) {
        float ndcY = (1.0f - (static_cast<float>(y) + 0.5f) * invH * 2.0f) * tanHalf;
        uint32_t* row = outPixelsRGBA8888 + static_cast<size_t>(y) * static_cast<size_t>(stridePixels);

        for (uint32_t x = 0; x < width; ++x) {
            float ndcX = ((static_cast<float>(x) + 0.5f) * invW * 2.0f - 1.0f) * aspect * tanHalf;
            Vec3 rayDir = (camFwd + camRight * ndcX + camUp * ndcY).Normalized();

            Vec3 skyCol = WaterSurfaceSimulator::EvaluatePreethamSky(rayDir, sunDir, params.skyTurbidity);
            if (params.cutoutEnabled) {
                Vec3 col = ShadeCutawayPixel(camPos, rayDir, simulator, sunDir, skyCol, minWaveY, maxWaveY);
                row[x] = PackRgba(col.x, col.y, col.z);
                continue;
            }

            Vec3 hitPos;
            float hitT = RaymarchWaterHeightfield(camPos, rayDir, simulator, minWaveY, maxWaveY, maxHorizonDist, hitPos);

            if (hitT < 0.0f) {
                row[x] = PackRgba(skyCol.x, skyCol.y, skyCol.z);
                continue;
            }

            Vec3 disp = simulator.SampleDisplacement(hitPos.x, hitPos.z);
            Vec3 normal;
            float jacobian = 1.0f;
            simulator.SampleNormalAndJacobian(hitPos.x - disp.x * 0.55f, hitPos.z - disp.z * 0.55f, normal, jacobian);

            float distFade = std::exp(-hitT * 0.022f);
            normal = Vec3::Lerp(Vec3(0.0f, 1.0f, 0.0f), normal, distFade).Normalized();
            if (Vec3::Dot(normal, -rayDir) < 0.0f) {
                normal = Vec3(normal.x, std::abs(normal.y), normal.z).Normalized();
            }

            Vec3 viewDir = -rayDir;
            float ndotv = std::clamp(Vec3::Dot(normal, viewDir), 0.0f, 1.0f);
            float fresnel = std::clamp(f0 + (1.0f - f0) * std::pow(1.0f - ndotv, 5.0f), 0.0f, 1.0f);
            float foamMask = std::clamp((params.foamThreshold - jacobian) * 2.6f, 0.0f, 1.0f) * distFade;

            if (params.debugMode == WaterSurfaceDebugMode::DISPLACEMENT_MAP) {
                Vec3 vis(
                    std::clamp(0.5f + disp.x * 0.35f, 0.0f, 1.0f),
                    std::clamp(0.5f + disp.y * 0.55f, 0.0f, 1.0f),
                    std::clamp(0.5f + disp.z * 0.35f, 0.0f, 1.0f)
                );
                row[x] = PackRgba(vis.x, vis.y, vis.z);
                continue;
            }
            if (params.debugMode == WaterSurfaceDebugMode::NORMAL_MAP) {
                Vec3 vis = normal * 0.5f + Vec3(0.5f, 0.5f, 0.5f);
                row[x] = PackRgba(vis.x, vis.y, vis.z);
                continue;
            }
            if (params.debugMode == WaterSurfaceDebugMode::FRESNEL_AND_FOAM) {
                Vec3 vis(foamMask, fresnel, 0.15f + 0.55f * fresnel);
                row[x] = PackRgba(vis.x, vis.y, vis.z);
                continue;
            }

            Vec3 refractDir = RefractRay(rayDir, normal, eta);
            Vec3 bedPos;
            float bedT = RaymarchSeabed(hitPos, refractDir, simulator, bedPos);

            if (params.debugMode == WaterSurfaceDebugMode::SEABED_DEPTH) {
                float depthNorm = std::clamp(bedT / std::max(2.0f, params.waterDepthM * 1.8f), 0.0f, 1.0f);
                Vec3 shallow(0.28f, 0.92f, 0.84f);
                Vec3 trench(0.03f, 0.12f, 0.32f);
                Vec3 vis = Vec3::Lerp(shallow, trench, depthNorm);
                row[x] = PackRgba(vis.x, vis.y, vis.z);
                continue;
            }

            float hL = simulator.SampleSeabedHeight(bedPos.x - 0.25f, bedPos.z);
            float hR = simulator.SampleSeabedHeight(bedPos.x + 0.25f, bedPos.z);
            float hD = simulator.SampleSeabedHeight(bedPos.x, bedPos.z - 0.25f);
            float hU = simulator.SampleSeabedHeight(bedPos.x, bedPos.z + 0.25f);
            Vec3 bedNormal = Vec3(hL - hR, 0.5f, hD - hU).Normalized();
            float bedDiffuse = std::max(0.18f, Vec3::Dot(bedNormal, sunDir));

            float sandRipple = 0.5f + 0.5f * std::sin(bedPos.x * 2.4f + std::cos(bedPos.z * 1.9f));
            Vec3 sandAlbedo = Vec3::Lerp(Vec3(0.58f, 0.54f, 0.42f), Vec3(0.76f, 0.72f, 0.56f), sandRipple);

            Vec3 causticN;
            float causticJ = 1.0f;
            simulator.SampleNormalAndJacobian(bedPos.x + sunDir.x * bedT * 0.4f, bedPos.z + sunDir.z * bedT * 0.4f, causticN, causticJ);
            float causticFocus = std::pow(std::clamp(1.25f - causticJ * 0.85f, 0.0f, 1.4f), 2.0f);
            float causticWave = 0.5f + 0.5f * std::sin(bedPos.x * 3.1f + time * 1.7f) * std::cos(bedPos.z * 2.7f - time * 1.3f);
            float causticBoost = (causticFocus * 0.65f + causticWave * 0.35f) * std::exp(-bedT * 0.14f);

            Vec3 bedRadiance = sandAlbedo * bedDiffuse + Vec3(0.45f, 0.68f, 0.72f) * causticBoost * 0.55f;

            Vec3 opticalEx = sigmaAbsorb * (bedT * params.absorptionStrength);
            Vec3 transmittance(std::exp(-opticalEx.x), std::exp(-opticalEx.y), std::exp(-opticalEx.z));
            Vec3 deepWaterScatter(0.015f, 0.16f, 0.26f);
            Vec3 refractedColor(
                deepWaterScatter.x + (bedRadiance.x - deepWaterScatter.x) * transmittance.x,
                deepWaterScatter.y + (bedRadiance.y - deepWaterScatter.y) * transmittance.y,
                deepWaterScatter.z + (bedRadiance.z - deepWaterScatter.z) * transmittance.z
            );

            float crestFactor = std::clamp((hitPos.y + 0.1f) * 1.2f, 0.0f, 1.0f);
            Vec3 scatterDir = (sunDir + normal * 0.55f).Normalized();
            float sssGlow = std::pow(std::clamp(Vec3::Dot(viewDir, -scatterDir), 0.0f, 1.0f), 3.0f);
            refractedColor += Vec3(0.06f, 0.52f, 0.48f) * (sssGlow * 0.85f + crestFactor * 0.22f) * params.scatterStrength;

            Vec3 reflectDir = (rayDir - normal * (2.0f * Vec3::Dot(rayDir, normal))).Normalized();
            reflectDir.y = std::max(0.02f, reflectDir.y);
            Vec3 reflectedSky = WaterSurfaceSimulator::EvaluatePreethamSky(reflectDir, sunDir, params.skyTurbidity) *
                                params.reflectionStrength;

            Vec3 halfVec = (sunDir + viewDir).Normalized();
            float ndoth = std::max(0.0f, Vec3::Dot(normal, halfVec));
            float sunSpec = std::pow(ndoth, 240.0f) * 2.6f * distFade;
            Vec3 sunGlint = Vec3(1.0f, 0.94f, 0.82f) * sunSpec;

            Vec3 waterCol = Vec3::Lerp(refractedColor, reflectedSky, fresnel) + sunGlint;

            Vec3 foamCol(0.92f, 0.96f, 0.98f);
            waterCol = Vec3::Lerp(waterCol, foamCol * (0.55f + 0.45f * std::max(0.2f, sunDir.y)), foamMask * 0.88f);

            float fog = 1.0f - std::exp(-hitT * 0.016f);
            Vec3 finalCol = Vec3::Lerp(waterCol, skyCol, std::clamp(fog, 0.0f, 0.92f));

            row[x] = PackRgba(finalCol.x, finalCol.y, finalCol.z);
        }
    }

    if (params.debugMode == WaterSurfaceDebugMode::WIREFRAME_GRID || params.cutoutEnabled) {
        RenderDisplacedGridOverlay(outPixelsRGBA8888, stridePixels, width, height, simulator, camera);
    }

    auto t1 = std::chrono::steady_clock::now();
    m_LastRenderMs = std::chrono::duration<float, std::milli>(t1 - t0).count();
}

void WaterSurfaceRenderer::RenderDisplacedGridOverlay(
    uint32_t* outPixelsRGBA8888,
    int32_t stridePixels,
    uint32_t width,
    uint32_t height,
    const WaterSurfaceSimulator& simulator,
    const OrbitalCamera& camera
) const {
    const auto& params = simulator.GetParams();
    const Vec3 camPos = camera.GetPosition();
    const Vec3 camFwd = camera.GetForward();
    const Vec3 camRight = camera.GetRight();
    const Vec3 camUp = camera.GetUp();
    const float nearPlane = camera.GetNearPlane();
    const float invTanFov = 1.0f / std::max(1e-4f, camera.GetTanHalfFov());
    const float invAspectTanFov = invTanFov / std::max(1e-4f, camera.GetAspect());
    const float fw = static_cast<float>(width);
    const float fh = static_cast<float>(height);
    const int32_t w = static_cast<int32_t>(width);
    const int32_t h = static_cast<int32_t>(height);

    // Draw clean cutaway cube frame when in Cutaway Block mode
    if (params.cutoutEnabled) {
        const float hx = std::max(1.0f, params.cutoutWidthM * 0.5f);
        const float hz = std::max(1.0f, params.cutoutDepthM * 0.5f);
        const float yBot = -std::max(1.5f, params.cutoutHeightM);

        Vec3 cornersBot[4] = {
            Vec3(-hx, yBot, -hz),
            Vec3(hx, yBot, -hz),
            Vec3(hx, yBot, hz),
            Vec3(-hx, yBot, hz)
        };
        Vec3 cornersTop[4] = {
            Vec3(-hx, SampleWaveElevation(simulator, -hx, -hz), -hz),
            Vec3(hx, SampleWaveElevation(simulator, hx, -hz), -hz),
            Vec3(hx, SampleWaveElevation(simulator, hx, hz), hz),
            Vec3(-hx, SampleWaveElevation(simulator, -hx, hz), hz)
        };

        Vec2 sBot[4], sTop[4];
        bool vBot[4], vTop[4];
        for (int i = 0; i < 4; ++i) {
            vBot[i] = ProjectWorldToScreen(cornersBot[i], camPos, camFwd, camRight, camUp, nearPlane, invAspectTanFov, invTanFov, fw, fh, sBot[i]);
            vTop[i] = ProjectWorldToScreen(cornersTop[i], camPos, camFwd, camRight, camUp, nearPlane, invAspectTanFov, invTanFov, fw, fh, sTop[i]);
        }
        for (int i = 0; i < 4; ++i) {
            int next = (i + 1) & 3;
            if (vBot[i] && vBot[next]) {
                DrawLineRgba(outPixelsRGBA8888, stridePixels, w, h, sBot[i].x, sBot[i].y, sBot[next].x, sBot[next].y, 100, 210, 255, 0.45f);
            }
            if (vBot[i] && vTop[i]) {
                DrawLineRgba(outPixelsRGBA8888, stridePixels, w, h, sBot[i].x, sBot[i].y, sTop[i].x, sTop[i].y, 100, 210, 255, 0.40f);
            }
        }
    }

    if (params.debugMode != WaterSurfaceDebugMode::WIREFRAME_GRID) {
        return;
    }

    constexpr int kGridN = 20;
    const float spanX = params.cutoutEnabled ? std::max(2.0f, params.cutoutWidthM) : 10.0f;
    const float spanZ = params.cutoutEnabled ? std::max(2.0f, params.cutoutDepthM) : 10.0f;
    const float stepX = spanX / static_cast<float>(kGridN);
    const float stepZ = spanZ / static_cast<float>(kGridN);

    Vec2 screenPts[(kGridN + 1) * (kGridN + 1)];
    bool visible[(kGridN + 1) * (kGridN + 1)];

    for (int gz = 0; gz <= kGridN; ++gz) {
        float wz = -spanZ * 0.5f + static_cast<float>(gz) * stepZ;
        for (int gx = 0; gx <= kGridN; ++gx) {
            float wx = -spanX * 0.5f + static_cast<float>(gx) * stepX;
            Vec3 worldPt;
            if (params.cutoutEnabled) {
                worldPt = Vec3(wx, SampleWaveElevation(simulator, wx, wz), wz);
            } else {
                Vec3 disp = simulator.SampleDisplacement(wx, wz);
                worldPt = Vec3(wx + disp.x, disp.y, wz + disp.z);
            }
            int idx = gz * (kGridN + 1) + gx;
            visible[idx] = ProjectWorldToScreen(worldPt, camPos, camFwd, camRight, camUp, nearPlane, invAspectTanFov, invTanFov, fw, fh, screenPts[idx]);
        }
    }

    for (int gz = 0; gz <= kGridN; ++gz) {
        for (int gx = 0; gx <= kGridN; ++gx) {
            int idx = gz * (kGridN + 1) + gx;
            if (!visible[idx]) continue;
            if (gx < kGridN) {
                int right = idx + 1;
                if (visible[right]) {
                    DrawLineRgba(outPixelsRGBA8888, stridePixels, w, h,
                                 screenPts[idx].x, screenPts[idx].y,
                                 screenPts[right].x, screenPts[right].y,
                                 56, 224, 255, 0.72f);
                }
            }
            if (gz < kGridN) {
                int down = idx + (kGridN + 1);
                if (visible[down]) {
                    DrawLineRgba(outPixelsRGBA8888, stridePixels, w, h,
                                 screenPts[idx].x, screenPts[idx].y,
                                 screenPts[down].x, screenPts[down].y,
                                 56, 224, 255, 0.72f);
                }
            }
        }
    }
}

} // namespace sphere::water
