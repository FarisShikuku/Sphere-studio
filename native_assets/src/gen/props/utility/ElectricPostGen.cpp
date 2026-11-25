#include "ElectricPostGen.h"
#include <algorithm>
#include <vector>

namespace procedural::props {

ElectricPostGen::ElectricPostGen() {
    params_ = ElectricPostParams{};
}

bool ElectricPostGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void ElectricPostGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload ElectricPostGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;

    // Palette
    uint32_t postColor = (params_.materialType == 1) ? 0xFF888888 : // Concrete
                         (params_.materialType == 2) ? 0xFF707880 : // Galvanized steel
                                                       0xFF523B28;  // Treated creosote timber
    uint32_t armColor = 0xFF5C4330;
    uint32_t insulatorColor = 0xFF2A52BE; // Glazed ceramic blue
    uint32_t metalColor = 0xFF4A4E54;
    uint32_t cableColor = 0xFF222428;     // Conductor dark charcoal

    float h = params_.height;
    float rb = params_.baseRadius;
    float rt = params_.topRadius;
    float armWidth = params_.crossarmWidth;
    int nArms = std::max(1, std::min(3, params_.crossarmCount));
    int poleCount = std::max(1, std::min(3, params_.poleCount));
    float span = params_.spanLength;

    // Determine pole Z positions
    std::vector<float> poleZPositions;
    if (poleCount == 1) {
        poleZPositions.push_back(0.0f);
    } else if (poleCount == 2) {
        poleZPositions.push_back(-span * 0.5f);
        poleZPositions.push_back(span * 0.5f);
    } else { // 3 poles
        poleZPositions.push_back(-span);
        poleZPositions.push_back(0.0f);
        poleZPositions.push_back(span);
    }

    // Structure to record insulator contact points for stringing bent cylinder power lines
    struct WireAnchor {
        Vec3 pos;
    };
    std::vector<std::vector<WireAnchor>> poleWireAnchors(poleZPositions.size());

    // Generate each power post using continuous cylinders
    for (size_t pIdx = 0; pIdx < poleZPositions.size(); ++pIdx) {
        float pZ = poleZPositions[pIdx];

        // 1. Concrete footing collar (continuous cylinder)
        PropMeshHelper::appendOrientedCylinder(
            payload,
            Vec3{0.0f, 0.0f, pZ},
            Vec3{0.0f, 0.35f, pZ},
            rb * 1.35f,
            rb * 1.15f,
            16,
            0xFF3E4246,
            true,
            true
        );

        // 2. Main vertical power post pole (continuous cylinder)
        PropMeshHelper::appendOrientedCylinder(
            payload,
            Vec3{0.0f, 0.35f, pZ},
            Vec3{0.0f, h, pZ},
            rb,
            rt,
            18,
            postColor,
            false,
            true
        );

        // 3. Ground wire conduit clamped along the pole (continuous cylinder)
        PropMeshHelper::appendOrientedCylinder(
            payload,
            Vec3{rb * 0.95f, 0.35f, pZ},
            Vec3{rt * 0.95f, h - 1.0f, pZ},
            0.022f,
            0.022f,
            8,
            metalColor,
            true,
            true
        );

        // 4. Crossarms near top - each is a continuous cylinder with diagonal brace cylinders
        for (int a = 0; a < nArms; ++a) {
            float armY = h - 0.65f - a * 0.95f;
            float halfW = armWidth * 0.5f;

            // Continuous horizontal cylinder crossarm
            PropMeshHelper::appendOrientedCylinder(
                payload,
                Vec3{-halfW, armY, pZ},
                Vec3{halfW, armY, pZ},
                0.065f,
                0.065f,
                14,
                armColor,
                true,
                true
            );

            // Diagonal brace struts (continuous cylinders from pole to crossarm)
            float strutAttachX = armWidth * 0.32f;
            PropMeshHelper::appendOrientedCylinder(
                payload,
                Vec3{-strutAttachX, armY - 0.04f, pZ + 0.04f},
                Vec3{0.0f, armY - 0.52f, pZ + 0.07f},
                0.022f,
                0.022f,
                10,
                metalColor,
                true,
                true
            );
            PropMeshHelper::appendOrientedCylinder(
                payload,
                Vec3{strutAttachX, armY - 0.04f, pZ + 0.04f},
                Vec3{0.0f, armY - 0.52f, pZ + 0.07f},
                0.022f,
                0.022f,
                10,
                metalColor,
                true,
                true
            );

            // Insulators on crossarms (continuous vertical cylinders & flared shed cylinders)
            if (params_.hasInsulators) {
                float positions[3] = {-armWidth * 0.42f, 0.0f, armWidth * 0.42f};
                for (float px : positions) {
                    float pinBaseY = armY + 0.06f;
                    float pinTopY = pinBaseY + 0.22f;

                    // Insulator spindle pin (continuous cylinder)
                    PropMeshHelper::appendOrientedCylinder(
                        payload,
                        Vec3{px, pinBaseY, pZ},
                        Vec3{px, pinTopY, pZ},
                        0.028f,
                        0.022f,
                        10,
                        metalColor,
                        false,
                        true
                    );

                    // Ceramic bell skirt sheds (continuous flared cylinders)
                    PropMeshHelper::appendOrientedCylinder(
                        payload,
                        Vec3{px, pinBaseY + 0.05f, pZ},
                        Vec3{px, pinBaseY + 0.11f, pZ},
                        0.075f,
                        0.055f,
                        12,
                        insulatorColor,
                        true,
                        true
                    );
                    PropMeshHelper::appendOrientedCylinder(
                        payload,
                        Vec3{px, pinBaseY + 0.13f, pZ},
                        Vec3{px, pinBaseY + 0.19f, pZ},
                        0.068f,
                        0.048f,
                        12,
                        insulatorColor,
                        true,
                        true
                    );

                    // Record wire attachment point
                    poleWireAnchors[pIdx].push_back(WireAnchor{Vec3{px, pinTopY + 0.03f, pZ}});
                }
            }
        }

        // Top apex lightning shield wire insulator pin (continuous cylinder)
        float apexY = h + 0.25f;
        PropMeshHelper::appendOrientedCylinder(
            payload,
            Vec3{0.0f, h, pZ},
            Vec3{0.0f, apexY, pZ},
            0.025f,
            0.020f,
            8,
            metalColor,
            false,
            true
        );
        PropMeshHelper::appendOrientedCylinder(
            payload,
            Vec3{0.0f, h + 0.08f, pZ},
            Vec3{0.0f, h + 0.18f, pZ},
            0.055f,
            0.045f,
            10,
            insulatorColor,
            true,
            true
        );
        poleWireAnchors[pIdx].push_back(WireAnchor{Vec3{0.0f, apexY, pZ}});

        // Pole-mounted transformer drum (continuous cylinders)
        if (params_.hasTransformer && pIdx == 0) {
            float transY = h - 2.4f;

            // Mounting bracket band (continuous cylinder)
            PropMeshHelper::appendOrientedCylinder(
                payload,
                Vec3{0.0f, transY - 0.05f, pZ},
                Vec3{0.0f, transY + 0.05f, pZ},
                rb * 1.08f,
                rb * 1.08f,
                16,
                metalColor,
                false,
                false
            );

            // Transformer drum tank (continuous cylinder)
            PropMeshHelper::appendOrientedCylinder(
                payload,
                Vec3{0.0f, transY - 0.50f, pZ + 0.45f},
                Vec3{0.0f, transY + 0.50f, pZ + 0.45f},
                0.24f,
                0.24f,
                18,
                0xFF5C6470,
                true,
                true
            );

            // High voltage primary and secondary bushings (continuous cylinders)
            PropMeshHelper::appendOrientedCylinder(
                payload,
                Vec3{-0.11f, transY + 0.50f, pZ + 0.45f},
                Vec3{-0.15f, transY + 0.72f, pZ + 0.45f},
                0.038f,
                0.025f,
                10,
                insulatorColor,
                true,
                true
            );
            PropMeshHelper::appendOrientedCylinder(
                payload,
                Vec3{0.11f, transY + 0.50f, pZ + 0.45f},
                Vec3{0.15f, transY + 0.72f, pZ + 0.45f},
                0.038f,
                0.025f,
                10,
                insulatorColor,
                true,
                true
            );
        }
    }

