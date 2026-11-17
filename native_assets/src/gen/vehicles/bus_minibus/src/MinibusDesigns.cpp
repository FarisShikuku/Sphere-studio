#include "MinibusDesigns.h"
#include "CabComponents.h"
#include "ChassisComponents.h"
#include "RunningGear.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace sphere::vehicles::minibus {

using sphere::trailers::gear::RunningGear;
using sphere::vehicles::cab::CabComponents;
using sphere::vehicles::chassis::ChassisComponents;

namespace {

constexpr float kPi = 3.14159265358979323846f;

inline Vec3 OffsetPt(const Vec3& p, const Vec3& n, float d) {
    return Vec3(p.x + n.x * d, p.y + n.y * d, p.z + n.z * d);
}

inline Vec3 SafeNormalize(const Vec3& v, const Vec3& fallback) {
    float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
    if (lenSq < 1e-8f) return fallback;
    float inv = 1.0f / std::sqrt(lenSq);
    return Vec3(v.x * inv, v.y * inv, v.z * inv);
}

inline void AppendTri(Mesh* mesh, const Vec3& a, const Vec3& b, const Vec3& c, const Vec4& col) {
    if (!mesh) return;
    Vec3 e1(b.x - a.x, b.y - a.y, b.z - a.z);
    Vec3 e2(c.x - a.x, c.y - a.y, c.z - a.z);
    Vec3 n = SafeNormalize(
        Vec3(e1.y * e2.z - e1.z * e2.y, e1.z * e2.x - e1.x * e2.z, e1.x * e2.y - e1.y * e2.x),
        Vec3(0.0f, 1.0f, 0.0f)
    );
    uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
    Vertex v0; v0.position = a; v0.normal = n; v0.color = col; v0.uv[0] = 0.0f; v0.uv[1] = 0.0f;
    Vertex v1; v1.position = b; v1.normal = n; v1.color = col; v1.uv[0] = 1.0f; v1.uv[1] = 0.0f;
    Vertex v2; v2.position = c; v2.normal = n; v2.color = col; v2.uv[0] = 1.0f; v2.uv[1] = 1.0f;
    mesh->vertices.push_back(v0);
    mesh->vertices.push_back(v1);
    mesh->vertices.push_back(v2);
    mesh->indices.push_back(base + 0);
    mesh->indices.push_back(base + 1);
    mesh->indices.push_back(base + 2);
}

/**
 * Compound-curved automotive front end for vans and minibuses:
 * Smooth curved bumper -> sloped aerodynamic hood/bonnet -> curved windshield cowl ->
 * contoured A-pillars -> crowned roof brow dome joining seamlessly with roof rails.
 */
struct VanFrontSurface {
    MinibusStyle style = MinibusStyle::MATATU_NGANYA;
    float xFront = 0.0f;
    float xCowl = 0.0f;
    float xRoofBrow = 0.0f;
    float xRoofJoin = 0.0f;
    float floorY = 0.30f;
    float hoodTopY = 1.12f;
    float winTopY = 2.08f;
    float roofTopY = 2.34f;
    float hw = 0.94f;

    float uFlat = 0.68f;
    float rCorner = 0.16f;
    float alpha = 0.32f;
    float sinA = 0.0f;
    float cosA = 0.0f;
    float zFlatEdge = 0.0f;
    float Rfront = 0.0f;
    float dxFrontBulge = 0.0f;
    float rTopDome = 0.18f;
    float roofCrown = 0.055f;

    void Init(
        MinibusStyle inStyle,
        float inXFront, float inXCowl, float inXRoofJoin,
        float inFloorY, float inHoodTopY, float inWinTopY, float inRoofTopY,
        float inHw
    ) {
        style = inStyle;
        xFront = inXFront;
        xCowl = inXCowl;
        xRoofJoin = inXRoofJoin;
        floorY = inFloorY;
        hoodTopY = inHoodTopY;
        winTopY = inWinTopY;
        roofTopY = inRoofTopY;
        hw = inHw;

        switch (style) {
            case MinibusStyle::SPRINTER_COMMERCIAL:
                uFlat = 0.64f;
                rCorner = std::min(0.24f, hw * 0.22f);
                alpha = 0.36f;
                xRoofBrow = inXCowl + 0.32f;
                roofCrown = 0.070f;
                break;
            case MinibusStyle::AIRPORT_SHUTTLE:
                uFlat = 0.70f;
                rCorner = std::min(0.22f, hw * 0.20f);
                alpha = 0.30f;
                xRoofBrow = inXCowl + 0.24f;
                roofCrown = 0.065f;
                break;
            case MinibusStyle::SCHOOL_BUS_VAN:
                uFlat = 0.74f;
                rCorner = std::min(0.18f, hw * 0.17f);
                alpha = 0.24f;
                xRoofBrow = inXCowl + 0.20f;
                roofCrown = 0.050f;
                break;
            case MinibusStyle::MATATU_NGANYA:
            default:
                uFlat = 0.68f;
                rCorner = std::min(0.20f, hw * 0.20f);
                alpha = 0.34f;
                xRoofBrow = inXCowl + 0.28f;
                roofCrown = 0.055f;
                break;
        }

        sinA = std::sin(alpha);
        cosA = std::cos(alpha);
        zFlatEdge = std::max(0.28f, hw - rCorner * (1.0f - sinA));
        Rfront = zFlatEdge / sinA;
        dxFrontBulge = Rfront * (1.0f - cosA);
        rTopDome = std::min(0.20f, std::max(0.08f, hw * 0.18f));
    }

    // Smooth vertical contour from bumper chin -> grille -> bonnet slope -> windshield A-pillar
    void EvalHeightAnchors(float y, float& xNoseCenter, float& xSideAnchor, float& hoodTaperZ) const {
        const float chinTop = floorY + 0.14f;
        const float grilleTop = floorY + (hoodTopY - floorY) * 0.54f;
        hoodTaperZ = 1.0f;

        if (y <= chinTop) {
            float t = std::clamp((y - floorY) / 0.14f, 0.0f, 1.0f);
            float a = t * (kPi * 0.5f);
            xNoseCenter = (xFront + 0.02f) + 0.055f * (1.0f - std::sin(a));
            xSideAnchor = xFront + 0.18f + 0.03f * std::cos(a);
            hoodTaperZ = 0.96f + 0.02f * std::sin(a);
        } else if (y <= grilleTop) {
            float t = std::clamp((y - chinTop) / std::max(0.08f, grilleTop - chinTop), 0.0f, 1.0f);
            float forwardSweep = (style == MinibusStyle::SPRINTER_COMMERCIAL) ? 0.14f : 0.07f;
            float sideMidX = std::min(xCowl, xFront + 0.36f);
            xNoseCenter = (xFront + 0.02f) + forwardSweep * t;
            xSideAnchor = xFront + 0.18f + (sideMidX - xFront - 0.18f) * t;
            hoodTaperZ = 0.98f + 0.02f * t;
        } else if (y <= hoodTopY) {
            float t = std::clamp((y - grilleTop) / std::max(0.08f, hoodTopY - grilleTop), 0.0f, 1.0f);
            float grilleX = (xFront + 0.02f) + ((style == MinibusStyle::SPRINTER_COMMERCIAL) ? 0.14f : 0.07f);
            float sideMidX = std::min(xCowl, xFront + 0.36f);
            float bonnetCurve = std::sin(t * kPi * 0.5f);
            xNoseCenter = grilleX + (xCowl - grilleX - 0.06f) * (0.42f * t + 0.58f * (1.0f - std::cos(t * kPi * 0.5f)));
            // Keep lower grille corner clear of wheel arch cutout before sweeping back along upper fender crest
            xSideAnchor = sideMidX + (xCowl - sideMidX) * (t * t * 0.65f + bonnetCurve * 0.35f * t);
            hoodTaperZ = 1.0f;
        } else {
            float t = std::clamp((y - hoodTopY) / std::max(0.15f, winTopY - hoodTopY), 0.0f, 1.0f);
            float cowlCenterX = xCowl - 0.06f;
            xNoseCenter = cowlCenterX + (xRoofBrow - cowlCenterX) * t - 0.022f * std::sin(kPi * t);
            xSideAnchor = xCowl + (xRoofBrow + 0.05f - xCowl) * t;
            hoodTaperZ = 1.0f;
        }
    }

    Vec3 EvalFrontPos(float u, float y) const {
        float xNoseCenter = 0.0f, xSideAnchor = 0.0f, taperZ = 1.0f;
        EvalHeightAnchors(y, xNoseCenter, xSideAnchor, taperZ);

        float uc = std::clamp(u, -1.0f, 1.0f);
        float absU = std::abs(uc);
        float sign = (uc >= 0.0f) ? 1.0f : -1.0f;

        float x = 0.0f, z = 0.0f;
        if (absU <= uFlat) {
            float phi = (absU / uFlat) * alpha;
            z = sign * (Rfront * std::sin(phi)) * taperZ;
            x = xNoseCenter + Rfront * (1.0f - std::cos(phi));
        } else {
            float tc = (absU - uFlat) / (1.0f - uFlat);
            float phi = alpha + tc * (kPi * 0.5f - alpha);
            z = sign * (zFlatEdge + rCorner * (std::sin(phi) - sinA)) * taperZ;
            float dxCorner = std::max(0.04f, xSideAnchor - (xNoseCenter + dxFrontBulge));
            x = (xNoseCenter + dxFrontBulge) + dxCorner * ((cosA - std::cos(phi)) / cosA);
        }

        // Gentle bonnet center crown + upper windshield header arch
        float sCenter = std::min(1.0f, absU / uFlat);
        float yArchLift = 0.0f;
        if (y <= hoodTopY) {
            float wh = std::clamp((y - floorY - 0.14f) / std::max(0.10f, hoodTopY - floorY - 0.14f), 0.0f, 1.0f);
            yArchLift = wh * 0.018f * (1.0f - sCenter * sCenter);
        } else {
            float wt = std::clamp((y - hoodTopY) / std::max(0.15f, winTopY - hoodTopY), 0.0f, 1.0f);
            yArchLift = (0.018f * (1.0f - wt) + 0.025f * wt) * (1.0f - sCenter * sCenter);
        }
        return Vec3(x, y + yArchLift, z);
    }

