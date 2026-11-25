#pragma once

#include "IGenerator.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace procedural {

class GeneratorRegistry {
public:
    static GeneratorRegistry& instance() {
        static GeneratorRegistry sInstance;
        return sInstance;
    }

    void registerGenerator(std::shared_ptr<IGenerator> generator) {
        if (!generator) return;
        generators_[generator->getName()] = std::move(generator);
    }

    std::shared_ptr<IGenerator> getGenerator(const std::string& name) const {
        auto it = generators_.find(name);
        return (it != generators_.end()) ? it->second : nullptr;
    }

    std::vector<std::shared_ptr<IGenerator>> getByCategory(GeneratorCategory category) const {
        std::vector<std::shared_ptr<IGenerator>> out;
        for (const auto& [name, gen] : generators_) {
            if (gen && gen->getCategory() == category) {
                out.push_back(gen);
            }
        }
        return out;
    }

private:
    GeneratorRegistry() = default;
    std::unordered_map<std::string, std::shared_ptr<IGenerator>> generators_;
};

namespace buildings {
using procedural::GeneratorRegistry;
} // namespace buildings

} // namespace procedural
