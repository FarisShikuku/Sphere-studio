#version 460
// Shadow Map Offscreen Depth Vertex Shader
// Reference: SaschaWillems/Vulkan (shadowmapping/offscreen.vert)

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoord;

layout(push_constant) uniform ShadowPushConstants {
    mat4 modelMatrix;
    mat4 lightSpaceMatrix; // Light projection * light view matrix
} push;

layout(location = 0) out vec2 outTexCoord;

void main() {
    outTexCoord = inTexCoord;
    gl_Position = push.lightSpaceMatrix * push.modelMatrix * vec4(inPosition, 1.0);
}