    Vec3 EvalRoofArchPos(float x, float u) const {
        float uc = std::clamp(u, -1.0f, 1.0f);
        float absU = std::abs(uc);
        float sign = (uc >= 0.0f) ? 1.0f : -1.0f;

        float zShoulder = hw - rTopDome;
        float nzShoulder = zShoulder / hw;
        float yShoulder = roofTopY + roofCrown * (1.0f - nzShoulder * nzShoulder);

        if (absU <= uFlat) {
            float s = absU / uFlat;
            float z = sign * s * zShoulder;
            float nz = z / hw;
            float y = roofTopY + roofCrown * (1.0f - nz * nz);
            return Vec3(x, y, z);
        } else {
            float t = (absU - uFlat) / (1.0f - uFlat);
            float a = t * (kPi * 0.5f);
            float z = sign * (zShoulder + rTopDome * std::sin(a));
            float y = winTopY + (yShoulder - winTopY) * std::cos(a);
            return Vec3(x, y, z);
        }
    }

    Vec3 EvalDomePos(float u, float vDome) const {
        float vt = std::clamp(vDome, 0.0f, 1.0f);
        Vec3 p0 = EvalFrontPos(u, winTopY);
        Vec3 p1 = EvalRoofArchPos(xRoofJoin, u);
        float phi = vt * (kPi * 0.5f);
        float fx = 0.22f * vt + 0.78f * (1.0f - std::cos(phi));
        float fy = std::sin(phi);
        float domeExtra = (style == MinibusStyle::AIRPORT_SHUTTLE)
            ? 0.075f * std::sin(vt * kPi) * std::max(0.0f, 1.0f - (u * u) / 0.55f)
            : 0.0f;
        return Vec3(
            p0.x + (p1.x - p0.x) * fx,
            p0.y + (p1.y - p0.y) * fy + domeExtra,
            p0.z + (p1.z - p0.z) * fy
        );
    }

    Vec3 EvalFrontNormal(float u, float y) const {
        float du = 0.01f, dy = 0.015f;
        Vec3 pu0 = EvalFrontPos(u - du, y);
        Vec3 pu1 = EvalFrontPos(u + du, y);
        Vec3 py0 = EvalFrontPos(u, y - dy);
        Vec3 py1 = EvalFrontPos(u, y + dy);
        Vec3 dU(pu1.x - pu0.x, pu1.y - pu0.y, pu1.z - pu0.z);
        Vec3 dY(py1.x - py0.x, py1.y - py0.y, py1.z - py0.z);
        return SafeNormalize(Vec3(
            dU.y * dY.z - dU.z * dY.y,
            dU.z * dY.x - dU.x * dY.z,
            dU.x * dY.y - dU.y * dY.x
        ), Vec3(-1.0f, 0.0f, 0.0f));
    }

    Vec3 EvalDomeNormal(float u, float vDome) const {
        float du = 0.01f, dv = 0.015f;
        float va = std::max(0.0f, vDome - dv);
        float vb = std::min(1.0f, vDome + dv);
        Vec3 pu0 = EvalDomePos(u - du, vDome);
        Vec3 pu1 = EvalDomePos(u + du, vDome);
        Vec3 pv0 = EvalDomePos(u, va);
        Vec3 pv1 = EvalDomePos(u, vb);
        Vec3 dU(pu1.x - pu0.x, pu1.y - pu0.y, pu1.z - pu0.z);
        Vec3 dV(pv1.x - pv0.x, pv1.y - pv0.y, pv1.z - pv0.z);
        return SafeNormalize(Vec3(
            dU.y * dV.z - dU.z * dV.y,
            dU.z * dV.x - dU.x * dV.z,
            dU.x * dV.y - dU.y * dV.x
        ), Vec3(-0.6f, 0.8f, 0.0f));
    }
};

struct VanRearSurface {
    float xRear = 0.0f;
    float hw = 0.94f;
    float uFlat = 0.72f;
    float rCorner = 0.14f;
    float maxBulgeX = 0.032f;

    void Init(float inXRear, float inHw, float cornerR = 0.14f) {
        xRear = inXRear;
        hw = inHw;
        rCorner = std::min(cornerR, inHw * 0.22f);
    }

    Vec3 EvalPos(float u, float y) const {
        float uc = std::clamp(u, -1.0f, 1.0f);
        float absU = std::abs(uc);
        float sign = (uc >= 0.0f) ? 1.0f : -1.0f;
        float zFlatEdge = std::max(0.24f, hw - rCorner);
        if (absU <= uFlat) {
            float s = absU / uFlat;
            float z = sign * s * zFlatEdge;
            float x = xRear + maxBulgeX * (1.0f - s * s);
            return Vec3(x, y, z);
        } else {
            float t = (absU - uFlat) / (1.0f - uFlat);
            float a = t * (kPi * 0.5f);
            float z = sign * (zFlatEdge + rCorner * std::sin(a));
            float x = xRear - rCorner * (1.0f - std::cos(a));
            return Vec3(x, y, z);
        }
    }

