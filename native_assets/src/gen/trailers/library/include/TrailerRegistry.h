#pragma once
#include "TrailerID.h"
#include "TrailerDef.h"
#include "ITrailer.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>

namespace sphere::trailers {

/**
 * Singleton Trailer Registry.
 * Provides lazy procedural generation & caching of trailer meshes and PBR textures,
 * plus convenient static TrailerRegistry::Get(TrailerID) spawning.
 */
class TrailerRegistry {
public:
    static TrailerRegistry& Get();
    static std::shared_ptr<ITrailer> Get(TrailerID id) {
        return Get().GetTrailer(id);
    }

    void Initialize();
    void RegisterTrailer(std::shared_ptr<ITrailer> generator);
    std::shared_ptr<ITrailer> GetTrailer(TrailerID id);
    const TrailerDef& GetDef(TrailerID id) const;
    void SetDef(TrailerID id, const TrailerDef& def);

    std::shared_ptr<Mesh> RequestMesh(TrailerID id, uint32_t lod = 0);
    VkImage RequestImage(TrailerID id, uint32_t channel = 0);

    bool HasTrailer(TrailerID id) const;
    std::vector<TrailerID> GetAllTrailerIDs() const;
    std::vector<TrailerID> GetTrailersByCategory(TrailerCategory category) const;

    size_t GetTotalTrailersCount() const;

private:
    TrailerRegistry();
    ~TrailerRegistry() = default;

    void RegisterDefaultGenerators();

    mutable std::mutex m_Mutex;
    std::unordered_map<TrailerID, std::shared_ptr<ITrailer>> m_Generators;
    std::unordered_map<TrailerID, TrailerDef> m_Definitions;
    TrailerDef m_FallbackDef;
};

} // namespace sphere::trailers
