#include "CivicGen.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void CivicGen::generatePublicLibraryMuseum(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(30.0f, params_.mainWidth);
    float d = std::max(20.0f, params_.mainDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;
    int32_t stories = std::clamp(params_.stories, 2, 5);
    float flH = std::max(3.8f, params_.storyHeight);
    float mainH = stories * flH;

    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t limestone   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t stuccoMat   = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t champAlum   = static_cast<uint32_t>(MaterialSlot::WallChampagneAluminum);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t corniceMat  = static_cast<uint32_t>(MaterialSlot::CorniceStone);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t glassMat    = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t roofMat     = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t turfMat     = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t planterMat  = static_cast<uint32_t>(MaterialSlot::PlanterBox);

    // 1. Monumental Elevated Travertine Stylobate Terrace & Grand Ceremonial Steps (+Z)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.5f, -0.35f, -hd - 2.0f},
        Vec3{ hw + 2.5f,  0.00f,  hd + 5.0f},
        foundMat);
    float podiumTop = 1.20f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.8f, 0.00f, -hd - 0.8f},
        Vec3{ hw + 0.8f, podiumTop, hd + 2.4f},
        limestone);

    // Ceremonial wide front stairs ascending to podiumTop
    int steps = 6;
    for (int s = 0; s < steps; ++s) {
        float sy = (s + 1) * (podiumTop / static_cast<float>(steps));
        float sz = hd + 4.4f - s * 0.34f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-7.5f, 0.0f, sz - 0.36f},
            Vec3{ 7.5f, sy,   sz},
            limestone);
    }

    // Landscaped Plaza Planter Boxes flanking steps
    for (float px : {-hw + 3.0f, hw - 3.0f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 2.2f, podiumTop, hd + 0.4f},
            Vec3{px + 2.2f, podiumTop + 0.55f, hd + 1.8f},
            planterMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 2.1f, podiumTop + 0.55f, hd + 0.5f},
            Vec3{px + 2.1f, podiumTop + 0.90f, hd + 1.7f},
            turfMat);
    }

    // 2. Central Double-Height Glazed Reading Atrium & Sculpture Hall
    float atriumHW = std::min(7.5f, hw * 0.44f);
    float atriumTop = podiumTop + mainH + 1.8f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-atriumHW, podiumTop, -hd + 1.2f},
        Vec3{ atriumHW, atriumTop,  hd - 0.6f},
        glassTinted);
    // Champagne anodized vertical louver mullions across atrium facade
    for (float mx = -atriumHW + 0.8f; mx <= atriumHW - 0.8f; mx += 1.9f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mx - 0.09f, podiumTop, hd - 0.68f},
            Vec3{mx + 0.09f, atriumTop, hd - 0.42f},
            champAlum);
    }
    // Skylight Monitor Lantern Roof atop Central Reading Hall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-atriumHW - 0.3f, atriumTop, -hd + 1.0f},
        Vec3{ atriumHW + 0.3f, atriumTop + 0.55f, hd - 0.4f},
        blackAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-atriumHW + 1.2f, atriumTop + 0.55f, -hd + 3.0f},
        Vec3{ atriumHW - 1.2f, atriumTop + 1.65f,  hd - 2.5f},
        glassMat);

    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{0.0f, podiumTop, hd - 0.58f},
        3.6f, 3.2f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::RevolvingDoor, true);

    // 3. Flanking Classical-Modernist Exhibition & Stacks Wings (-X and +X)
    for (int side : {-1, 1}) {
        float wx0 = (side < 0) ? -hw : atriumHW;
        float wx1 = (side < 0) ? -atriumHW : hw;
        float wingTop = podiumTop + mainH;

        // Ground Colonnade Gallery with Recessed Shadow Wall & Tall Limestone Columns
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wx0, podiumTop, -hd},
            Vec3{wx1, podiumTop + flH, hd - 2.4f},
            limestone);
        // Sculpted colonnade pillars along front gallery
        int cols = 4;
        float colSpan = (wx1 - wx0 - 1.6f) / static_cast<float>(cols - 1);
        for (int c = 0; c < cols; ++c) {
            float cx = wx0 + 0.8f + c * colSpan;
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{cx, podiumTop, hd - 0.6f},
                Vec3{cx, podiumTop + flH, hd - 0.6f},
                0.38f, 0.34f, 12, corniceMat);
        }

        // Upper Cantilevered Exhibition Archive Cube overhanging Colonnade!
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wx0, podiumTop + flH, -hd - 0.2f},
            Vec3{wx1, wingTop, hd + 0.4f},
            stuccoMat);
        // Horizontal architectural stone cornice crown + recessed museum ribbon windows
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wx0 - 0.25f, wingTop, -hd - 0.35f},
            Vec3{wx1 + 0.25f, wingTop + 0.65f, hd + 0.55f},
            corniceMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wx0 + 1.0f, wingTop - 0.3f, -hd + 1.0f},
            Vec3{wx1 - 1.0f, wingTop + 0.70f, hd - 1.0f},
            roofMat);

        BuildingComponents::generateWindow(outVerts, outIndices,
            Vec3{(wx0 + wx1) * 0.5f, podiumTop + flH + (mainH - flH) * 0.5f, hd + 0.4f},
            (wx1 - wx0) * 0.70f, 1.4f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
            WindowType::RibbonStrip, GlassType::ClearFloat);
    }
}

} // namespace procedural::buildings
