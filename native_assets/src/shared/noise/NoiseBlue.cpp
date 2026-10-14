#include "NoiseBlue.h"
#include <cmath>
#include <algorithm>
#include <random>

namespace sphere {

std::vector<uint8_t> NoiseBlue::GenerateTexture(uint32_t width, uint32_t height) {
    if (width == 0 || height == 0) return {};

    uint32_t totalPixels = width * height;
    std::vector<uint8_t> texture(totalPixels, 0);

    // Void-and-Cluster / Mitchell Best-Candidate Algorithm for tileable blue noise
    std::vector<float> energy(totalPixels, 0.0f);
    std::vector<bool> taken(totalPixels, false);

    const float sigma = 1.9f;
    const float twoSigmaSq = 2.0f * sigma * sigma;
    const int radius = static_cast<int>(std::ceil(3.0f * sigma));

    std::mt19937 rng(1337);
    std::uniform_int_distribution<uint32_t> distW(0, width - 1);
    std::uniform_int_distribution<uint32_t> distH(0, height - 1);

    // Generate rank distribution
    for (uint32_t rank = 0; rank < totalPixels; ++rank) {
        int bestX = -1;
        int bestY = -1;
        float minEnergy = 1e30f;

        // Sample candidates on toroidal grid
        const int numCandidates = std::min(40u, totalPixels - rank);
        for (int c = 0; c < numCandidates; ++c) {
            int cx = static_cast<int>(distW(rng));
            int cy = static_cast<int>(distH(rng));
            int idx = cy * static_cast<int>(width) + cx;

            if (taken[idx]) {
                // If collision, find next free slot
                for (uint32_t k = 0; k < totalPixels; ++k) {
                    uint32_t sIdx = (idx + k) % totalPixels;
                    if (!taken[sIdx]) {
                        cx = static_cast<int>(sIdx % width);
                        cy = static_cast<int>(sIdx / width);
                        idx = static_cast<int>(sIdx);
                        break;
                    }
                }
            }

            if (!taken[idx] && energy[idx] < minEnergy) {
                minEnergy = energy[idx];
                bestX = cx;
                bestY = cy;
            }
        }

        if (bestX < 0) {
            for (uint32_t i = 0; i < totalPixels; ++i) {
                if (!taken[i]) {
                    bestX = static_cast<int>(i % width);
                    bestY = static_cast<int>(i / width);
                    break;
                }
            }
        }

        int chosenIdx = bestY * static_cast<int>(width) + bestX;
        taken[chosenIdx] = true;

        // Assign normalized byte value [0..255]
        texture[chosenIdx] = static_cast<uint8_t>((static_cast<uint64_t>(rank) * 255u) / (totalPixels - 1));

        // Splat Gaussian energy kernel around chosen sample with toroidal wrapping
        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                float distSq = static_cast<float>(dx * dx + dy * dy);
                float weight = std::exp(-distSq / twoSigmaSq);

                int wrapX = (bestX + dx) % static_cast<int>(width);
                if (wrapX < 0) wrapX += width;
                int wrapY = (bestY + dy) % static_cast<int>(height);
                if (wrapY < 0) wrapY += height;

                energy[wrapY * width + wrapX] += weight;
            }
        }
    }

    return texture;
}

} // namespace sphere
