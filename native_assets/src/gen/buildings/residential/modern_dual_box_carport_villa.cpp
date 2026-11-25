#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendBoxVehicle(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float cx, float y0, float cz,
    float len, float wid, float hgt,
    uint32_t bodyMat, uint32_t glassMat, uint32_t tireMat
) {
    float hw = wid * 0.5f;
    float hl = len * 0.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw, y0 + 0.22f, cz - hl},
        Vec3{cx + hw, y0 + hgt * 0.55f, cz + hl}, bodyMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw * 0.85f, y0 + hgt * 0.55f, cz - hl * 0.48f},
        Vec3{cx + hw * 0.85f, y0 + hgt, cz + hl * 0.34f}, glassMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw * 0.78f, y0 + hgt - 0.05f, cz - hl * 0.42f},
        Vec3{cx + hw * 0.78f, y0 + hgt + 0.02f, cz + hl * 0.28f}, bodyMat);
    for (float sx : {-1.0f, 1.0f}) {
        for (float sz : {-hl * 0.58f, hl * 0.58f}) {
            float wx = cx + sx * (hw - 0.04f);
            float wy = y0 + 0.31f;
            float wz = cz + sz;
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{wx - 0.11f, wy, wz}, Vec3{wx + 0.11f, wy, wz},
                0.31f, 0.31f, 12, tireMat, true, true);
        }
    }
}

} // namespace

