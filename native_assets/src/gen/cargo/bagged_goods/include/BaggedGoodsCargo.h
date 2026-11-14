#pragma once
#include "../../library/include/ICargo.h"
#include "BaggedGoodsVariants.h"

namespace sphere::cargo {

class BaggedGoodsCargo : public ICargo {
public:
    explicit BaggedGoodsCargo(CargoID variantId = CargoID::SUGAR_BAG_50KG);
    ~BaggedGoodsCargo() override = default;

    CargoID GetID() const override { return m_Def.id; }
    const char* GetName() const override { return m_Def.name.c_str(); }
    const char* GetCategory() const override { return "Bagged Goods"; }

    const CargoDef& GetDef() const override { return m_Def; }
    void SetDef(const CargoDef& def) override { m_Def = def; m_CachedMesh.reset(); }

    std::shared_ptr<Mesh> GenerateMesh(uint32_t lod = 0) override;
    std::shared_ptr<Mesh> GetCachedMesh() const override { return m_CachedMesh; }

    bool GenerateTextures(VkDevice device = VK_NULL_HANDLE, 
                          VkQueue queue = VK_NULL_HANDLE, 
                          VkCommandPool pool = VK_NULL_HANDLE) override;
    bool IsGenerated() const override { return m_IsGenerated; }

    VkBuffer GetVertexBuffer() const override { return m_VertexBuffer; }
    VkBuffer GetIndexBuffer() const override { return m_IndexBuffer; }
    VkImage GetAlbedoImage() const override { return m_AlbedoImage; }
    VkImage GetNormalImage() const override { return m_NormalImage; }
    VkImage GetOrmImage() const override { return m_OrmImage; }

    uint32_t GetVertexCount() const override { return m_CachedMesh ? static_cast<uint32_t>(m_CachedMesh->vertices.size()) : 0; }
    uint32_t GetIndexCount() const override { return m_CachedMesh ? static_cast<uint32_t>(m_CachedMesh->indices.size()) : 0; }
    const AABB& GetBounds() const override { return m_Bounds; }

private:
    CargoDef m_Def;
    std::shared_ptr<Mesh> m_CachedMesh = nullptr;
    AABB m_Bounds;
    bool m_IsGenerated = false;

    VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
    VkBuffer m_IndexBuffer = VK_NULL_HANDLE;
    VkImage m_AlbedoImage = VK_NULL_HANDLE;
    VkImage m_NormalImage = VK_NULL_HANDLE;
    VkImage m_OrmImage = VK_NULL_HANDLE;
};

} // namespace sphere::cargo
