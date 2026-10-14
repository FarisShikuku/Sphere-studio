#pragma once
#include "Transform.h"

namespace sphere {
class Camera {
public:
    Camera(float fovDeg = 60.0f, float aspectRatio = 1.777f, float nearPlane = 0.1f, float farPlane = 1000.0f);

    void SetPerspective(float fovDeg, float aspectRatio, float nearPlane, float farPlane);
    void SetPosition(const Vec3& pos) { m_Position = pos; }
    void SetRotation(float pitch, float yaw);

    const Mat4& GetViewMatrix() const { return m_ViewMatrix; }
    const Mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
    const Vec3& GetPosition() const { return m_Position; }

private:
    Vec3 m_Position{0.0f, 2.0f, -5.0f};
    float m_Pitch = 0.0f;
    float m_Yaw = 0.0f;
    float m_Fov = 60.0f;
    float m_AspectRatio = 1.777f;
    float m_Near = 0.1f;
    float m_Far = 1000.0f;
    Mat4 m_ViewMatrix{};
    Mat4 m_ProjectionMatrix{};
};
}\n