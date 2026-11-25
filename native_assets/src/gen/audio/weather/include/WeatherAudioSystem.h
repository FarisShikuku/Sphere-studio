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

struct RainDropletVoice {
    float phase = 0.0f;
    float phaseInc = 0.0f;
    float freqDecay = 0.9997f;
    float env = 0.0f;
    float envDecay = 0.996f;
    float amp = 0.0f;
    bool active = false;
};

struct WeatherBiquad {
    float z1 = 0.0f;
    float z2 = 0.0f;
    float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
    float a1 = 0.0f, a2 = 0.0f;

    void SetLowPass(float cutoffHz, float q, float sampleRate);
    void SetBandPass(float centerHz, float q, float sampleRate);
    float Process(float x);
    void Reset() { z1 = 0.0f; z2 = 0.0f; }
};

/**
 * WeatherAudioMixer:
 * Zero-allocation mono PCM-16 mixing stage combining granular droplet rain,
 * resonant sweeping wind formants, warm snow/fog ambience, and RAM-cached thunder cracks.
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
 * - Eliminates raw high-pass white-noise static via Kellet pink/brown filtering & biquad shaping.
 * - Polyphonic 32-voice resonant water droplet synthesis over warm low-passed rain sheet.
 * - Dual-formant resonant wind gusts and multi-stage low-passed sub-bass thunder booms.
 * - Supports optional real audio sample overrides uploaded from app assets (WAV/OGG/MP3 decoded).
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

    /**
     * Uploads a real decoded mono PCM16 sample into RAM cache to override procedural synthesis.
     * Slots: 10="rain_loop", 11="wind_loop", 12="thunder_close", 13="thunder_distant"
     */
    void UploadCustomSample(int32_t slotId, const int16_t* pcm16, int32_t sampleCount, int32_t sampleRate);

    void RenderAudioBlock(int16_t* outPcm16, int32_t sampleCount);

    WeatherAudioTelemetry GetTelemetry() const;
    bool HasAudibleSignal() const;

private:
    WeatherAudioSystem();
    void BakeOneShotClips();
    float NextWhiteNoise();
    float NextPinkNoise();
    void SpawnRainDroplet(float rainIntensity);

    mutable std::mutex m_Mutex;
    WeatherAudioParams m_Params;

    // Pink & Brown noise states
    float m_PinkB0 = 0.0f, m_PinkB1 = 0.0f, m_PinkB2 = 0.0f;
    float m_PinkB3 = 0.0f, m_PinkB4 = 0.0f, m_PinkB5 = 0.0f, m_PinkB6 = 0.0f;
    float m_BrownState = 0.0f;

    // Biquad shaping filters (eliminates static hiss)
    WeatherBiquad m_RainBedLp;
    WeatherBiquad m_RainMidWarmth;
    WeatherBiquad m_WindLowBp;
    WeatherBiquad m_WindAirBp;
    WeatherBiquad m_AmbienceLp;

    float m_GustPhase = 0.0f;
    float m_GustPhase2 = 0.37f;
    float m_DropletSpawnAccum = 0.0f;
    float m_CurrentThunderLevel = 0.0f;
    uint32_t m_RngState = 0x517CC1B7u;
    uint32_t m_CoeffUpdateCounter = 0;

    static constexpr int kMaxDroplets = 32;
    RainDropletVoice m_Droplets[kMaxDroplets];

    WeatherPcmClip m_ClipThunderClose;
    WeatherPcmClip m_ClipThunderDistant;
    WeatherPcmClip m_ClipGustWhoosh;
    WeatherPcmClip m_ClipHailBurst;
    WeatherPcmClip m_CustomRainLoop;
    WeatherPcmClip m_CustomWindLoop;
    size_t m_CustomRainCursor = 0;
    size_t m_CustomWindCursor = 0;

    static constexpr int kMaxVoices = 6;
    ActiveWeatherVoice m_Voices[kMaxVoices];

    std::vector<float> m_RainBuf;
    std::vector<float> m_WindBuf;
    std::vector<float> m_AmbBuf;
    std::vector<float> m_OneShotBuf;
};

} // namespace sphere::audio
