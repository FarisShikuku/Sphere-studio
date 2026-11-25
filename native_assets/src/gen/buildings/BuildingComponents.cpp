#include "BuildingComponents.h"
#include <cmath>
#include <algorithm>
#include <functional>

namespace procedural::buildings {

namespace {
constexpr float kPi = 3.14159265358979323846f;
}

void BuildingComponents::appendQuad(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3,
    const Vec3& normal,
    uint32_t materialId
) {
    uint32_t baseIdx = static_cast<uint32_t>(vertices.size());

    vertices.push_back({p0, normal, {0.0f, 0.0f}, materialId});
    vertices.push_back({p1, normal, {1.0f, 0.0f}, materialId});
    vertices.push_back({p2, normal, {1.0f, 1.0f}, materialId});
    vertices.push_back({p3, normal, {0.0f, 1.0f}, materialId});

    indices.push_back(baseIdx);
    indices.push_back(baseIdx + 1);
    indices.push_back(baseIdx + 2);

    indices.push_back(baseIdx);
    indices.push_back(baseIdx + 2);
    indices.push_back(baseIdx + 3);
}

void BuildingComponents::appendBox(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& min, const Vec3& max,
    uint32_t materialId
) {
    // 6 faces of an axis-aligned box
    // Front (+Z)
    appendQuad(vertices, indices,
        {min.x, min.y, max.z}, {max.x, min.y, max.z},
        {max.x, max.y, max.z}, {min.x, max.y, max.z},
        {0.0f, 0.0f, 1.0f}, materialId);
    // Back (-Z)
    appendQuad(vertices, indices,
        {max.x, min.y, min.z}, {min.x, min.y, min.z},
        {min.x, max.y, min.z}, {max.x, max.y, min.z},
        {0.0f, 0.0f, -1.0f}, materialId);
    // Left (-X)
    appendQuad(vertices, indices,
        {min.x, min.y, min.z}, {min.x, min.y, max.z},
        {min.x, max.y, max.z}, {min.x, max.y, min.z},
        {-1.0f, 0.0f, 0.0f}, materialId);
    // Right (+X)
    appendQuad(vertices, indices,
        {max.x, min.y, max.z}, {max.x, min.y, min.z},
        {max.x, max.y, min.z}, {max.x, max.y, max.z},
        {1.0f, 0.0f, 0.0f}, materialId);
    // Top (+Y)
    appendQuad(vertices, indices,
        {min.x, max.y, max.z}, {max.x, max.y, max.z},
        {max.x, max.y, min.z}, {min.x, max.y, min.z},
        {0.0f, 1.0f, 0.0f}, materialId);
    // Bottom (-Y)
    appendQuad(vertices, indices,
        {min.x, min.y, min.z}, {max.x, min.y, min.z},
        {max.x, min.y, max.z}, {min.x, min.y, max.z},
        {0.0f, -1.0f, 0.0f}, materialId);
}

void BuildingComponents::appendOrientedBox(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& center,
    const Vec3& tangent,
    const Vec3& up,
    const Vec3& normal,
    float halfT,
    float halfH,
    float halfN,
    uint32_t materialId
) {
    Vec3 U = (up.length() > 1e-5f) ? up.normalized() : Vec3{0.0f, 1.0f, 0.0f};
    Vec3 T = (tangent - U * tangent.dot(U));
    T = (T.length() > 1e-5f) ? T.normalized() : Vec3{1.0f, 0.0f, 0.0f};
    Vec3 N = normal - T * normal.dot(T) - U * normal.dot(U);
    N = (N.length() > 1e-5f) ? N.normalized() : T.cross(U).normalized();
    if (N.dot(normal) < 0.0f) {
        N = -N;
    }
    // Ensure right-handed basis (T x U) . N > 0 so all 6 faces have consistent CCW outward winding
    if (T.cross(U).dot(N) < 0.0f) {
        T = -T;
    }

    Vec3 tOff = T * std::abs(halfT);
    Vec3 hOff = U * std::abs(halfH);
    Vec3 nOff = N * std::abs(halfN);

    // Front (+N)
    appendQuad(vertices, indices,
        center - tOff - hOff + nOff, center + tOff - hOff + nOff,
        center + tOff + hOff + nOff, center - tOff + hOff + nOff,
        N, materialId);
    // Back (-N)
    appendQuad(vertices, indices,
        center + tOff - hOff - nOff, center - tOff - hOff - nOff,
        center - tOff + hOff - nOff, center + tOff + hOff - nOff,
        -N, materialId);
    // Left (-T)
    appendQuad(vertices, indices,
        center - tOff - hOff - nOff, center - tOff - hOff + nOff,
        center - tOff + hOff + nOff, center - tOff + hOff - nOff,
        -T, materialId);
    // Right (+T)
    appendQuad(vertices, indices,
        center + tOff - hOff + nOff, center + tOff - hOff - nOff,
        center + tOff + hOff - nOff, center + tOff + hOff + nOff,
        T, materialId);
    // Top (+U)
    appendQuad(vertices, indices,
        center - tOff + hOff + nOff, center + tOff + hOff + nOff,
        center + tOff + hOff - nOff, center - tOff + hOff - nOff,
        U, materialId);
    // Bottom (-U)
    appendQuad(vertices, indices,
        center - tOff - hOff - nOff, center + tOff - hOff - nOff,
        center + tOff - hOff + nOff, center - tOff - hOff + nOff,
        -U, materialId);
}

void BuildingComponents::appendOrientedCylinder(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& p0,
    const Vec3& p1,
    float radius0,
    float radius1,
    int segments,
    uint32_t materialId,
    bool cap0,
    bool cap1
) {
    Vec3 dir = p1 - p0;
    float len = dir.length();
    if (len < 1e-4f) return;
    Vec3 t = dir * (1.0f / len);

    Vec3 ref = (std::abs(t.y) < 0.9f) ? Vec3{0.0f, 1.0f, 0.0f} : Vec3{1.0f, 0.0f, 0.0f};
    Vec3 u = t.cross(ref).normalized();
    Vec3 v = t.cross(u).normalized();

    if (segments < 3) segments = 3;
    float angleStep = 2.0f * kPi / static_cast<float>(segments);

    auto addVertex = [&](const Vec3& pos, const Vec3& norm) -> uint32_t {
        uint32_t idx = static_cast<uint32_t>(vertices.size());
        vertices.push_back({pos, norm, {0.0f, 0.0f}, materialId});
        return idx;
    };

    // Side quads
    for (int i = 0; i < segments; ++i) {
        float a0 = i * angleStep;
        float a1 = (i + 1) * angleStep;
        float cos0 = std::cos(a0), sin0 = std::sin(a0);
        float cos1 = std::cos(a1), sin1 = std::sin(a1);

        Vec3 radDir0 = u * cos0 + v * sin0;
        Vec3 radDir1 = u * cos1 + v * sin1;

        Vec3 b0 = p0 + radDir0 * radius0;
        Vec3 b1 = p0 + radDir1 * radius0;
        Vec3 t1 = p1 + radDir1 * radius1;
        Vec3 t0 = p1 + radDir0 * radius1;

        Vec3 norm = ((radDir0 + radDir1) * 0.5f).normalized();

        appendQuad(vertices, indices, b0, b1, t1, t0, norm, materialId);
    }

    // Cap at p0
    if (cap0 && radius0 > 0.001f) {
        Vec3 norm = -t;
        uint32_t cIdx = addVertex(p0, norm);
        uint32_t rimStart = static_cast<uint32_t>(vertices.size());
        for (int i = 0; i <= segments; ++i) {
            float a = i * angleStep;
            Vec3 p = p0 + (u * std::cos(a) + v * std::sin(a)) * radius0;
            addVertex(p, norm);
        }
        for (int i = 0; i < segments; ++i) {
            indices.push_back(cIdx);
            indices.push_back(rimStart + i + 1);
            indices.push_back(rimStart + i);
        }
    }

    // Cap at p1
    if (cap1 && radius1 > 0.001f) {
        Vec3 norm = t;
        uint32_t cIdx = addVertex(p1, norm);
        uint32_t rimStart = static_cast<uint32_t>(vertices.size());
        for (int i = 0; i <= segments; ++i) {
            float a = i * angleStep;
            Vec3 p = p1 + (u * std::cos(a) + v * std::sin(a)) * radius1;
            addVertex(p, norm);
        }
        for (int i = 0; i < segments; ++i) {
            indices.push_back(cIdx);
            indices.push_back(rimStart + i);
            indices.push_back(rimStart + i + 1);
        }
    }
}

void BuildingComponents::appendBentCylinderCatenary(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& p0,
    const Vec3& p1,
    float sag,
    float radius,
    int lengthSegments,
    int radialSegments,
    uint32_t materialId
) {
    if (lengthSegments < 4) lengthSegments = 4;
    if (radialSegments < 3) radialSegments = 3;

    auto addVertex = [&](const Vec3& pos, const Vec3& norm) -> uint32_t {
        uint32_t idx = static_cast<uint32_t>(vertices.size());
        vertices.push_back({pos, norm, {0.0f, 0.0f}, materialId});
        return idx;
    };

    const float a = 0.85f;
    const float coshHalf = std::cosh(0.5f / a);
    const float denom = std::max(coshHalf - 1.0f, 1e-4f);

    auto evaluateCatenaryPoint = [&](float t) -> Vec3 {
        Vec3 linear = p0 * (1.0f - t) + p1 * t;
        float u = t - 0.5f;
        float dy = -sag * (coshHalf - std::cosh(u / a)) / denom;
        return Vec3{linear.x, linear.y + dy, linear.z};
    };

    auto evaluateCatenaryTangent = [&](float t) -> Vec3 {
        Vec3 dLin = p1 - p0;
        float u = t - 0.5f;
        float ddy = (sag / (a * denom)) * std::sinh(u / a);
        Vec3 tan{dLin.x, dLin.y + ddy, dLin.z};
        float len = tan.length();
        return (len > 1e-5f) ? (tan * (1.0f / len)) : Vec3{0.0f, 0.0f, 1.0f};
    };

    float angleStep = 2.0f * kPi / static_cast<float>(radialSegments);
    std::vector<std::vector<uint32_t>> rings(lengthSegments + 1);

    for (int i = 0; i <= lengthSegments; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(lengthSegments);
        Vec3 center = evaluateCatenaryPoint(t);
        Vec3 tangent = evaluateCatenaryTangent(t);

        Vec3 ref = (std::abs(tangent.y) < 0.92f) ? Vec3{0.0f, 1.0f, 0.0f} : Vec3{1.0f, 0.0f, 0.0f};
        Vec3 normal = tangent.cross(ref).normalized();
        Vec3 binormal = tangent.cross(normal).normalized();

        rings[i].resize(radialSegments);
        for (int j = 0; j < radialSegments; ++j) {
            float theta = j * angleStep;
            Vec3 rad = normal * std::cos(theta) + binormal * std::sin(theta);
            Vec3 pos = center + rad * radius;
            rings[i][j] = addVertex(pos, rad);
        }
    }

    // Connect rings
    for (int i = 0; i < lengthSegments; ++i) {
        for (int j = 0; j < radialSegments; ++j) {
            int nextJ = (j + 1) % radialSegments;
            uint32_t v0 = rings[i][j];
            uint32_t v1 = rings[i][nextJ];
            uint32_t v2 = rings[i + 1][nextJ];
            uint32_t v3 = rings[i + 1][j];

            indices.push_back(v0);
            indices.push_back(v1);
            indices.push_back(v2);

            indices.push_back(v0);
            indices.push_back(v2);
            indices.push_back(v3);
        }
    }

    // Cap start (t=0)
    Vec3 startTan = evaluateCatenaryTangent(0.0f);
    uint32_t c0 = addVertex(evaluateCatenaryPoint(0.0f), -startTan);
    for (int j = 0; j < radialSegments; ++j) {
        int nextJ = (j + 1) % radialSegments;
        indices.push_back(c0);
        indices.push_back(rings[0][nextJ]);
        indices.push_back(rings[0][j]);
    }

    // Cap end (t=1)
    Vec3 endTan = evaluateCatenaryTangent(1.0f);
    uint32_t c1 = addVertex(evaluateCatenaryPoint(1.0f), endTan);
    for (int j = 0; j < radialSegments; ++j) {
        int nextJ = (j + 1) % radialSegments;
        indices.push_back(c1);
        indices.push_back(rings[lengthSegments][j]);
        indices.push_back(rings[lengthSegments][nextJ]);
    }
}

uint32_t BuildingComponents::getGlassMaterial(GlassType glassType) {
    switch (glassType) {
        case GlassType::SolarTintedSmoke: return static_cast<uint32_t>(MaterialSlot::GlassTinted);
        case GlassType::ReflectiveChromeMirror: return static_cast<uint32_t>(MaterialSlot::GlassReflective);
        case GlassType::LowEEmerald: return static_cast<uint32_t>(MaterialSlot::GlassEmerald);
        case GlassType::BronzeArchitectural: return static_cast<uint32_t>(MaterialSlot::GlassBronze);
        case GlassType::FrostedSatinPrivacy: return static_cast<uint32_t>(MaterialSlot::GlassFrosted);
        case GlassType::GoldMirroredLuxury: return static_cast<uint32_t>(MaterialSlot::GlassGold);
        case GlassType::StainedGlassArtisan: return static_cast<uint32_t>(MaterialSlot::GlassStained);
        case GlassType::ClearFloat:
        default: return static_cast<uint32_t>(MaterialSlot::Glass);
    }
}

uint32_t BuildingComponents::getWallMaterial(WallMaterialType wallType) {
    switch (wallType) {
        case WallMaterialType::BrickRedMasonry: return static_cast<uint32_t>(MaterialSlot::WallBrick);
        case WallMaterialType::LimestonePortland: return static_cast<uint32_t>(MaterialSlot::WallLimestone);
        case WallMaterialType::GlassCurtainWall: return static_cast<uint32_t>(MaterialSlot::Glass);
        case WallMaterialType::WhiteStuccoModern: return static_cast<uint32_t>(MaterialSlot::WallStucco);
        case WallMaterialType::WeatheredCortenSteel: return static_cast<uint32_t>(MaterialSlot::WallCorten);
        case WallMaterialType::DarkBasaltSlate: return static_cast<uint32_t>(MaterialSlot::WallBasalt);
        case WallMaterialType::NaturalTimberSiding: return static_cast<uint32_t>(MaterialSlot::WallTimber);
        case WallMaterialType::BlackAluminumGlassGrid: return static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
        case WallMaterialType::StructuralSiliconeGlazing: return static_cast<uint32_t>(MaterialSlot::GlassReflective);
        case WallMaterialType::ChampagneAluminumGlass: return static_cast<uint32_t>(MaterialSlot::WallChampagneAluminum);
        case WallMaterialType::DoubleSkinAerodynamicGlass: return static_cast<uint32_t>(MaterialSlot::GlassEmerald);
        case WallMaterialType::BronzeAluminumSolarFins: return static_cast<uint32_t>(MaterialSlot::WallBronzeAluminum);
        case WallMaterialType::GunmetalAluminumCassette: return static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
        case WallMaterialType::PerforatedAluminumBriseSoleil: return static_cast<uint32_t>(MaterialSlot::FrameMetal);
        case WallMaterialType::SpiderPointFixedGlass: return static_cast<uint32_t>(MaterialSlot::Glass);
        case WallMaterialType::ConcreteBrutalist:
        default: return static_cast<uint32_t>(MaterialSlot::WallConcrete);
    }
}

bool BuildingComponents::isCurtainWallMaterial(WallMaterialType wallType) {
    switch (wallType) {
        case WallMaterialType::GlassCurtainWall:
        case WallMaterialType::BlackAluminumGlassGrid:
        case WallMaterialType::StructuralSiliconeGlazing:
        case WallMaterialType::ChampagneAluminumGlass:
        case WallMaterialType::DoubleSkinAerodynamicGlass:
        case WallMaterialType::BronzeAluminumSolarFins:
        case WallMaterialType::GunmetalAluminumCassette:
        case WallMaterialType::PerforatedAluminumBriseSoleil:
        case WallMaterialType::SpiderPointFixedGlass:
            return true;
        default:
            return false;
    }
}

void BuildingComponents::generateCurtainWallBay(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& bayStart,
    const Vec3& bayEnd,
    const Vec3& normal,
    const Vec3& tangent,
    float floorBottomY,
    float floorTopY,
    WallMaterialType wallType,
    bool isEntranceBay,
    DoorType doorType,
    float doorWidth,
    float doorHeight,
    bool hasCanopy,
    GlassType glassType
) {
    float bayWidth = (bayEnd - bayStart).length();
    float bayH = floorTopY - floorBottomY;
    Vec3 bayCenter = (bayStart + bayEnd) * 0.5f;

    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t spandrelMat = static_cast<uint32_t>(MaterialSlot::WallSpandrelGlass);

    // Pick frame and glass material slots based on chosen wall system
    if (wallType == WallMaterialType::BlackAluminumGlassGrid) {
        frameMat = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
        glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    } else if (wallType == WallMaterialType::StructuralSiliconeGlazing) {
        frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
        glassMat = static_cast<uint32_t>(MaterialSlot::GlassReflective);
    } else if (wallType == WallMaterialType::ChampagneAluminumGlass) {
        frameMat = static_cast<uint32_t>(MaterialSlot::WallChampagneAluminum);
        glassMat = static_cast<uint32_t>(MaterialSlot::GlassGold);
    } else if (wallType == WallMaterialType::DoubleSkinAerodynamicGlass) {
        frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
        glassMat = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    } else if (wallType == WallMaterialType::BronzeAluminumSolarFins) {
        frameMat = static_cast<uint32_t>(MaterialSlot::WallBronzeAluminum);
        glassMat = static_cast<uint32_t>(MaterialSlot::GlassBronze);
    } else if (wallType == WallMaterialType::GunmetalAluminumCassette) {
        frameMat = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
        glassMat = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    } else if (wallType == WallMaterialType::PerforatedAluminumBriseSoleil) {
        frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
        glassMat = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    } else if (wallType == WallMaterialType::SpiderPointFixedGlass) {
        frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
        glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    }

    // Apply custom GlassType tint if explicitly selected
    if (glassType != GlassType::ClearFloat) {
        glassMat = getGlassMaterial(glassType);
    }

    if (isEntranceBay && doorType != DoorType::None) {
        float dw = std::min(bayWidth * 0.85f, doorWidth);
        float dh = std::min(bayH - 0.4f, doorHeight);
        Vec3 leftBayEdge = bayCenter - tangent * (dw * 0.5f);
        Vec3 rightBayEdge = bayCenter + tangent * (dw * 0.5f);

        // Glass sidelights
        appendQuad(vertices, indices,
            bayStart + Vec3{0, floorBottomY, 0}, leftBayEdge + Vec3{0, floorBottomY, 0},
            leftBayEdge + Vec3{0, floorTopY, 0}, bayStart + Vec3{0, floorTopY, 0},
            normal, glassMat);

        appendQuad(vertices, indices,
            rightBayEdge + Vec3{0, floorBottomY, 0}, bayEnd + Vec3{0, floorBottomY, 0},
            bayEnd + Vec3{0, floorTopY, 0}, rightBayEdge + Vec3{0, floorTopY, 0},
            normal, glassMat);

        // Glass transom above door
        appendQuad(vertices, indices,
            leftBayEdge + Vec3{0, floorBottomY + dh, 0}, rightBayEdge + Vec3{0, floorBottomY + dh, 0},
            rightBayEdge + Vec3{0, floorTopY, 0}, leftBayEdge + Vec3{0, floorTopY, 0},
            normal, glassMat);

        // Door component
        Vec3 doorGround = bayCenter + Vec3{0, floorBottomY, 0};
        generateDoor(vertices, indices, doorGround, dw, dh, normal, tangent, doorType, hasCanopy);
        return;
    }

    Vec3 spanVec = bayEnd - bayStart;
    float spanLen = spanVec.length();
    Vec3 bayT = (spanLen > 1e-4f) ? (spanVec * (1.0f / spanLen)) : tangent.normalized();
    Vec3 upVec{0.0f, 1.0f, 0.0f};
    Vec3 bayN = normal - bayT * normal.dot(bayT);
    bayN = (bayN.length() > 1e-4f) ? bayN.normalized() : bayT.cross(upVec).normalized();
    if (bayN.dot(normal) < 0.0f) {
        bayN = -bayN;
    }

    bool isSpiderGlass = (wallType == WallMaterialType::SpiderPointFixedGlass);
    float spandrelH = (wallType == WallMaterialType::StructuralSiliconeGlazing || isSpiderGlass)
        ? 0.0f
        : std::clamp(bayH * 0.22f, 0.45f, 0.85f);
    float visionBottomY = floorBottomY + spandrelH;
    float visionTopY = floorTopY - 0.08f;

    // 1. Spandrel Zone
    if (spandrelH > 0.0f) {
        appendQuad(vertices, indices,
            bayStart + Vec3{0, floorBottomY, 0}, bayEnd + Vec3{0, floorBottomY, 0},
            bayEnd + Vec3{0, visionBottomY, 0}, bayStart + Vec3{0, visionBottomY, 0},
            bayN, spandrelMat);

        // Horizontal aluminum transom cap aligned cleanly along bayStart -> bayEnd
        Vec3 tCenter = bayCenter + Vec3{0, visionBottomY - 0.03f, 0} + bayN * 0.03f;
        appendOrientedBox(vertices, indices, tCenter, bayT, upVec, bayN,
            bayWidth * 0.5f, 0.03f, 0.05f, frameMat);
    }

    // 2. Full-Height Panoramic Vision Glass
    Vec3 gOffset = bayN * 0.03f;
    appendQuad(vertices, indices,
        bayStart + gOffset + Vec3{0, visionBottomY, 0}, bayEnd + gOffset + Vec3{0, visionBottomY, 0},
        bayEnd + gOffset + Vec3{0, visionTopY, 0}, bayStart + gOffset + Vec3{0, visionTopY, 0},
        bayN, glassMat);

    // 3. Vertical Framing / Mullions OR Spider-Point Fittings
    if (isSpiderGlass) {
        // Structural stainless steel 4-way spider nodes with articulated glass bolts
        auto appendSpiderFitting = [&](const Vec3& pt) {
            float armLen = 0.16f;
            // Central structural boss
            appendOrientedBox(vertices, indices,
                pt + bayN * 0.0425f, bayT, upVec, bayN,
                0.03f, 0.03f, 0.0225f, frameMat);

            // 4 stainless steel tension arms
            Vec3 d0 = (bayT * 0.707f + Vec3{0, 0.707f, 0}) * armLen;
            Vec3 d1 = (-bayT * 0.707f + Vec3{0, 0.707f, 0}) * armLen;
            Vec3 d2 = (-bayT * 0.707f - Vec3{0, 0.707f, 0}) * armLen;
            Vec3 d3 = (bayT * 0.707f - Vec3{0, 0.707f, 0}) * armLen;
            appendOrientedCylinder(vertices, indices, pt + bayN * 0.04f, pt + bayN * 0.04f + d0, 0.012f, 0.012f, 6, frameMat, true, true);
            appendOrientedCylinder(vertices, indices, pt + bayN * 0.04f, pt + bayN * 0.04f + d1, 0.012f, 0.012f, 6, frameMat, true, true);
            appendOrientedCylinder(vertices, indices, pt + bayN * 0.04f, pt + bayN * 0.04f + d2, 0.012f, 0.012f, 6, frameMat, true, true);
            appendOrientedCylinder(vertices, indices, pt + bayN * 0.04f, pt + bayN * 0.04f + d3, 0.012f, 0.012f, 6, frameMat, true, true);
        };

        float midY = (floorBottomY + floorTopY) * 0.5f;
        appendSpiderFitting(bayStart + Vec3{0, midY, 0});
        appendSpiderFitting(bayEnd + Vec3{0, midY, 0});
        if (bayWidth > 2.6f) {
            appendSpiderFitting(bayCenter + Vec3{0, midY, 0});
        }
    } else {
        float mw = (wallType == WallMaterialType::StructuralSiliconeGlazing) ? 0.02f : 0.07f;
        float md = (wallType == WallMaterialType::StructuralSiliconeGlazing) ? 0.03f : 0.12f;
        float midFloorY = (floorBottomY + floorTopY) * 0.5f;

        appendOrientedBox(vertices, indices,
            bayStart + Vec3{0, midFloorY, 0} + bayN * (md * 0.5f), bayT, upVec, bayN,
            mw * 0.5f, bayH * 0.5f, md * 0.5f, frameMat);

        appendOrientedBox(vertices, indices,
            bayEnd + Vec3{0, midFloorY, 0} + bayN * (md * 0.5f), bayT, upVec, bayN,
            mw * 0.5f, bayH * 0.5f, md * 0.5f, frameMat);

        if (bayWidth > 2.8f) {
            float midVisY = (visionBottomY + visionTopY) * 0.5f;
            float visH = visionTopY - visionBottomY;
            appendOrientedBox(vertices, indices,
                bayCenter + Vec3{0, midVisY, 0} + bayN * (md * 0.425f), bayT, upVec, bayN,
                mw * 0.5f, visH * 0.5f, md * 0.425f, frameMat);
        }
    }

    // 4. Horizontal Ceiling Head Transom aligned cleanly along bayStart -> bayEnd
    if (!isSpiderGlass) {
        Vec3 hCenter = bayCenter + Vec3{0, visionTopY - 0.03f, 0} + bayN * 0.025f;
        appendOrientedBox(vertices, indices, hCenter, bayT, upVec, bayN,
            bayWidth * 0.5f, 0.03f, 0.045f, frameMat);
    }

    // 5. Vertical Solar Fins
    float finD = 0.0f;
    if (wallType == WallMaterialType::BronzeAluminumSolarFins) finD = 0.28f;
    else if (wallType == WallMaterialType::DoubleSkinAerodynamicGlass) finD = 0.22f;
    else if (wallType == WallMaterialType::GlassCurtainWall) finD = 0.12f;
    else if (wallType == WallMaterialType::ChampagneAluminumGlass) finD = 0.15f;

    if (finD > 0.01f) {
        float finThick = 0.04f;
        float midFloorY = (floorBottomY + floorTopY) * 0.5f;
        appendOrientedBox(vertices, indices,
            bayCenter + Vec3{0, midFloorY, 0} + bayN * (0.08f + finD * 0.5f), bayT, upVec, bayN,
            finThick * 0.5f, bayH * 0.5f, finD * 0.5f, frameMat);
    }
}

void BuildingComponents::generateSlantedCurtainWallBay(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& p0,
    const Vec3& p1,
    const Vec3& p2,
    const Vec3& p3,
    const Vec3& outwardHint,
    int32_t bayCount,
    WallMaterialType wallType,
    bool isEntranceBay,
    DoorType doorType,
    float doorWidth,
    float doorHeight,
    bool hasCanopy,
    GlassType glassType
) {
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t spandrelMat = static_cast<uint32_t>(MaterialSlot::WallSpandrelGlass);

    if (wallType == WallMaterialType::BlackAluminumGlassGrid) {
        frameMat = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
        glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    } else if (wallType == WallMaterialType::StructuralSiliconeGlazing) {
        frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
        glassMat = static_cast<uint32_t>(MaterialSlot::GlassReflective);
    } else if (wallType == WallMaterialType::ChampagneAluminumGlass) {
        frameMat = static_cast<uint32_t>(MaterialSlot::WallChampagneAluminum);
        glassMat = static_cast<uint32_t>(MaterialSlot::GlassGold);
    } else if (wallType == WallMaterialType::DoubleSkinAerodynamicGlass) {
        frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
        glassMat = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    } else if (wallType == WallMaterialType::BronzeAluminumSolarFins) {
        frameMat = static_cast<uint32_t>(MaterialSlot::WallBronzeAluminum);
        glassMat = static_cast<uint32_t>(MaterialSlot::GlassBronze);
    } else if (wallType == WallMaterialType::GunmetalAluminumCassette) {
        frameMat = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
        glassMat = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    } else if (wallType == WallMaterialType::PerforatedAluminumBriseSoleil) {
        frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
        glassMat = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    } else if (wallType == WallMaterialType::SpiderPointFixedGlass) {
        frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
        glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    }

    if (glassType != GlassType::ClearFloat) {
        glassMat = getGlassMaterial(glassType);
    }

    Vec3 hint = outwardHint.length() > 1e-5f ? outwardHint.normalized() : Vec3{0.0f, 0.0f, 1.0f};
    auto orientNorm = [&](Vec3 n) -> Vec3 {
        float len = n.length();
        if (len < 1e-6f) return hint;
        n = n * (1.0f / len);
        if (n.dot(hint) < 0.0f) n = -n;
        return n;
    };

    auto evalSurf = [&](float u, float v) -> Vec3 {
        Vec3 bot = p0 * (1.0f - u) + p1 * u;
        Vec3 top = p3 * (1.0f - u) + p2 * u;
        return bot * (1.0f - v) + top * v;
    };

    auto appendTriPatch = [&](const Vec3& q0, const Vec3& q1, const Vec3& q2, const Vec3& q3, uint32_t matId) {
        Vec3 nA = orientNorm((q1 - q0).cross(q2 - q0));
        Vec3 nB = orientNorm((q2 - q0).cross(q3 - q0));

        uint32_t idx = static_cast<uint32_t>(vertices.size());
        vertices.push_back({q0, nA, {0.0f, 0.0f}, matId});
        vertices.push_back({q1, nA, {1.0f, 0.0f}, matId});
        vertices.push_back({q2, nA, {1.0f, 1.0f}, matId});
        indices.push_back(idx);
        indices.push_back(idx + 1);
        indices.push_back(idx + 2);

        idx = static_cast<uint32_t>(vertices.size());
        vertices.push_back({q0, nB, {0.0f, 0.0f}, matId});
        vertices.push_back({q2, nB, {1.0f, 1.0f}, matId});
        vertices.push_back({q3, nB, {0.0f, 1.0f}, matId});
        indices.push_back(idx);
        indices.push_back(idx + 1);
        indices.push_back(idx + 2);
    };

    int32_t numBays = std::max(1, bayCount);
    bool isSpiderGlass = (wallType == WallMaterialType::SpiderPointFixedGlass);
    bool isSSG = (wallType == WallMaterialType::StructuralSiliconeGlazing);
    float spandrelFrac = (isSSG || isSpiderGlass) ? 0.0f : 0.22f;
    float mullionRad = isSSG ? 0.025f : 0.062f;

    float finD = 0.0f;
    if (wallType == WallMaterialType::BronzeAluminumSolarFins) finD = 0.28f;
    else if (wallType == WallMaterialType::DoubleSkinAerodynamicGlass) finD = 0.22f;
    else if (wallType == WallMaterialType::GlassCurtainWall) finD = 0.12f;
    else if (wallType == WallMaterialType::ChampagneAluminumGlass) finD = 0.15f;

    for (int32_t b = 0; b < numBays; ++b) {
        float u0 = static_cast<float>(b) / static_cast<float>(numBays);
        float u1 = static_cast<float>(b + 1) / static_cast<float>(numBays);
        float uMid = (u0 + u1) * 0.5f;

        Vec3 b0 = evalSurf(u0, 0.0f);
        Vec3 b1 = evalSurf(u1, 0.0f);
        Vec3 t0 = evalSurf(u0, 1.0f);
        Vec3 t1 = evalSurf(u1, 1.0f);

        Vec3 bayTan = (b1 - b0).normalized();
        Vec3 bayUpDir = (evalSurf(uMid, 1.0f) - evalSurf(uMid, 0.0f)).normalized();
        Vec3 bayNorm = orientNorm(bayTan.cross(bayUpDir));
        float bayW = (b1 - b0).length();
        float bayH = ((t0 + t1) * 0.5f - (b0 + b1) * 0.5f).length();

        if (isEntranceBay && b == (numBays / 2) && doorType != DoorType::None) {
            float dw = std::min(bayW * 0.85f, doorWidth);
            float dh = std::min(bayH - 0.35f, doorHeight);
            Vec3 doorCenterGround = (b0 + b1) * 0.5f + bayNorm * 0.03f;
            appendTriPatch(b0 + bayNorm * 0.015f, b1 + bayNorm * 0.015f, t1 + bayNorm * 0.015f, t0 + bayNorm * 0.015f, glassMat);
            generateDoor(vertices, indices, doorCenterGround, dw, dh, bayNorm, bayTan, doorType, hasCanopy);
        } else {
            // 1. Spandrel glass zone + transom cap
            if (spandrelFrac > 0.0f) {
                Vec3 sT0 = evalSurf(u0, spandrelFrac);
                Vec3 sT1 = evalSurf(u1, spandrelFrac);
                appendTriPatch(b0, b1, sT1, sT0, spandrelMat);
                appendOrientedCylinder(vertices, indices, sT0 + bayNorm * 0.045f, sT1 + bayNorm * 0.045f, 0.052f, 0.052f, 6, frameMat, true, true);
            }

            // 2. Vision glass pane
            Vec3 gOff = bayNorm * 0.018f;
            Vec3 vB0 = evalSurf(u0, spandrelFrac) + gOff;
            Vec3 vB1 = evalSurf(u1, spandrelFrac) + gOff;
            Vec3 vT0 = evalSurf(u0, 0.985f) + gOff;
            Vec3 vT1 = evalSurf(u1, 0.985f) + gOff;
            appendTriPatch(vB0, vB1, vT1, vT0, glassMat);

            // 3. Horizontal sill & ceiling head frame bars
            if (!isSpiderGlass) {
                appendOrientedCylinder(vertices, indices, b0 + bayNorm * 0.05f, b1 + bayNorm * 0.05f, 0.058f, 0.058f, 6, frameMat, true, true);
                appendOrientedCylinder(vertices, indices, t0 + bayNorm * 0.05f, t1 + bayNorm * 0.05f, 0.055f, 0.055f, 6, frameMat, true, true);
            }

            // 4. Vertical/slanted framing mullions or spider-point nodes
            if (isSpiderGlass) {
                auto appendNode = [&](const Vec3& pt) {
                    appendOrientedCylinder(vertices, indices, pt + bayNorm * 0.02f, pt + bayNorm * 0.08f, 0.04f, 0.03f, 6, frameMat, true, true);
                    Vec3 armA = (bayTan * 0.707f + bayUpDir * 0.707f) * 0.16f;
                    Vec3 armB = (-bayTan * 0.707f + bayUpDir * 0.707f) * 0.16f;
                    appendOrientedCylinder(vertices, indices, pt + bayNorm * 0.05f - armA, pt + bayNorm * 0.05f + armA, 0.014f, 0.014f, 6, frameMat, true, true);
                    appendOrientedCylinder(vertices, indices, pt + bayNorm * 0.05f - armB, pt + bayNorm * 0.05f + armB, 0.014f, 0.014f, 6, frameMat, true, true);
                };
                appendNode(evalSurf(u0, 0.5f));
                appendNode(evalSurf(u1, 0.5f));
            } else {
                appendOrientedCylinder(vertices, indices, b0 + bayNorm * 0.05f, t0 + bayNorm * 0.05f, mullionRad, mullionRad, 6, frameMat, true, true);
                if (b == numBays - 1) {
                    appendOrientedCylinder(vertices, indices, b1 + bayNorm * 0.05f, t1 + bayNorm * 0.05f, mullionRad, mullionRad, 6, frameMat, true, true);
                }
                if (bayW > 2.8f) {
                    Vec3 midB = evalSurf(uMid, spandrelFrac) + bayNorm * 0.045f;
                    Vec3 midT = evalSurf(uMid, 0.985f) + bayNorm * 0.045f;
                    appendOrientedCylinder(vertices, indices, midB, midT, mullionRad * 0.85f, mullionRad * 0.85f, 6, frameMat, true, true);
                }
                if (finD > 0.01f) {
                    Vec3 fB = evalSurf(uMid, 0.0f) + bayNorm * 0.04f;
                    Vec3 fT = evalSurf(uMid, 1.0f) + bayNorm * 0.04f;
                    Vec3 finCenter = (fB + fT) * 0.5f + bayNorm * (finD * 0.5f);
                    float finHeight = (fT - fB).length();
                    appendOrientedBox(vertices, indices,
                        finCenter, bayTan, bayUpDir, bayNorm,
                        0.02f, finHeight * 0.5f, finD * 0.5f, frameMat);
                }
            }
        }
    }
}

void BuildingComponents::generateWindow(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& center,
    float width,
    float height,
    const Vec3& normal,
    const Vec3& tangent,
    WindowType type,
    GlassType glassType
) {
    Vec3 up{0.0f, 1.0f, 0.0f};
    Vec3 right = tangent.normalized();
    float hw = width * 0.5f;
    float hh = height * 0.5f;

    float frameThickness = std::min(0.065f, std::max(0.014f, width * 0.11f));
    float frameDepth = 0.05f;
    float fw = frameThickness;
    float barHalfW = std::min(0.028f, fw * 0.45f);
    Vec3 glassOffset = normal * 0.018f;
    Vec3 casingOffset = normal * 0.048f;
    Vec3 mullionOffset = normal * 0.068f;
    Vec3 sillOffset = normal * 0.120f;

    // Corner points of the outer casing frame
    Vec3 pBL = center - right * hw - up * hh + casingOffset;
    Vec3 pBR = center + right * hw - up * hh + casingOffset;
    Vec3 pTR = center + right * hw + up * hh + casingOffset;
    Vec3 pTL = center - right * hw + up * hh + casingOffset;

    // Corner points of the recessed glass panel (strictly behind casing & mullions)
    Vec3 gBL = center - right * hw - up * hh + glassOffset;
    Vec3 gBR = center + right * hw - up * hh + glassOffset;
    Vec3 gTR = center + right * hw + up * hh + glassOffset;
    Vec3 gTL = center - right * hw + up * hh + glassOffset;

    // Left Jamb casing
    Vec3 lj0 = pBL - right * fw;
    Vec3 lj1 = pBL;
    Vec3 lj2 = pTL;
    Vec3 lj3 = pTL - right * fw;
    appendQuad(vertices, indices, lj0, lj1, lj2, lj3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));

