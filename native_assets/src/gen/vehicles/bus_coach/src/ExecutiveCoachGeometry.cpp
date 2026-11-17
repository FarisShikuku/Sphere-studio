#include "ExecutiveCoachGeometry.h"
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

inline Vec3 SafeNormalize(const Vec3& v, const Vec3& fallback = Vec3(-1.0f, 0.0f, 0.0f)) {
    float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
    if (lenSq < 1e-8f) return fallback;
    float inv = 1.0f / std::sqrt(lenSq);
    return Vec3(v.x * inv, v.y * inv, v.z * inv);
}

inline Vec3 Cross(const Vec3& a, const Vec3& b) {
    return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
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

struct ExecSample {
    float u;
    float vBot;
    float vTop;
};

// C1 two-arc curved aerodynamic front + deeply raked panoramic windshield + seamless unified roof dome
struct ExecFrontSurface {
    float xFront = 0.0f;
    float xApillar = 0.0f;
    float xDomeEnd = 0.0f;
    float skirtBotY = 0.28f;
    float noseTopY = 1.36f;
    float winTopY = 3.36f;
    float winSideTop = 3.48f;
    float H = 3.84f;
    float hw = 1.275f;

    float uCenterZone = 0.64f;
    float rCorner = 0.27f;
    float alpha = 0.40f;
    float sinA = 0.0f;
    float cosA = 1.0f;
    float zFlatEdge = 0.0f;
    float Rfront = 1.0f;
    float dxBulge = 0.0f;

    void Init(float inXFront, float inSkirtBotY, float inNoseTopY, float inWinTopY, float inWinSideTop, float inH, float inHw) {
        xFront = inXFront;
        xApillar = inXFront + 0.78f;
        xDomeEnd = inXFront + 2.52f;
        skirtBotY = inSkirtBotY;
        noseTopY = inNoseTopY;
        winTopY = inWinTopY;
        winSideTop = inWinSideTop;
        H = inH;
        hw = inHw;

        sinA = std::sin(alpha);
        cosA = std::cos(alpha);
        zFlatEdge = std::max(0.45f, hw - rCorner * (1.0f - sinA));
        Rfront = zFlatEdge / sinA;
        dxBulge = Rfront * (1.0f - cosA);
    }

    // Smooth compound-curved vertical profile & C1 two-arc horizontal V-sweep
    Vec3 EvalFrontPos(float u, float y) const {
        float uc = std::clamp(u, -1.0f, 1.0f);
        float absU = std::abs(uc);
        float sign = (uc >= 0.0f) ? 1.0f : -1.0f;

        float xCenter = 0.0f;
        float xCorner = 0.0f;
        const float chinTop = skirtBotY + 0.16f;
        if (y <= chinTop) {
            float t = std::clamp((y - skirtBotY) / 0.16f, 0.0f, 1.0f);
            float a = t * (kPi * 0.5f);
            xCenter = (xFront + 0.02f) + 0.085f * (1.0f - std::sin(a));
            xCorner = (xApillar - 0.12f) + 0.045f * std::cos(a);
        } else if (y <= noseTopY) {
            float t = std::clamp((y - chinTop) / std::max(0.10f, noseTopY - chinTop), 0.0f, 1.0f);
            xCenter = (xFront + 0.02f) + 0.12f * t - 0.036f * std::sin(kPi * t);
            xCorner = (xApillar - 0.12f) + 0.06f * t;
        } else {
            float t = std::clamp((y - noseTopY) / std::max(0.20f, winTopY - noseTopY), 0.0f, 1.0f);
            // Smooth upward-back raked windshield curve tangent to noseTopY and roof brow
            float rakeCurve = 0.72f * t + 0.28f * (t * t);
            xCenter = (xFront + 0.14f) + 0.52f * rakeCurve;
            xCorner = (xApillar - 0.06f) + 0.34f * t;
        }

        float x = 0.0f, z = 0.0f;
        if (absU <= uCenterZone) {
            float phi = (absU / uCenterZone) * alpha;
            z = sign * (Rfront * std::sin(phi));
            x = xCenter + Rfront * (1.0f - std::cos(phi));
        } else {
            float tc = (absU - uCenterZone) / (1.0f - uCenterZone);
            float phi = alpha + tc * (kPi * 0.5f - alpha);
            z = sign * (zFlatEdge + rCorner * (std::sin(phi) - sinA));
            float dxCorner = std::max(0.06f, xCorner - (xCenter + dxBulge));
            x = (xCenter + dxBulge) + dxCorner * ((cosA - std::cos(phi)) / cosA);
        }

        // Gentle center header crown blending seamlessly up into side corner rail
        float wTop = std::clamp((y - noseTopY) / std::max(0.20f, winTopY - noseTopY), 0.0f, 1.0f);
        float sCenter = std::min(1.0f, absU / uCenterZone);
        float browArch = 0.045f * (1.0f - sCenter * sCenter);
        float cornerRise = (winSideTop - winTopY) * std::pow(std::max(0.0f, (absU - uCenterZone) / (1.0f - uCenterZone)), 1.25f);
        float yEff = y + wTop * (browArch + cornerRise);
        return Vec3(x, yEff, z);
    }

    Vec3 EvalFrontNormal(float u, float y) const {
        float du = 0.012f, dy = 0.015f;
        Vec3 pu0 = EvalFrontPos(u - du, y), pu1 = EvalFrontPos(u + du, y);
        Vec3 py0 = EvalFrontPos(u, y - dy), py1 = EvalFrontPos(u, y + dy);
        Vec3 dU(pu1.x - pu0.x, pu1.y - pu0.y, pu1.z - pu0.z);
        Vec3 dY(py1.x - py0.x, py1.y - py0.y, py1.z - py0.z);
        return SafeNormalize(Cross(dU, dY), Vec3(-1.0f, 0.0f, 0.0f));
    }

    // Unified smooth roof arch profile across main body [xDomeEnd .. xRear]
    Vec3 EvalRoofArchPos(float x, float u, float topDrop = 0.0f) const {
        float uc = std::clamp(u, -1.0f, 1.0f);
        float absU = std::abs(uc);
        float sign = (uc >= 0.0f) ? 1.0f : -1.0f;
        const float rShoulder = 0.34f;
        float zShoulder = hw - rShoulder;
        float roofPeakY = H - 0.02f - topDrop;
        float yShoulder = roofPeakY - 0.07f;

        if (absU <= uCenterZone) {
            float s = absU / uCenterZone;
            float z = sign * s * zShoulder;
            float y = roofPeakY - (roofPeakY - yShoulder) * (s * s);
            return Vec3(x, y, z);
        } else {
            float tc = (absU - uCenterZone) / (1.0f - uCenterZone);
            float a = tc * (kPi * 0.5f);
            float z = sign * (zShoulder + rShoulder * std::sin(a));
            float y = winSideTop + (yShoulder - winSideTop) * std::cos(a);
            return Vec3(x, y, z);
        }
    }

    // Single unified smooth aero cab dome connecting windshield header C1-tangentially to roof arch (zero pinch!)
    Vec3 EvalTopDomePos(float u, float vt) const {
        float t = std::clamp(vt, 0.0f, 1.0f);
        Vec3 p0 = EvalFrontPos(u, winTopY);
        Vec3 p1 = EvalRoofArchPos(xDomeEnd, u, 0.0f);
        float phi = t * (kPi * 0.5f);
        // Smooth progressive curve: climbs smoothly without forming a vertical wall lip or pinched waist
        float fx = 0.38f * t + 0.62f * (1.0f - std::cos(phi));
        float fy = std::sin(phi);
        float fz = t;
        return Vec3(
            p0.x + (p1.x - p0.x) * fx,
            p0.y + (p1.y - p0.y) * fy,
            p0.z + (p1.z - p0.z) * fz
        );
    }

    Vec3 EvalTopDomeNormal(float u, float vt) const {
        float du = 0.012f, dv = 0.015f;
        float va = std::max(0.0f, vt - dv), vb = std::min(1.0f, vt + dv);
        Vec3 pu0 = EvalTopDomePos(u - du, vt), pu1 = EvalTopDomePos(u + du, vt);
        Vec3 pv0 = EvalTopDomePos(u, va),      pv1 = EvalTopDomePos(u, vb);
        Vec3 dU(pu1.x - pu0.x, pu1.y - pu0.y, pu1.z - pu0.z);
        Vec3 dV(pv1.x - pv0.x, pv1.y - pv0.y, pv1.z - pv0.z);
        return SafeNormalize(Cross(dU, dV), Vec3(-0.3f, 0.95f, 0.0f));
    }
};

// Compound-curved convex rear transom + rounded wraparound D-pillars + upper spoiler lip
struct ExecRearSurface {
    float xWinEnd = 0.0f;
    float xRear = 0.0f;
    float skirtBotY = 0.28f;
    float deckFloorY = 1.40f;
    float winSideTop = 3.48f;
    float H = 3.84f;
    float hw = 1.275f;

    float uCenterZone = 0.68f;
    float rCorner = 0.24f;
    float alpha = 0.26f;
    float sinA = 0.0f;
    float cosA = 1.0f;
    float zFlatEdge = 0.0f;
    float Rrear = 1.0f;
    float dxBulge = 0.0f;

    void Init(float inXWinEnd, float inXRear, float inSkirtBotY, float inDeckFloorY, float inWinSideTop, float inH, float inHw) {
        xWinEnd = inXWinEnd;
        xRear = inXRear;
        skirtBotY = inSkirtBotY;
        deckFloorY = inDeckFloorY;
        winSideTop = inWinSideTop;
        H = inH;
        hw = inHw;

        sinA = std::sin(alpha);
        cosA = std::cos(alpha);
        zFlatEdge = std::max(0.45f, hw - rCorner * (1.0f - sinA));
        Rrear = zFlatEdge / sinA;
        dxBulge = Rrear * (1.0f - cosA);
    }

    Vec3 EvalRearPos(float u, float y) const {
        float uc = std::clamp(u, -1.0f, 1.0f);
        float absU = std::abs(uc);
        float sign = (uc >= 0.0f) ? 1.0f : -1.0f;

        // Graceful convex vertical transom profile: tucked diffuser -> rounded bumper -> convex deck -> upper spoiler crest
        float ySpan = std::max(0.5f, winSideTop - skirtBotY);
        float ty = std::clamp((y - skirtBotY) / ySpan, 0.0f, 1.0f);
        float xCenter = xRear + 0.065f * std::sin(kPi * ty) - 0.04f * ty;
        if (y > winSideTop - 0.16f) {
            float st = std::clamp((y - (winSideTop - 0.16f)) / 0.24f, 0.0f, 1.0f);
            xCenter += 0.075f * std::sin(st * kPi * 0.5f); // Subtle sculpted upper spoiler kick
        }
        float xCorner = xRear - 0.12f - 0.03f * ty;

        float x = 0.0f, z = 0.0f;
        if (absU <= uCenterZone) {
            float phi = (absU / uCenterZone) * alpha;
            z = sign * (Rrear * std::sin(phi));
            x = xCenter - Rrear * (1.0f - std::cos(phi));
        } else {
            float tc = (absU - uCenterZone) / (1.0f - uCenterZone);
            float phi = alpha + tc * (kPi * 0.5f - alpha);
            z = sign * (zFlatEdge + rCorner * (std::sin(phi) - sinA));
            float dxCorner = std::max(0.05f, (xCenter - dxBulge) - xCorner);
            x = (xCenter - dxBulge) - dxCorner * ((cosA - std::cos(phi)) / cosA);
        }
        return Vec3(x, y, z);
    }

    Vec3 EvalRearNormal(float u, float y) const {
        float du = 0.012f, dy = 0.015f;
        Vec3 pu0 = EvalRearPos(u - du, y), pu1 = EvalRearPos(u + du, y);
        Vec3 py0 = EvalRearPos(u, y - dy), py1 = EvalRearPos(u, y + dy);
        Vec3 dU(pu1.x - pu0.x, pu1.y - pu0.y, pu1.z - pu0.z);
        Vec3 dY(py1.x - py0.x, py1.y - py0.y, py1.z - py0.z);
        return SafeNormalize(Cross(dY, dU), Vec3(1.0f, 0.0f, 0.0f));
    }
};

void AppendExecFrontPatch(
    Mesh* mesh, const ExecFrontSurface& surf,
    float u0, float u1, float y0, float y1,
    int segU, int segV, float offsetOut, const Vec4& col
) {
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

void AppendExecFrontRibbon(
    Mesh* mesh, const ExecFrontSurface& surf,
    std::vector<ExecSample> samples,
    float protrudeOut, float depthIn, const Vec4& col
) {
    if (samples.size() < 2) return;
    if (samples.front().u > samples.back().u) std::reverse(samples.begin(), samples.end());
    size_t n = samples.size();
    std::vector<Vec3> fB(n), fT(n), bB(n), bT(n);
    for (size_t i = 0; i < n; ++i) {
        float u = samples[i].u;
        float yb = std::min(samples[i].vBot, samples[i].vTop - 0.004f);
        float yt = std::max(samples[i].vTop, yb + 0.004f);
        Vec3 pb = surf.EvalFrontPos(u, yb), nb = surf.EvalFrontNormal(u, yb);
        Vec3 pt = surf.EvalFrontPos(u, yt), nt = surf.EvalFrontNormal(u, yt);
        fB[i] = OffsetPt(pb, nb, protrudeOut);  bB[i] = OffsetPt(pb, nb, -depthIn);
        fT[i] = OffsetPt(pt, nt, protrudeOut);  bT[i] = OffsetPt(pt, nt, -depthIn);
    }
    for (size_t i = 0; i + 1 < n; ++i) {
        CabComponents::AppendQuad(mesh, fB[i], fB[i + 1], fT[i + 1], fT[i], col);
        CabComponents::AppendQuad(mesh, fT[i], fT[i + 1], bT[i + 1], bT[i], col);
        CabComponents::AppendQuad(mesh, bB[i], bB[i + 1], fB[i + 1], fB[i], col);
    }
    CabComponents::AppendQuad(mesh, bB.front(), fB.front(), fT.front(), bT.front(), col);
    CabComponents::AppendQuad(mesh, fB.back(), bB.back(), bT.back(), fT.back(), col);
}

void AppendExecDomePatch(
    Mesh* mesh, const ExecFrontSurface& surf,
    float u0, float u1, float v0, float v1,
    int segU, int segV, float offsetOut, const Vec4& col
) {
    float uMin = std::min(u0, u1), uMax = std::max(u0, u1);
    int nu = std::max(1, segU), nv = std::max(1, segV);
    for (int j = 0; j < nv; ++j) {
        float va = v0 + (v1 - v0) * (static_cast<float>(j) / static_cast<float>(nv));
        float vb = v0 + (v1 - v0) * (static_cast<float>(j + 1) / static_cast<float>(nv));
        for (int i = 0; i < nu; ++i) {
            float ua = uMin + (uMax - uMin) * (static_cast<float>(i) / static_cast<float>(nu));
            float ub = uMin + (uMax - uMin) * (static_cast<float>(i + 1) / static_cast<float>(nu));
            Vec3 a = OffsetPt(surf.EvalTopDomePos(ua, va), surf.EvalTopDomeNormal(ua, va), offsetOut);
            Vec3 b = OffsetPt(surf.EvalTopDomePos(ub, va), surf.EvalTopDomeNormal(ub, va), offsetOut);
            Vec3 c = OffsetPt(surf.EvalTopDomePos(ub, vb), surf.EvalTopDomeNormal(ub, vb), offsetOut);
            Vec3 d = OffsetPt(surf.EvalTopDomePos(ua, vb), surf.EvalTopDomeNormal(ua, vb), offsetOut);
            CabComponents::AppendQuad(mesh, a, b, c, d, col);
        }
    }
}

void AppendExecRearPatch(
    Mesh* mesh, const ExecRearSurface& rSurf,
    float u0, float u1, float y0, float y1,
    int segU, int segV, float offsetOut, const Vec4& col
) {
    float uMin = std::min(u0, u1), uMax = std::max(u0, u1);
    int nu = std::max(1, segU), nv = std::max(1, segV);
    for (int j = 0; j < nv; ++j) {
        float ya = y0 + (y1 - y0) * (static_cast<float>(j) / static_cast<float>(nv));
        float yb = y0 + (y1 - y0) * (static_cast<float>(j + 1) / static_cast<float>(nv));
        for (int i = 0; i < nu; ++i) {
            float ua = uMin + (uMax - uMin) * (static_cast<float>(i) / static_cast<float>(nu));
            float ub = uMin + (uMax - uMin) * (static_cast<float>(i + 1) / static_cast<float>(nu));
            Vec3 a = OffsetPt(rSurf.EvalRearPos(ua, ya), rSurf.EvalRearNormal(ua, ya), offsetOut);
            Vec3 b = OffsetPt(rSurf.EvalRearPos(ub, ya), rSurf.EvalRearNormal(ub, ya), offsetOut);
            Vec3 c = OffsetPt(rSurf.EvalRearPos(ub, yb), rSurf.EvalRearNormal(ub, yb), offsetOut);
            Vec3 d = OffsetPt(rSurf.EvalRearPos(ua, yb), rSurf.EvalRearNormal(ua, yb), offsetOut);
            CabComponents::AppendQuad(mesh, a, d, c, b, col); // +X outward CCW
        }
    }
}

void AppendExecRearRibbon(
    Mesh* mesh, const ExecRearSurface& rSurf,
    std::vector<ExecSample> samples,
    float protrudeOut, float depthIn, const Vec4& col
) {
    if (samples.size() < 2) return;
    if (samples.front().u > samples.back().u) std::reverse(samples.begin(), samples.end());
    size_t n = samples.size();
    std::vector<Vec3> fB(n), fT(n), bB(n), bT(n);
    for (size_t i = 0; i < n; ++i) {
        float u = samples[i].u;
        float yb = std::min(samples[i].vBot, samples[i].vTop - 0.004f);
        float yt = std::max(samples[i].vTop, yb + 0.004f);
        Vec3 pb = rSurf.EvalRearPos(u, yb), nb = rSurf.EvalRearNormal(u, yb);
        Vec3 pt = rSurf.EvalRearPos(u, yt), nt = rSurf.EvalRearNormal(u, yt);
        fB[i] = OffsetPt(pb, nb, protrudeOut);  bB[i] = OffsetPt(pb, nb, -depthIn);
        fT[i] = OffsetPt(pt, nt, protrudeOut);  bT[i] = OffsetPt(pt, nt, -depthIn);
    }
    for (size_t i = 0; i + 1 < n; ++i) {
        CabComponents::AppendQuad(mesh, fB[i], fT[i], fT[i + 1], fB[i + 1], col);
        CabComponents::AppendQuad(mesh, fT[i], bT[i], bT[i + 1], fT[i + 1], col);
        CabComponents::AppendQuad(mesh, bB[i], fB[i], fB[i + 1], bB[i + 1], col);
    }
    CabComponents::AppendQuad(mesh, fB.front(), bB.front(), bT.front(), fT.front(), col);
    CabComponents::AppendQuad(mesh, bB.back(), fB.back(), fT.back(), bT.back(), col);
}

void AppendExecDisc(
    Mesh* mesh, const ExecFrontSurface& surf,
    float uC, float yC, float radU, float radY,
    float protrudeOut, float depthIn, int segs, const Vec4& col
) {
    Vec3 pc = surf.EvalFrontPos(uC, yC), nc = surf.EvalFrontNormal(uC, yC);
    Vec3 centerF = OffsetPt(pc, nc, protrudeOut + 0.004f);
    std::vector<Vec3> rimF(segs), rimB(segs);
    for (int k = 0; k < segs; ++k) {
        float a = 2.0f * kPi * (static_cast<float>(k) / static_cast<float>(segs));
        float uk = uC + radU * std::cos(a), yk = yC + radY * std::sin(a);
        Vec3 pk = surf.EvalFrontPos(uk, yk), nk = surf.EvalFrontNormal(uk, yk);
        rimF[k] = OffsetPt(pk, nk, protrudeOut);
        rimB[k] = OffsetPt(pk, nk, -depthIn);
    }
    for (int k = 0; k < segs; ++k) {
        int kn = (k + 1) % segs;
        PushTri(mesh, centerF, rimF[k], rimF[kn], col);
        CabComponents::AppendQuad(mesh, rimB[k], rimB[kn], rimF[kn], rimF[k], col);
    }
}

void AppendExecRing(
    Mesh* mesh, const ExecFrontSurface& surf,
    float uC, float yC, float rInU, float rInY, float rOutU, float rOutY,
    float protrudeOut, float depthIn, int segs, const Vec4& col
) {
    std::vector<Vec3> iF(segs), iB(segs), oF(segs), oB(segs);
    for (int k = 0; k < segs; ++k) {
        float a = 2.0f * kPi * (static_cast<float>(k) / static_cast<float>(segs));
        float ca = std::cos(a), sa = std::sin(a);
        Vec3 pi = surf.EvalFrontPos(uC + rInU * ca, yC + rInY * sa), ni = surf.EvalFrontNormal(uC + rInU * ca, yC + rInY * sa);
        Vec3 po = surf.EvalFrontPos(uC + rOutU * ca, yC + rOutY * sa), no = surf.EvalFrontNormal(uC + rOutU * ca, yC + rOutY * sa);
        iF[k] = OffsetPt(pi, ni, protrudeOut);  iB[k] = OffsetPt(pi, ni, -depthIn);
        oF[k] = OffsetPt(po, no, protrudeOut);  oB[k] = OffsetPt(po, no, -depthIn);
    }
    for (int k = 0; k < segs; ++k) {
        int kn = (k + 1) % segs;
        CabComponents::AppendQuad(mesh, iF[k], oF[k], oF[kn], iF[kn], col);
        CabComponents::AppendQuad(mesh, oB[k], oB[kn], oF[kn], oF[k], col);
        CabComponents::AppendQuad(mesh, iF[k], iF[kn], iB[kn], iB[k], col);
    }
}

// Smooth connected lofted elliptical tube along arbitrary 3D spline (zero Minecraft stair-steps!)
void AppendSweptTube(
    Mesh* mesh,
    const std::vector<Vec3>& spine,
    const std::vector<float>& rxList,
    const std::vector<float>& ryList,
    int ringSegs,
    const Vec4& col
) {
    if (spine.size() < 2) return;
    size_t n = spine.size();
    std::vector<std::vector<Vec3>> rings(n);
    for (size_t i = 0; i < n; ++i) {
        Vec3 tangent;
        if (i == 0) {
            tangent = Vec3(spine[1].x - spine[0].x, spine[1].y - spine[0].y, spine[1].z - spine[0].z);
        } else if (i + 1 == n) {
            tangent = Vec3(spine[n - 1].x - spine[n - 2].x, spine[n - 1].y - spine[n - 2].y, spine[n - 1].z - spine[n - 2].z);
        } else {
            tangent = Vec3(spine[i + 1].x - spine[i - 1].x, spine[i + 1].y - spine[i - 1].y, spine[i + 1].z - spine[i - 1].z);
        }
        Vec3 T = SafeNormalize(tangent, Vec3(-1.0f, 0.0f, 0.0f));
        Vec3 refUp = (std::abs(T.y) < 0.90f) ? Vec3(0.0f, 1.0f, 0.0f) : Vec3(1.0f, 0.0f, 0.0f);
        Vec3 N = SafeNormalize(Cross(refUp, T), Vec3(0.0f, 0.0f, 1.0f));
        Vec3 B = SafeNormalize(Cross(T, N), Vec3(0.0f, 1.0f, 0.0f));

        rings[i].reserve(ringSegs);
        for (int k = 0; k < ringSegs; ++k) {
            float a = 2.0f * kPi * (static_cast<float>(k) / static_cast<float>(ringSegs));
            float ca = std::cos(a), sa = std::sin(a);
            rings[i].emplace_back(
                spine[i].x + N.x * (rxList[i] * ca) + B.x * (ryList[i] * sa),
                spine[i].y + N.y * (rxList[i] * ca) + B.y * (ryList[i] * sa),
                spine[i].z + N.z * (rxList[i] * ca) + B.z * (ryList[i] * sa)
            );
        }
    }
    for (size_t i = 0; i + 1 < n; ++i) {
        for (int k = 0; k < ringSegs; ++k) {
            int kn = (k + 1) % ringSegs;
            CabComponents::AppendQuad(mesh, rings[i][k], rings[i][kn], rings[i + 1][kn], rings[i + 1][k], col);
        }
    }
    for (int k = 0; k < ringSegs; ++k) {
        int kn = (k + 1) % ringSegs;
        PushTri(mesh, spine.front(), rings.front()[kn], rings.front()[k], col);
        PushTri(mesh, spine.back(),  rings.back()[k],   rings.back()[kn], col);
    }
}

// Low-profile, smoothly blended rooftop aero HVAC crown + flush vents + slim shark-fin antenna
void AppendFlushAeroRoofCrown(
    Mesh* mesh,
    const ExecFrontSurface& surf,
    float xFront, float xRear, float H, float hw,
    const Vec4& bodyPaint, const Vec4& glossBlack
) {
    // Low, wide aerodynamic spine fairing emerging softly behind the curved front dome
    std::vector<HullStationX> crownFairing = {
        {xFront + 1.82f, H - 0.14f, H - 0.01f, hw - 0.56f, 0.04f, 0.24f, 0.03f, 0.16f},
        {xFront + 2.55f, H - 0.12f, H + 0.07f, hw - 0.34f, 0.04f, 0.28f, 0.04f, 0.08f},
        {xFront + 3.80f, H - 0.10f, H + 0.09f, hw - 0.24f, 0.04f, 0.26f, 0.04f, 0.00f},
        {xRear  - 2.20f, H - 0.09f, H + 0.08f, hw - 0.24f, 0.04f, 0.24f, 0.03f, 0.00f},
        {xRear  - 1.15f, H - 0.12f, H - 0.02f, hw - 0.44f, 0.04f, 0.20f, 0.02f, 0.00f},
    };
    CabComponents::AppendSculptedHullX(mesh, crownFairing, bodyPaint, 5);

    // Central flush tapered aero intake pocket + side sleek black louver strakes
    CabComponents::AppendFrustumX(
        mesh,
        xFront + 2.18f, H + 0.025f, H + 0.065f, 0.05f,
        xFront + 2.68f, H + 0.055f, H + 0.092f, 0.06f,
        glossBlack
    );
    for (int side : {-1, 1}) {
        float sz = static_cast<float>(side) * 0.26f;
        RunningGear::AppendBox(
            mesh,
            Vec3(xFront + 2.26f, H + 0.05f, std::min(sz - 0.055f, sz + 0.055f)),
            Vec3(xFront + 2.74f, H + 0.092f, std::max(sz - 0.055f, sz + 0.055f)),
            glossBlack
        );
    }

    // Razor-slim forward swept black shark-fin antenna cleanly rooted in the upper brow crown
    float antX = xFront + 1.48f;
    Vec3 antBase = surf.EvalTopDomePos(0.0f, 0.56f);
    CabComponents::AppendFrustumX(
        mesh,
        antX,        antBase.y - 0.02f, antBase.y + 0.032f, 0.010f,
        antX + 0.14f, antBase.y - 0.02f, antBase.y + 0.078f, 0.016f,
        glossBlack
    );
}

void AppendChromeWheelCap(Mesh* mesh, float ax, float cy, float hw, const Vec4& chromeCol) {
    const int segs = 16;
    const float rOuter = 0.25f;
    const float rDome  = 0.12f;
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zRim  = s * (hw - 0.032f);
        float zDome = s * (hw + 0.008f);
        Vec3 center(ax, cy, zDome);
        for (int k = 0; k < segs; ++k) {
            float a0 = 2.0f * kPi * (static_cast<float>(k) / static_cast<float>(segs));
            float a1 = 2.0f * kPi * (static_cast<float>(k + 1) / static_cast<float>(segs));
            Vec3 o0(ax + rOuter * std::cos(a0), cy + rOuter * std::sin(a0), zRim);
            Vec3 o1(ax + rOuter * std::cos(a1), cy + rOuter * std::sin(a1), zRim);
            Vec3 d0(ax + rDome  * std::cos(a0), cy + rDome  * std::sin(a0), zRim + s * 0.022f);
            Vec3 d1(ax + rDome  * std::cos(a1), cy + rDome  * std::sin(a1), zRim + s * 0.022f);
            if (side > 0) {
                CabComponents::AppendQuad(mesh, o0, o1, d1, d0, chromeCol);
                PushTri(mesh, center, d0, d1, chromeCol);
            } else {
                CabComponents::AppendQuad(mesh, o1, o0, d0, d1, chromeCol);
                PushTri(mesh, center, d1, d0, chromeCol);
            }
        }
    }
}

// Continuous Bezier-lofted gooseneck overhead mirrors + rounded aero mirror shells + driver spotter mirror
void AppendExecutiveMirrors(
    Mesh* mesh,
    const ExecFrontSurface& surf,
    float hw,
    const Vec4& bodyPaint,
    const Vec4& darkTrim,
    const Vec4& chromeCol
) {
    const Vec4 glassMirror(0.56f, 0.66f, 0.78f, 1.0f);
    Vec3 anchor = surf.EvalFrontPos(0.95f, surf.winTopY - 0.04f);

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        // Smooth 12-point Bezier lofted gooseneck tube curving UP, forward, and outward
        Vec3 p0(anchor.x,          anchor.y,          s * (hw - 0.03f));
        Vec3 p1(anchor.x - 0.16f,  anchor.y + 0.19f,  s * (hw + 0.12f));
        Vec3 p2(anchor.x - 0.44f,  anchor.y + 0.20f,  s * (hw + 0.28f));
        Vec3 p3(anchor.x - 0.56f,  anchor.y + 0.05f,  s * (hw + 0.31f));

        std::vector<Vec3> spine;
        std::vector<float> rx, ry;
        const int steps = 11;
        for (int i = 0; i <= steps; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(steps);
            float u = 1.0f - t;
            float b0 = u * u * u, b1 = 3.0f * u * u * t, b2 = 3.0f * u * t * t, b3 = t * t * t;
            spine.emplace_back(
                b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x,
                b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y,
                b0 * p0.z + b1 * p1.z + b2 * p2.z + b3 * p3.z
            );
            rx.push_back(0.048f - 0.016f * t);
            ry.push_back(0.036f - 0.008f * t);
        }
        AppendSweptTube(mesh, spine, rx, ry, 8, bodyPaint);

        // Smooth rounded vertical teardrop mirror capsule hanging from Bezier tip
        float headX = p3.x;
        float headYTop = p3.y + 0.08f;
        float headYBot = p3.y - 0.40f;
        float headZ = p3.z;
        std::vector<Vec3> podSpine = {
            Vec3(headX, headYTop + 0.01f, headZ),
            Vec3(headX, headYTop - 0.04f, headZ),
            Vec3(headX, (headYTop + headYBot) * 0.5f, headZ),
            Vec3(headX, headYBot + 0.06f, headZ),
            Vec3(headX, headYBot, headZ),
        };
        std::vector<float> podRx = {0.036f, 0.068f, 0.074f, 0.064f, 0.034f};
        std::vector<float> podRy = {0.032f, 0.062f, 0.068f, 0.058f, 0.030f};
        AppendSweptTube(mesh, podSpine, podRx, podRy, 10, bodyPaint);

        // Glossy rear mirror plate & front chrome styling blade on capsule
        RunningGear::AppendBox(
            mesh,
            Vec3(headX + 0.052f, headYBot + 0.04f, headZ - 0.050f),
            Vec3(headX + 0.072f, headYTop - 0.04f, headZ + 0.050f),
            glassMirror
        );
        RunningGear::AppendBox(
            mesh,
            Vec3(headX - 0.078f, (headYTop + headYBot) * 0.5f - 0.014f, headZ - 0.052f),
            Vec3(headX - 0.052f, (headYTop + headYBot) * 0.5f + 0.014f, headZ + 0.052f),
            chromeCol
        );
    }

    // Smooth curved lower driver-side (-Z) spotter mirror
    Vec3 spStart(anchor.x - 0.02f, surf.winTopY - 1.02f, -hw + 0.01f);
    Vec3 spEnd  (anchor.x - 0.08f, surf.winTopY - 1.06f, -hw - 0.10f);
    AppendSweptTube(mesh, {spStart, Vec3(anchor.x - 0.05f, surf.winTopY - 1.00f, -hw - 0.05f), spEnd}, {0.024f, 0.022f, 0.020f}, {0.024f, 0.022f, 0.020f}, 8, darkTrim);
    AppendSweptTube(
        mesh,
        {Vec3(spEnd.x, surf.winTopY - 0.90f, spEnd.z), Vec3(spEnd.x, surf.winTopY - 1.12f, spEnd.z), Vec3(spEnd.x, surf.winTopY - 1.36f, spEnd.z)},
        {0.032f, 0.056f, 0.030f}, {0.028f, 0.050f, 0.026f}, 8, darkTrim
    );
    RunningGear::AppendBox(
        mesh,
        Vec3(spEnd.x + 0.040f, surf.winTopY - 1.32f, spEnd.z - 0.038f),
        Vec3(spEnd.x + 0.058f, surf.winTopY - 0.94f, spEnd.z + 0.038f),
        glassMirror
    );
}

