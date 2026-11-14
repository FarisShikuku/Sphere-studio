#include "NativeMeshRasterizer.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace sphere::scene {

namespace {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kCreaseCosThreshold = 0.52f; // ~58.7 deg: smooth cylinders/spheres/tires, crisp 90 deg boxes

inline uint32_t PackRGBA8888(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    return (static_cast<uint32_t>(a) << 24) |
           (static_cast<uint32_t>(b) << 16) |
           (static_cast<uint32_t>(g) << 8) |
           static_cast<uint32_t>(r);
}

inline uint8_t ClampU8(float v) {
    int iv = static_cast<int>(v * 255.0f + 0.5f);
    if (iv < 0) return 0;
    if (iv > 255) return 255;
    return static_cast<uint8_t>(iv);
}

inline void AlphaBlendPixel(uint32_t& dst, float srcR, float srcG, float srcB, float alpha) {
    float a = std::clamp(alpha, 0.0f, 1.0f);
    if (a <= 0.004f) return;
    float invA = 1.0f - a;
    float dR = static_cast<float>(dst & 0xFFu) * (1.0f / 255.0f);
    float dG = static_cast<float>((dst >> 8) & 0xFFu) * (1.0f / 255.0f);
    float dB = static_cast<float>((dst >> 16) & 0xFFu) * (1.0f / 255.0f);
    dst = PackRGBA8888(
        ClampU8(srcR * a + dR * invA),
        ClampU8(srcG * a + dG * invA),
        ClampU8(srcB * a + dB * invA),
        255
    );
}

struct PreparedMesh {
    const Mesh* sourcePtr = nullptr;
    size_t vertexCount = 0;
    size_t indexCount = 0;
    uint32_t triCount = 0;
    std::vector<Vec3> cornerNormals; // 3 * triCount smooth crease-aware normals
    AABB bounds;
};

struct QuantKey {
    int64_t x, y, z;
    bool operator==(const QuantKey& o) const { return x == o.x && y == o.y && z == o.z; }
};

struct QuantKeyHash {
    size_t operator()(const QuantKey& k) const {
        uint64_t h = static_cast<uint64_t>(k.x) * 73856093ull ^
                     static_cast<uint64_t>(k.y) * 19349663ull ^
                     static_cast<uint64_t>(k.z) * 83492791ull;
        return static_cast<size_t>(h);
    }
};

PreparedMesh BuildPreparedMesh(const Mesh* mesh) {
    PreparedMesh pm;
    if (!mesh || mesh->vertices.empty() || mesh->indices.size() < 3) {
        return pm;
    }
    pm.sourcePtr = mesh;
    pm.vertexCount = mesh->vertices.size();
    pm.indexCount = mesh->indices.size();
    pm.triCount = static_cast<uint32_t>(mesh->indices.size() / 3);
    pm.bounds = mesh->bounds;

    std::vector<Vec3> faceNormals(pm.triCount, Vec3(0.0f, 1.0f, 0.0f));
    std::unordered_map<QuantKey, std::vector<uint32_t>, QuantKeyHash> posBuckets;
    posBuckets.reserve(mesh->vertices.size());

    auto makeKey = [](const Vec3& p) -> QuantKey {
        return QuantKey{
            static_cast<int64_t>(std::llround(p.x * 2000.0f)),
            static_cast<int64_t>(std::llround(p.y * 2000.0f)),
            static_cast<int64_t>(std::llround(p.z * 2000.0f))
        };
    };

    for (uint32_t t = 0; t < pm.triCount; ++t) {
        uint32_t i0 = mesh->indices[t * 3 + 0];
        uint32_t i1 = mesh->indices[t * 3 + 1];
        uint32_t i2 = mesh->indices[t * 3 + 2];
        if (i0 >= pm.vertexCount || i1 >= pm.vertexCount || i2 >= pm.vertexCount) continue;

        const Vec3& p0 = mesh->vertices[i0].position;
        const Vec3& p1 = mesh->vertices[i1].position;
        const Vec3& p2 = mesh->vertices[i2].position;
        Vec3 crossN = Vec3::Cross(p1 - p0, p2 - p0);
        float len = crossN.Length();
        faceNormals[t] = (len > 1e-7f) ? (crossN * (1.0f / len)) : Vec3(0.0f, 1.0f, 0.0f);

        posBuckets[makeKey(p0)].push_back(t);
        posBuckets[makeKey(p1)].push_back(t);
        posBuckets[makeKey(p2)].push_back(t);
    }

    pm.cornerNormals.resize(static_cast<size_t>(pm.triCount) * 3, Vec3(0.0f, 1.0f, 0.0f));
    for (uint32_t t = 0; t < pm.triCount; ++t) {
        const Vec3& fn = faceNormals[t];
        for (int c = 0; c < 3; ++c) {
            uint32_t vi = mesh->indices[t * 3 + c];
            if (vi >= pm.vertexCount) continue;
            const auto& neighbors = posBuckets[makeKey(mesh->vertices[vi].position)];
            Vec3 accum(0.0f, 0.0f, 0.0f);
            for (uint32_t nt : neighbors) {
                const Vec3& nfn = faceNormals[nt];
                if (Vec3::Dot(fn, nfn) > kCreaseCosThreshold) {
                    accum += nfn;
                }
            }
            float alen = accum.Length();
            pm.cornerNormals[t * 3 + c] = (alen > 1e-6f) ? (accum * (1.0f / alen)) : fn;
        }
    }
    return pm;
}

PreparedMesh GetPreparedMeshCopy(const Mesh* mesh) {
    static std::mutex sCacheMutex;
    static std::unordered_map<const Mesh*, PreparedMesh> sCache;
    std::lock_guard<std::mutex> lock(sCacheMutex);
    auto it = sCache.find(mesh);
    if (it != sCache.end() &&
        it->second.vertexCount == mesh->vertices.size() &&
        it->second.indexCount == mesh->indices.size()) {
        return it->second;
    }
    if (sCache.size() > 160) {
        sCache.clear();
    }
    PreparedMesh built = BuildPreparedMesh(mesh);
    sCache[mesh] = built;
    return built;
}

struct CameraXform {
    float cx = 0.0f, cy = 0.0f;
    float baseScale = 100.0f;
    float cosY = 1.0f, sinY = 0.0f;
    float cosX = 1.0f, sinX = 0.0f;
    float camDist = 3.8f;

