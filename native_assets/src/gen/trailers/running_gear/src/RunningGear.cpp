#include "RunningGear.h"
#include <cmath>
#include <algorithm>

namespace sphere::trailers::gear {

void RunningGear::AppendBox(Mesh* mesh, const Vec3& minP, const Vec3& maxP, const Vec4& color) {
    uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
    Vec3 corners[8] = {
        Vec3(minP.x, minP.y, maxP.z), Vec3(maxP.x, minP.y, maxP.z),
        Vec3(maxP.x, maxP.y, maxP.z), Vec3(minP.x, maxP.y, maxP.z),
        Vec3(minP.x, minP.y, minP.z), Vec3(maxP.x, minP.y, minP.z),
        Vec3(maxP.x, maxP.y, minP.z), Vec3(minP.x, maxP.y, minP.z)
    };
    for (int i = 0; i < 8; ++i) {
        Vertex v;
        v.position = corners[i];
        v.uv[0] = (i % 2 == 0) ? 0.0f : 1.0f;
        v.uv[1] = (i < 4) ? 0.0f : 1.0f;
        v.color = color;
        mesh->vertices.push_back(v);
    }
    const uint32_t tris[36] = {
        0, 1, 2, 0, 2, 3,
        5, 4, 7, 5, 7, 6,
        3, 2, 6, 3, 6, 7,
        4, 5, 1, 4, 1, 0,
        1, 5, 6, 1, 6, 2,
        4, 0, 3, 4, 3, 7
    };
    for (int i = 0; i < 36; ++i) {
        mesh->indices.push_back(base + tris[i]);
    }
}

void RunningGear::AppendWheelZ(
    Mesh* mesh,
    float cx, float cy, float cz,
    float radius, float width,
    int segments,
    const Vec4& tireColor,
    const Vec4& rimColor,
    const Vec4& brakeColor,
    float steerAngleRad
) {
    int segs = std::max(segments, 28);
    float cS = std::cos(steerAngleRad);
    float sS = std::sin(steerAngleRad);
    auto rotPt = [&](float lx, float ly, float lz) -> Vec3 {
        return Vec3(cx + lx * cS - lz * sS, cy + ly, cz + lx * sS + lz * cS);
    };

    auto pushTri = [&](const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec4& col) {
        uint32_t idx = static_cast<uint32_t>(mesh->vertices.size());
        Vertex v0; v0.position = p0; v0.color = col; v0.uv[0] = 0.0f; v0.uv[1] = 0.0f;
        Vertex v1; v1.position = p1; v1.color = col; v1.uv[0] = 1.0f; v1.uv[1] = 0.0f;
        Vertex v2; v2.position = p2; v2.color = col; v2.uv[0] = 1.0f; v2.uv[1] = 1.0f;
        mesh->vertices.push_back(v0);
        mesh->vertices.push_back(v1);
        mesh->vertices.push_back(v2);
        mesh->indices.push_back(idx);
        mesh->indices.push_back(idx + 1);
        mesh->indices.push_back(idx + 2);
    };
    auto pushQuad = [&](const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const Vec4& col) {
        pushTri(a, b, c, col);
        pushTri(a, c, d, col);
    };

    float hw = width * 0.5f;
    float treadHW = hw * 0.86f;
    float shR = radius * 0.93f;
    float rimR = radius * 0.55f;
    float dishR = radius * 0.48f;
    float hubR = radius * 0.21f;
    float dishZ = hw * 0.56f;
    float hubZ = hw * 0.78f;

    Vec4 shoulderCol(
        std::min(1.0f, tireColor.x * 1.25f),
        std::min(1.0f, tireColor.y * 1.25f),
        std::min(1.0f, tireColor.z * 1.25f),
        1.0f
    );
    Vec4 hubCol(0.16f, 0.18f, 0.22f, 1.0f);

    bool isLeft = (cz < 0.0f);
    float rotorInnerZ = isLeft ? hw * 0.92f : -hw * 1.18f;
    float rotorOuterZ = isLeft ? hw * 1.18f : -hw * 0.92f;
    float rotorR = radius * 0.43f;

    Vec3 cA = rotPt(0.0f, 0.0f, -hubZ);
    Vec3 cB = rotPt(0.0f, 0.0f, hubZ);
    Vec3 rCA = rotPt(0.0f, 0.0f, rotorInnerZ);
    Vec3 rCB = rotPt(0.0f, 0.0f, rotorOuterZ);

    for (int i = 0; i < segs; ++i) {
        float a0 = (static_cast<float>(i) / static_cast<float>(segs)) * 6.2831853f;
        float a1 = (static_cast<float>(i + 1) / static_cast<float>(segs)) * 6.2831853f;
        float c0 = std::cos(a0), s0 = std::sin(a0);
        float c1 = std::cos(a1), s1 = std::sin(a1);

        Vec3 sideA0 = rotPt(shR * c0, shR * s0, -hw),       sideA1 = rotPt(shR * c1, shR * s1, -hw);
        Vec3 treadA0 = rotPt(radius * c0, radius * s0, -treadHW), treadA1 = rotPt(radius * c1, radius * s1, -treadHW);
        Vec3 treadB0 = rotPt(radius * c0, radius * s0, treadHW),  treadB1 = rotPt(radius * c1, radius * s1, treadHW);
        Vec3 sideB0 = rotPt(shR * c0, shR * s0, hw),        sideB1 = rotPt(shR * c1, shR * s1, hw);

        Vec3 rimA0 = rotPt(rimR * c0, rimR * s0, -hw * 0.94f), rimA1 = rotPt(rimR * c1, rimR * s1, -hw * 0.94f);
        Vec3 rimB0 = rotPt(rimR * c0, rimR * s0, hw * 0.94f),  rimB1 = rotPt(rimR * c1, rimR * s1, hw * 0.94f);
        Vec3 dishA0 = rotPt(dishR * c0, dishR * s0, -dishZ), dishA1 = rotPt(dishR * c1, dishR * s1, -dishZ);
        Vec3 dishB0 = rotPt(dishR * c0, dishR * s0, dishZ),  dishB1 = rotPt(dishR * c1, dishR * s1, dishZ);

        Vec3 hBaseA0 = rotPt(hubR * c0, hubR * s0, -dishZ), hBaseA1 = rotPt(hubR * c1, hubR * s1, -dishZ);
        Vec3 hBaseB0 = rotPt(hubR * c0, hubR * s0, dishZ),  hBaseB1 = rotPt(hubR * c1, hubR * s1, dishZ);
        Vec3 hTipA0 = rotPt(hubR * 0.85f * c0, hubR * 0.85f * s0, -hubZ), hTipA1 = rotPt(hubR * 0.85f * c1, hubR * 0.85f * s1, -hubZ);
        Vec3 hTipB0 = rotPt(hubR * 0.85f * c0, hubR * 0.85f * s0, hubZ),  hTipB1 = rotPt(hubR * 0.85f * c1, hubR * 0.85f * s1, hubZ);

        Vec3 rA0 = rotPt(rotorR * c0, rotorR * s0, rotorInnerZ), rA1 = rotPt(rotorR * c1, rotorR * s1, rotorInnerZ);
        Vec3 rB0 = rotPt(rotorR * c0, rotorR * s0, rotorOuterZ), rB1 = rotPt(rotorR * c1, rotorR * s1, rotorOuterZ);

        // Outward tread crown & bevelled shoulders
        pushQuad(sideA0, sideA1, treadA1, treadA0, shoulderCol);
        pushQuad(treadA0, treadA1, treadB1, treadB0, tireColor);
        pushQuad(treadB0, treadB1, sideB1, sideB0, shoulderCol);

        // Sidewalls
        pushQuad(rimA0, rimA1, sideA1, sideA0, tireColor);
        pushQuad(rimB0, sideB0, sideB1, rimB1, tireColor);

        // Recessed rim barrel + wheel dish
        pushQuad(rimA0, dishA0, dishA1, rimA1, rimColor);
        pushQuad(dishB0, rimB0, rimB1, dishB1, rimColor);
        pushQuad(hBaseA0, hBaseA1, dishA1, dishA0, rimColor);
        pushQuad(hBaseB0, dishB0, dishB1, hBaseB1, rimColor);

        // Center hubcap + solid caps
        pushQuad(hTipA0, hTipA1, hBaseA1, hBaseA0, hubCol);
        pushQuad(hBaseB0, hBaseB1, hTipB1, hTipB0, hubCol);
        pushTri(cA, hTipA1, hTipA0, hubCol);
        pushTri(cB, hTipB0, hTipB1, hubCol);

        // Inboard brake rotor / drum
        pushQuad(rA0, rA1, rB1, rB0, brakeColor);
        pushTri(rCA, rA1, rA0, brakeColor);
        pushTri(rCB, rB0, rB1, brakeColor);
    }
}

void RunningGear::AppendMesh(
    Mesh* mesh,
    const RunningGearConfig& cfg,
    float bogieCenterX,
    float chassisBottomY,
    float landingGearX,
    bool landingGearExtended
) {
    if (!mesh || cfg.axle_count == 0) return;

    const Vec4 frameSteel(0.14f, 0.16f, 0.19f, 1.0f);
    const Vec4 tireRubber(0.08f, 0.08f, 0.09f, 1.0f);
    const Vec4 silverRim(0.76f, 0.79f, 0.84f, 1.0f);
    const Vec4 brakeRed = (cfg.brake_type == BrakeType::DISC_ABS)
        ? Vec4(0.72f, 0.12f, 0.11f, 1.0f)
        : Vec4(0.28f, 0.29f, 0.31f, 1.0f);
    const Vec4 airBagBlack(0.11f, 0.11f, 0.12f, 1.0f);
    const Vec4 mudflapDark(0.09f, 0.10f, 0.12f, 1.0f);
    const Vec4 bumperSilver(0.78f, 0.81f, 0.85f, 1.0f);
    const Vec4 tailRed(0.86f, 0.10f, 0.10f, 1.0f);
    const Vec4 tailAmber(0.95f, 0.62f, 0.06f, 1.0f);

    float halfTrack = cfg.track_width_m * 0.5f;
    float totalSpan = static_cast<float>(cfg.axle_count - 1) * cfg.axle_spacing_m;
    float outerOffset = cfg.has_dual_wheels ? (cfg.tire_width_m * 0.58f) : 0.0f;

    // Subframe rails (inboard of tires, maintaining clearance)
    AppendBox(
        mesh,
        Vec3(bogieCenterX - totalSpan * 0.5f - 0.65f, chassisBottomY - 0.18f, -halfTrack * 0.62f),
        Vec3(bogieCenterX + totalSpan * 0.5f + 0.65f, chassisBottomY, -halfTrack * 0.50f),
        frameSteel
    );
    AppendBox(
        mesh,
        Vec3(bogieCenterX - totalSpan * 0.5f - 0.65f, chassisBottomY - 0.18f, halfTrack * 0.50f),
        Vec3(bogieCenterX + totalSpan * 0.5f + 0.65f, chassisBottomY, halfTrack * 0.62f),
        frameSteel
    );

    float rearAxleX = bogieCenterX + totalSpan * 0.5f;
    float frontAxleX = bogieCenterX - totalSpan * 0.5f;

    for (uint32_t a = 0; a < cfg.axle_count; ++a) {
        float ax = frontAxleX + static_cast<float>(a) * cfg.axle_spacing_m;
        float liftOffset = (cfg.has_lift_axle && cfg.lift_axle_raised && a == 0) ? 0.16f : 0.0f;
        float ay = cfg.wheel_radius_m + liftOffset;
        float steerRad = (cfg.has_steerable_axle && a == cfg.axle_count - 1)
            ? (cfg.steer_angle_deg * 3.14159265f / 180.0f)
            : 0.0f;

        // Solid tubular axle beam
        AppendBox(
            mesh,
            Vec3(ax - 0.075f, ay - 0.075f, -halfTrack - outerOffset),
            Vec3(ax + 0.075f, ay + 0.075f, halfTrack + outerOffset),
            frameSteel
        );

        // Suspension air bags or leaf springs
        float springTop = std::max(ay + 0.10f, chassisBottomY - 0.16f);
        if (cfg.suspension_type == SuspensionType::AIR_SUSPENSION) {
            AppendBox(mesh, Vec3(ax - 0.14f, ay + 0.07f, -halfTrack * 0.64f), Vec3(ax + 0.14f, springTop, -halfTrack * 0.48f), airBagBlack);
            AppendBox(mesh, Vec3(ax - 0.14f, ay + 0.07f, halfTrack * 0.48f), Vec3(ax + 0.14f, springTop, halfTrack * 0.64f), airBagBlack);
        } else {
            AppendBox(mesh, Vec3(ax - 0.42f, ay + 0.06f, -halfTrack * 0.62f), Vec3(ax + 0.42f, ay + 0.16f, -halfTrack * 0.50f), frameSteel);
            AppendBox(mesh, Vec3(ax - 0.42f, ay + 0.06f, halfTrack * 0.50f), Vec3(ax + 0.42f, ay + 0.16f, halfTrack * 0.62f), frameSteel);
        }

        // Solid left & right tires (single or dual-wheel bogies)
        if (cfg.has_dual_wheels) {
            AppendWheelZ(mesh, ax, ay, -halfTrack + outerOffset, cfg.wheel_radius_m, cfg.tire_width_m, 28, tireRubber, silverRim, brakeRed, steerRad);
            AppendWheelZ(mesh, ax, ay, -halfTrack - outerOffset, cfg.wheel_radius_m, cfg.tire_width_m, 28, tireRubber, silverRim, brakeRed, steerRad);
            AppendWheelZ(mesh, ax, ay,  halfTrack - outerOffset, cfg.wheel_radius_m, cfg.tire_width_m, 28, tireRubber, silverRim, brakeRed, steerRad);
            AppendWheelZ(mesh, ax, ay,  halfTrack + outerOffset, cfg.wheel_radius_m, cfg.tire_width_m, 28, tireRubber, silverRim, brakeRed, steerRad);
        } else {
            AppendWheelZ(mesh, ax, ay, -halfTrack, cfg.wheel_radius_m, cfg.tire_width_m, 28, tireRubber, silverRim, brakeRed, steerRad);
            AppendWheelZ(mesh, ax, ay,  halfTrack, cfg.wheel_radius_m, cfg.tire_width_m, 28, tireRubber, silverRim, brakeRed, steerRad);
        }
    }

    // Rear mudflaps behind final axle
    float mudZOuter = halfTrack + outerOffset + cfg.tire_width_m * 0.48f;
    float mudZInner = halfTrack - outerOffset - cfg.tire_width_m * 0.48f;
    float flapX = rearAxleX + cfg.wheel_radius_m + 0.08f;
    AppendBox(mesh, Vec3(flapX, 0.22f, -mudZOuter), Vec3(flapX + 0.04f, cfg.wheel_radius_m * 1.45f, -mudZInner), mudflapDark);
    AppendBox(mesh, Vec3(flapX, 0.22f,  mudZInner), Vec3(flapX + 0.04f, cfg.wheel_radius_m * 1.45f,  mudZOuter), mudflapDark);

    // ECE R58 / DOT rear underrun bumper & tail light housings (skip on dollies/short small ball-hitch utility)
    if (cfg.hitch_type != HitchType::BALL_HITCH_50MM) {
        float bumperX = std::max(flapX + 0.28f, cfg.trailer_length_m * 0.5f - 0.14f);
        float bumpTopY = std::min(chassisBottomY, 0.56f);
        float bumpBotY = 0.42f;
        AppendBox(mesh, Vec3(bumperX - 0.06f, bumpBotY, -mudZOuter), Vec3(bumperX + 0.08f, bumpTopY, mudZOuter), bumperSilver);
        AppendBox(mesh, Vec3(bumperX - 0.08f, bumpTopY, -halfTrack * 0.58f), Vec3(bumperX, chassisBottomY, -halfTrack * 0.48f), frameSteel);
        AppendBox(mesh, Vec3(bumperX - 0.08f, bumpTopY,  halfTrack * 0.48f), Vec3(bumperX, chassisBottomY,  halfTrack * 0.58f), frameSteel);
        // Red brake + amber indicator lamps
        AppendBox(mesh, Vec3(bumperX + 0.07f, bumpBotY + 0.03f, -mudZOuter + 0.06f), Vec3(bumperX + 0.11f, bumpTopY - 0.02f, -mudZOuter + 0.24f), tailRed);
        AppendBox(mesh, Vec3(bumperX + 0.07f, bumpBotY + 0.03f, -mudZOuter + 0.25f), Vec3(bumperX + 0.11f, bumpTopY - 0.02f, -mudZOuter + 0.36f), tailAmber);
        AppendBox(mesh, Vec3(bumperX + 0.07f, bumpBotY + 0.03f,  mudZOuter - 0.24f), Vec3(bumperX + 0.11f, bumpTopY - 0.02f,  mudZOuter - 0.06f), tailRed);
        AppendBox(mesh, Vec3(bumperX + 0.07f, bumpBotY + 0.03f,  mudZOuter - 0.36f), Vec3(bumperX + 0.11f, bumpTopY - 0.02f,  mudZOuter - 0.25f), tailAmber);
    }

    // Telescopic front landing gear legs & sand shoes
    float legBottomY = landingGearExtended ? 0.02f : std::max(0.28f, chassisBottomY - 0.62f);
    AppendBox(mesh, Vec3(landingGearX - 0.08f, legBottomY, -halfTrack * 0.60f), Vec3(landingGearX + 0.08f, chassisBottomY, -halfTrack * 0.48f), frameSteel);
    AppendBox(mesh, Vec3(landingGearX - 0.08f, legBottomY,  halfTrack * 0.48f), Vec3(landingGearX + 0.08f, chassisBottomY,  halfTrack * 0.60f), frameSteel);
    AppendBox(mesh, Vec3(landingGearX - 0.16f, legBottomY - 0.03f, -halfTrack * 0.66f), Vec3(landingGearX + 0.16f, legBottomY, -halfTrack * 0.42f), frameSteel);
    AppendBox(mesh, Vec3(landingGearX - 0.16f, legBottomY - 0.03f,  halfTrack * 0.42f), Vec3(landingGearX + 0.16f, legBottomY,  halfTrack * 0.66f), frameSteel);

    // Side cyclist underrun guard rails between landing gear & bogie on full-size semis
    float guardStartX = landingGearX + 0.38f;
    float guardEndX = frontAxleX - cfg.wheel_radius_m - 0.22f;
    if (guardEndX - guardStartX > 1.2f && cfg.trailer_length_m >= 8.0f && cfg.hitch_type != HitchType::PINTLE_RING_DRAWBAR) {
        float sideZ = halfTrack + outerOffset + cfg.tire_width_m * 0.32f;
        AppendBox(mesh, Vec3(guardStartX, 0.44f, -sideZ), Vec3(guardEndX, 0.52f, -sideZ + 0.04f), bumperSilver);
        AppendBox(mesh, Vec3(guardStartX, 0.44f,  sideZ - 0.04f), Vec3(guardEndX, 0.52f, sideZ), bumperSilver);
        AppendBox(mesh, Vec3(guardStartX + 0.35f, 0.48f, -sideZ + 0.04f), Vec3(guardStartX + 0.45f, chassisBottomY, -halfTrack * 0.54f), frameSteel);
        AppendBox(mesh, Vec3(guardEndX - 0.45f,   0.48f, -sideZ + 0.04f), Vec3(guardEndX - 0.35f,   chassisBottomY, -halfTrack * 0.54f), frameSteel);
        AppendBox(mesh, Vec3(guardStartX + 0.35f, 0.48f,  halfTrack * 0.54f), Vec3(guardStartX + 0.45f, chassisBottomY,  sideZ - 0.04f), frameSteel);
        AppendBox(mesh, Vec3(guardEndX - 0.45f,   0.48f,  halfTrack * 0.54f), Vec3(guardEndX - 0.35f,   chassisBottomY,  sideZ - 0.04f), frameSteel);
    }

    // 2-inch SAE Fifth-Wheel Kingpin assembly ahead of landing gear
    if (cfg.hitch_type == HitchType::FIFTH_WHEEL_KINGPIN || cfg.hitch_type == HitchType::DETACHABLE_GOOSENECK) {
        float kpX = -cfg.trailer_length_m * 0.5f + 1.65f;
        AppendBox(mesh, Vec3(kpX - 0.45f, chassisBottomY - 0.05f, -0.52f), Vec3(kpX + 0.45f, chassisBottomY, 0.52f), frameSteel);
        AppendBox(mesh, Vec3(kpX - 0.07f, chassisBottomY - 0.19f, -0.07f), Vec3(kpX + 0.07f, chassisBottomY - 0.04f, 0.07f), bumperSilver);
    }
}

} // namespace sphere::trailers::gear
