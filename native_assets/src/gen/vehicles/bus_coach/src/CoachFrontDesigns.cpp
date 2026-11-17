#include "CoachFrontDesigns.h"
#include "CabComponents.h"
#include "RunningGear.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace sphere::vehicles::coach {

using sphere::trailers::gear::RunningGear;
using sphere::vehicles::cab::CabComponents;

namespace {

constexpr float kPi = 3.14159265358979323846f;

struct FrontSurface {
    float xFront = 0.0f;
    float xApillar = 0.0f;
    float xRoofJoin = 0.0f;
    float skirtBotY = 0.28f;
    float noseTopY = 1.32f;
    float windshieldTopY = 3.38f;
    float winSideTop = 3.34f;
    float H = 3.80f;
    float hw = 1.275f;
    bool isDoubleDeck = false;
    float ddMidY = 2.30f;

    // C1 two-arc front & corner fillet parameters
    float uFlat = 0.68f;
    float rCorner = 0.28f;
    float alpha = 0.34f;
    float sinA = 0.0f;
    float cosA = 1.0f;
    float zFlatEdge = 0.0f;
    float Rfront = 0.0f;
    float dxFrontBulge = 0.0f;

    // Smooth roof arch parameters matching CoachRoofCanopySmooth
    float rTopDome = 0.24f;
    float roofTopY = 3.76f;
    float roofCrown = 0.08f;

    void Init(
        float inXFront, float inXApillar,
        float inSkirtBotY, float inNoseTopY, float inWinTopY, float inWinSideTop,
        float inH, float inHw,
        bool inIsDoubleDeck = false
    ) {
        xFront = inXFront;
        xApillar = inXApillar;
        isDoubleDeck = inIsDoubleDeck;
        ddMidY = inSkirtBotY + 2.02f;
        xRoofJoin = inXFront + (isDoubleDeck ? 1.26f : 1.08f);
        skirtBotY = inSkirtBotY;
        noseTopY = inNoseTopY;
        windshieldTopY = inWinTopY;
        winSideTop = inWinSideTop;
        H = inH;
        hw = inHw;

        sinA = std::sin(alpha);
        cosA = std::cos(alpha);
        zFlatEdge = std::max(0.45f, hw - rCorner * (1.0f - sinA));
        Rfront = zFlatEdge / sinA;
        dxFrontBulge = Rfront * (1.0f - cosA);

        roofTopY = H - 0.04f;
        rTopDome = std::min(0.28f, std::max(0.08f, hw * 0.24f));
    }

    void EvalHeightAnchors(float y, float& xNoseCenter, float& xSideAnchor) const {
        const float chinTop = skirtBotY + 0.14f;
        if (y <= chinTop) {
            float t = std::clamp((y - skirtBotY) / 0.14f, 0.0f, 1.0f);
            float a = t * (kPi * 0.5f);
            xNoseCenter = (xFront + 0.04f) + 0.075f * (1.0f - std::sin(a));
            xSideAnchor = xApillar + 0.035f * std::cos(a);
        } else if (y <= noseTopY) {
            float t = std::clamp((y - chinTop) / std::max(0.10f, noseTopY - chinTop), 0.0f, 1.0f);
            xNoseCenter = (xFront + 0.04f) + 0.055f * t - 0.025f * std::sin(kPi * t);
            xSideAnchor = xApillar;
        } else if (isDoubleDeck) {
            // Double-decker: nearly vertical proud lower deck glass curving smoothly back above mid-deck into upper aero brow
            if (y <= ddMidY) {
                float tLow = std::clamp((y - noseTopY) / std::max(0.20f, ddMidY - noseTopY), 0.0f, 1.0f);
                xNoseCenter = (xFront + 0.095f) + 0.085f * tLow;
                xSideAnchor = xApillar + 0.045f * tLow;
            } else {
                float tHigh = std::clamp((y - ddMidY) / std::max(0.20f, windshieldTopY - ddMidY), 0.0f, 1.0f);
                float sweepC = std::pow(tHigh, 1.32f);
                float sweepS = std::pow(tHigh, 1.24f);
                xNoseCenter = (xFront + 0.180f) + 0.280f * sweepC;
                xSideAnchor = (xApillar + 0.045f) + 0.235f * sweepS;
            }
        } else {
            float t = std::clamp((y - noseTopY) / std::max(0.20f, windshieldTopY - noseTopY), 0.0f, 1.0f);
            xNoseCenter = (xFront + 0.095f) + 0.27f * t - 0.032f * std::sin(kPi * t);
            xSideAnchor = xApillar + 0.12f * t;
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

        // Subtle arched brow & gentle corner drop along the upper windshield perimeter
        float wTop = std::clamp((y - noseTopY) / std::max(0.20f, windshieldTopY - noseTopY), 0.0f, 1.0f);
        float dropT = std::pow(std::max(0.0f, (absU - uFlat) / (1.0f - uFlat)), 1.35f);
        float yDropCorner = (windshieldTopY - winSideTop) * dropT;
        float sCenter = std::min(1.0f, absU / uFlat);
        float yArchLift = 0.030f * (1.0f - sCenter * sCenter);
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
        Vec3 p0 = EvalFrontPos(u, windshieldTopY);
        Vec3 p1 = EvalRoofArchPos(xRoofJoin, u, 0.0f);
        float phi = vt * (kPi * 0.5f);
        float fx = 0.20f * vt + 0.80f * (1.0f - std::cos(phi));
        float fy = std::sin(phi);
        return Vec3(
            p0.x + (p1.x - p0.x) * fx,
            p0.y + (p1.y - p0.y) * fy,
            p0.z + (p1.z - p0.z) * vt
        );
    }

    static Vec3 SafeNormalize(const Vec3& v) {
        float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
        if (lenSq < 1e-8f) return Vec3(-1.0f, 0.0f, 0.0f);
        float inv = 1.0f / std::sqrt(lenSq);
        return Vec3(v.x * inv, v.y * inv, v.z * inv);
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
        // dU x dY points outward (-X, ±Z)
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

inline Vec3 OffsetPt(const Vec3& p, const Vec3& n, float d) {
    return Vec3(p.x + n.x * d, p.y + n.y * d, p.z + n.z * d);
}

struct RibbonSample {
    float u;
    float vBot; // y on front, or vDome [0..1] on dome
    float vTop;
};

// Emits a curved band directly on FrontSurface between [u0..u1] and [y0..y1]
void AppendFrontSurfacePatch(
    Mesh* mesh,
    const FrontSurface& surf,
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

// Emits full compound-curved lower nose shell (chin roll-under + bumper + corner wrap + belly/side closure)
void AppendSmoothFrontNoseShell(
    Mesh* mesh,
    const FrontSurface& surf,
    const Vec4& col
) {
    const int nu = 22;
    std::vector<float> yLevels = {
        surf.skirtBotY,
        surf.skirtBotY + 0.045f,
        surf.skirtBotY + 0.095f,
        surf.skirtBotY + 0.14f,
        surf.skirtBotY + (surf.noseTopY - surf.skirtBotY) * 0.42f,
        surf.skirtBotY + (surf.noseTopY - surf.skirtBotY) * 0.72f,
        surf.noseTopY
    };

    for (size_t j = 0; j + 1 < yLevels.size(); ++j) {
        float ya = yLevels[j];
        float yb = yLevels[j + 1];
        for (int i = 0; i < nu; ++i) {
            float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
            float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
            Vec3 a = surf.EvalFrontPos(ua, ya);
            Vec3 b = surf.EvalFrontPos(ub, ya);
            Vec3 c = surf.EvalFrontPos(ub, yb);
            Vec3 d = surf.EvalFrontPos(ua, yb);
            CabComponents::AppendQuad(mesh, a, b, c, d, col);
        }
    }

    // Bottom chin underside closure back to xApillar & Top cowl sill back to xApillar + 0.08
    for (int i = 0; i < nu; ++i) {
        float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
        float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
        Vec3 botA = surf.EvalFrontPos(ua, surf.skirtBotY);
        Vec3 botB = surf.EvalFrontPos(ub, surf.skirtBotY);
        CabComponents::AppendQuad(mesh, Vec3(surf.xApillar + 0.05f, surf.skirtBotY, botA.z), Vec3(surf.xApillar + 0.05f, surf.skirtBotY, botB.z), botB, botA, col);

        Vec3 topA = surf.EvalFrontPos(ua, surf.noseTopY);
        Vec3 topB = surf.EvalFrontPos(ub, surf.noseTopY);
        CabComponents::AppendQuad(mesh, topA, topB, Vec3(surf.xApillar + 0.08f, surf.noseTopY, topB.z), Vec3(surf.xApillar + 0.08f, surf.noseTopY, topA.z), col);
    }
}

// Emits smooth compound-curved top brow & roof dome from windshieldTopY back to xRoofJoin
void AppendSmoothRoofDome(
    Mesh* mesh,
    const FrontSurface& surf,
    float vStart, float vEnd,
    float offsetOut,
    int segU, int segV,
    const Vec4& col
) {
    int nu = std::max(4, segU);
    int nv = std::max(2, segV);
    for (int j = 0; j < nv; ++j) {
        float va = vStart + (vEnd - vStart) * (static_cast<float>(j) / static_cast<float>(nv));
        float vb = vStart + (vEnd - vStart) * (static_cast<float>(j + 1) / static_cast<float>(nv));
        for (int i = 0; i < nu; ++i) {
            float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
            float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
            Vec3 a = OffsetPt(surf.EvalDomePos(ua, va), surf.EvalDomeNormal(ua, va), offsetOut);
            Vec3 b = OffsetPt(surf.EvalDomePos(ub, va), surf.EvalDomeNormal(ub, va), offsetOut);
            Vec3 c = OffsetPt(surf.EvalDomePos(ub, vb), surf.EvalDomeNormal(ub, vb), offsetOut);
            Vec3 d = OffsetPt(surf.EvalDomePos(ua, vb), surf.EvalDomeNormal(ua, vb), offsetOut);
            CabComponents::AppendQuad(mesh, a, b, c, d, col);
        }
    }
}

// Emits a beveled 3D polygonal ribbon strictly projected onto FrontSurface (u increasing left->right)
void AppendBeveledFrontRibbon(
    Mesh* mesh,
    const FrontSurface& surf,
    std::vector<RibbonSample> samples,
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
        // Front face
        CabComponents::AppendQuad(mesh, fB[i], fB[i + 1], fT[i + 1], fT[i], col);
        // Top bevel edge
        CabComponents::AppendQuad(mesh, fT[i], fT[i + 1], bT[i + 1], bT[i], col);
        // Bottom bevel edge
        CabComponents::AppendQuad(mesh, bB[i], bB[i + 1], fB[i + 1], fB[i], col);
    }
    // Left (-u) & Right (+u) end caps
    CabComponents::AppendQuad(mesh, bB.front(), fB.front(), fT.front(), bT.front(), col);
    CabComponents::AppendQuad(mesh, fB.back(), bB.back(), bT.back(), fT.back(), col);
}

// Same beveled polygon ribbon projected cleanly onto the curved roof dome (vBot/vTop in [0..1])
void AppendBeveledDomeRibbon(
    Mesh* mesh,
    const FrontSurface& surf,
    std::vector<RibbonSample> samples,
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
        float vb = std::clamp(samples[i].vBot, 0.0f, 0.98f);
        float vt = std::clamp(samples[i].vTop, vb + 0.01f, 1.0f);
        Vec3 pb = surf.EvalDomePos(u, vb);
        Vec3 nb = surf.EvalDomeNormal(u, vb);
        Vec3 pt = surf.EvalDomePos(u, vt);
        Vec3 nt = surf.EvalDomeNormal(u, vt);
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
    CabComponents::AppendQuad(mesh, fB.back(), bB.back(), bT.back(), fT.back(), col);
}

// Emits a crisp circular/oval sculpted disc on the curved front surface
void AppendSurfaceDisc(
    Mesh* mesh,
    const FrontSurface& surf,
    float uC, float yC,
    float radU, float radY,
    float protrudeOut, float depthIn,
    int segs,
    const Vec4& col
) {
    if (!mesh || segs < 6) return;
    Vec3 pc = surf.EvalFrontPos(uC, yC);
    Vec3 nc = surf.EvalFrontNormal(uC, yC);
    Vec3 centerF = OffsetPt(pc, nc, protrudeOut + 0.004f);

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
        uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
        Vertex v0; v0.position = centerF;  v0.color = col;
        Vertex v1; v1.position = rimF[k];  v1.color = col;
        Vertex v2; v2.position = rimF[kn]; v2.color = col;
        mesh->vertices.push_back(v0);
        mesh->vertices.push_back(v1);
        mesh->vertices.push_back(v2);
        mesh->indices.push_back(base + 0);
        mesh->indices.push_back(base + 1);
        mesh->indices.push_back(base + 2);
        // Side rim wall
        CabComponents::AppendQuad(mesh, rimB[k], rimB[kn], rimF[kn], rimF[k], col);
    }
}

// Emits an annular metallic bezel ring around a circular projector or fog optic
void AppendSurfaceRing(
    Mesh* mesh,
    const FrontSurface& surf,
    float uC, float yC,
    float rInU, float rInY,
    float rOutU, float rOutY,
    float protrudeOut, float depthIn,
    int segs,
    const Vec4& col
) {
    if (!mesh || segs < 6) return;
    std::vector<Vec3> iF(segs), iB(segs), oF(segs), oB(segs);
    for (int k = 0; k < segs; ++k) {
        float a = 2.0f * kPi * (static_cast<float>(k) / static_cast<float>(segs));
        float ca = std::cos(a), sa = std::sin(a);
        Vec3 pi = surf.EvalFrontPos(uC + rInU * ca, yC + rInY * sa);
        Vec3 ni = surf.EvalFrontNormal(uC + rInU * ca, yC + rInY * sa);
        Vec3 po = surf.EvalFrontPos(uC + rOutU * ca, yC + rOutY * sa);
        Vec3 no = surf.EvalFrontNormal(uC + rOutU * ca, yC + rOutY * sa);
        iF[k] = OffsetPt(pi, ni, protrudeOut);
        iB[k] = OffsetPt(pi, ni, -depthIn);
        oF[k] = OffsetPt(po, no, protrudeOut);
        oB[k] = OffsetPt(po, no, -depthIn);
    }
    for (int k = 0; k < segs; ++k) {
        int kn = (k + 1) % segs;
        CabComponents::AppendQuad(mesh, iF[k], oF[k], oF[kn], iF[kn], col);
        CabComponents::AppendQuad(mesh, oB[k], oB[kn], oF[kn], oF[k], col);
        CabComponents::AppendQuad(mesh, iF[k], iF[kn], iB[kn], iB[k], col);
    }
}

void AppendSculptedMedallion(
    Mesh* mesh,
    const FrontSurface& surf,
    float centerY,
    float radM,
    const Vec4& chromeCol,
    const Vec4& darkBacking,
    bool isVolvoDiagonal
) {
    float radU = radM / std::max(0.8f, surf.hw);
    // Dark recessed circular disc
    AppendSurfaceDisc(mesh, surf, 0.0f, centerY, radU * 1.02f, radM * 1.02f, 0.012f, 0.016f, 12, darkBacking);
    // Bright chrome outer circular ring
    AppendSurfaceRing(mesh, surf, 0.0f, centerY, radU * 0.78f, radM * 0.78f, radU * 1.04f, radM * 1.04f, 0.018f, 0.014f, 12, chromeCol);
    // Horizontal crossbar
    AppendBeveledFrontRibbon(
        mesh, surf,
        {{-radU * 1.10f, centerY - radM * 0.16f, centerY + radM * 0.16f},
         { 0.0f,         centerY - radM * 0.16f, centerY + radM * 0.16f},
         { radU * 1.10f, centerY - radM * 0.16f, centerY + radM * 0.16f}},
        0.021f, 0.010f, chromeCol
    );
    if (isVolvoDiagonal) {
        // Continuous angled chrome slash across badge
        float uSpan = radU * 2.1f;
        float ySpan = radM * 1.45f;
        std::vector<RibbonSample> slash;
        for (int k = -3; k <= 3; ++k) {
            float t = static_cast<float>(k) / 3.0f;
            slash.push_back({uSpan * t, centerY + ySpan * t - 0.016f, centerY + ySpan * t + 0.016f});
        }
        AppendBeveledFrontRibbon(mesh, surf, slash, 0.018f, 0.010f, chromeCol);
    } else {
        // Vertical twin crest bar
        AppendBeveledFrontRibbon(
            mesh, surf,
            {{-radU * 0.24f, centerY - radM * 0.75f, centerY + radM * 0.75f},
             { radU * 0.24f, centerY - radM * 0.75f, centerY + radM * 0.75f}},
            0.021f, 0.010f, chromeCol
        );
    }
}

void AppendSculptedSlattedGrille(
    Mesh* mesh,
    const FrontSurface& surf,
    float uHalf,
    float uHalfTop,
    float yBot,
    float yTop,
    int slats,
    bool hasBorder,
    const Vec4& surroundCol,
    const Vec4& backingCol,
    const Vec4& slatCol
) {
    // Tapered dark trapezoidal honeycomb cavity
    std::vector<RibbonSample> cavity;
    int segs = 8;
    for (int i = 0; i <= segs; ++i) {
        float t = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(segs));
        float uMaxAtY = uHalf + (uHalfTop - uHalf) * 0.5f;
        float edgeLift = std::max(0.0f, std::abs(t) - 0.55f) * 0.03f;
        cavity.push_back({t * uMaxAtY, yBot + edgeLift, yTop - edgeLift * 0.4f});
    }
    AppendBeveledFrontRibbon(mesh, surf, cavity, 0.008f, 0.018f, backingCol);

    if (hasBorder) {
        float b = 0.022f;
        // Top arched border
        AppendBeveledFrontRibbon(
            mesh, surf,
            {{-uHalfTop, yTop - b, yTop + b}, {0.0f, yTop - b, yTop + b}, {uHalfTop, yTop - b, yTop + b}},
            0.015f, 0.012f, surroundCol
        );
        // Bottom arched border
        AppendBeveledFrontRibbon(
            mesh, surf,
            {{-uHalf, yBot - b, yBot + b}, {0.0f, yBot - b, yBot + b}, {uHalf, yBot - b, yBot + b}},
            0.015f, 0.012f, surroundCol
        );
        // Left & right angled side pillars
        for (int side : {-1, 1}) {
            float s = static_cast<float>(side);
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{s * std::min(uHalf, uHalfTop) - 0.02f, yBot, yTop},
                 {s * std::max(uHalf, uHalfTop) + 0.02f, yBot, yTop}},
                0.015f, 0.012f, surroundCol
            );
        }
    }

