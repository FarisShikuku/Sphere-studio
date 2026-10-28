#pragma once
#include "MaterialDef.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>

namespace sphere {

class MaterialRegistry {
public:
    static MaterialRegistry& Get();

    void Initialize();
    const MaterialDef& GetMaterial(MaterialID id) const;
    void SetMaterial(MaterialID id, const MaterialDef& def);
    bool HasMaterial(MaterialID id) const;

    std::vector<MaterialID> GetAllMaterialIDs() const;
    std::vector<MaterialID> GetMaterialsByCategory(const std::string& category) const;

private:
    MaterialRegistry();
    ~MaterialRegistry() = default;

    void RegisterDefaultPresets();

    mutable std::mutex m_Mutex;
    std::unordered_map<MaterialID, MaterialDef> m_Materials;
    MaterialDef m_Fallback;
};

} // namespace sphere
