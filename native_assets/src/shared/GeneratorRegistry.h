#pragma once
#include "IGenerator.h"
#include <unordered_map>
#include <vector>
#include <memory>

namespace sphere {
class GeneratorRegistry {
public:
    static GeneratorRegistry& Get();

    void Initialize();
    void Register(std::shared_ptr<IGenerator> generator);
    std::shared_ptr<IGenerator> GetGenerator(const std::string& name) const;
    std::vector<std::shared_ptr<IGenerator>> GetGeneratorsByType(GeneratorType type) const;

private:
    std::unordered_map<std::string, std::shared_ptr<IGenerator>> m_Generators;
};
}\n