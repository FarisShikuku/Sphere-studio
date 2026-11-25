// ============================================================================
// VegetationMath.cpp - Vector & Procedural Noise Implementations
// ============================================================================

#include "VegetationMath.h"

namespace vegetation {

Mat3 Mat3::fromAxisAngle(const Vec3& axis, float angleRad) {
    Vec3 a = axis.normalized();
    float c = std::cos(angleRad);
    float s = std::sin(angleRad);
    float t = 1.0f - c;

    Mat3 m;
    m.col0 = Vec3(t * a.x * a.x + c,       t * a.x * a.y + s * a.z, t * a.x * a.z - s * a.y);
    m.col1 = Vec3(t * a.x * a.y - s * a.z, t * a.y * a.y + c,       t * a.y * a.z + s * a.x);
    m.col2 = Vec3(t * a.x * a.z + s * a.y, t * a.y * a.z - s * a.x, t * a.z * a.z + c);
    return m;
}

Mat3 Mat3::fromEuler(float pitchRad, float yawRad, float rollRad) {
    Mat3 ry = fromAxisAngle(Vec3(0, 1, 0), yawRad);
    Mat3 rx = fromAxisAngle(Vec3(1, 0, 0), pitchRad);
    Mat3 rz = fromAxisAngle(Vec3(0, 0, 1), rollRad);
    
    // Combined rotation: ry * rx * rz
    Mat3 res;
    res.col0 = ry.transform(rx.transform(rz.col0));
    res.col1 = ry.transform(rx.transform(rz.col1));
    res.col2 = ry.transform(rx.transform(rz.col2));
    return res;
}

} // namespace vegetation

// Deterministic Value & Simplex Noise implementation in pure C
static inline float hash31(float x, float y, float z) {
    float n = std::sin(x * 12.9898f + y * 78.233f + z * 37.719f) * 43758.5453f;
    return n - std::floor(n);
}

static inline float smooth(float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

extern "C" {

float veg_noise3d(float x, float y, float z) {
    float xi = std::floor(x);
    float yi = std::floor(y);
    float zi = std::floor(z);

    float xf = x - xi;
    float yf = y - yi;
    float zf = z - zi;

    float u = smooth(xf);
    float v = smooth(yf);
    float w = smooth(zf);

    float c000 = hash31(xi,     yi,     zi);
    float c100 = hash31(xi + 1, yi,     zi);
    float c010 = hash31(xi,     yi + 1, zi);
    float c110 = hash31(xi + 1, yi + 1, zi);

    float c001 = hash31(xi,     yi,     zi + 1);
    float c101 = hash31(xi + 1, yi,     zi + 1);
    float c011 = hash31(xi,     yi + 1, zi + 1);
    float c1101 = hash31(xi + 1, yi + 1, zi + 1);

    float x00 = c000 * (1 - u) + c100 * u;
    float x10 = c010 * (1 - u) + c110 * u;
    float x01 = c001 * (1 - u) + c101 * u;
    float x11 = c011 * (1 - u) + c1101 * u;

    float y0 = x00 * (1 - v) + x10 * v;
    float y1 = x01 * (1 - v) + x11 * v;

    return (y0 * (1 - w) + y1 * w) * 2.0f - 1.0f;
}

float veg_fbm3d(float x, float y, float z, int octaves, float lacunarity, float gain) {
    float total = 0.0f;
    float freq = 1.0f;
    float amp = 0.5f;
    for (int i = 0; i < octaves; ++i) {
        total += amp * veg_noise3d(x * freq, y * freq, z * freq);
        freq *= lacunarity;
        amp *= gain;
    }
    return total;
}

float veg_voronoi3d(float x, float y, float z, float* outCellX, float* outCellY, float* outCellZ) {
    float xi = std::floor(x);
    float yi = std::floor(y);
    float zi = std::floor(z);

    float minDist = 999.0f;
    float bestCx = 0, bestCy = 0, bestCz = 0;

    for (int dz = -1; dz <= 1; ++dz) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                float cx = xi + dx;
                float cy = yi + dy;
                float cz = zi + dz;

                float px = cx + hash31(cx, cy, cz);
                float py = cy + hash31(cx + 31.0f, cy + 17.0f, cz);
                float pz = cz + hash31(cx + 7.0f, cy + 53.0f, cz + 19.0f);

                float dist = std::sqrt((x - px) * (x - px) + (y - py) * (y - py) + (z - pz) * (z - pz));
                if (dist < minDist) {
                    minDist = dist;
                    bestCx = px;
                    bestCy = py;
                    bestCz = pz;
                }
            }
        }
    }

    if (outCellX) *outCellX = bestCx;
    if (outCellY) *outCellY = bestCy;
    if (outCellZ) *outCellZ = bestCz;

    return minDist;
}

} // extern "C"