    // Right Jamb casing
    Vec3 rj0 = pBR;
    Vec3 rj1 = pBR + right * fw;
    Vec3 rj2 = pTR + right * fw;
    Vec3 rj3 = pTR;
    appendQuad(vertices, indices, rj0, rj1, rj2, rj3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));

    // Top Lintel Header casing
    Vec3 th0 = pTL - right * fw;
    Vec3 th1 = pTR + right * fw;
    Vec3 th2 = th1 + up * fw;
    Vec3 th3 = th0 + up * fw;
    appendQuad(vertices, indices, th0, th1, th2, th3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));

    // Bottom Sill shelf with forward protrusion for maximum clarity from any viewpoint
    Vec3 sillExtend = right * (fw + std::min(0.035f, fw * 0.55f));
    Vec3 sillBL = center - right * hw - up * (hh + fw * 0.9f) - sillExtend + sillOffset;
    Vec3 sillBR = center + right * hw - up * (hh + fw * 0.9f) + sillExtend + sillOffset;
    Vec3 sillTR = center + right * hw - up * hh + sillExtend + sillOffset;
    Vec3 sillTL = center - right * hw - up * hh - sillExtend + sillOffset;
    appendQuad(vertices, indices, sillBL, sillBR, sillTR, sillTL, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));

    // Recessed Glass panel cleanly behind outer frame and mullions
    uint32_t glassMat = getGlassMaterial(glassType);
    appendQuad(vertices, indices, gBL, gBR, gTR, gTL, normal, glassMat);

    // Window style specific mullions / panes (placed at mullionOffset cleanly in front of glass)
    switch (type) {
        case WindowType::ModernCasement: {
            Vec3 barP0 = center - right * (fw * 0.4f) - up * hh + mullionOffset;
            Vec3 barP1 = center + right * (fw * 0.4f) - up * hh + mullionOffset;
            Vec3 barP2 = center + right * (fw * 0.4f) + up * hh + mullionOffset;
            Vec3 barP3 = center - right * (fw * 0.4f) + up * hh + mullionOffset;
            appendQuad(vertices, indices, barP0, barP1, barP2, barP3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));
            break;
        }
        case WindowType::DoubleHungSash: {
            float midY = -hh + height * 0.55f;
            Vec3 h0 = center - right * hw + up * (midY - fw * 0.5f) + mullionOffset;
            Vec3 h1 = center + right * hw + up * (midY - fw * 0.5f) + mullionOffset;
            Vec3 h2 = center + right * hw + up * (midY + fw * 0.5f) + mullionOffset;
            Vec3 h3 = center - right * hw + up * (midY + fw * 0.5f) + mullionOffset;
            appendQuad(vertices, indices, h0, h1, h2, h3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));
            break;
        }
        case WindowType::PictureGlass: {
            float beadW = std::min(0.025f, fw * 0.45f);
            Vec3 bTop0 = pTL; Vec3 bTop1 = pTR; Vec3 bTop2 = pTR - up * beadW; Vec3 bTop3 = pTL - up * beadW;
            appendQuad(vertices, indices, bTop0, bTop1, bTop2, bTop3, normal, static_cast<uint32_t>(MaterialSlot::WallBlackAluminum));
            Vec3 bBot0 = pBL; Vec3 bBot1 = pBR; Vec3 bBot2 = pBR + up * beadW; Vec3 bBot3 = pBL + up * beadW;
            appendQuad(vertices, indices, bBot0, bBot1, bBot2, bBot3, normal, static_cast<uint32_t>(MaterialSlot::WallBlackAluminum));
            Vec3 bL0 = pBL; Vec3 bL1 = pBL + right * beadW; Vec3 bL2 = pTL + right * beadW; Vec3 bL3 = pTL;
            appendQuad(vertices, indices, bL0, bL1, bL2, bL3, normal, static_cast<uint32_t>(MaterialSlot::WallBlackAluminum));
            Vec3 bR0 = pBR - right * beadW; Vec3 bR1 = pBR; Vec3 bR2 = pTR; Vec3 bR3 = pTR - right * beadW;
            appendQuad(vertices, indices, bR0, bR1, bR2, bR3, normal, static_cast<uint32_t>(MaterialSlot::WallBlackAluminum));
            break;
        }
        case WindowType::CurtainWall:
            break;

        case WindowType::RibbonStrip: {
            int slats = 2;
            for (int i = 1; i <= slats; ++i) {
                float frac = static_cast<float>(i) / (slats + 1);
                float yPos = -hh + height * frac;
                Vec3 h0 = center - right * hw + up * (yPos - barHalfW) + mullionOffset;
                Vec3 h1 = center + right * hw + up * (yPos - barHalfW) + mullionOffset;
                Vec3 h2 = center + right * hw + up * (yPos + barHalfW) + mullionOffset;
                Vec3 h3 = center - right * hw + up * (yPos + barHalfW) + mullionOffset;
                appendQuad(vertices, indices, h0, h1, h2, h3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));
            }
            break;
        }
        case WindowType::ArchedHistoric: {
            Vec3 vm0 = center - right * (fw * 0.4f) - up * hh + mullionOffset;
            Vec3 vm1 = center + right * (fw * 0.4f) - up * hh + mullionOffset;
            Vec3 vm2 = center + right * (fw * 0.4f) + up * hh + mullionOffset;
            Vec3 vm3 = center - right * (fw * 0.4f) + up * hh + mullionOffset;
            appendQuad(vertices, indices, vm0, vm1, vm2, vm3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));

            float yPos = 0.0f;
            Vec3 h0 = center - right * hw + up * (yPos - barHalfW) + mullionOffset;
            Vec3 h1 = center + right * hw + up * (yPos - barHalfW) + mullionOffset;
            Vec3 h2 = center + right * hw + up * (yPos + barHalfW) + mullionOffset;
            Vec3 h3 = center - right * hw + up * (yPos + barHalfW) + mullionOffset;
            appendQuad(vertices, indices, h0, h1, h2, h3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));
            break;
        }
        case WindowType::MultiPaneFactory: {
            float mulHalf = std::min(0.02f, fw * 0.35f);
            for (float frac : {-0.33f, 0.33f}) {
                Vec3 vm0 = center + right * (hw * frac - mulHalf) - up * hh + mullionOffset;
                Vec3 vm1 = center + right * (hw * frac + mulHalf) - up * hh + mullionOffset;
                Vec3 vm2 = center + right * (hw * frac + mulHalf) + up * hh + mullionOffset;
                Vec3 vm3 = center + right * (hw * frac - mulHalf) + up * hh + mullionOffset;
                appendQuad(vertices, indices, vm0, vm1, vm2, vm3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));

                Vec3 hm0 = center - right * hw + up * (hh * frac - mulHalf) + mullionOffset;
                Vec3 hm1 = center + right * hw + up * (hh * frac - mulHalf) + mullionOffset;
                Vec3 hm2 = center + right * hw + up * (hh * frac + mulHalf) + mullionOffset;
                Vec3 hm3 = center - right * hw + up * (hh * frac + mulHalf) + mullionOffset;
                appendQuad(vertices, indices, hm0, hm1, hm2, hm3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));
            }
            break;
        }
        case WindowType::BayWindow: {
            float bayProj = 0.35f;
            float frontHw = hw * 0.68f;
            Vec3 fBL = center - right * frontHw - up * hh + normal * bayProj;
            Vec3 fBR = center + right * frontHw - up * hh + normal * bayProj;
            Vec3 fTR = center + right * frontHw + up * hh + normal * bayProj;
            Vec3 fTL = center - right * frontHw + up * hh + normal * bayProj;

            appendQuad(vertices, indices, fBL, fBR, fTR, fTL, normal, glassMat);
            Vec3 leftNormal = (-right + normal).normalized();
            appendQuad(vertices, indices, pBL, fBL, fTL, pTL, leftNormal, glassMat);
            Vec3 rightNormal = (right + normal).normalized();
            appendQuad(vertices, indices, fBR, pBR, pTR, fTR, rightNormal, glassMat);
            appendQuad(vertices, indices, pTL, pTR, fTR, fTL, up, static_cast<uint32_t>(MaterialSlot::RoofSurface));
            appendQuad(vertices, indices, pBL, fBL, fBR, pBR, -up, static_cast<uint32_t>(MaterialSlot::Foundation));
            break;
        }
        case WindowType::CornerWrap: {
            for (float frac : {-0.3f, 0.3f}) {
                Vec3 b0 = center - right * hw + up * (hh * frac - barHalfW) + mullionOffset;
                Vec3 b1 = center + right * hw + up * (hh * frac - barHalfW) + mullionOffset;
                Vec3 b2 = center + right * hw + up * (hh * frac + barHalfW) + mullionOffset;
                Vec3 b3 = center - right * hw + up * (hh * frac + barHalfW) + mullionOffset;
                appendQuad(vertices, indices, b0, b1, b2, b3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));
            }
            break;
        }
        case WindowType::ClerestoryTransom: {
            int barCount = 4;
            float barHalf = std::min(0.015f, fw * 0.3f);
            for (int b = 1; b <= barCount; ++b) {
                float frac = -1.0f + 2.0f * (static_cast<float>(b) / (barCount + 1));
                Vec3 b0 = center + right * (hw * frac - barHalf) - up * hh + mullionOffset;
                Vec3 b1 = center + right * (hw * frac + barHalf) - up * hh + mullionOffset;
                Vec3 b2 = center + right * (hw * frac + barHalf) + up * hh + mullionOffset;
                Vec3 b3 = center + right * (hw * frac - barHalf) + up * hh + mullionOffset;
                appendQuad(vertices, indices, b0, b1, b2, b3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));
            }
            break;
        }
        case WindowType::RoundOculus: {
            Vec3 cVert0 = center - right * barHalfW - up * hh + mullionOffset;
            Vec3 cVert1 = center + right * barHalfW - up * hh + mullionOffset;
            Vec3 cVert2 = center + right * barHalfW + up * hh + mullionOffset;
            Vec3 cVert3 = center - right * barHalfW + up * hh + mullionOffset;
            appendQuad(vertices, indices, cVert0, cVert1, cVert2, cVert3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));

            Vec3 cHoriz0 = center - right * hw - up * barHalfW + mullionOffset;
            Vec3 cHoriz1 = center + right * hw - up * barHalfW + mullionOffset;
            Vec3 cHoriz2 = center + right * hw + up * barHalfW + mullionOffset;
            Vec3 cHoriz3 = center - right * hw + up * barHalfW + mullionOffset;
            appendQuad(vertices, indices, cHoriz0, cHoriz1, cHoriz2, cHoriz3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));
            break;
        }
        case WindowType::LouveredJalousie: {
            int slats = 5;
            float inset = std::min(0.02f, hw * 0.15f);
            for (int s = 0; s < slats; ++s) {
                float yB = -hh + (height * (static_cast<float>(s) / slats));
                float yT = -hh + (height * (static_cast<float>(s + 1) / slats));
                Vec3 l0 = center - right * (hw - inset) + up * yB + normal * 0.085f;
                Vec3 l1 = center + right * (hw - inset) + up * yB + normal * 0.085f;
                Vec3 l2 = center + right * (hw - inset) + up * yT + normal * 0.030f;
                Vec3 l3 = center - right * (hw - inset) + up * yT + normal * 0.030f;
                appendQuad(vertices, indices, l0, l1, l2, l3, normal, glassMat);
            }
            break;
        }
        case WindowType::ArchedGothicLancet: {
            float barHalf = std::min(0.02f, fw * 0.35f);
            for (float side : {-0.32f, 0.32f}) {
                Vec3 vm0 = center + right * (hw * side - barHalf) - up * hh + mullionOffset;
                Vec3 vm1 = center + right * (hw * side + barHalf) - up * hh + mullionOffset;
                Vec3 vm2 = center + right * (hw * side + barHalf) + up * (hh * 0.5f) + mullionOffset;
                Vec3 vm3 = center + right * (hw * side - barHalf) + up * (hh * 0.5f) + mullionOffset;
                appendQuad(vertices, indices, vm0, vm1, vm2, vm3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));
            }
            Vec3 arch0 = center - right * hw + up * (hh * 0.5f - barHalfW) + mullionOffset;
            Vec3 arch1 = center + right * hw + up * (hh * 0.5f - barHalfW) + mullionOffset;
            Vec3 arch2 = center + right * hw + up * (hh * 0.5f + barHalfW) + mullionOffset;
            Vec3 arch3 = center - right * hw + up * (hh * 0.5f + barHalfW) + mullionOffset;
            appendQuad(vertices, indices, arch0, arch1, arch2, arch3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));
            break;
        }
        case WindowType::DiamondLeadGlazing: {
            uint32_t leadMat = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
            float leadRad = std::min(0.012f, fw * 0.25f);
            const int diagonalCount = 3;
            for (int d = -diagonalCount; d <= diagonalCount; ++d) {
                float shift = d * (hw * 0.45f);
                Vec3 p0 = center + right * shift - up * hh + mullionOffset;
                Vec3 p1 = center + right * (shift + hw * 0.9f) + up * hh + mullionOffset;
                appendOrientedCylinder(vertices, indices, p0, p1, leadRad, leadRad, 6, leadMat, false, false);
                Vec3 q0 = center + right * shift + up * hh + mullionOffset;
                Vec3 q1 = center + right * (shift + hw * 0.9f) - up * hh + mullionOffset;
                appendOrientedCylinder(vertices, indices, q0, q1, leadRad, leadRad, 6, leadMat, false, false);
            }
            break;
        }
        case WindowType::PivotingHorizontalHopper: {
            uint32_t metalMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
            Vec3 ax0 = center - right * hw + mullionOffset;
            Vec3 ax1 = center + right * hw + mullionOffset;
            appendOrientedCylinder(vertices, indices, ax0, ax1, 0.022f, 0.022f, 8, metalMat, true, true);
            Vec3 sTop0 = ax0 + normal * 0.12f + up * (hh * 0.95f);
            Vec3 sTop1 = ax1 + normal * 0.12f + up * (hh * 0.95f);
            appendQuad(vertices, indices, ax0, ax1, sTop1, sTop0, normal, glassMat);
            appendOrientedCylinder(vertices, indices, ax0, sTop0, 0.014f, 0.014f, 6, metalMat, true, true);
            appendOrientedCylinder(vertices, indices, ax1, sTop1, 0.014f, 0.014f, 6, metalMat, true, true);
            break;
        }
        case WindowType::PivotingVerticalSash: {
            uint32_t metalMat = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
            Vec3 sp0 = center - up * hh + mullionOffset;
            Vec3 sp1 = center + up * hh + mullionOffset;
            appendOrientedCylinder(vertices, indices, sp0, sp1, 0.024f, 0.024f, 8, metalMat, true, true);
            Vec3 lOut0 = sp0 - right * hw + normal * 0.18f;
            Vec3 lOut1 = sp1 - right * hw + normal * 0.18f;
            appendQuad(vertices, indices, lOut0, sp0, sp1, lOut1, normal, glassMat);
            Vec3 rIn0 = sp0 + right * hw - normal * 0.14f;
            Vec3 rIn1 = sp1 + right * hw - normal * 0.14f;
            appendQuad(vertices, indices, sp0, rIn0, rIn1, sp1, normal, glassMat);
            break;
        }
        case WindowType::OctagonalBullseye: {
            uint32_t metalMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
            float chamfer = std::min(hw, hh) * 0.35f;
            appendQuad(vertices, indices, pTL, pTL + right * chamfer, pTL - up * chamfer, pTL, normal, metalMat);
            appendQuad(vertices, indices, pTR - right * chamfer, pTR, pTR, pTR - up * chamfer, normal, metalMat);
            appendQuad(vertices, indices, pBL, pBL - up * 0.01f, pBL + up * chamfer, pBL + right * chamfer, normal, metalMat);
            appendQuad(vertices, indices, pBR - right * chamfer, pBR - up * 0.01f, pBR, pBR + up * chamfer, normal, metalMat);
            appendOrientedCylinder(vertices, indices, center + casingOffset, center + mullionOffset * 1.15f, std::min(0.08f, hw * 0.25f), std::min(0.08f, hw * 0.25f), 8, metalMat, true, true);
            for (int k = 0; k < 8; ++k) {
                float ang = k * (kPi * 0.25f);
                Vec3 spokeEnd = center + right * (std::cos(ang) * std::max(0.02f, hw - 0.04f)) + up * (std::sin(ang) * std::max(0.02f, hh - 0.04f)) + mullionOffset;
                appendOrientedCylinder(vertices, indices, center + mullionOffset, spokeEnd, 0.012f, 0.012f, 6, metalMat, false, true);
            }
            break;
        }
        case WindowType::PalladianTripleArched: {
            uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
            float sideW = hw * 0.45f;
            float mulHalf = std::min(0.032f, fw * 0.5f);
            for (float mx : {-sideW, sideW}) {
                Vec3 m0 = center + right * (mx - mulHalf) - up * hh + mullionOffset;
                Vec3 m1 = center + right * (mx + mulHalf) - up * hh + mullionOffset;
                Vec3 m2 = center + right * (mx + mulHalf) + up * (hh * 0.35f) + mullionOffset;
                Vec3 m3 = center + right * (mx - mulHalf) + up * (hh * 0.35f) + mullionOffset;
                appendQuad(vertices, indices, m0, m1, m2, m3, normal, frameMat);
            }
            Vec3 trL0 = center - right * hw + up * (hh * 0.35f - barHalfW) + mullionOffset;
            Vec3 trL1 = center - right * sideW + up * (hh * 0.35f - barHalfW) + mullionOffset;
            Vec3 trL2 = center - right * sideW + up * (hh * 0.35f + barHalfW) + mullionOffset;
            Vec3 trL3 = center - right * hw + up * (hh * 0.35f + barHalfW) + mullionOffset;
            appendQuad(vertices, indices, trL0, trL1, trL2, trL3, normal, frameMat);

            Vec3 trR0 = center + right * sideW + up * (hh * 0.35f - barHalfW) + mullionOffset;
            Vec3 trR1 = center + right * hw + up * (hh * 0.35f - barHalfW) + mullionOffset;
            Vec3 trR2 = center + right * hw + up * (hh * 0.35f + barHalfW) + mullionOffset;
            Vec3 trR3 = center + right * sideW + up * (hh * 0.35f + barHalfW) + mullionOffset;
            appendQuad(vertices, indices, trR0, trR1, trR2, trR3, normal, frameMat);

            int arcSteps = 12;
            float archR = sideW;
            Vec3 archCenter = center + up * (hh * 0.35f) + mullionOffset;
            for (int a = 0; a < arcSteps; ++a) {
                float t0 = (static_cast<float>(a) / arcSteps) * kPi;
                float t1 = (static_cast<float>(a + 1) / arcSteps) * kPi;
                Vec3 a0 = archCenter + right * (std::cos(t0) * archR) + up * (std::sin(t0) * archR * 0.65f);
                Vec3 a1 = archCenter + right * (std::cos(t1) * archR) + up * (std::sin(t1) * archR * 0.65f);
                appendOrientedCylinder(vertices, indices, a0, a1, 0.02f, 0.02f, 6, frameMat, true, true);
            }
            break;
        }
        case WindowType::TriangularApexTransom: {
            uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
            Vec3 tBL = center - right * hw - up * hh + mullionOffset;
            Vec3 tBR = center + right * hw - up * hh + mullionOffset;
            Vec3 apex = center + up * hh + mullionOffset;
            float rakeRad = std::min(0.032f, fw * 0.5f);
            appendOrientedCylinder(vertices, indices, tBL, apex, rakeRad, rakeRad, 8, frameMat, true, true);
            appendOrientedCylinder(vertices, indices, tBR, apex, rakeRad, rakeRad, 8, frameMat, true, true);
            appendOrientedCylinder(vertices, indices, tBL, tBR, rakeRad, rakeRad, 8, frameMat, true, true);
            for (float frac : {-0.55f, -0.2f, 0.2f, 0.55f}) {
                Vec3 bPt = center + right * (hw * frac) - up * hh + mullionOffset;
                appendOrientedCylinder(vertices, indices, bPt, apex, 0.014f, 0.014f, 6, frameMat, false, false);
            }
            break;
        }
        case WindowType::HexagonalHoneycombAperture: {
            uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::WallChampagneAluminum);
            const int hexSides = 6;
            float hexR = std::min(hw, hh) * 0.95f;
            for (int s = 0; s < hexSides; ++s) {
                float a0 = s * (kPi / 3.0f);
                float a1 = (s + 1) * (kPi / 3.0f);
                Vec3 h0 = center + right * (std::cos(a0) * hexR) + up * (std::sin(a0) * hexR) + mullionOffset;
                Vec3 h1 = center + right * (std::cos(a1) * hexR) + up * (std::sin(a1) * hexR) + mullionOffset;
                appendOrientedCylinder(vertices, indices, h0, h1, 0.024f, 0.024f, 8, frameMat, true, true);
                appendOrientedCylinder(vertices, indices, center + mullionOffset, h0, 0.014f, 0.014f, 6, frameMat, false, false);
            }
            break;
        }
        case WindowType::BriseSoleilDeepFinned: {
            uint32_t finMat = static_cast<uint32_t>(MaterialSlot::WallBronzeAluminum);
            int finCount = 4;
            float finDepth = 0.25f;
            float finHalf = std::min(0.015f, fw * 0.3f);
            for (int f = 0; f < finCount; ++f) {
                float frac = -1.0f + 2.0f * (static_cast<float>(f) / (finCount - 1));
                float fx = hw * frac * 0.88f;
                Vec3 fBase0 = center + right * (fx - finHalf) - up * hh + casingOffset;
                Vec3 fBase1 = center + right * (fx + finHalf) - up * hh + casingOffset;
                Vec3 fBase2 = center + right * (fx + finHalf) + up * hh + casingOffset;
                Vec3 fBase3 = center + right * (fx - finHalf) + up * hh + casingOffset;

                Vec3 fTip0 = fBase0 + normal * finDepth;
                Vec3 fTip1 = fBase1 + normal * finDepth;
                Vec3 fTip2 = fBase2 + normal * finDepth;
                Vec3 fTip3 = fBase3 + normal * finDepth;

                appendQuad(vertices, indices, fBase0, fTip0, fTip3, fBase3, -right, finMat);
                appendQuad(vertices, indices, fTip1, fBase1, fBase2, fTip2, right, finMat);
                appendQuad(vertices, indices, fTip0, fTip1, fTip2, fTip3, normal, finMat);
            }
            break;
        }
        case WindowType::SlidingPocketSash: {
            uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
            float mulHalf = std::min(0.032f, fw * 0.5f);
            Vec3 mr0 = center - right * mulHalf - up * hh + mullionOffset;
            Vec3 mr1 = center + right * mulHalf - up * hh + mullionOffset;
            Vec3 mr2 = center + right * mulHalf + up * hh + mullionOffset;
            Vec3 mr3 = center - right * mulHalf + up * hh + mullionOffset;
            appendQuad(vertices, indices, mr0, mr1, mr2, mr3, normal, frameMat);
            appendOrientedBox(vertices, indices,
                center + right * 0.02f + normal * 0.095f,
                right, up, normal,
                0.01f, 0.12f, 0.01f,
                static_cast<uint32_t>(MaterialSlot::BrassHardware));
            break;
        }
        case WindowType::FrenchCasementFullHeight: {
            uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
            float trY = -hh + height * 0.75f;
            Vec3 tr0 = center - right * hw + up * (trY - barHalfW) + mullionOffset;
            Vec3 tr1 = center + right * hw + up * (trY - barHalfW) + mullionOffset;
            Vec3 tr2 = center + right * hw + up * (trY + barHalfW) + mullionOffset;
            Vec3 tr3 = center - right * hw + up * (trY + barHalfW) + mullionOffset;
            appendQuad(vertices, indices, tr0, tr1, tr2, tr3, normal, frameMat);
            Vec3 as0 = center - right * barHalfW - up * hh + mullionOffset;
            Vec3 as1 = center + right * barHalfW - up * hh + mullionOffset;
            Vec3 as2 = center + right * barHalfW + up * trY + mullionOffset;
            Vec3 as3 = center - right * barHalfW + up * trY + mullionOffset;
            appendQuad(vertices, indices, as0, as1, as2, as3, normal, frameMat);
            appendOrientedCylinder(vertices, indices,
                center + right * 0.04f - up * (hh * 0.6f) + normal * 0.085f,
                center + right * 0.04f + up * (trY * 0.8f) + normal * 0.085f,
                0.010f, 0.010f, 6, static_cast<uint32_t>(MaterialSlot::BrassHardware), true, true);
            break;
        }
    }
}