    for (int i = 1; i <= slats; ++i) {
        float f = static_cast<float>(i) / static_cast<float>(slats + 1);
        float sy = yBot + (yTop - yBot) * f;
        float su = (uHalf + (uHalfTop - uHalf) * f) * 0.94f;
        AppendBeveledFrontRibbon(
            mesh, surf,
            {{-su, sy - 0.010f, sy + 0.010f}, {0.0f, sy - 0.010f, sy + 0.010f}, {su, sy - 0.010f, sy + 0.010f}},
            0.013f, 0.008f, slatCol
        );
    }
}

void AppendSurfaceTwinWipers(
    Mesh* mesh,
    const FrontSurface& surf,
    float sillY,
    const Vec4& col
) {
    AppendBeveledFrontRibbon(
        mesh, surf,
        {{-0.62f, sillY + 0.02f, sillY + 0.055f}, {-0.34f, sillY + 0.03f, sillY + 0.065f}, {-0.04f, sillY + 0.015f, sillY + 0.050f}},
        0.020f, 0.010f, col
    );
    AppendBeveledFrontRibbon(
        mesh, surf,
        {{ 0.04f, sillY + 0.015f, sillY + 0.050f}, { 0.34f, sillY + 0.03f, sillY + 0.065f}, { 0.62f, sillY + 0.02f, sillY + 0.055f}},
        0.020f, 0.010f, col
    );
}

// Smooth black outer curved corner A-pillar frames wrapping seamlessly into side glass
void AppendCurvedCornerPillars(
    Mesh* mesh,
    const FrontSurface& surf,
    float yBot, float yTop,
    const Vec4& pillarCol
) {
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float ua = (side < 0) ? -1.0f : 0.86f;
        float ub = (side < 0) ? -0.86f : 1.0f;
        AppendFrontSurfacePatch(mesh, surf, ua, ub, yBot, yTop, 4, 8, 0.004f, pillarCol);

        // Close rear edge along z = ±hw cleanly back to xApillar + 0.14f
        int steps = 8;
        float zWall = s * surf.hw;
        float xBack = surf.xApillar + 0.14f;
        for (int j = 0; j < steps; ++j) {
            float ya = yBot + (yTop - yBot) * (static_cast<float>(j) / static_cast<float>(steps));
            float yb = yBot + (yTop - yBot) * (static_cast<float>(j + 1) / static_cast<float>(steps));
            Vec3 pa = surf.EvalFrontPos(s * 1.0f, ya);
            Vec3 pb = surf.EvalFrontPos(s * 1.0f, yb);
            if (side > 0) {
                CabComponents::AppendQuad(mesh, Vec3(pa.x, pa.y, zWall), Vec3(xBack, pa.y, zWall), Vec3(xBack, pb.y, zWall), Vec3(pb.x, pb.y, zWall), pillarCol);
            } else {
                CabComponents::AppendQuad(mesh, Vec3(xBack, pa.y, zWall), Vec3(pa.x, pa.y, zWall), Vec3(pb.x, pb.y, zWall), Vec3(xBack, pb.y, zWall), pillarCol);
            }
        }
    }
}

void AppendCurvedMidDeckBeam(
    Mesh* mesh,
    const FrontSurface& surf,
    float yBot, float yTop,
    const Vec4& beamCol
) {
    AppendFrontSurfacePatch(mesh, surf, -1.0f, 1.0f, yBot, yTop, 18, 2, 0.010f, beamCol);
}

void AppendPillDotMarkersOnDome(
    Mesh* mesh,
    const FrontSurface& surf,
    const std::vector<float>& uPositions,
    float vBot, float vTop,
    const Vec4& col
) {
    for (float u : uPositions) {
        AppendBeveledDomeRibbon(
            mesh, surf,
            {{u - 0.032f, vBot, vTop}, {u, vBot - 0.008f, vTop + 0.008f}, {u + 0.032f, vBot, vTop}},
            0.016f, 0.008f, col
        );
    }
}

} // namespace

