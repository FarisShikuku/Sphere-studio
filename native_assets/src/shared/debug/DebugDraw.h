#pragma once
#include "scene/Transform.h"
#include "scene/AABB.h"
#include <vector>
#include <mutex>

namespace sphere {

struct DebugLine {
    Vec3 start;
    Vec3 end;
    Vec4 color;
};

class DebugDraw {
public:
    static DebugDraw& Get();

    void DrawLine(const Vec3& start, const Vec3& end, const Vec4& color);
    void DrawRay(const Vec3& origin, const Vec3& direction, float length, const Vec4& color);
    void DrawAABB(const AABB& aabb, const Vec4& color);
    void DrawSphere(const Vec3& center, float radius, const Vec4& color, uint32_t segments = 32);
    void DrawFrustum(const Mat4& invViewProj, const Vec4& color);
    void Clear();

    const std::vector<DebugLine>& GetLines() const { return m_Lines; }

private:
    std::vector<DebugLine> m_Lines;
    mutable std::mutex m_Mutex;
};

} // namespace sphere