void BuildingComponents::generateDoor(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& centerGround,
    float width,
    float height,
    const Vec3& normal,
    const Vec3& tangent,
    DoorType type,
    bool withCanopy
) {
    if (type == DoorType::None) return;

    Vec3 up{0.0f, 1.0f, 0.0f};
    Vec3 right = tangent.normalized();
    float hw = width * 0.5f;

    Vec3 doorCenter = centerGround + up * (height * 0.5f);
    float doorRelief = 0.06f;
    Vec3 doorOffset = normal * doorRelief;

    // Corner points of door leaf
    Vec3 pBL = doorCenter - right * hw - up * (height * 0.5f) + doorOffset;
    Vec3 pBR = doorCenter + right * hw - up * (height * 0.5f) + doorOffset;
    Vec3 pTR = doorCenter + right * hw + up * (height * 0.5f) + doorOffset;
    Vec3 pTL = doorCenter - right * hw + up * (height * 0.5f) + doorOffset;

    uint32_t mat = (type == DoorType::DoubleGlassCommercial || type == DoorType::RevolvingDoor ||
                    type == DoorType::AutomaticSliding || type == DoorType::FrenchDoublePatio) ?
        static_cast<uint32_t>(MaterialSlot::Glass) :
        (type == DoorType::SecurityGateGrille ? static_cast<uint32_t>(MaterialSlot::FrameMetal) :
         static_cast<uint32_t>(MaterialSlot::DoorWoodMetal));

    appendQuad(vertices, indices, pBL, pBR, pTR, pTL, normal, mat);

    // Complete 3-sided Outer Door Casing
    float fw = 0.12f;
    Vec3 casingOffset = normal * (doorRelief + 0.04f);

    // Left Jamb
    Vec3 jL0 = pBL - right * fw;
    Vec3 jL1 = pBL;
    Vec3 jL2 = pTL;
    Vec3 jL3 = pTL - right * fw;
    appendQuad(vertices, indices, jL0, jL1, jL2, jL3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));

    // Right Jamb
    Vec3 jR0 = pBR;
    Vec3 jR1 = pBR + right * fw;
    Vec3 jR2 = pTR + right * fw;
    Vec3 jR3 = pTR;
    appendQuad(vertices, indices, jR0, jR1, jR2, jR3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));

    // Top Header Lintel
    Vec3 fTop0 = pTL - right * fw;
    Vec3 fTop1 = pTR + right * fw;
    Vec3 fTop2 = fTop1 + up * fw;
    Vec3 fTop3 = fTop0 + up * fw;
    appendQuad(vertices, indices, fTop0, fTop1, fTop2, fTop3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));

    // Style-specific door elements
    switch (type) {
        case DoorType::DoubleGlassCommercial: {
            // Center vertical divider
            Vec3 div0 = doorCenter - right * 0.03f - up * (height * 0.5f) + casingOffset;
            Vec3 div1 = doorCenter + right * 0.03f - up * (height * 0.5f) + casingOffset;
            Vec3 div2 = doorCenter + right * 0.03f + up * (height * 0.5f) + casingOffset;
            Vec3 div3 = doorCenter - right * 0.03f + up * (height * 0.5f) + casingOffset;
            appendQuad(vertices, indices, div0, div1, div2, div3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));

            // High-contrast vertical push handles
            Vec3 hL0 = doorCenter - right * 0.12f - up * 0.15f + normal * (doorRelief + 0.08f);
            Vec3 hL1 = doorCenter - right * 0.08f - up * 0.15f + normal * (doorRelief + 0.08f);
            Vec3 hL2 = doorCenter - right * 0.08f + up * 0.20f + normal * (doorRelief + 0.08f);
            Vec3 hL3 = doorCenter - right * 0.12f + up * 0.20f + normal * (doorRelief + 0.08f);
            appendQuad(vertices, indices, hL0, hL1, hL2, hL3, normal, static_cast<uint32_t>(MaterialSlot::HvacMetal));

            Vec3 hR0 = doorCenter + right * 0.08f - up * 0.15f + normal * (doorRelief + 0.08f);
            Vec3 hR1 = doorCenter + right * 0.12f - up * 0.15f + normal * (doorRelief + 0.08f);
            Vec3 hR2 = doorCenter + right * 0.12f + up * 0.20f + normal * (doorRelief + 0.08f);
            Vec3 hR3 = doorCenter + right * 0.08f + up * 0.20f + normal * (doorRelief + 0.08f);
            appendQuad(vertices, indices, hR0, hR1, hR2, hR3, normal, static_cast<uint32_t>(MaterialSlot::HvacMetal));
            break;
        }
        case DoorType::RevolvingDoor: {
            // Central spindle shaft
            float spindleR = 0.06f;
            appendOrientedBox(vertices, indices,
                doorCenter + normal * (0.5f + spindleR),
                right, up, normal,
                spindleR, height * 0.5f, spindleR,
                static_cast<uint32_t>(MaterialSlot::FrameMetal));

            // 4 revolving glass leaves radiating from center
            float leafDepth = 0.75f;
            appendQuad(vertices, indices,
                doorCenter - up * (height * 0.5f) + normal * 0.5f,
                doorCenter - up * (height * 0.5f) + normal * (0.5f + leafDepth),
                doorCenter + up * (height * 0.5f) + normal * (0.5f + leafDepth),
                doorCenter + up * (height * 0.5f) + normal * 0.5f,
                right, static_cast<uint32_t>(MaterialSlot::Glass));
            appendQuad(vertices, indices,
                doorCenter - up * (height * 0.5f) + normal * (0.5f - leafDepth),
                doorCenter - up * (height * 0.5f) + normal * 0.5f,
                doorCenter + up * (height * 0.5f) + normal * 0.5f,
                doorCenter + up * (height * 0.5f) + normal * (0.5f - leafDepth),
                right, static_cast<uint32_t>(MaterialSlot::Glass));
            appendQuad(vertices, indices,
                doorCenter - up * (height * 0.5f) + normal * 0.5f,
                doorCenter - up * (height * 0.5f) + normal * 0.5f + right * leafDepth,
                doorCenter + up * (height * 0.5f) + normal * 0.5f + right * leafDepth,
                doorCenter + up * (height * 0.5f) + normal * 0.5f,
                normal, static_cast<uint32_t>(MaterialSlot::Glass));
            appendQuad(vertices, indices,
                doorCenter - up * (height * 0.5f) + normal * 0.5f - right * leafDepth,
                doorCenter - up * (height * 0.5f) + normal * 0.5f,
                doorCenter + up * (height * 0.5f) + normal * 0.5f,
                doorCenter + up * (height * 0.5f) + normal * 0.5f - right * leafDepth,
                normal, static_cast<uint32_t>(MaterialSlot::Glass));

            // Overhead drum cap
            appendQuad(vertices, indices,
                doorCenter - right * hw + up * (height * 0.5f) - normal * 0.1f,
                doorCenter + right * hw + up * (height * 0.5f) - normal * 0.1f,
                doorCenter + right * hw + up * (height * 0.5f + 0.35f) + normal * (leafDepth + 0.1f),
                doorCenter - right * hw + up * (height * 0.5f + 0.35f) + normal * (leafDepth + 0.1f),
                up, static_cast<uint32_t>(MaterialSlot::FrameMetal));
            break;
        }
        case DoorType::AutomaticSliding: {
            // Top motorized sliding track enclosure box
            Vec3 trackL0 = pTL - right * fw + up * 0.02f + normal * (doorRelief + 0.06f);
            Vec3 trackR0 = pTR + right * fw + up * 0.02f + normal * (doorRelief + 0.06f);
            Vec3 trackR1 = trackR0 + up * 0.22f;
            Vec3 trackL1 = trackL0 + up * 0.22f;
            appendQuad(vertices, indices, trackL0, trackR0, trackR1, trackL1, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));

            // Overlap seam & guide channel
            Vec3 ch0 = doorCenter - right * 0.02f - up * (height * 0.5f) + casingOffset;
            Vec3 ch1 = doorCenter + right * 0.02f - up * (height * 0.5f) + casingOffset;
            Vec3 ch2 = doorCenter + right * 0.02f + up * (height * 0.5f) + casingOffset;
            Vec3 ch3 = doorCenter - right * 0.02f + up * (height * 0.5f) + casingOffset;
            appendQuad(vertices, indices, ch0, ch1, ch2, ch3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));

            // Motion sensor unit in header
            Vec3 sensor0 = doorCenter - right * 0.12f + up * (height * 0.5f + 0.06f) + normal * (doorRelief + 0.09f);
            Vec3 sensor1 = doorCenter + right * 0.12f + up * (height * 0.5f + 0.06f) + normal * (doorRelief + 0.09f);
            Vec3 sensor2 = sensor1 + up * 0.06f;
            Vec3 sensor3 = sensor0 + up * 0.06f;
            appendQuad(vertices, indices, sensor0, sensor1, sensor2, sensor3, normal, static_cast<uint32_t>(MaterialSlot::HvacMetal));
            break;
        }
        case DoorType::FrenchDoublePatio: {
            // Divided French double glass doors with brass handles
            Vec3 mid0 = doorCenter - right * 0.03f - up * (height * 0.5f) + casingOffset;
            Vec3 mid1 = doorCenter + right * 0.03f - up * (height * 0.5f) + casingOffset;
            Vec3 mid2 = doorCenter + right * 0.03f + up * (height * 0.5f) + casingOffset;
            Vec3 mid3 = doorCenter - right * 0.03f + up * (height * 0.5f) + casingOffset;
            appendQuad(vertices, indices, mid0, mid1, mid2, mid3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));

            // 4 horizontal divided lite bars per door
            for (float frac : {-0.3f, -0.05f, 0.2f, 0.38f}) {
                Vec3 b0 = pBL + up * (height * (frac + 0.5f) - 0.015f) + casingOffset;
                Vec3 b1 = pBR + up * (height * (frac + 0.5f) - 0.015f) + casingOffset;
                Vec3 b2 = pBR + up * (height * (frac + 0.5f) + 0.015f) + casingOffset;
                Vec3 b3 = pBL + up * (height * (frac + 0.5f) + 0.015f) + casingOffset;
                appendQuad(vertices, indices, b0, b1, b2, b3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));
            }
            // Dual brass lever handles
            for (float side : {-0.08f, 0.08f}) {
                Vec3 h0 = doorCenter + right * side - up * 0.08f + normal * (doorRelief + 0.08f);
                Vec3 h1 = h0 + right * (side > 0 ? 0.04f : -0.04f);
                Vec3 h2 = h1 + up * 0.16f;
                Vec3 h3 = h0 + up * 0.16f;
                appendQuad(vertices, indices, h0, h1, h2, h3, normal, static_cast<uint32_t>(MaterialSlot::BrassHardware));
            }
            break;
        }
        case DoorType::CanopiedGrandPortal: {
            // Monumental portal with flanking classical pilasters & pediment
            float colW = 0.25f;
            float colD = 0.30f;
            // Left pilaster
            appendOrientedBox(vertices, indices,
                doorCenter - right * (hw + colW * 0.5f + 0.05f) + up * 0.15f + normal * (colD * 0.5f),
                right, up, normal,
                colW * 0.5f, height * 0.5f + 0.15f, colD * 0.5f,
                static_cast<uint32_t>(MaterialSlot::CorniceStone));
            // Right pilaster
            appendOrientedBox(vertices, indices,
                doorCenter + right * (hw + colW * 0.5f + 0.05f) + up * 0.15f + normal * (colD * 0.5f),
                right, up, normal,
                colW * 0.5f, height * 0.5f + 0.15f, colD * 0.5f,
                static_cast<uint32_t>(MaterialSlot::CorniceStone));
            // Pediment entablature header
            appendOrientedBox(vertices, indices,
                doorCenter + up * (height * 0.5f + 0.325f) + normal * ((colD + 0.15f) * 0.5f),
                right, up, normal,
                hw + colW + 0.15f, 0.225f, (colD + 0.15f) * 0.5f,
                static_cast<uint32_t>(MaterialSlot::CorniceStone));
            break;
        }
        case DoorType::SecurityGateGrille: {
            // Heavy-duty retractable security grille with horizontal and diagonal bars
            for (int r = 1; r <= 6; ++r) {
                float yPos = -height * 0.5f + height * (static_cast<float>(r) / 7.0f);
                Vec3 g0 = doorCenter - right * hw + up * (yPos - 0.02f) + casingOffset * 1.1f;
                Vec3 g1 = doorCenter + right * hw + up * (yPos - 0.02f) + casingOffset * 1.1f;
                Vec3 g2 = doorCenter + right * hw + up * (yPos + 0.02f) + casingOffset * 1.1f;
                Vec3 g3 = doorCenter - right * hw + up * (yPos + 0.02f) + casingOffset * 1.1f;
                appendQuad(vertices, indices, g0, g1, g2, g3, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));
            }
            break;
        }
        default: {
            // Single handle for standard single entry doors
            Vec3 h0 = pBL + right * (width * 0.82f) + up * (height * 0.45f) + normal * (doorRelief + 0.06f);
            Vec3 h1 = h0 + right * 0.04f;
            Vec3 h2 = h1 + up * 0.16f;
            Vec3 h3 = h0 + up * 0.16f;
            appendQuad(vertices, indices, h0, h1, h2, h3, normal, static_cast<uint32_t>(MaterialSlot::HvacMetal));
            break;
        }
    }

    // Entrance steps (Top tread and front riser)
    float stepDepth = 0.65f;
    float stepH = 0.18f;
    Vec3 stepFront = normal * stepDepth;
    Vec3 s0 = pBL - right * (fw + 0.2f) + stepFront;
    Vec3 s1 = pBR + right * (fw + 0.2f) + stepFront;
    Vec3 s2 = pBR + right * (fw + 0.2f);
    Vec3 s3 = pBL - right * (fw + 0.2f);
    appendQuad(vertices, indices, s0, s1, s2, s3, up, static_cast<uint32_t>(MaterialSlot::Foundation));
    Vec3 r0 = s0 - up * stepH;
    Vec3 r1 = s1 - up * stepH;
    appendQuad(vertices, indices, r0, r1, s1, s0, normal, static_cast<uint32_t>(MaterialSlot::Foundation));

    // Entrance canopy
    if (withCanopy) {
        float canopyDepth = 1.6f;
        float canopyWidth = width + 1.2f;
        float canopyThick = 0.18f;
        Vec3 canopyBase = doorCenter + up * (height * 0.5f + fw + 0.05f);
        Vec3 cBL = canopyBase - right * (canopyWidth * 0.5f);
        Vec3 cBR = canopyBase + right * (canopyWidth * 0.5f);
        Vec3 cTR = cBR + normal * canopyDepth;
        Vec3 cTL = cBL + normal * canopyDepth;

        appendQuad(vertices, indices, cBL + up * canopyThick, cBR + up * canopyThick, cTR + up * canopyThick, cTL + up * canopyThick, up, static_cast<uint32_t>(MaterialSlot::FrameMetal));
        appendQuad(vertices, indices, cTL, cTR, cBR, cBL, -up, static_cast<uint32_t>(MaterialSlot::Foundation));
        appendQuad(vertices, indices, cTL, cTR, cTR + up * canopyThick, cTL + up * canopyThick, normal, static_cast<uint32_t>(MaterialSlot::FrameMetal));

        // Recessed 3D warm soffit bulb centered under entrance canopy
        Vec3 soffitCenter = canopyBase + normal * (canopyDepth * 0.52f);
        appendOrientedCylinder(vertices, indices,
            soffitCenter, soffitCenter - up * 0.05f,
            0.14f, 0.12f, 10,
            static_cast<uint32_t>(MaterialSlot::LightFixtureMetal), true, true);
        appendOrientedCylinder(vertices, indices,
            soffitCenter - up * 0.05f, soffitCenter - up * 0.13f,
            0.09f, 0.05f, 10,
            static_cast<uint32_t>(MaterialSlot::LightBulbWarm), true, true);
    }
}