    void ProjectPoint(
        float wx, float wy, float wz,
        float& sx, float& sy, float& invZ, float& viewZ
    ) const {
        float rx = wx * cosY - wz * sinY;
        float rz = wx * sinY + wz * cosY;
        float ry = wy * cosX - rz * sinX;
        float fz = wy * sinX + rz * cosX;
        float denom = std::max(0.45f, camDist - fz);
        invZ = 1.0f / denom;
        float persp = camDist * invZ;
        sx = cx + rx * baseScale * persp;
        sy = cy - ry * baseScale * persp;
        viewZ = fz;
    }

    Vec3 RotateNormalToView(const Vec3& n) const {
        float rx = n.x * cosY - n.z * sinY;
        float rz = n.x * sinY + n.z * cosY;
        float ry = n.y * cosX - rz * sinX;
        float fz = n.y * sinX + rz * cosX;
        return Vec3(rx, ry, fz).Normalized();
    }
};

struct FrameBufferView {
    uint32_t* pixels = nullptr;
    int32_t stridePixels = 0;
    int32_t width = 0;
    int32_t height = 0;
    std::vector<float> zBuffer;

    void Begin(uint32_t* px, int32_t stride, int32_t w, int32_t h) {
        pixels = px;
        stridePixels = stride;
        width = w;
        height = h;
        zBuffer.assign(static_cast<size_t>(w) * static_cast<size_t>(h), 0.0f);
    }

    void FillStudioBackground(float centerX, float centerY, float maxRadius,
                              float shadowCx, float shadowCy, float shadowRx, float shadowRy) {
        float invMaxR = 1.0f / std::max(1.0f, maxRadius);
        float invSRx2 = 1.0f / std::max(4.0f, shadowRx * shadowRx);
        float invSRy2 = 1.0f / std::max(4.0f, shadowRy * shadowRy);

        for (int32_t y = 0; y < height; ++y) {
            float dy = static_cast<float>(y) - centerY;
            float sdy = static_cast<float>(y) - shadowCy;
            float sdy2 = sdy * sdy * invSRy2;
            uint32_t* row = pixels + static_cast<size_t>(y) * stridePixels;

            for (int32_t x = 0; x < width; ++x) {
                float dx = static_cast<float>(x) - centerX;
                float distT = std::min(1.0f, std::sqrt(dx * dx + dy * dy) * invMaxR);
                float r, g, b;
                if (distT < 0.55f) {
                    float k = distT / 0.55f;
                    r = (30.0f + (13.0f - 30.0f) * k) / 255.0f;
                    g = (41.0f + (19.0f - 41.0f) * k) / 255.0f;
                    b = (59.0f + (31.0f - 59.0f) * k) / 255.0f;
                } else {
                    float k = (distT - 0.55f) / 0.45f;
                    r = (13.0f + (3.0f - 13.0f) * k) / 255.0f;
                    g = (19.0f + (5.0f - 19.0f) * k) / 255.0f;
                    b = (31.0f + (8.0f - 31.0f) * k) / 255.0f;
                }

                // Soft elliptical contact floor shadow
                if (sdy2 < 1.0f) {
                    float sdx = static_cast<float>(x) - shadowCx;
                    float e2 = sdx * sdx * invSRx2 + sdy2;
                    if (e2 < 1.0f) {
                        float falloff = 1.0f - std::sqrt(e2);
                        float dark = 1.0f - 0.82f * falloff * falloff;
                        r *= dark;
                        g *= dark;
                        b *= dark;
                    }
                }
                row[x] = PackRGBA8888(ClampU8(r), ClampU8(g), ClampU8(b), 255);
            }
        }
    }