    Vec3 EvalNormal(float u, float y) const {
        float du = 0.01f, dy = 0.015f;
        Vec3 pu0 = EvalPos(u - du, y);
        Vec3 pu1 = EvalPos(u + du, y);
        Vec3 py0 = EvalPos(u, y - dy);
        Vec3 py1 = EvalPos(u, y + dy);
        Vec3 dU(pu1.x - pu0.x, pu1.y - pu0.y, pu1.z - pu0.z);
        Vec3 dY(py1.x - py0.x, py1.y - py0.y, py1.z - py0.z);
        // dY x dU -> +X outward rear normal
        return SafeNormalize(Vec3(
            dY.y * dU.z - dY.z * dU.y,
            dY.z * dU.x - dY.x * dU.z,
            dY.x * dU.y - dY.y * dU.x
        ), Vec3(1.0f, 0.0f, 0.0f));
    }
};

struct RibbonSample {
    float u;
    float yBot;
    float yTop;
};

void AppendFrontPatch(
    Mesh* mesh,
    const VanFrontSurface& surf,
    float u0, float u1,
    float y0, float y1,
    int segU, int segV,
    float offsetOut,
    const Vec4& col
) {
    if (!mesh || std::abs(u1 - u0) < 1e-4f || y1 <= y0 + 1e-4f) return;
    float uMin = std::min(u0, u1), uMax = std::max(u0, u1);
    int nu = std::max(1, segU), nv = std::max(1, segV);
    for (int j = 0; j < nv; ++j) {
        float ya = y0 + (y1 - y0) * (static_cast<float>(j) / static_cast<float>(nv));
        float yb = y0 + (y1 - y0) * (static_cast<float>(j + 1) / static_cast<float>(nv));
        for (int i = 0; i < nu; ++i) {
            float ua = uMin + (uMax - uMin) * (static_cast<float>(i) / static_cast<float>(nu));
            float ub = uMin + (uMax - uMin) * (static_cast<float>(i + 1) / static_cast<float>(nu));
            Vec3 a = OffsetPt(surf.EvalFrontPos(ua, ya), surf.EvalFrontNormal(ua, ya), offsetOut);
            Vec3 b = OffsetPt(surf.EvalFrontPos(ub, ya), surf.EvalFrontNormal(ub, ya), offsetOut);
            Vec3 c = OffsetPt(surf.EvalFrontPos(ub, yb), surf.EvalFrontNormal(ub, yb), offsetOut);
            Vec3 d = OffsetPt(surf.EvalFrontPos(ua, yb), surf.EvalFrontNormal(ua, yb), offsetOut);
            CabComponents::AppendQuad(mesh, a, b, c, d, col);
        }
    }
}

void AppendFrontRibbon(
    Mesh* mesh,
    const VanFrontSurface& surf,
    const std::vector<RibbonSample>& samples,
    float backOffset,
    float frontOffset,
    const Vec4& col
) {
    if (!mesh || samples.size() < 2) return;
    std::vector<Vec3> fB, fT, bB, bT;
    fB.reserve(samples.size());
    fT.reserve(samples.size());
    bB.reserve(samples.size());
    bT.reserve(samples.size());
    for (const auto& s : samples) {
        Vec3 pb = surf.EvalFrontPos(s.u, s.yBot);
        Vec3 nb = surf.EvalFrontNormal(s.u, s.yBot);
        Vec3 pt = surf.EvalFrontPos(s.u, s.yTop);
        Vec3 nt = surf.EvalFrontNormal(s.u, s.yTop);
        fB.push_back(OffsetPt(pb, nb, frontOffset));
        fT.push_back(OffsetPt(pt, nt, frontOffset));
        bB.push_back(OffsetPt(pb, nb, backOffset));
        bT.push_back(OffsetPt(pt, nt, backOffset));
    }
    for (size_t i = 0; i + 1 < samples.size(); ++i) {
        CabComponents::AppendQuad(mesh, fB[i], fB[i + 1], fT[i + 1], fT[i], col);
        CabComponents::AppendQuad(mesh, fT[i], fT[i + 1], bT[i + 1], bT[i], col);
        CabComponents::AppendQuad(mesh, bB[i], bB[i + 1], fB[i + 1], fB[i], col);
    }
    CabComponents::AppendQuad(mesh, bB.front(), fB.front(), fT.front(), bT.front(), col);
    CabComponents::AppendQuad(mesh, fB.back(), bB.back(), bT.back(), fT.back(), col);
}

void AppendFrontMirrorRibbon(
    Mesh* mesh,
    const VanFrontSurface& surf,
    const std::vector<RibbonSample>& rightSamples,
    float backOffset,
    float frontOffset,
    const Vec4& col
) {
    AppendFrontRibbon(mesh, surf, rightSamples, backOffset, frontOffset, col);
    std::vector<RibbonSample> leftSamples;
    leftSamples.reserve(rightSamples.size());
    for (auto it = rightSamples.rbegin(); it != rightSamples.rend(); ++it) {
        leftSamples.push_back({-it->u, it->yBot, it->yTop});
    }
    AppendFrontRibbon(mesh, surf, leftSamples, backOffset, frontOffset, col);
}

void AppendFrontDisc(
    Mesh* mesh,
    const VanFrontSurface& surf,
    float uCenter, float yCenter,
    float radiusU, float radiusY,
    float backOffset, float frontOffset,
    const Vec4& col,
    int segs = 14
) {
    if (!mesh) return;
    Vec3 pc = surf.EvalFrontPos(uCenter, yCenter);
    Vec3 nc = surf.EvalFrontNormal(uCenter, yCenter);
    Vec3 centerF = OffsetPt(pc, nc, frontOffset);
    std::vector<Vec3> rimF(segs), rimB(segs);
    for (int i = 0; i < segs; ++i) {
        float ang = (2.0f * kPi * static_cast<float>(i)) / static_cast<float>(segs);
        float ui = uCenter + radiusU * std::cos(ang);
        float yi = yCenter + radiusY * std::sin(ang);
        Vec3 pi = surf.EvalFrontPos(ui, yi);
        Vec3 ni = surf.EvalFrontNormal(ui, yi);
        rimF[i] = OffsetPt(pi, ni, frontOffset);
        rimB[i] = OffsetPt(pi, ni, backOffset);
    }
    for (int i = 0; i < segs; ++i) {
        int in = (i + 1) % segs;
        AppendTri(mesh, centerF, rimF[i], rimF[in], col);
        CabComponents::AppendQuad(mesh, rimB[i], rimB[in], rimF[in], rimF[i], col);
    }
}

void AppendRearPatch(
    Mesh* mesh,
    const VanRearSurface& surf,
    float u0, float u1,
    float y0, float y1,
    int segU, int segV,
    float offsetOut,
    const Vec4& col
) {
    if (!mesh || std::abs(u1 - u0) < 1e-4f || y1 <= y0 + 1e-4f) return;
    float uMin = std::min(u0, u1), uMax = std::max(u0, u1);
    int nu = std::max(1, segU), nv = std::max(1, segV);
    for (int j = 0; j < nv; ++j) {
        float ya = y0 + (y1 - y0) * (static_cast<float>(j) / static_cast<float>(nv));
        float yb = y0 + (y1 - y0) * (static_cast<float>(j + 1) / static_cast<float>(nv));
        for (int i = 0; i < nu; ++i) {
            float ua = uMin + (uMax - uMin) * (static_cast<float>(i) / static_cast<float>(nu));
            float ub = uMin + (uMax - uMin) * (static_cast<float>(i + 1) / static_cast<float>(nu));
            Vec3 a = OffsetPt(surf.EvalPos(ua, ya), surf.EvalNormal(ua, ya), offsetOut);
            Vec3 b = OffsetPt(surf.EvalPos(ub, ya), surf.EvalNormal(ub, ya), offsetOut);
            Vec3 c = OffsetPt(surf.EvalPos(ub, yb), surf.EvalNormal(ub, yb), offsetOut);
            Vec3 d = OffsetPt(surf.EvalPos(ua, yb), surf.EvalNormal(ua, yb), offsetOut);
            CabComponents::AppendQuad(mesh, b, a, d, c, col);
        }
    }
}

void AppendRearRibbon(
    Mesh* mesh,
    const VanRearSurface& surf,
    const std::vector<RibbonSample>& samples,
    float backOffset,
    float frontOffset,
    const Vec4& col
) {
    if (!mesh || samples.size() < 2) return;
    std::vector<Vec3> fB, fT, bB, bT;
    fB.reserve(samples.size());
    fT.reserve(samples.size());
    bB.reserve(samples.size());
    bT.reserve(samples.size());
    for (const auto& s : samples) {
        Vec3 pb = surf.EvalPos(s.u, s.yBot);
        Vec3 nb = surf.EvalNormal(s.u, s.yBot);
        Vec3 pt = surf.EvalPos(s.u, s.yTop);
        Vec3 nt = surf.EvalNormal(s.u, s.yTop);
        fB.push_back(OffsetPt(pb, nb, frontOffset));
        fT.push_back(OffsetPt(pt, nt, frontOffset));
        bB.push_back(OffsetPt(pb, nb, backOffset));
        bT.push_back(OffsetPt(pt, nt, backOffset));
    }
    for (size_t i = 0; i + 1 < samples.size(); ++i) {
        CabComponents::AppendQuad(mesh, fB[i + 1], fB[i], fT[i], fT[i + 1], col);
        CabComponents::AppendQuad(mesh, fT[i + 1], fT[i], bT[i], bT[i + 1], col);
        CabComponents::AppendQuad(mesh, bB[i + 1], bB[i], fB[i], fB[i + 1], col);
    }
    CabComponents::AppendQuad(mesh, fB.front(), bB.front(), bT.front(), fT.front(), col);
    CabComponents::AppendQuad(mesh, bB.back(), fB.back(), fT.back(), bT.back(), col);
}

void AppendRearMirrorRibbon(
    Mesh* mesh,
    const VanRearSurface& surf,
    const std::vector<RibbonSample>& rightSamples,
    float backOffset,
    float frontOffset,
    const Vec4& col
) {
    AppendRearRibbon(mesh, surf, rightSamples, backOffset, frontOffset, col);
    std::vector<RibbonSample> leftSamples;
    leftSamples.reserve(rightSamples.size());
    for (auto it = rightSamples.rbegin(); it != rightSamples.rend(); ++it) {
        leftSamples.push_back({-it->u, it->yBot, it->yTop});
    }
    AppendRearRibbon(mesh, surf, leftSamples, backOffset, frontOffset, col);
}

void AppendRearDisc(
    Mesh* mesh,
    const VanRearSurface& surf,
    float uCenter, float yCenter,
    float radiusU, float radiusY,
    float backOffset, float frontOffset,
    const Vec4& col,
    int segs = 12
) {
    if (!mesh) return;
    Vec3 pc = surf.EvalPos(uCenter, yCenter);
    Vec3 nc = surf.EvalNormal(uCenter, yCenter);
    Vec3 centerF = OffsetPt(pc, nc, frontOffset);
    std::vector<Vec3> rimF(segs), rimB(segs);
    for (int i = 0; i < segs; ++i) {
        float ang = (2.0f * kPi * static_cast<float>(i)) / static_cast<float>(segs);
        float ui = uCenter + radiusU * std::cos(ang);
        float yi = yCenter + radiusY * std::sin(ang);
        Vec3 pi = surf.EvalPos(ui, yi);
        Vec3 ni = surf.EvalNormal(ui, yi);
        rimF[i] = OffsetPt(pi, ni, frontOffset);
        rimB[i] = OffsetPt(pi, ni, backOffset);
    }
    for (int i = 0; i < segs; ++i) {
        int in = (i + 1) % segs;
        AppendTri(mesh, centerF, rimF[in], rimF[i], col);
        CabComponents::AppendQuad(mesh, rimB[in], rimB[i], rimF[i], rimF[in], col);
    }
}

void AppendVanNoseAndRoofShell(
    Mesh* mesh,
    const VanFrontSurface& surf,
    float xRear,
    const Vec4& bodyPaint,
    const Vec4& glossBlack
) {
    // 1. Compound-curved lower front bumper, grille surround & sloped bonnet/hood up to cowl
    AppendFrontPatch(mesh, surf, -1.0f, 1.0f, surf.floorY, surf.hoodTopY, 22, 10, 0.0f, bodyPaint);

    // 2. Smooth contoured left & right A-pillars wrapping around corners into side cab shoulders
    AppendFrontPatch(mesh, surf, -1.0f, -0.84f, surf.hoodTopY, surf.winTopY, 4, 8, 0.008f, glossBlack);
    AppendFrontPatch(mesh, surf,  0.84f,  1.0f, surf.hoodTopY, surf.winTopY, 4, 8, 0.008f, glossBlack);

    // Sloped side A-pillar cheek bridging from curved corner edge back to xRoofJoin (no boxy forward corner!)
    const int aSegs = 8;
    for (int side : {-1, 1}) {
        float uEdge = static_cast<float>(side);
        float zOut = static_cast<float>(side) * surf.hw;
        float zIn  = static_cast<float>(side) * (surf.hw - 0.05f);
        for (int j = 0; j < aSegs; ++j) {
            float ya = surf.hoodTopY + (surf.winTopY - surf.hoodTopY) * (static_cast<float>(j) / static_cast<float>(aSegs));
            float yb = surf.hoodTopY + (surf.winTopY - surf.hoodTopY) * (static_cast<float>(j + 1) / static_cast<float>(aSegs));
            float xaF = surf.EvalFrontPos(uEdge, ya).x;
            float xbF = surf.EvalFrontPos(uEdge, yb).x;
            float xaB = std::max(xaF, surf.xRoofJoin);
            float xbB = std::max(xbF, surf.xRoofJoin);
            if (side > 0) {
                CabComponents::AppendQuad(mesh, Vec3(xaF, ya, zOut), Vec3(xaB, ya, zOut), Vec3(xbB, yb, zOut), Vec3(xbF, yb, zOut), glossBlack);
                CabComponents::AppendQuad(mesh, Vec3(xaB, ya, zIn),  Vec3(xaF, ya, zIn),  Vec3(xbF, yb, zIn),  Vec3(xbB, yb, zIn),  glossBlack);
            } else {
                CabComponents::AppendQuad(mesh, Vec3(xaB, ya, zOut), Vec3(xaF, ya, zOut), Vec3(xbF, yb, zOut), Vec3(xbB, yb, zOut), glossBlack);
                CabComponents::AppendQuad(mesh, Vec3(xaF, ya, zIn),  Vec3(xaB, ya, zIn),  Vec3(xbB, yb, zIn),  Vec3(xbF, yb, zIn),  glossBlack);
            }
        }
    }

    // Curved inner cowl dash/frit trim right at base of windshield
    AppendFrontPatch(mesh, surf, -0.86f, 0.86f, surf.hoodTopY - 0.01f, surf.hoodTopY + 0.06f, 16, 2, 0.009f, glossBlack);

    // Dual curved windshield wiper arms along base of glass
    for (float sideSign : {-1.0f, 1.0f}) {
        std::vector<RibbonSample> wiper = {
            {sideSign * 0.05f, surf.hoodTopY + 0.065f, surf.hoodTopY + 0.088f},
            {sideSign * 0.36f, surf.hoodTopY + 0.085f, surf.hoodTopY + 0.106f},
            {sideSign * 0.68f, surf.hoodTopY + 0.070f, surf.hoodTopY + 0.090f},
        };
        if (sideSign < 0.0f) std::reverse(wiper.begin(), wiper.end());
        AppendFrontRibbon(mesh, surf, wiper, 0.008f, 0.022f, glossBlack);
    }

    // 3. Compound-curved upper brow / dome from winTopY -> roof arch at xRoofJoin
    const int nU = 22;
    const int nV = 7;
    for (int j = 0; j < nV; ++j) {
        float va = static_cast<float>(j) / static_cast<float>(nV);
        float vb = static_cast<float>(j + 1) / static_cast<float>(nV);
        for (int i = 0; i < nU; ++i) {
            float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nU));
            float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nU));
            Vec3 a = surf.EvalDomePos(ua, va);
            Vec3 b = surf.EvalDomePos(ub, va);
            Vec3 c = surf.EvalDomePos(ub, vb);
            Vec3 d = surf.EvalDomePos(ua, vb);
            CabComponents::AppendQuad(mesh, a, b, c, d, bodyPaint);
        }
    }

    // Smooth roof arch extrusion along X from xRoofJoin to xRoofRear
    for (int i = 0; i < nU; ++i) {
        float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nU));
        float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nU));
        Vec3 a = surf.EvalRoofArchPos(surf.xRoofJoin, ua);
        Vec3 b = surf.EvalRoofArchPos(surf.xRoofJoin, ub);
        Vec3 c = surf.EvalRoofArchPos(xRear, ub);
        Vec3 d = surf.EvalRoofArchPos(xRear, ua);
        CabComponents::AppendQuad(mesh, a, b, c, d, bodyPaint);
    }
    // Close roof underside soffit strip at y = winTopY so cabin ceiling is enclosed
    for (int i = 0; i < nU; ++i) {
        float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nU));
        float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nU));
        Vec3 pFrontA = surf.EvalRoofArchPos(surf.xRoofJoin, ua);
        Vec3 pFrontB = surf.EvalRoofArchPos(surf.xRoofJoin, ub);
        Vec3 pRearA  = surf.EvalRoofArchPos(xRear, ua);
        Vec3 pRearB  = surf.EvalRoofArchPos(xRear, ub);
        CabComponents::AppendQuad(mesh, pFrontB, pFrontA, pRearA, pRearB, glossBlack);
    }
}