namespace {

constexpr float kRoofEps = 1e-4f;

inline float cross2D(const Vec2& a, const Vec2& b) {
    return a.x * b.y - a.y * b.x;
}

inline float signedArea2D(const std::vector<Vec2>& poly) {
    float a2 = 0.0f;
    size_t n = poly.size();
    for (size_t i = 0; i < n; ++i) {
        size_t j = (i + 1) % n;
        a2 += cross2D(poly[i], poly[j]);
    }
    return a2;
}

inline std::vector<Vec2> normalizeCanonicalContour(const std::vector<Vec2>& raw) {
    std::vector<Vec2> poly;
    poly.reserve(raw.size());
    for (const auto& p : raw) {
        if (poly.empty()) {
            poly.push_back(p);
        } else {
            float dx = p.x - poly.back().x;
            float dz = p.y - poly.back().y;
            if (dx * dx + dz * dz > 1e-6f) {
                poly.push_back(p);
            }
        }
    }
    if (poly.size() >= 2) {
        float dx = poly.front().x - poly.back().x;
        float dz = poly.front().y - poly.back().y;
        if (dx * dx + dz * dz <= 1e-6f) {
            poly.pop_back();
        }
    }
    if (poly.size() < 3) return poly;
    if (signedArea2D(poly) > 0.0f) {
        std::reverse(poly.begin(), poly.end());
    }
    return poly;
}

struct ContourMetrics {
    Vec2 centroid{0.0f, 0.0f};
    float minX = 0.0f, maxX = 0.0f;
    float minZ = 0.0f, maxZ = 0.0f;
    float minInradius = 1.0f;
    bool isAxisAlignedRect = false;
};

inline bool pointInPolygon2D(const Vec2& p, const std::vector<Vec2>& poly) {
    bool inside = false;
    size_t n = poly.size();
    for (size_t i = 0, j = n - 1; i < n; j = i++) {
        float xi = poly[i].x, zi = poly[i].y;
        float xj = poly[j].x, zj = poly[j].y;
        bool intersect = ((zi > p.y) != (zj > p.y)) &&
            (p.x < (xj - xi) * (p.y - zi) / ((zj - zi) + (zj == zi ? 1e-9f : 0.0f)) + xi);
        if (intersect) inside = !inside;
    }
    return inside;
}

inline ContourMetrics computeContourMetrics(const std::vector<Vec2>& poly) {
    ContourMetrics m;
    if (poly.empty()) return m;
    m.minX = m.maxX = poly[0].x;
    m.minZ = m.maxZ = poly[0].y;
    Vec2 avg{0.0f, 0.0f};
    for (const auto& p : poly) {
        m.minX = std::min(m.minX, p.x);
        m.maxX = std::max(m.maxX, p.x);
        m.minZ = std::min(m.minZ, p.y);
        m.maxZ = std::max(m.maxZ, p.y);
        avg.x += p.x;
        avg.y += p.y;
    }
    avg.x /= static_cast<float>(poly.size());
    avg.y /= static_cast<float>(poly.size());

    float a2 = signedArea2D(poly);
    if (std::abs(a2) > 1e-4f) {
        float cx = 0.0f, cz = 0.0f;
        size_t n = poly.size();
        for (size_t i = 0; i < n; ++i) {
            size_t j = (i + 1) % n;
            float c = cross2D(poly[i], poly[j]);
            cx += (poly[i].x + poly[j].x) * c;
            cz += (poly[i].y + poly[j].y) * c;
        }
        m.centroid = Vec2{cx / (3.0f * a2), cz / (3.0f * a2)};
        if (!pointInPolygon2D(m.centroid, poly)) {
            m.centroid = avg;
        }
    } else {
        m.centroid = avg;
    }

    float minDist = 1e9f;
    size_t n = poly.size();
    for (size_t i = 0; i < n; ++i) {
        size_t j = (i + 1) % n;
        Vec2 a = poly[i], b = poly[j];
        float ex = b.x - a.x, ez = b.y - a.y;
        float lenSq = ex * ex + ez * ez;
        float t = (lenSq > 1e-6f)
            ? std::clamp(((m.centroid.x - a.x) * ex + (m.centroid.y - a.y) * ez) / lenSq, 0.0f, 1.0f)
            : 0.0f;
        float px = a.x + ex * t - m.centroid.x;
        float pz = a.y + ez * t - m.centroid.y;
        minDist = std::min(minDist, std::sqrt(px * px + pz * pz));
    }
    m.minInradius = std::max(0.25f, minDist);

    if (n == 4) {
        bool axisAligned = true;
        for (size_t i = 0; i < 4; ++i) {
            size_t j = (i + 1) % 4;
            float dx = std::abs(poly[j].x - poly[i].x);
            float dz = std::abs(poly[j].y - poly[i].y);
            if (!(dx < 0.02f || dz < 0.02f)) {
                axisAligned = false;
                break;
            }
        }
        m.isAxisAlignedRect = axisAligned;
    }
    return m;
}

inline std::vector<Vec2> offsetContourMiter(
    const std::vector<Vec2>& poly,
    const Vec2& centroid,
    float delta
) {
    size_t n = poly.size();
    std::vector<Vec2> out(n);
    if (n < 3 || std::abs(delta) < 1e-5f) return poly;

    for (size_t i = 0; i < n; ++i) {
        size_t prev = (i + n - 1) % n;
        size_t next = (i + 1) % n;
        Vec2 e0{poly[i].x - poly[prev].x, poly[i].y - poly[prev].y};
        Vec2 e1{poly[next].x - poly[i].x, poly[next].y - poly[i].y};
        float l0 = std::sqrt(e0.x * e0.x + e0.y * e0.y);
        float l1 = std::sqrt(e1.x * e1.x + e1.y * e1.y);
        // Edge outward normal = (-dz, dx) for canonical A2 < 0 contour
        Vec2 n0 = (l0 > 1e-5f) ? Vec2{-e0.y / l0, e0.x / l0} : Vec2{0.0f, 1.0f};
        Vec2 n1 = (l1 > 1e-5f) ? Vec2{-e1.y / l1, e1.x / l1} : Vec2{0.0f, 1.0f};
        Vec2 bis{n0.x + n1.x, n0.y + n1.y};
        float bl = std::sqrt(bis.x * bis.x + bis.y * bis.y);
        if (bl > 1e-4f) {
            bis.x /= bl;
            bis.y /= bl;
            float denom = std::max(0.45f, bis.x * n0.x + bis.y * n0.y);
            float scale = std::min(1.65f, 1.0f / denom);
            out[i] = Vec2{poly[i].x + bis.x * (delta * scale), poly[i].y + bis.y * (delta * scale)};
        } else {
            Vec2 radial{poly[i].x - centroid.x, poly[i].y - centroid.y};
            float rl = std::sqrt(radial.x * radial.x + radial.y * radial.y);
            if (rl > 1e-5f) {
                out[i] = Vec2{poly[i].x + (radial.x / rl) * delta, poly[i].y + (radial.y / rl) * delta};
            } else {
                out[i] = poly[i];
            }
        }
    }
    return out;
}

inline std::vector<Vec2> insetContourTowardCentroid(
    const std::vector<Vec2>& poly,
    const Vec2& centroid,
    float insetDist,
    float minInradius
) {
    if (insetDist <= 1e-4f) return poly;
    float frac = std::clamp(insetDist / std::max(0.35f, minInradius), 0.02f, 0.72f);
    // Use mitered inset when mostly smooth/convex, blend softly to centroid-scale to guarantee zero self-intersection
    std::vector<Vec2> miter = offsetContourMiter(poly, centroid, -insetDist);
    std::vector<Vec2> out(poly.size());
    for (size_t i = 0; i < poly.size(); ++i) {
        size_t prev = (i + poly.size() - 1) % poly.size();
        size_t next = (i + 1) % poly.size();
        Vec2 d0{poly[i].x - poly[prev].x, poly[i].y - poly[prev].y};
        Vec2 d1{poly[next].x - poly[i].x, poly[next].y - poly[i].y};
        float l0 = std::sqrt(d0.x * d0.x + d0.y * d0.y);
        float l1 = std::sqrt(d1.x * d1.x + d1.y * d1.y);
        float cosAng = (l0 > 1e-4f && l1 > 1e-4f)
            ? ((d0.x * d1.x + d0.y * d1.y) / (l0 * l1))
            : 1.0f;
        Vec2 scaled{
            centroid.x + (poly[i].x - centroid.x) * (1.0f - frac),
            centroid.y + (poly[i].y - centroid.y) * (1.0f - frac)
        };
        float edgeLen = std::min(l0, l1);
        float w = (cosAng > 0.1f && edgeLen > insetDist * 2.4f) ? 1.0f : 0.0f;
        out[i] = Vec2{
            scaled.x * (1.0f - w) + miter[i].x * w,
            scaled.y * (1.0f - w) + miter[i].y * w
        };
    }
    return out;
}

inline std::vector<Vec2> scaleContour(const std::vector<Vec2>& poly, const Vec2& c, float scale) {
    std::vector<Vec2> out;
    out.reserve(poly.size());
    for (const auto& p : poly) {
        out.push_back(Vec2{c.x + (p.x - c.x) * scale, c.y + (p.y - c.y) * scale});
    }
    return out;
}

struct Tri2D {
    Vec2 a, b, c;
};

inline bool pointInTri2D(const Vec2& p, const Vec2& a, const Vec2& b, const Vec2& c) {
    float c1 = cross2D(Vec2{b.x - a.x, b.y - a.y}, Vec2{p.x - a.x, p.y - a.y});
    float c2 = cross2D(Vec2{c.x - b.x, c.y - b.y}, Vec2{p.x - b.x, p.y - b.y});
    float c3 = cross2D(Vec2{a.x - c.x, a.y - c.y}, Vec2{p.x - c.x, p.y - c.y});
    // In canonical A2 < 0 winding, interior is <= 0 on all 3 edges
    return (c1 <= 1e-5f && c2 <= 1e-5f && c3 <= 1e-5f);
}

inline std::vector<Tri2D> triangulatePolygonXZ(const std::vector<Vec2>& rawPoly) {
    std::vector<Tri2D> tris;
    std::vector<Vec2> poly = normalizeCanonicalContour(rawPoly);
    if (poly.size() < 3) return tris;
    if (poly.size() == 3) {
        tris.push_back({poly[0], poly[1], poly[2]});
        return tris;
    }

    std::vector<int> idx(poly.size());
    for (size_t i = 0; i < poly.size(); ++i) idx[i] = static_cast<int>(i);

    int guard = static_cast<int>(poly.size() * poly.size()) + 16;
    while (idx.size() > 3 && guard-- > 0) {
        bool earFound = false;
        size_t m = idx.size();
        for (size_t k = 0; k < m; ++k) {
            int ia = idx[(k + m - 1) % m];
            int ib = idx[k];
            int ic = idx[(k + 1) % m];
            const Vec2& a = poly[ia];
            const Vec2& b = poly[ib];
            const Vec2& c = poly[ic];
            float cr = cross2D(Vec2{b.x - a.x, b.y - a.y}, Vec2{c.x - a.x, c.y - a.y});
            if (cr >= -1e-6f) continue; // reflex or collinear

            bool containsOther = false;
            for (size_t j = 0; j < m; ++j) {
                int ij = idx[j];
                if (ij == ia || ij == ib || ij == ic) continue;
                if (pointInTri2D(poly[ij], a, b, c)) {
                    containsOther = true;
                    break;
                }
            }
            if (!containsOther) {
                tris.push_back({a, b, c});
                idx.erase(idx.begin() + static_cast<std::ptrdiff_t>(k));
                earFound = true;
                break;
            }
        }
        if (!earFound) break;
    }
    if (idx.size() == 3) {
        tris.push_back({poly[idx[0]], poly[idx[1]], poly[idx[2]]});
    } else if (idx.size() > 3) {
        for (size_t k = 1; k + 1 < idx.size(); ++k) {
            tris.push_back({poly[idx[0]], poly[idx[k]], poly[idx[k + 1]]});
        }
    }
    return tris;
}

// Clip a convex polygon against half-plane nx * x + nz * z + d >= 0
inline std::vector<Vec2> clipConvexHalfPlane(
    const std::vector<Vec2>& poly,
    float nx, float nz, float d
) {
    std::vector<Vec2> out;
    size_t n = poly.size();
    if (n == 0) return out;
    auto eval = [&](const Vec2& p) { return nx * p.x + nz * p.y + d; };
    for (size_t i = 0; i < n; ++i) {
        const Vec2& a = poly[i];
        const Vec2& b = poly[(i + 1) % n];
        float da = eval(a);
        float db = eval(b);
        bool inA = (da >= -1e-5f);
        bool inB = (db >= -1e-5f);
        if (inA && inB) {
            out.push_back(b);
        } else if (inA && !inB) {
            float t = da / (da - db);
            out.push_back(Vec2{a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t});
        } else if (!inA && inB) {
            float t = da / (da - db);
            out.push_back(Vec2{a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t});
            out.push_back(b);
        }
    }
    return out;
}

inline void appendTri3D(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& p0, const Vec3& p1, const Vec3& p2,
    const Vec3& normal,
    uint32_t materialId
) {
    uint32_t baseIdx = static_cast<uint32_t>(vertices.size());
    vertices.push_back({p0, normal, {0.0f, 0.0f}, materialId});
    vertices.push_back({p1, normal, {1.0f, 0.0f}, materialId});
    vertices.push_back({p2, normal, {0.5f, 1.0f}, materialId});
    indices.push_back(baseIdx);
    indices.push_back(baseIdx + 1);
    indices.push_back(baseIdx + 2);
}

inline void appendUpwardTri3D(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& p0, const Vec3& p1, const Vec3& p2,
    uint32_t materialId,
    bool emitSoffit = false,
    uint32_t soffitMat = 0,
    float soffitThick = 0.14f
) {
    Vec3 rawN = (p1 - p0).cross(p2 - p0);
    if (rawN.length() < 1e-6f) return;
    Vec3 n = rawN.normalized();
    if (n.y < 0.0f) {
        n = -n;
        appendTri3D(vertices, indices, p0, p2, p1, n, materialId);
        if (emitSoffit) {
            Vec3 off{0.0f, -soffitThick, 0.0f};
            appendTri3D(vertices, indices, p0 + off, p1 + off, p2 + off, -n, soffitMat);
        }
    } else {
        appendTri3D(vertices, indices, p0, p1, p2, n, materialId);
        if (emitSoffit) {
            Vec3 off{0.0f, -soffitThick, 0.0f};
            appendTri3D(vertices, indices, p0 + off, p2 + off, p1 + off, -n, soffitMat);
        }
    }
}

inline void emitClippedZBandHeightField(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const std::vector<Tri2D>& tris,
    float zLow, float zHigh,
    const std::function<float(float, float)>& heightFn,
    uint32_t matId,
    bool emitSoffit,
    uint32_t soffitMat
) {
    for (const auto& t : tris) {
        std::vector<Vec2> poly = {t.a, t.b, t.c};
        poly = clipConvexHalfPlane(poly, 0.0f, 1.0f, -zLow);   // z >= zLow
        if (poly.size() < 3) continue;
        poly = clipConvexHalfPlane(poly, 0.0f, -1.0f, zHigh);  // z <= zHigh
        if (poly.size() < 3) continue;
        for (size_t k = 1; k + 1 < poly.size(); ++k) {
            const Vec2& a = poly[0];
            const Vec2& b = poly[k];
            const Vec2& c = poly[k + 1];
            appendUpwardTri3D(
                vertices, indices,
                Vec3{a.x, heightFn(a.x, a.y), a.y},
                Vec3{b.x, heightFn(b.x, b.y), b.y},
                Vec3{c.x, heightFn(c.x, c.y), c.y},
                matId, emitSoffit, soffitMat
            );
        }
    }
}

inline void emitClippedXBandHeightField(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const std::vector<Tri2D>& tris,
    float xLow, float xHigh,
    const std::function<float(float, float)>& heightFn,
    uint32_t matId,
    bool emitSoffit,
    uint32_t soffitMat
) {
    for (const auto& t : tris) {
        std::vector<Vec2> poly = {t.a, t.b, t.c};
        poly = clipConvexHalfPlane(poly, 1.0f, 0.0f, -xLow);   // x >= xLow
        if (poly.size() < 3) continue;
        poly = clipConvexHalfPlane(poly, -1.0f, 0.0f, xHigh);  // x <= xHigh
        if (poly.size() < 3) continue;
        for (size_t k = 1; k + 1 < poly.size(); ++k) {
            const Vec2& a = poly[0];
            const Vec2& b = poly[k];
            const Vec2& c = poly[k + 1];
            appendUpwardTri3D(
                vertices, indices,
                Vec3{a.x, heightFn(a.x, a.y), a.y},
                Vec3{b.x, heightFn(b.x, b.y), b.y},
                Vec3{c.x, heightFn(c.x, c.y), c.y},
                matId, emitSoffit, soffitMat
            );
        }
    }
}

// Seals vertical gable/tympanum walls around basePoly from baseTopY up to roofHeightAt(x, z),
// splitting any edge that crosses Z/X breakpoints so curved/angled walls conform cleanly.
inline void emitPerimeterClosureWallZBreaks(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const std::vector<Vec2>& basePoly,
    float baseTopY,
    const std::vector<float>& zBreaks,
    const std::function<float(float, float)>& heightFn,
    uint32_t wallMat
) {
    size_t n = basePoly.size();
    for (size_t i = 0; i < n; ++i) {
        const Vec2& a = basePoly[i];
        const Vec2& b = basePoly[(i + 1) % n];
        std::vector<float> ts = {0.0f, 1.0f};
        float dz = b.y - a.y;
        if (std::abs(dz) > 1e-4f) {
            for (float zb : zBreaks) {
                float t = (zb - a.y) / dz;
                if (t > 1e-3f && t < 1.0f - 1e-3f) ts.push_back(t);
            }
        }
        std::sort(ts.begin(), ts.end());
        Vec3 norm = Vec3{-(b.y - a.y), 0.0f, (b.x - a.x)}.normalized();
        for (size_t k = 0; k + 1 < ts.size(); ++k) {
            float t0 = ts[k], t1 = ts[k + 1];
            if (t1 - t0 < 1e-4f) continue;
            Vec2 p0{a.x + (b.x - a.x) * t0, a.y + (b.y - a.y) * t0};
            Vec2 p1{a.x + (b.x - a.x) * t1, a.y + (b.y - a.y) * t1};
            float y0 = std::max(baseTopY, heightFn(p0.x, p0.y));
            float y1 = std::max(baseTopY, heightFn(p1.x, p1.y));
            if (y0 <= baseTopY + 0.01f && y1 <= baseTopY + 0.01f) continue;
            BuildingComponents::appendQuad(
                vertices, indices,
                Vec3{p0.x, baseTopY, p0.y},
                Vec3{p1.x, baseTopY, p1.y},
                Vec3{p1.x, y1, p1.y},
                Vec3{p0.x, y0, p0.y},
                norm, wallMat
            );
        }
    }
}

inline void emitPerimeterClosureWallXBreaks(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const std::vector<Vec2>& basePoly,
    float baseTopY,
    const std::vector<float>& xBreaks,
    const std::function<float(float, float)>& heightFn,
    uint32_t wallMat
) {
    size_t n = basePoly.size();
    for (size_t i = 0; i < n; ++i) {
        const Vec2& a = basePoly[i];
        const Vec2& b = basePoly[(i + 1) % n];
        std::vector<float> ts = {0.0f, 1.0f};
        float dx = b.x - a.x;
        if (std::abs(dx) > 1e-4f) {
            for (float xb : xBreaks) {
                float t = (xb - a.x) / dx;
                if (t > 1e-3f && t < 1.0f - 1e-3f) ts.push_back(t);
            }
        }
        std::sort(ts.begin(), ts.end());
        Vec3 norm = Vec3{-(b.y - a.y), 0.0f, (b.x - a.x)}.normalized();
        for (size_t k = 0; k + 1 < ts.size(); ++k) {
            float t0 = ts[k], t1 = ts[k + 1];
            if (t1 - t0 < 1e-4f) continue;
            Vec2 p0{a.x + (b.x - a.x) * t0, a.y + (b.y - a.y) * t0};
            Vec2 p1{a.x + (b.x - a.x) * t1, a.y + (b.y - a.y) * t1};
            float y0 = std::max(baseTopY, heightFn(p0.x, p0.y));
            float y1 = std::max(baseTopY, heightFn(p1.x, p1.y));
            if (y0 <= baseTopY + 0.01f && y1 <= baseTopY + 0.01f) continue;
            BuildingComponents::appendQuad(
                vertices, indices,
                Vec3{p0.x, baseTopY, p0.y},
                Vec3{p1.x, baseTopY, p1.y},
                Vec3{p1.x, y1, p1.y},
                Vec3{p0.x, y0, p0.y},
                norm, wallMat
            );
        }
    }
}

inline bool chordXAtZ(const std::vector<Vec2>& poly, float z, float& outMinX, float& outMaxX) {
    bool found = false;
    float mn = 1e9f, mx = -1e9f;
    size_t n = poly.size();
    for (size_t i = 0; i < n; ++i) {
        const Vec2& a = poly[i];
        const Vec2& b = poly[(i + 1) % n];
        if ((a.y <= z && b.y >= z) || (b.y <= z && a.y >= z)) {
            float dz = b.y - a.y;
            if (std::abs(dz) < 1e-5f) {
                mn = std::min({mn, a.x, b.x});
                mx = std::max({mx, a.x, b.x});
                found = true;
            } else {
                float t = (z - a.y) / dz;
                float x = a.x + t * (b.x - a.x);
                mn = std::min(mn, x);
                mx = std::max(mx, x);
                found = true;
            }
        }
    }
    if (found && mx > mn + 0.05f) {
        outMinX = mn;
        outMaxX = mx;
        return true;
    }
    return false;
}

inline bool chordZAtX(const std::vector<Vec2>& poly, float x, float& outMinZ, float& outMaxZ) {
    bool found = false;
    float mn = 1e9f, mx = -1e9f;
    size_t n = poly.size();
    for (size_t i = 0; i < n; ++i) {
        const Vec2& a = poly[i];
        const Vec2& b = poly[(i + 1) % n];
        if ((a.x <= x && b.x >= x) || (b.x <= x && a.x >= x)) {
            float dx = b.x - a.x;
            if (std::abs(dx) < 1e-5f) {
                mn = std::min({mn, a.y, b.y});
                mx = std::max({mx, a.y, b.y});
                found = true;
            } else {
                float t = (x - a.x) / dx;
                float z = a.y + t * (b.y - a.y);
                mn = std::min(mn, z);
                mx = std::max(mx, z);
                found = true;
            }
        }
    }
    if (found && mx > mn + 0.05f) {
        outMinZ = mn;
        outMaxZ = mx;
        return true;
    }
    return false;
}

inline std::vector<Vec2> buildContourForVolumeShape(
    BuildingVolumeShape shape,
    float baseWidth,
    float baseDepth,
    float baseRadius
) {
    float hw = std::max(0.5f, baseWidth * 0.5f);
    float hd = std::max(0.5f, baseDepth * 0.5f);
    std::vector<Vec2> pts;

    switch (shape) {
        case BuildingVolumeShape::Oval: {
            const int segs = 48;
            pts.reserve(segs);
            for (int i = segs - 1; i >= 0; --i) {
                float a = (static_cast<float>(i) / segs) * (kPi * 2.0f);
                pts.push_back(Vec2{hw * std::cos(a), hd * std::sin(a)});
            }
            break;
        }
        case BuildingVolumeShape::SemiOval: {
            const int segs = 32;
            pts.reserve(segs + 1);
            for (int i = segs; i >= 0; --i) {
                float a = (static_cast<float>(i) / segs) * kPi; // PI .. 0 -> (-hw, 0) via (0, hd) to (hw, 0)
                pts.push_back(Vec2{hw * std::cos(a), hd * std::sin(a)});
            }
            break;
        }
        case BuildingVolumeShape::Cylinder: {
            float r = (baseRadius > 0.1f) ? baseRadius : hw;
            const int segs = 48;
            pts.reserve(segs);
            for (int i = segs - 1; i >= 0; --i) {
                float a = (static_cast<float>(i) / segs) * (kPi * 2.0f);
                pts.push_back(Vec2{r * std::cos(a), r * std::sin(a)});
            }
            break;
        }
        case BuildingVolumeShape::TriangleSemiCircleHybrid: {
            // Exact match to ProceduralBuildingGenerator::generateTriangleSemiCircleMesh
            pts.push_back(Vec2{0.0f, hd});
            pts.push_back(Vec2{hw, 0.0f});
            const int arcSegs = 28;
            for (int i = 1; i < arcSegs; ++i) {
                float th = -static_cast<float>(i) / arcSegs * kPi;
                pts.push_back(Vec2{hw * std::cos(th), hd * std::sin(th)});
            }
            pts.push_back(Vec2{-hw, 0.0f});
            break;
        }
        case BuildingVolumeShape::TriangularFlatiron: {
            pts = {{0.0f, hd}, {hw, -hd}, {-hw, -hd}};
            break;
        }
        case BuildingVolumeShape::LShaped: {
            pts = {
                {-hw, -hd}, {-hw, hd}, {0.0f, hd},
                {0.0f, 0.0f}, {hw, 0.0f}, {hw, -hd}
            };
            break;
        }
        case BuildingVolumeShape::Octagon:
        case BuildingVolumeShape::Hexagon: {
            int sides = (shape == BuildingVolumeShape::Octagon) ? 8 : 6;
            float r = (baseRadius > 0.1f) ? baseRadius : std::min(hw, hd);
            pts.reserve(sides);
            for (int i = sides - 1; i >= 0; --i) {
                float a = (static_cast<float>(i) / sides) * (2.0f * kPi);
                pts.push_back(Vec2{r * std::cos(a), r * std::sin(a)});
            }
            break;
        }
        case BuildingVolumeShape::StarTriPointY: {
            float rOut = (baseRadius > 0.1f) ? baseRadius : hw;
            float rIn = rOut * 0.38f;
            for (int i = 5; i >= 0; --i) {
                float ang = (i * 60.0f + 90.0f) * (kPi / 180.0f);
                float r = (i % 2 == 0) ? rOut : rIn;
                pts.push_back(Vec2{std::cos(ang) * r, std::sin(ang) * r});
            }
            break;
        }
        case BuildingVolumeShape::StarFourPointCross: {
            float rOut = (baseRadius > 0.1f) ? baseRadius : hw;
            float rIn = rOut * 0.44f;
            for (int i = 7; i >= 0; --i) {
                float ang = (i * 45.0f) * (kPi / 180.0f);
                float r = (i % 2 == 0) ? rOut : rIn;
                pts.push_back(Vec2{std::cos(ang) * r, std::sin(ang) * r});
            }
            break;
        }
        case BuildingVolumeShape::StarFivePointApex: {
            float rOut = (baseRadius > 0.1f) ? baseRadius : hw;
            float rIn = rOut * 0.42f;
            for (int i = 9; i >= 0; --i) {
                float ang = (i * 36.0f + 90.0f) * (kPi / 180.0f);
                float r = (i % 2 == 0) ? rOut : rIn;
                pts.push_back(Vec2{std::cos(ang) * r, std::sin(ang) * r});
            }
            break;
        }
        default: {
            pts = {{-hw, hd}, {hw, hd}, {hw, -hd}, {-hw, -hd}};
            break;
        }
    }
    return normalizeCanonicalContour(pts);
}

} // namespace

