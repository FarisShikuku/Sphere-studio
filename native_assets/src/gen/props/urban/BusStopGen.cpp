#include "BusStopGen.h"

namespace procedural::props {

BusStopGen::BusStopGen() = default;

bool BusStopGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void BusStopGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload BusStopGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = 8 * sizeof(float);

    uint32_t frameCol = (params_.frameColor == 0) ? 0xFF282C34 : // Charcoal
                        (params_.frameColor == 1) ? 0xFF99A0A8 : // Stainless
                                                    0xFF1D5A9C;  // Blue

    uint32_t glassCol = 0x887AC8E2; // Semi-transparent cyan safety glass
    uint32_t roofCol  = 0xFF353A42;
    uint32_t benchCol = 0xFFBA8A55;

    float L = params_.length;
    float D = params_.depth;
    float H = params_.height;
    float halfL = L * 0.5f;

    // 1. Concrete foundation floor slab
    PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.05f, 0.0f}, Vec3{L + 0.6f, 0.10f, D + 0.6f}, 0xFF60646B);

    // 2. Structural tubular pillars (4 rear posts + 2 front corner posts)
    int rearPillarCount = 4;
    float pR = 0.05f;
    for (int i = 0; i < rearPillarCount; ++i) {
        float x = -halfL + i * (L / (rearPillarCount - 1));
        // Rear pillar
        PropMeshHelper::appendCylinder(payload, Vec3{x, 0.10f, -D * 0.5f}, pR, pR, H, 12, frameCol);
    }
    // 2 Front corner pillars
    PropMeshHelper::appendCylinder(payload, Vec3{-halfL, 0.10f, D * 0.5f}, pR, pR, H, 12, frameCol);
    PropMeshHelper::appendCylinder(payload, Vec3{ halfL, 0.10f, D * 0.5f}, pR, pR, H, 12, frameCol);

    // Horizontal top perimeter header beams
    PropMeshHelper::appendBox(payload, Vec3{0.0f, H + 0.10f, -D * 0.5f}, Vec3{L, 0.08f, 0.08f}, frameCol);
    PropMeshHelper::appendBox(payload, Vec3{0.0f, H + 0.10f,  D * 0.5f}, Vec3{L, 0.08f, 0.08f}, frameCol);
    PropMeshHelper::appendBox(payload, Vec3{-halfL, H + 0.10f, 0.0f}, Vec3{0.08f, 0.08f, D}, frameCol);
    PropMeshHelper::appendBox(payload, Vec3{ halfL, H + 0.10f, 0.0f}, Vec3{0.08f, 0.08f, D}, frameCol);

    // 3. Glazed glass safety windscreens (Rear & Left side)
    if (params_.hasGlassPanels) {
        // Rear 3 bays of glass
        for (int i = 0; i < rearPillarCount - 1; ++i) {
            float x0 = -halfL + i * (L / (rearPillarCount - 1));
            float x1 = -halfL + (i + 1) * (L / (rearPillarCount - 1));
            float bayW = (x1 - x0) - 0.12f;
            Vec3 gCenter{(x0 + x1) * 0.5f, 0.20f + (H - 0.35f) * 0.5f, -D * 0.5f};
            PropMeshHelper::appendBox(payload, gCenter, Vec3{bayW, H - 0.35f, 0.015f}, glassCol);
            // Frosted warning dots band across glass
            PropMeshHelper::appendBox(payload, Vec3{gCenter.x, 1.15f, -D * 0.5f}, Vec3{bayW, 0.06f, 0.02f}, 0xFFE0E0E0);
        }

        // Left side windscreen panel
        PropMeshHelper::appendBox(payload, Vec3{-halfL, 0.20f + (H - 0.35f) * 0.5f, 0.0f}, Vec3{0.015f, H - 0.35f, D - 0.14f}, glassCol);
    }

    // 4. Roof Canopy
    float roofBaseY = H + 0.14f;
    if (params_.roofType == 0) {
        // Curved barrel canopy
        int segments = 8;
        float rOverhangX = L + 0.4f;
        float rOverhangZ = D + 0.4f;
        for (int s = 0; s < segments; ++s) {
            float t0 = static_cast<float>(s) / segments;
            float t1 = static_cast<float>(s + 1) / segments;
            float z0 = -rOverhangZ * 0.5f + t0 * rOverhangZ;
            float z1 = -rOverhangZ * 0.5f + t1 * rOverhangZ;
            float arch0 = 0.22f * std::sin(t0 * 3.14159f);
            float arch1 = 0.22f * std::sin(t1 * 3.14159f);

            Vec3 cMid{0.0f, roofBaseY + (arch0 + arch1) * 0.5f, (z0 + z1) * 0.5f};
            PropMeshHelper::appendBox(payload, cMid, Vec3{rOverhangX, 0.04f, (z1 - z0)}, roofCol);
        }
    } else if (params_.roofType == 1) {
        // Flat cantilever roof with fascias
        PropMeshHelper::appendBox(payload, Vec3{0.0f, roofBaseY + 0.05f, 0.0f}, Vec3{L + 0.5f, 0.10f, D + 0.5f}, roofCol);
    } else {
        // Gable roof
        PropMeshHelper::appendBox(payload, Vec3{0.0f, roofBaseY + 0.15f, 0.0f}, Vec3{L + 0.4f, 0.30f, D + 0.4f}, roofCol);
    }

    // 5. Integrated Timber Bench inside shelter
    if (params_.hasIntegratedBench) {
        float benchL = L * 0.65f;
        float bSeatY = 0.48f;
        // Legs
        PropMeshHelper::appendBox(payload, Vec3{-benchL * 0.4f, bSeatY * 0.5f, -D * 0.25f}, Vec3{0.06f, bSeatY, 0.06f}, frameCol);
        PropMeshHelper::appendBox(payload, Vec3{ benchL * 0.4f, bSeatY * 0.5f, -D * 0.25f}, Vec3{0.06f, bSeatY, 0.06f}, frameCol);
        // Slats
        PropMeshHelper::appendBox(payload, Vec3{0.0f, bSeatY, -D * 0.25f}, Vec3{benchL, 0.04f, 0.35f}, benchCol);
    }

    // 6. Timetable Display Totem / Map Case
    if (params_.hasTimetableTotem) {
        Vec3 tCenter{halfL * 0.8f, 1.35f, -D * 0.45f};
        PropMeshHelper::appendBox(payload, tCenter, Vec3{0.65f, 0.95f, 0.08f}, 0xFF1C2025);
        // White illuminated display map
        PropMeshHelper::appendBox(payload, tCenter + Vec3{0.0f, 0.0f, 0.045f}, Vec3{0.55f, 0.82f, 0.01f}, 0xFFFFFFEE);
    }

    return payload;
}

} // namespace procedural::props
