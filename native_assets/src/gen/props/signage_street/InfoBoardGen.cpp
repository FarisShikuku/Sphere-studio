#include "InfoBoardGen.h"

namespace procedural::props {

InfoBoardGen::InfoBoardGen() {
    params_ = InfoBoardParams{};
}

bool InfoBoardGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void InfoBoardGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload InfoBoardGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;

    uint32_t faceColor = (params_.boardColor == 1) ? 0xFF1D4ED8 : // Interstate Blue
                         (params_.boardColor == 2) ? 0xFF78350F : // Tourist Brown
                                                     0xFF15803D;  // Highway Green
    uint32_t frameColor = 0xFFD1D5DB; // Silver aluminum extruded border
    uint32_t postSteel = 0xFF4B5563;  // Galvanized square steel posts
    uint32_t canopyColor = 0xFF374151;

    float bw = params_.boardWidth;
    float bh = params_.boardHeight;
    float ph = params_.postHeight;

    // Dual posts / H-frame
    float postSpacing = bw * 0.75f;
    float pX0 = -postSpacing * 0.5f;
    float pX1 =  postSpacing * 0.5f;

    // Concrete pier footings
    PropMeshHelper::appendBox(payload, Vec3{pX0, 0.2f, 0.0f}, Vec3{0.35f, 0.4f, 0.35f}, 0xFF666666);
    PropMeshHelper::appendBox(payload, Vec3{pX1, 0.2f, 0.0f}, Vec3{0.35f, 0.4f, 0.35f}, 0xFF666666);

    // Upright steel posts
    float totalPostH = ph + bh * 0.8f;
    PropMeshHelper::appendBox(payload, Vec3{pX0, totalPostH * 0.5f, 0.0f}, Vec3{0.12f, totalPostH, 0.12f}, postSteel);
    PropMeshHelper::appendBox(payload, Vec3{pX1, totalPostH * 0.5f, 0.0f}, Vec3{0.12f, totalPostH, 0.12f}, postSteel);

    // Main sign board frame & face
    float boardCenterY = ph + bh * 0.5f;
    Vec3 boardPos{0.0f, boardCenterY, 0.08f};
    PropMeshHelper::appendBox(payload, boardPos, Vec3{bw, bh, 0.06f}, frameColor);
    PropMeshHelper::appendBox(payload, boardPos + Vec3{0.0f, 0.0f, 0.02f}, Vec3{bw * 0.94f, bh * 0.92f, 0.03f}, faceColor);

    // White reflective border stripe & highway text layout bars
    PropMeshHelper::appendBox(payload, boardPos + Vec3{0.0f, bh * 0.22f, 0.04f}, Vec3{bw * 0.82f, 0.14f, 0.01f}, 0xFFF9FAFB);
    PropMeshHelper::appendBox(payload, boardPos + Vec3{0.0f, -bh * 0.15f, 0.04f}, Vec3{bw * 0.70f, 0.10f, 0.01f}, 0xFFF9FAFB);

    // Rain canopy hood atop board
    if (params_.hasRoofCanopy) {
        Vec3 canopyPos{0.0f, boardCenterY + bh * 0.5f + 0.05f, 0.15f};
        PropMeshHelper::appendBox(payload, canopyPos, Vec3{bw + 0.15f, 0.06f, 0.35f}, canopyColor);
    }

    return payload;
}

} // namespace procedural::props
