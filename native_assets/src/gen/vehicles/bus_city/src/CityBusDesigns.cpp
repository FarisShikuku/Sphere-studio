#include "CityBusDesigns.h"
#include "CabComponents.h"
#include "RunningGear.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace sphere::vehicles::city {

using sphere::trailers::gear::RunningGear;
using sphere::vehicles::cab::CabComponents;

namespace {

constexpr float kPi = 3.14159265358979323846f;

struct CityFrontSurface {
    float xFront = 0.0f;
    float xApillar = 0.0f;
    float xRoofJoin = 0.0f;
    float floorY = 0.28f;
    float noseTopY = 1.07f;
    float winSideTop = 2.76f;
    float roofTopY = 3.02f;
    float hw = 1.275f;

    float uFlat = 0.68f;
    float rCorner = 0.24f;
    float alpha = 0.30f;
    float sinA = 0.0f;
    float cosA = 0.0f;
    float zFlatEdge = 0.0f;
    float Rfront = 0.0f;
    float dxFrontBulge = 0.0f;
    float rTopDome = 0.22f;
    float roofCrown = 0.06f;

    void Init(
        float inXFront, float inXApillar,
        float inFloorY, float inNoseTopY, float inWinSideTop, float inRoofTopY,
        float inHw
    ) {
        xFront = inXFront;
        xApillar = inXApillar;
        xRoofJoin = inXFront + 0.96f;
        floorY = inFloorY;
        noseTopY = inNoseTopY;
        winSideTop = inWinSideTop;
        roofTopY = inRoofTopY;
        hw = inHw;

        sinA = std::sin(alpha);
        cosA = std::cos(alpha);
        zFlatEdge = std::max(0.45f, hw - rCorner * (1.0f - sinA));
        Rfront = zFlatEdge / sinA;
        dxFrontBulge = Rfront * (1.0f - cosA);
        rTopDome = std::min(0.24f, std::max(0.08f, hw * 0.20f));
    }

    void EvalHeightAnchors(float y, float& xNoseCenter, float& xSideAnchor) const {
        const float chinTop = floorY + 0.14f;
        if (y <= chinTop) {
            float t = std::clamp((y - floorY) / 0.14f, 0.0f, 1.0f);
            float a = t * (kPi * 0.5f);
            xNoseCenter = (xFront + 0.04f) + 0.065f * (1.0f - std::sin(a));
            xSideAnchor = xApillar + 0.025f * std::cos(a);
        } else if (y <= noseTopY) {
            float t = std::clamp((y - chinTop) / std::max(0.10f, noseTopY - chinTop), 0.0f, 1.0f);
            xNoseCenter = (xFront + 0.04f) + 0.05f * t - 0.02f * std::sin(kPi * t);
            xSideAnchor = xApillar;
        } else {
            float t = std::clamp((y - noseTopY) / std::max(0.20f, winSideTop - noseTopY), 0.0f, 1.0f);
            xNoseCenter = (xFront + 0.09f) + 0.22f * t - 0.025f * std::sin(kPi * t);
            xSideAnchor = xApillar + 0.10f * t;
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

        float wTop = std::clamp((y - noseTopY) / std::max(0.20f, winSideTop - noseTopY), 0.0f, 1.0f);
        float sCenter = std::min(1.0f, absU / uFlat);
        float yArchLift = 0.025f * (1.0f - sCenter * sCenter);
        return Vec3(x, y + wTop * yArchLift, z);
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
        Vec3 p0 = EvalFrontPos(u, winSideTop);
        Vec3 p1 = EvalRoofArchPos(xRoofJoin, u, 0.0f);
        float phi = vt * (kPi * 0.5f);
        float fx = 0.16f * vt + 0.84f * (1.0f - std::cos(phi));
        float fy = std::sin(phi);
        return Vec3(
            p0.x + (p1.x - p0.x) * fx,
            p0.y + (p1.y - p0.y) * fy,
            p0.z + (p1.z - p0.z) * fy
        );
    }

    static Vec3 SafeNormalize(const Vec3& v) {
        float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
        if (lenSq < 1e-8f) return Vec3(-1.0f, 0.0f, 0.0f);
        float inv = 1.0f / std::sqrt(lenSq);
        return Vec3(v.x * inv, v.y * inv, v.z * inv);
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
        ));
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
        ));
    }
};

struct RibbonSample {
    float u;
    float vBot;
    float vTop;
};

inline Vec3 OffsetPt(const Vec3& p, const Vec3& n, float d) {
    return Vec3(p.x + n.x * d, p.y + n.y * d, p.z + n.z * d);
}