    inline void ShadeCornerPBR(
        const Vec3& albedo,
        const Vec3& viewNormal,
        float roughness,
        float metallic,
        float& outR, float& outG, float& outB
    ) const {
        // Key light (upper-right-front), cool fill (left), rim light
        static const Vec3 kKeyDir = Vec3(0.52f, 0.72f, 0.46f).Normalized();
        static const Vec3 kFillDir = Vec3(-0.62f, 0.25f, 0.38f).Normalized();
        static const Vec3 kViewDir = Vec3(0.0f, 0.0f, 1.0f);
        static const Vec3 kHalfVec = (kKeyDir + kViewDir).Normalized();

        float nDotL = std::max(0.0f, Vec3::Dot(viewNormal, kKeyDir));
        float fillDiff = std::max(0.0f, Vec3::Dot(viewNormal, kFillDir)) * 0.24f;
        float hemi = 0.20f + 0.14f * std::clamp(viewNormal.y, -1.0f, 1.0f);

        float nDotH = std::max(0.0f, Vec3::Dot(viewNormal, kHalfVec));
        float shininess = std::max(4.0f, 96.0f * (1.0f - roughness) * (1.0f - roughness));
        float spec = std::pow(nDotH, shininess) * (0.20f + 0.68f * (1.0f - roughness));

        // Rim grazing highlight
        float fresnel = std::pow(std::clamp(1.0f - std::abs(viewNormal.z), 0.0f, 1.0f), 3.0f);
        float rim = fresnel * (0.10f + 0.16f * metallic);

        float diffuseWeight = 1.0f - metallic * 0.35f;
        float lit = hemi + nDotL * 0.78f * diffuseWeight + fillDiff;

        float specR = (1.0f - metallic) * 0.96f + metallic * albedo.x;
        float specG = (1.0f - metallic) * 0.96f + metallic * albedo.y;
        float specB = (1.0f - metallic) * 0.96f + metallic * albedo.z;

        outR = std::clamp(albedo.x * lit + specR * spec + rim * 0.65f, 0.0f, 1.0f);
        outG = std::clamp(albedo.y * lit + specG * spec + rim * 0.75f, 0.0f, 1.0f);
        outB = std::clamp(albedo.z * lit + specB * spec + rim * 0.95f, 0.0f, 1.0f);
    }

    void DrawLine(float x0, float y0, float x1, float y1, uint32_t rgba, float thickness) {
        float dx = x1 - x0;
        float dy = y1 - y0;
        int steps = static_cast<int>(std::ceil(std::max(std::abs(dx), std::abs(dy)))) + 1;
        if (steps <= 1) return;
        float a = static_cast<float>((rgba >> 24) & 0xFFu) / 255.0f;
        float r = static_cast<float>(rgba & 0xFFu) / 255.0f;
        float g = static_cast<float>((rgba >> 8) & 0xFFu) / 255.0f;
        float b = static_cast<float>((rgba >> 16) & 0xFFu) / 255.0f;
        int halfT = std::max(0, static_cast<int>(thickness * 0.5f));
        for (int s = 0; s <= steps; ++s) {
            float t = static_cast<float>(s) / static_cast<float>(steps);
            int px = static_cast<int>(x0 + dx * t);
            int py = static_cast<int>(y0 + dy * t);
            for (int oy = -halfT; oy <= halfT; ++oy) {
                int yy = py + oy;
                if (yy < 0 || yy >= height) continue;
                uint32_t* row = pixels + static_cast<size_t>(yy) * stridePixels;
                for (int ox = -halfT; ox <= halfT; ++ox) {
                    int xx = px + ox;
                    if (xx < 0 || xx >= width) continue;
                    AlphaBlendPixel(row[xx], r, g, b, a);
                }
            }
        }
    }

