#include "Seed.h"

namespace sphere {
uint32_t Seed::Mix(uint32_t a, uint32_t b) {
    uint32_t h = a ^ (b * 0x85ebca6b);
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

float Seed::HashToFloat(uint32_t h) {
    return static_cast<float>(h & 0x00FFFFFF) / static_cast<float>(0x01000000);
}

float Seed::RandomFloat(uint32_t& state) {
    state = state * 1664525u + 1013904223u;
    return HashToFloat(state);
}
}\n