void AppendMatatuFrontDetails(
    Mesh* mesh,
    const VanFrontSurface& surf,
    bool headlightsOn,
    const Vec4& accent,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol
) {
    const Vec4 drlCol = headlightsOn ? Vec4(0.98f, 0.24f, 0.64f, 1.0f) : Vec4(0.55f, 0.16f, 0.36f, 1.0f);
    const Vec4 cyanDrl = headlightsOn ? Vec4(0.18f, 0.90f, 1.0f, 1.0f) : Vec4(0.24f, 0.44f, 0.56f, 1.0f);
    const Vec4 headGlow = headlightsOn ? Vec4(1.0f, 0.96f, 0.86f, 1.0f) : Vec4(0.76f, 0.82f, 0.90f, 1.0f);

    const float bBot = surf.floorY;
    const float bTop = surf.floorY + (surf.hoodTopY - surf.floorY) * 0.40f;
    const float gBot = bTop + 0.02f;
    const float gTop = surf.floorY + (surf.hoodTopY - surf.floorY) * 0.78f;

    // 1. Sport body-kit front aero lip & glossy black lower intake dam
    AppendFrontPatch(mesh, surf, -0.96f, 0.96f, bBot - 0.02f, bBot + 0.08f, 16, 2, 0.026f, darkTrim);
    AppendFrontPatch(mesh, surf, -0.44f, 0.44f, bBot + 0.08f, bTop - 0.03f, 10, 3, 0.015f, glossBlack);
    // Neon magenta & cyan chin splitters
    AppendFrontPatch(mesh, surf, -0.78f, 0.78f, bBot - 0.01f, bBot + 0.025f, 12, 1, 0.030f, accent);

    // 2. Nganya aggressive diamond mesh grille with neon upper eyebrow strip
    AppendFrontPatch(mesh, surf, -0.52f, 0.52f, gBot, gTop, 12, 4, 0.014f, glossBlack);
    for (int s = 0; s < 3; ++s) {
        float sy = gBot + 0.04f + (gTop - gBot - 0.08f) * (static_cast<float>(s) / 2.0f);
        AppendFrontPatch(mesh, surf, -0.48f, 0.48f, sy - 0.012f, sy + 0.012f, 10, 1, 0.024f, (s == 1) ? chromeCol : darkTrim);
    }
    // Glowing neon cyan center bar across upper grille
    AppendFrontPatch(mesh, surf, -0.50f, 0.50f, gTop - 0.028f, gTop + 0.005f, 10, 1, 0.026f, cyanDrl);

    // 3. Angry swept twin-projector Nganya headlamps + pink neon DRL eyebrows
    std::vector<RibbonSample> lampPod = {
        {0.52f, gBot + 0.01f, gTop - 0.02f},
        {0.68f, gBot + 0.02f, gTop + 0.03f},
        {0.86f, gBot + 0.04f, gTop + 0.08f},
        {0.93f, gBot + 0.08f, gTop + 0.11f},
    };
    AppendFrontMirrorRibbon(mesh, surf, lampPod, 0.006f, 0.018f, glossBlack);

    std::vector<RibbonSample> eyebrow = {
        {0.50f, gTop - 0.02f, gTop + 0.01f},
        {0.70f, gTop + 0.02f, gTop + 0.05f},
        {0.94f, gTop + 0.09f, gTop + 0.125f},
    };
    AppendFrontMirrorRibbon(mesh, surf, eyebrow, 0.014f, 0.028f, drlCol);

    for (float side : {-1.0f, 1.0f}) {
        AppendFrontDisc(mesh, surf, side * 0.63f, (gBot + gTop) * 0.48f, 0.065f, 0.052f, 0.012f, 0.028f, headGlow, 12);
        AppendFrontDisc(mesh, surf, side * 0.79f, (gBot + gTop) * 0.54f, 0.058f, 0.046f, 0.012f, 0.028f, cyanDrl, 12);
        // Fog lamps + glowing surrounds
        AppendFrontDisc(mesh, surf, side * 0.72f, bBot + 0.16f, 0.06f, 0.042f, 0.012f, 0.026f, headGlow, 10);
    }

    // 4. Twin-vent sporty hood scoop on sloped bonnet
    float scoopY0 = surf.hoodTopY - 0.22f;
    float scoopY1 = surf.hoodTopY - 0.05f;
    AppendFrontPatch(mesh, surf, -0.32f, 0.32f, scoopY0, scoopY1, 8, 3, 0.034f, glossBlack);
    AppendFrontPatch(mesh, surf, -0.26f, 0.26f, scoopY0 + 0.02f, scoopY0 + 0.06f, 6, 1, 0.042f, accent);

    // 5. Heavy curved chrome off-road bull bar & twin circular rally spotlights
    std::vector<RibbonSample> bullUpper = {
        {-0.62f, bTop + 0.04f, bTop + 0.09f},
        {-0.32f, bTop + 0.06f, bTop + 0.11f},
        { 0.00f, bTop + 0.06f, bTop + 0.11f},
        { 0.32f, bTop + 0.06f, bTop + 0.11f},
        { 0.62f, bTop + 0.04f, bTop + 0.09f},
    };
    AppendFrontRibbon(mesh, surf, bullUpper, 0.07f, 0.13f, chromeCol);
    std::vector<RibbonSample> bullLower = {
        {-0.60f, bBot + 0.04f, bBot + 0.08f},
        { 0.00f, bBot + 0.04f, bBot + 0.08f},
        { 0.60f, bBot + 0.04f, bBot + 0.08f},
    };
    AppendFrontRibbon(mesh, surf, bullLower, 0.07f, 0.12f, chromeCol);
    for (float sx : {-0.58f, -0.28f, 0.28f, 0.58f}) {
        AppendFrontPatch(mesh, surf, sx - 0.025f, sx + 0.025f, bBot + 0.05f, bTop + 0.08f, 2, 2, 0.115f, chromeCol);
    }
    for (float side : {-1.0f, 1.0f}) {
        AppendFrontDisc(mesh, surf, side * 0.22f, bTop + 0.12f, 0.085f, 0.068f, 0.075f, 0.13f, chromeCol, 12);
        AppendFrontDisc(mesh, surf, side * 0.22f, bTop + 0.12f, 0.066f, 0.052f, 0.125f, 0.140f, headGlow, 12);
    }

    // 6. Roof brow 6-pod off-road LED light bar above windshield
    float barZ = surf.hw * 0.64f;
    float barX = surf.xRoofJoin + 0.04f;
    float barY = surf.roofTopY + surf.roofCrown;
    RunningGear::AppendBox(mesh, Vec3(barX - 0.06f, barY, -barZ), Vec3(barX + 0.06f, barY + 0.08f, barZ), darkTrim);
    for (int i = 0; i < 6; ++i) {
        float cz = -barZ * 0.80f + (barZ * 1.60f) * (static_cast<float>(i) / 5.0f);
        RunningGear::AppendBox(mesh, Vec3(barX - 0.075f, barY + 0.015f, cz - 0.05f), Vec3(barX - 0.055f, barY + 0.07f, cz + 0.05f), (i % 2 == 0) ? cyanDrl : headGlow);
    }
}

