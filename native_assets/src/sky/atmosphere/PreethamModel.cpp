#include "PreethamModel.h"
#include <cmath>
#include <algorithm>

namespace sphere {

Vec3 PreethamModel::CalculateZenithColor(float sunTheta) const {
    float T = std::clamp(turbidity, 1.0f, 10.0f);
    float cosTheta = std::cos(sunTheta);

    // Perez coefficients
    float Yz = (4.0453f * T - 4.9710f) * std::tan((4.0f / 9.0f - T / 120.0f) * (3.14159f - 2.0f * sunTheta)) - 0.2155f * T + 2.4192f;
    Yz = std::max(0.0f, Yz);

    return Vec3{0.18f * (Yz / 15.0f), 0.42f * (Yz / 15.0f), 0.85f * (Yz / 15.0f)};
}

Vec3 PreethamModel::EvaluatePerez(float theta, float gamma) const {
    float cosGamma = std::cos(gamma);
    float f = (1.0f + 0.5f * std::exp(0.5f / std::max(0.01f, std::cos(theta)))) *
              (1.0f - std::exp(-0.8f / std::max(0.01f, std::cos(theta))));
    return Vec3{f * 0.7f, f * 0.85f, f * 1.0f};
}

} // namespace sphere
