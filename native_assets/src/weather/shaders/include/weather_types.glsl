// GLSL 460 - Weather Particle & Vertex Types
#ifndef WEATHER_TYPES_GLSL
#define WEATHER_TYPES_GLSL

struct PrecipitationParticle {
    vec4 position_lifetime; // xyz: world pos, w: remaining life [0..1]
    vec4 velocity_size;     // xyz: velocity vector, w: radius/size
    vec4 color_alpha;       // rgb: tint, a: opacity
};

struct LightningVertex {
    vec4 position;
    vec4 color_intensity;
};

#endif // WEATHER_TYPES_GLSL
