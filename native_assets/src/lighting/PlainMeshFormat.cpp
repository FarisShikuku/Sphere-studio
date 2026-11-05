#include "PlainMeshFormat.h"
#include <cstring>
#include <cmath>
#include <algorithm>

namespace sphere::lighting {

static const char kPlainMagic[8] = {'P', 'L', 'A', 'I', 'N', 'S', 'D', 'F'};

std::vector<uint8_t> PlainMeshFormat::Serialize(const PlainMeshData& data) {
    size_t headerSize = sizeof(PlainHeader);
    size_t vertSize = data.vertices.size() * sizeof(PlainVertex);
    size_t idxSize = data.indices.size() * sizeof(uint32_t);
    size_t sdfSize = data.sdfDistanceField.size() * sizeof(float);

    size_t totalBytes = headerSize + vertSize + idxSize + sdfSize;
    std::vector<uint8_t> buffer(totalBytes);

    uint8_t* ptr = buffer.data();

    // Copy Header
    std::memcpy(ptr, &data.header, headerSize);
    ptr += headerSize;

    // Copy Vertices
    if (vertSize > 0) {
        std::memcpy(ptr, data.vertices.data(), vertSize);
        ptr += vertSize;
    }

    // Copy Indices
    if (idxSize > 0) {
        std::memcpy(ptr, data.indices.data(), idxSize);
        ptr += idxSize;
    }

    // Copy SDF volume
    if (sdfSize > 0) {
        std::memcpy(ptr, data.sdfDistanceField.data(), sdfSize);
    }

    return buffer;
}

bool PlainMeshFormat::Deserialize(const uint8_t* bytes, size_t size, PlainMeshData& outData) {
    if (!bytes || size < sizeof(PlainHeader)) {
        return false;
    }

    const PlainHeader* hdr = reinterpret_cast<const PlainHeader*>(bytes);
    if (std::memcmp(hdr->magic, kPlainMagic, 8) != 0) {
        return false;
    }

    outData.header = *hdr;
    const uint8_t* ptr = bytes + sizeof(PlainHeader);

    outData.vertices.resize(hdr->vertexCount);
    size_t vertBytes = hdr->vertexCount * sizeof(PlainVertex);
    if (ptr + vertBytes > bytes + size) return false;
    std::memcpy(outData.vertices.data(), ptr, vertBytes);
    ptr += vertBytes;

    outData.indices.resize(hdr->indexCount);
    size_t idxBytes = hdr->indexCount * sizeof(uint32_t);
    if (ptr + idxBytes > bytes + size) return false;
    std::memcpy(outData.indices.data(), ptr, idxBytes);
    ptr += idxBytes;

    size_t sdfTotal = hdr->sdfDimX * hdr->sdfDimY * hdr->sdfDimZ;
    outData.sdfDistanceField.resize(sdfTotal);
    size_t sdfBytes = sdfTotal * sizeof(float);
    if (ptr + sdfBytes > bytes + size) return false;
    std::memcpy(outData.sdfDistanceField.data(), ptr, sdfBytes);

    return true;
}

PlainMeshData PlainMeshFormat::GenerateReferenceMeshWithSdf(const std::string& meshType, uint32_t sdfRes) {
    PlainMeshData data;
    std::memcpy(data.header.magic, kPlainMagic, 8);
    data.header.version = 1;
    data.header.sdfDimX = sdfRes;
    data.header.sdfDimY = sdfRes;
    data.header.sdfDimZ = sdfRes;

    // Build geometry (cube reference: 1.2m size)
    float halfSize = 0.6f;
    data.header.boundsMin[0] = -halfSize;
    data.header.boundsMin[1] = 0.0f;
    data.header.boundsMin[2] = -halfSize;
    data.header.boundsMax[0] = halfSize;
    data.header.boundsMax[1] = halfSize * 2.0f;
    data.header.boundsMax[2] = halfSize;
    data.header.voxelSpacing = (halfSize * 2.4f) / static_cast<float>(sdfRes);

    // 8 cube corners -> 24 vertices for hard normals
    data.vertices.reserve(24);
    // Face normals: +Z, -Z, +X, -X, +Y, -Y
    auto addQuad = [&](const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, const Vec3& n) {
        uint32_t baseIdx = static_cast<uint32_t>(data.vertices.size());
        PlainVertex v0{{p0.x, p0.y, p0.z}, {n.x, n.y, n.z}, {1, 0, 0, 1}, {0, 0}};
        PlainVertex v1{{p1.x, p1.y, p1.z}, {n.x, n.y, n.z}, {1, 0, 0, 1}, {1, 0}};
        PlainVertex v2{{p2.x, p2.y, p2.z}, {n.x, n.y, n.z}, {1, 0, 0, 1}, {1, 1}};
        PlainVertex v3{{p3.x, p3.y, p3.z}, {n.x, n.y, n.z}, {1, 0, 0, 1}, {0, 1}};
        data.vertices.push_back(v0);
        data.vertices.push_back(v1);
        data.vertices.push_back(v2);
        data.vertices.push_back(v3);
        data.indices.push_back(baseIdx + 0);
        data.indices.push_back(baseIdx + 1);
        data.indices.push_back(baseIdx + 2);
        data.indices.push_back(baseIdx + 0);
        data.indices.push_back(baseIdx + 2);
        data.indices.push_back(baseIdx + 3);
    };

    float y0 = 0.0f, y1 = 1.2f;
    // Front (+Z)
    addQuad({-halfSize, y0, halfSize}, {halfSize, y0, halfSize}, {halfSize, y1, halfSize}, {-halfSize, y1, halfSize}, {0, 0, 1});
    // Back (-Z)
    addQuad({halfSize, y0, -halfSize}, {-halfSize, y0, -halfSize}, {-halfSize, y1, -halfSize}, {halfSize, y1, -halfSize}, {0, 0, -1});
    // Right (+X)
    addQuad({halfSize, y0, halfSize}, {halfSize, y0, -halfSize}, {halfSize, y1, -halfSize}, {halfSize, y1, halfSize}, {1, 0, 0});
    // Left (-X)
    addQuad({-halfSize, y0, -halfSize}, {-halfSize, y0, halfSize}, {-halfSize, y1, halfSize}, {-halfSize, y1, -halfSize}, {-1, 0, 0});
    // Top (+Y)
    addQuad({-halfSize, y1, halfSize}, {halfSize, y1, halfSize}, {halfSize, y1, -halfSize}, {-halfSize, y1, -halfSize}, {0, 1, 0});
    // Bottom (-Y)
    addQuad({-halfSize, y0, -halfSize}, {halfSize, y0, -halfSize}, {halfSize, y0, halfSize}, {-halfSize, y0, halfSize}, {0, -1, 0});

    data.header.vertexCount = static_cast<uint32_t>(data.vertices.size());
    data.header.indexCount = static_cast<uint32_t>(data.indices.size());

    // Bake exact 3D Signed Distance Field for the Box
    size_t totalCells = sdfRes * sdfRes * sdfRes;
    data.sdfDistanceField.resize(totalCells);

    for (uint32_t z = 0; z < sdfRes; ++z) {
        float pz = -halfSize * 1.2f + (static_cast<float>(z) + 0.5f) * data.header.voxelSpacing;
        for (uint32_t y = 0; y < sdfRes; ++y) {
            float py = -0.1f + (static_cast<float>(y) + 0.5f) * data.header.voxelSpacing;
            for (uint32_t x = 0; x < sdfRes; ++x) {
                float px = -halfSize * 1.2f + (static_cast<float>(x) + 0.5f) * data.header.voxelSpacing;

                // Signed distance to box centered at (0, 0.6, 0) with half-extent (0.6, 0.6, 0.6)
                Vec3 d = Vec3{std::abs(px) - halfSize, std::abs(py - 0.6f) - halfSize, std::abs(pz) - halfSize};
                Vec3 outside{std::max(d.x, 0.0f), std::max(d.y, 0.0f), std::max(d.z, 0.0f)};
                float dist = outside.Length() + std::min(std::max({d.x, d.y, d.z}), 0.0f);

                size_t idx = z * (sdfRes * sdfRes) + y * sdfRes + x;
                data.sdfDistanceField[idx] = dist;
            }
        }
    }

    return data;
}

} // namespace sphere::lighting