// Bespoke Irizar-style sweeping J-hook front cab window + dual curved chrome spears + flush passenger glass
void AppendExecutiveSideArchitecture(
    Mesh* mesh,
    float xFrontCorner,
    float xFrontAxle,
    float xWinEnd,
    float archTopY,
    float noseTopY,
    float winSideTop,
    float hw,
    float entryFrontX,
    float entryBackX,
    bool emitGlassPass,
    const Vec4& bodyPaint,
    const Vec4& glossBlack,
    const Vec4& chromeCol,
    const Vec4& sideGlass
) {
    const float xPillarStart = xFrontCorner;
    const float xJLow        = xFrontAxle - 1.05f;
    const float xBpillarTop  = xFrontAxle + 0.35f;
    const float yLowDrop     = archTopY + 0.18f;
    const float yMainBot     = noseTopY + 0.94f;
    const float yFrontTop    = winSideTop - 0.09f;

    auto evalWindowBot = [&](float x) -> float {
        if (x <= xJLow) {
            float t = std::clamp((x - xPillarStart) / std::max(0.05f, xJLow - xPillarStart), 0.0f, 1.0f);
            return (yLowDrop + 0.08f) - 0.08f * std::sin(t * kPi * 0.5f);
        }
        if (x < xBpillarTop) {
            float t = (x - xJLow) / std::max(0.05f, xBpillarTop - xJLow);
            float sCurve = t * t * (3.0f - 2.0f * t);
            return yLowDrop + (yMainBot - yLowDrop) * sCurve;
        }
        return yMainBot;
    };

    auto evalWindowTop = [&](float x) -> float {
        float xTopBlendEnd = xBpillarTop - 0.25f;
        if (x >= xTopBlendEnd) return winSideTop;
        float t = std::clamp((x - xPillarStart) / std::max(0.05f, xTopBlendEnd - xPillarStart), 0.0f, 1.0f);
        return yFrontTop + (winSideTop - yFrontTop) * std::sin(t * kPi * 0.5f);
    };

    auto evalLowerSpearY = [&](float x) -> float {
        float xSpearStart = xFrontCorner + 0.06f;
        float xSpearJoin  = xBpillarTop + 0.55f;
        if (x <= xSpearStart) return archTopY + 0.02f;
        if (x >= xSpearJoin)  return yMainBot - 0.07f;
        float t = (x - xSpearStart) / (xSpearJoin - xSpearStart);
        float arch = 0.14f * std::sin(t * kPi);
        return (archTopY + 0.02f) + ((yMainBot - 0.07f) - (archTopY + 0.02f)) * (t * t * (3.0f - 2.0f * t)) + arch * 0.25f;
    };

    const int curveSegs = 16;
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zWallOut = s * hw;
        float zWallIn  = s * (hw - 0.07f);
        float zGlass   = s * (hw + 0.008f);
        float zChrome  = s * (hw + 0.015f);
        bool skipRightDoor = (side > 0);

        if (!emitGlassPass) {
            for (int i = 0; i < curveSegs; ++i) {
                float xa = xPillarStart + (xBpillarTop - xPillarStart) * (static_cast<float>(i) / static_cast<float>(curveSegs));
                float xb = xPillarStart + (xBpillarTop - xPillarStart) * (static_cast<float>(i + 1) / static_cast<float>(curveSegs));
                if (skipRightDoor && xa >= entryFrontX - 0.02f && xb <= entryBackX + 0.02f) continue;

                float ya = evalWindowBot(xa), yb = evalWindowBot(xb);
                float yBase = archTopY - 0.01f;
                if (side > 0) {
                    CabComponents::AppendQuad(mesh, Vec3(xa, yBase, zWallOut), Vec3(xb, yBase, zWallOut), Vec3(xb, yb, zWallOut), Vec3(xa, ya, zWallOut), bodyPaint);
                    CabComponents::AppendQuad(mesh, Vec3(xb, yBase, zWallIn), Vec3(xa, yBase, zWallIn), Vec3(xa, ya, zWallIn), Vec3(xb, yb, zWallIn), glossBlack);
                } else {
                    CabComponents::AppendQuad(mesh, Vec3(xb, yBase, zWallOut), Vec3(xa, yBase, zWallOut), Vec3(xa, ya, zWallOut), Vec3(xb, yb, zWallOut), bodyPaint);
                    CabComponents::AppendQuad(mesh, Vec3(xa, yBase, zWallIn), Vec3(xb, yBase, zWallIn), Vec3(xb, yb, zWallIn), Vec3(xa, ya, zWallIn), glossBlack);
                }
                float barH = 0.032f;
                if (side > 0) {
                    CabComponents::AppendQuad(mesh, Vec3(xa, ya - barH, zChrome), Vec3(xb, yb - barH, zChrome), Vec3(xb, yb + 0.008f, zChrome), Vec3(xa, ya + 0.008f, zChrome), chromeCol);
                } else {
                    CabComponents::AppendQuad(mesh, Vec3(xb, yb - barH, zChrome), Vec3(xa, ya - barH, zChrome), Vec3(xa, ya + 0.008f, zChrome), Vec3(xb, yb + 0.008f, zChrome), chromeCol);
                }
            }

            // Second lower curved chrome spear arching over front wheel well up into beltline
            for (int i = 0; i < curveSegs; ++i) {
                float x0 = (xFrontCorner + 0.06f) + ((xBpillarTop + 0.55f) - (xFrontCorner + 0.06f)) * (static_cast<float>(i) / static_cast<float>(curveSegs));
                float x1 = (xFrontCorner + 0.06f) + ((xBpillarTop + 0.55f) - (xFrontCorner + 0.06f)) * (static_cast<float>(i + 1) / static_cast<float>(curveSegs));
                if (skipRightDoor && x0 >= entryFrontX - 0.02f && x1 <= entryBackX + 0.02f) continue;
                float sy0 = evalLowerSpearY(x0), sy1 = evalLowerSpearY(x1);
                float sh = 0.022f;
                if (side > 0) {
                    CabComponents::AppendQuad(mesh, Vec3(x0, sy0 - sh, zChrome), Vec3(x1, sy1 - sh, zChrome), Vec3(x1, sy1, zChrome), Vec3(x0, sy0, zChrome), chromeCol);
                } else {
                    CabComponents::AppendQuad(mesh, Vec3(x1, sy1 - sh, zChrome), Vec3(x0, sy0 - sh, zChrome), Vec3(x0, sy0, zChrome), Vec3(x1, sy1, zChrome), chromeCol);
                }
            }

            RunningGear::AppendBox(mesh, Vec3(xBpillarTop, archTopY - 0.01f, std::min(zWallIn, zWallOut)), Vec3(xWinEnd, yMainBot, std::max(zWallIn, zWallOut)), bodyPaint);
            RunningGear::AppendBox(mesh, Vec3(xBpillarTop + 0.50f, yMainBot - 0.092f, s > 0 ? hw : -hw - 0.015f), Vec3(xWinEnd + 0.02f, yMainBot - 0.066f, s > 0 ? hw + 0.015f : -hw), chromeCol);
            RunningGear::AppendBox(mesh, Vec3(xBpillarTop, yMainBot - 0.022f, s > 0 ? hw : -hw - 0.014f), Vec3(xWinEnd, yMainBot + 0.008f, s > 0 ? hw + 0.014f : -hw), glossBlack);

            float zMullIn = s * (hw - 0.02f), zMullOut = s * (hw + 0.004f);
            RunningGear::AppendBox(mesh, Vec3(xBpillarTop - 0.025f, yMainBot, std::min(zMullIn, zMullOut)), Vec3(xBpillarTop + 0.025f, winSideTop, std::max(zMullIn, zMullOut)), glossBlack);
            const int passengerPanes = 6;
            float span = xWinEnd - xBpillarTop;
            for (int p = 1; p < passengerPanes; ++p) {
                float px = xBpillarTop + (span * static_cast<float>(p)) / static_cast<float>(passengerPanes);
                RunningGear::AppendBox(mesh, Vec3(px - 0.014f, yMainBot, std::min(zMullIn, zMullOut)), Vec3(px + 0.014f, winSideTop, std::max(zMullIn, zMullOut)), glossBlack);
            }
        } else {
            auto evalCornerFloorX = [&](float y) -> float {
                if (y <= noseTopY) return xPillarStart;
                float t = std::clamp((y - noseTopY) / std::max(0.10f, winSideTop - noseTopY), 0.0f, 1.0f);
                return xPillarStart + 0.34f * t;
            };
            for (int i = 0; i < curveSegs; ++i) {
                float xa = xPillarStart + (xBpillarTop - xPillarStart) * (static_cast<float>(i) / static_cast<float>(curveSegs));
                float xb = xPillarStart + (xBpillarTop - xPillarStart) * (static_cast<float>(i + 1) / static_cast<float>(curveSegs));
                if (skipRightDoor && xa >= entryFrontX - 0.01f && xb <= entryBackX + 0.01f) continue;

                float yBa = evalWindowBot(xa), yBb = evalWindowBot(xb);
                float yTa = evalWindowTop(xa), yTb = evalWindowTop(xb);
                float xaBot = std::max(xa, evalCornerFloorX(yBa)), xbBot = std::max(xb, evalCornerFloorX(yBb));
                float xaTop = std::max(xa, evalCornerFloorX(yTa)), xbTop = std::max(xb, evalCornerFloorX(yTb));
                if (xbTop <= xaTop + 0.002f && xbBot <= xaBot + 0.002f) continue;
                if (side > 0) {
                    CabComponents::AppendQuad(mesh, Vec3(xaBot, yBa, zGlass), Vec3(xbBot, yBb, zGlass), Vec3(xbTop, yTb, zGlass), Vec3(xaTop, yTa, zGlass), sideGlass);
                } else {
                    CabComponents::AppendQuad(mesh, Vec3(xbBot, yBb, zGlass), Vec3(xaBot, yBa, zGlass), Vec3(xaTop, yTa, zGlass), Vec3(xbTop, yTb, zGlass), sideGlass);
                }
            }
            if (side > 0) {
                CabComponents::AppendQuad(mesh, Vec3(xBpillarTop, yMainBot, zGlass), Vec3(xWinEnd, yMainBot, zGlass), Vec3(xWinEnd, winSideTop, zGlass), Vec3(xBpillarTop, winSideTop, zGlass), sideGlass);
            } else {
                CabComponents::AppendQuad(mesh, Vec3(xWinEnd, yMainBot, zGlass), Vec3(xBpillarTop, yMainBot, zGlass), Vec3(xBpillarTop, winSideTop, zGlass), Vec3(xWinEnd, winSideTop, zGlass), sideGlass);
            }
        }
    }
}

