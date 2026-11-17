#include "VolvoDoubleDeckerGeometry.h"
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

struct DDRibbonSample {
    float u;
    float vBot;
    float vTop;
};

// Unified C1 compound-curved dual-deck Volvo front + roof dome + cantrail surface
struct DDSurface {
    bool is9800DD = true;
    float xFront = 0.0f;
    float xApillar = 0.0f;
    float xRoofJoin = 0.0f;
    float skirtBotY = 0.28f;
    float noseCenterTopY = 1.15f;
    float ddMidBotY = 2.26f;
    float ddMidTopY = 2.52f;
    float upperWinTopY = 3.74f;
    float winSideTop = 3.78f;
    float H = 4.00f;
    float hw = 1.275f;

    // C1 two-arc planform curve (generous wrap-around front corners)
    float uFlat = 0.64f;
    float rCorner = 0.34f;
    float alpha = 0.44f;
    float sinA = 0.0f;
    float cosA = 1.0f;
    float zFlatEdge = 0.0f;
    float Rfront = 1.0f;
    float dxFrontBulge = 0.0f;

    // Smooth upper roof arch
    float rTopDome = 0.22f;
    float roofTopY = 3.96f;
    float roofCrown = 0.055f;

    void Init(
        bool inIs9800DD,
        float inXFront,
        float inSkirtBotY,
        float inWinSideTop,
        float inH,
        float inHw
    ) {
        is9800DD = inIs9800DD;
        xFront = inXFront;
        xApillar = inXFront + 0.66f;
        xRoofJoin = inXFront + 1.32f;
        skirtBotY = inSkirtBotY;
        noseCenterTopY = inSkirtBotY + 0.94f; // ~1.22m center lower windshield bottom
        ddMidBotY = inSkirtBotY + 1.94f;      // ~2.22m lower windshield top
        ddMidTopY = ddMidBotY + 0.34f;        // ~2.56m upper windshield bottom
        winSideTop = inWinSideTop;            // H - 0.30f = 3.70m
        upperWinTopY = inH - 0.32f;           // ~3.68m upper glass brow center
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

    // Subtle curved smile along the lower windshield sill (lower in center, rising gracefully toward A-pillars)
    float EvalWinBottomY(float u) const {
        float absU = std::abs(std::clamp(u, -1.0f, 1.0f));
        float s = std::min(1.0f, absU / 0.90f);
        return noseCenterTopY + 0.11f * std::pow(s, 2.2f);
    }

    // Bowed Volvo V-collar lower edge (0 at center dips low, outer edge rises smoothly toward outer lamps)
    float EvalCollarBotY(float u) const {
        float absU = std::abs(std::clamp(u, -1.0f, 1.0f));
        float t = std::clamp(absU / 0.82f, 0.0f, 1.0f);
        float yC = skirtBotY + (is9800DD ? 0.50f : 0.46f);
        float yEdge = EvalWinBottomY(0.82f) - 0.10f;
        return yC + (yEdge - yC) * std::pow(t, 1.38f);
    }

    void EvalHeightAnchors(float y, float& xNoseCenter, float& xSideAnchor) const {
        const float chinTop = skirtBotY + 0.14f;
        if (y <= chinTop) {
            float t = std::clamp((y - skirtBotY) / 0.14f, 0.0f, 1.0f);
            float a = t * (kPi * 0.5f);
            xNoseCenter = (xFront + 0.03f) + 0.075f * (1.0f - std::sin(a));
            xSideAnchor = xApillar + 0.030f * std::cos(a);
        } else if (y <= noseCenterTopY + 0.12f) {
            float t = std::clamp((y - chinTop) / std::max(0.10f, (noseCenterTopY + 0.12f) - chinTop), 0.0f, 1.0f);
            xNoseCenter = (xFront + 0.03f) + 0.055f * t - 0.018f * std::sin(kPi * t);
            xSideAnchor = xApillar;
        } else if (y <= ddMidTopY) {
            // Proud near-vertical lower-deck windshield
            float tLow = std::clamp((y - (noseCenterTopY + 0.12f)) / std::max(0.20f, ddMidTopY - (noseCenterTopY + 0.12f)), 0.0f, 1.0f);
            xNoseCenter = (xFront + 0.085f) + 0.105f * tLow;
            xSideAnchor = xApillar + 0.050f * tLow;
        } else {
            // Swept-back upper-deck panoramic windshield curving back into aerodynamic roof dome
            float tHigh = std::clamp((y - ddMidTopY) / std::max(0.20f, upperWinTopY - ddMidTopY), 0.0f, 1.0f);
            float sweepC = std::pow(tHigh, 1.34f);
            float sweepS = std::pow(tHigh, 1.22f);
            xNoseCenter = (xFront + 0.190f) + 0.340f * sweepC;
            xSideAnchor = (xApillar + 0.050f) + 0.240f * sweepS;
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

        // Subtle arched brow & gentle corner blend into winSideTop at upper windshield perimeter
        float wTop = std::clamp((y - ddMidTopY) / std::max(0.20f, upperWinTopY - ddMidTopY), 0.0f, 1.0f);
        float dropT = std::pow(std::max(0.0f, (absU - uFlat) / (1.0f - uFlat)), 1.35f);
        float yDropCorner = (upperWinTopY - winSideTop) * dropT;
        float sCenter = std::min(1.0f, absU / uFlat);
        float yArchLift = 0.032f * (1.0f - sCenter * sCenter);
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
        float fx = 0.22f * vt + 0.78f * (1.0f - std::cos(phi));
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
    const DDSurface& surf,
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
    const DDSurface& surf,
    std::vector<DDRibbonSample> samples,
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
    const DDSurface& surf,
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
    const DDSurface& surf,
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
    const DDSurface& surf,
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
    const DDSurface& surf,
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

// Emits exact non-overlapping nose skin where V-collar, lower intake, fog pockets, and headlamp bays
// are directly embedded into the curved C1 nose topology — zero coplanar overlap or Z-fighting.
void AppendDDIntegratedNoseSkin(
    Mesh* mesh,
    const DDSurface& surf,
    const Vec4& bodyPaint,
    const Vec4& glossBlack
) {
    const int nu = 44;
    const float yChin = surf.skirtBotY + 0.03f;
    const float yFogTop = surf.skirtBotY + 0.145f;
    const float yIntakeBot = surf.skirtBotY + 0.155f;
    const float yIntakeTop = surf.skirtBotY + 0.340f;

    auto headBotY = [&](float absU) -> float {
        float t = std::clamp((absU - 0.59f) / 0.26f, 0.0f, 1.0f);
        return (surf.skirtBotY + 0.18f) + 0.02f * t;
    };
    auto headTopY = [&](float absU) -> float {
        float t = std::clamp((absU - 0.59f) / 0.26f, 0.0f, 1.0f);
        return (surf.skirtBotY + 0.48f) + 0.10f * t;
    };

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

        float wTopA = surf.EvalWinBottomY(ua);
        float wTopB = surf.EvalWinBottomY(ub);
        float colBotA = surf.EvalCollarBotY(ua);
        float colBotB = surf.EvalCollarBotY(ub);

        if (absU <= 0.51f) {
            // Center zone: lower Ice Cyan bumper lip -> flush gloss-black air intake mouth -> Ice Cyan cross-bar -> gloss-black Volvo V-collar
            emitColBand(surf.skirtBotY, surf.skirtBotY, yIntakeBot, yIntakeBot, 2, bodyPaint);
            emitColBand(yIntakeBot, yIntakeBot, yIntakeTop, yIntakeTop, 3, glossBlack);
            emitColBand(yIntakeTop, yIntakeTop, colBotA, colBotB, 3, bodyPaint);
            emitColBand(colBotA, colBotB, wTopA, wTopB, 4, glossBlack);
        } else if (absU <= 0.59f) {
            // Narrow painted cheek bridge between lower intake mouth & main headlamps
            emitColBand(surf.skirtBotY, surf.skirtBotY, colBotA, colBotB, 6, bodyPaint);
            emitColBand(colBotA, colBotB, wTopA, wTopB, 4, glossBlack);
        } else if (absU <= 0.84f) {
            // Headlamp & corner fog pocket zone
            float hBA = headBotY(std::abs(ua)), hBB = headBotY(std::abs(ub));
            float hTA = headTopY(std::abs(ua)), hTB = headTopY(std::abs(ub));
            emitColBand(surf.skirtBotY, surf.skirtBotY, yChin, yChin, 1, bodyPaint);
            emitColBand(yChin, yChin, yFogTop, yFogTop, 2, (absU >= 0.62f && absU <= 0.82f) ? glossBlack : bodyPaint);
            emitColBand(yFogTop, yFogTop, hBA, hBB, 1, bodyPaint);
            emitColBand(hBA, hBB, hTA, hTB, 3, glossBlack);
            if (colBotA > hTA + 0.01f && colBotB > hTB + 0.01f) {
                emitColBand(hTA, hTB, colBotA, colBotB, 2, bodyPaint);
            } else {
                emitColBand(hTA, hTB, std::max(hTA, colBotA), std::max(hTB, colBotB), 1, bodyPaint);
            }
            if (absU <= 0.80f) {
                emitColBand(std::max(hTA, colBotA), std::max(hTB, colBotB), wTopA, wTopB, 3, glossBlack);
            } else {
                emitColBand(std::max(hTA, colBotA), std::max(hTB, colBotB), wTopA, wTopB, 3, bodyPaint);
            }
        } else {
            // Smooth rounded painted outer nose cheek wrapping around corner to lower A-pillar
            emitColBand(surf.skirtBotY, surf.skirtBotY, wTopA, wTopB, 8, bodyPaint);
        }

        // Bottom chin floor closure
        Vec3 botA = surf.EvalFrontPos(ua, surf.skirtBotY);
        Vec3 botB = surf.EvalFrontPos(ub, surf.skirtBotY);
        CabComponents::AppendQuad(mesh, Vec3(surf.xApillar, surf.skirtBotY, botA.z), Vec3(surf.xApillar, surf.skirtBotY, botB.z), botB, botA, bodyPaint);
    }
}

void AppendDDFasciaDetails(
    Mesh* mesh,
    const DDSurface& surf,
    bool headlightsOn,
    const Vec4& bodyPaint,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol
) {
    const Vec4 drlWhite = headlightsOn ? Vec4(0.96f, 0.98f, 1.00f, 1.0f) : Vec4(0.76f, 0.80f, 0.86f, 1.0f);
    const Vec4 projLens = headlightsOn ? Vec4(0.95f, 0.97f, 1.00f, 1.0f) : Vec4(0.56f, 0.62f, 0.70f, 1.0f);
    const Vec4 amberCol = headlightsOn ? Vec4(1.00f, 0.58f, 0.08f, 1.0f) : Vec4(0.60f, 0.34f, 0.08f, 1.0f);
    const Vec4 fogCol   = headlightsOn ? Vec4(0.94f, 0.96f, 1.00f, 1.0f) : Vec4(0.60f, 0.66f, 0.74f, 1.0f);

    // 1. Continuous slim chrome top brow bar framing the Volvo Iron Mark collar
    {
        std::vector<DDRibbonSample> browBar;
        for (int k = -10; k <= 10; ++k) {
            float u = 0.78f * (static_cast<float>(k) / 10.0f);
            float yt = surf.EvalWinBottomY(u) - 0.048f;
            browBar.push_back({u, yt - 0.020f, yt});
        }
        AppendFrontRibbon(mesh, surf, browBar, 0.015f, 0.008f, chromeCol);
    }

    // 2. Volvo Iron Mark circular chrome ring + central dark disc + continuous diagonal chrome slash
    {
        float ySlash0 = surf.EvalCollarBotY(-0.56f) + 0.04f;
        float ySlashC = 0.5f * (surf.EvalCollarBotY(0.0f) + surf.EvalWinBottomY(0.0f) - 0.06f);
        float ySlash1 = surf.EvalWinBottomY(0.56f) - 0.085f;
        AppendFrontRibbon(
            mesh, surf,
            {{-0.56f, ySlash0 - 0.012f, ySlash0 + 0.012f},
             {-0.16f, ySlashC - 0.042f, ySlashC - 0.018f},
             { 0.16f, ySlashC + 0.014f, ySlashC + 0.038f},
             { 0.56f, ySlash1 - 0.012f, ySlash1 + 0.012f}},
            0.018f, 0.008f, chromeCol
        );
        AppendSurfaceRing(mesh, surf, 0.0f, ySlashC, 0.046f, 0.046f, 0.064f, 0.064f, 0.022f, 0.008f, 18, chromeCol);
        AppendSurfaceDisc(mesh, surf, 0.0f, ySlashC, 0.046f, 0.046f, 0.020f, 0.008f, 16, glossBlack);
        AppendFrontRibbon(mesh, surf, {{-0.046f, ySlashC - 0.012f, ySlashC + 0.012f}, {0.046f, ySlashC - 0.012f, ySlashC + 0.012f}}, 0.025f, 0.005f, chromeCol);
    }

    // 3. Lower intake center grille crossbar & vertical divider fins
    {
        float yMidIntake = surf.skirtBotY + 0.245f;
        AppendFrontRibbon(mesh, surf, {{-0.48f, yMidIntake - 0.012f, yMidIntake + 0.012f}, {0.48f, yMidIntake - 0.012f, yMidIntake + 0.012f}}, 0.006f, 0.010f, darkTrim);
        AppendFrontRibbon(mesh, surf, {{-0.15f, surf.skirtBotY + 0.155f, surf.skirtBotY + 0.170f}, {0.15f, surf.skirtBotY + 0.155f, surf.skirtBotY + 0.170f}}, 0.010f, 0.006f, darkTrim);
    }

    // 4. Mid-deck front solid divider band [ddMidBotY .. ddMidTopY] & lower windshield sill seal
    AppendFrontPatch(mesh, surf, -0.90f, 0.90f, surf.ddMidBotY, surf.ddMidTopY, 20, 3, 0.002f, glossBlack);
    {
        // Continuous gloss-black sill seal bridging top of lower nose flush to lower windshield glass
        std::vector<DDRibbonSample> sillSeal;
        for (int k = -12; k <= 12; ++k) {
            float u = 0.90f * (static_cast<float>(k) / 12.0f);
            float ys = surf.EvalWinBottomY(u);
            sillSeal.push_back({u, ys - 0.015f, ys + 0.024f});
        }
        AppendFrontRibbon(mesh, surf, sillSeal, 0.004f, 0.014f, glossBlack);
    }
    if (!surf.is9800DD) {
        AppendFrontRibbon(mesh, surf, {{-0.18f, surf.ddMidBotY + 0.08f, surf.ddMidBotY + 0.12f}, {0.18f, surf.ddMidBotY + 0.08f, surf.ddMidBotY + 0.12f}}, 0.012f, 0.006f, darkTrim);
    }

    // 5. Continuous Ice Cyan Outer A-Pillars (wrapping unbroken from lower bumper all the way to upper roof corner)
    for (int side : {-1, 1}) {
        float uOutA = (side < 0) ? -1.00f : 0.905f;
        float uOutB = (side < 0) ? -0.905f : 1.00f;
        float uTrimA = (side < 0) ? -0.905f : 0.885f;
        float uTrimB = (side < 0) ? -0.885f : 0.905f;
        float yLowStart = surf.EvalWinBottomY(0.92f) - 0.02f;
        // Seamless full-height Ice Cyan outer A-pillar frame
        AppendFrontPatch(mesh, surf, uOutA, uOutB, yLowStart, surf.upperWinTopY, 4, 14, 0.002f, bodyPaint);
        // Inner flush gloss-black window perimeter seal strip
        AppendFrontPatch(mesh, surf, uTrimA, uTrimB, surf.EvalWinBottomY(0.89f), surf.ddMidBotY, 2, 6, 0.002f, glossBlack);
        AppendFrontPatch(mesh, surf, uTrimA, uTrimB, surf.ddMidTopY, surf.upperWinTopY, 2, 6, 0.002f, glossBlack);
    }

    // 6. Seamless Curved Roof Dome:
    //    Center brow [-0.89 .. +0.89] = glossBlack front transitioning smoothly into bodyPaint roof
    //    Outer corner shoulders [-1.00 .. -0.89] & [0.89 .. 1.00] = continuous bodyPaint connecting A-pillar straight to roof!
    if (surf.is9800DD) {
        AppendSmoothDome(mesh, surf, -0.89f, 0.89f, 0.0f, 0.44f, 0.002f, 20, 5, glossBlack);
        AppendSmoothDome(mesh, surf, -0.89f, 0.89f, 0.44f, 1.0f, 0.002f, 20, 6, bodyPaint);
        AppendSmoothDome(mesh, surf, -1.00f, -0.89f, 0.0f, 1.0f, 0.002f, 3, 10, bodyPaint);
        AppendSmoothDome(mesh, surf,  0.89f,  1.00f, 0.0f, 1.0f, 0.002f, 3, 10, bodyPaint);
        // 3 central + 2 outer amber DOT brow pills + side corner amber markers + upper deck sweeping wiper
        for (float cu : {-0.18f, 0.0f, 0.18f}) {
            AppendDomeLight(mesh, surf, cu - 0.055f, cu + 0.055f, 0.10f, 0.18f, 0.012f, amberCol);
        }
        AppendDomeLight(mesh, surf, -0.72f, -0.56f, 0.10f, 0.18f, 0.012f, amberCol);
        AppendDomeLight(mesh, surf,  0.56f,  0.72f, 0.10f, 0.18f, 0.012f, amberCol);
        AppendDomeLight(mesh, surf, -0.96f, -0.89f, 0.16f, 0.25f, 0.014f, amberCol);
        AppendDomeLight(mesh, surf,  0.89f,  0.96f, 0.16f, 0.25f, 0.014f, amberCol);
        AppendFrontRibbon(mesh, surf, {{-0.44f, surf.ddMidTopY + 0.16f, surf.ddMidTopY + 0.19f}, {0.16f, surf.ddMidTopY + 0.04f, surf.ddMidTopY + 0.07f}}, 0.018f, 0.005f, glossBlack);
    } else {
        AppendSmoothDome(mesh, surf, -0.89f, 0.89f, 0.0f, 0.64f, 0.002f, 20, 6, glossBlack);
        AppendSmoothDome(mesh, surf, -0.89f, 0.89f, 0.64f, 1.0f, 0.002f, 20, 4, bodyPaint);
        AppendSmoothDome(mesh, surf, -1.00f, -0.89f, 0.0f, 1.0f, 0.002f, 3, 10, bodyPaint);
        AppendSmoothDome(mesh, surf,  0.89f,  1.00f, 0.0f, 1.0f, 0.002f, 3, 10, bodyPaint);
        AppendDomeLight(mesh, surf, -0.68f, -0.50f, 0.10f, 0.18f, 0.012f, Vec4(0.90f, 0.93f, 0.98f, 1.0f));
        AppendDomeLight(mesh, surf,  0.50f,  0.68f, 0.10f, 0.18f, 0.012f, Vec4(0.90f, 0.93f, 0.98f, 1.0f));
    }

    // Dual opposing lower windshield panoramic wipers
    AppendFrontRibbon(mesh, surf, {{-0.62f, surf.EvalWinBottomY(-0.62f) + 0.02f, surf.EvalWinBottomY(-0.62f) + 0.05f}, {-0.02f, surf.noseCenterTopY + 0.03f, surf.noseCenterTopY + 0.06f}}, 0.018f, 0.006f, glossBlack);
    AppendFrontRibbon(mesh, surf, {{0.02f, surf.noseCenterTopY + 0.03f, surf.noseCenterTopY + 0.06f}, {0.62f, surf.EvalWinBottomY(0.62f) + 0.02f, surf.EvalWinBottomY(0.62f) + 0.05f}}, 0.018f, 0.006f, glossBlack);

    // 7. Signature Volvo J/L-hook LED DRL Headlamps + Dual Projectors + Corner Fog Pockets
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float uIn  = s * 0.605f;
        float uMid = s * 0.720f;
        float uOut = s * 0.825f;
        float yLBotIn  = surf.skirtBotY + 0.195f;
        float yLBotOut = surf.skirtBotY + 0.215f;
        float yLTopOut = surf.skirtBotY + 0.560f;

        // Sharp white J/L LED DRL bottom bar + outer vertical bar
        AppendFrontRibbon(mesh, surf, {{uIn, yLBotIn, yLBotIn + 0.024f}, {uOut, yLBotOut, yLBotOut + 0.024f}}, 0.014f, 0.006f, drlWhite);
        AppendFrontRibbon(mesh, surf, {{s * 0.80f, yLBotOut, yLTopOut}, {uOut, yLBotOut, yLTopOut}}, 0.014f, 0.006f, drlWhite);
        // Subtle outer turn amber indicator sliver
        AppendFrontRibbon(mesh, surf, {{uOut, yLBotOut + 0.14f, yLTopOut - 0.02f}, {s * 0.838f, yLBotOut + 0.14f, yLTopOut - 0.02f}}, 0.015f, 0.005f, amberCol);
        // Twin stacked crystal LED projectors inside dark lamp bezel
        AppendSurfaceRing(mesh, surf, uMid - s * 0.01f, surf.skirtBotY + 0.305f, 0.026f, 0.030f, 0.042f, 0.046f, 0.015f, 0.006f, 12, chromeCol);
        AppendSurfaceDisc(mesh, surf, uMid - s * 0.01f, surf.skirtBotY + 0.305f, 0.025f, 0.029f, 0.018f, 0.006f, 12, projLens);
        AppendSurfaceRing(mesh, surf, uMid - s * 0.01f, surf.skirtBotY + 0.435f, 0.026f, 0.030f, 0.042f, 0.046f, 0.015f, 0.006f, 12, chromeCol);
        AppendSurfaceDisc(mesh, surf, uMid - s * 0.01f, surf.skirtBotY + 0.435f, 0.025f, 0.029f, 0.018f, 0.006f, 12, projLens);

        // Lower corner round projector fog lamp inside embedded fog pocket
        AppendSurfaceRing(mesh, surf, s * 0.72f, surf.skirtBotY + 0.088f, 0.024f, 0.026f, 0.038f, 0.040f, 0.014f, 0.006f, 12, chromeCol);
        AppendSurfaceDisc(mesh, surf, s * 0.72f, surf.skirtBotY + 0.088f, 0.023f, 0.025f, 0.017f, 0.006f, 12, fogCol);
    }
}

void AppendDDRoofCanopy(
    Mesh* mesh,
    const DDSurface& surf,
    float winEndX,
    float xRear,
    const Vec4& roofColor
) {
    const int nu = 24;
    struct XStation {
        float x;
        float topDrop;
    };
    std::vector<XStation> stations = {
        {surf.xRoofJoin, 0.0f},
        {surf.xFront + 2.4f, 0.0f},
        {winEndX,        0.0f},
        {xRear - 0.12f,  0.04f},
        {xRear,          0.09f}
    };
    for (size_t s = 0; s + 1 < stations.size(); ++s) {
        float x0 = stations[s].x, d0 = stations[s].topDrop;
        float x1 = stations[s + 1].x, d1 = stations[s + 1].topDrop;
        for (int i = 0; i < nu; ++i) {
            float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
            float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
            Vec3 p0a = surf.EvalRoofArchPos(x0, ua, d0);
            Vec3 p0b = surf.EvalRoofArchPos(x0, ub, d0);
            Vec3 p1b = surf.EvalRoofArchPos(x1, ub, d1);
            Vec3 p1a = surf.EvalRoofArchPos(x1, ua, d1);
            CabComponents::AppendQuad(mesh, p0a, p0b, p1b, p1a, roofColor);
        }
    }
    Vec3 rearCenter(xRear, surf.winSideTop + 0.04f, 0.0f);
    for (int i = 0; i < nu; ++i) {
        float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
        float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
        Vec3 ra = surf.EvalRoofArchPos(xRear, ua, 0.09f);
        Vec3 rb = surf.EvalRoofArchPos(xRear, ub, 0.09f);
        PushTri(mesh, rearCenter, rb, ra, roofColor);
    }
}

void AppendDDCameraWingMirrors(
    Mesh* mesh,
    const DDSurface& surf,
    const Vec4& bodyPaint,
    const Vec4& darkTrim
) {
    float podY = surf.ddMidTopY - 0.08f; // ~2.44m right where A-pillar ribbon meets mid-deck
    float armPivotX = surf.xFront + 0.72f;
    const Vec4 stalkCol = surf.is9800DD ? darkTrim : bodyPaint;
    const Vec4 camLens(0.18f, 0.22f, 0.28f, 1.0f);

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zIn  = s * (surf.hw - 0.02f);
        float zMid = s * (surf.hw + 0.13f);
        float zOut = s * (surf.hw + 0.24f);
        float zMin0 = std::min(zIn, zMid), zMax0 = std::max(zIn, zMid);
        float zMin1 = std::min(zMid, zOut), zMax1 = std::max(zMid, zOut);

        // Thick sculpted digital mirror wing arm
        RunningGear::AppendBox(mesh, Vec3(armPivotX - 0.15f, podY - 0.025f, zMin0), Vec3(armPivotX + 0.02f, podY + 0.032f, zMax0), stalkCol);
        RunningGear::AppendBox(mesh, Vec3(armPivotX - 0.22f, podY - 0.035f, zMin1), Vec3(armPivotX - 0.08f, podY + 0.020f, zMax1), stalkCol);

        // Streamlined downward camera head pod
        float pz0 = s > 0 ? zOut - 0.035f : zOut - 0.075f;
        float pz1 = s > 0 ? zOut + 0.075f : zOut + 0.035f;
        RunningGear::AppendBox(mesh, Vec3(armPivotX - 0.25f, podY - 0.115f, pz0), Vec3(armPivotX - 0.11f, podY + 0.025f, pz1), darkTrim);
        RunningGear::AppendBox(mesh, Vec3(armPivotX - 0.105f, podY - 0.095f, pz0 + 0.015f), Vec3(armPivotX - 0.09f, podY - 0.025f, pz1 - 0.015f), camLens);

        if (surf.is9800DD && side > 0) {
            // Secondary upper proximity camera pod above curb (+Z) A-pillar corner matching 9800 DD studio photo
            RunningGear::AppendBox(mesh, Vec3(armPivotX - 0.08f, podY + 0.19f, surf.hw - 0.01f), Vec3(armPivotX + 0.06f, podY + 0.26f, surf.hw + 0.11f), darkTrim);
        }
    }
}

struct DDFlankLayout {
    bool is9800DD = true;
    float xFront = 0.0f;
    float xApillar = 0.0f;
    float xRoofJoin = 0.0f;
    float entryFrontX = 0.0f;
    float entryBackX = 0.0f;
    float frontAxleX = 0.0f;
    float driveAxleX = 0.0f;
    float archHalfLen = 0.63f;
    float winEndX = 0.0f;
    float xRear = 0.0f;
    float skirtBotY = 0.28f;
    float archTopY = 1.14f;
    float lowerWinTop = 2.26f;
    float upperWinBot = 2.86f;
    float winSideTop = 3.78f;

