#pragma once
#include "SurfaceDef.h"
#include <unordered_map>
#include <vector>
#include <mutex>

namespace sphere {

class SurfaceRegistry {
public:
    static SurfaceRegistry& Get();

    void Initialize();
    const SurfaceDef& GetSurface(const std::string& id) const;
    void RegisterSurface(const SurfaceDef& surface);
    std::vector<SurfaceDef> GetAllSurfaces() const;
    std::vector<SurfaceDef> GetSurfacesByCategory(const std::string& category) const;

private:
    SurfaceRegistry();
    ~SurfaceRegistry() = default;

    void RegisterDefaultSurfaces();

    mutable std::mutex m_Mutex;
    std::unordered_map<std::string, SurfaceDef> m_Surfaces;
    SurfaceDef m_Fallback;
};

} // namespace sphere