void AppendSprinterFrontDetails(
    Mesh* mesh,
    const VanFrontSurface& surf,
    bool headlightsOn,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol
) {
    const Vec4 headGlow = headlightsOn ? Vec4(0.98f, 0.98f, 1.0f, 1.0f) : Vec4(0.78f, 0.84f, 0.92f, 1.0f);
    const Vec4 drlStrip = headlightsOn ? Vec4(0.90f, 0.96f, 1.0f, 1.0f) : Vec4(0.60f, 0.66f, 0.72f, 1.0f);

    const float bBot = surf.floorY;
    const float bTop = surf.floorY + (surf.hoodTopY - surf.floorY) * 0.34f;
    const float gBot = bTop + 0.02f;
    const float gTop = surf.floorY + (surf.hoodTopY - surf.floorY) * 0.68f;

    // 1. Dark graphite moulded commercial front bumper with integrated corner step pocket
    AppendFrontPatch(mesh, surf, -1.0f, 1.0f, bBot - 0.01f, bTop, 18, 4, 0.012f, darkTrim);
    AppendFrontPatch(mesh, surf, -0.38f, 0.38f, bBot + 0.05f, bTop - 0.05f, 8, 2, 0.016f, glossBlack);
    // Silver license plate mount & twin bumper step cutouts
    AppendFrontPatch(mesh, surf, -0.22f, 0.22f, bBot + 0.08f, bTop - 0.06f, 4, 1, 0.022f, chromeCol);

    // 2. Signature trapezoidal European 3-slat chrome grille + central silver crest star ring
    AppendFrontPatch(mesh, surf, -0.44f, 0.44f, gBot, gTop, 10, 4, 0.014f, glossBlack);
    for (int s = 0; s < 3; ++s) {
        float sy = gBot + 0.045f + (gTop - gBot - 0.09f) * (static_cast<float>(s) / 2.0f);
        float halfSpan = 0.34f + 0.05f * (static_cast<float>(s) / 2.0f);
        AppendFrontPatch(mesh, surf, -halfSpan, halfSpan, sy - 0.014f, sy + 0.014f, 8, 1, 0.024f, chromeCol);
    }
    // Outer trapezoidal bezel
    AppendFrontMirrorRibbon(mesh, surf, {
        {0.0f,  gBot - 0.01f, gBot + 0.015f},
        {0.38f, gBot,         gBot + 0.025f},
        {0.46f, gTop - 0.01f, gTop + 0.018f},
        {0.0f,  gTop - 0.008f,gTop + 0.020f},
    }, 0.010f, 0.022f, chromeCol);

    // Central circular Mercedes/Sprinter-style silver medallion ring
    float emblemY = (gBot + gTop) * 0.50f;
    AppendFrontDisc(mesh, surf, 0.0f, emblemY, 0.095f, 0.075f, 0.018f, 0.032f, chromeCol, 16);
    AppendFrontDisc(mesh, surf, 0.0f, emblemY, 0.074f, 0.058f, 0.030f, 0.036f, glossBlack, 16);
    AppendFrontPatch(mesh, surf, -0.012f, 0.012f, emblemY, emblemY + 0.055f, 1, 1, 0.040f, chromeCol);

    // 3. Swept sharp angular commercial headlamps extending outward into fender corners
    std::vector<RibbonSample> headHousing = {
        {0.48f, gBot + 0.01f, gTop + 0.01f},
        {0.68f, gBot + 0.02f, gTop + 0.05f},
        {0.88f, gBot + 0.06f, gTop + 0.11f},
        {0.96f, gBot + 0.11f, gTop + 0.16f},
    };
    AppendFrontMirrorRibbon(mesh, surf, headHousing, 0.008f, 0.018f, glossBlack);
    std::vector<RibbonSample> headDrl = {
        {0.48f, gBot + 0.015f, gBot + 0.035f},
        {0.72f, gBot + 0.030f, gBot + 0.052f},
        {0.95f, gBot + 0.105f, gBot + 0.130f},
    };
    AppendFrontMirrorRibbon(mesh, surf, headDrl, 0.014f, 0.026f, drlStrip);
    for (float side : {-1.0f, 1.0f}) {
        AppendFrontDisc(mesh, surf, side * 0.66f, (gBot + gTop) * 0.52f, 0.065f, 0.050f, 0.012f, 0.026f, headGlow, 12);
        AppendFrontDisc(mesh, surf, side * 0.80f, (gBot + gTop) * 0.62f, 0.048f, 0.038f, 0.012f, 0.026f, headGlow, 10);
        // Corner bumper fog lamps
        AppendFrontDisc(mesh, surf, side * 0.78f, bBot + 0.13f, 0.048f, 0.036f, 0.012f, 0.022f, headGlow, 10);
    }

    // 4. Sculpted dual power-dome creases on sloped Sprinter bonnet
    for (float side : {-1.0f, 1.0f}) {
        std::vector<RibbonSample> bonnetRib = {
            {side * 0.42f, gTop + 0.03f, gTop + 0.07f},
            {side * 0.56f, surf.hoodTopY - 0.06f, surf.hoodTopY - 0.01f},
        };
        if (side < 0.0f) std::reverse(bonnetRib.begin(), bonnetRib.end());
        AppendFrontRibbon(mesh, surf, bonnetRib, 0.008f, 0.020f, darkTrim);
    }
}

void AppendShuttleFrontDetails(
    Mesh* mesh,
    const VanFrontSurface& surf,
    bool headlightsOn,
    const Vec4& accent,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol
) {
    const Vec4 headGlow = headlightsOn ? Vec4(1.0f, 0.96f, 0.88f, 1.0f) : Vec4(0.80f, 0.84f, 0.90f, 1.0f);
    const Vec4 amberLed = headlightsOn ? Vec4(1.0f, 0.74f, 0.12f, 1.0f) : Vec4(0.62f, 0.44f, 0.12f, 1.0f);

    const float bBot = surf.floorY;
    const float bTop = surf.floorY + (surf.hoodTopY - surf.floorY) * 0.36f;
    const float gBot = bTop + 0.03f;
    const float gTop = surf.floorY + (surf.hoodTopY - surf.floorY) * 0.78f;

    // 1. Heavy commercial airport bumper with chrome trim blade & lower skid grille
    AppendFrontPatch(mesh, surf, -1.0f, 1.0f, bBot - 0.02f, bTop, 18, 4, 0.018f, darkTrim);
    AppendFrontPatch(mesh, surf, -0.84f, 0.84f, bTop - 0.04f, bTop, 14, 1, 0.026f, chromeCol);
    AppendFrontPatch(mesh, surf, -0.50f, 0.50f, bBot + 0.04f, bBot + 0.15f, 10, 2, 0.024f, glossBlack);

    // 2. Wide chrome-framed commercial horizontal-rib grille with gold transit badge
    AppendFrontPatch(mesh, surf, -0.58f, 0.58f, gBot, gTop, 14, 5, 0.015f, glossBlack);
    for (int s = 0; s < 4; ++s) {
        float sy = gBot + 0.035f + (gTop - gBot - 0.07f) * (static_cast<float>(s) / 3.0f);
        AppendFrontPatch(mesh, surf, -0.55f, 0.55f, sy - 0.014f, sy + 0.014f, 12, 1, 0.026f, chromeCol);
    }
    AppendFrontPatch(mesh, surf, -0.10f, 0.10f, (gBot + gTop) * 0.5f - 0.04f, (gBot + gTop) * 0.5f + 0.04f, 2, 1, 0.034f, accent);

    // 3. Quad stacked rectangular shuttle headlamps with corner amber turn indicators
    AppendFrontPatch(mesh, surf, -0.94f, -0.62f, gBot, gTop + 0.02f, 5, 3, 0.016f, chromeCol);
    AppendFrontPatch(mesh, surf,  0.62f,  0.94f, gBot, gTop + 0.02f, 5, 3, 0.016f, chromeCol);
    for (float side : {-1.0f, 1.0f}) {
        float uA = std::min(side * 0.65f, side * 0.77f);
        float uB = std::max(side * 0.65f, side * 0.77f);
        float uC = std::min(side * 0.80f, side * 0.91f);
        float uD = std::max(side * 0.80f, side * 0.91f);
        AppendFrontPatch(mesh, surf, uA, uB, gBot + 0.03f, (gBot + gTop) * 0.50f, 3, 2, 0.026f, headGlow);
        AppendFrontPatch(mesh, surf, uC, uD, gBot + 0.03f, (gBot + gTop) * 0.50f, 3, 2, 0.026f, headGlow);
        // Upper amber indicator strip inside headlamp cluster
        AppendFrontPatch(mesh, surf, std::min(side * 0.65f, side * 0.91f), std::max(side * 0.65f, side * 0.91f),
                         (gBot + gTop) * 0.54f, gTop - 0.01f, 4, 1, 0.026f, amberLed);
        // Bumper fog lamps
        AppendFrontPatch(mesh, surf, std::min(side * 0.64f, side * 0.86f), std::max(side * 0.64f, side * 0.86f),
                         bBot + 0.06f, bBot + 0.14f, 3, 1, 0.026f, headGlow);
    }

    // 4. Illuminated curved amber header destination dome & rooftop airport amber beacon
    for (int i = 0; i < 12; ++i) {
        float ua = -0.64f + 1.28f * (static_cast<float>(i) / 12.0f);
        float ub = -0.64f + 1.28f * (static_cast<float>(i + 1) / 12.0f);
        Vec3 a = OffsetPt(surf.EvalDomePos(ua, 0.18f), surf.EvalDomeNormal(ua, 0.18f), 0.016f);
        Vec3 b = OffsetPt(surf.EvalDomePos(ub, 0.18f), surf.EvalDomeNormal(ub, 0.18f), 0.016f);
        Vec3 c = OffsetPt(surf.EvalDomePos(ub, 0.62f), surf.EvalDomeNormal(ub, 0.62f), 0.016f);
        Vec3 d = OffsetPt(surf.EvalDomePos(ua, 0.62f), surf.EvalDomeNormal(ua, 0.62f), 0.016f);
        CabComponents::AppendQuad(mesh, a, b, c, d, glossBlack);
        if (i >= 1 && i <= 10) {
            Vec3 la = OffsetPt(surf.EvalDomePos(ua, 0.25f), surf.EvalDomeNormal(ua, 0.25f), 0.024f);
            Vec3 lb = OffsetPt(surf.EvalDomePos(ub, 0.25f), surf.EvalDomeNormal(ub, 0.25f), 0.024f);
            Vec3 lc = OffsetPt(surf.EvalDomePos(ub, 0.54f), surf.EvalDomeNormal(ub, 0.54f), 0.024f);
            Vec3 ld = OffsetPt(surf.EvalDomePos(ua, 0.54f), surf.EvalDomeNormal(ua, 0.54f), 0.024f);
            CabComponents::AppendQuad(mesh, la, lb, lc, ld, amberLed);
        }
    }
    // Airport rooftop amber strobe beacon
    float bx = surf.xRoofJoin + 0.18f;
    float by = surf.roofTopY + surf.roofCrown;
    RunningGear::AppendBox(mesh, Vec3(bx - 0.08f, by, -0.08f), Vec3(bx + 0.08f, by + 0.03f, 0.08f), darkTrim);
    RunningGear::AppendBox(mesh, Vec3(bx - 0.055f, by + 0.03f, -0.055f), Vec3(bx + 0.055f, by + 0.11f, 0.055f), amberLed);
}

