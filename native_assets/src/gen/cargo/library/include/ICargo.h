#pragma once
#include "CargoDef.h"
#include "scene/Mesh.h"
#include "scene/AABB.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <string>

namespace sphere::cargo {

/**
 * Base interface for procedural cargo generators.
 * Handles both procedural 3D mesh synthesis (via compute or parametric CPU/GPU pipeline)
 * and PBR texture maps (Albedo, Normal, ORM) with Vulkan resource management.
 */
class ICargo {
public:
    virtual ~ICargo() = default;

    virtual CargoID GetID() const = 0;
    virtual const char* GetName() const = 0;
    virtual const char* GetCategory() const = 0;

    virtual const CargoDef& GetDef() const = 0;
    virtual void SetDef(const CargoDef& def) = 0;

    // Procedural Mesh Generation
    virtual std::shared_ptr<Mesh> GenerateMesh(uint32_t lod = 0) = 0;
    virtual std::shared_ptr<Mesh> GetCachedMesh() const = 0;

    // Vulkan Texture & Buffer Generation
    virtual bool GenerateTextures(VkDevice device = VK_NULL_HANDLE, 
                                  VkQueue queue = VK_NULL_HANDLE, 
                                  VkCommandPool pool = VK_NULL_HANDLE) = 0;
    virtual bool IsGenerated() const = 0;

    // Vulkan Handles & Buffers
    virtual VkBuffer GetVertexBuffer() const = 0;
    virtual VkBuffer GetIndexBuffer() const = 0;
    virtual VkImage GetAlbedoImage() const = 0;
    virtual VkImage GetNormalImage() const = 0;
    virtual VkImage GetOrmImage() const = 0;

    virtual uint32_t GetVertexCount() const = 0;
    virtual uint32_t GetIndexCount() const = 0;
    virtual const AABB& GetBounds() const = 0;
};

} // namespace sphere::cargo