void AppendCoachFrontDesign(
    Mesh* mesh,
    CoachDesignStyle style,
    float xFront,
    float xApillar,
    float skirtBotY,
    float noseTopY,
    float windshieldTopY,
    float winSideTop,
    float H,
    float hw,
    bool headlightsOn,
    const Vec4& bodyPaint,
    const Vec4& accent,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol,
    const Vec4& ledDisplay
) {
    if (!mesh) return;

    const bool isDD = (style == CoachDesignStyle::VOLVO_9700_DD || style == CoachDesignStyle::VOLVO_9800_DD);
    FrontSurface surf;
    surf.Init(xFront, xApillar, skirtBotY, noseTopY, windshieldTopY, winSideTop, H, hw, isDD);

    const Vec4 drlCol    = headlightsOn ? Vec4(0.97f, 0.99f, 1.00f, 1.0f) : Vec4(0.68f, 0.74f, 0.80f, 1.0f);
    const Vec4 beamCol   = headlightsOn ? Vec4(0.86f, 0.96f, 1.00f, 1.0f) : Vec4(0.48f, 0.55f, 0.64f, 1.0f);
    const Vec4 fogCol    = headlightsOn ? Vec4(0.98f, 0.97f, 0.88f, 1.0f) : Vec4(0.56f, 0.60f, 0.65f, 1.0f);
    const Vec4 amberCol  = headlightsOn ? Vec4(1.00f, 0.64f, 0.08f, 1.0f) : Vec4(0.62f, 0.38f, 0.08f, 1.0f);
    const Vec4 plateWht  = Vec4(0.94f, 0.95f, 0.97f, 1.0f);
    const Vec4 silverLip = Vec4(0.84f, 0.87f, 0.92f, 1.0f);

    // 1. Compound-curved lower nose & wrapped cheek corners
    AppendSmoothFrontNoseShell(mesh, surf, bodyPaint);

    // Sculpted dashboard cowl sill safely inset behind curved windshield glass
    RunningGear::AppendBox(
        mesh,
        Vec3(xFront + 0.34f, noseTopY - 0.07f, -hw + 0.30f),
        Vec3(xApillar + 0.08f, noseTopY + 0.01f, hw - 0.30f),
        darkTrim
    );

    // Flush curved corner A-pillars wrapping smoothly into side glass
    if (isDD) {
        // Volvo 9700DD / 9800DD signature: slender body-painted vertical A-pillar along lower deck + gloss-black upper corner wrapping dome
        float midTop = skirtBotY + 2.30f;
        AppendCurvedCornerPillars(mesh, surf, noseTopY - 0.01f, midTop, bodyPaint);
        AppendCurvedCornerPillars(mesh, surf, midTop, windshieldTopY, glossBlack);
    } else {
        const Vec4 pillarCol = (style == CoachDesignStyle::VOLVO_9700_TEAL || style == CoachDesignStyle::VOLVO_9800_AERO)
            ? bodyPaint
            : ((style == CoachDesignStyle::VOLVO_9900_THEATER || style == CoachDesignStyle::CHAMPAGNE_SR2) ? darkTrim : glossBlack);
        AppendCurvedCornerPillars(mesh, surf, noseTopY - 0.01f, windshieldTopY, pillarCol);
    }

    // Common curved windshield wipers
    AppendSurfaceTwinWipers(mesh, surf, noseTopY, glossBlack);

    // Smooth curved roof dome rolling back seamlessly to RoofCanopy
    Vec4 domeCol = bodyPaint;
    if (style == CoachDesignStyle::UNVI_HIGH_DECKER ||
        style == CoachDesignStyle::VOLVO_9700_DD ||
        style == CoachDesignStyle::VOLVO_9800_DD ||
        style == CoachDesignStyle::YUTONG_TOURING ||
        style == CoachDesignStyle::NEOPLAN_TOURLINER ||
        style == CoachDesignStyle::TWIN_GLASS_DOME ||
        style == CoachDesignStyle::OBSIDIAN_VIP_6X2) {
        domeCol = glossBlack;
    } else if (style == CoachDesignStyle::CHAMPAGNE_SR2 || style == CoachDesignStyle::VOLVO_9900_THEATER) {
        domeCol = (style == CoachDesignStyle::VOLVO_9900_THEATER) ? darkTrim : bodyPaint;
    } else if (style == CoachDesignStyle::VOLVO_9700_USCAN) {
        domeCol = accent;
    }

    if (style == CoachDesignStyle::UNVI_HIGH_DECKER) {
        // UNVI: black lower forehead brow curving cleanly into pearl white upper dome
        AppendSmoothRoofDome(mesh, surf, 0.0f, 0.36f, 0.002f, 20, 4, glossBlack);
        AppendSmoothRoofDome(mesh, surf, 0.36f, 1.0f, 0.002f, 20, 6, bodyPaint);
    } else if (style == CoachDesignStyle::NEOPLAN_TOURLINER) {
        // Neoplan: glossy black upper dome sweeping back into pearl white roof shell
        AppendSmoothRoofDome(mesh, surf, 0.0f, 0.64f, 0.002f, 20, 6, glossBlack);
        AppendSmoothRoofDome(mesh, surf, 0.64f, 1.0f, 0.002f, 20, 4, bodyPaint);
    } else {
        AppendSmoothRoofDome(mesh, surf, 0.0f, 1.0f, 0.002f, 22, 8, domeCol);
    }

    // Now sculpt each model's bespoke front fascia, curved lights, DRL ribbons, and custom grilles
    switch (style) {
        case CoachDesignStyle::UNVI_HIGH_DECKER: {
            // Destination LED matrix embedded seamlessly inside curved black brow
            AppendBeveledDomeRibbon(
                mesh, surf,
                {{-0.44f, 0.08f, 0.25f}, {0.0f, 0.08f, 0.25f}, {0.44f, 0.08f, 0.25f}},
                0.014f, 0.008f, ledDisplay
            );

            // Tapered sweeping black V-mask below windshield
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.78f, noseTopY - 0.14f, noseTopY + 0.01f},
                 {-0.42f, skirtBotY + 0.42f, noseTopY + 0.01f},
                 { 0.0f,  skirtBotY + 0.36f, noseTopY + 0.01f},
                 { 0.42f, skirtBotY + 0.42f, noseTopY + 0.01f},
                 { 0.78f, noseTopY - 0.14f, noseTopY + 0.01f}},
                0.009f, 0.014f, glossBlack
            );
            AppendSculptedMedallion(mesh, surf, (skirtBotY + 0.38f + noseTopY) * 0.52f, 0.085f, chromeCol, glossBlack, true);

            // Sculpted trapezoidal lower bumper grille + dual silver louver slats + plate
            AppendSculptedSlattedGrille(mesh, surf, 0.40f, 0.48f, skirtBotY + 0.07f, skirtBotY + 0.24f, 2, true, darkTrim, glossBlack, silverLip);
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.20f, skirtBotY + 0.26f, skirtBotY + 0.34f}, {0.20f, skirtBotY + 0.26f, skirtBotY + 0.34f}},
                0.014f, 0.008f, plateWht
            );

            // Swept boomerang upward-flaring LED wing headlamps curving cleanly around the corners
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                std::vector<RibbonSample> wingBody = {
                    {s * 0.42f, skirtBotY + 0.24f, skirtBotY + 0.37f},
                    {s * 0.58f, skirtBotY + 0.30f, skirtBotY + 0.45f},
                    {s * 0.74f, skirtBotY + 0.39f, skirtBotY + 0.55f},
                    {s * 0.87f, skirtBotY + 0.47f, skirtBotY + 0.61f}
                };
                AppendBeveledFrontRibbon(mesh, surf, wingBody, 0.015f, 0.012f, glossBlack);

                // Smooth swept top-edge DRL wing blade
                std::vector<RibbonSample> wingDrl = {
                    {s * 0.43f, skirtBotY + 0.34f, skirtBotY + 0.365f},
                    {s * 0.58f, skirtBotY + 0.42f, skirtBotY + 0.445f},
                    {s * 0.74f, skirtBotY + 0.52f, skirtBotY + 0.545f},
                    {s * 0.86f, skirtBotY + 0.58f, skirtBotY + 0.605f}
                };
                AppendBeveledFrontRibbon(mesh, surf, wingDrl, 0.022f, 0.008f, drlCol);

                // 3 diagonal circular chrome-ringed crystal projectors inside wing
                for (int p = 0; p < 3; ++p) {
                    float t = static_cast<float>(p) / 2.0f;
                    float pu = s * (0.49f + 0.29f * t);
                    float py = skirtBotY + 0.31f + 0.20f * t;
                    AppendSurfaceRing(mesh, surf, pu, py, 0.026f, 0.028f, 0.038f, 0.042f, 0.022f, 0.008f, 12, silverLip);
                    AppendSurfaceDisc(mesh, surf, pu, py, 0.025f, 0.027f, 0.021f, 0.006f, 12, beamCol);
                }

                // Rounded beveled lower bumper corner fog lamp
                float fu = s * 0.74f;
                float fy = skirtBotY + 0.14f;
                AppendSurfaceRing(mesh, surf, fu, fy, 0.030f, 0.036f, 0.052f, 0.058f, 0.015f, 0.010f, 12, darkTrim);
                AppendSurfaceDisc(mesh, surf, fu, fy, 0.029f, 0.035f, 0.017f, 0.008f, 12, fogCol);
            }
            break;
        }

        case CoachDesignStyle::VOLVO_9900_THEATER: {
            // Swept curved air-slot cowl below windshield
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.74f, noseTopY - 0.12f, noseTopY - 0.02f}, {0.0f, noseTopY - 0.13f, noseTopY - 0.02f}, {0.74f, noseTopY - 0.12f, noseTopY - 0.02f}},
                0.010f, 0.012f, glossBlack
            );
            // Trapezoidal slatted Volvo upper grille + circular badge
            float gBot = skirtBotY + 0.42f;
            float gTop = noseTopY - 0.17f;
            AppendSculptedSlattedGrille(mesh, surf, 0.32f, 0.38f, gBot, gTop, 3, true, silverLip, glossBlack, darkTrim);
            AppendSculptedMedallion(mesh, surf, (gBot + gTop) * 0.5f, 0.080f, chromeCol, glossBlack, true);

            // Wide curved Euro lower intake grille + plate
            AppendSculptedSlattedGrille(mesh, surf, 0.46f, 0.50f, skirtBotY + 0.08f, skirtBotY + 0.27f, 2, true, darkTrim, darkTrim, darkTrim);
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.21f, skirtBotY + 0.11f, skirtBotY + 0.21f}, {0.21f, skirtBotY + 0.11f, skirtBotY + 0.21f}},
                0.016f, 0.008f, plateWht
            );
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.21f, skirtBotY + 0.11f, skirtBotY + 0.21f}, {-0.16f, skirtBotY + 0.11f, skirtBotY + 0.21f}},
                0.018f, 0.006f, Vec4(0.08f, 0.26f, 0.72f, 1.0f)
            );

            // Wide almond/swept Euro headlamps wrapping cleanly around corners + circular chrome projector lenses + round chrome-ringed fogs
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                std::vector<RibbonSample> housing = {
                    {s * 0.40f, gBot + 0.01f, gTop - 0.04f},
                    {s * 0.58f, gBot + 0.00f, gTop + 0.00f},
                    {s * 0.76f, gBot + 0.02f, gTop + 0.04f},
                    {s * 0.88f, gBot + 0.05f, gTop + 0.05f}
                };
                AppendBeveledFrontRibbon(mesh, surf, housing, 0.014f, 0.012f, darkTrim);

                // Inner bright chrome reflector bowl
                std::vector<RibbonSample> reflector = {
                    {s * 0.42f, gBot + 0.025f, gTop - 0.05f},
                    {s * 0.58f, gBot + 0.015f, gTop - 0.015f},
                    {s * 0.75f, gBot + 0.035f, gTop + 0.025f},
                    {s * 0.86f, gBot + 0.06f,  gTop + 0.035f}
                };
                AppendBeveledFrontRibbon(mesh, surf, reflector, 0.018f, 0.008f, silverLip);

                // Lower curved DRL ribbon
                std::vector<RibbonSample> drl = {
                    {s * 0.42f, gBot + 0.018f, gBot + 0.042f},
                    {s * 0.62f, gBot + 0.012f, gBot + 0.036f},
                    {s * 0.80f, gBot + 0.035f, gBot + 0.060f}
                };
                AppendBeveledFrontRibbon(mesh, surf, drl, 0.022f, 0.006f, drlCol);

                // Twin round crystal projectors + outer amber turn pod
                AppendSurfaceDisc(mesh, surf, s * 0.51f, (gBot + gTop) * 0.5f + 0.005f, 0.036f, 0.042f, 0.022f, 0.008f, 12, beamCol);
                AppendSurfaceDisc(mesh, surf, s * 0.65f, (gBot + gTop) * 0.5f + 0.015f, 0.034f, 0.040f, 0.022f, 0.008f, 12, beamCol);
                AppendSurfaceDisc(mesh, surf, s * 0.79f, (gBot + gTop) * 0.5f + 0.030f, 0.028f, 0.034f, 0.022f, 0.008f, 10, amberCol);

                // Circular fog with chrome C-ring
                AppendSurfaceRing(mesh, surf, s * 0.76f, skirtBotY + 0.18f, 0.032f, 0.038f, 0.050f, 0.058f, 0.016f, 0.010f, 12, silverLip);
                AppendSurfaceDisc(mesh, surf, s * 0.76f, skirtBotY + 0.18f, 0.031f, 0.037f, 0.018f, 0.008f, 12, fogCol);
            }
            break;
        }

        case CoachDesignStyle::VOLVO_BZR_ELECTRIC: {
            // Smooth eyebrow LED corner ticks on painted dome
            for (float su : {-0.76f, 0.76f}) {
                AppendBeveledDomeRibbon(mesh, surf, {{su - 0.05f, 0.10f, 0.22f}, {su + 0.05f, 0.10f, 0.22f}}, 0.015f, 0.008f, drlCol);
            }

            // Full-width curved gloss-black upper mask + full-width arched chrome brow blade + circular badge
            float uMaskBot = skirtBotY + 0.55f;
            AppendFrontSurfacePatch(mesh, surf, -0.92f, 0.92f, uMaskBot, noseTopY, 16, 4, 0.008f, glossBlack);
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.90f, noseTopY - 0.055f, noseTopY - 0.015f}, {0.0f, noseTopY - 0.065f, noseTopY - 0.022f}, {0.90f, noseTopY - 0.055f, noseTopY - 0.015f}},
                0.016f, 0.008f, chromeCol
            );
            AppendSculptedMedallion(mesh, surf, (uMaskBot + noseTopY - 0.04f) * 0.5f, 0.085f, chromeCol, glossBlack, true);

            // Wide EV lower trapezoidal cooling aperture with satin-silver chin splitter blade
            AppendSculptedSlattedGrille(mesh, surf, 0.42f, 0.36f, skirtBotY + 0.08f, skirtBotY + 0.40f, 3, false, silverLip, glossBlack, darkTrim);
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.48f, skirtBotY + 0.05f, skirtBotY + 0.085f}, {0.0f, skirtBotY + 0.04f, skirtBotY + 0.075f}, {0.48f, skirtBotY + 0.05f, skirtBotY + 0.085f}},
                0.018f, 0.010f, silverLip
            );

            // Vertical V-tapered LED headlamps + continuous J-hook DRLs + dual round jewel projectors + angled fog pockets
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                std::vector<RibbonSample> lampPod = {
                    {s * 0.54f, skirtBotY + 0.21f, skirtBotY + 0.44f},
                    {s * 0.68f, skirtBotY + 0.21f, skirtBotY + 0.50f},
                    {s * 0.84f, skirtBotY + 0.23f, skirtBotY + 0.54f}
                };
                AppendBeveledFrontRibbon(mesh, surf, lampPod, 0.016f, 0.012f, glossBlack);
                // Crisp outer vertical + lower L-blade DRL light pipe
                AppendBeveledFrontRibbon(mesh, surf, {{s * 0.80f, skirtBotY + 0.22f, skirtBotY + 0.52f}, {s * 0.84f, skirtBotY + 0.23f, skirtBotY + 0.53f}}, 0.023f, 0.008f, drlCol);
                AppendBeveledFrontRibbon(mesh, surf, {{s * 0.56f, skirtBotY + 0.215f, skirtBotY + 0.245f}, {s * 0.83f, skirtBotY + 0.230f, skirtBotY + 0.260f}}, 0.023f, 0.008f, drlCol);
                // Dual stacked circular projectors
                AppendSurfaceRing(mesh, surf, s * 0.69f, skirtBotY + 0.32f, 0.026f, 0.032f, 0.038f, 0.044f, 0.022f, 0.006f, 12, silverLip);
                AppendSurfaceDisc(mesh, surf, s * 0.69f, skirtBotY + 0.32f, 0.025f, 0.031f, 0.023f, 0.006f, 12, beamCol);
                AppendSurfaceRing(mesh, surf, s * 0.72f, skirtBotY + 0.44f, 0.026f, 0.032f, 0.038f, 0.044f, 0.022f, 0.006f, 12, silverLip);
                AppendSurfaceDisc(mesh, surf, s * 0.72f, skirtBotY + 0.44f, 0.025f, 0.031f, 0.023f, 0.006f, 12, beamCol);

                // Angled lower cheek fog intake
                AppendBeveledFrontRibbon(
                    mesh, surf,
                    {{s * 0.58f, skirtBotY + 0.05f, skirtBotY + 0.14f}, {s * 0.80f, skirtBotY + 0.07f, skirtBotY + 0.18f}},
                    0.015f, 0.010f, glossBlack
                );
                AppendSurfaceDisc(mesh, surf, s * 0.70f, skirtBotY + 0.11f, 0.028f, 0.032f, 0.021f, 0.008f, 12, fogCol);
            }
            break;
        }

        case CoachDesignStyle::VOLVO_9700_USCAN: {
            // 5 pill DOT amber markers on curved brow + sleek center vertical split-windshield mullion
            AppendPillDotMarkersOnDome(mesh, surf, {-0.78f, -0.18f, 0.0f, 0.18f, 0.78f}, 0.58f, 0.70f, amberCol);
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.022f, noseTopY - 0.02f, windshieldTopY + 0.02f}, {0.022f, noseTopY - 0.02f, windshieldTopY + 0.02f}},
                0.012f, 0.010f, glossBlack
            );
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.86f, noseTopY - 0.08f, noseTopY + 0.01f}, {0.0f, noseTopY - 0.08f, noseTopY + 0.01f}, {0.86f, noseTopY - 0.08f, noseTopY + 0.01f}},
                0.008f, 0.010f, glossBlack
            );

            // Wide chrome-ringed rectangular horizontal grille with slats & round emblem
            float gBot = skirtBotY + 0.36f;
            float gTop = skirtBotY + 0.56f;
            AppendSculptedSlattedGrille(mesh, surf, 0.38f, 0.38f, gBot, gTop, 3, true, chromeCol, glossBlack, darkTrim);
            AppendSculptedMedallion(mesh, surf, (gBot + gTop) * 0.5f, 0.072f, chromeCol, glossBlack, true);

            // Lower chin slot
            AppendSculptedSlattedGrille(mesh, surf, 0.38f, 0.36f, skirtBotY + 0.08f, skirtBotY + 0.22f, 2, false, darkTrim, darkTrim, glossBlack);

            // Wide wrap-around slim horizontal headlamps + continuous eyebrow + circular projectors + round recessed fogs
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                std::vector<RibbonSample> lamp = {
                    {s * 0.44f, gBot + 0.02f, gTop - 0.01f},
                    {s * 0.66f, gBot + 0.02f, gTop + 0.01f},
                    {s * 0.87f, gBot + 0.04f, gTop + 0.03f}
                };
                AppendBeveledFrontRibbon(mesh, surf, lamp, 0.014f, 0.010f, silverLip);
                AppendBeveledFrontRibbon(
                    mesh, surf,
                    {{s * 0.45f, gTop - 0.04f, gTop - 0.015f}, {s * 0.85f, gTop - 0.01f, gTop + 0.02f}},
                    0.022f, 0.006f, drlCol
                );
                AppendSurfaceDisc(mesh, surf, s * 0.54f, (gBot + gTop) * 0.5f, 0.032f, 0.038f, 0.022f, 0.006f, 12, beamCol);
                AppendSurfaceDisc(mesh, surf, s * 0.67f, (gBot + gTop) * 0.5f + 0.008f, 0.032f, 0.038f, 0.022f, 0.006f, 12, beamCol);
                AppendSurfaceDisc(mesh, surf, s * 0.79f, (gBot + gTop) * 0.5f + 0.015f, 0.026f, 0.032f, 0.022f, 0.006f, 10, amberCol);

                AppendSurfaceRing(mesh, surf, s * 0.66f, skirtBotY + 0.18f, 0.032f, 0.038f, 0.050f, 0.058f, 0.014f, 0.010f, 12, darkTrim);
                AppendSurfaceDisc(mesh, surf, s * 0.66f, skirtBotY + 0.18f, 0.031f, 0.037f, 0.018f, 0.008f, 12, fogCol);
            }
            break;
        }

        case CoachDesignStyle::VOLVO_9700_DD:
        case CoachDesignStyle::VOLVO_9800_DD: {
            bool is9800DD = (style == CoachDesignStyle::VOLVO_9800_DD);
            if (is9800DD) {
                // 9800DD: 3 tight center amber pills + 2 outer brow pills + side amber dome corner markers
                AppendPillDotMarkersOnDome(mesh, surf, {-0.14f, 0.0f, 0.14f}, 0.44f, 0.56f, amberCol);
                AppendPillDotMarkersOnDome(mesh, surf, {-0.78f, 0.78f}, 0.44f, 0.56f, amberCol);
                for (int side : {-1, 1}) {
                    Vec3 cMark = surf.EvalDomePos(static_cast<float>(side) * 0.96f, 0.62f);
                    RunningGear::AppendBox(
                        mesh,
                        Vec3(cMark.x - 0.025f, cMark.y - 0.016f, side > 0 ? cMark.z : cMark.z - 0.018f),
                        Vec3(cMark.x + 0.035f, cMark.y + 0.016f, side > 0 ? cMark.z + 0.018f : cMark.z),
                        amberCol
                    );
                }
            } else {
                // 9700DD: twin flush silver-white outer brow sensor/marker pods
                AppendPillDotMarkersOnDome(mesh, surf, {-0.72f, 0.72f}, 0.44f, 0.58f, drlCol);
            }

            // Smooth curved mid-deck beam separating proud lower windshield and swept upper windshield
            float midBot = skirtBotY + 2.02f;
            float midTop = midBot + 0.28f;
            AppendCurvedMidDeckBeam(mesh, surf, midBot, midTop, glossBlack);
            if (!is9800DD) {
                AppendSculptedSlattedGrille(mesh, surf, 0.34f, 0.34f, midBot + 0.08f, midBot + 0.17f, 2, false, darkTrim, darkTrim, glossBlack);
            } else {
                // 9800DD iconic single wide driver-side upper deck wiper arm
                AppendBeveledFrontRibbon(
                    mesh, surf,
                    {{-0.58f, midTop + 0.025f, midTop + 0.060f},
                     {-0.26f, midTop + 0.040f, midTop + 0.075f},
                     { 0.06f, midTop + 0.015f, midTop + 0.048f}},
                    0.020f, 0.010f, glossBlack
                );
            }

            // Bowed black V-collar directly under lower windshield + full-width chrome brow bar + circular Volvo diagonal slash badge
            float collarBot = skirtBotY + 0.50f;
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.86f, noseTopY - 0.08f, noseTopY + 0.01f},
                 {-0.52f, collarBot + 0.02f, noseTopY + 0.01f},
                 { 0.0f,  collarBot - 0.02f, noseTopY + 0.01f},
                 { 0.52f, collarBot + 0.02f, noseTopY + 0.01f},
                 { 0.86f, noseTopY - 0.08f, noseTopY + 0.01f}},
                0.010f, 0.014f, glossBlack
            );
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.84f, noseTopY - 0.05f, noseTopY - 0.02f}, {0.0f, noseTopY - 0.06f, noseTopY - 0.03f}, {0.84f, noseTopY - 0.05f, noseTopY - 0.02f}},
                0.018f, 0.008f, chromeCol
            );
            AppendSculptedMedallion(mesh, surf, (collarBot + noseTopY - 0.03f) * 0.52f, 0.082f, chromeCol, glossBlack, true);

            // Clean flush recessed black trapezoidal lower bumper air intake (no heavy silver rectangular border!)
            AppendSculptedSlattedGrille(mesh, surf, 0.44f, 0.40f, skirtBotY + 0.11f, skirtBotY + 0.34f, 2, false, darkTrim, glossBlack, darkTrim);

            // Vertical V-fang Volvo LED headlamps + sleek black triangular corner fog scoops
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                std::vector<RibbonSample> vPod = {
                    {s * 0.58f, skirtBotY + 0.20f, skirtBotY + 0.44f},
                    {s * 0.74f, skirtBotY + 0.21f, skirtBotY + 0.52f},
                    {s * 0.86f, skirtBotY + 0.24f, skirtBotY + 0.55f}
                };
                AppendBeveledFrontRibbon(mesh, surf, vPod, 0.015f, 0.012f, glossBlack);
                // Signature white outer vertical + angled lower V-hook DRL light pipe
                AppendBeveledFrontRibbon(mesh, surf, {{s * 0.82f, skirtBotY + 0.24f, skirtBotY + 0.53f}, {s * 0.855f, skirtBotY + 0.25f, skirtBotY + 0.54f}}, 0.023f, 0.008f, drlCol);
                AppendBeveledFrontRibbon(mesh, surf, {{s * 0.60f, skirtBotY + 0.205f, skirtBotY + 0.235f}, {s * 0.85f, skirtBotY + 0.24f, skirtBotY + 0.27f}}, 0.023f, 0.008f, drlCol);
                // Stacked twin dark crystal projectors with chrome bezels
                AppendSurfaceRing(mesh, surf, s * 0.72f, skirtBotY + 0.32f, 0.025f, 0.030f, 0.036f, 0.042f, 0.022f, 0.006f, 12, silverLip);
                AppendSurfaceDisc(mesh, surf, s * 0.72f, skirtBotY + 0.32f, 0.024f, 0.029f, 0.023f, 0.006f, 12, beamCol);
                AppendSurfaceRing(mesh, surf, s * 0.75f, skirtBotY + 0.45f, 0.025f, 0.030f, 0.036f, 0.042f, 0.022f, 0.006f, 12, silverLip);
                AppendSurfaceDisc(mesh, surf, s * 0.75f, skirtBotY + 0.45f, 0.024f, 0.029f, 0.023f, 0.006f, 12, beamCol);

                // Clean low-slung horizontal corner aero fog pocket + round fog lens
                AppendBeveledFrontRibbon(mesh, surf, {{s * 0.62f, skirtBotY + 0.04f, skirtBotY + 0.12f}, {s * 0.82f, skirtBotY + 0.06f, skirtBotY + 0.15f}}, 0.013f, 0.012f, glossBlack);
                AppendSurfaceRing(mesh, surf, s * 0.74f, skirtBotY + 0.095f, 0.024f, 0.026f, 0.035f, 0.038f, 0.018f, 0.006f, 12, silverLip);
                AppendSurfaceDisc(mesh, surf, s * 0.74f, skirtBotY + 0.095f, 0.023f, 0.025f, 0.020f, 0.006f, 12, fogCol);
            }
            break;
        }

        case CoachDesignStyle::VOLVO_9700_TEAL:
        case CoachDesignStyle::VOLVO_9800_AERO: {
            bool is9800Aero = (style == CoachDesignStyle::VOLVO_9800_AERO);
            if (is9800Aero) {
                // 3 center + 2 corner DOT amber brow pills on Periwinkle Aero dome
                AppendPillDotMarkersOnDome(mesh, surf, {-0.14f, 0.0f, 0.14f}, 0.14f, 0.26f, amberCol);
                AppendPillDotMarkersOnDome(mesh, surf, {-0.76f, 0.76f}, 0.14f, 0.26f, amberCol);
            }

            float gBot = is9800Aero ? (noseTopY - 0.11f) : (skirtBotY + 0.48f);
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.84f, noseTopY - 0.08f, noseTopY + 0.01f},
                 {-0.46f, gBot,            noseTopY + 0.01f},
                 { 0.0f,  gBot - 0.03f,    noseTopY + 0.01f},
                 { 0.46f, gBot,            noseTopY + 0.01f},
                 { 0.84f, noseTopY - 0.08f, noseTopY + 0.01f}},
                0.010f, 0.014f, glossBlack
            );
            if (!is9800Aero) {
                AppendBeveledFrontRibbon(
                    mesh, surf,
                    {{-0.82f, noseTopY - 0.06f, noseTopY - 0.025f}, {0.0f, noseTopY - 0.07f, noseTopY - 0.035f}, {0.82f, noseTopY - 0.06f, noseTopY - 0.025f}},
                    0.018f, 0.008f, chromeCol
                );
                AppendSculptedMedallion(mesh, surf, (gBot + noseTopY - 0.03f) * 0.5f, 0.080f, chromeCol, glossBlack, true);
            } else {
                // Clean painted nose medallion + V-crease shadow on 9800 Aero
                AppendSculptedMedallion(mesh, surf, skirtBotY + 0.62f, 0.082f, chromeCol, bodyPaint, true);
            }

            AppendSculptedSlattedGrille(mesh, surf, 0.36f, 0.32f, skirtBotY + 0.10f, skirtBotY + 0.30f, 2, false, darkTrim, darkTrim, glossBlack);

            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                std::vector<RibbonSample> vPod = {
                    {s * 0.58f, skirtBotY + 0.22f, skirtBotY + 0.44f},
                    {s * 0.74f, skirtBotY + 0.22f, skirtBotY + 0.52f},
                    {s * 0.85f, skirtBotY + 0.25f, skirtBotY + 0.56f}
                };
                AppendBeveledFrontRibbon(mesh, surf, vPod, 0.016f, 0.012f, glossBlack);
                AppendBeveledFrontRibbon(mesh, surf, {{s * 0.81f, skirtBotY + 0.25f, skirtBotY + 0.54f}, {s * 0.85f, skirtBotY + 0.26f, skirtBotY + 0.55f}}, 0.023f, 0.008f, drlCol);
                AppendBeveledFrontRibbon(mesh, surf, {{s * 0.59f, skirtBotY + 0.225f, skirtBotY + 0.255f}, {s * 0.84f, skirtBotY + 0.25f, skirtBotY + 0.28f}}, 0.023f, 0.008f, drlCol);
                AppendSurfaceRing(mesh, surf, s * 0.71f, skirtBotY + 0.33f, 0.025f, 0.030f, 0.036f, 0.042f, 0.022f, 0.006f, 12, silverLip);
                AppendSurfaceDisc(mesh, surf, s * 0.71f, skirtBotY + 0.33f, 0.024f, 0.029f, 0.023f, 0.006f, 12, beamCol);
                AppendSurfaceRing(mesh, surf, s * 0.74f, skirtBotY + 0.45f, 0.025f, 0.030f, 0.036f, 0.042f, 0.022f, 0.006f, 12, silverLip);
                AppendSurfaceDisc(mesh, surf, s * 0.74f, skirtBotY + 0.45f, 0.024f, 0.029f, 0.023f, 0.006f, 12, beamCol);

                AppendBeveledFrontRibbon(mesh, surf, {{s * 0.62f, skirtBotY + 0.05f, skirtBotY + 0.13f}, {s * 0.82f, skirtBotY + 0.07f, skirtBotY + 0.16f}}, 0.014f, 0.010f, glossBlack);
                AppendSurfaceDisc(mesh, surf, s * 0.73f, skirtBotY + 0.10f, 0.024f, 0.028f, 0.021f, 0.008f, 12, fogCol);
            }
            break;
        }

        case CoachDesignStyle::YUTONG_TOURING: {
            // Sweeping black apron + twin continuous curved chrome U-whisker smile bars + wordmark chrome bar
            AppendFrontSurfacePatch(mesh, surf, -0.84f, 0.84f, noseTopY - 0.22f, noseTopY + 0.01f, 14, 2, 0.009f, glossBlack);
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.32f, noseTopY - 0.15f, noseTopY - 0.09f}, {0.32f, noseTopY - 0.15f, noseTopY - 0.09f}},
                0.016f, 0.008f, chromeCol
            );
            // Twin sweeping chrome smile whiskers curving cleanly UP into the swept headlights
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.58f, skirtBotY + 0.36f, skirtBotY + 0.395f},
                 {-0.26f, skirtBotY + 0.30f, skirtBotY + 0.335f},
                 { 0.0f,  skirtBotY + 0.285f,skirtBotY + 0.320f},
                 { 0.26f, skirtBotY + 0.30f, skirtBotY + 0.335f},
                 { 0.58f, skirtBotY + 0.36f, skirtBotY + 0.395f}},
                0.018f, 0.008f, chromeCol
            );
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.48f, skirtBotY + 0.24f, skirtBotY + 0.27f},
                 { 0.0f,  skirtBotY + 0.21f, skirtBotY + 0.24f},
                 { 0.48f, skirtBotY + 0.24f, skirtBotY + 0.27f}},
                0.016f, 0.008f, chromeCol
            );
            AppendSculptedSlattedGrille(mesh, surf, 0.36f, 0.42f, skirtBotY + 0.08f, skirtBotY + 0.22f, 2, false, darkTrim, darkTrim, glossBlack);
            AppendBeveledFrontRibbon(mesh, surf, {{-0.18f, skirtBotY + 0.11f, skirtBotY + 0.20f}, {0.18f, skirtBotY + 0.11f, skirtBotY + 0.20f}}, 0.017f, 0.006f, plateWht);

            // Upward-sweeping crescent eagle-wing crystal headlamps + 3 round projectors + curved LED eyebrow
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                std::vector<RibbonSample> wing = {
                    {s * 0.42f, skirtBotY + 0.31f, skirtBotY + 0.42f},
                    {s * 0.58f, skirtBotY + 0.36f, skirtBotY + 0.49f},
                    {s * 0.74f, skirtBotY + 0.43f, skirtBotY + 0.58f},
                    {s * 0.87f, skirtBotY + 0.51f, skirtBotY + 0.64f}
                };
                AppendBeveledFrontRibbon(mesh, surf, wing, 0.015f, 0.010f, silverLip);
                std::vector<RibbonSample> drl = {
                    {s * 0.43f, skirtBotY + 0.315f, skirtBotY + 0.340f},
                    {s * 0.62f, skirtBotY + 0.365f, skirtBotY + 0.390f},
                    {s * 0.85f, skirtBotY + 0.510f, skirtBotY + 0.535f}
                };
                AppendBeveledFrontRibbon(mesh, surf, drl, 0.022f, 0.006f, drlCol);
                for (int p = 0; p < 3; ++p) {
                    float t = static_cast<float>(p) / 2.0f;
                    AppendSurfaceDisc(mesh, surf, s * (0.50f + 0.27f * t), skirtBotY + 0.38f + 0.16f * t, 0.026f, 0.030f, 0.022f, 0.006f, 12, beamCol);
                }
                AppendSurfaceRing(mesh, surf, s * 0.68f, skirtBotY + 0.16f, 0.030f, 0.034f, 0.048f, 0.054f, 0.015f, 0.010f, 12, chromeCol);
                AppendSurfaceDisc(mesh, surf, s * 0.68f, skirtBotY + 0.16f, 0.029f, 0.033f, 0.018f, 0.008f, 12, fogCol);
            }
            break;
        }

        case CoachDesignStyle::AERO_BLADE_VIP: {
            // Iconic center recessed black brow + glowing amber horizontal & swept diagonal LED light blades
            AppendBeveledDomeRibbon(
                mesh, surf,
                {{-0.48f, 0.08f, 0.82f}, {-0.40f, 0.06f, 0.88f}, {0.0f, 0.05f, 0.90f}, {0.40f, 0.06f, 0.88f}, {0.48f, 0.08f, 0.82f}},
                0.010f, 0.010f, glossBlack
            );
            AppendBeveledDomeRibbon(
                mesh, surf,
                {{-0.28f, 0.16f, 0.24f}, {0.0f, 0.15f, 0.23f}, {0.28f, 0.16f, 0.24f}},
                0.017f, 0.008f, amberCol
            );
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                // Swept diagonal amber shoulder blade + outer horizontal brow bar
                AppendBeveledDomeRibbon(
                    mesh, surf,
                    {{s * 0.44f, 0.12f, 0.20f}, {s * 0.56f, 0.46f, 0.55f}, {s * 0.62f, 0.74f, 0.82f}},
                    0.017f, 0.008f, amberCol
                );
                AppendBeveledDomeRibbon(
                    mesh, surf,
                    {{s * 0.60f, 0.06f, 0.14f}, {s * 0.82f, 0.06f, 0.14f}},
                    0.016f, 0.008f, amberCol
                );
            }

            // Dual stacked glossy-black honeycomb trapezoidal grilles + X-frame metallic silver bridge + amber splitter underline
            AppendSculptedSlattedGrille(mesh, surf, 0.42f, 0.48f, skirtBotY + 0.44f, skirtBotY + 0.68f, 3, true, silverLip, glossBlack, darkTrim);
            AppendSculptedSlattedGrille(mesh, surf, 0.38f, 0.44f, skirtBotY + 0.07f, skirtBotY + 0.30f, 2, true, silverLip, glossBlack, darkTrim);
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.40f, skirtBotY + 0.045f, skirtBotY + 0.075f}, {0.0f, skirtBotY + 0.04f, skirtBotY + 0.07f}, {0.40f, skirtBotY + 0.045f, skirtBotY + 0.075f}},
                0.019f, 0.008f, amberCol
            );

            // Sharp inward-tapering triple-LED claw headlamps + vertical corner glowing amber air-curtain cheek blades + round fogs
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                std::vector<RibbonSample> claw = {
                    {s * 0.46f, skirtBotY + 0.50f, skirtBotY + 0.60f},
                    {s * 0.66f, skirtBotY + 0.53f, skirtBotY + 0.69f},
                    {s * 0.87f, skirtBotY + 0.57f, skirtBotY + 0.77f}
                };
                AppendBeveledFrontRibbon(mesh, surf, claw, 0.016f, 0.012f, glossBlack);
                std::vector<RibbonSample> clawDrl = {
                    {s * 0.47f, skirtBotY + 0.575f, skirtBotY + 0.605f},
                    {s * 0.66f, skirtBotY + 0.660f, skirtBotY + 0.690f},
                    {s * 0.86f, skirtBotY + 0.740f, skirtBotY + 0.770f}
                };
                AppendBeveledFrontRibbon(mesh, surf, clawDrl, 0.023f, 0.008f, drlCol);
                for (int p = 0; p < 3; ++p) {
                    float t = static_cast<float>(p) / 2.0f;
                    AppendSurfaceDisc(mesh, surf, s * (0.55f + 0.24f * t), skirtBotY + 0.56f + 0.08f * t, 0.025f, 0.030f, 0.023f, 0.006f, 10, beamCol);
                }
                // Lower corner intake scoop + glowing amber inner fin blade + round chrome fog
                std::vector<RibbonSample> scoop = {
                    {s * 0.48f, skirtBotY + 0.08f, skirtBotY + 0.24f},
                    {s * 0.68f, skirtBotY + 0.08f, skirtBotY + 0.34f},
                    {s * 0.84f, skirtBotY + 0.10f, skirtBotY + 0.40f}
                };
                AppendBeveledFrontRibbon(mesh, surf, scoop, 0.014f, 0.012f, glossBlack);
                std::vector<RibbonSample> amberBlade = {
                    {s * 0.49f, skirtBotY + 0.09f, skirtBotY + 0.12f},
                    {s * 0.56f, skirtBotY + 0.16f, skirtBotY + 0.32f},
                    {s * 0.59f, skirtBotY + 0.26f, skirtBotY + 0.36f}
                };
                AppendBeveledFrontRibbon(mesh, surf, amberBlade, 0.021f, 0.008f, amberCol);
                AppendSurfaceRing(mesh, surf, s * 0.73f, skirtBotY + 0.16f, 0.030f, 0.035f, 0.044f, 0.050f, 0.021f, 0.008f, 12, silverLip);
                AppendSurfaceDisc(mesh, surf, s * 0.73f, skirtBotY + 0.16f, 0.029f, 0.034f, 0.022f, 0.006f, 12, fogCol);
            }
            break;
        }

        case CoachDesignStyle::CHAMPAGNE_SR2: {
            // Deep curved anthracite collar + full-width chrome smile wing bar + round badge
            AppendFrontSurfacePatch(mesh, surf, -0.84f, 0.84f, noseTopY - 0.23f, noseTopY + 0.01f, 14, 2, 0.009f, darkTrim);
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.84f, noseTopY - 0.21f, noseTopY - 0.17f},
                 {-0.40f, noseTopY - 0.25f, noseTopY - 0.21f},
                 { 0.0f,  noseTopY - 0.26f, noseTopY - 0.22f},
                 { 0.40f, noseTopY - 0.25f, noseTopY - 0.21f},
                 { 0.84f, noseTopY - 0.21f, noseTopY - 0.17f}},
                0.017f, 0.008f, chromeCol
            );
            AppendSculptedMedallion(mesh, surf, noseTopY - 0.11f, 0.068f, chromeCol, darkTrim, false);
            AppendBeveledFrontRibbon(mesh, surf, {{-0.18f, skirtBotY + 0.10f, skirtBotY + 0.19f}, {0.18f, skirtBotY + 0.10f, skirtBotY + 0.19f}}, 0.016f, 0.008f, plateWht);

            // Sleek swept almond crystal headlamps + curved LED underline + slanted chrome fog pockets
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                std::vector<RibbonSample> lamp = {
                    {s * 0.42f, skirtBotY + 0.46f, skirtBotY + 0.59f},
                    {s * 0.64f, skirtBotY + 0.47f, skirtBotY + 0.63f},
                    {s * 0.85f, skirtBotY + 0.52f, skirtBotY + 0.66f}
                };
                AppendBeveledFrontRibbon(mesh, surf, lamp, 0.014f, 0.010f, darkTrim);
                AppendBeveledFrontRibbon(
                    mesh, surf,
                    {{s * 0.43f, skirtBotY + 0.465f, skirtBotY + 0.495f}, {s * 0.84f, skirtBotY + 0.525f, skirtBotY + 0.555f}},
                    0.021f, 0.006f, drlCol
                );
                AppendSurfaceRing(mesh, surf, s * 0.55f, skirtBotY + 0.55f, 0.026f, 0.032f, 0.038f, 0.044f, 0.021f, 0.006f, 12, silverLip);
                AppendSurfaceDisc(mesh, surf, s * 0.55f, skirtBotY + 0.55f, 0.025f, 0.031f, 0.022f, 0.006f, 12, beamCol);
                AppendSurfaceDisc(mesh, surf, s * 0.72f, skirtBotY + 0.58f, 0.026f, 0.032f, 0.022f, 0.006f, 12, beamCol);

                // Slanted silver-louvered lower fog pod
                AppendBeveledFrontRibbon(mesh, surf, {{s * 0.56f, skirtBotY + 0.12f, skirtBotY + 0.22f}, {s * 0.78f, skirtBotY + 0.14f, skirtBotY + 0.26f}}, 0.015f, 0.008f, silverLip);
                AppendSurfaceDisc(mesh, surf, s * 0.69f, skirtBotY + 0.19f, 0.028f, 0.032f, 0.021f, 0.006f, 12, fogCol);
            }
            break;
        }

        case CoachDesignStyle::NEOPLAN_TOURLINER: {
            // Neoplan sharp tapered V-shield black collar + twin sculpted horizontal radiator mouths
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.78f, noseTopY - 0.14f, noseTopY + 0.01f},
                 {-0.42f, skirtBotY + 0.52f, noseTopY + 0.01f},
                 { 0.0f,  skirtBotY + 0.48f, noseTopY + 0.01f},
                 { 0.42f, skirtBotY + 0.52f, noseTopY + 0.01f},
                 { 0.78f, noseTopY - 0.14f, noseTopY + 0.01f}},
                0.009f, 0.014f, glossBlack
            );
            AppendSculptedSlattedGrille(mesh, surf, 0.34f, 0.40f, skirtBotY + 0.25f, skirtBotY + 0.42f, 2, true, chromeCol, glossBlack, darkTrim);
            AppendSculptedSlattedGrille(mesh, surf, 0.30f, 0.34f, skirtBotY + 0.08f, skirtBotY + 0.20f, 1, false, darkTrim, darkTrim, glossBlack);
            AppendBeveledFrontRibbon(mesh, surf, {{-0.17f, skirtBotY + 0.10f, skirtBotY + 0.18f}, {0.17f, skirtBotY + 0.10f, skirtBotY + 0.18f}}, 0.017f, 0.006f, plateWht);

            // Sharp swept Neoplan "angel-eye" razor headlamps + angled eyebrow LEDs + chevron corner fogs
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                std::vector<RibbonSample> eye = {
                    {s * 0.42f, skirtBotY + 0.33f, skirtBotY + 0.44f},
                    {s * 0.62f, skirtBotY + 0.39f, skirtBotY + 0.51f},
                    {s * 0.86f, skirtBotY + 0.49f, skirtBotY + 0.60f}
                };
                AppendBeveledFrontRibbon(mesh, surf, eye, 0.015f, 0.012f, glossBlack);
                std::vector<RibbonSample> brow = {
                    {s * 0.43f, skirtBotY + 0.41f, skirtBotY + 0.44f},
                    {s * 0.62f, skirtBotY + 0.48f, skirtBotY + 0.51f},
                    {s * 0.85f, skirtBotY + 0.57f, skirtBotY + 0.60f}
                };
                AppendBeveledFrontRibbon(mesh, surf, brow, 0.022f, 0.006f, drlCol);
                AppendSurfaceDisc(mesh, surf, s * 0.55f, skirtBotY + 0.41f, 0.026f, 0.030f, 0.022f, 0.006f, 12, beamCol);
                AppendSurfaceDisc(mesh, surf, s * 0.72f, skirtBotY + 0.49f, 0.026f, 0.030f, 0.022f, 0.006f, 12, beamCol);

                AppendBeveledFrontRibbon(mesh, surf, {{s * 0.60f, skirtBotY + 0.09f, skirtBotY + 0.18f}, {s * 0.80f, skirtBotY + 0.12f, skirtBotY + 0.22f}}, 0.015f, 0.010f, darkTrim);
                AppendSurfaceDisc(mesh, surf, s * 0.71f, skirtBotY + 0.15f, 0.026f, 0.030f, 0.021f, 0.006f, 12, fogCol);
            }
            break;
        }

        case CoachDesignStyle::TWIN_GLASS_DOME: {
            // 3 centered blue crown LEDs & white corner markers on glossy black curved dome
            const Vec4 blueRoofLed(0.18f, 0.44f, 0.98f, 1.0f);
            AppendPillDotMarkersOnDome(mesh, surf, {-0.18f, 0.0f, 0.18f}, 0.56f, 0.70f, blueRoofLed);
            AppendPillDotMarkersOnDome(mesh, surf, {-0.76f, 0.76f}, 0.54f, 0.68f, drlCol);

            // Curved black mid-deck structural divider + recessed opaque dome backing
            float midBot = skirtBotY + 2.04f;
            float midTop = midBot + 0.24f;
            AppendCurvedMidDeckBeam(mesh, surf, midBot, midTop, glossBlack);
            RunningGear::AppendBox(
                mesh,
                Vec3(xFront + 0.72f, midTop + 0.02f, -hw + 0.30f),
                Vec3(xApillar + 0.24f, H - 0.26f, hw - 0.30f),
                glossBlack
            );

            // Smooth curved black shield + continuous parabolic chrome U-smile bar + centered emblem crest
            float uBot = skirtBotY + 0.44f;
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.78f, noseTopY - 0.10f, noseTopY + 0.01f},
                 {-0.46f, uBot + 0.03f,     noseTopY + 0.01f},
                 { 0.0f,  uBot,             noseTopY + 0.01f},
                 { 0.46f, uBot + 0.03f,     noseTopY + 0.01f},
                 { 0.78f, noseTopY - 0.10f, noseTopY + 0.01f}},
                0.010f, 0.014f, glossBlack
            );
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.48f, uBot + 0.02f, uBot + 0.055f},
                 {-0.22f, uBot - 0.01f, uBot + 0.025f},
                 { 0.0f,  uBot - 0.02f, uBot + 0.015f},
                 { 0.22f, uBot - 0.01f, uBot + 0.025f},
                 { 0.48f, uBot + 0.02f, uBot + 0.055f}},
                0.018f, 0.008f, chromeCol
            );
            AppendSculptedMedallion(mesh, surf, uBot + 0.18f, 0.064f, chromeCol, glossBlack, false);
            AppendBeveledFrontRibbon(mesh, surf, {{-0.18f, skirtBotY + 0.16f, skirtBotY + 0.25f}, {0.18f, skirtBotY + 0.16f, skirtBotY + 0.25f}}, 0.016f, 0.008f, plateWht);

            // Large leaf/teardrop swept crystal headlamps + twin chrome-ringed projectors + oval silver-ringed fogs
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                std::vector<RibbonSample> leaf = {
                    {s * 0.43f, skirtBotY + 0.33f, skirtBotY + 0.45f},
                    {s * 0.62f, skirtBotY + 0.34f, skirtBotY + 0.49f},
                    {s * 0.85f, skirtBotY + 0.39f, skirtBotY + 0.54f}
                };
                AppendBeveledFrontRibbon(mesh, surf, leaf, 0.015f, 0.010f, silverLip);
                AppendSurfaceRing(mesh, surf, s * 0.54f, skirtBotY + 0.41f, 0.024f, 0.028f, 0.036f, 0.042f, 0.021f, 0.006f, 12, glossBlack);
                AppendSurfaceDisc(mesh, surf, s * 0.54f, skirtBotY + 0.41f, 0.023f, 0.027f, 0.022f, 0.006f, 12, beamCol);
                AppendSurfaceRing(mesh, surf, s * 0.69f, skirtBotY + 0.43f, 0.024f, 0.028f, 0.036f, 0.042f, 0.021f, 0.006f, 12, glossBlack);
                AppendSurfaceDisc(mesh, surf, s * 0.69f, skirtBotY + 0.43f, 0.023f, 0.027f, 0.022f, 0.006f, 12, beamCol);
                AppendSurfaceDisc(mesh, surf, s * 0.80f, skirtBotY + 0.46f, 0.022f, 0.026f, 0.022f, 0.006f, 10, amberCol);

                AppendSurfaceRing(mesh, surf, s * 0.68f, skirtBotY + 0.16f, 0.032f, 0.028f, 0.048f, 0.042f, 0.015f, 0.010f, 12, silverLip);
                AppendSurfaceDisc(mesh, surf, s * 0.68f, skirtBotY + 0.16f, 0.031f, 0.027f, 0.018f, 0.008f, 12, fogCol);
            }
            break;
        }

        case CoachDesignStyle::OBSIDIAN_VIP_6X2:
        default: {
            // Signature Obsidian continuous bright chrome U-smile frame linking swept headlights + multi-slat gloss-black grille
            float gBot = skirtBotY + 0.36f;
            float gTop = noseTopY - 0.14f;
            AppendSculptedSlattedGrille(mesh, surf, 0.38f, 0.42f, gBot + 0.02f, gTop, 3, true, darkTrim, glossBlack, darkTrim);
            // Continuous sweeping chrome U-smile across bumper from left wing -> under grille -> right wing
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.84f, gBot + 0.07f, gBot + 0.105f},
                 {-0.48f, gBot + 0.00f, gBot + 0.035f},
                 {-0.26f, gBot - 0.02f, gBot + 0.015f},
                 { 0.0f,  gBot - 0.025f,gBot + 0.010f},
                 { 0.26f, gBot - 0.02f, gBot + 0.015f},
                 { 0.48f, gBot + 0.00f, gBot + 0.035f},
                 { 0.84f, gBot + 0.07f, gBot + 0.105f}},
                0.018f, 0.008f, chromeCol
            );
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.42f, gTop - 0.015f, gTop + 0.018f}, {0.0f, gTop - 0.025f, gTop + 0.008f}, {0.42f, gTop - 0.015f, gTop + 0.018f}},
                0.017f, 0.008f, chromeCol
            );
            AppendSculptedSlattedGrille(mesh, surf, 0.34f, 0.36f, skirtBotY + 0.08f, skirtBotY + 0.22f, 2, false, darkTrim, darkTrim, glossBlack);

            // Swept falcon-wing double-U halo LED headlamps + angled chrome-underlined corner fog pods
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                std::vector<RibbonSample> wing = {
                    {s * 0.43f, gBot + 0.03f, gTop + 0.01f},
                    {s * 0.64f, gBot + 0.05f, gTop + 0.05f},
                    {s * 0.86f, gBot + 0.10f, gTop + 0.10f}
                };
                AppendBeveledFrontRibbon(mesh, surf, wing, 0.015f, 0.012f, silverLip);
                // Glowing double-U white DRL contour along bottom & outer rim
                std::vector<RibbonSample> drl = {
                    {s * 0.44f, gBot + 0.035f, gBot + 0.062f},
                    {s * 0.65f, gBot + 0.055f, gBot + 0.082f},
                    {s * 0.85f, gBot + 0.105f, gBot + 0.132f}
                };
                AppendBeveledFrontRibbon(mesh, surf, drl, 0.023f, 0.006f, drlCol);
                AppendSurfaceDisc(mesh, surf, s * 0.56f, (gBot + gTop) * 0.5f + 0.02f, 0.028f, 0.034f, 0.022f, 0.006f, 12, beamCol);
                AppendSurfaceDisc(mesh, surf, s * 0.72f, (gBot + gTop) * 0.5f + 0.04f, 0.028f, 0.034f, 0.022f, 0.006f, 12, beamCol);

                AppendBeveledFrontRibbon(mesh, surf, {{s * 0.60f, skirtBotY + 0.11f, skirtBotY + 0.22f}, {s * 0.78f, skirtBotY + 0.13f, skirtBotY + 0.25f}}, 0.015f, 0.010f, silverLip);
                AppendSurfaceDisc(mesh, surf, s * 0.70f, skirtBotY + 0.18f, 0.028f, 0.032f, 0.021f, 0.006f, 12, fogCol);
            }
            break;
        }

        case CoachDesignStyle::EXECUTIVE_COACH_I6: {
            // 1. Contoured brow crown HVAC intake louvers + forward black shark-fin roof antenna & sensor caps
            AppendBeveledDomeRibbon(
                mesh, surf,
                {{-0.36f, 0.76f, 0.92f}, {0.0f, 0.74f, 0.90f}, {0.36f, 0.76f, 0.92f}},
                0.012f, 0.010f, glossBlack
            );
            {
                Vec3 antBase = surf.EvalDomePos(0.0f, 0.42f);
                RunningGear::AppendBox(
                    mesh,
                    Vec3(antBase.x - 0.06f, antBase.y - 0.01f, -0.022f),
                    Vec3(antBase.x + 0.08f, antBase.y + 0.11f,  0.022f),
                    glossBlack
                );
                AppendPillDotMarkersOnDome(mesh, surf, {-0.26f, 0.26f}, 0.44f, 0.52f, glossBlack);
            }

            // 2. Signature curved gloss-black upper V-mask directly beneath panoramic windshield + crest emblem
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.88f, noseTopY - 0.12f, noseTopY + 0.01f},
                 {-0.46f, skirtBotY + 0.55f, noseTopY + 0.01f},
                 { 0.0f,  skirtBotY + 0.36f, noseTopY + 0.01f},
                 { 0.46f, skirtBotY + 0.55f, noseTopY + 0.01f},
                 { 0.88f, noseTopY - 0.12f, noseTopY + 0.01f}},
                0.010f, 0.014f, glossBlack
            );
            // Centered silver crest wordmark bar + subtle olive-gold VIP underline
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.14f, noseTopY - 0.115f, noseTopY - 0.055f}, {0.14f, noseTopY - 0.115f, noseTopY - 0.055f}},
                0.018f, 0.008f, chromeCol
            );
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.18f, skirtBotY + 0.54f, skirtBotY + 0.58f}, {0.18f, skirtBotY + 0.54f, skirtBotY + 0.58f}},
                0.014f, 0.006f, Vec4(0.45f, 0.50f, 0.24f, 1.0f)
            );

            // 3. Iconic full-width gleaming chrome V-chevron smile framing the black mask & sweeping out to corners
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.94f, noseTopY - 0.08f, noseTopY - 0.03f},
                 {-0.76f, skirtBotY + 0.65f, skirtBotY + 0.70f},
                 {-0.42f, skirtBotY + 0.49f, skirtBotY + 0.54f},
                 { 0.00f, skirtBotY + 0.32f, skirtBotY + 0.37f},
                 { 0.42f, skirtBotY + 0.49f, skirtBotY + 0.54f},
                 { 0.76f, skirtBotY + 0.65f, skirtBotY + 0.70f},
                 { 0.94f, noseTopY - 0.08f, noseTopY - 0.03f}},
                0.019f, 0.008f, chromeCol
            );
            // Recessed center dark radar/tow square right below V-apex
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.075f, skirtBotY + 0.22f, skirtBotY + 0.30f}, {0.075f, skirtBotY + 0.22f, skirtBotY + 0.30f}},
                0.013f, 0.012f, glossBlack
            );

            // 4. Central wide white Euro license plate with blue EU left strip + slim chin intake slit
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.22f, skirtBotY + 0.105f, skirtBotY + 0.195f}, {0.22f, skirtBotY + 0.105f, skirtBotY + 0.195f}},
                0.016f, 0.008f, plateWht
            );
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.22f, skirtBotY + 0.105f, skirtBotY + 0.195f}, {-0.17f, skirtBotY + 0.105f, skirtBotY + 0.195f}},
                0.018f, 0.006f, Vec4(0.08f, 0.26f, 0.72f, 1.0f)
            );
            AppendSculptedSlattedGrille(mesh, surf, 0.46f, 0.42f, skirtBotY + 0.025f, skirtBotY + 0.080f, 1, false, darkTrim, glossBlack, darkTrim);

            // 5. Swept-back angular triangulated headlamps + crisp DRL wing outline + chrome-ringed corner fog pockets
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                std::vector<RibbonSample> wingHousing = {
                    {s * 0.40f, skirtBotY + 0.36f, skirtBotY + 0.45f},
                    {s * 0.62f, skirtBotY + 0.39f, skirtBotY + 0.51f},
                    {s * 0.88f, skirtBotY + 0.46f, skirtBotY + 0.58f}
                };
                AppendBeveledFrontRibbon(mesh, surf, wingHousing, 0.015f, 0.012f, glossBlack);

                // Crisp chrome/silver outer wing frame & bright continuous lower/outer LED DRL signature
                std::vector<RibbonSample> drlBlade = {
                    {s * 0.41f, skirtBotY + 0.365f, skirtBotY + 0.395f},
                    {s * 0.62f, skirtBotY + 0.395f, skirtBotY + 0.425f},
                    {s * 0.87f, skirtBotY + 0.465f, skirtBotY + 0.495f}
                };
                AppendBeveledFrontRibbon(mesh, surf, drlBlade, 0.023f, 0.007f, drlCol);
                AppendBeveledFrontRibbon(
                    mesh, surf,
                    {{s * 0.41f, skirtBotY + 0.430f, skirtBotY + 0.455f},
                     {s * 0.62f, skirtBotY + 0.485f, skirtBotY + 0.510f},
                     {s * 0.87f, skirtBotY + 0.555f, skirtBotY + 0.580f}},
                    0.021f, 0.006f, silverLip
                );

                // Dual circular crystal projector lenses + outer corner amber indicator
                AppendSurfaceRing(mesh, surf, s * 0.53f, skirtBotY + 0.42f, 0.024f, 0.028f, 0.035f, 0.040f, 0.022f, 0.006f, 12, silverLip);
                AppendSurfaceDisc(mesh, surf, s * 0.53f, skirtBotY + 0.42f, 0.023f, 0.027f, 0.023f, 0.006f, 12, beamCol);
                AppendSurfaceRing(mesh, surf, s * 0.68f, skirtBotY + 0.46f, 0.024f, 0.028f, 0.035f, 0.040f, 0.022f, 0.006f, 12, silverLip);
                AppendSurfaceDisc(mesh, surf, s * 0.68f, skirtBotY + 0.46f, 0.023f, 0.027f, 0.023f, 0.006f, 12, beamCol);
                AppendSurfaceDisc(mesh, surf, s * 0.81f, skirtBotY + 0.51f, 0.022f, 0.026f, 0.023f, 0.006f, 10, amberCol);

                // Deep dark outer lower corner fog pocket + chrome-ringed circular fog lamp
                AppendBeveledFrontRibbon(
                    mesh, surf,
                    {{s * 0.63f, skirtBotY + 0.08f, skirtBotY + 0.21f},
                     {s * 0.88f, skirtBotY + 0.10f, skirtBotY + 0.23f}},
                    0.013f, 0.014f, glossBlack
                );
                AppendSurfaceRing(mesh, surf, s * 0.71f, skirtBotY + 0.15f, 0.026f, 0.030f, 0.040f, 0.045f, 0.018f, 0.008f, 12, chromeCol);
                AppendSurfaceDisc(mesh, surf, s * 0.71f, skirtBotY + 0.15f, 0.025f, 0.029f, 0.020f, 0.008f, 12, fogCol);
            }
            break;
        }
    }
}

