#pragma once
// ============================================================================
// Botanical3DSceneRenderer.h
// Native C++17 Full 3D Botanical Scene Rasterizer (Sky, Ground, Trunks,
// Branches, Petioles, Analytical Subpixel Leaves, Blossoms, Fruits & Accents)
// ============================================================================

#include "../common/VegetationMath.h"
#include <cstdint>
#include <vector>

namespace vegetation {

struct TreeBranchSegment;
struct TreeLeafData;

namespace scene {

struct RgbF {
    float r{0.5f};
    float g{0.5f};
    float b{0.5f};
};

struct ScreenProj {
    float x{0.0f};
    float y{0.0f};
    float depth{0.0f};
    bool valid{false};
};

class DepthFrameBuffer {
public:
    DepthFrameBuffer(int w, int h, int32_t* pixels);

    int width() const { return m_width; }
    int height() const { return m_height; }

    void setCamera(
        const Vec3& camPos,
        const Vec3& fwd,
        const Vec3& right,
        const Vec3& up,
        float centerX,
        float centerY,
        float fovScale
    );

    ScreenProj project(const Vec3& worldPos, float minDepth = 0.25f) const;

    const Vec3& camPos() const { return m_camPos; }
    const Vec3& camFwd() const { return m_fwd; }
    const Vec3& camRight() const { return m_right; }
    const Vec3& camUp() const { return m_up; }
    float fovScale() const { return m_fovScale; }
    float centerX() const { return m_centerX; }
    float centerY() const { return m_centerY; }

    void fillStudioGroundPlane(
        const Vec3& sunDir,
        float canopyRadius,
        float rootRadius,
        float plantHeight,
        bool isShrubTheme
    );

    void drawAaLine3D(
        const Vec3& p0,
        const Vec3& p1,
        float thicknessPx,
        uint8_t r, uint8_t g, uint8_t b, float alpha,
        bool writeDepth = true,
        float depthBias = 0.0f
    );

    void drawShadedCylinder3D(
        const Vec3& p0,
        const Vec3& p1,
        float r0,
        float r1,
        const RgbF& barkBase,
        const Vec3& sunDir,
        float aoFactor,
        bool isWireframe,
        bool drawJointCap
    );

    void drawShadedSphere3D(
        const Vec3& center,
        float worldRadius,
        const RgbF& baseColor,
        const Vec3& sunDir,
        float specStrength = 0.25f,
        float minPx = 1.5f,
        float maxPx = 38.0f
    );

    void drawGroundShadowEllipse(
        const Vec3& rootPos,
        float worldRadiusX,
        float worldRadiusZ
    );

    void blendPixel(int x, int y, float depth, uint8_t r, uint8_t g, uint8_t b, float alpha, bool writeDepth);
    void forcePixel(int x, int y, float depth, uint8_t r, uint8_t g, uint8_t b);
    float getDepth(int x, int y) const;

    static inline uint8_t toByte(float v) {
        int iv = static_cast<int>(std::clamp(v, 0.0f, 1.0f) * 255.0f + 0.5f);
        return static_cast<uint8_t>(std::clamp(iv, 0, 255));
    }

private:
    int m_width{0};
    int m_height{0};
    int32_t* m_pixels{nullptr};
    std::vector<float> m_zbuf;

