#include "OrbitalCamera.hpp"
#include <algorithm>
#include <cmath>

namespace sphere::water {

namespace {
constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;
}

void OrbitalCamera::SetOrbit(float yawDeg, float pitchDeg, float distance) {
    m_YawDeg = yawDeg;
    m_PitchDeg = std::clamp(pitchDeg, -82.0f, 82.0f);
    m_Distance = std::clamp(distance, 1.4f, 120.0f);
    UpdateMatrices();
}

void OrbitalCamera::AddOrbitDelta(float deltaYawDeg, float deltaPitchDeg) {
    m_YawDeg += deltaYawDeg;
    m_PitchDeg = std::clamp(m_PitchDeg + deltaPitchDeg, -82.0f, 82.0f);
    UpdateMatrices();
}

void OrbitalCamera::SetTarget(const Vec3& target) {
    m_Target = target;
    UpdateMatrices();
}

void OrbitalCamera::SetViewport(uint32_t width, uint32_t height, float fovDeg) {
    m_Width = std::max(1u, width);
    m_Height = std::max(1u, height);
    m_FovDeg = std::clamp(fovDeg, 25.0f, 85.0f);
    m_Aspect = static_cast<float>(m_Width) / static_cast<float>(m_Height);
    m_TanHalfFov = std::tan(m_FovDeg * 0.5f * kDegToRad);
    UpdateMatrices();
}

void OrbitalCamera::UpdateMatrices() {
    float yawRad = m_YawDeg * kDegToRad;
    float pitchRad = m_PitchDeg * kDegToRad;

    float cosP = std::cos(pitchRad);
    float sinP = std::sin(pitchRad);
    float cosY = std::cos(yawRad);
    float sinY = std::sin(yawRad);

    Vec3 offset(
        m_Distance * cosP * sinY,
        m_Distance * sinP,
        m_Distance * cosP * cosY
    );
    m_Position = m_Target + offset;
    m_Forward = (m_Target - m_Position).Normalized();

    Vec3 worldUp(0.0f, 1.0f, 0.0f);
    m_Right = Vec3::Cross(m_Forward, worldUp).Normalized();
    if (m_Right.LengthSq() < 1e-6f) {
        m_Right = Vec3(1.0f, 0.0f, 0.0f);
    }
    m_Up = Vec3::Cross(m_Right, m_Forward).Normalized();

    m_View = Mat4::LookAt(m_Position, m_Target, m_Up);
    m_Proj = Mat4::Perspective(m_FovDeg * kDegToRad, m_Aspect, m_NearPlane, m_FarPlane);
    m_ViewProj = m_Proj * m_View;
}

void OrbitalCamera::ScreenToWorldRay(float u, float v, Vec3& outOrigin, Vec3& outDir) const {
    // u in [0, 1] left->right, v in [0, 1] top->bottom
    float ndcX = (u * 2.0f - 1.0f) * m_Aspect * m_TanHalfFov;
    float ndcY = (1.0f - v * 2.0f) * m_TanHalfFov;

    outOrigin = m_Position;
    outDir = (m_Forward + m_Right * ndcX + m_Up * ndcY).Normalized();
}

} // namespace sphere::water
