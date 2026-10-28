#pragma once
#include "FontAtlas.h"
#include <string>
#include <vector>
#include <array>

namespace sphere {

enum class TextRenderMode : uint32_t {
    SCREEN_OVERLAY_HUD = 0,   // 2D orthographic screen space overlay (Sascha Willems textoverlay)
    SDF_DISTANCE_FIELD_2D,    // 2D crisp vector-quality signed distance field
    BILLBOARD_3D_WORLD,       // 3D world space text billboard rotating toward camera
    BILLBOARD_3D_AXIAL,       // 3D cylindrical billboard locked to vertical Y-axis
    SURFACE_DECAL_3D          // Flat in-world text decal oriented along surface normals
};

enum class TextAlign : uint32_t {
    LEFT = 0,
    CENTER,
    RIGHT,
    JUSTIFY
};

/**
 * Shading and distance field styling parameters.
 * Closely mapped to Sascha Willems distancefieldfonts.frag uniforms.
 */
struct SdfShadingParams {
    std::array<float, 4> textColor = {1.0f, 1.0f, 1.0f, 1.0f};      // Primary RGBA text color
    bool enableOutline = true;
    float outlineWidth = 0.12f;                                        // Outline stroke thickness [0.0 - 0.5]
    std::array<float, 4> outlineColor = {0.05f, 0.05f, 0.08f, 1.0f};  // Outline stroke RGBA
    bool enableDropShadow = true;
    std::array<float, 2> shadowOffset = {0.005f, -0.006f};             // UV or screen space offset
    float shadowSoftness = 0.25f;                                      // Shadow feathering / blur [0.0 - 1.0]
    std::array<float, 4> shadowColor = {0.0f, 0.0f, 0.0f, 0.75f};     // Drop shadow RGBA
    float smoothing = 0.045f;                                          // Subpixel edge AA smoothstep range
    float thicknessBias = 0.0f;                                        // Bold / thin font dilation bias [-0.2, 0.3]
    bool enableGlow = false;
    float glowRadius = 0.35f;                                          // Emissive outer bloom radius
    std::array<float, 4> glowColor = {0.2f, 0.8f, 1.0f, 0.6f};        // Neon / cyber glow RGBA
};

/**
 * 3D Transformation and world placement for in-scene text.
 */
struct TextTransform3D {
    std::array<float, 3> position = {0.0f, 1.5f, 0.0f}; // World space coordinates (meters)
    std::array<float, 3> rotation = {0.0f, 0.0f, 0.0f}; // Euler angles in degrees
    float scale = 1.0f;                                 // Uniform world scale
    bool screenFacing = true;                           // Automatically face active scene camera
    bool lockVerticalY = false;                         // Constrain billboard to horizontal spin only
};

/**
 * Complete definition of a text element or typography preset.
 */
struct TextDef {
    std::string id;
    std::string name;
    std::string category;     // e.g. "HUD & Telemetry", "3D Labels", "Sci-Fi Terminals", "Cinematic"
    std::string description;

    std::string textContent = "Sphere Engine 3D";
    std::string fontId = "roboto_bold";

    TextRenderMode renderMode = TextRenderMode::SDF_DISTANCE_FIELD_2D;
    TextAlign alignment = TextAlign::CENTER;

    float fontSize = 32.0f;           // Visual scale in pixels (2D) or meters (3D)
    float letterSpacing = 1.0f;       // Horizontal tracking multiplier
    float lineHeightMultiplier = 1.2f;// Vertical leading multiplier
    float maxWidth = 600.0f;          // Wrap constraint (0 = unconstrained)

    SdfShadingParams sdfParams;
    TextTransform3D transform;

    // Background plaque / frame styling
    bool showBackgroundBadge = true;
    std::array<float, 4> badgeColor = {0.08f, 0.10f, 0.14f, 0.85f};
    std::array<float, 4> badgeBorderColor = {0.25f, 0.60f, 0.95f, 0.60f};
    float badgeCornerRadius = 8.0f;
    float badgePadding = 12.0f;
};

/**
 * Low-level vertex layout for Vulkan vertex buffer.
 * Matches Sascha Willems VulkanTextOverlay and distancefieldfonts pipeline.
 */
struct TextVertex {
    std::array<float, 3> pos;   // X, Y, Z
    std::array<float, 2> uv;    // U, V texture coords
    std::array<float, 4> color; // Vertex color / tint
};

} // namespace sphere
