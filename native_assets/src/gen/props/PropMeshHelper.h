#pragma once

#include "../IGenerator.h"
#include "../shared/math/MathTypes.h"
#include <vector>
#include <cmath>
#include <cstdint>
#include <algorithm>

namespace procedural::props {

struct PropVertex {
    Vec3 position;
    Vec3 normal;
    float colorR = 0.8f;
    float colorG = 0.8f;
    float colorB = 0.8f;
};

class PropMeshHelper {
public:
    static constexpr float kPi = 3.14159265358979323846f;
    static constexpr uint32_t kVertexStrideFloats = 9; // pos(3), normal(3), color(3)

    static void appendQuad(
        GeneratedMeshPayload& payload,
        const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3,
        const Vec3& normal,
        const Vec2& /*uv0*/ = {0.0f, 0.0f}, const Vec2& /*uv1*/ = {1.0f, 0.0f},
        const Vec2& /*uv2*/ = {1.0f, 1.0f}, const Vec2& /*uv3*/ = {0.0f, 1.0f},
        uint32_t color = 0xFFCCCCCC
    ) {
        payload.vertexStride = kVertexStrideFloats * sizeof(float);
        uint32_t baseIdx = payload.vertexCount;

        float cr = ((color >> 16) & 0xFF) / 255.0f;
        float cg = ((color >> 8) & 0xFF) / 255.0f;
        float cb = (color & 0xFF) / 255.0f;

        auto addV = [&](const Vec3& p) {
            payload.vertexBuffer.push_back(p.x);
            payload.vertexBuffer.push_back(p.y);
            payload.vertexBuffer.push_back(p.z);
            payload.vertexBuffer.push_back(normal.x);
            payload.vertexBuffer.push_back(normal.y);
            payload.vertexBuffer.push_back(normal.z);
            payload.vertexBuffer.push_back(cr);
            payload.vertexBuffer.push_back(cg);
            payload.vertexBuffer.push_back(cb);
            payload.vertexCount++;
        };

        addV(p0);
        addV(p1);
        addV(p2);
        addV(p3);

        payload.indexBuffer.push_back(baseIdx + 0);
        payload.indexBuffer.push_back(baseIdx + 1);
        payload.indexBuffer.push_back(baseIdx + 2);
        payload.indexBuffer.push_back(baseIdx + 0);
        payload.indexBuffer.push_back(baseIdx + 2);
        payload.indexBuffer.push_back(baseIdx + 3);
        payload.indexCount += 6;
    }

    static void appendQuad(
        GeneratedMeshPayload& payload,
        const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3,
        uint32_t color
    ) {
        Vec3 normal = (p1 - p0).cross(p2 - p0).normalized();
        appendQuad(payload, p0, p1, p2, p3, normal, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, color);
    }

