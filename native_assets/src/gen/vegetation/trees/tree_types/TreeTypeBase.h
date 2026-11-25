#pragma once
// ============================================================================
// TreeTypeBase.h - Shared Mathematical Base Class for Procedural Tree Types
// Native C++17 implementation importing shared VegetationMath and Tree taxonomy.
// ============================================================================

#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include "../../common/VegetationMath.h"
#include "../TreeSpecies.h"
#include "../../forest/ForestCanopyGenerator.h"

namespace vegetation {

struct TreeBranchSegment {
    Vec3 startPos{0.0f, 0.0f, 0.0f};
    Vec3 endPos{0.0f, 1.0f, 0.0f};
    float radiusStart{0.45f};
    float radiusEnd{0.32f};
    int recursionDepth{0};
    int branchIndex{0};
    float windWeight{0.0f};
    Vec3 normal{0.0f, 1.0f, 0.0f};
};

struct TreeLeafData {
    Vec3 position{0.0f, 0.0f, 0.0f};
    Vec3 normal{0.0f, 1.0f, 0.0f};
    Vec3 tangent{1.0f, 0.0f, 0.0f};
    float size{0.8f};
    float colorR{0.2f}, colorG{0.5f}, colorB{0.15f};
    float windWeight{0.0f};
    uint32_t branchIndex{0};
};

class TreeTypeBase {
public:
    virtual ~TreeTypeBase() = default;

    virtual TreeSpeciesId getSpeciesId() const = 0;
    virtual const char* getSpeciesName() const = 0;
    virtual const char* getBotanicalName() const = 0;

    // Parameter getters & setters (Branch count, leaves, dimensions)
    int getBranchNumber() const { return m_branchNumber; }
    void setBranchNumber(int count) { m_branchNumber = std::max(1, count); }

    int getMaxRecursionDepth() const { return m_maxRecursionDepth; }
    void setMaxRecursionDepth(int depth) { m_maxRecursionDepth = std::clamp(depth, 1, 6); }

    float getBranchAngleDeg() const { return m_branchAngleDeg; }
    void setBranchAngleDeg(float angle) { m_branchAngleDeg = std::clamp(angle, 10.0f, 85.0f); }

    int getLeafCount() const { return m_leafCount; }
    void setLeafCount(int count) { m_leafCount = std::max(0, count); }

    float getLeafScale() const { return m_leafScale; }
    void setLeafScale(float scale) { m_leafScale = std::max(0.1f, scale); }

    float getTrunkRadius() const { return m_trunkRadius; }
    void setTrunkRadius(float r) { m_trunkRadius = std::max(0.05f, r); }

    float getTreeHeight() const { return m_treeHeight; }
    void setTreeHeight(float h) { m_treeHeight = std::max(1.0f, h); }

    // Controlled wind option (User can activate or deactivate)
    bool isWindEnabled() const { return m_windEnabled; }
    void setWindEnabled(bool enabled) { m_windEnabled = enabled; }

    float getWindSpeedMps() const { return m_windSpeedMps; }
    void setWindSpeedMps(float spd) { m_windSpeedMps = std::max(0.0f, spd); }

    float getWindFlexibility() const { return m_windFlexibility; }
    void setWindFlexibility(float flex) { m_windFlexibility = std::clamp(flex, 0.0f, 2.0f); }

    // Mathematical wind deflection evaluation:
    // When wind is deactivated, deflection is strictly zero!
    Vec3 computeWindDeflection(const Vec3& pos, float timeSec) const {
        if (!m_windEnabled || m_windSpeedMps <= 1e-4f || m_windFlexibility <= 1e-4f) {
            return Vec3(0.0f, 0.0f, 0.0f);
        }
        float heightFrac = std::clamp(pos.y / m_treeHeight, 0.0f, 1.5f);
        float bending = heightFrac * heightFrac * (m_windSpeedMps * 0.045f) * m_windFlexibility;
        float wave = std::sin(pos.x * 0.4f + pos.z * 0.4f + timeSec * (1.2f + m_windSpeedMps * 0.35f));
        float gust = std::sin(timeSec * 0.8f + pos.y * 0.2f) * 0.35f;
        float dx = (wave + gust) * bending;
        float dz = (wave * 0.65f) * bending;
        float dy = -0.08f * (dx * dx + dz * dz); // Slight droop when bent
        return Vec3(dx, dy, dz);
    }

