#ifndef SPHERE_WEATHER_AUDIO_COMMON_GLSL
#define SPHERE_WEATHER_AUDIO_COMMON_GLSL

const float TWO_PI = 6.28318530718;

float weatherAudioHash(uint state) {
    uint x = state * 747796405u + 2891336453u;
    uint word = ((x >> ((x >> 28u) + 4u)) ^ x) * 277803737u;
    uint res = (word >> 22u) ^ word;
    return (float(res) / 4294967295.0) * 2.0 - 1.0;
}

#endif
