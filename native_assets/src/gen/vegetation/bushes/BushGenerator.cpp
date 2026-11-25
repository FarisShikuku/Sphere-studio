// ============================================================================
// BushGenerator.cpp - Procedural 3D Shrub, Cane, Leaf & Accent Synthesizer
//
// Generates real botanically structured multi-stem 3D shrub architectures:
//   - Basal root crown & curved De Casteljau primary woody canes (order 0)
//   - Leonardo Pipe-Model bifurcating secondary boughs & twigs (order 1..3)
//   - Opposite/decussate, whorled, Fibonacci spiral, pinnate & rosette leaves
//     anchored directly onto parent branch segments in true 3D orientations
//   - Species-specific 3D floral, bract, cherry, thorn, fiddlehead, and
//     cactoid ribs/cladode/whip structures
// ============================================================================

#include "BushGenerator.h"
#include <algorithm>
#include <cmath>

namespace vegetation {

namespace {
constexpr float kPi = 3.14159265359f;
constexpr float kGoldenAngle = 2.3999632297f;

inline Vec3 safeNormalized(const Vec3& v, const Vec3& fallback = Vec3(0.0f, 1.0f, 0.0f)) {
    float len = v.length();
    if (len < 1e-5f) return fallback;
    return v * (1.0f / len);
}

inline Vec3 evalCubicBezier(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t) {
    float u = 1.0f - t;
    float uu = u * u;
    float tt = t * t;
    return p0 * (uu * u) + p1 * (3.0f * uu * t) + p2 * (3.0f * u * tt) + p3 * (tt * t);
}

inline void orthonormalBasisFromDir(const Vec3& dir, Vec3& outRight, Vec3& outUp) {
    Vec3 ref = (std::abs(dir.y) < 0.92f) ? Vec3(0.0f, 1.0f, 0.0f) : Vec3(1.0f, 0.0f, 0.0f);
    outRight = safeNormalized(cross(dir, ref), Vec3(1.0f, 0.0f, 0.0f));
    outUp = safeNormalized(cross(outRight, dir), Vec3(0.0f, 1.0f, 0.0f));
}
} // namespace

BushSynthesisParams getPredefinedBushParams(BushArchetype archetype) {
    BushSynthesisParams p;
    p.archetype = archetype;
    switch (archetype) {
        case BushArchetype::BOXWOOD_SPHERE:
            p.radiusMeters = 0.9f;
            p.heightMeters = 1.0f;
            p.stemBranchCount = 10;
            p.leafClusterDensity = 64;
            p.leafDroopFactor = 0.1f;
            p.leafSizeMeters = 0.11f;
            p.colorR = 0.18f; p.colorG = 0.42f; p.colorB = 0.19f;
            break;
        case BushArchetype::WILD_BRAMBLE:
            p.radiusMeters = 1.6f;
            p.heightMeters = 1.2f;
            p.stemBranchCount = 14;
            p.leafClusterDensity = 50;
            p.leafDroopFactor = 0.45f;
            p.leafSizeMeters = 0.16f;
            p.colorR = 0.25f; p.colorG = 0.46f; p.colorB = 0.16f;
            break;
        case BushArchetype::FLOWERING_HYDRANGEA:
            p.radiusMeters = 1.3f;
            p.heightMeters = 1.3f;
            p.stemBranchCount = 12;
            p.leafClusterDensity = 52;
            p.leafDroopFactor = 0.20f;
            p.leafSizeMeters = 0.22f;
            p.colorR = 0.22f; p.colorG = 0.56f; p.colorB = 0.24f;
            p.accentColorR = 0.26f; p.accentColorG = 0.65f; p.accentColorB = 0.96f;
            break;
        case BushArchetype::MOUNTAIN_JUNIPER:
            p.radiusMeters = 1.4f;
            p.heightMeters = 0.8f;
            p.stemBranchCount = 8;
            p.leafClusterDensity = 40;
            p.leafDroopFactor = 0.15f;
            p.leafSizeMeters = 0.14f;
            p.colorR = 0.16f; p.colorG = 0.36f; p.colorB = 0.26f;
            break;
        case BushArchetype::DESERT_SAGEBRUSH:
            p.radiusMeters = 1.1f;
            p.heightMeters = 0.95f;
            p.stemBranchCount = 9;
            p.leafClusterDensity = 38;
            p.leafDroopFactor = 0.25f;
            p.leafSizeMeters = 0.12f;
            p.colorR = 0.50f; p.colorG = 0.56f; p.colorB = 0.48f;
            break;
        case BushArchetype::HIGHLAND_TEA_BUSH:
            p.radiusMeters = 1.35f;
            p.heightMeters = 1.1f;
            p.stemBranchCount = 12;
            p.leafClusterDensity = 54;
            p.leafDroopFactor = 0.05f;
            p.leafSizeMeters = 0.15f;
            p.colorR = 0.12f; p.colorG = 0.42f; p.colorB = 0.15f;
            p.accentColorR = 0.46f; p.accentColorG = 0.82f; p.accentColorB = 0.46f;
            break;
        case BushArchetype::ARABICA_COFFEE_SHRUB:
            p.radiusMeters = 1.45f;
            p.heightMeters = 1.55f;
            p.stemBranchCount = 10;
            p.leafClusterDensity = 46;
            p.leafDroopFactor = 0.22f;
            p.leafSizeMeters = 0.21f;
            p.colorR = 0.08f; p.colorG = 0.32f; p.colorB = 0.08f;
            p.accentColorR = 0.83f; p.accentColorG = 0.18f; p.accentColorB = 0.18f;
            break;
        case BushArchetype::THORNY_BOUGAINVILLEA:
            p.radiusMeters = 1.7f;
            p.heightMeters = 1.6f;
            p.stemBranchCount = 14;
            p.leafClusterDensity = 50;
            p.leafDroopFactor = 0.40f;
            p.leafSizeMeters = 0.16f;
            p.colorR = 0.18f; p.colorG = 0.49f; p.colorB = 0.20f;
            p.accentColorR = 0.91f; p.accentColorG = 0.12f; p.accentColorB = 0.39f;
            break;
        case BushArchetype::TROPICAL_HIBISCUS:
            p.radiusMeters = 1.3f;
            p.heightMeters = 1.45f;
            p.stemBranchCount = 10;
            p.leafClusterDensity = 42;
            p.leafDroopFactor = 0.20f;
            p.leafSizeMeters = 0.18f;
            p.colorR = 0.15f; p.colorG = 0.38f; p.colorB = 0.16f;
            p.accentColorR = 1.0f; p.accentColorG = 0.09f; p.accentColorB = 0.27f;
            break;
        case BushArchetype::WILD_SAGE_LANTANA:
            p.radiusMeters = 1.4f;
            p.heightMeters = 1.25f;
            p.stemBranchCount = 12;
            p.leafClusterDensity = 48;
            p.leafDroopFactor = 0.28f;
            p.leafSizeMeters = 0.14f;
            p.colorR = 0.18f; p.colorG = 0.49f; p.colorB = 0.20f;
            p.accentColorR = 1.0f; p.accentColorG = 0.60f; p.accentColorB = 0.0f;
            break;
        case BushArchetype::COASTAL_ROSEMARY:
            p.radiusMeters = 1.15f;
            p.heightMeters = 0.95f;
            p.stemBranchCount = 16;
            p.leafClusterDensity = 60;
            p.leafDroopFactor = 0.12f;
            p.leafSizeMeters = 0.10f;
            p.colorR = 0.24f; p.colorG = 0.38f; p.colorB = 0.27f;
            p.accentColorR = 0.62f; p.accentColorG = 0.66f; p.accentColorB = 0.85f;
            break;
        case BushArchetype::SISAL_AGAVE_ROSETTE:
            p.radiusMeters = 1.5f;
            p.heightMeters = 1.35f;
            p.stemBranchCount = 18;
            p.leafClusterDensity = 24;
            p.leafDroopFactor = 0.30f;
            p.leafSizeMeters = 0.35f;
            p.colorR = 0.27f; p.colorG = 0.35f; p.colorB = 0.39f;
            p.accentColorR = 0.55f; p.accentColorG = 0.43f; p.accentColorB = 0.39f;
            break;
        case BushArchetype::KING_PROTEA:
            p.radiusMeters = 1.3f;
            p.heightMeters = 1.4f;
            p.stemBranchCount = 8;
            p.leafClusterDensity = 36;
            p.leafDroopFactor = 0.15f;
            p.leafSizeMeters = 0.21f;
            p.colorR = 0.20f; p.colorG = 0.41f; p.colorB = 0.12f;
            p.accentColorR = 0.94f; p.accentColorG = 0.38f; p.accentColorB = 0.57f;
            break;
        case BushArchetype::SAVANNA_RED_THORN:
            p.radiusMeters = 1.55f;
            p.heightMeters = 1.6f;
            p.stemBranchCount = 11;
            p.leafClusterDensity = 40;
            p.leafDroopFactor = 0.32f;
            p.leafSizeMeters = 0.18f;
            p.colorR = 0.33f; p.colorG = 0.55f; p.colorB = 0.18f;
            p.accentColorR = 1.0f; p.accentColorG = 0.92f; p.accentColorB = 0.23f;
            break;
        case BushArchetype::HIGHLAND_TREE_FERN:
            p.radiusMeters = 1.65f;
            p.heightMeters = 1.7f;
            p.stemBranchCount = 10;
            p.leafClusterDensity = 42;
            p.leafDroopFactor = 0.55f;
            p.leafSizeMeters = 0.38f;
            p.colorR = 0.18f; p.colorG = 0.49f; p.colorB = 0.20f;
            p.accentColorR = 0.51f; p.accentColorG = 0.78f; p.accentColorB = 0.52f;
            break;
        case BushArchetype::DESERT_SALTBUSH:
            p.radiusMeters = 1.35f;
            p.heightMeters = 0.9f;
            p.stemBranchCount = 14;
            p.leafClusterDensity = 45;
            p.leafDroopFactor = 0.15f;
            p.leafSizeMeters = 0.11f;
            p.colorR = 0.65f; p.colorG = 0.72f; p.colorB = 0.60f;
            p.accentColorR = 1.0f; p.accentColorG = 0.96f; p.accentColorB = 0.62f;
            break;
        case BushArchetype::SAGUARO_CACTUS:
            p.radiusMeters = 1.35f; p.heightMeters = 2.5f; p.stemBranchCount = 5; p.leafClusterDensity = 24;
            p.colorR = 0.16f; p.colorG = 0.35f; p.colorB = 0.16f;
            p.accentColorR = 1.0f; p.accentColorG = 0.98f; p.accentColorB = 0.77f;
            break;
        case BushArchetype::PRICKLY_PEAR_CACTUS:
            p.radiusMeters = 1.6f; p.heightMeters = 1.4f; p.stemBranchCount = 12; p.leafClusterDensity = 32;
            p.colorR = 0.22f; p.colorG = 0.56f; p.colorB = 0.24f;
            p.accentColorR = 0.85f; p.accentColorG = 0.11f; p.accentColorB = 0.38f;
            break;
        case BushArchetype::GOLDEN_BARREL_CACTUS:
            p.radiusMeters = 1.05f; p.heightMeters = 0.95f; p.stemBranchCount = 8; p.leafClusterDensity = 22;
            p.colorR = 0.18f; p.colorG = 0.49f; p.colorB = 0.20f;
            p.accentColorR = 1.0f; p.accentColorG = 0.84f; p.accentColorB = 0.31f;
            break;
        case BushArchetype::ORGAN_PIPE_CACTUS:
            p.radiusMeters = 1.55f; p.heightMeters = 2.15f; p.stemBranchCount = 14; p.leafClusterDensity = 26;
            p.colorR = 0.14f; p.colorG = 0.33f; p.colorB = 0.14f;
            p.accentColorR = 0.97f; p.accentColorG = 0.73f; p.accentColorB = 0.82f;
            break;
        case BushArchetype::JUMPING_CHOLLA:
            p.radiusMeters = 1.4f; p.heightMeters = 1.55f; p.stemBranchCount = 16; p.leafClusterDensity = 38;
            p.colorR = 0.33f; p.colorG = 0.55f; p.colorB = 0.18f;
            p.accentColorR = 0.92f; p.accentColorG = 0.94f; p.accentColorB = 0.95f;
            break;
        case BushArchetype::JOSHUA_TREE:
            p.radiusMeters = 1.5f; p.heightMeters = 2.3f; p.stemBranchCount = 9; p.leafClusterDensity = 30;
            p.colorR = 0.20f; p.colorG = 0.41f; p.colorB = 0.12f;
            p.accentColorR = 1.0f; p.accentColorG = 0.96f; p.accentColorB = 0.62f;
            break;
        case BushArchetype::OCOTILLO_DESERT_CORAL:
            p.radiusMeters = 1.75f; p.heightMeters = 2.4f; p.stemBranchCount = 18; p.leafClusterDensity = 44;
            p.colorR = 0.26f; p.colorG = 0.63f; p.colorB = 0.28f;
            p.accentColorR = 1.0f; p.accentColorG = 0.24f; p.accentColorB = 0.0f;
            break;
        case BushArchetype::FISHHOOK_BARREL_CACTUS:
            p.radiusMeters = 1.05f; p.heightMeters = 1.15f; p.stemBranchCount = 7; p.leafClusterDensity = 24;
            p.colorR = 0.11f; p.colorG = 0.37f; p.colorB = 0.13f;
            p.accentColorR = 1.0f; p.accentColorG = 0.44f; p.accentColorB = 0.26f;
            break;
        case BushArchetype::ALOE_TORCH_SUCCULENT:
            p.radiusMeters = 1.55f; p.heightMeters = 1.65f; p.stemBranchCount = 12; p.leafClusterDensity = 32;
            p.colorR = 0.15f; p.colorG = 0.65f; p.colorB = 0.60f;
            p.accentColorR = 1.0f; p.accentColorG = 0.24f; p.accentColorB = 0.0f;
            break;
        case BushArchetype::ZEBRA_HAWORTHIA:
            p.radiusMeters = 0.85f; p.heightMeters = 0.7f; p.stemBranchCount = 16; p.leafClusterDensity = 28;
            p.colorR = 0.08f; p.colorG = 0.32f; p.colorB = 0.08f;
            p.accentColorR = 0.92f; p.accentColorG = 0.94f; p.accentColorB = 0.95f;
            break;
    }
    return p;
}

BushGenerator::BushGenerator(const BushSynthesisParams& params)
    : m_params(params) {}

ShrubStructureResult BushGenerator::synthesizeShrubStructure(uint64_t seed) const {
    FastRng rng(seed);
    ShrubStructureResult res;

    const float radM = std::max(0.35f, m_params.radiusMeters);
    const float hM = std::max(0.35f, m_params.heightMeters);
    const int stemCount = std::clamp(m_params.stemBranchCount, 3, 28);
    const int density = std::clamp(m_params.leafClusterDensity, 12, 96);
    const float leafBaseSize = std::clamp(m_params.leafSizeMeters > 0.01f ? m_params.leafSizeMeters : 0.15f, 0.06f, 0.45f);
    const BushArchetype arch = m_params.archetype;

    auto addSegment = [&](const Vec3& a, const Vec3& b, float r0, float r1, int32_t order, float windW) -> int32_t {
        int32_t idx = static_cast<int32_t>(res.branches.size());
        res.branches.push_back({a, b, r0, r1, order, windW});
        if (order <= 1) res.caneCount++;
        else res.twigCount++;
        return idx;
    };

    auto addLeaf = [&](const Vec3& attach, const Vec3& dir, float sizeM, int32_t branchIdx, float tint) {
        res.leaves.push_back({attach, safeNormalized(dir), sizeM, branchIdx, std::clamp(tint, 0.0f, 1.0f)});
    };

    auto addAccent = [&](const Vec3& pos, const Vec3& dir, float sizeM, ShrubAccentKind kind, int32_t branchIdx) {
        if (!m_params.hasAccents || kind == ShrubAccentKind::NONE) return;
        res.accents.push_back({pos, safeNormalized(dir), sizeM, kind, branchIdx, rng.nextRange(0.0f, 6.28318f)});
    };

    // Helper: split cubic Bezier cane into N connected tapering cylindrical segments and return end branch indices/positions
    auto buildCurvedChain = [&](const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3,
                                float rStart, float rEnd, int steps, int32_t order,
                                std::vector<Vec3>& outPts, std::vector<int32_t>& outSegIdx,
                                std::vector<float>& outRadii) {
        outPts.clear();
        outSegIdx.clear();
        outRadii.clear();
        Vec3 prev = p0;
        outPts.push_back(p0);
        outRadii.push_back(rStart);
        for (int i = 1; i <= steps; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(steps);
            Vec3 cur = evalCubicBezier(p0, p1, p2, p3, t);
            float rb = rStart + (rEnd - rStart) * (static_cast<float>(i - 1) / steps);
            float rt = rStart + (rEnd - rStart) * t;
            float windW = std::clamp(cur.y / hM, 0.0f, 1.3f) * m_params.windFlexibility;
            int32_t sIdx = addSegment(prev, cur, rb, rt, order, windW);
            outSegIdx.push_back(sIdx);
            outPts.push_back(cur);
            outRadii.push_back(rt);
            prev = cur;
        }
    };

    // ------------------------------------------------------------------------
    // Specialized Desert & Succulent / Rosette Morphologies
    // ------------------------------------------------------------------------
    if (arch == BushArchetype::SISAL_AGAVE_ROSETTE || arch == BushArchetype::ZEBRA_HAWORTHIA ||
        arch == BushArchetype::ALOE_TORCH_SUCCULENT) {
        const bool isZebra = (arch == BushArchetype::ZEBRA_HAWORTHIA);
        const bool isAloe = (arch == BushArchetype::ALOE_TORCH_SUCCULENT);
        int rosetteHeads = isAloe ? std::clamp(stemCount / 3, 2, 5) : 1;

        for (int head = 0; head < rosetteHeads; ++head) {
            Vec3 headOrigin(0.0f, 0.0f, 0.0f);
            int32_t headParentSeg = -1;
            if (isAloe) {
                float hAng = (static_cast<float>(head) / rosetteHeads) * 2.0f * kPi + 0.35f;
                float hSpread = radM * rng.nextRange(0.26f, 0.48f);
                float hRise = hM * rng.nextRange(0.32f, 0.56f);
                Vec3 trunkMid(std::cos(hAng) * hSpread * 0.45f, hRise * 0.55f, std::sin(hAng) * hSpread * 0.45f);
                headOrigin = Vec3(std::cos(hAng) * hSpread, hRise, std::sin(hAng) * hSpread);
                addSegment(Vec3(0, 0.0f, 0), trunkMid, 0.075f * radM, 0.055f * radM, 0, 0.1f * m_params.windFlexibility);
                headParentSeg = addSegment(trunkMid, headOrigin, 0.055f * radM, 0.042f * radM, 1, 0.25f * m_params.windFlexibility);
            }

            int bladesPerHead = isAloe ? std::clamp((stemCount * 2) / rosetteHeads + 8, 12, 22) : std::clamp(stemCount + 8, 14, 34);
            float rScale = isAloe ? 0.58f : 1.0f;
            float hScale = isAloe ? 0.48f : 1.0f;

            for (int i = 0; i < bladesPerHead; ++i) {
                float tier = static_cast<float>(i) / static_cast<float>(std::max(1, bladesPerHead - 1));
                float phi = static_cast<float>(i) * kGoldenAngle + head * 1.1f;
                float cPhi = std::cos(phi);
                float sPhi = std::sin(phi);

                // Outer lower blades spread wide, inner central blades point steeply upright
                float pitchUp = 0.22f + tier * 1.05f;
                float len = radM * rScale * (1.05f - tier * 0.48f) * rng.nextRange(0.90f, 1.08f);
                float droopY = (1.0f - tier) * 0.12f * hM * hScale;

                Vec3 radial(cPhi, 0.0f, sPhi);
                Vec3 p0 = headOrigin + radial * 0.02f;
                Vec3 p1 = p0 + radial * (len * 0.34f * std::cos(pitchUp * 0.65f)) + Vec3(0, len * 0.36f * std::sin(pitchUp) + 0.04f, 0);
                Vec3 p2 = p0 + radial * (len * 0.72f * std::cos(pitchUp * 0.45f)) + Vec3(0, len * 0.68f * std::sin(pitchUp) - droopY * 0.4f, 0);
                Vec3 p3 = p0 + radial * (len * std::cos(pitchUp * 0.35f)) + Vec3(0, len * 0.88f * std::sin(pitchUp) - droopY, 0);

                float baseW = (isZebra ? 0.058f : 0.072f) * radM * rScale * (1.0f - 0.28f * tier);
                std::vector<Vec3> pts;
                std::vector<int32_t> segs;
                std::vector<float> radii;
                buildCurvedChain(p0, p1, p2, p3, baseW, baseW * 0.12f, 3, 2, pts, segs, radii);

                // Marginal tooth spinelets along channeled blade edges
                if (!isZebra) {
                    Vec3 tang(-sPhi, 0.0f, cPhi);
                    for (int k = 1; k <= 2; ++k) {
                        Vec3 midPt = pts[k];
                        addAccent(midPt + tang * radii[k], safeNormalized(radial * 0.6f + tang * 0.8f + Vec3(0, 0.25f, 0)), 0.045f, ShrubAccentKind::THORN_SPINE, segs[k - 1]);
                        addAccent(midPt - tang * radii[k], safeNormalized(radial * 0.6f - tang * 0.8f + Vec3(0, 0.25f, 0)), 0.045f, ShrubAccentKind::THORN_SPINE, segs[k - 1]);
                    }
                    // Dark terminal needle spine at tip
                    Vec3 tipDir = safeNormalized(p3 - pts[2], radial);
                    addAccent(p3, tipDir, 0.11f * radM, ShrubAccentKind::THORN_SPINE, segs.back());
                } else {
                    // Zebra Haworthia white ridge accents
                    for (int k = 1; k <= 2; ++k) {
                        addAccent(pts[k], safeNormalized(pts[k] - pts[k - 1]), baseW * 0.9f, ShrubAccentKind::WINGED_FRUIT, segs[k - 1]);
                    }
                }
            }

            if (isAloe && head < 3) {
                // Tall fiery conical flower raceme rising out of Aloe rosette crown
                Vec3 stalkBase = headOrigin + Vec3(0, 0.04f, 0);
                Vec3 stalkMid = stalkBase + Vec3(rng.nextRange(-0.08f, 0.08f), hM * 0.34f, rng.nextRange(-0.08f, 0.08f));
                Vec3 stalkTop = stalkMid + Vec3(0.02f, hM * 0.28f, 0.02f);
                addSegment(stalkBase, stalkMid, 0.022f, 0.016f, 1, 0.45f * m_params.windFlexibility);
                int32_t topSeg = addSegment(stalkMid, stalkTop, 0.016f, 0.010f, 2, 0.75f * m_params.windFlexibility);
                addAccent(stalkTop, Vec3(0, 1, 0), 0.22f * hM, ShrubAccentKind::TORCH_SPIKE, topSeg);
            }
        }
        return res;
    }

    if (arch == BushArchetype::HIGHLAND_TREE_FERN) {
        // Fibrous caudex trunklet rising from soil
        const float trunkH = hM * 0.44f;
        Vec3 c0(0, 0, 0);
        Vec3 c1(0.02f, trunkH * 0.52f, -0.01f);
        Vec3 c2(0.0f, trunkH, 0.0f);
        addSegment(c0, c1, 0.12f * radM, 0.095f * radM, 0, 0.08f * m_params.windFlexibility);
        int32_t crownSeg = addSegment(c1, c2, 0.095f * radM, 0.080f * radM, 0, 0.18f * m_params.windFlexibility);

        // Coiled fiddlehead crosiers in the center crown
        for (int f = 0; f < 3; ++f) {
            float fAng = f * 2.094f + 0.4f;
            Vec3 crozierDir = safeNormalized(Vec3(std::cos(fAng) * 0.35f, 1.0f, std::sin(fAng) * 0.35f));
            Vec3 fTip = c2 + crozierDir * (hM * 0.22f);
            int32_t cSeg = addSegment(c2, fTip, 0.024f, 0.012f, 1, 0.3f * m_params.windFlexibility);
            addAccent(fTip, crozierDir, 0.12f, ShrubAccentKind::FERN_FIDDLEHEAD, cSeg);
        }

        // Arching woody frond rachises arranged in two phyllotactic tiers, clothed in pinnate pinnae
        const int fronds = std::clamp(stemCount, 6, 18);
        for (int f = 0; f < fronds; ++f) {
            float tierFrac = (f % 2 == 0) ? 1.0f : 0.78f;
            float az = (static_cast<float>(f) / fronds) * 2.0f * kPi + rng.nextRange(-0.08f, 0.08f);
            Vec3 radial(std::cos(az), 0.0f, std::sin(az));
            Vec3 tang(-std::sin(az), 0.0f, std::cos(az));

            float reach = radM * (0.95f + 0.22f * tierFrac);
            float peakY = hM * (0.92f + 0.24f * tierFrac);
            float tipDroopY = trunkH * (0.38f + 0.30f * tierFrac) - m_params.leafDroopFactor * 0.15f;

            Vec3 p0 = c2;
            Vec3 p1 = c2 + radial * (reach * 0.28f) + Vec3(0, (peakY - trunkH) * 0.85f, 0);
            Vec3 p2 = c2 + radial * (reach * 0.72f) + Vec3(0, (peakY - trunkH) * 0.95f, 0);
            Vec3 p3 = radial * reach + Vec3(0, tipDroopY, 0);

            std::vector<Vec3> pts;
            std::vector<int32_t> segs;
            std::vector<float> radii;
            buildCurvedChain(p0, p1, p2, p3, 0.026f, 0.005f, 5, 1, pts, segs, radii);

            // Symmetric lateral pinnae leaves projecting along the 3D rachis curve, ending at tip
            for (int k = 1; k < static_cast<int>(pts.size()); ++k) {
                Vec3 segAxis = safeNormalized(pts[k] - pts[k - 1]);
                float frac = static_cast<float>(k) / static_cast<float>(pts.size() - 1);
                float pinnaLen = reach * (0.28f * std::sin(frac * kPi * 0.85f + 0.15f)) * (1.0f - 0.35f * frac * frac);
                Vec3 sideR = safeNormalized(cross(segAxis, Vec3(0, 1, 0)), tang);
                int subCount = (k == static_cast<int>(pts.size()) - 1) ? 3 : 2;
                for (int sub = 0; sub < subCount; ++sub) {
                    float st = (subCount == 3)
                        ? (sub == 0 ? 0.30f : (sub == 1 ? 0.72f : 0.985f))
                        : (sub == 0 ? 0.35f : 0.80f);
                    bool isApical = (k == static_cast<int>(pts.size()) - 1 && sub == 2);
                    Vec3 basePt = pts[k - 1] * (1.0f - st) + pts[k] * st;
                    if (isApical) {
                        addLeaf(basePt, safeNormalized(segAxis * 0.88f + Vec3(0, -0.08f, 0)), std::max(0.08f, reach * 0.10f), segs[k - 1], 0.95f);
                    } else {
                        Vec3 dirL = safeNormalized(sideR * 0.88f + segAxis * 0.38f + Vec3(0, -0.10f, 0));
                        Vec3 dirR = safeNormalized(sideR * -0.88f + segAxis * 0.38f + Vec3(0, -0.10f, 0));
                        addLeaf(basePt, dirL, std::max(0.11f, pinnaLen), segs[k - 1], 0.4f + 0.5f * frac);
                        addLeaf(basePt, dirR, std::max(0.11f, pinnaLen), segs[k - 1], 0.4f + 0.5f * frac);
                    }
                }
            }
        }
        (void)crownSeg;
        return res;
    }

    if (arch == BushArchetype::SAGUARO_CACTUS || arch == BushArchetype::ORGAN_PIPE_CACTUS ||
        arch == BushArchetype::GOLDEN_BARREL_CACTUS || arch == BushArchetype::FISHHOOK_BARREL_CACTUS ||
        arch == BushArchetype::PRICKLY_PEAR_CACTUS || arch == BushArchetype::JUMPING_CHOLLA ||
        arch == BushArchetype::JOSHUA_TREE || arch == BushArchetype::OCOTILLO_DESERT_CORAL) {
        if (arch == BushArchetype::SAGUARO_CACTUS || arch == BushArchetype::ORGAN_PIPE_CACTUS) {
            int pipes = (arch == BushArchetype::SAGUARO_CACTUS) ? 1 : std::clamp(stemCount, 6, 14);
            for (int p = 0; p < pipes; ++p) {
                float pAng = (static_cast<float>(p) / pipes) * 2.0f * kPi + 0.3f;
                float rOff = (pipes == 1) ? 0.0f : radM * rng.nextRange(0.22f, 0.48f);
                float pHeight = (pipes == 1) ? hM : hM * rng.nextRange(0.68f, 1.0f);
                Vec3 p0(0.0f, 0.0f, 0.0f);
                Vec3 p1(std::cos(pAng) * rOff * 0.65f, pHeight * 0.25f, std::sin(pAng) * rOff * 0.65f);
                Vec3 p2(std::cos(pAng) * rOff, pHeight * 0.65f, std::sin(pAng) * rOff);
                Vec3 p3(std::cos(pAng) * rOff * 1.05f, pHeight, std::sin(pAng) * rOff * 1.05f);
                float radStem = (pipes == 1) ? radM * 0.22f : radM * 0.095f;
                std::vector<Vec3> pts;
                std::vector<int32_t> segs;
                std::vector<float> radii;
                buildCurvedChain(p0, p1, p2, p3, radStem * 0.92f, radStem * 0.85f, 4, 0, pts, segs, radii);
                addAccent(p3 + Vec3(0, 0.03f, 0), Vec3(0, 1, 0), 0.16f, ShrubAccentKind::FLOWER_BLOSSOM, segs.back());

                // Add cactus ribs/spine clusters along stem
                for (int r = 0; r < 6; ++r) {
                    float ra = (static_cast<float>(r) / 6.0f) * 2.0f * kPi;
                    Vec3 radial(std::cos(ra), 0.0f, std::sin(ra));
                    for (int k = 1; k < static_cast<int>(pts.size()); ++k) {
                        addAccent(pts[k] + radial * radii[k], radial, 0.06f, ShrubAccentKind::THORN_SPINE, segs[k - 1]);
                    }
                }
                if (arch == BushArchetype::SAGUARO_CACTUS) {
                    int arms = std::clamp(stemCount - 1, 2, 5);
                    for (int a = 0; a < arms; ++a) {
                        float armAng = (static_cast<float>(a) / arms) * 2.0f * kPi + 0.5f;
                        int nodeIdx = 1 + (a % 2);
                        Vec3 baseA = pts[nodeIdx];
                        Vec3 elbow = baseA + Vec3(std::cos(armAng) * radM * 0.36f, -0.04f, std::sin(armAng) * radM * 0.36f);
                        Vec3 bend = elbow + Vec3(std::cos(armAng) * radM * 0.14f, hM * 0.14f, std::sin(armAng) * radM * 0.14f);
                        Vec3 tipA = bend + Vec3(0, hM * rng.nextRange(0.24f, 0.40f), 0);
                        std::vector<Vec3> aPts; std::vector<int32_t> aSegs; std::vector<float> aR;
                        buildCurvedChain(baseA, elbow, bend, tipA, radStem * 0.68f, radStem * 0.60f, 3, 1, aPts, aSegs, aR);
                        addAccent(tipA, Vec3(0, 1, 0), 0.13f, ShrubAccentKind::FLOWER_BLOSSOM, aSegs.back());
                    }
                }
            }
            return res;
        }
        if (arch == BushArchetype::GOLDEN_BARREL_CACTUS || arch == BushArchetype::FISHHOOK_BARREL_CACTUS) {
            int cluster = (arch == BushArchetype::GOLDEN_BARREL_CACTUS) ? 3 : 2;
            for (int c = 0; c < cluster; ++c) {
                float cScale = (c == 0) ? 1.0f : rng.nextRange(0.45f, 0.62f);
                float cAng = c * 2.1f + 0.4f;
                Vec3 base = (c == 0) ? Vec3(0, 0, 0) : Vec3(std::cos(cAng) * radM * 0.48f, 0, std::sin(cAng) * radM * 0.48f);
                float bh = hM * cScale;
                float br = radM * 0.38f * cScale;
                Vec3 p1 = base + Vec3(0, bh * 0.35f, 0);
                Vec3 p2 = base + Vec3(0, bh * 0.72f, 0);
                Vec3 p3 = base + Vec3(0, bh, 0);
                int32_t s0 = addSegment(base, p1, br * 0.78f, br, 0, 0.02f);
                int32_t s1 = addSegment(p1, p2, br, br * 0.92f, 0, 0.03f);
                int32_t s2 = addSegment(p2, p3, br * 0.92f, br * 0.48f, 0, 0.04f);
                for (int rib = 0; rib < 10; ++rib) {
                    float ra = (static_cast<float>(rib) / 10.0f) * 2.0f * kPi;
                    Vec3 radial(std::cos(ra), 0.15f, std::sin(ra));
                    addAccent(p1 + radial * br, radial, 0.11f * cScale, ShrubAccentKind::THORN_SPINE, s0);
                    addAccent(p2 + radial * (br * 0.92f), radial, 0.12f * cScale, ShrubAccentKind::THORN_SPINE, s1);
                }
                addAccent(p3, Vec3(0, 1, 0), 0.18f * cScale, ShrubAccentKind::FLOWER_BLOSSOM, s2);
            }
            return res;
        }
        if (arch == BushArchetype::PRICKLY_PEAR_CACTUS || arch == BushArchetype::JUMPING_CHOLLA) {
            const bool isPad = (arch == BushArchetype::PRICKLY_PEAR_CACTUS);
            int basalCount = isPad ? 3 : 4;
            for (int b = 0; b < basalCount; ++b) {
                float ba = (static_cast<float>(b) / basalCount) * 2.0f * kPi + 0.25f;
                Vec3 rDir(std::cos(ba), 0.0f, std::sin(ba));
                Vec3 p0(0, 0, 0);
                Vec3 p1 = rDir * (radM * 0.14f) + Vec3(0, hM * 0.26f, 0);
                float rPad = isPad ? radM * 0.11f : radM * 0.06f;
                int32_t seg1 = addSegment(p0, p1, rPad * 0.75f, rPad, 0, 0.1f * m_params.windFlexibility);
                for (int child = 0; child < 2; ++child) {
                    float ca = ba + (child == 0 ? -0.52f : 0.52f);
                    Vec3 cDir = safeNormalized(Vec3(std::cos(ca) * 0.65f, 1.0f, std::sin(ca) * 0.65f));
                    Vec3 p2 = p1 + cDir * (hM * 0.34f);
                    int32_t seg2 = addSegment(p1, p2, rPad * 0.85f, rPad * 0.82f, 1, 0.25f * m_params.windFlexibility);
                    addAccent((p1 + p2) * 0.5f, safeNormalized(Vec3(std::cos(ca), 0.2f, std::sin(ca))), 0.08f, ShrubAccentKind::THORN_SPINE, seg2);
                    for (int g = 0; g < 2; ++g) {
                        float ga = ca + (g == 0 ? -0.45f : 0.45f);
                        Vec3 gDir = safeNormalized(Vec3(std::cos(ga) * 0.75f, 0.9f, std::sin(ga) * 0.75f));
                        Vec3 p3 = p2 + gDir * (hM * 0.28f);
                        int32_t seg3 = addSegment(p2, p3, rPad * 0.72f, rPad * 0.65f, 2, 0.45f * m_params.windFlexibility);
                        addAccent(p3, gDir, 0.09f, isPad ? ShrubAccentKind::BERRIES : ShrubAccentKind::THORN_SPINE, seg3);
                    }
                }
                addAccent(p1, rDir, 0.06f, ShrubAccentKind::WINGED_FRUIT, seg1);
            }
            return res;
        }
        if (arch == BushArchetype::JOSHUA_TREE) {
            Vec3 t0(0, 0, 0), t1(0, hM * 0.28f, 0), t2(0.03f, hM * 0.52f, -0.02f);
            addSegment(t0, t1, radM * 0.13f, radM * 0.105f, 0, 0.05f * m_params.windFlexibility);
            addSegment(t1, t2, radM * 0.105f, radM * 0.088f, 0, 0.12f * m_params.windFlexibility);
            int limbs = std::clamp(stemCount / 2, 3, 6);
            for (int l = 0; l < limbs; ++l) {
                float la = (static_cast<float>(l) / limbs) * 2.0f * kPi + 0.4f;
                Vec3 dir1 = safeNormalized(Vec3(std::cos(la) * 0.65f, 0.85f, std::sin(la) * 0.65f));
                Vec3 mid = t2 + dir1 * (hM * 0.26f);
                addSegment(t2, mid, radM * 0.07f, radM * 0.055f, 1, 0.25f * m_params.windFlexibility);
                for (int f = 0; f < 2; ++f) {
                    float fa = la + (f == 0 ? -0.45f : 0.45f);
                    Vec3 dir2 = safeNormalized(Vec3(std::cos(fa) * 0.55f, 1.0f, std::sin(fa) * 0.55f));
                    Vec3 tip = mid + dir2 * (hM * rng.nextRange(0.18f, 0.28f));
                    int32_t tipSeg = addSegment(mid, tip, radM * 0.052f, radM * 0.038f, 2, 0.42f * m_params.windFlexibility);
                    for (int d = 0; d < 12; ++d) {
                        float da = d * kGoldenAngle;
                        float lift = -0.35f + (d / 12.0f) * 1.15f;
                        Vec3 bladeDir = safeNormalized(Vec3(std::cos(da), lift, std::sin(da)));
                        addLeaf(tip, bladeDir, leafBaseSize * 1.6f, tipSeg, 0.6f + 0.4f * (d / 12.0f));
                    }
                    addAccent(tip + dir2 * 0.06f, dir2, 0.16f, ShrubAccentKind::FLOWER_BLOSSOM, tipSeg);
                }
            }
            return res;
        }
        // OCOTILLO_DESERT_CORAL: vase fountain of arching whip canes with flame spikes
        int canes = std::clamp(stemCount, 8, 22);
        for (int c = 0; c < canes; ++c) {
            float ang = (static_cast<float>(c) / canes) * 2.0f * kPi + rng.nextRange(-0.12f, 0.12f);
            Vec3 radial(std::cos(ang), 0.0f, std::sin(ang));
            float reach = radM * rng.nextRange(0.75f, 1.12f);
            float heightC = hM * rng.nextRange(0.82f, 1.08f);
            Vec3 p0 = radial * (0.04f * radM);
            Vec3 p1 = radial * (reach * 0.25f) + Vec3(0, heightC * 0.35f, 0);
            Vec3 p2 = radial * (reach * 0.62f) + Vec3(0, heightC * 0.72f, 0);
            Vec3 p3 = radial * reach + Vec3(0, heightC, 0);
            std::vector<Vec3> pts; std::vector<int32_t> segs; std::vector<float> radii;
            buildCurvedChain(p0, p1, p2, p3, 0.022f, 0.005f, 4, 0, pts, segs, radii);
            for (int k = 1; k < static_cast<int>(pts.size()); ++k) {
                float u = static_cast<float>(k - 1) / static_cast<float>(std::max(1, static_cast<int>(pts.size()) - 2));
                float segT = (k == static_cast<int>(pts.size()) - 1) ? 0.96f : (0.25f + 0.65f * (1.0f - std::pow(1.0f - u, 1.8f)));
                Vec3 pos = pts[k - 1] * (1.0f - segT) + pts[k] * segT;
                float sizeScale = 1.05f - 0.42f * u;
                addLeaf(pos, safeNormalized(radial + Vec3(0, 0.4f, 0)), 0.068f * sizeScale, segs[k - 1], 0.4f + 0.5f * u);
                addAccent(pos, safeNormalized(radial * 0.9f + Vec3(0, 0.25f, 0)), 0.045f, ShrubAccentKind::THORN_SPINE, segs[k - 1]);
            }
            // Paired small apical leaves capping the cane end beneath the coral torch spike
            Vec3 endAxis = safeNormalized(p3 - pts[pts.size() - 2]);
            Vec3 endSide(-endAxis.z, 0.0f, endAxis.x);
            addLeaf(p3, safeNormalized(endAxis * 0.72f + endSide * 0.55f), 0.040f, segs.back(), 0.95f);
            addLeaf(p3, safeNormalized(endAxis * 0.72f - endSide * 0.55f), 0.040f, segs.back(), 0.95f);
            addAccent(p3, endAxis, 0.22f, ShrubAccentKind::TORCH_SPIKE, segs.back());
        }
        return res;
    }

    // ------------------------------------------------------------------------
    // General Multi-Stem Woody Shrub Architecture (Order 0 Root Crown -> Canes -> Boughs -> Twigs -> Phyllotaxis Leaves + Accents)
    // ------------------------------------------------------------------------
    const bool isTeaTabletop = (arch == BushArchetype::HIGHLAND_TEA_BUSH);
    const bool isBoxwoodGlobe = (arch == BushArchetype::BOXWOOD_SPHERE);
    const bool isCoffeePlagiotropic = (arch == BushArchetype::ARABICA_COFFEE_SHRUB);
    const bool isBougainvillea = (arch == BushArchetype::THORNY_BOUGAINVILLEA);
    const bool isHibiscus = (arch == BushArchetype::TROPICAL_HIBISCUS);
    const bool isLantana = (arch == BushArchetype::WILD_SAGE_LANTANA);
    const bool isRosemary = (arch == BushArchetype::COASTAL_ROSEMARY);
    const bool isProtea = (arch == BushArchetype::KING_PROTEA);
    const bool isSavannaThorn = (arch == BushArchetype::SAVANNA_RED_THORN);
    const bool isHydrangea = (arch == BushArchetype::FLOWERING_HYDRANGEA);
    const bool isSaltbush = (arch == BushArchetype::DESERT_SALTBUSH || arch == BushArchetype::DESERT_SAGEBRUSH);

    const ShrubAccentKind speciesAccent = [&]() -> ShrubAccentKind {
        if (isCoffeePlagiotropic) return ShrubAccentKind::BERRIES;
        if (isBougainvillea) return ShrubAccentKind::BOUGAINVILLEA_BRACT;
        if (isProtea) return ShrubAccentKind::PROTEA_CUP;
        if (isSavannaThorn) return ShrubAccentKind::ACACIA_PUFF;
        if (isHydrangea) return ShrubAccentKind::MOPHEAD_CORYMB;
        if (isHibiscus || isLantana || isRosemary) return ShrubAccentKind::FLOWER_BLOSSOM;
        if (arch == BushArchetype::DESERT_SALTBUSH) return ShrubAccentKind::WINGED_FRUIT;
        return ShrubAccentKind::NONE;
    }();

    // Central subterranean-to-basal root collar (keeps all canes anchored in soil)
    const Vec3 soilCenter(0.0f, 0.0f, 0.0f);
    const float rootCollarH = isCoffeePlagiotropic ? hM * 0.14f : hM * 0.07f;
    int32_t rootSeg = addSegment(
        Vec3(0.0f, 0.0f, 0.0f),
        Vec3(0.0f, rootCollarH, 0.0f),
        std::clamp(radM * 0.065f, 0.032f, 0.09f),
        std::clamp(radM * 0.052f, 0.026f, 0.075f),
        0,
        0.0f
    );
    (void)rootSeg;

    // Collect terminal twigs for proportional foliage/accent filling
    struct TwigAttachSite {
        Vec3 startPt;
        Vec3 endPt;
        Vec3 dir;
        int32_t branchIdx;
        float heightNorm;
        bool isTerminal;
    };
    std::vector<TwigAttachSite> foliarTwigs;

    const float baseCaneRadius = std::clamp(radM * (isProtea ? 0.036f : (isRosemary ? 0.020f : 0.028f)), 0.015f, 0.055f);
    const float leavesPerTwigTarget = std::max(2.0f, static_cast<float>(density) * 0.14f);

    if (isCoffeePlagiotropic) {
        // Coffea arabica: 2-4 vertical orthotropic trunks with tiered opposite horizontal plagiotropic side shoots
        int trunks = std::clamp(stemCount / 3, 2, 4);
        for (int t = 0; t < trunks; ++t) {
            float tAng = (static_cast<float>(t) / trunks) * 2.0f * kPi + 0.2f;
            float tOff = (trunks == 1) ? 0.0f : radM * 0.10f;
            Vec3 p0(0.0f, rootCollarH, 0.0f);
            Vec3 p1(std::cos(tAng) * tOff * 0.7f, hM * 0.36f, std::sin(tAng) * tOff * 0.7f);
            Vec3 p2(std::cos(tAng) * tOff * 0.9f, hM * 0.68f, std::sin(tAng) * tOff * 0.9f);
            Vec3 p3(std::cos(tAng) * tOff * 1.1f, hM * 0.96f, std::sin(tAng) * tOff * 1.1f);
            std::vector<Vec3> mPts; std::vector<int32_t> mSegs; std::vector<float> mR;
            buildCurvedChain(p0, p1, p2, p3, baseCaneRadius * 1.1f, baseCaneRadius * 0.25f, 5, 0, mPts, mSegs, mR);

            // Spawn paired opposite horizontal plagiotropic lateral boughs along each orthotropic node
            for (int tier = 1; tier <= 4; ++tier) {
                Vec3 nodePos = mPts[tier];
                float nodeFrac = static_cast<float>(tier) / 5.0f;
                float latReach = radM * (0.95f - 0.38f * nodeFrac) * rng.nextRange(0.88f, 1.08f);
                float baseTierAng = tAng + tier * 1.5707963f; // Decussate 90-deg twist per node!
                for (int pair = 0; pair < 2; ++pair) {
                    float bAng = baseTierAng + pair * kPi + rng.nextRange(-0.18f, 0.18f);
                    Vec3 bDir(std::cos(bAng), 0.0f, std::sin(bAng));
                    Vec3 bp0 = nodePos;
                    Vec3 bp1 = nodePos + bDir * (latReach * 0.35f) + Vec3(0, 0.08f * hM, 0);
                    Vec3 bp2 = nodePos + bDir * (latReach * 0.72f) + Vec3(0, 0.04f * hM, 0);
                    Vec3 bp3 = nodePos + bDir * latReach + Vec3(0, -0.10f * hM, 0);
                    std::vector<Vec3> bPts; std::vector<int32_t> bSegs; std::vector<float> bR;
                    buildCurvedChain(bp0, bp1, bp2, bp3, mR[tier] * 0.62f, 0.005f, 3, 1, bPts, bSegs, bR);
                    for (int s = 0; s < static_cast<int>(bSegs.size()); ++s) {
                        Vec3 sDir = safeNormalized(bPts[s + 1] - bPts[s]);
                        foliarTwigs.push_back({bPts[s], bPts[s + 1], sDir, bSegs[s], std::clamp(bPts[s + 1].y / hM, 0.1f, 1.0f), s == static_cast<int>(bSegs.size()) - 1});
                        // Tight opposite red Coffee Cherry pairs right at the plagiotropic bough nodes
                        addAccent(bPts[s + 1], sDir, 0.042f, ShrubAccentKind::BERRIES, bSegs[s]);
                    }
                }
            }
            // Apical tip leaves on orthotropic crown
            foliarTwigs.push_back({mPts[4], mPts[5], safeNormalized(mPts[5] - mPts[4]), mSegs.back(), 1.0f, true});
        }
    } else {
        // Multi-stem basal canes radiating smoothly via cubic De Casteljau splines
        for (int s = 0; s < stemCount; ++s) {
            float ringFrac = (stemCount > 6 && s < stemCount / 3) ? 0.45f : 1.0f; // inner core canes vs outer rim canes
            float caneAz = (static_cast<float>(s) / stemCount) * 2.0f * kPi + rng.nextRange(-0.22f, 0.22f);
            Vec3 radial(std::cos(caneAz), 0.0f, std::sin(caneAz));
            Vec3 tang(-std::sin(caneAz), 0.0f, std::cos(caneAz));

            float reach = radM * ringFrac * rng.nextRange(0.72f, 1.04f);
            float caneH = hM * rng.nextRange(0.84f, 1.05f);
            if (isTeaTabletop) {
                caneH = hM * rng.nextRange(0.92f, 1.02f); // Flattened plucked tabletop horizon!
            } else if (isBoxwoodGlobe) {
                caneH = hM * (ringFrac < 0.7f ? 0.98f : 0.72f);
            } else if (isRosemary) {
                reach *= 0.68f;
                caneH = hM * rng.nextRange(0.80f, 1.08f);
            }

            Vec3 p0 = Vec3(0.0f, rootCollarH, 0.0f) + radial * (0.02f * radM);
            Vec3 p1 = p0 + radial * (reach * (isBougainvillea ? 0.24f : 0.34f)) + Vec3(0, caneH * 0.45f, 0);
            Vec3 p2 = p0 + radial * (reach * (isBougainvillea ? 0.62f : 0.68f)) + tang * rng.nextRange(-0.12f, 0.12f) * radM
                      + Vec3(0, caneH * (isBougainvillea ? 1.12f : 0.88f), 0);
            Vec3 p3 = radial * (reach * (isBougainvillea ? 1.18f : 0.94f))
                      + Vec3(0, caneH * (isBougainvillea ? 0.68f : (isTeaTabletop ? 0.96f : 0.90f)), 0);

            int caneSteps = (isSavannaThorn || isSaltbush) ? 4 : 3;
            std::vector<Vec3> pts;
            std::vector<int32_t> segs;
            std::vector<float> radii;
            buildCurvedChain(p0, p1, p2, p3, baseCaneRadius, baseCaneRadius * 0.42f, caneSteps, 0, pts, segs, radii);

            // Spawn secondary Leonardo Pipe-Model bifurcations from intermediate cane joints
            for (int k = 1; k <= caneSteps; ++k) {
                Vec3 joint = pts[k];
                Vec3 caneTan = safeNormalized(pts[k] - pts[k - 1]);
                float caneRadAtK = radii[k];
                int branchesHere = (k == caneSteps) ? 2 : (isRosemary ? 2 : 1);

                // Da Vinci Pipe Model child radius conservation (eta ~ 2.0 -> rChild = rParent / sqrt(n))
                float childR0 = std::max(0.0045f, caneRadAtK * (k == caneSteps ? 0.68f : 0.58f));

                for (int b = 0; b < branchesHere; ++b) {
                    float sideSign = (b % 2 == 0 ? 1.0f : -1.0f) * ((k + s) % 2 == 0 ? 1.0f : -1.0f);
                    float yawSpread = rng.nextRange(0.32f, 0.72f) * sideSign;
                    float pitchLift = isTeaTabletop ? rng.nextRange(0.18f, 0.48f)
                                    : (isBougainvillea ? rng.nextRange(-0.18f, 0.35f) : rng.nextRange(0.24f, 0.62f));

                    Vec3 outDir = safeNormalized(caneTan * 0.78f + tang * std::sin(yawSpread) * 0.72f + radial * 0.35f + Vec3(0, pitchLift, 0));
                    float twigLen = radM * rng.nextRange(0.24f, 0.46f) * (k == caneSteps ? 0.82f : 1.0f);
                    if (isProtea) twigLen *= 0.65f;

                    Vec3 bMid = joint + outDir * (twigLen * 0.52f) + Vec3(0, isSavannaThorn ? 0.05f : 0.02f, 0);
                    Vec3 bEnd = joint + outDir * twigLen + tang * (sideSign * 0.06f * radM) + Vec3(0, isBougainvillea ? -0.08f : 0.04f, 0);

                    int32_t bSeg1 = addSegment(joint, bMid, childR0, childR0 * 0.68f, 1, std::clamp(bMid.y / hM, 0.05f, 1.2f) * m_params.windFlexibility);
                    int32_t bSeg2 = addSegment(bMid, bEnd, childR0 * 0.68f, childR0 * 0.36f, 2, std::clamp(bEnd.y / hM, 0.05f, 1.3f) * m_params.windFlexibility);

                    foliarTwigs.push_back({joint, bMid, safeNormalized(bMid - joint), bSeg1, std::clamp(bMid.y / hM, 0.1f, 1.0f), false});
                    foliarTwigs.push_back({bMid, bEnd, safeNormalized(bEnd - bMid), bSeg2, std::clamp(bEnd.y / hM, 0.1f, 1.0f), true});

                    // Paired thorns at cane/twig nodes for Savanna Thorn & Bougainvillea
                    if (isSavannaThorn || isBougainvillea || arch == BushArchetype::WILD_BRAMBLE) {
                        addAccent(bMid, safeNormalized(tang * sideSign + Vec3(0, 0.35f, 0)), 0.055f, ShrubAccentKind::THORN_SPINE, bSeg1);
                    }

                    // Order 3 forked fine terminal shootlets for dense shrubs (Tea, Boxwood, Lantana, Saltbush)
                    if ((isTeaTabletop || isBoxwoodGlobe || isLantana || isSaltbush || isHibiscus) && k >= 2) {
                        for (int tf = 0; tf < 2; ++tf) {
                            float fSign = (tf == 0) ? -1.0f : 1.0f;
                            Vec3 shootDir = safeNormalized(outDir * 0.72f + tang * (fSign * 0.55f) + Vec3(0, isTeaTabletop ? 0.25f : 0.42f, 0));
                            float shootLen = twigLen * rng.nextRange(0.42f, 0.66f);
                            Vec3 sTip = bEnd + shootDir * shootLen;
                            if (isTeaTabletop && sTip.y > hM * 1.04f) {
                                sTip.y = hM * 1.04f; // Contoured tea table canopy!
                            }
                            int32_t tSeg = addSegment(bEnd, sTip, childR0 * 0.34f, 0.0028f, 3, std::clamp(sTip.y / hM, 0.1f, 1.4f) * m_params.windFlexibility);
                            foliarTwigs.push_back({bEnd, sTip, safeNormalized(sTip - bEnd), tSeg, std::clamp(sTip.y / hM, 0.1f, 1.0f), true});
                        }
                    }
                }
            }
        }
    }

    // ------------------------------------------------------------------------
    // Populate True 3D Botanical Leaves & Accents Anchored to Woody Twigs
    // ------------------------------------------------------------------------
    const int targetLeafTotal = std::clamp(density * 4, 60, 420);
    const int twigCount = std::max(1, static_cast<int>(foliarTwigs.size()));
    const bool isOppositePhyllotaxis = isCoffeePlagiotropic || isBoxwoodGlobe || isLantana || isHydrangea || isRosemary || isSavannaThorn;

    int leavesPlaced = 0;
    for (int ti = 0; ti < static_cast<int>(foliarTwigs.size()); ++ti) {
        const auto& tw = foliarTwigs[ti];
        Vec3 rightB, upB;
        orthonormalBasisFromDir(tw.dir, rightB, upB);

        int countOnTwig = std::max(3, (targetLeafTotal + twigCount - 1) / twigCount);
        if (tw.isTerminal) countOnTwig = std::max(countOnTwig + 1, isOppositePhyllotaxis ? 4 : 3);
        if (isRosemary) countOnTwig = std::max(5, countOnTwig + 2);

        if (tw.isTerminal && isOppositePhyllotaxis) {
            // Opposite / decussate shrub stems: closer pairs toward tip, ending with 2 terminal leaves at tw.endPt
            int nodePairs = std::clamp((countOnTwig + 1) / 2, 2, 5);
            float basePhi = static_cast<float>(ti) * 1.15f;
            for (int np = 0; np < nodePairs; ++np) {
                bool isTip = (np == nodePairs - 1);
                float u = static_cast<float>(np) / static_cast<float>(std::max(1, nodePairs - 1));
                float frac = isTip ? 0.985f : (0.18f + 0.79f * (1.0f - std::pow(1.0f - u, 2.05f)));
                Vec3 attach = tw.startPt * (1.0f - frac) + tw.endPt * frac;

                float taperT = std::clamp((frac - 0.22f) / 0.765f, 0.0f, 1.0f);
                float smoothT = taperT * taperT * (3.0f - 2.0f * taperT);
                float sizeMul = (1.08f - 0.50f * smoothT) * rng.nextRange(0.93f, 1.07f);
                float twist = static_cast<float>(np) * (kPi * 0.5f);
                float sideTilt = isTip ? (isRosemary ? 0.42f : 0.60f) : (isRosemary ? 0.52f : 0.84f);
                float droop = -m_params.leafDroopFactor * 0.40f + (tw.heightNorm * 0.20f);

                for (int pairSide = 0; pairSide < 2; ++pairSide) {
                    float phi = basePhi + twist + (pairSide == 0 ? 0.0f : kPi);
                    Vec3 radialLeaf = rightB * std::cos(phi) + upB * std::sin(phi);
                    float fwdWeight = isTip ? 0.72f : (1.0f - sideTilt * 0.45f);
                    Vec3 leafDir = safeNormalized(tw.dir * fwdWeight + radialLeaf * sideTilt + Vec3(0.0f, droop, 0.0f));
                    addLeaf(attach, leafDir, leafBaseSize * sizeMul, tw.branchIdx, std::clamp(tw.heightNorm + (isTip ? 0.14f : 0.0f), 0.1f, 1.0f));
                    leavesPlaced++;
                }
            }
        } else if (tw.isTerminal) {
            // Alternate / spiral shrub stems: compressed acropetal spacing toward tip, ending in 1 apical leaf
            int nodeCount = std::clamp(countOnTwig, 3, 7);
            float basePhi = static_cast<float>(leavesPlaced) * kGoldenAngle + ti * 0.618f;
            for (int li = 0; li < nodeCount; ++li) {
                bool isTip = (li == nodeCount - 1);
                float u = static_cast<float>(li) / static_cast<float>(std::max(1, nodeCount - 1));
                float frac = isTip ? 0.985f : (0.18f + 0.79f * (1.0f - std::pow(1.0f - u, 2.05f)));
                Vec3 attach = tw.startPt * (1.0f - frac) + tw.endPt * frac;

                float taperT = std::clamp((frac - 0.22f) / 0.765f, 0.0f, 1.0f);
                float smoothT = taperT * taperT * (3.0f - 2.0f * taperT);
                float sizeMul = (1.08f - 0.48f * smoothT) * rng.nextRange(0.93f, 1.07f);

                float phi = basePhi + static_cast<float>(li) * kGoldenAngle;
                Vec3 radialLeaf = rightB * std::cos(phi) + upB * std::sin(phi);
                float sideTilt = isTip ? 0.24f : (0.86f - 0.28f * (u * u));
                float fwdWeight = isTip ? 0.88f : (1.0f - sideTilt * 0.45f);
                float droop = -m_params.leafDroopFactor * 0.40f + (tw.heightNorm * 0.20f);
                Vec3 leafDir = safeNormalized(tw.dir * fwdWeight + radialLeaf * sideTilt + Vec3(0.0f, droop, 0.0f));

                addLeaf(attach, leafDir, leafBaseSize * sizeMul, tw.branchIdx, std::clamp(tw.heightNorm + (isTip ? 0.14f : 0.0f), 0.1f, 1.0f));
                leavesPlaced++;
            }
        } else {
            // Subterminal twig/cane segments: mature foliage spaced along distal section
            for (int li = 0; li < countOnTwig; ++li) {
                float u = static_cast<float>(li) / static_cast<float>(std::max(1, countOnTwig - 1));
                float frac = 0.34f + 0.58f * (1.0f - std::pow(1.0f - u, 1.55f));
                Vec3 attach = tw.startPt * (1.0f - frac) + tw.endPt * frac;

                float phi;
                float sideTilt;
                if (isOppositePhyllotaxis) {
                    int nodePair = li / 2;
                    float twist = (nodePair % 2 == 0) ? 0.0f : (kPi * 0.5f);
                    phi = twist + ((li % 2 == 0) ? 0.0f : kPi);
                    sideTilt = isRosemary ? 0.52f : 0.84f;
                } else {
                    phi = static_cast<float>(leavesPlaced) * kGoldenAngle + ti * 0.618f;
                    sideTilt = 0.80f;
                }

                Vec3 radialLeaf = rightB * std::cos(phi) + upB * std::sin(phi);
                float droop = -m_params.leafDroopFactor * 0.45f + (tw.heightNorm * 0.22f);
                Vec3 leafDir = safeNormalized(tw.dir * (1.0f - sideTilt * 0.45f) + radialLeaf * sideTilt + Vec3(0.0f, droop, 0.0f));
                float scaleVar = rng.nextRange(0.95f, 1.14f);
                addLeaf(attach, leafDir, leafBaseSize * scaleVar, tw.branchIdx, tw.heightNorm);
                leavesPlaced++;
            }
        }

        // Attach species-specific floral/bract/mophead/puff accents on terminal outer twigs
        if (tw.isTerminal && speciesAccent != ShrubAccentKind::NONE && speciesAccent != ShrubAccentKind::BERRIES) {
            bool spawnHere = true;
            if (isProtea && (ti % 3 != 0)) spawnHere = false;
            else if (isHibiscus && (ti % 2 != 0)) spawnHere = false;
            else if (isHydrangea && (ti % 2 != 0)) spawnHere = false;
            else if (isRosemary && (ti % 2 != 0)) spawnHere = false;

            if (spawnHere) {
                float accentSize = [&]() {
                    switch (speciesAccent) {
                        case ShrubAccentKind::PROTEA_CUP: return 0.21f;
                        case ShrubAccentKind::MOPHEAD_CORYMB: return 0.19f;
                        case ShrubAccentKind::FLOWER_BLOSSOM: return isHibiscus ? 0.14f : (isLantana ? 0.095f : 0.055f);
                        case ShrubAccentKind::BOUGAINVILLEA_BRACT: return 0.105f;
                        case ShrubAccentKind::ACACIA_PUFF: return 0.068f;
                        case ShrubAccentKind::WINGED_FRUIT: return 0.055f;
                        default: return 0.08f;
                    }
                }();
                Vec3 accentDir = safeNormalized(tw.dir * 0.75f + Vec3(0.0f, 0.35f, 0.0f));
                addAccent(tw.endPt, accentDir, accentSize, speciesAccent, tw.branchIdx);
            }
        }
    }

    (void)leavesPerTwigTarget;
    return res;
}

BushMeshData BushGenerator::generateBushMesh(uint64_t seed) {
    BushMeshData mesh;
    ShrubStructureResult structure = synthesizeShrubStructure(seed);

    // Build real 3D triangulated cylinder bark prisms for branches and diamond/ovate leaf quads
    const float radM = std::max(0.4f, m_params.radiusMeters);
    const float hM = std::max(0.4f, m_params.heightMeters);

    for (const auto& seg : structure.branches) {
        Vec3 axis = seg.endPos - seg.startPos;
        if (axis.lengthSq() < 1e-6f) continue;
        Vec3 dir = axis.normalized();
        Vec3 rVec, uVec;
        orthonormalBasisFromDir(dir, rVec, uVec);

        const int sides = 5;
        uint32_t baseV = static_cast<uint32_t>(mesh.vertices.size());
        for (int s = 0; s < sides; ++s) {
            float a = (static_cast<float>(s) / sides) * 2.0f * kPi;
            Vec3 n = safeNormalized(rVec * std::cos(a) + uVec * std::sin(a));
            Vec3 vBot = seg.startPos + n * seg.radiusBottom;
            Vec3 vTop = seg.endPos + n * seg.radiusTop;
            mesh.vertices.push_back({vBot.toC(), n.toC(), {0.0f, 0.0f}, {0.36f, 0.26f, 0.18f, 1.0f}, seg.windWeight});
            mesh.vertices.push_back({vTop.toC(), n.toC(), {1.0f, 1.0f}, {0.36f, 0.26f, 0.18f, 1.0f}, seg.windWeight});
        }
        for (int s = 0; s < sides; ++s) {
            int next = (s + 1) % sides;
            uint32_t b0 = baseV + s * 2;
            uint32_t t0 = b0 + 1;
            uint32_t b1 = baseV + next * 2;
            uint32_t t1 = b1 + 1;
            mesh.indices.push_back(b0); mesh.indices.push_back(b1); mesh.indices.push_back(t1);
            mesh.indices.push_back(b0); mesh.indices.push_back(t1); mesh.indices.push_back(t0);
        }
    }

    for (const auto& leaf : structure.leaves) {
        Vec3 rightB, upB;
        orthonormalBasisFromDir(leaf.midribDir, rightB, upB);
        float halfW = leaf.sizeMeters * 0.36f;
        Vec3 vBase = leaf.twigAttachment;
        Vec3 vMid = vBase + leaf.midribDir * (leaf.sizeMeters * 0.48f);
        Vec3 vApex = vBase + leaf.midribDir * leaf.sizeMeters;
        Vec3 vLeft = vMid - rightB * halfW;
        Vec3 vRight = vMid + rightB * halfW;
        Vec3 norm = safeNormalized(upB, Vec3(0, 1, 0));

        float sunMul = 0.78f + 0.38f * leaf.colorTint;
        float windW = std::clamp(vApex.y / hM, 0.0f, 1.2f) * m_params.windFlexibility;

        uint32_t baseIdx = static_cast<uint32_t>(mesh.vertices.size());
        mesh.vertices.push_back({vBase.toC(), norm.toC(), {0.5f, 0.0f}, {m_params.colorR * sunMul, m_params.colorG * sunMul, m_params.colorB * sunMul, 1.0f}, windW});
        mesh.vertices.push_back({vRight.toC(), norm.toC(), {1.0f, 0.5f}, {m_params.colorR * sunMul, m_params.colorG * sunMul, m_params.colorB * sunMul, 1.0f}, windW});
        mesh.vertices.push_back({vApex.toC(), norm.toC(), {0.5f, 1.0f}, {m_params.colorR * sunMul, m_params.colorG * sunMul, m_params.colorB * sunMul, 1.0f}, windW});
        mesh.vertices.push_back({vLeft.toC(), norm.toC(), {0.0f, 0.5f}, {m_params.colorR * sunMul, m_params.colorG * sunMul, m_params.colorB * sunMul, 1.0f}, windW});

        mesh.indices.push_back(baseIdx + 0); mesh.indices.push_back(baseIdx + 1); mesh.indices.push_back(baseIdx + 2);
        mesh.indices.push_back(baseIdx + 0); mesh.indices.push_back(baseIdx + 2); mesh.indices.push_back(baseIdx + 3);
    }

    mesh.aabbMin = Vec3(-radM, 0.0f, -radM);
    mesh.aabbMax = Vec3(radM, hM, radM);
    return mesh;
}

} // namespace vegetation
