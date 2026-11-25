#include "PalletGen.h"

namespace procedural::props {

PalletGen::PalletGen() = default;

bool PalletGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void PalletGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload PalletGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = 8 * sizeof(float);

    uint32_t woodCol = 0xFFC29B63; // Raw pine timber
    uint32_t brandCol = 0xFF332211; // EUR branded burn mark

    float L = params_.length;
    float W = (params_.palletSpec == 1) ? 1.0f : params_.width; // 1000mm for GMA
    float H = params_.height;

    float boardThick = 0.022f; // 22mm timber thickness
    float blockH = H - boardThick * 2.0f; // 100mm blocks
    float blockW = 0.145f;
    float blockL = 0.145f;

    // 1. Bottom Skids: 3 longitudinal runner boards
    float runnerX[3] = {-W * 0.5f + blockW * 0.5f, 0.0f, W * 0.5f - blockW * 0.5f};
    for (float rx : runnerX) {
        PropMeshHelper::appendBox(payload, Vec3{rx, boardThick * 0.5f, 0.0f}, Vec3{blockW, boardThick, L}, woodCol);
    }

    // 2. 9 Solid spacer blocks (3 rows of 3 blocks)
    float blockZ[3] = {-L * 0.5f + blockL * 0.5f, 0.0f, L * 0.5f - blockL * 0.5f};
    for (float rx : runnerX) {
        for (float rz : blockZ) {
            Vec3 bCenter{rx, boardThick + blockH * 0.5f, rz};
            PropMeshHelper::appendBox(payload, bCenter, Vec3{blockW, blockH, blockL}, woodCol);
        }
    }

    // 3. 3 Cross stringer boards across the blocks
    float stringerY = boardThick + blockH + boardThick * 0.5f;
    for (float rz : blockZ) {
        PropMeshHelper::appendBox(payload, Vec3{0.0f, stringerY, rz}, Vec3{W, boardThick, blockL}, woodCol);
    }

    // 4. Top Deck boards (transverse or longitudinal)
    float deckTopY = boardThick + blockH + boardThick + boardThick * 0.5f;
    int nBoards = params_.topBoardCount;
    if (nBoards < 4) nBoards = 4;
    float deckBoardW = (L * 0.85f) / static_cast<float>(nBoards);

    for (int i = 0; i < nBoards; ++i) {
        float z = -L * 0.5f + deckBoardW * 0.5f + i * ((L - deckBoardW) / static_cast<float>(nBoards - 1));
        PropMeshHelper::appendBox(payload, Vec3{0.0f, deckTopY, z}, Vec3{W, boardThick, deckBoardW * 0.90f}, woodCol);
    }

    // EPAL / EUR stencil markings on corner blocks
    PropMeshHelper::appendBox(payload, Vec3{runnerX[0] - blockW * 0.51f, boardThick + blockH * 0.5f, blockZ[0]}, Vec3{0.01f, 0.05f, 0.08f}, brandCol);
    PropMeshHelper::appendBox(payload, Vec3{runnerX[2] + blockW * 0.51f, boardThick + blockH * 0.5f, blockZ[2]}, Vec3{0.01f, 0.05f, 0.08f}, brandCol);

    return payload;
}

} // namespace procedural::props
