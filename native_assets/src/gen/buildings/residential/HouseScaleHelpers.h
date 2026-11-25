#pragma once

#include "HouseGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace procedural::buildings::res_scale {

inline int32_t stories(const ResidentialParams& p, int32_t minS = 1, int32_t maxS = 30) {
    return std::clamp(p.stories, minS, maxS);
}

inline float floorHeight(const ResidentialParams& p, float defaultH = 3.2f) {
    float raw = (p.storyHeight > 0.5f) ? p.storyHeight : defaultH;
    return std::clamp(raw, 2.4f, 6.5f);
}

inline float width(const ResidentialParams& p, float minW = 4.0f, float defaultW = 14.0f) {
    float raw = (p.footprintWidth > 0.5f) ? p.footprintWidth : defaultW;
    return std::clamp(raw, minW, 60.0f);
}

inline float depth(const ResidentialParams& p, float minD = 4.0f, float defaultD = 10.0f) {
    float raw = (p.footprintDepth > 0.5f) ? p.footprintDepth : defaultD;
    return std::clamp(raw, minD, 50.0f);
}

inline uint32_t wallMat(const ResidentialParams& p, MaterialSlot defaultSlot) {
    return p.useExtendedParams
        ? BuildingComponents::getWallMaterial(p.wallMaterial)
        : static_cast<uint32_t>(defaultSlot);
}

inline WindowType windowType(const ResidentialParams& p, WindowType defaultType) {
    return p.useExtendedParams ? p.windowType : defaultType;
}

inline GlassType glassType(const ResidentialParams& p, GlassType defaultGlass = GlassType::ClearFloat) {
    return p.useExtendedParams ? p.glassType : defaultGlass;
}

inline DoorType doorType(const ResidentialParams& p, DoorType defaultDoor) {
    return p.useExtendedParams ? p.doorType : defaultDoor;
}

inline RoofType roofType(const ResidentialParams& p, RoofType defaultRoof) {
    return p.useExtendedParams ? p.roofStyle : defaultRoof;
}

inline float roofHeight(const ResidentialParams& p, float defaultRH) {
    float raw = p.useExtendedParams ? p.roofHeight : defaultRH;
    return std::clamp(raw, 0.8f, 10.0f);
}

inline float roofOverhang(const ResidentialParams& p, float defaultOv = 0.5f) {
    float raw = p.useExtendedParams ? p.roofOverhang : defaultOv;
    return std::clamp(raw, 0.0f, 2.5f);
}

inline bool showClutter(const ResidentialParams& p, bool defaultClutter = false) {
    return p.useExtendedParams ? p.showRooftopClutter : defaultClutter;
}

inline bool showShutters(const ResidentialParams& p, bool defaultShutters = false) {
    return p.useExtendedParams ? p.hasWindowShutters : defaultShutters;
}

inline bool showSunshades(const ResidentialParams& p, bool defaultLouvres = false) {
    return p.useExtendedParams ? p.hasSunLouvres : defaultLouvres;
}

inline bool showBalconies(const ResidentialParams& p) {
    return p.hasPorchOrBalcony;
}

inline float balconyDepth(const ResidentialParams& p, float defaultDepth = 1.2f) {
    float raw = p.useExtendedParams ? p.balconyDepth : defaultDepth;
    return std::clamp(raw, 0.6f, 2.5f);
}

inline bool showCornice(const ResidentialParams& p, bool defaultCornice = true) {
    return p.useExtendedParams ? p.hasCornice : defaultCornice;
}

/**
 * Computes human-scale clamped window dimensions that fit strictly within [floorY, floorY + flH]
 * leaving >= 0.25m sill floor clearance and >= 0.20m upper header lintel clearance.
 */
inline void clampWindowDims(
    const ResidentialParams& p,
    float flH,
    float maxFacadeW,
    float defaultW,
    float defaultH,
    float defaultSill,
    float& outW,
    float& outH,
    float& outSill
) {
    float reqW = p.useExtendedParams ? p.windowWidth : defaultW;
    float reqH = p.useExtendedParams ? p.windowHeight : defaultH;
    float reqSill = p.useExtendedParams ? p.windowSillHeight : defaultSill;

    outSill = std::clamp(reqSill, 0.20f, std::max(0.25f, flH - 1.05f));
    float maxAllowedH = std::max(0.55f, flH - outSill - 0.22f);
    outH = std::clamp(reqH, 0.55f, maxAllowedH);
    outW = std::clamp(reqW, 0.55f, std::max(0.60f, maxFacadeW - 0.60f));
}

