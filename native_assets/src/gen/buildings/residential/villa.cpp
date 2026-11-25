#include "HouseGen.h"
#include "HouseScaleHelpers.h"

namespace procedural::buildings {

void HouseGen::generateVilla(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    int32_t stories = res_scale::stories(params_, 1, 6);
    float flH = res_scale::floorHeight(params_, 3.4f);
    float w = res_scale::width(params_, 8.0f, 18.0f);
    float d = res_scale::depth(params_, 6.0f, 12.0f);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallMat = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t timberMat = static_cast<uint32_t>(MaterialSlot::PergolaWood);

    // Terraced luxury villa plinth
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw - 1.0f, -0.5f, -hd - 1.0f},
        Vec3{hw + 1.0f, 0.0f, hd + 1.0f},
        foundMat
    );

    // Main multi-storey West Pavilion (-hw to +0.5m)
    float splitX = w * 0.05f;
    float leftW = splitX - (-hw);
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd},
        Vec3{splitX, h, hd},
        wallMat
    );

    // East Wing (lower terrace pavilion from splitX to hw)
    float eastStories = std::max(1, stories - (stories > 1 ? 1 : 0));
    float eastH = eastStories * flH;
    float eastHd = hd * 0.72f;
    float eastW = hw - splitX;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{splitX, 0.0f, -eastHd},
        Vec3{hw, eastH, eastHd},
        wallMat
    );

    if (res_scale::showCornice(params_, true)) {
        res_scale::appendBeltCourses(outVerts, outIndices, (leftW * 0.5f), hd, stories, flH, frameMat);
    }

    // Modern flush entry door on main west wing
    float doorX = -hw * 0.25f;
    float doorW, doorH;
    res_scale::clampDoorDims(params_, flH, leftW, 1.5f, 2.4f, doorW, doorH);
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{doorX, 0.0f, hd + 0.02f}, flH, leftW,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::SingleModern, 1.5f, 2.4f, true
    );

    // Scaled panoramic windows on West Pavilion & East Pavilion
    float leftCenterX = (-hw + splitX) * 0.5f;
    float eastCenterX = (splitX + hw) * 0.5f;
    for (int s = 0; s < stories; ++s) {
        float y0 = s * flH;
        if (s == 0) {
            float leftRoom = std::max(1.2f, doorX - doorW * 0.6f - (-hw) - 0.5f);
            float winCenterX = -hw + 0.4f + leftRoom * 0.5f;
            float wW, wH, sill;
            res_scale::clampWindowDims(params_, flH, leftRoom, 2.8f, 2.1f, 0.60f, wW, wH, sill);
            BuildingComponents::generateWindow(
                outVerts, outIndices,
                Vec3{winCenterX, y0 + sill + wH * 0.5f, hd + 0.02f},
                wW, wH, Vec3{0, 0, 1}, Vec3{1, 0, 0},
                res_scale::windowType(params_, WindowType::PictureGlass),
                res_scale::glassType(params_, GlassType::ClearFloat)
            );
        } else {
            res_scale::appendScaledWindowRow(
                outVerts, outIndices, params_,
                Vec3{leftCenterX, 0.0f, hd + 0.02f}, leftW, y0, flH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                2, WindowType::PictureGlass, GlassType::ClearFloat, 2.6f, 2.0f, 0.65f
            );
        }

        // Rear facade (-Z) on West Pavilion
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{leftCenterX, 0.0f, -hd - 0.02f}, leftW, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            2, WindowType::PictureGlass, GlassType::ClearFloat, 2.4f, 1.9f, 0.70f
        );

        // East Pavilion windows if story <= eastStories - 1
        if (s < eastStories) {
            res_scale::appendScaledWindowRow(
                outVerts, outIndices, params_,
                Vec3{eastCenterX, 0.0f, eastHd + 0.02f}, eastW, y0, flH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                2, WindowType::PictureGlass, GlassType::ClearFloat, 2.6f, 2.0f, 0.65f
            );
        }
    }

    // Open pergola terrace on East wing roof
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{splitX + 0.3f, eastH, -eastHd * 0.9f},
            Vec3{hw - 0.3f, eastH + 0.16f, eastHd * 0.9f},
            frameMat
        );
        for (float px = splitX + 0.6f; px < hw - 0.4f; px += 1.4f) {
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{px - 0.06f, eastH + 0.16f, -eastHd * 0.85f},
                Vec3{px + 0.06f, eastH + 0.28f, eastHd * 0.85f},
                timberMat
            );
        }
    }

    // Solid enclosed roof on West Pavilion
    float roofBaseCenterX = (-hw + splitX) * 0.5f;
    // Emit flat/pitched roof properly centered over west wing if roof differs from flat
    RoofType rt = res_scale::roofType(params_, RoofType::NoneFlat);
    if (rt == RoofType::NoneFlat) {
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{-hw - 0.35f, h - 0.05f, -hd - 0.35f},
            Vec3{splitX + 0.35f, h + 0.24f, hd + 0.35f},
            static_cast<uint32_t>(MaterialSlot::RoofSurface)
        );
    } else {
        // Offset vertices around west wing center temporarily via RoofGen
        std::vector<BuildingVertex> roofVerts;
        std::vector<uint32_t> roofIdx;
        RoofGenParams rp;
        rp.type = rt;
        rp.baseWidth = leftW;
        rp.baseDepth = d;
        rp.baseTopY = h;
        rp.roofHeight = res_scale::roofHeight(params_, 2.2f);
        rp.overhang = res_scale::roofOverhang(params_, 0.6f);
        rp.showRooftopClutter = res_scale::showClutter(params_, false);
        RoofGen::generate(roofVerts, roofIdx, rp);
        uint32_t baseOff = static_cast<uint32_t>(outVerts.size());
        for (auto v : roofVerts) {
            v.position.x += roofBaseCenterX;
            outVerts.push_back(v);
        }
        for (uint32_t idx : roofIdx) {
            outIndices.push_back(baseOff + idx);
        }
    }
}

} // namespace procedural::buildings
