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

    // 6. Extended >12m Reference Coaches & Double-Deckers (10)
    add(std::make_shared<CoachBus>(VehicleID::COACH_VOLVO_9700_DD));
    add(std::make_shared<CoachBus>(VehicleID::COACH_VOLVO_9800_DD));
    add(std::make_shared<CoachBus>(VehicleID::COACH_VOLVO_9700_TEAL));
    add(std::make_shared<CoachBus>(VehicleID::COACH_VOLVO_9800_AERO));
    add(std::make_shared<CoachBus>(VehicleID::COACH_YUTONG_TOURING));
    add(std::make_shared<CoachBus>(VehicleID::COACH_AERO_BLADE_VIP));
    add(std::make_shared<CoachBus>(VehicleID::COACH_CHAMPAGNE_SR2));
    add(std::make_shared<CoachBus>(VehicleID::COACH_NEOPLAN_TOURLINER));
    add(std::make_shared<CoachBus>(VehicleID::COACH_TWIN_GLASS_DOME));
    add(std::make_shared<CoachBus>(VehicleID::COACH_OBSIDIAN_VIP_6X2));
    add(std::make_shared<CoachBus>(VehicleID::COACH_EXECUTIVE_I6));
    add(std::make_shared<CoachBus>(VehicleID::COACH_LUXURY_VIP_4X2));

    // 7. Real-World Rigid Lorries
    add(std::make_shared<RigidLorry>(VehicleID::LORRY_UD_QUESTER_TIPPER));
    add(std::make_shared<RigidLorry>(VehicleID::LORRY_MAN_TGX_BOX));
    add(std::make_shared<RigidLorry>(VehicleID::LORRY_SCANIA_P320_CURTAINSIDER));

    // 8. Real-World Articulated Tractors
    add(std::make_shared<ArticulatedTractor>(VehicleID::TRACTOR_VOLVO_FH_ELECTRIC));
    add(std::make_shared<ArticulatedTractor>(VehicleID::TRACTOR_MERCEDES_EACTROS_600));
    add(std::make_shared<ArticulatedTractor>(VehicleID::TRACTOR_MERCEDES_GENH2));
    add(std::make_shared<ArticulatedTractor>(VehicleID::TRACTOR_SCANIA_45R_ELECTRIC));
    add(std::make_shared<ArticulatedTractor>(VehicleID::TRACTOR_SCANIA_R560_SUPER));
    add(std::make_shared<ArticulatedTractor>(VehicleID::TRACTOR_SCANIA_S770_LONGLINE));
    add(std::make_shared<ArticulatedTractor>(VehicleID::TRACTOR_SCANIA_40R_XT_ELECTRIC));
    add(std::make_shared<ArticulatedTractor>(VehicleID::TRACTOR_FORD_FMAX_500));
    add(std::make_shared<ArticulatedTractor>(VehicleID::TRACTOR_IVECO_SEWAY_ELECTRIC));
    add(std::make_shared<ArticulatedTractor>(VehicleID::TRACTOR_VW_CONSTELLATION_25390));
    add(std::make_shared<ArticulatedTractor>(VehicleID::TRACTOR_VW_METEOR_28460));
    add(std::make_shared<ArticulatedTractor>(VehicleID::TRACTOR_MAN_TGX_18510));
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
