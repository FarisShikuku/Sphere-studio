#include "WeatherAudioSystem.h"
#include <algorithm>
#include <cmath>

namespace sphere::audio {

void WeatherAudioMixer::MixBlockToPcm16(
    const float* rainSamples,
    const float* windSamples,
    const float* ambienceSamples,
    const float* oneShotSamples,
    int32_t sampleCount,
    float masterGain,
    int16_t* outPcm16
) {
    if (!outPcm16 || sampleCount <= 0) return;

    for (int32_t i = 0; i < sampleCount; ++i) {
        float mixed = (rainSamples ? rainSamples[i] : 0.0f) +
                      (windSamples ? windSamples[i] : 0.0f) +
                      (ambienceSamples ? ambienceSamples[i] : 0.0f) +
                      (oneShotSamples ? oneShotSamples[i] : 0.0f);
        mixed *= masterGain;

        float limited = std::tanh(mixed * 0.95f);
        int32_t s = static_cast<int32_t>(std::clamp(limited, -1.0f, 1.0f) * 32767.0f);
        outPcm16[i] = static_cast<int16_t>(s);
    }
}

} // namespace sphere::audio
