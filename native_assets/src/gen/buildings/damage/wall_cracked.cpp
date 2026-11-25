#include "DamageGen.h"
#include "../BuildingComponents.h"
#include <cmath>

namespace procedural::buildings {

void DamageGen::generateCrackedWall(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    Vec3 orig = params_.origin;
    float r = params_.radius;
    float sev = params_.severity;

    uint32_t wallMat = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t rebarMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);

    float wallW = 8.0f;
    float wallH = 4.0f;
    float hw = wallW * 0.5f;

    // Background wall segment
    BuildingComponents::appendBox(
        outVerts, outIndices,
        orig + Vec3{-hw, 0.0f, -0.2f},
        orig + Vec3{ hw, wallH, 0.0f},
        wallMat
    );

    // Structural breach aperture / blast crater in center of wall
    float breachR = r * sev * 0.7f;
    int segs = 10;
    float angleStep = 6.28318f / segs;
    Vec3 breachCenter = orig + Vec3{0.0f, wallH * 0.5f, 0.0f};

    // Spalled fractured perimeter jagged blocks around breach
    for (int i = 0; i < segs; ++i) {
        float a = i * angleStep;
        float dist = breachR * (0.8f + 0.35f * std::sin(a * 3.0f));
        float bx = breachCenter.x + std::cos(a) * dist;
        float by = breachCenter.y + std::sin(a) * dist;

        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{bx - 0.25f, by - 0.25f, -0.15f},
            Vec3{bx + 0.25f, by + 0.25f,  0.15f},
            foundMat
        );
    }

    // Exposed bent steel rebar grid across the structural breach
    if (params_.exposeRebar) {
        float rebarThickness = 0.025f;
        int bars = 4;
        for (int b = 0; b < bars; ++b) {
            float frac = (b - 1.5f) / 1.5f;
            float by = breachCenter.y + frac * breachR * 0.65f;
            // Horizontal rebar bar with slight bend
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{breachCenter.x - breachR * 0.9f, by - rebarThickness, -0.05f},
                Vec3{breachCenter.x + breachR * 0.9f, by + rebarThickness, -0.02f},
                rebarMat
            );

            float bx = breachCenter.x + frac * breachR * 0.65f;
            // Vertical rebar bar
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{bx - rebarThickness, breachCenter.y - breachR * 0.9f, -0.05f},
                Vec3{bx + rebarThickness, breachCenter.y + breachR * 0.9f, -0.02f},
                rebarMat
            );
        }
    }
}

} // namespace procedural::buildings