void AppendSchoolVanFrontDetails(
    Mesh* mesh,
    const VanFrontSurface& surf,
    bool headlightsOn,
    float doorOpenRatio,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol
) {
    const Vec4 headGlow = headlightsOn ? Vec4(1.0f, 0.94f, 0.78f, 1.0f) : Vec4(0.84f, 0.86f, 0.90f, 1.0f);
    const Vec4 warnRed = headlightsOn ? Vec4(1.0f, 0.14f, 0.14f, 1.0f) : Vec4(0.58f, 0.12f, 0.12f, 1.0f);
    const Vec4 warnAmber = headlightsOn ? Vec4(1.0f, 0.72f, 0.08f, 1.0f) : Vec4(0.62f, 0.44f, 0.08f, 1.0f);
    const Vec4 schoolYellow(0.96f, 0.70f, 0.06f, 1.0f);

    const float bBot = surf.floorY;
    const float bTop = surf.floorY + (surf.hoodTopY - surf.floorY) * 0.34f;
    const float gBot = bTop + 0.03f;
    const float gTop = surf.floorY + (surf.hoodTopY - surf.floorY) * 0.78f;

    // 1. Heavy matte charcoal steel bumper + tubular pedestrian crossing-control gate arm
    AppendFrontPatch(mesh, surf, -1.0f, 1.0f, bBot - 0.03f, bTop, 18, 4, 0.025f, darkTrim);
    AppendFrontPatch(mesh, surf, -0.88f, 0.88f, bBot + 0.04f, bBot + 0.08f, 12, 1, 0.032f, glossBlack);
    // Yellow hinged crossing-control arm protruding forward when door_open_ratio > 0
    float swing = std::clamp(doorOpenRatio, 0.0f, 1.0f);
    float armFwd = 0.05f + 0.45f * swing;
    float armZ = 0.52f - 0.62f * (1.0f - swing);
    RunningGear::AppendBox(
        mesh,
        Vec3(surf.xFront - armFwd, bBot + 0.09f, std::min(armZ, 0.56f)),
        Vec3(surf.xFront + 0.04f,  bBot + 0.14f, std::max(armZ, 0.56f)),
        schoolYellow
    );

    // 2. Classic 4-slat black & yellow horizontal school bus radiator grille
    AppendFrontPatch(mesh, surf, -0.48f, 0.48f, gBot, gTop, 12, 4, 0.015f, glossBlack);
    for (int s = 0; s < 4; ++s) {
        float sy = gBot + 0.035f + (gTop - gBot - 0.07f) * (static_cast<float>(s) / 3.0f);
        AppendFrontPatch(mesh, surf, -0.45f, 0.45f, sy - 0.013f, sy + 0.013f, 10, 1, 0.024f, schoolYellow);
    }

    // 3. Classic circular sealed-beam round headlamps + amber turn signal bullets
    for (float side : {-1.0f, 1.0f}) {
        AppendFrontDisc(mesh, surf, side * 0.72f, (gBot + gTop) * 0.50f, 0.11f, 0.095f, 0.010f, 0.024f, chromeCol, 14);
        AppendFrontDisc(mesh, surf, side * 0.72f, (gBot + gTop) * 0.50f, 0.085f, 0.075f, 0.022f, 0.032f, headGlow, 14);
        // Amber circular turn indicator next to each round headlight
        AppendFrontDisc(mesh, surf, side * 0.88f, (gBot + gTop) * 0.62f, 0.046f, 0.040f, 0.012f, 0.026f, warnAmber, 10);
    }

    // 4. Black roof brow visor with 4 circular school warning lamps (2 Red outer, 2 Amber inner)
    for (int i = 0; i < 16; ++i) {
        float ua = -0.84f + 1.68f * (static_cast<float>(i) / 16.0f);
        float ub = -0.84f + 1.68f * (static_cast<float>(i + 1) / 16.0f);
        Vec3 a = OffsetPt(surf.EvalDomePos(ua, 0.10f), surf.EvalDomeNormal(ua, 0.10f), 0.014f);
        Vec3 b = OffsetPt(surf.EvalDomePos(ub, 0.10f), surf.EvalDomeNormal(ub, 0.10f), 0.014f);
        Vec3 c = OffsetPt(surf.EvalDomePos(ub, 0.68f), surf.EvalDomeNormal(ub, 0.68f), 0.014f);
        Vec3 d = OffsetPt(surf.EvalDomePos(ua, 0.68f), surf.EvalDomeNormal(ua, 0.68f), 0.014f);
        CabComponents::AppendQuad(mesh, a, b, c, d, glossBlack);
    }
    const float lampU[4] = {-0.62f, -0.44f, 0.44f, 0.62f};
    const bool lampRed[4] = {true, false, false, true};
    for (int k = 0; k < 4; ++k) {
        Vec3 pc = OffsetPt(surf.EvalDomePos(lampU[k], 0.36f), surf.EvalDomeNormal(lampU[k], 0.36f), 0.028f);
        Vec3 nc = surf.EvalDomeNormal(lampU[k], 0.36f);
        RunningGear::AppendBox(
            mesh,
            Vec3(pc.x - 0.03f, pc.y - 0.055f, pc.z - 0.055f),
            Vec3(pc.x + 0.03f + nc.x * -0.02f, pc.y + 0.055f, pc.z + 0.055f),
            lampRed[k] ? warnRed : warnAmber
        );
    }

    // 5. Twin structural black rub rails along upper belt and rocker on both sides
    float railX0 = surf.xRoofJoin + 0.06f;
    float railX1 = surf.xRoofJoin + 3.20f; // side trim handled cleanly in Minibus.cpp if long
    (void)railX0; (void)railX1;
}

} // namespace

void AppendMinibusFrontAndRoof(
    Mesh* mesh,
    MinibusStyle style,
    float xFront,
    float xCowl,
    float xRoofJoin,
    float xRear,
    float floorY,
    float hoodTopY,
    float winBotY,
    float winTopY,
    float roofTopY,
    float hw,
    bool headlightsOn,
    float doorOpenRatio,
    const Vec4& bodyPaint,
    const Vec4& accent,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol
) {
    (void)winBotY;
    if (!mesh) return;
    VanFrontSurface surf;
    surf.Init(style, xFront, xCowl, xRoofJoin, floorY, hoodTopY, winTopY, roofTopY, hw);
    AppendVanNoseAndRoofShell(mesh, surf, xRear, bodyPaint, glossBlack);

    switch (style) {
        case MinibusStyle::SPRINTER_COMMERCIAL:
            AppendSprinterFrontDetails(mesh, surf, headlightsOn, glossBlack, darkTrim, chromeCol);
            break;
        case MinibusStyle::AIRPORT_SHUTTLE:
            AppendShuttleFrontDetails(mesh, surf, headlightsOn, accent, glossBlack, darkTrim, chromeCol);
            break;
        case MinibusStyle::SCHOOL_BUS_VAN:
            AppendSchoolVanFrontDetails(mesh, surf, headlightsOn, doorOpenRatio, glossBlack, darkTrim, chromeCol);
            break;
        case MinibusStyle::MATATU_NGANYA:
        default:
            AppendMatatuFrontDetails(mesh, surf, headlightsOn, accent, glossBlack, darkTrim, chromeCol);
            break;
    }
}

void AppendMinibusCurvedWindshield(
    Mesh* mesh,
    MinibusStyle style,
    float xFront,
    float xCowl,
    float xRoofJoin,
    float floorY,
    float hoodTopY,
    float winTopY,
    float roofTopY,
    float hw,
    const Vec4& glassColor
) {
    if (!mesh) return;
    VanFrontSurface surf;
    surf.Init(style, xFront, xCowl, xRoofJoin, floorY, hoodTopY, winTopY, roofTopY, hw);
    AppendFrontPatch(mesh, surf, -0.85f, 0.85f, hoodTopY + 0.05f, winTopY - 0.01f, 18, 8, 0.010f, glassColor);
}