    float xPeak = 0.0f;
    float xValley = 0.0f;
    float xClimbStart = 0.0f;
    float xClimbEnd = 0.0f;
    float xProwStart = 0.0f;
    float xProwFull = 0.0f;
    float xUpperGlassStart = 0.0f;

    void Init(
        bool inIs9800DD,
        float inXFront, float inXApillar, float inXRoofJoin,
        float inEntryFrontX, float inEntryBackX,
        float inFrontAxleX, float inDriveAxleX, float inArchHalfLen,
        float inWinEndX, float inXRear,
        float inSkirtBotY, float inArchTopY,
        float inLowerWinTop, float inWinSideTop
    ) {
        is9800DD = inIs9800DD;
        xFront = inXFront;
        xApillar = inXApillar;
        xRoofJoin = inXRoofJoin;
        entryFrontX = inEntryFrontX;
        entryBackX = inEntryBackX;
        frontAxleX = inFrontAxleX;
        driveAxleX = inDriveAxleX;
        archHalfLen = inArchHalfLen;
        winEndX = inWinEndX;
        xRear = inXRear;
        skirtBotY = inSkirtBotY;
        archTopY = inArchTopY;
        lowerWinTop = inLowerWinTop;
        winSideTop = inWinSideTop;
        upperWinBot = inLowerWinTop + 0.70f;

        xPeak = frontAxleX + 0.20f;
        xValley = frontAxleX + 1.66f;
        xClimbStart = driveAxleX - 1.48f;
        xClimbEnd = is9800DD ? (driveAxleX + 0.72f) : (driveAxleX + 0.36f);
        xProwStart = entryBackX + 0.42f;
        xProwFull = frontAxleX + 0.70f;
        xUpperGlassStart = entryBackX + 0.14f;
    }

