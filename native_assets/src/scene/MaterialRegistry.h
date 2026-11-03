#pragma once
#include "Material.h"
#include <unordered_map>
#include <memory>
namespace sphere {
class MaterialRegistry {
public:
    static MaterialRegistry& Get();
    void Register(const Material& material);
    const Material* GetMaterial(const std::string& name) const;
private:
    std::unordered_map<std::string, Material> m_Materials;
};
}