    // PlantGL Mathematical Constants (plantgl.pdf Section 3.1 & Leonardo da Vinci rule)
    static constexpr float GOLDEN_ANGLE_RAD = 2.399963229728653f; // pi / (1 + sqrt(5)) golden phyllotaxis angle
    static constexpr float LEONARDO_RATIO = 0.7071067811865475f; // wr = 1 / sqrt(2) area-preserving cross section

    // PlantGL Asymmetric Crown Hull Parameters (plantgl.pdf Section 3.2.1 & Appendix B.1)
    virtual CrownControlParams getCrownHullParams() const {
        CrownControlParams p;
        p.trunkHeight = m_treeHeight * 0.38f;
        p.crownHeight = m_treeHeight * 0.62f;
        p.rxPos = m_treeHeight * 0.34f;
        p.rxNeg = m_treeHeight * 0.32f;
        p.ryPos = m_treeHeight * 0.34f;
        p.ryNeg = m_treeHeight * 0.32f;
        p.hzXPos = p.trunkHeight + p.crownHeight * 0.42f;
        p.hzXNeg = p.trunkHeight + p.crownHeight * 0.44f;
        p.hzYPos = p.trunkHeight + p.crownHeight * 0.40f;
        p.hzYNeg = p.trunkHeight + p.crownHeight * 0.45f;
        p.shapeTop = 2.0f;
        p.shapeBot = 1.8f;
        p.leafCount = m_leafCount;
        return p;
    }

    // PlantGL Between-Scale Constraint: Evaluate maximum allowed radial envelope at height y and azimuth theta
    float evaluateCrownRadiusAt(float y, float thetaRad) const {
        CrownControlParams p = getCrownHullParams();
        if (y <= p.trunkHeight) {
            return m_trunkRadius * 1.35f;
        }
        float crownTop = p.trunkHeight + p.crownHeight;
        if (y >= crownTop) {
            return 0.08f;
        }
        float hFrac = (y - p.trunkHeight) / std::max(0.1f, p.crownHeight);
        return ForestCanopyGenerator::evaluateCrownRadius(p, thetaRad, hFrac);
    }

    // Procedural synthesis interface
    virtual void buildTreeStructure(
        std::vector<TreeBranchSegment>& outBranches,
        std::vector<TreeLeafData>& outLeaves,
        uint64_t seed = 42
    ) = 0;

