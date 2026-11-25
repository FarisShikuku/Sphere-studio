#include "CrateGen.h"

namespace procedural::props {

CrateGen::CrateGen() = default;

bool CrateGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void CrateGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload CrateGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = 8 * sizeof(float);

    uint32_t panelCol = (params_.woodStyle == 0) ? 0xFFC9A26A : // Pine
                        (params_.woodStyle == 1) ? 0xFF4D5638 : // Military Olive
                                                   0xFF704728;  // Hardwood

    uint32_t frameCol = (params_.woodStyle == 0) ? 0xFFA88048 :
                        (params_.woodStyle == 1) ? 0xFF3D452B :
                                                   0xFF58351B;

    Vec3 sz{params_.width, params_.height, params_.depth};
    Vec3 half = sz * 0.5f;

    // Inner core recessed box
    PropMeshHelper::appendBox(payload, Vec3{0.0f, half.y, 0.0f}, sz * 0.96f, panelCol);

    float plankThick = 0.035f;
    float plankW = 0.12f;

    // Outer edge framing beams (12 edges of the crate)
    // 4 Vertical corner posts
    float xOffsets[4] = {-half.x + plankW * 0.5f,  half.x - plankW * 0.5f, -half.x + plankW * 0.5f,  half.x - plankW * 0.5f};
    float zOffsets[4] = {-half.z + plankW * 0.5f, -half.z + plankW * 0.5f,  half.z - plankW * 0.5f,  half.z - plankW * 0.5f};

    for (int i = 0; i < 4; ++i) {
        PropMeshHelper::appendBox(payload, Vec3{xOffsets[i], half.y, zOffsets[i]}, Vec3{plankW, sz.y, plankW}, frameCol);
    }

    // Horizontal top & bottom rim perimeter beams
    float yLevels[2] = {plankW * 0.5f, sz.y - plankW * 0.5f};
    for (float yL : yLevels) {
        // Front & Back
        PropMeshHelper::appendBox(payload, Vec3{0.0f, yL,  half.z - plankThick * 0.5f}, Vec3{sz.x, plankW, plankThick}, frameCol);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, yL, -half.z + plankThick * 0.5f}, Vec3{sz.x, plankW, plankThick}, frameCol);
        // Left & Right
        PropMeshHelper::appendBox(payload, Vec3{ half.x - plankThick * 0.5f, yL, 0.0f}, Vec3{plankThick, plankW, sz.z}, frameCol);
        PropMeshHelper::appendBox(payload, Vec3{-half.x + plankThick * 0.5f, yL, 0.0f}, Vec3{plankThick, plankW, sz.z}, frameCol);
    }

    // Diagonal X Cross-braces on vertical facades
    if (params_.hasCrossBraces) {
        float diagAngle = std::atan2(sz.y, sz.x);
        float diagLen = std::sqrt(sz.x * sz.x + sz.y * sz.y) * 0.85f;

        // Front Face (+Z) diagonals
        PropMeshHelper::appendBox(payload, Vec3{0.0f, half.y, half.z - plankThick * 0.5f}, Vec3{diagLen, plankW * 0.8f, plankThick}, frameCol, diagAngle);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, half.y, half.z - plankThick * 0.5f}, Vec3{diagLen, plankW * 0.8f, plankThick}, frameCol, -diagAngle);

        // Back Face (-Z) diagonals
        PropMeshHelper::appendBox(payload, Vec3{0.0f, half.y, -half.z + plankThick * 0.5f}, Vec3{diagLen, plankW * 0.8f, plankThick}, frameCol, diagAngle);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, half.y, -half.z + plankThick * 0.5f}, Vec3{diagLen, plankW * 0.8f, plankThick}, frameCol, -diagAngle);
    }

    // Metallic corner reinforcement L-brackets
    if (params_.hasMetalCorners) {
        float bracketSize = 0.14f;
        for (float sx : {-1.0f, 1.0f}) {
            for (float sy : {0.0f, 1.0f}) {
                for (float szSign : {-1.0f, 1.0f}) {
                    float y = (sy == 0.0f) ? bracketSize * 0.5f : sz.y - bracketSize * 0.5f;
                    Vec3 bPos{sx * (half.x - bracketSize * 0.4f), y, szSign * (half.z - bracketSize * 0.4f)};
                    PropMeshHelper::appendBox(payload, bPos, Vec3{bracketSize, bracketSize, bracketSize} * 0.8f, 0xFF33383E);
                }
            }
        }
    }

    return payload;
}

} // namespace procedural::props
