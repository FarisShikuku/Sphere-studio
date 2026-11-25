// ============================================================================
// ForestEcosystemEngine.cpp
// C++17 Implementation of PlantGL Asymmetric Crown Hulls & Stochastic 3D Trees
// ============================================================================
#include "ForestEcosystemEngine.hpp"
#include <cmath>
#include <algorithm>

namespace forest_ecosystem {

void ForestEcosystemEngine::evaluatePlantGLAsymmetricHull(
    const PlantGLAsymmetricHullParams& hull,
    float thetaRad,
    float phiRad,
    Vec3f& outLocalPos,
    Vec3f& outNormal
) {
    float cosT = std::cos(thetaRad);
    float sinT = std::sin(thetaRad);

    float rx = (cosT >= 0.0f) ? hull.rxPos : hull.rxNeg;
    float ry = (sinT >= 0.0f) ? hull.ryPos : hull.ryNeg;
    float hzX = (cosT >= 0.0f) ? hull.hzXPos : hull.hzXNeg;
    float hzY = (sinT >= 0.0f) ? hull.hzYPos : hull.hzYNeg;

    float denom = std::sqrt((ry * cosT) * (ry * cosT) + (rx * sinT) * (rx * sinT));
    float rTheta = (denom > 1e-5f) ? ((rx * ry) / denom) : rx;
    float hzTheta = hzX * (cosT * cosT) + hzY * (sinT * sinT);

    float absPhi = std::abs(phiRad);
    float cosPhi = std::clamp(std::cos(absPhi), 0.0f, 1.0f);
    float sinPhi = std::clamp(std::sin(absPhi), 0.0f, 1.0f);

    if (phiRad >= 0.0f) {
        float cTop = std::max(0.25f, hull.shapeTop);
        float u = std::pow(cosPhi, 2.0f / cTop);
        float v = std::pow(sinPhi, 1.35f / cTop);
        outLocalPos.x = hull.offsetX * (1.0f - u) + rTheta * cosT * u;
        outLocalPos.z = hull.offsetY * (1.0f - u) + rTheta * sinT * u;
        outLocalPos.y = hull.trunkHeight + hzTheta + (hull.crownHeight - hzTheta) * v;
    } else {
        float cBot = std::max(0.25f, hull.shapeBot);
        float u = std::pow(cosPhi, 2.0f / cBot);
        float v = std::pow(sinPhi, 1.35f / cBot);
        outLocalPos.x = hull.offsetX * (1.0f - u) + rTheta * cosT * u;
        outLocalPos.z = hull.offsetY * (1.0f - u) + rTheta * sinT * u;
        outLocalPos.y = hull.trunkHeight + hzTheta * (1.0f - v);
    }

    float ny = (phiRad >= 0.0f) ? (sinPhi / std::max(0.4f, hull.shapeTop))
                                : (-sinPhi / std::max(0.4f, hull.shapeBot));
    float nx = cosT * cosPhi;
    float nz = sinT * cosPhi;
    float nLen = std::sqrt(nx * nx + ny * ny + nz * nz);
    outNormal = (nLen > 1e-6f) ? Vec3f{nx / nLen, ny / nLen, nz / nLen} : Vec3f{0.0f, 1.0f, 0.0f};
}

} // namespace forest_ecosystem
