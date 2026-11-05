#pragma once
#include "scene/Transform.h"
#include <array>

namespace sphere::lighting {

/**
 * TaaPipeline - Temporal Anti-Aliasing & Subpixel Accumulation
 * Referencing Gaukler/PlainRenderer TAA implementation.
 *
 * Provides:
 * - 16-sample Halton (2,3) subpixel camera projection jitter
 * - Reprojection matrix computation with velocity buffer support
 * - YCoCg color bounding-box neighborhood clamping against ghosting
 */

struct alignas(16) GpuTaaParams {
    Vec4 jitterOffsetAndHistoryWeight; // xy = jitter offset in clip-space, z = history weight (e.g. 0.90), w = frameIndex
    Vec4 screenDimensions;             // xy = width, height, zw = 1/width, 1/height
    Mat4 currentToPreviousClip;        // Reprojection matrix
};

class TaaPipeline {
public:
    TaaPipeline();
    ~TaaPipeline() = default;

    void SetResolution(uint32_t width, uint32_t height);
    void AdvanceFrame();

    // Gets current subpixel offset in pixels (-0.5 to +0.5)
    Vec2 GetCurrentPixelJitter() const;

    // Gets current subpixel offset in clip-space (-1 to +1 / res)
    Vec2 GetCurrentClipJitter() const;

    // Modifies projection matrix with current subpixel jitter
    Mat4 ApplyJitterToProjection(const Mat4& proj) const;

    void SetHistoryWeight(float weight) { m_historyWeight = std::clamp(weight, 0.5f, 0.98f); }
    float GetHistoryWeight() const { return m_historyWeight; }

    uint32_t GetFrameIndex() const { return m_frameIndex; }

    GpuTaaParams BuildGpuTaaParams(const Mat4& currentViewProj, const Mat4& prevViewProj) const;

private:
    uint32_t m_width = 1920;
    uint32_t m_height = 1080;
    uint32_t m_frameIndex = 0;
    float m_historyWeight = 0.92f;

    static constexpr size_t kJitterSampleCount = 16;
    std::array<Vec2, kJitterSampleCount> m_haltonSequence;
};

} // namespace sphere::lighting
