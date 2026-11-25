#include "BirchTreeType.h"

namespace vegetation {

BirchTreeType::BirchTreeType() {
    m_branchNumber = 40;
    m_maxRecursionDepth = 4;
    m_branchAngleDeg = 38.0f;
    m_leafCount = 1250;
    m_leafScale = 0.95f;
    m_trunkRadius = 0.30f;
    m_treeHeight = 16.0f;
    m_windFlexibility = 0.65f; // High flex pendulous branches
    m_barkColorR = 0.88f; m_barkColorG = 0.88f; m_barkColorB = 0.84f; // Silvery white bark
    m_leafColorR = 0.28f; m_leafColorG = 0.55f; m_leafColorB = 0.22f; // Fresh bright green cordate leaves
    m_terminalLeafCount = 1;
    m_realLeafLengthMeters = 0.095f;
}

CrownControlParams BirchTreeType::getCrownHullParams() const {
    CrownControlParams p;
    // Silver Birch: slender upright crown with weeping pendulous outer skirt (plantgl.pdf)
    p.trunkHeight = m_treeHeight * 0.30f;
    p.crownHeight = m_treeHeight * 0.70f;
    p.rxPos = m_treeHeight * 0.26f;
    p.rxNeg = m_treeHeight * 0.24f;
    p.ryPos = m_treeHeight * 0.25f;
    p.ryNeg = m_treeHeight * 0.24f;
    p.hzXPos = p.trunkHeight + p.crownHeight * 0.50f;
    p.hzXNeg = p.trunkHeight + p.crownHeight * 0.52f;
    p.hzYPos = p.trunkHeight + p.crownHeight * 0.48f;
    p.hzYNeg = p.trunkHeight + p.crownHeight * 0.50f;
    p.shapeTop = 1.5f; // Parabolic slender tip
    p.shapeBot = 1.6f;
    p.leafCount = m_leafCount;
    return p;
}

void BirchTreeType::buildTreeStructure(
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    uint64_t seed
) {
    outBranches.clear();
    outLeaves.clear();

    FastRng rng(seed);

    // 1. Slender, graceful upright silver trunk rising to ~78% of tree height
    // Continuous apical leader with smooth taper from m_trunkRadius down to 0.02m at crown apex
    const int trunkSteps = 10;
    const float trunkReachHeight = m_treeHeight * 0.78f;
    const float stepH = trunkReachHeight / static_cast<float>(trunkSteps);

    Vec3 currPos(0.0f, 0.0f, 0.0f);
    float currR = m_trunkRadius;

    struct TrunkNode {
        Vec3 pos;
        float radius;
        float heightFrac;
    };
    std::vector<TrunkNode> trunkNodes;

    for (int t = 0; t < trunkSteps; ++t) {
        // Subtle organic trunk curve (swaying slightly in 3D, not planar)
        float curveX = rng.nextRange(-0.025f, 0.025f) * stepH;
        float curveZ = rng.nextRange(-0.025f, 0.025f) * stepH;
        Vec3 nextPos = currPos + Vec3(curveX, stepH, curveZ);

        // Smooth gradual tapering so apex terminates in a natural slender shoot tip
        float frac = static_cast<float>(t + 1) / static_cast<float>(trunkSteps);
        float nextR = (t == trunkSteps - 1) ? 0.008f : std::max(0.020f, m_trunkRadius * (1.0f - frac * 0.92f));

        TreeBranchSegment seg;
        seg.startPos = currPos;
        seg.endPos = nextPos;
        seg.radiusStart = currR;
        seg.radiusEnd = nextR;
        seg.recursionDepth = 0;
        seg.branchIndex = static_cast<int>(outBranches.size());
        seg.windWeight = (nextPos.y / m_treeHeight) * 0.25f;
        seg.normal = Vec3(0.0f, 1.0f, 0.0f);

        outBranches.push_back(seg);

        // Scaffold attachment candidates along upper 60% of trunk (t >= 4)
        // Keeps the lower trunk iconic silvery white and bare
        if (t >= 4 && t < trunkSteps - 1) {
            trunkNodes.push_back({nextPos, nextR, nextPos.y / m_treeHeight});
        }

        currPos = nextPos;
        currR = nextR;
    }

    const int maxAllowedBranches = std::max(260, m_branchNumber * 7);

    // Sub-apical fine crown shoots clothing the tapered silver birch leader tip
    for (int ap = 0; ap < 3; ++ap) {
        float apAz = static_cast<float>(ap) * (6.28318f / 3.0f) + rng.nextRange(-0.2f, 0.2f);
        Vec3 apDir = Vec3(std::cos(apAz) * 0.35f, 0.92f, std::sin(apAz) * 0.35f).normalized();
        Vec3 apTip = currPos + apDir * (m_treeHeight * 0.08f);

        TreeBranchSegment apSeg;
        apSeg.startPos = currPos;
        apSeg.endPos = apTip;
        apSeg.radiusStart = 0.008f;
        apSeg.radiusEnd = 0.004f;
        apSeg.recursionDepth = 2;
        apSeg.branchIndex = static_cast<int>(outBranches.size());
        apSeg.windWeight = 0.92f;
        apSeg.normal = apDir;

        outBranches.push_back(apSeg);
    }

    // 2. Primary scaffold boughs (Order 1) radiating in full 3D Fibonacci spiral phyllotaxis
    const int boughCount = std::clamp(m_branchNumber / 5, 6, 9);
    const size_t numTrunkNodes = trunkNodes.size();

    struct ScaffoldTip {
        Vec3 pos;
        Vec3 midPos;
        Vec3 dir;
        float radius;
        float midRadius;
        float heightFrac;
    };
    std::vector<ScaffoldTip> scaffoldTips;

    float baseAzimuth = rng.nextFloat01() * 6.28318f;

    for (int b = 0; b < boughCount; ++b) {
        if (outBranches.size() >= static_cast<size_t>(maxAllowedBranches)) break;

        size_t nodeIdx = (numTrunkNodes > 0) ? (b % numTrunkNodes) : 0;
        const auto& node = (numTrunkNodes > 0) ? trunkNodes[nodeIdx] : TrunkNode{currPos, currR, 0.7f};

        float phi = baseAzimuth + static_cast<float>(b) * GOLDEN_ANGLE_RAD + rng.nextRange(-0.15f, 0.15f);
        float thetaRad = (m_branchAngleDeg + rng.nextRange(-5.0f, 6.0f)) * (3.14159f / 180.0f);

        Vec3 radialDir(std::cos(phi), 0.0f, std::sin(phi));
        Vec3 attachPos = node.pos;
        Vec3 boughDir = (radialDir * std::sin(thetaRad) + Vec3(0.0f, std::cos(thetaRad), 0.0f)).normalized();

        float heightFactor = 1.0f - 0.45f * node.heightFrac;
        float boughLen = (m_treeHeight * 0.25f) * heightFactor * rng.nextRange(0.85f, 1.15f);
        Vec3 midPos = attachPos + boughDir * (boughLen * 0.55f);
        Vec3 tipPos = attachPos + boughDir * boughLen;

        float startR = std::max(0.028f, node.radius * 0.48f);
        float midR = startR * 0.84f;
        float endR = startR * LEONARDO_RATIO;

        TreeBranchSegment bSeg;
        bSeg.startPos = attachPos;
        bSeg.endPos = tipPos;
        bSeg.radiusStart = startR;
        bSeg.radiusEnd = endR;
        bSeg.recursionDepth = 1;
        bSeg.branchIndex = static_cast<int>(outBranches.size());
        bSeg.windWeight = (tipPos.y / m_treeHeight) * 0.55f;
        bSeg.normal = boughDir;

        outBranches.push_back(bSeg);

        scaffoldTips.push_back({tipPos, midPos, boughDir, endR, midR, tipPos.y / m_treeHeight});
    }

    // 3. Secondary spreading lateral twigs (Order 2) at mid-bough and tip
    struct TwigTip {
        Vec3 pos;
        Vec3 dir;
        float radius;
    };
    std::vector<TwigTip> weepingTwigAnchors;

    for (size_t s = 0; s < scaffoldTips.size(); ++s) {
        if (outBranches.size() >= static_cast<size_t>(maxAllowedBranches)) break;

        const auto& sc = scaffoldTips[s];
        Vec3 perp = cross(sc.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (perp.lengthSq() < 0.01f) perp = cross(sc.dir, Vec3(1.0f, 0.0f, 0.0f));
        perp = perp.normalized();

        // Mid-bough bilateral laterals
        for (int side = -1; side <= 1; side += 2) {
            if (outBranches.size() >= static_cast<size_t>(maxAllowedBranches)) break;
            Vec3 midTwigDir = (sc.dir * 0.68f + perp * (0.66f * static_cast<float>(side)) + Vec3(0.0f, 0.06f, 0.0f)).normalized();
            float midTwigLen = (m_treeHeight * 0.14f) * rng.nextRange(0.85f, 1.15f);
            Vec3 midTwigTip = sc.midPos + midTwigDir * midTwigLen;
            float mEndR = sc.midRadius * LEONARDO_RATIO;

            TreeBranchSegment mSeg;
            mSeg.startPos = sc.midPos;
            mSeg.endPos = midTwigTip;
            mSeg.radiusStart = sc.midRadius * 0.85f;
            mSeg.radiusEnd = mEndR;
            mSeg.recursionDepth = 2;
            mSeg.branchIndex = static_cast<int>(outBranches.size());
            mSeg.windWeight = (midTwigTip.y / m_treeHeight) * 0.72f;
            mSeg.normal = midTwigDir;
            outBranches.push_back(mSeg);
            weepingTwigAnchors.push_back({midTwigTip, midTwigDir, mEndR});
        }

        // Tip bilateral laterals
        for (int side = -1; side <= 1; side += 2) {
            if (outBranches.size() >= static_cast<size_t>(maxAllowedBranches)) break;

            Vec3 twigDir = (sc.dir * 0.72f + perp * (0.62f * static_cast<float>(side)) + Vec3(0.0f, 0.08f, 0.0f)).normalized();
            float twigLen = (m_treeHeight * 0.16f) * rng.nextRange(0.85f, 1.15f);
            Vec3 twigTip = sc.pos + twigDir * twigLen;

            float startR = sc.radius;
            float endR = startR * LEONARDO_RATIO;

            TreeBranchSegment tSeg;
            tSeg.startPos = sc.pos;
            tSeg.endPos = twigTip;
            tSeg.radiusStart = startR;
            tSeg.radiusEnd = endR;
            tSeg.recursionDepth = 2;
            tSeg.branchIndex = static_cast<int>(outBranches.size());
            tSeg.windWeight = (twigTip.y / m_treeHeight) * 0.72f;
            tSeg.normal = twigDir;

            outBranches.push_back(tSeg);

            weepingTwigAnchors.push_back({twigTip, twigDir, endR});
        }
    }

    // 4. Pendulous weeping terminal shoots (Order 3 & Order 4)
    std::vector<TwigTip> order3Tips;
    for (size_t w = 0; w < weepingTwigAnchors.size(); ++w) {
        if (outBranches.size() >= static_cast<size_t>(maxAllowedBranches)) break;

        const auto& anchor = weepingTwigAnchors[w];
        Vec3 perp = cross(anchor.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (perp.lengthSq() < 0.01f) perp = cross(anchor.dir, Vec3(1.0f, 0.0f, 0.0f));
        perp = perp.normalized();

        for (int k = -1; k <= 1; k += 2) {
            if (outBranches.size() >= static_cast<size_t>(maxAllowedBranches)) break;
            float hangLen = (m_treeHeight * 0.13f) * rng.nextRange(0.85f, 1.18f);
            Vec3 droopDir = (anchor.dir * 0.34f + perp * (0.28f * static_cast<float>(k)) + Vec3(0.0f, -0.78f - rng.nextRange(0.0f, 0.15f), 0.0f)).normalized();
            Vec3 droopTip = anchor.pos + droopDir * hangLen;
            float endR = std::max(0.004f, anchor.radius * 0.50f);

            TreeBranchSegment wSeg;
            wSeg.startPos = anchor.pos;
            wSeg.endPos = droopTip;
            wSeg.radiusStart = anchor.radius;
            wSeg.radiusEnd = endR;
            wSeg.recursionDepth = 3;
            wSeg.branchIndex = static_cast<int>(outBranches.size());
            wSeg.windWeight = std::min(1.0f, (anchor.pos.y / m_treeHeight) * 0.90f);
            wSeg.normal = droopDir;
            outBranches.push_back(wSeg);
            order3Tips.push_back({droopTip, droopDir, endR});
        }
    }

    for (size_t i = 0; i < order3Tips.size(); ++i) {
        if (outBranches.size() >= static_cast<size_t>(maxAllowedBranches)) break;
        const auto& o3 = order3Tips[i];
        float shootLen = (m_treeHeight * 0.08f) * rng.nextRange(0.85f, 1.15f);
        Vec3 shootDir = Vec3(
            o3.dir.x * 0.25f + rng.nextRange(-0.18f, 0.18f),
            -0.92f,
            o3.dir.z * 0.25f + rng.nextRange(-0.18f, 0.18f)
        ).normalized();

        TreeBranchSegment s4;
        s4.startPos = o3.pos;
        s4.endPos = o3.pos + shootDir * shootLen;
        s4.radiusStart = o3.radius;
        s4.radiusEnd = 0.003f;
        s4.recursionDepth = 4;
        s4.branchIndex = static_cast<int>(outBranches.size());
        s4.windWeight = 0.96f;
        s4.normal = shootDir;
        outBranches.push_back(s4);
    }

    // 5. Birch cordate foliage placed on cylinder branch edges using Stojanovic (2016)
    int targetLeaves = std::max(1050, m_leafCount);
    generateStojanovicLeaves(
        outBranches,
        outLeaves,
        rng,
        m_leafScale,
        m_leafColorR,
        m_leafColorG,
        m_leafColorB,
        m_treeHeight,
        targetLeaves,
        false, // isGiantFrond = false
        2,     // minRecursionDepth = 2
        m_terminalLeafCount,
        m_realLeafLengthMeters
    );
}

} // namespace vegetation
