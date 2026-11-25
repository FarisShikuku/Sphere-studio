// ============================================================================
// SavannaAndBanyanBuilder.cpp
// Savanna Umbrella Acacias, Flamboyant, Aerial Stilt Mangrove, & Banyan Figs
// ============================================================================

#include "TreeCategoryBuilders.h"
#include <cmath>
#include <algorithm>

namespace vegetation {
namespace categories {

void buildSavannaUmbrella(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
) {
    int limbCount = std::clamp(4 + (ctx.branchNumber - 28) / 8, 4, 7);
    int trunkSteps = 10;
    float trunkH = ctx.treeHeight * 0.72f;
    float stepH = trunkH / static_cast<float>(trunkSteps);
    float clearBoleFrac = (ctx.speciesId == TreeSpeciesId::YELLOW_FEVER_TREE) ? 0.48f
                        : (ctx.speciesId == TreeSpeciesId::WHISTLING_THORN)    ? 0.32f
                        : (ctx.speciesId == TreeSpeciesId::FLAMBOYANT_POINCIANA) ? 0.38f
                        : 0.42f;

    Vec3 currPos(0.0f, 0.0f, 0.0f);
    float currR = ctx.trunkRadius * 1.25f;

    struct TrunkAttachmentNode {
        Vec3 pos;
        Vec3 dir;
        float radius;
        float heightFrac;
    };
    std::vector<TrunkAttachmentNode> trunkNodes;

    for (int t = 0; t < trunkSteps; ++t) {
        float nX = veg_noise3d(currPos.x * 0.35f, currPos.y * 0.35f, currPos.z * 0.35f) * (stepH * 0.08f);
        float nZ = veg_noise3d(currPos.x * 0.35f + 14.7f, currPos.y * 0.35f + 6.3f, currPos.z * 0.35f + 27.1f) * (stepH * 0.08f);
        Vec3 nextPos = currPos + Vec3(nX + rng.nextRange(-0.012f, 0.012f) * stepH, stepH, nZ + rng.nextRange(-0.012f, 0.012f) * stepH);
        float hFrac = nextPos.y / ctx.treeHeight;
        float stepTaper = (hFrac < clearBoleFrac) ? 0.96f : 0.90f;
        float nextR = (t == trunkSteps - 1) ? std::max(0.012f, currR * 0.45f) : (currR * stepTaper);

        TreeBranchSegment seg;
        seg.startPos = currPos;
        seg.endPos = nextPos;
        seg.radiusStart = currR;
        seg.radiusEnd = nextR;
        seg.recursionDepth = 0;
        seg.branchIndex = static_cast<int>(outBranches.size());
        seg.windWeight = (nextPos.y / ctx.treeHeight) * 0.12f;
        seg.normal = (nextPos - currPos).normalized();
        outBranches.push_back(seg);

        if (hFrac >= clearBoleFrac && t < trunkSteps - 1) {
            float crownT = std::clamp((hFrac - clearBoleFrac) / std::max(0.12f, 0.70f - clearBoleFrac), 0.0f, 1.0f);
            trunkNodes.push_back({nextPos, seg.normal, nextR, crownT});
        }
        currPos = nextPos;
        currR = nextR;
    }

    float baseAz = rng.nextFloat01() * 6.283185f;
    size_t numNodes = trunkNodes.size();
    float lBottom = ctx.treeHeight * 0.56f;
    float lTop = ctx.treeHeight * 0.25f;
    const int maxAllowedBranches = std::clamp(ctx.branchNumber * 7, 210, 380);

    struct LimbTip {
        Vec3 tip;
        Vec3 dir;
        Vec3 proxPos;
        Vec3 proxDir;
        Vec3 midPos;
        Vec3 midDir;
        float radius;
        float length;
    };
    std::vector<LimbTip> limbTips;

    for (int l = 0; l < limbCount; ++l) {
        float heightFrac = static_cast<float>(l) / static_cast<float>(std::max(1, limbCount - 1));
        size_t nodeIdx = (numNodes > 0)
            ? std::min(numNodes - 1, static_cast<size_t>(heightFrac * static_cast<float>(numNodes)))
            : 0;
        const auto& node = (numNodes > 0) ? trunkNodes[nodeIdx] : TrunkAttachmentNode{currPos, Vec3(0,1,0), currR, heightFrac};

        float az = baseAz + static_cast<float>(l) * TreeTypeBase::GOLDEN_ANGLE_RAD + rng.nextRange(-0.08f, 0.08f);
        float pitchDeg = 90.0f - 20.0f * heightFrac + rng.nextRange(-1.5f, 1.5f);
        float pitchRad = pitchDeg * (3.14159265f / 180.0f);
        float limbLen = (lBottom - (lBottom - lTop) * std::pow(heightFrac, 0.85f)) * rng.nextRange(0.94f, 1.06f);

        Vec3 dir = Vec3(std::sin(pitchRad) * std::cos(az), std::cos(pitchRad), std::sin(pitchRad) * std::sin(az)).normalized();

        int limbSteps = 3;
        Vec3 lStart = node.pos;
        float lStartR = node.radius * 0.52f * std::sqrt(limbLen / lBottom);
        Vec3 recordedProxPos = lStart;
        Vec3 recordedProxDir = dir;
        Vec3 recordedMidPos = lStart;
        Vec3 recordedMidDir = dir;

        for (int s = 0; s < limbSteps; ++s) {
            float t1 = static_cast<float>(s + 1) / static_cast<float>(limbSteps);
            float segFraction = 1.0f / static_cast<float>(limbSteps);
            Vec3 straightEnd = node.pos + dir * (limbLen * t1);

            float gnarlAmp = (s < limbSteps - 1) ? (limbLen * 0.040f) : 0.0f;
            float nX = veg_noise3d(straightEnd.x * 0.35f, straightEnd.y * 0.35f, straightEnd.z * 0.35f) * gnarlAmp;
            float nY = veg_noise3d(straightEnd.x * 0.35f + 11.1f, straightEnd.y * 0.35f + 8.9f, straightEnd.z * 0.35f) * (gnarlAmp * 0.25f);
            float nZ = veg_noise3d(straightEnd.x * 0.35f + 23.5f, straightEnd.y * 0.35f + 17.3f, straightEnd.z * 0.35f) * gnarlAmp;

            Vec3 lEnd = straightEnd + Vec3(nX, nY, nZ);
            float lEndR = lStartR * (1.0f - segFraction * 0.35f);

            TreeBranchSegment lSeg;
            lSeg.startPos = lStart;
            lSeg.endPos = lEnd;
            lSeg.radiusStart = lStartR;
            lSeg.radiusEnd = lEndR;
            lSeg.recursionDepth = 1;
            lSeg.branchIndex = static_cast<int>(outBranches.size());
            lSeg.windWeight = std::min(1.0f, (lEnd.y / ctx.treeHeight) * 0.60f);
            lSeg.normal = (lEnd - lStart).normalized();
            outBranches.push_back(lSeg);

            if (s == 0) {
                recordedProxPos = lEnd;
                recordedProxDir = lSeg.normal;
            } else if (s == 1) {
                recordedMidPos = lEnd;
                recordedMidDir = lSeg.normal;
            }
            lStart = lEnd;
            lStartR = lEndR;
        }
        limbTips.push_back({lStart, dir, recordedProxPos, recordedProxDir, recordedMidPos, recordedMidDir, lStartR, limbLen});
    }

    struct SiblingTip {
        Vec3 tip;
        Vec3 dir;
        float radius;
    };
    std::vector<SiblingTip> siblingTips;

    for (int ap = 0; ap < 3; ++ap) {
        float apAz = baseAz + static_cast<float>(ap) * (6.283185f / 3.0f) + rng.nextRange(-0.2f, 0.2f);
        Vec3 apDir = Vec3(std::cos(apAz) * 0.96f, 0.28f, std::sin(apAz) * 0.96f).normalized();
        float apLen = lTop * 0.48f;
        Vec3 apTip = currPos + apDir * apLen;
        float apR = std::max(0.010f, currR * 0.72f);

        TreeBranchSegment apSeg;
        apSeg.startPos = currPos;
        apSeg.endPos = apTip;
        apSeg.radiusStart = apR;
        apSeg.radiusEnd = apR * 0.65f;
        apSeg.recursionDepth = 2;
        apSeg.branchIndex = static_cast<int>(outBranches.size());
        apSeg.windWeight = 0.78f;
        apSeg.normal = apDir;
        outBranches.push_back(apSeg);
        siblingTips.push_back({apTip, apDir, apSeg.radiusEnd});
    }

    for (size_t li = 0; li < limbTips.size(); ++li) {
        const auto& lt = limbTips[li];
        Vec3 side = cross(lt.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (side.lengthSq() < 1e-4f) side = cross(lt.dir, Vec3(1.0f, 0.0f, 0.0f));
        side = side.normalized();

        // Proximal mid-canopy fill spray
        {
            float proxSign = (li % 2 == 0) ? 1.0f : -1.0f;
            Vec3 proxDir = (lt.proxDir * 0.74f + side * (0.58f * proxSign)).normalized();
            proxDir.y = std::clamp(proxDir.y * 0.42f + 0.08f, 0.04f, 0.22f);
            proxDir = proxDir.normalized();

            float proxLen = lt.length * 0.28f;
            Vec3 proxEnd = lt.proxPos + proxDir * proxLen;
            float proxR = lt.radius * 0.88f;

            TreeBranchSegment pSeg;
            pSeg.startPos = lt.proxPos;
            pSeg.endPos = proxEnd;
            pSeg.radiusStart = proxR;
            pSeg.radiusEnd = proxR * 0.62f;
            pSeg.recursionDepth = 2;
            pSeg.branchIndex = static_cast<int>(outBranches.size());
            pSeg.windWeight = 0.68f;
            pSeg.normal = proxDir;
            outBranches.push_back(pSeg);
            siblingTips.push_back({proxEnd, proxDir, proxR * 0.62f});
        }

        // Bilateral mid-limb umbrella sprays
        for (int latSide = -1; latSide <= 1; latSide += 2) {
            float sideDev = static_cast<float>(latSide) * 0.48f + rng.nextRange(-0.06f, 0.06f);
            Vec3 latDir = (lt.midDir * std::cos(0.40f) + side * std::sin(sideDev)).normalized();
            latDir.y = std::clamp(latDir.y * 0.38f + 0.06f, 0.02f, 0.16f);
            latDir = latDir.normalized();

            float latLen = lt.length * 0.33f * rng.nextRange(0.88f, 1.12f);
            Vec3 latEnd = lt.midPos + latDir * latLen;
            float latR = lt.radius * 0.82f;

            TreeBranchSegment latSeg;
            latSeg.startPos = lt.midPos;
            latSeg.endPos = latEnd;
            latSeg.radiusStart = latR;
            latSeg.radiusEnd = latR * 0.60f;
            latSeg.recursionDepth = 2;
            latSeg.branchIndex = static_cast<int>(outBranches.size());
            latSeg.windWeight = 0.72f;
            latSeg.normal = latDir;
            outBranches.push_back(latSeg);
            siblingTips.push_back({latEnd, latDir, latR * 0.60f});
        }

        float sibLen = lt.length * 0.38f;
        float sibStartR = lt.radius * 0.68f;
        for (int k = 0; k < 2; ++k) {
            float fanAngle = (k == 0 ? -0.36f : 0.36f) + rng.nextRange(-0.05f, 0.05f);
            Vec3 sibDir = (lt.dir * std::cos(fanAngle) + side * std::sin(fanAngle)).normalized();
            sibDir.y = std::clamp(sibDir.y * 0.38f + 0.06f, 0.02f, 0.18f);
            sibDir = sibDir.normalized();

            Vec3 sStart = lt.tip;
            float sR = sibStartR;
            for (int ss = 0; ss < 2; ++ss) {
                float segLen = sibLen * 0.5f;
                Vec3 sEnd = sStart + sibDir * segLen + Vec3(
                    veg_noise3d(sStart.x * 0.4f, sStart.y * 0.4f, sStart.z * 0.4f) * (segLen * 0.035f),
                    veg_noise3d(sStart.x * 0.4f + 3.1f, sStart.y * 0.4f + 7.2f, sStart.z * 0.4f) * (segLen * 0.015f),
                    veg_noise3d(sStart.x * 0.4f + 11.4f, sStart.y * 0.4f + 1.8f, sStart.z * 0.4f) * (segLen * 0.035f)
                );
                float sEndR = sR * 0.78f;

                TreeBranchSegment sSeg;
                sSeg.startPos = sStart;
                sSeg.endPos = sEnd;
                sSeg.radiusStart = sR;
                sSeg.radiusEnd = sEndR;
                sSeg.recursionDepth = 2;
                sSeg.branchIndex = static_cast<int>(outBranches.size());
                sSeg.windWeight = 0.78f;
                sSeg.normal = sibDir;
                outBranches.push_back(sSeg);
                sStart = sEnd;
                sR = sEndR;
            }
            siblingTips.push_back({sStart, sibDir, sR});
        }
    }

    std::vector<SiblingTip> order3Tips;
    for (const auto& st : siblingTips) {
        if (outBranches.size() >= static_cast<size_t>(maxAllowedBranches)) break;
        Vec3 side = cross(st.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (side.lengthSq() < 1e-4f) side = cross(st.dir, Vec3(1.0f, 0.0f, 0.0f));
        side = side.normalized();
        float twigLen = ctx.treeHeight * 0.095f;

        for (int tw = 0; tw < 2; ++tw) {
            if (outBranches.size() >= static_cast<size_t>(maxAllowedBranches)) break;
            float dev = (tw == 0 ? 0.34f : -0.34f) + rng.nextRange(-0.05f, 0.05f);
            Vec3 twDir = (st.dir * std::cos(dev) + side * std::sin(dev)).normalized();
            twDir.y = std::clamp(twDir.y * 0.30f + 0.04f, 0.02f, 0.14f);
            twDir = twDir.normalized();

            Vec3 twEnd = st.tip + twDir * twigLen;
            float twEndR = std::max(0.005f, st.radius * 0.42f);

            TreeBranchSegment twSeg;
            twSeg.startPos = st.tip;
            twSeg.endPos = twEnd;
            twSeg.radiusStart = st.radius * 0.65f;
            twSeg.radiusEnd = twEndR;
            twSeg.recursionDepth = 3;
            twSeg.branchIndex = static_cast<int>(outBranches.size());
            twSeg.windWeight = 0.88f;
            twSeg.normal = twDir;
            outBranches.push_back(twSeg);
            order3Tips.push_back({twEnd, twDir, twEndR});
        }
    }

    // Order 4 flat-topped terminal canopy twiglets for authentic layered savanna parasol density
    for (const auto& o3 : order3Tips) {
        if (outBranches.size() >= static_cast<size_t>(maxAllowedBranches)) break;
        Vec3 side = cross(o3.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (side.lengthSq() < 1e-4f) side = cross(o3.dir, Vec3(1.0f, 0.0f, 0.0f));
        side = side.normalized();
        float shootLen = ctx.treeHeight * 0.055f;

        for (int f = 0; f < 2; ++f) {
            if (outBranches.size() >= static_cast<size_t>(maxAllowedBranches)) break;
            float dev = (f == 0 ? 0.30f : -0.30f) + rng.nextRange(-0.05f, 0.05f);
            Vec3 shDir = (o3.dir * std::cos(dev) + side * std::sin(dev)).normalized();
            shDir.y = std::clamp(shDir.y * 0.25f + 0.03f, 0.01f, 0.12f);
            shDir = shDir.normalized();

            TreeBranchSegment shSeg;
            shSeg.startPos = o3.tip;
            shSeg.endPos = o3.tip + shDir * shootLen;
            shSeg.radiusStart = o3.radius;
            shSeg.radiusEnd = 0.003f;
            shSeg.recursionDepth = 4;
            shSeg.branchIndex = static_cast<int>(outBranches.size());
            shSeg.windWeight = 0.94f;
            shSeg.normal = shDir;
            outBranches.push_back(shSeg);
        }
    }

    int leafCount = std::max(1150, ctx.leafCount);
    TreeTypeBase::generateStojanovicLeaves(
        outBranches, outLeaves, rng,
        ctx.leafScale, ctx.leafColorR, ctx.leafColorG, ctx.leafColorB,
        ctx.treeHeight, leafCount, false, 2, ctx.terminalLeafCount, ctx.realLeafLengthMeters
    );
}

void buildMangroveStiltRoots(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
) {
    CrownControlParams hull = computeSpeciesCrownHull(ctx);
    float trunkH = hull.trunkHeight;

    int stiltCount = 8;
    for (int s = 0; s < stiltCount; ++s) {
        float az = static_cast<float>(s) * (6.28318f / static_cast<float>(stiltCount)) + rng.nextRange(-0.18f, 0.18f);
        float footDist = ctx.treeHeight * rng.nextRange(0.20f, 0.28f);
        float stiltH = trunkH * (0.28f + (static_cast<float>(s) / static_cast<float>(stiltCount)) * 0.45f);
        Vec3 rootOrigin(0.0f, stiltH, 0.0f);

        int rootSteps = 3;
        Vec3 rPrev = rootOrigin;
        float rRadius = ctx.trunkRadius * 0.42f;
        for (int rs = 1; rs <= rootSteps; ++rs) {
            float t = static_cast<float>(rs) / static_cast<float>(rootSteps);
            float radialFrac = std::sin(t * 1.570796f);
            float yFrac = 1.0f - (t * t);
            Vec3 rNext(std::cos(az) * (footDist * radialFrac), stiltH * yFrac, std::sin(az) * (footDist * radialFrac));
            float rNextRad = ctx.trunkRadius * (0.42f - t * 0.12f);

            TreeBranchSegment rSeg;
            rSeg.startPos = rPrev;
            rSeg.endPos = rNext;
            rSeg.radiusStart = rRadius;
            rSeg.radiusEnd = rNextRad;
            rSeg.recursionDepth = 0;
            rSeg.branchIndex = static_cast<int>(outBranches.size());
            rSeg.windWeight = (rPrev.y / ctx.treeHeight) * 0.04f;
            rSeg.normal = (rNext - rPrev).normalized();
            outBranches.push_back(rSeg);

            rPrev = rNext;
            rRadius = rNextRad;
        }
    }

    buildDecurrentBroadleaf(ctx, outBranches, outLeaves, rng);
}

void buildBanyanStranglerFig(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
) {
    // Mugumo Sacred Fig (Ficus thonningii) & Sycamore Fig (Ficus sycomorus):
    // Buttressed trunk bole rising to wide horizontal spreading boughs + hanging aerial drop/pillar roots
    buildDecurrentBroadleaf(ctx, outBranches, outLeaves, rng);

    // Add iconic vertical hanging aerial drop roots from inner horizontal primary limbs (depth == 1)
    int dropRootsAdded = 0;
    const int maxDropRoots = 6;
    for (size_t i = 0; i < outBranches.size() && dropRootsAdded < maxDropRoots; ++i) {
        const auto& b = outBranches[i];
        if (b.recursionDepth != 1) continue;
        float horizR = std::hypot(b.endPos.x, b.endPos.z);
        if (horizR < ctx.treeHeight * 0.10f || b.endPos.y < ctx.treeHeight * 0.35f) continue;

        Vec3 anchor = lerp(b.startPos, b.endPos, 0.55f);
        bool reachesSoil = (dropRootsAdded % 2 == 0);
        float bottomY = reachesSoil ? 0.0f : anchor.y * rng.nextRange(0.28f, 0.52f);

        int steps = 3;
        Vec3 cur = anchor;
        float curR = reachesSoil ? std::clamp(b.radiusEnd * 0.36f, 0.032f, 0.085f)
                                 : std::clamp(b.radiusEnd * 0.22f, 0.012f, 0.035f);
        for (int st = 1; st <= steps; ++st) {
            float frac = static_cast<float>(st) / static_cast<float>(steps);
            float y = anchor.y + (bottomY - anchor.y) * frac;
            Vec3 next(
                anchor.x + rng.nextRange(-0.04f, 0.04f),
                y,
                anchor.z + rng.nextRange(-0.04f, 0.04f)
            );
            float nextR = reachesSoil ? (curR * 1.04f) : std::max(0.005f, curR * 0.72f);

            TreeBranchSegment dr;
            dr.startPos = cur;
            dr.endPos = next;
            dr.radiusStart = curR;
            dr.radiusEnd = nextR;
            dr.recursionDepth = 0; // Woody trunk/prop root cylinder
            dr.branchIndex = static_cast<int>(outBranches.size());
            dr.windWeight = reachesSoil ? (1.0f - frac) * 0.08f : 0.25f;
            dr.normal = (next - cur).normalized();
            outBranches.push_back(dr);

            cur = next;
            curR = nextR;
        }
        dropRootsAdded++;
    }
}

} // namespace categories
} // namespace vegetation