void AppendCoachCurvedWindshields(
    Mesh* mesh,
    CoachDesignStyle style,
    float xFront,
    float xApillar,
    float skirtBotY,
    float noseTopY,
    float windshieldTopY,
    float winSideTop,
    float H,
    float hw,
    const Vec4& glassColor
) {
    if (!mesh) return;
    bool isDD = (style == CoachDesignStyle::VOLVO_9700_DD || style == CoachDesignStyle::VOLVO_9800_DD);
    FrontSurface surf;
    surf.Init(xFront, xApillar, skirtBotY, noseTopY, windshieldTopY, winSideTop, H, hw, isDD);

    bool isTwinDome = (style == CoachDesignStyle::TWIN_GLASS_DOME);

    if (isDD || isTwinDome) {
        float midBot = isTwinDome ? (skirtBotY + 2.04f) : (skirtBotY + 2.02f);
        float midTop = isTwinDome ? (midBot + 0.24f) : (midBot + 0.28f);
        AppendFrontSurfacePatch(mesh, surf, -0.86f, 0.86f, noseTopY, midBot, 16, 4, -0.005f, glassColor);
        AppendFrontSurfacePatch(mesh, surf, -0.86f, 0.86f, midTop, windshieldTopY, 16, 5, -0.005f, glassColor);
    } else {
        AppendFrontSurfacePatch(mesh, surf, -0.86f, 0.86f, noseTopY, windshieldTopY, 16, 6, -0.005f, glassColor);
    }
}