/**
 * Computes human-scale doorway dimensions guaranteed never to exceed ground floor ceiling height
 * or wall span.
 */
inline void clampDoorDims(
    const ResidentialParams& p,
    float flH,
    float maxWallSpan,
    float defaultW,
    float defaultH,
    float& outW,
    float& outH
) {
    float reqW = p.useExtendedParams ? p.doorWidth : defaultW;
    float reqH = p.useExtendedParams ? p.doorHeight : defaultH;
    outW = std::clamp(reqW, 0.85f, std::max(0.95f, maxWallSpan - 0.60f));
    outH = std::clamp(reqH, 1.95f, std::max(2.00f, flH - 0.18f));
}

/**
 * Generates an entrance door cleanly clamped to wall geometry.
 */
inline void appendScaledDoor(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    const ResidentialParams& p,
    const Vec3& centerGround,
    float flH,
    float maxWallSpan,
    const Vec3& normal,
    const Vec3& tangent,
    DoorType defaultDoor,
    float defaultW = 1.2f,
    float defaultH = 2.25f,
    bool defaultCanopy = true
) {
    DoorType dt = doorType(p, defaultDoor);
    if (dt == DoorType::None) return;
    float dw, dh;
    clampDoorDims(p, flH, maxWallSpan, defaultW, defaultH, dw, dh);
    bool canopy = p.useExtendedParams ? p.hasEntranceCanopy : defaultCanopy;
    BuildingComponents::generateDoor(outVerts, outIndices, centerGround, dw, dh, normal, tangent, dt, canopy);
}

/**
 * Generates a row of strictly-scaled windows along a facade centered at facadeCenter
 * (center at floor ground y0), automatically enforcing pier margins, collision avoidance,
 * and optional center doorway exclusion.
 */
inline void appendScaledWindowRow(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    const ResidentialParams& p,
    const Vec3& facadeCenterGround,
    float spanLength,
    float floorY,
    float flH,
    const Vec3& normal,
    const Vec3& tangent,
    int defaultCount,
    WindowType defaultWin,
    GlassType defaultGlass = GlassType::ClearFloat,
    float defaultWinW = 1.35f,
    float defaultWinH = 1.55f,
    float defaultSill = 0.80f,
    float excludeMinU = 0.0f,
    float excludeMaxU = 0.0f,
    bool defaultShutters = false,
    bool defaultSunshades = false
) {
    if (spanLength < 1.2f) return;
    int reqCount = p.useExtendedParams ? p.windowsPerFacade : defaultCount;
    if (reqCount <= 0) return;

    float winW, winH, sill;
    clampWindowDims(p, flH, spanLength, defaultWinW, defaultWinH, defaultSill, winW, winH, sill);

    float edgeMargin = 0.45f;
    float minGap = 0.35f;
    float usableSpan = std::max(0.8f, spanLength - 2.0f * edgeMargin);
    int maxFit = std::max(1, static_cast<int>((usableSpan + minGap) / (winW + minGap)));
    int count = std::min(reqCount, maxFit);
    if (count > 1) {
        float maxWPerSlot = (usableSpan - (count - 1) * minGap) / static_cast<float>(count);
        if (maxWPerSlot < winW) {
            winW = std::max(0.55f, maxWPerSlot);
        }
    }

    float winCenterY = floorY + sill + winH * 0.5f;
    WindowType wt = windowType(p, defaultWin);
    GlassType gt = glassType(p, defaultGlass);
    bool shutters = showShutters(p, defaultShutters);
    bool louvres = showSunshades(p, defaultSunshades);

    float slotW = usableSpan / static_cast<float>(count);
    for (int i = 0; i < count; ++i) {
        float u = -usableSpan * 0.5f + (i + 0.5f) * slotW;
        float uL = u - winW * 0.5f;
        float uR = u + winW * 0.5f;
        if (excludeMaxU > excludeMinU && uR > excludeMinU - 0.15f && uL < excludeMaxU + 0.15f) {
            continue;
        }
        Vec3 pos = facadeCenterGround + tangent * u + Vec3{0.0f, winCenterY, 0.0f};
        BuildingComponents::generateWindow(outVerts, outIndices, pos, winW, winH, normal, tangent, wt, gt);
        if (shutters && slotW > winW * 1.45f) {
            BuildingComponents::generateWindowShutters(outVerts, outIndices, pos, winW, winH, normal, tangent);
        }
        if (louvres) {
            Vec3 topPos = pos + Vec3{0.0f, winH * 0.5f + 0.06f, 0.0f};
            BuildingComponents::generateSunLouvres(outVerts, outIndices, topPos, winW + 0.16f, 0.38f, normal, tangent);
        }
    }
}

