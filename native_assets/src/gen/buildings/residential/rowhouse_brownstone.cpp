#include "HouseGen.h"
#include "HouseScaleHelpers.h"

namespace procedural::buildings {

void HouseGen::generateRowhouseBrownstone(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    int32_t stories = res_scale::stories(params_, 2, 10);
    float w = res_scale::width(params_, 6.0f, 8.5f);
    float d = res_scale::depth(params_, 10.0f, 16.0f);
    float flH = res_scale::floorHeight(params_, 3.4f);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t brickMat = res_scale::wallMat(params_, MaterialSlot::WallBrick);
    uint32_t stoneMat = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t metalMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);

    // 1. Foundation plinth
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw - 0.2f, -0.4f, -hd - 0.2f},
        Vec3{hw + 0.2f, 0.0f, hd + 0.2f},
        foundMat
    );

    // 2. Main masonry block
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd},
        Vec3{hw, h, hd},
        brickMat
    );

    // 3. Limestone belt courses between each floor
    if (res_scale::showCornice(params_, true)) {
        res_scale::appendBeltCourses(outVerts, outIndices, hw, hd, stories, flH, stoneMat);
    }

    // 4. Ground floor rusticated limestone base strip
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw - 0.02f, 0.0f, hd - 0.01f},
        Vec3{hw + 0.02f, flH, hd + 0.08f},
        stoneMat
    );

    // 5. Scaled elevated entrance stoop (front right)
    float stoopW = std::clamp(w * 0.28f, 1.5f, 2.6f);
    float stoopX = hw - stoopW * 0.62f;
    float stoopH = std::clamp(flH * 0.42f, 0.9f, 1.6f);
    float stoopL = std::clamp(stoopH * 1.9f, 1.8f, 3.2f);
    int numSteps = 7;
    float stepH = stoopH / numSteps;
    float stepL = stoopL / numSteps;

    for (int st = 0; st < numSteps; ++st) {
        float y0 = st * stepH;
        float y1 = (st + 1) * stepH;
        float z0 = hd + stoopL - (st + 1) * stepL;
        float z1 = hd + stoopL;
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{stoopX - stoopW * 0.5f, y0, z0},
            Vec3{stoopX + stoopW * 0.5f, y1, z1},
            stoneMat
        );
    }

    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{stoopX - stoopW * 0.5f, 0.0f, hd},
        Vec3{stoopX + stoopW * 0.5f, stoopH, hd + 1.15f},
        stoneMat
    );

    // Wrought-iron railings on stoop
    for (float side : {-1.0f, 1.0f}) {
        float rx = stoopX + side * (stoopW * 0.48f);
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{rx, stoopH + 0.85f, hd + 0.2f},
            Vec3{rx, 0.85f, hd + stoopL},
            0.03f, 0.03f, 8, metalMat, true, true
        );
        for (int b = 0; b < 6; ++b) {
            float frac = static_cast<float>(b) / 5.0f;
            float bz = (hd + 0.2f) + frac * (stoopL - 0.2f);
            float by0 = stoopH * (1.0f - frac);
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{rx, by0, bz},
                Vec3{rx, by0 + 0.85f, bz},
                0.016f, 0.016f, 6, metalMat, true, true
            );
        }
    }

    // Parlor entrance door atop stoop (clamped so door fits under flH + sill)
    float availDoorH = std::max(2.0f, (stories > 1 ? 1.5f * flH : flH) - stoopH - 0.25f);
    float doorW, doorH;
    res_scale::clampDoorDims(params_, availDoorH + 0.2f, stoopW, 1.35f, 2.35f, doorW, doorH);
    DoorType dt = res_scale::doorType(params_, DoorType::ArchedWooden);
    if (dt != DoorType::None) {
        BuildingComponents::generateDoor(
            outVerts, outIndices,
            Vec3{stoopX, stoopH, hd + 0.08f},
            doorW, doorH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            dt, params_.useExtendedParams ? params_.hasEntranceCanopy : false
        );
    }

    // 6. Projecting Oriel Bay Window on intermediate stories (left bay)
    float bayW = std::clamp(w * 0.42f, 2.0f, 3.8f);
    float bayX = -hw + bayW * 0.62f;
    float bayProj = std::clamp(res_scale::balconyDepth(params_, 1.0f), 0.7f, 1.35f);
    float bayY0 = flH * 1.02f;
    float bayY1 = std::max(bayY0 + flH * 0.8f, (stories - 1) * flH);

    if (stories >= 3) {
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{bayX - bayW * 0.46f, flH - 0.40f, hd},
            Vec3{bayX + bayW * 0.46f, flH, hd + bayProj * 0.85f},
            stoneMat
        );
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{bayX - bayW * 0.36f, bayY0, hd},
            Vec3{bayX + bayW * 0.36f, bayY1, hd + bayProj},
            stoneMat
        );
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{bayX - bayW * 0.50f, bayY0, hd},
            Vec3{bayX - bayW * 0.30f, bayY1, hd + bayProj * 0.75f},
            stoneMat
        );
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{bayX + bayW * 0.30f, bayY0, hd},
            Vec3{bayX + bayW * 0.50f, bayY1, hd + bayProj * 0.75f},
            stoneMat
        );
    }

    // Scaled front & rear windows per floor
    float bayWinW, bayWinH, baySill;
    res_scale::clampWindowDims(params_, flH, bayW * 0.68f, 1.25f, 1.75f, 0.75f, bayWinW, bayWinH, baySill);
    WindowType wt = res_scale::windowType(params_, WindowType::DoubleHungSash);
    GlassType gt = res_scale::glassType(params_, GlassType::ClearFloat);

    for (int s = 0; s < stories; ++s) {
        float y0 = s * flH;
        if (s >= 1 && s < stories - 1 && stories >= 3) {
            float wy = y0 + baySill + bayWinH * 0.5f;
            BuildingComponents::generateWindow(
                outVerts, outIndices,
                Vec3{bayX, wy, hd + bayProj + 0.02f},
                bayWinW, bayWinH, Vec3{0, 0, 1}, Vec3{1, 0, 0}, wt, gt
            );
            BuildingComponents::generateWindow(
                outVerts, outIndices,
                Vec3{stoopX, wy, hd + 0.02f},
                std::min(bayWinW, stoopW * 0.72f), bayWinH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0}, wt, gt
            );
        } else {
            float exclMin = (s == 0) ? (stoopX - stoopW * 0.55f) : 0.0f;
            float exclMax = (s == 0) ? (stoopX + stoopW * 0.55f) : 0.0f;
            res_scale::appendScaledWindowRow(
                outVerts, outIndices, params_,
                Vec3{0.0f, 0.0f, hd + 0.08f}, w, y0, flH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                3, WindowType::ArchedHistoric, GlassType::ClearFloat,
                1.15f, 1.65f, 0.78f,
                exclMin, exclMax
            );
        }

        // Rear windows (-Z)
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            3, WindowType::DoubleHungSash, GlassType::ClearFloat,
            1.15f, 1.65f, 0.78f
        );
    }

    // 7. Classical corbelled dentil cornice at roofline
    if (res_scale::showCornice(params_, true)) {
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{-hw - 0.30f, h - 0.22f, -hd - 0.20f},
            Vec3{hw + 0.30f, h + 0.38f, hd + 0.42f},
            stoneMat
        );
        int modillionCount = std::max(4, static_cast<int>(w * 1.1f));
        for (int m = 0; m < modillionCount; ++m) {
            float mx = -hw * 0.88f + m * (hw * 1.76f / (modillionCount - 1));
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{mx - 0.12f, h - 0.36f, hd + 0.05f},
                Vec3{mx + 0.12f, h, hd + 0.38f},
                stoneMat
            );
        }
    }

    // 8. Adaptive mansard or user-selected roof
    float roofBaseY = res_scale::showCornice(params_, true) ? (h + 0.38f) : h;
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, roofBaseY, RoofType::Mansard, 2.4f, 0.35f, false
    );

    // Party-wall chimneys
    float rH = res_scale::roofHeight(params_, 2.4f);
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw, h, -0.5f},
        Vec3{-hw + 0.85f, roofBaseY + rH + 1.1f, 0.9f},
        brickMat
    );
}

} // namespace procedural::buildings