void AppendCoachRoofCanopySmooth(
    Mesh* mesh,
    float xFront,
    float winEndX,
    float xRear,
    float winSideTop,
    float H,
    float hw,
    const Vec4& roofColor,
    bool isDoubleDeck
) {
    if (!mesh) return;
    FrontSurface surf;
    surf.Init(xFront, xFront + 0.62f, 0.28f, 1.32f, winSideTop + 0.04f, winSideTop, H, hw, isDoubleDeck);

    const float xStart = surf.xRoofJoin; // Exact vertex seam matching AppendSmoothRoofDome!
    const int nu = 22;
    struct XStation {
        float x;
        float topDrop;
    };
    std::vector<XStation> stations = {
        {xStart,       0.0f},
        {xFront + 2.1f,0.0f},
        {winEndX,      0.0f},
        {xRear - 0.10f,0.04f},
        {xRear,        0.09f}
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
            // (p0a, p0b, p1b, p1a) -> (p0b - p0a) x (p1b - p0a) = (+Z) x (+X) = +Y outward!
            CabComponents::AppendQuad(mesh, p0a, p0b, p1b, p1a, roofColor);
        }
    }

    // Rear tail roof arch closure + subtle bottom sill return
    Vec3 rearCenter(xRear, winSideTop + 0.04f, 0.0f);
    for (int i = 0; i < nu; ++i) {
        float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
        float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
        Vec3 ra = surf.EvalRoofArchPos(xRear, ua, 0.09f);
        Vec3 rb = surf.EvalRoofArchPos(xRear, ub, 0.09f);
        uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
        Vertex v0; v0.position = rearCenter; v0.color = roofColor;
        Vertex v1; v1.position = rb;         v1.color = roofColor;
        Vertex v2; v2.position = ra;         v2.color = roofColor;
        mesh->vertices.push_back(v0);
        mesh->vertices.push_back(v1);
        mesh->vertices.push_back(v2);
        mesh->indices.push_back(base + 0);
        mesh->indices.push_back(base + 1);
        mesh->indices.push_back(base + 2);
    }
}

