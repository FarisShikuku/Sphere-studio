#include "NeonSignGen.h"
#include "../BuildingComponents.h"

namespace procedural::buildings {

void NeonSignGen::generateBillboard(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = params_.width;
    float h = params_.height;
    float colH = params_.columnHeight;
    Vec3 pos = params_.position;

    uint32_t steelMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t canvasMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t hvacMat = static_cast<uint32_t>(MaterialSlot::HvacMetal);

    // Central cylindrical or square structural steel mast
    float colR = 0.45f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pos + Vec3{-colR, 0.0f, -colR},
        pos + Vec3{ colR, colH,  colR},
        steelMat
    );

    // Heavy horizontal support truss beam atop column
    float hw = w * 0.5f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pos + Vec3{-hw, colH - 0.3f, -0.4f},
        pos + Vec3{ hw, colH + 0.3f,  0.4f},
        steelMat
    );

    // Diagonal back-stay trusses
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pos + Vec3{-hw * 0.7f, colH, -0.3f},
        pos + Vec3{-hw * 0.7f + 0.15f, colH + h * 0.7f, -1.2f},
        steelMat
    );
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pos + Vec3{ hw * 0.7f - 0.15f, colH, -0.3f},
        pos + Vec3{ hw * 0.7f, colH + h * 0.7f, -1.2f},
        steelMat
    );

    // Main billboard display canvas face
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pos + Vec3{-hw, colH, 0.0f},
        pos + Vec3{ hw, colH + h, 0.15f},
        canvasMat
    );

    // Perimeter structural trim moulding
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pos + Vec3{-hw - 0.1f, colH - 0.08f, 0.12f},
        pos + Vec3{ hw + 0.1f, colH + h + 0.08f, 0.22f},
        steelMat
    );

    // Maintenance catwalk & safety railing
    if (params_.hasCatwalk) {
        float walkDepth = 1.0f;
        BuildingComponents::appendBox(
            outVerts, outIndices,
            pos + Vec3{-hw, colH - 0.2f, 0.15f},
            pos + Vec3{ hw, colH, 0.15f + walkDepth},
            steelMat
        );
        // Safety railing
        BuildingComponents::appendBox(
            outVerts, outIndices,
            pos + Vec3{-hw, colH, 0.15f + walkDepth - 0.05f},
            pos + Vec3{ hw, colH + 1.0f, 0.15f + walkDepth},
            steelMat
        );
    }

    // Spotlights shining up onto board
    if (params_.hasSpotlights) {
        int lights = 4;
        float spacing = w / (lights + 1);
        for (int i = 1; i <= lights; ++i) {
            float lx = -hw + i * spacing;
            BuildingComponents::appendBox(
                outVerts, outIndices,
                pos + Vec3{lx - 0.15f, colH - 0.1f, 0.9f},
                pos + Vec3{lx + 0.15f, colH + 0.2f, 1.2f},
                hvacMat
            );
        }
    }
}

void NeonSignGen::generateNeonSign(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = params_.width;
    float h = params_.height;
    Vec3 pos = params_.position;

    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t neonMat = static_cast<uint32_t>(MaterialSlot::Glass);

    float hw = w * 0.5f;
    float hh = h * 0.5f;

    // Backplate backing box
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pos + Vec3{-hw, -hh, -0.05f},
        pos + Vec3{ hw,  hh,  0.0f},
        frameMat
    );

    // Neon tube contours (lettering / border channels)
    float t = 0.08f;
    // Outer glowing border
    BuildingComponents::appendBox(outVerts, outIndices, pos + Vec3{-hw + 0.2f,  hh - 0.3f, 0.0f}, pos + Vec3{ hw - 0.2f,  hh - 0.3f + t, 0.06f}, neonMat);
    BuildingComponents::appendBox(outVerts, outIndices, pos + Vec3{-hw + 0.2f, -hh + 0.3f, 0.0f}, pos + Vec3{ hw - 0.2f, -hh + 0.3f + t, 0.06f}, neonMat);
    BuildingComponents::appendBox(outVerts, outIndices, pos + Vec3{-hw + 0.2f, -hh + 0.3f, 0.0f}, pos + Vec3{-hw + 0.2f + t,  hh - 0.3f, 0.06f}, neonMat);
    BuildingComponents::appendBox(outVerts, outIndices, pos + Vec3{ hw - 0.2f - t, -hh + 0.3f, 0.0f}, pos + Vec3{ hw - 0.2f,  hh - 0.3f, 0.06f}, neonMat);

    // Central neon lettering bar
    BuildingComponents::appendBox(outVerts, outIndices, pos + Vec3{-hw * 0.6f, -0.05f, 0.0f}, pos + Vec3{hw * 0.6f, 0.05f, 0.08f}, neonMat);
}

} // namespace procedural::buildings
