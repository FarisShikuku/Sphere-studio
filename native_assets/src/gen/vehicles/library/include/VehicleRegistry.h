#pragma once
#include "IVehicle.h"
#include "VehicleID.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <vulkan/vulkan.h>

namespace sphere::vehicles {

class VehicleRegistry {
public:
    static VehicleRegistry& Get();

    void Initialize();

    std::shared_ptr<IVehicle> GetVehicle(VehicleID id);
    const VehicleDef& GetDef(VehicleID id);
    void SetDef(VehicleID id, const VehicleDef& def);

    std::shared_ptr<Mesh> RequestMesh(VehicleID id, uint32_t lod = 0);
    bool RequestTextures(VehicleID id,
                         VkDevice device = VK_NULL_HANDLE,
                         VkQueue queue = VK_NULL_HANDLE,
                         VkCommandPool pool = VK_NULL_HANDLE);

    std::vector<VehicleID> GetVehiclesByCategory(const std::string& category) const;
    size_t GetTotalVehiclesCount() const { return static_cast<size_t>(VehicleID::COUNT); }

private:
    VehicleRegistry();
    void RegisterAll();

    std::unordered_map<uint32_t, std::shared_ptr<IVehicle>> m_Vehicles;
    bool m_Initialized = false;
};

} // namespace sphere::vehicles