void AppendFrontPatch(
    Mesh* mesh,
    const CityFrontSurface& surf,
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

void AppendSmoothCityNoseShell(Mesh* mesh, const CityFrontSurface& surf, const Vec4& col) {
    const int nu = 20;
    std::vector<float> yLevels = {
        surf.floorY,
        surf.floorY + 0.07f,
        surf.floorY + 0.14f,
        surf.floorY + (surf.noseTopY - surf.floorY) * 0.45f,
        surf.floorY + (surf.noseTopY - surf.floorY) * 0.75f,
        surf.noseTopY
    };
    for (size_t j = 0; j + 1 < yLevels.size(); ++j) {
        float ya = yLevels[j], yb = yLevels[j + 1];
        for (int i = 0; i < nu; ++i) {
            float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
            float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
            CabComponents::AppendQuad(mesh, surf.EvalFrontPos(ua, ya), surf.EvalFrontPos(ub, ya), surf.EvalFrontPos(ub, yb), surf.EvalFrontPos(ua, yb), col);
        }
    }
    for (int i = 0; i < nu; ++i) {
        float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
        float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
        Vec3 botA = surf.EvalFrontPos(ua, surf.floorY);
        Vec3 botB = surf.EvalFrontPos(ub, surf.floorY);
        CabComponents::AppendQuad(mesh, Vec3(surf.xApillar + 0.05f, surf.floorY, botA.z), Vec3(surf.xApillar + 0.05f, surf.floorY, botB.z), botB, botA, col);
        Vec3 topA = surf.EvalFrontPos(ua, surf.noseTopY);
        Vec3 topB = surf.EvalFrontPos(ub, surf.noseTopY);
        CabComponents::AppendQuad(mesh, topA, topB, Vec3(surf.xApillar + 0.08f, surf.noseTopY, topB.z), Vec3(surf.xApillar + 0.08f, surf.noseTopY, topA.z), col);
    }
}

void AppendSmoothCityRoofDome(
    Mesh* mesh, const CityFrontSurface& surf,
    float vStart, float vEnd, float offsetOut, int segU, int segV, const Vec4& col
) {
    int nu = std::max(4, segU), nv = std::max(2, segV);
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

void AppendBeveledFrontRibbon(
    Mesh* mesh, const CityFrontSurface& surf,
    std::vector<RibbonSample> samples, float protrudeOut, float depthIn, const Vec4& col
) {
    if (!mesh || samples.size() < 2) return;
    if (samples.front().u > samples.back().u) std::reverse(samples.begin(), samples.end());
    const size_t n = samples.size();
    std::vector<Vec3> fB(n), fT(n), bB(n), bT(n);
    for (size_t i = 0; i < n; ++i) {
        float u = samples[i].u;
        float yb = std::min(samples[i].vBot, samples[i].vTop - 0.005f);
        float yt = std::max(samples[i].vTop, yb + 0.005f);
        Vec3 pb = surf.EvalFrontPos(u, yb), nb = surf.EvalFrontNormal(u, yb);
        Vec3 pt = surf.EvalFrontPos(u, yt), nt = surf.EvalFrontNormal(u, yt);
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

void AppendBeveledDomeRibbon(
    Mesh* mesh, const CityFrontSurface& surf,
    std::vector<RibbonSample> samples, float protrudeOut, float depthIn, const Vec4& col
) {
    if (!mesh || samples.size() < 2) return;
    if (samples.front().u > samples.back().u) std::reverse(samples.begin(), samples.end());
    const size_t n = samples.size();
    std::vector<Vec3> fB(n), fT(n), bB(n), bT(n);
    for (size_t i = 0; i < n; ++i) {
        float u = samples[i].u;
        float vb = std::clamp(samples[i].vBot, 0.0f, 0.98f);
        float vt = std::clamp(samples[i].vTop, vb + 0.01f, 1.0f);
        Vec3 pb = surf.EvalDomePos(u, vb), nb = surf.EvalDomeNormal(u, vb);
        Vec3 pt = surf.EvalDomePos(u, vt), nt = surf.EvalDomeNormal(u, vt);
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

void AppendCityFrontDisc(
    Mesh* mesh, const CityFrontSurface& surf,
    float uC, float yC, float radU, float radY, float protrudeOut, float depthIn, int segs, const Vec4& col
) {
    if (!mesh || segs < 6) return;
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
        uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
        Vertex v0; v0.position = centerF;  v0.color = col;
        Vertex v1; v1.position = rimF[k];  v1.color = col;
        Vertex v2; v2.position = rimF[kn]; v2.color = col;
        mesh->vertices.push_back(v0); mesh->vertices.push_back(v1); mesh->vertices.push_back(v2);
        mesh->indices.push_back(base + 0); mesh->indices.push_back(base + 1); mesh->indices.push_back(base + 2);
        CabComponents::AppendQuad(mesh, rimB[k], rimB[kn], rimF[kn], rimF[k], col);
    }
}

void AppendCityFrontRing(
    Mesh* mesh, const CityFrontSurface& surf,
    float uC, float yC, float rInU, float rInY, float rOutU, float rOutY,
    float protrudeOut, float depthIn, int segs, const Vec4& col
) {
    if (!mesh || segs < 6) return;
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

void AppendCitySlattedGrille(
    Mesh* mesh, const CityFrontSurface& surf,
    float uHalf, float yBot, float yTop, int slats,
    const Vec4& surroundCol, const Vec4& backingCol, const Vec4& slatCol
) {
    AppendFrontPatch(mesh, surf, -uHalf, uHalf, yBot, yTop, 6, 2, 0.008f, backingCol);
    AppendBeveledFrontRibbon(mesh, surf, {{-uHalf, yTop - 0.014f, yTop + 0.014f}, {uHalf, yTop - 0.014f, yTop + 0.014f}}, 0.015f, 0.010f, surroundCol);
    AppendBeveledFrontRibbon(mesh, surf, {{-uHalf, yBot - 0.014f, yBot + 0.014f}, {uHalf, yBot - 0.014f, yBot + 0.014f}}, 0.015f, 0.010f, surroundCol);
    for (int i = 1; i <= slats; ++i) {
        float sy = yBot + (yTop - yBot) * (static_cast<float>(i) / static_cast<float>(slats + 1));
        AppendBeveledFrontRibbon(mesh, surf, {{-uHalf * 0.94f, sy - 0.009f, sy + 0.009f}, {uHalf * 0.94f, sy - 0.009f, sy + 0.009f}}, 0.014f, 0.008f, slatCol);
    }
}

void AppendCityCurvedCornerPillars(Mesh* mesh, const CityFrontSurface& surf, float yBot, float yTop, const Vec4& pillarCol) {
    for (int side : {-1, 1}) {
        float s = static_cast<float>(side);
        float ua = (side < 0) ? -1.0f : 0.87f;
        float ub = (side < 0) ? -0.87f : 1.0f;
        AppendFrontPatch(mesh, surf, ua, ub, yBot, yTop, 4, 8, 0.004f, pillarCol);

        int steps = 8;
        float zWall = s * surf.hw;
        float xBack = surf.xApillar + 0.10f;
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

struct CityRearSurface {
    float xRear = 0.0f;
    float floorY = 0.28f;
    float topY = 2.80f;
    float hwRear = 1.235f;

    void Init(float inXRear, float inFloorY, float inTopY, float inHw) {
        xRear = inXRear;
        floorY = inFloorY;
        topY = inTopY;
        hwRear = std::max(0.8f, inHw - 0.04f);
    }

    Vec3 EvalRearPos(float u, float y) const {
        float uc = std::clamp(u, -1.0f, 1.0f);
        float z = uc * (hwRear - 0.07f);
        float curveX = 0.025f * (1.0f - uc * uc);
        return Vec3(xRear + curveX, y, z);
    }

    Vec3 EvalRearNormal(float u, float y) const {
        float du = 0.02f, dy = 0.02f;
        Vec3 pu0 = EvalRearPos(u - du, y), pu1 = EvalRearPos(u + du, y);
        Vec3 py0 = EvalRearPos(u, y - dy), py1 = EvalRearPos(u, y + dy);
        Vec3 dU(pu1.x - pu0.x, pu1.y - pu0.y, pu1.z - pu0.z);
        Vec3 dY(py1.x - py0.x, py1.y - py0.y, py1.z - py0.z);
        return CityFrontSurface::SafeNormalize(Vec3(
            dY.y * dU.z - dY.z * dU.y,
            dY.z * dU.x - dY.x * dU.z,
            dY.x * dU.y - dY.y * dU.x
        ));
    }
};

void AppendCityRearPatch(
    Mesh* mesh, const CityRearSurface& surf,
    float u0, float u1, float y0, float y1, int segU, int segV, float offsetOut, const Vec4& col
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
            Vec3 a = OffsetPt(surf.EvalRearPos(ua, ya), surf.EvalRearNormal(ua, ya), offsetOut);
            Vec3 b = OffsetPt(surf.EvalRearPos(ub, ya), surf.EvalRearNormal(ub, ya), offsetOut);
            Vec3 c = OffsetPt(surf.EvalRearPos(ub, yb), surf.EvalRearNormal(ub, yb), offsetOut);
            Vec3 d = OffsetPt(surf.EvalRearPos(ua, yb), surf.EvalRearNormal(ua, yb), offsetOut);
            CabComponents::AppendQuad(mesh, b, a, d, c, col);
        }
    }
}

void AppendCityRearRibbon(
    Mesh* mesh, const CityRearSurface& surf,
    std::vector<RibbonSample> samples, float protrudeOut, float depthIn, const Vec4& col
) {
    if (!mesh || samples.size() < 2) return;
    if (samples.front().u > samples.back().u) std::reverse(samples.begin(), samples.end());
    const size_t n = samples.size();
    std::vector<Vec3> fB(n), fT(n), bB(n), bT(n);
    for (size_t i = 0; i < n; ++i) {
        float u = samples[i].u;
        float yb = std::min(samples[i].vBot, samples[i].vTop - 0.005f);
        float yt = std::max(samples[i].vTop, yb + 0.005f);
        Vec3 pb = surf.EvalRearPos(u, yb), nb = surf.EvalRearNormal(u, yb);
        Vec3 pt = surf.EvalRearPos(u, yt), nt = surf.EvalRearNormal(u, yt);
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

void AppendCityRearDisc(
    Mesh* mesh, const CityRearSurface& surf,
    float uC, float yC, float radU, float radY, float protrudeOut, float depthIn, int segs, const Vec4& col
) {
    if (!mesh || segs < 6) return;
    Vec3 pc = surf.EvalRearPos(uC, yC), nc = surf.EvalRearNormal(uC, yC);
    Vec3 centerF = OffsetPt(pc, nc, protrudeOut + 0.003f);
    std::vector<Vec3> rimF(segs), rimB(segs);
    for (int k = 0; k < segs; ++k) {
        float a = 2.0f * kPi * (static_cast<float>(k) / static_cast<float>(segs));
        float uk = uC + radU * std::cos(a), yk = yC + radY * std::sin(a);
        Vec3 pk = surf.EvalRearPos(uk, yk), nk = surf.EvalRearNormal(uk, yk);
        rimF[k] = OffsetPt(pk, nk, protrudeOut);
        rimB[k] = OffsetPt(pk, nk, -depthIn);
    }
    for (int k = 0; k < segs; ++k) {
        int kn = (k + 1) % segs;
        uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
        Vertex v0; v0.position = centerF;  v0.color = col;
        Vertex v1; v1.position = rimF[kn]; v1.color = col;
        Vertex v2; v2.position = rimF[k];  v2.color = col;
        mesh->vertices.push_back(v0); mesh->vertices.push_back(v1); mesh->vertices.push_back(v2);
        mesh->indices.push_back(base + 0); mesh->indices.push_back(base + 1); mesh->indices.push_back(base + 2);
        CabComponents::AppendQuad(mesh, rimB[kn], rimB[k], rimF[k], rimF[kn], col);
    }
}

} // namespace

void AppendCityFrontDesign(
    Mesh* mesh,
    CityBusStyle style,
    float xFront,
    float xApillar,
    float floorY,
    float noseTopY,
    float winSideTop,
    float roofTopY,
    float hw,
    bool headlightsOn,
    const Vec4& bodyPaint,
    const Vec4& accent,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol,
    const Vec4& destAmber
) {
    if (!mesh) return;
    CityFrontSurface surf;
    surf.Init(xFront, xApillar, floorY, noseTopY, winSideTop, roofTopY, hw);

    const Vec4 drlCol   = headlightsOn ? Vec4(0.97f, 0.99f, 1.00f, 1.0f) : Vec4(0.66f, 0.72f, 0.78f, 1.0f);
    const Vec4 beamCol  = headlightsOn ? Vec4(0.88f, 0.97f, 1.00f, 1.0f) : Vec4(0.46f, 0.54f, 0.62f, 1.0f);
    const Vec4 fogCol   = headlightsOn ? Vec4(0.98f, 0.96f, 0.86f, 1.0f) : Vec4(0.54f, 0.58f, 0.62f, 1.0f);
    const Vec4 amberCol = headlightsOn ? Vec4(1.00f, 0.64f, 0.08f, 1.0f) : Vec4(0.62f, 0.38f, 0.08f, 1.0f);
    const Vec4 evCyan   = headlightsOn ? Vec4(0.18f, 0.92f, 0.96f, 1.0f) : Vec4(0.14f, 0.58f, 0.64f, 1.0f);
    const Vec4 plateWht = Vec4(0.94f, 0.95f, 0.97f, 1.0f);

    // Compound-curved lower nose shell, cowl sill, smooth corner A-pillars, wipers & curved front roof dome
    AppendSmoothCityNoseShell(mesh, surf, bodyPaint);
    RunningGear::AppendBox(
        mesh,
        Vec3(xFront + 0.24f, noseTopY - 0.06f, -hw + 0.22f),
        Vec3(xApillar + 0.06f, noseTopY + 0.01f, hw - 0.22f),
        darkTrim
    );

    AppendCityCurvedCornerPillars(mesh, surf, noseTopY - 0.01f, winSideTop, glossBlack);
    AppendBeveledFrontRibbon(
        mesh, surf,
        {{-0.60f, noseTopY + 0.02f, noseTopY + 0.055f}, {-0.05f, noseTopY + 0.015f, noseTopY + 0.050f}},
        0.018f, 0.008f, glossBlack
    );
    AppendBeveledFrontRibbon(
        mesh, surf,
        {{0.05f, noseTopY + 0.015f, noseTopY + 0.050f}, {0.60f, noseTopY + 0.02f, noseTopY + 0.055f}},
        0.018f, 0.008f, glossBlack
    );

    const Vec4 domeCol = (style == CityBusStyle::ELECTRIC_LOW_FLOOR) ? glossBlack : bodyPaint;
    AppendSmoothCityRoofDome(mesh, surf, 0.0f, 1.0f, 0.002f, 20, 8, domeCol);

    switch (style) {
        case CityBusStyle::STANDARD_TRANSIT: {
            // Classic 12m municipal transit: black header brow + recessed amber route sign, twin-round chrome transit headlamps, 2-slat grille & steel bumperettes
            AppendBeveledDomeRibbon(mesh, surf, {{-0.62f, 0.08f, 0.58f}, {0.62f, 0.08f, 0.58f}}, 0.008f, 0.008f, glossBlack);
            AppendBeveledDomeRibbon(mesh, surf, {{-0.48f, 0.16f, 0.44f}, {0.0f, 0.16f, 0.44f}, {0.48f, 0.16f, 0.44f}}, 0.014f, 0.006f, destAmber);

            AppendCitySlattedGrille(mesh, surf, 0.36f, floorY + 0.28f, noseTopY - 0.14f, 2, chromeCol, glossBlack, chromeCol);
            AppendFrontPatch(mesh, surf, -0.96f, 0.96f, floorY + 0.03f, floorY + 0.22f, 12, 2, 0.016f, darkTrim);
            AppendBeveledFrontRibbon(mesh, surf, {{-0.18f, floorY + 0.07f, floorY + 0.16f}, {0.18f, floorY + 0.07f, floorY + 0.16f}}, 0.022f, 0.006f, plateWht);

            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                // Twin round chrome-ringed halogen/LED transit lamps inside dark housing
                AppendFrontPatch(mesh, surf, s * 0.44f, s * 0.86f, floorY + 0.30f, noseTopY - 0.12f, 4, 2, 0.012f, glossBlack);
                AppendCityFrontRing(mesh, surf, s * 0.57f, floorY + 0.48f, 0.036f, 0.044f, 0.052f, 0.062f, 0.020f, 0.008f, 12, chromeCol);
                AppendCityFrontDisc(mesh, surf, s * 0.57f, floorY + 0.48f, 0.035f, 0.043f, 0.021f, 0.006f, 12, beamCol);
                AppendCityFrontRing(mesh, surf, s * 0.74f, floorY + 0.48f, 0.034f, 0.042f, 0.050f, 0.060f, 0.020f, 0.008f, 12, chromeCol);
                AppendCityFrontDisc(mesh, surf, s * 0.74f, floorY + 0.48f, 0.033f, 0.041f, 0.021f, 0.006f, 12, beamCol);
                AppendCityFrontDisc(mesh, surf, s * 0.83f, floorY + 0.56f, 0.020f, 0.024f, 0.020f, 0.006f, 10, amberCol);
                // Corner heavy-duty rubber/stainless bumperette
                AppendFrontPatch(mesh, surf, s * 0.72f, s * 0.94f, floorY + 0.04f, floorY + 0.21f, 2, 2, 0.024f, chromeCol);
            }
            break;
        }

        case CityBusStyle::DOUBLE_DECKER: {
            // Two-story double-decker: prominent cream mid-deck beam with wide route display + vertical triple-LED corner pods & tow eyes
            float midBot = 2.04f;
            float midTop = 2.58f;
            AppendFrontPatch(mesh, surf, -1.0f, 1.0f, midBot, midTop, 18, 3, 0.010f, accent);
            AppendBeveledFrontRibbon(mesh, surf, {{-0.64f, midBot + 0.14f, midTop - 0.10f}, {0.64f, midBot + 0.14f, midTop - 0.10f}}, 0.014f, 0.010f, glossBlack);
            AppendBeveledFrontRibbon(mesh, surf, {{-0.54f, midBot + 0.18f, midTop - 0.14f}, {0.0f, midBot + 0.18f, midTop - 0.14f}, {0.54f, midBot + 0.18f, midTop - 0.14f}}, 0.018f, 0.006f, destAmber);

            // Asymmetric lower slatted city radiator grille + stainless tow eyes
            AppendCitySlattedGrille(mesh, surf, 0.34f, floorY + 0.14f, noseTopY - 0.16f, 4, darkTrim, glossBlack, chromeCol);
            AppendBeveledFrontRibbon(mesh, surf, {{-0.18f, floorY + 0.05f, floorY + 0.12f}, {0.18f, floorY + 0.05f, floorY + 0.12f}}, 0.022f, 0.006f, plateWht);

            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                // Vertical stacked double-decker triple projector cluster + L-DRL
                AppendFrontPatch(mesh, surf, s * 0.56f, s * 0.88f, floorY + 0.24f, noseTopY - 0.06f, 3, 4, 0.014f, glossBlack);
                AppendBeveledFrontRibbon(mesh, surf, {{s * 0.82f, floorY + 0.26f, noseTopY - 0.08f}, {s * 0.87f, floorY + 0.26f, noseTopY - 0.08f}}, 0.022f, 0.006f, drlCol);
                AppendCityFrontDisc(mesh, surf, s * 0.69f, floorY + 0.36f, 0.032f, 0.036f, 0.021f, 0.006f, 12, beamCol);
                AppendCityFrontDisc(mesh, surf, s * 0.70f, floorY + 0.50f, 0.032f, 0.036f, 0.021f, 0.006f, 12, beamCol);
                AppendCityFrontDisc(mesh, surf, s * 0.71f, floorY + 0.62f, 0.024f, 0.028f, 0.021f, 0.006f, 10, amberCol);
                AppendCityFrontDisc(mesh, surf, s * 0.68f, floorY + 0.12f, 0.028f, 0.032f, 0.020f, 0.006f, 10, fogCol);
            }
            break;
        }

        case CityBusStyle::ARTICULATED_BRT: {
            // Streamlined BRT nose: bold V-chevron black/orange radiator grille, boomerang LED headlamps, amber BRT corner roof strobes
            AppendBeveledDomeRibbon(mesh, surf, {{-0.64f, 0.10f, 0.54f}, {0.64f, 0.10f, 0.54f}}, 0.009f, 0.008f, glossBlack);
            AppendBeveledDomeRibbon(mesh, surf, {{-0.50f, 0.18f, 0.42f}, {0.50f, 0.18f, 0.42f}}, 0.015f, 0.006f, destAmber);
            for (float su : {-0.80f, 0.80f}) {
                AppendBeveledDomeRibbon(mesh, surf, {{su - 0.06f, 0.55f, 0.78f}, {su + 0.06f, 0.55f, 0.78f}}, 0.022f, 0.006f, amberCol);
            }

            // V-chevron orange accent wing across matte dark BRT mask
            AppendCitySlattedGrille(mesh, surf, 0.42f, floorY + 0.16f, noseTopY - 0.14f, 3, accent, glossBlack, darkTrim);
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.72f, noseTopY - 0.10f, noseTopY - 0.03f},
                 {-0.26f, noseTopY - 0.22f, noseTopY - 0.14f},
                 { 0.0f,  noseTopY - 0.25f, noseTopY - 0.17f},
                 { 0.26f, noseTopY - 0.22f, noseTopY - 0.14f},
                 { 0.72f, noseTopY - 0.10f, noseTopY - 0.03f}},
                0.019f, 0.008f, accent
            );
            AppendBeveledFrontRibbon(mesh, surf, {{-0.18f, floorY + 0.05f, floorY + 0.14f}, {0.18f, floorY + 0.05f, floorY + 0.14f}}, 0.020f, 0.006f, plateWht);

            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                // Upward-swept boomerang BRT headlamps
                std::vector<RibbonSample> wing = {
                    {s * 0.46f, floorY + 0.26f, floorY + 0.40f},
                    {s * 0.66f, floorY + 0.32f, floorY + 0.48f},
                    {s * 0.87f, floorY + 0.44f, floorY + 0.62f}
                };
                AppendBeveledFrontRibbon(mesh, surf, wing, 0.016f, 0.012f, glossBlack);
                AppendBeveledFrontRibbon(mesh, surf, {{s * 0.48f, floorY + 0.27f, floorY + 0.30f}, {s * 0.86f, floorY + 0.45f, floorY + 0.48f}}, 0.022f, 0.006f, amberCol);
                AppendCityFrontDisc(mesh, surf, s * 0.60f, floorY + 0.39f, 0.028f, 0.034f, 0.022f, 0.006f, 12, beamCol);
                AppendCityFrontDisc(mesh, surf, s * 0.77f, floorY + 0.49f, 0.028f, 0.034f, 0.022f, 0.006f, 12, beamCol);
                AppendCityFrontDisc(mesh, surf, s * 0.70f, floorY + 0.14f, 0.030f, 0.034f, 0.018f, 0.006f, 10, fogCol);
            }
            break;
        }

        case CityBusStyle::ELECTRIC_LOW_FLOOR:
        default: {
            // Futuristic e-BRT sealed aerodynamic nose: full-width glowing cyan/white EV lightbar + C-hook corner blades, zero open grille slats!
            AppendBeveledDomeRibbon(mesh, surf, {{-0.62f, 0.12f, 0.48f}, {0.62f, 0.12f, 0.48f}}, 0.014f, 0.006f, evCyan);
            AppendBeveledDomeRibbon(mesh, surf, {{-0.44f, 0.16f, 0.42f}, {0.44f, 0.16f, 0.42f}}, 0.017f, 0.006f, destAmber);

            // Smooth gloss-black upper EV shield spanning full width + continuous horizon LED signature bar
            AppendFrontPatch(mesh, surf, -0.94f, 0.94f, noseTopY - 0.28f, noseTopY + 0.01f, 16, 2, 0.010f, glossBlack);
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.90f, noseTopY - 0.14f, noseTopY - 0.08f},
                 { 0.0f,  noseTopY - 0.16f, noseTopY - 0.10f},
                 { 0.90f, noseTopY - 0.14f, noseTopY - 0.08f}},
                0.020f, 0.006f, drlCol
            );
            // Clean sealed aerodynamic teal chin splitter
            AppendBeveledFrontRibbon(
                mesh, surf,
                {{-0.68f, floorY + 0.04f, floorY + 0.09f},
                 { 0.0f,  floorY + 0.03f, floorY + 0.08f},
                 { 0.68f, floorY + 0.04f, floorY + 0.09f}},
                0.020f, 0.008f, evCyan
            );
            AppendBeveledFrontRibbon(mesh, surf, {{-0.18f, floorY + 0.16f, floorY + 0.25f}, {0.18f, floorY + 0.16f, floorY + 0.25f}}, 0.018f, 0.006f, plateWht);

            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                // Vertical C-blade EV corner pods
                AppendFrontPatch(mesh, surf, s * 0.62f, s * 0.90f, floorY + 0.20f, noseTopY - 0.28f, 3, 3, 0.014f, glossBlack);
                AppendBeveledFrontRibbon(mesh, surf, {{s * 0.84f, floorY + 0.22f, noseTopY - 0.26f}, {s * 0.89f, floorY + 0.22f, noseTopY - 0.26f}}, 0.022f, 0.006f, evCyan);
                AppendCityFrontDisc(mesh, surf, s * 0.72f, floorY + 0.34f, 0.028f, 0.032f, 0.022f, 0.006f, 12, beamCol);
                AppendCityFrontDisc(mesh, surf, s * 0.74f, floorY + 0.46f, 0.028f, 0.032f, 0.022f, 0.006f, 12, beamCol);
            }
            break;
        }
    }
}

