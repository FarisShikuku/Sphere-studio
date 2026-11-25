#include "WarehouseGen.h"
#include "../BuildingComponents.h"
#include "../interiors/RoomGen.h"
#include <cmath>
#include <algorithm>

namespace procedural::buildings {

WarehouseGen::WarehouseGen() = default;

bool WarehouseGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void WarehouseGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload WarehouseGen::generateMesh(const GeneratorContext& /*ctx*/) {
    std::vector<BuildingVertex> verts;
    std::vector<uint32_t> indices;

    switch (params_.subType) {
        case IndustrialType::Warehouse:
            generateWarehouse(verts, indices);
            break;
        case IndustrialType::Factory:
            generateFactory(verts, indices);
            break;
        case IndustrialType::Silo:
            generateSilo(verts, indices);
            break;
        case IndustrialType::PowerPlant:
            generatePowerPlant(verts, indices);
            break;
        case IndustrialType::Refinery:
            generateRefinery(verts, indices);
            break;
        case IndustrialType::ContainerTerminal:
            generateContainerTerminal(verts, indices);
            break;
        case IndustrialType::SteelMill:
            generateSteelMill(verts, indices);
            break;
        case IndustrialType::CleanroomFab:
            generateCleanroomFab(verts, indices);
            break;
        case IndustrialType::PowerLines:
            generatePowerLines(verts, indices);
            break;
        case IndustrialType::WaterTreatment:
            generateWaterTreatment(verts, indices);
            break;
        case IndustrialType::DataCenter:
            generateDataCenter(verts, indices);
            break;
        case IndustrialType::AerospaceHangar:
            generateAerospaceHangar(verts, indices);
            break;
        case IndustrialType::WasteIncinerator:
            generateWasteIncinerator(verts, indices);
            break;
        case IndustrialType::BiopharmPlant:
            generateBiopharmPlant(verts, indices);
            break;
    }

    GeneratedMeshPayload payload;
    payload.vertexStride = 9 * sizeof(float);
    payload.vertexCount = static_cast<uint32_t>(verts.size());
    payload.indexCount = static_cast<uint32_t>(indices.size());
    payload.indexBuffer = indices;

    payload.vertexBuffer.reserve(verts.size() * 9);
    for (const auto& v : verts) {
        payload.vertexBuffer.push_back(v.position.x);
        payload.vertexBuffer.push_back(v.position.y);
        payload.vertexBuffer.push_back(v.position.z);
        payload.vertexBuffer.push_back(v.normal.x);
        payload.vertexBuffer.push_back(v.normal.y);
        payload.vertexBuffer.push_back(v.normal.z);
        payload.vertexBuffer.push_back(v.uv.x);
        payload.vertexBuffer.push_back(v.uv.y);
        payload.vertexBuffer.push_back(static_cast<float>(v.materialId));
    }

    return payload;
}

void WarehouseGen::generateWarehouse(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::clamp(params_.length > 0.5f ? params_.length : 32.0f, 12.0f, 70.0f);
    float d = std::clamp(params_.width > 0.5f ? params_.width : 20.0f, 10.0f, 50.0f);
    float h = std::clamp(params_.height > 0.5f ? params_.height : 8.5f, 4.8f, 20.0f);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallMat = params_.useExtendedParams
        ? BuildingComponents::getWallMaterial(params_.wallMaterial)
        : static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    GlassType gt = params_.useExtendedParams ? params_.glassType : GlassType::ClearFloat;
    uint32_t glassMat = BuildingComponents::getGlassMaterial(gt);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t hvacMat = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t accentMat = static_cast<uint32_t>(MaterialSlot::WallSpandrelGlass);
    uint32_t darkMetalMat = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t silverMetalMat = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);

