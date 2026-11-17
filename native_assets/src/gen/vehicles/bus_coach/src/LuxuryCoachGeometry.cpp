#include "LuxuryCoachGeometry.h"
#include "CabComponents.h"
#include "ChassisComponents.h"
#include "RunningGear.h"
#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

namespace sphere::vehicles::coach {

using sphere::trailers::gear::RunningGear;
using sphere::vehicles::cab::CabComponents;
using sphere::vehicles::cab::HullStationX;
using sphere::vehicles::chassis::ChassisComponents;

namespace {

constexpr float kPi = 3.14159265358979323846f;

inline float Smooth01(float t) {
    float c = std::clamp(t, 0.0f, 1.0f);
    return c * c * (3.0f - 2.0f * c);
}

inline Vec3 SafeNormalize(const Vec3& v, const Vec3& fallback = Vec3(-1.0f, 0.0f, 0.0f)) {
    float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
    if (lenSq < 1e-8f) return fallback;
    float inv = 1.0f / std::sqrt(lenSq);
    return Vec3(v.x * inv, v.y * inv, v.z * inv);
}

inline Vec3 OffsetPt(const Vec3& p, const Vec3& n, float d) {
    return Vec3(p.x + n.x * d, p.y + n.y * d, p.z + n.z * d);
}

inline void PushTri(Mesh* mesh, const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec4& col) {
    uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
    Vertex v0; v0.position = p0; v0.color = col;
    Vertex v1; v1.position = p1; v1.color = col;
    Vertex v2; v2.position = p2; v2.color = col;
    mesh->vertices.push_back(v0);
    mesh->vertices.push_back(v1);
    mesh->vertices.push_back(v2);
    mesh->indices.push_back(base + 0);
    mesh->indices.push_back(base + 1);
    mesh->indices.push_back(base + 2);
}

inline void AppendSideStripQuad(
    Mesh* mesh,
    float x0, float x1,
    float yB0, float yT0,
    float yB1, float yT1,
    float zAbs,
    bool isRightSide,
    const Vec4& col
) {
    if (!mesh || x1 <= x0 + 1e-4f) return;
    if (std::min(yT0 - yB0, yT1 - yB1) < 1e-4f && std::max(yT0 - yB0, yT1 - yB1) < 1e-4f) return;
    float z = isRightSide ? zAbs : -zAbs;
    Vec3 p0B(x0, yB0, z), p1B(x1, yB1, z), p1T(x1, yT1, z), p0T(x0, yT0, z);
    if (isRightSide) {
        CabComponents::AppendQuad(mesh, p0B, p1B, p1T, p0T, col);
    } else {
        CabComponents::AppendQuad(mesh, p1B, p0B, p0T, p1T, col);
    }
}

inline void AppendDoubleSidedWallBand(
    Mesh* mesh,
    float x0, float x1,
    float yB0, float yT0,
    float yB1, float yT1,
    float hw,
    float thickness,
    bool isRightSide,
    const Vec4& outerCol,
    const Vec4& innerCol
) {
    if (!mesh || x1 <= x0 + 1e-4f) return;
    float s = isRightSide ? 1.0f : -1.0f;
    float zOut = s * hw;
    float zIn  = s * (hw - thickness);
    if (isRightSide) {
        CabComponents::AppendQuad(mesh, Vec3(x0, yB0, zOut), Vec3(x1, yB1, zOut), Vec3(x1, yT1, zOut), Vec3(x0, yT0, zOut), outerCol);
        CabComponents::AppendQuad(mesh, Vec3(x1, yB1, zIn),  Vec3(x0, yB0, zIn),  Vec3(x0, yT0, zIn),  Vec3(x1, yT1, zIn),  innerCol);
        CabComponents::AppendQuad(mesh, Vec3(x0, yT0, zOut), Vec3(x1, yT1, zOut), Vec3(x1, yT1, zIn),  Vec3(x0, yT0, zIn),  innerCol);
        CabComponents::AppendQuad(mesh, Vec3(x0, yB0, zIn),  Vec3(x1, yB1, zIn),  Vec3(x1, yB1, zOut), Vec3(x0, yB0, zOut), innerCol);
    } else {
        CabComponents::AppendQuad(mesh, Vec3(x1, yB1, zOut), Vec3(x0, yB0, zOut), Vec3(x0, yT0, zOut), Vec3(x1, yT1, zOut), outerCol);
        CabComponents::AppendQuad(mesh, Vec3(x0, yB0, zIn),  Vec3(x1, yB1, zIn),  Vec3(x1, yT1, zIn),  Vec3(x0, yT0, zIn),  innerCol);
        CabComponents::AppendQuad(mesh, Vec3(x1, yT1, zOut), Vec3(x0, yT0, zOut), Vec3(x0, yT0, zIn),  Vec3(x1, yT1, zIn),  innerCol);
        CabComponents::AppendQuad(mesh, Vec3(x1, yB1, zIn),  Vec3(x0, yB0, zIn),  Vec3(x0, yB0, zOut), Vec3(x1, yB1, zOut), innerCol);
    }
}

struct LuxRibbonSample {
    float u;
    float vBot;
    float vTop;
};

// Compound-curved C1 Luxury Touring Coach front fascia, panoramic visor dome, & roof canopy
struct LuxSurface {
    float xFront = 0.0f;
    float xApillar = 0.0f;
    float xRoofJoin = 0.0f;
    float skirtBotY = 0.28f;
    float winBotCenterY = 1.06f;
    float midBotY = 2.26f;
    float midTopY = 2.48f;
    float upperWinTopY = 3.60f;
    float winSideTop = 3.62f;
    float H = 3.82f;
    float hw = 1.275f;

    float uFlat = 0.62f;
    float rCorner = 0.35f;
    float alpha = 0.45f;
    float sinA = 0.0f;
    float cosA = 1.0f;
    float zFlatEdge = 0.0f;
    float Rfront = 1.0f;
    float dxFrontBulge = 0.0f;

    float rTopDome = 0.22f;
    float roofTopY = 3.78f;
    float roofCrown = 0.055f;

    void Init(
        float inXFront,
        float inSkirtBotY,
        float inWinSideTop,
        float inH,
        float inHw
    ) {
        xFront = inXFront;
        xApillar = inXFront + 0.66f;
        xRoofJoin = inXFront + 1.38f;
        skirtBotY = inSkirtBotY;
        winBotCenterY = inSkirtBotY + 0.88f; // ~1.16m panoramic lower windshield sill
        midBotY = inSkirtBotY + 1.98f;       // ~2.26m lower windshield top
        midTopY = midBotY + 0.24f;           // ~2.50m upper swept glass visor bottom
        winSideTop = inWinSideTop;           // ~3.48m
        upperWinTopY = inH - 0.34f;          // ~3.48m upper visor brow
        H = inH;
        hw = inHw;

        sinA = std::sin(alpha);
        cosA = std::cos(alpha);
        zFlatEdge = std::max(0.45f, hw - rCorner * (1.0f - sinA));
        Rfront = zFlatEdge / sinA;
        dxFrontBulge = Rfront * (1.0f - cosA);

        roofTopY = H - 0.045f;
        rTopDome = std::min(0.24f, std::max(0.08f, hw * 0.20f));
    }

    // Subtle upward smile along the lower windshield sill
    float EvalWinBottomY(float u) const {
        float absU = std::abs(std::clamp(u, -1.0f, 1.0f));
        float s = std::min(1.0f, absU / 0.90f);
        return winBotCenterY + 0.06f * std::pow(s, 2.0f);
    }

    // Iconic smiling chrome mustache curve dipping low in center (~0.66m) and sweeping up around headlamps (~0.88m)
    float EvalSmileBarY(float u) const {
        float absU = std::abs(std::clamp(u, -1.0f, 1.0f));
        float t = std::clamp(absU / 0.88f, 0.0f, 1.0f);
        return (skirtBotY + 0.38f) + 0.24f * std::pow(t, 1.75f);
    }

    // Outer swept almond headlamp bottom contour
    float EvalLampBotY(float u) const {
        float absU = std::abs(std::clamp(u, -1.0f, 1.0f));
        float t = std::clamp((absU - 0.56f) / 0.34f, 0.0f, 1.0f);
        return (skirtBotY + 0.39f) + 0.16f * std::pow(t, 1.15f);
    }

    // Outer swept almond headlamp top contour
    float EvalLampTopY(float u) const {
        float absU = std::abs(std::clamp(u, -1.0f, 1.0f));
        float t = std::clamp((absU - 0.56f) / 0.34f, 0.0f, 1.0f);
        return (skirtBotY + 0.55f) + 0.31f * std::pow(t, 0.85f);
    }

    void EvalHeightAnchors(float y, float& xNoseCenter, float& xSideAnchor) const {
        const float chinTop = skirtBotY + 0.14f;
        if (y <= chinTop) {
            float t = std::clamp((y - skirtBotY) / 0.14f, 0.0f, 1.0f);
            float a = t * (kPi * 0.5f);
            xNoseCenter = (xFront + 0.02f) + 0.080f * (1.0f - std::sin(a));
            xSideAnchor = xApillar + 0.030f * std::cos(a);
        } else if (y <= winBotCenterY + 0.06f) {
            float t = std::clamp((y - chinTop) / std::max(0.10f, (winBotCenterY + 0.06f) - chinTop), 0.0f, 1.0f);
            xNoseCenter = (xFront + 0.02f) + 0.065f * t - 0.015f * std::sin(kPi * t);
            xSideAnchor = xApillar;
        } else if (y <= midTopY) {
            // Proud near-vertical main windshield
            float tLow = std::clamp((y - (winBotCenterY + 0.06f)) / std::max(0.20f, midTopY - (winBotCenterY + 0.06f)), 0.0f, 1.0f);
            xNoseCenter = (xFront + 0.085f) + 0.115f * tLow;
            xSideAnchor = xApillar + 0.065f * tLow;
        } else {
            // Swept-back upper panoramic dark visor curving gracefully into aero roof dome
            float tHigh = std::clamp((y - midTopY) / std::max(0.20f, upperWinTopY - midTopY), 0.0f, 1.0f);
            float sweepC = std::pow(tHigh, 1.42f);
            float sweepS = std::pow(tHigh, 1.28f);
            xNoseCenter = (xFront + 0.200f) + 0.430f * sweepC;
            xSideAnchor = (xApillar + 0.065f) + 0.305f * sweepS;
        }
    }