    float EvalLowWaistY(float x, bool isRightSide) const {
        float yPeak = archTopY + (is9800DD ? 0.74f : 0.62f);
        float yVal = archTopY + (is9800DD ? 0.26f : 0.20f);
        float yClimb0 = yVal + 0.16f;
        float yLowEnd = lowerWinTop - (is9800DD ? 0.04f : 0.02f);

        if (!isRightSide && x <= entryBackX + 0.16f) {
            return archTopY - 0.04f;
        }
        if (x < xPeak) {
            float x0 = isRightSide ? entryBackX : (entryBackX + 0.16f);
            float y0 = isRightSide ? (archTopY - 0.01f) : (archTopY - 0.04f);
            float t = Smooth01((x - x0) / std::max(0.10f, xPeak - x0));
            return y0 + (yPeak - y0) * t;
        }
        if (x < xValley) {
            float t = Smooth01((x - xPeak) / std::max(0.10f, xValley - xPeak));
            return yPeak + (yVal - yPeak) * t;
        }
        if (x < xClimbStart) {
            float t = (x - xValley) / std::max(0.10f, xClimbStart - xValley);
            return yVal + (yClimb0 - yVal) * t;
        }
        if (x < xClimbEnd) {
            float rawT = std::clamp((x - xClimbStart) / std::max(0.10f, xClimbEnd - xClimbStart), 0.0f, 1.0f);
            float t = is9800DD ? (0.20f * Smooth01(rawT) + 0.80f * rawT) : rawT;
            return yClimb0 + (yLowEnd - yClimb0) * t;
        }
        return yLowEnd;
    }

