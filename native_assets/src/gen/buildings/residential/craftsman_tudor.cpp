#include "HouseGen.h"
#include "HouseScaleHelpers.h"

namespace procedural::buildings {

void HouseGen::generateCraftsmanTudor(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    int32_t stories = res_scale::stories(params_, 1, 6);
    float w = res_scale::width(params_, 8.0f, 14.0f);
    float d = res_scale::depth(params_, 7.0f, 12.0f);
    float flH = res_scale::floorHeight(params_, 3.2f);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t stoneMat = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t upperWallMat = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t timberMat = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t brickMat = static_cast<uint32_t>(MaterialSlot::WallBrick);

    // 1. Stone foundation plinth
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw - 0.35f, -0.4f, -hd - 0.35f},
        Vec3{hw + 0.35f, 0.0f, hd + 0.35f},
        foundMat
    );

    // 2. Ground floor rustic fieldstone base
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd},
        Vec3{hw, flH, hd},
        stoneMat
    );

    // 3. Upper stories half-timbered volume
    if (stories > 1) {
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{-hw, flH, -hd},
            Vec3{hw, h, hd},
            upperWallMat
        );

        // Half-timbering horizontal beams & vertical posts per upper story
        for (int s = 1; s < stories; ++s) {
            float y0 = s * flH;
            float y1 = (s + 1) * flH;
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{-hw - 0.04f, y0, -hd - 0.04f},
                Vec3{hw + 0.04f, y0 + 0.16f, hd + 0.05f},
                timberMat
            );
            int posts = std::max(4, static_cast<int>(w * 0.5f));
            for (int p = 0; p < posts; ++p) {
                float px = -hw + p * (w / (posts - 1));
                BuildingComponents::appendBox(
                    outVerts, outIndices,
                    Vec3{px - 0.08f, y0, hd - 0.02f},
                    Vec3{px + 0.08f, y1, hd + 0.05f},
                    timberMat
                );
            }
        }
    }

    // 4. Steep Tudor Cross-Gable Wing (front left)
    float gableW = std::clamp(w * 0.44f, 3.2f, 6.8f);
    float gableX = -hw + gableW * 0.52f;
    float gableProj = 1.15f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{gableX - gableW * 0.5f, 0.0f, hd},
        Vec3{gableX + gableW * 0.5f, h, hd + gableProj},
        upperWallMat
    );

    // 5. Covered Craftsman Front Porch (front right)
    float porchW = std::max(2.6f, (hw - 0.25f) - (gableX + gableW * 0.5f));
    float porchX = (gableX + gableW * 0.5f) + porchW * 0.5f + 0.12f;
    float porchD = std::clamp(res_scale::balconyDepth(params_, 2.4f), 1.4f, 3.2f);
    float porchY = std::min(flH * 0.88f, 2.95f);

    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{porchX - porchW * 0.5f, 0.0f, hd},
            Vec3{porchX + porchW * 0.5f, 0.24f, hd + porchD},
            timberMat
        );
        for (float sideX : {-1.0f, 1.0f}) {
            float cx = porchX + sideX * (porchW * 0.40f);
            float cz = hd + porchD - 0.35f;
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{cx - 0.38f, 0.24f, cz - 0.38f},
                Vec3{cx + 0.38f, 1.15f, cz + 0.38f},
                stoneMat
            );
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{cx - 0.18f, 1.15f, cz - 0.18f},
                Vec3{cx + 0.18f, porchY, cz + 0.18f},
                timberMat
            );
        }
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{porchX - porchW * 0.52f, porchY, hd},
            Vec3{porchX + porchW * 0.52f, porchY + 0.55f, hd + porchD + 0.25f},
            roofMat
        );
    }

    // Scaled front doorway inside porch
    float baseFloorY = res_scale::showBalconies(params_) ? 0.24f : 0.0f;
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{porchX, baseFloorY, hd + 0.02f}, flH - baseFloorY, porchW,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::ArchedWooden, 1.3f, 2.25f, false
    );

    // 6. Scaled Windows across gable bay, porch upper level, rear, and sides
    for (int s = 0; s < stories; ++s) {
        float y0 = s * flH;
        // Projecting front gable bay windows
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{gableX, 0.0f, hd + gableProj + 0.02f}, gableW, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            1, WindowType::DiamondLeadGlazing, GlassType::ClearFloat, 2.0f, 1.65f, 0.78f
        );
        // Right front section windows (above porch)
        if (s > 0) {
            res_scale::appendScaledWindowRow(
                outVerts, outIndices, params_,
                Vec3{porchX, 0.0f, hd + 0.02f}, porchW, y0, flH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                1, WindowType::DiamondLeadGlazing, GlassType::ClearFloat, 1.6f, 1.55f, 0.80f
            );
        }
        // Rear windows (-Z)
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            3, WindowType::DiamondLeadGlazing, GlassType::ClearFloat, 1.4f, 1.55f, 0.80f
        );
        // Side windows (+X)
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            2, WindowType::DiamondLeadGlazing, GlassType::ClearFloat, 1.3f, 1.50f, 0.80f
        );
    }

    // 7. Adaptive Main Roof + Cross Gable Peak over Tudor Bay
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, h, RoofType::Gable, 3.4f, 0.6f, false
    );

    if (res_scale::roofType(params_, RoofType::Gable) == RoofType::Gable ||
        res_scale::roofType(params_, RoofType::Gable) == RoofType::Hip) {
        float peakH = res_scale::roofHeight(params_, 3.4f) * 0.92f;
        Vec3 gL{gableX - gableW * 0.5f, h, hd + gableProj};
        Vec3 gR{gableX + gableW * 0.5f, h, hd + gableProj};
        Vec3 gTop{gableX, h + peakH, hd + gableProj};
        BuildingComponents::appendQuad(outVerts, outIndices, gL, gR, gTop, gTop, Vec3{0, 0, 1}, upperWallMat);
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{gableX - gableW * 0.52f, h + peakH * 0.25f, hd + 0.1f},
            Vec3{gableX + gableW * 0.52f, h + peakH * 0.92f, hd + gableProj + 0.15f},
            roofMat
        );
    }

    // 8. Exterior Stone Chimney (-X side)
    float chimW = std::clamp(d * 0.18f, 1.3f, 2.2f);
    float chimD = 1.25f;
    float chimH = h + res_scale::roofHeight(params_, 3.4f) + 0.8f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw - chimD, 0.0f, -chimW * 0.5f},
        Vec3{-hw, flH * 1.1f, chimW * 0.5f},
        stoneMat
    );
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw - chimD * 0.72f, flH * 1.1f, -chimW * 0.35f},
        Vec3{-hw, chimH, chimW * 0.35f},
        brickMat
    );
    for (float cyz : {-0.26f, 0.26f}) {
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{-hw - chimD * 0.38f, chimH, cyz},
            Vec3{-hw - chimD * 0.38f, chimH + 0.65f, cyz},
            0.16f, 0.14f, 10, stoneMat, true, true
        );
    }
}

} // namespace procedural::buildings
