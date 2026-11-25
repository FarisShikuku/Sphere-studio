#pragma once

// ============================================================================
// ForestEcosystemEngine.hpp
// Unified C++17 & Vulkan 1.2 Dual-Resolution 3D Forest & Botanical Tree Engine
//
// Strictly implements the two distinct leaf & crown paradigms from the two PDFs:
//
//  1. DOCUMENT 1 — SCATTERED LEAVES IN ASYMMETRIC CROWN HULLS (`plantgl.pdf`):
//     Christophe Pradal & Frédéric Boudon, "Asymmetric Crown Hulls: A Simple
//     Geometric Model of the Tree Crown", UMR AMAP / INRIA Virtual Plants.
//     - 6 extreme control points (P0, P1, P2, P3, Pb, Pt) with 4 cardinal radii
//       (Rx+, Rx-, Ry+, Ry-) and 4 peripheral heights (Hz_x+, Hz_x-, Hz_y+, Hz_y-),
//       eccentricity offset (Ox, Oy), and 2 vertical shape exponents (C_top, C_bot).
//     - Volumetric scattered leaves: Individual 3D leaf platelets sampled
//       stochastically within the asymmetric hull shell rho in [rho_min, 1.0]
//       with Beer-Lambert self-shadowing and leaf inclination distribution.
//
//  2. DOCUMENT 2 — DETAILED STEMS & SHAPED LEAVES WITH PETIOLE
//     (`A_method_for_generating_stochastic_3d_tree_models_ (1).pdf`):
//     - 3D Turtle [H, L, U] recursive branching stems with Leonardo da Vinci's
//       Pipe Model taper (r_parent^eta = sum r_child^eta) & tropism torque.
//     - Well-defined 3D leaves with explicit Petiole (leaf stem), Curved Midrib,
//       Pinnate Lateral Veins, and Parametric Lamina Boundary Contour W(s)
//       (Ovate Acuminate, Lobed Oak/Maple, Cordate Serrated, Lanceolate).
// ============================================================================

#include <cstdint>
#include <vector>

namespace forest_ecosystem {

struct Vec3f {
    float x;
    float y;
    float z;
};

enum class LeafMorphologyKind : int {
    OVATE_ACUMINATE = 0,
    LOBED_OAK_MAPLE = 1,
    CORDATE_SERRATED = 2,
    LANCEOLATE_ACICULAR = 3
};

struct PlantGLAsymmetricHullParams {
    float rxPos = 3.6f;
    float rxNeg = 3.2f;
    float ryPos = 3.5f;
    float ryNeg = 3.1f;
    float hzXPos = 4.2f;
    float hzXNeg = 4.5f;
    float hzYPos = 4.0f;
    float hzYNeg = 4.6f;
    float offsetX = 0.15f;
    float offsetY = -0.10f;
    float crownHeight = 10.5f;
    float shapeTop = 1.85f;
    float shapeBot = 1.45f;
    float trunkHeight = 4.2f;
    float trunkRadius = 0.42f;
    int scatteredLeafCount = 140;
};

struct ScatteredHullLeafPlatelet {
    Vec3f centerPos;
    Vec3f normal;
    Vec3f tangent;
    float length;
    float width;
    float beerLambertLight;
};

struct StochasticLSystemParams {
    int maxOrder = 4;
    float initialLength = 4.6f;
    float initialRadius = 0.48f;
    float lengthRatioDom = 0.82f;
    float lengthRatioLat = 0.72f;
    float branchingAngleDeg = 42.0f;
    float angleStdDevDeg = 7.5f;
    float phyllotaxisDeg = 137.508f;
    float pipeModelEta = 2.0f;
    float areaSplitGamma = 0.58f;
    float tropismSuscept = 0.16f;
    float probMonopodial = 0.45f;
    float probSympodial = 0.40f;
    float probTernary = 0.15f;
    LeafMorphologyKind leafShape = LeafMorphologyKind::OVATE_ACUMINATE;
    float petioleLength = 0.38f;
    float leafBladeLength = 0.95f;
    float leafBladeWidth = 0.58f;
    float leafMidribCamber = 0.22f;
    uint32_t randomSeed = 1337u;
};

struct BranchCylinderSegment {
    Vec3f startPos;
    Vec3f endPos;
    Vec3f heading;
    float radiusBottom;
    float radiusTop;
    int order;
};

struct DetailedLeafWithPetiole3D {
    Vec3f twigAttachmentPos;
    Vec3f bladeBasePos;
    Vec3f bladeApexPos;
    Vec3f bladeNormal;
    Vec3f contourVertices[10];
    float sunlitFactor;
};

class ForestEcosystemEngine {
public:
    static void evaluatePlantGLAsymmetricHull(
        const PlantGLAsymmetricHullParams& hull,
        float thetaRad,
        float phiRad,
        Vec3f& outLocalPos,
        Vec3f& outNormal
    );
};

} // namespace forest_ecosystem
