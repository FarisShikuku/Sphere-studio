// ============================================================================
// CactusGeometryGenerator.cpp - Native C++17 Procedural Desert Cactus Geometry
// ============================================================================

#include "CactusGeometryGenerator.h"
#include <cmath>
#include <algorithm>

namespace vegetation {

static constexpr float PI_F = 3.14159265358979323846f;

CactusGeometryGenerator::CactusGeometryGenerator(const BushSynthesisParams& params)
    : m_params(params) {}

DesertCactusSceneMesh CactusGeometryGenerator::generateSceneMesh(uint64_t seed) {
    DesertCactusSceneMesh mesh;
    mesh.aabbMin = Vec3(-m_params.radiusMeters, 0.0f, -m_params.radiusMeters);
    mesh.aabbMax = Vec3(m_params.radiusMeters, m_params.heightMeters, m_params.radiusMeters);

    switch (m_params.archetype) {
        case BushArchetype::SAGUARO_CACTUS:
            mesh.speciesName = "Carnegiea gigantea";
            generateSaguaro(mesh, seed);
            break;
        case BushArchetype::PRICKLY_PEAR_CACTUS:
            mesh.speciesName = "Opuntia ficus-indica";
            generatePricklyPear(mesh, seed);
            break;
        case BushArchetype::GOLDEN_BARREL_CACTUS:
            mesh.speciesName = "Echinocactus grusonii";
            generateBarrelCactus(mesh, true, seed);
            break;
        case BushArchetype::ORGAN_PIPE_CACTUS:
            mesh.speciesName = "Stenocereus thurberi";
            generateOrganPipe(mesh, seed);
            break;
        case BushArchetype::JUMPING_CHOLLA:
            mesh.speciesName = "Cylindropuntia fulgida";
            generateCholla(mesh, seed);
            break;
        case BushArchetype::JOSHUA_TREE:
            mesh.speciesName = "Yucca brevifolia";
            generateJoshuaTree(mesh, seed);
            break;
        case BushArchetype::OCOTILLO_DESERT_CORAL:
            mesh.speciesName = "Fouquieria splendens";
            generateOcotillo(mesh, seed);
            break;
        case BushArchetype::FISHHOOK_BARREL_CACTUS:
            mesh.speciesName = "Ferocactus wislizeni";
            generateBarrelCactus(mesh, false, seed);
            break;
        case BushArchetype::ALOE_TORCH_SUCCULENT:
            mesh.speciesName = "Aloe arborescens";
            generateAloeTorch(mesh, seed);
            break;
        case BushArchetype::ZEBRA_HAWORTHIA:
            mesh.speciesName = "Haworthiopsis fasciata";
            generateZebraHaworthia(mesh, seed);
            break;
        default:
            generateSaguaro(mesh, seed);
            break;
    }

    return mesh;
}

void CactusGeometryGenerator::generateSaguaro(DesertCactusSceneMesh& mesh, uint64_t seed) {
    const float trunkH = m_params.heightMeters;
    const float trunkR = m_params.radiusMeters * 0.22f;
    const int ribCount = 12;
    mesh.ribCount = ribCount;
    mesh.armCount = 3;

    Vec3 baseColor(m_params.colorR, m_params.colorG, m_params.colorB);
    Vec3 flowerColor(0.98f, 0.98f, 0.82f);

    // Fluted central column trunk
    const int verticalSteps = 16;
    for (int y = 0; y <= verticalSteps; ++y) {
        float vNorm = static_cast<float>(y) / verticalSteps;
        float currY = vNorm * trunkH;
        float currR = (vNorm > 0.85f) ? trunkR * (1.0f - (vNorm - 0.85f) / 0.15f * 0.35f) : trunkR;

        for (int r = 0; r < ribCount; ++r) {
            float theta = (static_cast<float>(r) / ribCount) * 2.0f * PI_F;
            float pleat = (r % 2 == 0) ? 1.0f : 0.84f; // Fluted accordion depth
            float rx = std::cos(theta) * currR * pleat;
            float rz = std::sin(theta) * currR * pleat;

            Vec3 pos(rx, currY, rz);
            Vec3 norm(std::cos(theta), 0.0f, std::sin(theta));
            mesh.vertices.push_back({pos, norm, Vec2(static_cast<float>(r) / ribCount, vNorm), baseColor, 0.4f});
        }
    }

    // Connect trunk indices
    for (int y = 0; y < verticalSteps; ++y) {
        for (int r = 0; r < ribCount; ++r) {
            int nextR = (r + 1) % ribCount;
            uint32_t i0 = y * ribCount + r;
            uint32_t i1 = y * ribCount + nextR;
            uint32_t i2 = (y + 1) * ribCount + r;
            uint32_t i3 = (y + 1) * ribCount + nextR;

            mesh.indices.push_back(i0); mesh.indices.push_back(i2); mesh.indices.push_back(i1);
            mesh.indices.push_back(i1); mesh.indices.push_back(i2); mesh.indices.push_back(i3);
        }
    }

    // Apex crown inflorescence
    mesh.vertices.push_back({Vec3(0.0f, trunkH + 0.05f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec2(0.5f, 1.0f), flowerColor, 1.0f});
}

void CactusGeometryGenerator::generatePricklyPear(DesertCactusSceneMesh& mesh, uint64_t seed) {
    Vec3 padColor(m_params.colorR, m_params.colorG, m_params.colorB);
    Vec3 tunaColor(0.85f, 0.10f, 0.38f); // Magenta prickly pear fruit

    // Generates flat cladode pads in branching hierarchical chain
    const int padCount = 10;
    for (int p = 0; p < padCount; ++p) {
        float padAngle = p * 0.65f;
        float padDist = (p * 0.12f) * m_params.radiusMeters;
        float padBaseY = (p * 0.14f) * m_params.heightMeters;

        Vec3 center(std::cos(padAngle) * padDist, padBaseY, std::sin(padAngle) * padDist);
        mesh.vertices.push_back({center, Vec3(0.0f, 0.0f, 1.0f), Vec2(0.5f, 0.5f), padColor, 0.6f});
    }
}

void CactusGeometryGenerator::generateBarrelCactus(DesertCactusSceneMesh& mesh, bool isGolden, uint64_t seed) {
    const float radius = m_params.radiusMeters * 0.55f;
    const float height = isGolden ? m_params.heightMeters * 0.85f : m_params.heightMeters * 1.15f;
    const int ribCount = 16;
    mesh.ribCount = ribCount;

    Vec3 bodyColor(m_params.colorR, m_params.colorG, m_params.colorB);
    Vec3 spineColor = isGolden ? Vec3(1.0f, 0.85f, 0.25f) : Vec3(0.95f, 0.40f, 0.20f);

    const int rings = 12;
    for (int ring = 0; ring <= rings; ++ring) {
        float phi = (static_cast<float>(ring) / rings) * (PI_F * 0.5f);
        float ringR = radius * std::sin(phi);
        float ringY = height * std::cos(phi);

        for (int r = 0; r < ribCount; ++r) {
            float theta = (static_cast<float>(r) / ribCount) * 2.0f * PI_F;
            float pleat = (r % 2 == 0) ? 1.05f : 0.95f;
            Vec3 pos(std::cos(theta) * ringR * pleat, height - ringY, std::sin(theta) * ringR * pleat);
            Vec3 norm = Vec3(pos.x, 0.0f, pos.z).normalized();
            mesh.vertices.push_back({pos, norm, Vec2(static_cast<float>(r) / ribCount, static_cast<float>(ring) / rings), bodyColor, 0.8f});
        }
    }
}

void CactusGeometryGenerator::generateOrganPipe(DesertCactusSceneMesh& mesh, uint64_t seed) {
    const int pipeCount = 14;
    Vec3 pipeColor(m_params.colorR, m_params.colorG, m_params.colorB);

    for (int p = 0; p < pipeCount; ++p) {
        float angle = (static_cast<float>(p) / pipeCount) * 2.0f * PI_F;
        float spread = (0.25f + (p % 4) * 0.05f) * m_params.radiusMeters;
        float pipeH = (0.75f + (p % 6) * 0.08f) * m_params.heightMeters;

        Vec3 bot(std::cos(angle) * spread, 0.0f, std::sin(angle) * spread);
        Vec3 top(std::cos(angle) * spread * 1.1f, pipeH, std::sin(angle) * spread * 1.1f);

        mesh.vertices.push_back({bot, Vec3(std::cos(angle), 0.0f, std::sin(angle)), Vec2(0.0f, 0.0f), pipeColor, 0.3f});
        mesh.vertices.push_back({top, Vec3(std::cos(angle), 0.0f, std::sin(angle)), Vec2(0.0f, 1.0f), pipeColor, 0.5f});
    }
}

void CactusGeometryGenerator::generateCholla(DesertCactusSceneMesh& mesh, uint64_t seed) {
    Vec3 stemColor(m_params.colorR, m_params.colorG, m_params.colorB);
    Vec3 silverSpineColor(0.92f, 0.94f, 0.96f);

    const int jointCount = 16;
    for (int j = 0; j < jointCount; ++j) {
        float a = (static_cast<float>(j) / jointCount) * 2.0f * PI_F;
        float r = (0.4f + (j % 3) * 0.15f) * m_params.radiusMeters;
        float y = (0.3f + (j % 5) * 0.12f) * m_params.heightMeters;

        Vec3 jointPos(std::cos(a) * r, y, std::sin(a) * r);
        mesh.vertices.push_back({jointPos, Vec3(0.0f, 1.0f, 0.0f), Vec2(0.5f, 0.5f), stemColor, 0.9f});
    }
}

void CactusGeometryGenerator::generateJoshuaTree(DesertCactusSceneMesh& mesh, uint64_t seed) {
    Vec3 barkColor(0.36f, 0.25f, 0.20f);
    Vec3 bayonetColor(0.20f, 0.42f, 0.12f);
    Vec3 bloomColor(0.95f, 0.95f, 0.80f);

    // Woody trunk
    Vec3 trunkBot(0.0f, 0.0f, 0.0f);
    Vec3 trunkTop(0.0f, m_params.heightMeters * 0.45f, 0.0f);
    mesh.vertices.push_back({trunkBot, Vec3(0.0f, 0.0f, 1.0f), Vec2(0.0f, 0.0f), barkColor, 0.0f});
    mesh.vertices.push_back({trunkTop, Vec3(0.0f, 0.0f, 1.0f), Vec2(0.0f, 1.0f), barkColor, 0.0f});

    // Pompom bayonet clusters at limb ends
    const int limbCount = 4;
    for (int l = 0; l < limbCount; ++l) {
        float la = (static_cast<float>(l) / limbCount) * 2.0f * PI_F + 0.3f;
        float lx = std::cos(la) * m_params.radiusMeters * 0.55f;
        float lz = std::sin(la) * m_params.radiusMeters * 0.55f;
        float ly = m_params.heightMeters * (0.65f + (l % 2) * 0.15f);

        Vec3 headPos(lx, ly, lz);
        mesh.vertices.push_back({headPos, Vec3(0.0f, 1.0f, 0.0f), Vec2(0.5f, 0.5f), bayonetColor, 0.7f});
    }
}

void CactusGeometryGenerator::generateOcotillo(DesertCactusSceneMesh& mesh, uint64_t seed) {
    Vec3 caneColor(0.31f, 0.20f, 0.18f);
    Vec3 flameSpikeColor(1.0f, 0.24f, 0.0f); // Fiery vermilion flower

    const int caneCount = 18;
    for (int c = 0; c < caneCount; ++c) {
        float ca = (static_cast<float>(c) / caneCount) * 2.0f * PI_F;
        float spread = (0.75f + (c % 4) * 0.08f) * m_params.radiusMeters;
        float caneH = (0.85f + (c % 5) * 0.06f) * m_params.heightMeters;

        Vec3 tip(std::cos(ca) * spread, caneH, std::sin(ca) * spread);
        mesh.vertices.push_back({Vec3(std::cos(ca) * 0.05f, 0.0f, std::sin(ca) * 0.05f), Vec3(0.0f, 1.0f, 0.0f), Vec2(0.0f, 0.0f), caneColor, 0.2f});
        mesh.vertices.push_back({tip, Vec3(0.0f, 1.0f, 0.0f), Vec2(0.0f, 1.0f), flameSpikeColor, 1.0f});
    }
}

void CactusGeometryGenerator::generateAloeTorch(DesertCactusSceneMesh& mesh, uint64_t seed) {
    Vec3 leafColor(0.15f, 0.65f, 0.55f); // Glaucous sea-green
    Vec3 torchColor(1.0f, 0.22f, 0.0f); // Scarlet orange torch

    const int headCount = 4;
    for (int h = 0; h < headCount; ++h) {
        float ha = (static_cast<float>(h) / headCount) * 2.0f * PI_F;
        float hs = (h == 0) ? 0.0f : 0.4f * m_params.radiusMeters;

        Vec3 headCenter(std::cos(ha) * hs, 0.35f * m_params.heightMeters, std::sin(ha) * hs);
        Vec3 torchApex(std::cos(ha) * hs, m_params.heightMeters * 0.95f, std::sin(ha) * hs);

        mesh.vertices.push_back({headCenter, Vec3(0.0f, 1.0f, 0.0f), Vec2(0.5f, 0.0f), leafColor, 0.5f});
        mesh.vertices.push_back({torchApex, Vec3(0.0f, 1.0f, 0.0f), Vec2(0.5f, 1.0f), torchColor, 1.0f});
    }
}

void CactusGeometryGenerator::generateZebraHaworthia(DesertCactusSceneMesh& mesh, uint64_t seed) {
    Vec3 leafColor(0.11f, 0.36f, 0.12f);
    Vec3 bandColor(0.92f, 0.95f, 0.96f); // Porcelain white raised bands

    const int leafCount = 18;
    for (int l = 0; l < leafCount; ++l) {
        float phi = l * 2.39996f;
        float frac = static_cast<float>(l + 1) / leafCount;
        float r = frac * m_params.radiusMeters * 0.75f;
        float y = 0.02f + (1.0f - frac * 0.5f) * m_params.heightMeters * 0.65f;

        Vec3 tip(std::cos(phi) * r, y, std::sin(phi) * r);
        mesh.vertices.push_back({Vec3(0.0f, 0.02f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec2(0.0f, 0.0f), leafColor, 0.1f});
        mesh.vertices.push_back({tip, Vec3(0.0f, 1.0f, 0.0f), Vec2(1.0f, 1.0f), bandColor, 0.8f});
    }
}

} // namespace vegetation
