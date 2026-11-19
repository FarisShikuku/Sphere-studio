#pragma once

#include "WeatherAudioParams.h"
#include <cstdint>
#include <mutex>
#include <vector>

namespace sphere::audio {

enum class WeatherOneShotId : int32_t {
    THUNDER_CLAP_CLOSE = 0,
    THUNDER_RUMBLE_DISTANT = 1,
    GUST_WHOOSH = 2,
    HAIL_BURST = 3
};

struct WeatherPcmClip {
    std::vector<float> samples; // mono normalized [-1, 1] at 44.1 kHz
    uint32_t sample_rate = 44100;
};

struct ActiveWeatherVoice {
    WeatherOneShotId id = WeatherOneShotId::THUNDER_CLAP_CLOSE;
    size_t cursor = 0;
    float gain = 1.0f;
    bool active = false;
};

/**
 * WeatherAudioMixer:
 * Zero-allocation mono PCM-16 mixing stage combining procedural rain sheets,
 * resonant sweeping wind formants, snow/fog ambience, and RAM-cached thunder cracks.
 */
class WeatherAudioMixer {
public:
    static void MixBlockToPcm16(
        const float* rainSamples,
        const float* windSamples,
        const float* ambienceSamples,
        const float* oneShotSamples,
        int32_t sampleCount,
        float masterGain,
        int16_t* outPcm16
    );
};

/**
 * WeatherAudioSystem:
 * Singleton procedural + RAM-cached PCM weather sound synthesizer for Dynamic Weather Studio.
 * - Rain: High-density filtered noise + stochastic individual drop splashes + heavy downpour bed.
 * - Wind: Sweeping resonant bandpass filter modulated by gust LFOs proportional to wind_speed_kmh.
 * - Snow & Fog: Crystalline high-band air shimmer and deep low atmospheric mist bed.
 * - Thunder & Gusts: Zero-latency RAM-cached 44.1kHz PCM one-shot bursts synced to lightning.
 */
class WeatherAudioSystem {
public:
    static constexpr uint32_t kSampleRate = 44100;

    static WeatherAudioSystem& Get();

    void SetWeatherParams(
        float rainIntensity,
        float snowIntensity,
        float windSpeedKmh,
        float windAzimuthDeg,
        float fogDensity,
        bool stormActive,
        bool enabled,
        bool muted
    );

    void TriggerThunder(bool closeStrike, float gain = 1.0f);
    void TriggerOneShot(WeatherOneShotId id, float gain = 1.0f);

    void RenderAudioBlock(int16_t* outPcm16, int32_t sampleCount);

    WeatherAudioTelemetry GetTelemetry() const;
    bool HasAudibleSignal() const;

private:
    WeatherAudioSystem();
    void BakeOneShotClips();
    float NextNoise();

    mutable std::mutex m_Mutex;
    WeatherAudioParams m_Params;

    // Filter & LFO states
    float m_RainLp = 0.0f;
    float m_RainHp = 0.0f;
    float m_RainPrevIn = 0.0f;
    float m_RainSplashEnv = 0.0f;

    float m_WindBpState1 = 0.0f;
    float m_WindBpState2 = 0.0f;
    float m_WindLow = 0.0f;
    float m_GustPhase = 0.0f;
    float m_GustPhase2 = 0.37f;

    float m_AmbienceFilter = 0.0f;
    float m_CurrentThunderLevel = 0.0f;
    uint32_t m_RngState = 0x517CC1B7u;

    WeatherPcmClip m_ClipThunderClose;
    WeatherPcmClip m_ClipThunderDistant;
    WeatherPcmClip m_ClipGustWhoosh;
    WeatherPcmClip m_ClipHailBurst;

    static constexpr int kMaxVoices = 6;
    ActiveWeatherVoice m_Voices[kMaxVoices];

    std::vector<float> m_RainBuf;
    std::vector<float> m_WindBuf;
    std::vector<float> m_AmbBuf;
    std::vector<float> m_OneShotBuf;
};

} // namespace sphere::audio
