#include "TrailerRegistry.h"
#include "FlatbedTrailer.h"
#include "TankerTrailer.h"
#include "BoxVanTrailer.h"
#include "ChassisTrailer.h"
#include "LoggingTrailer.h"
#include "CurtainsiderTrailer.h"
#include "BulkHopperTrailer.h"
#include "CarCarrierTrailer.h"
#include "HeavyHaulageTrailer.h"
#include "DollyTrailer.h"
#include "RecreationalTrailer.h"
#include "SpecialtyTrailer.h"

namespace sphere::trailers {

TrailerRegistry& TrailerRegistry::Get() {
    static TrailerRegistry instance;
    return instance;
}

TrailerRegistry::TrailerRegistry() {
    m_FallbackDef.name = "Fallback Trailer";
    m_FallbackDef.category = "Flatbeds";
    RegisterDefaultGenerators();
}

void TrailerRegistry::Initialize() {
    std::lock_guard<std::mutex> lock(m_Mutex);
    if (m_Generators.empty()) {
        RegisterDefaultGenerators();
    }
}

void TrailerRegistry::RegisterDefaultGenerators() {
    for (uint32_t i = static_cast<uint32_t>(TrailerID::FLATBED_STD); i <= static_cast<uint32_t>(TrailerID::FLATBED_SIDE_LOADER); ++i) {
        auto id = static_cast<TrailerID>(i);
        auto gen = std::make_shared<FlatbedTrailer>(id);
        m_Generators[id] = gen;
        m_Definitions[id] = gen->GetDef();
    }
    for (uint32_t i = static_cast<uint32_t>(TrailerID::TANKER_WATER); i <= static_cast<uint32_t>(TrailerID::TANKER_POWDER); ++i) {
        auto id = static_cast<TrailerID>(i);
        auto gen = std::make_shared<TankerTrailer>(id);
        m_Generators[id] = gen;
        m_Definitions[id] = gen->GetDef();
    }
    for (uint32_t i = static_cast<uint32_t>(TrailerID::DRY_VAN_53FT); i <= static_cast<uint32_t>(TrailerID::BOX_DOUBLE_DECK); ++i) {
        auto id = static_cast<TrailerID>(i);
        auto gen = std::make_shared<BoxVanTrailer>(id);
        m_Generators[id] = gen;
        m_Definitions[id] = gen->GetDef();
    }
    for (uint32_t i = static_cast<uint32_t>(TrailerID::CHASSIS_20FT); i <= static_cast<uint32_t>(TrailerID::CHASSIS_TIPPING); ++i) {
        auto id = static_cast<TrailerID>(i);
        auto gen = std::make_shared<ChassisTrailer>(id);
        m_Generators[id] = gen;
        m_Definitions[id] = gen->GetDef();
    }
    for (uint32_t i = static_cast<uint32_t>(TrailerID::LOGGING_POLE_TRAILER); i <= static_cast<uint32_t>(TrailerID::LOGGING_EXTENDABLE_LOG); ++i) {
        auto id = static_cast<TrailerID>(i);
        auto gen = std::make_shared<LoggingTrailer>(id);
        m_Generators[id] = gen;
        m_Definitions[id] = gen->GetDef();
    }
    for (uint32_t i = static_cast<uint32_t>(TrailerID::CURTAINSIDER_STANDARD); i <= static_cast<uint32_t>(TrailerID::CURTAINSIDER_SIDE_LOADING); ++i) {
        auto id = static_cast<TrailerID>(i);
        auto gen = std::make_shared<CurtainsiderTrailer>(id);
        m_Generators[id] = gen;
        m_Definitions[id] = gen->GetDef();
    }
    for (uint32_t i = static_cast<uint32_t>(TrailerID::HOPPER_GRAIN); i <= static_cast<uint32_t>(TrailerID::HOPPER_SIDE_TIPPER); ++i) {
        auto id = static_cast<TrailerID>(i);
        auto gen = std::make_shared<BulkHopperTrailer>(id);
        m_Generators[id] = gen;
        m_Definitions[id] = gen->GetDef();
    }
    for (uint32_t i = static_cast<uint32_t>(TrailerID::CARCARRIER_SINGLE_DECK); i <= static_cast<uint32_t>(TrailerID::CARCARRIER_ENCLOSED); ++i) {
        auto id = static_cast<TrailerID>(i);
        auto gen = std::make_shared<CarCarrierTrailer>(id);
        m_Generators[id] = gen;
        m_Definitions[id] = gen->GetDef();
    }
    for (uint32_t i = static_cast<uint32_t>(TrailerID::HEAVY_LOWBOY); i <= static_cast<uint32_t>(TrailerID::HEAVY_TUNNEL_TRANSPORTER); ++i) {
        auto id = static_cast<TrailerID>(i);
        auto gen = std::make_shared<HeavyHaulageTrailer>(id);
        m_Generators[id] = gen;
        m_Definitions[id] = gen->GetDef();
    }
    for (uint32_t i = static_cast<uint32_t>(TrailerID::DOLLY_CONVERTER); i <= static_cast<uint32_t>(TrailerID::DOLLY_JEEP); ++i) {
        auto id = static_cast<TrailerID>(i);
        auto gen = std::make_shared<DollyTrailer>(id);
        m_Generators[id] = gen;
        m_Definitions[id] = gen->GetDef();
    }
    for (uint32_t i = static_cast<uint32_t>(TrailerID::RECREATION_BOAT_TRAILER); i <= static_cast<uint32_t>(TrailerID::RECREATION_UTILITY_TRAILER); ++i) {
        auto id = static_cast<TrailerID>(i);
        auto gen = std::make_shared<RecreationalTrailer>(id);
        m_Generators[id] = gen;
        m_Definitions[id] = gen->GetDef();
    }
    for (uint32_t i = static_cast<uint32_t>(TrailerID::SPECIALTY_LIVESTOCK); i <= static_cast<uint32_t>(TrailerID::SPECIALTY_ARMORED_CAR); ++i) {
        auto id = static_cast<TrailerID>(i);
        auto gen = std::make_shared<SpecialtyTrailer>(id);
        m_Generators[id] = gen;
        m_Definitions[id] = gen->GetDef();
    }
}

void TrailerRegistry::RegisterTrailer(std::shared_ptr<ITrailer> generator) {
    if (!generator) return;
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Generators[generator->GetID()] = generator;
    m_Definitions[generator->GetID()] = generator->GetDef();
}

std::shared_ptr<ITrailer> TrailerRegistry::GetTrailer(TrailerID id) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    auto it = m_Generators.find(id);
    if (it != m_Generators.end()) return it->second;
    return nullptr;
}

