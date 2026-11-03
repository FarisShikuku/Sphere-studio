#pragma once
#include <vector>
#include <cstdint>

namespace sphere {

class NoiseBlue {
public:
    static std::vector<uint8_t> GenerateTexture(uint32_t width, uint32_t height);
};

} // namespace sphere