// Sculpted convex compound-curved luxury rear transom, C-blade LED clusters, chrome V-bar, rear glass, spoiler & dual exhausts
void AppendExecutiveRearFascia(
    Mesh* mesh,
    const ExecRearSurface& rSurf,
    bool headlightsOn,
    bool emitGlassPass,
    const Vec4& bodyPaint,
    const Vec4& chromeCol,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& plateWht,
    const Vec4& euBlue,
    const Vec4& rearGlass
) {
    const Vec4 tailRed   = headlightsOn ? Vec4(0.96f, 0.12f, 0.14f, 1.0f) : Vec4(0.56f, 0.10f, 0.12f, 1.0f);
    const Vec4 tailAmber = headlightsOn ? Vec4(1.00f, 0.64f, 0.08f, 1.0f) : Vec4(0.58f, 0.36f, 0.08f, 1.0f);
    const Vec4 tailRev   = headlightsOn ? Vec4(0.92f, 0.96f, 1.00f, 1.0f) : Vec4(0.68f, 0.72f, 0.78f, 1.0f);

    if (emitGlassPass) {
        // Flush curved panoramic dark-tinted rear transom window (sleek real-world height)
        AppendExecRearPatch(mesh, rSurf, -0.72f, 0.72f, rSurf.deckFloorY + 0.68f, rSurf.winSideTop - 0.20f, 16, 4, 0.012f, rearGlass);
        return;
    }

    // 1. Compound-curved rear shell wrapping smoothly into body corners [winEndX .. rSurf]
    const int nu = 24;
    std::vector<float> yRows = {
        rSurf.skirtBotY + 0.10f,
        rSurf.skirtBotY + 0.28f,
        rSurf.skirtBotY + 0.62f,
        rSurf.deckFloorY,
        rSurf.deckFloorY + 0.68f,
        rSurf.winSideTop - 0.20f,
        rSurf.winSideTop
    };
    for (size_t j = 0; j + 1 < yRows.size(); ++j) {
        AppendExecRearPatch(mesh, rSurf, -1.0f, 1.0f, yRows[j], yRows[j + 1], nu, 1, 0.0f, bodyPaint);
    }
    // Bridge rear outer corners (-1 & +1) seamlessly forward to winEndX side walls & roof arch
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float zSide = s * rSurf.hw;
        for (size_t j = 0; j + 1 < yRows.size(); ++j) {
            Vec3 pa = rSurf.EvalRearPos(s, yRows[j]), pb = rSurf.EvalRearPos(s, yRows[j + 1]);
            if (side > 0) {
                CabComponents::AppendQuad(mesh, Vec3(rSurf.xWinEnd, pa.y, zSide), Vec3(pa.x, pa.y, zSide), Vec3(pb.x, pb.y, zSide), Vec3(rSurf.xWinEnd, pb.y, zSide), bodyPaint);
            } else {
                CabComponents::AppendQuad(mesh, Vec3(pa.x, pa.y, zSide), Vec3(rSurf.xWinEnd, pa.y, zSide), Vec3(rSurf.xWinEnd, pb.y, zSide), Vec3(pb.x, pb.y, zSide), bodyPaint);
            }
        }
    }
    // Bottom rear pan closure
    for (int i = 0; i < nu; ++i) {
        float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
        float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
        Vec3 p0 = rSurf.EvalRearPos(ua, rSurf.skirtBotY + 0.10f);
        Vec3 p1 = rSurf.EvalRearPos(ub, rSurf.skirtBotY + 0.10f);
        CabComponents::AppendQuad(mesh, p0, p1, Vec3(rSurf.xWinEnd, rSurf.skirtBotY + 0.10f, p1.z), Vec3(rSurf.xWinEnd, rSurf.skirtBotY + 0.10f, p0.z), darkTrim);
    }

    // 2. Curved upper roof aero spoiler lip + full-width slim glowing third brake light blade
    AppendExecRearRibbon(
        mesh, rSurf,
        {{-0.86f, rSurf.winSideTop - 0.06f, rSurf.winSideTop + 0.05f},
         { 0.00f, rSurf.winSideTop - 0.04f, rSurf.winSideTop + 0.07f},
         { 0.86f, rSurf.winSideTop - 0.06f, rSurf.winSideTop + 0.05f}},
        0.045f, 0.02f, bodyPaint
    );
    AppendExecRearRibbon(
        mesh, rSurf,
        {{-0.36f, rSurf.winSideTop - 0.025f, rSurf.winSideTop + 0.012f},
         { 0.00f, rSurf.winSideTop - 0.015f, rSurf.winSideTop + 0.022f},
         { 0.36f, rSurf.winSideTop - 0.025f, rSurf.winSideTop + 0.012f}},
        0.052f, 0.008f, tailRed
    );

    // 3. Curved gloss-black rear window frit bezel + center top emblem header
    AppendExecRearPatch(mesh, rSurf, -0.78f, 0.78f, rSurf.deckFloorY + 0.62f, rSurf.winSideTop - 0.15f, 16, 4, 0.007f, glossBlack);

    // 4. Gleaming curved rear chrome V-chevron mirroring the front fascia + circular badge crest
    AppendExecRearRibbon(
        mesh, rSurf,
        {{-0.78f, rSurf.deckFloorY + 0.56f, rSurf.deckFloorY + 0.61f},
         {-0.42f, rSurf.deckFloorY + 0.46f, rSurf.deckFloorY + 0.51f},
         { 0.00f, rSurf.deckFloorY + 0.38f, rSurf.deckFloorY + 0.43f},
         { 0.42f, rSurf.deckFloorY + 0.46f, rSurf.deckFloorY + 0.51f},
         { 0.78f, rSurf.deckFloorY + 0.56f, rSurf.deckFloorY + 0.61f}},
        0.018f, 0.008f, chromeCol
    );
    // Centered chrome badge oval at V apex
    AppendExecRearRibbon(
        mesh, rSurf,
        {{-0.09f, rSurf.deckFloorY + 0.44f, rSurf.deckFloorY + 0.52f},
         { 0.00f, rSurf.deckFloorY + 0.43f, rSurf.deckFloorY + 0.53f},
         { 0.09f, rSurf.deckFloorY + 0.44f, rSurf.deckFloorY + 0.52f}},
        0.020f, 0.008f, chromeCol
    );

    // 5. Recessed curved engine deck grille louvers + illuminated Euro license plate cove
    AppendExecRearPatch(mesh, rSurf, -0.44f, 0.44f, rSurf.skirtBotY + 0.42f, rSurf.deckFloorY + 0.08f, 10, 2, 0.006f, glossBlack);
    for (int slat = 0; slat < 4; ++slat) {
        float sy = rSurf.skirtBotY + 0.48f + static_cast<float>(slat) * 0.12f;
        AppendExecRearRibbon(
            mesh, rSurf,
            {{-0.40f, sy, sy + 0.020f}, {0.0f, sy, sy + 0.020f}, {0.40f, sy, sy + 0.020f}},
            0.014f, 0.006f, chromeCol
        );
    }
    // Lower diffuser + illuminated license plate
    AppendExecRearPatch(mesh, rSurf, -0.84f, 0.84f, rSurf.skirtBotY + 0.11f, rSurf.skirtBotY + 0.33f, 16, 2, 0.008f, darkTrim);
    AppendExecRearRibbon(
        mesh, rSurf,
        {{-0.21f, rSurf.skirtBotY + 0.16f, rSurf.skirtBotY + 0.26f},
         { 0.21f, rSurf.skirtBotY + 0.16f, rSurf.skirtBotY + 0.26f}},
        0.018f, 0.008f, plateWht
    );
    AppendExecRearRibbon(
        mesh, rSurf,
        {{-0.21f, rSurf.skirtBotY + 0.16f, rSurf.skirtBotY + 0.26f},
         {-0.165f, rSurf.skirtBotY + 0.16f, rSurf.skirtBotY + 0.26f}},
        0.020f, 0.006f, euBlue
    );

    // 6. Sculpted wraparound C-blade LED corner taillights & cylindrical dual chrome exhaust tips
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        // Curved gloss-black corner taillight housing
        AppendExecRearRibbon(
            mesh, rSurf,
            {{s * 0.56f, rSurf.skirtBotY + 0.48f, rSurf.skirtBotY + 0.68f},
             {s * 0.76f, rSurf.skirtBotY + 0.50f, rSurf.skirtBotY + 0.73f},
             {s * 0.93f, rSurf.skirtBotY + 0.55f, rSurf.skirtBotY + 0.80f}},
            0.014f, 0.010f, glossBlack
        );
        // Sweeping crimson LED C-blade signature
        AppendExecRearRibbon(
            mesh, rSurf,
            {{s * 0.58f, rSurf.skirtBotY + 0.59f, rSurf.skirtBotY + 0.65f},
             {s * 0.76f, rSurf.skirtBotY + 0.63f, rSurf.skirtBotY + 0.70f},
             {s * 0.91f, rSurf.skirtBotY + 0.69f, rSurf.skirtBotY + 0.77f}},
            0.021f, 0.006f, tailRed
        );
        // Lower amber turn indicator strip + inner white reverse light
        AppendExecRearRibbon(
            mesh, rSurf,
            {{s * 0.69f, rSurf.skirtBotY + 0.51f, rSurf.skirtBotY + 0.56f},
             {s * 0.90f, rSurf.skirtBotY + 0.56f, rSurf.skirtBotY + 0.62f}},
            0.021f, 0.006f, tailAmber
        );
        AppendExecRearRibbon(
            mesh, rSurf,
            {{s * 0.57f, rSurf.skirtBotY + 0.50f, rSurf.skirtBotY + 0.55f},
             {s * 0.68f, rSurf.skirtBotY + 0.51f, rSurf.skirtBotY + 0.56f}},
            0.021f, 0.006f, tailRev
        );
        // Lower corner red reflector bar
        AppendExecRearRibbon(
            mesh, rSurf,
            {{s * 0.66f, rSurf.skirtBotY + 0.17f, rSurf.skirtBotY + 0.21f},
             {s * 0.82f, rSurf.skirtBotY + 0.18f, rSurf.skirtBotY + 0.23f}},
            0.016f, 0.006f, tailRed
        );
        // Cylindrical twin chrome exhaust tips per side (chrome pipe shell with deeply recessed dark exhaust bore)
        for (float zOff : {0.58f, 0.70f}) {
            Vec3 pipeC = rSurf.EvalRearPos(s * zOff * 0.78f, rSurf.skirtBotY + 0.145f);
            float tz = s * zOff;
            ChassisComponents::AppendClosedCylinderX(
                mesh,
                rSurf.xWinEnd + 0.18f, pipeC.x + 0.055f,
                pipeC.y, tz,
                0.042f, 12, chromeCol, glossBlack, 0.45f
            );
        }
    }
}

} // namespace

