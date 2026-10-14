#ifndef MATH_COMMON_GLSL
#define MATH_COMMON_GLSL

const float PI = 3.14159265358979323846;
const float TWO_PI = 6.28318530717958647692;
const float HALF_PI = 1.57079632679489661923;
const float INV_PI = 0.31830988618379067154;

// Unpack two-component normal map (BC5 / RG)
vec3 UnpackNormal(vec2 packedNormal) {
    vec2 n = packedNormal * 2.0 - 1.0;
    float z = sqrt(max(0.0, 1.0 - dot(n, n)));
    return vec3(n, z);
}

// Construct TBN frame from vertex normal and tangent
mat3 ConstructTBN(vec3 normal, vec4 tangent) {
    vec3 N = normalize(normal);
    vec3 T = normalize(tangent.xyz - N * dot(tangent.xyz, N));
    vec3 B = cross(N, T) * tangent.w;
    return mat3(T, B, N);
}

// Linear depth reconstruction
float LinearizeDepth(float depth, float nearPlane, float farPlane) {
    return (nearPlane * farPlane) / (farPlane - depth * (farPlane - nearPlane));
}

#endif // MATH_COMMON_GLSL