void HouseGen::generateModernDualBoxCarportVilla(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 11.5f, 14.5f);
    float d = res_scale::depth(params_, 12.0f, 17.0f);
    float flH = res_scale::floorHeight(params_, 3.35f);
    int stories = res_scale::stories(params_, 2, 3);
    float totalH = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t slateGrey  = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t darkBasalt = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t timberWall = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t greenLawn  = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t sidewalk   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t tarmacMat  = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t blueGlass  = static_cast<uint32_t>(MaterialSlot::GlassReflective);
    uint32_t darkGlass  = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t balcGlass  = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t frameMetal = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t amberGlow  = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t tireMat    = static_cast<uint32_t>(MaterialSlot::TireRubber);
    uint32_t whiteCar   = static_cast<uint32_t>(MaterialSlot::VehicleWhite);

    GlassType gt = res_scale::glassType(params_, GlassType::ReflectiveChromeMirror);
    WindowType wt = res_scale::windowType(params_, WindowType::ModernCasement);

    // 1. Yard, Perimeter Boundary Walls, Driveway Apron & Stepping Stones
    float plinthY = 0.14f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.2f, -0.18f, -hd - 1.5f},
        Vec3{ hw + 2.2f,  0.02f,  hd + 5.0f}, tarmacMat);
    // Green front lawn on left entry zone
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.8f, 0.02f, hd - 0.5f},
        Vec3{-0.2f, 0.09f, hd + 4.0f}, greenLawn);
    // Light paved driveway apron on right carport zone
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-0.2f, 0.02f, -hd * 0.4f},
        Vec3{ hw + 1.8f, 0.10f, hd + 4.0f}, sidewalk);

    // Flanking crisp white compound perimeter walls + sconces
    for (float sx : {-1.0f, 1.0f}) {
        float wx0 = (sx < 0) ? (-hw - 1.8f) : (hw + 1.55f);
        float wx1 = (sx < 0) ? (-hw - 1.55f) : (hw + 1.8f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wx0, 0.02f, -hd}, Vec3{wx1, 2.35f, hd + 3.8f}, crispWhite);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wx0 - 0.04f, 1.25f, hd + 2.2f}, Vec3{wx1 + 0.04f, 1.55f, hd + 2.45f}, amberGlow);
    }

    // 4 Floating entry stepping pavers leading to entry door
    for (int st = 0; st < 4; ++st) {
        float pz = hd + 3.1f - st * 0.72f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-1.85f, 0.09f, pz - 0.24f},
            Vec3{-0.15f, 0.16f, pz + 0.24f}, crispWhite);
    }

    // Base structural slab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd}, Vec3{hw, plinthY, hd}, darkBasalt);

    // 2. Main Rear Residential Block (Behind Stairwell, Entry & Carport)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.2f, plinthY, -hd},
        Vec3{ hw - 0.15f, plinthY + totalH, hd - 3.6f}, crispWhite);

    // Rear courtyard lap pool deck (12m x 17m pool feature)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.2f, 0.04f, -hd - 3.2f},
        Vec3{ hw - 1.2f, 0.14f, -hd - 0.2f}, blueGlass);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.6f, 0.02f, -hd - 3.6f},
        Vec3{ hw - 0.6f, 0.10f, -hd - 3.2f}, sidewalk);

    // 3. Left Signature White Box Frame Enclosing 2-Storey Glass Stairwell
    float stairX0 = -hw;
    float stairX1 = -1.95f;
    float wallT = 0.42f;
    // Outer U-box frame (left wall, right pier, top beam, ground sill)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{stairX0, plinthY, hd - 3.6f},
        Vec3{stairX0 + wallT, plinthY + totalH + 0.25f, hd + 0.25f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{stairX1 - wallT, plinthY, hd - 3.6f},
        Vec3{stairX1, plinthY + totalH + 0.25f, hd + 0.25f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{stairX0, plinthY + totalH - 0.35f, hd - 3.6f},
        Vec3{stairX1, plinthY + totalH + 0.25f, hd + 0.25f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{stairX0, plinthY, hd - 3.6f},
        Vec3{stairX1, plinthY + 0.45f, hd + 0.25f}, crispWhite);

    // Deep dark recessed surround & full 2-storey curtain glass inside left frame
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{stairX0 + wallT, plinthY + 0.45f, hd - 0.65f},
        Vec3{stairX1 - wallT, plinthY + totalH - 0.35f, hd - 0.48f}, blueGlass);
    for (int bar = 1; bar <= 3; ++bar) {
        float by = plinthY + 0.45f + bar * ((totalH - 0.80f) / 4.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{stairX0 + wallT, by - 0.04f, hd - 0.48f},
            Vec3{stairX1 - wallT, by + 0.04f, hd - 0.40f}, frameMetal);
    }

    // 4. Center Vertical Fluted Timber / Slate Tower Spine with Pivot Door
    float spineX0 = stairX1;
    float spineX1 = -0.15f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{spineX0, plinthY, hd - 3.6f},
        Vec3{spineX1, plinthY + totalH + 0.10f, hd - 0.15f}, timberWall);
    // Horizontal architectural groove trim lines
    for (float gy = plinthY + 0.55f; gy < plinthY + totalH - 0.2f; gy += 0.55f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{spineX0 + 0.04f, gy, hd - 0.16f},
            Vec3{spineX1 - 0.04f, gy + 0.03f, hd - 0.12f}, darkBasalt);
    }
    // Tall flush front entry door with brushed steel bar handle
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{spineX0 + 0.24f, plinthY, hd - 0.16f},
        Vec3{spineX1 - 0.24f, plinthY + flH * 0.82f, hd - 0.08f}, slateGrey);
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{spineX1 - 0.38f, plinthY + 0.85f, hd - 0.04f},
        Vec3{spineX1 - 0.38f, plinthY + 1.95f, hd - 0.04f},
        0.025f, 0.025f, 8, crispWhite, true, true);

    // 5. Right Ground 2-Car Open Carport with Grooved Pier, Halo Ceiling LED & Vehicles
    float carX0 = spineX1;
    float carX1 = hw;
    float carBackZ = hd - 4.4f;
    // Left dark carport pier with sconce
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{carX0, plinthY, carBackZ},
        Vec3{carX0 + 0.95f, plinthY + flH, hd - 0.05f}, slateGrey);
    // Horizontal grooves on ground pier
    for (int g = 1; g < 5; ++g) {
        float gy = plinthY + g * (flH / 5.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{carX0, gy - 0.025f, hd - 0.06f},
            Vec3{carX0 + 0.95f, gy + 0.025f, hd - 0.02f}, darkBasalt);
    }
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{carX0 + 0.42f, plinthY + 1.45f, hd - 0.03f},
        Vec3{carX0 + 0.56f, plinthY + 1.72f, hd + 0.04f}, amberGlow);

    // Right carport outer pillar & vertical amber light fins inside garage wall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{carX1 - 0.48f, plinthY, carBackZ},
        Vec3{carX1, plinthY + flH, hd - 0.05f}, slateGrey);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{carX0 + 0.95f, plinthY, carBackZ - 0.30f},
        Vec3{carX1 - 0.48f, plinthY + flH, carBackZ}, darkBasalt);
    // Sliding glass garden door visible at back of carport
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{carX0 + 1.6f, plinthY + 0.1f, carBackZ - 0.02f},
        Vec3{carX0 + 4.1f, plinthY + 2.45f, carBackZ + 0.06f}, blueGlass);
    // 4 Vertical amber LED strips on interior right carport wall
    for (int f = 0; f < 4; ++f) {
        float fz = carBackZ + 0.65f + f * 0.92f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{carX1 - 0.52f, plinthY + 0.15f, fz},
            Vec3{carX1 - 0.46f, plinthY + flH - 0.15f, fz + 0.06f}, amberGlow);
    }
    // Recessed rectangular LED halo strip on carport ceiling
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{carX0 + 1.45f, plinthY + flH - 0.08f, carBackZ + 0.6f},
        Vec3{carX1 - 0.95f, plinthY + flH - 0.02f, hd - 0.7f}, amberGlow);

    if (params_.hasGarage) {
        float cw = carX1 - carX0 - 1.4f;
        appendBoxVehicle(outVerts, outIndices,
            carX0 + 1.0f + cw * 0.28f, plinthY, hd - 2.0f,
            4.25f, 1.85f, 1.58f, whiteCar, darkGlass, tireMat);
        appendBoxVehicle(outVerts, outIndices,
            carX0 + 1.0f + cw * 0.74f, plinthY, hd - 2.2f,
            4.45f, 1.92f, 1.72f, darkBasalt, darkGlass, tireMat);
    }

    // 6. Cantilevered Upper-Right Overlapping C-Frame Balcony Box
    float uY0 = plinthY + flH;
    float uY1 = plinthY + totalH;
    float boxX0 = spineX0 - 0.15f;
    float boxX1 = hw + 0.18f;
    float boxFrontZ = hd + 0.45f;
    // Bottom cantilevered balcony slab, top brow beam, right vertical cheek, left recessed wall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{boxX0, uY0 - 0.12f, carBackZ},
        Vec3{boxX1, uY0 + 0.28f, boxFrontZ}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{boxX0, uY1 - 0.38f, carBackZ},
        Vec3{boxX1, uY1 + 0.28f, boxFrontZ}, slateGrey);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{boxX1 - 0.45f, uY0, carBackZ},
        Vec3{boxX1, uY1 + 0.28f, boxFrontZ}, slateGrey);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{boxX0, uY0 + 0.28f, carBackZ},
        Vec3{boxX0 + 1.75f, uY1 - 0.38f, hd - 0.55f}, slateGrey);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{boxX0 + 0.85f, uY0 + 1.45f, hd - 0.56f},
        Vec3{boxX0 + 0.99f, uY0 + 1.72f, hd - 0.49f}, amberGlow);

    // Glass balcony railing & panoramic sliding balcony doors
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{boxX0 + 1.75f, uY0 + 0.28f, boxFrontZ - 0.14f},
            Vec3{boxX1 - 0.45f, uY0 + 1.24f, boxFrontZ - 0.06f}, balcGlass);
    }
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{boxX0 + 1.85f, uY0 + 0.28f, carBackZ + 0.65f},
        Vec3{boxX1 - 0.65f, uY1 - 0.42f, carBackZ + 0.80f}, darkGlass);
    // Overhead soffit LED ring strip
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{boxX0 + 2.0f, uY1 - 0.42f, carBackZ + 1.2f},
        Vec3{boxX1 - 0.9f, uY1 - 0.36f, boxFrontZ - 0.5f}, amberGlow);

    // 7. Rear & Side Windows + Flat Roof Slab
    for (int s = 0; s < stories; ++s) {
        float fy = plinthY + s * flH;
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd}, w - 1.2f, fy, flH,
            Vec3{0.0f, 0.0f, -1.0f}, Vec3{-1.0f, 0.0f, 0.0f}, 3, wt, gt);
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{-hw + 0.2f, 0.0f, -1.2f}, d * 0.52f, fy, flH,
            Vec3{-1.0f, 0.0f, 0.0f}, Vec3{0.0f, 0.0f, -1.0f}, 2, wt, gt);
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{hw - 0.15f, 0.0f, -1.2f}, d * 0.52f, fy, flH,
            Vec3{1.0f, 0.0f, 0.0f}, Vec3{0.0f, 0.0f, 1.0f}, 2, wt, gt);
    }

    res_scale::appendAdaptiveRoof(outVerts, outIndices, params_,
        w - 0.2f, d - 3.4f, plinthY + totalH, RoofType::FlatParapet, 0.48f, 0.0f);
}

} // namespace procedural::buildings