void AppendMinibusMirrors(
    Mesh* mesh,
    MinibusStyle style,
    float xCowl,
    float xRoofJoin,
    float hoodTopY,
    float winTopY,
    float hw,
    const Vec4& bodyPaint,
    const Vec4& darkTrim,
    const Vec4& chromeCol
) {
    if (!mesh) return;
    const Vec4 mirrorGlass(0.70f, 0.78f, 0.86f, 1.0f);

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        if (style == MinibusStyle::SPRINTER_COMMERCIAL) {
            // Sleek European A-pillar wing mirrors mounted slightly higher on pillar
            float armX = xCowl + 0.16f;
            float armY = hoodTopY + 0.24f;
            float zIn = s * (hw - 0.02f);
            float zOut = s * (hw + 0.19f);
            RunningGear::AppendBox(mesh, Vec3(armX, armY - 0.02f, std::min(zIn, zOut)), Vec3(armX + 0.05f, armY + 0.03f, std::max(zIn, zOut)), darkTrim);
            RunningGear::AppendBox(mesh, Vec3(armX - 0.04f, armY - 0.08f, s > 0 ? hw + 0.14f : -hw - 0.24f), Vec3(armX + 0.06f, armY + 0.14f, s > 0 ? hw + 0.24f : -hw - 0.14f), bodyPaint);
            RunningGear::AppendBox(mesh, Vec3(armX + 0.05f, armY - 0.06f, s > 0 ? hw + 0.15f : -hw - 0.23f), Vec3(armX + 0.065f, armY + 0.12f, s > 0 ? hw + 0.23f : -hw - 0.15f), mirrorGlass);
        } else if (style == MinibusStyle::AIRPORT_SHUTTLE) {
            // Extended commercial dual-glass cantilever coach mirrors
            float topX = xRoofJoin - 0.06f;
            float topY = winTopY - 0.10f;
            float headX = topX - 0.12f;
            RunningGear::AppendBox(mesh, Vec3(headX, topY - 0.02f, s > 0 ? hw - 0.02f : -hw - 0.24f), Vec3(topX, topY + 0.025f, s > 0 ? hw + 0.24f : -hw + 0.02f), darkTrim);
            RunningGear::AppendBox(mesh, Vec3(headX - 0.02f, topY - 0.24f, s > 0 ? hw + 0.18f : -hw - 0.28f), Vec3(headX + 0.06f, topY + 0.02f, s > 0 ? hw + 0.28f : -hw - 0.18f), darkTrim);
            RunningGear::AppendBox(mesh, Vec3(headX + 0.05f, topY - 0.22f, s > 0 ? hw + 0.19f : -hw - 0.27f), Vec3(headX + 0.068f, topY, s > 0 ? hw + 0.27f : -hw - 0.19f), mirrorGlass);
        } else if (style == MinibusStyle::SCHOOL_BUS_VAN) {
            // Dual-arm yellow/black commercial school bus mirrors + front convex crossover pod
            float armX = xCowl + 0.14f;
            float armY = hoodTopY + 0.22f;
            RunningGear::AppendBox(mesh, Vec3(armX, armY + 0.16f, s > 0 ? hw - 0.02f : -hw - 0.22f), Vec3(armX + 0.03f, armY + 0.19f, s > 0 ? hw + 0.22f : -hw + 0.02f), darkTrim);
            RunningGear::AppendBox(mesh, Vec3(armX, armY - 0.04f, s > 0 ? hw - 0.02f : -hw - 0.22f), Vec3(armX + 0.03f, armY - 0.01f, s > 0 ? hw + 0.22f : -hw + 0.02f), darkTrim);
            RunningGear::AppendBox(mesh, Vec3(armX - 0.03f, armY - 0.06f, s > 0 ? hw + 0.16f : -hw - 0.26f), Vec3(armX + 0.05f, armY + 0.20f, s > 0 ? hw + 0.26f : -hw - 0.16f), bodyPaint);
            // Front hood convex safety crossover mirror arm
            float frontX = xCowl - 0.22f;
            RunningGear::AppendBox(mesh, Vec3(frontX - 0.08f, hoodTopY, s > 0 ? hw - 0.18f : -hw - 0.02f), Vec3(frontX, hoodTopY + 0.18f, s > 0 ? hw + 0.02f : -hw + 0.18f), darkTrim);
        } else {
            // Matatu sport twin-stalk chrome rally mirrors
            float armX = xCowl + 0.16f;
            float armY = hoodTopY + 0.20f;
            RunningGear::AppendBox(mesh, Vec3(armX, armY - 0.02f, s > 0 ? hw - 0.02f : -hw - 0.18f), Vec3(armX + 0.04f, armY + 0.02f, s > 0 ? hw + 0.18f : -hw + 0.02f), darkTrim);
            RunningGear::AppendBox(mesh, Vec3(armX - 0.03f, armY - 0.06f, s > 0 ? hw + 0.12f : -hw - 0.22f), Vec3(armX + 0.05f, armY + 0.10f, s > 0 ? hw + 0.22f : -hw - 0.12f), chromeCol);
            RunningGear::AppendBox(mesh, Vec3(armX + 0.045f, armY - 0.04f, s > 0 ? hw + 0.13f : -hw - 0.21f), Vec3(armX + 0.058f, armY + 0.08f, s > 0 ? hw + 0.21f : -hw - 0.13f), mirrorGlass);
        }
    }
}

