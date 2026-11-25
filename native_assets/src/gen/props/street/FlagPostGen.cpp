#include "FlagPostGen.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace procedural::props {

namespace {
inline float clampF(float v, float lo, float hi) {
    return std::max(lo, std::min(hi, v));
}

// Official Republic of Kenya Flag Palette
constexpr uint32_t kKenyaBlack  = 0xFF15171A;
constexpr uint32_t kKenyaWhite  = 0xFFF4F6F8;
constexpr uint32_t kKenyaRed    = 0xFFBA0C0C;
constexpr uint32_t kKenyaGreen  = 0xFF006B2B;

// Architectural hardware & pedestal colors
constexpr uint32_t kGraniteDark  = 0xFF475569;
constexpr uint32_t kGraniteLight = 0xFF94A3B8;
constexpr uint32_t kPoleMetal    = 0xFFD8E0E8;
constexpr uint32_t kPoleCollar   = 0xFF64748B;
constexpr uint32_t kGoldFinial   = 0xFFEAB308;
constexpr uint32_t kBrassTrim    = 0xFFD97706;
constexpr uint32_t kHalyardRope  = 0xFFE2E8F0;
constexpr uint32_t kUplightBody  = 0xFF1E293B;
constexpr uint32_t kUplightGlow  = 0xFFFFF3C4;
} // namespace

FlagPostGen::FlagPostGen() = default;