    Vec3 EvalFrontPos(float u, float y) const {
        float xNoseCenter = 0.0f, xSideAnchor = 0.0f;
        EvalHeightAnchors(y, xNoseCenter, xSideAnchor);

        float uc = std::clamp(u, -1.0f, 1.0f);
        float absU = std::abs(uc);
        float sign = (uc >= 0.0f) ? 1.0f : -1.0f;

        float x = 0.0f, z = 0.0f;
        if (absU <= uFlat) {
            float phi = (absU / uFlat) * alpha;
            z = sign * (Rfront * std::sin(phi));
            x = xNoseCenter + Rfront * (1.0f - std::cos(phi));
        } else {
            float tc = (absU - uFlat) / (1.0f - uFlat);
            float phi = alpha + tc * (kPi * 0.5f - alpha);
            z = sign * (zFlatEdge + rCorner * (std::sin(phi) - sinA));
            float dxCorner = std::max(0.05f, xSideAnchor - (xNoseCenter + dxFrontBulge));
            x = (xNoseCenter + dxFrontBulge) + dxCorner * ((cosA - std::cos(phi)) / cosA);
        }

        // Gentle aerodynamic brow crown across upper visor perimeter
        float wTop = std::clamp((y - midTopY) / std::max(0.20f, upperWinTopY - midTopY), 0.0f, 1.0f);
        float dropT = std::pow(std::max(0.0f, (absU - uFlat) / (1.0f - uFlat)), 1.35f);
        float yDropCorner = (upperWinTopY - winSideTop) * dropT;
        float sCenter = std::min(1.0f, absU / uFlat);
        float yArchLift = 0.036f * (1.0f - sCenter * sCenter);
        float yEff = y + wTop * (yArchLift - yDropCorner);
        return Vec3(x, yEff, z);
    }

    Vec3 EvalRoofArchPos(float x, float u, float topDrop = 0.0f) const {
        float uc = std::clamp(u, -1.0f, 1.0f);
        float absU = std::abs(uc);
        float sign = (uc >= 0.0f) ? 1.0f : -1.0f;
        float yPeak = roofTopY - topDrop;
        float zShoulder = hw - rTopDome;
        float nzShoulder = zShoulder / hw;
        float yShoulder = yPeak + roofCrown * (1.0f - nzShoulder * nzShoulder);

        if (absU <= uFlat) {
            float s = absU / uFlat;
            float z = sign * s * zShoulder;
            float nz = z / hw;
            float y = yPeak + roofCrown * (1.0f - nz * nz);
            return Vec3(x, y, z);
        } else {
            float t = (absU - uFlat) / (1.0f - uFlat);
            float a = t * (kPi * 0.5f);
            float z = sign * (zShoulder + rTopDome * std::sin(a));
            float y = winSideTop + (yShoulder - winSideTop) * std::cos(a);
            return Vec3(x, y, z);
        }
    }

    Vec3 EvalDomePos(float u, float vDome) const {
        float vt = std::clamp(vDome, 0.0f, 1.0f);
        Vec3 p0 = EvalFrontPos(u, upperWinTopY);
        Vec3 p1 = EvalRoofArchPos(xRoofJoin, u, 0.0f);
        float phi = vt * (kPi * 0.5f);
        float fx = 0.24f * vt + 0.76f * (1.0f - std::cos(phi));
        float fy = std::sin(phi);
        return Vec3(
            p0.x + (p1.x - p0.x) * fx,
            p0.y + (p1.y - p0.y) * fy,
            p0.z + (p1.z - p0.z) * vt
        );
    }

    Vec3 EvalFrontNormal(float u, float y) const {
        float du = 0.01f;
        float dy = 0.015f;
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
        ));
    }

    Vec3 EvalDomeNormal(float u, float vDome) const {
        float du = 0.01f;
        float dv = 0.015f;
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
        ));
    }
};

void AppendFrontPatch(
    Mesh* mesh,
    const LuxSurface& surf,
    float u0, float u1,
    float y0, float y1,
    int segU, int segV,
    float offsetOut,
    const Vec4& col
) {
    if (!mesh || std::abs(u1 - u0) < 1e-4f || y1 <= y0 + 1e-4f) return;
    float uMin = std::min(u0, u1);
    float uMax = std::max(u0, u1);
    int nu = std::max(1, segU);
    int nv = std::max(1, segV);
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
    const LuxSurface& surf,
    std::vector<LuxRibbonSample> samples,
    float protrudeOut,
    float depthIn,
    const Vec4& col
) {
    if (!mesh || samples.size() < 2) return;
    if (samples.front().u > samples.back().u) {
        std::reverse(samples.begin(), samples.end());
    }
    const size_t n = samples.size();
    std::vector<Vec3> fB(n), fT(n), bB(n), bT(n);
    for (size_t i = 0; i < n; ++i) {
        float u = samples[i].u;
        float yb = std::min(samples[i].vBot, samples[i].vTop - 0.005f);
        float yt = std::max(samples[i].vTop, yb + 0.005f);
        Vec3 pb = surf.EvalFrontPos(u, yb);
        Vec3 nb = surf.EvalFrontNormal(u, yb);
        Vec3 pt = surf.EvalFrontPos(u, yt);
        Vec3 nt = surf.EvalFrontNormal(u, yt);
        fB[i] = OffsetPt(pb, nb, protrudeOut);
        bB[i] = OffsetPt(pb, nb, -depthIn);
        fT[i] = OffsetPt(pt, nt, protrudeOut);
        bT[i] = OffsetPt(pt, nt, -depthIn);
    }
    for (size_t i = 0; i + 1 < n; ++i) {
        CabComponents::AppendQuad(mesh, fB[i], fB[i + 1], fT[i + 1], fT[i], col);
        CabComponents::AppendQuad(mesh, fT[i], fT[i + 1], bT[i + 1], bT[i], col);
        CabComponents::AppendQuad(mesh, bB[i], bB[i + 1], fB[i + 1], fB[i], col);
    }
    CabComponents::AppendQuad(mesh, bB.front(), fB.front(), fT.front(), bT.front(), col);
    CabComponents::AppendQuad(mesh, fB.back(),  bB.back(),  bT.back(),  fT.back(),  col);
}

void AppendSurfaceDisc(
    Mesh* mesh,
    const LuxSurface& surf,
    float uC, float yC,
    float radU, float radY,
    float protrudeOut, float depthIn,
    int segs,
    const Vec4& col
) {
    if (!mesh || segs < 6) return;
    Vec3 pc = surf.EvalFrontPos(uC, yC);
    Vec3 nc = surf.EvalFrontNormal(uC, yC);
    Vec3 centerF = OffsetPt(pc, nc, protrudeOut + 0.003f);

    std::vector<Vec3> rimF(segs), rimB(segs);
    for (int k = 0; k < segs; ++k) {
        float a = 2.0f * kPi * (static_cast<float>(k) / static_cast<float>(segs));
        float uk = uC + radU * std::cos(a);
        float yk = yC + radY * std::sin(a);
        Vec3 pk = surf.EvalFrontPos(uk, yk);
        Vec3 nk = surf.EvalFrontNormal(uk, yk);
        rimF[k] = OffsetPt(pk, nk, protrudeOut);
        rimB[k] = OffsetPt(pk, nk, -depthIn);
    }
    for (int k = 0; k < segs; ++k) {
        int kn = (k + 1) % segs;
        PushTri(mesh, centerF, rimF[k], rimF[kn], col);
        CabComponents::AppendQuad(mesh, rimB[k], rimB[kn], rimF[kn], rimF[k], col);
    }
}

void AppendSurfaceRing(
    Mesh* mesh,
    const LuxSurface& surf,
    float uC, float yC,
    float radUIn, float radYIn,
    float radUOut, float radYOut,
    float protrudeOut, float depthIn,
    int segs,
    const Vec4& col
) {
    if (!mesh || segs < 6) return;
    std::vector<Vec3> inF(segs), outF(segs), outB(segs);
    for (int k = 0; k < segs; ++k) {
        float a = 2.0f * kPi * (static_cast<float>(k) / static_cast<float>(segs));
        float ca = std::cos(a), sa = std::sin(a);
        Vec3 pi = surf.EvalFrontPos(uC + radUIn * ca, yC + radYIn * sa);
        Vec3 ni = surf.EvalFrontNormal(uC + radUIn * ca, yC + radYIn * sa);
        Vec3 po = surf.EvalFrontPos(uC + radUOut * ca, yC + radYOut * sa);
        Vec3 no = surf.EvalFrontNormal(uC + radUOut * ca, yC + radYOut * sa);
        inF[k]  = OffsetPt(pi, ni, protrudeOut);
        outF[k] = OffsetPt(po, no, protrudeOut);
        outB[k] = OffsetPt(po, no, -depthIn);
    }
    for (int k = 0; k < segs; ++k) {
        int kn = (k + 1) % segs;
        CabComponents::AppendQuad(mesh, inF[k], outF[k], outF[kn], inF[kn], col);
        CabComponents::AppendQuad(mesh, outB[k], outB[kn], outF[kn], outF[k], col);
    }
}

