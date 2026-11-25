#ifndef OCTAHEDRAL_IMPOSTOR_GLSL
#define OCTAHEDRAL_IMPOSTOR_GLSL

vec3 decodeOctahedralNormal(vec2 uv) {
    vec2 f = uv * 2.0 - 1.0;
    vec3 n = vec3(f.x, 1.0 - abs(f.x) - abs(f.y), f.y);
    float t = clamp(-n.y, 0.0, 1.0);
    n.x += (n.x >= 0.0) ? -t : t;
    n.z += (n.z >= 0.0) ? -t : t;
    return normalize(n);
}

#endif // OCTAHEDRAL_IMPOSTOR_GLSL