void AppendCoachMirrorsForStyle(
    Mesh* mesh,
    CoachDesignStyle style,
    float xFront,
    float windshieldTopY,
    float H,
    float hw,
    const Vec4& bodyPaint,
    const Vec4& darkTrim
) {
    if (!mesh) return;
    Vec4 glassMirror(0.58f, 0.68f, 0.80f, 1.0f);
    Vec4 whiteMirror(0.94f, 0.95f, 0.97f, 1.0f);
    Vec4 amberRepeat(0.98f, 0.64f, 0.10f, 1.0f);

    float pivotX = xFront + 0.64f;

    switch (style) {
        case CoachDesignStyle::UNVI_HIGH_DECKER:
        case CoachDesignStyle::TWIN_GLASS_DOME: {
            float armY = (style == CoachDesignStyle::TWIN_GLASS_DOME) ? (H - 1.38f) : (windshieldTopY - 0.22f);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                float zIn  = s * (hw - 0.04f);
                float zOut = s * (hw + 0.28f);
                float zMin = std::min(zIn, zOut), zMax = std::max(zIn, zOut);
                RunningGear::AppendBox(mesh, Vec3(pivotX - 0.42f, armY + 0.03f, zMin), Vec3(pivotX + 0.04f, armY + 0.10f, zMax), whiteMirror);
                float dropX = pivotX - 0.40f;
                float hz0 = s > 0 ? zOut - 0.04f : zOut - 0.11f;
                float hz1 = s > 0 ? zOut + 0.11f : zOut + 0.04f;
                RunningGear::AppendBox(mesh, Vec3(dropX - 0.06f, armY - 0.40f, hz0), Vec3(dropX + 0.06f, armY + 0.06f, hz1), whiteMirror);
                RunningGear::AppendBox(mesh, Vec3(dropX + 0.055f, armY - 0.37f, hz0 + 0.015f), Vec3(dropX + 0.075f, armY + 0.03f, hz1 - 0.015f), glassMirror);
            }
            if (style == CoachDesignStyle::UNVI_HIGH_DECKER) {
                RunningGear::AppendBox(mesh, Vec3(pivotX - 0.06f, windshieldTopY - 1.15f, -hw - 0.14f), Vec3(pivotX + 0.05f, windshieldTopY - 0.85f, -hw - 0.02f), darkTrim);
            }
            break;
        }

        case CoachDesignStyle::AERO_BLADE_VIP:
        case CoachDesignStyle::CHAMPAGNE_SR2: {
            float armY = windshieldTopY - 0.18f;
            const Vec4 shellCol = (style == CoachDesignStyle::AERO_BLADE_VIP) ? Vec4(0.78f, 0.80f, 0.84f, 1.0f) : bodyPaint;
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                float zIn  = s * (hw - 0.04f);
                float zOut = s * (hw + 0.27f);
                float zMin = std::min(zIn, zOut), zMax = std::max(zIn, zOut);
                RunningGear::AppendBox(mesh, Vec3(pivotX - 0.38f, armY + 0.04f, zMin), Vec3(pivotX + 0.04f, armY + 0.12f, zMax), shellCol);
                float dropX = pivotX - 0.36f;
                float hz0 = s > 0 ? zOut - 0.05f : zOut - 0.12f;
                float hz1 = s > 0 ? zOut + 0.12f : zOut + 0.05f;
                RunningGear::AppendBox(mesh, Vec3(dropX - 0.07f, armY - 0.46f, hz0), Vec3(dropX + 0.06f, armY + 0.08f, hz1), shellCol);
                RunningGear::AppendBox(mesh, Vec3(dropX + 0.055f, armY - 0.43f, hz0 + 0.015f), Vec3(dropX + 0.075f, armY + 0.04f, hz1 - 0.015f), glassMirror);
                RunningGear::AppendBox(mesh, Vec3(dropX - 0.078f, armY - 0.22f, hz0 + 0.02f), Vec3(dropX - 0.05f, armY - 0.16f, hz1 - 0.02f), amberRepeat);
            }
            break;
        }

        case CoachDesignStyle::VOLVO_9900_THEATER:
        case CoachDesignStyle::NEOPLAN_TOURLINER:
        case CoachDesignStyle::OBSIDIAN_VIP_6X2: {
            float armY = windshieldTopY - 0.14f;
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                float zIn  = s * (hw - 0.04f);
                float zOut = s * (hw + 0.28f);
                float zMin = std::min(zIn, zOut), zMax = std::max(zIn, zOut);
                RunningGear::AppendBox(mesh, Vec3(pivotX - 0.45f, armY + 0.02f, zMin), Vec3(pivotX + 0.04f, armY + 0.08f, zMax), darkTrim);
                float dropX = pivotX - 0.43f;
                float hz0 = s > 0 ? zOut - 0.04f : zOut - 0.10f;
                float hz1 = s > 0 ? zOut + 0.10f : zOut + 0.04f;
                RunningGear::AppendBox(mesh, Vec3(dropX - 0.065f, armY - 0.44f, hz0), Vec3(dropX + 0.055f, armY + 0.04f, hz1), darkTrim);
                RunningGear::AppendBox(mesh, Vec3(dropX + 0.05f, armY - 0.41f, hz0 + 0.015f), Vec3(dropX + 0.07f, armY + 0.01f, hz1 - 0.015f), glassMirror);
            }
            break;
        }

        case CoachDesignStyle::EXECUTIVE_COACH_I6: {
            // Sweeping forward-arched body-colored gooseneck arms + tall sculpted mirror heads + lower -Z spotter mirror
            float armY = windshieldTopY - 0.08f;
            Vec4 chromeTrim(0.84f, 0.87f, 0.92f, 1.0f);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                float zIn  = s * (hw - 0.04f);
                float zOut = s * (hw + 0.28f);
                float zMid = s * (hw + 0.14f);
                // Smooth 3-segment curved gooseneck arch forward & outboard
                RunningGear::AppendBox(mesh, Vec3(pivotX - 0.22f, armY + 0.03f, std::min(zIn, zMid)), Vec3(pivotX + 0.05f, armY + 0.11f, std::max(zIn, zMid)), bodyPaint);
                RunningGear::AppendBox(mesh, Vec3(pivotX - 0.42f, armY + 0.00f, std::min(zMid, zOut)), Vec3(pivotX - 0.16f, armY + 0.09f, std::max(zMid, zOut)), bodyPaint);
                float dropX = pivotX - 0.44f;
                float hz0 = s > 0 ? zOut - 0.05f : zOut - 0.11f;
                float hz1 = s > 0 ? zOut + 0.11f : zOut + 0.05f;
                RunningGear::AppendBox(mesh, Vec3(dropX - 0.075f, armY - 0.44f, hz0), Vec3(dropX + 0.06f, armY + 0.06f, hz1), bodyPaint);
                RunningGear::AppendBox(mesh, Vec3(dropX + 0.055f, armY - 0.41f, hz0 + 0.015f), Vec3(dropX + 0.075f, armY + 0.02f, hz1 - 0.015f), glassMirror);
                RunningGear::AppendBox(mesh, Vec3(dropX - 0.082f, armY - 0.24f, hz0 + 0.02f), Vec3(dropX - 0.06f, armY - 0.20f, hz1 - 0.02f), chromeTrim);
            }
            // Extra lower vertical spotter mirror on the right/driver (-Z) A-pillar corner matching Irizar i6
            RunningGear::AppendBox(mesh, Vec3(pivotX - 0.06f, windshieldTopY - 1.38f, -hw - 0.13f), Vec3(pivotX + 0.06f, windshieldTopY - 0.96f, -hw - 0.01f), darkTrim);
            RunningGear::AppendBox(mesh, Vec3(pivotX + 0.055f, windshieldTopY - 1.35f, -hw - 0.115f), Vec3(pivotX + 0.072f, windshieldTopY - 0.99f, -hw - 0.025f), glassMirror);
            break;
        }

        case CoachDesignStyle::VOLVO_9700_DD:
        case CoachDesignStyle::VOLVO_9800_DD:
        case CoachDesignStyle::VOLVO_9700_TEAL:
        case CoachDesignStyle::VOLVO_9800_AERO:
        case CoachDesignStyle::VOLVO_BZR_ELECTRIC:
        case CoachDesignStyle::YUTONG_TOURING: {
            bool isDD = (style == CoachDesignStyle::VOLVO_9700_DD || style == CoachDesignStyle::VOLVO_9800_DD);
            bool is9800DD = (style == CoachDesignStyle::VOLVO_9800_DD);
            float podY = isDD ? (H - 1.56f) : (windshieldTopY - 0.18f);
            float armPivotX = isDD ? (xFront + 0.68f) : (pivotX + 0.04f);
            const Vec4 stalkCol = (style == CoachDesignStyle::VOLVO_9700_DD || style == CoachDesignStyle::YUTONG_TOURING)
                ? bodyPaint : darkTrim;
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                float zIn  = s * (hw - 0.02f);
                float zMid = s * (hw + 0.12f);
                float zOut = s * (hw + (isDD ? 0.22f : 0.20f));
                // Swept tapered cantilevered digital camera wing arm
                CabComponents::AppendQuad(
                    mesh,
                    Vec3(armPivotX - 0.05f, podY - 0.032f, zIn),
                    Vec3(armPivotX - 0.16f, podY - 0.022f, zMid),
                    Vec3(armPivotX - 0.14f, podY + 0.032f, zMid),
                    Vec3(armPivotX + 0.04f, podY + 0.040f, zIn),
                    stalkCol
                );
                CabComponents::AppendQuad(
                    mesh,
                    Vec3(armPivotX - 0.16f, podY - 0.022f, zMid),
                    Vec3(armPivotX - 0.22f, podY - 0.038f, zOut),
                    Vec3(armPivotX - 0.20f, podY + 0.018f, zOut),
                    Vec3(armPivotX - 0.14f, podY + 0.032f, zMid),
                    stalkCol
                );
                // Streamlined camera-mirror pod head (tapered frustum, zero boxy edges)
                RunningGear::AppendBox(
                    mesh,
                    Vec3(armPivotX - 0.24f, podY - 0.095f, s > 0 ? zOut - 0.04f : zOut - 0.065f),
                    Vec3(armPivotX - 0.13f, podY + 0.030f, s > 0 ? zOut + 0.065f : zOut + 0.04f),
                    darkTrim
                );
                if (is9800DD && side > 0) {
                    // Upper curbside close-proximity spotter camera/mirror pod matching Volvo 9800 DD photo
                    RunningGear::AppendBox(
                        mesh,
                        Vec3(armPivotX - 0.11f, podY + 0.20f, hw - 0.01f),
                        Vec3(armPivotX + 0.03f, podY + 0.28f, hw + 0.12f),
                        darkTrim
                    );
                }
            }
            break;
        }

        case CoachDesignStyle::VOLVO_9700_USCAN:
        default: {
            float armY = H - 0.44f;
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                float zIn  = s * (hw - 0.04f);
                float zOut = s * (hw + 0.30f);
                float zMin = std::min(zIn, zOut), zMax = std::max(zIn, zOut);
                RunningGear::AppendBox(mesh, Vec3(pivotX - 0.46f, armY + 0.02f, zMin), Vec3(pivotX + 0.04f, armY + 0.09f, zMax), whiteMirror);
                float dropX = pivotX - 0.44f;
                float hz0 = s > 0 ? zOut - 0.04f : zOut - 0.11f;
                float hz1 = s > 0 ? zOut + 0.11f : zOut + 0.04f;
                RunningGear::AppendBox(mesh, Vec3(dropX - 0.065f, armY - 0.46f, hz0), Vec3(dropX + 0.06f, armY + 0.06f, hz1), whiteMirror);
                RunningGear::AppendBox(mesh, Vec3(dropX + 0.055f, armY - 0.43f, hz0 + 0.015f), Vec3(dropX + 0.075f, armY + 0.03f, hz1 - 0.015f), glassMirror);
            }
            break;
        }
    }
}

namespace {

struct RearSurface {
    float xRear = 0.0f;
    float skirtBotY = 0.28f;
    float winSideTop = 3.34f;
    float H = 3.80f;
    float hwRear = 1.225f;

    void Init(float inXRear, float inSkirtBotY, float inWinSideTop, float inH, float inHw) {
        xRear = inXRear;
        skirtBotY = inSkirtBotY;
        winSideTop = inWinSideTop;
        H = inH;
        hwRear = std::max(0.8f, inHw - 0.05f);
    }

    Vec3 EvalRearPos(float u, float y) const {
        float uc = std::clamp(u, -1.0f, 1.0f);
        float z = uc * (hwRear - 0.08f);
        float normZ = std::abs(uc);
        float curveX = 0.028f * (1.0f - normZ * normZ);
        float tY = std::clamp((y - skirtBotY) / std::max(0.3f, winSideTop - skirtBotY), 0.0f, 1.0f);
        float leanX = -0.020f * tY;
        return Vec3(xRear + curveX + leanX, y, z);
    }

    Vec3 EvalRearNormal(float u, float y) const {
        float du = 0.02f;
        float dy = 0.02f;
        Vec3 pu0 = EvalRearPos(u - du, y);
        Vec3 pu1 = EvalRearPos(u + du, y);
        Vec3 py0 = EvalRearPos(u, y - dy);
        Vec3 py1 = EvalRearPos(u, y + dy);
        Vec3 dU(pu1.x - pu0.x, pu1.y - pu0.y, pu1.z - pu0.z);
        Vec3 dY(py1.x - py0.x, py1.y - py0.y, py1.z - py0.z);
        return FrontSurface::SafeNormalize(Vec3(
            dY.y * dU.z - dY.z * dU.y,
            dY.z * dU.x - dY.x * dU.z,
            dY.x * dU.y - dY.y * dU.x
        ));
    }
};

void AppendRearPatch(
    Mesh* mesh,
    const RearSurface& surf,
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
            Vec3 a = OffsetPt(surf.EvalRearPos(ua, ya), surf.EvalRearNormal(ua, ya), offsetOut);
            Vec3 b = OffsetPt(surf.EvalRearPos(ub, ya), surf.EvalRearNormal(ub, ya), offsetOut);
            Vec3 c = OffsetPt(surf.EvalRearPos(ub, yb), surf.EvalRearNormal(ub, yb), offsetOut);
            Vec3 d = OffsetPt(surf.EvalRearPos(ua, yb), surf.EvalRearNormal(ua, yb), offsetOut);
            CabComponents::AppendQuad(mesh, b, a, d, c, col);
        }
    }
}

void AppendRearRibbon(
    Mesh* mesh,
    const RearSurface& surf,
    std::vector<RibbonSample> samples,
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
        Vec3 pb = surf.EvalRearPos(u, yb);
        Vec3 nb = surf.EvalRearNormal(u, yb);
        Vec3 pt = surf.EvalRearPos(u, yt);
        Vec3 nt = surf.EvalRearNormal(u, yt);
        fB[i] = OffsetPt(pb, nb, protrudeOut);
        bB[i] = OffsetPt(pb, nb, -depthIn);
        fT[i] = OffsetPt(pt, nt, protrudeOut);
        bT[i] = OffsetPt(pt, nt, -depthIn);
    }
    for (size_t i = 0; i + 1 < n; ++i) {
        CabComponents::AppendQuad(mesh, fB[i + 1], fB[i], fT[i], fT[i + 1], col);
        CabComponents::AppendQuad(mesh, fT[i + 1], fT[i], bT[i], bT[i + 1], col);
        CabComponents::AppendQuad(mesh, bB[i + 1], bB[i], fB[i], fB[i + 1], col);
    }
    CabComponents::AppendQuad(mesh, fB.front(), bB.front(), bT.front(), fT.front(), col);
    CabComponents::AppendQuad(mesh, bB.back(),  fB.back(),  fT.back(),  bT.back(),  col);
}

