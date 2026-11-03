#pragma once
#include "MaterialID.h"
#include "scene/Transform.h"
#include <string>
namespace sphere {
/**
 * Physical Material Definition adhering to Sascha Willems Vulkan PBR conventions
 * (Cook-Torrance microfacet BRDF, GGX distribution, Fresnel-Schlick, Snell's law refraction)
 */
struct MaterialDef {
    MaterialID id = MaterialID::BRICK_RED;
    std::string name;
    std::string category;
    // Base PBR Properties (Sascha Willems pbrbasic / pbribl)
    Vec4 baseColorFactor{1.0f, 1.0f, 1.0f, 1.0f}; // RGBA Albedo / Tint
    float metallic = 0.0f;                        // 0.0 = Dielectric, 1.0 = Pure Metal
    float roughness = 0.5f;                       // GGX roughness [0.0 = smooth mirror, 1.0 = matte]
    float occlusionStrength = 1.0f;               // Ambient occlusion factor
    // Optical & Refraction Properties (Sascha Willems raytracingtextures / refraction)
    float transmission = 0.0f;                    // 0.0 = Opaque, 1.0 = Fully transmissive/refractive
    float ior = 1.0f;                             // Index of Refraction (Air=1.0, Water=1.333, Glass=1.52, Diamond=2.42)
    float dispersion = 0.0f;                      // Chromatic aberration / Abbe dispersion
    float refractionRoughness = 0.0f;             // Frosted / blurred refraction spread
    Vec3 absorptionColor{1.0f, 1.0f, 1.0f};       // Beer-Lambert volumetric absorption (I = I0 * e^(-sigma*d))
    float absorptionDensity = 0.0f;
    // Subsurface Scattering (Foliage, Leaves, Skin, Wax)
    float subsurface = 0.0f;                      // SSS blending weight [0.0 - 1.0]
    Vec3 subsurfaceColor{0.8f, 0.9f, 0.4f};       // SSS internal scattering tint
    float subsurfaceRadius = 1.0f;                // Mean free path scattering radius in mm
    // Surface Perturbation & Detail
    float normalScale = 1.0f;                     // Normal map perturbation depth
    float displacementDepth = 0.02f;              // Parallax / tessellation height scale (meters)
    float anisotropy = 0.0f;                      // Anisotropic highlight stretch (brushed metals)
    // Representation Mode (Surface texture vs 3D Instanced geometry)
    SurfaceMode surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;
    float tileScaleMeters = 1.0f;                 // World-space UV tiling scale (e.g. 0.9m for brick)
    std::string textureAssetPath;                 // Optional local image or procedural shader seed
    // Bindless Heap Indices (Vulkan descriptor indexing)
    uint32_t albedoTexIndex = 0;
    uint32_t normalTexIndex = 0;
    uint32_t ormTexIndex = 0;
    uint32_t emissiveTexIndex = 0;
};
} // namespace spher