    static void appendBox(
        GeneratedMeshPayload& payload,
        const Vec3& center,
        const Vec3& size,
        uint32_t color = 0xFF888888,
        float rotY = 0.0f
    ) {
        payload.vertexStride = kVertexStrideFloats * sizeof(float);
        float hx = size.x * 0.5f;
        float hy = size.y * 0.5f;
        float hz = size.z * 0.5f;

        auto rot = [rotY](const Vec3& p) -> Vec3 {
            if (std::abs(rotY) < 1e-5f) return p;
            float c = std::cos(rotY);
            float s = std::sin(rotY);
            return {p.x * c - p.z * s, p.y, p.x * s + p.z * c};
        };

        Vec3 c = center;
        Vec3 p000 = c + rot(Vec3{-hx, -hy, -hz});
        Vec3 p100 = c + rot(Vec3{ hx, -hy, -hz});
        Vec3 p110 = c + rot(Vec3{ hx,  hy, -hz});
        Vec3 p010 = c + rot(Vec3{-hx,  hy, -hz});
        Vec3 p001 = c + rot(Vec3{-hx, -hy,  hz});
        Vec3 p101 = c + rot(Vec3{ hx, -hy,  hz});
        Vec3 p111 = c + rot(Vec3{ hx,  hy,  hz});
        Vec3 p011 = c + rot(Vec3{-hx,  hy,  hz});

        Vec3 nFront = rot(Vec3{0, 0, 1}).normalized();
        Vec3 nBack  = rot(Vec3{0, 0, -1}).normalized();
        Vec3 nLeft  = rot(Vec3{-1, 0, 0}).normalized();
        Vec3 nRight = rot(Vec3{1, 0, 0}).normalized();
        Vec3 nTop   = rot(Vec3{0, 1, 0}).normalized();
        Vec3 nBot   = rot(Vec3{0, -1, 0}).normalized();

        // Front face (+Z)
        appendQuad(payload, p001, p101, p111, p011, nFront, {0,0}, {1,0}, {1,1}, {0,1}, color);
        // Back face (-Z)
        appendQuad(payload, p100, p000, p010, p110, nBack, {0,0}, {1,0}, {1,1}, {0,1}, color);
        // Left face (-X)
        appendQuad(payload, p000, p001, p011, p010, nLeft, {0,0}, {1,0}, {1,1}, {0,1}, color);
        // Right face (+X)
        appendQuad(payload, p101, p100, p110, p111, nRight, {0,0}, {1,0}, {1,1}, {0,1}, color);
        // Top face (+Y)
        appendQuad(payload, p011, p111, p110, p010, nTop, {0,0}, {1,0}, {1,1}, {0,1}, color);
        // Bottom face (-Y)
        appendQuad(payload, p000, p100, p101, p001, nBot, {0,0}, {1,0}, {1,1}, {0,1}, color);
    }

    static void appendStrut(
        GeneratedMeshPayload& payload,
        const Vec3& p0,
        const Vec3& p1,
        float thickness = 0.05f,
        uint32_t color = 0xFFCCCCCC
    ) {
        Vec3 d = p1 - p0;
        float len = d.length();
        if (len < 1e-4f) return;
        Vec3 n = d * (1.0f / len);
        Vec3 ref = (std::abs(n.y) < 0.9f) ? Vec3{0.0f, 1.0f, 0.0f} : Vec3{1.0f, 0.0f, 0.0f};
        Vec3 u = n.cross(ref).normalized() * (thickness * 0.5f);
        Vec3 v = n.cross(u).normalized() * (thickness * 0.5f);

        Vec3 p00 = p0 - u - v;
        Vec3 p01 = p0 + u - v;
        Vec3 p02 = p0 + u + v;
        Vec3 p03 = p0 - u + v;

        Vec3 p10 = p1 - u - v;
        Vec3 p11 = p1 + u - v;
        Vec3 p12 = p1 + u + v;
        Vec3 p13 = p1 - u + v;

        appendQuad(payload, p00, p01, p11, p10, (-v).normalized(), {0,0}, {1,0}, {1,1}, {0,1}, color);
        appendQuad(payload, p01, p02, p12, p11,  u.normalized(), {0,0}, {1,0}, {1,1}, {0,1}, color);
        appendQuad(payload, p02, p03, p13, p12,  v.normalized(), {0,0}, {1,0}, {1,1}, {0,1}, color);
        appendQuad(payload, p03, p00, p10, p13, (-u).normalized(), {0,0}, {1,0}, {1,1}, {0,1}, color);
    }

    // Convenient uniform-radius cylinder overload
    static void appendCylinder(
        GeneratedMeshPayload& payload,
        const Vec3& baseCenter,
        float radius,
        float height,
        uint32_t color = 0xFF777777,
        int segments = 32,
        bool capBottom = true,
        bool capTop = true
    ) {
        appendCylinder(payload, baseCenter, radius, radius, height, segments, color, capBottom, capTop);
    }