    // PlantGL & Stojanovic (2016): Conifer Needle Clusters & Paired Fascicles
    // Every terminal twig ends at the tip with an apical needle brush, with acropetal spacing
    // compression and gentle apical needle size taper toward shoot tips.
    static void generateConiferNeedles(
        const std::vector<TreeBranchSegment>& branches,
        std::vector<TreeLeafData>& outLeaves,
        FastRng& rng,
        float leafScale,
        float colorR, float colorG, float colorB,
        float treeHeight,
        int targetNeedleCount,
        bool isSpruce = true
    ) {
        if (branches.empty() || targetNeedleCount <= 0) return;

        // Detect parent-child topology to find true terminal shoots
        std::vector<bool> hasChild(branches.size(), false);
        for (size_t i = 0; i < branches.size(); ++i) {
            for (size_t j = 0; j < branches.size(); ++j) {
                if (i != j && (branches[i].endPos - branches[j].startPos).lengthSq() < 0.0025f) {
                    hasChild[i] = true;
                    break;
                }
            }
        }

        std::vector<size_t> terminalTwigs;
        std::vector<size_t> subterminalTwigs;
        for (size_t i = 0; i < branches.size(); ++i) {
            if (branches[i].recursionDepth >= 2) {
                if (!hasChild[i]) terminalTwigs.push_back(i);
                else subterminalTwigs.push_back(i);
            }
        }
        if (terminalTwigs.empty() && subterminalTwigs.empty()) {
            for (size_t i = 0; i < branches.size(); ++i) {
                if (branches[i].recursionDepth >= 1) {
                    if (!hasChild[i]) terminalTwigs.push_back(i);
                    else subterminalTwigs.push_back(i);
                }
            }
        }
        if (terminalTwigs.empty()) {
            terminalTwigs = subterminalTwigs;
            subterminalTwigs.clear();
        }
        if (terminalTwigs.empty()) return;

        int needlesPerBundle = isSpruce ? 4 : 2;
        float baseNeedleLen = (isSpruce ? 0.042f : 0.068f) * leafScale;

        auto emitBundleAt = [&](size_t segIdx, float tFrac, float az, float sizeMul, bool isApicalTip) {
            const auto& seg = branches[segIdx];
            Vec3 dir = seg.endPos - seg.startPos;
            float segLen = dir.length();
            if (segLen < 1e-4f) return;
            dir = dir * (1.0f / segLen);

            Vec3 sheathBase = seg.startPos + dir * (segLen * tFrac);
            Vec3 side = cross(dir, Vec3(0.0f, 1.0f, 0.0f));
            if (side.lengthSq() < 0.01f) side = cross(dir, Vec3(1.0f, 0.0f, 0.0f));
            side = side.normalized();
            Vec3 up = cross(dir, side).normalized();

            Vec3 sheathNormal = (side * std::cos(az) + up * std::sin(az)).normalized();
            float rAtT = seg.radiusStart + (seg.radiusEnd - seg.radiusStart) * tFrac;
            sheathBase = sheathBase + sheathNormal * (rAtT * 0.90f);

            // Terminal apical bud needles project closely forward along shoot axis; lateral spread wider
            Vec3 growthDir = isApicalTip
                ? (dir * 0.88f + sheathNormal * 0.24f + Vec3(0.0f, 0.16f, 0.0f)).normalized()
                : (dir * 0.52f + sheathNormal * 0.56f + Vec3(0.0f, 0.30f, 0.0f)).normalized();

            int countHere = isApicalTip ? (isSpruce ? 3 : 2) : needlesPerBundle;
            for (int n = 0; n < countHere; ++n) {
                float spreadAngle = isSpruce
                    ? ((static_cast<float>(n) - 0.5f * (countHere - 1)) * (isApicalTip ? 0.22f : 0.30f))
                    : ((n == 0 ? -0.18f : 0.18f));
                Vec3 needleDir = (growthDir + side * spreadAngle + up * rng.nextRange(-0.07f, 0.07f)).normalized();
                float needleLen = baseNeedleLen * sizeMul * rng.nextRange(0.92f, 1.08f);

                TreeLeafData leaf;
                leaf.position = sheathBase;
                leaf.normal = needleDir;
                leaf.tangent = cross(needleDir, side).normalized();
                leaf.size = needleLen;
                float flushBoost = isApicalTip ? 1.08f : 1.0f;
                leaf.colorR = colorR * rng.nextRange(0.88f, 1.12f) * flushBoost;
                leaf.colorG = colorG * rng.nextRange(0.90f, 1.12f) * flushBoost;
                leaf.colorB = colorB * rng.nextRange(0.88f, 1.10f);
                leaf.windWeight = std::clamp(sheathBase.y / std::max(0.5f, treeHeight), 0.1f, 1.0f);
                leaf.branchIndex = static_cast<uint32_t>(segIdx);

                outLeaves.push_back(leaf);
            }
        };

        // 1. Place acropetal needle nodes along EVERY terminal shoot, always ending at t = 0.985f!
        int termCount = static_cast<int>(terminalTwigs.size());
        int termBudget = static_cast<int>(targetNeedleCount * (subterminalTwigs.empty() ? 1.0f : 0.78f));
        int nodesPerTerm = std::clamp(termBudget / std::max(1, termCount * needlesPerBundle), 4, 8);

        for (size_t ti = 0; ti < terminalTwigs.size(); ++ti) {
            size_t segIdx = terminalTwigs[ti];
            float baseAz = rng.nextFloat01() * 6.28318f;
            for (int k = 0; k < nodesPerTerm; ++k) {
                float u = static_cast<float>(k) / static_cast<float>(std::max(1, nodesPerTerm - 1));
                // Acropetal compression: wide near base, tightly packed near shoot apex (0.985)
                float tFrac = (k == nodesPerTerm - 1)
                    ? 0.985f
                    : (0.18f + 0.79f * (1.0f - std::pow(1.0f - u, 2.0f)));
                // Apical needle size taper: 1.06x mature base -> 0.62x compact newly flushed tip needles
                float taperT = std::clamp((tFrac - 0.25f) / 0.735f, 0.0f, 1.0f);
                float sizeMul = 1.06f - 0.44f * (taperT * taperT * (3.0f - 2.0f * taperT));
                float az = baseAz + static_cast<float>(k) * GOLDEN_ANGLE_RAD;
                emitBundleAt(segIdx, tFrac, az, sizeMul, (k == nodesPerTerm - 1));
            }
        }

        // 2. Fill remaining needle budget across subterminal twigs (and extra nodes on terminal shoots)
        int safety = 0;
        size_t subPtr = 0;
        while (static_cast<int>(outLeaves.size()) < targetNeedleCount && safety++ < targetNeedleCount * 2) {
            if (!subterminalTwigs.empty()) {
                size_t segIdx = subterminalTwigs[subPtr % subterminalTwigs.size()];
                subPtr++;
                float tFrac = 0.35f + 0.58f * rng.nextFloat01();
                emitBundleAt(segIdx, tFrac, rng.nextFloat01() * 6.28318f, rng.nextRange(0.95f, 1.10f), false);
            } else {
                size_t segIdx = terminalTwigs[subPtr % terminalTwigs.size()];
                subPtr++;
                float u = rng.nextFloat01();
                float tFrac = 0.24f + 0.70f * (1.0f - std::pow(1.0f - u, 1.9f));
                float taperT = std::clamp((tFrac - 0.25f) / 0.735f, 0.0f, 1.0f);
                float sizeMul = 1.06f - 0.44f * (taperT * taperT * (3.0f - 2.0f * taperT));
                emitBundleAt(segIdx, tFrac, rng.nextFloat01() * 6.28318f, sizeMul, false);
            }
        }
    }

