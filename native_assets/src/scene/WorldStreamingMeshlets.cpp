#include "WorldStreamingMeshlets.h"
#include <algorithm>
#include <cmath>

namespace sphere::scene {

std::vector<MeshletCluster> WorldStreamingMeshlets::BuildMeshlets(const Mesh* mesh, uint32_t maxTris) {
    std::vector<MeshletCluster> clusters;
    if (!mesh || mesh->indices.size() < 3 || maxTris == 0) {
        return clusters;
    }

    const uint32_t totalTris = static_cast<uint32_t>(mesh->indices.size() / 3);
    const size_t nV = mesh->vertices.size();
    clusters.reserve((totalTris + maxTris - 1) / maxTris);

    for (uint32_t startTri = 0; startTri < totalTris; startTri += maxTris) {
        uint32_t count = std::min(maxTris, totalTris - startTri);
        MeshletCluster c;
        c.triangle_offset = startTri;
        c.triangle_count = count;

        Vec3 minP(1e9f, 1e9f, 1e9f);
        Vec3 maxP(-1e9f, -1e9f, -1e9f);
        uint32_t minVi = UINT32_MAX;

        for (uint32_t t = 0; t < count; ++t) {
            uint32_t base = (startTri + t) * 3;
            for (int k = 0; k < 3; ++k) {
                uint32_t vi = mesh->indices[base + k];
                if (vi >= nV) continue;
                minVi = std::min(minVi, vi);
                const Vec3& p = mesh->vertices[vi].position;
                minP.x = std::min(minP.x, p.x);
                minP.y = std::min(minP.y, p.y);
                minP.z = std::min(minP.z, p.z);
                maxP.x = std::max(maxP.x, p.x);
                maxP.y = std::max(maxP.y, p.y);
                maxP.z = std::max(maxP.z, p.z);
            }
        }

        c.vertex_offset = (minVi == UINT32_MAX) ? 0u : minVi;
        c.bound_center = (minP + maxP) * 0.5f;
        Vec3 halfExt = (maxP - minP) * 0.5f;
        c.bound_radius = halfExt.Length();
        clusters.push_back(c);
    }

    return clusters;
}

WorldStreamingTelemetry WorldStreamingMeshlets::EvaluateTelemetry(
    const Mesh* heroVehicleMesh,
    const Mesh* hitchedTrailerMesh,
    const Mesh* loadedCargoMesh,
    int32_t cargoQuantity,
    float distanceDrivenM
) {
    WorldStreamingTelemetry t;
    t.cell_size_m = kGridCellSizeM;
    const float safeDist = std::isfinite(distanceDrivenM) ? distanceDrivenM : 0.0f;
    t.grid_cell_x = static_cast<int32_t>(std::floor(std::clamp(safeDist / kGridCellSizeM, -1.0e7f, 1.0e7f)));
    t.grid_cell_z = 0;
    t.loaded_ring_cells = 9; // 3x3 active streaming ring (192m x 192m)
    t.max_tris_per_meshlet = static_cast<int32_t>(kMaxMeshletTris);

    auto countClusters = [](const Mesh* m) -> int32_t {
        if (!m || m->indices.size() < 3) return 0;
        uint32_t tris = static_cast<uint32_t>(m->indices.size() / 3);
        return static_cast<int32_t>((tris + kMaxMeshletTris - 1) / kMaxMeshletTris);
    };

    int32_t totalMeshlets = countClusters(heroVehicleMesh) + countClusters(hitchedTrailerMesh);
    if (loadedCargoMesh && cargoQuantity > 0) {
        int32_t unitMeshlets = countClusters(loadedCargoMesh);
        totalMeshlets += unitMeshlets * std::min(cargoQuantity, 48);
    }
    t.hero_meshlet_count = totalMeshlets;

    // Distant parked fleet & road props merged into 1 HLOD proxy batch per outer ring cell
    t.hlod_proxy_batches = 6;
    t.hlod_culled_tris = totalMeshlets * 92;
    return t;
}

} // namespace sphere::scene
