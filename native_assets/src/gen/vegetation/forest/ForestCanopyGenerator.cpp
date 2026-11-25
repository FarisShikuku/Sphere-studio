// ============================================================================
// ForestCanopyGenerator.cpp
// Native C++17 PlantGL Asymmetric Crown Hull Implementation
// Part of the Unified Vegetation Subsystem (native-assets/src/vegetation/forest/)
// ============================================================================

#include "ForestCanopyGenerator.h"
#include "../../core/Assert.h"
#include <cmath>
#include <algorithm>

namespace vegetation {

namespace {
constexpr float PI_F = 3.14159265358979323846f;
constexpr float TWO_PI_F = 6.28318530717958647692f;
}

float ForestCanopyGenerator::evaluateCrownRadius(const CrownControlParams& p, float thetaRad, float heightFrac) {
    // Normalise angle to [0, 2pi)
    float t = std::fmod(thetaRad, TWO_PI_F);
    if (t < 0.0f) t += TWO_PI_F;

    float cosT = std::cos(t);
    float sinT = std::sin(t);

    float rx = (cosT >= 0.0f) ? p.rxPos : p.rxNeg;
    float ry = (sinT >= 0.0f) ? p.ryPos : p.ryNeg;

    // Superelliptical azimuthal profile
    float denom = (cosT * cosT) / (rx * rx) + (sinT * sinT) / (ry * ry);
    float rBase = (denom > 1e-6f) ? 1.0f / std::sqrt(denom) : 1.0f;

    // Vertical profile (top exponent C_top, bottom exponent C_bot)
    float verticalShape = 1.0f;
    if (heightFrac >= 0.5f) {
        float u = (heightFrac - 0.5f) / 0.5f;
        verticalShape = std::pow(std::clamp(1.0f - u * u, 0.0f, 1.0f), 1.0f / p.shapeTop);
    } else {
        float v = (0.5f - heightFrac) / 0.5f;
        verticalShape = std::pow(std::clamp(1.0f - v * v, 0.0f, 1.0f), 1.0f / p.shapeBot);
    }

    return rBase * verticalShape;
}

CrownMesh3D ForestCanopyGenerator::generateWireframeHull(const CrownControlParams& params, int slices, int stacks) {
    ENGINE_CORE_ASSERT(slices >= 4, "Slices must be >= 4");
    ENGINE_CORE_ASSERT(stacks >= 2, "Stacks must be >= 2");

    CrownMesh3D mesh;
    mesh.vertices.reserve((stacks + 1) * (slices + 1));
    mesh.indices.reserve(stacks * slices * 6);

    for (int j = 0; j <= stacks; ++j) {
        float hFrac = static_cast<float>(j) / static_cast<float>(stacks);
        float py = params.trunkHeight + hFrac * params.crownHeight;

        for (int i = 0; i <= slices; ++i) {
            float theta = (static_cast<float>(i) / static_cast<float>(slices)) * TWO_PI_F;
            float r = evaluateCrownRadius(params, theta, hFrac);

            float px = r * std::cos(theta) + params.offsetX;
            float pz = r * std::sin(theta) + params.offsetY;

            CrownMeshVertex v;
            v.px = px;
            v.py = py;
            v.pz = pz;
            v.nx = std::cos(theta);
            v.ny = 0.0f;
            v.nz = std::sin(theta);
            v.u = static_cast<float>(i) / static_cast<float>(slices);
            v.v = hFrac;
            mesh.vertices.push_back(v);
        }
    }

    for (int j = 0; j < stacks; ++j) {
        for (int i = 0; i < slices; ++i) {
            uint32_t row1 = j * (slices + 1);
            uint32_t row2 = (j + 1) * (slices + 1);

            mesh.indices.push_back(row1 + i);
            mesh.indices.push_back(row2 + i);
            mesh.indices.push_back(row1 + i + 1);

            mesh.indices.push_back(row1 + i + 1);
            mesh.indices.push_back(row2 + i);
            mesh.indices.push_back(row2 + i + 1);
        }
    }

    return mesh;
}

std::vector<LeafPlatelet3D> ForestCanopyGenerator::scatterVolumetricLeaves(const CrownControlParams& params, uint32_t seed) {
    std::vector<LeafPlatelet3D> leaves;
    leaves.reserve(params.leafCount);

    uint32_t rng = seed;
    auto lcg = [&rng]() -> float {
        rng = rng * 1664525u + 1013904223u;
        return static_cast<float>(rng & 0x00FFFFFFu) / static_cast<float>(0x01000000u);
    };

    for (int i = 0; i < params.leafCount; ++i) {
        float hFrac = 0.15f + lcg() * 0.80f;
        float theta = lcg() * TWO_PI_F;
        float rMax = evaluateCrownRadius(params, theta, hFrac);

        // Foliage envelope concentrated near hull boundary [0.45 * rMax, 1.0 * rMax]
        float rFrac = 0.45f + 0.55f * std::sqrt(lcg());
        float r = rMax * rFrac;

        LeafPlatelet3D leaf;
        leaf.posX = r * std::cos(theta) + params.offsetX;
        leaf.posY = params.trunkHeight + hFrac * params.crownHeight;
        leaf.posZ = r * std::sin(theta) + params.offsetY;

        // Leaf normal tilts outwards with random orientation
        leaf.normX = std::cos(theta) * 0.8f + (lcg() - 0.5f) * 0.4f;
        leaf.normY = 0.5f + lcg() * 0.5f;
        leaf.normZ = std::sin(theta) * 0.8f + (lcg() - 0.5f) * 0.4f;
        float nLen = std::sqrt(leaf.normX * leaf.normX + leaf.normY * leaf.normY + leaf.normZ * leaf.normZ);
        if (nLen > 1e-4f) {
            leaf.normX /= nLen;
            leaf.normY /= nLen;
            leaf.normZ /= nLen;
        }

        leaf.scale = 0.18f + lcg() * 0.12f;
        // Inner leaves are shadowed (lower shade factor), outer leaves catch sunlight
        leaf.shadeFactor = std::clamp(rFrac * 0.8f + 0.2f, 0.2f, 1.0f);

        leaves.push_back(leaf);
    }

    return leaves;
}

} // namespace vegetation
