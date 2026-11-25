#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void HouseGen::generateMinimalistRibbonBungalow(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 11.0f, 13.8f);
    float d = res_scale::depth(params_, 9.5f, 12.2f);
    float flH = res_scale::floorHeight(params_, 3.45f);
    int stories = res_scale::stories(params_, 1, 2);
    float totalH = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t midGrey    = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t cedarWood  = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t darkFascia = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t blackFrame = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t darkGlass  = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t gravelPave = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t greenLawn  = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t amberLight = static_cast<uint32_t>(MaterialSlot::GlassGold);

    GlassType gt = res_scale::glassType(params_, GlassType::SolarTintedSmoke);
    WindowType wt = res_scale::windowType(params_, WindowType::ModernCasement);

    // 1. White Gravel Courtyard, Green Lawn Border & Round Stepping Pavers + Patio Table
    float plinthY = 0.24f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 3.6f, -0.15f, -hd - 1.5f},
        Vec3{ hw + 2.8f,  0.02f,  hd + 4.8f}, greenLawn);
    // White gravel surround path
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.2f, 0.02f, -hd - 0.8f},
        Vec3{ hw + 2.2f, 0.08f,  hd + 3.8f}, gravelPave);
    // Front green turf accent islands & stepping stones
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.2f, 0.08f, hd + 1.8f},
        Vec3{-0.4f, 0.13f, hd + 4.6f}, greenLawn);
    for (int st = 0; st < 4; ++st) {
        float px = -1.6f + (st % 2) * 0.95f;
        float pz = hd + 4.0f - st * 0.65f;
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{px, 0.08f, pz}, Vec3{px, 0.16f, pz},
            0.34f, 0.34f, 12, crispWhite, true, true);
    }

    // Floating crisp white entry steps leading up to covered porch
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-2.25f, 0.08f, hd + 0.15f},
        Vec3{ 0.15f, 0.18f, hd + 1.15f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-2.25f, 0.18f, hd - 0.55f},
        Vec3{ 0.15f, 0.28f, hd + 0.65f}, crispWhite);

    // Side outdoor dining terrace (far left patio)
    float tx = -hw - 2.4f, tz = hd - 1.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{tx - 1.3f, 0.05f, tz - 1.0f},
        Vec3{tx + 1.3f, 0.12f, tz + 1.0f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{tx - 0.85f, 0.72f, tz - 0.45f},
        Vec3{tx + 0.85f, 0.79f, tz + 0.45f}, cedarWood);
    for (float lx : {-0.75f, 0.75f}) {
        for (float lz : {-0.38f, 0.38f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{tx + lx - 0.03f, 0.12f, tz + lz - 0.03f},
                Vec3{tx + lx + 0.03f, 0.72f, tz + lz + 0.03f}, blackFrame);
        }
    }

    // Perimeter low garden wall along right & rear
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw + 2.2f, 0.02f, -hd - 1.2f},
        Vec3{hw + 2.45f, 1.55f, hd + 3.2f}, midGrey);
    for (float rz = -hd; rz <= hd + 2.8f; rz += 0.35f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw + 2.24f, 1.55f, rz - 0.02f},
            Vec3{hw + 2.38f, 2.05f, rz + 0.02f}, blackFrame);
    }

    // 2. Main Rear Structural Floorplan Volume
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.25f, 0.0f, -hd}, Vec3{hw, plinthY, hd}, midGrey);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.35f, plinthY, -hd},
        Vec3{-0.25f, plinthY + totalH, hd - 1.85f}, midGrey);

    // 3. Left Wing: Grey Cantilevered Porch Roof, 3 Slim Steel Columns & Arched-Transom Glass Porch Door
    float porchTopY = plinthY + flH * 0.88f;
    // Grey porch slab roof + under-soffit linear warm LED
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.85f, porchTopY, hd - 2.0f},
        Vec3{-0.10f, porchTopY + 0.34f, hd + 0.85f}, midGrey);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.5f, porchTopY - 0.04f, hd - 0.6f},
        Vec3{-0.6f, porchTopY, hd - 0.48f}, amberLight);

    // Signature 3 slim charcoal steel tubular columns on far left front porch edge
    for (int col = 0; col < 3; ++col) {
        float cx = -hw - 0.42f + col * 0.22f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx - 0.04f, 0.08f, hd + 0.45f},
            Vec3{cx + 0.04f, porchTopY + 0.10f, hd + 0.53f}, blackFrame);
    }
    // Low grey porch wing privacy wall & rear porch pillar
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.25f, plinthY, hd - 1.85f},
        Vec3{-hw + 0.55f, porchTopY, hd - 0.55f}, midGrey);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.25f, plinthY, hd + 0.42f},
        Vec3{-2.45f, plinthY + 0.92f, hd + 0.65f}, midGrey);

    // Glazed French glass entry door with black frame at recessed porch rear
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-2.25f, plinthY, hd - 1.88f},
        Vec3{-0.65f, plinthY + 2.55f, hd - 1.76f}, blackFrame);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-2.12f, plinthY + 0.08f, hd - 1.84f},
        Vec3{-0.78f, plinthY + 2.42f, hd - 1.70f}, darkGlass);

    // 4. Right Wing: Signature White Inverted-U Ribbon Portal Wrapping Cedar Wall & Corner Glass Box
    float ribX0 = -0.25f;
    float ribX1 = hw + 0.55f;
    float ribTopY = plinthY + totalH + 0.58f;
    // Left vertical ribbon leg + bottom extended plinth deck + top projecting cornice beam
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{ribX0, plinthY, hd - 2.4f},
        Vec3{ribX0 + 0.72f, ribTopY, hd + 0.35f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{ribX0, 0.08f, -hd + 0.5f},
        Vec3{ribX1 + 0.45f, plinthY + 0.14f, hd + 0.45f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{ribX0, ribTopY - 0.46f, -hd},
        Vec3{ribX1 + 0.55f, ribTopY, hd + 0.52f}, crispWhite);
    // Secondary inner horizontal white eyebrow shelf over window wall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{ribX0 + 0.72f, plinthY + flH - 0.15f, hd - 1.2f},
        Vec3{ribX1 + 0.05f, plinthY + flH + 0.12f, hd + 0.25f}, crispWhite);
    // Dark charcoal slatted transom strip between upper white eyebrows
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{ribX0 + 0.72f, plinthY + flH + 0.12f, hd - 0.8f},
        Vec3{ribX1 - 0.15f, ribTopY - 0.46f, hd - 0.15f}, darkFascia);

    // Warm Horizontal Cedar Cladding Panel & Glowing Wall Sconce inside Ribbon Portal
    float cedarEndX = ribX0 + 2.45f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{ribX0 + 0.72f, plinthY + 0.14f, -hd},
        Vec3{cedarEndX, plinthY + flH - 0.15f, hd - 0.08f}, cedarWood);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{(ribX0 + 0.72f + cedarEndX) * 0.5f - 0.08f, plinthY + 1.55f, hd - 0.09f},
        Vec3{(ribX0 + 0.72f + cedarEndX) * 0.5f + 0.08f, plinthY + 1.78f, hd - 0.02f}, amberLight);

    // Floor-to-ceiling Black-Framed Corner-Wrap Living Room Glass Box
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cedarEndX, plinthY + 0.14f, -hd + 0.4f},
        Vec3{hw - 0.05f, plinthY + flH - 0.15f, hd - 0.10f}, darkGlass);
    for (int mx = 0; mx <= 3; ++mx) {
        float xL = cedarEndX + mx * ((hw - 0.05f - cedarEndX) / 3.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{xL - 0.04f, plinthY + 0.14f, hd - 0.12f},
            Vec3{xL + 0.04f, plinthY + flH - 0.15f, hd - 0.02f}, blackFrame);
    }
    for (int my = 1; my <= 2; ++my) {
        float yL = plinthY + 0.14f + my * ((flH - 0.29f) / 3.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cedarEndX, yL - 0.04f, hd - 0.12f},
            Vec3{hw - 0.05f, yL + 0.04f, hd - 0.02f}, blackFrame);
    }
    // Right glass return mullions
    for (int mz = 1; mz <= 2; ++mz) {
        float zL = hd - 0.10f - mz * 1.4f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw - 0.08f, plinthY + 0.14f, zL - 0.04f},
            Vec3{hw + 0.02f, plinthY + flH - 0.15f, zL + 0.04f}, blackFrame);
    }

    // 5. Split Dark Charcoal Fascia Flat Roof Planes
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.35f, porchTopY + 0.65f, -hd - 0.3f},
        Vec3{ribX0 + 0.15f, porchTopY + 0.98f, hd + 0.15f}, darkFascia);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{ribX0 - 0.35f, ribTopY, -hd - 0.3f},
        Vec3{ribX1 + 0.85f, ribTopY + 0.38f, hd + 0.72f}, darkFascia);

    // 6. Rear & Left Side Windows
    res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
        Vec3{0.0f, 0.0f, -hd}, w - 1.2f, plinthY, flH,
        Vec3{0.0f, 0.0f, -1.0f}, Vec3{-1.0f, 0.0f, 0.0f}, 3, wt, gt);
    res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
        Vec3{-hw + 0.35f, 0.0f, -1.2f}, d * 0.55f, plinthY, flH,
        Vec3{-1.0f, 0.0f, 0.0f}, Vec3{0.0f, 0.0f, -1.0f}, 2, wt, gt);
}

} // namespace procedural::buildings