    void RasterizeMesh(
        const PreparedMesh& pm,
        const CameraXform& cam,
        float localScale,
        float offsetX,
        float offsetY,
        float offsetZ,
        float roughness,
        float metallic
    ) {
        if (!pm.sourcePtr || pm.triCount == 0) return;
        const Mesh& mesh = *pm.sourcePtr;

        struct ProjVert {
            float sx, sy, invZ;
            Vec3 viewN;
            Vec4 col;
        };

        auto transformPoint = [&](const Vec3& p, float& sx, float& sy, float& invZ) {
            float wx = p.x * localScale + offsetX;
            float wy = p.y * localScale + offsetY;
            float wz = p.z * localScale + offsetZ;
            float vz;
            cam.ProjectPoint(wx, wy, wz, sx, sy, invZ, vz);
        };

        for (uint32_t t = 0; t < pm.triCount; ++t) {
            uint32_t i0 = mesh.indices[t * 3 + 0];
            uint32_t i1 = mesh.indices[t * 3 + 1];
            uint32_t i2 = mesh.indices[t * 3 + 2];
            if (i0 >= pm.vertexCount || i1 >= pm.vertexCount || i2 >= pm.vertexCount) continue;

            const Vertex& v0 = mesh.vertices[i0];
            const Vertex& v1 = mesh.vertices[i1];
            const Vertex& v2 = mesh.vertices[i2];

            float sx0, sy0, iz0;
            float sx1, sy1, iz1;
            float sx2, sy2, iz2;
            transformPoint(v0.position, sx0, sy0, iz0);
            transformPoint(v1.position, sx1, sy1, iz1);
            transformPoint(v2.position, sx2, sy2, iz2);

            if (iz0 <= 0.0f || iz1 <= 0.0f || iz2 <= 0.0f) continue;

            // Outward CCW in 3D projects to det2d < 0 because sy is Y-down
            float det2d = (sx1 - sx0) * (sy2 - sy0) - (sy1 - sy0) * (sx2 - sx0);
            if (det2d >= -1e-4f) continue;
            float invArea = 1.0f / (-det2d);

            int32_t minPx = std::max(0, static_cast<int32_t>(std::floor(std::min({sx0, sx1, sx2}))));
            int32_t maxPx = std::min(width - 1, static_cast<int32_t>(std::ceil(std::max({sx0, sx1, sx2}))));
            int32_t minPy = std::max(0, static_cast<int32_t>(std::floor(std::min({sy0, sy1, sy2}))));
            int32_t maxPy = std::min(height - 1, static_cast<int32_t>(std::ceil(std::max({sy0, sy1, sy2}))));
            if (minPx > maxPx || minPy > maxPy) continue;

            Vec3 vn0 = cam.RotateNormalToView(pm.cornerNormals[t * 3 + 0]);
            Vec3 vn1 = cam.RotateNormalToView(pm.cornerNormals[t * 3 + 1]);
            Vec3 vn2 = cam.RotateNormalToView(pm.cornerNormals[t * 3 + 2]);

            float r0, g0, b0, r1, g1, b1, r2, g2, b2;
            ShadeCornerPBR(Vec3(v0.color.x, v0.color.y, v0.color.z), vn0, roughness, metallic, r0, g0, b0);
            ShadeCornerPBR(Vec3(v1.color.x, v1.color.y, v1.color.z), vn1, roughness, metallic, r1, g1, b1);
            ShadeCornerPBR(Vec3(v2.color.x, v2.color.y, v2.color.z), vn2, roughness, metallic, r2, g2, b2);

            float avgAlpha = std::clamp((v0.color.w + v1.color.w + v2.color.w) * (1.0f / 3.0f), 0.0f, 1.0f);
            bool isOpaque = (avgAlpha >= 0.99f);

            for (int32_t py = minPy; py <= maxPy; ++py) {
                float pcfy = static_cast<float>(py) + 0.5f;
                size_t zRowOff = static_cast<size_t>(py) * width;
                uint32_t* pRow = pixels + static_cast<size_t>(py) * stridePixels;

                for (int32_t px = minPx; px <= maxPx; ++px) {
                    float pcfx = static_cast<float>(px) + 0.5f;

                    float e0 = -((sx2 - sx1) * (pcfy - sy1) - (sy2 - sy1) * (pcfx - sx1));
                    if (e0 < -0.001f) continue;
                    float e1 = -((sx0 - sx2) * (pcfy - sy2) - (sy0 - sy2) * (pcfx - sx2));
                    if (e1 < -0.001f) continue;
                    float e2 = -((sx1 - sx0) * (pcfy - sy0) - (sy1 - sy0) * (pcfx - sx0));
                    if (e2 < -0.001f) continue;

                    float w0 = e0 * invArea;
                    float w1 = e1 * invArea;
                    float w2 = e2 * invArea;
                    float iz = w0 * iz0 + w1 * iz1 + w2 * iz2;

                    size_t zIdx = zRowOff + static_cast<size_t>(px);
                    if (iz <= zBuffer[zIdx]) continue;

                    float cr = std::clamp(w0 * r0 + w1 * r1 + w2 * r2, 0.0f, 1.0f);
                    float cg = std::clamp(w0 * g0 + w1 * g1 + w2 * g2, 0.0f, 1.0f);
                    float cb = std::clamp(w0 * b0 + w1 * b1 + w2 * b2, 0.0f, 1.0f);

                    if (isOpaque) {
                        zBuffer[zIdx] = iz;
                        pRow[px] = PackRGBA8888(ClampU8(cr), ClampU8(cg), ClampU8(cb), 255);
                    } else {
                        AlphaBlendPixel(pRow[px], cr, cg, cb, avgAlpha);
                    }
                }
            }
        }
    }
};

} // namespace