std::shared_ptr<Mesh> GenerateExecutiveCoachMesh(const VehicleDef& def) {
    auto mesh = std::make_shared<Mesh>();

    const float L = std::clamp(def.dimensions.x, 12.2f, 15.0f);
    const float W = std::clamp(def.dimensions.y, 2.45f, 2.60f);
    const float H = std::clamp(def.dimensions.z, 3.60f, 4.00f);
    const float hw = W * 0.5f;
    const float wheelR = std::clamp(def.wheel_radius_m, 0.48f, 0.54f);
    const float tireW = std::clamp(def.tire_width_m, 0.28f, 0.34f);

    const float frontHalfTrack = hw - tireW * 0.50f - 0.025f;
    const float rearHalfTrack  = hw - tireW * 1.06f - 0.025f;

    const float xFront = -L * 0.5f;
    const float xRear  =  L * 0.5f;
    const float skirtBotY = std::clamp(def.ground_clearance_m, 0.24f, 0.34f);
    const float archTopY = wheelR * 2.0f + 0.14f;
    const float archHalfLen = wheelR + 0.13f;
    const float cutRadius = wheelR + 0.085f;
    const float deckFloorY = std::max(skirtBotY + 1.12f, wheelR * 2.0f + 0.34f);
    const float noseTopY = std::max(archTopY + 0.22f, skirtBotY + 1.14f);
    const float winSideTop = H - 0.40f;
    const float winTopY = winSideTop - 0.16f;
    const float winEndX = xRear - 0.60f;

    const float frontAxleX = xFront + 2.48f;
    const float driveAxleX = std::min(xRear - 2.35f, frontAxleX + def.wheelbase_m);
    const float tagAxleX   = driveAxleX + 1.38f;

    const float entryFrontX = xFront + 1.04f;
    const float entryBackX  = xFront + 1.80f;

    const Vec4 bodyPaint = def.base_color;
    const Vec4 chromeCol = def.accent_color;
    const Vec4 glossBlack(0.05f, 0.06f, 0.08f, 1.0f);
    const Vec4 darkTrim(0.09f, 0.10f, 0.12f, 1.0f);
    const Vec4 linerDark(0.045f, 0.05f, 0.06f, 1.0f);
    const Vec4 interiorSeat(0.20f, 0.24f, 0.30f, 1.0f);
    const Vec4 interiorFloor(0.16f, 0.17f, 0.20f, 1.0f);
    const Vec4 amberMarker(1.00f, 0.58f, 0.08f, 1.0f);
    const Vec4 bayDoorCol(
        std::min(1.0f, bodyPaint.x * 0.94f + 0.015f),
        std::min(1.0f, bodyPaint.y * 0.94f + 0.015f),
        std::min(1.0f, bodyPaint.z * 0.94f + 0.015f),
        1.0f
    );
    const Vec4 windshieldGlass(0.26f, 0.44f, 0.62f, 0.32f);
    const Vec4 sideGlass(0.14f, 0.26f, 0.40f, 0.50f);

    const Vec4 drlCol   = def.headlights_on ? Vec4(0.98f, 0.99f, 1.00f, 1.0f) : Vec4(0.70f, 0.75f, 0.80f, 1.0f);
    const Vec4 beamCol  = def.headlights_on ? Vec4(0.88f, 0.97f, 1.00f, 1.0f) : Vec4(0.46f, 0.54f, 0.62f, 1.0f);
    const Vec4 fogCol   = def.headlights_on ? Vec4(0.98f, 0.97f, 0.88f, 1.0f) : Vec4(0.56f, 0.60f, 0.65f, 1.0f);
    const Vec4 amberCol = def.headlights_on ? Vec4(1.00f, 0.64f, 0.08f, 1.0f) : Vec4(0.62f, 0.38f, 0.08f, 1.0f);
    const Vec4 plateWht = Vec4(0.94f, 0.95f, 0.97f, 1.0f);
    const Vec4 euBlue   = Vec4(0.08f, 0.26f, 0.74f, 1.0f);

    ExecFrontSurface surf;
    surf.Init(xFront, skirtBotY, noseTopY, winTopY, winSideTop, H, hw);
    ExecRearSurface rearSurf;
    rearSurf.Init(winEndX, xRear, skirtBotY, deckFloorY, winSideTop, H, hw);

    // 1. Frame rails, tri-axle running gear, well-cut semicircular wheel arches & chrome hub caps
    ChassisComponents::AppendFrameRails(mesh.get(), xFront + 0.45f, xRear - 0.25f, skirtBotY + 0.06f, skirtBotY + 0.24f, 0.45f, darkTrim);
    ChassisComponents::AppendVehicleAxle(mesh.get(), frontAxleX, wheelR, tireW, frontHalfTrack, false, def.steer_angle_deg);
    ChassisComponents::AppendVehicleAxle(mesh.get(), driveAxleX, wheelR, tireW, rearHalfTrack, true, 0.0f);
    ChassisComponents::AppendVehicleAxle(mesh.get(), tagAxleX,   wheelR, tireW, frontHalfTrack, false, -def.steer_angle_deg * 0.35f);

    auto emitExecWheelBay = [&](float ax, float innerZ) {
        CabComponents::AppendWheelWellLiner(mesh.get(), ax, archHalfLen, skirtBotY, archTopY, hw, innerZ, linerDark);
        CabComponents::AppendSemicircularWheelArchSkirt(mesh.get(), ax, wheelR, cutRadius, archHalfLen, skirtBotY, archTopY, -hw, false, 0.07f, bodyPaint, linerDark);
        CabComponents::AppendSemicircularWheelArchSkirt(mesh.get(), ax, wheelR, cutRadius, archHalfLen, skirtBotY, archTopY,  hw, true,  0.07f, bodyPaint, linerDark);
        CabComponents::AppendWheelArchFender(mesh.get(), ax, wheelR, cutRadius, -hw, false, 0.036f, darkTrim);
        CabComponents::AppendWheelArchFender(mesh.get(), ax, wheelR, cutRadius,  hw, true,  0.036f, darkTrim);
        AppendChromeWheelCap(mesh.get(), ax, wheelR, hw, chromeCol);
    };
    emitExecWheelBay(frontAxleX, hw - tireW * 1.15f - 0.14f);
    emitExecWheelBay(driveAxleX, hw - tireW * 2.15f - 0.14f);
    emitExecWheelBay(tagAxleX,   hw - tireW * 1.15f - 0.14f);

    // 2. Cockpit & Cabin Interior safely inset behind curved raked windshield & dome corners (zero ceiling protrusion!)
    RunningGear::AppendBox(mesh.get(), Vec3(xFront + 0.88f, deckFloorY - 0.08f, -hw + 0.14f), Vec3(xFront + 1.32f, deckFloorY, hw - 0.14f), interiorFloor);
    CabComponents::AppendDriverCockpit(mesh.get(), xFront + 0.90f, -(hw - 0.05f) * 0.54f, deckFloorY, darkTrim, interiorSeat, darkTrim);
    CabComponents::AppendBusInterior(
        mesh.get(),
        xFront + 1.30f, winEndX - 0.04f,
        deckFloorY, winSideTop - 0.08f,
        hw - 0.08f,
        entryFrontX, entryBackX,
        skirtBotY + 0.10f,
        11, false, 0.0f, 0.0f,
        interiorSeat, interiorFloor, darkTrim
    );
    float bayStartX = frontAxleX + archHalfLen + 0.12f;
    float bayEndX   = driveAxleX - archHalfLen - 0.12f;
    RunningGear::AppendBox(mesh.get(), Vec3(bayStartX, skirtBotY + 0.10f, -hw + 0.14f), Vec3(bayEndX, deckFloorY - 0.06f, hw - 0.14f), linerDark);

    // 3. SILKY-SMOOTH C1 COMPOUND-CURVED EXECUTIVE FRONT FASCIA & UNIFIED ROOF BROW DOME (PASS 1)
    {
        const int nu = 28;
        std::vector<float> yLevels = {
            skirtBotY,
            skirtBotY + 0.04f,
            skirtBotY + 0.09f,
            skirtBotY + 0.14f,
            skirtBotY + 0.19f,
            skirtBotY + (noseTopY - skirtBotY) * 0.38f,
            skirtBotY + (noseTopY - skirtBotY) * 0.62f,
            skirtBotY + (noseTopY - skirtBotY) * 0.82f,
            noseTopY
        };
        for (size_t j = 0; j + 1 < yLevels.size(); ++j) {
            float ya = yLevels[j], yb = yLevels[j + 1];
            for (int i = 0; i < nu; ++i) {
                float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
                float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
                CabComponents::AppendQuad(mesh.get(), surf.EvalFrontPos(ua, ya), surf.EvalFrontPos(ub, ya), surf.EvalFrontPos(ub, yb), surf.EvalFrontPos(ua, yb), bodyPaint);
            }
        }
        // Bottom chin floor & top dash cowl closure
        for (int i = 0; i < nu; ++i) {
            float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
            float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
            Vec3 botA = surf.EvalFrontPos(ua, skirtBotY), botB = surf.EvalFrontPos(ub, skirtBotY);
            CabComponents::AppendQuad(mesh.get(), Vec3(surf.xApillar, skirtBotY, botA.z), Vec3(surf.xApillar, skirtBotY, botB.z), botB, botA, bodyPaint);
            Vec3 topA = surf.EvalFrontPos(ua, noseTopY), topB = surf.EvalFrontPos(ub, noseTopY);
            CabComponents::AppendQuad(mesh.get(), topA, topB, Vec3(surf.xApillar + 0.14f, noseTopY, topB.z), Vec3(surf.xApillar + 0.14f, noseTopY, topA.z), darkTrim);
        }

        // Seamless lower bumper cheeks & raked black A-pillar side flanges
        for (int side : {-1, 1}) {
            float s = static_cast<float>(side);
            float zWall = s * hw;
            float ua = (side < 0) ? -1.0f : 0.87f;
            float ub = (side < 0) ? -0.87f : 1.0f;
            AppendExecFrontPatch(mesh.get(), surf, ua, ub, noseTopY - 0.01f, winTopY + 0.02f, 6, 10, 0.004f, glossBlack);

            const float xCheekBack = surf.xApillar - 0.03f;
            for (size_t j = 0; j + 1 < yLevels.size(); ++j) {
                Vec3 pa = surf.EvalFrontPos(s, yLevels[j]), pb = surf.EvalFrontPos(s, yLevels[j + 1]);
                if (side > 0) {
                    CabComponents::AppendQuad(mesh.get(), Vec3(pa.x, pa.y, zWall), Vec3(xCheekBack, pa.y, zWall), Vec3(xCheekBack, pb.y, zWall), Vec3(pb.x, pb.y, zWall), bodyPaint);
                } else {
                    CabComponents::AppendQuad(mesh.get(), Vec3(xCheekBack, pa.y, zWall), Vec3(pa.x, pa.y, zWall), Vec3(pb.x, pb.y, zWall), Vec3(xCheekBack, pb.y, zWall), bodyPaint);
                }
            }
            const int pillarSteps = 10;
            for (int j = 0; j < pillarSteps; ++j) {
                float ya = (noseTopY - 0.01f) + ((winTopY + 0.02f) - (noseTopY - 0.01f)) * (static_cast<float>(j) / static_cast<float>(pillarSteps));
                float yb = (noseTopY - 0.01f) + ((winTopY + 0.02f) - (noseTopY - 0.01f)) * (static_cast<float>(j + 1) / static_cast<float>(pillarSteps));
                Vec3 pa = surf.EvalFrontPos(s, ya), pb = surf.EvalFrontPos(s, yb);
                float xBackA = pa.x + 0.09f, xBackB = pb.x + 0.09f;
                if (side > 0) {
                    CabComponents::AppendQuad(mesh.get(), Vec3(pa.x, pa.y, zWall), Vec3(xBackA, pa.y, zWall), Vec3(xBackB, pb.y, zWall), Vec3(pb.x, pb.y, zWall), glossBlack);
                } else {
                    CabComponents::AppendQuad(mesh.get(), Vec3(xBackA, pa.y, zWall), Vec3(pa.x, pa.y, zWall), Vec3(pb.x, pb.y, zWall), Vec3(xBackB, pb.y, zWall), glossBlack);
                }
            }
        }

        // Single unified smooth aero cab dome: curved black brow collar (0..0.14) flowing C1-cleanly into bodyPaint crown (0.14..1.0)
        AppendExecDomePatch(mesh.get(), surf, -1.0f, 1.0f, 0.0f,  0.14f, 28, 3,  0.002f, glossBlack);
        AppendExecDomePatch(mesh.get(), surf, -1.0f, 1.0f, 0.14f, 1.0f,  28, 12, 0.002f, bodyPaint);

        // Twin curved wipers parked cleanly at windshield sill
        AppendExecFrontRibbon(mesh.get(), surf, {{-0.62f, noseTopY + 0.02f, noseTopY + 0.055f}, {-0.05f, noseTopY + 0.01f, noseTopY + 0.045f}}, 0.022f, 0.010f, glossBlack);
        AppendExecFrontRibbon(mesh.get(), surf, {{ 0.05f, noseTopY + 0.01f, noseTopY + 0.045f}, { 0.62f, noseTopY + 0.02f, noseTopY + 0.055f}}, 0.022f, 0.010f, glossBlack);

        // Curved gloss-black upper V-smile apron + chrome emblem bar
        AppendExecFrontRibbon(
            mesh.get(), surf,
            {{-0.90f, noseTopY - 0.07f, noseTopY + 0.015f},
             {-0.48f, noseTopY - 0.17f, noseTopY + 0.015f},
             { 0.00f, noseTopY - 0.22f, noseTopY + 0.015f},
             { 0.48f, noseTopY - 0.17f, noseTopY + 0.015f},
             { 0.90f, noseTopY - 0.07f, noseTopY + 0.015f}},
            0.010f, 0.012f, glossBlack
        );
        AppendExecFrontRibbon(
            mesh.get(), surf,
            {{-0.14f, noseTopY - 0.18f, noseTopY - 0.12f}, {0.14f, noseTopY - 0.18f, noseTopY - 0.12f}},
            0.018f, 0.006f, chromeCol
        );

        // Flowing chrome V-chevron spear converging gracefully above ACC sensor & plate
        AppendExecFrontRibbon(
            mesh.get(), surf,
            {{-0.96f, noseTopY - 0.02f, noseTopY + 0.03f},
             {-0.76f, skirtBotY + 0.72f, skirtBotY + 0.765f},
             {-0.42f, skirtBotY + 0.51f, skirtBotY + 0.555f},
             { 0.00f, skirtBotY + 0.30f, skirtBotY + 0.345f},
             { 0.42f, skirtBotY + 0.51f, skirtBotY + 0.555f},
             { 0.76f, skirtBotY + 0.72f, skirtBotY + 0.765f},
             { 0.96f, noseTopY - 0.02f, noseTopY + 0.03f}},
            0.020f, 0.008f, chromeCol
        );
        AppendExecFrontRibbon(
            mesh.get(), surf,
            {{-0.055f, skirtBotY + 0.21f, skirtBotY + 0.285f}, {0.055f, skirtBotY + 0.21f, skirtBotY + 0.285f}},
            0.014f, 0.012f, glossBlack
        );
        AppendExecFrontRibbon(
            mesh.get(), surf,
            {{-0.21f, skirtBotY + 0.10f, skirtBotY + 0.19f}, {0.21f, skirtBotY + 0.10f, skirtBotY + 0.19f}},
            0.017f, 0.008f, plateWht
        );
        AppendExecFrontRibbon(
            mesh.get(), surf,
            {{-0.21f, skirtBotY + 0.10f, skirtBotY + 0.19f}, {-0.165f, skirtBotY + 0.10f, skirtBotY + 0.19f}},
            0.019f, 0.006f, euBlue
        );
        AppendExecFrontRibbon(
            mesh.get(), surf,
            {{-0.48f, skirtBotY + 0.025f, skirtBotY + 0.070f}, {0.48f, skirtBotY + 0.025f, skirtBotY + 0.070f}},
            0.010f, 0.014f, glossBlack
        );

        // Swept crystal projector headlamps + LED DRL signature + fog air curtains
        for (int side : {-1, 1}) {
            float s = static_cast<float>(side);
            AppendExecFrontRibbon(
                mesh.get(), surf,
                {{s * 0.36f, skirtBotY + 0.34f, skirtBotY + 0.40f},
                 {s * 0.58f, skirtBotY + 0.36f, skirtBotY + 0.49f},
                 {s * 0.87f, skirtBotY + 0.42f, skirtBotY + 0.59f}},
                0.015f, 0.012f, glossBlack
            );
            AppendExecFrontRibbon(
                mesh.get(), surf,
                {{s * 0.37f, skirtBotY + 0.345f, skirtBotY + 0.370f},
                 {s * 0.58f, skirtBotY + 0.365f, skirtBotY + 0.390f},
                 {s * 0.86f, skirtBotY + 0.425f, skirtBotY + 0.450f}},
                0.023f, 0.006f, drlCol
            );
            AppendExecFrontRibbon(
                mesh.get(), surf,
                {{s * 0.37f, skirtBotY + 0.380f, skirtBotY + 0.402f},
                 {s * 0.58f, skirtBotY + 0.468f, skirtBotY + 0.490f},
                 {s * 0.86f, skirtBotY + 0.565f, skirtBotY + 0.588f}},
                0.022f, 0.006f, chromeCol
            );
            AppendExecRing(mesh.get(), surf, s * 0.53f, skirtBotY + 0.415f, 0.023f, 0.026f, 0.034f, 0.038f, 0.022f, 0.006f, 14, chromeCol);
            AppendExecDisc(mesh.get(), surf, s * 0.53f, skirtBotY + 0.415f, 0.022f, 0.025f, 0.023f, 0.006f, 14, beamCol);
            AppendExecRing(mesh.get(), surf, s * 0.68f, skirtBotY + 0.465f, 0.023f, 0.026f, 0.034f, 0.038f, 0.022f, 0.006f, 14, chromeCol);
            AppendExecDisc(mesh.get(), surf, s * 0.68f, skirtBotY + 0.465f, 0.022f, 0.025f, 0.023f, 0.006f, 14, beamCol);
            AppendExecDisc(mesh.get(), surf, s * 0.81f, skirtBotY + 0.515f, 0.020f, 0.024f, 0.023f, 0.006f, 12, amberCol);

            AppendExecFrontRibbon(
                mesh.get(), surf,
                {{s * 0.58f, skirtBotY + 0.07f, skirtBotY + 0.19f},
                 {s * 0.89f, skirtBotY + 0.09f, skirtBotY + 0.22f}},
                0.012f, 0.016f, glossBlack
            );
            AppendExecRing(mesh.get(), surf, s * 0.68f, skirtBotY + 0.14f, 0.026f, 0.030f, 0.040f, 0.045f, 0.019f, 0.008f, 14, chromeCol);
            AppendExecDisc(mesh.get(), surf, s * 0.68f, skirtBotY + 0.14f, 0.025f, 0.029f, 0.021f, 0.008f, 14, fogCol);
        }
    }

    // 4. SMOOTH ROUND-SHOULDER ROOF SHELL + FLUSH-BLENDED AERO CROWN & SHARK-FIN ANTENNA
    {
        const int nu = 28;
        struct XStation { float x; float drop; };
        std::vector<XStation> stations = {
            {surf.xDomeEnd, 0.0f},
            {winEndX,       0.0f},
            {xRear - 0.14f, 0.03f},
            {xRear,         0.06f}
        };
        for (size_t s = 0; s + 1 < stations.size(); ++s) {
            float x0 = stations[s].x, d0 = stations[s].drop;
            float x1 = stations[s + 1].x, d1 = stations[s + 1].drop;
            for (int i = 0; i < nu; ++i) {
                float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
                float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
                CabComponents::AppendQuad(
                    mesh.get(),
                    surf.EvalRoofArchPos(x0, ua, d0),
                    surf.EvalRoofArchPos(x0, ub, d0),
                    surf.EvalRoofArchPos(x1, ub, d1),
                    surf.EvalRoofArchPos(x1, ua, d1),
                    bodyPaint
                );
            }
        }
        AppendFlushAeroRoofCrown(mesh.get(), surf, xFront, xRear, H, hw, bodyPaint, glossBlack);
    }

    // 5. SIDE SKIRTS, J-HOOK WINDOW ARCHITECTURE, LUGGAGE BAYS & LOFTED GOOSENECK MIRRORS (PASS 1)
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
    CabComponents::AppendSkirtWithCutouts(mesh.get(), surf.xApillar - 0.06f, winEndX, skirtBotY, archTopY,  hw, true,  0.07f, rightSkipSkirt, bodyPaint);
    CabComponents::AppendSkirtWithCutouts(mesh.get(), surf.xApillar - 0.06f, winEndX, skirtBotY, archTopY, -hw, false, 0.07f, leftSkipSkirt,  bodyPaint);

    AppendExecutiveSideArchitecture(
        mesh.get(), surf.xApillar - 0.04f, frontAxleX, winEndX, archTopY, noseTopY, winSideTop, hw,
        entryFrontX, entryBackX, false, bodyPaint, glossBlack, chromeCol, sideGlass
    );

    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        for (float mx : {xFront + 1.38f, frontAxleX + 1.45f, (frontAxleX + driveAxleX) * 0.62f, xRear - 0.72f}) {
            RunningGear::AppendBox(mesh.get(), Vec3(mx - 0.055f, skirtBotY + 0.14f, s > 0 ? hw : -hw - 0.014f), Vec3(mx + 0.055f, skirtBotY + 0.19f, s > 0 ? hw + 0.014f : -hw), amberMarker);
        }
        for (int lv = 0; lv < 5; ++lv) {
            float ly = skirtBotY + 0.24f + static_cast<float>(lv) * 0.11f;
            RunningGear::AppendBox(mesh.get(), Vec3(xRear - 0.94f, ly, s > 0 ? hw : -hw - 0.012f), Vec3(xRear - 0.28f, ly + 0.040f, s > 0 ? hw + 0.012f : -hw), darkTrim);
        }
    }

    float baySpan = bayEndX - bayStartX;
    for (int b = 0; b < 3; ++b) {
        float bx0 = bayStartX + (baySpan * static_cast<float>(b)) / 3.0f + 0.03f;
        float bx1 = bayStartX + (baySpan * static_cast<float>(b + 1)) / 3.0f - 0.03f;
        float bayLift = std::clamp(def.action_ratio, 0.0f, 1.0f) * 0.45f;
        float zOut = hw + 0.014f + bayLift * 0.28f;
        float yTop = deckFloorY - 0.08f;
        float yBot = skirtBotY + 0.12f + bayLift * 0.48f;
        if (yTop > yBot + 0.15f) {
            RunningGear::AppendBox(mesh.get(), Vec3(bx0, yBot, -zOut), Vec3(bx1, yTop, -hw + 0.01f), bayDoorCol);
            RunningGear::AppendBox(mesh.get(), Vec3(bx0, yBot,  hw - 0.01f), Vec3(bx1, yTop,  zOut), bayDoorCol);
            float midB = (bx0 + bx1) * 0.5f;
            RunningGear::AppendBox(mesh.get(), Vec3(midB - 0.06f, yTop - 0.16f, -zOut - 0.010f), Vec3(midB + 0.06f, yTop - 0.10f, -zOut), darkTrim);
            RunningGear::AppendBox(mesh.get(), Vec3(midB - 0.06f, yTop - 0.16f,  zOut), Vec3(midB + 0.06f, yTop - 0.10f,  zOut + 0.010f), darkTrim);
        }
    }

    CabComponents::AppendAnimatedDoor(
        mesh.get(), entryFrontX, entryBackX, skirtBotY + 0.12f, winSideTop - 0.08f, hw, true,
        def.door_open_ratio, false, false, bayDoorCol, glossBlack, sideGlass
    );
    AppendExecutiveMirrors(mesh.get(), surf, hw, bodyPaint, darkTrim, chromeCol);

    // 6. BEAUTIFIED COMPOUND-CURVED REAR TRANSOM, ROOF ARCH HEADER, C-BLADE LED TAILLIGHTS & DUAL EXHAUSTS (PASS 1)
    AppendExecutiveRearFascia(
        mesh.get(), rearSurf, def.headlights_on, false,
        bodyPaint, chromeCol, glossBlack, darkTrim, plateWht, euBlue, sideGlass
    );
    {
        const int nu = 24;
        const float yHeaderBase = winSideTop;
        for (int i = 0; i < nu; ++i) {
            float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
            float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
            Vec3 r0 = rearSurf.EvalRearPos(ua, yHeaderBase);
            Vec3 r1 = rearSurf.EvalRearPos(ub, yHeaderBase);
            Vec3 t0 = surf.EvalRoofArchPos(xRear, ua, 0.06f);
            Vec3 t1 = surf.EvalRoofArchPos(xRear, ub, 0.06f);
            CabComponents::AppendQuad(mesh.get(), r0, t0, t1, r1, bodyPaint);
        }
    }

    // 7. TRANSLUCENT AUTOMOTIVE GLASS PASS (DRAWN LAST!)
    AppendExecFrontPatch(mesh.get(), surf, -0.87f, 0.87f, noseTopY, winTopY, 24, 8, -0.004f, windshieldGlass);
    AppendExecutiveSideArchitecture(
        mesh.get(), surf.xApillar - 0.04f, frontAxleX, winEndX, archTopY, noseTopY, winSideTop, hw,
        entryFrontX, entryBackX, true, bodyPaint, glossBlack, chromeCol, sideGlass
    );
    CabComponents::AppendAnimatedDoor(
        mesh.get(), entryFrontX, entryBackX, skirtBotY + 0.12f, winSideTop - 0.08f, hw, true,
        def.door_open_ratio, false, true, bayDoorCol, glossBlack, sideGlass
    );
    AppendExecutiveRearFascia(
        mesh.get(), rearSurf, def.headlights_on, true,
        bodyPaint, chromeCol, glossBlack, darkTrim, plateWht, euBlue, sideGlass
    );

    mesh->CalculateBounds();
    return mesh;
}

} // namespace sphere::vehicles::coach