/**
 * Generates adaptive roof geometry via RoofGen, and guarantees an enclosed horizontal roof cap
 * when RoofType::NoneFlat is chosen so hollow-walled buildings never remain open at the top.
 */
inline void appendAdaptiveRoof(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    const ResidentialParams& p,
    float baseWidth,
    float baseDepth,
    float baseTopY,
    RoofType defaultRoof,
    float defaultRoofH = 2.6f,
    float defaultOverhang = 0.5f,
    bool defaultClutter = false
) {
    RoofType rt = roofType(p, defaultRoof);
    float ov = roofOverhang(p, defaultOverhang);
    if (rt == RoofType::NoneFlat) {
        float hw = baseWidth * 0.5f + ov * 0.35f;
        float hd = baseDepth * 0.5f + ov * 0.35f;
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{-hw, baseTopY - 0.05f, -hd},
            Vec3{hw, baseTopY + 0.22f, hd},
            static_cast<uint32_t>(MaterialSlot::RoofSurface)
        );
        return;
    }

    RoofGenParams rParams;
    rParams.type = rt;
    rParams.baseWidth = baseWidth;
    rParams.baseDepth = baseDepth;
    rParams.baseTopY = baseTopY;
    rParams.roofHeight = roofHeight(p, defaultRoofH);
    rParams.overhang = ov;
    rParams.showRooftopClutter = showClutter(p, defaultClutter);
    RoofGen::generate(outVerts, outIndices, rParams);
}

/**
 * Generates an adaptive wing/garage roof centered at world XZ (centerX, centerZ),
 * keeping its structural footprint aligned over the wing walls.
 */
inline void appendWingRoof(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    const ResidentialParams& p,
    float centerX,
    float centerZ,
    float baseWidth,
    float baseDepth,
    float baseTopY,
    RoofType wingRoof,
    float defaultRoofH = 1.6f,
    float defaultOverhang = 0.35f
) {
    size_t startVert = outVerts.size();
    float ov = std::clamp(roofOverhang(p, defaultOverhang), 0.0f, 0.55f);
    if (wingRoof == RoofType::NoneFlat) {
        float hw = baseWidth * 0.5f + ov * 0.35f;
        float hd = baseDepth * 0.5f + ov * 0.35f;
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{-hw, baseTopY - 0.05f, -hd},
            Vec3{hw, baseTopY + 0.22f, hd},
            static_cast<uint32_t>(MaterialSlot::RoofSurface)
        );
    } else {
        RoofGenParams rParams;
        rParams.type = wingRoof;
        rParams.baseWidth = baseWidth;
        rParams.baseDepth = baseDepth;
        rParams.baseTopY = baseTopY;
        rParams.roofHeight = std::min(defaultRoofH, roofHeight(p, defaultRoofH));
        rParams.overhang = (wingRoof == RoofType::FlatParapet || wingRoof == RoofType::GreenRoofTerrace) ? 0.0f : ov;
        rParams.showRooftopClutter = false;
        RoofGen::generate(outVerts, outIndices, rParams);
    }
    for (size_t i = startVert; i < outVerts.size(); ++i) {
        outVerts[i].position.x += centerX;
        outVerts[i].position.z += centerZ;
    }
}

/**
 * Emits continuous exterior floor belt courses / cornice trim around a rectangular perimeter.
 */
inline void appendBeltCourses(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float hw,
    float hd,
    int32_t stories,
    float flH,
    uint32_t beltMat
) {
    for (int s = 1; s < stories; ++s) {
        float fy = s * flH;
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{-hw - 0.08f, fy - 0.08f, -hd - 0.08f},
            Vec3{hw + 0.08f, fy + 0.08f, hd + 0.08f},
            beltMat
        );
    }
}

} // namespace procedural::buildings::res_scale
