#pragma once
// ============================================================================
// LSystemTree.h - 3D Turtle Geometry Synthesizer & Branch Mesh Builder in C++17
// Implements Nemanja Stojanovic (2016): "A method for generating stochastic 3D
// tree models with Python in Autodesk Maya" (J. Graphic Eng. & Design 7(2):25-30)
// ============================================================================

#include <vector>
#include <string>
#include "TreeSpecies.h"
#include "TreeGrammar.h"
#include "../common/VegetationMath.h"

namespace vegetation {

// Orthonormal Turtle Frame: H x L = U (Equation 1 of paper)
struct TurtleState {
    Vec3 position{0.0f, 0.0f, 0.0f};
    Vec3 heading{0.0f, 1.0f, 0.0f}; // H
    Vec3 left{-1.0f, 0.0f, 0.0f};    // L
    Vec3 up{0.0f, 0.0f, 1.0f};       // U
    float currentRadius{0.45f};
    int depth{0};
    int iterationStep{0};
};

struct TreeMeshVertex {
    Vec3 position;
    Vec3 normal;
    float uvU{0.0f};
    float uvV{0.0f};
    float r{1.0f}, g{1.0f}, b{1.0f}, a{1.0f};
    float windWeight{0.0f};
    uint32_t isLeaf{0};
};

struct TreeMeshData {
    std::vector<TreeMeshVertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t branchVertexCount{0};
    uint32_t leafVertexCount{0};
    Vec3 aabbMin{0.0f, 0.0f, 0.0f};
    Vec3 aabbMax{0.0f, 0.0f, 0.0f};
};

class LSystemTree {
public:
    // Leonardo da Vinci postulate constant: wr = 1/sqrt(2) = 0.707
    static constexpr float LEONARDO_DA_VINCI_WR = 0.70710678f;

    explicit LSystemTree(const TreeSpeciesConfig& config);

    TreeMeshData generateTreeMesh(uint64_t seed);

private:
    void interpretSymbol(
        char symbol,
        TurtleState& state,
        std::vector<TurtleState>& stateStack,
        TreeMeshData& mesh,
        FastRng& rng,
        float branchLength,
        int totalIterations
    );

    // Truncated cone branch generation with radial subdivisions Sx
    void emitBranchSegment(
        const Vec3& start,
        const Vec3& end,
        float startRadius,
        float endRadius,
        TreeMeshData& mesh,
        float windWeight,
        int depth,
        FastRng& rng
    );

    // Stojanovic (2016) Pages 4-5: Leaf attachment on cylinder edges
    // governed by discrete random variable X in {A, B, C, D} with equal 0.25 probability
    void emitStojanovicLeavesOnEdges(
        const std::vector<Vec3>& ringStart,
        const std::vector<Vec3>& ringEnd,
        const std::vector<Vec3>& normals,
        TreeMeshData& mesh,
        FastRng& rng,
        float windWeight
    );

    // Procedural Perlin noise bark color modulation (Eq 2, 3, 4 + Sinusoid)
    void computeBarkColor(
        float u, float v,
        float& outR, float& outG, float& outB
    ) const;

    TreeSpeciesConfig m_config;
};

} // namespace vegetation
