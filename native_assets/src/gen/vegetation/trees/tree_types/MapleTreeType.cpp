#include "MapleTreeType.h"

namespace vegetation {

MapleTreeType::MapleTreeType() {
    m_branchNumber = 44;
    m_maxRecursionDepth = 4;
    m_branchAngleDeg = 42.0f;
    m_leafCount = 1450;
    m_leafScale = 1.10f;
    m_trunkRadius = 0.48f;
    m_treeHeight = 14.5f;
    m_windFlexibility = 0.32f;
    m_barkColorR = 0.34f; m_barkColorG = 0.24f; m_barkColorB = 0.16f;
    m_leafColorR = 0.46f; m_leafColorG = 0.68f; m_leafColorB = 0.18f;
    m_terminalLeafCount = 2;
    m_realLeafLengthMeters = 0.135f;
}

void MapleTreeType::buildTreeStructure(
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    uint64_t seed
) {
    outBranches.clear();
    outLeaves.clear();

    FastRng rng(seed);

    // 1. Sturdy clear trunk rising cleanly to ~48% height before first opposite boughs
    int trunkSteps = 7;
    float stepH = (m_treeHeight * 0.48f) / static_cast<float>(trunkSteps);
    Vec3 currPos(0.0f, 0.0f, 0.0f);
    float currR = m_trunkRadius;

    struct MapleNode {
        Vec3 pos;
        float radius;
    };
    std::vector<MapleNode> trunkNodes;

    for (int t = 0; t < trunkSteps; ++t) {
        float nX = rng.nextRange(-0.02f, 0.02f) * stepH;
        float nZ = rng.nextRange(-0.02f, 0.02f) * stepH;
        Vec3 nextPos = currPos + Vec3(nX, stepH, nZ);
        float nextR = currR * 0.90f;

        TreeBranchSegment seg;
        seg.startPos = currPos;
        seg.endPos = nextPos;
        seg.radiusStart = currR;
        seg.radiusEnd = nextR;
        seg.recursionDepth = 0;
        seg.branchIndex = static_cast<int>(outBranches.size());
        seg.windWeight = (nextPos.y / m_treeHeight) * 0.20f;
        seg.normal = (nextPos - currPos).normalized();

        outBranches.push_back(seg);

        if (t >= 3) {
            trunkNodes.push_back({nextPos, std::max(0.06f, currR * 0.85f)});
        }

        currPos = nextPos;
        currR = nextR;
    }

    // 2. Opposite symmetrical branching pairs (decussate phyllotaxis)
    int targetBranches = std::clamp(m_branchNumber * 6, 220, 360);
    int branchesCreated = trunkSteps;
    std::vector<MapleNode> branchTips = trunkNodes.empty()
        ? std::vector<MapleNode>{{currPos, m_trunkRadius * 0.50f}}
        : trunkNodes;

    int level = 1;
    while (branchesCreated < targetBranches && level <= m_maxRecursionDepth) {
        size_t tipCount = branchTips.size();
        if (tipCount == 0) break;

        std::vector<MapleNode> nextTips;
        for (size_t i = 0; i < tipCount && branchesCreated < targetBranches; ++i) {
            const auto& baseNode = branchTips[i];
            float phiBase = rng.nextFloat01() * 3.14159f;

            // Opposite pair: 180 degrees apart
            for (int pair = 0; pair < 2 && branchesCreated < targetBranches; ++pair) {
                float phi = phiBase + (pair * 3.14159f) + rng.nextRange(-0.14f, 0.14f);
                float thetaRad = (m_branchAngleDeg + rng.nextRange(-5.0f, 5.0f)) * (3.14159f / 180.0f);
                float len = (m_treeHeight * 0.25f) / static_cast<float>(level);

                float dx = std::sin(thetaRad) * std::cos(phi) * len;
                float dy = std::cos(thetaRad) * len * 0.9f;
                float dz = std::sin(thetaRad) * std::sin(phi) * len;

                Vec3 tip = baseNode.pos + Vec3(dx, dy, dz);
                bool isTerminal = (level == m_maxRecursionDepth || branchesCreated + 1 >= targetBranches);
                float startR = std::max(0.014f, baseNode.radius * 0.64f);
                float endR = isTerminal ? 0.005f : std::max(0.010f, startR * 0.65f);

                TreeBranchSegment bSeg;
                bSeg.startPos = baseNode.pos;
                bSeg.endPos = tip;
                bSeg.radiusStart = startR;
                bSeg.radiusEnd = endR;
                bSeg.recursionDepth = level;
                bSeg.branchIndex = static_cast<int>(outBranches.size());
                bSeg.windWeight = std::min(1.0f, (tip.y / m_treeHeight) * 0.9f);
                bSeg.normal = (tip - baseNode.pos).normalized();

                outBranches.push_back(bSeg);
                nextTips.push_back({tip, endR});
                if (level <= 2) {
                    Vec3 midNodePos = baseNode.pos + Vec3(dx, dy, dz) * 0.56f;
                    nextTips.push_back({midNodePos, (startR + endR) * 0.5f});
                }
                branchesCreated++;
            }
        }
        branchTips = std::move(nextTips);
        level++;
    }

    // Taper any terminal branches without children to fine shoot tips
    std::vector<bool> hasChild(outBranches.size(), false);
    for (size_t i = 0; i < outBranches.size(); ++i) {
        for (size_t j = 0; j < outBranches.size(); ++j) {
            if (i != j && (outBranches[i].endPos - outBranches[j].startPos).lengthSq() < 0.04f) {
                hasChild[i] = true;
                break;
            }
        }
    }
    for (size_t i = 0; i < outBranches.size(); ++i) {
        if (!hasChild[i]) {
            outBranches[i].radiusEnd = (outBranches[i].recursionDepth == 0 ? 0.010f : 0.005f);
        }
    }

    // 3. Maple opposite decussate foliage placed along shoots ending in a terminal pair
    int targetLeaves = std::max(1250, m_leafCount);
    generateStojanovicLeaves(outBranches, outLeaves, rng, m_leafScale, m_leafColorR, m_leafColorG, m_leafColorB, m_treeHeight, targetLeaves, false, 2, m_terminalLeafCount, m_realLeafLengthMeters);
}

} // namespace vegetation
