#include "Camera.h"
#include <cmath>

namespace sphere {
Camera::Camera(float fovDeg, float aspectRatio, float nearPlane, float farPlane)
    : m_Fov(fovDeg), m_AspectRatio(aspectRatio), m_Near(nearPlane), m_Far(farPlane) {
    SetPerspective(fovDeg, aspectRatio, nearPlane, farPlane);
}

void Camera::SetPerspective(float fovDeg, float aspectRatio, float nearPlane, float farPlane) {
    m_Fov = fovDeg;
    m_AspectRatio = aspectRatio;
    m_Near = nearPlane;
    m_Far = farPlane;
}

void Camera::SetRotation(float pitch, float yaw) {
    m_Pitch = pitch;
    m_Yaw = yaw;
}
}\n