#include "SkeletonGen.h"
#include <cmath>
#include <algorithm>

namespace sphere {

std::vector<VertexSkinWeight> SkeletonGen::CalculateSmoothWeights(
    const std::vector<Vec3>& vertices,
    const std::vector<Bone>& bones,
    const std::vector<Mat4>& globalTransforms,
    float falloffPower
) {
    std::vector<VertexSkinWeight> result(vertices.size());
    if (bones.empty() || vertices.empty()) {
        return result;
    }

    // Extract bone origin positions in world space
    std::vector<Vec3> boneOrigins(bones.size());
    for (size_t b = 0; b < bones.size(); ++b) {
        boneOrigins[b] = {
            globalTransforms[b].m[12],
            globalTransforms[b].m[13],
            globalTransforms[b].m[14]
        };
    }

    struct BoneDist {
        uint32_t boneIndex;
        float distanceSq;
    };

    for (size_t vIdx = 0; vIdx < vertices.size(); ++vIdx) {
        const auto& vert = vertices[vIdx];
        std::vector<BoneDist> dists(bones.size());

        for (size_t b = 0; b < bones.size(); ++b) {
            Vec3 p0 = boneOrigins[b];
            Vec3 p1 = p0;

            // If bone has a child, compute segment from this bone to child bone
            for (size_t c = 0; c < bones.size(); ++c) {
                if (bones[c].parentIndex == static_cast<int32_t>(b)) {
                    p1 = boneOrigins[c];
                    break;
                }
            }

            // Shortest distance from point 'vert' to line segment p0 -> p1
            Vec3 ab = { p1.x - p0.x, p1.y - p0.y, p1.z - p0.z };
            Vec3 ap = { vert.x - p0.x, vert.y - p0.y, vert.z - p0.z };
            float abLenSq = ab.x * ab.x + ab.y * ab.y + ab.z * ab.z;

            float distSq = 0.0f;
            if (abLenSq < 1e-6f) {
                distSq = ap.x * ap.x + ap.y * ap.y + ap.z * ap.z;
            } else {
                float t = std::clamp((ap.x * ab.x + ap.y * ab.y + ap.z * ab.z) / abLenSq, 0.0f, 1.0f);
                Vec3 closest = { p0.x + ab.x * t, p0.y + ab.y * t, p0.z + ab.z * t };
                float dx = vert.x - closest.x;
                float dy = vert.y - closest.y;
                float dz = vert.z - closest.z;
                distSq = dx * dx + dy * dy + dz * dz;
            }

            dists[b] = { static_cast<uint32_t>(b), distSq };
        }

        // Sort by closest bone distance
        std::sort(dists.begin(), dists.end(), [](const BoneDist& a, const BoneDist& b) {
            return a.distanceSq < b.distanceSq;
        });

        // Compute heat decay weights for up to 4 bones
        uint32_t numInfluences = std::min(4u, static_cast<uint32_t>(bones.size()));
        float totalWeight = 0.0f;

        for (uint32_t i = 0; i < numInfluences; ++i) {
            float dist = std::sqrt(dists[i].distanceSq);
            float w = 1.0f / (std::pow(dist + 0.05f, falloffPower));
            result[vIdx].boneIndices[i] = dists[i].boneIndex;
            result[vIdx].weights[i] = w;
            totalWeight += w;
        }

        // Normalize weights so they sum to 1.0
        if (totalWeight > 1e-6f) {
            float invTotal = 1.0f / totalWeight;
            for (uint32_t i = 0; i < numInfluences; ++i) {
                result[vIdx].weights[i] *= invTotal;
            }
        } else {
            result[vIdx].weights[0] = 1.0f;
        }
    }

    return result;
}

} // namespace sphere