void BuildingComponents::generateRoof(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    BuildingVolumeShape shape,
    float baseWidth,
    float baseDepth,
    float baseRadius,
    float baseTopY,
    RoofType type,
    float roofHeight,
    float overhang,
    bool showClutter
) {
    if (type == RoofType::NoneFlat) return;

    if (shape == BuildingVolumeShape::TorusRingPavilion) {
        float rOut = (baseRadius > 0.1f) ? baseRadius : (baseWidth * 0.5f);
        float rIn = rOut * 0.52f;
        generateRingRoof(vertices, indices, rOut, rIn, 32, baseTopY, type, roofHeight, overhang, showClutter);
        return;
    }

    std::vector<Vec2> contour = buildContourForVolumeShape(shape, baseWidth, baseDepth, baseRadius);
    generateRoofForContour(vertices, indices, contour, baseTopY, type, roofHeight, overhang, showClutter);
}

void BuildingComponents::generateRoofForContour(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const std::vector<Vec2>& rawContourXZ,
    float baseTopY,
    RoofType type,
    float roofHeight,
    float overhang,
    bool showClutter,
    uint32_t wallMaterialId
) {
    if (type == RoofType::NoneFlat) return;
    std::vector<Vec2> basePoly = normalizeCanonicalContour(rawContourXZ);
    if (basePoly.size() < 3) return;

    ContourMetrics m = computeContourMetrics(basePoly);
    float cx = m.centroid.x;
    float cz = m.centroid.y;
    float halfW = std::max(0.4f, (m.maxX - m.minX) * 0.5f);
    float halfD = std::max(0.4f, (m.maxZ - m.minZ) * 0.5f);

    // Parapet & flat-terrace roofs always align flush with facade perimeter (never overhang mid-air!)
    float effOverhang = (type == RoofType::FlatParapet || type == RoofType::GreenRoofTerrace)
        ? 0.0f
        : std::clamp(overhang, 0.0f, std::max(0.15f, m.minInradius * 0.28f));

    std::vector<Vec2> overPoly = (effOverhang > 1e-4f)
        ? offsetContourMiter(basePoly, m.centroid, effOverhang)
        : basePoly;
    overPoly = normalizeCanonicalContour(overPoly);
    ContourMetrics mo = computeContourMetrics(overPoly);

    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t wallMat = wallMaterialId;
    uint32_t metalMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t brassMat = static_cast<uint32_t>(MaterialSlot::BrassHardware);

    switch (type) {
        case RoofType::FlatParapet: {
            float parapetHeight = 0.85f;
            float topY = baseTopY + parapetHeight;
            float deckY = baseTopY + 0.10f;
            float wallThick = std::min(0.28f, m.minInradius * 0.26f);
            std::vector<Vec2> inPoly = insetContourTowardCentroid(basePoly, m.centroid, wallThick, m.minInradius);

            size_t n = basePoly.size();
            for (size_t i = 0; i < n; ++i) {
                size_t next = (i + 1) % n;
                const Vec2& o0 = basePoly[i];
                const Vec2& o1 = basePoly[next];
                const Vec2& i0 = inPoly[i];
                const Vec2& i1 = inPoly[next];
                Vec3 outNorm = Vec3{-(o1.y - o0.y), 0.0f, (o1.x - o0.x)}.normalized();
                // Outer parapet wall flush with building facade
                appendQuad(vertices, indices,
                    Vec3{o0.x, baseTopY, o0.y}, Vec3{o1.x, baseTopY, o1.y},
                    Vec3{o1.x, topY, o1.y}, Vec3{o0.x, topY, o0.y},
                    outNorm, wallMat);
                // Coping top rim
                appendQuad(vertices, indices,
                    Vec3{o0.x, topY, o0.y}, Vec3{o1.x, topY, o1.y},
                    Vec3{i1.x, topY, i1.y}, Vec3{i0.x, topY, i0.y},
                    Vec3{0.0f, 1.0f, 0.0f}, roofMat);
                // Inner parapet drop face
                appendQuad(vertices, indices,
                    Vec3{i1.x, deckY, i1.y}, Vec3{i0.x, deckY, i0.y},
                    Vec3{i0.x, topY, i0.y}, Vec3{i1.x, topY, i1.y},
                    -outNorm, roofMat);
            }

            // Conforming recessed roof deck
            std::vector<Tri2D> deckTris = triangulatePolygonXZ(inPoly);
            for (const auto& t : deckTris) {
                appendTri3D(vertices, indices,
                    Vec3{t.a.x, deckY, t.a.y}, Vec3{t.b.x, deckY, t.b.y}, Vec3{t.c.x, deckY, t.c.y},
                    Vec3{0.0f, 1.0f, 0.0f}, roofMat);
            }

            float safeR = m.minInradius - wallThick;
            if (showClutter && safeR > 1.25f) {
                uint32_t hvacMat = static_cast<uint32_t>(MaterialSlot::HvacMetal);
                float unitW = std::min(2.2f, safeR * 0.55f);
                float unitD = std::min(1.6f, safeR * 0.45f);
                float unitH = std::min(1.9f, 0.65f + safeR * 0.14f);
                appendBox(vertices, indices,
                    {cx - unitW * 0.5f, deckY + 0.02f, cz - unitD * 0.5f},
                    {cx + unitW * 0.5f, deckY + unitH, cz + unitD * 0.5f},
                    hvacMat);
                if (safeR > 3.2f) {
                    float auxX = cx + safeR * 0.36f;
                    float auxZ = cz + safeR * 0.18f;
                    float auxW = std::min(1.5f, safeR * 0.24f);
                    float auxD = std::min(1.3f, safeR * 0.22f);
                    if (pointInPolygon2D(Vec2{auxX, auxZ}, inPoly)) {
                        appendBox(vertices, indices,
                            {auxX - auxW * 0.5f, deckY + 0.02f, auxZ - auxD * 0.5f},
                            {auxX + auxW * 0.5f, deckY + 0.95f, auxZ + auxD * 0.5f},
                            hvacMat);
                    }
                }
            }
            break;
        }

        case RoofType::GreenRoofTerrace: {
            float topY = baseTopY + 0.90f;
            float deckY = baseTopY + 0.15f;
            float wallThick = std::min(0.30f, m.minInradius * 0.26f);
            std::vector<Vec2> inPoly = insetContourTowardCentroid(basePoly, m.centroid, wallThick, m.minInradius);

            size_t n = basePoly.size();
            for (size_t i = 0; i < n; ++i) {
                size_t next = (i + 1) % n;
                const Vec2& o0 = basePoly[i];
                const Vec2& o1 = basePoly[next];
                const Vec2& i0 = inPoly[i];
                const Vec2& i1 = inPoly[next];
                Vec3 outNorm = Vec3{-(o1.y - o0.y), 0.0f, (o1.x - o0.x)}.normalized();
                appendQuad(vertices, indices,
                    Vec3{o0.x, baseTopY, o0.y}, Vec3{o1.x, baseTopY, o1.y},
                    Vec3{o1.x, topY, o1.y}, Vec3{o0.x, topY, o0.y},
                    outNorm, wallMat);
                appendQuad(vertices, indices,
                    Vec3{o0.x, topY, o0.y}, Vec3{o1.x, topY, o1.y},
                    Vec3{i1.x, topY, i1.y}, Vec3{i0.x, topY, i0.y},
                    Vec3{0.0f, 1.0f, 0.0f}, wallMat);
                appendQuad(vertices, indices,
                    Vec3{i1.x, deckY, i1.y}, Vec3{i0.x, deckY, i0.y},
                    Vec3{i0.x, topY, i0.y}, Vec3{i1.x, topY, i1.y},
                    -outNorm, wallMat);
            }

            std::vector<Tri2D> turfTris = triangulatePolygonXZ(inPoly);
            uint32_t turfMat = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
            for (const auto& t : turfTris) {
                appendTri3D(vertices, indices,
                    Vec3{t.a.x, deckY, t.a.y}, Vec3{t.b.x, deckY, t.b.y}, Vec3{t.c.x, deckY, t.c.y},
                    Vec3{0.0f, 1.0f, 0.0f}, turfMat);
            }

            // Inscribed planter boxes & pergola fitted strictly inside interior contour
            float safeR = m.minInradius - wallThick;
            if (safeR > 1.4f) {
                float boxHW = std::min(safeR * 0.56f, 4.2f);
                float boxHD = std::min(safeR * 0.56f, 4.2f);
                uint32_t planMat = static_cast<uint32_t>(MaterialSlot::PlanterBox);
                appendBox(vertices, indices,
                    {cx - boxHW, deckY, cz + boxHD - 0.55f},
                    {cx + boxHW, deckY + 0.50f, cz + boxHD}, planMat);
                appendBox(vertices, indices,
                    {cx - boxHW, deckY, cz - boxHD},
                    {cx + boxHW, deckY + 0.50f, cz - boxHD + 0.55f}, planMat);

                float pergH = 2.35f;
                float pergW = boxHW * 0.85f;
                float pergD = boxHD * 0.72f;
                uint32_t pergMat = static_cast<uint32_t>(MaterialSlot::PergolaWood);
                if (pergW > 0.8f && pergD > 0.8f) {
                    appendBox(vertices, indices, {cx - pergW, deckY, cz + pergD - 0.2f}, {cx - pergW + 0.2f, baseTopY + pergH, cz + pergD}, pergMat);
                    appendBox(vertices, indices, {cx + pergW - 0.2f, deckY, cz + pergD - 0.2f}, {cx + pergW, baseTopY + pergH, cz + pergD}, pergMat);
                    appendBox(vertices, indices, {cx + pergW - 0.2f, deckY, cz - pergD}, {cx + pergW, baseTopY + pergH, cz - pergD + 0.2f}, pergMat);
                    appendBox(vertices, indices, {cx - pergW, deckY, cz - pergD}, {cx - pergW + 0.2f, baseTopY + pergH, cz - pergD + 0.2f}, pergMat);
                    for (int r = 0; r <= 4; ++r) {
                        float rZ = cz - pergD + (static_cast<float>(r) / 4.0f) * (pergD * 2.0f);
                        appendBox(vertices, indices, {cx - pergW - 0.22f, baseTopY + pergH, rZ - 0.08f}, {cx + pergW + 0.22f, baseTopY + pergH + 0.16f, rZ + 0.08f}, pergMat);
                    }
                }
            }
            break;
        }

        case RoofType::Gable: {
            float pitchDrop = effOverhang * (roofHeight / std::max(0.8f, halfD));
            float zSpanF = std::max(0.5f, m.maxZ - cz);
            float zSpanB = std::max(0.5f, cz - m.minZ);
            auto yGable = [&](float /*x*/, float z) -> float {
                if (z >= cz) {
                    return baseTopY + roofHeight * (1.0f - (z - cz) / zSpanF);
                }
                return baseTopY + roofHeight * (1.0f - (cz - z) / zSpanB);
            };
            (void)pitchDrop;
            std::vector<Tri2D> tris = triangulatePolygonXZ(overPoly);
            emitClippedZBandHeightField(vertices, indices, tris, cz, mo.maxZ + 0.5f, yGable, roofMat, true, metalMat);
            emitClippedZBandHeightField(vertices, indices, tris, mo.minZ - 0.5f, cz, yGable, roofMat, true, metalMat);
            emitPerimeterClosureWallZBreaks(vertices, indices, basePoly, baseTopY, {cz}, yGable, wallMat);
            break;
        }

        case RoofType::Hip: {
            if (m.isAxisAlignedRect) {
                float hw = halfW + effOverhang;
                float hd = halfD + effOverhang;
                float apexY = baseTopY + roofHeight;
                float ridgeHw = std::max(0.0f, hw - hd * 0.75f);
                Vec3 rL{cx - ridgeHw, apexY, cz};
                Vec3 rR{cx + ridgeHw, apexY, cz};
                Vec3 eFL{cx - hw, baseTopY, cz + hd};
                Vec3 eFR{cx + hw, baseTopY, cz + hd};
                Vec3 eBR{cx + hw, baseTopY, cz - hd};
                Vec3 eBL{cx - hw, baseTopY, cz - hd};
                appendQuad(vertices, indices, eFL, eFR, rR, rL, {0.0f, 0.707f, 0.707f}, roofMat);
                appendQuad(vertices, indices, eBR, eBL, rL, rR, {0.0f, 0.707f, -0.707f}, roofMat);
                Vec3 nL{-0.707f, 0.707f, 0.0f};
                appendTri3D(vertices, indices, eBL, eFL, rL, nL, roofMat);
                Vec3 nR{0.707f, 0.707f, 0.0f};
                appendTri3D(vertices, indices, eFR, eBR, rR, nR, roofMat);
            } else {
                // Conforming hipped crest spine along longer X span if convex/elongated, else apex centroid
                float apexY = baseTopY + roofHeight;
                float spineHalf = (mo.maxX - mo.minX > (mo.maxZ - mo.minZ) * 1.15f)
                    ? std::max(0.0f, ((mo.maxX - mo.minX) - (mo.maxZ - mo.minZ)) * 0.35f)
                    : 0.0f;
                // Guarantee spine stays inside polygon
                if (spineHalf > 0.1f && !pointInPolygon2D(Vec2{cx + spineHalf, cz}, basePoly)) {
                    spineHalf = 0.0f;
                }
                size_t n = overPoly.size();
                for (size_t i = 0; i < n; ++i) {
                    const Vec2& a = overPoly[i];
                    const Vec2& b = overPoly[(i + 1) % n];
                    float ax0 = std::clamp(a.x, cx - spineHalf, cx + spineHalf);
                    float bx0 = std::clamp(b.x, cx - spineHalf, cx + spineHalf);
                    Vec3 p0{a.x, baseTopY, a.y};
                    Vec3 p1{b.x, baseTopY, b.y};
                    Vec3 r1{bx0, apexY, cz};
                    Vec3 r0{ax0, apexY, cz};
                    if (std::abs(ax0 - bx0) < 1e-4f) {
                        appendUpwardTri3D(vertices, indices, p0, p1, r0, roofMat, true, metalMat, 0.12f);
                    } else {
                        appendUpwardTri3D(vertices, indices, p0, p1, r1, roofMat, true, metalMat, 0.12f);
                        appendUpwardTri3D(vertices, indices, p0, r1, r0, roofMat, true, metalMat, 0.12f);
                    }
                }
            }
            break;
        }

        case RoofType::Mansard: {
            float curbY = baseTopY + roofHeight * 0.65f;
            std::vector<Vec2> curbPoly = scaleContour(overPoly, m.centroid, 0.74f);
            size_t n = overPoly.size();
            for (size_t i = 0; i < n; ++i) {
                size_t next = (i + 1) % n;
                Vec3 p0{overPoly[i].x, baseTopY, overPoly[i].y};
                Vec3 p1{overPoly[next].x, baseTopY, overPoly[next].y};
                Vec3 p2{curbPoly[next].x, curbY, curbPoly[next].y};
                Vec3 p3{curbPoly[i].x, curbY, curbPoly[i].y};
                Vec3 norm = ((p1 - p0).cross(p3 - p0)).normalized();
                if (norm.y < 0.0f) norm = -norm;
                appendQuad(vertices, indices, p0, p1, p2, p3, norm, roofMat);
            }
            std::vector<Tri2D> topTris = triangulatePolygonXZ(curbPoly);
            for (const auto& t : topTris) {
                appendTri3D(vertices, indices,
                    Vec3{t.a.x, curbY, t.a.y}, Vec3{t.b.x, curbY, t.b.y}, Vec3{t.c.x, curbY, t.c.y},
                    Vec3{0.0f, 1.0f, 0.0f}, roofMat);
            }
            break;
        }

        case RoofType::Dome: {
            const int rings = 12;
            size_t n = overPoly.size();
            for (int r = 0; r < rings; ++r) {
                float phi0 = (static_cast<float>(r) / rings) * (kPi * 0.5f);
                float phi1 = (static_cast<float>(r + 1) / rings) * (kPi * 0.5f);
                float s0 = std::cos(phi0), y0 = baseTopY + roofHeight * std::sin(phi0);
                float s1 = std::cos(phi1), y1 = baseTopY + roofHeight * std::sin(phi1);
                for (size_t i = 0; i < n; ++i) {
                    size_t next = (i + 1) % n;
                    Vec3 p0{cx + (overPoly[i].x - cx) * s0, y0, cz + (overPoly[i].y - cz) * s0};
                    Vec3 p1{cx + (overPoly[next].x - cx) * s0, y0, cz + (overPoly[next].y - cz) * s0};
                    Vec3 p2{cx + (overPoly[next].x - cx) * s1, y1, cz + (overPoly[next].y - cz) * s1};
                    Vec3 p3{cx + (overPoly[i].x - cx) * s1, y1, cz + (overPoly[i].y - cz) * s1};
                    if (r == rings - 1) {
                        Vec3 apex{cx, baseTopY + roofHeight, cz};
                        appendUpwardTri3D(vertices, indices, p0, p1, apex, roofMat);
                    } else {
                        appendUpwardTri3D(vertices, indices, p0, p1, p2, roofMat);
                        appendUpwardTri3D(vertices, indices, p0, p2, p3, roofMat);
                    }
                }
            }
            break;
        }

        case RoofType::PyramidalApex: {
            Vec3 apex{cx, baseTopY + roofHeight, cz};
            size_t n = overPoly.size();
            for (size_t i = 0; i < n; ++i) {
                size_t next = (i + 1) % n;
                Vec3 p0{overPoly[i].x, baseTopY, overPoly[i].y};
                Vec3 p1{overPoly[next].x, baseTopY, overPoly[next].y};
                appendUpwardTri3D(vertices, indices, p0, p1, apex, roofMat, effOverhang > 0.05f, metalMat, 0.10f);
            }
            break;
        }

        case RoofType::ShedMonopitch: {
            float zRange = std::max(0.6f, m.maxZ - m.minZ);
            auto yShed = [&](float /*x*/, float z) -> float {
                return baseTopY + roofHeight * std::clamp((m.maxZ - z) / zRange, 0.0f, 1.15f);
            };
            std::vector<Tri2D> tris = triangulatePolygonXZ(overPoly);
            for (const auto& t : tris) {
                appendUpwardTri3D(vertices, indices,
                    Vec3{t.a.x, yShed(t.a.x, t.a.y), t.a.y},
                    Vec3{t.b.x, yShed(t.b.x, t.b.y), t.b.y},
                    Vec3{t.c.x, yShed(t.c.x, t.c.y), t.c.y},
                    roofMat, true, metalMat);
            }
            emitPerimeterClosureWallZBreaks(vertices, indices, basePoly, baseTopY, {}, yShed, wallMat);
            break;
        }

        case RoofType::SawtoothIndustrial: {
            int teeth = 3;
            float zLen = std::max(1.2f, m.maxZ - m.minZ);
            float toothD = zLen / teeth;
            float toothH = roofHeight * 0.85f;
            std::vector<Tri2D> tris = triangulatePolygonXZ(overPoly);
            std::vector<float> zBreaks;
            for (int t = 0; t < teeth; ++t) {
                float zStart = m.maxZ - t * toothD;
                float zRidge = zStart - toothD * 0.28f;
                float zEnd = zStart - toothD;
                zBreaks.push_back(zRidge);
                zBreaks.push_back(zEnd);
                auto yFront = [&](float, float z) {
                    float f = std::clamp((zStart - z) / std::max(0.1f, toothD * 0.28f), 0.0f, 1.0f);
                    return baseTopY + toothH * f;
                };
                auto yBack = [&](float, float z) {
                    float f = std::clamp((z - zEnd) / std::max(0.1f, toothD * 0.72f), 0.0f, 1.0f);
                    return baseTopY + toothH * f;
                };
                emitClippedZBandHeightField(vertices, indices, tris, zRidge, zStart + 0.5f, yFront, glassMat, true, metalMat);
                emitClippedZBandHeightField(vertices, indices, tris, zEnd - 0.5f, zRidge, yBack, roofMat, true, metalMat);
            }
            auto yToothAt = [&](float, float z) -> float {
                float rel = std::clamp((m.maxZ - z) / zLen, 0.0f, 1.0f) * teeth;
                int tIdx = std::min(teeth - 1, std::max(0, static_cast<int>(rel)));
                float local = rel - tIdx;
                return (local < 0.28f)
                    ? (baseTopY + toothH * (local / 0.28f))
                    : (baseTopY + toothH * (1.0f - (local - 0.28f) / 0.72f));
            };
            emitPerimeterClosureWallZBreaks(vertices, indices, basePoly, baseTopY, zBreaks, yToothAt, wallMat);
            break;
        }

        case RoofType::BarrelVault: {
            int segs = 24;
            float xLen = std::max(1.0f, m.maxX - m.minX);
            std::vector<Tri2D> tris = triangulatePolygonXZ(overPoly);
            auto yVault = [&](float x, float /*z*/) -> float {
                float u = std::clamp((x - m.minX) / xLen, 0.0f, 1.0f);
                return baseTopY + roofHeight * std::sin(u * kPi);
            };
            std::vector<float> xBreaks;
            for (int s = 0; s < segs; ++s) {
                float x0 = mo.minX + (static_cast<float>(s) / segs) * (mo.maxX - mo.minX);
                float x1 = mo.minX + (static_cast<float>(s + 1) / segs) * (mo.maxX - mo.minX);
                if (s > 0) xBreaks.push_back(x0);
                emitClippedXBandHeightField(vertices, indices, tris, x0 - 0.01f, x1 + 0.01f, yVault, roofMat, true, metalMat);
            }
            emitPerimeterClosureWallXBreaks(vertices, indices, basePoly, baseTopY, xBreaks, yVault, wallMat);
            break;
        }

        case RoofType::GambrelDutch: {
            float knuckleY = baseTopY + roofHeight * 0.58f;
            float apexY = baseTopY + roofHeight;
            float zSpanF = std::max(0.6f, m.maxZ - cz);
            float zSpanB = std::max(0.6f, cz - m.minZ);
            float kf = cz + zSpanF * 0.65f;
            float kb = cz - zSpanB * 0.65f;

            auto yGambrel = [&](float /*x*/, float z) -> float {
                if (z >= kf) {
                    return baseTopY + (knuckleY - baseTopY) * std::clamp((m.maxZ - z) / (zSpanF * 0.35f), 0.0f, 1.0f);
                } else if (z >= cz) {
                    return knuckleY + (apexY - knuckleY) * std::clamp((kf - z) / (zSpanF * 0.65f), 0.0f, 1.0f);
                } else if (z >= kb) {
                    return knuckleY + (apexY - knuckleY) * std::clamp((z - kb) / (zSpanB * 0.65f), 0.0f, 1.0f);
                }
                return baseTopY + (knuckleY - baseTopY) * std::clamp((z - m.minZ) / (zSpanB * 0.35f), 0.0f, 1.0f);
            };
            std::vector<Tri2D> tris = triangulatePolygonXZ(overPoly);
            emitClippedZBandHeightField(vertices, indices, tris, kf, mo.maxZ + 0.5f, yGambrel, roofMat, true, metalMat);
            emitClippedZBandHeightField(vertices, indices, tris, cz, kf, yGambrel, roofMat, true, metalMat);
            emitClippedZBandHeightField(vertices, indices, tris, kb, cz, yGambrel, roofMat, true, metalMat);
            emitClippedZBandHeightField(vertices, indices, tris, mo.minZ - 0.5f, kb, yGambrel, roofMat, true, metalMat);
            emitPerimeterClosureWallZBreaks(vertices, indices, basePoly, baseTopY, {kf, cz, kb}, yGambrel, wallMat);
            break;
        }

        case RoofType::Butterfly: {
            float outerY = baseTopY + roofHeight;
            float valleyY = baseTopY + 0.18f;
            float zSpanF = std::max(0.6f, m.maxZ - cz);
            float zSpanB = std::max(0.6f, cz - m.minZ);
            auto yButterfly = [&](float /*x*/, float z) -> float {
                float ratio = (z >= cz) ? ((z - cz) / zSpanF) : ((cz - z) / zSpanB);
                return valleyY + (outerY - valleyY) * std::min(1.15f, ratio);
            };
            std::vector<Tri2D> tris = triangulatePolygonXZ(overPoly);
            emitClippedZBandHeightField(vertices, indices, tris, cz, mo.maxZ + 0.5f, yButterfly, roofMat, true, metalMat);
            emitClippedZBandHeightField(vertices, indices, tris, mo.minZ - 0.5f, cz, yButterfly, roofMat, true, metalMat);

            // Clamped valley gutter beam fitted strictly to exact X chord at z = cz
            float chMinX = cx - halfW * 0.8f, chMaxX = cx + halfW * 0.8f;
            if (chordXAtZ(basePoly, cz, chMinX, chMaxX)) {
                appendBox(vertices, indices,
                    {chMinX, baseTopY, cz - 0.22f},
                    {chMaxX, valleyY + 0.14f, cz + 0.22f}, metalMat);
            }
            emitPerimeterClosureWallZBreaks(vertices, indices, basePoly, baseTopY, {cz}, yButterfly, glassMat);
            break;
        }

        case RoofType::CrossGableMultiDormer: {
            // Primary conforming Gable + central intersecting cross-gable & dormers inside inscribed chord
            float zSpanF = std::max(0.6f, m.maxZ - cz);
            float zSpanB = std::max(0.6f, cz - m.minZ);
            auto yPrimary = [&](float /*x*/, float z) -> float {
                if (z >= cz) return baseTopY + roofHeight * (1.0f - (z - cz) / zSpanF);
                return baseTopY + roofHeight * (1.0f - (cz - z) / zSpanB);
            };
            std::vector<Tri2D> tris = triangulatePolygonXZ(overPoly);
            emitClippedZBandHeightField(vertices, indices, tris, cz, mo.maxZ + 0.5f, yPrimary, roofMat, true, metalMat);
            emitClippedZBandHeightField(vertices, indices, tris, mo.minZ - 0.5f, cz, yPrimary, roofMat, true, metalMat);
            emitPerimeterClosureWallZBreaks(vertices, indices, basePoly, baseTopY, {cz}, yPrimary, wallMat);

            // Central intersecting Z-ridge cross gable clamped cleanly to center chord
            float crossMinZ = cz - halfD * 0.8f, crossMaxZ = cz + halfD * 0.8f;
            chordZAtX(basePoly, cx, crossMinZ, crossMaxZ);
            float cHw = std::min(halfW * 0.36f, m.minInradius * 0.52f);
            float zTopMin = crossMinZ + (cz - crossMinZ) * 0.15f;
            float zTopMax = crossMaxZ - (crossMaxZ - cz) * 0.15f;
            chordZAtX(basePoly, cx - cHw, zTopMin, zTopMax);
            float zTmpMin = zTopMin, zTmpMax = zTopMax;
            if (chordZAtX(basePoly, cx + cHw, zTmpMin, zTmpMax)) {
                zTopMin = std::max(zTopMin, zTmpMin);
                zTopMax = std::min(zTopMax, zTmpMax);
            }
            if (cHw > 1.1f && (zTopMax - zTopMin) > 2.0f) {
                float cApexY = baseTopY + roofHeight * 0.94f;
                appendQuad(vertices, indices,
                    {cx - cHw, baseTopY, zTopMax}, {cx, cApexY, zTopMax},
                    {cx, cApexY, zTopMin}, {cx - cHw, baseTopY, zTopMin},
                    {-0.707f, 0.707f, 0.0f}, roofMat);
                appendQuad(vertices, indices,
                    {cx, cApexY, zTopMax}, {cx + cHw, baseTopY, zTopMax},
                    {cx + cHw, baseTopY, zTopMin}, {cx, cApexY, zTopMin},
                    {0.707f, 0.707f, 0.0f}, roofMat);
                appendTri3D(vertices, indices,
                    {cx - cHw, baseTopY, zTopMax}, {cx + cHw, baseTopY, zTopMax}, {cx, cApexY, zTopMax},
                    {0.0f, 0.0f, 1.0f}, wallMat);
                appendTri3D(vertices, indices,
                    {cx + cHw, baseTopY, zTopMin}, {cx - cHw, baseTopY, zTopMin}, {cx, cApexY, zTopMin},
                    {0.0f, 0.0f, -1.0f}, wallMat);
                appendBox(vertices, indices,
                    {cx - 0.48f, baseTopY + 0.32f, zTopMax - 0.05f},
                    {cx + 0.48f, baseTopY + roofHeight * 0.52f, zTopMax + 0.08f},
                    glassMat);
            }

            // Flanking glazed dormers only where interior chord provides structural support
            for (float dxSign : {-1.0f, 1.0f}) {
                float dx = cx + dxSign * std::max(cHw + 1.25f, halfW * 0.55f);
                float dMinZ = cz, dMaxZ = cz;
                if (chordZAtX(basePoly, dx, dMinZ, dMaxZ) && (dMaxZ - cz) > 2.4f &&
                    pointInPolygon2D(Vec2{dx, cz + (dMaxZ - cz) * 0.55f}, basePoly)) {
                    float dW = 0.72f;
                    float dY0 = baseTopY + roofHeight * 0.24f;
                    float dY1 = dY0 + roofHeight * 0.38f;
                    float z0 = cz + (dMaxZ - cz) * 0.32f;
                    float z1 = cz + (dMaxZ - cz) * 0.68f;
                    appendBox(vertices, indices, {dx - dW, dY0, z0}, {dx + dW, dY1, z1}, wallMat);
                    appendBox(vertices, indices, {dx - dW * 0.65f, dY0 + 0.14f, z1 - 0.02f}, {dx + dW * 0.65f, dY1 - 0.10f, z1 + 0.05f}, glassMat);
                    appendQuad(vertices, indices, {dx - dW - 0.12f, dY1, z1 + 0.08f}, {dx, dY1 + 0.48f, z1 + 0.08f}, {dx, dY1 + 0.48f, z0 - 0.12f}, {dx - dW - 0.12f, dY1, z0 - 0.12f}, {-0.6f, 0.8f, 0.0f}, metalMat);
                    appendQuad(vertices, indices, {dx, dY1 + 0.48f, z1 + 0.08f}, {dx + dW + 0.12f, dY1, z1 + 0.08f}, {dx + dW + 0.12f, dY1, z0 - 0.12f}, {dx, dY1 + 0.48f, z0 - 0.12f}, {0.6f, 0.8f, 0.0f}, metalMat);
                }
            }
            break;
        }

        case RoofType::DutchHipWithCupola: {
            float deckY = baseTopY + roofHeight * 0.58f;
            std::vector<Vec2> topPoly = scaleContour(overPoly, m.centroid, 0.55f);
            size_t n = overPoly.size();
            for (size_t i = 0; i < n; ++i) {
                size_t next = (i + 1) % n;
                Vec3 p0{overPoly[i].x, baseTopY, overPoly[i].y};
                Vec3 p1{overPoly[next].x, baseTopY, overPoly[next].y};
                Vec3 p2{topPoly[next].x, deckY, topPoly[next].y};
                Vec3 p3{topPoly[i].x, deckY, topPoly[i].y};
                Vec3 norm = ((p1 - p0).cross(p3 - p0)).normalized();
                if (norm.y < 0.0f) norm = -norm;
                appendQuad(vertices, indices, p0, p1, p2, p3, norm, roofMat);
            }
            std::vector<Tri2D> topTris = triangulatePolygonXZ(topPoly);
            for (const auto& t : topTris) {
                appendTri3D(vertices, indices,
                    Vec3{t.a.x, deckY, t.a.y}, Vec3{t.b.x, deckY, t.b.y}, Vec3{t.c.x, deckY, t.c.y},
                    Vec3{0.0f, 1.0f, 0.0f}, roofMat);
            }

            // Conforming Belvedere Lantern Cupola fitted inside inscribed top plateau
            float cupR = std::min(m.minInradius * 0.30f, 2.0f);
            float cupTopY = deckY + roofHeight * 0.35f;
            appendOrientedCylinder(vertices, indices,
                Vec3{cx, deckY, cz}, Vec3{cx, cupTopY, cz},
                cupR, cupR, 20, wallMat, true, true);
            appendOrientedCylinder(vertices, indices,
                Vec3{cx, deckY + 0.14f, cz}, Vec3{cx, cupTopY - 0.12f, cz},
                cupR * 1.04f, cupR * 1.04f, 20, glassMat, false, false);
            Vec3 apex{cx, cupTopY + roofHeight * 0.28f, cz};
            appendOrientedCylinder(vertices, indices,
                Vec3{cx, cupTopY, cz}, apex,
                cupR * 1.24f, 0.05f, 20, roofMat, true, true);
            appendOrientedCylinder(vertices, indices,
                apex, apex + Vec3{0.0f, 1.05f, 0.0f},
                0.06f, 0.015f, 16, brassMat, true, true);
            break;
        }

        case RoofType::PagodaCurvedEaves: {
            uint32_t woodMat = static_cast<uint32_t>(MaterialSlot::PergolaWood);
            auto emitCurvedContourTier = [&](float yBase, float tierH, float baseScale, float topScale) {
                const int rings = 6;
                size_t n = overPoly.size();
                for (int r = 0; r < rings; ++r) {
                    float t0 = static_cast<float>(r) / rings;
                    float t1 = static_cast<float>(r + 1) / rings;
                    float s0 = baseScale * (1.0f - (1.0f - topScale) * std::pow(t0, 0.72f));
                    float s1 = baseScale * (1.0f - (1.0f - topScale) * std::pow(t1, 0.72f));
                    float y0 = yBase + tierH * (t0 * t0 * 0.85f + t0 * 0.15f);
                    float y1 = yBase + tierH * (t1 * t1 * 0.85f + t1 * 0.15f);
                    if (r == 0) y0 += 0.22f;
                    for (size_t i = 0; i < n; ++i) {
                        size_t next = (i + 1) % n;
                        Vec3 p0{cx + (overPoly[i].x - cx) * s0, y0, cz + (overPoly[i].y - cz) * s0};
                        Vec3 p1{cx + (overPoly[next].x - cx) * s0, y0, cz + (overPoly[next].y - cz) * s0};
                        Vec3 p2{cx + (overPoly[next].x - cx) * s1, y1, cz + (overPoly[next].y - cz) * s1};
                        Vec3 p3{cx + (overPoly[i].x - cx) * s1, y1, cz + (overPoly[i].y - cz) * s1};
                        appendUpwardTri3D(vertices, indices, p0, p1, p2, roofMat, r == 0, woodMat, 0.10f);
                        appendUpwardTri3D(vertices, indices, p0, p2, p3, roofMat, r == 0, woodMat, 0.10f);
                    }
                }
            };

            // Lower conforming timber collar & swept curved tier
            std::vector<Vec2> collar = scaleContour(basePoly, m.centroid, 0.88f);
            for (size_t i = 0; i < collar.size(); ++i) {
                size_t next = (i + 1) % collar.size();
                Vec3 norm = Vec3{-(collar[next].y - collar[i].y), 0.0f, (collar[next].x - collar[i].x)}.normalized();
                appendQuad(vertices, indices,
                    Vec3{collar[i].x, baseTopY, collar[i].y}, Vec3{collar[next].x, baseTopY, collar[next].y},
                    Vec3{collar[next].x, baseTopY + 0.32f, collar[next].y}, Vec3{collar[i].x, baseTopY + 0.32f, collar[i].y},
                    norm, woodMat);
            }
            float t1H = roofHeight * 0.45f;
            emitCurvedContourTier(baseTopY + 0.14f, t1H, 1.04f, 0.56f);

            // Second tier tambour + curved tier 2 + gold finial centered at (cx, cz)
            float t2Base = baseTopY + 0.14f + t1H;
            float t2TambH = roofHeight * 0.18f;
            std::vector<Vec2> t2Poly = scaleContour(overPoly, m.centroid, 0.52f);
            for (size_t i = 0; i < t2Poly.size(); ++i) {
                size_t next = (i + 1) % t2Poly.size();
                Vec3 norm = Vec3{-(t2Poly[next].y - t2Poly[i].y), 0.0f, (t2Poly[next].x - t2Poly[i].x)}.normalized();
                appendQuad(vertices, indices,
                    Vec3{t2Poly[i].x, t2Base, t2Poly[i].y}, Vec3{t2Poly[next].x, t2Base, t2Poly[next].y},
                    Vec3{t2Poly[next].x, t2Base + t2TambH, t2Poly[next].y}, Vec3{t2Poly[i].x, t2Base + t2TambH, t2Poly[i].y},
                    norm, woodMat);
            }
            float t2Y = t2Base + t2TambH;
            float t2H = roofHeight * 0.42f;
            emitCurvedContourTier(t2Y, t2H, 0.66f, 0.08f);
            appendOrientedCylinder(vertices, indices,
                Vec3{cx, t2Y + t2H, cz}, Vec3{cx, t2Y + t2H + 1.25f, cz},
                0.11f, 0.02f, 20, brassMat, true, true);
            break;
        }

        case RoofType::GeodesicSkylightCrown: {
            float curbH = 0.50f;
            size_t n = basePoly.size();
            for (size_t i = 0; i < n; ++i) {
                size_t next = (i + 1) % n;
                Vec3 norm = Vec3{-(basePoly[next].y - basePoly[i].y), 0.0f, (basePoly[next].x - basePoly[i].x)}.normalized();
                appendQuad(vertices, indices,
                    Vec3{basePoly[i].x, baseTopY, basePoly[i].y}, Vec3{basePoly[next].x, baseTopY, basePoly[next].y},
                    Vec3{basePoly[next].x, baseTopY + curbH, basePoly[next].y}, Vec3{basePoly[i].x, baseTopY + curbH, basePoly[i].y},
                    norm, wallMat);
            }

            // Conforming geodesic lattice shell smoothly morphing every contour edge inward to (cx, cz)
            float domeBaseY = baseTopY + curbH;
            const int rings = 8;
            for (int r = 0; r < rings; ++r) {
                float phi0 = (static_cast<float>(r) / rings) * (kPi * 0.5f);
                float phi1 = (static_cast<float>(r + 1) / rings) * (kPi * 0.5f);
                float s0 = std::cos(phi0), y0 = domeBaseY + roofHeight * std::sin(phi0);
                float s1 = std::cos(phi1), y1 = domeBaseY + roofHeight * std::sin(phi1);
                for (size_t i = 0; i < n; ++i) {
                    size_t next = (i + 1) % n;
                    Vec3 p0{cx + (basePoly[i].x - cx) * s0, y0, cz + (basePoly[i].y - cz) * s0};
                    Vec3 p1{cx + (basePoly[next].x - cx) * s0, y0, cz + (basePoly[next].y - cz) * s0};
                    Vec3 p2{cx + (basePoly[next].x - cx) * s1, y1, cz + (basePoly[next].y - cz) * s1};
                    Vec3 p3{cx + (basePoly[i].x - cx) * s1, y1, cz + (basePoly[i].y - cz) * s1};
                    if (r == rings - 1) {
                        Vec3 apex{cx, domeBaseY + roofHeight, cz};
                        appendUpwardTri3D(vertices, indices, p0, p1, apex, glassMat);
                        appendOrientedCylinder(vertices, indices, p0, apex, 0.04f, 0.04f, 6, metalMat, false, false);
                    } else {
                        appendUpwardTri3D(vertices, indices, p0, p1, p2, glassMat);
                        appendUpwardTri3D(vertices, indices, p0, p2, p3, glassMat);
                        if (n <= 18 || (i % 2 == 0)) {
                            appendOrientedCylinder(vertices, indices, p0, p1, 0.04f, 0.04f, 6, metalMat, false, false);
                            appendOrientedCylinder(vertices, indices, p0, p2, 0.036f, 0.036f, 6, metalMat, false, false);
                            appendOrientedCylinder(vertices, indices, p0, p3, 0.038f, 0.038f, 6, metalMat, false, false);
                        }
                    }
                }
            }
            break;
        }

        case RoofType::SplitClerestoryTwinPitch: {
            float lowPitchY = baseTopY + roofHeight * 0.48f;
            float highPitchLowY = baseTopY + roofHeight * 0.35f;
            float highPitchTopY = baseTopY + roofHeight;
            float zSpanF = std::max(0.5f, m.maxZ - cz);
            float zSpanB = std::max(0.5f, cz - m.minZ);
            auto ySplit = [&](float /*x*/, float z) -> float {
                if (z >= cz) {
                    return lowPitchY - (lowPitchY - baseTopY) * ((z - cz) / zSpanF);
                }
                return highPitchTopY - (highPitchTopY - highPitchLowY) * ((cz - z) / zSpanB);
            };
            std::vector<Tri2D> tris = triangulatePolygonXZ(overPoly);
            emitClippedZBandHeightField(vertices, indices, tris, cz, mo.maxZ + 0.5f, ySplit, roofMat, true, metalMat);
            emitClippedZBandHeightField(vertices, indices, tris, mo.minZ - 0.5f, cz, ySplit, roofMat, true, metalMat);
            emitPerimeterClosureWallZBreaks(vertices, indices, basePoly, baseTopY, {cz}, ySplit, wallMat);

            float chMinX = cx - halfW * 0.8f, chMaxX = cx + halfW * 0.8f;
            if (chordXAtZ(basePoly, cz, chMinX, chMaxX)) {
                appendQuad(vertices, indices,
                    {chMinX, lowPitchY, cz + 0.02f}, {chMaxX, lowPitchY, cz + 0.02f},
                    {chMaxX, highPitchTopY, cz + 0.02f}, {chMinX, highPitchTopY, cz + 0.02f},
                    {0.0f, 0.0f, 1.0f}, glassMat);
                int mulCount = 4;
                for (int k = 1; k < mulCount; ++k) {
                    float mx = chMinX + (static_cast<float>(k) / mulCount) * (chMaxX - chMinX);
                    appendBox(vertices, indices,
                        {mx - 0.05f, lowPitchY, cz - 0.04f},
                        {mx + 0.05f, highPitchTopY, cz + 0.08f}, metalMat);
                }
            }
            break;
        }

        case RoofType::HyperbolicParaboloidShell: {
            auto hyparY = [&](float x, float z) -> float {
                float u = (x - cx) / std::max(0.5f, halfW);
                float v = (z - cz) / std::max(0.5f, halfD);
                return baseTopY + roofHeight * (0.45f + 0.55f * std::clamp(u * v, -0.78f, 1.0f));
            };
            const int rings = 8;
            size_t n = overPoly.size();
            for (int r = 0; r < rings; ++r) {
                float t0 = static_cast<float>(r) / rings;
                float t1 = static_cast<float>(r + 1) / rings;
                for (size_t i = 0; i < n; ++i) {
                    size_t next = (i + 1) % n;
                    Vec2 a0{cx + (overPoly[i].x - cx) * t0, cz + (overPoly[i].y - cz) * t0};
                    Vec2 b0{cx + (overPoly[next].x - cx) * t0, cz + (overPoly[next].y - cz) * t0};
                    Vec2 b1{cx + (overPoly[next].x - cx) * t1, cz + (overPoly[next].y - cz) * t1};
                    Vec2 a1{cx + (overPoly[i].x - cx) * t1, cz + (overPoly[i].y - cz) * t1};
                    if (r == 0) {
                        appendUpwardTri3D(vertices, indices,
                            Vec3{a0.x, hyparY(a0.x, a0.y), a0.y},
                            Vec3{a1.x, hyparY(a1.x, a1.y), a1.y},
                            Vec3{b1.x, hyparY(b1.x, b1.y), b1.y},
                            roofMat, true, metalMat, 0.16f);
                    } else {
                        appendUpwardTri3D(vertices, indices,
                            Vec3{a0.x, hyparY(a0.x, a0.y), a0.y},
                            Vec3{a1.x, hyparY(a1.x, a1.y), a1.y},
                            Vec3{b1.x, hyparY(b1.x, b1.y), b1.y},
                            roofMat, true, metalMat, 0.16f);
                        appendUpwardTri3D(vertices, indices,
                            Vec3{a0.x, hyparY(a0.x, a0.y), a0.y},
                            Vec3{b1.x, hyparY(b1.x, b1.y), b1.y},
                            Vec3{b0.x, hyparY(b0.x, b0.y), b0.y},
                            roofMat, true, metalMat, 0.16f);
                    }
                }
            }
            // Anchor perimeter support piers directly on basePoly contour edges
            for (size_t k = 0; k < 4; ++k) {
                size_t idx = (k * n) / 4;
                const Vec2& p = basePoly[idx];
                appendOrientedCylinder(vertices, indices,
                    Vec3{p.x, baseTopY - 0.08f, p.y},
                    Vec3{p.x, hyparY(p.x, p.y), p.y},
                    0.16f, 0.12f, 12, metalMat, true, true);
            }
            break;
        }

        case RoofType::ConicalSpireLantern: {
            float midY = baseTopY + roofHeight * 0.42f;
            std::vector<Vec2> midPoly = scaleContour(overPoly, m.centroid, 0.48f);
            size_t n = overPoly.size();
            for (size_t i = 0; i < n; ++i) {
                size_t next = (i + 1) % n;
                Vec3 p0{overPoly[i].x, baseTopY, overPoly[i].y};
                Vec3 p1{overPoly[next].x, baseTopY, overPoly[next].y};
                Vec3 p2{midPoly[next].x, midY, midPoly[next].y};
                Vec3 p3{midPoly[i].x, midY, midPoly[i].y};
                Vec3 norm = ((p1 - p0).cross(p3 - p0)).normalized();
                if (norm.y < 0.0f) norm = -norm;
                appendQuad(vertices, indices, p0, p1, p2, p3, norm, roofMat);
            }
            std::vector<Tri2D> midTris = triangulatePolygonXZ(midPoly);
            for (const auto& t : midTris) {
                appendTri3D(vertices, indices,
                    Vec3{t.a.x, midY, t.a.y}, Vec3{t.b.x, midY, t.b.y}, Vec3{t.c.x, midY, t.c.y},
                    Vec3{0.0f, 1.0f, 0.0f}, wallMat);
            }

            float drumR = std::min(m.minInradius * 0.34f, 3.0f);
            float drumH = roofHeight * 0.26f;
            appendOrientedCylinder(vertices, indices,
                Vec3{cx, midY, cz}, Vec3{cx, midY + drumH, cz},
                drumR, drumR, 24, glassMat, true, true);

            float spireBaseY = midY + drumH;
            float spireH = roofHeight * 0.52f;
            appendOrientedCylinder(vertices, indices,
                Vec3{cx, spireBaseY, cz}, Vec3{cx, spireBaseY + spireH, cz},
                drumR * 1.18f, 0.08f, 24, roofMat, true, true);
            appendOrientedCylinder(vertices, indices,
                Vec3{cx, spireBaseY + spireH, cz}, Vec3{cx, spireBaseY + spireH + 1.35f, cz},
                0.08f, 0.015f, 16, brassMat, true, true);
            break;
        }

        case RoofType::DiagridSailCanopy: {
            uint32_t sailMat = static_cast<uint32_t>(MaterialSlot::WallStucco);
            // Conforming sky terrace deck slab over actual building perimeter
            std::vector<Vec2> deckPoly = scaleContour(basePoly, m.centroid, 0.96f);
            std::vector<Tri2D> deckTris = triangulatePolygonXZ(deckPoly);
            for (const auto& t : deckTris) {
                appendTri3D(vertices, indices,
                    Vec3{t.a.x, baseTopY + 0.04f, t.a.y},
                    Vec3{t.b.x, baseTopY + 0.04f, t.b.y},
                    Vec3{t.c.x, baseTopY + 0.04f, t.c.y},
                    Vec3{0.0f, 1.0f, 0.0f}, roofMat);
            }

            // Undulating multi-wave tensile sail height function
            float mastH = roofHeight * 0.85f;
            auto sailY = [&](float x, float z) -> float {
                float u = (x - cx) / std::max(0.5f, halfW);
                float v = (z - cz) / std::max(0.5f, halfD);
                float wave = std::sin(u * kPi) * std::cos(v * kPi * 0.85f) * 0.45f;
                float lift = 0.32f * std::min(1.5f, u * u + v * v);
                return baseTopY + mastH * 0.65f + roofHeight * 0.35f * (wave + lift);
            };

            // Inclined V-mast struts anchored strictly at 4 evenly spaced perimeter vertices
            size_t n = basePoly.size();
            for (size_t k = 0; k < 4; ++k) {
                size_t idx = (k * n) / 4;
                const Vec2& bp = basePoly[idx];
                const Vec2& op = overPoly[idx];
                Vec2 mBase{cx + (bp.x - cx) * 0.82f, cz + (bp.y - cz) * 0.82f};
                appendOrientedCylinder(vertices, indices,
                    Vec3{mBase.x, baseTopY, mBase.y},
                    Vec3{op.x, sailY(op.x, op.y), op.y},
                    0.09f, 0.06f, 12, metalMat, true, true);
            }

            const int rings = 8;
            for (int r = 0; r < rings; ++r) {
                float t0 = static_cast<float>(r) / rings;
                float t1 = static_cast<float>(r + 1) / rings;
                for (size_t i = 0; i < n; ++i) {
                    size_t next = (i + 1) % n;
                    Vec2 a0{cx + (overPoly[i].x - cx) * t0, cz + (overPoly[i].y - cz) * t0};
                    Vec2 b0{cx + (overPoly[next].x - cx) * t0, cz + (overPoly[next].y - cz) * t0};
                    Vec2 b1{cx + (overPoly[next].x - cx) * t1, cz + (overPoly[next].y - cz) * t1};
                    Vec2 a1{cx + (overPoly[i].x - cx) * t1, cz + (overPoly[i].y - cz) * t1};
                    if (r == 0) {
                        appendUpwardTri3D(vertices, indices,
                            Vec3{a0.x, sailY(a0.x, a0.y), a0.y},
                            Vec3{a1.x, sailY(a1.x, a1.y), a1.y},
                            Vec3{b1.x, sailY(b1.x, b1.y), b1.y},
                            sailMat, true, metalMat, 0.08f);
                    } else {
                        appendUpwardTri3D(vertices, indices,
                            Vec3{a0.x, sailY(a0.x, a0.y), a0.y},
                            Vec3{a1.x, sailY(a1.x, a1.y), a1.y},
                            Vec3{b1.x, sailY(b1.x, b1.y), b1.y},
                            sailMat, true, metalMat, 0.08f);
                        appendUpwardTri3D(vertices, indices,
                            Vec3{a0.x, sailY(a0.x, a0.y), a0.y},
                            Vec3{b1.x, sailY(b1.x, b1.y), b1.y},
                            Vec3{b0.x, sailY(b0.x, b0.y), b0.y},
                            sailMat, true, metalMat, 0.08f);
                    }
                }
            }
            break;
        }

        case RoofType::TieredSteppedZigguratCrown: {
            uint32_t stoneMat = static_cast<uint32_t>(MaterialSlot::CorniceStone);
            uint32_t goldMat = static_cast<uint32_t>(MaterialSlot::GlassGold);
            float tierH = roofHeight * 0.28f;
            size_t n = basePoly.size();
            for (int t = 0; t < 3; ++t) {
                float sc = 0.88f - t * 0.22f;
                float th0 = baseTopY + t * tierH;
                float th1 = th0 + tierH;
                std::vector<Vec2> tierPoly = scaleContour(basePoly, m.centroid, sc);
                for (size_t i = 0; i < n; ++i) {
                    size_t next = (i + 1) % n;
                    Vec3 norm = Vec3{-(tierPoly[next].y - tierPoly[i].y), 0.0f, (tierPoly[next].x - tierPoly[i].x)}.normalized();
                    appendQuad(vertices, indices,
                        Vec3{tierPoly[i].x, th0, tierPoly[i].y}, Vec3{tierPoly[next].x, th0, tierPoly[next].y},
                        Vec3{tierPoly[next].x, th1, tierPoly[next].y}, Vec3{tierPoly[i].x, th1, tierPoly[i].y},
                        norm, stoneMat);
                }
                std::vector<Tri2D> topTris = triangulatePolygonXZ(tierPoly);
                for (const auto& tri : topTris) {
                    appendTri3D(vertices, indices,
                        Vec3{tri.a.x, th1, tri.a.y}, Vec3{tri.b.x, th1, tri.b.y}, Vec3{tri.c.x, th1, tri.c.y},
                        Vec3{0.0f, 1.0f, 0.0f}, stoneMat);
                }
                // Illuminated Art Deco vertical corner flutes at key contour vertices
                int fluteCount = std::min<int>(static_cast<int>(n), 6);
                for (int fk = 0; fk < fluteCount; ++fk) {
                    const Vec2& fp = tierPoly[(fk * n) / fluteCount];
                    appendOrientedCylinder(vertices, indices,
                        Vec3{fp.x, th0 + 0.06f, fp.y}, Vec3{fp.x, th1 + 0.04f, fp.y},
                        0.12f, 0.12f, 10, goldMat, true, true);
                }
            }
            float crownTop = baseTopY + 3.0f * tierH;
            appendOrientedCylinder(vertices, indices,
                Vec3{cx, crownTop, cz}, Vec3{cx, crownTop + roofHeight * 0.65f, cz},
                std::min(0.28f, m.minInradius * 0.14f), 0.03f, 20, metalMat, true, true);
            break;
        }

        default:
            break;
    }
}