    // Stojanovic (2016) + Acropetal Shoot Phyllotaxis:
    //  - Trunk (depth == 0) strictly receives ZERO leaves.
    //  - Every terminal branch/stem ends at t = 0.985f with 1 (alternate/spiral) or 2 (opposite/paired)
    //    apical leaves depending on the botanical nature of the tree.
    //  - Internode spacing compresses progressively toward the stem tip (closeness increases toward tip).
    //  - Leaf sizes calibrate to real-world species proportions and taper smoothly smaller toward stem tips.
    static void generateStojanovicLeaves(
        const std::vector<TreeBranchSegment>& branches,
        std::vector<TreeLeafData>& outLeaves,
        FastRng& rng,
        float leafScale,
        float colorR, float colorG, float colorB,
        float treeHeight,
        int targetLeafCount,
        bool isGiantFrond = false,
        int minRecursionDepth = 1,
        int terminalLeafCount = 1,
        float realLeafLengthMeters = 0.0f
    ) {
        if (branches.empty() || targetLeafCount <= 0) return;

        // Detect terminal segments (branches without children emerging near their endPos)
        std::vector<bool> hasChild(branches.size(), false);
        for (size_t i = 0; i < branches.size(); ++i) {
            for (size_t j = 0; j < branches.size(); ++j) {
                if (i != j && (branches[i].endPos - branches[j].startPos).lengthSq() < 0.0025f) {
                    hasChild[i] = true;
                    break;
                }
            }
        }

        std::vector<size_t> terminalBranches;
        std::vector<size_t> subterminalBranches;
        for (size_t i = 0; i < branches.size(); ++i) {
            if (branches[i].recursionDepth >= minRecursionDepth) {
                if (!hasChild[i]) terminalBranches.push_back(i);
                else subterminalBranches.push_back(i);
            }
        }
        if (terminalBranches.empty() && subterminalBranches.empty() && minRecursionDepth > 1) {
            for (size_t i = 0; i < branches.size(); ++i) {
                if (branches[i].recursionDepth >= 1) {
                    if (!hasChild[i]) terminalBranches.push_back(i);
                    else subterminalBranches.push_back(i);
                }
            }
        }
        if (terminalBranches.empty() && subterminalBranches.empty()) {
            return; // Strictly zero leaves on bare trunk
        }
        if (terminalBranches.empty()) {
            terminalBranches = subterminalBranches;
            subterminalBranches.clear();
        }

        const int termCount = std::clamp(terminalLeafCount, 1, 2);

        auto smoothstepVal = [](float e0, float e1, float x) -> float {
            float u = std::clamp((x - e0) / std::max(1e-5f, e1 - e0), 0.0f, 1.0f);
            return u * u * (3.0f - 2.0f * u);
        };

        auto emitLeafOnSegment = [&](size_t bIdx, float tAlong, float thetaRad, float lateralFactor, float sizeTaperMul, bool isApicalTip) {
            const auto& seg = branches[bIdx];
            Vec3 dir = (seg.endPos - seg.startPos);
            float len = dir.length();
            if (len < 1e-4f) return;
            dir = dir * (1.0f / len);

            Vec3 side = cross(dir, Vec3(0.0f, 1.0f, 0.0f));
            if (side.lengthSq() < 0.01f) side = cross(dir, Vec3(1.0f, 0.0f, 0.0f));
            side = side.normalized();
            Vec3 up = cross(dir, side).normalized();

            Vec3 N_p = (side * std::cos(thetaRad) + up * std::sin(thetaRad)).normalized();
            float rAtT = seg.radiusStart + (seg.radiusEnd - seg.radiusStart) * tAlong;
            Vec3 attachPoint = seg.startPos + dir * (len * tAlong) + N_p * (rAtT * 0.90f);

            Vec3 growthDir;
            if (dir.y < -0.22f) {
                // Pendulous weeping streamer: tip leaf points smoothly downward, lateral leaves splay slightly outward
                float dirMix = 0.90f - 0.34f * lateralFactor;
                float radMix = 0.16f + 0.44f * lateralFactor;
                growthDir = (dir * dirMix + N_p * radMix + Vec3(0.0f, -0.14f, 0.0f)).normalized();
            } else {
                // Shoot axis orientation: apical terminal leaves extend forward/outward cleanly from twig end
                float dirMix = isApicalTip ? (termCount == 1 ? 0.84f : 0.70f) : (0.76f - 0.46f * lateralFactor);
                float radMix = isApicalTip ? (termCount == 1 ? 0.24f : 0.64f) : (0.20f + 0.68f * lateralFactor);
                Vec3 phototropism = Vec3(0.0f, isApicalTip ? 0.14f : 0.16f, 0.0f);
                growthDir = (dir * dirMix + N_p * radMix + phototropism).normalized();
            }

            float leafDimension;
            if (isGiantFrond) {
                // Palm pinnate frond pinnae: taper gracefully smaller toward frond apex
                float baseGiant = (realLeafLengthMeters > 0.25f) ? realLeafLengthMeters : 1.72f;
                leafDimension = baseGiant * sizeTaperMul * rng.nextRange(0.92f, 1.08f) * leafScale;
            } else {
                float twigR = std::max(0.012f, seg.radiusStart);
                float baseReal = (realLeafLengthMeters > 0.0f)
                    ? realLeafLengthMeters
                    : std::clamp(twigR * 3.8f + 0.075f, 0.085f, 0.18f);
                leafDimension = baseReal * sizeTaperMul * rng.nextRange(0.93f, 1.07f) * leafScale;
            }

            TreeLeafData leaf;
            leaf.position = attachPoint;
            leaf.normal = growthDir;
            leaf.tangent = dir;
            leaf.size = leafDimension;
            // Young freshly-flushed apical leaves carry a slightly brighter luminous green hue
            float freshTint = (tAlong > 0.86f) ? 1.07f : 1.0f;
            leaf.colorR = std::min(1.0f, colorR * rng.nextRange(0.88f, 1.12f) * freshTint);
            leaf.colorG = std::min(1.0f, colorG * rng.nextRange(0.90f, 1.12f) * freshTint);
            leaf.colorB = std::min(1.0f, colorB * rng.nextRange(0.88f, 1.10f));
            leaf.windWeight = std::clamp(attachPoint.y / std::max(0.5f, treeHeight), 0.2f, 1.0f);
            leaf.branchIndex = static_cast<uint32_t>(bIdx);

            outLeaves.push_back(leaf);
        };

        const int T = static_cast<int>(terminalBranches.size());
        int termBudget = static_cast<int>(targetLeafCount * (subterminalBranches.empty() ? 1.0f : 0.78f));

        // Phase 1: Every terminal stem gets progressive acropetal foliage AND ends cleanly with 1 or 2 tip leaves!
        if (termCount >= 2) {
            // Opposite / decussate / paripinnate species: paired leaves per node, ending in 2 opposite apical leaves
            int nodePairs = std::clamp(termBudget / std::max(1, T * 2), 3, 7);
            if (isGiantFrond) nodePairs = std::clamp(termBudget / std::max(1, T * 2), 4, 9);

            for (size_t ti = 0; ti < terminalBranches.size(); ++ti) {
                size_t bIdx = terminalBranches[ti];
                float basePhi = rng.nextFloat01() * 3.14159265f;

                for (int k = 0; k < nodePairs; ++k) {
                    bool isTip = (k == nodePairs - 1);
                    float u = static_cast<float>(k) / static_cast<float>(std::max(1, nodePairs - 1));
                    // Convex acropetal compression: internode gaps shrink markedly toward the stem tip
                    float tAlong = isTip ? 0.985f : (0.16f + 0.825f * (1.0f - std::pow(1.0f - u, 2.15f)));
                    // Size tapers smoothly downward from full mature basal/mid leaves (~1.08x) to young tip pair (~0.56x)
                    float taperT = smoothstepVal(0.20f, 0.985f, tAlong);
                    float sizeMul = isGiantFrond ? (1.10f - 0.54f * std::pow(taperT, 1.15f))
                                                 : (1.08f - 0.52f * std::pow(taperT, 1.25f));
                    float lateralFactor = isTip ? 0.58f : (0.94f - 0.28f * (u * u));
                    // Decussate 90-deg twist per node along opposite twigs (flat opposite pairs on pinnate fronds)
                    float twist = isGiantFrond ? 0.0f : (static_cast<float>(k) * 1.5707963f);

                    for (int pairSide = 0; pairSide < 2; ++pairSide) {
                        float theta = basePhi + twist + (pairSide == 0 ? 0.0f : 3.14159265f) + rng.nextRange(-0.08f, 0.08f);
                        emitLeafOnSegment(bIdx, tAlong, theta, lateralFactor, sizeMul, isTip);
                    }
                }
            }
        } else {
            // Alternate / spiral / single-apex species: spiral nodes compressing toward tip, ending with 1 apical leaf
            int nodesPerStem = std::clamp(termBudget / std::max(1, T), 4, 11);
            if (T <= 48 && targetLeafCount >= 320) {
                nodesPerStem = std::clamp(termBudget / std::max(1, T), 6, 15);
            }

            for (size_t ti = 0; ti < terminalBranches.size(); ++ti) {
                size_t bIdx = terminalBranches[ti];
                float basePhi = rng.nextFloat01() * 6.2831853f;

                for (int k = 0; k < nodesPerStem; ++k) {
                    bool isTip = (k == nodesPerStem - 1);
                    float u = static_cast<float>(k) / static_cast<float>(std::max(1, nodesPerStem - 1));
                    // Convex acropetal compression: internode spacing narrows progressively approaching shoot apex
                    float tAlong = isTip ? 0.985f : (0.16f + 0.825f * (1.0f - std::pow(1.0f - u, 2.15f)));
                    // Size tapers smoothly downward toward stem apex: 1.08x mature mid-stem -> 0.58x apical tip leaf
                    float taperT = smoothstepVal(0.20f, 0.985f, tAlong);
                    float sizeMul = isGiantFrond ? (1.10f - 0.52f * std::pow(taperT, 1.15f))
                                                 : (1.08f - 0.50f * std::pow(taperT, 1.25f));
                    float lateralFactor = isTip ? 0.18f : (0.94f - 0.36f * (u * u));
                    float theta = basePhi + static_cast<float>(k) * GOLDEN_ANGLE_RAD + rng.nextRange(-0.10f, 0.10f);

                    emitLeafOnSegment(bIdx, tAlong, theta, lateralFactor, sizeMul, isTip);
                }
            }
        }

        // Phase 2: Fill any remaining leaf budget on subterminal branches (distal sections) with acropetal spacing
        int safety = 0;
        size_t subPtr = 0;
        while (static_cast<int>(outLeaves.size()) < targetLeafCount && safety++ < targetLeafCount * 3) {
            if (!subterminalBranches.empty()) {
                size_t bIdx = subterminalBranches[subPtr % subterminalBranches.size()];
                subPtr++;
                float u = rng.nextFloat01();
                float tAlong = 0.36f + 0.58f * (1.0f - std::pow(1.0f - u, 1.85f));
                float taperT = smoothstepVal(0.35f, 0.95f, tAlong);
                float sizeMul = 1.10f - 0.24f * taperT; // Older subterminal twigs have mature larger foliage
                float theta = (termCount >= 2)
                    ? (static_cast<float>(subPtr % 4) * 1.5707963f + rng.nextRange(-0.15f, 0.15f))
                    : (static_cast<float>(subPtr) * GOLDEN_ANGLE_RAD);
                emitLeafOnSegment(bIdx, tAlong, theta, 0.88f, sizeMul, false);
            } else {
                // Extra distal nodes along terminal stems while honoring acropetal density & size gradient
                size_t bIdx = terminalBranches[subPtr % terminalBranches.size()];
                subPtr++;
                float u = rng.nextFloat01();
                float tAlong = 0.28f + 0.66f * (1.0f - std::pow(1.0f - u, 2.0f));
                float taperT = smoothstepVal(0.22f, 0.985f, tAlong);
                float sizeMul = 1.06f - 0.46f * std::pow(taperT, 1.2f);
                emitLeafOnSegment(bIdx, tAlong, static_cast<float>(subPtr) * GOLDEN_ANGLE_RAD, 0.82f, sizeMul, false);
            }
        }
    }

protected:
    int m_branchNumber{20};
    int m_maxRecursionDepth{4};
    float m_branchAngleDeg{35.0f};
    int m_leafCount{200};
    float m_leafScale{1.0f};
    float m_trunkRadius{0.45f};
    float m_treeHeight{14.0f};
    bool m_windEnabled{true};
    float m_windSpeedMps{4.5f};
    float m_windFlexibility{0.35f};

    int m_terminalLeafCount{1};
    float m_realLeafLengthMeters{0.13f};

    float m_barkColorR{0.36f}, m_barkColorG{0.25f}, m_barkColorB{0.18f};
    float m_leafColorR{0.22f}, m_leafColorG{0.48f}, m_leafColorB{0.16f};
};

} // namespace vegetation
