#include "Transform.h"
namespace sphere {
Mat4 Transform::GetMatrix() const {
    Mat4 t = Mat4::Translation(position);
    Mat4 r = Mat4::FromQuat(rotation);
    Mat4 s = Mat4::Scaling(scale);
    // M = T * R * S
    Mat4 rs = Mat4::Multiply(r, s);
    return Mat4::Multiply(t, rs);
}
void Transform::SetEulerRotation(float pitchDeg, float yawDeg, float rollDeg) {
    constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;
    rotation = Quat::FromEuler(pitchDeg * kDegToRad, yawDeg * kDegToRad, rollDeg * kDegToRad);
}
} // namespace sphere