bool FlagPostGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void FlagPostGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload FlagPostGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    const float poleH = clampF(params_.poleHeight, 3.5f, 16.0f);
    const float baseR = clampF(params_.poleRadius, 0.04f, 0.16f);
    const float topR = baseR * 0.56f;

    // 3:2 Kenyan flag dimensions, bounded sensibly relative to pole height
    const float maxFlagLen = poleH * 0.58f;
    const float flagLen = clampF(params_.flagLength, 1.0f, std::max(1.2f, maxFlagLen));
    const float flagH = flagLen * (2.0f / 3.0f);

    const int mountStyle = std::max(0, std::min(2, params_.mountStyle));
    const int windMode = std::max(0, std::min(2, params_.windMode));

    // =========================================================================
    // 1. ARCHITECTURAL PEDESTAL & BASE MOUNTING HARDWARE
    // =========================================================================
    float shaftBaseY = 0.12f;

    if (mountStyle == 0) {
        // Plaza Monument: two-stepped square stone plinth + bolted steel flange
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.11f, 0.0f}, Vec3{1.05f, 0.22f, 1.05f}, kGraniteDark);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.31f, 0.0f}, Vec3{0.72f, 0.18f, 0.72f}, kGraniteLight);
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.40f, 0.0f}, baseR * 2.2f, baseR * 1.9f, 0.06f, 24, kPoleCollar);
        for (int b = 0; b < 6; ++b) {
            float a = b * (2.0f * PropMeshHelper::kPi / 6.0f);
            float br = baseR * 1.75f;
            PropMeshHelper::appendCylinder(
                payload,
                Vec3{std::cos(a) * br, 0.46f, std::sin(a) * br},
                0.018f, 0.018f, 0.028f, 6, kPoleMetal
            );
        }
        shaftBaseY = 0.46f;
    } else if (mountStyle == 1) {
        // Ceremonial Dais: circular stepped stone platform + flared collar shroud
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.0f, 0.0f}, 0.85f, 0.82f, 0.18f, 36, kGraniteDark);
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.18f, 0.0f}, 0.56f, 0.53f, 0.16f, 36, kGraniteLight);
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.34f, 0.0f}, baseR * 2.1f, baseR * 1.35f, 0.22f, 32, kPoleCollar);
        shaftBaseY = 0.56f;
    } else {
        // Boulevard Sidewalk Flange: compact steel baseplate & tapered shroud sleeve
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.0f, 0.0f}, baseR * 2.8f, baseR * 2.8f, 0.045f, 24, kGraniteDark);
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.045f, 0.0f}, baseR * 1.95f, baseR * 1.15f, 0.18f, 24, kPoleCollar);
        for (int b = 0; b < 4; ++b) {
            float a = b * (PropMeshHelper::kPi * 0.5f) + 0.39f;
            float br = baseR * 2.25f;
            PropMeshHelper::appendCylinder(
                payload,
                Vec3{std::cos(a) * br, 0.045f, std::sin(a) * br},
                0.016f, 0.016f, 0.03f, 6, kPoleMetal
            );
        }
        shaftBaseY = 0.22f;
    }

    // Optional base architectural uplights & horn cleat
    if (params_.hasUplightsAndCleat) {
        const float lightDist = (mountStyle == 2) ? (baseR * 3.4f) : 0.42f;
        const float lightBaseY = (mountStyle == 0) ? 0.22f : (mountStyle == 1) ? 0.18f : 0.0f;
        for (int l = 0; l < 2; ++l) {
            float angle = (l == 0) ? 0.65f : -2.49f;
            Vec3 basePos{std::cos(angle) * lightDist, lightBaseY, std::sin(angle) * lightDist};
            Vec3 topPos = basePos + Vec3{-std::cos(angle) * 0.05f, 0.13f, -std::sin(angle) * 0.05f};
            PropMeshHelper::appendOrientedCylinder(payload, basePos, topPos, 0.042f, 0.048f, 16, kUplightBody);
            PropMeshHelper::appendOrientedCylinder(payload, topPos, topPos + Vec3{-std::cos(angle) * 0.012f, 0.018f, -std::sin(angle) * 0.012f}, 0.040f, 0.040f, 16, kUplightGlow);
        }

        // Halyard cleat on pole front/side at ~1.35m height
        const float cleatY = std::min(poleH * 0.35f, 1.40f);
        const float cleatX = baseR * 0.92f;
        PropMeshHelper::appendBox(payload, Vec3{cleatX + 0.015f, cleatY, 0.0f}, Vec3{0.03f, 0.08f, 0.025f}, kBrassTrim);
        PropMeshHelper::appendOrientedCylinder(
            payload,
            Vec3{cleatX + 0.04f, cleatY - 0.10f, 0.0f},
            Vec3{cleatX + 0.04f, cleatY + 0.10f, 0.0f},
            0.011f, 0.011f, 10, kPoleMetal
        );
    }

    // =========================================================================
    // 2. TAPERED POLE MAST, JOINT COLLARS, REVOLVING TRUCK & GOLD FINIAL
    // =========================================================================
    const float mastLen = std::max(1.5f, poleH - shaftBaseY - 0.22f);
    PropMeshHelper::appendCylinder(
        payload, Vec3{0.0f, shaftBaseY, 0.0f}, baseR, topR, mastLen, 32, kPoleMetal
    );

    const float mastTopY = shaftBaseY + mastLen;

    // Subtle mast section collar rings on taller flagpoles
    if (mastLen > 5.0f) {
        for (int s = 1; s <= 2; ++s) {
            float frac = static_cast<float>(s) / 3.0f;
            float ringY = shaftBaseY + mastLen * frac;
            float ringR = baseR * (1.0f - frac) + topR * frac;
            PropMeshHelper::appendCylinder(payload, Vec3{0.0f, ringY, 0.0f}, ringR * 1.12f, ringR * 1.12f, 0.035f, 24, kPoleCollar);
        }
    }

    // Revolving truck pulley head + finial neck + gold ball finial
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, mastTopY, 0.0f}, topR * 1.45f, topR * 1.25f, 0.09f, 24, kPoleCollar);
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, mastTopY + 0.09f, 0.0f}, topR * 0.55f, topR * 0.45f, 0.07f, 20, kBrassTrim);
    const float finialRadius = std::max(0.075f, topR * 1.95f);
    PropMeshHelper::appendSphere(payload, Vec3{0.0f, mastTopY + 0.16f + finialRadius * 0.85f, 0.0f}, finialRadius, 24, 32, kGoldFinial);

    // Pulley wheel & vertical halyard line
    const float halyardX = topR + 0.025f;
    const float flagTopY = mastTopY - 0.06f;
    const float flagBotY = flagTopY - flagH;
    const float halyardBotY = params_.hasUplightsAndCleat ? std::min(poleH * 0.35f, 1.40f) : (shaftBaseY + 0.3f);

    PropMeshHelper::appendOrientedCylinder(
        payload,
        Vec3{halyardX, halyardBotY, 0.012f},
        Vec3{halyardX, mastTopY + 0.04f, 0.012f},
        0.007f, 0.007f, 8, kHalyardRope
    );
    PropMeshHelper::appendOrientedCylinder(
        payload,
        Vec3{halyardX + 0.016f, halyardBotY, -0.012f},
        Vec3{halyardX + 0.016f, mastTopY + 0.04f, -0.012f},
        0.007f, 0.007f, 8, kHalyardRope
    );

    // Brass hoist snap rings attaching the Kenyan flag to the halyard
    const float flagStartX = halyardX + 0.018f;
    for (float ringFrac : {0.06f, 0.94f}) {
        float ry = flagBotY + flagH * ringFrac;
        PropMeshHelper::appendCylinder(payload, Vec3{halyardX + 0.008f, ry - 0.015f, 0.0f}, 0.018f, 0.018f, 0.030f, 12, kBrassTrim);
    }

    // =========================================================================
    // 3. KENYAN FLAG WAVING SURFACE PARAMETERIZATION
    //    Bands (bottom-to-top v in [0,1], exact 6:1:6:1:6 proportions):
    //      [0.00, 0.30] Green  (#006B2B)
    //      [0.30, 0.35] White  (#F4F6F8)
    //      [0.35, 0.65] Red    (#BA0C0C)
    //      [0.65, 0.70] White  (#F4F6F8)
    //      [0.70, 1.00] Black  (#15171A)
    // =========================================================================
    const float waveAmp = (windMode == 0) ? (flagLen * 0.065f)
                        : (windMode == 2) ? (flagLen * 0.155f)
                                          : (flagLen * 0.110f);
    const float waveFreq = (windMode == 0) ? 2.15f : (windMode == 2) ? 3.20f : 2.65f;
    const float droopAmt = (windMode == 0) ? (flagH * 0.075f)
                         : (windMode == 2) ? (flagH * 0.018f)
                                           : (flagH * 0.042f);

    auto evalMidPoint = [&](float u, float v) -> Vec3 {
        float uc = clampF(u, 0.0f, 1.0f);
        float vc = clampF(v, 0.0f, 1.0f);
        float env = std::pow(uc, 0.88f);
        float phase = uc * PropMeshHelper::kPi * waveFreq;
        float wz = waveAmp * env * (std::sin(phase) + 0.22f * std::sin(phase * 1.85f + (vc - 0.5f) * 1.6f));
        float dy = -droopAmt * (uc * uc) + 0.014f * flagH * env * std::cos(phase * 0.92f - vc * 0.7f);
        // Gentle horizontal foreshortening so waving cloth maintains realistic arc length
        float dx = uc * flagLen * (1.0f - 0.035f * env * (0.5f + 0.5f * std::cos(phase * 2.0f)));
        return Vec3{flagStartX + dx, flagBotY + vc * flagH + dy, wz};
    };

    auto evalSurfaceNormal = [&](float u, float v) -> Vec3 {
        const float eps = 0.004f;
        Vec3 pu0 = evalMidPoint(u - eps, v);
        Vec3 pu1 = evalMidPoint(u + eps, v);
        Vec3 pv0 = evalMidPoint(u, v - eps);
        Vec3 pv1 = evalMidPoint(u, v + eps);
        Vec3 tu = pu1 - pu0;
        Vec3 tv = pv1 - pv0;
        return tu.cross(tv).normalized();
    };

    // Emit hoist canvas heading tape along u in [0, 0.025]
    constexpr float kClothHalfThick = 0.0035f;
    constexpr int kCols = 28;
    const float hoistU = 0.026f;

    auto emitDoubleSidedGridBand = [&](float u0, float u1, int subCols, float v0, float v1, int subRows, uint32_t color) {
        for (int c = 0; c < subCols; ++c) {
            float ua = u0 + (u1 - u0) * (static_cast<float>(c) / subCols);
            float ub = u0 + (u1 - u0) * (static_cast<float>(c + 1) / subCols);
            for (int r = 0; r < subRows; ++r) {
                float va = v0 + (v1 - v0) * (static_cast<float>(r) / subRows);
                float vb = v0 + (v1 - v0) * (static_cast<float>(r + 1) / subRows);

                Vec3 p00 = evalMidPoint(ua, va);
                Vec3 p10 = evalMidPoint(ub, va);
                Vec3 p11 = evalMidPoint(ub, vb);
                Vec3 p01 = evalMidPoint(ua, vb);

                Vec3 n00 = evalSurfaceNormal(ua, va);
                Vec3 n10 = evalSurfaceNormal(ub, va);
                Vec3 n11 = evalSurfaceNormal(ub, vb);
                Vec3 n01 = evalSurfaceNormal(ua, vb);

                // Front face (+normal)
                PropMeshHelper::appendQuad(
                    payload,
                    p00 + n00 * kClothHalfThick,
                    p10 + n10 * kClothHalfThick,
                    p11 + n11 * kClothHalfThick,
                    p01 + n01 * kClothHalfThick,
                    color
                );
                // Back face (-normal, reversed winding)
                PropMeshHelper::appendQuad(
                    payload,
                    p10 - n10 * kClothHalfThick,
                    p00 - n00 * kClothHalfThick,
                    p01 - n01 * kClothHalfThick,
                    p11 - n11 * kClothHalfThick,
                    color
                );
            }
        }
    };

    // Hoist white canvas strip
    emitDoubleSidedGridBand(0.0f, hoistU, 1, 0.0f, 1.0f, 6, kKenyaWhite);

    // Exact 6 : 1 : 6 : 1 : 6 Kenyan Flag stripes along fly width
    struct StripeBand {
        float v0;
        float v1;
        int rows;
        uint32_t color;
    };
    const StripeBand stripes[] = {
        {0.00f, 0.30f, 3, kKenyaGreen},
        {0.30f, 0.35f, 1, kKenyaWhite},
        {0.35f, 0.65f, 3, kKenyaRed},
        {0.65f, 0.70f, 1, kKenyaWhite},
        {0.70f, 1.00f, 3, kKenyaBlack}
    };
    for (const auto& s : stripes) {
        emitDoubleSidedGridBand(hoistU, 1.0f, kCols, s.v0, s.v1, s.rows, s.color);
    }

    // Stitched border thickness quads along top, bottom, hoist, and fly edges
    for (int c = 0; c < kCols; ++c) {
        float ua = hoistU + (1.0f - hoistU) * (static_cast<float>(c) / kCols);
        float ub = hoistU + (1.0f - hoistU) * (static_cast<float>(c + 1) / kCols);

        // Top edge (Black)
        Vec3 pt0 = evalMidPoint(ua, 1.0f), pt1 = evalMidPoint(ub, 1.0f);
        Vec3 nt0 = evalSurfaceNormal(ua, 1.0f), nt1 = evalSurfaceNormal(ub, 1.0f);
        PropMeshHelper::appendQuad(payload, pt0 - nt0 * kClothHalfThick, pt0 + nt0 * kClothHalfThick,
                                   pt1 + nt1 * kClothHalfThick, pt1 - nt1 * kClothHalfThick, kKenyaBlack);

        // Bottom edge (Green)
        Vec3 pb0 = evalMidPoint(ua, 0.0f), pb1 = evalMidPoint(ub, 0.0f);
        Vec3 nb0 = evalSurfaceNormal(ua, 0.0f), nb1 = evalSurfaceNormal(ub, 0.0f);
        PropMeshHelper::appendQuad(payload, pb0 + nb0 * kClothHalfThick, pb0 - nb0 * kClothHalfThick,
                                   pb1 - nb1 * kClothHalfThick, pb1 + nb1 * kClothHalfThick, kKenyaGreen);
    }
    for (const auto& s : stripes) {
        Vec3 pf0 = evalMidPoint(1.0f, s.v0), pf1 = evalMidPoint(1.0f, s.v1);
        Vec3 nf0 = evalSurfaceNormal(1.0f, s.v0), nf1 = evalSurfaceNormal(1.0f, s.v1);
        PropMeshHelper::appendQuad(payload, pf0 + nf0 * kClothHalfThick, pf0 - nf0 * kClothHalfThick,
                                   pf1 - nf1 * kClothHalfThick, pf1 + nf1 * kClothHalfThick, s.color);
    }

    // =========================================================================
    // 4. CENTERED MAASAI SHIELD & TWO CROSSED SPEARS (DOUBLE-SIDED)
    //    Local emblem space (lx, ly) in meters centered at (u = 0.50, v = 0.50)
    // =========================================================================
    const float emblemCenterU = 0.50f;
    const float emblemCenterV = 0.50f;

    auto mapEmblemPt = [&](float lx, float ly, float normalOffset) -> Vec3 {
        float u = clampF(emblemCenterU + lx / flagLen, 0.02f, 0.98f);
        float v = clampF(emblemCenterV + ly / flagH, 0.02f, 0.98f);
        Vec3 mid = evalMidPoint(u, v);
        Vec3 nrm = evalSurfaceNormal(u, v);
        return mid + nrm * normalOffset;
    };

    // Helper: Emit a convex/star-convex 2D polygon on BOTH +front and -back sides of the cloth
    auto emitDoubleSidedEmblemPoly = [&](const std::vector<Vec2>& ccwPts, float layerDepth, uint32_t color) {
        if (ccwPts.size() < 3) return;
        Vec2 center{0.0f, 0.0f};
        for (const auto& pt : ccwPts) {
            center = center + pt;
        }
        center = center * (1.0f / static_cast<float>(ccwPts.size()));

        const float cr = ((color >> 16) & 0xFF) / 255.0f;
        const float cg = ((color >> 8) & 0xFF) / 255.0f;
        const float cb = (color & 0xFF) / 255.0f;

        auto emitFan = [&](float offset, bool reverseWinding) {
            uint32_t cIdx = payload.vertexCount;
            Vec3 cPos = mapEmblemPt(center.x, center.y, offset);
            Vec3 cNrm = evalSurfaceNormal(
                clampF(emblemCenterU + center.x / flagLen, 0.02f, 0.98f),
                clampF(emblemCenterV + center.y / flagH, 0.02f, 0.98f)
            ) * (reverseWinding ? -1.0f : 1.0f);

            auto pushV = [&](const Vec3& pos, const Vec3& nrm) {
                payload.vertexBuffer.push_back(pos.x);
                payload.vertexBuffer.push_back(pos.y);
                payload.vertexBuffer.push_back(pos.z);
                payload.vertexBuffer.push_back(nrm.x);
                payload.vertexBuffer.push_back(nrm.y);
                payload.vertexBuffer.push_back(nrm.z);
                payload.vertexBuffer.push_back(cr);
                payload.vertexBuffer.push_back(cg);
                payload.vertexBuffer.push_back(cb);
                payload.vertexCount++;
            };

            pushV(cPos, cNrm);
            uint32_t rimStart = payload.vertexCount;
            for (const auto& pt : ccwPts) {
                Vec3 pPos = mapEmblemPt(pt.x, pt.y, offset);
                Vec3 pNrm = evalSurfaceNormal(
                    clampF(emblemCenterU + pt.x / flagLen, 0.02f, 0.98f),
                    clampF(emblemCenterV + pt.y / flagH, 0.02f, 0.98f)
                ) * (reverseWinding ? -1.0f : 1.0f);
                pushV(pPos, pNrm);
            }

            const uint32_t nPts = static_cast<uint32_t>(ccwPts.size());
            for (uint32_t i = 0; i < nPts; ++i) {
                uint32_t nextI = (i + 1) % nPts;
                if (!reverseWinding) {
                    payload.indexBuffer.push_back(cIdx);
                    payload.indexBuffer.push_back(rimStart + i);
                    payload.indexBuffer.push_back(rimStart + nextI);
                } else {
                    payload.indexBuffer.push_back(cIdx);
                    payload.indexBuffer.push_back(rimStart + nextI);
                    payload.indexBuffer.push_back(rimStart + i);
                }
                payload.indexCount += 3;
            }
        };

        emitFan(+layerDepth, false);
        emitFan(-layerDepth, true);
    };

    // Helper: Emit a curved strip between two endpoints with constant half-width in metric shield space
    auto emitDoubleSidedRibbon = [&](Vec2 a, Vec2 b, float halfW, int segs, float layerDepth, uint32_t color) {
        Vec2 d = b - a;
        float len = d.length();
        if (len < 1e-5f) return;
        Vec2 perp{-d.y / len, d.x / len};

        for (int i = 0; i < segs; ++i) {
            float t0 = static_cast<float>(i) / segs;
            float t1 = static_cast<float>(i + 1) / segs;
            Vec2 c0 = a + d * t0;
            Vec2 c1 = a + d * t1;

            Vec2 p00 = c0 - perp * halfW;
            Vec2 p10 = c1 - perp * halfW;
            Vec2 p11 = c1 + perp * halfW;
            Vec2 p01 = c0 + perp * halfW;

            // Front (+layerDepth)
            PropMeshHelper::appendQuad(
                payload,
                mapEmblemPt(p00.x, p00.y, +layerDepth),
                mapEmblemPt(p10.x, p10.y, +layerDepth),
                mapEmblemPt(p11.x, p11.y, +layerDepth),
                mapEmblemPt(p01.x, p01.y, +layerDepth),
                color
            );
            // Back (-layerDepth)
            PropMeshHelper::appendQuad(
                payload,
                mapEmblemPt(p10.x, p10.y, -layerDepth),
                mapEmblemPt(p00.x, p00.y, -layerDepth),
                mapEmblemPt(p01.x, p01.y, -layerDepth),
                mapEmblemPt(p11.x, p11.y, -layerDepth),
                color
            );
        }
    };

    // 4A. Two Crossed Maasai Spears (behind the shield at layerDepth = 0.0075f)
    const float spearDepth = kClothHalfThick + 0.0040f;
    const float spearHalfLen = flagH * 0.375f;
    const float spearAngleRad = 29.0f * (PropMeshHelper::kPi / 180.0f);
    const float shaftHalfW = flagH * 0.0090f;
    const float bladeHalfW = flagH * 0.0255f;

    for (float sign : {-1.0f, 1.0f}) {
        Vec2 dir{sign * std::sin(spearAngleRad), std::cos(spearAngleRad)};
        Vec2 perp{-dir.y, dir.x};

        // Main spear shaft (with small dark trim band near head collar)
        Vec2 butt = dir * (-spearHalfLen);
        Vec2 collar = dir * (spearHalfLen * 0.69f);
        emitDoubleSidedRibbon(butt, collar, shaftHalfW, 10, spearDepth, kKenyaWhite);

        // Dark collar wrapping at spearhead base
        emitDoubleSidedRibbon(dir * (spearHalfLen * 0.66f), dir * (spearHalfLen * 0.72f), shaftHalfW * 1.35f, 1, spearDepth + 0.0008f, kKenyaBlack);

        // Leaf/diamond Maasai spearhead blade
        Vec2 baseL = collar - perp * (shaftHalfW * 1.1f);
        Vec2 baseR = collar + perp * (shaftHalfW * 1.1f);
        Vec2 belly = dir * (spearHalfLen * 0.81f);
        Vec2 bellyL = belly - perp * bladeHalfW;
        Vec2 bellyR = belly + perp * bladeHalfW;
        Vec2 tip = dir * spearHalfLen;

        // CCW order for front face depends on perpendicular orientation: (-perp) is left when dir points +Y
        std::vector<Vec2> bladeCCW = {baseL, baseR, bellyR, tip, bellyL};
        emitDoubleSidedEmblemPoly(bladeCCW, spearDepth + 0.0006f, kKenyaWhite);
    }

    // 4B. Centered Maasai Shield (layered above spears cleanly at +0.011m .. +0.017m)
    const float shieldH = flagH * 0.275f; // half-height (~55% of flag height)
    const float shieldW = flagH * 0.145f; // half-width  (~29% of flag height)
    constexpr int kShieldSegs = 32;

    auto shieldEdgeX = [&](float t) -> float {
        float s = std::sin(t);
        float mag = std::pow(std::abs(s), 1.18f);
        return shieldW * std::copysign(mag, s);
    };
    auto shieldEdgeY = [&](float t) -> float {
        return shieldH * std::cos(t);
    };

    // Layer 1: Crisp White Outer Shield Trim Border
    {
        std::vector<Vec2> outerTrim;
        outerTrim.reserve(kShieldSegs);
        // Generate CCW from top (t = 0 -> 2pi in sin/cos actually goes CW if x=sin, y=cos; reverse so CCW!)
        for (int i = 0; i < kShieldSegs; ++i) {
            float t = -static_cast<float>(i) * (2.0f * PropMeshHelper::kPi / kShieldSegs);
            outerTrim.emplace_back(shieldEdgeX(t) * 1.06f, shieldEdgeY(t) * 1.035f);
        }
        emitDoubleSidedEmblemPoly(outerTrim, spearDepth + 0.0032f, kKenyaWhite);
    }

    // Layer 2: Crimson Red Maasai Shield Body (#BA0C0C)
    {
        std::vector<Vec2> redBody;
        redBody.reserve(kShieldSegs);
        for (int i = 0; i < kShieldSegs; ++i) {
            float t = -static_cast<float>(i) * (2.0f * PropMeshHelper::kPi / kShieldSegs);
            redBody.emplace_back(shieldEdgeX(t), shieldEdgeY(t));
        }
        emitDoubleSidedEmblemPoly(redBody, spearDepth + 0.0048f, kKenyaRed);
    }

    // Layer 3: Iconic Left & Right White Semi-Oval Crescents along Shield Flanks
    for (float side : {-1.0f, 1.0f}) {
        std::vector<Vec2> crescent;
        const float maxAngleSpan = 0.84f; // ~±48° around middle waist
        const int arcSteps = 12;
        // Outer arc follows shield perimeter closely; inner chord curves cleanly back toward waist
        for (int i = 0; i <= arcSteps; ++i) {
            float frac = static_cast<float>(i) / arcSteps;
            float angle = -maxAngleSpan + frac * (2.0f * maxAngleSpan);
            float t = (side > 0.0f) ? (PropMeshHelper::kPi * 0.5f - angle)
                                    : (PropMeshHelper::kPi * 1.5f + angle);
            float ox = shieldEdgeX(t) * 0.96f;
            float oy = shieldEdgeY(t) * 0.94f;
            crescent.emplace_back(ox, oy);
        }
        for (int i = arcSteps; i >= 0; --i) {
            float frac = static_cast<float>(i) / arcSteps;
            float yFrac = -1.0f + 2.0f * frac; // -1 bottom to +1 top
            float curveFactor = std::sqrt(std::max(0.0f, 1.0f - yFrac * yFrac * 0.85f));
            float ix = side * shieldW * (0.88f - 0.34f * curveFactor);
            float iy = shieldH * std::sin(maxAngleSpan) * yFrac * 0.94f;
            crescent.emplace_back(ix, iy);
        }
        // Ensure CCW order
        if (side < 0.0f) {
            std::reverse(crescent.begin(), crescent.end());
        }
        emitDoubleSidedEmblemPoly(crescent, spearDepth + 0.0064f, kKenyaWhite);
    }

    // Layer 4: Black Central Vertical Spindle / Hourglass Body
    {
        std::vector<Vec2> blackCenter;
        const int halfSteps = 14;
        // Right side from bottom tip up to top tip
        for (int i = 0; i <= halfSteps; ++i) {
            float yNorm = -1.0f + 2.0f * (static_cast<float>(i) / halfSteps);
            float tFromTop = std::acos(clampF(yNorm, -0.999f, 0.999f));
            float outerX = std::abs(shieldEdgeX(tFromTop));
            // Classic Maasai shield black vertical spindle covers central spine & widens gracefully in upper/lower quarters
            float spindleW = shieldW * (0.28f + 0.16f * std::sin(std::abs(yNorm) * PropMeshHelper::kPi)) * std::sqrt(std::max(0.0f, 1.0f - yNorm * yNorm));
            float x = std::min(outerX * 0.94f, spindleW);
            blackCenter.emplace_back(x, yNorm * shieldH * 0.97f);
        }
        // Left side from top tip down to bottom tip (CCW)
        for (int i = halfSteps; i >= 0; --i) {
            float yNorm = -1.0f + 2.0f * (static_cast<float>(i) / halfSteps);
            float tFromTop = std::acos(clampF(yNorm, -0.999f, 0.999f));
            float outerX = std::abs(shieldEdgeX(tFromTop));
            float spindleW = shieldW * (0.28f + 0.16f * std::sin(std::abs(yNorm) * PropMeshHelper::kPi)) * std::sqrt(std::max(0.0f, 1.0f - yNorm * yNorm));
            float x = std::min(outerX * 0.94f, spindleW);
            blackCenter.emplace_back(-x, yNorm * shieldH * 0.97f);
        }
        emitDoubleSidedEmblemPoly(blackCenter, spearDepth + 0.0080f, kKenyaBlack);
    }

    // Layer 5: White Central Vertical Spine Lines & Center Boss Oval + Inner Black Detail
    {
        // Vertical upper & lower white spine markings
        const float spineHalfW = shieldW * 0.045f;
        emitDoubleSidedRibbon(Vec2{0.0f, shieldH * 0.18f}, Vec2{0.0f, shieldH * 0.72f}, spineHalfW, 4, spearDepth + 0.0094f, kKenyaWhite);
        emitDoubleSidedRibbon(Vec2{0.0f, -shieldH * 0.72f}, Vec2{0.0f, -shieldH * 0.18f}, spineHalfW, 4, spearDepth + 0.0094f, kKenyaWhite);

        // Central white boss oval
        std::vector<Vec2> centerBoss;
        const int bossSteps = 16;
        for (int i = 0; i < bossSteps; ++i) {
            float a = static_cast<float>(i) * (2.0f * PropMeshHelper::kPi / bossSteps);
            centerBoss.emplace_back(shieldW * 0.15f * std::cos(a), shieldH * 0.14f * std::sin(a));
        }
        emitDoubleSidedEmblemPoly(centerBoss, spearDepth + 0.0096f, kKenyaWhite);

        // Tiny crisp black core dot inside white boss
        std::vector<Vec2> bossCore;
        for (int i = 0; i < 12; ++i) {
            float a = static_cast<float>(i) * (2.0f * PropMeshHelper::kPi / 12);
            bossCore.emplace_back(shieldW * 0.065f * std::cos(a), shieldH * 0.065f * std::sin(a));
        }
        emitDoubleSidedEmblemPoly(bossCore, spearDepth + 0.0110f, kKenyaBlack);
    }

    return payload;
}

} // namespace procedural::props
