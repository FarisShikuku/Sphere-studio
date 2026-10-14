#version 460
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoord;

layout(push_constant) uniform PushConstants {
    mat4 modelMatrix;
    mat4 viewProjMatrix;
} push;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outTangent;
layout(location = 3) out vec2 outTexCoord;

void main() {
    vec4 worldPos = push.modelMatrix * vec4(inPosition, 1.0);
    outWorldPos = worldPos.xyz;
    outNormal = normalize(mat3(push.modelMatrix) * inNormal);
    outTangent = vec4(normalize(mat3(push.modelMatrix) * inTangent.xyz), inTangent.w);
    outTexCoord = inTexCoord;
    gl_Position = push.viewProjMatrix * worldPos;
}
