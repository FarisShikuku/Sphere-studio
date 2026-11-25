#include "ShelfGen.h"

namespace procedural::props {

ShelfGen::ShelfGen() = default;

bool ShelfGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void ShelfGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload ShelfGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = 8 * sizeof(float);

    uint32_t shelfCol = (params_.shelfMaterial == 0) ? 0xFFE6CCA8 : // Light Birch
                        (params_.shelfMaterial == 1) ? 0xFF353A40 : // Industrial Steel
                                                       0xFF52341C;  // Dark Walnut

    uint32_t bookColors[4] = {0xFFB8281D, 0xFF1D5A9C, 0xFF2A7A44, 0xFFD89A12};

    float W = params_.width;
    float H = params_.height;
    float D = params_.depth;
    float boardThick = 0.025f;

    // 1. Left & Right vertical gable upright side panels
    float leftX = -W * 0.5f + boardThick * 0.5f;
    float rightX = W * 0.5f - boardThick * 0.5f;
    PropMeshHelper::appendBox(payload, Vec3{leftX, H * 0.5f, 0.0f}, Vec3{boardThick, H, D}, shelfCol);
    PropMeshHelper::appendBox(payload, Vec3{rightX, H * 0.5f, 0.0f}, Vec3{boardThick, H, D}, shelfCol);

    // 2. Base plinth / bottom kickplate
    float kickplateH = 0.07f;
    PropMeshHelper::appendBox(payload, Vec3{0.0f, kickplateH * 0.5f, D * 0.5f - boardThick}, Vec3{W - boardThick * 2.0f, kickplateH, boardThick}, shelfCol);

    // 3. Top cover board
    PropMeshHelper::appendBox(payload, Vec3{0.0f, H - boardThick * 0.5f, 0.0f}, Vec3{W, boardThick, D}, shelfCol);

    // 4. Backboard panel
    if (params_.hasBackboard) {
        PropMeshHelper::appendBox(payload, Vec3{0.0f, H * 0.5f, -D * 0.5f + 0.005f}, Vec3{W - boardThick * 2.0f, H - kickplateH, 0.01f}, shelfCol);
    }

    // 5. Horizontal shelf boards
    int tiers = std::max(params_.tierCount, 2);
    float availH = H - kickplateH - boardThick;
    float tierStep = availH / (tiers - 1);

    for (int t = 0; t < tiers; ++t) {
        float y = kickplateH + t * tierStep;
        PropMeshHelper::appendBox(payload, Vec3{0.0f, y, 0.0f}, Vec3{W - boardThick * 2.0f, boardThick, D * 0.96f}, shelfCol);

        // Procedural Books / storage boxes on shelves
        if (params_.hasBooksAndObjects && t < tiers - 1) {
            float shelfY = y + boardThick * 0.5f;
            float shelfInnerW = W - boardThick * 2.5f;

            // Group of books on the left
            int nBooks = 4 + (t % 3) * 2;
            float startBx = -shelfInnerW * 0.45f;
            for (int b = 0; b < nBooks; ++b) {
                float bW = 0.035f;
                float bH = 0.18f + (b % 3) * 0.03f;
                float bD = D * 0.65f;
                uint32_t bCol = bookColors[(t + b) % 4];
                PropMeshHelper::appendBox(payload, Vec3{startBx + b * (bW + 0.005f), shelfY + bH * 0.5f, 0.0f}, Vec3{bW, bH, bD}, bCol);
            }

            // Storage box or vase on the right
            if (t % 2 == 0) {
                float boxW = 0.22f;
                float boxH = 0.16f;
                PropMeshHelper::appendBox(payload, Vec3{shelfInnerW * 0.35f, shelfY + boxH * 0.5f, 0.0f}, Vec3{boxW, boxH, D * 0.8f}, 0xFF353940);
            } else {
                PropMeshHelper::appendCylinder(payload, Vec3{shelfInnerW * 0.35f, shelfY, 0.0f}, 0.07f, 0.05f, 0.18f, 10, 0xFF4A7C59);
            }
        }
    }

    return payload;
}

} // namespace procedural::props