    // 1. Heavy Asphalt Truck Maneuvering Yard & Staging Apron
    float yardMinX = -hw - 4.0f;
    float yardMaxX = hw + 4.0f;
    float yardMinZ = params_.showInside ? (-hd - 8.0f) : (-hd - 2.0f);
    float yardMaxZ = hd + 7.0f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{yardMinX, -0.4f, yardMinZ}, Vec3{yardMaxX, 0.0f, yardMaxZ}, foundMat);

    // 2. Concrete Base Foundation Plinth around Warehouse Envelope
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 0.2f, 0.0f, -hd - 0.2f}, Vec3{hw + 0.2f, 1.0f, hd + 0.2f}, foundMat);

    if (params_.showInside) {
        // Interior Epoxy Logistics Deck Slab at y = 1.0m
        uint32_t epoxyMat = static_cast<uint32_t>(MaterialSlot::ShopFloorEpoxy);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw + 0.22f, 0.98f, -hd + 0.22f}, Vec3{hw - 0.22f, 1.02f, hd - 0.22f}, epoxyMat);

        // Rear (-Z) Wall pierced cleanly for interior loading dock bays + Left (-X) Wall with clerestory windows
        int reqBays = std::clamp(params_.loadingBays, 2, 6);
        float inDockStartX = -hw + 2.2f;
        float inDockEndX = hw - 2.2f;
        float inBayW = (inDockEndX - inDockStartX) / static_cast<float>(reqBays);
        float inDoorW = std::clamp(inBayW - 1.1f, 2.4f, 3.4f);
        float inDoorH = std::min(3.8f, h - 2.1f);

        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 1.0f, -hd}, Vec3{inDockStartX, h, -hd + 0.25f}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{inDockEndX, 1.0f, -hd}, Vec3{hw, h, -hd + 0.25f}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{inDockStartX, 1.0f + inDoorH, -hd}, Vec3{inDockEndX, h, -hd + 0.25f}, wallMat);
        for (int b = 0; b <= reqBays; ++b) {
            float px = inDockStartX + b * inBayW;
            float x0 = std::max(inDockStartX, px - (inBayW - inDoorW) * 0.5f);
            float x1 = std::min(inDockEndX, px + (inBayW - inDoorW) * 0.5f);
            if (x1 > x0 + 0.02f) {
                BuildingComponents::appendBox(outVerts, outIndices, Vec3{x0, 1.0f, -hd}, Vec3{x1, 1.0f + inDoorH, -hd + 0.25f}, wallMat);
            }
        }

        // Full Left (-X) Wall & rear corporate band
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 1.0f, -hd}, Vec3{-hw + 0.25f, h, hd}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 0.05f, h - 0.65f, -hd - 0.05f}, Vec3{-hw + 0.28f, h - 0.30f, hd + 0.05f}, accentMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 0.05f, h - 0.65f, -hd - 0.05f}, Vec3{hw + 0.05f, h - 0.30f, -hd + 0.28f}, accentMat);

        // Clerestory glazing on Left (-X) & Rear (-Z) walls
        float ribbonH = std::min(1.2f, std::max(0.6f, params_.useExtendedParams ? params_.windowHeight : 1.0f));
        float ribbonY = std::max(2.2f, h - ribbonH - 0.85f);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 0.06f, ribbonY, -hd * 0.85f}, Vec3{-hw + 0.28f, ribbonY + ribbonH, hd * 0.85f}, glassMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw * 0.85f, ribbonY, -hd - 0.06f}, Vec3{hw * 0.85f, ribbonY + ribbonH, -hd + 0.28f}, glassMat);

        // Low Cutaway Sill Walls on Front (+Z) & Right (+X) + Perimeter Steel Portal Columns & Top Tie-Beam
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 1.0f, hd - 0.25f}, Vec3{hw, 1.48f, hd}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{hw - 0.25f, 1.0f, -hd}, Vec3{hw, 1.48f, hd}, wallMat);
        for (float pz : {-hd + 0.25f, 0.0f, hd - 0.25f}) {
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{hw - 0.28f, 1.0f, pz - 0.14f}, Vec3{hw, h, pz + 0.14f}, frameMat);
        }
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, h - 0.22f, hd - 0.25f}, Vec3{hw, h, hd}, frameMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{hw - 0.25f, h - 0.22f, -hd}, Vec3{hw, h, hd}, frameMat);

        // Open Cutaway Attached Logistics Office at Front-Left (-X, +Z) with Interior Desk
        float officeW = std::max(5.0f, std::min(11.0f, hw * 0.65f));
        float officeD = 3.2f;
        float offX0 = -hw;
        float offX1 = -hw + officeW;
        float offZ0 = hd;
        float offZ1 = hd + officeD;
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{offX0, 0.0f, offZ0}, Vec3{offX1, 0.22f, offZ1}, foundMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{offX0, 0.22f, offZ0}, Vec3{offX0 + 0.22f, 2.8f, offZ1}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{offX0, 0.22f, offZ1 - 0.22f}, Vec3{offX1, 1.05f, offZ1}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{offX1 - 0.22f, 0.22f, offZ0}, Vec3{offX1, 1.05f, offZ1}, wallMat);
        // Dispatch reception desk & chairs inside cutaway office
        uint32_t deskMat = static_cast<uint32_t>(MaterialSlot::DoorWoodMetal);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{offX0 + 1.0f, 0.22f, offZ0 + 0.9f}, Vec3{offX1 - 1.2f, 0.98f, offZ0 + 1.75f}, deskMat);

        // Populate rich selectable Warehouse Interior & Loading Docks inside warehouse slab!
        RoomGen::appendWarehouseInteriorContent(
            outVerts, outIndices,
            0.0f, 1.02f, 0.0f,
            w - 0.6f, d - 0.6f, h - 1.05f,
            params_.interiorDesign, reqBays
        );
        return;
    }

    // 3. Main Warehouse Logistics Hall
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 1.0f, -hd}, Vec3{hw, h, hd}, wallMat);

    // Corporate Runner Band
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 0.05f, h - 0.65f, -hd - 0.05f}, Vec3{hw + 0.05f, h - 0.30f, hd + 0.05f}, accentMat);

    // Structural Vertical Pilaster Reveals along Side Facades
    float sideXs[2] = {-hw, hw};
    for (float sideX : sideXs) {
        int numPilasters = 5;
        for (int p = 0; p <= numPilasters; ++p) {
            float pz = -hd + p * ((hd * 2.0f) / numPilasters);
            float pw = 0.35f;
            float pd = 0.12f;
            float sign = (sideX > 0.0f) ? 1.0f : -1.0f;
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{sideX - pw * 0.5f, 0.0f, pz - pd}, Vec3{sideX + pw * 0.5f + sign * 0.15f, h, pz + pd}, foundMat);
        }
    }

    // 4. Continuous High-Level Daylighting Clerestory Windows (Side & Rear)
    float ribbonH = std::min(1.2f, std::max(0.6f, params_.useExtendedParams ? params_.windowHeight : 1.0f));
    float ribbonY = std::max(2.2f, h - ribbonH - 0.85f);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 0.06f, ribbonY, -hd * 0.85f}, Vec3{-hw, ribbonY + ribbonH, hd * 0.85f}, glassMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{hw, ribbonY, -hd * 0.85f}, Vec3{hw + 0.06f, ribbonY + ribbonH, hd * 0.85f}, glassMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw * 0.85f, ribbonY, -hd - 0.06f}, Vec3{hw * 0.85f, ribbonY + ribbonH, -hd}, glassMat);

    // 5. Attached Modern Logistics Office / Dispatch Mezzanine Wing (Front-Left Corner)
    float officeW = std::max(5.0f, std::min(12.0f, hw * 0.70f));
    float officeD = 3.4f;
    float officeH = std::clamp(h * 0.60f, 3.8f, 6.0f);
    float offX0 = -hw;
    float offX1 = -hw + officeW;
    float offZ0 = hd;
    float offZ1 = hd + officeD;

    BuildingComponents::appendBox(outVerts, outIndices, Vec3{offX0, 0.0f, offZ0}, Vec3{offX1, officeH, offZ1}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{offX0 - 0.1f, officeH - 0.45f, offZ0}, Vec3{offX1 + 0.1f, officeH + 0.14f, offZ1 + 0.1f}, darkMetalMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{offX0 + 0.5f, officeH - 0.36f, offZ1 + 0.11f}, Vec3{offX1 - 0.5f, officeH - 0.08f, offZ1 + 0.14f}, accentMat);

    float offGlassX0 = offX0 + 0.6f;
    float offGlassX1 = offX1 - 0.6f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{offGlassX0, 0.4f, offZ1 - 0.02f}, Vec3{offGlassX1, officeH - 0.7f, offZ1 + 0.04f}, glassMat);
    int offMullions = 4;
    for (int m = 0; m <= offMullions; ++m) {
        float mx = offGlassX0 + m * ((offGlassX1 - offGlassX0) / offMullions);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{mx - 0.05f, 0.4f, offZ1 - 0.03f}, Vec3{mx + 0.05f, officeH - 0.7f, offZ1 + 0.08f}, silverMetalMat);
    }

    // Office commercial entrance portal clamped safely
    float offDoorX = offX1 - std::min(2.0f, officeW * 0.25f);
    DoorType dt = params_.useExtendedParams ? params_.doorType : DoorType::DoubleGlassCommercial;
    if (dt != DoorType::None) {
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{offDoorX, 0.0f, offZ1 + 0.05f},
            std::clamp(params_.useExtendedParams ? params_.doorWidth : 1.6f, 1.0f, officeW * 0.4f),
            std::clamp(params_.useExtendedParams ? params_.doorHeight : 2.35f, 2.0f, officeH - 0.4f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            dt, params_.useExtendedParams ? params_.hasEntranceCanopy : true);
    }
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{offX0 - 0.4f, 0.0f, offZ1}, Vec3{offX1 + 0.4f, 0.18f, offZ1 + 1.6f}, foundMat);

    // 6. Semi-Truck Loading Dock Bays
    float dockStartX = offX1 + 1.0f;
    float dockEndX = hw - 0.8f;
    float dockAvailableW = std::max(4.0f, dockEndX - dockStartX);
    float bayUnitW = 4.0f;
    int maxBaysFit = std::max(1, static_cast<int>(dockAvailableW / bayUnitW));
    int reqBays = std::clamp(params_.loadingBays, 1, 6);
    int numDocks = std::min(reqBays, maxBaysFit);
    float bayActualW = dockAvailableW / numDocks;
    float dockDoorW = std::min(3.2f, bayActualW - 0.6f);
    float dockDoorH = std::min(4.0f, h - 1.4f);

    for (int b = 0; b < numDocks; ++b) {
        float bayCenterX = dockStartX + b * bayActualW + bayActualW * 0.5f;
        float doorX0 = bayCenterX - dockDoorW * 0.5f;
        float doorX1 = bayCenterX + dockDoorW * 0.5f;

        float laneW = dockDoorW + 0.6f;
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{bayCenterX - laneW * 0.5f - 0.05f, 0.01f, hd + 0.8f}, Vec3{bayCenterX - laneW * 0.5f + 0.05f, 0.03f, yardMaxZ - 0.5f}, silverMetalMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{bayCenterX + laneW * 0.5f - 0.05f, 0.01f, hd + 0.8f}, Vec3{bayCenterX + laneW * 0.5f + 0.05f, 0.03f, yardMaxZ - 0.5f}, silverMetalMat);

        BuildingComponents::appendBox(outVerts, outIndices, Vec3{doorX0 - 0.40f, 0.0f, hd + 0.5f}, Vec3{doorX0 - 0.30f, 0.25f, hd + 3.8f}, frameMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{doorX1 + 0.30f, 0.0f, hd + 0.5f}, Vec3{doorX1 + 0.40f, 0.25f, hd + 3.8f}, frameMat);

        BuildingComponents::appendBox(outVerts, outIndices, Vec3{doorX0 + 0.15f, 0.0f, hd - 0.3f}, Vec3{doorX1 - 0.15f, 0.15f, hd + 0.5f}, darkMetalMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{doorX0 + 0.30f, 0.14f, hd - 0.1f}, Vec3{doorX1 - 0.30f, 0.18f, hd + 0.55f}, accentMat);

        float sealD = 0.48f;
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{doorX0 - 0.38f, 0.2f, hd}, Vec3{doorX0, dockDoorH + 0.45f, hd + sealD}, darkMetalMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{doorX1, 0.2f, hd}, Vec3{doorX1 + 0.38f, dockDoorH + 0.45f, hd + sealD}, darkMetalMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{doorX0 - 0.38f, dockDoorH, hd}, Vec3{doorX1 + 0.38f, dockDoorH + 0.50f, hd + sealD}, darkMetalMat);

        BuildingComponents::appendBox(outVerts, outIndices, Vec3{doorX0, 0.18f, hd - 0.04f}, Vec3{doorX1, dockDoorH, hd + 0.04f}, silverMetalMat);
        float vLiteY = dockDoorH * 0.45f;
        for (int vl = -1; vl <= 1; ++vl) {
            float vx = bayCenterX + vl * (dockDoorW * 0.24f);
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{vx - 0.22f, vLiteY - 0.14f, hd + 0.05f}, Vec3{vx + 0.22f, vLiteY + 0.14f, hd + 0.08f}, glassMat);
        }

        BuildingComponents::appendBox(outVerts, outIndices, Vec3{doorX0 - 0.34f, 0.2f, hd + 0.45f}, Vec3{doorX0 - 0.12f, 1.3f, hd + 0.72f}, frameMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{doorX1 + 0.12f, 0.2f, hd + 0.45f}, Vec3{doorX1 + 0.34f, 1.3f, hd + 0.72f}, frameMat);

        float signY = dockDoorH + 0.75f;
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{bayCenterX - 0.45f, signY - 0.22f, hd + 0.05f}, Vec3{bayCenterX + 0.45f, signY + 0.22f, hd + 0.10f}, darkMetalMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{bayCenterX - 0.38f, signY - 0.15f, hd + 0.11f}, Vec3{bayCenterX + 0.38f, signY + 0.15f, hd + 0.13f}, silverMetalMat);
    }

    // 7. Adaptive Roof with Translucent Ridge Skylight & Ventilators
    float roofOverhang = params_.useExtendedParams ? std::clamp(params_.roofOverhang, 0.1f, 1.5f) : 0.6f;
    float rH = params_.useExtendedParams ? std::clamp(params_.roofHeight, 1.0f, 5.0f) : 2.4f;
    RoofType rt = params_.useExtendedParams ? params_.roofStyle : RoofType::Gable;

    if (rt == RoofType::NoneFlat || rt == RoofType::FlatParapet) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - roofOverhang * 0.5f, h - 0.05f, -hd - roofOverhang * 0.5f},
            Vec3{hw + roofOverhang * 0.5f, h + 0.28f, hd + roofOverhang * 0.5f}, roofMat);
    } else {
        float ridgeY = h + rH;
        Vec3 rFL{-hw - roofOverhang, h,  hd + roofOverhang};
        Vec3 rFR{ hw + roofOverhang, h,  hd + roofOverhang};
        Vec3 rBR{ hw + roofOverhang, h, -hd - roofOverhang};
        Vec3 rBL{-hw - roofOverhang, h, -hd - roofOverhang};
        Vec3 rApexL{-hw - roofOverhang, ridgeY, 0.0f};
        Vec3 rApexR{ hw + roofOverhang, ridgeY, 0.0f};

        BuildingComponents::appendQuad(outVerts, outIndices, rFL, rFR, rApexR, rApexL, Vec3{0, 0.707f, 0.707f}, roofMat);
        BuildingComponents::appendQuad(outVerts, outIndices, rBR, rBL, rApexL, rApexR, Vec3{0, 0.707f, -0.707f}, roofMat);

        Vec3 gL0{-hw, h, hd}; Vec3 gL1{rApexL.x, rApexL.y, 0.0f}; Vec3 gL2{-hw, h, -hd};
        BuildingComponents::appendQuad(outVerts, outIndices, gL0, gL1, gL1, gL2, Vec3{-1, 0, 0}, wallMat);
        Vec3 gR0{hw, h, -hd}; Vec3 gR1{rApexR.x, rApexR.y, 0.0f}; Vec3 gR2{hw, h, hd};
        BuildingComponents::appendQuad(outVerts, outIndices, gR0, gR1, gR1, gR2, Vec3{1, 0, 0}, wallMat);

        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw * 0.75f, ridgeY - 0.05f, -1.0f}, Vec3{hw * 0.75f, ridgeY + 0.25f, 1.0f}, glassMat);
    }

    if (!params_.useExtendedParams || params_.showRooftopClutter) {
        float baseClutterY = (rt == RoofType::NoneFlat || rt == RoofType::FlatParapet) ? h + 0.28f : h + rH + 0.1f;
        for (int tv = -2; tv <= 2; ++tv) {
            float tvX = tv * (hw * 0.35f);
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{tvX - 0.35f, baseClutterY, -0.35f}, Vec3{tvX + 0.35f, baseClutterY + 0.65f, 0.35f}, silverMetalMat);
        }
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{hw * 0.2f, h + 0.4f, -hd * 0.5f}, Vec3{std::min(hw - 0.5f, hw * 0.2f + 3.0f), h + 1.8f, std::min(hd - 0.5f, -hd * 0.5f + 2.0f)}, hvacMat);
    }

    // Exterior Wall Safety Caged Access Ladder to Roof
    float ladX = hw + 0.05f;
    float ladZ = hd * 0.6f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{ladX, 0.0f, ladZ - 0.25f}, Vec3{ladX + 0.10f, h + 0.8f, ladZ + 0.25f}, silverMetalMat);
    for (int lh = 2; lh <= static_cast<int>(h); ++lh) {
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{ladX, static_cast<float>(lh), ladZ - 0.35f}, Vec3{ladX + 0.40f, static_cast<float>(lh) + 0.08f, ladZ + 0.35f}, frameMat);
    }
}

} // namespace procedural::buildings