void NativeMeshRasterizer::RenderCargoBitmap(
    uint32_t* pixelsRGBA8888,
    int32_t stridePixels,
    int32_t width,
    int32_t height,
    const Mesh* cargoMesh,
    const Mesh* palletMesh,
    const CargoRasterParams& params
) {
    if (!pixelsRGBA8888 || width <= 0 || height <= 0 || !cargoMesh) return;

    const PreparedMesh pmCargo = GetPreparedMeshCopy(cargoMesh);
    float radX = params.rotXDeg * (kPi / 180.0f);
    float radY = params.rotYDeg * (kPi / 180.0f);

    CameraXform cam;
    cam.cx = static_cast<float>(width) * 0.5f;
    cam.cy = static_cast<float>(height) * 0.53f;
    cam.cosY = std::cos(radY);
    cam.sinY = std::sin(radY);
    cam.cosX = std::cos(radX);
    cam.sinX = std::sin(radX);
    cam.baseScale = static_cast<float>(std::min(width, height)) * 0.44f * params.zoom;
    cam.camDist = 3.4f;

    float cSpanX = std::max(0.04f, pmCargo.bounds.max.x - pmCargo.bounds.min.x);
    float cSpanY = std::max(0.04f, pmCargo.bounds.max.y - pmCargo.bounds.min.y);
    float cSpanZ = std::max(0.04f, pmCargo.bounds.max.z - pmCargo.bounds.min.z);
    float maxCargoSpan = std::max({cSpanX, cSpanY, cSpanZ});
    float unitNorm = 0.85f / maxCargoSpan;

    int cols = 1, rows = 1, tiers = 1;
    float unitScaleRatio = 1.0f;
    float palletScaleRatio = 1.05f;
    switch (params.stackingMode) {
        case 1: cols = 2; rows = 2; tiers = 2; unitScaleRatio = 0.48f; palletScaleRatio = 1.25f; break;
        case 2: cols = 2; rows = 2; tiers = 3; unitScaleRatio = 0.42f; palletScaleRatio = 1.32f; break;
        case 3: cols = 3; rows = 2; tiers = 3; unitScaleRatio = 0.36f; palletScaleRatio = 1.50f; break;
        default: break;
    }

    float effectiveScale = unitNorm * unitScaleRatio;
    float floorY = -0.34f;
    bool hasPallet = (params.showPallet || params.stackingMode > 0) && palletMesh && params.cargoId != 10;

    float sx, sy, iz, vz;
    cam.ProjectPoint(0.0f, floorY, 0.0f, sx, sy, iz, vz);
    float shadowR = cam.baseScale * ((params.stackingMode > 0) ? 1.25f : 0.95f);
    float tiltSquash = std::clamp(std::abs(cam.sinX), 0.22f, 0.72f);

    thread_local FrameBufferView fb;
    fb.Begin(pixelsRGBA8888, stridePixels, width, height);
    fb.FillStudioBackground(
        cam.cx, cam.cy, static_cast<float>(std::max(width, height)) * 0.85f,
        sx, sy, shadowR, shadowR * tiltSquash
    );

    float cargoFloorY = floorY;
    if (hasPallet) {
        const PreparedMesh pmPallet = GetPreparedMeshCopy(palletMesh);
        float pSpanX = std::max(0.2f, pmPallet.bounds.max.x - pmPallet.bounds.min.x);
        float pSpanZ = std::max(0.2f, pmPallet.bounds.max.z - pmPallet.bounds.min.z);
        float pSpanY = std::max(0.05f, pmPallet.bounds.max.y - pmPallet.bounds.min.y);
        float pMaxSpan = std::max(pSpanX, pSpanZ);
        float pScale = (0.76f / pMaxSpan) * palletScaleRatio;
        float palletOffsetY = floorY - pmPallet.bounds.min.y * pScale;
        float palletOffsetX = -((pmPallet.bounds.min.x + pmPallet.bounds.max.x) * 0.5f) * pScale;
        float palletOffsetZ = -((pmPallet.bounds.min.z + pmPallet.bounds.max.z) * 0.5f) * pScale;
        cargoFloorY = floorY + pSpanY * pScale;
        fb.RasterizeMesh(pmPallet, cam, pScale, palletOffsetX, palletOffsetY, palletOffsetZ, 0.85f, 0.0f);
    }

    float meshW = std::max(0.20f, cSpanX * effectiveScale);
    float meshH = std::max(0.14f, cSpanY * effectiveScale);
    float meshD = std::max(0.20f, cSpanZ * effectiveScale);
    float stepX = meshW * 1.08f;
    float stepZ = meshD * 1.08f;
    float stepY = meshH * 1.01f;

    float cxLocal = (pmCargo.bounds.min.x + pmCargo.bounds.max.x) * 0.5f * effectiveScale;
    float czLocal = (pmCargo.bounds.min.z + pmCargo.bounds.max.z) * 0.5f * effectiveScale;

    for (int t = 0; t < tiers; ++t) {
        float offsetY = cargoFloorY - pmCargo.bounds.min.y * effectiveScale + static_cast<float>(t) * stepY;
        for (int r = 0; r < rows; ++r) {
            float offsetZ = (static_cast<float>(r) - (rows - 1) * 0.5f) * stepZ - czLocal;
            for (int c = 0; c < cols; ++c) {
                float offsetX = (static_cast<float>(c) - (cols - 1) * 0.5f) * stepX - cxLocal;
                fb.RasterizeMesh(
                    pmCargo, cam, effectiveScale,
                    offsetX, offsetY, offsetZ,
                    params.roughness, params.metallic
                );
            }
        }
    }

    // Optional 3D shrinkwrap edges & yellow polyester strapping cage on multi-unit stacks
    if (params.stackingMode > 0 && (params.showShrinkwrap || params.showStraps)) {
        float halfStackX = static_cast<float>(cols) * stepX * 0.51f;
        float halfStackZ = static_cast<float>(rows) * stepZ * 0.51f;
        float minStackY = cargoFloorY;
        float maxStackY = cargoFloorY + static_cast<float>(tiers) * stepY;

        auto proj = [&](float x, float y, float z, float& ox, float& oy) {
            float i, v;
            cam.ProjectPoint(x, y, z, ox, oy, i, v);
        };

        if (params.showShrinkwrap) {
            uint32_t rimRGBA = PackRGBA8888(255, 255, 255, 110);
            float xs[2] = {-halfStackX, halfStackX};
            float ys[2] = {minStackY, maxStackY};
            float zs[2] = {-halfStackZ, halfStackZ};
            float px[8], py[8];
            int idx = 0;
            for (int yi = 0; yi < 2; ++yi)
                for (int zi = 0; zi < 2; ++zi)
                    for (int xi = 0; xi < 2; ++xi)
                        proj(xs[xi], ys[yi], zs[zi], px[idx], py[idx]), ++idx;
            const int edges[12][2] = {
                {0,1},{2,3},{0,2},{1,3},
                {4,5},{6,7},{4,6},{5,7},
                {0,4},{1,5},{2,6},{3,7}
            };
            for (auto& e : edges) {
                fb.DrawLine(px[e[0]], py[e[0]], px[e[1]], py[e[1]], rimRGBA, 1.5f);
            }
        }

        if (params.showStraps) {
            uint32_t strapRGBA = PackRGBA8888(250, 204, 21, 245);
            float m = 0.015f;
            float bands[2] = {-halfStackX * 0.52f, halfStackX * 0.52f};
            for (float xb : bands) {
                float p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y;
                proj(xb, minStackY - m, -halfStackZ - m, p0x, p0y);
                proj(xb, maxStackY + m, -halfStackZ - m, p1x, p1y);
                proj(xb, maxStackY + m,  halfStackZ + m, p2x, p2y);
                proj(xb, minStackY - m,  halfStackZ + m, p3x, p3y);
                fb.DrawLine(p0x, p0y, p1x, p1y, strapRGBA, 3.0f);
                fb.DrawLine(p1x, p1y, p2x, p2y, strapRGBA, 3.0f);
                fb.DrawLine(p2x, p2y, p3x, p3y, strapRGBA, 3.0f);
                fb.DrawLine(p3x, p3y, p0x, p0y, strapRGBA, 3.0f);
            }
        }
    }
}

