#pragma once

#include "FluidParams.hpp"
#include <algorithm>
#include <cmath>

namespace sphere::water {

inline float BoxSDF(const Vec3& p, const Vec3& center, const Vec3& halfExtents) {
    Vec3 d(
        std::abs(p.x - center.x) - halfExtents.x,
        std::abs(p.y - center.y) - halfExtents.y,
        std::abs(p.z - center.z) - halfExtents.z
    );
    Vec3 clamped(std::max(d.x, 0.0f), std::max(d.y, 0.0f), std::max(d.z, 0.0f));
    float outside = clamped.Length();
    float inside = std::min(std::max(d.x, std::max(d.y, d.z)), 0.0f);
    return outside + inside;
}

inline float RockNoise(const Vec3& p, float roughness) {
    if (roughness <= 0.01f) return 0.0f;
    float n1 = std::sin(p.z * 7.2f + p.y * 4.8f) * std::cos(p.x * 6.1f - p.z * 4.4f);
    float n2 = 0.5f * std::sin(p.y * 12.6f - p.z * 9.3f + p.x * 5.7f);
    return (n1 + n2) * 0.038f * roughness;
}

/**
 * Unified 3D Signed Distance Function for the natural waterfall cliff,
 * sloped crest lip, overhang undercut, stepped rock terraces, and gorge side-banks.
 * Shared identically by FluidSimulator (SPH collision) and FluidRenderer (raymarched cliff).
 */
inline float CliffSDF(const Vec3& p, const FluidPhysicsParams& phys) {
    if (!phys.waterfallEnabled && phys.scenario != WaterSpawnScenario::WATERFALL_CLIFF) {
        return 100.0f;
    }

    const float halfW = std::max(0.6f, phys.boundsWidth * 0.5f);
    const float halfD = std::max(0.6f, phys.boundsDepth * 0.5f);
    const float cliffH = std::clamp(phys.waterfallCliffHeight, 0.55f, std::max(0.85f, phys.boundsHeight - 0.22f));
    const float lipX = std::clamp(-halfW + phys.waterfallCliffDepth, -halfW + 0.35f, halfW - 0.65f);
    const float chuteHalfZ = std::clamp(phys.waterfallCliffWidth * 0.5f, 0.22f, halfD - 0.04f);

    // 1. Main cliff plateau mass: [-halfW .. lipX], [0 .. cliffTopY], [-halfD .. +halfD]
    // Apply crest lip slope across the outer 0.42m of the plateau
    float slopeRad = std::clamp(phys.cliffLipSlopeDeg, 0.0f, 45.0f) * (3.14159265f / 180.0f);
    float slopeT = std::clamp((p.x - (lipX - 0.42f)) / 0.42f, 0.0f, 1.0f);
    float localTopY = cliffH - slopeT * 0.38f * std::tan(slopeRad);
    localTopY = std::max(0.35f, localTopY);

    // Apply natural undercut recess beneath the lip
    float hNorm = std::clamp(p.y / std::max(cliffH, 0.1f), 0.0f, 1.0f);
    float undercutRecess = phys.cliffUndercut * std::sin(hNorm * 3.14159265f);
    if (p.y > cliffH - 0.10f) undercutRecess = 0.0f;

    float mainWallMaxX = lipX - undercutRecess;
    float dMain = BoxSDF(
        p,
        Vec3((-halfW + mainWallMaxX) * 0.5f, localTopY * 0.5f, 0.0f),
        Vec3(std::max(0.08f, (mainWallMaxX + halfW) * 0.5f), localTopY * 0.5f, halfD + 0.05f)
    );

    float d = dMain;

    // 2. Stepped rock terraces along the cliff drop (1..3 ledges stepping outward in +X)
    int32_t steps = std::clamp(phys.cliffSteppedLedges, 0, 3);
    if (steps > 0) {
        float availPoolX = std::max(0.25f, (halfW - lipX) * 0.52f);
        for (int32_t s = 1; s <= steps; ++s) {
            float frac = static_cast<float>(s) / static_cast<float>(steps + 1);
            float shelfTopY = cliffH * (1.0f - frac);
            float shelfReach = std::min(availPoolX, 0.22f * static_cast<float>(s));
            float shelfMaxX = lipX + shelfReach;
            float dShelf = BoxSDF(
                p,
                Vec3((-halfW + shelfMaxX) * 0.5f, shelfTopY * 0.5f, 0.0f),
                Vec3(std::max(0.08f, (shelfMaxX + halfW) * 0.5f), shelfTopY * 0.5f, halfD + 0.05f)
            );
            d = std::min(d, dShelf);
        }
    }

    // 3. Natural canyon / gorge side-banks flanking the chute width on the upper plateau
    if (phys.cliffRockChanneling > 0.05f && chuteHalfZ < halfD - 0.05f) {
        float bankExtraH = 0.42f * phys.cliffRockChanneling;
        float bankTopY = cliffH + bankExtraH;
        float bankMinZ = chuteHalfZ;
        float bankMaxZ = halfD + 0.06f;
        float bankHalfZ = std::max(0.04f, (bankMaxZ - bankMinZ) * 0.5f);
        float bankCenterZ = (bankMinZ + bankMaxZ) * 0.5f;
        float bankMaxX = lipX + 0.04f;

        float dLeftBank = BoxSDF(
            p,
            Vec3((-halfW + bankMaxX) * 0.5f, bankTopY * 0.5f, -bankCenterZ),
            Vec3(std::max(0.06f, (bankMaxX + halfW) * 0.5f), bankTopY * 0.5f, bankHalfZ)
        );
        float dRightBank = BoxSDF(
            p,
            Vec3((-halfW + bankMaxX) * 0.5f, bankTopY * 0.5f, bankCenterZ),
            Vec3(std::max(0.06f, (bankMaxX + halfW) * 0.5f), bankTopY * 0.5f, bankHalfZ)
        );
        d = std::min(d, std::min(dLeftBank, dRightBank));
    }

    // Add organic rock roughness displacement
    d -= RockNoise(p, phys.cliffRoughness);
    return d;
}

inline Vec3 CliffNormal(const Vec3& p, const FluidPhysicsParams& phys) {
    constexpr float eps = 0.012f;
    float dx = CliffSDF(Vec3(p.x + eps, p.y, p.z), phys) - CliffSDF(Vec3(p.x - eps, p.y, p.z), phys);
    float dy = CliffSDF(Vec3(p.x, p.y + eps, p.z), phys) - CliffSDF(Vec3(p.x, p.y - eps, p.z), phys);
    float dz = CliffSDF(Vec3(p.x, p.y, p.z + eps), phys) - CliffSDF(Vec3(p.x, p.y, p.z - eps), phys);
    Vec3 n(dx, dy, dz);
    float len = n.Length();
    return (len > 1e-6f) ? (n / len) : Vec3(1.0f, 0.0f, 0.0f);
}

inline void GetCliffBoundsAABB(const FluidPhysicsParams& phys, Vec3& outMin, Vec3& outMax) {
    const float halfW = std::max(0.6f, phys.boundsWidth * 0.5f);
    const float halfD = std::max(0.6f, phys.boundsDepth * 0.5f);
    const float cliffH = std::clamp(phys.waterfallCliffHeight, 0.55f, std::max(0.85f, phys.boundsHeight - 0.22f));
    const float lipX = std::clamp(-halfW + phys.waterfallCliffDepth, -halfW + 0.35f, halfW - 0.65f);
    float availPoolX = std::max(0.25f, (halfW - lipX) * 0.52f);
    int32_t steps = std::clamp(phys.cliffSteppedLedges, 0, 3);
    float maxReach = (steps > 0) ? std::min(availPoolX, 0.22f * static_cast<float>(steps)) : 0.0f;

    outMin = Vec3(-halfW - 0.05f, 0.0f, -halfD - 0.05f);
    outMax = Vec3(lipX + maxReach + 0.12f, cliffH + 0.42f * phys.cliffRockChanneling + 0.12f, halfD + 0.05f);
}

} // namespace sphere::water
