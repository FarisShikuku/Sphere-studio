#include "TaaPipeline.h"
#include <algorithm>

namespace sphere::lighting {

static float Halton(uint32_t index, uint32_t base) {
    float f = 1.0f;
    float r = 0.0f;
    while (index > 0) {
        f = f / static_cast<float>(base);
        r = r + f * static_cast<float>(index % base);
        index = index / base;
    }
    return r;
}

TaaPipeline::TaaPipeline() {
    // Precompute 16-sample Halton (2, 3) sequence mapped to [-0.5, 0.5] pixel space
    for (size_t i = 0; i < kJitterSampleCount; ++i) {
        float x = Halton(static_cast<uint32_t>(i + 1), 2) - 0.5f;
        float y = Halton(static_cast<uint32_t>(i + 1), 3) - 0.5f;
        m_haltonSequence[i] = Vec2{x, y};
    }
}

void TaaPipeline::SetResolution(uint32_t width, uint32_t height) {
    m_width = std::max(1u, width);
    m_height = std::max(1u, height);
}

void TaaPipeline::AdvanceFrame() {
    m_frameIndex = (m_frameIndex + 1) % static_cast<uint32_t>(kJitterSampleCount);
}

Vec2 TaaPipeline::GetCurrentPixelJitter() const {
    return m_haltonSequence[m_frameIndex];
}

Vec2 TaaPipeline::GetCurrentClipJitter() const {
    Vec2 pixelJitter = GetCurrentPixelJitter();
    return Vec2{
        (pixelJitter.x * 2.0f) / static_cast<float>(m_width),
        (pixelJitter.y * 2.0f) / static_cast<float>(m_height)
    };
}

Mat4 TaaPipeline::ApplyJitterToProjection(const Mat4& proj) const {
    Mat4 jittered = proj;
    Vec2 clipJitter = GetCurrentClipJitter();
    jittered.m[8] += clipJitter.x;
    jittered.m[9] += clipJitter.y;
    return jittered;
}

GpuTaaParams TaaPipeline::BuildGpuTaaParams(const Mat4& currentViewProj, const Mat4& prevViewProj) const {
    GpuTaaParams gpu{};
    Vec2 clipJitter = GetCurrentClipJitter();

    gpu.jitterOffsetAndHistoryWeight = Vec4{
        clipJitter.x,
        clipJitter.y,
        m_historyWeight,
        static_cast<float>(m_frameIndex)
    };

    gpu.screenDimensions = Vec4{
        static_cast<float>(m_width),
        static_cast<float>(m_height),
        1.0f / static_cast<float>(m_width),
        1.0f / static_cast<float>(m_height)
    };

    gpu.currentToPreviousClip = prevViewProj; // Reprojection target
    return gpu;
}

} // namespace sphere::lighting
