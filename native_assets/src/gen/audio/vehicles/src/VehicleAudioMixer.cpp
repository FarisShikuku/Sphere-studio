#include "EngineAudioSystem.h"
#include <algorithm>
#include <cmath>

namespace sphere::audio {

void VehicleAudioMixer::MixBlockToPcm16(
    const float* engineSamples,
    const float* tireSamples,
    const float* oneShotSamples,
    int32_t sampleCount,
    float masterGain,
    int16_t* outPcm16
) {
    if (!outPcm16 || sampleCount <= 0) return;

    for (int32_t i = 0; i < sampleCount; ++i) {
        float mixed = (engineSamples ? engineSamples[i] : 0.0f) +
                      (tireSamples ? tireSamples[i] : 0.0f) +
                      (oneShotSamples ? oneShotSamples[i] : 0.0f);
        mixed *= masterGain;

        // Musical soft saturation prevents digital clipping when horn + full throttle overlap
        float limited = std::tanh(mixed * 0.92f);
        int32_t s = static_cast<int32_t>(std::clamp(limited, -1.0f, 1.0f) * 32767.0f);
        outPcm16[i] = static_cast<int16_t>(s);
    }
}

} // namespace sphere::audio
