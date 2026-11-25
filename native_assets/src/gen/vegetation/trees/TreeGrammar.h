#pragma once
// ============================================================================
// TreeGrammar.h - Lindenmayer L-System String Rewriting Grammar Engine in C++17
// Supports deterministic and stochastic parametric productions (Stojanovic 2016).
// ============================================================================

#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include "../common/VegetationMath.h"

namespace vegetation {

struct GrammarRule {
    char predecessor;
    std::string successor;
    float probability{1.0f};
};

class TreeGrammar {
public:
    TreeGrammar() = default;

    void setAxiom(const std::string& axiom) { m_axiom = axiom; }
    void addRule(char predecessor, const std::string& successor, float probability = 1.0f);
    void clearRules() { m_rules.clear(); }

    std::string derive(int iterations, FastRng& rng) const;

    static TreeGrammar createSpeciesGrammar(const std::string& axiomStr, const std::string& rulesStr);

    // Exact 6 stochastic productions p1...p6 from Nemanja Stojanovic (2016) Page 5
    // with probabilities 3/12, 1/12, 3/12, 1/12, 3/12, 1/12
    static TreeGrammar createStojanovicStochasticGrammar();

private:
    std::string m_axiom{"A"};
    std::unordered_map<char, std::vector<GrammarRule>> m_rules;
};

} // namespace vegetation
