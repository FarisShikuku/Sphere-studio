#ifndef AUDIO_COMMON_GLSL
#define AUDIO_COMMON_GLSL

const float AUDIO_TWO_PI = 6.28318530718;

float softClipAudio(float sample) {
    return tanh(sample * 0.92);
}

uint audioHash(uint n) {
    n = (n << 13u) ^ n;
    return n * (n * n * 15731u + 789221u) + 1376312589u;
}

float whiteNoise(uint idx, uint seed) {
    uint h = audioHash(idx ^ seed);
    return (float(h & 0x00FFFFFFu) / 8388607.5) - 1.0;
}

#endif
