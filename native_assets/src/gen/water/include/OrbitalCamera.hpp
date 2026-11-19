#pragma once

#include "scene/Transform.h"

namespace sphere::water {

/**
 * OrbitalCamera
 *
 * 3D spherical orbital camera controller ported from talvinckb/OpenGL-Water-Simulation
 * for Vulkan clip-space conventions (right-handed view, Y-up, [0, 1] depth range).
 */
class OrbitalCamera {
public:
    OrbitalCamera() = default;

    void SetOrbit(float yawDeg, float pitchDeg, float distance);
    void AddOrbitDelta(float deltaYawDeg, float deltaPitchDeg);
    void SetTarget(const Vec3& target);
    void SetViewport(uint32_t width, uint32_t height, float fovDeg = 48.0f);

    void UpdateMatrices();

    const Mat4& GetViewMatrix() const { return m_View; }
    const Mat4& GetProjectionMatrix() const { return m_Proj; }
    const Mat4& GetViewProjectionMatrix() const { return m_ViewProj; }
    const Vec3& GetPosition() const { return m_Position; }
    const Vec3& GetTarget() const { return m_Target; }
    const Vec3& GetForward() const { return m_Forward; }
    const Vec3& GetRight() const { return m_Right; }
    const Vec3& GetUp() const { return m_Up; }

    float GetYawDeg() const { return m_YawDeg; }
    float GetPitchDeg() const { return m_PitchDeg; }
    float GetDistance() const { return m_Distance; }
    float GetNearPlane() const { return m_NearPlane; }
    float GetFarPlane() const { return m_FarPlane; }
    float GetTanHalfFov() const { return m_TanHalfFov; }
    float GetAspect() const { return m_Aspect; }

    // Unprojects normalized screen coordinate u,v in [0,1] into a world-space ray
    void ScreenToWorldRay(float u, float v, Vec3& outOrigin, Vec3& outDir) const;

private:
    float m_YawDeg = -34.0f;
    float m_PitchDeg = 22.0f;
    float m_Distance = 4.6f;

    Vec3 m_Target{0.0f, 0.85f, 0.0f};
    Vec3 m_Position{0.0f, 2.2f, 4.2f};
    Vec3 m_Forward{0.0f, 0.0f, -1.0f};
    Vec3 m_Right{1.0f, 0.0f, 0.0f};
    Vec3 m_Up{0.0f, 1.0f, 0.0f};

    uint32_t m_Width = 360;
    uint32_t m_Height = 240;
    float m_FovDeg = 48.0f;
    float m_NearPlane = 0.1f;
    float m_FarPlane = 60.0f;
    float m_TanHalfFov = 0.4452f;
    float m_Aspect = 1.5f;

    Mat4 m_View;
    Mat4 m_Proj;
    Mat4 m_ViewProj;
};

} // namespace sphere::water