void AppendSmoothDome(
    Mesh* mesh,
    const LuxSurface& surf,
    float u0, float u1,
    float v0, float v1,
    float offsetOut,
    int segU, int segV,
    const Vec4& col
) {
    if (!mesh || u1 <= u0 + 1e-4f || v1 <= v0 + 1e-4f) return;
    int nu = std::max(2, segU);
    int nv = std::max(1, segV);
    for (int j = 0; j < nv; ++j) {
        float va = v0 + (v1 - v0) * (static_cast<float>(j) / static_cast<float>(nv));
        float vb = v0 + (v1 - v0) * (static_cast<float>(j + 1) / static_cast<float>(nv));
        for (int i = 0; i < nu; ++i) {
            float ua = u0 + (u1 - u0) * (static_cast<float>(i) / static_cast<float>(nu));
            float ub = u0 + (u1 - u0) * (static_cast<float>(i + 1) / static_cast<float>(nu));
            Vec3 p0a = OffsetPt(surf.EvalDomePos(ua, va), surf.EvalDomeNormal(ua, va), offsetOut);
            Vec3 p0b = OffsetPt(surf.EvalDomePos(ub, va), surf.EvalDomeNormal(ub, va), offsetOut);
            Vec3 p1b = OffsetPt(surf.EvalDomePos(ub, vb), surf.EvalDomeNormal(ub, vb), offsetOut);
            Vec3 p1a = OffsetPt(surf.EvalDomePos(ua, vb), surf.EvalDomeNormal(ua, vb), offsetOut);
            CabComponents::AppendQuad(mesh, p0a, p0b, p1b, p1a, col);
        }
    }
}

void AppendDomeLight(
    Mesh* mesh,
    const LuxSurface& surf,
    float u0, float u1,
    float v0, float v1,
    float protrudeOut,
    const Vec4& col
) {
    if (!mesh) return;
    float uMin = std::min(u0, u1), uMax = std::max(u0, u1);
    const int segU = 4;
    for (int i = 0; i < segU; ++i) {
        float ua = uMin + (uMax - uMin) * (static_cast<float>(i) / static_cast<float>(segU));
        float ub = uMin + (uMax - uMin) * (static_cast<float>(i + 1) / static_cast<float>(segU));
        Vec3 a = OffsetPt(surf.EvalDomePos(ua, v0), surf.EvalDomeNormal(ua, v0), protrudeOut);
        Vec3 b = OffsetPt(surf.EvalDomePos(ub, v0), surf.EvalDomeNormal(ub, v0), protrudeOut);
        Vec3 c = OffsetPt(surf.EvalDomePos(ub, v1), surf.EvalDomeNormal(ub, v1), protrudeOut);
        Vec3 d = OffsetPt(surf.EvalDomePos(ua, v1), surf.EvalDomeNormal(ua, v1), protrudeOut);
        CabComponents::AppendQuad(mesh, a, b, c, d, col);
    }
}

// Disjoint non-overlapping curved nose skin:
// Pearl White lower bumper apron + gloss-black smiling center cowl + outer swept almond lamp pockets + lower fog recesses
void AppendLuxuryIntegratedNoseSkin(
    Mesh* mesh,
    const LuxSurface& surf,
    const Vec4& bodyPaint,
    const Vec4& glossBlack,
    const Vec4& darkTrim
) {
    const int nu = 44;
    const float yFogBot = surf.skirtBotY + 0.08f;
    const float yFogTop = surf.skirtBotY + 0.22f;
    const float yPlateBot = surf.skirtBotY + 0.14f;
    const float yPlateTop = surf.skirtBotY + 0.30f;

    for (int i = 0; i < nu; ++i) {
        float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
        float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
        float uMid = 0.5f * (ua + ub);
        float absU = std::abs(uMid);

        auto emitColBand = [&](float y0a, float y0b, float y1a, float y1b, int segs, const Vec4& col) {
            if (std::max(y1a - y0a, y1b - y0b) <= 1e-4f) return;
            int nv = std::max(1, segs);
            for (int j = 0; j < nv; ++j) {
                float f0 = static_cast<float>(j) / static_cast<float>(nv);
                float f1 = static_cast<float>(j + 1) / static_cast<float>(nv);
                float ya0 = y0a + (y1a - y0a) * f0;
                float yb0 = y0b + (y1b - y0b) * f0;
                float ya1 = y0a + (y1a - y0a) * f1;
                float yb1 = y0b + (y1b - y0b) * f1;
                Vec3 a = surf.EvalFrontPos(ua, ya0);
                Vec3 b = surf.EvalFrontPos(ub, yb0);
                Vec3 c = surf.EvalFrontPos(ub, yb1);
                Vec3 d = surf.EvalFrontPos(ua, ya1);
                CabComponents::AppendQuad(mesh, a, b, c, d, col);
            }
        };

        float ySmA = surf.EvalSmileBarY(ua);
        float ySmB = surf.EvalSmileBarY(ub);
        float yWinA = surf.EvalWinBottomY(ua);
        float yWinB = surf.EvalWinBottomY(ub);

        if (absU <= 0.26f) {
            // Center plate recess & pearl-white chin + upper gloss-black smiling cowl
            emitColBand(surf.skirtBotY, surf.skirtBotY, yPlateBot, yPlateBot, 2, bodyPaint);
            emitColBand(yPlateBot, yPlateBot, yPlateTop, yPlateTop, 2, (absU <= 0.20f) ? Vec4(0.84f, 0.85f, 0.87f, 1.0f) : bodyPaint);
            emitColBand(yPlateTop, yPlateTop, ySmA, ySmB, 2, bodyPaint);
            emitColBand(ySmA, ySmB, yWinA, yWinB, 4, glossBlack);
        } else if (absU <= 0.56f) {
            // Smooth Pearl White cheek below smiling chrome line + black nose cowl above
            emitColBand(surf.skirtBotY, surf.skirtBotY, ySmA, ySmB, 4, bodyPaint);
            emitColBand(ySmA, ySmB, yWinA, yWinB, 4, glossBlack);
        } else if (absU <= 0.89f) {
            // Swept almond multi-projector headlamp pocket + lower bumper fog pocket
            float lBA = surf.EvalLampBotY(ua), lBB = surf.EvalLampBotY(ub);
            float lTA = std::min(yWinA, surf.EvalLampTopY(ua));
            float lTB = std::min(yWinB, surf.EvalLampTopY(ub));
            emitColBand(surf.skirtBotY, surf.skirtBotY, yFogBot, yFogBot, 1, bodyPaint);
            emitColBand(yFogBot, yFogBot, yFogTop, yFogTop, 2, (absU >= 0.65f && absU <= 0.82f) ? darkTrim : bodyPaint);
            emitColBand(yFogTop, yFogTop, lBA, lBB, 2, bodyPaint);
            emitColBand(lBA, lBB, lTA, lTB, 3, glossBlack);
            if (yWinA > lTA + 0.01f || yWinB > lTB + 0.01f) {
                emitColBand(lTA, lTB, std::max(lTA, yWinA), std::max(lTB, yWinB), 2, bodyPaint);
            }
        } else {
            // Gloss-black outer lower windshield corner frame merging seamlessly down into pearl white outer cheek
            float yCornerSplit = surf.skirtBotY + 0.85f;
            emitColBand(surf.skirtBotY, surf.skirtBotY, yCornerSplit, yCornerSplit, 5, bodyPaint);
            emitColBand(yCornerSplit, yCornerSplit, yWinA, yWinB, 4, glossBlack);
        }

        // Bottom chin closure
        Vec3 botA = surf.EvalFrontPos(ua, surf.skirtBotY);
        Vec3 botB = surf.EvalFrontPos(ub, surf.skirtBotY);
        CabComponents::AppendQuad(mesh, Vec3(surf.xApillar, surf.skirtBotY, botA.z), Vec3(surf.xApillar, surf.skirtBotY, botB.z), botB, botA, bodyPaint);
    }
}