    float EvalMidBotY(float x) const {
        float t = std::clamp((x - xProwStart) / std::max(0.20f, winEndX - xProwStart), 0.0f, 1.0f);
        return (lowerWinTop - 0.04f) + 0.10f * t;
    }

    float EvalMidTopY(float x) const {
        if (x <= xProwStart) return EvalMidBotY(x) + 0.04f;
        if (x < xProwFull) {
            float t = std::clamp((x - xProwStart) / std::max(0.10f, xProwFull - xProwStart), 0.0f, 1.0f);
            float h = is9800DD ? std::pow(t, 0.52f) : t;
            return (lowerWinTop - 0.01f) + (upperWinBot + 0.02f - (lowerWinTop - 0.01f)) * h;
        }
        float tRear = std::clamp((x - xProwFull) / std::max(0.20f, winEndX - xProwFull), 0.0f, 1.0f);
        return (upperWinBot + 0.02f) - 0.12f * tRear;
    }

    float EvalPillarFrontX(float y) const {
        float y0 = lowerWinTop;
        float t = std::clamp((y - y0) / std::max(0.10f, winSideTop - y0), 0.0f, 1.0f);
        // Aligns continuously with frontSurf.EvalFrontPos(±1.0f, y).x (~xFront + 0.695 -> xFront + 0.945)
        float xBot = xFront + 0.705f;
        float xTop = xFront + 0.945f;
        return xBot + (xTop - xBot) * std::pow(t, 1.20f);
    }

    float EvalPillarBackX(float y) const {
        float y0 = lowerWinTop;
        float t = std::clamp((y - y0) / std::max(0.10f, winSideTop - y0), 0.0f, 1.0f);
        return (xFront + 0.84f) + ((xRoofJoin + 0.04f) - (xFront + 0.84f)) * std::pow(t, 1.04f);
    }
};

void AppendDDRearDesign(
    Mesh* mesh,
    bool is9800DD,
    float xRear,
    float skirtBotY,
    float midY,
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
        float cx = 0.028f * (1.0f - nz * nz);
        float ty = std::clamp((y - skirtBotY) / std::max(0.3f, winSideTop - skirtBotY), 0.0f, 1.0f);
        return Vec3(xRear + cx - 0.020f * ty, y, z);
    };

    auto appendRearQuad = [&](float u0, float u1, float y0, float y1, float dxOut, const Vec4& col) {
        Vec3 a = rearPos(u0, y0), b = rearPos(u1, y0), c = rearPos(u1, y1), d = rearPos(u0, y1);
        a.x += dxOut; b.x += dxOut; c.x += dxOut; d.x += dxOut;
        CabComponents::AppendQuad(mesh, b, a, d, c, col);
    };

    // Lower bumper valance + license recess + upper rear observation window glass panel
    appendRearQuad(-0.96f, 0.96f, skirtBotY + 0.10f, skirtBotY + 0.36f, 0.018f, darkTrim);
    appendRearQuad(-0.18f, 0.18f, skirtBotY + 0.17f, skirtBotY + 0.27f, 0.024f, plateWht);
    appendRearQuad(-0.82f, 0.82f, midY + 0.48f, winSideTop - 0.14f, 0.010f, glossBlack);
    appendRearQuad(-0.88f, 0.88f, midY + 0.04f, midY + 0.34f, 0.014f, is9800DD ? bodyPaint : glossBlack);

    // Center engine radiator louvered door
    appendRearQuad(-0.50f, 0.50f, skirtBotY + 0.44f, midY - 0.26f, 0.008f, glossBlack);
    int slats = is9800DD ? 3 : 4;
    for (int i = 0; i <= slats; ++i) {
        float sy = (skirtBotY + 0.44f) + (midY - 0.26f - (skirtBotY + 0.44f)) * (static_cast<float>(i) / static_cast<float>(slats));
        appendRearQuad(-0.48f, 0.48f, sy - 0.012f, sy + 0.012f, 0.014f, chromeCol);
    }

    // Side taillights & exhaust tips
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float uInner = std::min(s * 0.72f, s * 0.90f);
        float uOuter = std::max(s * 0.72f, s * 0.90f);
        appendRearQuad(uInner, uOuter, skirtBotY + 0.44f, midY - 0.16f, 0.014f, glossBlack);
        if (is9800DD) {
            float ua = std::min(s * 0.82f, s * 0.89f), ub = std::max(s * 0.82f, s * 0.89f);
            appendRearQuad(ua, ub, skirtBotY + 0.48f, midY - 0.22f, 0.022f, tailRed);
            float uc = std::min(s * 0.74f, s * 0.80f), ud = std::max(s * 0.74f, s * 0.80f);
            appendRearQuad(uc, ud, skirtBotY + 0.62f, skirtBotY + 0.90f, 0.022f, tailAmber);
            appendRearQuad(uc, ud, skirtBotY + 0.48f, skirtBotY + 0.58f, 0.022f, revWhite);
            RunningGear::AppendBox(mesh, Vec3(xRear - 0.04f, skirtBotY + 0.12f, s * 0.66f - 0.05f), Vec3(xRear + 0.05f, skirtBotY + 0.19f, s * 0.66f + 0.05f), chromeCol);
        } else {
            float ua = std::min(s * 0.78f, s * 0.88f), ub = std::max(s * 0.78f, s * 0.88f);
            appendRearQuad(ua, ub, skirtBotY + 0.56f, midY - 0.18f, 0.022f, tailRed);
            float uc = std::min(s * 0.73f, s * 0.77f), ud = std::max(s * 0.73f, s * 0.77f);
            appendRearQuad(uc, ud, skirtBotY + 0.48f, skirtBotY + 0.62f, 0.022f, tailAmber);
        }
    }

    if (is9800DD) {
        RunningGear::AppendBox(mesh, Vec3(xRear - 0.06f, H - 0.16f, -hw + 0.18f), Vec3(xRear + 0.07f, H - 0.08f, hw - 0.18f), glossBlack);
    }
    appendRearQuad(-0.36f, 0.36f, H - 0.24f, H - 0.18f, 0.022f, tailRed);
}

} // namespace