void NativeMeshRasterizer::RenderTrailerBitmap(
    uint32_t* pixelsRGBA8888,
    int32_t stridePixels,
    int32_t width,
    int32_t height,
    const Mesh* trailerMesh,
    const Mesh* loadedCargoMesh,
    const TrailerRasterParams& params
) {
    if (!pixelsRGBA8888 || width <= 0 || height <= 0 || !trailerMesh) return;

    const PreparedMesh pmTrailer = GetPreparedMeshCopy(trailerMesh);
    float radX = params.rotXDeg * (kPi / 180.0f);
    float radY = params.rotYDeg * (kPi / 180.0f);

    float spanX = std::max(0.6f, pmTrailer.bounds.max.x - pmTrailer.bounds.min.x);
    float spanY = std::max(0.3f, pmTrailer.bounds.max.y - pmTrailer.bounds.min.y);
    float spanZ = std::max(0.3f, pmTrailer.bounds.max.z - pmTrailer.bounds.min.z);
    float maxHoriz = std::max(spanX, spanZ);

    float fitScaleX = (static_cast<float>(width) * 0.72f) / (maxHoriz * 1.08f);
    float fitScaleY = (static_cast<float>(height) * 0.56f) / (spanY + maxHoriz * 0.16f);
    float worldScale = std::min(fitScaleX, fitScaleY) * params.zoom;

    float midY = (pmTrailer.bounds.min.y + pmTrailer.bounds.max.y) * 0.5f;
    CameraXform cam;
    cam.cx = static_cast<float>(width) * 0.5f;
    cam.cy = static_cast<float>(height) * 0.54f + midY * worldScale * 0.85f;
    cam.baseScale = worldScale;
    cam.cosY = std::cos(radY);
    cam.sinY = std::sin(radY);
    cam.cosX = std::cos(radX);
    cam.sinX = std::sin(radX);
    cam.camDist = std::max(14.0f, spanX * 1.85f);

    float shadowRadius = maxHoriz * 0.56f * worldScale;
    float tiltSquash = std::clamp(std::abs(cam.sinX), 0.18f, 0.55f);

    thread_local FrameBufferView fb;
    fb.Begin(pixelsRGBA8888, stridePixels, width, height);
    fb.FillStudioBackground(
        static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.55f,
        static_cast<float>(std::max(width, height)) * 0.90f,
        cam.cx, cam.cy, shadowRadius, shadowRadius * tiltSquash
    );

    // Render trailer 1:1 at true metric scale
    fb.RasterizeMesh(pmTrailer, cam, 1.0f, 0.0f, 0.0f, 0.0f, params.roughness, params.metallic);

    // Optional loaded CargoRegistry::Get() payload at true 1:1 real-world scale on trailer deck
    if (params.showLoadedCargo && loadedCargoMesh && params.cargoQuantity > 0) {
        const PreparedMesh pmCargo = GetPreparedMeshCopy(loadedCargoMesh);
        float cLen = std::clamp(pmCargo.bounds.max.x - pmCargo.bounds.min.x, 0.18f, 4.5f);
        float cWid = std::clamp(pmCargo.bounds.max.z - pmCargo.bounds.min.z, 0.18f, 2.2f);
        float cHgt = std::clamp(pmCargo.bounds.max.y - pmCargo.bounds.min.y, 0.12f, 2.0f);

        float usableL = std::max(1.2f, params.deckLengthM * 0.86f);
        float usableW = std::max(0.8f, params.deckWidthM * 0.84f);
        int cols = std::max(1, static_cast<int>(usableL / (cLen * 1.15f)));
        int rows = std::max(1, static_cast<int>(usableW / (cWid * 1.15f)));
        int maxRender = std::min(params.cargoQuantity, 48);

        float cxLocal = (pmCargo.bounds.min.x + pmCargo.bounds.max.x) * 0.5f;
        float czLocal = (pmCargo.bounds.min.z + pmCargo.bounds.max.z) * 0.5f;

        int rendered = 0;
        for (int tier = 0; tier < 3 && rendered < maxRender; ++tier) {
            float oy = params.cargoFloorY + 0.01f - pmCargo.bounds.min.y + static_cast<float>(tier) * (cHgt * 1.02f);
            for (int r = 0; r < rows && rendered < maxRender; ++r) {
                float oz = (static_cast<float>(r) - (rows - 1) * 0.5f) * (cWid * 1.12f) - czLocal;
                for (int c = 0; c < cols && rendered < maxRender; ++c) {
                    float ox = -usableL * 0.44f + static_cast<float>(c) * (cLen * 1.12f) - cxLocal;
                    fb.RasterizeMesh(pmCargo, cam, 1.0f, ox, oy, oz, 0.68f, 0.05f);
                    ++rendered;
                }
            }
        }
    }
}