void AppendRearDisc(
    Mesh* mesh,
    const RearSurface& surf,
    float uC, float yC,
    float radU, float radY,
    float protrudeOut, float depthIn,
    int segs,
    const Vec4& col
) {
    if (!mesh || segs < 6) return;
    Vec3 pc = surf.EvalRearPos(uC, yC);
    Vec3 nc = surf.EvalRearNormal(uC, yC);
    Vec3 centerF = OffsetPt(pc, nc, protrudeOut + 0.003f);

    std::vector<Vec3> rimF(segs), rimB(segs);
    for (int k = 0; k < segs; ++k) {
        float a = 2.0f * kPi * (static_cast<float>(k) / static_cast<float>(segs));
        float uk = uC + radU * std::cos(a);
        float yk = yC + radY * std::sin(a);
        Vec3 pk = surf.EvalRearPos(uk, yk);
        Vec3 nk = surf.EvalRearNormal(uk, yk);
        rimF[k] = OffsetPt(pk, nk, protrudeOut);
        rimB[k] = OffsetPt(pk, nk, -depthIn);
    }
    for (int k = 0; k < segs; ++k) {
        int kn = (k + 1) % segs;
        uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
        Vertex v0; v0.position = centerF;  v0.color = col;
        Vertex v1; v1.position = rimF[kn]; v1.color = col;
        Vertex v2; v2.position = rimF[k];  v2.color = col;
        mesh->vertices.push_back(v0);
        mesh->vertices.push_back(v1);
        mesh->vertices.push_back(v2);
        mesh->indices.push_back(base + 0);
        mesh->indices.push_back(base + 1);
        mesh->indices.push_back(base + 2);
        CabComponents::AppendQuad(mesh, rimB[kn], rimB[k], rimF[k], rimF[kn], col);
    }
}

void AppendRearSlattedGrille(
    Mesh* mesh,
    const RearSurface& surf,
    float uHalf,
    float yBot, float yTop,
    int slats,
    const Vec4& surroundCol,
    const Vec4& backingCol,
    const Vec4& slatCol
) {
    AppendRearPatch(mesh, surf, -uHalf, uHalf, yBot, yTop, 6, 2, 0.008f, backingCol);
    AppendRearRibbon(mesh, surf, {{-uHalf, yTop - 0.015f, yTop + 0.015f}, {uHalf, yTop - 0.015f, yTop + 0.015f}}, 0.014f, 0.010f, surroundCol);
    AppendRearRibbon(mesh, surf, {{-uHalf, yBot - 0.015f, yBot + 0.015f}, {uHalf, yBot - 0.015f, yBot + 0.015f}}, 0.014f, 0.010f, surroundCol);
    for (int i = 1; i <= slats; ++i) {
        float sy = yBot + (yTop - yBot) * (static_cast<float>(i) / static_cast<float>(slats + 1));
        AppendRearRibbon(mesh, surf, {{-uHalf * 0.94f, sy - 0.009f, sy + 0.009f}, {uHalf * 0.94f, sy - 0.009f, sy + 0.009f}}, 0.013f, 0.008f, slatCol);
    }
}

} // namespace

