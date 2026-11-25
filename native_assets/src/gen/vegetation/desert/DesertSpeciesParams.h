#pragma once
#include <string>
#include <cstdint>

namespace vegetation {
namespace desert {

enum class DesertCactusArchetype {
    GIANT_SAGUARO = 0,
    PRICKLY_PEAR,
    GOLDEN_BARREL,
    ORGAN_PIPE,
    JUMPING_CHOLLA,
    JOSHUA_TREE,
    OCOTILLO,
    FISHHOOK_BARREL,
    KRANTZ_ALOE,
    ZEBRA_HAWORTHIA
};

struct DesertSpeciesParams {
    DesertCactusArchetype archetype{DesertCactusArchetype::GIANT_SAGUARO};
    std::string displayName{"Giant Saguaro"};
    std::string commonName{"Giant Saguaro"};
    std::string botanicalName{"Carnegiea gigantea"};
    std::string nativeBiome{"Sonoran Desert"};
    std::string biome{"Sonoran Desert"};
    float heightMeters{4.5f};
    float baseRadiusMeters{0.32f};
    int stemRibCount{24};
    int areoleSpineDensity{18};
    float pleatingDepth{0.045f};
    bool showSpines{true};
    bool showBlooms{true};
    float windFlexibility{0.04f};
    float trunkGreenR{0.22f};
    float trunkGreenG{0.48f};
    float trunkGreenB{0.28f};
};

inline DesertSpeciesParams getPredefinedDesertParams(DesertCactusArchetype archetype) {
    DesertSpeciesParams p;
    p.archetype = archetype;
    switch (archetype) {
        case DesertCactusArchetype::GIANT_SAGUARO:
            p.displayName = "Giant Saguaro";
            p.botanicalName = "Carnegiea gigantea";
            p.nativeBiome = "Sonoran Desert";
            p.heightMeters = 5.2f;
            p.baseRadiusMeters = 0.35f;
            p.stemRibCount = 24;
            p.areoleSpineDensity = 20;
            p.pleatingDepth = 0.05f;
            p.trunkGreenR = 0.22f; p.trunkGreenG = 0.46f; p.trunkGreenB = 0.26f;
            break;
        case DesertCactusArchetype::PRICKLY_PEAR:
            p.displayName = "Prickly Pear";
            p.botanicalName = "Opuntia ficus-indica";
            p.nativeBiome = "Chihuahuan / Sonoran";
            p.heightMeters = 1.8f;
            p.baseRadiusMeters = 0.40f;
            p.stemRibCount = 8;
            p.areoleSpineDensity = 12;
            p.pleatingDepth = 0.02f;
            p.trunkGreenR = 0.26f; p.trunkGreenG = 0.52f; p.trunkGreenB = 0.22f;
            break;
        case DesertCactusArchetype::GOLDEN_BARREL:
            p.displayName = "Golden Barrel";
            p.botanicalName = "Echinocactus grusonii";
            p.nativeBiome = "Central Mexican Plateau";
            p.heightMeters = 0.85f;
            p.baseRadiusMeters = 0.45f;
            p.stemRibCount = 35;
            p.areoleSpineDensity = 28;
            p.pleatingDepth = 0.07f;
            p.trunkGreenR = 0.32f; p.trunkGreenG = 0.58f; p.trunkGreenB = 0.20f;
            break;
        case DesertCactusArchetype::ORGAN_PIPE:
            p.displayName = "Organ Pipe Cactus";
            p.botanicalName = "Stenocereus thurberi";
            p.nativeBiome = "Sonoran Desert";
            p.heightMeters = 3.8f;
            p.baseRadiusMeters = 0.16f;
            p.stemRibCount = 18;
            p.areoleSpineDensity = 16;
            p.pleatingDepth = 0.04f;
            p.trunkGreenR = 0.18f; p.trunkGreenG = 0.42f; p.trunkGreenB = 0.30f;
            break;
        case DesertCactusArchetype::JUMPING_CHOLLA:
            p.displayName = "Jumping Cholla";
            p.botanicalName = "Cylindropuntia fulgida";
            p.nativeBiome = "Sonoran Desert";
            p.heightMeters = 2.1f;
            p.baseRadiusMeters = 0.22f;
            p.stemRibCount = 12;
            p.areoleSpineDensity = 25;
            p.pleatingDepth = 0.03f;
            p.trunkGreenR = 0.30f; p.trunkGreenG = 0.48f; p.trunkGreenB = 0.26f;
            break;
        case DesertCactusArchetype::JOSHUA_TREE:
            p.displayName = "Joshua Tree";
            p.botanicalName = "Yucca brevifolia";
            p.nativeBiome = "Mojave Desert";
            p.heightMeters = 4.2f;
            p.baseRadiusMeters = 0.38f;
            p.stemRibCount = 16;
            p.areoleSpineDensity = 14;
            p.pleatingDepth = 0.035f;
            p.trunkGreenR = 0.20f; p.trunkGreenG = 0.40f; p.trunkGreenB = 0.24f;
            break;
        case DesertCactusArchetype::OCOTILLO:
            p.displayName = "Ocotillo";
            p.botanicalName = "Fouquieria splendens";
            p.nativeBiome = "Sonoran & Chihuahuan";
            p.heightMeters = 3.6f;
            p.baseRadiusMeters = 0.10f;
            p.stemRibCount = 10;
            p.areoleSpineDensity = 22;
            p.pleatingDepth = 0.025f;
            p.trunkGreenR = 0.25f; p.trunkGreenG = 0.45f; p.trunkGreenB = 0.20f;
            break;
        case DesertCactusArchetype::FISHHOOK_BARREL:
            p.displayName = "Fishhook Barrel";
            p.botanicalName = "Ferocactus wislizeni";
            p.nativeBiome = "Sonoran / Chihuahuan";
            p.heightMeters = 1.2f;
            p.baseRadiusMeters = 0.40f;
            p.stemRibCount = 26;
            p.areoleSpineDensity = 24;
            p.pleatingDepth = 0.06f;
            p.trunkGreenR = 0.24f; p.trunkGreenG = 0.46f; p.trunkGreenB = 0.26f;
            break;
        case DesertCactusArchetype::KRANTZ_ALOE:
            p.displayName = "Krantz Aloe";
            p.botanicalName = "Aloe arborescens";
            p.nativeBiome = "Semi-Arid Karoo";
            p.heightMeters = 2.0f;
            p.baseRadiusMeters = 0.30f;
            p.stemRibCount = 14;
            p.areoleSpineDensity = 10;
            p.pleatingDepth = 0.03f;
            p.trunkGreenR = 0.20f; p.trunkGreenG = 0.50f; p.trunkGreenB = 0.32f;
            break;
        case DesertCactusArchetype::ZEBRA_HAWORTHIA:
            p.displayName = "Zebra Haworthia";
            p.botanicalName = "Haworthiopsis fasciata";
            p.nativeBiome = "Succulent Karoo";
            p.heightMeters = 0.35f;
            p.baseRadiusMeters = 0.18f;
            p.stemRibCount = 12;
            p.areoleSpineDensity = 8;
            p.pleatingDepth = 0.02f;
            p.trunkGreenR = 0.15f; p.trunkGreenG = 0.38f; p.trunkGreenB = 0.22f;
            break;
    }
    p.commonName = p.displayName;
    p.biome = p.nativeBiome;
    return p;
}

} // namespace desert
} // namespace vegetation