void AppendLuxuryFasciaDetails(
    Mesh* mesh,
    const LuxSurface& surf,
    bool headlightsOn,
    const Vec4& bodyPaint,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol
) {
    const Vec4 headlampBulb = headlightsOn ? Vec4(0.96f, 0.98f, 1.00f, 1.0f) : Vec4(0.62f, 0.68f, 0.75f, 1.0f);
    const Vec4 drlStrip     = headlightsOn ? Vec4(0.95f, 0.97f, 1.00f, 1.0f) : Vec4(0.76f, 0.80f, 0.86f, 1.0f);
    const Vec4 amberCol     = headlightsOn ? Vec4(1.00f, 0.58f, 0.08f, 1.0f) : Vec4(0.64f, 0.36f, 0.08f, 1.0f);
    const Vec4 fogLens      = headlightsOn ? Vec4(0.94f, 0.96f, 1.00f, 1.0f) : Vec4(0.60f, 0.66f, 0.74f, 1.0f);
    const Vec4 markerBlue   = headlightsOn ? Vec4(0.18f, 0.56f, 1.00f, 1.0f) : Vec4(0.14f, 0.36f, 0.68f, 1.0f);

    // 1. Wide smiling chrome mustache bar beneath gloss-black cowl & center crest shield badge
    {
        std::vector<LuxRibbonSample> smile;
        for (int k = -14; k <= 14; ++k) {
            float u = 0.88f * (static_cast<float>(k) / 14.0f);
            float ys = (std::abs(u) <= 0.56f) ? surf.EvalSmileBarY(u) : surf.EvalLampBotY(u);
            smile.push_back({u, ys - 0.015f, ys + 0.016f});
        }
        AppendFrontRibbon(mesh, surf, smile, 0.016f, 0.008f, chromeCol);
    }
    // Vertical star/shield chrome badge in center of gloss-black cowl
    {
        float yShieldC = 0.5f * (surf.EvalSmileBarY(0.0f) + surf.EvalWinBottomY(0.0f)) + 0.01f;
        AppendFrontRibbon(mesh, surf, {{-0.042f, yShieldC - 0.068f, yShieldC + 0.068f}, {0.042f, yShieldC - 0.068f, yShieldC + 0.068f}}, 0.018f, 0.006f, chromeCol);
        AppendFrontRibbon(mesh, surf, {{-0.032f, yShieldC - 0.052f, yShieldC + 0.052f}, {0.032f, yShieldC - 0.052f, yShieldC + 0.052f}}, 0.022f, 0.004f, darkTrim);
        AppendSurfaceDisc(mesh, surf, 0.0f, yShieldC, 0.024f, 0.032f, 0.025f, 0.004f, 12, chromeCol);
    }

    // 2. Swept almond multi-projector headlamps & lower bumper fog lamps
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        // Chrome upper eyelid trim curving over almond headlamp
        AppendFrontRibbon(
            mesh, surf,
            {{s * 0.56f, surf.EvalLampBotY(s * 0.56f), surf.EvalLampBotY(s * 0.56f) + 0.022f},
             {s * 0.72f, surf.EvalLampTopY(s * 0.72f) - 0.015f, surf.EvalLampTopY(s * 0.72f) + 0.012f},
             {s * 0.89f, surf.EvalLampTopY(s * 0.89f) - 0.018f, surf.EvalLampTopY(s * 0.89f) + 0.010f}},
            0.015f, 0.006f, chromeCol
        );
        // Triple stepped projector lamps along rising almond housing
        for (int p = 0; p < 3; ++p) {
            float fp = static_cast<float>(p) / 2.0f;
            float uP = s * (0.625f + fp * 0.175f);
            float yP = 0.5f * (surf.EvalLampBotY(uP) + surf.EvalLampTopY(uP));
            float r = 0.024f + fp * 0.004f;
            AppendSurfaceRing(mesh, surf, uP, yP, r, r * 1.15f, r + 0.014f, r * 1.15f + 0.014f, 0.014f, 0.006f, 12, chromeCol);
            AppendSurfaceDisc(mesh, surf, uP, yP, r, r * 1.15f, 0.017f, 0.005f, 12, headlampBulb);
        }
        // Bright swept DRL eyebrow & corner amber turn signal triangle
        AppendFrontRibbon(
            mesh, surf,
            {{s * 0.58f, surf.EvalLampBotY(s * 0.58f) + 0.02f, surf.EvalLampBotY(s * 0.58f) + 0.042f},
             {s * 0.86f, surf.EvalLampBotY(s * 0.86f) + 0.04f, surf.EvalLampBotY(s * 0.86f) + 0.062f}},
            0.016f, 0.005f, drlStrip
        );
        AppendFrontRibbon(
            mesh, surf,
            {{s * 0.83f, surf.EvalLampBotY(s * 0.83f) + 0.05f, surf.EvalLampTopY(s * 0.83f) - 0.02f},
             {s * 0.89f, surf.EvalLampBotY(s * 0.89f) + 0.05f, surf.EvalLampTopY(s * 0.89f) - 0.02f}},
            0.016f, 0.005f, amberCol
        );
        // Lower round fog lamp inside dark corner pocket
        AppendSurfaceRing(mesh, surf, s * 0.73f, surf.skirtBotY + 0.15f, 0.026f, 0.028f, 0.040f, 0.042f, 0.014f, 0.006f, 12, chromeCol);
        AppendSurfaceDisc(mesh, surf, s * 0.73f, surf.skirtBotY + 0.15f, 0.025f, 0.027f, 0.017f, 0.006f, 12, fogLens);
    }

    // 3. Flush gloss-black mid-divider [midBotY .. midTopY], perimeter sill, and full-height gloss-black A-pillars
    AppendFrontPatch(mesh, surf, -0.90f, 0.90f, surf.midBotY, surf.midTopY, 20, 3, 0.002f, glossBlack);
    {
        std::vector<LuxRibbonSample> sill;
        for (int k = -12; k <= 12; ++k) {
            float u = 0.90f * (static_cast<float>(k) / 12.0f);
            float ys = surf.EvalWinBottomY(u);
            sill.push_back({u, ys - 0.015f, ys + 0.024f});
        }
        AppendFrontRibbon(mesh, surf, sill, 0.004f, 0.014f, glossBlack);
    }
    for (int side : {-1, 1}) {
        float uA = (side < 0) ? -1.00f : 0.885f;
        float uB = (side < 0) ? -0.885f : 1.00f;
        AppendFrontPatch(mesh, surf, uA, uB, surf.EvalWinBottomY(0.90f) - 0.02f, surf.upperWinTopY, 4, 14, 0.002f, glossBlack);
    }

    // 4. Swept gloss-black curved roof dome brow with 3 blue LED roof markers + outer white corner shoulders
    AppendSmoothDome(mesh, surf, -0.92f, 0.92f, 0.0f, 0.42f, 0.002f, 22, 5, glossBlack);
    AppendSmoothDome(mesh, surf, -0.92f, 0.92f, 0.42f, 1.0f, 0.002f, 22, 6, bodyPaint);
    AppendSmoothDome(mesh, surf, -1.00f, -0.92f, 0.0f, 1.0f, 0.002f, 3, 10, bodyPaint);
    AppendSmoothDome(mesh, surf,  0.92f,  1.00f, 0.0f, 1.0f, 0.002f, 3, 10, bodyPaint);

    // 3 signature blue LED marker pills + 2 outer clear marker pills
    for (float cu : {-0.22f, 0.0f, 0.22f}) {
        AppendDomeLight(mesh, surf, cu - 0.045f, cu + 0.045f, 0.09f, 0.18f, 0.012f, markerBlue);
    }
    AppendDomeLight(mesh, surf, -0.78f, -0.68f, 0.11f, 0.19f, 0.012f, chromeCol);
    AppendDomeLight(mesh, surf,  0.68f,  0.78f, 0.11f, 0.19f, 0.012f, chromeCol);

    // Dual opposing panoramic wipers
    AppendFrontRibbon(mesh, surf, {{-0.62f, surf.EvalWinBottomY(-0.62f) + 0.02f, surf.EvalWinBottomY(-0.62f) + 0.05f}, {-0.02f, surf.winBotCenterY + 0.03f, surf.winBotCenterY + 0.06f}}, 0.018f, 0.006f, glossBlack);
    AppendFrontRibbon(mesh, surf, {{0.02f, surf.winBotCenterY + 0.03f, surf.winBotCenterY + 0.06f}, {0.62f, surf.EvalWinBottomY(0.62f) + 0.02f, surf.EvalWinBottomY(0.62f) + 0.05f}}, 0.018f, 0.006f, glossBlack);
}

void AppendLuxuryRoofAndACPod(
    Mesh* mesh,
    const LuxSurface& surf,
    float winEndX,
    float xRear,
    const Vec4& bodyPaint,
    const Vec4& darkTrim
) {
    const int nu = 24;
    struct XStation { float x; float topDrop; };
    std::vector<XStation> stations = {
        {surf.xRoofJoin, 0.0f},
        {surf.xFront + 2.5f, 0.0f},
        {winEndX,        0.0f},
        {xRear - 0.14f,  0.04f},
        {xRear,          0.09f}
    };
    for (size_t s = 0; s + 1 < stations.size(); ++s) {
        float x0 = stations[s].x, d0 = stations[s].topDrop;
        float x1 = stations[s + 1].x, d1 = stations[s + 1].topDrop;
        for (int i = 0; i < nu; ++i) {
            float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
            float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
            CabComponents::AppendQuad(
                mesh,
                surf.EvalRoofArchPos(x0, ua, d0),
                surf.EvalRoofArchPos(x0, ub, d0),
                surf.EvalRoofArchPos(x1, ub, d1),
                surf.EvalRoofArchPos(x1, ua, d1),
                bodyPaint
            );
        }
    }
    Vec3 rearCenter(xRear, surf.winSideTop + 0.04f, 0.0f);
    for (int i = 0; i < nu; ++i) {
        float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
        float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
        PushTri(mesh, rearCenter, surf.EvalRoofArchPos(xRear, ub, 0.09f), surf.EvalRoofArchPos(xRear, ua, 0.09f), bodyPaint);
    }

    // Streamlined white integrated A/C condenser pod (matching Image 2 & Image 3 side silhouette)
    float ac0 = surf.xRoofJoin + 1.10f;
    float ac1 = xRear - 3.90f;
    std::vector<HullStationX> acPod = {
        {ac0,         surf.H - 0.04f, surf.H + 0.04f, 0.80f, 0.05f, 0.14f, 0.03f, 0.0f},
        {ac0 + 0.70f, surf.H - 0.04f, surf.H + 0.14f, 0.90f, 0.05f, 0.14f, 0.03f, 0.0f},
        {ac1 - 0.50f, surf.H - 0.04f, surf.H + 0.14f, 0.90f, 0.05f, 0.14f, 0.03f, 0.0f},
        {ac1,         surf.H - 0.04f, surf.H + 0.04f, 0.80f, 0.05f, 0.14f, 0.03f, 0.0f},
    };
    CabComponents::AppendSculptedHullX(mesh, acPod, bodyPaint, 4);
    RunningGear::AppendBox(mesh, Vec3(ac0 + 0.95f, surf.H + 0.01f, -0.92f), Vec3(ac1 - 0.85f, surf.H + 0.08f, -0.89f), darkTrim);
    RunningGear::AppendBox(mesh, Vec3(ac0 + 0.95f, surf.H + 0.01f,  0.89f), Vec3(ac1 - 0.85f, surf.H + 0.08f,  0.92f), darkTrim);
}

