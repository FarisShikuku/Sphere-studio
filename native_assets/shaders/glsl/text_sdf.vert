#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;

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

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec4 outColor;
layout (location = 2) out vec2 outShadowUV;

void main() {
    outUV = inUV;
    outColor = inColor * pushConstants.textColor;
    outShadowUV = inUV - pushConstants.shadowOffset;
    gl_Position = pushConstants.projection * vec4(inPos, 1.0);
}
