#pragma once
#include "VehicleDef.h"
#include "ITrailer.h"
#include "scene/Mesh.h"
#include "scene/AABB.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <string>

namespace sphere::vehicles {

/**
 * Base interface for all procedural C++/Vulkan lorry and bus generators.
 * Supports lazy mesh caching, algorithmic livery seeding, and 1:1 metric
 * fifth-wheel hitching to TrailerRegistry trailers.
 */
class IVehicle {
public:
    virtual ~IVehicle() = default;

    virtual VehicleID GetID() const = 0;
    virtual const char* GetName() const = 0;
    virtual const char* GetCategory() const = 0;

    virtual const VehicleDef& GetDef() const = 0;
    virtual void SetDef(const VehicleDef& def) = 0;

    virtual std::shared_ptr<Mesh> GenerateMesh(uint32_t lod = 0) = 0;
    virtual std::shared_ptr<Mesh> GetCachedMesh() const = 0;

    virtual bool GenerateTextures(VkDevice device = VK_NULL_HANDLE,
                                  VkQueue queue = VK_NULL_HANDLE,
                                  VkCommandPool pool = VK_NULL_HANDLE) = 0;
    virtual bool IsGenerated() const = 0;

    // Livery & Random Seed Integration (e.g., Matatu algorithmic graffiti)
    virtual void SetLivery(LiveryID livery);
    virtual void SetRandomLivery(uint32_t seed);

    // Fifth-wheel Tractor <-> Semi-Trailer Coupling
    virtual bool CanHitchTrailer() const;
    virtual bool Hitch(std::shared_ptr<trailers::ITrailer> trailer);
    virtual void Unhitch();
    virtual std::shared_ptr<trailers::ITrailer> GetHitchedTrailer() const { return m_HitchedTrailer; }
    virtual float GetTotalMassKg() const;

    // Vulkan Handles
    virtual VkBuffer GetVertexBuffer() const = 0;
    virtual VkBuffer GetIndexBuffer() const = 0;
    virtual VkImage GetAlbedoImage() const = 0;
    virtual VkImage GetNormalImage() const = 0;
    virtual VkImage GetOrmImage() const = 0;

    virtual uint32_t GetVertexCount() const = 0;
    virtual uint32_t GetIndexCount() const = 0;
    virtual const AABB& GetBounds() const = 0;

protected:
    std::shared_ptr<trailers::ITrailer> m_HitchedTrailer = nullptr;
};

} // namespace sphere::vehicles