    static void appendCylinder(
        GeneratedMeshPayload& payload,
        const Vec3& baseCenter,
        float radiusBottom,
        float radiusTop,
        float height,
        int segments = 32,
        uint32_t color = 0xFF777777,
        bool capBottom = true,
        bool capTop = true
    ) {
        payload.vertexStride = kVertexStrideFloats * sizeof(float);
        if (segments < 3) segments = 3;
        // Preserve intentional triangular/square/pentagonal/hexagonal prisms (segments <= 6),
        // and double/smooth circular & conical primitives (segments >= 8)
        if (segments >= 8 && segments < 32) {
            segments = std::min(48, std::max(32, segments * 2));
        }
        float angleStep = 2.0f * kPi / static_cast<float>(segments);

        float cr = ((color >> 16) & 0xFF) / 255.0f;
        float cg = ((color >> 8) & 0xFF) / 255.0f;
        float cb = (color & 0xFF) / 255.0f;

        // Side quads
        for (int i = 0; i < segments; ++i) {
            float a0 = i * angleStep;
            float a1 = (i + 1) * angleStep;
            float c0 = std::cos(a0), s0 = std::sin(a0);
            float c1 = std::cos(a1), s1 = std::sin(a1);

            Vec3 b0 = baseCenter + Vec3{c0 * radiusBottom, 0.0f, s0 * radiusBottom};
            Vec3 b1 = baseCenter + Vec3{c1 * radiusBottom, 0.0f, s1 * radiusBottom};
            Vec3 t0 = baseCenter + Vec3{c0 * radiusTop, height, s0 * radiusTop};
            Vec3 t1 = baseCenter + Vec3{c1 * radiusTop, height, s1 * radiusTop};

            Vec3 norm = Vec3{(c0 + c1) * 0.5f, (radiusBottom - radiusTop) / std::max(height, 0.001f), (s0 + s1) * 0.5f}.normalized();

            appendQuad(payload, b0, b1, t1, t0, norm, {0, 0}, {1, 0}, {1, 1}, {0, 1}, color);
        }

        // Top Cap
        if (capTop && radiusTop > 0.001f) {
            Vec3 topCenter = baseCenter + Vec3{0.0f, height, 0.0f};
            Vec3 norm{0.0f, 1.0f, 0.0f};
            uint32_t cIdx = payload.vertexCount;
            payload.vertexBuffer.push_back(topCenter.x);
            payload.vertexBuffer.push_back(topCenter.y);
            payload.vertexBuffer.push_back(topCenter.z);
            payload.vertexBuffer.push_back(norm.x);
            payload.vertexBuffer.push_back(norm.y);
            payload.vertexBuffer.push_back(norm.z);
            payload.vertexBuffer.push_back(cr);
            payload.vertexBuffer.push_back(cg);
            payload.vertexBuffer.push_back(cb);
            payload.vertexCount++;

            uint32_t rimStart = payload.vertexCount;
            for (int i = 0; i <= segments; ++i) {
                float a = i * angleStep;
                float c = std::cos(a), s = std::sin(a);
                Vec3 p = topCenter + Vec3{c * radiusTop, 0.0f, s * radiusTop};
                payload.vertexBuffer.push_back(p.x);
                payload.vertexBuffer.push_back(p.y);
                payload.vertexBuffer.push_back(p.z);
                payload.vertexBuffer.push_back(norm.x);
                payload.vertexBuffer.push_back(norm.y);
                payload.vertexBuffer.push_back(norm.z);
                payload.vertexBuffer.push_back(cr);
                payload.vertexBuffer.push_back(cg);
                payload.vertexBuffer.push_back(cb);
                payload.vertexCount++;
            }

            for (int i = 0; i < segments; ++i) {
                payload.indexBuffer.push_back(cIdx);
                payload.indexBuffer.push_back(rimStart + i);
                payload.indexBuffer.push_back(rimStart + i + 1);
                payload.indexCount += 3;
            }
        }

        // Bottom Cap
        if (capBottom && radiusBottom > 0.001f) {
            Vec3 botCenter = baseCenter;
            Vec3 norm{0.0f, -1.0f, 0.0f};
            uint32_t cIdx = payload.vertexCount;
            payload.vertexBuffer.push_back(botCenter.x);
            payload.vertexBuffer.push_back(botCenter.y);
            payload.vertexBuffer.push_back(botCenter.z);
            payload.vertexBuffer.push_back(norm.x);
            payload.vertexBuffer.push_back(norm.y);
            payload.vertexBuffer.push_back(norm.z);
            payload.vertexBuffer.push_back(cr);
            payload.vertexBuffer.push_back(cg);
            payload.vertexBuffer.push_back(cb);
            payload.vertexCount++;

            uint32_t rimStart = payload.vertexCount;
            for (int i = 0; i <= segments; ++i) {
                float a = i * angleStep;
                float c = std::cos(a), s = std::sin(a);
                Vec3 p = botCenter + Vec3{c * radiusBottom, 0.0f, s * radiusBottom};
                payload.vertexBuffer.push_back(p.x);
                payload.vertexBuffer.push_back(p.y);
                payload.vertexBuffer.push_back(p.z);
                payload.vertexBuffer.push_back(norm.x);
                payload.vertexBuffer.push_back(norm.y);
                payload.vertexBuffer.push_back(norm.z);
                payload.vertexBuffer.push_back(cr);
                payload.vertexBuffer.push_back(cg);
                payload.vertexBuffer.push_back(cb);
                payload.vertexCount++;
            }

            for (int i = 0; i < segments; ++i) {
                payload.indexBuffer.push_back(cIdx);
                payload.indexBuffer.push_back(rimStart + i + 1);
                payload.indexBuffer.push_back(rimStart + i);
                payload.indexCount += 3;
            }
        }
    }