void BuildingComponents::generateRingRoof(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    float outerRadius,
    float innerRadius,
    int32_t segments,
    float baseTopY,
    RoofType type,
    float roofHeight,
    float overhang,
    bool /*showClutter*/,
    uint32_t wallMaterialId
) {
    if (type == RoofType::NoneFlat) return;
    if (segments < 12) segments = 12;
    float rOut = outerRadius;
    float rIn = std::min(innerRadius, outerRadius * 0.85f);
    float rMid = (rOut + rIn) * 0.5f;
    float bandHalfW = (rOut - rIn) * 0.5f;

    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t wallMat = wallMaterialId;
    uint32_t metalMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t turfMat = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);

    float angleStep = (2.0f * kPi) / segments;
    float effOv = (type == RoofType::FlatParapet || type == RoofType::GreenRoofTerrace)
        ? 0.0f
        : std::clamp(overhang, 0.0f, bandHalfW * 0.35f);

    if (type == RoofType::FlatParapet || type == RoofType::GreenRoofTerrace) {
        float paraH = (type == RoofType::GreenRoofTerrace) ? 0.90f : 0.85f;
        float topY = baseTopY + paraH;
        float deckY = baseTopY + 0.12f;
        float rimT = std::min(0.26f, bandHalfW * 0.25f);
        uint32_t outerMat = (type == RoofType::GreenRoofTerrace) ? wallMat : wallMat;
        uint32_t deckMat = (type == RoofType::GreenRoofTerrace) ? turfMat : roofMat;

        for (int i = 0; i < segments; ++i) {
            float a0 = i * angleStep;
            float a1 = (i + 1) * angleStep;
            float c0 = std::cos(a0), s0 = std::sin(a0);
            float c1 = std::cos(a1), s1 = std::sin(a1);
            Vec3 nOut = Vec3{(c0 + c1) * 0.5f, 0.0f, (s0 + s1) * 0.5f}.normalized();
            // Outer parapet face & coping
            appendQuad(vertices, indices,
                {c0 * rOut, baseTopY, s0 * rOut}, {c1 * rOut, baseTopY, s1 * rOut},
                {c1 * rOut, topY, s1 * rOut}, {c0 * rOut, topY, s0 * rOut},
                nOut, outerMat);
            appendQuad(vertices, indices,
                {c0 * rOut, topY, s0 * rOut}, {c1 * rOut, topY, s1 * rOut},
                {c1 * (rOut - rimT), topY, s1 * (rOut - rimT)}, {c0 * (rOut - rimT), topY, s0 * (rOut - rimT)},
                {0, 1, 0}, roofMat);
            // Inner atrium parapet face & coping
            appendQuad(vertices, indices,
                {c1 * rIn, baseTopY, s1 * rIn}, {c0 * rIn, baseTopY, s0 * rIn},
                {c0 * rIn, topY, s0 * rIn}, {c1 * rIn, topY, s1 * rIn},
                -nOut, outerMat);
            appendQuad(vertices, indices,
                {c1 * rIn, topY, s1 * rIn}, {c0 * rIn, topY, s0 * rIn},
                {c0 * (rIn + rimT), topY, s0 * (rIn + rimT)}, {c1 * (rIn + rimT), topY, s1 * (rIn + rimT)},
                {0, 1, 0}, roofMat);
            // Annular deck
            appendQuad(vertices, indices,
                {c0 * (rIn + rimT), deckY, s0 * (rIn + rimT)},
                {c0 * (rOut - rimT), deckY, s0 * (rOut - rimT)},
                {c1 * (rOut - rimT), deckY, s1 * (rOut - rimT)},
                {c1 * (rIn + rimT), deckY, s1 * (rIn + rimT)},
                {0, 1, 0}, deckMat);
        }
        return;
    }

    // All pitched/vaulted/complex annular roofs preserve the open courtyard hole [0 .. rIn]
    const int radSteps = (type == RoofType::Dome || type == RoofType::GeodesicSkylightCrown ||
                          type == RoofType::BarrelVault || type == RoofType::PagodaCurvedEaves ||
                          type == RoofType::DiagridSailCanopy) ? 6 : 2;
    float rStart = rIn - effOv;
    float rEnd = rOut + effOv;

    auto ringProfileY = [&](float u, float theta) -> float {
        // u in [0 (inner eave) .. 1 (outer eave)]
        switch (type) {
            case RoofType::ShedMonopitch:
                return baseTopY + roofHeight * u;
            case RoofType::Butterfly:
                return baseTopY + 0.18f + (roofHeight - 0.18f) * std::abs(u - 0.5f) * 2.0f;
            case RoofType::Dome:
            case RoofType::GeodesicSkylightCrown:
            case RoofType::BarrelVault:
                return baseTopY + roofHeight * std::sin(u * kPi);
            case RoofType::Mansard:
            case RoofType::DutchHipWithCupola:
            case RoofType::ConicalSpireLantern: {
                float d = std::abs(u - 0.5f) * 2.0f;
                return (d < 0.45f)
                    ? (baseTopY + roofHeight * 0.72f)
                    : (baseTopY + roofHeight * 0.72f * (1.0f - (d - 0.45f) / 0.55f));
            }
            case RoofType::DiagridSailCanopy: {
                float w = 0.5f + 0.5f * std::sin(theta * 4.0f) * std::sin(u * kPi);
                return baseTopY + roofHeight * (0.35f + 0.65f * w);
            }
            case RoofType::HyperbolicParaboloidShell: {
                float wave = std::cos(theta * 2.0f) * (u - 0.5f) * 2.0f;
                return baseTopY + roofHeight * (0.5f + 0.5f * wave);
            }
            case RoofType::TieredSteppedZigguratCrown: {
                float d = std::abs(u - 0.5f) * 2.0f;
                return baseTopY + roofHeight * (d < 0.33f ? 0.95f : (d < 0.66f ? 0.60f : 0.25f));
            }
            default: // Gable, Hip, PyramidalApex, Gambrel, Sawtooth, Pagoda, etc.
                return baseTopY + roofHeight * (1.0f - std::abs(u - 0.5f) * 2.0f);
        }
    };

    uint32_t surfMat = (type == RoofType::GeodesicSkylightCrown) ? glassMat : roofMat;
    for (int i = 0; i < segments; ++i) {
        float a0 = i * angleStep;
        float a1 = (i + 1) * angleStep;
        float c0 = std::cos(a0), s0 = std::sin(a0);
        float c1 = std::cos(a1), s1 = std::sin(a1);
        for (int rs = 0; rs < radSteps; ++rs) {
            float u0 = static_cast<float>(rs) / radSteps;
            float u1 = static_cast<float>(rs + 1) / radSteps;
            float rr0 = rStart + (rEnd - rStart) * u0;
            float rr1 = rStart + (rEnd - rStart) * u1;

            Vec3 p0{c0 * rr0, ringProfileY(u0, a0), s0 * rr0};
            Vec3 p1{c0 * rr1, ringProfileY(u1, a0), s0 * rr1};
            Vec3 p2{c1 * rr1, ringProfileY(u1, a1), s1 * rr1};
            Vec3 p3{c1 * rr0, ringProfileY(u0, a1), s1 * rr0};
            appendUpwardTri3D(vertices, indices, p0, p1, p2, surfMat, true, metalMat, 0.10f);
            appendUpwardTri3D(vertices, indices, p0, p2, p3, surfMat, true, metalMat, 0.10f);
            if (type == RoofType::GeodesicSkylightCrown) {
                appendOrientedCylinder(vertices, indices, p0, p1, 0.04f, 0.04f, 5, metalMat, false, false);
                appendOrientedCylinder(vertices, indices, p1, p2, 0.04f, 0.04f, 5, metalMat, false, false);
            }
        }
        // Seal vertical inner & outer eave risers if roof starts above baseTopY (Shed / Butterfly / Hypar)
        float yOuter = ringProfileY(1.0f, a0);
        float yInner = ringProfileY(0.0f, a0);
        Vec3 nOut = Vec3{(c0 + c1) * 0.5f, 0.0f, (s0 + s1) * 0.5f}.normalized();
        if (yOuter > baseTopY + 0.03f) {
            appendQuad(vertices, indices,
                {c0 * rOut, baseTopY, s0 * rOut}, {c1 * rOut, baseTopY, s1 * rOut},
                {c1 * rOut, ringProfileY(1.0f, a1), s1 * rOut}, {c0 * rOut, yOuter, s0 * rOut},
                nOut, wallMat);
        }
        if (yInner > baseTopY + 0.03f) {
            appendQuad(vertices, indices,
                {c1 * rIn, baseTopY, s1 * rIn}, {c0 * rIn, baseTopY, s0 * rIn},
                {c0 * rIn, yInner, s0 * rIn}, {c1 * rIn, ringProfileY(0.0f, a1), s1 * rIn},
                -nOut, wallMat);
        }
    }
    (void)rMid;
}

