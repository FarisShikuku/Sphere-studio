#include "TowerGen.h"
#include "../BuildingComponents.h"
#include "../interiors/RoomGen.h"
#include <algorithm>

namespace procedural::buildings {

void TowerGen::generateServiceGarage(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(28.0f, params_.footprintWidth);
    float d = std::max(18.0f, params_.footprintDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;
    float bayH = std::max(5.8f, params_.storyHeight);

    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t wallMat     = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t brickMat    = static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t frameMat    = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t silverAlum  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t roofMat     = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t glassMat    = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t signRed     = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t signOrange  = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t yellowMat   = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t hvacMat     = static_cast<uint32_t>(MaterialSlot::HvacMetal);

    // 1. Paved Tarmac Service Forecourt & Customer Lot
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 3.5f, -0.35f, -hd - 2.0f},
        Vec3{ hw + 3.5f,  0.00f,  hd + 8.0f},
        tarmacMat);

    // Painted customer parking spaces in front
    for (float px = -hw + 1.5f; px <= hw - 2.0f; px += 3.4f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px, 0.0f, hd + 3.5f},
            Vec3{px + 0.12f, 0.025f, hd + 7.2f},
            silverAlum);
    }

    // 2. Main Concrete Workshop Foundation Plinth
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.25f, 0.0f, -hd - 0.25f},
        Vec3{ hw + 0.25f, 0.25f, hd + 0.25f},
        foundMat);

    // Split layout: Left 26% = Customer Lobby & Parts Office, Right 74% = Spacious Multi-Bay Vehicle Repair Hall
    float splitX = -hw + w * 0.26f;
    float lobbyH = 4.2f;
    int numBays = (w >= 30.0f) ? 4 : 3;
    float shopX0 = splitX;
    float shopX1 = hw;
    float shopW = shopX1 - shopX0;

    if (params_.showInside) {
        // Cutaway Customer Lobby Wing (Left) - open top & low front wall showing service desk and waiting seats
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.25f, -hd}, Vec3{splitX, lobbyH, -hd + 0.25f}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.25f, -hd}, Vec3{-hw + 0.25f, lobbyH, hd - 0.1f}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.25f, hd - 0.35f}, Vec3{splitX, 1.05f, hd - 0.1f}, brickMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw + 0.3f, 0.25f, -hd + 0.3f}, Vec3{splitX - 0.2f, 0.29f, hd - 0.3f},
            static_cast<uint32_t>(MaterialSlot::ShopFloorEpoxy));
        // Customer waiting bench & parts counter inside lobby
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw + 1.2f, 0.29f, 0.5f}, Vec3{splitX - 1.0f, 1.30f, 1.35f}, signOrange);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw + 1.0f, 0.29f, -hd + 1.4f}, Vec3{-hw + 3.4f, 0.85f, -hd + 2.2f}, frameMat);

        // Workshop Floor Slab & Rear/Dividing Full-Height Walls
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{shopX0, 0.25f, -hd}, Vec3{shopX1, 0.30f, hd}, foundMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{shopX0, 0.25f, -hd}, Vec3{shopX1, bayH, -hd + 0.35f}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{shopX0 - 0.20f, 0.25f, -hd}, Vec3{shopX0 + 0.20f, bayH, hd}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{shopX0, 0.25f, -hd - 0.04f}, Vec3{shopX1, 1.35f, -hd + 0.37f}, brickMat);
        BuildingComponents::generateWindow(outVerts, outIndices,
            Vec3{(shopX0 + shopX1) * 0.5f, bayH - 1.0f, -hd},
            shopW * 0.65f, 0.9f, Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            WindowType::ClerestoryTransom, GlassType::FrostedSatinPrivacy);

        // Cutaway low front (+Z) & right (+X) brick plinths + perimeter steel columns & brand lintel beam
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{shopX0, 0.25f, hd - 0.35f}, Vec3{shopX1, 0.62f, hd}, brickMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{shopX1 - 0.35f, 0.25f, -hd}, Vec3{shopX1, 0.62f, hd}, brickMat);
        for (float pz : {-hd + 0.25f, 0.0f, hd - 0.25f}) {
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{shopX1 - 0.32f, 0.25f, pz - 0.14f}, Vec3{shopX1, bayH, pz + 0.14f}, frameMat);
        }
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{shopX0, bayH - 0.30f, hd - 0.25f}, Vec3{shopX1, bayH, hd}, signOrange);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{shopX1 - 0.25f, bayH - 0.25f, -hd}, Vec3{shopX1, bayH, hd}, frameMat);

        // Full Interior Fitout with Hydraulic 2-Post / 4-Post / Scissor Lifts, Tires, Vehicles (Sedan, SUV, Truck, Bus)
        RoomGen::appendVehicleRepairInteriorContent(
            outVerts, outIndices,
            (shopX0 + shopX1) * 0.5f, 0.30f, 0.0f,
            shopW - 0.5f, d - 0.6f, bayH - 0.4f,
            params_.interiorDesign
        );

        // Corner Auto Service Pylon Sign
        float signX = -hw - 1.5f;
        float signZ = hd + 2.5f;
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{signX - 0.55f, 0.0f, signZ - 0.25f}, Vec3{signX + 0.55f, 5.4f, signZ + 0.25f}, blackAlum);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{signX - 0.50f, 3.6f, signZ - 0.28f}, Vec3{signX + 0.50f, 5.2f, signZ + 0.28f}, signOrange);
        return;
    }

    // 3. Customer Reception & Parts Lobby Wing (Left)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.25f, -hd},
        Vec3{splitX, lobbyH, hd - 0.1f},
        wallMat);
    // Brick lower architectural wainscot
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.04f, 0.25f, -hd - 0.04f},
        Vec3{splitX + 0.04f, 1.25f, hd - 0.05f},
        brickMat);
    // Storefront panoramic glazing & automatic entrance door facing +Z
    BuildingComponents::generateWindow(outVerts, outIndices,
        Vec3{(-hw + splitX) * 0.5f - 1.6f, 2.2f, hd - 0.1f},
        2.8f, 2.1f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        WindowType::PictureGlass, GlassType::ClearFloat);
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{splitX - 2.0f, 0.25f, hd - 0.1f},
        1.8f, 2.5f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true);

    // Customer lobby flat roof & brand parapet
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.25f, lobbyH, -hd - 0.25f},
        Vec3{splitX + 0.1f, lobbyH + 0.45f, hd + 0.25f},
        roofMat);

    // 4. Multi-Bay Mechanic Workshop Hall (Right)

    // Workshop Interior Concrete Floor Slab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{shopX0, 0.25f, -hd},
        Vec3{shopX1, 0.30f,  hd},
        foundMat);

    // Rear solid wall, Right wall, & Dividing wall between lobby and workshop
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{shopX0, 0.25f, -hd},
        Vec3{shopX1, bayH, -hd + 0.45f},
        wallMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{shopX1 - 0.45f, 0.25f, -hd},
        Vec3{shopX1, bayH, hd},
        wallMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{shopX0 - 0.25f, 0.25f, -hd},
        Vec3{shopX0 + 0.25f, bayH, hd},
        wallMat);

    // Lower brick wainscot on right & rear walls
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{shopX0, 0.25f, -hd - 0.04f},
        Vec3{shopX1 + 0.04f, 1.35f, hd + 0.04f},
        brickMat);
    // Clerestory daylight windows on rear wall
    BuildingComponents::generateWindow(outVerts, outIndices,
        Vec3{(shopX0 + shopX1) * 0.5f, bayH - 1.0f, -hd},
        shopW * 0.65f, 0.9f, Vec3{0, 0, -1}, Vec3{-1, 0, 0},
        WindowType::ClerestoryTransom, GlassType::FrostedSatinPrivacy);

    // Front Service Bay Pier Columns, Headers & Recessed Roll-Up Bays
    float pierW = 0.85f;
    float baySpan = shopW / static_cast<float>(numBays);
    float doorW = baySpan - pierW;
    float doorH = std::min(bayH - 1.1f, 3.6f);

    // Continuous front top lintel header above bays
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{shopX0, 0.25f + doorH, hd - 0.45f},
        Vec3{shopX1, bayH, hd},
        wallMat);

    for (int b = 0; b <= numBays; ++b) {
        float px = shopX0 + b * baySpan;
        float xL = std::max(shopX0, px - pierW * 0.5f);
        float xR = std::min(shopX1, px + pierW * 0.5f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{xL, 0.25f, hd - 0.45f},
            Vec3{xR, 0.25f + doorH, hd},
            brickMat);
        // Yellow protective corner bollards guarding each garage bay pier
        if (b > 0 && b < numBays) {
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{px, 0.25f, hd + 0.35f},
                Vec3{px, 1.25f, hd + 0.35f},
                0.10f, 0.10f, 8, yellowMat);
        }
    }

    for (int b = 0; b < numBays; ++b) {
        float bayCenterX = shopX0 + (b + 0.5f) * baySpan;
        float bayDoorL = bayCenterX - doorW * 0.5f + 0.08f;
        float bayDoorR = bayCenterX + doorW * 0.5f - 0.08f;

        // Open/half-raised corrugated sectional roll-up door (leaving lower 2.5m open to reveal car lifts!)
        bool isOpenBay = (b % 2 == 0);
        float shutterBotY = isOpenBay ? (0.25f + doorH * 0.68f) : 0.28f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bayDoorL, shutterBotY, hd - 0.25f},
            Vec3{bayDoorR, 0.25f + doorH, hd - 0.12f},
            silverAlum);
        // Glass vision row inside garage door panel
        float glassRowY = isOpenBay ? (shutterBotY + 0.15f) : (0.25f + doorH * 0.55f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bayDoorL + 0.25f, glassRowY, hd - 0.27f},
            Vec3{bayDoorR - 0.25f, glassRowY + 0.48f, hd - 0.10f},
            glassMat);

        // Hydraulic 2-Post Vehicle Lift inside every bay (clearly visible through open bays!)
        float liftZ = 0.5f;
        float postSpread = 1.45f;
        for (int s : {-1, 1}) {
            float lx = bayCenterX + s * postSpread;
            // Upright hydraulic column
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{lx - 0.14f, 0.30f, liftZ - 0.18f},
                Vec3{lx + 0.14f, 3.25f, liftZ + 0.18f},
                yellowMat);
            // Swing lift arms
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{bayCenterX + s * 0.55f, 0.95f, liftZ - 0.65f},
                Vec3{lx, 1.10f, liftZ + 0.65f},
                blackAlum);
        }
        // Overhead lift crossbeam bar
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bayCenterX - postSpread - 0.14f, 3.15f, liftZ - 0.12f},
            Vec3{bayCenterX + postSpread + 0.14f, 3.35f, liftZ + 0.12f},
            yellowMat);

        // Mechanic Red Tool Chest Workbench against rear wall
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bayCenterX - 1.1f, 0.30f, -hd + 0.55f},
            Vec3{bayCenterX + 1.1f, 1.35f, -hd + 1.25f},
            signRed);
    }

    // 5. Workshop Roof Deck, High Parapet & Brand Service Fascia
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{shopX0 - 0.2f, bayH, -hd - 0.25f},
        Vec3{shopX1 + 0.25f, bayH + 0.55f, hd + 0.35f},
        roofMat);
    // Bold Red & Orange Auto-Service Brand Stripe along entire front fascia
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.28f, lobbyH - 0.25f, hd + 0.26f},
        Vec3{ shopX1 + 0.28f, bayH + 0.45f, hd + 0.40f},
        blackAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{shopX0 + 0.1f, bayH - 0.15f, hd + 0.40f},
        Vec3{shopX1 + 0.25f, bayH + 0.30f, hd + 0.48f},
        signOrange);

    // Rooftop Air Compressor Tank & HVAC Units
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{shopX1 - 3.5f, bayH + 0.95f, -2.0f},
        Vec3{shopX1 - 1.5f, bayH + 0.95f, -2.0f},
        0.45f, 0.45f, 10, frameMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{shopX0 + 2.0f, bayH + 0.55f, -2.0f},
        Vec3{shopX0 + 4.2f, bayH + 1.45f,  0.2f},
        hvacMat);

    // 6. Exterior Tire Storage Display Rack & Service Pylon Sign
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{shopX1 + 0.05f, 0.25f, -2.0f},
        Vec3{shopX1 + 0.95f, 2.35f,  2.5f},
        frameMat);
    for (float tz = -1.5f; tz <= 2.0f; tz += 0.7f) {
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{shopX1 + 0.2f, 0.85f, tz},
            Vec3{shopX1 + 0.8f, 0.85f, tz},
            0.32f, 0.32f, 10, blackAlum);
    }

    // Corner Auto Service Pylon Sign
    float signX = -hw - 1.5f;
    float signZ = hd + 2.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{signX - 0.55f, 0.0f, signZ - 0.25f},
        Vec3{signX + 0.55f, 5.4f, signZ + 0.25f},
        blackAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{signX - 0.50f, 3.6f, signZ - 0.28f},
        Vec3{signX + 0.50f, 5.2f, signZ + 0.28f},
        signOrange);
}

} // namespace procedural::buildings