void NativeMeshRasterizer::RenderVehicleBitmap(
    uint32_t* pixelsRGBA8888,
    int32_t stridePixels,
    int32_t width,
    int32_t height,
    const Mesh* vehicleMesh,
    const Mesh* hitchedTrailerMesh,
    const Mesh* loadedCargoMesh,
    const VehicleRasterParams& params
) {
    if (!pixelsRGBA8888 || width <= 0 || height <= 0 || !vehicleMesh) return;

    const PreparedMesh pmVehicle = GetPreparedMeshCopy(vehicleMesh);
    bool hasTrailer = (params.showHitchedTrailer && hitchedTrailerMesh != nullptr);
    PreparedMesh pmTrailer;
    float trailerOffsetX = 0.0f;

    float minX = pmVehicle.bounds.min.x;
    float maxX = pmVehicle.bounds.max.x;
    float minY = pmVehicle.bounds.min.y;
    float maxY = pmVehicle.bounds.max.y;
    float minZ = pmVehicle.bounds.min.z;
    float maxZ = pmVehicle.bounds.max.z;

    if (hasTrailer) {
        pmTrailer = GetPreparedMeshCopy(hitchedTrailerMesh);
        float trailerKingpinLocalX = -params.trailerLengthM * 0.5f + params.trailerKingpinOffsetM;
        trailerOffsetX = params.fifthWheelOffsetM - trailerKingpinLocalX;
        minX = std::min(minX, pmTrailer.bounds.min.x + trailerOffsetX);
        maxX = std::max(maxX, pmTrailer.bounds.max.x + trailerOffsetX);
        minY = std::min(minY, pmTrailer.bounds.min.y);
        maxY = std::max(maxY, pmTrailer.bounds.max.y);
        minZ = std::min(minZ, pmTrailer.bounds.min.z);
        maxZ = std::max(maxZ, pmTrailer.bounds.max.z);
    }

    float centerShiftX = -(minX + maxX) * 0.5f;
    float spanX = std::max(1.0f, maxX - minX);
    float spanY = std::max(0.8f, maxY - minY);
    float spanZ = std::max(0.8f, maxZ - minZ);
    float maxHoriz = std::max(spanX, spanZ);

    float radX = params.rotXDeg * (kPi / 180.0f);
    float radY = params.rotYDeg * (kPi / 180.0f);

    float fitScaleX = (static_cast<float>(width) * 0.74f) / (maxHoriz * 1.06f);
    float fitScaleY = (static_cast<float>(height) * 0.58f) / (spanY + maxHoriz * 0.14f);
    float worldScale = std::min(fitScaleX, fitScaleY) * params.zoom;

    float midY = (minY + maxY) * 0.5f;
    CameraXform cam;
    cam.cx = static_cast<float>(width) * 0.5f;
    cam.cy = static_cast<float>(height) * 0.55f + midY * worldScale * 0.85f;
    cam.baseScale = worldScale;
    cam.cosY = std::cos(radY);
    cam.sinY = std::sin(radY);
    cam.cosX = std::cos(radX);
    cam.sinX = std::sin(radX);
    cam.camDist = std::max(12.0f, spanX * 1.85f);

    float shadowRadius = maxHoriz * 0.55f * worldScale;
    float tiltSquash = std::clamp(std::abs(cam.sinX), 0.18f, 0.55f);

    thread_local FrameBufferView fb;
    fb.Begin(pixelsRGBA8888, stridePixels, width, height);
    fb.FillStudioBackground(
        static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.55f,
        static_cast<float>(std::max(width, height)) * 0.90f,
        cam.cx, cam.cy, shadowRadius, shadowRadius * tiltSquash
    );

    // Render Vehicle at true 1:1 metric scale centered on combined rig bounds
    fb.RasterizeMesh(pmVehicle, cam, 1.0f, centerShiftX, 0.0f, 0.0f, params.roughness, params.metallic);

    if (hasTrailer) {
        float trShiftX = trailerOffsetX + centerShiftX;
        fb.RasterizeMesh(pmTrailer, cam, 1.0f, trShiftX, 0.0f, 0.0f, 0.48f, 0.52f);

        if (params.showLoadedCargo && loadedCargoMesh && params.cargoQuantity > 0) {
            const PreparedMesh pmCargo = GetPreparedMeshCopy(loadedCargoMesh);
            float cLen = std::clamp(pmCargo.bounds.max.x - pmCargo.bounds.min.x, 0.18f, 4.5f);
            float cWid = std::clamp(pmCargo.bounds.max.z - pmCargo.bounds.min.z, 0.18f, 2.2f);
            float cHgt = std::clamp(pmCargo.bounds.max.y - pmCargo.bounds.min.y, 0.12f, 2.0f);

            float usableL = std::max(1.2f, params.trailerLengthM * 0.86f);
            float usableW = std::max(0.8f, params.trailerWidthM * 0.84f);
            int cols = std::max(1, static_cast<int>(usableL / (cLen * 1.15f)));
            int rows = std::max(1, static_cast<int>(usableW / (cWid * 1.15f)));
            int maxRender = std::min(params.cargoQuantity, 48);

            float cxLocal = (pmCargo.bounds.min.x + pmCargo.bounds.max.x) * 0.5f;
            float czLocal = (pmCargo.bounds.min.z + pmCargo.bounds.max.z) * 0.5f;

            int rendered = 0;
            for (int tier = 0; tier < 3 && rendered < maxRender; ++tier) {
                float oy = params.trailerDeckY + 0.01f - pmCargo.bounds.min.y + static_cast<float>(tier) * (cHgt * 1.02f);
                for (int r = 0; r < rows && rendered < maxRender; ++r) {
                    float oz = (static_cast<float>(r) - (rows - 1) * 0.5f) * (cWid * 1.12f) - czLocal;
                    for (int c = 0; c < cols && rendered < maxRender; ++c) {
                        float ox = trShiftX - usableL * 0.44f + static_cast<float>(c) * (cLen * 1.12f) - cxLocal;
                        fb.RasterizeMesh(pmCargo, cam, 1.0f, ox, oy, oz, 0.68f, 0.05f);
                        ++rendered;
                    }
                }
            }
        }
    }
}