    Vec3 m_camPos{0, 2, 10};
    Vec3 m_fwd{0, 0, -1};
    Vec3 m_right{1, 0, 0};
    Vec3 m_up{0, 1, 0};
    float m_centerX{0.0f};
    float m_centerY{0.0f};
    float m_fovScale{400.0f};
};

struct LeafRenderItem {
    Vec3 twigAttach;
    Vec3 bladeBase;
    Vec3 midribDir;
    Vec3 wingDir;
    Vec3 normalDir;
    float lengthMeters{0.15f};
    float petioleMeters{0.04f};
    float sunlitFactor{0.65f};
    float tintVariation{0.5f};
    float depthKey{0.0f};
    uint32_t indexSeed{0};
    bool isGiantFrond{false};
    bool allowSplits{true};
};

struct BotanicalTreeRenderParams {
    int speciesId{0};
    uint64_t seed{42};
    float heightMeters{15.0f};
    float trunkRadiusMeters{0.5f};
    int branchCount{32};
    int leafCount{360};
    float branchAngleDeg{42.0f};
    float leafScale{1.0f};
    int leafMorphologyId{6};
    RgbF barkColor{0.36f, 0.25f, 0.18f};
    RgbF crownBaseColor{0.14f, 0.40f, 0.18f};
    RgbF crownSunlitColor{0.38f, 0.72f, 0.25f};
    bool showLeaves{true};
    bool showBlossoms{true};
    int bloomStyleId{0};
    RgbF flowerPrimary{1.0f, 0.78f, 0.86f};
    RgbF flowerSecondary{0.98f, 0.85f, 0.30f};
    bool showFruits{true};
    int fruitStyleId{0};
    RgbF fruitPrimary{0.95f, 0.55f, 0.12f};
    RgbF fruitSecondary{0.32f, 0.58f, 0.16f};
    bool showWireframeHull{false};
    bool windEnabled{true};
    float windSpeedMps{4.5f};
    float windFlexibility{0.35f};
    float timeSeconds{0.0f};
    float sunElevationDeg{46.0f};
    float sunAzimuthDeg{135.0f};
    int cameraFocus{0}; // 0 = Full Tree, 1 = Branching, 2 = Leaf & Petiole
    float camAzimuthDeg{36.0f};
    float camElevationDeg{24.0f};
    float camDistanceMeters{24.0f};
};

struct BotanicalShrubRenderParams {
    int bushArchetypeId{0};
    uint64_t seed{0x51A7B3ULL};
    float radiusMeters{1.35f};
    float heightMeters{1.10f};
    int stemCount{12};
    int clusterDensity{54};
    float windFlexibility{0.45f};
    bool showBranches{true};
    bool showAccents{true};
    bool showWireframe{false};
    int leafMorphologyId{13};
    RgbF stemColor{0.31f, 0.21f, 0.16f};
    RgbF foliageColor{0.12f, 0.42f, 0.15f};
    RgbF accentColor{0.83f, 0.18f, 0.18f};
    int accentTypeId{0};
    float timeSeconds{0.0f};
    int cameraFocus{0}; // 0 = Full Shrub, 1 = Canes & Twigs, 2 = Leaves & Accents
    float camAzimuthDeg{34.0f};
    float camElevationDeg{20.0f};
    float camDistanceMeters{3.6f};
};

// Analytical Botanical Leaf, Needle, Blossom, Fruit & Accent Rasterizers (BotanicalLeafRasterizer.cpp)
void rasterizeBotanicalLeaf(
    DepthFrameBuffer& fb,
    const LeafRenderItem& leaf,
    int leafMorphologyId,
    const RgbF& baseColor,
    const RgbF& sunlitColor,
    const RgbF& stemColor,
    const Vec3& sunDir,
    float foliageAlphaScale,
    bool isCloseup,
    bool isWireframe
);

void rasterizeTreeBlossom3D(
    DepthFrameBuffer& fb,
    const Vec3& pos,
    const Vec3& dir,
    float sizeMeters,
    int bloomStyleId,
    const RgbF& primaryRgb,
    const RgbF& secondaryRgb,
    const Vec3& sunDir,
    uint32_t seedIdx,
    bool isCloseup
);

void rasterizeTreeFruit3D(
    DepthFrameBuffer& fb,
    const Vec3& pos,
    const Vec3& dir,
    float sizeMeters,
    int fruitStyleId,
    const RgbF& primaryRgb,
    const RgbF& secondaryRgb,
    const Vec3& sunDir,
    uint32_t seedIdx,
    bool isCloseup
);

void rasterizeBananaInfructescence3D(
    DepthFrameBuffer& fb,
    const Vec3& stemCrownPos,
    float outAzRad,
    float bunchScaleMeters,
    const RgbF& fruitPrimary,
    const RgbF& bellSecondary,
    const RgbF& stemColor,
    const Vec3& sunDir,
    bool isCloseup
);

void rasterizeShrubAccent3D(
    DepthFrameBuffer& fb,
    const Vec3& pos,
    const Vec3& dir,
    float sizeMeters,
    int accentKindId,
    float phase,
    const RgbF& accentRgb,
    const RgbF& foliageRgb,
    const Vec3& sunDir,
    bool isCloseup
);

void renderTreeReproductiveOrgans(
    DepthFrameBuffer& fb,
    const BotanicalTreeRenderParams& params,
    const std::vector<TreeBranchSegment>& branches,
    const std::vector<TreeLeafData>& rawLeaves,
    float heightSpan,
    const Vec3& sunDir,
    bool isCloseup
);

// Top-Level C++17 Scene Entry Points (Botanical3DSceneRenderer.cpp)
void renderTreeSceneToArgb(
    const BotanicalTreeRenderParams& params,
    int width,
    int height,
    int32_t* outArgbPixels,
    float* outStats8
);

void renderShrubSceneToArgb(
    const BotanicalShrubRenderParams& params,
    int width,
    int height,
    int32_t* outArgbPixels,
    float* outStats8
);

} // namespace scene
} // namespace vegetation
