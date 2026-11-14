#pragma once
#include "TrailerDef.h"
#include "ICargo.h"
#include "scene/Mesh.h"
#include "scene/AABB.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <string>

namespace sphere::trailers {

/**
 * Base interface for all procedural trailer generators.
 * Supports lazy Vulkan buffer/image generation and dynamic cargo loading via CargoRegistry.
 */
class ITrailer {
public:
    virtual ~ITrailer() = default;

    virtual TrailerID GetID() const = 0;
    virtual const char* GetName() const = 0;
    virtual const char* GetCategory() const = 0;

    virtual const TrailerDef& GetDef() const = 0;
    virtual void SetDef(const TrailerDef& def) = 0;

    // Procedural Mesh Generation (Chassis + Superstructure + Shared RunningGear)
    virtual std::shared_ptr<Mesh> GenerateMesh(uint32_t lod = 0) = 0;
    virtual std::shared_ptr<Mesh> GetCachedMesh() const = 0;

    // Vulkan PBR Texture & Buffer Generation
    virtual bool GenerateTextures(VkDevice device = VK_NULL_HANDLE,
                                  VkQueue queue = VK_NULL_HANDLE,
                                  VkCommandPool pool = VK_NULL_HANDLE) = 0;
    virtual bool IsGenerated() const = 0;

    // Cargo Integration
    virtual bool LoadCargo(std::shared_ptr<cargo::ICargo> cargo, uint32_t quantity);
    virtual void ClearCargo();
    virtual std::shared_ptr<cargo::ICargo> GetLoadedCargo() const { return m_LoadedCargo; }
    virtual uint32_t GetLoadedCargoQuantity() const { return m_LoadedQuantity; }
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
    std::shared_ptr<cargo::ICargo> m_LoadedCargo = nullptr;
    uint32_t m_LoadedQuantity = 0;
};

} // namespace sphere::trailers
