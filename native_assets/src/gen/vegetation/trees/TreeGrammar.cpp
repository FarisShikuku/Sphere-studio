// ============================================================================
// TreeGrammar.cpp - L-System Parser & Stochastic Derivation Engine in C++17
// ============================================================================

#include "TreeGrammar.h"
#include "TreeSpecies.h"
#include <sstream>

namespace vegetation {

void TreeGrammar::addRule(char predecessor, const std::string& successor, float probability) {
    m_rules[predecessor].push_back({predecessor, successor, probability});
}

std::string TreeGrammar::derive(int iterations, FastRng& rng) const {
    std::string current = m_axiom;

    for (int iter = 0; iter < iterations; ++iter) {
        std::string nextGen;
        nextGen.reserve(current.size() * 3);

        for (char ch : current) {
            auto it = m_rules.find(ch);
            if (it == m_rules.end() || it->second.empty()) {
                nextGen.push_back(ch);
                continue;
            }

            const auto& candidateRules = it->second;
            if (candidateRules.size() == 1) {
                nextGen.append(candidateRules[0].successor);
            } else {
                // Stochastic rule selection based on probabilities
                float totalProb = 0.0f;
                for (const auto& r : candidateRules) totalProb += r.probability;

                float pick = rng.nextRange(0.0f, totalProb);
                float accum = 0.0f;
                bool picked = false;
                for (const auto& r : candidateRules) {
                    accum += r.probability;
                    if (pick <= accum) {
                        nextGen.append(r.successor);
                        picked = true;
                        break;
                    }
                }
                if (!picked) {
                    nextGen.append(candidateRules.back().successor);
                }
            }
        }
        current = std::move(nextGen);
        if (current.size() > 50000) break; // Memory safety guard
    }

    return current;
}

TreeGrammar TreeGrammar::createSpeciesGrammar(const std::string& axiomStr, const std::string& rulesStr) {
    TreeGrammar grammar;
    grammar.setAxiom(axiomStr);

    // Format: "A=F[&+B][&-B];B=F[+B][-B]"
    std::stringstream ss(rulesStr);
    std::string ruleToken;
    while (std::getline(ss, ruleToken, ';')) {
        size_t eqPos = ruleToken.find('=');
        if (eqPos != std::string::npos && eqPos > 0) {
            char pred = ruleToken[0];
            std::string succ = ruleToken.substr(eqPos + 1);
            grammar.addRule(pred, succ, 1.0f);
        }
    }
    return grammar;
}

TreeGrammar TreeGrammar::createStojanovicStochasticGrammar() {
    // Exact 6 stochastic productions from Stojanovic (2016) Page 5
    // Axiom: w = F A
    // p1 (3/12): A -> [\&/^FA][\&FA]/&FA
    // p2 (1/12): A -> [\&FA]/&FA
    // p3 (3/12): A -> [\&/^FA][/^FA]\^FA
    // p4 (1/12): A -> [/^FA]\^FA
    // p5 (3/12): A -> [\&/^FA][/^FA]/&FA
    // p6 (1/12): A -> [FA][/^FA]/^FA
    TreeGrammar grammar;
    grammar.setAxiom("FA");

    // Predecessor 'A' with 6 stochastic rules:
    grammar.addRule('A', "[\\&/^FA][\\&FA]/&FA", 3.0f / 12.0f);
    grammar.addRule('A', "[\\&FA]/&FA", 1.0f / 12.0f);
    grammar.addRule('A', "[\\&/^FA][/^FA]\\^FA", 3.0f / 12.0f);
    grammar.addRule('A', "[/^FA]\\^FA", 1.0f / 12.0f);
    grammar.addRule('A', "[\\&/^FA][/^FA]/&FA", 3.0f / 12.0f);
    grammar.addRule('A', "[FA][/^FA]/^FA", 1.0f / 12.0f);

    return grammar;
}

TreeSpeciesConfig getPredefinedTreeSpecies(TreeSpeciesId id) {
    TreeSpeciesConfig cfg;
    cfg.speciesId = id;
    switch (id) {
        case TreeSpeciesId::ENGLISH_OAK:
            cfg.commonName = "English Oak";
            cfg.botanicalName = "Quercus robur";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B][^+B][^-B]FA;B=F[+B][-B]FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 44.0f;
            cfg.treeHeightMeters = 18.5f;
            cfg.trunkRadiusMeters = 0.75f;
            cfg.trunkColorR = 0.38f; cfg.trunkColorG = 0.26f; cfg.trunkColorB = 0.17f;
            cfg.leafColorR = 0.18f; cfg.leafColorG = 0.44f; cfg.leafColorB = 0.14f;
            cfg.branchCount = 48;
            cfg.leafCount = 1650;
            cfg.stemLengthRatio = 0.36f;
            break;
        case TreeSpeciesId::SCOTS_PINE:
            cfg.commonName = "Scots Pine";
            cfg.botanicalName = "Pinus sylvestris";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=[&FL!A]/////[&FL!A]///////[&FL!A];F=FF";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 48.0f;
            cfg.treeHeightMeters = 22.0f;
            cfg.trunkRadiusMeters = 0.48f;
            cfg.trunkColorR = 0.46f; cfg.trunkColorG = 0.28f; cfg.trunkColorB = 0.20f;
            cfg.leafColorR = 0.12f; cfg.leafColorG = 0.32f; cfg.leafColorB = 0.18f;
            cfg.branchCount = 48;
            cfg.leafCount = 1850;
            cfg.stemLengthRatio = 0.28f;
            break;
        case TreeSpeciesId::SILVER_BIRCH:
            cfg.commonName = "Silver Birch";
            cfg.botanicalName = "Betula pendula";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[&B][^B]L";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 32.0f;
            cfg.treeHeightMeters = 16.5f;
            cfg.trunkRadiusMeters = 0.32f;
            cfg.trunkColorR = 0.88f; cfg.trunkColorG = 0.88f; cfg.trunkColorB = 0.85f;
            cfg.leafColorR = 0.35f; cfg.leafColorG = 0.62f; cfg.leafColorB = 0.18f;
            cfg.branchCount = 40;
            cfg.leafCount = 1250;
            cfg.stemLengthRatio = 0.34f;
            break;
        case TreeSpeciesId::DATE_PALM:
            cfg.commonName = "Date Palm";
            cfg.botanicalName = "Phoenix dactylifera";
            cfg.lsystemAxiom = "FFFFFA";
            cfg.lsystemRules = "A=[&L][+&L][-&L][^&L][+^&L][-^&L]";
            cfg.maxRecursionDepth = 2;
            cfg.branchAngleMeanDeg = 45.0f;
            cfg.treeHeightMeters = 18.0f;
            cfg.trunkRadiusMeters = 0.40f;
            cfg.trunkColorR = 0.42f; cfg.trunkColorG = 0.34f; cfg.trunkColorB = 0.25f;
            cfg.leafColorR = 0.28f; cfg.leafColorG = 0.50f; cfg.leafColorB = 0.12f;
            cfg.branchCount = 22;
            cfg.leafCount = 264;
            cfg.stemLengthRatio = 0.75f;
            break;
        case TreeSpeciesId::WEEPING_WILLOW:
            cfg.commonName = "Weeping Willow";
            cfg.botanicalName = "Salix babylonica";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B][^+B][^-B]FA;B=F[+B][-B]L";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 48.0f;
            cfg.treeHeightMeters = 13.5f;
            cfg.trunkRadiusMeters = 0.62f;
            cfg.trunkColorR = 0.35f; cfg.trunkColorG = 0.28f; cfg.trunkColorB = 0.20f;
            cfg.leafColorR = 0.25f; cfg.leafColorG = 0.52f; cfg.leafColorB = 0.16f;
            cfg.windFlexibility = 0.85f;
            cfg.branchCount = 64;
            cfg.leafCount = 1950;
            cfg.stemLengthRatio = 0.42f;
            break;
        case TreeSpeciesId::SAVANNA_ACACIA:
            cfg.commonName = "Savanna Acacia";
            cfg.botanicalName = "Vachellia tortilis";
            cfg.lsystemAxiom = "FFA";
            cfg.lsystemRules = "A=[&FB]////[&FB]////////[&FB];B=F[+L][-L]B";
            cfg.maxRecursionDepth = 3;
            cfg.branchAngleMeanDeg = 82.0f;
            cfg.treeHeightMeters = 11.0f;
            cfg.trunkRadiusMeters = 0.44f;
            cfg.trunkColorR = 0.35f; cfg.trunkColorG = 0.25f; cfg.trunkColorB = 0.18f;
            cfg.leafColorR = 0.38f; cfg.leafColorG = 0.60f; cfg.leafColorB = 0.18f;
            cfg.branchCount = 48;
            cfg.leafCount = 1400;
            cfg.stemLengthRatio = 0.32f;
            break;
        case TreeSpeciesId::GIANT_REDWOOD:
            cfg.commonName = "Coast Redwood";
            cfg.botanicalName = "Sequoia sempervirens";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=[&FL!A]/////[&FL!A]///////[&FL!A];F=FF";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 48.0f;
            cfg.treeHeightMeters = 36.0f;
            cfg.trunkRadiusMeters = 1.40f;
            cfg.trunkColorR = 0.49f; cfg.trunkColorG = 0.24f; cfg.trunkColorB = 0.11f;
            cfg.leafColorR = 0.11f; cfg.leafColorG = 0.31f; cfg.leafColorB = 0.16f;
            cfg.windFlexibility = 0.12f;
            cfg.branchCount = 60;
            cfg.leafCount = 2550;
            cfg.stemLengthRatio = 0.24f;
            break;
        case TreeSpeciesId::SUGAR_MAPLE:
            cfg.commonName = "Sugar Maple";
            cfg.botanicalName = "Acer saccharum";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 40.0f;
            cfg.treeHeightMeters = 19.0f;
            cfg.trunkRadiusMeters = 0.54f;
            cfg.trunkColorR = 0.34f; cfg.trunkColorG = 0.24f; cfg.trunkColorB = 0.16f;
            cfg.leafColorR = 0.46f; cfg.leafColorG = 0.68f; cfg.leafColorB = 0.18f;
            cfg.branchCount = 44;
            cfg.leafCount = 1450;
            cfg.stemLengthRatio = 0.36f;
            break;
        case TreeSpeciesId::NORWAY_SPRUCE:
            cfg.commonName = "Norway Spruce";
            cfg.botanicalName = "Picea abies";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=[&FL!A]////[&FL!A]////[&FL!A];F=FF";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 56.0f;
            cfg.treeHeightMeters = 26.0f;
            cfg.trunkRadiusMeters = 0.52f;
            cfg.trunkColorR = 0.38f; cfg.trunkColorG = 0.28f; cfg.trunkColorB = 0.22f;
            cfg.leafColorR = 0.10f; cfg.leafColorG = 0.38f; cfg.leafColorB = 0.16f;
            cfg.windFlexibility = 0.18f;
            cfg.branchCount = 54;
            cfg.leafCount = 2200;
            cfg.stemLengthRatio = 0.24f;
            break;
        case TreeSpeciesId::GIANT_SEQUOIA:
            cfg.commonName = "Giant Sequoia";
            cfg.botanicalName = "Sequoiadendron giganteum";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=[&FL!A]////[&FL!A]////[&FL!A];F=FF";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 52.0f;
            cfg.treeHeightMeters = 32.0f;
            cfg.trunkRadiusMeters = 1.25f;
            cfg.trunkColorR = 0.54f; cfg.trunkColorG = 0.26f; cfg.trunkColorB = 0.13f;
            cfg.leafColorR = 0.10f; cfg.leafColorG = 0.32f; cfg.leafColorB = 0.23f;
            cfg.windFlexibility = 0.12f;
            cfg.branchCount = 58;
            cfg.leafCount = 2450;
            cfg.stemLengthRatio = 0.24f;
            break;
        case TreeSpeciesId::EUROPEAN_BEECH:
            cfg.commonName = "European Beech";
            cfg.botanicalName = "Fagus sylvatica";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B][^+B][^-B]FA;B=F[+B][-B]FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 38.0f;
            cfg.treeHeightMeters = 22.0f;
            cfg.trunkRadiusMeters = 0.68f;
            cfg.trunkColorR = 0.46f; cfg.trunkColorG = 0.45f; cfg.trunkColorB = 0.44f;
            cfg.leafColorR = 0.21f; cfg.leafColorG = 0.50f; cfg.leafColorB = 0.20f;
            cfg.branchCount = 48;
            cfg.leafCount = 1750;
            cfg.stemLengthRatio = 0.34f;
            break;
        case TreeSpeciesId::GINKGO_BILOBA:
            cfg.commonName = "Ginkgo Biloba";
            cfg.botanicalName = "Ginkgo biloba";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 36.0f;
            cfg.treeHeightMeters = 20.0f;
            cfg.trunkRadiusMeters = 0.50f;
            cfg.trunkColorR = 0.43f; cfg.trunkColorG = 0.37f; cfg.trunkColorB = 0.32f;
            cfg.leafColorR = 0.55f; cfg.leafColorG = 0.67f; cfg.leafColorB = 0.17f;
            cfg.branchCount = 42;
            cfg.leafCount = 1350;
            cfg.stemLengthRatio = 0.36f;
            break;
        case TreeSpeciesId::MEDITERRANEAN_OLIVE:
            cfg.commonName = "Ancient Olive";
            cfg.botanicalName = "Olea europaea";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[&L][^L]L";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 46.0f;
            cfg.treeHeightMeters = 8.5f;
            cfg.trunkRadiusMeters = 0.70f;
            cfg.trunkColorR = 0.42f; cfg.trunkColorG = 0.36f; cfg.trunkColorB = 0.31f;
            cfg.leafColorR = 0.29f; cfg.leafColorG = 0.43f; cfg.leafColorB = 0.32f;
            cfg.branchCount = 42;
            cfg.leafCount = 1350;
            cfg.stemLengthRatio = 0.28f;
            break;
        case TreeSpeciesId::SWEET_CHESTNUT:
            cfg.commonName = "Sweet Chestnut";
            cfg.botanicalName = "Castanea sativa";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B][^+B][^-B]FA;B=F[+B][-B]FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 42.0f;
            cfg.treeHeightMeters = 21.0f;
            cfg.trunkRadiusMeters = 0.82f;
            cfg.trunkColorR = 0.33f; cfg.trunkColorG = 0.23f; cfg.trunkColorB = 0.15f;
            cfg.leafColorR = 0.18f; cfg.leafColorG = 0.43f; cfg.leafColorB = 0.20f;
            cfg.branchCount = 48;
            cfg.leafCount = 1750;
            cfg.stemLengthRatio = 0.36f;
            break;
        case TreeSpeciesId::EUROPEAN_LARCH:
            cfg.commonName = "European Larch";
            cfg.botanicalName = "Larix decidua";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=[&FL!A]////[&FL!A]////[&FL!A];F=FF";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 50.0f;
            cfg.treeHeightMeters = 25.0f;
            cfg.trunkRadiusMeters = 0.48f;
            cfg.trunkColorR = 0.43f; cfg.trunkColorG = 0.27f; cfg.trunkColorB = 0.20f;
            cfg.leafColorR = 0.24f; cfg.leafColorG = 0.52f; cfg.leafColorB = 0.20f;
            cfg.branchCount = 50;
            cfg.leafCount = 1950;
            cfg.stemLengthRatio = 0.26f;
            break;
        case TreeSpeciesId::JAPANESE_CHERRY:
            cfg.commonName = "Japanese Cherry";
            cfg.botanicalName = "Prunus serrulata";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 36.0f;
            cfg.treeHeightMeters = 9.8f;
            cfg.trunkRadiusMeters = 0.38f;
            cfg.trunkColorR = 0.43f; cfg.trunkColorG = 0.21f; cfg.trunkColorB = 0.17f;
            cfg.leafColorR = 0.30f; cfg.leafColorG = 0.53f; cfg.leafColorB = 0.24f;
            cfg.branchCount = 40;
            cfg.leafCount = 1250;
            cfg.stemLengthRatio = 0.32f;
            break;
        case TreeSpeciesId::ATLAS_CEDAR:
            cfg.commonName = "Atlas Cedar";
            cfg.botanicalName = "Cedrus atlantica";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=[&FL!A]////[&FL!A]////[&FL!A];F=FF";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 62.0f;
            cfg.treeHeightMeters = 24.0f;
            cfg.trunkRadiusMeters = 0.70f;
            cfg.trunkColorR = 0.28f; cfg.trunkColorG = 0.27f; cfg.trunkColorB = 0.26f;
            cfg.leafColorR = 0.17f; cfg.leafColorG = 0.36f; cfg.leafColorB = 0.35f;
            cfg.branchCount = 52;
            cfg.leafCount = 2050;
            cfg.stemLengthRatio = 0.26f;
            break;
        case TreeSpeciesId::RED_MAPLE:
            cfg.commonName = "Red Maple";
            cfg.botanicalName = "Acer rubrum";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 38.0f;
            cfg.treeHeightMeters = 17.0f;
            cfg.trunkRadiusMeters = 0.48f;
            cfg.trunkColorR = 0.32f; cfg.trunkColorG = 0.28f; cfg.trunkColorB = 0.27f;
            cfg.leafColorR = 0.62f; cfg.leafColorG = 0.20f; cfg.leafColorB = 0.14f;
            cfg.branchCount = 42;
            cfg.leafCount = 1400;
            cfg.stemLengthRatio = 0.34f;
            break;
        case TreeSpeciesId::AFRICAN_BAOBAB:
            cfg.commonName = "African Baobab (Mbuyu)";
            cfg.botanicalName = "Adansonia digitata";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=[&FB][^FB][+FB][-FB];B=F[&L][^L]B";
            cfg.maxRecursionDepth = 3;
            cfg.branchAngleMeanDeg = 52.0f;
            cfg.treeHeightMeters = 14.0f;
            cfg.trunkRadiusMeters = 2.20f;
            cfg.trunkColorR = 0.38f; cfg.trunkColorG = 0.36f; cfg.trunkColorB = 0.38f;
            cfg.leafColorR = 0.22f; cfg.leafColorG = 0.44f; cfg.leafColorB = 0.18f;
            cfg.branchCount = 38;
            cfg.leafCount = 1150;
            cfg.stemLengthRatio = 0.48f;
            break;
        case TreeSpeciesId::YELLOW_FEVER_TREE:
            cfg.commonName = "Yellow Fever Tree (Olerai)";
            cfg.botanicalName = "Vachellia xanthophloea";
            cfg.lsystemAxiom = "FFA";
            cfg.lsystemRules = "A=[&FB]////[&FB]////////[&FB];B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 58.0f;
            cfg.treeHeightMeters = 20.0f;
            cfg.trunkRadiusMeters = 0.46f;
            cfg.trunkColorR = 0.72f; cfg.trunkColorG = 0.70f; cfg.trunkColorB = 0.22f;
            cfg.leafColorR = 0.32f; cfg.leafColorG = 0.54f; cfg.leafColorB = 0.20f;
            cfg.branchCount = 44;
            cfg.leafCount = 1350;
            cfg.stemLengthRatio = 0.32f;
            break;
        case TreeSpeciesId::MUGUMO_SACRED_FIG:
            cfg.commonName = "Mugumo Sacred Fig";
            cfg.botanicalName = "Ficus thonningii";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B][^+B][^-B]FA;B=F[+B][-B]L";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 42.0f;
            cfg.treeHeightMeters = 18.0f;
            cfg.trunkRadiusMeters = 0.85f;
            cfg.trunkColorR = 0.40f; cfg.trunkColorG = 0.37f; cfg.trunkColorB = 0.34f;
            cfg.leafColorR = 0.11f; cfg.leafColorG = 0.31f; cfg.leafColorB = 0.13f;
            cfg.branchCount = 50;
            cfg.leafCount = 2000;
            cfg.stemLengthRatio = 0.42f;
            break;
        case TreeSpeciesId::WHISTLING_THORN:
            cfg.commonName = "Whistling Thorn (Eluai)";
            cfg.botanicalName = "Vachellia drepanolobium";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=[&FB]///[&FB]//////[&FB];B=F[+L][-L]";
            cfg.maxRecursionDepth = 3;
            cfg.branchAngleMeanDeg = 64.0f;
            cfg.treeHeightMeters = 5.5f;
            cfg.trunkRadiusMeters = 0.22f;
            cfg.trunkColorR = 0.27f; cfg.trunkColorG = 0.22f; cfg.trunkColorB = 0.18f;
            cfg.leafColorR = 0.34f; cfg.leafColorG = 0.51f; cfg.leafColorB = 0.21f;
            cfg.branchCount = 34;
            cfg.leafCount = 1150;
            cfg.stemLengthRatio = 0.26f;
            break;
        case TreeSpeciesId::AFRICAN_BLACKWOOD:
            cfg.commonName = "African Blackwood (Mpingo)";
            cfg.botanicalName = "Dalbergia melanoxylon";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[&L][^L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 48.0f;
            cfg.treeHeightMeters = 8.5f;
            cfg.trunkRadiusMeters = 0.35f;
            cfg.trunkColorR = 0.36f; cfg.trunkColorG = 0.32f; cfg.trunkColorB = 0.28f;
            cfg.leafColorR = 0.24f; cfg.leafColorG = 0.42f; cfg.leafColorB = 0.19f;
            cfg.branchCount = 36;
            cfg.leafCount = 1150;
            cfg.stemLengthRatio = 0.30f;
            break;
        case TreeSpeciesId::NANDI_FLAME:
            cfg.commonName = "Nandi Flame (African Tulip)";
            cfg.botanicalName = "Spathodea campanulata";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B]FA;B=F[+B][-B]L";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 44.0f;
            cfg.treeHeightMeters = 16.5f;
            cfg.trunkRadiusMeters = 0.50f;
            cfg.trunkColorR = 0.47f; cfg.trunkColorG = 0.42f; cfg.trunkColorB = 0.37f;
            cfg.leafColorR = 0.16f; cfg.leafColorG = 0.38f; cfg.leafColorB = 0.15f;
            cfg.branchCount = 44;
            cfg.leafCount = 1550;
            cfg.stemLengthRatio = 0.40f;
            break;
        case TreeSpeciesId::SYCAMORE_FIG:
            cfg.commonName = "Sycamore Fig (Mukuyu)";
            cfg.botanicalName = "Ficus sycomorus";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B][^+B][^-B]FA;B=F[+B][-B]FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 46.0f;
            cfg.treeHeightMeters = 22.0f;
            cfg.trunkRadiusMeters = 1.10f;
            cfg.trunkColorR = 0.55f; cfg.trunkColorG = 0.51f; cfg.trunkColorB = 0.44f;
            cfg.leafColorR = 0.18f; cfg.leafColorG = 0.40f; cfg.leafColorB = 0.16f;
            cfg.branchCount = 52;
            cfg.leafCount = 2100;
            cfg.stemLengthRatio = 0.45f;
            break;
        case TreeSpeciesId::AFRICAN_WILD_OLIVE:
            cfg.commonName = "African Wild Olive (Mutamaiyu)";
            cfg.botanicalName = "Olea europaea subsp. cuspidata";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[&L][^L]L";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 44.0f;
            cfg.treeHeightMeters = 13.0f;
            cfg.trunkRadiusMeters = 0.58f;
            cfg.trunkColorR = 0.28f; cfg.trunkColorG = 0.26f; cfg.trunkColorB = 0.23f;
            cfg.leafColorR = 0.24f; cfg.leafColorG = 0.38f; cfg.leafColorB = 0.27f;
            cfg.branchCount = 44;
            cfg.leafCount = 1450;
            cfg.stemLengthRatio = 0.32f;
            break;
        case TreeSpeciesId::MERU_OAK:
            cfg.commonName = "Meru Oak (East African Teak)";
            cfg.botanicalName = "Vitex keniensis";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=FF[&+B][&-B]FA;B=F[+B][-B]FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 36.0f;
            cfg.treeHeightMeters = 28.0f;
            cfg.trunkRadiusMeters = 0.95f;
            cfg.trunkColorR = 0.43f; cfg.trunkColorG = 0.39f; cfg.trunkColorB = 0.35f;
            cfg.leafColorR = 0.16f; cfg.leafColorG = 0.36f; cfg.leafColorB = 0.15f;
            cfg.branchCount = 48;
            cfg.leafCount = 1850;
            cfg.stemLengthRatio = 0.46f;
            break;
        case TreeSpeciesId::SAUSAGE_TREE:
            cfg.commonName = "Sausage Tree (Muratina)";
            cfg.botanicalName = "Kigelia africana";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B][^+B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 48.0f;
            cfg.treeHeightMeters = 14.0f;
            cfg.trunkRadiusMeters = 0.65f;
            cfg.trunkColorR = 0.41f; cfg.trunkColorG = 0.36f; cfg.trunkColorB = 0.31f;
            cfg.leafColorR = 0.21f; cfg.leafColorG = 0.41f; cfg.leafColorB = 0.17f;
            cfg.branchCount = 44;
            cfg.leafCount = 1500;
            cfg.stemLengthRatio = 0.42f;
            break;
        case TreeSpeciesId::COCONUT_PALM:
            cfg.commonName = "Coconut Palm (Mnazi)";
            cfg.botanicalName = "Cocos nucifera";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=[&FL]////[&FL]////[&FL]////[&FL]////[&FL]";
            cfg.maxRecursionDepth = 2;
            cfg.branchAngleMeanDeg = 52.0f;
            cfg.treeHeightMeters = 18.0f;
            cfg.trunkRadiusMeters = 0.32f;
            cfg.trunkColorR = 0.54f; cfg.trunkColorG = 0.48f; cfg.trunkColorB = 0.40f;
            cfg.leafColorR = 0.18f; cfg.leafColorG = 0.52f; cfg.leafColorB = 0.14f;
            cfg.branchCount = 22;
            cfg.leafCount = 264;
            cfg.stemLengthRatio = 0.78f;
            break;
        case TreeSpeciesId::RED_MANGROVE:
            cfg.commonName = "Red Mangrove (Mkoko)";
            cfg.botanicalName = "Rhizophora mucronata";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B][^+B]A;B=F[+L][-L]L";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 44.0f;
            cfg.treeHeightMeters = 9.5f;
            cfg.trunkRadiusMeters = 0.36f;
            cfg.trunkColorR = 0.32f; cfg.trunkColorG = 0.22f; cfg.trunkColorB = 0.16f;
            cfg.leafColorR = 0.14f; cfg.leafColorG = 0.38f; cfg.leafColorB = 0.12f;
            cfg.branchCount = 42;
            cfg.leafCount = 1400;
            cfg.stemLengthRatio = 0.38f;
            break;
        case TreeSpeciesId::BEACH_CASUARINA:
            cfg.commonName = "Whistling Pine (Mvinje)";
            cfg.botanicalName = "Casuarina equisetifolia";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=F[&B]////[&B]////[&B]FA;B=F[+L][-L]FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 48.0f;
            cfg.treeHeightMeters = 22.0f;
            cfg.trunkRadiusMeters = 0.44f;
            cfg.trunkColorR = 0.38f; cfg.trunkColorG = 0.32f; cfg.trunkColorB = 0.26f;
            cfg.leafColorR = 0.22f; cfg.leafColorG = 0.42f; cfg.leafColorB = 0.25f;
            cfg.branchCount = 48;
            cfg.leafCount = 1800;
            cfg.stemLengthRatio = 0.24f;
            break;
        case TreeSpeciesId::DOUM_PALM:
            cfg.commonName = "Doum Palm (Turkana Fan Palm)";
            cfg.botanicalName = "Hyphaene thebaica";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=F[+FA][-FA];B=[&L]///[&L]";
            cfg.maxRecursionDepth = 3;
            cfg.branchAngleMeanDeg = 36.0f;
            cfg.treeHeightMeters = 12.5f;
            cfg.trunkRadiusMeters = 0.42f;
            cfg.trunkColorR = 0.48f; cfg.trunkColorG = 0.42f; cfg.trunkColorB = 0.36f;
            cfg.leafColorR = 0.26f; cfg.leafColorG = 0.46f; cfg.leafColorB = 0.22f;
            cfg.branchCount = 24;
            cfg.leafCount = 180;
            cfg.stemLengthRatio = 0.55f;
            break;
        case TreeSpeciesId::DESERT_ROSE:
            cfg.commonName = "Desert Rose (Adenium Caudex)";
            cfg.botanicalName = "Adenium obesum";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=[+FB][-FB][&FB];B=F[+L][-L]";
            cfg.maxRecursionDepth = 3;
            cfg.branchAngleMeanDeg = 46.0f;
            cfg.treeHeightMeters = 3.8f;
            cfg.trunkRadiusMeters = 0.65f;
            cfg.trunkColorR = 0.56f; cfg.trunkColorG = 0.50f; cfg.trunkColorB = 0.44f;
            cfg.leafColorR = 0.20f; cfg.leafColorG = 0.48f; cfg.leafColorB = 0.18f;
            cfg.branchCount = 20;
            cfg.leafCount = 240;
            cfg.stemLengthRatio = 0.22f;
            break;
        case TreeSpeciesId::CANDELABRA_EUPHORBIA:
            cfg.commonName = "Candelabra Euphorbia";
            cfg.botanicalName = "Euphorbia candelabrum";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=[+FB][-FB][^+FB][^-FB];B=F[^F]";
            cfg.maxRecursionDepth = 3;
            cfg.branchAngleMeanDeg = 28.0f;
            cfg.treeHeightMeters = 11.0f;
            cfg.trunkRadiusMeters = 0.52f;
            cfg.trunkColorR = 0.36f; cfg.trunkColorG = 0.44f; cfg.trunkColorB = 0.24f;
            cfg.leafColorR = 0.22f; cfg.leafColorG = 0.50f; cfg.leafColorB = 0.16f;
            cfg.branchCount = 26;
            cfg.leafCount = 260;
            cfg.stemLengthRatio = 0.28f;
            break;
        case TreeSpeciesId::BANANA_TREE:
            cfg.commonName = "Banana Plant (Musa Grove)";
            cfg.botanicalName = "Musa acuminata";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=[&FL]///[&FL]///[&FL]///[&FL]";
            cfg.maxRecursionDepth = 2;
            cfg.branchAngleMeanDeg = 42.0f;
            cfg.treeHeightMeters = 4.5f;
            cfg.trunkRadiusMeters = 0.28f;
            cfg.trunkColorR = 0.44f; cfg.trunkColorG = 0.58f; cfg.trunkColorB = 0.26f;
            cfg.leafColorR = 0.28f; cfg.leafColorG = 0.64f; cfg.leafColorB = 0.18f;
            cfg.branchCount = 13;
            cfg.leafCount = 13;
            cfg.stemLengthRatio = 0.60f;
            break;
        case TreeSpeciesId::MANGO_TREE:
            cfg.commonName = "Mango Tree (Dense Canopy)";
            cfg.botanicalName = "Mangifera indica";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B][^+B][^-B]FA;B=F[+L][-L]FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 42.0f;
            cfg.treeHeightMeters = 16.0f;
            cfg.trunkRadiusMeters = 0.72f;
            cfg.trunkColorR = 0.38f; cfg.trunkColorG = 0.32f; cfg.trunkColorB = 0.26f;
            cfg.leafColorR = 0.10f; cfg.leafColorG = 0.34f; cfg.leafColorB = 0.12f;
            cfg.branchCount = 48;
            cfg.leafCount = 1950;
            cfg.stemLengthRatio = 0.38f;
            break;
        case TreeSpeciesId::PAPAYA_TREE:
            cfg.commonName = "Papaya Tree (Carica Column)";
            cfg.botanicalName = "Carica papaya";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=[&FL]///[&FL]///[&FL]///[&FL]";
            cfg.maxRecursionDepth = 2;
            cfg.branchAngleMeanDeg = 38.0f;
            cfg.treeHeightMeters = 5.2f;
            cfg.trunkRadiusMeters = 0.22f;
            cfg.trunkColorR = 0.52f; cfg.trunkColorG = 0.48f; cfg.trunkColorB = 0.42f;
            cfg.leafColorR = 0.20f; cfg.leafColorG = 0.56f; cfg.leafColorB = 0.16f;
            cfg.branchCount = 16;
            cfg.leafCount = 18;
            cfg.stemLengthRatio = 0.75f;
            break;
        case TreeSpeciesId::JACARANDA:
            cfg.commonName = "Jacaranda (Blue Mist)";
            cfg.botanicalName = "Jacaranda mimosifolia";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B][^+B][^-B]FA;B=F[+B][-B]FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 46.0f;
            cfg.treeHeightMeters = 13.5f;
            cfg.trunkRadiusMeters = 0.46f;
            cfg.trunkColorR = 0.42f; cfg.trunkColorG = 0.37f; cfg.trunkColorB = 0.32f;
            cfg.leafColorR = 0.24f; cfg.leafColorG = 0.47f; cfg.leafColorB = 0.19f;
            cfg.branchCount = 48;
            cfg.leafCount = 1650;
            cfg.stemLengthRatio = 0.36f;
            break;
        case TreeSpeciesId::FLAMBOYANT_POINCIANA:
            cfg.commonName = "Flamboyant (Royal Poinciana)";
            cfg.botanicalName = "Delonix regia";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B]FA;B=F[+L][-L]FB";
            cfg.maxRecursionDepth = 3;
            cfg.branchAngleMeanDeg = 64.0f;
            cfg.treeHeightMeters = 12.0f;
            cfg.trunkRadiusMeters = 0.56f;
            cfg.trunkColorR = 0.49f; cfg.trunkColorG = 0.45f; cfg.trunkColorB = 0.41f;
            cfg.leafColorR = 0.20f; cfg.leafColorG = 0.48f; cfg.leafColorB = 0.16f;
            cfg.branchCount = 52;
            cfg.leafCount = 1650;
            cfg.stemLengthRatio = 0.34f;
            break;
        case TreeSpeciesId::YELLOW_TRUMPET_TREE:
            cfg.commonName = "Golden Poui (Araguaney)";
            cfg.botanicalName = "Handroanthus chrysanthus";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 42.0f;
            cfg.treeHeightMeters = 11.5f;
            cfg.trunkRadiusMeters = 0.42f;
            cfg.trunkColorR = 0.42f; cfg.trunkColorG = 0.37f; cfg.trunkColorB = 0.31f;
            cfg.leafColorR = 0.23f; cfg.leafColorG = 0.43f; cfg.leafColorB = 0.17f;
            cfg.branchCount = 44;
            cfg.leafCount = 1400;
            cfg.stemLengthRatio = 0.40f;
            break;
        case TreeSpeciesId::SOUTHERN_MAGNOLIA:
            cfg.commonName = "Southern Magnolia";
            cfg.botanicalName = "Magnolia grandiflora";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B]FA;B=F[+B][-B]FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 44.0f;
            cfg.treeHeightMeters = 18.0f;
            cfg.trunkRadiusMeters = 0.58f;
            cfg.trunkColorR = 0.39f; cfg.trunkColorG = 0.33f; cfg.trunkColorB = 0.28f;
            cfg.leafColorR = 0.10f; cfg.leafColorG = 0.30f; cfg.leafColorB = 0.13f;
            cfg.branchCount = 48;
            cfg.leafCount = 1650;
            cfg.stemLengthRatio = 0.36f;
            break;
        case TreeSpeciesId::CREPE_MYRTLE:
            cfg.commonName = "Crape Myrtle";
            cfg.botanicalName = "Lagerstroemia indica";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 32.0f;
            cfg.treeHeightMeters = 6.8f;
            cfg.trunkRadiusMeters = 0.24f;
            cfg.trunkColorR = 0.69f; cfg.trunkColorG = 0.54f; cfg.trunkColorB = 0.41f;
            cfg.leafColorR = 0.21f; cfg.leafColorG = 0.45f; cfg.leafColorB = 0.19f;
            cfg.branchCount = 42;
            cfg.leafCount = 1100;
            cfg.stemLengthRatio = 0.34f;
            break;
        case TreeSpeciesId::GOLDEN_SHOWER_CASSIA:
            cfg.commonName = "Golden Shower Tree";
            cfg.botanicalName = "Cassia fistula";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B]FA;B=F[+L][-L]FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 44.0f;
            cfg.treeHeightMeters = 10.5f;
            cfg.trunkRadiusMeters = 0.36f;
            cfg.trunkColorR = 0.49f; cfg.trunkColorG = 0.48f; cfg.trunkColorB = 0.43f;
            cfg.leafColorR = 0.26f; cfg.leafColorG = 0.51f; cfg.leafColorB = 0.19f;
            cfg.branchCount = 44;
            cfg.leafCount = 1450;
            cfg.stemLengthRatio = 0.38f;
            break;
        case TreeSpeciesId::FRANGIPANI_PLUMERIA:
            cfg.commonName = "Frangipani (Plumeria)";
            cfg.botanicalName = "Plumeria rubra";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=[+FB][-FB][&FB];B=F[+L][-L]";
            cfg.maxRecursionDepth = 3;
            cfg.branchAngleMeanDeg = 38.0f;
            cfg.treeHeightMeters = 5.5f;
            cfg.trunkRadiusMeters = 0.26f;
            cfg.trunkColorR = 0.55f; cfg.trunkColorG = 0.53f; cfg.trunkColorB = 0.49f;
            cfg.leafColorR = 0.18f; cfg.leafColorG = 0.43f; cfg.leafColorB = 0.16f;
            cfg.branchCount = 26;
            cfg.leafCount = 420;
            cfg.stemLengthRatio = 0.32f;
            break;
        case TreeSpeciesId::CRIMSON_BOTTLEBRUSH:
            cfg.commonName = "Crimson Bottlebrush";
            cfg.botanicalName = "Melaleuca citrina";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 46.0f;
            cfg.treeHeightMeters = 6.2f;
            cfg.trunkRadiusMeters = 0.22f;
            cfg.trunkColorR = 0.46f; cfg.trunkColorG = 0.40f; cfg.trunkColorB = 0.34f;
            cfg.leafColorR = 0.24f; cfg.leafColorG = 0.48f; cfg.leafColorB = 0.22f;
            cfg.branchCount = 42;
            cfg.leafCount = 1350;
            cfg.stemLengthRatio = 0.28f;
            break;
        case TreeSpeciesId::CAPE_CHESTNUT:
            cfg.commonName = "Cape Chestnut (Muraru)";
            cfg.botanicalName = "Calodendrum capense";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B][^+B][^-B]FA;B=F[+B][-B]FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 42.0f;
            cfg.treeHeightMeters = 14.5f;
            cfg.trunkRadiusMeters = 0.50f;
            cfg.trunkColorR = 0.51f; cfg.trunkColorG = 0.49f; cfg.trunkColorB = 0.45f;
            cfg.leafColorR = 0.16f; cfg.leafColorG = 0.37f; cfg.leafColorB = 0.16f;
            cfg.branchCount = 48;
            cfg.leafCount = 1650;
            cfg.stemLengthRatio = 0.40f;
            break;
        case TreeSpeciesId::ITALIAN_CYPRESS:
            cfg.commonName = "Italian Cypress";
            cfg.botanicalName = "Cupressus sempervirens";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=F[+B][-B][&B][^B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 18.0f;
            cfg.treeHeightMeters = 20.0f;
            cfg.trunkRadiusMeters = 0.32f;
            cfg.trunkColorR = 0.31f; cfg.trunkColorG = 0.24f; cfg.trunkColorB = 0.19f;
            cfg.leafColorR = 0.09f; cfg.leafColorG = 0.29f; cfg.leafColorB = 0.17f;
            cfg.branchCount = 64;
            cfg.leafCount = 2350;
            cfg.stemLengthRatio = 0.18f;
            break;
        case TreeSpeciesId::JAPANESE_MAPLE:
            cfg.commonName = "Japanese Maple (Momiji)";
            cfg.botanicalName = "Acer palmatum";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 44.0f;
            cfg.treeHeightMeters = 5.8f;
            cfg.trunkRadiusMeters = 0.24f;
            cfg.trunkColorR = 0.42f; cfg.trunkColorG = 0.43f; cfg.trunkColorB = 0.39f;
            cfg.leafColorR = 0.61f; cfg.leafColorG = 0.11f; cfg.leafColorB = 0.14f;
            cfg.branchCount = 44;
            cfg.leafCount = 1250;
            cfg.stemLengthRatio = 0.42f;
            break;
        case TreeSpeciesId::BOUGAINVILLEA_STANDARD:
            cfg.commonName = "Standard Bougainvillea";
            cfg.botanicalName = "Bougainvillea spectabilis";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 48.0f;
            cfg.treeHeightMeters = 4.2f;
            cfg.trunkRadiusMeters = 0.18f;
            cfg.trunkColorR = 0.39f; cfg.trunkColorG = 0.33f; cfg.trunkColorB = 0.27f;
            cfg.leafColorR = 0.17f; cfg.leafColorG = 0.38f; cfg.leafColorB = 0.16f;
            cfg.branchCount = 40;
            cfg.leafCount = 1200;
            cfg.stemLengthRatio = 0.32f;
            break;
        case TreeSpeciesId::FLOWERING_DOGWOOD:
            cfg.commonName = "Flowering Dogwood";
            cfg.botanicalName = "Cornus florida";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 58.0f;
            cfg.treeHeightMeters = 7.5f;
            cfg.trunkRadiusMeters = 0.28f;
            cfg.trunkColorR = 0.29f; cfg.trunkColorG = 0.24f; cfg.trunkColorB = 0.21f;
            cfg.leafColorR = 0.22f; cfg.leafColorG = 0.46f; cfg.leafColorB = 0.19f;
            cfg.branchCount = 38;
            cfg.leafCount = 1150;
            cfg.stemLengthRatio = 0.38f;
            break;
        case TreeSpeciesId::AVOCADO_TREE:
            cfg.commonName = "Avocado Tree (Parachichi)";
            cfg.botanicalName = "Persea americana";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B][^+B][^-B]FA;B=F[+B][-B]FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 42.0f;
            cfg.treeHeightMeters = 13.5f;
            cfg.trunkRadiusMeters = 0.48f;
            cfg.trunkColorR = 0.36f; cfg.trunkColorG = 0.28f; cfg.trunkColorB = 0.21f;
            cfg.leafColorR = 0.12f; cfg.leafColorG = 0.36f; cfg.leafColorB = 0.14f;
            cfg.branchCount = 48;
            cfg.leafCount = 1850;
            cfg.stemLengthRatio = 0.36f;
            break;
        case TreeSpeciesId::SWEET_ORANGE:
            cfg.commonName = "Sweet Orange (Mchungwa)";
            cfg.botanicalName = "Citrus x sinensis";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 54.0f;
            cfg.treeHeightMeters = 7.2f;
            cfg.trunkRadiusMeters = 0.28f;
            cfg.trunkColorR = 0.38f; cfg.trunkColorG = 0.33f; cfg.trunkColorB = 0.26f;
            cfg.leafColorR = 0.11f; cfg.leafColorG = 0.40f; cfg.leafColorB = 0.14f;
            cfg.branchCount = 46;
            cfg.leafCount = 1650;
            cfg.stemLengthRatio = 0.28f;
            break;
        case TreeSpeciesId::LEMON_TREE:
            cfg.commonName = "Meyer Lemon Tree (Mlimau)";
            cfg.botanicalName = "Citrus limon";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 56.0f;
            cfg.treeHeightMeters = 5.4f;
            cfg.trunkRadiusMeters = 0.22f;
            cfg.trunkColorR = 0.41f; cfg.trunkColorG = 0.36f; cfg.trunkColorB = 0.28f;
            cfg.leafColorR = 0.16f; cfg.leafColorG = 0.45f; cfg.leafColorB = 0.16f;
            cfg.branchCount = 42;
            cfg.leafCount = 1450;
            cfg.stemLengthRatio = 0.28f;
            break;
        case TreeSpeciesId::MACADAMIA_TREE:
            cfg.commonName = "Macadamia Nut Tree";
            cfg.botanicalName = "Macadamia integrifolia";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B][^+B][^-B]FA;B=F[+B][-B]FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 38.0f;
            cfg.treeHeightMeters = 12.0f;
            cfg.trunkRadiusMeters = 0.42f;
            cfg.trunkColorR = 0.39f; cfg.trunkColorG = 0.33f; cfg.trunkColorB = 0.27f;
            cfg.leafColorR = 0.13f; cfg.leafColorG = 0.38f; cfg.leafColorB = 0.14f;
            cfg.branchCount = 48;
            cfg.leafCount = 1800;
            cfg.stemLengthRatio = 0.26f;
            break;
        case TreeSpeciesId::JACKFRUIT_TREE:
            cfg.commonName = "Jackfruit Tree (Mfenesi)";
            cfg.botanicalName = "Artocarpus heterophyllus";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B][^+B][^-B]FA;B=F[+B][-B]FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 44.0f;
            cfg.treeHeightMeters = 14.5f;
            cfg.trunkRadiusMeters = 0.56f;
            cfg.trunkColorR = 0.35f; cfg.trunkColorG = 0.26f; cfg.trunkColorB = 0.19f;
            cfg.leafColorR = 0.10f; cfg.leafColorG = 0.36f; cfg.leafColorB = 0.13f;
            cfg.branchCount = 46;
            cfg.leafCount = 1750;
            cfg.stemLengthRatio = 0.32f;
            break;
        case TreeSpeciesId::GUAVA_TREE:
            cfg.commonName = "Guava Tree (Mpera)";
            cfg.botanicalName = "Psidium guajava";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 46.0f;
            cfg.treeHeightMeters = 6.2f;
            cfg.trunkRadiusMeters = 0.24f;
            cfg.trunkColorR = 0.62f; cfg.trunkColorG = 0.50f; cfg.trunkColorB = 0.40f;
            cfg.leafColorR = 0.18f; cfg.leafColorG = 0.44f; cfg.leafColorB = 0.18f;
            cfg.branchCount = 42;
            cfg.leafCount = 1250;
            cfg.stemLengthRatio = 0.28f;
            break;
        case TreeSpeciesId::PASSION_FRUIT_STANDARD:
            cfg.commonName = "Passion Fruit Vine (Matunda)";
            cfg.botanicalName = "Passiflora edulis";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 52.0f;
            cfg.treeHeightMeters = 3.2f;
            cfg.trunkRadiusMeters = 0.024f;
            cfg.trunkColorR = 0.33f; cfg.trunkColorG = 0.48f; cfg.trunkColorB = 0.18f;
            cfg.leafColorR = 0.15f; cfg.leafColorG = 0.44f; cfg.leafColorB = 0.16f;
            cfg.branchCount = 40;
            cfg.leafCount = 1100;
            cfg.stemLengthRatio = 0.36f;
            break;
        case TreeSpeciesId::POMEGRANATE_TREE:
            cfg.commonName = "Pomegranate (Komamanga)";
            cfg.botanicalName = "Punica granatum";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 42.0f;
            cfg.treeHeightMeters = 5.2f;
            cfg.trunkRadiusMeters = 0.22f;
            cfg.trunkColorR = 0.48f; cfg.trunkColorG = 0.42f; cfg.trunkColorB = 0.36f;
            cfg.leafColorR = 0.18f; cfg.leafColorG = 0.46f; cfg.leafColorB = 0.16f;
            cfg.branchCount = 42;
            cfg.leafCount = 1250;
            cfg.stemLengthRatio = 0.26f;
            break;
        case TreeSpeciesId::HONEYCRISP_APPLE:
            cfg.commonName = "Orchard Apple Tree";
            cfg.botanicalName = "Malus domestica";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 50.0f;
            cfg.treeHeightMeters = 7.8f;
            cfg.trunkRadiusMeters = 0.34f;
            cfg.trunkColorR = 0.36f; cfg.trunkColorG = 0.28f; cfg.trunkColorB = 0.22f;
            cfg.leafColorR = 0.18f; cfg.leafColorG = 0.46f; cfg.leafColorB = 0.18f;
            cfg.branchCount = 44;
            cfg.leafCount = 1550;
            cfg.stemLengthRatio = 0.34f;
            break;
        case TreeSpeciesId::SWEET_PEAR:
            cfg.commonName = "European Pear Tree";
            cfg.botanicalName = "Pyrus communis";
            cfg.lsystemAxiom = "FA";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 32.0f;
            cfg.treeHeightMeters = 9.5f;
            cfg.trunkRadiusMeters = 0.32f;
            cfg.trunkColorR = 0.38f; cfg.trunkColorG = 0.31f; cfg.trunkColorB = 0.25f;
            cfg.leafColorR = 0.17f; cfg.leafColorG = 0.45f; cfg.leafColorB = 0.17f;
            cfg.branchCount = 42;
            cfg.leafCount = 1450;
            cfg.stemLengthRatio = 0.36f;
            break;
        case TreeSpeciesId::PEACH_TREE:
            cfg.commonName = "Peach Tree";
            cfg.botanicalName = "Prunus persica";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 46.0f;
            cfg.treeHeightMeters = 5.8f;
            cfg.trunkRadiusMeters = 0.25f;
            cfg.trunkColorR = 0.34f; cfg.trunkColorG = 0.24f; cfg.trunkColorB = 0.20f;
            cfg.leafColorR = 0.20f; cfg.leafColorG = 0.48f; cfg.leafColorB = 0.18f;
            cfg.branchCount = 40;
            cfg.leafCount = 1350;
            cfg.stemLengthRatio = 0.30f;
            break;
        case TreeSpeciesId::CACAO_TREE:
            cfg.commonName = "Cocoa / Cacao Tree";
            cfg.botanicalName = "Theobroma cacao";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 52.0f;
            cfg.treeHeightMeters = 6.8f;
            cfg.trunkRadiusMeters = 0.24f;
            cfg.trunkColorR = 0.35f; cfg.trunkColorG = 0.25f; cfg.trunkColorB = 0.19f;
            cfg.leafColorR = 0.13f; cfg.leafColorG = 0.39f; cfg.leafColorB = 0.15f;
            cfg.branchCount = 38;
            cfg.leafCount = 1250;
            cfg.stemLengthRatio = 0.34f;
            break;
        case TreeSpeciesId::BREADFRUIT_TREE:
            cfg.commonName = "Breadfruit Tree (Mshelisheli)";
            cfg.botanicalName = "Artocarpus altilis";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B][^+B][^-B]FA;B=F[+B][-B]FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 44.0f;
            cfg.treeHeightMeters = 15.0f;
            cfg.trunkRadiusMeters = 0.54f;
            cfg.trunkColorR = 0.44f; cfg.trunkColorG = 0.39f; cfg.trunkColorB = 0.32f;
            cfg.leafColorR = 0.14f; cfg.leafColorG = 0.44f; cfg.leafColorB = 0.15f;
            cfg.branchCount = 40;
            cfg.leafCount = 1150;
            cfg.stemLengthRatio = 0.38f;
            break;
        case TreeSpeciesId::LYCHEE_TREE:
            cfg.commonName = "Lychee Tree";
            cfg.botanicalName = "Litchi chinensis";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B][^+B][^-B]FA;B=F[+B][-B]FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 42.0f;
            cfg.treeHeightMeters = 11.5f;
            cfg.trunkRadiusMeters = 0.46f;
            cfg.trunkColorR = 0.34f; cfg.trunkColorG = 0.28f; cfg.trunkColorB = 0.24f;
            cfg.leafColorR = 0.11f; cfg.leafColorG = 0.38f; cfg.leafColorB = 0.15f;
            cfg.branchCount = 48;
            cfg.leafCount = 1750;
            cfg.stemLengthRatio = 0.32f;
            break;
        case TreeSpeciesId::CASHEW_TREE:
            cfg.commonName = "Cashew Tree (Mkorosho)";
            cfg.botanicalName = "Anacardium occidentale";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[&+B][&-B]FA;B=F[+L][-L]B";
            cfg.maxRecursionDepth = 4;
            cfg.branchAngleMeanDeg = 56.0f;
            cfg.treeHeightMeters = 8.5f;
            cfg.trunkRadiusMeters = 0.38f;
            cfg.trunkColorR = 0.44f; cfg.trunkColorG = 0.36f; cfg.trunkColorB = 0.29f;
            cfg.leafColorR = 0.16f; cfg.leafColorG = 0.42f; cfg.leafColorB = 0.16f;
            cfg.branchCount = 44;
            cfg.leafCount = 1450;
            cfg.stemLengthRatio = 0.30f;
            break;
        default:
            cfg.commonName = "Conifer / Broadleaf";
            cfg.lsystemAxiom = "A";
            cfg.lsystemRules = "A=F[+B][-B]A;B=FB";
            cfg.maxRecursionDepth = 4;
            cfg.branchCount = 44;
            cfg.leafCount = 1450;
            break;
    }
    return cfg;
}

} // namespace vegetation
