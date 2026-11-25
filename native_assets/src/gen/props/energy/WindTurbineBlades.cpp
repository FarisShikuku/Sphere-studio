#include "WindTurbineBlades.h"

namespace procedural::props {

void WindTurbineBlades::generate(GeneratedMeshPayload& payload, const WindTurbineParams& params) {
    uint32_t bladeColor = 0xFFF1F5F9;
    uint32_t tipRed = 0xFFEF4444; // ICAO aviation warning red
    uint32_t rootColor = 0xFFCBD5E1;

    float bladeLen = params.rotorDiameter * 0.5f;
    int nBlades = std::max(2, std::min(4, params.bladeCount));
    float step = 2.0f * PropMeshHelper::kPi / nBlades;
    float pitchRad = params.bladePitchDeg * (PropMeshHelper::kPi / 180.0f);

    float nacY = params.hubHeight + 1.6f;
    Vec3 hubPos{0.0f, nacY, 3.8f};

    for (int b = 0; b < nBlades; ++b) {
        float rotAngle = b * step + pitchRad;
        float cosA = std::cos(rotAngle);
        float sinA = std::sin(rotAngle);

        // Cylindrical pitch bearing root extending radially along the blade axis
        Vec3 rootStart = hubPos + Vec3{cosA * 0.9f, sinA * 0.9f, 0.0f};
        Vec3 rootEnd   = hubPos + Vec3{cosA * 2.0f, sinA * 2.0f, 0.0f};
        PropMeshHelper::appendOrientedCylinder(payload, rootStart, rootEnd, 0.45f, 0.38f, 16, rootColor, true, true);

        // Aerodynamic airfoil blade: root to tip with 6 spanwise stations
        int bladeSegs = 6;
        float segH = (bladeLen - 2.0f) / bladeSegs;

        for (int s = 0; s < bladeSegs; ++s) {
            float distFromHub = 2.0f + (s + 0.5f) * segH;
            float t0 = static_cast<float>(s) / bladeSegs;
            float chord = 2.4f * (1.0f - 0.72f * t0);
            float thick = chord * 0.18f;

            Vec3 localPt{cosA * distFromHub, sinA * distFromHub, 0.0f};
            Vec3 pos = hubPos + localPt;

            uint32_t col = (s >= bladeSegs - 2 && s % 2 == 0) ? tipRed : bladeColor;
            PropMeshHelper::appendBox(payload, pos, Vec3{chord, segH * 0.96f, thick}, col, rotAngle);
        }
    }
}

} // namespace procedural::props