void AppendLuxurySwanNeckMirrors(
    Mesh* mesh,
    const LuxSurface& surf,
    const Vec4& bodyPaint,
    const Vec4& darkTrim
) {
    float mountY = surf.midTopY - 0.04f; // ~2.44m right at cowl beltline
    float armX   = surf.xFront + 0.66f;
    const Vec4 mirrorFace(0.68f, 0.74f, 0.82f, 1.0f);

    // Driver side (-Z): extended forward-swept white gooseneck aero arm + large vertical twin-glass head (Image 1)
    {
        std::vector<Vec3> spine = {
            Vec3(armX + 0.04f,  mountY,         -surf.hw + 0.02f),
            Vec3(armX - 0.18f,  mountY + 0.05f, -surf.hw - 0.18f),
            Vec3(armX - 0.44f,  mountY + 0.08f, -surf.hw - 0.30f),
            Vec3(armX - 0.66f,  mountY + 0.02f, -surf.hw - 0.30f),
            Vec3(armX - 0.72f,  mountY - 0.12f, -surf.hw - 0.30f),
        };
        for (size_t i = 0; i + 1 < spine.size(); ++i) {
            Vec3 a = spine[i], b = spine[i + 1];
            float r = (i == 0) ? 0.042f : 0.032f;
            Vec3 mn(std::min(a.x, b.x) - r, std::min(a.y, b.y) - r, std::min(a.z, b.z) - r);
            Vec3 mx(std::max(a.x, b.x) + r, std::max(a.y, b.y) + r, std::max(a.z, b.z) + r);
            RunningGear::AppendBox(mesh, mn, mx, bodyPaint);
        }
        Vec3 hC = spine.back();
        RunningGear::AppendBox(mesh, Vec3(hC.x - 0.07f, hC.y - 0.24f, hC.z - 0.06f), Vec3(hC.x + 0.07f, hC.y + 0.06f, hC.z + 0.06f), bodyPaint);
        RunningGear::AppendBox(mesh, Vec3(hC.x + 0.055f, hC.y - 0.22f, hC.z - 0.045f), Vec3(hC.x + 0.078f, hC.y + 0.04f, hC.z + 0.045f), mirrorFace);
        RunningGear::AppendBox(mesh, Vec3(hC.x - 0.075f, hC.y - 0.18f, hC.z - 0.03f), Vec3(hC.x - 0.060f, hC.y - 0.10f, hC.z + 0.03f), darkTrim);
    }

    // Curb side (+Z): sculpted shorter downward gooseneck arm + vertical pod
    {
        std::vector<Vec3> spine = {
            Vec3(armX + 0.04f,  mountY,         surf.hw - 0.02f),
            Vec3(armX - 0.18f,  mountY + 0.05f, surf.hw + 0.14f),
            Vec3(armX - 0.42f,  mountY + 0.04f, surf.hw + 0.20f),
            Vec3(armX - 0.48f,  mountY - 0.10f, surf.hw + 0.20f),
        };
        for (size_t i = 0; i + 1 < spine.size(); ++i) {
            Vec3 a = spine[i], b = spine[i + 1];
            float r = (i == 0) ? 0.042f : 0.032f;
            Vec3 mn(std::min(a.x, b.x) - r, std::min(a.y, b.y) - r, std::min(a.z, b.z) - r);
            Vec3 mx(std::max(a.x, b.x) + r, std::max(a.y, b.y) + r, std::max(a.z, b.z) + r);
            RunningGear::AppendBox(mesh, mn, mx, bodyPaint);
        }
        Vec3 hC = spine.back();
        RunningGear::AppendBox(mesh, Vec3(hC.x - 0.07f, hC.y - 0.26f, hC.z - 0.06f), Vec3(hC.x + 0.07f, hC.y + 0.06f, hC.z + 0.06f), bodyPaint);
        RunningGear::AppendBox(mesh, Vec3(hC.x + 0.055f, hC.y - 0.24f, hC.z - 0.045f), Vec3(hC.x + 0.078f, hC.y + 0.04f, hC.z + 0.045f), mirrorFace);
    }
}

void AppendLuxuryRearDesign(
    Mesh* mesh,
    float xRear,
    float skirtBotY,
    float winBotY,
    float winSideTop,
    float H,
    float hw,
    bool headlightsOn,
    const Vec4& bodyPaint,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol
) {
    float hwRear = std::max(0.80f, hw - 0.05f);
    const Vec4 tailRed   = headlightsOn ? Vec4(0.95f, 0.12f, 0.14f, 1.0f) : Vec4(0.55f, 0.10f, 0.12f, 1.0f);
    const Vec4 tailAmber = headlightsOn ? Vec4(1.00f, 0.62f, 0.08f, 1.0f) : Vec4(0.58f, 0.36f, 0.08f, 1.0f);
    const Vec4 revWhite  = headlightsOn ? Vec4(0.94f, 0.96f, 1.00f, 1.0f) : Vec4(0.62f, 0.65f, 0.70f, 1.0f);
    const Vec4 plateWht  = Vec4(0.94f, 0.95f, 0.97f, 1.0f);

    auto rearPos = [&](float u, float y) -> Vec3 {
        float uc = std::clamp(u, -1.0f, 1.0f);
        float z = uc * (hwRear - 0.08f);
        float nz = std::abs(uc);
        float cx = 0.032f * (1.0f - nz * nz);
        float ty = std::clamp((y - skirtBotY) / std::max(0.3f, winSideTop - skirtBotY), 0.0f, 1.0f);
        return Vec3(xRear + cx - 0.025f * ty, y, z);
    };

    auto appendRearQuad = [&](float u0, float u1, float y0, float y1, float dxOut, const Vec4& col) {
        Vec3 a = rearPos(u0, y0), b = rearPos(u1, y0), c = rearPos(u1, y1), d = rearPos(u0, y1);
        a.x += dxOut; b.x += dxOut; c.x += dxOut; d.x += dxOut;
        CabComponents::AppendQuad(mesh, b, a, d, c, col);
    };

    // Sculpted rear bumper + plate + gloss-black rear window + chrome rear trim bar
    appendRearQuad(-0.96f, 0.96f, skirtBotY + 0.10f, skirtBotY + 0.34f, 0.018f, bodyPaint);
    appendRearQuad(-0.94f, 0.94f, skirtBotY + 0.10f, skirtBotY + 0.16f, 0.022f, darkTrim);
    appendRearQuad(-0.18f, 0.18f, skirtBotY + 0.17f, skirtBotY + 0.27f, 0.024f, plateWht);
    appendRearQuad(-0.82f, 0.82f, winBotY + 0.16f, winSideTop - 0.16f, 0.012f, glossBlack);
    appendRearQuad(-0.48f, 0.48f, winBotY + 0.10f, winBotY + 0.13f, 0.018f, chromeCol);

    // Engine bay panel seams & chrome louvers
    appendRearQuad(-0.46f, 0.46f, skirtBotY + 0.42f, winBotY - 0.40f, 0.006f, glossBlack);
    for (int i = 0; i < 4; ++i) {
        float sy = (skirtBotY + 0.48f) + 0.16f * static_cast<float>(i);
        appendRearQuad(-0.42f, 0.42f, sy - 0.012f, sy + 0.012f, 0.014f, chromeCol);
    }

    // Swept horizontal + vertical boomerang rear LED tail lamps + dual high brake strip
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float u0 = std::min(s * 0.62f, s * 0.90f), u1 = std::max(s * 0.62f, s * 0.90f);
        appendRearQuad(u0, u1, skirtBotY + 0.44f, skirtBotY + 0.78f, 0.014f, glossBlack);
        appendRearQuad(u0, u1, skirtBotY + 0.56f, skirtBotY + 0.66f, 0.022f, tailRed);
        appendRearQuad(std::min(s * 0.72f, s * 0.88f), std::max(s * 0.72f, s * 0.88f), skirtBotY + 0.68f, skirtBotY + 0.76f, 0.022f, tailAmber);
        appendRearQuad(std::min(s * 0.64f, s * 0.74f), std::max(s * 0.64f, s * 0.74f), skirtBotY + 0.46f, skirtBotY + 0.54f, 0.022f, revWhite);
    }
    appendRearQuad(-0.34f, 0.34f, H - 0.22f, H - 0.16f, 0.022f, tailRed);
}

} // namespace

