#include "DebugDraw.h"
#include <cmath>

namespace sphere {

DebugDraw& DebugDraw::Get() {
    static DebugDraw instance;
    return instance;
}

void DebugDraw::DrawLine(const Vec3& start, const Vec3& end, const Vec4& color) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Lines.push_back({start, end, color});
}

void DebugDraw::DrawRay(const Vec3& origin, const Vec3& direction, float length, const Vec4& color) {
    float lenSq = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z;
    if (lenSq < 1e-8f) return;
    float invLen = 1.0f / std::sqrt(lenSq);
    Vec3 normDir = { direction.x * invLen, direction.y * invLen, direction.z * invLen };
    Vec3 end = { origin.x + normDir.x * length, origin.y + normDir.y * length, origin.z + normDir.z * length };

    DrawLine(origin, end, color);

    // Arrowhead at the tip
    Vec3 up = (std::abs(normDir.y) < 0.99f) ? Vec3{0, 1, 0} : Vec3{1, 0, 0};
    Vec3 right = {
        normDir.y * up.z - normDir.z * up.y,
        normDir.z * up.x - normDir.x * up.z,
        normDir.x * up.y - normDir.y * up.x
    };
    float arrowSize = length * 0.1f;
    if (arrowSize > 0.5f) arrowSize = 0.5f;

    Vec3 tip1 = {
        end.x - normDir.x * arrowSize + right.x * (arrowSize * 0.5f),
        end.y - normDir.y * arrowSize + right.y * (arrowSize * 0.5f),
        end.z - normDir.z * arrowSize + right.z * (arrowSize * 0.5f)
    };
    Vec3 tip2 = {
        end.x - normDir.x * arrowSize - right.x * (arrowSize * 0.5f),
        end.y - normDir.y * arrowSize - right.y * (arrowSize * 0.5f),
        end.z - normDir.z * arrowSize - right.z * (arrowSize * 0.5f)
    };
    DrawLine(end, tip1, color);
    DrawLine(end, tip2, color);
}

void DebugDraw::DrawAABB(const AABB& aabb, const Vec4& color) {
    Vec3 p0 = {aabb.min.x, aabb.min.y, aabb.min.z};
    Vec3 p1 = {aabb.max.x, aabb.min.y, aabb.min.z};
    Vec3 p2 = {aabb.max.x, aabb.max.y, aabb.min.z};
    Vec3 p3 = {aabb.min.x, aabb.max.y, aabb.min.z};
    Vec3 p4 = {aabb.min.x, aabb.min.y, aabb.max.z};
    Vec3 p5 = {aabb.max.x, aabb.min.y, aabb.max.z};
    Vec3 p6 = {aabb.max.x, aabb.max.y, aabb.max.z};
    Vec3 p7 = {aabb.min.x, aabb.max.y, aabb.max.z};

    DrawLine(p0, p1, color); DrawLine(p1, p2, color); DrawLine(p2, p3, color); DrawLine(p3, p0, color);
    DrawLine(p4, p5, color); DrawLine(p5, p6, color); DrawLine(p6, p7, color); DrawLine(p7, p4, color);
    DrawLine(p0, p4, color); DrawLine(p1, p5, color); DrawLine(p2, p6, color); DrawLine(p3, p7, color);
}

void DebugDraw::DrawSphere(const Vec3& center, float radius, const Vec4& color, uint32_t segments) {
    if (segments < 8) segments = 8;
    const float twoPi = 6.28318530718f;
    const float step = twoPi / static_cast<float>(segments);

    for (uint32_t i = 0; i < segments; ++i) {
        float a0 = static_cast<float>(i) * step;
        float a1 = static_cast<float>(i + 1) * step;

        float c0 = std::cos(a0) * radius;
        float s0 = std::sin(a0) * radius;
        float c1 = std::cos(a1) * radius;
        float s1 = std::sin(a1) * radius;

        // XY plane circle (latitude equator)
        DrawLine({center.x + c0, center.y + s0, center.z},
                 {center.x + c1, center.y + s1, center.z}, color);

        // XZ plane circle (horizontal equator)
        DrawLine({center.x + c0, center.y, center.z + s0},
                 {center.x + c1, center.y, center.z + s1}, color);

        // YZ plane circle (meridian)
        DrawLine({center.x, center.y + c0, center.z + s0},
                 {center.x, center.y + c1, center.z + s1}, color);
    }
}

void DebugDraw::DrawFrustum(const Mat4& invViewProj, const Vec4& color) {
    // 8 NDC cube corners: near (z=0) and far (z=1) in Vulkan depth range [0, 1]
    const Vec4 ndcCorners[8] = {
        {-1.0f, -1.0f, 0.0f, 1.0f},
        { 1.0f, -1.0f, 0.0f, 1.0f},
        { 1.0f,  1.0f, 0.0f, 1.0f},
        {-1.0f,  1.0f, 0.0f, 1.0f},
        {-1.0f, -1.0f, 1.0f, 1.0f},
        { 1.0f, -1.0f, 1.0f, 1.0f},
        { 1.0f,  1.0f, 1.0f, 1.0f},
        {-1.0f,  1.0f, 1.0f, 1.0f}
    };

    Vec3 worldCorners[8];
    for (int i = 0; i < 8; ++i) {
        const auto& c = ndcCorners[i];
        float x = invViewProj.m[0]*c.x + invViewProj.m[4]*c.y + invViewProj.m[8]*c.z + invViewProj.m[12]*c.w;
        float y = invViewProj.m[1]*c.x + invViewProj.m[5]*c.y + invViewProj.m[9]*c.z + invViewProj.m[13]*c.w;
        float z = invViewProj.m[2]*c.x + invViewProj.m[6]*c.y + invViewProj.m[10]*c.z + invViewProj.m[14]*c.w;
        float w = invViewProj.m[3]*c.x + invViewProj.m[7]*c.y + invViewProj.m[11]*c.z + invViewProj.m[15]*c.w;
        float invW = (std::abs(w) > 1e-6f) ? (1.0f / w) : 1.0f;
        worldCorners[i] = { x * invW, y * invW, z * invW };
    }

    // Near plane loop
    DrawLine(worldCorners[0], worldCorners[1], color);
    DrawLine(worldCorners[1], worldCorners[2], color);
    DrawLine(worldCorners[2], worldCorners[3], color);
    DrawLine(worldCorners[3], worldCorners[0], color);

    // Far plane loop
    DrawLine(worldCorners[4], worldCorners[5], color);
    DrawLine(worldCorners[5], worldCorners[6], color);
    DrawLine(worldCorners[6], worldCorners[7], color);
    DrawLine(worldCorners[7], worldCorners[4], color);

    // Connecting frustum side edges
    DrawLine(worldCorners[0], worldCorners[4], color);
    DrawLine(worldCorners[1], worldCorners[5], color);
    DrawLine(worldCorners[2], worldCorners[6], color);
    DrawLine(worldCorners[3], worldCorners[7], color);
}

void DebugDraw::Clear() {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Lines.clear();
}

} // namespace sphere
