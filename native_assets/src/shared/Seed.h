#pragma once
#include <cstdint>

namespace sphere {
class Seed {
public:
    static uint32_t Mix(uint32_t a, uint32_t b);
    static float HashToFloat(uint32_t h);
    static float RandomFloat(uint32_t& state);
};
}\n