void BuildingComponents::generateBalcony(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& baseCenter,
    float width,
    float depth,
    const Vec3& normal,
    const Vec3& tangent,
    BalconyType /*type*/
) {
    Vec3 right = tangent.normalized();
    Vec3 up{0.0f, 1.0f, 0.0f};
    float hw = width * 0.5f;

    Vec3 b0 = baseCenter - right * hw;
    Vec3 b1 = baseCenter + right * hw;
    Vec3 b2 = b1 + normal * depth;
    Vec3 b3 = b0 + normal * depth;

    // Floor slab
    appendQuad(vertices, indices, b0, b1, b2, b3, up, static_cast<uint32_t>(MaterialSlot::Foundation));
    appendQuad(vertices, indices, b3, b2, b1, b0, -up, static_cast<uint32_t>(MaterialSlot::Foundation));

    // Glass Balustrade front railing
    float railH = 0.95f;
    appendQuad(vertices, indices, b3, b2, b2 + up * railH, b3 + up * railH, normal, static_cast<uint32_t>(MaterialSlot::BalconyGlass));

    // Side return railings
    appendQuad(vertices, indices, b0, b3, b3 + up * railH, b0 + up * railH, -right, static_cast<uint32_t>(MaterialSlot::BalconyGlass));
    appendQuad(vertices, indices, b2, b1, b1 + up * railH, b2 + up * railH, right, static_cast<uint32_t>(MaterialSlot::BalconyGlass));
}

