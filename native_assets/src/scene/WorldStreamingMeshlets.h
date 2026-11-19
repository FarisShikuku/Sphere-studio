#pragma once

#include "Mesh.h"
#include <cstdint>
#include <vector>

namespace sphere::scene {

struct MeshletCluster {
    uint32_t vertex_offset = 0;
    uint32_t triangle_offset = 0;
    uint32_t triangle_count = 0; // Capped at <= 126 triangles per Nanite-style Vulkan cluster
    Vec3 bound_center{0.0f, 0.0f, 0.0f};
    float bound_radius = 0.0f;
};

struct WorldStreamingTelemetry {
    int32_t grid_cell_x = 0;
    int32_t grid_cell_z = 0;
    int32_t loaded_ring_cells = 9;     // 3x3 active streaming ring of 64m cells around player vehicle
    float cell_size_m = 64.0f;
    int32_t hero_meshlet_count = 0;    // Active hero vehicle + hitched trailer + cargo meshlets
    int32_t max_tris_per_meshlet = 126;
    int32_t hlod_proxy_batches = 0;    // Merged distant fleet proxy draw batches
    int32_t hlod_culled_tris = 0;      // Poly reduction via HLOD fleet proxies
};

class WorldStreamingMeshlets {
public:
    static constexpr uint32_t kMaxMeshletTris = 126u;
    static constexpr float kGridCellSizeM = 64.0f;

    static std::vector<MeshletCluster> BuildMeshlets(const Mesh* mesh, uint32_t maxTris = kMaxMeshletTris);

    static WorldStreamingTelemetry EvaluateTelemetry(
        const Mesh* heroVehicleMesh,
        const Mesh* hitchedTrailerMesh,
        const Mesh* loadedCargoMesh,
        int32_t cargoQuantity,
        float distanceDrivenM
    );
};

} // namespace sphere::scene
