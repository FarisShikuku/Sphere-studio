#pragma once
#include "Camera.h"
namespace sphere {
class CameraController {
public:
    CameraController(Camera& camera);
    void Update(float deltaTime);
    void Orbit(float deltaPitch, float deltaYaw);
    void Pan(float deltaX, float deltaY);
    void Zoom(float deltaZoom);
private:
    Camera& m_Camera;
    float m_Distance = 5.0f;
    float m_Pitch = 25.0f;
    float m_Yaw = -35.0f;
    Vec3 m_Target{0.0f, 0.0f, 0.0f};
};
}
