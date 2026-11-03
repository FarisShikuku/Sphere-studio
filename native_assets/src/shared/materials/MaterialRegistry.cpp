#include "MaterialRegistry.h"
#include <algorithm>
namespace sphere {
MaterialRegistry& MaterialRegistry::Get() {
    static MaterialRegistry instance;
    return instance;
}
MaterialRegistry::MaterialRegistry() {
    RegisterDefaultPresets();
}
void MaterialRegistry::Initialize() {
    std::lock_guard<std::mutex> lock(m_Mutex);
    RegisterDefaultPresets();
}
const MaterialDef& MaterialRegistry::GetMaterial(MaterialID id) const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    auto it = m_Materials.find(id);
    if (it != m_Materials.end()) {
        return it->second;
    }
    return m_Fallback;
}
void MaterialRegistry::SetMaterial(MaterialID id, const MaterialDef& def) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Materials[id] = def;
}
bool MaterialRegistry::HasMaterial(MaterialID id) const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_Materials.find(id) != m_Materials.end();
}
std::vector<MaterialID> MaterialRegistry::GetAllMaterialIDs() const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    std::vector<MaterialID> result;
    result.reserve(m_Materials.size());
    for (const auto& pair : m_Materials) {
        result.push_back(pair.first);
    }
    return result;
}
std::vector<MaterialID> MaterialRegistry::GetMaterialsByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    std::vector<MaterialID> result;
    for (const auto& pair : m_Materials) {
        if (pair.second.category == category) {
            result.push_back(pair.first);
        }
    }
    return result;
}
void MaterialRegistry::RegisterDefaultPresets() {
    m_Fallback.id = MaterialID::BRICK_RED;
    m_Fallback.name = "Default Neutral Material";
    m_Fallback.category = "Fallback";
    m_Fallback.baseColorFactor = Vec4{0.8f, 0.8f, 0.8f, 1.0f};
    m_Fallback.metallic = 0.0f;
    m_Fallback.roughness = 0.5f;
    m_Fallback.ior = 1.5f;
    // 1. Rusticated Red Brick (From user attachment 900mm stretcher)
    MaterialDef brickRusticated;
    brickRusticated.id = MaterialID::BRICK_RUSTICATED;
    brickRusticated.name = GetMaterialName(MaterialID::BRICK_RUSTICATED);
    brickRusticated.category = GetMaterialCategory(MaterialID::BRICK_RUSTICATED);
    brickRusticated.baseColorFactor = Vec4{0.78f, 0.36f, 0.24f, 1.0f};
    brickRusticated.metallic = 0.0f;
    brickRusticated.roughness = 0.85f;
    brickRusticated.occlusionStrength = 1.0f;
    brickRusticated.ior = 1.53f;
    brickRusticated.normalScale = 1.2f;
    brickRusticated.displacementDepth = 0.025f;
    brickRusticated.tileScaleMeters = 0.90f;
    brickRusticated.surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;
    brickRusticated.textureAssetPath = "textures/rusticated-red-brick.jpg";
    m_Materials[MaterialID::BRICK_RUSTICATED] = brickRusticated;
    // 2. Mondo Grass (Surface Only - user explicit requirement)
    MaterialDef grassSurface;
    grassSurface.id = MaterialID::GRASS_SURFACE;
    grassSurface.name = GetMaterialName(MaterialID::GRASS_SURFACE);
    grassSurface.category = GetMaterialCategory(MaterialID::GRASS_SURFACE);
    grassSurface.baseColorFactor = Vec4{0.25f, 0.42f, 0.16f, 1.0f};
    grassSurface.metallic = 0.0f;
    grassSurface.roughness = 0.72f;
    grassSurface.occlusionStrength = 0.9f;
    grassSurface.ior = 1.45f;
    grassSurface.subsurface = 0.35f;
    grassSurface.subsurfaceColor = Vec3{0.35f, 0.58f, 0.18f};
    grassSurface.subsurfaceRadius = 1.2f;
    grassSurface.normalScale = 0.8f;
    grassSurface.displacementDepth = 0.01f;
    grassSurface.tileScaleMeters = 0.30f;
    grassSurface.surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;
    grassSurface.textureAssetPath = "textures/mondo-grass.jpg";
    m_Materials[MaterialID::GRASS_SURFACE] = grassSurface;
    // 3. Mondo Grass (3D Instanced Blades)
    MaterialDef grass3D;
    grass3D.id = MaterialID::GRASS_3D_BLADES;
    grass3D.name = GetMaterialName(MaterialID::GRASS_3D_BLADES);
    grass3D.category = GetMaterialCategory(MaterialID::GRASS_3D_BLADES);
    grass3D.baseColorFactor = Vec4{0.22f, 0.45f, 0.15f, 1.0f};
    grass3D.metallic = 0.0f;
    grass3D.roughness = 0.45f;
    grass3D.ior = 1.45f;
    grass3D.subsurface = 0.65f;
    grass3D.subsurfaceColor = Vec3{0.40f, 0.75f, 0.20f};
    grass3D.subsurfaceRadius = 2.5f;
    grass3D.surfaceMode = SurfaceMode::GEOMETRY_3D_INSTANCED;
    grass3D.tileScaleMeters = 0.30f;
    m_Materials[MaterialID::GRASS_3D_BLADES] = grass3D;
    // 4. Earth with Foliage (From user attachment 700mm)
    MaterialDef earthFoliage;
    earthFoliage.id = MaterialID::EARTH_FOLIAGE;
    earthFoliage.name = GetMaterialName(MaterialID::EARTH_FOLIAGE);
    earthFoliage.category = GetMaterialCategory(MaterialID::EARTH_FOLIAGE);
    earthFoliage.baseColorFactor = Vec4{0.42f, 0.32f, 0.21f, 1.0f};
    earthFoliage.metallic = 0.0f;
    earthFoliage.roughness = 0.90f;
    earthFoliage.occlusionStrength = 1.0f;
    earthFoliage.ior = 1.50f;
    earthFoliage.subsurface = 0.20f;
    earthFoliage.subsurfaceColor = Vec3{0.45f, 0.55f, 0.15f};
    earthFoliage.normalScale = 1.1f;
    earthFoliage.displacementDepth = 0.03f;
    earthFoliage.tileScaleMeters = 0.70f;
    earthFoliage.surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;
    earthFoliage.textureAssetPath = "textures/earth-foliage.jpg";
    m_Materials[MaterialID::EARTH_FOLIAGE] = earthFoliage;
    // 5. Refractive Architectural Glass (Sascha Willems refraction & Snell's law)
    MaterialDef glassClear;
    glassClear.id = MaterialID::GLASS_CLEAR;
    glassClear.name = GetMaterialName(MaterialID::GLASS_CLEAR);
    glassClear.category = GetMaterialCategory(MaterialID::GLASS_CLEAR);
    glassClear.baseColorFactor = Vec4{0.95f, 0.97f, 1.0f, 0.05f};
    glassClear.metallic = 0.0f;
    glassClear.roughness = 0.03f;
    glassClear.transmission = 0.98f;
    glassClear.ior = 1.52f; // Crown Glass
    glassClear.dispersion = 0.015f; // Chromatic aberration
    glassClear.absorptionColor = Vec3{0.88f, 0.96f, 0.94f};
    glassClear.absorptionDensity = 0.1f;
    m_Materials[MaterialID::GLASS_CLEAR] = glassClear;
    // 6. Frosted Glass
    MaterialDef glassFrosted;
    glassFrosted.id = MaterialID::GLASS_FROSTED;
    glassFrosted.name = GetMaterialName(MaterialID::GLASS_FROSTED);
    glassFrosted.category = GetMaterialCategory(MaterialID::GLASS_FROSTED);
    glassFrosted.baseColorFactor = Vec4{0.90f, 0.95f, 0.98f, 0.20f};
    glassFrosted.metallic = 0.0f;
    glassFrosted.roughness = 0.40f;
    glassFrosted.transmission = 0.85f;
    glassFrosted.ior = 1.52f;
    glassFrosted.refractionRoughness = 0.35f;
    glassFrosted.normalScale = 0.4f;
    m_Materials[MaterialID::GLASS_FROSTED] = glassFrosted;
    // 7. Water Surface (Refraction + Wave normal)
    MaterialDef waterSurface;
    waterSurface.id = MaterialID::WATER_SURFACE;
    waterSurface.name = GetMaterialName(MaterialID::WATER_SURFACE);
    waterSurface.category = GetMaterialCategory(MaterialID::WATER_SURFACE);
    waterSurface.baseColorFactor = Vec4{0.10f, 0.35f, 0.45f, 0.40f};
    waterSurface.metallic = 0.0f;
    waterSurface.roughness = 0.05f;
    waterSurface.transmission = 0.92f;
    waterSurface.ior = 1.333f; // Pure Water
    waterSurface.dispersion = 0.008f;
    waterSurface.absorptionColor = Vec3{0.05f, 0.40f, 0.55f};
    waterSurface.absorptionDensity = 0.3f;
    m_Materials[MaterialID::WATER_SURFACE] = waterSurface;
    // 8. Brushed Anodized Aluminum (Anisotropic)
    MaterialDef metalBrushed;
    metalBrushed.id = MaterialID::METAL_BRUSHED;
    metalBrushed.name = GetMaterialName(MaterialID::METAL_BRUSHED);
    metalBrushed.category = GetMaterialCategory(MaterialID::METAL_BRUSHED);
    metalBrushed.baseColorFactor = Vec4{0.91f, 0.92f, 0.93f, 1.0f};
    metalBrushed.metallic = 1.0f;
    metalBrushed.roughness = 0.25f;
    metalBrushed.anisotropy = 0.75f;
    metalBrushed.ior = 2.40f;
    m_Materials[MaterialID::METAL_BRUSHED] = metalBrushed;
    // 9. Glacial Ice (Refractive dielectric + SSS)
    MaterialDef iceGlacial;
    iceGlacial.id = MaterialID::ICE_GLACIAL;
    iceGlacial.name = GetMaterialName(MaterialID::ICE_GLACIAL);
    iceGlacial.category = GetMaterialCategory(MaterialID::ICE_GLACIAL);
    iceGlacial.baseColorFactor = Vec4{0.70f, 0.88f, 0.95f, 0.50f};
    iceGlacial.metallic = 0.0f;
    iceGlacial.roughness = 0.15f;
    iceGlacial.transmission = 0.80f;
    iceGlacial.ior = 1.309f;
    iceGlacial.subsurface = 0.45f;
    iceGlacial.subsurfaceColor = Vec3{0.40f, 0.78f, 0.92f};
    m_Materials[MaterialID::ICE_GLACIAL] = iceGlacial;
    // 10. Subsurface Human Skin
    MaterialDef skin;
    skin.id = MaterialID::SKIN_HUMAN;
    skin.name = GetMaterialName(MaterialID::SKIN_HUMAN);
    skin.category = GetMaterialCategory(MaterialID::SKIN_HUMAN);
    skin.baseColorFactor = Vec4{0.86f, 0.67f, 0.56f, 1.0f};
    skin.metallic = 0.0f;
    skin.roughness = 0.48f;
    skin.ior = 1.40f;
    skin.subsurface = 0.85f;
    skin.subsurfaceColor = Vec3{0.95f, 0.25f, 0.15f}; // Subdermal hemoglobin red
    skin.subsurfaceRadius = 3.8f;
    m_Materials[MaterialID::SKIN_HUMAN] = skin;
}
} // namespace spher