void BuildingComponents::generateWindowShutters(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& center,
    float width,
    float height,
    const Vec3& normal,
    const Vec3& tangent
) {
    Vec3 right = tangent.normalized();
    Vec3 up{0.0f, 1.0f, 0.0f};
    float hw = width * 0.5f;
    float hh = height * 0.5f;
    float shutterW = hw * 0.45f;
    Vec3 offset = normal * 0.05f;

    // Left louvred shutter
    Vec3 sL0 = center - right * (hw + shutterW + 0.05f) - up * hh + offset;
    Vec3 sL1 = center - right * (hw + 0.05f) - up * hh + offset;
    Vec3 sL2 = center - right * (hw + 0.05f) + up * hh + offset;
    Vec3 sL3 = center - right * (hw + shutterW + 0.05f) + up * hh + offset;
    appendQuad(vertices, indices, sL0, sL1, sL2, sL3, normal, static_cast<uint32_t>(MaterialSlot::WoodShutters));

    // Right louvred shutter
    Vec3 sR0 = center + right * (hw + 0.05f) - up * hh + offset;
    Vec3 sR1 = center + right * (hw + shutterW + 0.05f) - up * hh + offset;
    Vec3 sR2 = center + right * (hw + shutterW + 0.05f) + up * hh + offset;
    Vec3 sR3 = center + right * (hw + 0.05f) + up * hh + offset;
    appendQuad(vertices, indices, sR0, sR1, sR2, sR3, normal, static_cast<uint32_t>(MaterialSlot::WoodShutters));
}

void BuildingComponents::generateGroundPillars(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& facadeStart,
    const Vec3& facadeEnd,
    float height,
    int32_t count,
    const Vec3& normal,
    uint32_t materialId
) {
    if (count <= 1) return;
    Vec3 dir = facadeEnd - facadeStart;
    float colSize = 0.35f;
    float colDepth = 0.5f;

    Vec3 upVec{0.0f, 1.0f, 0.0f};
    Vec3 tangVec = (dir.length() > 1e-4f) ? dir.normalized() : Vec3{1.0f, 0.0f, 0.0f};
    for (int i = 0; i < count; ++i) {
        float frac = static_cast<float>(i) / (count - 1);
        Vec3 base = facadeStart + dir * frac + normal * colDepth + Vec3{0.0f, height * 0.5f, 0.0f};
        appendOrientedBox(vertices, indices,
            base, tangVec, upVec, normal,
            colSize * 0.5f, height * 0.5f, colSize * 0.5f,
            materialId);
    }
}

void BuildingComponents::generateSunLouvres(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& windowTopCenter,
    float width,
    float projection,
    const Vec3& normal,
    const Vec3& tangent
) {
    Vec3 right = tangent.normalized();
    Vec3 up{0.0f, 1.0f, 0.0f};
    float hw = width * 0.5f + 0.1f;

    Vec3 l0 = windowTopCenter - right * hw;
    Vec3 l1 = windowTopCenter + right * hw;
    Vec3 l2 = l1 + normal * projection;
    Vec3 l3 = l0 + normal * projection;

    appendQuad(vertices, indices, l0, l1, l2, l3, up, static_cast<uint32_t>(MaterialSlot::FrameMetal));
    appendQuad(vertices, indices, l3, l2, l1, l0, -up, static_cast<uint32_t>(MaterialSlot::FrameMetal));
}

void BuildingComponents::generateFireEscape(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& startBottom,
    float totalHeight,
    int32_t floorCount,
    float floorHeight,
    const Vec3& normal,
    const Vec3& tangent
) {
    Vec3 right = tangent.normalized();
    Vec3 up{0.0f, 1.0f, 0.0f};
    float platW = 1.4f;
    float platD = 1.1f;
    uint32_t metalMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);

    for (int f = 1; f < floorCount; ++f) {
        Vec3 floorBase = startBottom + up * (f * floorHeight);
        Vec3 p0 = floorBase - right * (platW * 0.5f);
        Vec3 p1 = floorBase + right * (platW * 0.5f);
        Vec3 p2 = p1 + normal * platD;
        Vec3 p3 = p0 + normal * platD;

        appendQuad(vertices, indices, p0, p1, p2, p3, up, metalMat);
        // Railing
        appendQuad(vertices, indices, p3, p2, p2 + up * 0.9f, p3 + up * 0.9f, normal, metalMat);
    }
}

void BuildingComponents::generateCornice(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    float width,
    float depth,
    float topY,
    float projection
) {
    float hw = width * 0.5f + projection;
    float hd = depth * 0.5f + projection;
    float corniceH = 0.35f;

    appendBox(vertices, indices,
        {-hw, topY - corniceH, -hd},
        {hw, topY, hd},
        static_cast<uint32_t>(MaterialSlot::CorniceStone));
}

void BuildingComponents::generateCeilingPendantBulb(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& mountPos,
    float dropLength,
    float bulbRadius,
    bool isDaylight
) {
    uint32_t metalMat = static_cast<uint32_t>(MaterialSlot::LightFixtureMetal);
    uint32_t bulbMat = static_cast<uint32_t>(isDaylight ? MaterialSlot::LightBulbDaylight : MaterialSlot::LightBulbWarm);
    float stemLen = std::max(0.12f, dropLength);
    float r = std::max(0.08f, bulbRadius);

    // Ceiling canopy disc + drop cord/rod
    appendOrientedCylinder(vertices, indices,
        mountPos, {mountPos.x, mountPos.y - 0.05f, mountPos.z},
        r * 1.15f, r * 0.95f, 10, metalMat, true, true);
    appendOrientedCylinder(vertices, indices,
        {mountPos.x, mountPos.y - 0.05f, mountPos.z},
        {mountPos.x, mountPos.y - stemLen, mountPos.z},
        0.025f, 0.025f, 8, metalMat, false, true);

    // Flared metal luminaire reflector hood (lets 3D downward light cone escape cleanly)
    Vec3 hoodTop{mountPos.x, mountPos.y - stemLen + 0.08f, mountPos.z};
    Vec3 hoodBot{mountPos.x, mountPos.y - stemLen - 0.08f, mountPos.z};
    appendOrientedCylinder(vertices, indices,
        hoodTop, hoodBot,
        r * 0.65f, r * 1.85f, 12, metalMat, true, false);

    // Faceted 3D glowing bulb capsule protruding beneath hood
    Vec3 bulbTop{mountPos.x, mountPos.y - stemLen - 0.04f, mountPos.z};
    Vec3 bulbMid{mountPos.x, mountPos.y - stemLen - 0.04f - r * 0.9f, mountPos.z};
    Vec3 bulbTip{mountPos.x, mountPos.y - stemLen - 0.04f - r * 1.65f, mountPos.z};
    appendOrientedCylinder(vertices, indices, bulbTop, bulbMid, r * 0.72f, r, 10, bulbMat, true, false);
    appendOrientedCylinder(vertices, indices, bulbMid, bulbTip, r, r * 0.35f, 10, bulbMat, false, true);
}

void BuildingComponents::generateWallSconceLamp(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& wallPos,
    const Vec3& outwardNormal,
    bool isDaylight
) {
    uint32_t metalMat = static_cast<uint32_t>(MaterialSlot::LightFixtureMetal);
    uint32_t bulbMat = static_cast<uint32_t>(isDaylight ? MaterialSlot::LightBulbDaylight : MaterialSlot::LightBulbWarm);
    Vec3 N = (outwardNormal.length() > 1e-4f) ? outwardNormal.normalized() : Vec3{0.0f, 0.0f, 1.0f};
    Vec3 up{0.0f, 1.0f, 0.0f};
    Vec3 tang = up.cross(N);
    tang = (tang.length() > 1e-4f) ? tang.normalized() : Vec3{1.0f, 0.0f, 0.0f};

    // Wall backplate & bracket arm
    appendOrientedBox(vertices, indices, wallPos + N * 0.03f, tang, up, N, 0.09f, 0.14f, 0.03f, metalMat);
    appendOrientedCylinder(vertices, indices, wallPos + N * 0.04f, wallPos + N * 0.24f, 0.025f, 0.022f, 8, metalMat, true, true);

    // Cylindrical top hood + glowing 3D bulb hanging below
    Vec3 lampPos = wallPos + N * 0.26f;
    appendOrientedCylinder(vertices, indices,
        lampPos + up * 0.08f, lampPos - up * 0.04f,
        0.10f, 0.14f, 10, metalMat, true, false);
    appendOrientedCylinder(vertices, indices,
        lampPos - up * 0.02f, lampPos - up * 0.16f,
        0.09f, 0.05f, 10, bulbMat, true, true);
}

void BuildingComponents::generateFloorStandingLamp(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const Vec3& floorPos,
    float height,
    bool isDaylight
) {
    uint32_t metalMat = static_cast<uint32_t>(MaterialSlot::LightFixtureMetal);
    uint32_t bulbMat = static_cast<uint32_t>(isDaylight ? MaterialSlot::LightBulbDaylight : MaterialSlot::LightBulbWarm);
    float h = std::max(1.1f, height);

    // Weighted circular floor plinth + tall brushed metal pole
    appendOrientedCylinder(vertices, indices,
        floorPos, {floorPos.x, floorPos.y + 0.05f, floorPos.z},
        0.24f, 0.20f, 12, metalMat, true, true);
    appendOrientedCylinder(vertices, indices,
        {floorPos.x, floorPos.y + 0.05f, floorPos.z},
        {floorPos.x, floorPos.y + h - 0.16f, floorPos.z},
        0.035f, 0.028f, 8, metalMat, false, true);

    // Open-bottom drum/conical luminaire collar + prominent visible 3D bulb globe
    Vec3 bulbCenter{floorPos.x, floorPos.y + h - 0.06f, floorPos.z};
    appendOrientedCylinder(vertices, indices,
        {bulbCenter.x, bulbCenter.y + 0.16f, bulbCenter.z},
        {bulbCenter.x, bulbCenter.y + 0.04f, bulbCenter.z},
        0.20f, 0.28f, 12, metalMat, true, false);
    appendOrientedCylinder(vertices, indices,
        {bulbCenter.x, bulbCenter.y + 0.08f, bulbCenter.z},
        {bulbCenter.x, bulbCenter.y - 0.12f, bulbCenter.z},
        0.14f, 0.11f, 12, bulbMat, true, true);
}

} // namespace procedural::buildings
