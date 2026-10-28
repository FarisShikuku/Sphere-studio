#pragma once
#include "TextDef.h"
#include "FontAtlas.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace sphere {

/**
 * TextRegistry coordinates typography presets, font atlases, and quad mesh generation
 * for Sascha Willems style Vulkan text rendering.
 */
class TextRegistry {
public:
    static TextRegistry& get();

    void initializeDefaults();

    // Font Atlas registration & retrieval
    void registerAtlas(const std::string& fontId, const FontAtlas& atlas);
    const FontAtlas* getAtlas(const std::string& fontId) const;

    // Text template registration & retrieval
    void registerTextDef(const TextDef& textDef);
    const TextDef* getTextDef(const std::string& id) const;
    const std::vector<TextDef>& getAllTextDefs() const;

    // Mesh quad synthesizer: generates vertex list from string and font metrics
    // Generates 6 vertices (2 triangles) per glyph with proper UV mapping
    std::vector<TextVertex> generateTextQuads(
        const std::string& text,
        const std::string& fontId,
        float targetSize,
        TextAlign align,
        float letterSpacing = 1.0f,
        float lineHeightMultiplier = 1.2f,
        const std::array<float, 4>& tintColor = {1.0f, 1.0f, 1.0f, 1.0f}
    ) const;

private:
    TextRegistry();
    ~TextRegistry() = default;

    std::unordered_map<std::string, FontAtlas> m_atlases;
    std::unordered_map<std::string, TextDef> m_textDefs;
    std::vector<TextDef> m_orderedDefs;
};

} // namespace sphere
