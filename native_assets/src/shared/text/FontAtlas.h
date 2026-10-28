#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace sphere {

/**
 * Format of the precomputed glyph texture atlas.
 * Aligned with Sascha Willems Vulkan textoverlay & distancefieldfonts samples.
 */
enum class FontAtlasFormat : uint32_t {
    BITMAP_MONO_R8 = 0,      // Uncompressed 8-bit alpha mask (classic Sascha Willems textoverlay)
    SIGNED_DISTANCE_FIELD_R8, // Single-channel signed distance field (SDF)
    MULTI_CHANNEL_SDF_RGB8,   // Multi-channel signed distance field (MSDF) for sharp corners
    DISTANCE_FIELD_RGBA8      // SDF + precomputed interior glow or ambient occlusion
};

/**
 * Glyph metrics and UV texture coordinates within the atlas.
 */
struct GlyphMetric {
    uint32_t charCode = 0;       // ASCII / Unicode codepoint
    float uvMinX = 0.0f;        // Normalized atlas UV coordinates
    float uvMinY = 0.0f;
    float uvMaxX = 0.0f;
    float uvMaxY = 0.0f;
    float widthPixels = 0.0f;   // Glyph bounding box width in pixels
    float heightPixels = 0.0f;  // Glyph bounding box height in pixels
    float bearingX = 0.0f;      // Horizontal offset from cursor to glyph left edge
    float bearingY = 0.0f;      // Vertical offset from baseline to glyph top edge
    float advanceX = 0.0f;      // Horizontal advance to the next character
};

/**
 * FontAtlas metadata and glyph lookup table.
 */
struct FontAtlas {
    std::string fontName;
    std::string assetPath;
    FontAtlasFormat format = FontAtlasFormat::SIGNED_DISTANCE_FIELD_R8;
    uint32_t atlasWidth = 1024;
    uint32_t atlasHeight = 1024;
    float fontSize = 32.0f;       // Native baking point size
    float lineHeight = 40.0f;     // Vertical line spacing
    float ascender = 28.0f;       // Distance from baseline to highest point
    float descender = -8.0f;      // Distance from baseline to lowest point
    float distanceRange = 8.0f;   // SDF pixel spread / distance range in pixels

    std::unordered_map<uint32_t, GlyphMetric> glyphs;

    // Helper to fetch glyph, or fallback to '?' or space
    const GlyphMetric* getGlyph(uint32_t codepoint) const {
        auto it = glyphs.find(codepoint);
        if (it != glyphs.end()) return &it->second;
        auto fallback = glyphs.find(static_cast<uint32_t>('?'));
        if (fallback != glyphs.end()) return &fallback->second;
        return nullptr;
    }
};

} // namespace sphere
