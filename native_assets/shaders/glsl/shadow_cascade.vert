#version 460
// Cascaded Shadow Map (CSM) Vertex Shader
// Reference: SaschaWillems/Vulkan (shadowmappingcascade/cascade.vert)

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoord;

layout(push_constant) uniform CascadePushConstants {
    mat4 modelMatrix;
    mat4 cascadeLightSpaceMatrix; // Current cascade projection * light view
    uint cascadeIndex;
} push;

layout(location = 0) out vec2 outTexCoord;

void main() {
    outTexCoord = inTexCoord;
    gl_Position = push.cascadeLightSpaceMatrix * push.modelMatrix * vec4(inPosition, 1.0);
}
