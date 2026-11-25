#include "DeskGen.h"

namespace procedural::props {

DeskGen::DeskGen() = default;

bool DeskGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void DeskGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload DeskGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = 8 * sizeof(float);

    uint32_t topCol   = (params_.finishType == 0) ? 0xFFDEB887 : // Oak
                        (params_.finishType == 1) ? 0xFF5C4033 : // Walnut
                                                    0xFFF0F2F5;  // Modern White

    uint32_t legCol   = (params_.finishType == 2) ? 0xFF2A2E33 : topCol;
    uint32_t handleCol = 0xFFCCCCCC; // Brushed aluminium

    float W = params_.width;
    float D = params_.depth;
    float H = params_.height;
    float topThick = 0.035f;

    // 1. Desktop surface slab with chamfered profile
    PropMeshHelper::appendBox(payload, Vec3{0.0f, H - topThick * 0.5f, 0.0f}, Vec3{W, topThick, D}, topCol);

    // Cable pass-through grommet
    if (params_.hasCableGrommet) {
        PropMeshHelper::appendCylinder(payload, Vec3{W * 0.35f, H - topThick, -D * 0.35f}, 0.035f, 0.035f, topThick * 1.1f, 12, 0xFF333333);
    }

    float underY = H - topThick;

    // Helper to append a 3-drawer pedestal unit
    auto appendPedestal = [&](float posX) {
        float pedW = 0.42f;
        float pedD = D * 0.85f;
        float pedH = underY - 0.06f;
        Vec3 pedCenter{posX, 0.06f + pedH * 0.5f, 0.0f};

        // Outer cabinet carcass
        PropMeshHelper::appendBox(payload, pedCenter, Vec3{pedW, pedH, pedD}, legCol);

        // 3 Drawer front faces & bar handles
        float dH = (pedH - 0.04f) / 3.0f;
        for (int i = 0; i < 3; ++i) {
            float dy = 0.08f + i * dH + dH * 0.5f;
            // Drawer front
            PropMeshHelper::appendBox(payload, Vec3{posX, dy, pedD * 0.505f}, Vec3{pedW * 0.94f, dH * 0.90f, 0.015f}, topCol);
            // Modern bar pull handle
            PropMeshHelper::appendBox(payload, Vec3{posX, dy, pedD * 0.525f}, Vec3{0.14f, 0.02f, 0.02f}, handleCol);
        }
    };

    if (params_.drawerConfig == 0) {
        // Minimal Table: 4 square / tubular legs
        float legSize = 0.05f;
        float lx[2] = {-W * 0.5f + legSize * 0.8f, W * 0.5f - legSize * 0.8f};
        float lz[2] = {-D * 0.5f + legSize * 0.8f, D * 0.5f - legSize * 0.8f};
        for (float x : lx) {
            for (float z : lz) {
                PropMeshHelper::appendBox(payload, Vec3{x, underY * 0.5f, z}, Vec3{legSize, underY, legSize}, legCol);
            }
        }
    } else if (params_.drawerConfig == 1) {
        // Right Pedestal + Left pair of legs
        appendPedestal(W * 0.5f - 0.25f);
        // Left legs
        float legSize = 0.05f;
        float lx = -W * 0.5f + legSize * 0.8f;
        float lz[2] = {-D * 0.5f + legSize * 0.8f, D * 0.5f - legSize * 0.8f};
        for (float z : lz) {
            PropMeshHelper::appendBox(payload, Vec3{lx, underY * 0.5f, z}, Vec3{legSize, underY, legSize}, legCol);
        }
        // Left cross stretcher rail
        PropMeshHelper::appendBox(payload, Vec3{lx, 0.12f, 0.0f}, Vec3{legSize, 0.04f, D * 0.75f}, legCol);
    } else {
        // Executive Double Pedestal (Left + Right)
        appendPedestal(-W * 0.5f + 0.25f);
        appendPedestal( W * 0.5f - 0.25f);
    }

    // Rear Modesty Privacy Panel
    if (params_.hasModestyPanel) {
        float mPanelW = (params_.drawerConfig == 2) ? W * 0.5f : W * 0.85f;
        float mPanelH = underY * 0.65f;
        PropMeshHelper::appendBox(payload, Vec3{0.0f, underY - mPanelH * 0.5f, -D * 0.40f}, Vec3{mPanelW, mPanelH, 0.02f}, topCol);
    }

    return payload;
}

} // namespace procedural::props
