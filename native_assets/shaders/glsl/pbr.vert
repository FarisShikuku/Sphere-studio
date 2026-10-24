#version 460
// PBR Standard Forward/Deferred Pipeline with SaschaWillems Shadow Mapping
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoord;

layout(push_constant) uniform PushConstants {
    mat4 modelMatrix;
    mat4 viewProjMatrix;
    mat4 lightSpaceMatrix; // Added for SaschaWillems Shadow Mapping
} push;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outTangent;
layout(location = 3) out vec2 outTexCoord;
layout(location = 4) out vec4 outShadowCoord;

// Vulkan clip-space bias matrix (maps [-1, 1] XY to [0, 1] texture coordinates)
// Reference: SaschaWillems/Vulkan shadowmapping/scene.vert
const mat4 biasMat = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 1.0
);

void main() {
    vec4 worldPos = push.modelMatrix * vec4(inPosition, 1.0);
    outWorldPos = worldPos.xyz;
    outNormal = normalize(mat3(push.modelMatrix) * inNormal);
    outTangent = vec4(normalize(mat3(push.modelMatrix) * inTangent.xyz), inTangent.w);
    outTexCoord = inTexCoord;
    outShadowCoord = (biasMat * push.lightSpaceMatrix) * worldPos;
    gl_Position = push.viewProjMatrix * worldPos;
}
