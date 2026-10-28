#pragma once
#include "MaterialID.h"
#include "MaterialDef.h"
#include "GeneratorContext.h"
#include <string>
#include <cstdint>

namespace sphere {

enum class MaterialChannel : uint32_t {
    Albedo = 0,
    Normal = 1,
    ORM = 2,            // Packed Occlusion (R), Roughness (G), Metallic (B)
    Height = 3,         // Displacement / Parallax height
    Subsurface = 4,     // SSS translucency mask & mean free path
    Refraction = 5,     // Transmission & IOR map
    Emissive = 6,       // HDR self-illumination
    COUNT
};

class IMaterial {
public:
    virtual ~IMaterial() = default;

    virtual MaterialID GetID() const = 0;
    virtual const char* GetName() const = 0;
    virtual const char* GetCategory() const = 0;
    virtual const MaterialDef& GetDef() const = 0;
    virtual void SetDef(const MaterialDef& def) = 0;

    // Procedural texture baking / compute dispatch
    virtual bool GenerateTextures(GeneratorContext& context) = 0;
    virtual uint32_t GetTextureIndex(MaterialChannel channel) const = 0;
    virtual bool IsGenerated() const = 0;
};

} // namespace sphere
