#include "MaterialRegistry.h"

namespace sphere {
MaterialRegistry& MaterialRegistry::Get() {
    static MaterialRegistry instance;
    return instance;
}

void MaterialRegistry::Register(const Material& material) {
    m_Materials[material.name] = material;
}

const Material* MaterialRegistry::GetMaterial(const std::string& name) const {
    auto it = m_Materials.find(name);
    return it != m_Materials.end() ? &it->second : nullptr;
}
}\n