void NativeMeshRasterizer::RenderPbrStageRGBA8888(
    uint32_t* pixelsRGBA8888,
    int32_t stridePixels,
    int32_t width,
    int32_t height,
    const PbrStageRasterParams& params
) {
    if (!pixelsRGBA8888 || width <= 0 || height <= 0) return;

    float minDim = static_cast<float>(std::min(width, height));
    float cx = static_cast<float>(width) * 0.5f;
    float baseCy = static_cast<float>(height) * 0.64f;
    float maxObj = std::max(1.0f, std::max({params.objectWidth, params.objectHeight, params.objectDepth}) * 0.55f);
    float scale = (minDim * 0.18f) / maxObj;

    float hw = params.objectWidth * 0.5f;
    float hh = params.objectHeight;
    float hd = params.objectDepth * 0.5f;

    float intenFactor = std::clamp(0.45f + (params.lightIntensity / 5000.0f) * 0.55f, 0.35f, 1.0f);
    Vec4 col(
        std::clamp(params.lightColorR * intenFactor, 0.0f, 1.0f),
        std::clamp(params.lightColorG * intenFactor, 0.0f, 1.0f),
        std::clamp(params.lightColorB * intenFactor, 0.0f, 1.0f),
        1.0f
    );

    Mesh boxMesh;
    Vec3 corners[8] = {
        Vec3(-hw, 0.0f,  hd), Vec3( hw, 0.0f,  hd), Vec3( hw, hh,  hd), Vec3(-hw, hh,  hd),
        Vec3(-hw, 0.0f, -hd), Vec3( hw, 0.0f, -hd), Vec3( hw, hh, -hd), Vec3(-hw, hh, -hd)
    };
    for (int i = 0; i < 8; ++i) {
        Vertex v;
        v.position = corners[i];
        v.color = col;
        boxMesh.vertices.push_back(v);
    }
    const uint32_t tris[36] = {
        0,1,2, 0,2,3,
        5,4,7, 5,7,6,
        3,2,6, 3,6,7,
        4,5,1, 4,1,0,
        1,5,6, 1,6,2,
        4,0,3, 4,3,7
    };
    boxMesh.indices.assign(tris, tris + 36);
    boxMesh.CalculateBounds();
    const PreparedMesh pmBox = BuildPreparedMesh(&boxMesh);

    float radX = 22.0f * (kPi / 180.0f);
    float radY = -36.0f * (kPi / 180.0f);
    CameraXform cam;
    cam.cx = cx;
    cam.cy = baseCy;
    cam.baseScale = scale;
    cam.cosY = std::cos(radY);
    cam.sinY = std::sin(radY);
    cam.cosX = std::cos(radX);
    cam.sinX = std::sin(radX);
    cam.camDist = 9.0f;

    float stageW = (params.stageMode == 1)
        ? static_cast<float>(width) * 0.46f
        : std::max(hw * scale * 2.1f, static_cast<float>(width) * 0.32f);
    float stageH = (params.stageMode == 1)
        ? static_cast<float>(height) * 0.17f
        : std::max(hd * scale * 1.4f, static_cast<float>(height) * 0.11f);
    float shadowCx = cx - params.lightPosX * scale * 0.28f;
    float shadowCy = baseCy + params.lightPosZ * scale * 0.10f;

    thread_local FrameBufferView fb;
    fb.Begin(pixelsRGBA8888, stridePixels, width, height);
    fb.FillStudioBackground(
        cx, baseCy * 0.85f, static_cast<float>(std::max(width, height)) * 0.90f,
        shadowCx, shadowCy, std::max(hw * scale * 1.7f, stageW * 0.45f), stageH
    );

    fb.RasterizeMesh(pmBox, cam, 1.0f, 0.0f, 0.0f, 0.0f, params.roughness, params.metallic);
}

} // namespace sphere::scene