void AppendCoachRearDesign(
    Mesh* mesh,
    CoachDesignStyle style,
    float xRear,
    float skirtBotY,
    float deckFloorY,
    float winSideTop,
    float H,
    float hw,
    bool headlightsOn,
    bool isDoubleDeck,
    const Vec4& bodyPaint,
    const Vec4& accent,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol
) {
    if (!mesh) return;

    RearSurface surf;
    surf.Init(xRear, skirtBotY, winSideTop, H, hw);

    const Vec4 tailRed   = headlightsOn ? Vec4(0.95f, 0.12f, 0.14f, 1.0f) : Vec4(0.55f, 0.10f, 0.12f, 1.0f);
    const Vec4 tailAmber = headlightsOn ? Vec4(1.00f, 0.62f, 0.08f, 1.0f) : Vec4(0.58f, 0.36f, 0.08f, 1.0f);
    const Vec4 revWhite  = headlightsOn ? Vec4(0.94f, 0.96f, 1.00f, 1.0f) : Vec4(0.62f, 0.65f, 0.70f, 1.0f);
    const Vec4 plateWht  = Vec4(0.94f, 0.95f, 0.97f, 1.0f);
    const Vec4 evCyan    = Vec4(0.16f, 0.84f, 0.88f, 1.0f);

    // Lower bumper valance + license plate recess (style-specific color)
    const Vec4 bumperCol = (style == CoachDesignStyle::CHAMPAGNE_SR2 || style == CoachDesignStyle::VOLVO_BZR_ELECTRIC)
        ? bodyPaint
        : ((style == CoachDesignStyle::VOLVO_9700_USCAN) ? chromeCol : darkTrim);
    AppendRearPatch(mesh, surf, -0.96f, 0.96f, skirtBotY + 0.10f, skirtBotY + 0.36f, 12, 2, 0.018f, bumperCol);
    AppendRearRibbon(mesh, surf, {{-0.18f, skirtBotY + 0.17f, skirtBotY + 0.27f}, {0.18f, skirtBotY + 0.17f, skirtBotY + 0.27f}}, 0.024f, 0.008f, plateWht);

    const float rearWinBot = deckFloorY + 0.72f;
    switch (style) {
        case CoachDesignStyle::UNVI_HIGH_DECKER: {
            // Gloss-black curved rear glass mask + chrome nameplate + upward boomerang ruby LED blades + twin exhausts
            AppendRearPatch(mesh, surf, -0.78f, 0.78f, rearWinBot, winSideTop - 0.18f, 10, 4, 0.008f, glossBlack);
            AppendRearRibbon(mesh, surf, {{-0.44f, deckFloorY + 0.46f, deckFloorY + 0.51f}, {0.44f, deckFloorY + 0.46f, deckFloorY + 0.51f}}, 0.016f, 0.008f, chromeCol);
            AppendRearSlattedGrille(mesh, surf, 0.42f, skirtBotY + 0.44f, deckFloorY + 0.08f, 3, darkTrim, glossBlack, darkTrim);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                std::vector<RibbonSample> wing = {
                    {s * 0.48f, skirtBotY + 0.48f, skirtBotY + 0.62f},
                    {s * 0.68f, skirtBotY + 0.56f, skirtBotY + 0.74f},
                    {s * 0.88f, skirtBotY + 0.68f, skirtBotY + 0.92f}
                };
                AppendRearRibbon(mesh, surf, wing, 0.020f, 0.010f, glossBlack);
                AppendRearRibbon(mesh, surf, {{s * 0.50f, skirtBotY + 0.54f, skirtBotY + 0.60f}, {s * 0.87f, skirtBotY + 0.82f, skirtBotY + 0.90f}}, 0.026f, 0.006f, tailRed);
                AppendRearDisc(mesh, surf, s * 0.72f, skirtBotY + 0.64f, 0.026f, 0.030f, 0.026f, 0.006f, 10, tailAmber);
                // Satin chrome exhaust finisher
                RunningGear::AppendBox(mesh, Vec3(xRear - 0.04f, skirtBotY + 0.12f, s * 0.68f - 0.06f), Vec3(xRear + 0.05f, skirtBotY + 0.20f, s * 0.68f + 0.06f), chromeCol);
            }
            AppendRearRibbon(mesh, surf, {{-0.34f, winSideTop - 0.07f, winSideTop - 0.02f}, {0.34f, winSideTop - 0.07f, winSideTop - 0.02f}}, 0.020f, 0.008f, tailRed);
            break;
        }

        case CoachDesignStyle::VOLVO_9900_THEATER: {
            // Theater high-deck dark pewter shroud + twin vertical pods with quad circular Euro jewel rings + high spoiler brake strip
            AppendRearPatch(mesh, surf, -0.74f, 0.74f, rearWinBot + 0.14f, winSideTop - 0.16f, 10, 3, 0.008f, darkTrim);
            AppendRearRibbon(mesh, surf, {{-0.88f, deckFloorY + 0.52f, deckFloorY + 0.58f}, {0.88f, deckFloorY + 0.52f, deckFloorY + 0.58f}}, 0.018f, 0.008f, chromeCol);
            AppendRearSlattedGrille(mesh, surf, 0.48f, skirtBotY + 0.44f, deckFloorY + 0.14f, 4, chromeCol, glossBlack, darkTrim);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                AppendRearPatch(mesh, surf, s * 0.72f, s * 0.90f, skirtBotY + 0.42f, deckFloorY + 0.08f, 3, 4, 0.014f, glossBlack);
                AppendRearDisc(mesh, surf, s * 0.81f, skirtBotY + 0.56f, 0.042f, 0.048f, 0.024f, 0.006f, 12, tailRed);
                AppendRearDisc(mesh, surf, s * 0.81f, skirtBotY + 0.74f, 0.042f, 0.048f, 0.024f, 0.006f, 12, tailRed);
                AppendRearDisc(mesh, surf, s * 0.81f, skirtBotY + 0.92f, 0.036f, 0.040f, 0.024f, 0.006f, 12, tailAmber);
                AppendRearDisc(mesh, surf, s * 0.81f, skirtBotY + 1.04f, 0.026f, 0.028f, 0.024f, 0.006f, 10, revWhite);
            }
            // Theater-deck subtle rear lip wing with long brake strip
            RunningGear::AppendBox(mesh, Vec3(xRear - 0.06f, H - 0.16f, -hw + 0.26f), Vec3(xRear + 0.08f, H - 0.09f, hw - 0.26f), darkTrim);
            RunningGear::AppendBox(mesh, Vec3(xRear + 0.06f, H - 0.145f, -0.52f), Vec3(xRear + 0.09f, H - 0.105f, 0.52f), tailRed);
            break;
        }

        case CoachDesignStyle::VOLVO_BZR_ELECTRIC: {
            // Zero-emission sealed tailgate: continuous full-width connected crimson EV lightbar + vertical C-blades + cyan diffuser trim
            AppendRearPatch(mesh, surf, -0.82f, 0.82f, rearWinBot + 0.04f, winSideTop - 0.18f, 10, 4, 0.008f, glossBlack);
            // Smooth sealed painted rear EV engine cover (no noisy louvers or exhausts)
            AppendRearRibbon(mesh, surf, {{-0.62f, skirtBotY + 0.12f, skirtBotY + 0.155f}, {0.62f, skirtBotY + 0.12f, skirtBotY + 0.155f}}, 0.024f, 0.008f, evCyan);
            // Full-width glowing connected crimson taillight bar
            AppendRearRibbon(mesh, surf, {{-0.88f, deckFloorY - 0.08f, deckFloorY + 0.02f}, {0.0f, deckFloorY - 0.09f, deckFloorY + 0.01f}, {0.88f, deckFloorY - 0.08f, deckFloorY + 0.02f}}, 0.022f, 0.008f, tailRed);
            AppendRearRibbon(mesh, surf, {{-0.34f, deckFloorY + 0.06f, deckFloorY + 0.11f}, {0.34f, deckFloorY + 0.06f, deckFloorY + 0.11f}}, 0.020f, 0.006f, evCyan);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                AppendRearPatch(mesh, surf, s * 0.78f, s * 0.91f, skirtBotY + 0.42f, deckFloorY + 0.02f, 2, 4, 0.018f, glossBlack);
                AppendRearRibbon(mesh, surf, {{s * 0.84f, skirtBotY + 0.44f, deckFloorY - 0.02f}, {s * 0.89f, skirtBotY + 0.44f, deckFloorY - 0.02f}}, 0.024f, 0.006f, tailRed);
                AppendRearDisc(mesh, surf, s * 0.81f, skirtBotY + 0.50f, 0.024f, 0.028f, 0.024f, 0.006f, 10, tailAmber);
            }
            break;
        }

        case CoachDesignStyle::VOLVO_9700_USCAN: {
            // Classic North American stainless radiator service door + horizontal triple-lens lamps + 5 upper DOT beacons
            AppendRearSlattedGrille(mesh, surf, 0.52f, skirtBotY + 0.44f, deckFloorY + 0.28f, 5, chromeCol, Vec4(0.68f, 0.72f, 0.76f, 1.0f), darkTrim);
            AppendRearPatch(mesh, surf, -0.74f, 0.74f, rearWinBot + 0.06f, winSideTop - 0.18f, 8, 3, 0.008f, glossBlack);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                AppendRearPatch(mesh, surf, s * 0.62f, s * 0.90f, skirtBotY + 0.46f, skirtBotY + 0.86f, 3, 3, 0.016f, chromeCol);
                AppendRearRibbon(mesh, surf, {{s * 0.64f, skirtBotY + 0.49f, skirtBotY + 0.59f}, {s * 0.88f, skirtBotY + 0.49f, skirtBotY + 0.59f}}, 0.022f, 0.006f, revWhite);
                AppendRearRibbon(mesh, surf, {{s * 0.64f, skirtBotY + 0.61f, skirtBotY + 0.72f}, {s * 0.88f, skirtBotY + 0.61f, skirtBotY + 0.72f}}, 0.022f, 0.006f, tailAmber);
                AppendRearRibbon(mesh, surf, {{s * 0.64f, skirtBotY + 0.74f, skirtBotY + 0.84f}, {s * 0.88f, skirtBotY + 0.74f, skirtBotY + 0.84f}}, 0.022f, 0.006f, tailRed);
            }
            for (float uDot : {-0.76f, -0.18f, 0.0f, 0.18f, 0.76f}) {
                AppendRearDisc(mesh, surf, uDot, H - 0.18f, 0.026f, 0.026f, 0.022f, 0.006f, 8, tailRed);
            }
            break;
        }

        case CoachDesignStyle::VOLVO_9700_DD:
        case CoachDesignStyle::VOLVO_9800_DD: {
            bool is98 = (style == CoachDesignStyle::VOLVO_9800_DD);
            // Dual-level DD rear: upper deck rear observation glass band + mid divider + vertical jagged/tower taillights
            float midY = skirtBotY + 1.94f;
            AppendRearPatch(mesh, surf, -0.82f, 0.82f, midY + 0.42f, winSideTop - 0.14f, 10, 3, 0.008f, glossBlack);
            AppendRearRibbon(mesh, surf, {{-0.88f, midY, midY + 0.30f}, {0.88f, midY, midY + 0.30f}}, 0.016f, 0.010f, is98 ? bodyPaint : glossBlack);
            AppendRearSlattedGrille(mesh, surf, 0.50f, skirtBotY + 0.44f, midY - 0.28f, is98 ? 3 : 4, chromeCol, glossBlack, darkTrim);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                if (is98) {
                    // 9800DD smoked-chrome tower + crisply offset reverse/amber/red jewel bars + roof wrap spoiler
                    AppendRearPatch(mesh, surf, s * 0.74f, s * 0.91f, skirtBotY + 0.44f, midY - 0.18f, 3, 4, 0.016f, glossBlack);
                    AppendRearRibbon(mesh, surf, {{s * 0.83f, skirtBotY + 0.48f, midY - 0.24f}, {s * 0.89f, skirtBotY + 0.48f, midY - 0.24f}}, 0.024f, 0.006f, tailRed);
                    AppendRearRibbon(mesh, surf, {{s * 0.76f, skirtBotY + 0.62f, skirtBotY + 0.90f}, {s * 0.81f, skirtBotY + 0.62f, skirtBotY + 0.90f}}, 0.024f, 0.006f, tailAmber);
                    AppendRearRibbon(mesh, surf, {{s * 0.76f, skirtBotY + 0.48f, skirtBotY + 0.58f}, {s * 0.81f, skirtBotY + 0.48f, skirtBotY + 0.58f}}, 0.024f, 0.006f, revWhite);
                    RunningGear::AppendBox(mesh, Vec3(xRear - 0.04f, skirtBotY + 0.12f, s * 0.66f - 0.05f), Vec3(xRear + 0.05f, skirtBotY + 0.19f, s * 0.66f + 0.05f), chromeCol);
                } else {
                    // 9700DD sharp inward-angled V-fang LED blades
                    std::vector<RibbonSample> fang = {
                        {s * 0.66f, skirtBotY + 0.46f, skirtBotY + 0.64f},
                        {s * 0.78f, skirtBotY + 0.58f, midY - 0.20f},
                        {s * 0.90f, skirtBotY + 0.72f, midY - 0.14f}
                    };
                    AppendRearRibbon(mesh, surf, fang, 0.018f, 0.010f, glossBlack);
                    AppendRearRibbon(mesh, surf, {{s * 0.82f, skirtBotY + 0.60f, midY - 0.20f}, {s * 0.88f, skirtBotY + 0.74f, midY - 0.16f}}, 0.024f, 0.006f, tailRed);
                    AppendRearDisc(mesh, surf, s * 0.75f, skirtBotY + 0.55f, 0.026f, 0.030f, 0.024f, 0.006f, 10, tailAmber);
                }
            }
            if (is98) {
                RunningGear::AppendBox(mesh, Vec3(xRear - 0.06f, H - 0.16f, -hw + 0.18f), Vec3(xRear + 0.07f, H - 0.08f, hw - 0.18f), glossBlack);
            }
            AppendRearRibbon(mesh, surf, {{-0.38f, H - 0.24f, H - 0.18f}, {0.38f, H - 0.24f, H - 0.18f}}, 0.022f, 0.006f, tailRed);
            break;
        }

        case CoachDesignStyle::AERO_BLADE_VIP: {
            // Futuristic Aero Blade concept: amber vertical rear cheek fins, amber diffuser trim, hex mesh, quad chrome pipes & dual-fin roof spoiler
            AppendRearPatch(mesh, surf, -0.82f, 0.82f, rearWinBot + 0.04f, winSideTop - 0.14f, 10, 4, 0.008f, glossBlack);
            AppendRearSlattedGrille(mesh, surf, 0.46f, skirtBotY + 0.44f, deckFloorY + 0.06f, 3, chromeCol, glossBlack, darkTrim);
            AppendRearRibbon(mesh, surf, {{-0.64f, skirtBotY + 0.09f, skirtBotY + 0.13f}, {0.64f, skirtBotY + 0.09f, skirtBotY + 0.13f}}, 0.024f, 0.008f, tailAmber);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                // Claw tail pod + amber outer air-curtain blade
                AppendRearRibbon(mesh, surf, {{s * 0.54f, skirtBotY + 0.52f, skirtBotY + 0.64f}, {s * 0.74f, skirtBotY + 0.60f, skirtBotY + 0.76f}, {s * 0.88f, skirtBotY + 0.70f, skirtBotY + 0.88f}}, 0.022f, 0.010f, tailRed);
                AppendRearRibbon(mesh, surf, {{s * 0.88f, skirtBotY + 0.36f, deckFloorY + 0.22f}, {s * 0.92f, skirtBotY + 0.36f, deckFloorY + 0.22f}}, 0.025f, 0.008f, tailAmber);
                // Quad chrome pipes (pair per side)
                RunningGear::AppendBox(mesh, Vec3(xRear - 0.03f, skirtBotY + 0.12f, s * 0.56f - 0.045f), Vec3(xRear + 0.06f, skirtBotY + 0.20f, s * 0.56f + 0.045f), chromeCol);
                RunningGear::AppendBox(mesh, Vec3(xRear - 0.03f, skirtBotY + 0.12f, s * 0.70f - 0.045f), Vec3(xRear + 0.06f, skirtBotY + 0.20f, s * 0.70f + 0.045f), chromeCol);
                // Spoiler fin
                RunningGear::AppendBox(mesh, Vec3(xRear - 0.18f, H - 0.16f, s * 0.86f - 0.04f), Vec3(xRear + 0.10f, H + 0.02f, s * 0.86f + 0.04f), tailAmber);
            }
            RunningGear::AppendBox(mesh, Vec3(xRear - 0.04f, H - 0.07f, -hw + 0.32f), Vec3(xRear + 0.10f, H - 0.01f, hw - 0.32f), darkTrim);
            RunningGear::AppendBox(mesh, Vec3(xRear + 0.07f, H - 0.06f, -0.45f), Vec3(xRear + 0.11f, H - 0.02f, 0.45f), tailAmber);
            break;
        }

        case CoachDesignStyle::YUTONG_TOURING: {
            // Sweeping chrome U-smile rear garnish mirroring front whiskers + L-shaped eagle-claw tails
            AppendRearPatch(mesh, surf, -0.78f, 0.78f, rearWinBot + 0.04f, winSideTop - 0.18f, 10, 4, 0.008f, glossBlack);
            AppendRearRibbon(mesh, surf, {{-0.62f, deckFloorY + 0.44f, deckFloorY + 0.49f}, {0.0f, deckFloorY + 0.38f, deckFloorY + 0.43f}, {0.62f, deckFloorY + 0.44f, deckFloorY + 0.49f}}, 0.020f, 0.008f, chromeCol);
            AppendRearSlattedGrille(mesh, surf, 0.40f, skirtBotY + 0.44f, deckFloorY + 0.06f, 3, darkTrim, glossBlack, darkTrim);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                AppendRearRibbon(mesh, surf, {{s * 0.52f, skirtBotY + 0.56f, skirtBotY + 0.70f}, {s * 0.87f, skirtBotY + 0.62f, skirtBotY + 0.76f}}, 0.022f, 0.008f, tailRed);
                AppendRearRibbon(mesh, surf, {{s * 0.80f, skirtBotY + 0.62f, deckFloorY + 0.04f}, {s * 0.88f, skirtBotY + 0.62f, deckFloorY + 0.04f}}, 0.022f, 0.008f, tailRed);
                AppendRearDisc(mesh, surf, s * 0.64f, skirtBotY + 0.63f, 0.024f, 0.026f, 0.024f, 0.006f, 10, tailAmber);
            }
            AppendRearRibbon(mesh, surf, {{-0.30f, winSideTop - 0.07f, winSideTop - 0.02f}, {0.30f, winSideTop - 0.07f, winSideTop - 0.02f}}, 0.020f, 0.008f, tailRed);
            break;
        }

        case CoachDesignStyle::NEOPLAN_TOURLINER: {
            // Neoplan iconic deep V-cut rear glass & sharp diagonal razor-bar LED taillights
            AppendRearRibbon(mesh, surf, {{-0.78f, rearWinBot + 0.14f, winSideTop - 0.16f}, {0.0f, rearWinBot - 0.12f, winSideTop - 0.16f}, {0.78f, rearWinBot + 0.14f, winSideTop - 0.16f}}, 0.010f, 0.012f, glossBlack);
            AppendRearRibbon(mesh, surf, {{-0.26f, deckFloorY + 0.44f, deckFloorY + 0.48f}, {0.0f, deckFloorY + 0.36f, deckFloorY + 0.40f}, {0.26f, deckFloorY + 0.44f, deckFloorY + 0.48f}}, 0.018f, 0.008f, chromeCol);
            AppendRearSlattedGrille(mesh, surf, 0.38f, skirtBotY + 0.44f, deckFloorY + 0.04f, 2, darkTrim, glossBlack, chromeCol);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                AppendRearRibbon(mesh, surf, {{s * 0.52f, skirtBotY + 0.52f, skirtBotY + 0.60f}, {s * 0.88f, skirtBotY + 0.72f, skirtBotY + 0.80f}}, 0.023f, 0.008f, tailRed);
                AppendRearRibbon(mesh, surf, {{s * 0.52f, skirtBotY + 0.42f, skirtBotY + 0.48f}, {s * 0.86f, skirtBotY + 0.58f, skirtBotY + 0.64f}}, 0.023f, 0.008f, tailAmber);
            }
            AppendRearRibbon(mesh, surf, {{-0.28f, winSideTop - 0.08f, winSideTop - 0.02f}, {0.28f, winSideTop - 0.08f, winSideTop - 0.02f}}, 0.020f, 0.008f, tailRed);
            break;
        }

        case CoachDesignStyle::TWIN_GLASS_DOME: {
            // Ceramic white panoramic rear + upper observation dome trim + 3 blue crown beacons + teardrop taillights
            AppendRearPatch(mesh, surf, -0.82f, 0.82f, rearWinBot + 0.04f, H - 0.28f, 10, 4, 0.008f, glossBlack);
            for (float bu : {-0.18f, 0.0f, 0.18f}) {
                AppendRearDisc(mesh, surf, bu, H - 0.16f, 0.025f, 0.025f, 0.022f, 0.006f, 8, Vec4(0.18f, 0.44f, 0.98f, 1.0f));
            }
            AppendRearSlattedGrille(mesh, surf, 0.40f, skirtBotY + 0.44f, deckFloorY + 0.04f, 2, chromeCol, glossBlack, darkTrim);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                AppendRearDisc(mesh, surf, s * 0.76f, skirtBotY + 0.66f, 0.068f, 0.120f, 0.020f, 0.008f, 12, tailRed);
                AppendRearDisc(mesh, surf, s * 0.76f, skirtBotY + 0.86f, 0.036f, 0.042f, 0.023f, 0.006f, 10, tailAmber);
                RunningGear::AppendBox(mesh, Vec3(xRear - 0.04f, skirtBotY + 0.12f, s * 0.66f - 0.05f), Vec3(xRear + 0.05f, skirtBotY + 0.19f, s * 0.66f + 0.05f), chromeCol);
            }
            break;
        }

        case CoachDesignStyle::OBSIDIAN_VIP_6X2: {
            // Obsidian VIP gloss-black rear: continuous bright mirror-chrome U-frame + smoked dual-U halo taillights + quad black-chrome exhausts
            AppendRearPatch(mesh, surf, -0.80f, 0.80f, rearWinBot + 0.04f, winSideTop - 0.16f, 10, 4, 0.008f, glossBlack);
            AppendRearSlattedGrille(mesh, surf, 0.44f, skirtBotY + 0.44f, deckFloorY + 0.04f, 3, chromeCol, glossBlack, darkTrim);
            AppendRearRibbon(mesh, surf, {{-0.86f, deckFloorY + 0.34f, deckFloorY + 0.39f}, {0.0f, deckFloorY + 0.28f, deckFloorY + 0.33f}, {0.86f, deckFloorY + 0.34f, deckFloorY + 0.39f}}, 0.020f, 0.008f, chromeCol);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                AppendRearPatch(mesh, surf, s * 0.62f, s * 0.89f, skirtBotY + 0.48f, deckFloorY - 0.12f, 3, 3, 0.016f, chromeCol);
                AppendRearRibbon(mesh, surf, {{s * 0.64f, skirtBotY + 0.51f, skirtBotY + 0.57f}, {s * 0.87f, skirtBotY + 0.54f, skirtBotY + 0.60f}}, 0.024f, 0.006f, tailRed);
                AppendRearRibbon(mesh, surf, {{s * 0.64f, skirtBotY + 0.64f, skirtBotY + 0.70f}, {s * 0.87f, skirtBotY + 0.67f, skirtBotY + 0.73f}}, 0.024f, 0.006f, tailRed);
                RunningGear::AppendBox(mesh, Vec3(xRear - 0.03f, skirtBotY + 0.12f, s * 0.58f - 0.04f), Vec3(xRear + 0.05f, skirtBotY + 0.19f, s * 0.58f + 0.04f), chromeCol);
                RunningGear::AppendBox(mesh, Vec3(xRear - 0.03f, skirtBotY + 0.12f, s * 0.72f - 0.04f), Vec3(xRear + 0.05f, skirtBotY + 0.19f, s * 0.72f + 0.04f), chromeCol);
            }
            AppendRearRibbon(mesh, surf, {{-0.38f, winSideTop - 0.07f, winSideTop - 0.02f}, {0.38f, winSideTop - 0.07f, winSideTop - 0.02f}}, 0.022f, 0.006f, tailRed);
            break;
        }

        case CoachDesignStyle::CHAMPAGNE_SR2: {
            // Brushed champagne luxury rear: circular quad halo taillights, brushed chrome crossbar & recessed vents
            AppendRearPatch(mesh, surf, -0.76f, 0.76f, rearWinBot, winSideTop - 0.18f, 10, 4, 0.008f, darkTrim);
            AppendRearRibbon(mesh, surf, {{-0.76f, deckFloorY + 0.50f, deckFloorY + 0.55f}, {0.76f, deckFloorY + 0.50f, deckFloorY + 0.55f}}, 0.018f, 0.008f, chromeCol);
            AppendRearSlattedGrille(mesh, surf, 0.42f, skirtBotY + 0.46f, deckFloorY + 0.06f, 2, chromeCol, darkTrim, bodyPaint);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                AppendRearDisc(mesh, surf, s * 0.64f, skirtBotY + 0.64f, 0.044f, 0.052f, 0.022f, 0.006f, 12, tailRed);
                AppendRearDisc(mesh, surf, s * 0.79f, skirtBotY + 0.64f, 0.040f, 0.046f, 0.022f, 0.006f, 12, tailAmber);
                AppendRearDisc(mesh, surf, s * 0.72f, skirtBotY + 0.80f, 0.030f, 0.032f, 0.022f, 0.006f, 10, revWhite);
            }
            AppendRearRibbon(mesh, surf, {{-0.30f, winSideTop - 0.07f, winSideTop - 0.02f}, {0.30f, winSideTop - 0.07f, winSideTop - 0.02f}}, 0.020f, 0.008f, tailRed);
            break;
        }

        case CoachDesignStyle::EXECUTIVE_COACH_I6: {
            // Irizar i6 rear: curved black upper transom bezel + matching chrome V-chevron garnish + swept boomerang LED tails + twin exhausts
            AppendRearPatch(mesh, surf, -0.78f, 0.78f, rearWinBot + 0.04f, winSideTop - 0.16f, 10, 4, 0.008f, glossBlack);
            AppendRearRibbon(
                mesh, surf,
                {{-0.78f, deckFloorY + 0.52f, deckFloorY + 0.57f},
                 { 0.00f, deckFloorY + 0.39f, deckFloorY + 0.44f},
                 { 0.78f, deckFloorY + 0.52f, deckFloorY + 0.57f}},
                0.020f, 0.008f, chromeCol
            );
            AppendRearSlattedGrille(mesh, surf, 0.44f, skirtBotY + 0.44f, deckFloorY + 0.06f, 3, darkTrim, glossBlack, darkTrim);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                std::vector<RibbonSample> tailWing = {
                    {s * 0.48f, skirtBotY + 0.50f, skirtBotY + 0.62f},
                    {s * 0.68f, skirtBotY + 0.56f, skirtBotY + 0.72f},
                    {s * 0.88f, skirtBotY + 0.66f, skirtBotY + 0.86f}
                };
                AppendRearRibbon(mesh, surf, tailWing, 0.018f, 0.010f, glossBlack);
                AppendRearRibbon(mesh, surf, {{s * 0.50f, skirtBotY + 0.54f, skirtBotY + 0.60f}, {s * 0.87f, skirtBotY + 0.78f, skirtBotY + 0.84f}}, 0.025f, 0.006f, tailRed);
                AppendRearDisc(mesh, surf, s * 0.70f, skirtBotY + 0.62f, 0.026f, 0.030f, 0.024f, 0.006f, 10, tailAmber);
                AppendRearDisc(mesh, surf, s * 0.58f, skirtBotY + 0.57f, 0.022f, 0.024f, 0.024f, 0.006f, 10, revWhite);
                RunningGear::AppendBox(mesh, Vec3(xRear - 0.04f, skirtBotY + 0.12f, s * 0.68f - 0.055f), Vec3(xRear + 0.05f, skirtBotY + 0.19f, s * 0.68f + 0.055f), chromeCol);
            }
            RunningGear::AppendBox(mesh, Vec3(xRear - 0.06f, H - 0.16f, -hw + 0.24f), Vec3(xRear + 0.07f, H - 0.09f, hw - 0.24f), bodyPaint);
            AppendRearRibbon(mesh, surf, {{-0.34f, winSideTop - 0.07f, winSideTop - 0.02f}, {0.34f, winSideTop - 0.07f, winSideTop - 0.02f}}, 0.022f, 0.006f, tailRed);
            break;
        }

        case CoachDesignStyle::VOLVO_9700_TEAL:
        case CoachDesignStyle::VOLVO_9800_AERO:
        default: {
            bool is9800 = (style == CoachDesignStyle::VOLVO_9800_AERO);
            AppendRearPatch(mesh, surf, -0.78f, 0.78f, rearWinBot, winSideTop - 0.16f, 10, 4, 0.008f, glossBlack);
            AppendRearSlattedGrille(mesh, surf, 0.44f, skirtBotY + 0.44f, deckFloorY + 0.04f, is9800 ? 4 : 2, chromeCol, glossBlack, darkTrim);
            AppendRearRibbon(mesh, surf, {{-0.62f, deckFloorY + 0.44f, deckFloorY + 0.49f}, {0.62f, deckFloorY + 0.44f, deckFloorY + 0.49f}}, 0.018f, 0.008f, chromeCol);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                // Vertical Volvo C-hook / L-blade LED corner housings
                AppendRearPatch(mesh, surf, s * 0.74f, s * 0.90f, skirtBotY + 0.46f, deckFloorY - 0.04f, 3, 3, 0.016f, glossBlack);
                AppendRearRibbon(mesh, surf, {{s * 0.83f, skirtBotY + 0.48f, deckFloorY - 0.06f}, {s * 0.88f, skirtBotY + 0.48f, deckFloorY - 0.06f}}, 0.023f, 0.006f, tailRed);
                AppendRearRibbon(mesh, surf, {{s * 0.75f, skirtBotY + 0.48f, skirtBotY + 0.56f}, {s * 0.88f, skirtBotY + 0.48f, skirtBotY + 0.56f}}, 0.023f, 0.006f, is9800 ? tailAmber : revWhite);
            }
            if (is9800) {
                RunningGear::AppendBox(mesh, Vec3(xRear - 0.05f, H - 0.16f, -hw + 0.24f), Vec3(xRear + 0.06f, H - 0.09f, hw - 0.24f), bodyPaint);
                for (float uDot : {-0.76f, -0.18f, 0.0f, 0.18f, 0.76f}) {
                    AppendRearDisc(mesh, surf, uDot, H - 0.13f, 0.024f, 0.024f, 0.024f, 0.006f, 8, tailRed);
                }
            } else {
                AppendRearRibbon(mesh, surf, {{-0.34f, H - 0.24f, H - 0.18f}, {0.34f, H - 0.24f, H - 0.18f}}, 0.022f, 0.006f, tailRed);
                RunningGear::AppendBox(mesh, Vec3(xRear - 0.04f, skirtBotY + 0.12f, -0.70f), Vec3(xRear + 0.05f, skirtBotY + 0.19f, -0.58f), chromeCol);
            }
            break;
        }
    }
    (void)isDoubleDeck;
}

} // namespace sphere::vehicles::coach
