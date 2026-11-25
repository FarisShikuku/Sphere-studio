#include "HouseGen.h"
#include "HouseScaleHelpers.h"

namespace procedural::buildings {

void HouseGen::generateSuburbanHouse(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    int32_t stories = res_scale::stories(params_, 1, 8);
    float flH = res_scale::floorHeight(params_, 3.2f);
    float w = res_scale::width(params_, 5.5f, 14.0f);
    float d = res_scale::depth(params_, 5.0f, 10.0f);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallMat = res_scale::wallMat(params_, MaterialSlot::WallConcrete);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t trimMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);

    // 1. Foundation plinth
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw - 0.3f, -0.4f, -hd - 0.3f},
        Vec3{hw + 0.3f, 0.0f, hd + 0.3f},
        foundMat
    );

    // 2. Main enclosed house volume (wall-solid perimeter prevents holes or z-fighting when resizing)
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd},
        Vec3{hw, h, hd},
        wallMat
    );

    // Belt courses between floors
    if (res_scale::showCornice(params_, true)) {
        res_scale::appendBeltCourses(outVerts, outIndices, hw, hd, stories, flH, trimMat);
    }

    // 3. Scaled ground-floor entrance door & porch canopy (+Z facade)
    float doorW, doorH;
    res_scale::clampDoorDims(params_, flH, w, 1.25f, 2.25f, doorW, doorH);
    DoorType dt = res_scale::doorType(params_, DoorType::SingleModern);
    if (dt != DoorType::None) {
        res_scale::appendScaledDoor(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, hd + 0.02f}, flH, w,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::SingleModern, 1.25f, 2.25f, true
        );

        // Front porch columns when enabled & wide enough
        if (res_scale::showBalconies(params_) && w >= 6.0f) {
            float porchHalfW = std::min(doorW * 0.65f + 0.25f, hw * 0.45f);
            float colH = std::min(doorH + 0.15f, flH - 0.12f);
            float porchD = std::clamp(res_scale::balconyDepth(params_, 1.0f), 0.7f, 1.8f);
            for (float side : {-1.0f, 1.0f}) {
                BuildingComponents::appendBox(
                    outVerts, outIndices,
                    Vec3{side * porchHalfW - 0.07f, 0.0f, hd + porchD - 0.07f},
                    Vec3{side * porchHalfW + 0.07f, colH, hd + porchD + 0.07f},
                    trimMat
                );
            }
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{-porchHalfW - 0.15f, colH, hd},
                Vec3{porchHalfW + 0.15f, colH + 0.14f, hd + porchD + 0.18f},
                trimMat
            );
        }
    }

    // 4. Story-by-story strictly scaled window rows on all 4 facades
    float doorExclHalf = (dt != DoorType::None) ? (doorW * 0.55f + 0.35f) : 0.0f;
    for (int s = 0; s < stories; ++s) {
        float y0 = s * flH;
        // Front facade (+Z) - avoid door bay on story 0
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, hd + 0.02f}, w, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            3, WindowType::DoubleHungSash, GlassType::ClearFloat,
            1.35f, 1.55f, 0.80f,
            (s == 0) ? -doorExclHalf : 0.0f, (s == 0) ? doorExclHalf : 0.0f
        );

        // Rear facade (-Z)
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            std::max(1, params_.windowsPerFacade), WindowType::DoubleHungSash, GlassType::ClearFloat,
            1.35f, 1.55f, 0.80f
        );

        // Left side wall (-X)
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            std::max(1, params_.windowsPerFacade / 2), WindowType::DoubleHungSash, GlassType::ClearFloat,
            1.30f, 1.50f, 0.85f
        );

        // Right side wall (+X): skip ground floor if garage attached there
        if (!(s == 0 && params_.hasGarage)) {
            res_scale::appendScaledWindowRow(
                outVerts, outIndices, params_,
                Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
                Vec3{1, 0, 0}, Vec3{0, 0, -1},
                std::max(1, params_.windowsPerFacade / 2), WindowType::DoubleHungSash, GlassType::ClearFloat,
                1.30f, 1.50f, 0.85f
            );
        }
    }

    // 5. Attached 1-storey garage proportional to house depth & floor height
    if (params_.hasGarage) {
        float gw = std::clamp(w * 0.40f, 3.6f, 6.8f);
        float gd = std::clamp(d * 0.65f, 4.8f, std::max(4.8f, d - 0.6f));
        float gh = std::clamp(flH * 0.94f, 2.65f, 3.6f);
        float garageDoorW = std::clamp(gw * 0.68f, 2.3f, 5.2f);
        float garageDoorH = std::clamp(gh * 0.80f, 2.05f, gh - 0.25f);

        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{hw, 0.0f, -gd * 0.5f},
            Vec3{hw + gw, gh, gd * 0.5f},
            wallMat
        );
        // Garage roof trim slab
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{hw, gh, -gd * 0.5f - 0.2f},
            Vec3{hw + gw + 0.25f, gh + 0.22f, gd * 0.5f + 0.25f},
            static_cast<uint32_t>(MaterialSlot::RoofSurface)
        );
        // Sectional garage door
        BuildingComponents::generateDoor(
            outVerts, outIndices,
            Vec3{hw + gw * 0.5f, 0.0f, gd * 0.5f + 0.02f},
            garageDoorW, garageDoorH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false
        );
    }

    // 6. Adaptive roof
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, h, RoofType::Gable, 2.8f, 0.6f, false
    );
}

} // namespace procedural::buildings
