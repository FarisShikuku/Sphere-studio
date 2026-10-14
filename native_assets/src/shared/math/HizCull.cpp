#include "HiZ.h"
#include <cmath>
#include <algorithm>

namespace sphere {

bool HiZ::ProjectAABB(const AABB& aabb, const Mat4& viewProjection, float& minU, float& minV, float& maxU, float& maxV, float& nearestDepth) {
    const Vec3 corners[8] = {
        {aabb.min.x, aabb.min.y, aabb.min.z},
        {aabb.max.x, aabb.min.y, aabb.min.z},
        {aabb.max.x, aabb.max.y, aabb.min.z},
        {aabb.min.x, aabb.max.y, aabb.min.z},
        {aabb.min.x, aabb.min.y, aabb.max.z},
        {aabb.max.x, aabb.min.y, aabb.max.z},
        {aabb.max.x, aabb.max.y, aabb.max.z},
        {aabb.min.x, aabb.max.y, aabb.max.z}
    };

    minU = 1.0f; minV = 1.0f;
    maxU = 0.0f; maxV = 0.0f;
    nearestDepth = 1.0f;

    bool anyInFront = false;

    for (int i = 0; i < 8; ++i) {
        const auto& c = corners[i];
        // Multiply by viewProjection matrix (column-major)
        float clipX = viewProjection.m[0]*c.x + viewProjection.m[4]*c.y + viewProjection.m[8]*c.z  + viewProjection.m[12];
        float clipY = viewProjection.m[1]*c.x + viewProjection.m[5]*c.y + viewProjection.m[9]*c.z  + viewProjection.m[13];
        float clipZ = viewProjection.m[2]*c.x + viewProjection.m[6]*c.y + viewProjection.m[10]*c.z + viewProjection.m[14];
        float clipW = viewProjection.m[3]*c.x + viewProjection.m[7]*c.y + viewProjection.m[11]*c.z + viewProjection.m[15];

        if (clipW > 1e-4f) {
            anyInFront = true;
            float invW = 1.0f / clipW;
            // NDC coordinates in range [-1, 1] for XY, and [0, 1] for Z in Vulkan
            float ndcX = clipX * invW;
            float ndcY = clipY * invW;
            float ndcZ = clipZ * invW;

            // Map NDC [-1, 1] to UV [0, 1]
            float u = std::clamp(ndcX * 0.5f + 0.5f, 0.0f, 1.0f);
            float v = std::clamp(ndcY * 0.5f + 0.5f, 0.0f, 1.0f);

            minU = std::min(minU, u);
            minV = std::min(minV, v);
            maxU = std::max(maxU, u);
            maxV = std::max(maxV, v);

            nearestDepth = std::min(nearestDepth, ndcZ);
        }
    }

    return anyInFront;
}

bool HiZ::TestOcclusionAABB(const AABB& aabb, const Mat4& viewProjection, const HizBuffer& hiz) {
    if (hiz.mips.empty() || hiz.width == 0 || hiz.height == 0) {
        return false; // Not occluded if HiZ buffer is unavailable
    }

    float minU, minV, maxU, maxV, nearestDepth;
    if (!ProjectAABB(aabb, viewProjection, minU, minV, maxU, maxV, nearestDepth)) {
        return false; // Behind camera or outside clip space, let frustum culler handle it
    }

    // Compute bounding rectangle size in pixels on screen
    float pixelWidth = (maxU - minU) * static_cast<float>(hiz.width);
    float pixelHeight = (maxV - minV) * static_cast<float>(hiz.height);
    float maxPixelSize = std::max(pixelWidth, pixelHeight);

    if (maxPixelSize <= 0.0f) {
        return true; // Zero screen size can be culled
    }

    // Determine target mip level where the bounding box spans ~2x2 texels
    float lod = std::log2(std::max(1.0f, maxPixelSize));
    uint32_t mipLevel = static_cast<uint32_t>(std::floor(lod));
    if (mipLevel >= hiz.mipLevels) {
        mipLevel = hiz.mipLevels - 1;
    }

    uint32_t mipW = std::max(1u, hiz.width >> mipLevel);
    uint32_t mipH = std::max(1u, hiz.height >> mipLevel);
    const auto& mipData = hiz.mips[mipLevel];

    // Compute texel coords at this mip level
    uint32_t x0 = std::min(static_cast<uint32_t>(minU * mipW), mipW - 1);
    uint32_t x1 = std::min(static_cast<uint32_t>(maxU * mipW), mipW - 1);
    uint32_t y0 = std::min(static_cast<uint32_t>(minV * mipH), mipH - 1);
    uint32_t y1 = std::min(static_cast<uint32_t>(maxV * mipH), mipH - 1);

    // Sample the 4 bounding texels
    float d00 = mipData[y0 * mipW + x0];
    float d10 = mipData[y0 * mipW + x1];
    float d01 = mipData[y1 * mipW + x0];
    float d11 = mipData[y1 * mipW + x1];

    float maxDepthInTile = std::max(std::max(d00, d10), std::max(d01, d11));

    // If nearest depth of our AABB is further than max depth in Hi-Z tile, the object is occluded
    return nearestDepth > maxDepthInTile;
}

} // namespace sphere