void AppendMinibusRearDesign(
    Mesh* mesh,
    MinibusStyle style,
    float xSideEnd,
    float xRear,
    float floorY,
    float winBotY,
    float winTopY,
    float roofTopY,
    float hw,
    bool headlightsOn,
    const Vec4& bodyPaint,
    const Vec4& accent,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol
) {
    if (!mesh) return;
    const float cornerR = (style == MinibusStyle::SCHOOL_BUS_VAN) ? 0.11f : 0.15f;
    VanRearSurface surf;
    surf.Init(xRear, hw, cornerR);

    VanFrontSurface roofRef;
    roofRef.Init(style, xRear - 2.0f, xRear - 1.5f, xRear - cornerR, floorY, winBotY, winTopY, roofTopY, hw);

    const Vec4 tailRed = headlightsOn ? Vec4(1.0f, 0.12f, 0.14f, 1.0f) : Vec4(0.60f, 0.10f, 0.12f, 1.0f);
    const Vec4 amberSig = headlightsOn ? Vec4(1.0f, 0.72f, 0.10f, 1.0f) : Vec4(0.62f, 0.44f, 0.12f, 1.0f);
    const Vec4 revWhite(0.92f, 0.94f, 0.98f, 1.0f);
    const Vec4 cyanNeon = headlightsOn ? Vec4(0.16f, 0.90f, 1.0f, 1.0f) : Vec4(0.24f, 0.44f, 0.56f, 1.0f);

    // Side D-pillar cheeks bridging cleanly from xSideEnd to curved rear corners (xRear - cornerR)
    float xSideCornerEnd = xRear - cornerR;
    if (xSideCornerEnd > xSideEnd + 0.01f) {
        CabComponents::AppendSkirtWithCutouts(mesh, xSideEnd, xSideCornerEnd, floorY - 0.01f, winTopY + 0.01f, -hw, false, 0.06f, {}, bodyPaint);
        CabComponents::AppendSkirtWithCutouts(mesh, xSideEnd, xSideCornerEnd, floorY - 0.01f, winTopY + 0.01f,  hw, true,  0.06f, {}, bodyPaint);
    }

    // Smooth curved lower valance & rear quarter fascia shell
    AppendRearPatch(mesh, surf, -1.0f, 1.0f, floorY, winBotY, 18, 6, 0.008f, bodyPaint);
    // Corner D-pillar vertical caps beside rear window
    AppendRearPatch(mesh, surf, -1.0f, -0.78f, winBotY, winTopY, 4, 4, 0.008f, bodyPaint);
    AppendRearPatch(mesh, surf,  0.78f,  1.0f, winBotY, winTopY, 4, 4, 0.008f, bodyPaint);
    // Opaque black window frit border trim around rear glass opening
    AppendRearPatch(mesh, surf, -0.79f, 0.79f, winBotY - 0.02f, winBotY + 0.025f, 12, 1, 0.009f, glossBlack);
    AppendRearPatch(mesh, surf, -0.79f, 0.79f, winTopY - 0.025f, winTopY + 0.02f, 12, 1, 0.009f, glossBlack);
    AppendRearPatch(mesh, surf, -0.79f, -0.74f, winBotY, winTopY, 2, 4, 0.009f, glossBlack);
    AppendRearPatch(mesh, surf,  0.74f,  0.79f, winBotY, winTopY, 2, 4, 0.009f, glossBlack);

    // Smooth compound-curved upper rear roof brow dome closing roofArch(xRear - cornerR) -> surf(u, winTopY)
    const int nU = 22;
    const int nV = 6;
    auto evalRearDome = [&](float u, float v) -> Vec3 {
        Vec3 pTop = roofRef.EvalRoofArchPos(xSideCornerEnd, u);
        Vec3 pBot = surf.EvalPos(u, winTopY);
        float phi = v * (kPi * 0.5f);
        float fx = 1.0f - std::cos(phi);
        float fy = std::sin(phi);
        return Vec3(
            pBot.x + (pTop.x - pBot.x) * fx,
            pBot.y + (pTop.y - pBot.y) * fy,
            pBot.z + (pTop.z - pBot.z) * fy
        );
    };
    for (int j = 0; j < nV; ++j) {
        float va = static_cast<float>(j) / static_cast<float>(nV);
        float vb = static_cast<float>(j + 1) / static_cast<float>(nV);
        for (int i = 0; i < nU; ++i) {
            float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nU));
            float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nU));
            Vec3 a = evalRearDome(ua, va);
            Vec3 b = evalRearDome(ub, va);
            Vec3 c = evalRearDome(ub, vb);
            Vec3 d = evalRearDome(ua, vb);
            // At rear facing +X / +Y: b -> a -> d -> c is outward!
            CabComponents::AppendQuad(mesh, b, a, d, c, bodyPaint);
        }
    }

    switch (style) {
        case MinibusStyle::SPRINTER_COMMERCIAL: {
            // 50/50 split commercial barn doors with vertical center seam, grab handle & tall vertical Sprinter taillights
            AppendRearPatch(mesh, surf, -0.96f, 0.96f, floorY - 0.02f, floorY + 0.22f, 16, 2, 0.028f, darkTrim);
            // Center barn door seam + black grab handle
            AppendRearPatch(mesh, surf, -0.018f, 0.018f, floorY + 0.22f, winTopY, 1, 6, 0.016f, glossBlack);
            AppendRearPatch(mesh, surf, -0.14f, -0.04f, floorY + 0.58f, floorY + 0.72f, 2, 1, 0.026f, glossBlack);
            // Left & right Sprinter corner door hinges
            for (float sy : {floorY + 0.42f, winTopY - 0.22f}) {
                AppendRearPatch(mesh, surf, -0.82f, -0.74f, sy - 0.03f, sy + 0.03f, 2, 1, 0.022f, chromeCol);
                AppendRearPatch(mesh, surf,  0.74f,  0.82f, sy - 0.03f, sy + 0.03f, 2, 1, 0.022f, chromeCol);
            }
            // Tall vertical Mercedes Sprinter corner taillight towers
            for (float side : {-1.0f, 1.0f}) {
                float u0 = std::min(side * 0.80f, side * 0.94f);
                float u1 = std::max(side * 0.80f, side * 0.94f);
                AppendRearPatch(mesh, surf, u0, u1, floorY + 0.28f, floorY + 0.82f, 3, 4, 0.022f, tailRed);
                AppendRearPatch(mesh, surf, u0, u1, floorY + 0.48f, floorY + 0.58f, 3, 1, 0.025f, amberSig);
                AppendRearPatch(mesh, surf, u0, u1, floorY + 0.34f, floorY + 0.42f, 3, 1, 0.025f, revWhite);
            }
            // High-mount 3rd brake light above barn doors
            AppendRearPatch(mesh, surf, -0.22f, 0.22f, winTopY + 0.02f, winTopY + 0.09f, 4, 1, 0.024f, tailRed);
            break;
        }

        case MinibusStyle::AIRPORT_SHUTTLE: {
            // Wide shuttle lower bumper step, twin horizontal triple-pod lamps & chrome grab/license garnish
            AppendRearPatch(mesh, surf, -0.98f, 0.98f, floorY - 0.03f, floorY + 0.24f, 16, 2, 0.032f, darkTrim);
            AppendRearPatch(mesh, surf, -0.60f, 0.60f, floorY + 0.20f, floorY + 0.24f, 8, 1, 0.038f, chromeCol);
            AppendRearPatch(mesh, surf, -0.34f, 0.34f, floorY + 0.62f, floorY + 0.70f, 6, 1, 0.022f, chromeCol);
            for (float side : {-1.0f, 1.0f}) {
                AppendRearDisc(mesh, surf, side * 0.86f, floorY + 0.46f, 0.06f, 0.055f, 0.012f, 0.028f, tailRed, 10);
                AppendRearDisc(mesh, surf, side * 0.70f, floorY + 0.46f, 0.055f, 0.050f, 0.012f, 0.028f, amberSig, 10);
                AppendRearDisc(mesh, surf, side * 0.55f, floorY + 0.46f, 0.048f, 0.044f, 0.012f, 0.028f, revWhite, 10);
            }
            AppendRearPatch(mesh, surf, -0.45f, 0.45f, winTopY + 0.02f, winTopY + 0.09f, 6, 1, 0.022f, accent);
            // Single heavy curb-side exhaust pipe
            ChassisComponents::AppendClosedCylinderX(mesh, xRear - 0.24f, xRear + 0.09f, floorY + 0.06f, -hw * 0.58f, 0.042f, 12, chromeCol, darkTrim);
            break;
        }

        case MinibusStyle::SCHOOL_BUS_VAN: {
            // Heavy black school rear steel bumper, emergency exit door outline & 4-lamp upper warning bar
            AppendRearPatch(mesh, surf, -1.0f, 1.0f, floorY - 0.03f, floorY + 0.24f, 16, 2, 0.035f, darkTrim);
            // Emergency door seam frame & red handle
            AppendRearMirrorRibbon(mesh, surf, {
                {0.0f,  floorY + 0.25f, floorY + 0.27f},
                {0.34f, floorY + 0.25f, floorY + 0.27f},
                {0.34f, winTopY,        winTopY + 0.02f},
                {0.0f,  winTopY,        winTopY + 0.02f},
            }, 0.010f, 0.020f, glossBlack);
            AppendRearPatch(mesh, surf, 0.22f, 0.30f, floorY + 0.56f, floorY + 0.68f, 2, 1, 0.026f, tailRed);
            // Classic round 7-inch school bus red brake/amber turn/reverse circles
            for (float side : {-1.0f, 1.0f}) {
                AppendRearDisc(mesh, surf, side * 0.78f, floorY + 0.58f, 0.07f, 0.065f, 0.012f, 0.028f, tailRed, 12);
                AppendRearDisc(mesh, surf, side * 0.78f, floorY + 0.40f, 0.055f, 0.050f, 0.012f, 0.028f, amberSig, 10);
                AppendRearDisc(mesh, surf, side * 0.58f, floorY + 0.42f, 0.045f, 0.040f, 0.012f, 0.028f, revWhite, 10);
            }
            // Upper black visor strip with 4 circular warning lamps (Red outer, Amber inner)
            AppendRearPatch(mesh, surf, -0.82f, 0.82f, winTopY + 0.01f, std::min(roofTopY - 0.04f, winTopY + 0.19f), 12, 2, 0.016f, glossBlack);
            float wLampY = std::min(roofTopY - 0.09f, winTopY + 0.10f);
            AppendRearDisc(mesh, surf, -0.66f, wLampY, 0.055f, 0.050f, 0.016f, 0.032f, tailRed, 10);
            AppendRearDisc(mesh, surf, -0.46f, wLampY, 0.055f, 0.050f, 0.016f, 0.032f, amberSig, 10);
            AppendRearDisc(mesh, surf,  0.46f, wLampY, 0.055f, 0.050f, 0.016f, 0.032f, amberSig, 10);
            AppendRearDisc(mesh, surf,  0.66f, wLampY, 0.055f, 0.050f, 0.016f, 0.032f, tailRed, 10);
            break;
        }

        case MinibusStyle::MATATU_NGANYA:
        default: {
            // Sport diffuser bumper, twin chrome exhaust pipes, JDM smoked LED rear tail blades & rear roof wing spoiler!
            AppendRearPatch(mesh, surf, -0.98f, 0.98f, floorY - 0.02f, floorY + 0.22f, 16, 2, 0.028f, glossBlack);
            AppendRearPatch(mesh, surf, -0.80f, 0.80f, floorY, floorY + 0.03f, 12, 1, 0.034f, accent);
            // Center cyan/magenta Nganya LED light bar
            AppendRearPatch(mesh, surf, -0.46f, 0.46f, winBotY - 0.12f, winBotY - 0.04f, 8, 1, 0.024f, cyanNeon);
            for (float side : {-1.0f, 1.0f}) {
                float u0 = std::min(side * 0.72f, side * 0.94f);
                float u1 = std::max(side * 0.72f, side * 0.94f);
                AppendRearPatch(mesh, surf, u0, u1, floorY + 0.28f, winBotY + 0.18f, 3, 4, 0.020f, glossBlack);
                AppendRearPatch(mesh, surf, u0, u1, floorY + 0.34f, winBotY + 0.10f, 3, 3, 0.024f, tailRed);
                AppendRearPatch(mesh, surf, u0, u1, floorY + 0.44f, floorY + 0.52f, 3, 1, 0.027f, amberSig);
                // Twin chrome sport exhausts
                ChassisComponents::AppendClosedCylinderX(mesh, xRear - 0.18f, xRear + 0.11f, floorY + 0.05f, side * hw * 0.58f, 0.044f, 12, chromeCol, darkTrim);
            }
            // Sporty rear roof spoiler wing
            float spY = roofTopY + 0.04f;
            RunningGear::AppendBox(mesh, Vec3(xRear - 0.12f, roofTopY - 0.02f, -hw * 0.74f), Vec3(xRear + 0.08f, spY, -hw * 0.66f), accent);
            RunningGear::AppendBox(mesh, Vec3(xRear - 0.12f, roofTopY - 0.02f,  hw * 0.66f), Vec3(xRear + 0.08f, spY,  hw * 0.74f), accent);
            RunningGear::AppendBox(mesh, Vec3(xRear - 0.06f, spY, -hw * 0.86f), Vec3(xRear + 0.16f, spY + 0.055f, hw * 0.86f), glossBlack);
            break;
        }
    }
}

void AppendMinibusRearGlass(
    Mesh* mesh,
    MinibusStyle style,
    float xRear,
    float winBotY,
    float winTopY,
    float hw,
    const Vec4& rearGlassColor
) {
    if (!mesh) return;
    const float cornerR = (style == MinibusStyle::SCHOOL_BUS_VAN) ? 0.11f : 0.15f;
    VanRearSurface surf;
    surf.Init(xRear, hw, cornerR);
    AppendRearPatch(mesh, surf, -0.75f, 0.75f, winBotY + 0.02f, winTopY - 0.02f, 12, 4, 0.010f, rearGlassColor);
}

} // namespace sphere::vehicles::minibus