void AppendCityCurvedWindshields(
    Mesh* mesh,
    CityBusStyle style,
    float xFront,
    float xApillar,
    float floorY,
    float noseTopY,
    float lowerWinTop,
    float upperWinBot,
    float upperWinTop,
    float roofTopY,
    float hw,
    const Vec4& glassColor
) {
    if (!mesh) return;
    CityFrontSurface surf;
    float fullTop = (style == CityBusStyle::DOUBLE_DECKER) ? upperWinTop : lowerWinTop;
    surf.Init(xFront, xApillar, floorY, noseTopY, fullTop, roofTopY, hw);

    if (style == CityBusStyle::DOUBLE_DECKER) {
        AppendFrontPatch(mesh, surf, -0.87f, 0.87f, noseTopY, lowerWinTop, 16, 4, -0.005f, glassColor);
        AppendFrontPatch(mesh, surf, -0.87f, 0.87f, upperWinBot, upperWinTop, 16, 4, -0.005f, glassColor);
    } else {
        AppendFrontPatch(mesh, surf, -0.87f, 0.87f, noseTopY, lowerWinTop, 16, 6, -0.005f, glassColor);
    }
}

void AppendCityRoofCanopySmooth(
    Mesh* mesh,
    float xFront,
    float xStartTrim,
    float xEndTrim,
    float winSideTop,
    float roofTopY,
    float hw,
    const Vec4& roofColor,
    bool closeFront,
    bool closeRear
) {
    if (!mesh || xEndTrim <= xStartTrim + 0.2f) return;
    CityFrontSurface surf;
    surf.Init(xFront, xFront + 0.48f, 0.28f, 1.07f, winSideTop, roofTopY, hw);

    const float xStart = (xStartTrim > surf.xRoofJoin) ? xStartTrim : surf.xRoofJoin;
    const int nu = 20;
    std::vector<float> xs = {xStart, xStart + 0.8f, xEndTrim - 0.15f, xEndTrim};
    if (xEndTrim - xStart < 1.2f) xs = {xStart, xEndTrim};

    for (size_t s = 0; s + 1 < xs.size(); ++s) {
        float x0 = xs[s], x1 = xs[s + 1];
        float d1 = (closeRear && s + 2 == xs.size()) ? 0.06f : 0.0f;
        float d0 = 0.0f;
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

    if (closeFront) {
        Vec3 frontCenter(xStart, winSideTop + 0.02f, 0.0f);
        for (int i = 0; i < nu; ++i) {
            float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
            float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
            Vec3 fa = surf.EvalRoofArchPos(xStart, ua, 0.0f);
            Vec3 fb = surf.EvalRoofArchPos(xStart, ub, 0.0f);
            uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
            Vertex v0; v0.position = frontCenter; v0.color = roofColor;
            Vertex v1; v1.position = fa;          v1.color = roofColor;
            Vertex v2; v2.position = fb;          v2.color = roofColor;
            mesh->vertices.push_back(v0); mesh->vertices.push_back(v1); mesh->vertices.push_back(v2);
            mesh->indices.push_back(base + 0); mesh->indices.push_back(base + 1); mesh->indices.push_back(base + 2);
        }
    }
    if (closeRear) {
        Vec3 rearCenter(xEndTrim, winSideTop + 0.02f, 0.0f);
        for (int i = 0; i < nu; ++i) {
            float ua = -1.0f + 2.0f * (static_cast<float>(i) / static_cast<float>(nu));
            float ub = -1.0f + 2.0f * (static_cast<float>(i + 1) / static_cast<float>(nu));
            Vec3 ra = surf.EvalRoofArchPos(xEndTrim, ua, 0.06f);
            Vec3 rb = surf.EvalRoofArchPos(xEndTrim, ub, 0.06f);
            uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
            Vertex v0; v0.position = rearCenter; v0.color = roofColor;
            Vertex v1; v1.position = rb;         v1.color = roofColor;
            Vertex v2; v2.position = ra;         v2.color = roofColor;
            mesh->vertices.push_back(v0); mesh->vertices.push_back(v1); mesh->vertices.push_back(v2);
            mesh->indices.push_back(base + 0); mesh->indices.push_back(base + 1); mesh->indices.push_back(base + 2);
        }
    }
}

void AppendCityMirrorsForStyle(
    Mesh* mesh,
    CityBusStyle style,
    float xFront,
    float winTop,
    float hw,
    const Vec4& bodyPaint,
    const Vec4& darkTrim
) {
    if (!mesh) return;
    float pivotX = xFront + 0.56f;
    if (style == CityBusStyle::ELECTRIC_LOW_FLOOR) {
        // Futuristic digital camera-mirror pods (sleek aero blades + cyan status tick)
        float camY = winTop - 0.16f;
        const Vec4 cyanCam(0.16f, 0.88f, 0.94f, 1.0f);
        for (int side : {-1, 1}) {
            float s = static_cast<float>(side);
            float zIn = s * (hw - 0.03f), zOut = s * (hw + 0.18f);
            float zMin = std::min(zIn, zOut), zMax = std::max(zIn, zOut);
            RunningGear::AppendBox(mesh, Vec3(pivotX - 0.08f, camY - 0.035f, zMin), Vec3(pivotX + 0.05f, camY + 0.035f, zMax), darkTrim);
            RunningGear::AppendBox(mesh, Vec3(pivotX - 0.10f, camY - 0.10f, s > 0 ? zOut - 0.04f : zOut - 0.06f), Vec3(pivotX + 0.02f, camY + 0.04f, s > 0 ? zOut + 0.06f : zOut + 0.04f), bodyPaint);
            RunningGear::AppendBox(mesh, Vec3(pivotX - 0.11f, camY - 0.06f, s > 0 ? zOut - 0.02f : zOut - 0.04f), Vec3(pivotX - 0.08f, camY - 0.01f, s > 0 ? zOut + 0.04f : zOut + 0.02f), cyanCam);
        }
        return;
    }

    float armY = (style == CityBusStyle::DOUBLE_DECKER) ? 2.15f : (winTop - 0.14f);
    const Vec4 armCol = (style == CityBusStyle::ARTICULATED_BRT) ? bodyPaint : darkTrim;
    CabComponents::AppendCantileverCoachMirrors(mesh, pivotX, armY, hw, armCol);
}

void AppendCityRearDesign(
    Mesh* mesh,
    CityBusStyle style,
    float xRear,
    float floorY,
    float lowerWinTop,
    float upperWinTop,
    float hw,
    bool headlightsOn,
    const Vec4& bodyPaint,
    const Vec4& accent,
    const Vec4& glossBlack,
    const Vec4& darkTrim,
    const Vec4& chromeCol
) {
    if (!mesh) return;
    CityRearSurface surf;
    float topY = (style == CityBusStyle::DOUBLE_DECKER) ? upperWinTop : lowerWinTop;
    surf.Init(xRear, floorY, topY, hw);

    const Vec4 tailRed   = headlightsOn ? Vec4(0.94f, 0.12f, 0.14f, 1.0f) : Vec4(0.56f, 0.10f, 0.12f, 1.0f);
    const Vec4 tailAmber = headlightsOn ? Vec4(1.00f, 0.62f, 0.08f, 1.0f) : Vec4(0.58f, 0.36f, 0.08f, 1.0f);
    const Vec4 revWhite  = headlightsOn ? Vec4(0.94f, 0.96f, 1.00f, 1.0f) : Vec4(0.62f, 0.65f, 0.70f, 1.0f);
    const Vec4 evCyan    = headlightsOn ? Vec4(0.18f, 0.92f, 0.96f, 1.0f) : Vec4(0.14f, 0.58f, 0.64f, 1.0f);
    const Vec4 plateWht  = Vec4(0.94f, 0.95f, 0.97f, 1.0f);

    AppendCityRearPatch(mesh, surf, -0.94f, 0.94f, floorY + 0.04f, floorY + 0.26f, 10, 2, 0.018f, (style == CityBusStyle::ELECTRIC_LOW_FLOOR) ? bodyPaint : darkTrim);
    AppendCityRearRibbon(mesh, surf, {{-0.18f, floorY + 0.10f, floorY + 0.19f}, {0.18f, floorY + 0.10f, floorY + 0.19f}}, 0.024f, 0.008f, plateWht);

    switch (style) {
        case CityBusStyle::STANDARD_TRANSIT: {
            // Classic municipal rear radiator mesh + stacked circular triple municipal tail lamps + upper warning brake bar
            AppendCityRearPatch(mesh, surf, -0.58f, 0.58f, floorY + 0.32f, floorY + 0.98f, 6, 3, 0.012f, darkTrim);
            AppendCityRearPatch(mesh, surf, -0.74f, 0.74f, floorY + 1.12f, lowerWinTop - 0.18f, 8, 3, 0.008f, glossBlack);
            AppendCityRearRibbon(mesh, surf, {{-0.86f, floorY + 1.02f, floorY + 1.10f}, {0.86f, floorY + 1.02f, floorY + 1.10f}}, 0.018f, 0.006f, accent);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                AppendCityRearDisc(mesh, surf, s * 0.80f, floorY + 0.44f, 0.040f, 0.044f, 0.024f, 0.006f, 12, tailRed);
                AppendCityRearDisc(mesh, surf, s * 0.80f, floorY + 0.62f, 0.040f, 0.044f, 0.024f, 0.006f, 12, tailAmber);
                AppendCityRearDisc(mesh, surf, s * 0.80f, floorY + 0.80f, 0.032f, 0.036f, 0.024f, 0.006f, 10, revWhite);
            }
            AppendCityRearRibbon(mesh, surf, {{-0.34f, lowerWinTop - 0.08f, lowerWinTop - 0.02f}, {0.34f, lowerWinTop - 0.08f, lowerWinTop - 0.02f}}, 0.020f, 0.006f, tailAmber);
            break;
        }

        case CityBusStyle::DOUBLE_DECKER: {
            // Two-tier rear: cream mid-deck band wrapping around rear, offset upper window + tall lower/mid LED tail blades
            AppendCityRearRibbon(mesh, surf, {{-0.94f, 2.08f, 2.46f}, {0.94f, 2.08f, 2.46f}}, 0.016f, 0.008f, accent);
            AppendCityRearPatch(mesh, surf, -0.18f, 0.74f, 2.66f, upperWinTop - 0.18f, 6, 3, 0.008f, glossBlack);
            AppendCityRearPatch(mesh, surf, -0.58f, 0.58f, floorY + 0.34f, floorY + 1.38f, 6, 3, 0.014f, darkTrim);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                AppendCityRearPatch(mesh, surf, s * 0.72f, s * 0.90f, floorY + 0.36f, 1.92f, 3, 4, 0.016f, glossBlack);
                AppendCityRearRibbon(mesh, surf, {{s * 0.82f, floorY + 0.42f, 1.86f}, {s * 0.88f, floorY + 0.42f, 1.86f}}, 0.024f, 0.006f, tailRed);
                AppendCityRearDisc(mesh, surf, s * 0.77f, floorY + 0.62f, 0.028f, 0.034f, 0.024f, 0.006f, 10, tailAmber);
                AppendCityRearDisc(mesh, surf, s * 0.77f, floorY + 0.46f, 0.024f, 0.028f, 0.024f, 0.006f, 10, revWhite);
            }
            AppendCityRearRibbon(mesh, surf, {{-0.40f, upperWinTop - 0.08f, upperWinTop - 0.02f}, {0.40f, upperWinTop - 0.08f, upperWinTop - 0.02f}}, 0.022f, 0.006f, tailRed);
            break;
        }

        case CityBusStyle::ARTICULATED_BRT: {
            // Heavy articulated BRT rear: diagonal orange warning stripes on bumper, tall radiator grille & swept angled LED blades
            AppendCityRearPatch(mesh, surf, -0.64f, 0.64f, floorY + 0.32f, floorY + 1.12f, 6, 3, 0.014f, darkTrim);
            AppendCityRearPatch(mesh, surf, -0.78f, 0.78f, floorY + 1.24f, lowerWinTop - 0.16f, 8, 3, 0.008f, glossBlack);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                AppendCityRearRibbon(mesh, surf, {{s * 0.46f, floorY + 0.08f, floorY + 0.22f}, {s * 0.88f, floorY + 0.14f, floorY + 0.24f}}, 0.024f, 0.006f, accent);
                AppendCityRearRibbon(mesh, surf, {{s * 0.56f, floorY + 0.38f, floorY + 0.50f}, {s * 0.88f, floorY + 0.52f, floorY + 0.72f}}, 0.024f, 0.006f, tailRed);
                AppendCityRearDisc(mesh, surf, s * 0.78f, floorY + 0.86f, 0.032f, 0.036f, 0.024f, 0.006f, 10, tailAmber);
            }
            AppendCityRearRibbon(mesh, surf, {{-0.44f, lowerWinTop - 0.06f, lowerWinTop}, {0.44f, lowerWinTop - 0.06f, lowerWinTop}}, 0.022f, 0.006f, accent);
            break;
        }

        case CityBusStyle::ELECTRIC_LOW_FLOOR:
        default: {
            // Futuristic sealed e-BRT rear: continuous connected full-width crimson EV taillight bar + cyan diffuser accent
            AppendCityRearPatch(mesh, surf, -0.82f, 0.82f, floorY + 1.10f, lowerWinTop - 0.16f, 10, 3, 0.008f, glossBlack);
            AppendCityRearRibbon(mesh, surf, {{-0.62f, floorY + 0.06f, floorY + 0.10f}, {0.62f, floorY + 0.06f, floorY + 0.10f}}, 0.024f, 0.006f, evCyan);
            AppendCityRearRibbon(mesh, surf, {{-0.88f, floorY + 0.86f, floorY + 0.96f}, {0.0f, floorY + 0.84f, floorY + 0.94f}, {0.88f, floorY + 0.86f, floorY + 0.96f}}, 0.024f, 0.008f, tailRed);
            for (int side : {-1, 1}) {
                float s = static_cast<float>(side);
                AppendCityRearRibbon(mesh, surf, {{s * 0.82f, floorY + 0.36f, floorY + 0.96f}, {s * 0.89f, floorY + 0.36f, floorY + 0.96f}}, 0.024f, 0.006f, tailRed);
                AppendCityRearDisc(mesh, surf, s * 0.72f, floorY + 0.56f, 0.026f, 0.030f, 0.022f, 0.006f, 10, tailAmber);
                AppendCityRearDisc(mesh, surf, s * 0.72f, floorY + 0.42f, 0.024f, 0.026f, 0.022f, 0.006f, 10, revWhite);
            }
            AppendCityRearRibbon(mesh, surf, {{-0.34f, lowerWinTop - 0.06f, lowerWinTop}, {0.34f, lowerWinTop - 0.06f, lowerWinTop}}, 0.022f, 0.006f, chromeCol);
            break;
        }
    }
}

} // namespace sphere::vehicles::city
