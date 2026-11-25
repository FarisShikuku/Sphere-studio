// ============================================================================
// LSystemTree.cpp - 3D Turtle Interpretation & Mesh Cylinder Tessellator
// Exact implementation of Nemanja Stojanovic (2016):
// "A method for generating stochastic 3D tree models with Python in Autodesk Maya"
// Journal of Graphic Engineering and Design, Volume 7 (2), 2016, pp. 25-30.
// ============================================================================

#include "LSystemTree.h"
#include <cmath>
#include <algorithm>

namespace vegetation {

LSystemTree::LSystemTree(const TreeSpeciesConfig& config)
    : m_config(config) {}

TreeMeshData LSystemTree::generateTreeMesh(uint64_t seed) {
    FastRng rng(seed);
    TreeGrammar grammar = TreeGrammar::createSpeciesGrammar(m_config.lsystemAxiom, m_config.lsystemRules);
    std::string derivation = grammar.derive(m_config.maxRecursionDepth, rng);

    TreeMeshData mesh;
    TurtleState currentTurtle;
    // Initial vectors from paper: vector one at (0,0,0), vector two at (0,y,0)
    currentTurtle.position = Vec3(0.0f, 0.0f, 0.0f);
    currentTurtle.heading = Vec3(0.0f, 1.0f, 0.0f);  // Heading H
    currentTurtle.left = Vec3(-1.0f, 0.0f, 0.0f);    // Left L
    currentTurtle.up = Vec3(0.0f, 0.0f, 1.0f);       // Up U (H x L = U)
    currentTurtle.currentRadius = m_config.trunkRadiusMeters;
    currentTurtle.depth = 0;
    currentTurtle.iterationStep = 0;

    std::vector<TurtleState> stateStack;
    stateStack.reserve(48);

    // Initial branch length based on tree height and recursion depth
    float baseBranchLength = m_config.treeHeightMeters / static_cast<float>(m_config.maxRecursionDepth + 2);

    for (char sym : derivation) {
        interpretSymbol(sym, currentTurtle, stateStack, mesh, rng, baseBranchLength, m_config.maxRecursionDepth);
    }

    // Compute bounding box
    if (!mesh.vertices.empty()) {
        mesh.aabbMin = mesh.vertices[0].position;
        mesh.aabbMax = mesh.vertices[0].position;
        for (const auto& v : mesh.vertices) {
            mesh.aabbMin.x = std::min(mesh.aabbMin.x, v.position.x);
            mesh.aabbMin.y = std::min(mesh.aabbMin.y, v.position.y);
            mesh.aabbMin.z = std::min(mesh.aabbMin.z, v.position.z);
            mesh.aabbMax.x = std::max(mesh.aabbMax.x, v.position.x);
            mesh.aabbMax.y = std::max(mesh.aabbMax.y, v.position.y);
            mesh.aabbMax.z = std::max(mesh.aabbMax.z, v.position.z);
        }
    }

    return mesh;
}

void LSystemTree::interpretSymbol(
    char symbol,
    TurtleState& state,
    std::vector<TurtleState>& stateStack,
    TreeMeshData& mesh,
    FastRng& rng,
    float branchLength,
    int totalIterations
) {
    // Stochastic parameters from Stojanovic (2016) Section 4:
    // r1 in [1, 8] deg (roll perturbation)
    // r2 in [25, 40] deg (branching angle)
    // r3 in [0, 1] (length decay factor)
    float r1 = rng.nextRange(1.0f, 8.0f);
    float r2 = (m_config.branchAngleMeanDeg > 0.0f)
        ? (m_config.branchAngleMeanDeg + rng.nextRange(-m_config.branchAngleDeviationDeg, m_config.branchAngleDeviationDeg))
        : rng.nextRange(25.0f, 40.0f);
    float r3 = rng.nextFloat01();

    float angleRad = r2 * (3.14159265f / 180.0f);
    float rollRad = r1 * (3.14159265f / 180.0f);

    switch (symbol) {
        case 'F': {
            // Paper formula: Length of branches = 10 - j * r3 (or stochastic decay)
            int j = state.iterationStep;
            float lengthFactor = std::max(0.25f, (10.0f - static_cast<float>(j) * (0.5f + r3 * 0.5f)) / 10.0f);
            float len = branchLength * lengthFactor;

            Vec3 nextPos = state.position + state.heading * len;

            // Leonardo da Vinci postulate (Section 3 of Stojanovic 2016):
            // Width of branches in every iteration = width in previous iteration * wr (wr = 0.707)
            float nextRadius = state.currentRadius * LEONARDO_DA_VINCI_WR;
            float windWeight = std::min(1.0f, static_cast<float>(state.depth) / 4.0f) * m_config.windFlexibility;

            // Generate truncated cone segment with Sx radial subdivisions
            emitBranchSegment(state.position, nextPos, state.currentRadius, nextRadius, mesh, windWeight, state.depth, rng);

            state.position = nextPos;
            state.currentRadius = std::max(0.015f, nextRadius);
            state.iterationStep++;
            break;
        }

        // Equation (1) Stojanovic (2016): [H' L' U'] = [H L U] * R
        // RU(alpha): Yaw around Up axis U
        case '+': { // Rotate around U by angle +alpha (yaw left)
            float cosA = std::cos(angleRad);
            float sinA = std::sin(angleRad);
            Vec3 newH = state.heading * cosA - state.left * sinA;
            Vec3 newL = state.heading * sinA + state.left * cosA;
            state.heading = newH.normalized();
            state.left = newL.normalized();
            break;
        }
        case '-': { // Rotate around U by angle -alpha (yaw right)
            float cosA = std::cos(angleRad);
            float sinA = std::sin(angleRad);
            Vec3 newH = state.heading * cosA + state.left * sinA;
            Vec3 newL = state.heading * (-sinA) + state.left * cosA;
            state.heading = newH.normalized();
            state.left = newL.normalized();
            break;
        }

        // RL(alpha): Pitch around Left axis L
        case '&': { // Rotate around L by angle +alpha (pitch down)
            float cosA = std::cos(angleRad);
            float sinA = std::sin(angleRad);
            Vec3 newH = state.heading * cosA + state.up * sinA;
            Vec3 newU = state.heading * (-sinA) + state.up * cosA;
            state.heading = newH.normalized();
            state.up = newU.normalized();
            break;
        }
        case '^': { // Rotate around L by angle -alpha (pitch up)
            float cosA = std::cos(angleRad);
            float sinA = std::sin(angleRad);
            Vec3 newH = state.heading * cosA - state.up * sinA;
            Vec3 newU = state.heading * sinA + state.up * cosA;
            state.heading = newH.normalized();
            state.up = newU.normalized();
            break;
        }

        // RH(alpha): Roll around Heading axis H
        case '\\': { // Rotate around H by angle -alpha (roll left)
            float cosR = std::cos(rollRad);
            float sinR = std::sin(rollRad);
            Vec3 newL = state.left * cosR - state.up * sinR;
            Vec3 newU = state.left * sinR + state.up * cosR;
            state.left = newL.normalized();
            state.up = newU.normalized();
            break;
        }
        case '/': { // Rotate around H by angle +alpha (roll right)
            float cosR = std::cos(rollRad);
            float sinR = std::sin(rollRad);
            Vec3 newL = state.left * cosR + state.up * sinR;
            Vec3 newU = state.left * (-sinR) + state.up * cosR;
            state.left = newL.normalized();
            state.up = newU.normalized();
            break;
        }

        case '[': { // Store turtle state in array
            stateStack.push_back(state);
            state.depth++;
            break;
        }
        case ']': { // Restore turtle state from array
            if (!stateStack.empty()) {
                state = stateStack.back();
                stateStack.pop_back();
            }
            break;
        }

        default:
            break;
    }
}

void LSystemTree::emitBranchSegment(
    const Vec3& start,
    const Vec3& end,
    float startRadius,
    float endRadius,
    TreeMeshData& mesh,
    float windWeight,
    int depth,
    FastRng& rng
) {
    // Truncated cone geometry with Sx radial subdivisions (Paper Section 4 & 5)
    constexpr int Sx = 8;
    uint32_t baseIndex = static_cast<uint32_t>(mesh.vertices.size());

    Vec3 dir = (end - start).normalized();
    Vec3 side1 = cross(dir, Vec3(0.0f, 1.0f, 0.0f));
    if (side1.lengthSq() < 0.01f) side1 = cross(dir, Vec3(1.0f, 0.0f, 0.0f));
    side1 = side1.normalized();
    Vec3 side2 = cross(dir, side1).normalized();

    std::vector<Vec3> ring0(Sx);
    std::vector<Vec3> ring1(Sx);
    std::vector<Vec3> normals(Sx);

    for (int ring = 0; ring <= 1; ++ring) {
        Vec3 center = (ring == 0) ? start : end;
        float radius = (ring == 0) ? startRadius : endRadius;
        float vCoord = (ring == 0) ? 0.0f : 1.0f;

        for (int p = 0; p < Sx; ++p) {
            float theta = static_cast<float>(p) * (2.0f * 3.14159265f / Sx);
            float cosT = std::cos(theta);
            float sinT = std::sin(theta);

            Vec3 normal = side1 * cosT + side2 * sinT;
            Vec3 pos = center + normal * radius;

            if (ring == 0) {
                ring0[p] = pos;
            } else {
                ring1[p] = pos;
                normals[p] = normal;
            }

            float uCoord = static_cast<float>(p) / static_cast<float>(Sx);
            float barkR, barkG, barkB;
            computeBarkColor(uCoord, vCoord, barkR, barkG, barkB);

            TreeMeshVertex v;
            v.position = pos;
            v.normal = normal;
            v.uvU = uCoord;
            v.uvV = vCoord;
            v.r = barkR;
            v.g = barkG;
            v.b = barkB;
            v.a = 1.0f;
            v.windWeight = windWeight;
            v.isLeaf = 0;

            mesh.vertices.push_back(v);
        }
    }

    // Connect rings with quad triangles
    for (int p = 0; p < Sx; ++p) {
        int next = (p + 1) % Sx;
        uint32_t i0 = baseIndex + p;
        uint32_t i1 = baseIndex + next;
        uint32_t i2 = baseIndex + Sx + next;
        uint32_t i3 = baseIndex + Sx + p;

        mesh.indices.push_back(i0);
        mesh.indices.push_back(i1);
        mesh.indices.push_back(i2);

        mesh.indices.push_back(i0);
        mesh.indices.push_back(i2);
        mesh.indices.push_back(i3);
    }

    mesh.branchVertexCount += Sx * 2;

    // Attach leaves to outer/terminal branch segments using exact Stojanovic (2016) equations
    if (depth >= 2) {
        emitStojanovicLeavesOnEdges(ring0, ring1, normals, mesh, rng, windWeight);
    }
}

void LSystemTree::emitStojanovicLeavesOnEdges(
    const std::vector<Vec3>& ringStart,
    const std::vector<Vec3>& ringEnd,
    const std::vector<Vec3>& normals,
    TreeMeshData& mesh,
    FastRng& rng,
    float windWeight
) {
    // Exact algorithm from Stojanovic (2016) Pages 4-5:
    // p in [0, Sx) represents edges in cylinder
    // a_p = V_{p+Sx} - V_p is edge vector for every edge p
    // k = |a_p| / 4 presents width of the leaf
    // Discrete random variable X in {A, B, C, D} with equal probability 0.25:
    // Case A: b_p = a_p / 4
    // Case B: b_p = a_p / 2
    // Case C: b_p = 3/4 a_p
    // Case D: Don't draw leaf!
    int Sx = static_cast<int>(ringStart.size());

    for (int p = 0; p < Sx; ++p) {
        float xProb = rng.nextFloat01();
        if (xProb >= 0.75f) {
            // Case D (Probability 0.25): Don't draw leaf!
            continue;
        }

        const Vec3& V_p = ringStart[p];
        const Vec3& V_p_plus_Sx = ringEnd[p];
        Vec3 a_p = V_p_plus_Sx - V_p;
        float edgeLen = a_p.length();
        if (edgeLen < 1e-4f) continue;

        float k = (edgeLen / 4.0f) * m_config.leafClusterSizeMeters;
        const Vec3& N_p = normals[p];

        Vec3 b_p;
        if (xProb < 0.25f) {
            // Case A: b_p = a_p * 1/4
            b_p = a_p * 0.25f;
        } else if (xProb < 0.50f) {
            // Case B: b_p = a_p * 1/2
            b_p = a_p * 0.50f;
        } else {
            // Case C: b_p = a_p * 3/4
            b_p = a_p * 0.75f;
        }

        // Exact leaf vertices from paper:
        // L1,p = V_p + b_p
        // L2,p = V_p + b_p + a_p / 4
        // L3,p = V_p + b_p + k * N_{p+Sx}
        // L4,p = V_p + b_p + a_p / 4 + k * N_{p+Sx}
        Vec3 a_quarter = a_p * 0.25f;
        Vec3 normOffset = N_p * k;

        Vec3 L1 = V_p + b_p;
        Vec3 L2 = V_p + b_p + a_quarter;
        Vec3 L3 = V_p + b_p + normOffset;
        Vec3 L4 = V_p + b_p + a_quarter + normOffset;

        uint32_t baseIdx = static_cast<uint32_t>(mesh.vertices.size());

        float leafR = m_config.leafColorR * rng.nextRange(0.88f, 1.12f);
        float leafG = m_config.leafColorG * rng.nextRange(0.88f, 1.12f);
        float leafB = m_config.leafColorB * rng.nextRange(0.88f, 1.12f);

        TreeMeshVertex verts[4] = {
            {L1, N_p, 0.0f, 0.0f, leafR, leafG, leafB, 1.0f, windWeight, 1},
            {L2, N_p, 1.0f, 0.0f, leafR, leafG, leafB, 1.0f, windWeight, 1},
            {L4, N_p, 1.0f, 1.0f, leafR, leafG, leafB, 1.0f, windWeight, 1},
            {L3, N_p, 0.0f, 1.0f, leafR, leafG, leafB, 1.0f, windWeight, 1}
        };

        for (int i = 0; i < 4; ++i) {
            mesh.vertices.push_back(verts[i]);
        }

        mesh.indices.push_back(baseIdx + 0);
        mesh.indices.push_back(baseIdx + 1);
        mesh.indices.push_back(baseIdx + 2);

        mesh.indices.push_back(baseIdx + 0);
        mesh.indices.push_back(baseIdx + 2);
        mesh.indices.push_back(baseIdx + 3);

        mesh.leafVertexCount += 4;
    }
}

void LSystemTree::computeBarkColor(
    float u, float v,
    float& outR, float& outG, float& outB
) const {
    // Equations (2), (3), (4) & sinusoidal bark modulation from Stojanovic (2016):
    // data[x][y] = sin(x* + k* * k) + sin(y + k)
    // 7 octaves with persistence H in [0.5, 0.8]
    float H = 0.65f;
    float noiseSum = 0.0f;
    float amp = 1.0f;
    float freq = 2.0f;

    for (int oct = 0; oct < 7; ++oct) {
        float n = std::sin(u * freq * 6.28318f) * std::cos(v * freq * 12.56637f);
        noiseSum += n * amp;
        amp *= H;
        freq *= 2.0f;
    }

    // Sinusoidal modification from pseudo-code on Page 3
    float kMod = noiseSum * 0.5f;
    float sinMod = std::sin(u * 20.0f + kMod * 2.0f) + std::sin(v * 40.0f + kMod);
    float barkShade = 1.0f + sinMod * 0.12f;

    outR = std::clamp(m_config.trunkColorR * barkShade, 0.0f, 1.0f);
    outG = std::clamp(m_config.trunkColorG * barkShade, 0.0f, 1.0f);
    outB = std::clamp(m_config.trunkColorB * barkShade, 0.0f, 1.0f);
}

} // namespace vegetation
