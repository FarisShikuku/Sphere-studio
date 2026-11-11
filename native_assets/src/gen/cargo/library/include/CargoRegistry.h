#pragma once
#include "CargoID.h"
#include "CargoDef.h"
#include "ICargo.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>

namespace sphere::cargo {

/**
 * Singleton Cargo & Commodities Registry.
 * Handles lazy procedural generation and caching of 3D meshes (VkBuffers)
 * and PBR materials (VkImages) across warehouse, vehicle, and retail systems.
 */
class CargoRegistry {
public:
    static CargoRegistry& Get();

    void Initialize();
    void RegisterCargo(std::shared_ptr<ICargo> generator);
    std::shared_ptr<ICargo> GetCargo(CargoID id);
    const CargoDef& GetDef(CargoID id) const;
    void SetDef(CargoID id, const CargoDef& def);

    std::shared_ptr<Mesh> RequestMesh(CargoID id, uint32_t lod = 0);
    VkImage RequestImage(CargoID id, uint32_t channel = 0);

    bool HasCargo(CargoID id) const;
    std::vector<CargoID> GetAllCargoIDs() const;
    std::vector<CargoID> GetCargoByCategory(CargoCategory category) const;

    size_t GetTotalCommoditiesCount() const;

private:
    CargoRegistry();
    ~CargoRegistry() = default;

    void RegisterDefaultGenerators();

    mutable std::mutex m_Mutex;
    std::unordered_map<CargoID, std::shared_ptr<ICargo>> m_Generators;
    std::unordered_map<CargoID, CargoDef> m_Definitions;
    CargoDef m_FallbackDef;
};

} // namespace sphere::cargo
