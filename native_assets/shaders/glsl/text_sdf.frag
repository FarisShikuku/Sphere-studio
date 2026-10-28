#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2D samplerFontAtlas;

layout (push_constant) uniform PushConstants {
    mat4 projection;
    vec4 textColor;
    vec4 outlineColor;
    vec4 shadowColor;
    vec2 shadowOffset;
    float outlineWidth;
    float outlineEdge;
    float smoothing;
    float thicknessBias;
    float glowRadius;
    vec4 glowColor;
    uint renderFlags;
} pushConstants;

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec2 inShadowUV;

layout (location = 0) out vec4 outFragColor;

void main() {
    // Distance field sample: 0.5 is the exact glyph boundary
    float distance = texture(samplerFontAtlas, inUV).r;
    float edge = pushConstants.outlineEdge; // default 0.5
    float smoothDelta = max(pushConstants.smoothing, fwidth(distance) * 0.5);

    // Primary text alpha with smoothstep edge antialiasing
    float textAlpha = smoothstep(edge - smoothDelta, edge + smoothDelta, distance);

    vec4 finalColor = inColor;

    // Drop Shadow evaluation (Bit 1 in renderFlags)
    if ((pushConstants.renderFlags & (1u << 1)) != 0u) {
        float shadowDist = texture(samplerFontAtlas, inShadowUV).r;
        float shadowAlpha = smoothstep(edge - pushConstants.smoothing * 3.0, edge + pushConstants.smoothing * 3.0, shadowDist) * pushConstants.shadowColor.a;
        finalColor = mix(vec4(pushConstants.shadowColor.rgb, shadowAlpha), finalColor, textAlpha);
    }

    // Outline stroke evaluation (Bit 0 in renderFlags)
    if ((pushConstants.renderFlags & (1u << 0)) != 0u && pushConstants.outlineWidth > 0.0) {
        float outlineThreshold = edge - pushConstants.outlineWidth;
        float outlineAlpha = smoothstep(outlineThreshold - smoothDelta, outlineThreshold + smoothDelta, distance);
        vec4 stroke = vec4(pushConstants.outlineColor.rgb, pushConstants.outlineColor.a * outlineAlpha);
        finalColor = mix(stroke, inColor, textAlpha);
        finalColor.a = max(textAlpha, outlineAlpha * pushConstants.outlineColor.a);
    } else {
        finalColor.a *= textAlpha;
    }

    // Outer glow evaluation (Bit 2 in renderFlags)
    if ((pushConstants.renderFlags & (1u << 2)) != 0u && pushConstants.glowRadius > 0.0) {
        float glowThreshold = edge - pushConstants.glowRadius;
        float glowAlpha = smoothstep(glowThreshold, edge, distance) * pushConstants.glowColor.a;
        finalColor = mix(vec4(pushConstants.glowColor.rgb, glowAlpha), finalColor, textAlpha);
    }

    if (finalColor.a < 0.01) {
        discard;
    }

    outFragColor = finalColor;
}