    // Continuous cylinder between arbitrary 3D endpoints p0 and p1
    static void appendOrientedCylinder(
        GeneratedMeshPayload& payload,
        const Vec3& p0,
        const Vec3& p1,
        float radius0,
        float radius1,
        int segments = 16,
        uint32_t color = 0xFF777777,
        bool cap0 = true,
        bool cap1 = true
    ) {
        Vec3 dir = p1 - p0;
        float len = dir.length();
        if (len < 1e-4f) return;
        Vec3 t = dir * (1.0f / len);

        Vec3 ref = (std::abs(t.y) < 0.9f) ? Vec3{0.0f, 1.0f, 0.0f} : Vec3{1.0f, 0.0f, 0.0f};
        Vec3 u = t.cross(ref).normalized();
        Vec3 v = t.cross(u).normalized();

        payload.vertexStride = kVertexStrideFloats * sizeof(float);
        if (segments < 3) segments = 3;
        if (segments >= 8 && segments < 24) {
            segments = std::min(36, std::max(24, segments * 2));
        }
        float angleStep = 2.0f * kPi / static_cast<float>(segments);

        float cr = ((color >> 16) & 0xFF) / 255.0f;
        float cg = ((color >> 8) & 0xFF) / 255.0f;
        float cb = (color & 0xFF) / 255.0f;

        auto addVertex = [&](const Vec3& pos, const Vec3& norm) -> uint32_t {
            payload.vertexBuffer.push_back(pos.x);
            payload.vertexBuffer.push_back(pos.y);
            payload.vertexBuffer.push_back(pos.z);
            payload.vertexBuffer.push_back(norm.x);
            payload.vertexBuffer.push_back(norm.y);
            payload.vertexBuffer.push_back(norm.z);
            payload.vertexBuffer.push_back(cr);
            payload.vertexBuffer.push_back(cg);
            payload.vertexBuffer.push_back(cb);
            return payload.vertexCount++;
        };

        // Side quads
        for (int i = 0; i < segments; ++i) {
            float a0 = i * angleStep;
            float a1 = (i + 1) * angleStep;
            float cos0 = std::cos(a0), sin0 = std::sin(a0);
            float cos1 = std::cos(a1), sin1 = std::sin(a1);

            Vec3 radDir0 = u * cos0 + v * sin0;
            Vec3 radDir1 = u * cos1 + v * sin1;

            Vec3 b0 = p0 + radDir0 * radius0;
            Vec3 b1 = p0 + radDir1 * radius0;
            Vec3 t1 = p1 + radDir1 * radius1;
            Vec3 t0 = p1 + radDir0 * radius1;

            Vec3 norm = ((radDir0 + radDir1) * 0.5f).normalized();

            appendQuad(payload, b0, b1, t1, t0, norm, {0, 0}, {1, 0}, {1, 1}, {0, 1}, color);
        }

        // Cap at p0
        if (cap0 && radius0 > 0.001f) {
            Vec3 norm = -t;
            uint32_t cIdx = addVertex(p0, norm);
            uint32_t rimStart = payload.vertexCount;
            for (int i = 0; i <= segments; ++i) {
                float a = i * angleStep;
                Vec3 p = p0 + (u * std::cos(a) + v * std::sin(a)) * radius0;
                addVertex(p, norm);
            }
            for (int i = 0; i < segments; ++i) {
                payload.indexBuffer.push_back(cIdx);
                payload.indexBuffer.push_back(rimStart + i + 1);
                payload.indexBuffer.push_back(rimStart + i);
                payload.indexCount += 3;
            }
        }

        // Cap at p1
        if (cap1 && radius1 > 0.001f) {
            Vec3 norm = t;
            uint32_t cIdx = addVertex(p1, norm);
            uint32_t rimStart = payload.vertexCount;
            for (int i = 0; i <= segments; ++i) {
                float a = i * angleStep;
                Vec3 p = p1 + (u * std::cos(a) + v * std::sin(a)) * radius1;
                addVertex(p, norm);
            }
            for (int i = 0; i < segments; ++i) {
                payload.indexBuffer.push_back(cIdx);
                payload.indexBuffer.push_back(rimStart + i);
                payload.indexBuffer.push_back(rimStart + i + 1);
                payload.indexCount += 3;
            }
        }
    }

