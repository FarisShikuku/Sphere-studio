#include "GeneratorRegistry.h"
#include "core/Log.h"
#include "weather/WeatherSystem.h"

namespace sphere {
GeneratorRegistry& GeneratorRegistry::Get() {
    static GeneratorRegistry instance;
    return instance;
}

void GeneratorRegistry::Initialize() {
    SPHERE_LOG_INFO("GeneratorRegistry initialized. Ready for procedural asset bindings.");
    Register(std::make_shared<WeatherSystem>());
}


void GeneratorRegistry::Register(std::shared_ptr<IGenerator> generator) {
    if (generator) {
        m_Generators[generator->GetName()] = generator;
        SPHERE_LOG_INFO("Registered procedural generator: " + generator->GetName());
    }
}

std::shared_ptr<IGenerator> GeneratorRegistry::GetGenerator(const std::string& name) const {
    auto it = m_Generators.find(name);
    return it != m_Generators.end() ? it->second : nullptr;
}

std::vector<std::shared_ptr<IGenerator>> GeneratorRegistry::GetGeneratorsByType(GeneratorType type) const {
    std::vector<std::shared_ptr<IGenerator>> result;
    for (const auto& [name, gen] : m_Generators) {
        if (gen->GetType() == type) result.push_back(gen);
    }
    return result;
}
}\n