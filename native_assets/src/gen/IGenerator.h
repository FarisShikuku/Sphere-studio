#pragma once

#include <cstdint>
#include <vector>

namespace procedural {

enum class GeneratorCategory : int32_t {
    Buildings = 0,
    Terrain = 1,
    Roads = 2,
    Props = 3,
    Water = 4,
    Vehicles = 5
};

struct GeneratorContext {
    uint32_t seed = 1337u;
    float lodLevel = 0.0f;
    float timeSeconds = 0.0f;
};

struct GeneratedMeshPayload {
    uint32_t vertexStride = 0;
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
    std::vector<float> vertexBuffer;
    std::vector<uint32_t> indexBuffer;
};

class IGenerator {
public:
    virtual ~IGenerator() = default;
    virtual const char* getName() const = 0;
    virtual GeneratorCategory getCategory() const = 0;
    virtual bool initialize(const GeneratorContext& ctx) = 0;
    virtual void update(float timeSeconds, float dt) = 0;
    virtual GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) = 0;
};

namespace buildings {
using procedural::GeneratorCategory;
using procedural::GeneratorContext;
using procedural::GeneratedMeshPayload;
using procedural::IGenerator;
} // namespace buildings

} // namespace procedural
