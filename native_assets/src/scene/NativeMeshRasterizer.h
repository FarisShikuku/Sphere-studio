#pragma once
#include "Mesh.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace sphere::scene {

struct CargoRasterParams {
    int32_t cargoId = 0;
    int32_t stackingMode = 0; // 0=Single, 1=Pallet(2x2x2), 2=Rack(2x2x3), 3=TruckBed(3x2x3)
    float rotXDeg = 20.0f;
    float rotYDeg = 35.0f;
    float zoom = 1.0f;
    bool showPallet = false;
    bool showStraps = false;
    bool showShrinkwrap = false;
    float roughness = 0.65f;
    float metallic = 0.0f;
};

struct TrailerRasterParams {
    int32_t trailerId = 0;
    float rotXDeg = 16.0f;
    float rotYDeg = 32.0f;
    float zoom = 1.0f;
    float roughness = 0.45f;
    float metallic = 0.55f;
    float cargoFloorY = 1.25f;
    float deckLengthM = 13.6f;
    float deckWidthM = 2.55f;
    bool showLoadedCargo = false;
    int32_t cargoQuantity = 0;
};

struct VehicleRasterParams {
    int32_t vehicleId = 0;
    float rotXDeg = 15.0f;
    float rotYDeg = 35.0f;
    float zoom = 1.0f;
    float roughness = 0.32f;
    float metallic = 0.45f;
    float fifthWheelOffsetM = 0.0f;
    bool showHitchedTrailer = false;
    float trailerLengthM = 13.6f;
    float trailerWidthM = 2.55f;
    float trailerKingpinOffsetM = 1.65f;
    float trailerDeckY = 1.35f;
    bool showLoadedCargo = false;
    int32_t cargoQuantity = 0;
    bool insideCabinView = false;
    bool isRhd = false;
};

struct PbrStageRasterParams {
    float lightPosX = 0.0f;
    float lightPosY = 2.5f;
    float lightPosZ = 1.8f;
    float lightIntensity = 2500.0f;
    float lightColorR = 1.0f;
    float lightColorG = 0.95f;
    float lightColorB = 0.88f;
    float roughness = 0.45f;
    float metallic = 0.0f;
    float objectWidth = 2.0f;
    float objectHeight = 2.0f;
    float objectDepth = 2.0f;
    uint32_t stageMode = 0;
};

class NativeMeshRasterizer {
public:
    static void RenderCargoBitmap(
        uint32_t* pixelsRGBA8888,
        int32_t stridePixels,
        int32_t width,
        int32_t height,
        const Mesh* cargoMesh,
        const Mesh* palletMesh,
        const CargoRasterParams& params
    );

    static void RenderTrailerBitmap(
        uint32_t* pixelsRGBA8888,
        int32_t stridePixels,
        int32_t width,
        int32_t height,
        const Mesh* trailerMesh,
        const Mesh* loadedCargoMesh,
        const TrailerRasterParams& params
    );

    static void RenderVehicleBitmap(
        uint32_t* pixelsRGBA8888,
        int32_t stridePixels,
        int32_t width,
        int32_t height,
        const Mesh* vehicleMesh,
        const Mesh* hitchedTrailerMesh,
        const Mesh* loadedCargoMesh,
        const VehicleRasterParams& params
    );

    static void RenderPbrStageRGBA8888(
        uint32_t* pixelsRGBA8888,
        int32_t stridePixels,
        int32_t width,
        int32_t height,
        const PbrStageRasterParams& params
    );
};

} // namespace sphere::scene