    // Continuous bent cylinder swept along mathematical catenary sag curve
    // C(t) = (1 - t) * p0 + t * p1 + Vec3{0, catenarySag(t), 0}
    // catenarySag(t) = -sag * (cosh(0.5 / a) - cosh((t - 0.5) / a)) / (cosh(0.5 / a) - 1.0)
    static void appendBentCylinderCatenary(
        GeneratedMeshPayload& payload,
        const Vec3& p0,
        const Vec3& p1,
        float sag = 0.85f,
        float radius = 0.02f,
        int lengthSegments = 32,
        int radialSegments = 12,
        uint32_t color = 0xFF222222
    ) {
        lengthSegments = std::max(lengthSegments, 32);
        radialSegments = std::max(radialSegments, 12);

        payload.vertexStride = kVertexStrideFloats * sizeof(float);
        float cr = ((color >> 16) & 0xFF) / 255.0f;
        float cg = ((color >> 8) & 0xFF) / 255.0f;
        float cb = (color & 0xFF) / 255.0f;

        auto addVertex = [&](const Vec3& pos, const Vec3& norm) -> uint32_t {
            payload.vertexBuffer.push_back(pos.x);
            payload.vertexBuffer.push_back(pos.y);
            payload.vertexBuffer.push_back(pos.z);
            payload.vertexBuffer.push_back(norm.x);
            payload.vertexBuffer.push_back(norm.y);
            payload.vertexBuffer.push_back(norm.z);
            payload.vertexBuffer.push_back(cr);
            payload.vertexBuffer.push_back(cg);
            payload.vertexBuffer.push_back(cb);
            return payload.vertexCount++;
        };

        const float a = 0.85f;
        const float coshHalf = std::cosh(0.5f / a);
        const float denom = std::max(coshHalf - 1.0f, 1e-4f);

        auto evaluateCatenaryPoint = [&](float t) -> Vec3 {
            Vec3 linear = p0 * (1.0f - t) + p1 * t;
            float u = t - 0.5f;
            float dy = -sag * (coshHalf - std::cosh(u / a)) / denom;
            return Vec3{linear.x, linear.y + dy, linear.z};
        };

        auto evaluateCatenaryTangent = [&](float t) -> Vec3 {
            Vec3 dLin = p1 - p0;
            float u = t - 0.5f;
            float ddy = (sag / (a * denom)) * std::sinh(u / a);
            Vec3 tan{dLin.x, dLin.y + ddy, dLin.z};
            float len = tan.length();
            return (len > 1e-5f) ? (tan * (1.0f / len)) : Vec3{0.0f, 0.0f, 1.0f};
        };

        float angleStep = 2.0f * kPi / static_cast<float>(radialSegments);
        std::vector<std::vector<uint32_t>> rings(lengthSegments + 1);

        for (int i = 0; i <= lengthSegments; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(lengthSegments);
            Vec3 center = evaluateCatenaryPoint(t);
            Vec3 tangent = evaluateCatenaryTangent(t);

            Vec3 ref = (std::abs(tangent.y) < 0.92f) ? Vec3{0.0f, 1.0f, 0.0f} : Vec3{1.0f, 0.0f, 0.0f};
            Vec3 normal = tangent.cross(ref).normalized();
            Vec3 binormal = tangent.cross(normal).normalized();

            rings[i].resize(radialSegments);
            for (int j = 0; j < radialSegments; ++j) {
                float theta = j * angleStep;
                Vec3 rad = normal * std::cos(theta) + binormal * std::sin(theta);
                Vec3 pos = center + rad * radius;
                rings[i][j] = addVertex(pos, rad);
            }
        }

        // Connect rings
        for (int i = 0; i < lengthSegments; ++i) {
            for (int j = 0; j < radialSegments; ++j) {
                int nextJ = (j + 1) % radialSegments;
                uint32_t v0 = rings[i][j];
                uint32_t v1 = rings[i][nextJ];
                uint32_t v2 = rings[i + 1][nextJ];
                uint32_t v3 = rings[i + 1][j];

                payload.indexBuffer.push_back(v0);
                payload.indexBuffer.push_back(v1);
                payload.indexBuffer.push_back(v2);

                payload.indexBuffer.push_back(v0);
                payload.indexBuffer.push_back(v2);
                payload.indexBuffer.push_back(v3);
                payload.indexCount += 6;
            }
        }

        // Cap start (t=0)
        Vec3 startTan = evaluateCatenaryTangent(0.0f);
        uint32_t c0 = addVertex(evaluateCatenaryPoint(0.0f), -startTan);
        uint32_t rim0Start = rings[0][0];
        (void)rim0Start;
        for (int j = 0; j < radialSegments; ++j) {
            int nextJ = (j + 1) % radialSegments;
            payload.indexBuffer.push_back(c0);
            payload.indexBuffer.push_back(rings[0][nextJ]);
            payload.indexBuffer.push_back(rings[0][j]);
            payload.indexCount += 3;
        }

        // Cap end (t=1)
        Vec3 endTan = evaluateCatenaryTangent(1.0f);
        uint32_t c1 = addVertex(evaluateCatenaryPoint(1.0f), endTan);
        for (int j = 0; j < radialSegments; ++j) {
            int nextJ = (j + 1) % radialSegments;
            payload.indexBuffer.push_back(c1);
            payload.indexBuffer.push_back(rings[lengthSegments][j]);
            payload.indexBuffer.push_back(rings[lengthSegments][nextJ]);
            payload.indexCount += 3;
        }
    }

