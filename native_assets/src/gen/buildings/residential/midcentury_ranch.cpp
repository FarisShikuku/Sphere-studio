#include "HouseGen.h"
#include "HouseScaleHelpers.h"

namespace procedural::buildings {

void HouseGen::generateMidCenturyRanch(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    int32_t stories = res_scale::stories(params_, 1, 4);
    float w = res_scale::width(params_, 10.0f, 24.0f);
    float d = res_scale::depth(params_, 7.0f, 12.0f);
    float flH = res_scale::floorHeight(params_, 4.0f);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallMat = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t timberMat = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t stoneMat = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t metalMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);

    // 1. Concrete foundation slab
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw - 0.4f, -0.3f, -hd - 0.4f},
        Vec3{hw + 0.4f, 0.0f, hd + 0.4f},
        foundMat
    );

    // 2. Main living volume (-hw to houseRightX)
    float houseRightX = params_.hasGarage ? (hw * 0.38f) : hw;
    float livingW = houseRightX - (-hw);
    float livingCenterX = (-hw + houseRightX) * 0.5f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd},
        Vec3{houseRightX, h, hd},
        wallMat
    );

    // Horizontal cedar cladding band along top of ground story
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw - 0.02f, flH * 0.30f, hd - 0.01f},
        Vec3{houseRightX + 0.02f, flH - 0.35f, hd + 0.05f},
        timberMat
    );

    // 3. Signature stacked limestone hearth chimney spine
    float stoneWallX = -hw + livingW * 0.34f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{stoneWallX - 0.75f, 0.0f, -hd - 0.9f},
        Vec3{stoneWallX + 0.75f, h + 1.6f, hd + 1.1f},
        stoneMat
    );

    // 4. Integrated open-air carport / garage wing on +X side
    if (params_.hasGarage) {
        float portH = std::min(flH, 3.8f);
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{houseRightX, 0.0f, -hd},
            Vec3{hw, 0.12f, hd + 0.4f},
            foundMat
        );
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{houseRightX, 0.12f, -hd},
            Vec3{hw, portH, -hd + 0.35f},
            timberMat
        );
        // Carport flat canopy beam & slender steel posts
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{houseRightX, portH - 0.18f, -hd},
            Vec3{hw + 0.5f, portH + 0.12f, hd + 0.5f},
            roofMat
        );
        for (float px : {houseRightX + 0.35f, hw - 0.35f}) {
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{px, 0.12f, hd - 0.2f},
                Vec3{px, portH - 0.18f, hd - 0.2f},
                0.06f, 0.06f, 10, metalMat, true, true
            );
        }
    }

    // 5. Scaled entrance doorway near living-carport junction
    float doorX = houseRightX - 1.35f;
    float doorW, doorH;
    res_scale::clampDoorDims(params_, flH, livingW, 1.3f, 2.35f, doorW, doorH);
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{doorX, 0.0f, hd + 0.06f}, flH, livingW,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::SingleModern, 1.3f, 2.35f, true
    );

    // 6. Scaled picture/sliding windows per story avoiding chimney & door
    for (int s = 0; s < stories; ++s) {
        float y0 = s * flH;
        // Left of hearth chimney
        float leftSpan = (stoneWallX - 0.85f) - (-hw);
        if (leftSpan > 1.5f) {
            res_scale::appendScaledWindowRow(
                outVerts, outIndices, params_,
                Vec3{(-hw + stoneWallX - 0.85f) * 0.5f, 0.0f, hd + 0.06f},
                leftSpan, y0, flH, Vec3{0, 0, 1}, Vec3{1, 0, 0},
                1, WindowType::PictureGlass, GlassType::ClearFloat, 3.2f, 2.3f, 0.45f
            );
        }
        // Center-right of hearth chimney
        float rightEnd = (s == 0) ? (doorX - doorW * 0.6f - 0.2f) : houseRightX;
        float midSpan = rightEnd - (stoneWallX + 0.85f);
        if (midSpan > 1.5f) {
            res_scale::appendScaledWindowRow(
                outVerts, outIndices, params_,
                Vec3{(stoneWallX + 0.85f + rightEnd) * 0.5f, 0.0f, hd + 0.06f},
                midSpan, y0, flH, Vec3{0, 0, 1}, Vec3{1, 0, 0},
                1, WindowType::SlidingPocketSash, GlassType::ClearFloat, 2.8f, 2.2f, 0.45f
            );
        }
        // Rear facade windows (-Z)
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{livingCenterX, 0.0f, -hd - 0.02f}, livingW, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            3, WindowType::RibbonStrip, GlassType::ClearFloat, 2.2f, 1.4f, 1.1f
        );
    }

    // 7. Mid-century patio terrace slab & stone planter
    if (res_scale::showBalconies(params_)) {
        float patioD = std::clamp(res_scale::balconyDepth(params_, 2.6f), 1.4f, 3.4f);
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{-hw * 0.92f, 0.0f, hd},
            Vec3{houseRightX - 0.3f, 0.12f, hd + patioD},
            foundMat
        );
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{-hw * 0.92f, 0.12f, hd + patioD - 0.35f},
            Vec3{stoneWallX + 1.0f, 0.55f, hd + patioD},
            stoneMat
        );
    }

    // 8. Adaptive Low-Pitch / Shed / Butterfly Roof over living volume
    std::vector<BuildingVertex> rVerts;
    std::vector<uint32_t> rIdx;
    res_scale::appendAdaptiveRoof(
        rVerts, rIdx, params_,
        livingW, d, h, RoofType::ShedMonopitch, 1.4f, 1.1f, false
    );
    uint32_t bOff = static_cast<uint32_t>(outVerts.size());
    for (auto v : rVerts) {
        v.position.x += livingCenterX;
        outVerts.push_back(v);
    }
    for (uint32_t idx : rIdx) {
        outIndices.push_back(bOff + idx);
    }
}

} // namespace procedural::buildings
