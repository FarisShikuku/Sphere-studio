#pragma once
#include "scene/Transform.h"
#include <vector>
#include <string>
#include <cstdint>

namespace sphere::lighting {

/**
 * PlainMeshFormat - PlainRenderer's Custom Binary Mesh & SDF Container (.plain)
 *
 * In Gaukler/PlainRenderer, static meshes pack both triangle geometry and an
 * embedded continuous 3D Signed Distance Field (SDF) grid into a single binary file.
 * This allows Vulkan compute shaders to load distance fields directly into 3D textures (VkImage)
 * for real-time raymarching and Diffuse GI cone-tracing without runtime voxelization overhead.
 */

#pragma pack(push, 1)
struct PlainHeader {
    char magic[8];             // "PLAIN_SDF\0"
    uint32_t version;          // e.g. 1
    uint32_t vertexCount;
    uint32_t indexCount;
    uint32_t sdfDimX;          // SDF voxel resolution X
    uint32_t sdfDimY;          // SDF voxel resolution Y
    uint32_t sdfDimZ;          // SDF voxel resolution Z
    float boundsMin[3];
    float boundsMax[3];
    float voxelSpacing;        // In meters
};

struct PlainVertex {
    float pos[3];
    float normal[3];
    float tangent[4];
    float uv[2];
};
#pragma pack(pop)

struct PlainMeshData {
    PlainHeader header{};
    std::vector<PlainVertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<float> sdfDistanceField; // dimX * dimY * dimZ float values

    bool IsValid() const {
        return header.vertexCount > 0 && header.indexCount > 0 && !sdfDistanceField.empty();
    }
};

class PlainMeshFormat {
public:
    // Serializes mesh + SDF volume to binary memory buffer (.plain format)
    static std::vector<uint8_t> Serialize(const PlainMeshData& data);

    // Parses .plain binary memory buffer
    static bool Deserialize(const uint8_t* bytes, size_t size, PlainMeshData& outData);

    // Generates a calibrated reference mesh (Cube, Wall, Floor, Sphere) with baked SDF volume
    static PlainMeshData GenerateReferenceMeshWithSdf(const std::string& meshType, uint32_t sdfRes = 32);
};

} // namespace sphere::lighting