    static void appendCone(
        GeneratedMeshPayload& payload,
        const Vec3& baseCenter,
        float radiusBottom,
        float height,
        uint32_t color = 0xFF777777,
        int segments = 32
    ) {
        appendCylinder(payload, baseCenter, radiusBottom, 0.001f, height, segments, color, true, false);
    }

    static void appendSphere(
        GeneratedMeshPayload& payload,
        const Vec3& center,
        float radius,
        int rings = 20,
        int sectors = 32,
        uint32_t color = 0xFFAAAAAA
    ) {
        rings = std::max(rings, 20);
        sectors = std::max(sectors, 32);
        payload.vertexStride = kVertexStrideFloats * sizeof(float);
        float const R = 1.0f / static_cast<float>(rings);
        float const S = 1.0f / static_cast<float>(sectors);

        float cr = ((color >> 16) & 0xFF) / 255.0f;
        float cg = ((color >> 8) & 0xFF) / 255.0f;
        float cb = (color & 0xFF) / 255.0f;

        uint32_t baseIdx = payload.vertexCount;

        for (int r = 0; r <= rings; ++r) {
            float phi = kPi * r * R; // 0 to pi
            float y = std::cos(phi);
            float sinPhi = std::sin(phi);

            for (int s = 0; s <= sectors; ++s) {
                float theta = 2.0f * kPi * s * S; // 0 to 2pi
                float x = sinPhi * std::cos(theta);
                float z = sinPhi * std::sin(theta);

                Vec3 norm{x, y, z};
                Vec3 pos = center + norm * radius;

                payload.vertexBuffer.push_back(pos.x);
                payload.vertexBuffer.push_back(pos.y);
                payload.vertexBuffer.push_back(pos.z);
                payload.vertexBuffer.push_back(norm.x);
                payload.vertexBuffer.push_back(norm.y);
                payload.vertexBuffer.push_back(norm.z);
                payload.vertexBuffer.push_back(cr);
                payload.vertexBuffer.push_back(cg);
                payload.vertexBuffer.push_back(cb);
                payload.vertexCount++;
            }
        }

        for (int r = 0; r < rings; ++r) {
            for (int s = 0; s < sectors; ++s) {
                uint32_t cur = baseIdx + r * (sectors + 1) + s;
                uint32_t next = cur + (sectors + 1);

                payload.indexBuffer.push_back(cur);
                payload.indexBuffer.push_back(next);
                payload.indexBuffer.push_back(cur + 1);

                payload.indexBuffer.push_back(cur + 1);
                payload.indexBuffer.push_back(next);
                payload.indexBuffer.push_back(next + 1);
                payload.indexCount += 6;
            }
        }
    }