std::shared_ptr<Mesh> GenerateVolvoDoubleDeckerMesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const bool is9800DD = (def.id == VehicleID::COACH_VOLVO_9800_DD);

    const float L = std::clamp(def.dimensions.x, 12.0f, 15.2f);
    const float W = std::clamp(def.dimensions.y, 2.45f, 2.60f);
    const float H = std::clamp(def.dimensions.z, 3.85f, 4.10f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(def.wheel_radius_m, 0.45f, 0.56f);
    const float tireW = std::clamp(def.tire_width_m, 0.27f, 0.35f);

    const float frontHalfTrack = hw - tireW * 0.50f - 0.025f;
    const float rearHalfTrack  = hw - tireW * 1.06f - 0.025f;

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float skirtBotY = std::clamp(def.ground_clearance_m, 0.22f, 0.38f);
    const float archTopY = wheelR * 2.0f + 0.14f; // ~1.14m
    const float archHalfLen = wheelR + 0.13f;

    // Paint & Smoked Luxury Privacy Glazing Palette matching studio photo
    const Vec4 bodyPaint = def.base_color;
    const Vec4 midBladePaint = is9800DD
        ? Vec4(
            std::min(1.0f, bodyPaint.x * 0.85f + 0.16f),
            std::min(1.0f, bodyPaint.y * 0.85f + 0.16f),
            std::min(1.0f, bodyPaint.z * 0.85f + 0.16f),
            1.0f
          )
        : bodyPaint;
    const Vec4 glossBlack(0.055f, 0.060f, 0.075f, 1.0f);
    const Vec4 darkTrim(0.10f, 0.11f, 0.13f, 1.0f);
    const Vec4 linerDark(0.045f, 0.050f, 0.060f, 1.0f);
    const Vec4 interiorSeat(0.20f, 0.27f, 0.36f, 1.0f);
    const Vec4 interiorHeadrest(0.62f, 0.66f, 0.72f, 1.0f);
    const Vec4 interiorFloor(0.18f, 0.20f, 0.24f, 1.0f);
    const Vec4 chromeCol(0.84f, 0.87f, 0.92f, 1.0f);
    const Vec4 amberMarker(1.00f, 0.58f, 0.08f, 1.0f);
    const Vec4 bayDoorCol(
        std::min(1.0f, bodyPaint.x * 0.94f + 0.01f),
        std::min(1.0f, bodyPaint.y * 0.94f + 0.01f),
        std::min(1.0f, bodyPaint.z * 0.94f + 0.01f),
        1.0f
    );
    // Transparent lightly smoked automotive coach glass revealing cockpit & dual-deck seating cleanly
    const Vec4 windshieldGlass(0.18f, 0.32f, 0.44f, 0.34f);
    const Vec4 sideGlass(0.14f, 0.25f, 0.35f, 0.38f);

    // 1. Structural frame rails & 6x2 axles
    ChassisComponents::AppendFrameRails(mesh.get(), xFront + 0.35f, xRear - 0.22f, skirtBotY + 0.06f, skirtBotY + 0.24f, 0.45f, darkTrim);

    const float frontAxleX = xFront + 2.48f;
    const float driveAxleX = std::min(xRear - 2.35f, frontAxleX + def.wheelbase_m);
    const float tagAxleX = driveAxleX + 1.38f;

    ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxleX, wheelR, tireW, frontHalfTrack, false, def.steer_angle_deg);
    ChassisComponents::AppendVehicleAxle(mesh.get(), driveAxleX, wheelR, tireW, rearHalfTrack, true, 0.0f);
    ChassisComponents::AppendVehicleAxle(mesh.get(), tagAxleX, wheelR, tireW, frontHalfTrack, false, -def.steer_angle_deg * 0.35f);

    // Recessed semicircular wheel-well pockets & flared wheel-arch skirts
    auto emitDDWheelBay = [&](float ax, float innerZ) {
        const float cutR = wheelR + 0.085f;
        CabComponents::AppendWheelWellLiner(mesh.get(), ax, archHalfLen, skirtBotY, archTopY, hw, innerZ, linerDark);
        CabComponents::AppendSemicircularWheelArchSkirt(mesh.get(), ax, wheelR, cutR, archHalfLen, skirtBotY, archTopY, -hw, false, 0.07f, bodyPaint, linerDark);
        CabComponents::AppendSemicircularWheelArchSkirt(mesh.get(), ax, wheelR, cutR, archHalfLen, skirtBotY, archTopY,  hw, true,  0.07f, bodyPaint, linerDark);
        CabComponents::AppendWheelArchFender(mesh.get(), ax, wheelR, cutR, -hw, false, 0.036f, darkTrim);
        CabComponents::AppendWheelArchFender(mesh.get(), ax, wheelR, cutR,  hw, true,  0.036f, darkTrim);
    };
    emitDDWheelBay(frontAxleX, hw - tireW - 0.16f);
    emitDDWheelBay(driveAxleX, hw - tireW * 2.15f - 0.14f);
    emitDDWheelBay(tagAxleX, hw - tireW - 0.16f);

    // Landmarks (door front edge sits flush against front-right Ice Cyan A-pillar)
    const float entryFrontX = xFront + 0.76f;
    const float entryBackX  = xFront + 1.58f;
    const float winEndX     = xRear - 0.58f;
    const float winSideTop  = H - 0.30f;

    DDSurface frontSurf;
    frontSurf.Init(is9800DD, xFront, skirtBotY, winSideTop, H, hw);

    const float lowerWinTop     = frontSurf.ddMidBotY;       // 2.26m
    const float lowerDeckFloorY = skirtBotY + 0.18f;         // 0.46m
    const float upperDeckFloorY = frontSurf.ddMidTopY - 0.04f; // 2.48m

    // 2. Full Dual-Deck 3D Interiors + Sealed Forward Dashboard & Mid-Deck Bulkheads
    CabComponents::AppendBusInterior(
        mesh.get(),
        xFront + 0.80f, winEndX - 0.04f,
        lowerDeckFloorY, lowerWinTop - 0.04f,
        hw - 0.05f,
        entryFrontX, entryBackX,
        skirtBotY + 0.10f,
        12, false, 0.0f, 0.0f,
        interiorSeat, interiorFloor, darkTrim
    );
    // Forward cockpit dash shelf closing lower nose cleanly into lower interior cabin
    RunningGear::AppendBox(
        mesh.get(),
        Vec3(xFront + 0.42f, skirtBotY + 0.18f, -hw + 0.34f),
        Vec3(xFront + 0.94f, frontSurf.noseCenterTopY - 0.02f, hw - 0.34f),
        darkTrim
    );
    {
        float inHW = hw - 0.10f;
        float ceilHW = hw - 0.20f;
        Vec4 upCeiling(0.68f, 0.72f, 0.78f, 1.0f);
        // Sealed upper-deck front bulkhead shelf behind the black mid-deck nose divider
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.56f, lowerWinTop - 0.02f, -hw + 0.34f), Vec3(xFront + 1.04f, upperDeckFloorY, hw - 0.34f), darkTrim);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.96f, upperDeckFloorY - 0.06f, -inHW), Vec3(winEndX - 0.04f, upperDeckFloorY, inHW), interiorFloor);
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 1.04f, upperDeckFloorY, -0.22f), Vec3(winEndX - 0.10f, upperDeckFloorY + 0.015f, 0.22f), Vec4(0.46f, 0.50f, 0.56f, 1.0f));
        const int upRows = 12;
        float upStart = xFront + 1.16f;
        float upEnd = winEndX - 0.42f;
        float pitch = (upEnd - upStart) / static_cast<float>(upRows);
        for (int r = 0; r < upRows; ++r) {
            float rx = upStart + (static_cast<float>(r) + 0.5f) * pitch;
            CabComponents::AppendSeat(mesh.get(), Vec3(rx, upperDeckFloorY, -inHW + 0.24f), -1.0f, 0.40f, 0.86f, interiorSeat, interiorHeadrest, darkTrim);
            CabComponents::AppendSeat(mesh.get(), Vec3(rx, upperDeckFloorY, -inHW + 0.68f), -1.0f, 0.40f, 0.86f, interiorSeat, interiorHeadrest, darkTrim);
            CabComponents::AppendSeat(mesh.get(), Vec3(rx, upperDeckFloorY,  inHW - 0.68f), -1.0f, 0.40f, 0.86f, interiorSeat, interiorHeadrest, darkTrim);
            CabComponents::AppendSeat(mesh.get(), Vec3(rx, upperDeckFloorY,  inHW - 0.24f), -1.0f, 0.40f, 0.86f, interiorSeat, interiorHeadrest, darkTrim);
        }
        RunningGear::AppendBox(mesh.get(), Vec3(xFront + 1.38f, winSideTop - 0.04f, -ceilHW), Vec3(winEndX - 0.04f, winSideTop + 0.01f, ceilHW), upCeiling);
        RunningGear::AppendBox(mesh.get(), Vec3(winEndX - 0.08f, upperDeckFloorY, -inHW), Vec3(winEndX - 0.02f, winSideTop - 0.04f, inHW), upCeiling);
    }

    // 3. Zero-Z-Fighting Compound-Curved Front Nose, V-Collar, Lamps, Dome & Roof Canopy (Pass 1)
    AppendDDIntegratedNoseSkin(mesh.get(), frontSurf, bodyPaint, glossBlack);
    AppendDDFasciaDetails(mesh.get(), frontSurf, def.headlights_on, bodyPaint, glossBlack, darkTrim, chromeCol);
    AppendDDRoofCanopy(mesh.get(), frontSurf, winEndX, xRear, bodyPaint);

    // Subtle twin upper-deck roof hatches in body paint
    for (float hx : {xFront + 3.80f, xRear - 3.40f}) {
        RunningGear::AppendBox(mesh.get(), Vec3(hx - 0.40f, H - 0.02f, -0.42f), Vec3(hx + 0.40f, H + 0.045f, 0.42f), bodyPaint);
    }

    // Sculpted rear D-pillar tail enclosure [winEndX .. xRear]
    std::vector<HullStationX> rearTail = {
        {winEndX, skirtBotY + 0.02f, winSideTop + 0.02f, hw,         0.10f, 0.12f, 0.02f, 0.0f},
        {xRear,   skirtBotY + 0.12f, winSideTop + 0.02f, hw - 0.05f, 0.12f, 0.12f, 0.02f, 0.0f},
    };
    CabComponents::AppendSculptedHullX(mesh.get(), rearTail, bodyPaint, 4);

    // 4. Lower Side Skirts (skipping wheel bays and +Z entry doorway)
    std::vector<std::pair<float, float>> leftSkipSkirt = {
        {frontAxleX - archHalfLen, frontAxleX + archHalfLen},
        {driveAxleX - archHalfLen, driveAxleX + archHalfLen},
        {tagAxleX   - archHalfLen, tagAxleX   + archHalfLen},
    };
    std::vector<std::pair<float, float>> rightSkipSkirt = {
        {entryFrontX, entryBackX},
        {frontAxleX - archHalfLen, frontAxleX + archHalfLen},
        {driveAxleX - archHalfLen, driveAxleX + archHalfLen},
        {tagAxleX   - archHalfLen, tagAxleX   + archHalfLen},
    };
    CabComponents::AppendSkirtWithCutouts(mesh.get(), frontSurf.xApillar, winEndX, skirtBotY, archTopY,  hw, true,  0.07f, rightSkipSkirt, bodyPaint);
    CabComponents::AppendSkirtWithCutouts(mesh.get(), frontSurf.xApillar, winEndX, skirtBotY, archTopY, -hw, false, 0.07f, leftSkipSkirt, bodyPaint);

    // Subtle solid bodyPaint front-curb rocker threshold beneath entry door + rear corner filler ahead of door
    RunningGear::AppendBox(mesh.get(), Vec3(entryFrontX - 0.02f, skirtBotY, hw - 0.06f), Vec3(entryBackX + 0.02f, skirtBotY + 0.13f, hw), bodyPaint);

    // 5. Sculpted Volvo DD Side Waves, Mid-Deck Blade, Slanted Upper A-Pillar Ribbon & Mullions (Pass 1)
    DDFlankLayout dd;
    dd.Init(
        is9800DD,
        xFront, frontSurf.xApillar, frontSurf.xRoofJoin,
        entryFrontX, entryBackX,
        frontAxleX, driveAxleX, archHalfLen,
        winEndX, xRear,
        skirtBotY, archTopY,
        lowerWinTop, winSideTop
    );

    const float wallT = 0.07f;
    for (int side : {-1, 1}) {
        bool isRight = (side > 0);
        float s = static_cast<float>(side);

        // Lower painted wave wall rising smoothly behind door/driver bay & sweeping up rear diagonal
        float lowWallStart = isRight ? dd.entryBackX : dd.xApillar;
        const int lowSegs = 26;
        for (int i = 0; i < lowSegs; ++i) {
            float x0 = lowWallStart + (dd.winEndX - lowWallStart) * (static_cast<float>(i) / static_cast<float>(lowSegs));
            float x1 = lowWallStart + (dd.winEndX - lowWallStart) * (static_cast<float>(i + 1) / static_cast<float>(lowSegs));
            float yT0 = dd.EvalLowWaistY(x0, isRight);
            float yT1 = dd.EvalLowWaistY(x1, isRight);
            if (x1 > dd.xClimbEnd) {
                if (x0 >= dd.xClimbEnd) yT0 = dd.EvalMidBotY(x0) + 0.02f;
                yT1 = dd.EvalMidBotY(x1) + 0.02f;
            }
            AppendDoubleSidedWallBand(mesh.get(), x0, x1, archTopY - 0.01f, yT0, archTopY - 0.01f, yT1, hw, wallT, isRight, bodyPaint, darkTrim);
            if (x0 < dd.xClimbEnd) {
                AppendSideStripQuad(mesh.get(), x0, std::min(x1, dd.xClimbEnd), yT0 - 0.012f, yT0 + 0.016f, yT1 - 0.012f, yT1 + 0.016f, hw + 0.004f, isRight, glossBlack);
            }
        }

        if (isRight) {
            // Solid unified Ice Cyan corner cheek & pillar flange between curved front corner [xApillar] and door front edge [entryFrontX]
            AppendDoubleSidedWallBand(mesh.get(), dd.xApillar, dd.entryFrontX, archTopY - 0.01f, frontSurf.ddMidTopY, archTopY - 0.01f, frontSurf.ddMidTopY, hw, wallT, true, bodyPaint, darkTrim);
            // Solid gloss-black mid-deck header above passenger door up to mirror mount height
            AppendDoubleSidedWallBand(mesh.get(), dd.entryFrontX, dd.xUpperGlassStart, dd.lowerWinTop - 0.02f, frontSurf.ddMidTopY, dd.lowerWinTop - 0.02f, frontSurf.ddMidTopY, hw, wallT, true, glossBlack, darkTrim);
        } else {
            // Driver-side Ice Cyan front lower corner cheek merging into lower nose
            AppendDoubleSidedWallBand(mesh.get(), dd.xApillar, dd.xApillar + 0.10f, archTopY - 0.01f, frontSurf.ddMidTopY, archTopY - 0.01f, frontSurf.ddMidTopY, hw, wallT, false, bodyPaint, darkTrim);
            // Driver-side upper mid-deck header & slim vertical B-post
            AppendSideStripQuad(mesh.get(), dd.xApillar + 0.08f, dd.entryBackX + 0.16f, dd.lowerWinTop - 0.02f, frontSurf.ddMidTopY, dd.lowerWinTop - 0.02f, frontSurf.ddMidTopY, hw + 0.003f, false, glossBlack);
            AppendSideStripQuad(mesh.get(), dd.entryBackX + 0.11f, dd.entryBackX + 0.20f, dd.EvalLowWaistY(dd.entryBackX + 0.14f, false), dd.lowerWinTop, dd.EvalLowWaistY(dd.entryBackX + 0.18f, false), dd.lowerWinTop, hw + 0.005f, false, glossBlack);
        }

        // Sculpted Mid-Deck Blade [xProwStart .. winEndX] in metallic highlight tint
        const int midSegs = 26;
        for (int i = 0; i < midSegs; ++i) {
            float x0 = dd.xProwStart + (dd.winEndX - dd.xProwStart) * (static_cast<float>(i) / static_cast<float>(midSegs));
            float x1 = dd.xProwStart + (dd.winEndX - dd.xProwStart) * (static_cast<float>(i + 1) / static_cast<float>(midSegs));
            float yB0 = dd.EvalMidBotY(x0), yB1 = dd.EvalMidBotY(x1);
            float yT0 = dd.EvalMidTopY(x0), yT1 = dd.EvalMidTopY(x1);
            // Smoothly blend back to base bodyPaint behind the rear slash
            const Vec4 segCol = (x0 > dd.xClimbEnd) ? bodyPaint : midBladePaint;
            AppendDoubleSidedWallBand(mesh.get(), x0, x1, yB0, yT0, yB1, yT1, hw, wallT, isRight, segCol, darkTrim);
            AppendSideStripQuad(mesh.get(), x0, x1, yT0 - 0.014f, yT0 + 0.014f, yT1 - 0.014f, yT1 + 0.014f, hw + 0.004f, isRight, glossBlack);
            if (x0 < dd.xClimbEnd) {
                AppendSideStripQuad(mesh.get(), x0, std::min(x1, dd.xClimbEnd), yB0 - 0.010f, yB0 + 0.012f, yB1 - 0.010f, yB1 + 0.012f, hw + 0.004f, isRight, glossBlack);
            }
        }

        // Solid upper rear D-pillar flank continuing cleanly into rearTail at winEndX
        AppendDoubleSidedWallBand(mesh.get(), dd.winEndX - 0.04f, dd.winEndX + 0.02f, dd.lowerWinTop, winSideTop, dd.lowerWinTop, winSideTop, hw, wallT, isRight, bodyPaint, darkTrim);

        // Seamless Upper Front Slanted bodyPaint A-Pillar Flange (starting directly on curved corner xSideAnchor) + glossBlack Glass Bridge
        const int pilSegs = 12;
        float pilY0 = frontSurf.ddMidTopY - 0.04f;
        float pilY1 = winSideTop + 0.002f;
        for (int i = 0; i < pilSegs; ++i) {
            float ya = pilY0 + (pilY1 - pilY0) * (static_cast<float>(i) / static_cast<float>(pilSegs));
            float yb = pilY0 + (pilY1 - pilY0) * (static_cast<float>(i + 1) / static_cast<float>(pilSegs));
            float xfA = dd.EvalPillarFrontX(ya);
            float xfB = dd.EvalPillarFrontX(yb);
            float xbA = dd.EvalPillarBackX(ya);
            float xbB = dd.EvalPillarBackX(yb);
            float xMastR = dd.xUpperGlassStart;
            float zRib  = hw + 0.002f;
            float zMast = hw;
            float zMastIn = hw - wallT;
            if (isRight) {
                CabComponents::AppendQuad(mesh.get(), Vec3(xfA, ya, zRib),    Vec3(xbA, ya, zRib),      Vec3(xbB, yb, zRib),      Vec3(xfB, yb, zRib),    bodyPaint);
                CabComponents::AppendQuad(mesh.get(), Vec3(xbA, ya, zMast),   Vec3(xMastR, ya, zMast),   Vec3(xMastR, yb, zMast),   Vec3(xbB, yb, zMast),   glossBlack);
                CabComponents::AppendQuad(mesh.get(), Vec3(xMastR, ya, zMastIn), Vec3(xbA, ya, zMastIn), Vec3(xbB, yb, zMastIn), Vec3(xMastR, yb, zMastIn), darkTrim);
            } else {
                CabComponents::AppendQuad(mesh.get(), Vec3(xbA, ya, -zRib),   Vec3(xfA, ya, -zRib),     Vec3(xfB, yb, -zRib),     Vec3(xbB, yb, -zRib),   bodyPaint);
                CabComponents::AppendQuad(mesh.get(), Vec3(xMastR, ya, -zMast), Vec3(xbA, ya, -zMast),   Vec3(xbB, yb, -zMast),   Vec3(xMastR, yb, -zMast), glossBlack);
                CabComponents::AppendQuad(mesh.get(), Vec3(xbA, ya, -zMastIn), Vec3(xMastR, ya, -zMastIn), Vec3(xMastR, yb, -zMastIn), Vec3(xbB, yb, -zMastIn), darkTrim);
            }
        }
        // Gloss-black mid-deck prow bridge uniting lower black door/window into upper panoramic glazing
        AppendDoubleSidedWallBand(mesh.get(), dd.xUpperGlassStart, dd.xProwStart + 0.02f, dd.lowerWinTop - 0.02f, dd.lowerWinTop + 0.28f, dd.lowerWinTop - 0.02f, dd.EvalMidTopY(dd.xProwStart) + 0.02f, hw, wallT, isRight, glossBlack, darkTrim);

        // Signature Rear Diagonal Graphic (9800DD = clean razor black slash across D-pillar; 9700DD = black rear Z-wedge)
        if (is9800DD) {
            float sx0 = dd.xClimbEnd - 0.66f;
            float sy0 = dd.lowerWinTop - 0.28f;
            float sx1 = dd.winEndX + 0.02f;
            float sy1 = dd.EvalMidTopY(dd.winEndX) + 0.02f;
            AppendSideStripQuad(mesh.get(), sx0, sx1, sy0 - 0.065f, sy0 + 0.065f, sy1 - 0.085f, sy1 + 0.075f, hw + 0.011f, isRight, glossBlack);
        } else {
            float sx0 = dd.driveAxleX + 0.20f;
            AppendSideStripQuad(mesh.get(), sx0 - 0.62f, dd.winEndX, dd.lowerWinTop - 0.14f, dd.upperWinBot + 0.02f, dd.archTopY + 0.20f, dd.upperWinBot - 0.04f, hw + 0.010f, isRight, glossBlack);
        }

        // Window divider mullions (Pass 1)
        const int lowMulls = 4;
        float lm0 = isRight ? (dd.entryBackX + 0.10f) : (dd.entryBackX + 0.24f);
        float lm1 = dd.xClimbEnd - 0.25f;
        for (int m = 1; m <= lowMulls; ++m) {
            float mx = lm0 + (lm1 - lm0) * (static_cast<float>(m) / static_cast<float>(lowMulls + 1));
            float yb = dd.EvalLowWaistY(mx, isRight) - 0.01f;
            if (dd.lowerWinTop > yb + 0.18f) {
                AppendSideStripQuad(mesh.get(), mx - 0.020f, mx + 0.020f, yb, dd.lowerWinTop, yb, dd.lowerWinTop, hw - 0.01f, isRight, glossBlack);
            }
        }
        const int upMulls = 8;
        float um0 = dd.xUpperGlassStart + 0.12f;
        float um1 = dd.winEndX - 0.08f;
        for (int m = 1; m <= upMulls; ++m) {
            float mx = um0 + (um1 - um0) * (static_cast<float>(m) / static_cast<float>(upMulls + 1));
            float yb = (mx < dd.xProwStart) ? (dd.lowerWinTop - 0.02f) : (dd.EvalMidTopY(mx) - 0.01f);
            if (winSideTop > yb + 0.18f) {
                AppendSideStripQuad(mesh.get(), mx - 0.020f, mx + 0.020f, yb, winSideTop, yb, winSideTop, hw - 0.01f, isRight, glossBlack);
            }
        }

        // Roof cantrail bodyPaint wrap + amber marker pills
        AppendSideStripQuad(mesh.get(), frontSurf.xRoofJoin - 0.04f, dd.winEndX + 0.04f, winSideTop - 0.02f, winSideTop + 0.03f, winSideTop - 0.02f, winSideTop + 0.03f, hw + 0.004f, isRight, bodyPaint);

        // Front charging/access flap ahead of front steer wheel
        float chX0 = frontAxleX - archHalfLen - 0.24f;
        float chX1 = frontAxleX - archHalfLen - 0.03f;
        if (chX0 > entryBackX + 0.02f) {
            RunningGear::AppendBox(mesh.get(), Vec3(chX0, archTopY - 0.12f, isRight ? hw - 0.01f : -hw - 0.014f), Vec3(chX1, archTopY + 0.24f, isRight ? hw + 0.014f : -hw + 0.01f), darkTrim);
            RunningGear::AppendBox(mesh.get(), Vec3(chX0 + 0.03f, archTopY - 0.08f, isRight ? hw + 0.002f : -hw - 0.018f), Vec3(chX1 - 0.03f, archTopY + 0.20f, isRight ? hw + 0.018f : -hw - 0.002f), bodyPaint);
        }

        // Chrome VOLVO side badge plaque & lower DOT / upper cantrail amber markers & engine bay seams
        float bX = isRight ? (entryFrontX - 0.04f) : (dd.xApillar + 0.18f);
        RunningGear::AppendBox(mesh.get(), Vec3(bX - 0.07f, frontSurf.noseCenterTopY + 0.10f, isRight ? hw : -hw - 0.014f), Vec3(bX + 0.07f, frontSurf.noseCenterTopY + 0.15f, isRight ? hw + 0.014f : -hw), chromeCol);
        for (float mx : {frontAxleX - 0.72f, frontAxleX + 1.25f, (frontAxleX + driveAxleX) * 0.56f, driveAxleX - 1.15f}) {
            RunningGear::AppendBox(mesh.get(), Vec3(mx - 0.05f, skirtBotY + 0.15f, s > 0 ? hw : -hw - 0.012f), Vec3(mx + 0.05f, skirtBotY + 0.20f, s > 0 ? hw + 0.012f : -hw), amberMarker);
        }
        for (float rmx : {xFront + 3.40f, (xFront + xRear) * 0.5f, xRear - 2.40f}) {
            RunningGear::AppendBox(mesh.get(), Vec3(rmx - 0.05f, winSideTop + 0.02f, s > 0 ? hw - 0.02f : -hw - 0.005f), Vec3(rmx + 0.05f, winSideTop + 0.05f, s > 0 ? hw + 0.005f : -hw + 0.02f), amberMarker);
        }
        for (float sx : {xRear - 2.25f, xRear - 1.05f}) {
            RunningGear::AppendBox(mesh.get(), Vec3(sx - 0.008f, skirtBotY + 0.16f, s > 0 ? hw : -hw - 0.008f), Vec3(sx + 0.008f, dd.lowerWinTop - 0.18f, s > 0 ? hw + 0.008f : -hw), darkTrim);
        }
    }

    // 6. Flush Underfloor Luggage Doors
    float bayStartX = frontAxleX + archHalfLen + 0.12f;
    float bayEndX   = driveAxleX - archHalfLen - 0.12f;
    float bayLift = std::clamp(def.action_ratio, 0.0f, 1.0f) * 0.45f;
    int bayCount = 3;
    float baySpan = bayEndX - bayStartX;
    for (int b = 0; b < bayCount; ++b) {
        float bx0 = bayStartX + (baySpan * static_cast<float>(b)) / static_cast<float>(bayCount) + 0.035f;
        float bx1 = bayStartX + (baySpan * static_cast<float>(b + 1)) / static_cast<float>(bayCount) - 0.035f;
        float zOut = hw + 0.016f + bayLift * 0.28f;
        float yTop = std::min(dd.EvalLowWaistY(bx0, true), dd.EvalLowWaistY(bx1, true)) - 0.05f;
        float yBot = skirtBotY + 0.13f + bayLift * 0.48f;
        if (yTop > yBot + 0.15f) {
            RunningGear::AppendBox(mesh.get(), Vec3(bx0, yBot, -zOut), Vec3(bx1, yTop, -hw + 0.01f), bayDoorCol);
            RunningGear::AppendBox(mesh.get(), Vec3(bx0, yBot,  hw - 0.01f), Vec3(bx1, yTop,  zOut), bayDoorCol);
            float midB = (bx0 + bx1) * 0.5f;
            RunningGear::AppendBox(mesh.get(), Vec3(midB - 0.07f, yBot + 0.09f, -zOut - 0.010f), Vec3(midB + 0.07f, yBot + 0.14f, -zOut), darkTrim);
            RunningGear::AppendBox(mesh.get(), Vec3(midB - 0.07f, yBot + 0.09f,  zOut), Vec3(midB + 0.07f, yBot + 0.14f,  zOut + 0.010f), darkTrim);
        }
    }

    // 7. Openable Entry Door (Pass 1 Frame matching lower Ice Cyan cheek), Camera-Wing Mirrors & Rear Fascia
    CabComponents::AppendAnimatedDoor(
        mesh.get(),
        entryFrontX, entryBackX,
        skirtBotY + 0.13f, dd.lowerWinTop - 0.02f,
        hw, true,
        def.door_open_ratio, false, false,
        bodyPaint, glossBlack, sideGlass
    );
    AppendDDCameraWingMirrors(mesh.get(), frontSurf, bodyPaint, darkTrim);
    AppendDDRearDesign(mesh.get(), is9800DD, xRear, skirtBotY, dd.lowerWinTop, winSideTop, H, hw, def.headlights_on, bodyPaint, glossBlack, darkTrim, chromeCol);

    // 8. Transparent Tinted Automotive Glass (PASS 2 DRAWN LAST)
    // Lower front windshield conforming flush to EvalWinBottomY(u) and inner A-pillar gasket
    {
        const int nuG = 20;
        const int nvG = 5;
        for (int i = 0; i < nuG; ++i) {
            float ua = -0.89f + 1.78f * (static_cast<float>(i) / static_cast<float>(nuG));
            float ub = -0.89f + 1.78f * (static_cast<float>(i + 1) / static_cast<float>(nuG));
            float ybA = frontSurf.EvalWinBottomY(ua);
            float ybB = frontSurf.EvalWinBottomY(ub);
            for (int j = 0; j < nvG; ++j) {
                float f0 = static_cast<float>(j) / static_cast<float>(nvG);
                float f1 = static_cast<float>(j + 1) / static_cast<float>(nvG);
                float ya0 = ybA + (frontSurf.ddMidBotY - ybA) * f0;
                float yb0 = ybB + (frontSurf.ddMidBotY - ybB) * f0;
                float ya1 = ybA + (frontSurf.ddMidBotY - ybA) * f1;
                float yb1 = ybB + (frontSurf.ddMidBotY - ybB) * f1;
                Vec3 a = OffsetPt(frontSurf.EvalFrontPos(ua, ya0), frontSurf.EvalFrontNormal(ua, ya0), -0.0005f);
                Vec3 b = OffsetPt(frontSurf.EvalFrontPos(ub, yb0), frontSurf.EvalFrontNormal(ub, yb0), -0.0005f);
                Vec3 c = OffsetPt(frontSurf.EvalFrontPos(ub, yb1), frontSurf.EvalFrontNormal(ub, yb1), -0.0005f);
                Vec3 d = OffsetPt(frontSurf.EvalFrontPos(ua, ya1), frontSurf.EvalFrontNormal(ua, ya1), -0.0005f);
                CabComponents::AppendQuad(mesh.get(), a, b, c, d, windshieldGlass);
            }
        }
    }
    // Upper panoramic curved front windshield flush inside upper A-pillar frame
    AppendFrontPatch(mesh.get(), frontSurf, -0.89f, 0.89f, frontSurf.ddMidTopY, frontSurf.upperWinTopY, 20, 6, -0.0005f, windshieldGlass);

    // Side lower & upper smoked panoramic glass panes
    for (int side : {-1, 1}) {
        bool isRight = (side > 0);
        const float zGlass = hw - 0.025f;
        if (!isRight) {
            float dw0 = dd.xApillar + 0.02f;
            float dw1 = dd.entryBackX + 0.14f;
            AppendSideStripQuad(mesh.get(), dw0, dw1, dd.EvalLowWaistY(dw0, false), dd.lowerWinTop - 0.02f, dd.EvalLowWaistY(dw1, false), dd.lowerWinTop - 0.02f, zGlass, false, sideGlass);
        }
        float lg0 = isRight ? dd.entryBackX : (dd.entryBackX + 0.18f);
        float lg1 = dd.xClimbEnd;
        const int lowGSegs = 20;
        for (int i = 0; i < lowGSegs; ++i) {
            float x0 = lg0 + (lg1 - lg0) * (static_cast<float>(i) / static_cast<float>(lowGSegs));
            float x1 = lg0 + (lg1 - lg0) * (static_cast<float>(i + 1) / static_cast<float>(lowGSegs));
            float yB0 = dd.EvalLowWaistY(x0, isRight), yB1 = dd.EvalLowWaistY(x1, isRight);
            if (std::min(dd.lowerWinTop - 0.02f - yB0, dd.lowerWinTop - 0.02f - yB1) > 0.03f) {
                AppendSideStripQuad(mesh.get(), x0, x1, yB0, dd.lowerWinTop - 0.02f, yB1, dd.lowerWinTop - 0.02f, zGlass, isRight, sideGlass);
            }
        }
        const int upGSegs = 26;
        for (int i = 0; i < upGSegs; ++i) {
            float x0 = dd.xUpperGlassStart + (dd.winEndX - dd.xUpperGlassStart) * (static_cast<float>(i) / static_cast<float>(upGSegs));
            float x1 = dd.xUpperGlassStart + (dd.winEndX - dd.xUpperGlassStart) * (static_cast<float>(i + 1) / static_cast<float>(upGSegs));
            float yB0 = (x0 < dd.xProwStart) ? (dd.lowerWinTop - 0.02f) : dd.EvalMidTopY(x0);
            float yB1 = (x1 < dd.xProwStart) ? (dd.lowerWinTop - 0.02f) : dd.EvalMidTopY(x1);
            if (std::min(winSideTop - 0.01f - yB0, winSideTop - 0.01f - yB1) > 0.05f) {
                AppendSideStripQuad(mesh.get(), x0, x1, yB0, winSideTop - 0.01f, yB1, winSideTop - 0.01f, zGlass, isRight, sideGlass);
            }
        }
    }

    // Openable curb door transparent glass pass
    CabComponents::AppendAnimatedDoor(
        mesh.get(),
        entryFrontX, entryBackX,
        skirtBotY + 0.13f, dd.lowerWinTop - 0.02f,
        hw, true,
        def.door_open_ratio, false, true,
        bodyPaint, glossBlack, sideGlass
    );

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::coach
