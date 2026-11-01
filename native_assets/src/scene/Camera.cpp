#include "Camera.h"
#include <cmath>

namespace sphere {
Camera::Camera(float fovDeg, float aspectRatio, float nearPlane, float farPlane)
    : m_Fov(fovDeg), m_AspectRatio(aspectRatio), m_Near(nearPlane), m_Far(farPlane) {
    SetPerspective(fovDeg, aspectRatio, nearPlane, farPlane);
    Update();
}

void Camera::SetPerspective(float fovDeg, float aspectRatio, float nearPlane, float farPlane) {
    m_Fov = fovDeg;
    m_AspectRatio = aspectRatio;
    m_Near = nearPlane;
    m_Far = farPlane;
    float rad = fovDeg * 3.1415926535f / 180.0f;
    m_ProjectionMatrix = Mat4::Perspective(rad, aspectRatio, nearPlane, farPlane);
}

void Camera::SetPosition(const Vec3& pos) {
    m_Position = pos;
    Update();
}

void Camera::SetRotation(float pitch, float yaw) {
    m_Pitch = pitch;
    m_Yaw = yaw;
    Update();
}

void Camera::Update() {
    // Sascha Willems Vulkan Camera view matrix (camera.hpp: lookat mode):
    // rotM = rotate(pitch, (1,0,0)) * rotate(yaw, (0,1,0))
    // transM = translate(position)
    // view = transM * rotM
    float pitchRad = m_Pitch * 3.1415926535f / 180.0f;
    float yawRad = m_Yaw * 3.1415926535f / 180.0f;

    Mat4 rotX = Mat4::RotationX(pitchRad);
    Mat4 rotY = Mat4::RotationY(yawRad);
    Mat4 rotM = Mat4::Multiply(rotX, rotY);
    Mat4 transM = Mat4::Translation(m_Position);
    m_ViewMatrix = Mat4::Multiply(transM, rotM);
}
}\n