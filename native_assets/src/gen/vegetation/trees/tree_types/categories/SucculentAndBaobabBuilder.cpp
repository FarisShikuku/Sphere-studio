// ============================================================================
// SucculentAndBaobabBuilder.cpp
// Candelabra Euphorbia, Pachycaul African Baobab, Desert Rose & Frangipani
// ============================================================================

#include "TreeCategoryBuilders.h"
#include <cmath>
#include <algorithm>

namespace vegetation {
namespace categories {

void buildEuphorbiaCandelabra(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
) {
    CrownControlParams hull = computeSpeciesCrownHull(ctx);
    int trunkSteps = 8;
    float stepH = hull.trunkHeight / static_cast<float>(trunkSteps);
    Vec3 currPos(0.0f, 0.0f, 0.0f);
    float currR = ctx.trunkRadius;

    struct StemTierNode {
        Vec3 pos;
        float radius;
    };
    std::vector<StemTierNode> stemNodes;

    for (int t = 0; t < trunkSteps; ++t) {
        Vec3 nextPos = currPos + Vec3(0.0f, stepH, 0.0f);
        float nextR = (t == trunkSteps - 1) ? std::max(0.035f, currR * 0.65f) : (currR * 0.95f);

        TreeBranchSegment seg;
        seg.startPos = currPos;
        seg.endPos = nextPos;
        seg.radiusStart = currR;
        seg.radiusEnd = nextR;
        seg.recursionDepth = 0;
        seg.branchIndex = static_cast<int>(outBranches.size());
        seg.windWeight = (nextPos.y / ctx.treeHeight) * 0.15f;
        seg.normal = Vec3(0.0f, 1.0f, 0.0f);
        outBranches.push_back(seg);

        if (t >= 3 && t < trunkSteps - 1) {
            stemNodes.push_back({nextPos, nextR});
        }
        currPos = nextPos;
        currR = nextR;
    }

    int armCount = std::clamp(ctx.branchNumber / 2, 10, 16);
    float baseAz = rng.nextFloat01() * 6.28318f;
    size_t numNodes = stemNodes.size();

    for (int a = 0; a < armCount; ++a) {
        size_t nodeIdx = (numNodes > 0) ? (a % numNodes) : 0;
        const auto& node = (numNodes > 0) ? stemNodes[nodeIdx] : StemTierNode{currPos, currR};

        float az = baseAz + static_cast<float>(a) * (6.28318f / static_cast<float>(armCount)) + rng.nextRange(-0.15f, 0.15f);
        float armRadiusStart = node.radius * 0.40f;

        int armSegments = 4;
        float outSpread = (ctx.treeHeight * 0.28f) * rng.nextRange(0.85f, 1.15f);
        float verticalRise = (hull.crownHeight * 0.90f) * rng.nextRange(0.80f, 1.10f);

        Vec3 forkOrigin(0.0f, 0.0f, 0.0f);
        float forkRadius = armRadiusStart * 0.55f;

        for (int s = 0; s < armSegments; ++s) {
            float s0 = static_cast<float>(s) / static_cast<float>(armSegments);
            float s1 = static_cast<float>(s + 1) / static_cast<float>(armSegments);

            float rOut0 = outSpread * std::sin(s0 * 1.57079f);
            float rOut1 = outSpread * std::sin(s1 * 1.57079f);
            float yRise0 = verticalRise * (s0 * s0);
            float yRise1 = verticalRise * (s1 * s1);

            Vec3 p0 = node.pos + Vec3(rOut0 * std::cos(az), yRise0, rOut0 * std::sin(az));
            Vec3 p1 = node.pos + Vec3(rOut1 * std::cos(az), yRise1, rOut1 * std::sin(az));
            float r1 = armRadiusStart * (1.0f - s1 * 0.65f);

            TreeBranchSegment seg;
            seg.startPos = p0;
            seg.endPos = p1;
            seg.radiusStart = armRadiusStart * (1.0f - s0 * 0.65f);
            seg.radiusEnd = r1;
            seg.recursionDepth = 1;
            seg.branchIndex = static_cast<int>(outBranches.size());
            seg.windWeight = std::min(1.0f, (p1.y / ctx.treeHeight) * 0.70f);
            seg.normal = (p1 - p0).normalized();
            outBranches.push_back(seg);

            if (s == 1) {
                forkOrigin = p1;
                forkRadius = r1 * 0.80f;
            }
        }

        // Authentic secondary upright U-forks sprouting off primary candelabra elbows
        if (a % 2 == 0) {
            for (int f = -1; f <= 1; f += 2) {
                float fAz = az + static_cast<float>(f) * 0.52f;
                float subSpread = outSpread * 0.44f;
                float subRise = verticalRise * 0.52f;
                for (int ss = 0; ss < 3; ++ss) {
                    float s0 = static_cast<float>(ss) / 3.0f;
                    float s1 = static_cast<float>(ss + 1) / 3.0f;
                    Vec3 fp0 = forkOrigin + Vec3(subSpread * std::sin(s0 * 1.4f) * std::cos(fAz), subRise * (s0 * s0), subSpread * std::sin(s0 * 1.4f) * std::sin(fAz));
                    Vec3 fp1 = forkOrigin + Vec3(subSpread * std::sin(s1 * 1.4f) * std::cos(fAz), subRise * (s1 * s1), subSpread * std::sin(s1 * 1.4f) * std::sin(fAz));

                    TreeBranchSegment fSeg;
                    fSeg.startPos = fp0;
                    fSeg.endPos = fp1;
                    fSeg.radiusStart = forkRadius * (1.0f - s0 * 0.55f);
                    fSeg.radiusEnd = forkRadius * (1.0f - s1 * 0.55f);
                    fSeg.recursionDepth = 2;
                    fSeg.branchIndex = static_cast<int>(outBranches.size());
                    fSeg.windWeight = std::min(1.0f, (fp1.y / ctx.treeHeight) * 0.85f);
                    fSeg.normal = (fp1 - fp0).normalized();
                    outBranches.push_back(fSeg);
                }
            }
        }
    }

    int spineNodesCount = std::max(armCount * 10, ctx.leafCount);
    TreeTypeBase::generateStojanovicLeaves(
        outBranches, outLeaves, rng,
        ctx.leafScale * 0.75f, ctx.leafColorR, ctx.leafColorG, ctx.leafColorB,
        ctx.treeHeight, spineNodesCount, false, 1, ctx.terminalLeafCount, ctx.realLeafLengthMeters
    );
}

void buildDesertRoseAndFrangipani(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
) {
    const bool isFrangipani = (ctx.speciesId == TreeSpeciesId::FRANGIPANI_PLUMERIA);
    int caudexSteps = 6;
    float caudexHeight = ctx.treeHeight * (isFrangipani ? 0.34f : 0.38f);
    float stepH = caudexHeight / static_cast<float>(caudexSteps);
    Vec3 currPos(0.0f, 0.0f, 0.0f);
    float baseR = ctx.trunkRadius * (isFrangipani ? 1.05f : 1.35f);
    float currR = baseR;

    struct CaudexNode {
        Vec3 pos;
        Vec3 dir;
        float radius;
        float heightFrac;
    };
    std::vector<CaudexNode> caudexNodes;

    for (int t = 0; t < caudexSteps; ++t) {
        float frac = static_cast<float>(t + 1) / static_cast<float>(caudexSteps);
        float belly = isFrangipani ? (1.0f + 0.04f * std::sin(frac * 2.5f)) : (1.0f + 0.28f * std::sin(frac * 2.8f));
        float nextR = baseR * (1.0f - frac * (isFrangipani ? 0.24f : 0.42f)) * belly;

        float nX = veg_noise3d(currPos.x * 0.4f, currPos.y * 0.4f, currPos.z * 0.4f) * (stepH * 0.07f);
        float nZ = veg_noise3d(currPos.x * 0.4f + 14.2f, currPos.y * 0.4f + 6.8f, currPos.z * 0.4f + 22.4f) * (stepH * 0.07f);
        Vec3 nextPos = currPos + Vec3(nX + rng.nextRange(-0.01f, 0.01f) * stepH, stepH, nZ + rng.nextRange(-0.01f, 0.01f) * stepH);

        TreeBranchSegment seg;
        seg.startPos = currPos;
        seg.endPos = nextPos;
        seg.radiusStart = currR;
        seg.radiusEnd = nextR;
        seg.recursionDepth = 0;
        seg.branchIndex = static_cast<int>(outBranches.size());
        seg.windWeight = (nextPos.y / ctx.treeHeight) * 0.06f;
        seg.normal = (nextPos - currPos).normalized();
        outBranches.push_back(seg);

        if (t >= 2) {
            caudexNodes.push_back({nextPos, seg.normal, nextR, frac});
        }
        currPos = nextPos;
        currR = nextR;
    }

    int boughCount = std::clamp(ctx.branchNumber / 4, 4, 6);
    float baseAz = rng.nextFloat01() * 6.283185f;
    size_t numNodes = caudexNodes.size();

    struct SucculentLimb {
        Vec3 tip;
        Vec3 dir;
        Vec3 midPos;
        Vec3 midDir;
        float radius;
        float midRadius;
        float length;
    };
    std::vector<SucculentLimb> primaryLimbs;

    for (int b = 0; b < boughCount; ++b) {
        float frac = static_cast<float>(b) / static_cast<float>(boughCount);
        size_t nIdx = (numNodes > 0) ? std::min(numNodes - 1, static_cast<size_t>(frac * static_cast<float>(numNodes))) : 0;
        const auto& node = (numNodes > 0) ? caudexNodes[nIdx] : CaudexNode{currPos, Vec3(0,1,0), currR, 1.0f};

        float az = baseAz + static_cast<float>(b) * (6.283185f / static_cast<float>(boughCount)) + rng.nextRange(-0.15f, 0.15f);
        float initPitch = (52.0f + rng.nextRange(-4.0f, 4.0f)) * (3.14159265f / 180.0f);
        float finalPitch = (28.0f + rng.nextRange(-3.0f, 3.0f)) * (3.14159265f / 180.0f);

        float limbLen = ctx.treeHeight * (0.38f + rng.nextRange(-0.04f, 0.04f));
        Vec3 lStart = node.pos;
        float lStartR = node.radius * 0.48f;

        int bSteps = 3;
        Vec3 currBPos = lStart;
        float currBR = lStartR;
        Vec3 recordedMidPos = lStart;
        Vec3 recordedMidDir(0.0f, 1.0f, 0.0f);
        float recordedMidR = lStartR;

        for (int s = 0; s < bSteps; ++s) {
            float segFrac = 1.0f / static_cast<float>(bSteps);
            float stepT = static_cast<float>(s + 1) / static_cast<float>(bSteps);
            float curPitch = initPitch * (1.0f - stepT * 0.65f) + finalPitch * (stepT * 0.65f);
            Vec3 segDir = Vec3(std::sin(curPitch) * std::cos(az), std::cos(curPitch), std::sin(curPitch) * std::sin(az)).normalized();

            Vec3 straightEnd = currBPos + segDir * (limbLen * segFrac);
            float gnarl = limbLen * 0.035f;
            float nX = veg_noise3d(straightEnd.x * 0.35f, straightEnd.y * 0.35f, straightEnd.z * 0.35f) * gnarl;
            float nY = veg_noise3d(straightEnd.x * 0.35f + 7.2f, straightEnd.y * 0.35f + 11.0f, straightEnd.z * 0.35f) * (gnarl * 0.3f);
            float nZ = veg_noise3d(straightEnd.x * 0.35f + 15.4f, straightEnd.y * 0.35f + 3.9f, straightEnd.z * 0.35f) * gnarl;
            Vec3 bEnd = straightEnd + Vec3(nX, nY, nZ);
            float nextBR = currBR * (1.0f - segFrac * 0.28f);

            TreeBranchSegment bSeg;
            bSeg.startPos = currBPos;
            bSeg.endPos = bEnd;
            bSeg.radiusStart = currBR;
            bSeg.radiusEnd = nextBR;
            bSeg.recursionDepth = 1;
            bSeg.branchIndex = static_cast<int>(outBranches.size());
            bSeg.windWeight = (bEnd.y / ctx.treeHeight) * 0.30f;
            bSeg.normal = (bEnd - currBPos).normalized();
            outBranches.push_back(bSeg);

            if (s == 1) {
                recordedMidPos = bEnd;
                recordedMidDir = bSeg.normal;
                recordedMidR = nextBR;
            }
            currBPos = bEnd;
            currBR = nextBR;
        }
        primaryLimbs.push_back({currBPos, (currBPos - lStart).normalized(), recordedMidPos, recordedMidDir, currBR, recordedMidR, limbLen});
    }

    struct SucculentTwig {
        Vec3 tip;
        Vec3 dir;
        float radius;
    };
    std::vector<SucculentTwig> secondaryTips;

    for (size_t pi = 0; pi < primaryLimbs.size(); ++pi) {
        const auto& pl = primaryLimbs[pi];
        Vec3 side = cross(pl.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (side.lengthSq() < 1e-4f) side = cross(pl.dir, Vec3(1.0f, 0.0f, 0.0f));
        side = side.normalized();

        // Mid-limb dichotomous spur on primary branches
        {
            float mSign = (pi % 2 == 0) ? 1.0f : -1.0f;
            Vec3 mDir = (pl.midDir * 0.72f + side * (0.62f * mSign) + Vec3(0.0f, 0.16f, 0.0f)).normalized();
            Vec3 mEnd = pl.midPos + mDir * (pl.length * 0.34f);
            float mEndR = pl.midRadius * 0.62f;

            TreeBranchSegment mSeg;
            mSeg.startPos = pl.midPos;
            mSeg.endPos = mEnd;
            mSeg.radiusStart = pl.midRadius * 0.78f;
            mSeg.radiusEnd = mEndR;
            mSeg.recursionDepth = 2;
            mSeg.branchIndex = static_cast<int>(outBranches.size());
            mSeg.windWeight = 0.50f;
            mSeg.normal = mDir;
            outBranches.push_back(mSeg);
            secondaryTips.push_back({mEnd, mDir, mEndR});
        }

        float stemLen = pl.length * 0.42f;
        float stemStartR = pl.radius * 0.68f;

        for (int f = 0; f < 2; ++f) {
            float forkAngle = (f == 0 ? 0.36f : -0.36f) + rng.nextRange(-0.06f, 0.06f);
            Vec3 forkDir = (pl.dir * std::cos(forkAngle) + side * std::sin(forkAngle)).normalized();
            forkDir.y = std::max(0.40f, forkDir.y + 0.12f);
            forkDir = forkDir.normalized();

            Vec3 stStart = pl.tip;
            float stR = stemStartR;
            for (int ss = 0; ss < 2; ++ss) {
                float segLen = stemLen * 0.5f;
                Vec3 stEnd = stStart + forkDir * segLen + Vec3(
                    veg_noise3d(stStart.x * 0.4f, stStart.y * 0.4f, stStart.z * 0.4f) * (segLen * 0.03f),
                    veg_noise3d(stStart.x * 0.4f + 4.1f, stStart.y * 0.4f + 8.2f, stStart.z * 0.4f) * (segLen * 0.015f),
                    veg_noise3d(stStart.x * 0.4f + 12.6f, stStart.y * 0.4f + 2.3f, stStart.z * 0.4f) * (segLen * 0.03f)
                );
                float stEndR = stR * 0.75f;

                TreeBranchSegment stSeg;
                stSeg.startPos = stStart;
                stSeg.endPos = stEnd;
                stSeg.radiusStart = stR;
                stSeg.radiusEnd = stEndR;
                stSeg.recursionDepth = 2;
                stSeg.branchIndex = static_cast<int>(outBranches.size());
                stSeg.windWeight = 0.55f;
                stSeg.normal = forkDir;
                outBranches.push_back(stSeg);
                stStart = stEnd;
                stR = stEndR;
            }
            secondaryTips.push_back({stStart, forkDir, stR});
        }
    }

    const int tipsPerBranch = isFrangipani ? 3 : 2;
    for (const auto& st : secondaryTips) {
        Vec3 side = cross(st.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (side.lengthSq() < 1e-4f) side = cross(st.dir, Vec3(1.0f, 0.0f, 0.0f));
        side = side.normalized();
        float twigLen = ctx.treeHeight * 0.09f;

        for (int tw = 0; tw < tipsPerBranch; ++tw) {
            float dev = (tipsPerBranch == 3)
                ? (static_cast<float>(tw - 1) * 0.34f + rng.nextRange(-0.05f, 0.05f))
                : ((tw == 0 ? 0.28f : -0.28f) + rng.nextRange(-0.05f, 0.05f));
            Vec3 twDir = (st.dir * std::cos(dev) + side * std::sin(dev)).normalized();
            twDir.y = std::max(0.50f, twDir.y + 0.10f);
            twDir = twDir.normalized();

            TreeBranchSegment twSeg;
            twSeg.startPos = st.tip;
            twSeg.endPos = st.tip + twDir * twigLen;
            twSeg.radiusStart = st.radius * 0.65f;
            twSeg.radiusEnd = 0.006f;
            twSeg.recursionDepth = 3;
            twSeg.branchIndex = static_cast<int>(outBranches.size());
            twSeg.windWeight = 0.70f;
            twSeg.normal = twDir;
            outBranches.push_back(twSeg);
        }
    }

    int leafCount = std::max(isFrangipani ? 360 : 220, ctx.leafCount);
    TreeTypeBase::generateStojanovicLeaves(
        outBranches, outLeaves, rng,
        ctx.leafScale * 1.10f, ctx.leafColorR, ctx.leafColorG, ctx.leafColorB,
        ctx.treeHeight, leafCount, false, 2, ctx.terminalLeafCount, ctx.realLeafLengthMeters
    );
}

void buildBaobabPachycaul(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
) {
    int trunkSteps = 8;
    float trunkH = ctx.treeHeight * 0.78f;
    float stepH = trunkH / static_cast<float>(trunkSteps);
    Vec3 currPos(0.0f, 0.0f, 0.0f);
    float baseR = ctx.trunkRadius * 1.35f;
    float currR = baseR;

    struct BaobabNode {
        Vec3 pos;
        Vec3 dir;
        float radius;
        float heightFrac;
    };
    std::vector<BaobabNode> boughNodes;

    for (int t = 0; t < trunkSteps; ++t) {
        float frac = static_cast<float>(t + 1) / static_cast<float>(trunkSteps);
        float belly = 1.0f + 0.12f * std::sin(frac * 2.6f);
        float nextR = baseR * (1.0f - frac * 0.60f) * belly;

        float nX = veg_noise3d(currPos.x * 0.20f, currPos.y * 0.20f, currPos.z * 0.20f) * (stepH * 0.05f);
        float nZ = veg_noise3d(currPos.x * 0.20f + 13.5f, currPos.y * 0.20f + 7.1f, currPos.z * 0.20f + 21.8f) * (stepH * 0.05f);
        Vec3 nextPos = currPos + Vec3(nX + rng.nextRange(-0.01f, 0.01f) * stepH, stepH, nZ + rng.nextRange(-0.01f, 0.01f) * stepH);

        TreeBranchSegment seg;
        seg.startPos = currPos;
        seg.endPos = nextPos;
        seg.radiusStart = currR;
        seg.radiusEnd = nextR;
        seg.recursionDepth = 0;
        seg.branchIndex = static_cast<int>(outBranches.size());
        seg.windWeight = (nextPos.y / ctx.treeHeight) * 0.08f;
        seg.normal = (nextPos - currPos).normalized();
        outBranches.push_back(seg);

        float hFrac = nextPos.y / ctx.treeHeight;
        if (hFrac >= 0.44f && t < trunkSteps - 1) {
            boughNodes.push_back({nextPos, seg.normal, nextR, hFrac});
        }
        currPos = nextPos;
        currR = nextR;
    }

    Vec3 apexPos = currPos;
    float apexR = currR;
    int boughCount = std::clamp(3 + ctx.branchNumber / 14, 4, 6);
    float baseAz = rng.nextFloat01() * 6.283185f;
    size_t numNodes = boughNodes.size();

    struct BaobabLimb {
        Vec3 tip;
        Vec3 dir;
        Vec3 midPos;
        Vec3 midDir;
        bool hasMid;
        float radius;
        float midRadius;
        float length;
    };
    std::vector<BaobabLimb> scaffoldLimbs;

    for (int b = 0; b < boughCount; ++b) {
        float frac = static_cast<float>(b) / static_cast<float>(boughCount);
        size_t nIdx = (numNodes > 0) ? std::min(numNodes - 1, static_cast<size_t>(frac * static_cast<float>(numNodes))) : 0;
        const auto& node = (numNodes > 0) ? boughNodes[nIdx] : BaobabNode{apexPos, Vec3(0,1,0), apexR, 1.0f};

        float az = baseAz + static_cast<float>(b) * (6.283185f / static_cast<float>(boughCount)) + rng.nextRange(-0.15f, 0.15f);
        float pitchDeg = 62.0f - 18.0f * node.heightFrac + rng.nextRange(-4.0f, 4.0f);
        float pitchRad = pitchDeg * (3.14159265f / 180.0f);

        Vec3 dir = Vec3(std::sin(pitchRad) * std::cos(az), std::cos(pitchRad), std::sin(pitchRad) * std::sin(az)).normalized();
        float limbLen = ctx.treeHeight * (0.42f - node.heightFrac * 0.12f) * rng.nextRange(0.90f, 1.10f);
        Vec3 bStart = node.pos;
        float bStartR = node.radius * 0.52f;

        int bSteps = 4;
        Vec3 currBPos = bStart;
        float currBR = bStartR;
        Vec3 recordedMidPos = bStart;
        Vec3 recordedMidDir = dir;
        float recordedMidR = bStartR;

        for (int s = 0; s < bSteps; ++s) {
            float segFrac = 1.0f / static_cast<float>(bSteps);
            float stepT = static_cast<float>(s + 1) / static_cast<float>(bSteps);
            Vec3 curDir = dir;
            curDir.y += stepT * 0.20f;
            curDir = curDir.normalized();

            Vec3 straightEnd = currBPos + curDir * (limbLen * segFrac);
            float gnarlAmp = limbLen * 0.055f;
            float nX = veg_noise3d(straightEnd.x * 0.30f, straightEnd.y * 0.30f, straightEnd.z * 0.30f) * gnarlAmp;
            float nY = veg_noise3d(straightEnd.x * 0.30f + 8.4f, straightEnd.y * 0.30f + 14.2f, straightEnd.z * 0.30f) * (gnarlAmp * 0.45f);
            float nZ = veg_noise3d(straightEnd.x * 0.30f + 22.1f, straightEnd.y * 0.30f + 5.6f, straightEnd.z * 0.30f) * gnarlAmp;
            Vec3 bEnd = straightEnd + Vec3(nX, nY, nZ);
            float nextBR = currBR * (1.0f - segFrac * 0.22f);

            TreeBranchSegment bSeg;
            bSeg.startPos = currBPos;
            bSeg.endPos = bEnd;
            bSeg.radiusStart = currBR;
            bSeg.radiusEnd = nextBR;
            bSeg.recursionDepth = 1;
            bSeg.branchIndex = static_cast<int>(outBranches.size());
            bSeg.windWeight = std::min(1.0f, (bEnd.y / ctx.treeHeight) * 0.35f);
            bSeg.normal = (bEnd - currBPos).normalized();
            outBranches.push_back(bSeg);

            if (s == 1) {
                recordedMidPos = bEnd;
                recordedMidDir = bSeg.normal;
                recordedMidR = nextBR;
            }
            currBPos = bEnd;
            currBR = nextBR;
        }
        scaffoldLimbs.push_back({currBPos, (currBPos - bStart).normalized(), recordedMidPos, recordedMidDir, true, currBR, recordedMidR, limbLen});
    }

    for (int a = 0; a < 3; ++a) {
        float az = baseAz + static_cast<float>(a) * (6.283185f / 3.0f) + rng.nextRange(-0.12f, 0.12f);
        float pitchRad = (32.0f + rng.nextRange(-4.0f, 4.0f)) * (3.14159265f / 180.0f);
        Vec3 dir = Vec3(std::sin(pitchRad) * std::cos(az), std::cos(pitchRad), std::sin(pitchRad) * std::sin(az)).normalized();

        float limbLen = ctx.treeHeight * 0.22f;
        Vec3 aEnd = apexPos + dir * limbLen;
        float endR = apexR * 0.65f;

        TreeBranchSegment aSeg;
        aSeg.startPos = apexPos;
        aSeg.endPos = aEnd;
        aSeg.radiusStart = apexR * 0.65f;
        aSeg.radiusEnd = endR * 0.70f;
        aSeg.recursionDepth = 1;
        aSeg.branchIndex = static_cast<int>(outBranches.size());
        aSeg.windWeight = 0.50f;
        aSeg.normal = dir;
        outBranches.push_back(aSeg);
        scaffoldLimbs.push_back({aEnd, dir, aEnd, dir, false, endR * 0.70f, endR * 0.70f, limbLen});
    }

    struct BaobabTwig {
        Vec3 tip;
        Vec3 dir;
        float radius;
    };
    std::vector<BaobabTwig> secondaryTips;

    for (size_t li = 0; li < scaffoldLimbs.size(); ++li) {
        const auto& limb = scaffoldLimbs[li];
        Vec3 side = cross(limb.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (side.lengthSq() < 1e-4f) side = cross(limb.dir, Vec3(1.0f, 0.0f, 0.0f));
        side = side.normalized();
        Vec3 up = cross(limb.dir, side).normalized();

        if (limb.hasMid) {
            float mSign = (li % 2 == 0) ? 1.0f : -1.0f;
            Vec3 mDir = (limb.midDir * 0.68f + side * (0.64f * mSign) + Vec3(0.0f, 0.22f, 0.0f)).normalized();
            Vec3 mEnd = limb.midPos + mDir * (limb.length * 0.34f);
            float mEndR = limb.midRadius * 0.56f;

            TreeBranchSegment mSeg;
            mSeg.startPos = limb.midPos;
            mSeg.endPos = mEnd;
            mSeg.radiusStart = limb.midRadius * 0.75f;
            mSeg.radiusEnd = mEndR;
            mSeg.recursionDepth = 2;
            mSeg.branchIndex = static_cast<int>(outBranches.size());
            mSeg.windWeight = 0.58f;
            mSeg.normal = mDir;
            outBranches.push_back(mSeg);
            secondaryTips.push_back({mEnd, mDir, mEndR});
        }

        float subLen = limb.length * 0.38f;
        float subStartR = limb.radius * 0.65f;

        for (int f = 0; f < 2; ++f) {
            float phi = (f == 0 ? 0.70f : -0.70f) + rng.nextRange(-0.15f, 0.15f);
            float dev = 0.38f + rng.nextRange(-0.06f, 0.06f);
            Vec3 subDir = (limb.dir * std::cos(dev) + (side * std::cos(phi) + up * std::sin(phi)) * std::sin(dev)).normalized();
            subDir.y = std::max(0.20f, subDir.y + 0.15f);
            subDir = subDir.normalized();

            Vec3 subStart = limb.tip;
            float subR = subStartR;
            for (int ss = 0; ss < 2; ++ss) {
                float segLen = subLen * 0.5f;
                Vec3 subEnd = subStart + subDir * segLen + Vec3(
                    veg_noise3d(subStart.x * 0.35f, subStart.y * 0.35f, subStart.z * 0.35f) * (segLen * 0.04f),
                    veg_noise3d(subStart.x * 0.35f + 4.1f, subStart.y * 0.35f + 9.3f, subStart.z * 0.35f) * (segLen * 0.02f),
                    veg_noise3d(subStart.x * 0.35f + 18.2f, subStart.y * 0.35f + 3.4f, subStart.z * 0.35f) * (segLen * 0.04f)
                );
                float subEndR = subR * 0.75f;

                TreeBranchSegment subSeg;
                subSeg.startPos = subStart;
                subSeg.endPos = subEnd;
                subSeg.radiusStart = subR;
                subSeg.radiusEnd = subEndR;
                subSeg.recursionDepth = 2;
                subSeg.branchIndex = static_cast<int>(outBranches.size());
                subSeg.windWeight = 0.62f;
                subSeg.normal = subDir;
                outBranches.push_back(subSeg);
                subStart = subEnd;
                subR = subEndR;
            }
            secondaryTips.push_back({subStart, subDir, subR});
        }
    }

    std::vector<BaobabTwig> order3Tips;
    for (const auto& st : secondaryTips) {
        Vec3 side = cross(st.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (side.lengthSq() < 1e-4f) side = cross(st.dir, Vec3(1.0f, 0.0f, 0.0f));
        side = side.normalized();
        float twigLen = ctx.treeHeight * 0.11f;

        for (int tw = 0; tw < 2; ++tw) {
            float dev = (tw == 0 ? 0.32f : -0.32f) + rng.nextRange(-0.06f, 0.06f);
            Vec3 twDir = (st.dir * std::cos(dev) + side * std::sin(dev)).normalized();
            twDir.y = std::max(0.25f, twDir.y + 0.12f);
            twDir = twDir.normalized();

            Vec3 twEnd = st.tip + twDir * twigLen;
            float twEndR = std::max(0.006f, st.radius * 0.38f);

            TreeBranchSegment twSeg;
            twSeg.startPos = st.tip;
            twSeg.endPos = twEnd;
            twSeg.radiusStart = st.radius * 0.60f;
            twSeg.radiusEnd = twEndR;
            twSeg.recursionDepth = 3;
            twSeg.branchIndex = static_cast<int>(outBranches.size());
            twSeg.windWeight = 0.80f;
            twSeg.normal = twDir;
            outBranches.push_back(twSeg);
            order3Tips.push_back({twEnd, twDir, twEndR});
        }
    }

    for (const auto& o3 : order3Tips) {
        Vec3 side = cross(o3.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (side.lengthSq() < 1e-4f) side = cross(o3.dir, Vec3(1.0f, 0.0f, 0.0f));
        side = side.normalized();
        float shootLen = ctx.treeHeight * 0.062f;

        for (int f = 0; f < 2; ++f) {
            float dev = (f == 0 ? 0.28f : -0.28f) + rng.nextRange(-0.05f, 0.05f);
            Vec3 shDir = (o3.dir * std::cos(dev) + side * std::sin(dev)).normalized();
            shDir.y = std::max(0.22f, shDir.y + 0.12f);
            shDir = shDir.normalized();

            TreeBranchSegment shSeg;
            shSeg.startPos = o3.tip;
            shSeg.endPos = o3.tip + shDir * shootLen;
            shSeg.radiusStart = o3.radius;
            shSeg.radiusEnd = 0.004f;
            shSeg.recursionDepth = 4;
            shSeg.branchIndex = static_cast<int>(outBranches.size());
            shSeg.windWeight = 0.90f;
            shSeg.normal = shDir;
            outBranches.push_back(shSeg);
        }
    }

    int rosetteCount = std::max(1050, ctx.leafCount);
    TreeTypeBase::generateStojanovicLeaves(
        outBranches, outLeaves, rng,
        ctx.leafScale * 1.05f, ctx.leafColorR, ctx.leafColorG, ctx.leafColorB,
        ctx.treeHeight, rosetteCount, false, 2, ctx.terminalLeafCount, ctx.realLeafLengthMeters
    );
}

} // namespace categories
} // namespace vegetation