    /**
     * Unrolls indexed geometry into flat triangle array for high-performance JNI transfer:
     * 15 floats per triangle:
     * [0..2]  p0.x, p0.y, p0.z
     * [3..5]  p1.x, p1.y, p1.z
     * [6..8]  p2.x, p2.y, p2.z
     * [9..11] normal.x, normal.y, normal.z
     * [12..14] color.r, color.g, color.b
     */
    static std::vector<float> buildTriangleBuffer(const GeneratedMeshPayload& payload) {
        std::vector<float> tris;
        const uint32_t stride = kVertexStrideFloats;
        if (payload.vertexBuffer.size() < stride) return tris;

        size_t triCount = payload.indexBuffer.size() / 3;
        tris.reserve(triCount * 15);

        for (size_t t = 0; t < triCount; ++t) {
            uint32_t i0 = payload.indexBuffer[t * 3];
            uint32_t i1 = payload.indexBuffer[t * 3 + 1];
            uint32_t i2 = payload.indexBuffer[t * 3 + 2];

            if ((i0 + 1) * stride > payload.vertexBuffer.size() ||
                (i1 + 1) * stride > payload.vertexBuffer.size() ||
                (i2 + 1) * stride > payload.vertexBuffer.size()) {
                continue;
            }

            const float* v0 = &payload.vertexBuffer[i0 * stride];
            const float* v1 = &payload.vertexBuffer[i1 * stride];
            const float* v2 = &payload.vertexBuffer[i2 * stride];

            // p0 (x, y, z)
            tris.push_back(v0[0]); tris.push_back(v0[1]); tris.push_back(v0[2]);
            // p1 (x, y, z)
            tris.push_back(v1[0]); tris.push_back(v1[1]); tris.push_back(v1[2]);
            // p2 (x, y, z)
            tris.push_back(v2[0]); tris.push_back(v2[1]); tris.push_back(v2[2]);

            // Normal: calculate face normal
            float e1x = v1[0] - v0[0], e1y = v1[1] - v0[1], e1z = v1[2] - v0[2];
            float e2x = v2[0] - v0[0], e2y = v2[1] - v0[1], e2z = v2[2] - v0[2];
            float nx = e1y * e2z - e1z * e2y;
            float ny = e1z * e2x - e1x * e2z;
            float nz = e1x * e2y - e1y * e2x;
            float nlen = std::sqrt(nx * nx + ny * ny + nz * nz);
            if (nlen > 1e-5f) {
                nx /= nlen; ny /= nlen; nz /= nlen;
            } else {
                nx = v0[3]; ny = v0[4]; nz = v0[5];
            }
            tris.push_back(nx); tris.push_back(ny); tris.push_back(nz);

            // Material RGB Color
            float cr = (v0[6] + v1[6] + v2[6]) / 3.0f;
            float cg = (v0[7] + v1[7] + v2[7]) / 3.0f;
            float cb = (v0[8] + v1[8] + v2[8]) / 3.0f;
            tris.push_back(cr); tris.push_back(cg); tris.push_back(cb);
        }
        return tris;
    }
};

} // namespace procedural::props