std::shared_ptr<Mesh> GenerateLuxuryCoachMesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(def.dimensions.x, 11.2f, 13.8f);
    const float W = std::clamp(def.dimensions.y, 2.45f, 2.60f);
    const float H = std::clamp(def.dimensions.z, 3.60f, 3.95f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(def.wheel_radius_m, 0.46f, 0.54f);
    const float tireW = std::clamp(def.tire_width_m, 0.27f, 0.34f);

    const float frontHalfTrack = hw - tireW * 0.50f - 0.025f;
    const float rearHalfTrack  = hw - tireW * 1.06f - 0.025f;

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float skirtBotY = std::clamp(def.ground_clearance_m, 0.22f, 0.36f);
    const float archTopY = wheelR * 2.0f + 0.14f; // ~1.14m
    const float archHalfLen = wheelR + 0.13f;

    // Creamy Pearl White + Silver Chrome + Orange-Gold accents + Transparent Coach Tinted Glass
    const Vec4 bodyPaint = def.base_color;
    const Vec4 orangeAccent = def.accent_color;
    const Vec4 glossBlack(0.055f, 0.060f, 0.075f, 1.0f);
    const Vec4 darkTrim(0.11f, 0.12f, 0.14f, 1.0f);
    const Vec4 linerDark(0.05f, 0.055f, 0.065f, 1.0f);
    const Vec4 steelBaffle(0.42f, 0.45f, 0.50f, 1.0f);
    const Vec4 chromeCol(0.86f, 0.89f, 0.94f, 1.0f);
    const Vec4 amberMarker(1.00f, 0.58f, 0.08f, 1.0f);
    const Vec4 interiorSeat(0.20f, 0.27f, 0.36f, 1.0f);
    const Vec4 interiorFloor(0.18f, 0.20f, 0.24f, 1.0f);

    // Transparent automotive windows (lower windshield clear tinted, upper visor sleek deep smoke, side panes transparent)
    const Vec4 windshieldGlass(0.18f, 0.32f, 0.44f, 0.34f);
    const Vec4 upperVisorGlass(0.08f, 0.13f, 0.19f, 0.58f);
    const Vec4 sideGlass(0.14f, 0.24f, 0.34f, 0.42f);

    // 1. Frame rails & 4x2 axles
    ChassisComponents::AppendFrameRails(mesh.get(), xFront + 0.35f, xRear - 0.22f, skirtBotY + 0.06f, skirtBotY + 0.24f, 0.45f, darkTrim);

    const float frontAxleX = xFront + 2.48f;
    const float driveAxleX = std::min(xRear - 2.35f, frontAxleX + def.wheelbase_m);

    ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxleX, wheelR, tireW, frontHalfTrack, false, def.steer_angle_deg);
    ChassisComponents::AppendVehicleAxle(mesh.get(), driveAxleX, wheelR, tireW, rearHalfTrack, true, 0.0f);

    // Semicircular cleanly-cut wheel wells & flared wheel-arch eyebrows
    auto emitWheelBay = [&](float ax, float innerZ) {
        const float cutR = wheelR + 0.085f;
        CabComponents::AppendWheelWellLiner(mesh.get(), ax, archHalfLen, skirtBotY, archTopY, hw, innerZ, linerDark);
        CabComponents::AppendSemicircularWheelArchSkirt(mesh.get(), ax, wheelR, cutR, archHalfLen, skirtBotY, archTopY, -hw, false, 0.07f, bodyPaint, linerDark);
        CabComponents::AppendSemicircularWheelArchSkirt(mesh.get(), ax, wheelR, cutR, archHalfLen, skirtBotY, archTopY,  hw, true,  0.07f, bodyPaint, linerDark);
        CabComponents::AppendWheelArchFender(mesh.get(), ax, wheelR, cutR, -hw, false, 0.036f, bodyPaint);
        CabComponents::AppendWheelArchFender(mesh.get(), ax, wheelR, cutR,  hw, true,  0.036f, bodyPaint);
    };
    emitWheelBay(frontAxleX, hw - tireW - 0.16f);
    emitWheelBay(driveAxleX, hw - tireW * 2.15f - 0.14f);

    // Landmarks calibrated from Images 1-3 (4x2 12.0m High-Decker Luxury Touring Coach)
    const float entryFrontX = xFront + 0.72f;
    const float entryBackX  = xFront + 1.54f;
    const float winEndX     = xRear - 0.56f;
    const float winSideTop  = H - 0.34f;        // ~3.48m (sleeker ~1.04m glass band + thicker real roof cantrail)
    const float winMainBot  = skirtBotY + 2.16f;// ~2.44m panoramic window beltline
    const float bootTopY    = skirtBotY + 1.24f;// ~1.52m tall underfloor luggage boot beltline
    const float deckFloorY  = bootTopY + 0.06f; // ~1.58m passenger cabin floor above luggage holds

    LuxSurface frontSurf;
    frontSurf.Init(xFront, skirtBotY, winSideTop, H, hw);

    // 2. Full 3D VIP Interior (Cockpit, stairwell behind +Z door, 12 rows of VIP seats, upper dome bulkhead)
    CabComponents::AppendBusInterior(
        mesh.get(),
        xFront + 0.80f, winEndX - 0.04f,
        deckFloorY, winSideTop - 0.08f,
        hw - 0.05f,
        entryFrontX, entryBackX,
        skirtBotY + 0.10f,
        12, false, 0.0f, 0.0f,
        interiorSeat, interiorFloor, darkTrim
    );
    // Sealed lower dashboard shelf & upper dome modesty liner inset cleanly inside curved front shell
    RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.44f, skirtBotY + 0.22f, -hw + 0.34f), Vec3(xFront + 0.94f, frontSurf.winBotCenterY - 0.02f, hw - 0.34f), darkTrim);
    RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.78f, frontSurf.midBotY - 0.02f, -hw + 0.34f), Vec3(xFront + 1.38f, frontSurf.midTopY + 0.04f, hw - 0.34f), darkTrim);

    // 3. Recessed 3-Bay Underfloor Pass-Through Luggage Hold Interior (visible cleanly when boots lift open as in Image 3!)
    const float bayStartX = frontAxleX + archHalfLen + 0.10f;
    const float bayEndX   = driveAxleX - archHalfLen - 0.10f;
    const int   bayCount  = 3;
    const float baySpan   = bayEndX - bayStartX;
    {
        float inZ = hw - 0.10f;
        // Dark ribbed luggage compartment floor, interior ceiling liner, and brushed-steel central pass-through spine
        RunningGear::AppendBox(mesh.get(), Vec3(bayStartX, skirtBotY + 0.04f, -inZ), Vec3(bayEndX, skirtBotY + 0.11f, inZ), interiorFloor);
        RunningGear::AppendBox(mesh.get(), Vec3(bayStartX, bootTopY - 0.05f, -inZ), Vec3(bayEndX, deckFloorY - 0.04f, inZ), linerDark);
        RunningGear::AppendBox(mesh.get(), Vec3(bayStartX, skirtBotY + 0.11f, -0.22f), Vec3(bayEndX, bootTopY - 0.05f, 0.22f), steelBaffle);
        // Brushed-steel bay divider bulkheads between Boot 1 | Boot 2 | Boot 3 (exact match to Image 3!)
        for (int b = 0; b <= bayCount; ++b) {
            float dx = bayStartX + (baySpan * static_cast<float>(b)) / static_cast<float>(bayCount);
            RunningGear::AppendBox(mesh.get(), Vec3(dx - 0.03f, skirtBotY + 0.06f, -inZ), Vec3(dx + 0.03f, bootTopY - 0.02f, inZ), steelBaffle);
        }
        // Subtle polished sill strip along outer boot lip
        RunningGear::AppendBox(mesh.get(), Vec3(bayStartX, skirtBotY + 0.05f,  hw - 0.08f), Vec3(bayEndX, skirtBotY + 0.085f,  hw - 0.02f), chromeCol);
        RunningGear::AppendBox(mesh.get(), Vec3(bayStartX, skirtBotY + 0.05f, -hw + 0.02f), Vec3(bayEndX, skirtBotY + 0.085f, -hw + 0.08f), chromeCol);
    }

    // 4. Exterior Front Nose, Chrome Mustache, Almond Headlamps, Dome & Roof A/C (Pass 1)
    AppendLuxuryIntegratedNoseSkin(mesh.get(), frontSurf, bodyPaint, glossBlack, darkTrim);
    AppendLuxuryFasciaDetails(mesh.get(), frontSurf, def.headlights_on, bodyPaint, glossBlack, darkTrim, chromeCol);
    AppendLuxuryRoofAndACPod(mesh.get(), frontSurf, winEndX, xRear, bodyPaint, darkTrim);

    // Sculpted rear D-pillar enclosure
    std::vector<HullStationX> rearTail = {
        {winEndX, skirtBotY + 0.02f, winSideTop + 0.02f, hw,         0.10f, 0.12f, 0.02f, 0.0f},
        {xRear,   skirtBotY + 0.12f, winSideTop + 0.02f, hw - 0.05f, 0.12f, 0.12f, 0.02f, 0.0f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), rearTail, bodyPaint, 4);

    // 5. Lower Skirts (skipping wheel arches, right entry door, AND openable 3-bay luggage boot hold!)
    std::vector<std::pair<float, float>> leftSkipSkirt = {
        {frontAxleX - archHalfLen, frontAxleX + archHalfLen},
        {bayStartX, bayEndX},
        {driveAxleX - archHalfLen, driveAxleX + archHalfLen},
    };
    std::vector<std::pair<float, float>> rightSkipSkirt = {
        {entryFrontX, entryBackX},
        {frontAxleX - archHalfLen, frontAxleX + archHalfLen},
        {bayStartX, bayEndX},
        {driveAxleX - archHalfLen, driveAxleX + archHalfLen},
    };
    CabComponents::AppendSkirtWithCutouts(mesh.get(), frontSurf.xApillar, winEndX, skirtBotY, archTopY,  hw, true,  0.07f, rightSkipSkirt, bodyPaint);
    CabComponents::AppendSkirtWithCutouts(mesh.get(), frontSurf.xApillar, winEndX, skirtBotY, archTopY, -hw, false, 0.07f, leftSkipSkirt, bodyPaint);

    // 6. Side Bodywork: Black Cab Mask, Swept Z-Boomerang Wing, Upper Eyebrow Decal, Beltline, Louvers & D-Pillar (Pass 1)
    const float wallT = 0.07f;
    const float xCabMaskEnd = frontAxleX + 0.86f; // sweeps diagonally back above front wheel
    for (int side : {-1, 1}) {
        bool isRight = (side > 0);
        float s = static_cast<float>(side);

        // Front corner cheek ahead of door / driver window wrapping smoothly from lower bumper corner [archTopY]
        float yCornerCheek = archTopY - 0.01f; // ~1.13m (flush with skirtTop)
        if (isRight) {
            AppendDoubleSidedWallBand(mesh.get(), frontSurf.xApillar, entryFrontX, yCornerCheek, frontSurf.midTopY, yCornerCheek, frontSurf.midTopY, hw, wallT, true, glossBlack, darkTrim);
        } else {
            // Left (-Z) driver lower pearl-white cheek rising gently toward front wheel arch + diagonal amber flash (Image 1)
            float xDrvEnd = entryBackX + 0.14f;
            const float yDrvBotF = 1.44f, yDrvBotB = 1.50f, yDrvTop = 2.26f;
            AppendDoubleSidedWallBand(mesh.get(), frontSurf.xApillar, xDrvEnd, yCornerCheek, yDrvBotF, yCornerCheek, yDrvBotB, hw, wallT, false, bodyPaint, darkTrim);
            AppendSideStripQuad(mesh.get(), frontSurf.xApillar + 0.02f, frontSurf.xApillar + 0.56f, skirtBotY + 0.66f, skirtBotY + 0.78f, skirtBotY + 0.80f, skirtBotY + 0.94f, hw + 0.005f, false, orangeAccent);
            // Black surround framing the driver two-pane slider window
            AppendDoubleSidedWallBand(mesh.get(), frontSurf.xApillar, frontSurf.xApillar + 0.08f, yCornerCheek, frontSurf.midTopY, yCornerCheek, frontSurf.midTopY, hw, wallT, false, glossBlack, darkTrim);
            AppendDoubleSidedWallBand(mesh.get(), frontSurf.xApillar + 0.08f, xDrvEnd - 0.04f, yDrvTop, frontSurf.midTopY, yDrvTop, frontSurf.midTopY, hw, wallT, false, glossBlack, darkTrim);
            // Center vertical divider bar of the two-pane driver slider window (Images 2 & 3)
            float midDrvX = 0.5f * (frontSurf.xApillar + 0.08f + xDrvEnd - 0.04f);
            AppendSideStripQuad(mesh.get(), midDrvX - 0.025f, midDrvX + 0.025f, yDrvBotF, yDrvTop, yDrvBotF, yDrvTop, hw - 0.005f, false, glossBlack);
        }

        // Solid pearl-white body skirt/belt wall behind front door/driver mask up to rear D-pillar:
        // Above luggage boots [bootTopY .. winMainBot] across bay span, full [archTopY .. winMainBot] behind & ahead
        float xAfterCab = isRight ? entryBackX : (entryBackX + 0.10f);
        AppendDoubleSidedWallBand(mesh.get(), xAfterCab, bayStartX, archTopY - 0.01f, winMainBot, archTopY - 0.01f, winMainBot, hw, wallT, isRight, bodyPaint, darkTrim);
        AppendDoubleSidedWallBand(mesh.get(), bayStartX, bayEndX, bootTopY, winMainBot, bootTopY, winMainBot, hw, wallT, isRight, bodyPaint, darkTrim);
        AppendDoubleSidedWallBand(mesh.get(), bayEndX, winEndX, archTopY - 0.01f, winMainBot, archTopY - 0.01f, winMainBot, hw, wallT, isRight, bodyPaint, darkTrim);

        // Clean horizontal beltline crease separating upper white waist & lower luggage boots (Images 2 & 3)
        AppendSideStripQuad(mesh.get(), frontAxleX + archHalfLen, winEndX, bootTopY - 0.012f, bootTopY + 0.010f, bootTopY - 0.012f, bootTopY + 0.010f, hw + 0.004f, isRight, darkTrim);

        // Tapered gloss-black side cab mask sweeping back above front wheel arch (`xAfterCab .. xCabMaskEnd`)
        AppendSideStripQuad(mesh.get(), xAfterCab - 0.02f, xCabMaskEnd, winMainBot - 0.94f, frontSurf.midTopY, winMainBot - 0.16f, frontSurf.midTopY, hw + 0.004f, isRight, glossBlack);

        // Signature Silver-Chrome Z-Boomerang / Chevron Wing framing the black cab mask (`Images 1, 2, 3`)
        AppendSideStripQuad(mesh.get(), xAfterCab - 0.06f, frontAxleX + 0.36f, frontSurf.midTopY - 0.02f, frontSurf.midTopY + 0.12f, frontSurf.midTopY - 0.02f, frontSurf.midTopY + 0.12f, hw + 0.009f, isRight, chromeCol);
        AppendSideStripQuad(mesh.get(), frontAxleX + 0.34f, xCabMaskEnd + 0.16f, frontSurf.midTopY - 0.02f, frontSurf.midTopY + 0.12f, winMainBot - 0.14f, winMainBot + 0.03f, hw + 0.009f, isRight, chromeCol);
        AppendSideStripQuad(mesh.get(), xCabMaskEnd - 0.12f, xCabMaskEnd + 0.16f, winMainBot - 0.24f, winMainBot - 0.10f, winMainBot - 0.14f, winMainBot + 0.03f, hw + 0.009f, isRight, chromeCol);

        // Continuous gloss-black upper visor side triangle [xApillar .. eyebrow] + Pearl White curved eyebrow wing meeting passenger windows (`Images 1 & 2`)
        const int browSegs = 10;
        float pilY0 = frontSurf.midTopY - 0.02f;
        float pilY1 = winSideTop;
        for (int i = 0; i < browSegs; ++i) {
            float ya = pilY0 + (pilY1 - pilY0) * (static_cast<float>(i) / static_cast<float>(browSegs));
            float yb = pilY0 + (pilY1 - pilY0) * (static_cast<float>(i + 1) / static_cast<float>(browSegs));
            float tA = (ya - pilY0) / (pilY1 - pilY0);
            float tB = (yb - pilY0) / (pilY1 - pilY0);
            float xF_A = (xFront + 0.72f) + 0.30f * std::pow(tA, 1.25f);
            float xF_B = (xFront + 0.72f) + 0.30f * std::pow(tB, 1.25f);
            float xCurveA = (frontAxleX - 0.28f) - 0.62f * std::pow(tA, 0.85f);
            float xCurveB = (frontAxleX - 0.28f) - 0.62f * std::pow(tB, 0.85f);
            float xWingBackA = xCurveA + (0.28f - 0.06f * tA);
            float xWingBackB = xCurveB + (0.28f - 0.06f * tB);

            if (isRight) {
                CabComponents::AppendQuad(mesh.get(), Vec3(xF_A, ya, hw + 0.002f), Vec3(xCurveA, ya, hw + 0.002f), Vec3(xCurveB, yb, hw + 0.002f), Vec3(xF_B, yb, hw + 0.002f), glossBlack);
                CabComponents::AppendQuad(mesh.get(), Vec3(xCurveA, ya, hw - wallT), Vec3(xF_A, ya, hw - wallT), Vec3(xF_B, yb, hw - wallT), Vec3(xCurveB, yb, hw - wallT), darkTrim);
                CabComponents::AppendQuad(mesh.get(), Vec3(xCurveA - 0.03f, ya, hw + 0.007f), Vec3(xWingBackA, ya, hw + 0.007f), Vec3(xWingBackB, yb, hw + 0.007f), Vec3(xCurveB - 0.03f, yb, hw + 0.007f), bodyPaint);
            } else {
                CabComponents::AppendQuad(mesh.get(), Vec3(xCurveA, ya, -hw - 0.002f), Vec3(xF_A, ya, -hw - 0.002f), Vec3(xF_B, yb, -hw - 0.002f), Vec3(xCurveB, yb, -hw - 0.002f), glossBlack);
                CabComponents::AppendQuad(mesh.get(), Vec3(xF_A, ya, -hw + wallT), Vec3(xCurveA, ya, -hw + wallT), Vec3(xCurveB, yb, -hw + wallT), Vec3(xF_B, yb, -hw + wallT), darkTrim);
                CabComponents::AppendQuad(mesh.get(), Vec3(xWingBackA, ya, -hw - 0.007f), Vec3(xCurveA - 0.03f, ya, -hw - 0.007f), Vec3(xCurveB - 0.03f, yb, -hw - 0.007f), Vec3(xWingBackB, yb, -hw - 0.007f), bodyPaint);
            }
        }
        // Signature twin warm-orange wing speed stripes on upper eyebrow shoulder (`Image 1`)
        AppendSideStripQuad(mesh.get(), frontSurf.xRoofJoin - 0.04f, frontAxleX - 0.56f, winSideTop - 0.24f, winSideTop - 0.19f, winSideTop - 0.30f, winSideTop - 0.25f, hw + 0.011f, isRight, orangeAccent);
        AppendSideStripQuad(mesh.get(), frontSurf.xRoofJoin + 0.04f, frontAxleX - 0.60f, winSideTop - 0.32f, winSideTop - 0.28f, winSideTop - 0.37f, winSideTop - 0.33f, hw + 0.011f, isRight, orangeAccent);

        // Passenger side-window gloss-black lower beltline border & curved rear upward D-pillar kickup (`Images 1 & 2`)
        float winStart = frontAxleX - 0.04f;
        AppendSideStripQuad(mesh.get(), winStart, winEndX - 0.45f, winMainBot - 0.02f, winMainBot + 0.02f, winMainBot - 0.02f, winMainBot + 0.02f, hw + 0.003f, isRight, glossBlack);
        AppendSideStripQuad(mesh.get(), winEndX - 0.45f, winEndX + 0.02f, winMainBot - 0.02f, winMainBot + 0.02f, winMainBot + 0.18f, winMainBot + 0.24f, hw + 0.003f, isRight, glossBlack);
        AppendDoubleSidedWallBand(mesh.get(), winEndX - 0.45f, winEndX + 0.02f, winMainBot, winMainBot, winMainBot, winMainBot + 0.20f, hw, wallT, isRight, bodyPaint, darkTrim);

        // Window vertical black mullions (7 main panes)
        const int numMulls = 7;
        for (int m = 1; m <= numMulls; ++m) {
            float mx = winStart + (winEndX - 0.25f - winStart) * (static_cast<float>(m) / static_cast<float>(numMulls + 1));
            AppendSideStripQuad(mesh.get(), mx - 0.020f, mx + 0.020f, winMainBot, winSideTop, winMainBot, winSideTop, hw - 0.008f, isRight, glossBlack);
        }

        // Painted roof cantrail + rear engine intake louvers (`Images 1 & 2`)
        AppendSideStripQuad(mesh.get(), frontSurf.xRoofJoin - 0.02f, winEndX + 0.04f, winSideTop - 0.02f, winSideTop + 0.03f, winSideTop - 0.02f, winSideTop + 0.03f, hw + 0.004f, isRight, bodyPaint);
        float luvX0 = driveAxleX + archHalfLen + 0.85f;
        float luvX1 = xRear - 0.28f;
        if (luvX1 > luvX0 + 0.20f) {
            RunningGear::AppendBox(mesh.get(), Vec3(luvX0, archTopY + 0.06f, s > 0 ? hw : -hw - 0.010f), Vec3(luvX1, bootTopY - 0.04f, s > 0 ? hw + 0.010f : -hw), darkTrim);
            for (int lv = 0; lv < 7; ++lv) {
                float ly = archTopY + 0.09f + 0.040f * static_cast<float>(lv);
                if (ly < bootTopY - 0.06f) {
                    RunningGear::AppendBox(mesh.get(), Vec3(luvX0 + 0.02f, ly, s > 0 ? hw + 0.004f : -hw - 0.015f), Vec3(luvX1 - 0.02f, ly + 0.018f, s > 0 ? hw + 0.015f : -hw - 0.004f), bodyPaint);
                }
            }
        }
        // Subtle service door seams & amber side markers (`Images 1 & 2`)
        for (float sx : {driveAxleX + archHalfLen + 0.35f, xRear - 0.80f}) {
            RunningGear::AppendBox(mesh.get(), Vec3(sx - 0.008f, skirtBotY + 0.06f, s > 0 ? hw : -hw - 0.008f), Vec3(sx + 0.008f, bootTopY - 0.02f, s > 0 ? hw + 0.008f : -hw), darkTrim);
        }
        for (float mx : {frontAxleX - 0.56f, xRear - 0.38f}) {
            RunningGear::AppendBox(mesh.get(), Vec3(mx - 0.045f, archTopY - 0.20f, s > 0 ? hw : -hw - 0.012f), Vec3(mx + 0.045f, archTopY - 0.15f, s > 0 ? hw + 0.012f : -hw), amberMarker);
        }
    }

    // 7. 3 Full-Height Pantograph Luggage Boots (Closed = Image 2; Open = Image 3!!)
    float bootLift = std::clamp(def.action_ratio, 0.0f, 1.0f);
    float dyLift   = bootLift * 0.90f; // lifts cleanly beneath raised beltline just like Image 3!
    float dzOut    = 0.014f + 0.072f * std::sin(std::min(1.0f, bootLift * 1.45f) * kPi * 0.5f);
    for (int b = 0; b < bayCount; ++b) {
        float bx0 = bayStartX + (baySpan * static_cast<float>(b)) / static_cast<float>(bayCount) + 0.025f;
        float bx1 = bayStartX + (baySpan * static_cast<float>(b + 1)) / static_cast<float>(bayCount) - 0.025f;
        float midB = 0.5f * (bx0 + bx1);
        float yBot = skirtBotY + 0.06f + dyLift;
        float yTop = bootTopY - 0.015f + dyLift;

        for (int side : {-1, 1}) {
            float s = static_cast<float>(side);
            float zInner = s * (hw - 0.02f + dzOut - 0.014f);
            float zOuter = s * (hw + dzOut);
            float zMin = std::min(zInner, zOuter), zMax = std::max(zInner, zOuter);

            // White boot door panel + subtle dark frame rim
            RunningGear::AppendBox(mesh.get(), Vec3(bx0, yBot, zMin), Vec3(bx1, yTop, zMax), bodyPaint);
            RunningGear::AppendBox(mesh.get(), Vec3(bx0 - 0.010f, yBot, zMin), Vec3(bx0 + 0.008f, yTop, zMax), darkTrim);

            // Twin recessed oval black push-latches (`upper-center` & `lower-center`, matching Images 2 & 3!)
            float zLat0 = s > 0 ? zMax - 0.002f : zMin - 0.010f;
            float zLat1 = s > 0 ? zMax + 0.010f : zMin + 0.002f;
            for (float ly : {yBot + 0.14f, yTop - 0.24f}) {
                RunningGear::AppendBox(mesh.get(), Vec3(midB - 0.065f, ly - 0.028f, zLat0), Vec3(midB + 0.065f, ly + 0.028f, zLat1), darkTrim);
                RunningGear::AppendBox(mesh.get(), Vec3(midB - 0.025f, ly - 0.012f, s > 0 ? zLat1 : zLat0 - 0.004f), Vec3(midB + 0.025f, ly + 0.012f, s > 0 ? zLat1 + 0.004f : zLat0), chromeCol);
            }

            // Visible pantograph hinge arms inside top corners when boots lift open (`Image 3`)
            if (bootLift > 0.02f) {
                float armZ0 = s > 0 ? hw - 0.08f : -hw - dzOut + 0.02f;
                float armZ1 = s > 0 ? hw + dzOut - 0.02f : -hw + 0.08f;
                RunningGear::AppendBox(mesh.get(), Vec3(bx0 + 0.14f, bootTopY - 0.06f, armZ0), Vec3(bx0 + 0.19f, yTop - 0.06f, armZ1), darkTrim);
                RunningGear::AppendBox(mesh.get(), Vec3(bx1 - 0.19f, bootTopY - 0.06f, armZ0), Vec3(bx1 - 0.14f, yTop - 0.06f, armZ1), darkTrim);
            }
        }
    }

    // 8. Openable Front Entry Door (Pass 1 Frame), White Swan-Neck Aero Mirrors & Rear Fascia
    CabComponents::AppendAnimatedDoor(
        mesh.get(),
        entryFrontX, entryBackX,
        skirtBotY + 0.10f, frontSurf.midBotY - 0.02f,
        hw, true,
        def.door_open_ratio, false, false,
        bodyPaint, glossBlack, sideGlass
    );
    AppendLuxurySwanNeckMirrors(mesh.get(), frontSurf, bodyPaint, darkTrim);
    AppendLuxuryRearDesign(mesh.get(), xRear, skirtBotY, winMainBot, winSideTop, H, hw, def.headlights_on, bodyPaint, glossBlack, darkTrim, chromeCol);

    // 9. Transparent Automotive Glazing (PASS 2 DRAWN LAST)
    // Lower panoramic windshield flush inside black sill & A-pillars
    {
        const int nuG = 20, nvG = 5;
        for (int i = 0; i < nuG; ++i) {
            float ua = -0.89f + 1.78f * (static_cast<float>(i) / static_cast<float>(nuG));
            float ub = -0.89f + 1.78f * (static_cast<float>(i + 1) / static_cast<float>(nuG));
            float ybA = frontSurf.EvalWinBottomY(ua), ybB = frontSurf.EvalWinBottomY(ub);
            for (int j = 0; j < nvG; ++j) {
                float f0 = static_cast<float>(j) / static_cast<float>(nvG);
                float f1 = static_cast<float>(j + 1) / static_cast<float>(nvG);
                Vec3 a = OffsetPt(frontSurf.EvalFrontPos(ua, ybA + (frontSurf.midBotY - ybA) * f0), frontSurf.EvalFrontNormal(ua, ybA + (frontSurf.midBotY - ybA) * f0), -0.0005f);
                Vec3 b = OffsetPt(frontSurf.EvalFrontPos(ub, ybB + (frontSurf.midBotY - ybB) * f0), frontSurf.EvalFrontNormal(ub, ybB + (frontSurf.midBotY - ybB) * f0), -0.0005f);
                Vec3 c = OffsetPt(frontSurf.EvalFrontPos(ub, ybB + (frontSurf.midBotY - ybB) * f1), frontSurf.EvalFrontNormal(ub, ybB + (frontSurf.midBotY - ybB) * f1), -0.0005f);
                Vec3 d = OffsetPt(frontSurf.EvalFrontPos(ua, ybA + (frontSurf.midBotY - ybA) * f1), frontSurf.EvalFrontNormal(ua, ybA + (frontSurf.midBotY - ybA) * f1), -0.0005f);
                CabComponents::AppendQuad(mesh.get(), a, b, c, d, windshieldGlass);
            }
        }
    }
    // Swept upper panoramic glass visor
    AppendFrontPatch(mesh.get(), frontSurf, -0.89f, 0.89f, frontSurf.midTopY, frontSurf.upperWinTopY, 20, 6, -0.0005f, upperVisorGlass);

    // Side windows: left driver two-pane slider + both left/right long panoramic windows
    const float zGlass = hw - 0.025f;
    AppendSideStripQuad(mesh.get(), frontSurf.xApillar + 0.08f, entryBackX + 0.10f, 1.44f, 2.26f, 1.50f, 2.26f, zGlass, false, sideGlass);
    for (int side : {-1, 1}) {
        bool isRight = (side > 0);
        // Side panoramic glass starts right behind curved front eyebrow wing
        const int browGlassSegs = 6;
        float pilY0 = frontSurf.midTopY - 0.02f;
        float pilY1 = winSideTop - 0.01f;
        float xStraightWin = frontAxleX - 0.04f;
        for (int i = 0; i < browGlassSegs; ++i) {
            float ya = pilY0 + (pilY1 - pilY0) * (static_cast<float>(i) / static_cast<float>(browGlassSegs));
            float yb = pilY0 + (pilY1 - pilY0) * (static_cast<float>(i + 1) / static_cast<float>(browGlassSegs));
            float tA = (ya - pilY0) / (pilY1 - pilY0);
            float tB = (yb - pilY0) / (pilY1 - pilY0);
            float xA = (frontAxleX - 0.28f) - 0.62f * std::pow(tA, 0.85f) + (0.28f - 0.06f * tA);
            float xB = (frontAxleX - 0.28f) - 0.62f * std::pow(tB, 0.85f) + (0.28f - 0.06f * tB);
            AppendSideStripQuad(mesh.get(), std::min(xA, xStraightWin), xStraightWin, ya, yb, ya, yb, zGlass, isRight, sideGlass);
        }
        AppendSideStripQuad(mesh.get(), xStraightWin, winEndX - 0.45f, winMainBot, winSideTop - 0.01f, winMainBot, winSideTop - 0.01f, zGlass, isRight, sideGlass);
        AppendSideStripQuad(mesh.get(), winEndX - 0.45f, winEndX + 0.02f, winMainBot, winSideTop - 0.01f, winMainBot + 0.20f, winSideTop - 0.01f, zGlass, isRight, sideGlass);
    }

    // Entry door transparent glass (Pass 2)
    CabComponents::AppendAnimatedDoor(
        mesh.get(),
        entryFrontX, entryBackX,
        skirtBotY + 0.10f, frontSurf.midBotY - 0.02f,
        hw, true,
        def.door_open_ratio, false, true,
        bodyPaint, glossBlack, sideGlass
    );

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::coach
