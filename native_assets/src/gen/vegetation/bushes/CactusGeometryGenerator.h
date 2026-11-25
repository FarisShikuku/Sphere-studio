#pragma once
// ============================================================================
// CactusGeometryGenerator.h - Native C++17 Procedural 3D Cactus & Desert Scene Generator
// ============================================================================

#include "BushParams.h"
#include "../common/VegetationMath.h"
#include <vector>
#include <cstdint>

namespace vegetation {

struct DesertCactusVertex {
    Vec3 position;
    Vec3 normal;
    Vec2 uv;
    Vec3 color;
    float spineIntensity{0.0f};
};

struct DesertCactusSceneMesh {
    std::vector<DesertCactusVertex> vertices;
    std::vector<uint32_t> indices;
    Vec3 aabbMin;
    Vec3 aabbMax;
    std::string speciesName;
    uint32_t ribCount{0};
    uint32_t armCount{0};
};

/**
 * Procedural 3D Scene & Geometry Generator for Desert & Semi-Arid Cacti in C++17:
 * 1. Saguaro Cactus (Carnegiea gigantea) - Fluted ribs, candelabra arms, apex nocturnal flowers
 * 2. Prickly Pear (Opuntia ficus-indica) - Segmented cladode pads, glochids, ruby-red tunas
 * 3. Golden Barrel (Echinocactus grusonii) - Globose ribbed sphere with radial golden spines
 * 4. Organ Pipe Cactus (Stenocereus thurberi) - Multi-stem columnar vertical pipes
 * 5. Jumping Cholla (Cylindropuntia fulgida) - Segmented cylindrical joints with silver spine halos
 * 6. Joshua Tree (Yucca brevifolia) - Angular woody limbs with bayonet pompom spheres
 * 7. Ocotillo (Fouquieria splendens) - Inverted cone vase of thorny whip canes with flame spikes
 * 8. Fishhook Barrel (Ferocactus wislizeni) - Cylindrical barrel with recurved hook spines
 * 9. African Candelabra Aloe (Aloe arborescens) - Recurved toothed succulent rosettes & floral torches
 * 10. Zebra Succulent (Haworthiopsis fasciata) - Architectural rosette with white tubercle bands
 */
class CactusGeometryGenerator {
public:
    explicit CactusGeometryGenerator(const BushSynthesisParams& params);

    DesertCactusSceneMesh generateSceneMesh(uint64_t seed = 42);

private:
    void generateSaguaro(DesertCactusSceneMesh& mesh, uint64_t seed);
    void generatePricklyPear(DesertCactusSceneMesh& mesh, uint64_t seed);
    void generateBarrelCactus(DesertCactusSceneMesh& mesh, bool isGolden, uint64_t seed);
    void generateOrganPipe(DesertCactusSceneMesh& mesh, uint64_t seed);
    void generateCholla(DesertCactusSceneMesh& mesh, uint64_t seed);
    void generateJoshuaTree(DesertCactusSceneMesh& mesh, uint64_t seed);
    void generateOcotillo(DesertCactusSceneMesh& mesh, uint64_t seed);
    void generateAloeTorch(DesertCactusSceneMesh& mesh, uint64_t seed);
    void generateZebraHaworthia(DesertCactusSceneMesh& mesh, uint64_t seed);

    BushSynthesisParams m_params;
};

} // namespace vegetation
