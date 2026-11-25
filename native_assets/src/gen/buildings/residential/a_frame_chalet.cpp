#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void HouseGen::generateAFrameChalet(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 7.0f, 12.0f);
    float d = res_scale::depth(params_, 8.0f, 14.0f);
    float flH = res_scale::floorHeight(params_, 3.2f);
    int stories = res_scale::stories(params_, 1, 3);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t stoneMat    = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t timberWall  = res_scale::wallMat(params_, MaterialSlot::WallTimber);
    uint32_t roofMetal   = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    WindowType wt        = res_scale::windowType(params_, WindowType::PictureGlass);
    GlassType gt         = res_scale::glassType(params_, GlassType::ClearFloat);
    uint32_t glassMat    = BuildingComponents::getGlassMaterial(gt);
    bool hasDeck         = res_scale::showBalconies(params_);

    // 1. Mountain Stone Podium Base (raises chalet 0.85m above snow grade)
    float deckY = 0.85f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.5f, -0.35f, -hd - 0.5f},
        Vec3{hw + 0.5f, deckY, hd + (hasDeck ? 2.8f : 0.5f)},
        stoneMat);

    // Timber ski deck & railings extending out front
    if (hasDeck) {
        float deckExt = std::clamp(res_scale::balconyDepth(params_, 2.4f), 1.4f, 3.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.3f, deckY, hd},
            Vec3{hw + 0.3f, deckY + 0.14f, hd + deckExt},
            timberWall);
        BuildingComponents::generateBalcony(
            outVerts, outIndices,
            Vec3{0.0f, deckY, hd},
            w * 0.92f, deckExt,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            BalconyType::WroughtIron);
    }

    // 2. Iconic Steep Triangular Prism Roof Shell (Peak height scales smoothly with stories & floor height)
    float apexH = deckY + stories * flH + res_scale::roofHeight(params_, 2.8f);
    float eaveDropY = deckY + 0.10f;
    float ov = res_scale::roofOverhang(params_, 0.65f);
    float roofSpanHalfW = hw + ov * 0.5f;
    float roofZ0 = -hd - ov;
    float roofZ1 = hd + ov;

    // Left sloped roof plane
    Vec3 lNormal{-apexH, roofSpanHalfW, 0.0f};
    float lLen = std::sqrt(lNormal.x * lNormal.x + lNormal.y * lNormal.y);
    lNormal = {lNormal.x / lLen, lNormal.y / lLen, 0.0f};
    BuildingComponents::appendQuad(
        outVerts, outIndices,
        Vec3{-roofSpanHalfW, eaveDropY, roofZ1},
        Vec3{-roofSpanHalfW, eaveDropY, roofZ0},
        Vec3{0.0f, apexH, roofZ0},
        Vec3{0.0f, apexH, roofZ1},
        lNormal, roofMetal);
    // Inner soffit left
    BuildingComponents::appendQuad(
        outVerts, outIndices,
        Vec3{0.0f, apexH - 0.18f, roofZ1},
        Vec3{0.0f, apexH - 0.18f, roofZ0},
        Vec3{-roofSpanHalfW + 0.16f, eaveDropY, roofZ0},
        Vec3{-roofSpanHalfW + 0.16f, eaveDropY, roofZ1},
        Vec3{0, -1, 0}, timberWall);

    // Right sloped roof plane
    Vec3 rNormal{apexH, roofSpanHalfW, 0.0f};
    float rLen = std::sqrt(rNormal.x * rNormal.x + rNormal.y * rNormal.y);
    rNormal = {rNormal.x / rLen, rNormal.y / rLen, 0.0f};
    BuildingComponents::appendQuad(
        outVerts, outIndices,
        Vec3{roofSpanHalfW, eaveDropY, roofZ0},
        Vec3{roofSpanHalfW, eaveDropY, roofZ1},
        Vec3{0.0f, apexH, roofZ1},
        Vec3{0.0f, apexH, roofZ0},
        rNormal, roofMetal);
    // Inner soffit right
    BuildingComponents::appendQuad(
        outVerts, outIndices,
        Vec3{roofSpanHalfW - 0.16f, eaveDropY, roofZ1},
        Vec3{roofSpanHalfW - 0.16f, eaveDropY, roofZ0},
        Vec3{0.0f, apexH - 0.18f, roofZ0},
        Vec3{0.0f, apexH - 0.18f, roofZ1},
        Vec3{0, -1, 0}, timberWall);

    // Ridge Cap Beam
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-0.16f, apexH - 0.10f, roofZ0},
        Vec3{0.16f, apexH + 0.18f, roofZ1},
        roofMetal);

    // 3. Front & Rear Gable Timber End Walls (Stepped triangular timber fill inside the A-frame)
    int strips = 10;
    for (int i = 0; i < strips; ++i) {
        float t0 = static_cast<float>(i) / strips;
        float t1 = static_cast<float>(i + 1) / strips;
        float y0 = deckY + t0 * (apexH - deckY - 0.1f);
        float y1 = deckY + t1 * (apexH - deckY - 0.1f);
        float xSpan = hw * (1.0f - (t0 + t1) * 0.5f);
        if (xSpan < 0.25f) continue;
        // Rear gable solid wall
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-xSpan, y0, -hd},
            Vec3{xSpan, y1, -hd + 0.35f},
            timberWall);
        // Front gable end wall framing (timber borders + recessed glass prow)
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-xSpan, y0, hd - 0.35f},
            Vec3{xSpan, y1, hd},
            (i >= 1 && i <= 7) ? glassMat : timberWall);
    }

    // Glazed Prow Structural Mullion Grid on Front Facade
    for (float mx : {-hw * 0.36f, 0.0f, hw * 0.36f}) {
        float colTopY = deckY + (apexH - deckY) * (1.0f - std::abs(mx) / hw) - 0.15f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mx - 0.07f, deckY, hd - 0.02f},
            Vec3{mx + 0.07f, colTopY, hd + 0.08f},
            timberWall);
    }

    // Floor Slab Edges visible behind front glass prow
    for (int s = 1; s < stories; ++s) {
        float fy = deckY + s * flH;
        float frac = (fy - deckY) / std::max(1.0f, apexH - deckY);
        float fw = hw * std::max(0.15f, 1.0f - frac);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-fw, fy - 0.08f, hd - 0.12f},
            Vec3{fw, fy + 0.10f, hd + 0.12f},
            timberWall);
    }

    // 4. Front Entry Door (Offset cleanly inside the A-frame base) + Rear Scaled Windows
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{-hw * 0.24f, deckY + 0.14f, hd + 0.04f}, flH, w * 0.45f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::SingleModern, 1.15f, 2.2f, true);

    res_scale::appendScaledWindowRow(
        outVerts, outIndices, params_,
        Vec3{0.0f, 0.0f, -hd - 0.02f}, w * 0.65f, deckY, flH,
        Vec3{0, 0, -1}, Vec3{-1, 0, 0},
        2, wt, gt, 1.35f, 1.55f, 0.75f,
        0.0f, 0.0f, false, false);

    // 5. Side Hearth Stone Fireplace Chimney piercing slope (+X side)
    float chimH = apexH + 0.85f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw * 0.45f, -0.35f, -1.1f},
        Vec3{hw * 0.45f + 1.15f, chimH, 0.1f},
        foundMat);
}

} // namespace procedural::buildings
