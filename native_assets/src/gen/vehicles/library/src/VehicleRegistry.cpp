#include "VehicleRegistry.h"
#include "RigidLorry.h"
#include "ArticulatedTractor.h"
#include "CityBus.h"
#include "CoachBus.h"
#include "Minibus.h"
#include <algorithm>

namespace sphere::vehicles {

VehicleRegistry& VehicleRegistry::Get() {
    static VehicleRegistry instance;
    return instance;
}

VehicleRegistry::VehicleRegistry() {
    Initialize();
}

void VehicleRegistry::Initialize() {
    if (m_Initialized) return;
    RegisterAll();
    m_Initialized = true;
}

void VehicleRegistry::RegisterAll() {
    auto add = [&](std::shared_ptr<IVehicle> v) {
        if (v) {
            m_Vehicles[static_cast<uint32_t>(v->GetID())] = std::move(v);
        }
    };

    // 1. Rigid Lorries (5)
    add(std::make_shared<RigidLorry>(VehicleID::LORRY_RIGID_BOX));
    add(std::make_shared<RigidLorry>(VehicleID::LORRY_RIGID_FLATBED));
    add(std::make_shared<RigidLorry>(VehicleID::LORRY_RIGID_TIPPER));
    add(std::make_shared<RigidLorry>(VehicleID::LORRY_RIGID_CEMENT_MIXER));
    add(std::make_shared<RigidLorry>(VehicleID::LORRY_RIGID_TANKER));

    // 2. Articulated Tractor Units (4)
    add(std::make_shared<ArticulatedTractor>(VehicleID::TRACTOR_CAB_OVER));
    add(std::make_shared<ArticulatedTractor>(VehicleID::TRACTOR_CONVENTIONAL));
    add(std::make_shared<ArticulatedTractor>(VehicleID::TRACTOR_EURO_CAB));
    add(std::make_shared<ArticulatedTractor>(VehicleID::TRACTOR_AMERICAN_CAB));

    // 3. City Buses (4)
    add(std::make_shared<CityBus>(VehicleID::BUS_CITY_STANDARD));
    add(std::make_shared<CityBus>(VehicleID::BUS_CITY_DOUBLE_DECKER));
    add(std::make_shared<CityBus>(VehicleID::BUS_CITY_ARTICULATED_BENDY));
    add(std::make_shared<CityBus>(VehicleID::BUS_CITY_LOW_FLOOR));

    // 4. Intercity Coaches (4)
    add(std::make_shared<CoachBus>(VehicleID::COACH_HIGH_DECKER));
    add(std::make_shared<CoachBus>(VehicleID::COACH_SLEEPER));
    add(std::make_shared<CoachBus>(VehicleID::COACH_TOURING));
    add(std::make_shared<CoachBus>(VehicleID::COACH_LUXURY));

    // 5. Minibuses & Matatus (4)
    add(std::make_shared<Minibus>(VehicleID::MATATU_14_SEATER));
    add(std::make_shared<Minibus>(VehicleID::SPRINTER_VAN));
    add(std::make_shared<Minibus>(VehicleID::SHUTTLE_BUS));
    add(std::make_shared<Minibus>(VehicleID::SCHOOL_VAN));
}

std::shared_ptr<IVehicle> VehicleRegistry::GetVehicle(VehicleID id) {
    auto it = m_Vehicles.find(static_cast<uint32_t>(id));
    return (it != m_Vehicles.end()) ? it->second : nullptr;
}

const VehicleDef& VehicleRegistry::GetDef(VehicleID id) {
    static const VehicleDef kFallbackDef;
    auto v = GetVehicle(id);
    return v ? v->GetDef() : kFallbackDef;
}

void VehicleRegistry::SetDef(VehicleID id, const VehicleDef& def) {
    auto v = GetVehicle(id);
    if (v) v->SetDef(def);
}

std::shared_ptr<Mesh> VehicleRegistry::RequestMesh(VehicleID id, uint32_t lod) {
    auto v = GetVehicle(id);
    if (!v) return nullptr;
    auto cached = v->GetCachedMesh();
    return cached ? cached : v->GenerateMesh(lod);
}

bool VehicleRegistry::RequestTextures(VehicleID id, VkDevice device, VkQueue queue, VkCommandPool pool) {
    auto v = GetVehicle(id);
    return v ? v->GenerateTextures(device, queue, pool) : false;
}

std::vector<VehicleID> VehicleRegistry::GetVehiclesByCategory(const std::string& category) const {
    std::vector<VehicleID> out;
    for (uint32_t i = 0; i < static_cast<uint32_t>(VehicleID::COUNT); ++i) {
        auto id = static_cast<VehicleID>(i);
        if (category == GetVehicleCategory(id)) {
            out.push_back(id);
        }
    }
    return out;
}

} // namespace sphere::vehicles
