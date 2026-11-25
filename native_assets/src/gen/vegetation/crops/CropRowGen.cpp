// ============================================================================
// CropRowGen.cpp - Agricultural Furrow, Stalk, Leaf & Harvest Scene Engine
// ============================================================================

#include "CropRowGen.h"
#include <algorithm>
#include <cmath>

namespace vegetation {

namespace {
constexpr float kPi = 3.1415926535f;

inline Vec3 safeNorm(const Vec3& v, const Vec3& fallback = Vec3(0.0f, 1.0f, 0.0f)) {
    float l2 = v.lengthSq();
    if (l2 < 1e-8f) return fallback;
    return v * (1.0f / std::sqrt(l2));
}
} // namespace

CropSynthesisParams getPredefinedCropParams(CropArchetype archetype) {
    CropSynthesisParams p;
    p.archetype = archetype;
    switch (archetype) {
        case CropArchetype::CORN_MAIZE:
            p.rowSpacingMeters = 0.85f;
            p.plantIntervalMeters = 0.38f;
            p.cropHeightMeters = 2.15f;
            p.furrowDepthMeters = 0.16f;
            p.rowCount = 6;
            p.stalksPerRow = 10;
            break;
        case CropArchetype::WHEAT_FIELD:
            p.rowSpacingMeters = 0.42f;
            p.plantIntervalMeters = 0.24f;
            p.cropHeightMeters = 1.05f;
            p.furrowDepthMeters = 0.08f;
            p.rowCount = 8;
            p.stalksPerRow = 12;
            break;
        case CropArchetype::LAVENDER_ROWS:
            p.rowSpacingMeters = 1.15f;
            p.plantIntervalMeters = 0.55f;
            p.cropHeightMeters = 0.72f;
            p.furrowDepthMeters = 0.12f;
            p.rowCount = 6;
            p.stalksPerRow = 10;
            break;
        case CropArchetype::VINEYARD_TRELLIS:
            p.rowSpacingMeters = 1.85f;
            p.plantIntervalMeters = 0.95f;
            p.cropHeightMeters = 1.85f;
            p.furrowDepthMeters = 0.14f;
            p.rowCount = 4;
            p.stalksPerRow = 6;
            break;
        case CropArchetype::POTATO_FURROWS:
            p.rowSpacingMeters = 0.80f;
            p.plantIntervalMeters = 0.44f;
            p.cropHeightMeters = 0.58f;
            p.furrowDepthMeters = 0.24f;
            p.rowCount = 6;
            p.stalksPerRow = 10;
            break;
        case CropArchetype::BANANA_PLANTATION:
            p.rowSpacingMeters = 1.65f;
            p.plantIntervalMeters = 1.15f;
            p.cropHeightMeters = 2.65f;
            p.furrowDepthMeters = 0.16f;
            p.rowCount = 4;
            p.stalksPerRow = 5;
            break;
        case CropArchetype::CASSAVA_FIELD:
            p.rowSpacingMeters = 0.95f;
            p.plantIntervalMeters = 0.55f;
            p.cropHeightMeters = 1.55f;
            p.furrowDepthMeters = 0.20f;
            p.rowCount = 5;
            p.stalksPerRow = 8;
            break;
        case CropArchetype::COFFEE_PLANTATION:
            p.rowSpacingMeters = 1.25f;
            p.plantIntervalMeters = 0.75f;
            p.cropHeightMeters = 1.65f;
            p.furrowDepthMeters = 0.12f;
            p.rowCount = 5;
            p.stalksPerRow = 7;
            break;
        case CropArchetype::TEA_CONTOUR_HEDGES:
            p.rowSpacingMeters = 0.85f;
            p.plantIntervalMeters = 0.38f;
            p.cropHeightMeters = 0.95f;
            p.furrowDepthMeters = 0.14f;
            p.rowCount = 6;
            p.stalksPerRow = 12;
            break;
        case CropArchetype::SUGARCANE_ROWS:
            p.rowSpacingMeters = 1.05f;
            p.plantIntervalMeters = 0.42f;
            p.cropHeightMeters = 3.10f;
            p.furrowDepthMeters = 0.18f;
            p.rowCount = 5;
            p.stalksPerRow = 10;
            break;
    }
    p.rowLengthMeters = p.stalksPerRow * p.plantIntervalMeters;
    return p;
}

CropFieldMesh CropRowGen::generateCropField(const CropSynthesisParams& params, uint64_t seed) {
    FastRng rng(seed);
    CropFieldMesh mesh;

    int latSteps = std::max(4, params.rowCount) * 4;
    int longSteps = 32;
    float totalWidth = std::max(1, params.rowCount) * params.rowSpacingMeters;

    for (int j = 0; j <= longSteps; ++j) {
        float z = (static_cast<float>(j) / longSteps - 0.5f) * params.rowLengthMeters;
        for (int i = 0; i <= latSteps; ++i) {
            float u = static_cast<float>(i) / latSteps;
            float x = (u - 0.5f) * totalWidth;
            float furrowSin = std::sin(u * params.rowCount * 2.0f * kPi);
            float y = furrowSin * params.furrowDepthMeters;

            Vec3 normal(-furrowSin * 0.4f, 1.0f, 0.0f);
            normal = normal.normalized();

            mesh.furrowVertices.push_back({
                {x, y, z},
                normal.toC(),
                {u, static_cast<float>(j) / longSteps},
                {0.38f, 0.28f, 0.18f, 1.0f},
                0.0f
            });
        }
    }

    for (int j = 0; j < longSteps; ++j) {
        for (int i = 0; i < latSteps; ++i) {
            uint32_t i0 = j * (latSteps + 1) + i;
            uint32_t i1 = i0 + 1;
            uint32_t i2 = (j + 1) * (latSteps + 1) + i;
            uint32_t i3 = i2 + 1;

            mesh.furrowIndices.push_back(i0);
            mesh.furrowIndices.push_back(i2);
            mesh.furrowIndices.push_back(i1);

            mesh.furrowIndices.push_back(i1);
            mesh.furrowIndices.push_back(i2);
            mesh.furrowIndices.push_back(i3);
        }
    }

    for (int r = 0; r < params.rowCount; ++r) {
        float x = (static_cast<float>(r) - params.rowCount * 0.5f + 0.5f) * params.rowSpacingMeters;
        int plantsInRow = static_cast<int>(params.rowLengthMeters / std::max(0.1f, params.plantIntervalMeters));

        for (int p = 0; p < plantsInRow; ++p) {
            float z = -params.rowLengthMeters * 0.5f + static_cast<float>(p) * params.plantIntervalMeters;
            float jitterX = rng.nextRange(-0.04f, 0.04f);
            float jitterZ = rng.nextRange(-0.04f, 0.04f);
            mesh.plantStalkPositions.push_back({x + jitterX, params.furrowDepthMeters * 0.5f, z + jitterZ});
        }
    }

    mesh.totalStalkCount = static_cast<uint32_t>(mesh.plantStalkPositions.size());
    return mesh;
}

CropSceneResult CropRowGen::synthesizeCropScene(const CropSynthesisParams& params, uint64_t seed) {
    FastRng rng(seed);
    CropSceneResult res;

    const int rows = std::clamp(params.rowCount, 2, 12);
    const int perRow = std::clamp(params.stalksPerRow, 3, 16);
    const float spacing = std::clamp(params.rowSpacingMeters, 0.35f, 2.60f);
    const float maturity = std::clamp(params.maturityStage01, 0.20f, 1.0f);
    const float stalkH = std::clamp(params.cropHeightMeters, 0.35f, 4.20f) * (0.45f + 0.55f * maturity);
    const float furrowD = std::clamp(params.furrowDepthMeters, 0.02f, 0.38f);
    const float windFlex = std::clamp(params.windFlexibility, 0.0f, 1.0f);

    float plantStepZ = (params.plantIntervalMeters > 0.1f)
        ? params.plantIntervalMeters
        : std::max(0.24f, spacing * 0.55f);
    // Keep field compact and nicely framed in the 3D orbit viewport
    const float totalW = static_cast<float>(rows) * spacing;
    const float totalL = std::clamp(static_cast<float>(perRow) * plantStepZ, 1.8f, 7.5f);
    plantStepZ = totalL / static_cast<float>(perRow);

    res.rowCount = rows;

    // 1. Furrow & Ridge Terrain Mesh (sinusoidal ridges centered under each row)
    if (params.showFurrows) {
        const int segsPerRow = 6;
        const int xSamples = rows * segsPerRow;
        const int zSamples = 10;

        auto ridgeY = [&](float x) -> float {
            // Cosine crest peaks at every row center x = (r - rows*0.5 + 0.5) * spacing
            float u = (x / spacing) + static_cast<float>(rows) * 0.5f - 0.5f;
            float ridge = std::cos(u * 2.0f * kPi);
            return ridge * furrowD * 0.5f;
        };

        for (int iz = 0; iz < zSamples; ++iz) {
            float z0 = -totalL * 0.5f + (static_cast<float>(iz) / zSamples) * totalL;
            float z1 = -totalL * 0.5f + (static_cast<float>(iz + 1) / zSamples) * totalL;
            for (int ix = 0; ix < xSamples; ++ix) {
                float x0 = -totalW * 0.5f + (static_cast<float>(ix) / xSamples) * totalW;
                float x1 = -totalW * 0.5f + (static_cast<float>(ix + 1) / xSamples) * totalW;

                float y0 = ridgeY(x0);
                float y1 = ridgeY(x1);
                float ym = 0.5f * (y0 + y1);
                // 0 at ridge crest (+furrowD*0.5), 1 in trough (-furrowD*0.5)
                float moisture = std::clamp(0.5f - ym / std::max(0.02f, furrowD), 0.0f, 1.0f);

                CropFurrowQuad3D q;
                q.p0 = Vec3(x0, y0, z0);
                q.p1 = Vec3(x1, y1, z0);
                q.p2 = Vec3(x1, y1, z1);
                q.p3 = Vec3(x0, y0, z1);
                q.moistureShade = moisture;
                res.furrowQuads.push_back(q);
            }
        }
    }

    // 2. Per-Row Trellis Infrastructure (for Vineyard Trellis)
    if (params.archetype == CropArchetype::VINEYARD_TRELLIS) {
        float postYBase = params.showFurrows ? (furrowD * 0.5f) : 0.0f;
        float postH = stalkH * 1.05f;
        float zFront = -totalL * 0.5f + plantStepZ * 0.35f;
        float zBack = totalL * 0.5f - plantStepZ * 0.35f;
        float zMid = 0.5f * (zFront + zBack);

        for (int r = 0; r < rows; ++r) {
            float rx = (static_cast<float>(r) - static_cast<float>(rows) * 0.5f + 0.5f) * spacing;
            float postZs[3] = {zFront, zMid, zBack};
            for (int p = 0; p < 3; ++p) {
                CropStalkSegment3D post;
                post.startPos = Vec3(rx, postYBase - 0.04f, postZs[p]);
                post.endPos = Vec3(rx, postYBase + postH, postZs[p]);
                post.radiusBottom = 0.042f;
                post.radiusTop = 0.038f;
                post.kind = 2; // Wooden trellis post
                post.windWeight = 0.0f;
                res.stalks.push_back(post);
            }
            // Two horizontal galvanized training wires along the row
            for (int w = 0; w < 2; ++w) {
                float wy = postYBase + postH * (w == 0 ? 0.52f : 0.84f);
                CropStalkSegment3D wire;
                wire.startPos = Vec3(rx, wy, zFront);
                wire.endPos = Vec3(rx, wy, zBack);
                wire.radiusBottom = 0.006f;
                wire.radiusTop = 0.006f;
                wire.kind = 3; // Wire
                wire.windWeight = 0.0f;
                res.stalks.push_back(wire);
            }
        }
    }

    // 3. Individual Crop Plants Placed Along Every Furrow Ridge
    const float ridgeTopY = params.showFurrows ? (furrowD * 0.5f) : 0.0f;
    for (int r = 0; r < rows; ++r) {
        float rowX = (static_cast<float>(r) - static_cast<float>(rows) * 0.5f + 0.5f) * spacing;

        for (int p = 0; p < perRow; ++p) {
            float plantZ = -totalL * 0.5f + (static_cast<float>(p) + 0.5f) * plantStepZ;
            float jX = rng.nextRange(-0.025f, 0.025f);
            float jZ = rng.nextRange(-0.025f, 0.025f);
            Vec3 basePos(rowX + jX, ridgeTopY, plantZ + jZ);
            float h = stalkH * rng.nextRange(0.88f, 1.12f);
            float az = rng.nextFloat01() * 2.0f * kPi;
            res.totalPlants += 1;

            switch (params.archetype) {
                case CropArchetype::CORN_MAIZE:
                case CropArchetype::SUGARCANE_ROWS: {
                    bool isCane = (params.archetype == CropArchetype::SUGARCANE_ROWS);
                    int segs = isCane ? 5 : 4;
                    float rBase = isCane ? 0.026f : 0.022f;
                    Vec3 cur = basePos;
                    float leanX = rng.nextRange(-0.06f, 0.06f);
                    float leanZ = rng.nextRange(-0.06f, 0.06f);

                    for (int s = 0; s < segs; ++s) {
                        float t1 = static_cast<float>(s + 1) / static_cast<float>(segs);
                        Vec3 nxt = basePos + Vec3(leanX * t1 * t1, h * t1, leanZ * t1 * t1);
                        CropStalkSegment3D st;
                        st.startPos = cur;
                        st.endPos = nxt;
                        st.radiusBottom = rBase * (1.0f - 0.35f * (static_cast<float>(s) / segs));
                        st.radiusTop = rBase * (1.0f - 0.35f * t1);
                        st.kind = 0;
                        st.windWeight = windFlex * (t1 * t1);
                        res.stalks.push_back(st);

                        // Long alternating arching strap blades at every culm node
                        int leafPairs = isCane ? 2 : 2;
                        for (int lf = 0; lf < leafPairs; ++lf) {
                            float la = az + (s * 2 + lf) * 2.3f;
                            Vec3 mid = safeNorm(Vec3(std::cos(la), 0.35f - 0.25f * t1, std::sin(la)));
                            Vec3 wing = safeNorm(cross(Vec3(0.0f, 1.0f, 0.0f), mid), Vec3(1.0f, 0.0f, 0.0f));
                            CropLeafBlade3D bl;
                            bl.attachPos = lerp(cur, nxt, 0.4f + 0.45f * lf);
                            bl.midribDir = mid;
                            bl.wingDir = wing;
                            bl.lengthMeters = std::clamp(h * 0.34f, 0.22f, 0.72f);
                            bl.widthMeters = bl.lengthMeters * (isCane ? 0.14f : 0.18f);
                            bl.tint = t1;
                            bl.windWeight = windFlex * (t1 * 0.9f);
                            res.leaves.push_back(bl);
                        }

                        // Golden Maize cob ear growing at mid-stalk node
                        if (!isCane && params.showHarvestOrgans && maturity >= 0.45f && s == 1) {
                            float ea = az + 1.1f;
                            CropHarvestOrgan3D cob;
                            cob.position = nxt + Vec3(std::cos(ea) * 0.04f, 0.02f, std::sin(ea) * 0.04f);
                            cob.dir = safeNorm(Vec3(std::cos(ea) * 0.55f, 0.85f, std::sin(ea) * 0.55f));
                            cob.sizeMeters = (0.14f + 0.08f * maturity);
                            cob.kind = CropOrganKind::MAIZE_EAR_COB;
                            cob.windWeight = st.windWeight;
                            res.organs.push_back(cob);
                        }
                        cur = nxt;
                    }

                    if (params.showHarvestOrgans && maturity >= 0.40f) {
                        CropHarvestOrgan3D top;
                        top.position = cur;
                        top.dir = Vec3(0.0f, 1.0f, 0.0f);
                        top.sizeMeters = isCane ? 0.26f : 0.20f;
                        top.kind = isCane ? CropOrganKind::SUGARCANE_PLUME : CropOrganKind::MAIZE_TASSEL;
                        top.windWeight = windFlex;
                        res.organs.push_back(top);
                    }
                    break;
                }

                case CropArchetype::BANANA_PLANTATION: {
                    // Columnar green pseudostem (tapered sheath) + radiating crown of huge paddle leaves + hanging banana bunch
                    float stemR = 0.075f + 0.035f * maturity;
                    Vec3 crownTop = basePos + Vec3(0.0f, h * 0.78f, 0.0f);
                    Vec3 midPt = basePos + Vec3(0.0f, h * 0.40f, 0.0f);

                    CropStalkSegment3D s0{basePos, midPt, stemR, stemR * 0.85f, 0, windFlex * 0.20f};
                    CropStalkSegment3D s1{midPt, crownTop, stemR * 0.85f, stemR * 0.65f, 0, windFlex * 0.45f};
                    res.stalks.push_back(s0);
                    res.stalks.push_back(s1);

                    int fronds = 6;
                    for (int f = 0; f < fronds; ++f) {
                        float fa = az + f * 2.399963f;
                        float elev = 0.45f - (static_cast<float>(f) / fronds) * 0.35f;
                        Vec3 mid = safeNorm(Vec3(std::cos(fa), elev, std::sin(fa)));
                        Vec3 wing = safeNorm(cross(Vec3(0.0f, 1.0f, 0.0f), mid));
                        CropLeafBlade3D paddle;
                        paddle.attachPos = crownTop + Vec3(0.0f, -0.05f * (f % 2), 0.0f);
                        paddle.midribDir = mid;
                        paddle.wingDir = wing;
                        paddle.lengthMeters = std::clamp(h * 0.52f, 0.65f, 1.45f);
                        paddle.widthMeters = paddle.lengthMeters * 0.36f;
                        paddle.tint = 0.35f + 0.65f * (1.0f - static_cast<float>(f) / fronds);
                        paddle.windWeight = windFlex * 0.72f;
                        res.leaves.push_back(paddle);
                    }

                    if (params.showHarvestOrgans && maturity >= 0.45f) {
                        float ba = az + 0.6f;
                        CropHarvestOrgan3D bunch;
                        bunch.position = crownTop + Vec3(std::cos(ba) * 0.14f, -0.10f, std::sin(ba) * 0.14f);
                        bunch.dir = Vec3(0.0f, -1.0f, 0.0f);
                        bunch.sizeMeters = 0.32f * (0.65f + 0.35f * maturity);
                        bunch.kind = CropOrganKind::BANANA_HAND_BUNCH;
                        bunch.windWeight = windFlex * 0.55f;
                        res.organs.push_back(bunch);
                    }
                    break;
                }

                case CropArchetype::WHEAT_FIELD: {
                    // Dense tuft of 3 slender golden culms with bearded nodding wheat grain heads
                    for (int t = 0; t < 3; ++t) {
                        float ta = az + t * 2.094f;
                        Vec3 foot = basePos + Vec3(std::cos(ta) * 0.03f, 0.0f, std::sin(ta) * 0.03f);
                        Vec3 mid = foot + Vec3(std::cos(ta) * 0.03f, h * 0.52f, std::sin(ta) * 0.03f);
                        Vec3 tip = mid + Vec3(std::cos(ta) * 0.06f, h * 0.48f, std::sin(ta) * 0.06f);
                        res.stalks.push_back({foot, mid, 0.008f, 0.006f, 0, windFlex * 0.40f});
                        res.stalks.push_back({mid, tip, 0.006f, 0.004f, 0, windFlex * 0.92f});

                        Vec3 lDir = safeNorm(Vec3(std::cos(ta + 0.8f), 0.55f, std::sin(ta + 0.8f)));
                        Vec3 lWing = safeNorm(cross(Vec3(0.0f, 1.0f, 0.0f), lDir));
                        res.leaves.push_back({mid, lDir, lWing, h * 0.28f, 0.025f, 0.75f, windFlex * 0.6f});

                        if (params.showHarvestOrgans && maturity >= 0.35f) {
                            CropHarvestOrgan3D spike;
                            spike.position = tip;
                            spike.dir = safeNorm(Vec3(std::cos(ta) * 0.35f, 0.88f, std::sin(ta) * 0.35f));
                            spike.sizeMeters = 0.14f + 0.08f * maturity;
                            spike.kind = CropOrganKind::WHEAT_GRAIN_SPIKE;
                            spike.windWeight = windFlex;
                            res.organs.push_back(spike);
                        }
                    }
                    break;
                }

                case CropArchetype::CASSAVA_FIELD: {
                    // Knotted woody stem branching into 2-3 forks topped by 7-lobed palmate leaf crowns
                    Vec3 fork = basePos + Vec3(0.0f, h * 0.52f, 0.0f);
                    res.stalks.push_back({basePos, fork, 0.026f, 0.018f, 1, windFlex * 0.18f});
                    for (int b = 0; b < 3; ++b) {
                        float ba = az + b * 2.094f;
                        Vec3 bEnd = fork + Vec3(std::cos(ba) * 0.22f, h * 0.44f, std::sin(ba) * 0.22f);
                        res.stalks.push_back({fork, bEnd, 0.015f, 0.008f, 1, windFlex * 0.55f});

                        // Radiating palmate leaf cluster
                        for (int lf = 0; lf < 4; ++lf) {
                            float la = ba + lf * 1.57f;
                            Vec3 mid = safeNorm(Vec3(std::cos(la), 0.18f, std::sin(la)));
                            Vec3 wing = safeNorm(cross(Vec3(0.0f, 1.0f, 0.0f), mid));
                            res.leaves.push_back({bEnd, mid, wing, 0.22f, 0.14f, 0.55f + 0.45f * (lf % 2), windFlex * 0.72f});
                        }
                    }
                    break;
                }

                case CropArchetype::COFFEE_PLANTATION: {
                    // Central vertical trunk + 3 opposite tiers of horizontal fruiting branches + red coffee cherries
                    Vec3 top = basePos + Vec3(0.0f, h, 0.0f);
                    res.stalks.push_back({basePos, top, 0.030f, 0.010f, 1, windFlex * 0.25f});
                    for (int tier = 0; tier < 3; ++tier) {
                        float ty = 0.34f + tier * 0.24f;
                        Vec3 node = lerp(basePos, top, ty);
                        float bLen = spacing * 0.36f * (1.0f - 0.20f * tier);
                        for (int side = 0; side < 2; ++side) {
                            float ba = az + tier * 1.57f + side * kPi;
                            Vec3 bEnd = node + Vec3(std::cos(ba) * bLen, -0.04f, std::sin(ba) * bLen);
                            res.stalks.push_back({node, bEnd, 0.010f, 0.004f, 1, windFlex * 0.55f});

                            for (int lf = 0; lf < 2; ++lf) {
                                Vec3 anchor = lerp(node, bEnd, 0.45f + 0.45f * lf);
                                Vec3 lDir = safeNorm(Vec3(std::cos(ba + (lf == 0 ? 0.5f : -0.5f)), 0.10f, std::sin(ba + (lf == 0 ? 0.5f : -0.5f))));
                                Vec3 lWing = safeNorm(cross(Vec3(0.0f, 1.0f, 0.0f), lDir));
                                res.leaves.push_back({anchor, lDir, lWing, 0.16f, 0.075f, 0.3f + 0.5f * tier / 2.0f, windFlex * 0.65f});

                                if (params.showHarvestOrgans && maturity >= 0.40f) {
                                    CropHarvestOrgan3D berry;
                                    berry.position = anchor + Vec3(0.0f, -0.025f, 0.0f);
                                    berry.dir = Vec3(0.0f, -1.0f, 0.0f);
                                    berry.sizeMeters = 0.045f * (0.7f + 0.3f * maturity);
                                    berry.kind = CropOrganKind::COFFEE_CHERRY_CLUSTER;
                                    berry.windWeight = windFlex * 0.55f;
                                    res.organs.push_back(berry);
                                }
                            }
                        }
                    }
                    break;
                }

                case CropArchetype::TEA_CONTOUR_HEDGES: {
                    // Dense flat-topped pruned tea bush forming a continuous emerald hedge along the row
                    Vec3 tableCenter = basePos + Vec3(0.0f, h, 0.0f);
                    for (int cane = 0; cane < 4; ++cane) {
                        float ca = az + cane * (kPi * 0.5f);
                        Vec3 tip = tableCenter + Vec3(std::cos(ca) * spacing * 0.24f, rng.nextRange(-0.04f, 0.03f), std::sin(ca) * plantStepZ * 0.34f);
                        res.stalks.push_back({basePos, tip, 0.015f, 0.006f, 1, windFlex * 0.20f});
                        for (int lf = 0; lf < 3; ++lf) {
                            float la = ca + lf * 2.1f;
                            Vec3 lDir = safeNorm(Vec3(std::cos(la), 0.22f, std::sin(la)));
                            Vec3 lWing = safeNorm(cross(Vec3(0.0f, 1.0f, 0.0f), lDir));
                            res.leaves.push_back({tip, lDir, lWing, 0.12f, 0.055f, 0.75f + 0.25f * (lf % 2), windFlex * 0.40f});
                        }
                        if (params.showHarvestOrgans && cane % 2 == 0) {
                            res.organs.push_back({tip + Vec3(0.0f, 0.03f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), 0.055f, CropOrganKind::TEA_FLUSH_SHOOT, windFlex * 0.5f});
                        }
                    }
                    break;
                }

                case CropArchetype::VINEYARD_TRELLIS: {
                    // Gnarled trunk rising to first trellis wire + bilateral cordons + grape leaves + hanging purple clusters
                    float wireLow = h * 0.50f;
                    Vec3 head = basePos + Vec3(0.0f, wireLow, 0.0f);
                    res.stalks.push_back({basePos, head, 0.032f, 0.024f, 1, windFlex * 0.08f});

                    for (int dirSign : {-1, 1}) {
                        Vec3 armEnd = head + Vec3(rng.nextRange(-0.04f, 0.04f), 0.02f, dirSign * plantStepZ * 0.44f);
                        res.stalks.push_back({head, armEnd, 0.018f, 0.008f, 1, windFlex * 0.22f});

                        Vec3 shootTip = lerp(head, armEnd, 0.55f) + Vec3(dirSign * 0.14f, h * 0.36f, 0.0f);
                        res.stalks.push_back({lerp(head, armEnd, 0.55f), shootTip, 0.008f, 0.004f, 0, windFlex * 0.55f});

                        for (int lf = 0; lf < 3; ++lf) {
                            Vec3 anchor = lerp(lerp(head, armEnd, 0.35f), shootTip, lf * 0.45f);
                            float la = az + lf * 2.1f + dirSign;
                            Vec3 lDir = safeNorm(Vec3(std::cos(la), 0.15f, std::sin(la)));
                            Vec3 lWing = safeNorm(cross(Vec3(0.0f, 1.0f, 0.0f), lDir));
                            res.leaves.push_back({anchor, lDir, lWing, 0.18f, 0.16f, 0.5f + 0.4f * (lf % 2), windFlex * 0.6f});
                        }

                        if (params.showHarvestOrgans && maturity >= 0.40f) {
                            CropHarvestOrgan3D grape;
                            grape.position = lerp(head, armEnd, 0.62f) + Vec3(dirSign * 0.05f, -0.03f, 0.0f);
                            grape.dir = Vec3(0.0f, -1.0f, 0.0f);
                            grape.sizeMeters = 0.14f * (0.65f + 0.35f * maturity);
                            grape.kind = CropOrganKind::GRAPE_CLUSTER;
                            grape.windWeight = windFlex * 0.45f;
                            res.organs.push_back(grape);
                        }
                    }
                    break;
                }

                case CropArchetype::POTATO_FURROWS:
                case CropArchetype::LAVENDER_ROWS: {
                    bool isLav = (params.archetype == CropArchetype::LAVENDER_ROWS);
                    for (int st = 0; st < 4; ++st) {
                        float sa = az + st * (kPi * 0.5f);
                        Vec3 tip = basePos + Vec3(std::cos(sa) * 0.14f, h, std::sin(sa) * 0.14f);
                        res.stalks.push_back({basePos, tip, 0.009f, 0.004f, 0, windFlex * 0.55f});

                        Vec3 lDir = safeNorm(Vec3(std::cos(sa), 0.25f, std::sin(sa)));
                        Vec3 lWing = safeNorm(cross(Vec3(0.0f, 1.0f, 0.0f), lDir));
                        res.leaves.push_back({lerp(basePos, tip, 0.55f), lDir, lWing, 0.15f, isLav ? 0.03f : 0.085f, 0.6f, windFlex * 0.65f});

                        if (params.showHarvestOrgans && maturity >= 0.40f && st % 2 == 0) {
                            res.organs.push_back({tip, Vec3(0.0f, 1.0f, 0.0f), isLav ? 0.11f : 0.06f, CropOrganKind::POTATO_BLOSSOM, windFlex * 0.85f});
                        }
                    }
                    break;
                }
            }
        }
    }

    return res;
}

} // namespace vegetation
