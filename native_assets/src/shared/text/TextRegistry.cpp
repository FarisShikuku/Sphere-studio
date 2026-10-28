#include "TextRegistry.h"
#include <cmath>
#include <algorithm>

namespace sphere {

TextRegistry& TextRegistry::get() {
    static TextRegistry instance;
    return instance;
}

TextRegistry::TextRegistry() {
    initializeDefaults();
}

void TextRegistry::initializeDefaults() {
    // 1. Build default SDF Font Atlas: Roboto Flex / JetBrains Mono metrics
    FontAtlas robotoSdf;
    robotoSdf.fontName = "Roboto Bold SDF";
    robotoSdf.assetPath = "textures/fonts/roboto_sdf.ktx";
    robotoSdf.format = FontAtlasFormat::SIGNED_DISTANCE_FIELD_R8;
    robotoSdf.atlasWidth = 1024;
    robotoSdf.atlasHeight = 1024;
    robotoSdf.fontSize = 48.0f;
    robotoSdf.lineHeight = 56.0f;
    robotoSdf.ascender = 40.0f;
    robotoSdf.descender = -12.0f;
    robotoSdf.distanceRange = 8.0f;

    // Populate standard 16x16 grid for ASCII 32 - 126
    const int cols = 16;
    const float cellUvSize = 1.0f / cols;
    for (uint32_t c = 32; c <= 126; ++c) {
        int idx = c - 32;
        int row = idx / cols;
        int col = idx % cols;

        GlyphMetric gm;
        gm.charCode = c;
        gm.uvMinX = col * cellUvSize;
        gm.uvMinY = row * cellUvSize;
        gm.uvMaxX = (col + 1) * cellUvSize;
        gm.uvMaxY = (row + 1) * cellUvSize;
        gm.widthPixels = 48.0f;
        gm.heightPixels = 48.0f;
        gm.bearingX = 2.0f;
        gm.bearingY = 38.0f;
        gm.advanceX = (c == ' ' ? 22.0f : (c == 'i' || c == 'l' || c == '!' || c == '.' ? 18.0f : (c == 'm' || c == 'w' || c == 'W' ? 44.0f : 30.0f)));

        robotoSdf.glyphs[c] = gm;
    }
    registerAtlas("roboto_bold", robotoSdf);

    // 2. Monospace Atlas for Developer HUD / Telemetry
    FontAtlas monoSdf = robotoSdf;
    monoSdf.fontName = "JetBrains Mono SDF";
    monoSdf.assetPath = "textures/fonts/jetbrains_mono_sdf.ktx";
    for (auto& pair : monoSdf.glyphs) {
        pair.second.advanceX = 28.0f; // Fixed advance
    }
    registerAtlas("jetbrains_mono", monoSdf);

    // Preset 1: Vulkan Telemetry HUD (Sascha Willems classic FPS overlay)
    {
        TextDef hud;
        hud.id = "vulkan_telemetry_hud";
        hud.name = "Vulkan GPU Telemetry HUD";
        hud.category = "Engine HUD";
        hud.description = "Real-time engine performance metrics overlay showing FPS, VRAM allocation, and draw call counts.";
        hud.textContent = "VK_DEVICE: Mali-G78 / Adreno 740\nFRAME: 6.94 ms (144 FPS)\nVRAM: 1,420 MB / 4,096 MB\nPIPELINES: 48 (Bindless Descriptor Set 0)";
        hud.fontId = "jetbrains_mono";
        hud.renderMode = TextRenderMode::SCREEN_OVERLAY_HUD;
        hud.alignment = TextAlign::LEFT;
        hud.fontSize = 20.0f;
        hud.sdfParams.textColor = {0.20f, 0.95f, 0.65f, 1.0f}; // Matrix / terminal green
        hud.sdfParams.enableOutline = true;
        hud.sdfParams.outlineWidth = 0.16f;
        hud.sdfParams.outlineColor = {0.02f, 0.05f, 0.03f, 1.0f};
        hud.sdfParams.enableDropShadow = true;
        hud.sdfParams.shadowOffset = {0.003f, -0.004f};
        hud.sdfParams.shadowColor = {0.0f, 0.0f, 0.0f, 0.85f};
        hud.showBackgroundBadge = true;
        hud.badgeColor = {0.04f, 0.08f, 0.06f, 0.85f};
        hud.badgeBorderColor = {0.15f, 0.60f, 0.35f, 0.50f};
        registerTextDef(hud);
    }

    // Preset 2: 3D Scene Anchor Billboard (Hovering over 3D item in world space)
    {
        TextDef billboard;
        billboard.id = "billboard_stage_anchor";
        billboard.name = "3D Stage Coordinate Anchor";
        billboard.category = "3D World Labels";
        billboard.description = "In-world 3D billboard with spherical camera locking, floating directly above the calibration cube.";
        billboard.textContent = "PBR STAGE ORIGIN [0.0, 1.25, 0.0]\nMATERIAL: RUSTICATED BRICK MASONRY\nSHADOW: 4-CASCADE PCF 5x5";
        billboard.fontId = "roboto_bold";
        billboard.renderMode = TextRenderMode::BILLBOARD_3D_WORLD;
        billboard.alignment = TextAlign::CENTER;
        billboard.fontSize = 28.0f;
        billboard.transform.position = {0.0f, 1.85f, 0.0f};
        billboard.transform.screenFacing = true;
        billboard.sdfParams.textColor = {0.98f, 0.98f, 1.0f, 1.0f};
        billboard.sdfParams.enableOutline = true;
        billboard.sdfParams.outlineWidth = 0.18f;
        billboard.sdfParams.outlineColor = {0.08f, 0.12f, 0.20f, 1.0f};
        billboard.sdfParams.enableDropShadow = true;
        billboard.sdfParams.shadowSoftness = 0.35f;
        billboard.showBackgroundBadge = true;
        billboard.badgeColor = {0.08f, 0.12f, 0.18f, 0.88f};
        billboard.badgeBorderColor = {0.30f, 0.65f, 1.0f, 0.75f};
        registerTextDef(billboard);
    }

    // Preset 3: Sci-Fi Cyberpunk Terminal (Emissive neon glow)
    {
        TextDef terminal;
        terminal.id = "scifi_cyber_terminal";
        terminal.name = "Cyberpunk Quantum Terminal";
        terminal.category = "Sci-Fi Displays";
        terminal.description = "High-energy cyan emissive text with multi-channel signed distance field glow and soft drop shadow.";
        terminal.textContent = "SYNCHRONIZATION COMPLETE\nATMOSPHERIC COHERENCE: 99.8%\nVULKAN RENDER PASS #04: OK";
        terminal.fontId = "jetbrains_mono";
        terminal.renderMode = TextRenderMode::SDF_DISTANCE_FIELD_2D;
        terminal.alignment = TextAlign::CENTER;
        terminal.fontSize = 32.0f;
        terminal.sdfParams.textColor = {0.15f, 0.90f, 1.0f, 1.0f}; // Cyan
        terminal.sdfParams.enableOutline = true;
        terminal.sdfParams.outlineWidth = 0.14f;
        terminal.sdfParams.outlineColor = {0.01f, 0.15f, 0.25f, 1.0f};
        terminal.sdfParams.enableGlow = true;
        terminal.sdfParams.glowRadius = 0.45f;
        terminal.sdfParams.glowColor = {0.10f, 0.80f, 1.0f, 0.70f};
        terminal.showBackgroundBadge = true;
        terminal.badgeColor = {0.03f, 0.07f, 0.12f, 0.90f};
        terminal.badgeBorderColor = {0.15f, 0.75f, 0.95f, 0.80f};
        registerTextDef(terminal);
    }

    // Preset 4: Cinematic Golden Title
    {
        TextDef title;
        title.id = "cinematic_gold_title";
        title.name = "Cinematic Golden Serif";
        title.category = "Cinematics";
        title.description = "Deep warm gold typography with elegant drop shadow and smooth antialiased outlines.";
        title.textContent = "SPHERE ENGINE\nCHRONICLES OF VULKAN";
        title.fontId = "roboto_bold";
        title.renderMode = TextRenderMode::SDF_DISTANCE_FIELD_2D;
        title.alignment = TextAlign::CENTER;
        title.fontSize = 38.0f;
        title.letterSpacing = 1.25f;
        title.sdfParams.textColor = {1.0f, 0.84f, 0.40f, 1.0f}; // Warm Gold
        title.sdfParams.enableOutline = true;
        title.sdfParams.outlineWidth = 0.22f;
        title.sdfParams.outlineColor = {0.20f, 0.10f, 0.02f, 1.0f};
        title.sdfParams.enableDropShadow = true;
        title.sdfParams.shadowOffset = {0.008f, -0.010f};
        title.sdfParams.shadowSoftness = 0.40f;
        title.sdfParams.shadowColor = {0.0f, 0.0f, 0.0f, 0.90f};
        title.showBackgroundBadge = false;
        registerTextDef(title);
    }

    // Preset 5: Critical Warning Holo-Decal
    {
        TextDef warning;
        warning.id = "critical_hazard_warning";
        warning.name = "Critical Hazard Holo-Banner";
        warning.category = "Industrial & Alerts";
        warning.description = "High-contrast warning banner with thick black stroke and amber/crimson alert color.";
        warning.textContent = "CAUTION: HIGH VOLTAGE RAY TRACING\nUNIFORM BUFFER OVERFLOW IMMINENT\nREDUCE CASCADE COUNT";
        warning.fontId = "jetbrains_mono";
        warning.renderMode = TextRenderMode::SDF_DISTANCE_FIELD_2D;
        warning.alignment = TextAlign::CENTER;
        warning.fontSize = 24.0f;
        warning.sdfParams.textColor = {1.0f, 0.28f, 0.20f, 1.0f}; // Crimson Amber
        warning.sdfParams.enableOutline = true;
        warning.sdfParams.outlineWidth = 0.28f;
        warning.sdfParams.outlineColor = {0.10f, 0.01f, 0.01f, 1.0f};
        warning.sdfParams.enableDropShadow = true;
        warning.sdfParams.shadowOffset = {0.005f, -0.006f};
        warning.showBackgroundBadge = true;
        warning.badgeColor = {0.15f, 0.03f, 0.03f, 0.90f};
        warning.badgeBorderColor = {1.0f, 0.25f, 0.20f, 0.90f};
        registerTextDef(warning);
    }
}

void TextRegistry::registerAtlas(const std::string& fontId, const FontAtlas& atlas) {
    m_atlases[fontId] = atlas;
}

const FontAtlas* TextRegistry::getAtlas(const std::string& fontId) const {
    auto it = m_atlases.find(fontId);
    if (it != m_atlases.end()) return &it->second;
    if (!m_atlases.empty()) return &m_atlases.begin()->second;
    return nullptr;
}

void TextRegistry::registerTextDef(const TextDef& textDef) {
    m_textDefs[textDef.id] = textDef;
    m_orderedDefs.push_back(textDef);
}

const TextDef* TextRegistry::getTextDef(const std::string& id) const {
    auto it = m_textDefs.find(id);
    if (it != m_textDefs.end()) return &it->second;
    return nullptr;
}

const std::vector<TextDef>& TextRegistry::getAllTextDefs() const {
    return m_orderedDefs;
}

std::vector<TextVertex> TextRegistry::generateTextQuads(
    const std::string& text,
    const std::string& fontId,
    float targetSize,
    TextAlign align,
    float letterSpacing,
    float lineHeightMultiplier,
    const std::array<float, 4>& tintColor
) const {
    std::vector<TextVertex> vertices;
    const FontAtlas* atlas = getAtlas(fontId);
    if (!atlas || text.empty()) return vertices;

    float scale = targetSize / atlas->fontSize;
    float currentY = 0.0f;
    float lineStep = atlas->lineHeight * lineHeightMultiplier * scale;

    // Split text by lines
    std::vector<std::string> lines;
    size_t start = 0;
    while (start < text.length()) {
        size_t end = text.find('\n', start);
        if (end == std::string::npos) {
            lines.push_back(text.substr(start));
            break;
        } else {
            lines.push_back(text.substr(start, end - start));
            start = end + 1;
        }
    }

    for (const auto& line : lines) {
        // Calculate line total width for alignment
        float lineWidth = 0.0f;
        for (char c : line) {
            uint32_t codepoint = static_cast<unsigned char>(c);
            const GlyphMetric* gm = atlas->getGlyph(codepoint);
            if (gm) {
                lineWidth += gm->advanceX * scale * letterSpacing;
            }
        }

        float cursorX = 0.0f;
        if (align == TextAlign::CENTER) {
            cursorX = -lineWidth * 0.5f;
        } else if (align == TextAlign::RIGHT) {
            cursorX = -lineWidth;
        }

        for (char c : line) {
            uint32_t codepoint = static_cast<unsigned char>(c);
            const GlyphMetric* gm = atlas->getGlyph(codepoint);
            if (!gm) continue;

            float x0 = cursorX + gm->bearingX * scale;
            float y0 = currentY + (atlas->ascender - gm->bearingY) * scale;
            float x1 = x0 + gm->widthPixels * scale;
            float y1 = y0 + gm->heightPixels * scale;

            float u0 = gm->uvMinX;
            float v0 = gm->uvMinY;
            float u1 = gm->uvMaxX;
            float v1 = gm->uvMaxY;

            // Quad Triangle 1
            vertices.push_back({{x0, y0, 0.0f}, {u0, v0}, tintColor});
            vertices.push_back({{x1, y0, 0.0f}, {u1, v0}, tintColor});
            vertices.push_back({{x0, y1, 0.0f}, {u0, v1}, tintColor});

            // Quad Triangle 2
            vertices.push_back({{x1, y0, 0.0f}, {u1, v0}, tintColor});
            vertices.push_back({{x1, y1, 0.0f}, {u1, v1}, tintColor});
            vertices.push_back({{x0, y1, 0.0f}, {u0, v1}, tintColor});

            cursorX += gm->advanceX * scale * letterSpacing;
        }

        currentY += lineStep;
    }

    return vertices;
}

} // namespace sphere