    // 5. Continuous bent cylinder power lines swept along mathematical catenary sag curve
    if (params_.hasPowerLines) {
        float sag = params_.wireSag;
        float wireRadius = 0.018f; // Standard high-voltage transmission cable radius

        if (poleCount == 1) {
            // String lines passing through single post insulators to virtual anchor spans
            float spanExtent = span * 0.6f;
            for (const auto& anchor : poleWireAnchors[0]) {
                Vec3 pMid = anchor.pos;
                Vec3 pLeft{pMid.x, pMid.y + 0.35f, pMid.z - spanExtent};
                Vec3 pRight{pMid.x, pMid.y + 0.35f, pMid.z + spanExtent};

                // Left span bent cylinder
                PropMeshHelper::appendBentCylinderCatenary(
                    payload,
                    pLeft,
                    pMid,
                    sag * 0.7f,
                    wireRadius,
                    20,
                    10,
                    cableColor
                );
                // Right span bent cylinder
                PropMeshHelper::appendBentCylinderCatenary(
                    payload,
                    pMid,
                    pRight,
                    sag * 0.7f,
                    wireRadius,
                    20,
                    10,
                    cableColor
                );
            }
        } else {
            // Multi-pole continuous spans: string bent cylinders between consecutive poles
            for (size_t p = 0; p < poleZPositions.size() - 1; ++p) {
                const auto& anchorsA = poleWireAnchors[p];
                const auto& anchorsB = poleWireAnchors[p + 1];
                size_t wireCount = std::min(anchorsA.size(), anchorsB.size());

                for (size_t w = 0; w < wireCount; ++w) {
                    Vec3 p0 = anchorsA[w].pos;
                    Vec3 p1 = anchorsB[w].pos;

                    // Overhead shield wire at apex has slightly less sag due to higher tension
                    float wireSag = (w == wireCount - 1) ? (sag * 0.65f) : sag;

                    PropMeshHelper::appendBentCylinderCatenary(
                        payload,
                        p0,
                        p1,
                        wireSag,
                        wireRadius,
                        28,
                        10,
                        cableColor
                    );
                }
            }

            // Also add lead-in and lead-out terminal lines extending into the distance
            float tailLen = span * 0.35f;
            for (const auto& a : poleWireAnchors.front()) {
                Vec3 p1 = a.pos;
                Vec3 p0{p1.x, p1.y + 0.25f, p1.z - tailLen};
                PropMeshHelper::appendBentCylinderCatenary(payload, p0, p1, sag * 0.45f, wireRadius, 14, 8, cableColor);
            }
            for (const auto& a : poleWireAnchors.back()) {
                Vec3 p0 = a.pos;
                Vec3 p1{p0.x, p0.y + 0.25f, p0.z + tailLen};
                PropMeshHelper::appendBentCylinderCatenary(payload, p0, p1, sag * 0.45f, wireRadius, 14, 8, cableColor);
            }
        }
    }

    return payload;
}

} // namespace procedural::props
