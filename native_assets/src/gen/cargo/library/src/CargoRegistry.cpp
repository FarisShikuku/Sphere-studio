#include "CargoRegistry.h"
#include "../../bagged_goods/include/BaggedGoodsCargo.h"
#include "../../boxes_crates/include/BoxesCratesCargo.h"
#include "../../liquid_bulk/include/LiquidBulkCargo.h"
#include "../../raw_materials/include/RawMaterialsCargo.h"
#include "../../containers_bottles/include/ContainersBottlesCargo.h"
#include "../../specialty_industrial/include/SpecialtyIndustrialCargo.h"
#include "../../perishable_goods/include/PerishableGoodsCargo.h"

namespace sphere::cargo {

CargoRegistry& CargoRegistry::Get() {
    static CargoRegistry instance;
    return instance;
}

CargoRegistry::CargoRegistry() {
    Initialize();
}

void CargoRegistry::Initialize() {
    std::lock_guard<std::mutex> lock(m_Mutex);
    if (!m_Generators.empty()) return;
    RegisterDefaultGenerators();
}

void CargoRegistry::RegisterDefaultGenerators() {
    // 1. Bagged Goods (7 variants)
    for (const auto& v : BaggedGoodsVariants::GetAllVariants()) {
        auto gen = std::make_shared<BaggedGoodsCargo>(v.id);
        m_Generators[v.id] = gen;
        m_Definitions[v.id] = gen->GetDef();
    }

    // 2. Boxes & Crates (5 variants)
    for (const auto& v : BoxesCratesVariants::GetAllVariants()) {
        auto gen = std::make_shared<BoxesCratesCargo>(v.id);
        m_Generators[v.id] = gen;
        m_Definitions[v.id] = gen->GetDef();
    }

    // 3. Liquid Bulk (5 variants)
    for (const auto& v : LiquidBulkVariants::GetAllVariants()) {
        auto gen = std::make_shared<LiquidBulkCargo>(v.id);
        m_Generators[v.id] = gen;
        m_Definitions[v.id] = gen->GetDef();
    }

    // 4. Raw Materials (5 variants)
    for (const auto& v : RawMaterialsVariants::GetAllVariants()) {
        auto gen = std::make_shared<RawMaterialsCargo>(v.id);
        m_Generators[v.id] = gen;
        m_Definitions[v.id] = gen->GetDef();
    }

    // 5. Containers & Bottles (5 variants)
    for (const auto& v : ContainersBottlesVariants::GetAllVariants()) {
        auto gen = std::make_shared<ContainersBottlesCargo>(v.id);
        m_Generators[v.id] = gen;
        m_Definitions[v.id] = gen->GetDef();
    }

    // 6. Specialty Industrial (4 variants)
    for (const auto& v : SpecialtyIndustrialVariants::GetAllVariants()) {
        auto gen = std::make_shared<SpecialtyIndustrialCargo>(v.id);
        m_Generators[v.id] = gen;
        m_Definitions[v.id] = gen->GetDef();
    }

    // 7. Perishable Goods (4 variants)
    for (const auto& v : PerishableGoodsVariants::GetAllVariants()) {
        auto gen = std::make_shared<PerishableGoodsCargo>(v.id);
        m_Generators[v.id] = gen;
        m_Definitions[v.id] = gen->GetDef();
    }

    m_FallbackDef.id = CargoID::SUGAR_BAG_50KG;
    m_FallbackDef.name = "Default Cargo Commodity";
    m_FallbackDef.category = "General Goods";
}

void CargoRegistry::RegisterCargo(std::shared_ptr<ICargo> generator) {
    if (!generator) return;
    std::lock_guard<std::mutex> lock(m_Mutex);
    CargoID id = generator->GetID();
    m_Generators[id] = generator;
    m_Definitions[id] = generator->GetDef();
}

std::shared_ptr<ICargo> CargoRegistry::GetCargo(CargoID id) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    auto it = m_Generators.find(id);
    if (it != m_Generators.end()) {
        return it->second;
    }
    return nullptr;
}

const CargoDef& CargoRegistry::GetDef(CargoID id) const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    auto it = m_Definitions.find(id);
    if (it != m_Definitions.end()) {
        return it->second;
    }
    return m_FallbackDef;
}

void CargoRegistry::SetDef(CargoID id, const CargoDef& def) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Definitions[id] = def;
    auto it = m_Generators.find(id);
    if (it != m_Generators.end()) {
        it->second->SetDef(def);
    }
}

std::shared_ptr<Mesh> CargoRegistry::RequestMesh(CargoID id, uint32_t lod) {
    auto cargo = GetCargo(id);
    if (cargo) {
        if (!cargo->GetCachedMesh()) {
            return cargo->GenerateMesh(lod);
        }
        return cargo->GetCachedMesh();
    }
    return nullptr;
}

VkImage CargoRegistry::RequestImage(CargoID id, uint32_t channel) {
    auto cargo = GetCargo(id);
    if (cargo) {
        if (!cargo->IsGenerated()) {
            cargo->GenerateTextures();
        }
        if (channel == 0) return cargo->GetAlbedoImage();
        if (channel == 1) return cargo->GetNormalImage();
        return cargo->GetOrmImage();
    }
    return VK_NULL_HANDLE;
}

bool CargoRegistry::HasCargo(CargoID id) const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_Generators.find(id) != m_Generators.end();
}

std::vector<CargoID> CargoRegistry::GetAllCargoIDs() const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    std::vector<CargoID> ids;
    ids.reserve(m_Generators.size());
    for (const auto& [id, _] : m_Generators) {
        ids.push_back(id);
    }
    return ids;
}

std::vector<CargoID> CargoRegistry::GetCargoByCategory(CargoCategory category) const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    std::vector<CargoID> ids;
    for (const auto& [id, _] : m_Generators) {
        if (GetCargoCategoryEnum(id) == category) {
            ids.push_back(id);
        }
    }
    return ids;
}

size_t CargoRegistry::GetTotalCommoditiesCount() const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_Generators.size();
}

} // namespace sphere::cargo
