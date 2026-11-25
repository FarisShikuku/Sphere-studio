#version 450
// ============================================================================
// tree_vulkan.vert - Vulkan 1.2/1.3 Real-Time Procedural Tree Vertex Shader
// Implements Nemanja Stojanovic (2016) Leonardo da Vinci Pipe Model + Wind Sway
// Vulkan Y-down NDC clip-space transformation and UBO binding.
// ============================================================================

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inColor;
layout(location = 4) in float inWindWeight;
layout(location = 5) in uint inIsLeaf;

layout(set = 0, binding = 0) uniform TreeVulkanUBO {
    mat4 projection;
    mat4 view;
    mat4 model;
    vec4 windParams; // x: speed, y: directionRad, z: flexibility, w: timeSeconds
    vec4 sunDirection;
    vec4 ambientColor;
} ubo;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTexCoord;
layout(location = 3) out vec4 outColor;
layout(location = 4) out flat uint outIsLeaf;
layout(location = 5) out float outSunlitFactor;

void main() {
    vec3 worldPos = (ubo.model * vec4(inPosition, 1.0)).xyz;

    // Biomechanical wind sway deflection based on inWindWeight
    float windSpeed = ubo.windParams.x;
    float windDir = ubo.windParams.y;
    float windFlex = ubo.windParams.z;
    float timeSec = ubo.windParams.w;

    if (windSpeed > 0.05 && inWindWeight > 0.001) {
        float wave = sin(timeSec * (1.2 + windSpeed * 0.25) + worldPos.y * 0.35 + worldPos.x * 0.2);
        float gust = sin(timeSec * 0.65 + worldPos.z * 0.15) * 0.35;
        float bend = inWindWeight * inWindWeight * (windSpeed * 0.04) * windFlex;
        float total = (wave + gust) * bend;

        worldPos.x += total * cos(windDir);
        worldPos.z += total * sin(windDir);
        worldPos.y -= 0.1 * total * total; // slight gravitational droop under bend
    }

    mat3 normalMatrix = transpose(inverse(mat3(ubo.model)));
    vec3 worldNormal = normalize(normalMatrix * inNormal);

    outWorldPos = worldPos;
    outNormal = worldNormal;
    outTexCoord = inTexCoord;
    outColor = inColor;
    outIsLeaf = inIsLeaf;

    // Sunlit exposure factor
    vec3 sunDir = normalize(ubo.sunDirection.xyz);
    outSunlitFactor = clamp(dot(worldNormal, sunDir) * 0.5 + 0.5, 0.0, 1.0);

    // Vulkan Clip Space transformation (Y-down NDC)
    gl_Position = ubo.projection * ubo.view * vec4(worldPos, 1.0);
}
