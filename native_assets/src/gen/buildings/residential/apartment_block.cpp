#include "HouseGen.h"
#include "HouseScaleHelpers.h"

namespace procedural::buildings {

void HouseGen::generateApartmentBlock(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    int32_t stories = res_scale::stories(params_, 1, 24);
    float w = res_scale::width(params_, 8.0f, 22.0f);
    float d = res_scale::depth(params_, 7.0f, 14.0f);
    float flH = res_scale::floorHeight(params_, 3.2f);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallMat = res_scale::wallMat(params_, MaterialSlot::WallConcrete);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);

    // Foundation plinth
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw - 0.4f, -0.6f, -hd - 0.4f},
        Vec3{hw + 0.4f, 0.0f, hd + 0.4f},
        foundMat
    );

    // Main apartment volume
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd},
        Vec3{hw, h, hd},
        wallMat
    );

    // Cornice & belt courses
    if (res_scale::showCornice(params_, true)) {
        res_scale::appendBeltCourses(outVerts, outIndices, hw, hd, stories, flH, frameMat);
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, h, 0.35f);
    }

    // Scaled Lobby Entrance Door on floor 0
    float doorW, doorH;
    res_scale::clampDoorDims(params_, flH, w, 2.2f, 2.5f, doorW, doorH);
    DoorType dt = res_scale::doorType(params_, DoorType::DoubleGlassCommercial);
    if (dt != DoorType::None) {
        res_scale::appendScaledDoor(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, hd + 0.02f}, flH, w,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::DoubleGlassCommercial, 2.2f, 2.5f, true
        );
    }

    // Windows & cantilevered balconies per floor
    float doorExclHalf = (dt != DoorType::None) ? (doorW * 0.55f + 0.45f) : 0.0f;
    float balcD = res_scale::balconyDepth(params_, 1.2f);
    bool hasBalc = res_scale::showBalconies(params_);

    for (int story = 0; story < stories; ++story) {
        float y0 = story * flH;

        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, hd + 0.02f}, w, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            4, WindowType::ModernCasement, GlassType::ClearFloat,
            1.6f, 1.8f, 0.75f,
            (story == 0) ? -doorExclHalf : 0.0f, (story == 0) ? doorExclHalf : 0.0f
        );

        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, WindowType::ModernCasement, GlassType::ClearFloat,
            1.6f, 1.8f, 0.75f
        );

        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            std::max(1, params_.windowsPerFacade / 2), WindowType::ModernCasement, GlassType::ClearFloat,
            1.4f, 1.7f, 0.80f
        );

        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            std::max(1, params_.windowsPerFacade / 2), WindowType::ModernCasement, GlassType::ClearFloat,
            1.4f, 1.7f, 0.80f
        );

        if (hasBalc && story >= 1 && w >= 8.0f) {
            float bw = std::clamp(w * 0.22f, 2.0f, 4.2f);
            float offset = std::max(bw * 0.65f, hw - bw * 0.5f - 1.2f);
            for (float side : {-1.0f, 1.0f}) {
                BuildingComponents::generateBalcony(
                    outVerts, outIndices,
                    Vec3{side * offset, y0, hd},
                    bw, balcD, Vec3{0, 0, 1}, Vec3{1, 0, 0},
                    BalconyType::CantileveredGlass
                );
            }
        }
    }

    // Adaptive roof (defaults to FlatParapet with rooftop clutter)
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, h, RoofType::FlatParapet, 2.4f, 0.5f, true
    );
}

} // namespace procedural::buildings
