#include "CameraController.h"

namespace sphere {
CameraController::CameraController(Camera& camera) : m_Camera(camera) {}

void CameraController::Update(float deltaTime) {}
void CameraController::Orbit(float deltaPitch, float deltaYaw) {
    m_Pitch += deltaPitch;
    m_Yaw += deltaYaw;
}
void CameraController::Pan(float deltaX, float deltaY) {
    m_Target.x += deltaX;
    m_Target.y += deltaY;
}
void CameraController::Zoom(float deltaZoom) {
    m_Distance = std::max(0.5f, m_Distance - deltaZoom);
}
}\n