const TrailerDef& TrailerRegistry::GetDef(TrailerID id) const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    auto it = m_Definitions.find(id);
    if (it != m_Definitions.end()) return it->second;
    return m_FallbackDef;
}

void TrailerRegistry::SetDef(TrailerID id, const TrailerDef& def) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Definitions[id] = def;
    auto it = m_Generators.find(id);
    if (it != m_Generators.end()) {
        it->second->SetDef(def);
    }
}

std::shared_ptr<Mesh> TrailerRegistry::RequestMesh(TrailerID id, uint32_t lod) {
    std::shared_ptr<ITrailer> gen = GetTrailer(id);
    if (!gen) return nullptr;
    if (gen->GetCachedMesh() && lod == 0) {
        return gen->GetCachedMesh();
    }
    return gen->GenerateMesh(lod);
}

VkImage TrailerRegistry::RequestImage(TrailerID id, uint32_t channel) {
    std::shared_ptr<ITrailer> gen = GetTrailer(id);
    if (!gen) return VK_NULL_HANDLE;
    if (!gen->IsGenerated()) {
        gen->GenerateTextures();
    }
    if (channel == 0) return gen->GetAlbedoImage();
    if (channel == 1) return gen->GetNormalImage();
    return gen->GetOrmImage();
}

bool TrailerRegistry::HasTrailer(TrailerID id) const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_Generators.find(id) != m_Generators.end();
}

std::vector<TrailerID> TrailerRegistry::GetAllTrailerIDs() const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    std::vector<TrailerID> ids;
    ids.reserve(m_Generators.size());
    for (uint32_t i = 0; i < static_cast<uint32_t>(TrailerID::COUNT); ++i) {
        auto id = static_cast<TrailerID>(i);
        if (m_Generators.count(id)) ids.push_back(id);
    }
    return ids;
}

std::vector<TrailerID> TrailerRegistry::GetTrailersByCategory(TrailerCategory category) const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    std::vector<TrailerID> ids;
    for (uint32_t i = 0; i < static_cast<uint32_t>(TrailerID::COUNT); ++i) {
        auto id = static_cast<TrailerID>(i);
        if (m_Generators.count(id) && GetTrailerCategoryEnum(id) == category) {
            ids.push_back(id);
        }
    }
    return ids;
}

size_t TrailerRegistry::GetTotalTrailersCount() const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_Generators.size();
}

} // namespace sphere::trailers
