#include "WeatherAudioSystem.h"
#include <algorithm>
#include <cmath>

namespace sphere::audio {

namespace {
constexpr float kPi = 3.14159265358979323846f;
constexpr float kTwoPi = 6.2831853071795864769f;
} // namespace

void WeatherBiquad::SetLowPass(float cutoffHz, float q, float sampleRate) {
    float fc = std::clamp(cutoffHz, 20.0f, sampleRate * 0.44f);
    float w0 = kTwoPi * fc / sampleRate;
    float cosW0 = std::cos(w0);
    float alpha = std::sin(w0) / (2.0f * std::max(0.25f, q));
    float a0 = 1.0f + alpha;
    float invA0 = 1.0f / a0;

    b0 = ((1.0f - cosW0) * 0.5f) * invA0;
    b1 = (1.0f - cosW0) * invA0;
    b2 = b0;
    a1 = (-2.0f * cosW0) * invA0;
    a2 = (1.0f - alpha) * invA0;
}

void WeatherBiquad::SetBandPass(float centerHz, float q, float sampleRate) {
    float fc = std::clamp(centerHz, 25.0f, sampleRate * 0.44f);
    float w0 = kTwoPi * fc / sampleRate;
    float cosW0 = std::cos(w0);
    float alpha = std::sin(w0) / (2.0f * std::max(0.3f, q));
    float a0 = 1.0f + alpha;
    float invA0 = 1.0f / a0;

    b0 = alpha * invA0;
    b1 = 0.0f;
    b2 = -alpha * invA0;
    a1 = (-2.0f * cosW0) * invA0;
    a2 = (1.0f - alpha) * invA0;
}

float WeatherBiquad::Process(float x) {
    float y = b0 * x + z1;
    z1 = b1 * x - a1 * y + z2;
    z2 = b2 * x - a2 * y;
    return y;
}

WeatherAudioSystem& WeatherAudioSystem::Get() {
    static WeatherAudioSystem instance;
    return instance;
}

WeatherAudioSystem::WeatherAudioSystem() {
    BakeOneShotClips();
    m_RainBuf.resize(2048, 0.0f);
    m_WindBuf.resize(2048, 0.0f);
    m_AmbBuf.resize(2048, 0.0f);
    m_OneShotBuf.resize(2048, 0.0f);

    float sr = static_cast<float>(kSampleRate);
    m_RainBedLp.SetLowPass(1550.0f, 0.707f, sr);
    m_RainMidWarmth.SetLowPass(420.0f, 0.707f, sr);
    m_WindLowBp.SetBandPass(135.0f, 1.35f, sr);
    m_WindAirBp.SetBandPass(340.0f, 1.85f, sr);
    m_AmbienceLp.SetLowPass(320.0f, 0.707f, sr);
}

float WeatherAudioSystem::NextWhiteNoise() {
    uint32_t x = m_RngState;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    m_RngState = x;
    return static_cast<float>(static_cast<int32_t>(x)) * (1.0f / 2147483648.0f);
}

float WeatherAudioSystem::NextPinkNoise() {
    // Paul Kellet pink noise (-3 dB/octave natural roll-off eliminates white-noise static hiss)
    float white = NextWhiteNoise();
    m_PinkB0 = 0.99886f * m_PinkB0 + white * 0.0555179f;
    m_PinkB1 = 0.99332f * m_PinkB1 + white * 0.0750759f;
    m_PinkB2 = 0.96900f * m_PinkB2 + white * 0.1538520f;
    m_PinkB3 = 0.86650f * m_PinkB3 + white * 0.3104856f;
    m_PinkB4 = 0.55000f * m_PinkB4 + white * 0.5329522f;
    m_PinkB5 = -0.7616f * m_PinkB5 - white * 0.0168980f;
    float pink = m_PinkB0 + m_PinkB1 + m_PinkB2 + m_PinkB3 + m_PinkB4 + m_PinkB5 + m_PinkB6 + white * 0.5362f;
    m_PinkB6 = white * 0.115926f;
    return pink * 0.14f;
}

void WeatherAudioSystem::SpawnRainDroplet(float rainIntensity) {
    int slot = -1;
    for (int i = 0; i < kMaxDroplets; ++i) {
        if (!m_Droplets[i].active) {
            slot = i;
            break;
        }
    }
    if (slot < 0) return;

    float r1 = (NextWhiteNoise() * 0.5f + 0.5f);
    float r2 = (NextWhiteNoise() * 0.5f + 0.5f);
    float r3 = (NextWhiteNoise() * 0.5f + 0.5f);

    // Bubble-cavitation droplet plinks (650..2150 Hz) with subtle upward pitch chirp + fast organic decay
    float freqHz = 620.0f + r1 * r1 * 1550.0f;
    float sr = static_cast<float>(kSampleRate);
    float decayMs = 10.0f + r2 * 24.0f;

    RainDropletVoice& v = m_Droplets[slot];
    v.phase = 0.0f;
    v.phaseInc = freqHz / sr;
    v.freqDecay = 1.00018f; // Natural Minnaert bubble rising chirp
    v.env = 1.0f;
    v.envDecay = std::exp(-1.0f / (decayMs * 0.001f * sr));
    v.amp = (0.045f + 0.085f * r3) * (0.65f + 0.35f * rainIntensity);
    v.active = true;
}

void WeatherAudioSystem::BakeOneShotClips() {
    const float sr = static_cast<float>(kSampleRate);

    // 1. Close Lightning Crack + Cascading Sub-Bass Thunder Rumble (~2.6s, clean biquad LP, zero hiss)
    {
        size_t len = static_cast<size_t>(sr * 2.60f);
        m_ClipThunderClose.samples.resize(len);
        WeatherBiquad rumbleLp1, rumbleLp2, crackBp;
        rumbleLp1.SetLowPass(280.0f, 0.707f, sr);
        rumbleLp2.SetLowPass(165.0f, 0.707f, sr);
        crackBp.SetBandPass(520.0f, 0.9f, sr);

        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float pink = NextPinkNoise();
            float crackEnv = std::exp(-t * 18.0f);
            float echo1 = std::exp(-std::max(0.0f, t - 0.12f) * 1.9f) * (t > 0.08f ? 1.0f : 0.45f);
            float echo2 = (t > 0.52f) ? std::exp(-(t - 0.52f) * 1.35f) * 0.78f : 0.0f;
            float echo3 = (t > 1.15f) ? std::exp(-(t - 1.15f) * 1.65f) * 0.50f : 0.0f;
            float rumbleEnv = std::clamp(echo1 + echo2 * (0.72f + 0.28f * std::sin(t * 11.0f)) + echo3, 0.0f, 1.3f);

            float crack = crackBp.Process(pink) * crackEnv * 1.65f;
            float deepNoise = rumbleLp2.Process(rumbleLp1.Process(pink)) * 6.8f;
            float subBody = std::sin(kTwoPi * (54.0f + 16.0f * std::exp(-t * 4.0f)) * t) * 0.26f;
            float rumble = (deepNoise + subBody) * rumbleEnv;
            m_ClipThunderClose.samples[i] = std::clamp((crack + rumble) * 0.90f, -1.0f, 1.0f);
        }
    }

    // 2. Distant Rolling Thunder Boom (~3.1s, warm deep low-pass)
    {
        size_t len = static_cast<size_t>(sr * 3.10f);
        m_ClipThunderDistant.samples.resize(len);
        WeatherBiquad distLp1, distLp2;
        distLp1.SetLowPass(170.0f, 0.707f, sr);
        distLp2.SetLowPass(115.0f, 0.707f, sr);
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float pink = NextPinkNoise();
            float deep = distLp2.Process(distLp1.Process(pink)) * 8.5f;
            float attack = std::clamp(t / 0.22f, 0.0f, 1.0f);
            float decay = std::exp(-t * 1.05f);
            float rollMod = 0.68f + 0.32f * std::sin(t * 7.2f);
            m_ClipThunderDistant.samples[i] = std::clamp(deep * attack * decay * rollMod, -1.0f, 1.0f);
        }
    }

    // 3. Smooth Storm Gust Whoosh (~1.05s)
    {
        size_t len = static_cast<size_t>(sr * 1.05f);
        m_ClipGustWhoosh.samples.resize(len);
        WeatherBiquad gustBp;
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float env = std::sin(kPi * std::clamp(t / 1.05f, 0.0f, 1.0f));
            if (i % 64 == 0) {
                gustBp.SetBandPass(180.0f + 220.0f * env, 1.6f, sr);
            }
            float pink = NextPinkNoise();
            m_ClipGustWhoosh.samples[i] = std::clamp(gustBp.Process(pink) * env * 1.9f, -1.0f, 1.0f);
        }
    }

    // 4. Ice/Hail Tap Burst (~0.40s)
    {
        size_t len = static_cast<size_t>(sr * 0.40f);
        m_ClipHailBurst.samples.resize(len);
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float tap1 = std::sin(kTwoPi * 1280.0f * t) * std::exp(-t * 62.0f);
            float tap2 = (t > 0.12f) ? std::sin(kTwoPi * 1540.0f * (t - 0.12f)) * std::exp(-(t - 0.12f) * 68.0f) : 0.0f;
            m_ClipHailBurst.samples[i] = std::clamp((tap1 + tap2) * 0.55f, -1.0f, 1.0f);
        }
    }
}

void WeatherAudioSystem::UploadCustomSample(int32_t slotId, const int16_t* pcm16, int32_t sampleCount, int32_t sampleRate) {
    if (!pcm16 || sampleCount <= 0 || sampleRate <= 0) return;
    std::lock_guard<std::mutex> lock(m_Mutex);

    std::vector<float> normalized(static_cast<size_t>(sampleCount));
    constexpr float inv32768 = 1.0f / 32768.0f;
    for (int32_t i = 0; i < sampleCount; ++i) {
        normalized[i] = static_cast<float>(pcm16[i]) * inv32768;
    }

    std::vector<float> resampled;
    if (static_cast<uint32_t>(sampleRate) != kSampleRate && sampleCount > 2) {
        double ratio = static_cast<double>(kSampleRate) / static_cast<double>(sampleRate);
        size_t outLen = static_cast<size_t>(static_cast<double>(sampleCount) * ratio);
        resampled.resize(outLen);
        for (size_t i = 0; i < outLen; ++i) {
            double srcPos = static_cast<double>(i) / ratio;
            size_t i0 = static_cast<size_t>(srcPos);
            size_t i1 = std::min(i0 + 1, static_cast<size_t>(sampleCount - 1));
            float frac = static_cast<float>(srcPos - static_cast<double>(i0));
            resampled[i] = normalized[i0] * (1.0f - frac) + normalized[i1] * frac;
        }
    } else {
        resampled = std::move(normalized);
    }

    switch (slotId) {
        case 10:
            m_CustomRainLoop.samples = std::move(resampled);
            m_CustomRainCursor = 0;
            break;
        case 11:
            m_CustomWindLoop.samples = std::move(resampled);
            m_CustomWindCursor = 0;
            break;
        case 12:
            m_ClipThunderClose.samples = std::move(resampled);
            break;
        case 13:
            m_ClipThunderDistant.samples = std::move(resampled);
            break;
        default: break;
    }
}

void WeatherAudioSystem::SetWeatherParams(
    float rainIntensity,
    float snowIntensity,
    float windSpeedKmh,
    float windAzimuthDeg,
    float fogDensity,
    bool stormActive,
    bool enabled,
    bool muted
) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Params.rain_intensity = std::clamp(rainIntensity, 0.0f, 1.0f);
    m_Params.snow_intensity = std::clamp(snowIntensity, 0.0f, 1.0f);
    m_Params.wind_speed_kmh = std::clamp(windSpeedKmh, 0.0f, 95.0f);
    m_Params.wind_azimuth_deg = windAzimuthDeg;
    m_Params.fog_density = std::clamp(fogDensity, 0.0f, 1.0f);
    m_Params.storm_active = stormActive;
    m_Params.enabled = enabled;
    m_Params.muted = muted;
}

void WeatherAudioSystem::TriggerThunder(bool closeStrike, float gain) {
    TriggerOneShot(
        closeStrike ? WeatherOneShotId::THUNDER_CLAP_CLOSE : WeatherOneShotId::THUNDER_RUMBLE_DISTANT,
        gain
    );
}

void WeatherAudioSystem::TriggerOneShot(WeatherOneShotId id, float gain) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    for (int v = 0; v < kMaxVoices; ++v) {
        if (!m_Voices[v].active) {
            m_Voices[v] = {id, 0, std::clamp(gain, 0.05f, 1.5f), true};
            return;
        }
    }
    m_Voices[0] = {id, 0, std::clamp(gain, 0.05f, 1.5f), true};
}

void WeatherAudioSystem::RenderAudioBlock(int16_t* outPcm16, int32_t sampleCount) {
    if (!outPcm16 || sampleCount <= 0) return;

    std::lock_guard<std::mutex> lock(m_Mutex);
    const size_t count = static_cast<size_t>(sampleCount);
    if (m_RainBuf.size() < count) {
        m_RainBuf.resize(count);
        m_WindBuf.resize(count);
        m_AmbBuf.resize(count);
        m_OneShotBuf.resize(count);
    }

    const float sr = static_cast<float>(kSampleRate);
    const bool active = m_Params.enabled;

    const float rainGain = active ? (m_Params.rain_intensity * 0.65f) : 0.0f;
    const float windNorm = std::clamp(m_Params.wind_speed_kmh / 90.0f, 0.0f, 1.0f);
    const float windGain = active ? ((0.04f + windNorm * 0.58f) * (m_Params.storm_active ? 1.15f : 1.0f)) : 0.0f;
    const float ambGain = active ? std::max(m_Params.snow_intensity * 0.22f, m_Params.fog_density * 0.22f + 0.03f) : 0.0f;

    // Gentle rain cutoff (900..2100 Hz) avoids white-noise radio static
    m_RainBedLp.SetLowPass(880.0f + m_Params.rain_intensity * 1180.0f, 0.707f, sr);
    m_RainMidWarmth.SetLowPass(340.0f + m_Params.rain_intensity * 260.0f, 0.707f, sr);

    const float gustStep1 = 0.24f / sr;
    const float gustStep2 = 0.09f / sr;
    const float dropletsPerSample = (45.0f + m_Params.rain_intensity * 360.0f) / sr;
    const bool hasCustomRain = !m_CustomRainLoop.samples.empty();
    const bool hasCustomWind = !m_CustomWindLoop.samples.empty();
    float thunderPeak = 0.0f;

    for (size_t i = 0; i < count; ++i) {
        float pink = NextPinkNoise();
        m_BrownState = m_BrownState * 0.991f + pink * 0.075f;

        // 1. Rain: Custom WAV/MP3 loop OR physical warm rain wash + discrete modal droplet plinks
        float rainSample = 0.0f;
        if (rainGain > 0.005f) {
            if (hasCustomRain) {
                rainSample = m_CustomRainLoop.samples[m_CustomRainCursor++] * rainGain;
                if (m_CustomRainCursor >= m_CustomRainLoop.samples.size()) m_CustomRainCursor = 0;
            } else {
                m_DropletSpawnAccum += dropletsPerSample;
                if (m_DropletSpawnAccum >= 1.0f) {
                    m_DropletSpawnAccum -= 1.0f;
                    SpawnRainDroplet(m_Params.rain_intensity);
                }

                float dropletSum = 0.0f;
                for (int d = 0; d < kMaxDroplets; ++d) {
                    RainDropletVoice& v = m_Droplets[d];
                    if (!v.active) continue;
                    v.phase += v.phaseInc;
                    if (v.phase >= 1.0f) v.phase -= 1.0f;
                    v.phaseInc *= v.freqDecay;
                    dropletSum += std::sin(kTwoPi * v.phase) * v.env * v.amp;
                    v.env *= v.envDecay;
                    if (v.env < 0.004f) v.active = false;
                }

                float warmWash = m_RainBedLp.Process(pink * 0.85f) * 0.55f +
                                 m_RainMidWarmth.Process(m_BrownState * 1.45f) * 0.65f;
                rainSample = (warmWash * (0.45f + 0.55f * m_Params.rain_intensity) + dropletSum * 0.95f) * rainGain;
            }
        }
        m_RainBuf[i] = rainSample;

        // 2. Wind: Custom loop OR sweeping dual-formant resonant pink/brown air flow
        float windSample = 0.0f;
        if (windGain > 0.005f) {
            if (hasCustomWind) {
                windSample = m_CustomWindLoop.samples[m_CustomWindCursor++] * windGain;
                if (m_CustomWindCursor >= m_CustomWindLoop.samples.size()) m_CustomWindCursor = 0;
            } else {
                m_GustPhase += gustStep1; if (m_GustPhase >= 1.0f) m_GustPhase -= 1.0f;
                m_GustPhase2 += gustStep2; if (m_GustPhase2 >= 1.0f) m_GustPhase2 -= 1.0f;
                float gustLfo = 0.5f + 0.5f * std::sin(kTwoPi * m_GustPhase) * std::cos(kTwoPi * m_GustPhase2);

                if ((m_CoeffUpdateCounter++ & 63u) == 0u) {
                    float lowFc = 85.0f + windNorm * 145.0f + gustLfo * 70.0f;
                    float airFc = 220.0f + windNorm * 320.0f + gustLfo * 180.0f;
                    m_WindLowBp.SetBandPass(lowFc, 1.25f, sr);
                    m_WindAirBp.SetBandPass(airFc, 2.2f - windNorm * 0.6f, sr);
                }

                float windLow = m_WindLowBp.Process(m_BrownState * 1.9f + pink * 0.45f);
                float windAir = m_WindAirBp.Process(pink * 0.85f);
                windSample = (windLow * 1.45f + windAir * (0.35f + 0.45f * windNorm)) * windGain * (0.55f + 0.45f * gustLfo);
            }
        }
        m_WindBuf[i] = windSample;

        // 3. Snow & Fog Ambience (soft sub-airflow, zero high-pass white-noise static)
        float ambSample = 0.0f;
        if (ambGain > 0.005f) {
            float softAir = m_AmbienceLp.Process(m_BrownState * 1.2f + pink * 0.25f);
            ambSample = softAir * (0.85f + m_Params.fog_density * 0.8f + m_Params.snow_intensity * 0.4f) * ambGain;
        }
        m_AmbBuf[i] = ambSample;

        // 4. Thunder & Gust one-shots
        float osSample = 0.0f;
        for (int v = 0; v < kMaxVoices; ++v) {
            ActiveWeatherVoice& voice = m_Voices[v];
            if (!voice.active) continue;
            const WeatherPcmClip* clip = nullptr;
            switch (voice.id) {
                case WeatherOneShotId::THUNDER_CLAP_CLOSE: clip = &m_ClipThunderClose; break;
                case WeatherOneShotId::THUNDER_RUMBLE_DISTANT: clip = &m_ClipThunderDistant; break;
                case WeatherOneShotId::GUST_WHOOSH: clip = &m_ClipGustWhoosh; break;
                case WeatherOneShotId::HAIL_BURST: clip = &m_ClipHailBurst; break;
            }
            if (!clip || voice.cursor >= clip->samples.size()) {
                voice.active = false;
                continue;
            }
            float s = clip->samples[voice.cursor++] * (voice.gain * 1.25f);
            osSample += s;
            thunderPeak = std::max(thunderPeak, std::abs(s));
        }
        m_OneShotBuf[i] = osSample;
    }

    m_CurrentThunderLevel = std::clamp(thunderPeak, 0.0f, 1.0f);
    float masterGain = (m_Params.muted || !m_Params.enabled) ? 0.0f : m_Params.master_volume;
    WeatherAudioMixer::MixBlockToPcm16(
        m_RainBuf.data(),
        m_WindBuf.data(),
        m_AmbBuf.data(),
        m_OneShotBuf.data(),
        sampleCount,
        masterGain,
        outPcm16
    );
}

WeatherAudioTelemetry WeatherAudioSystem::GetTelemetry() const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    WeatherAudioTelemetry t;
    t.active = m_Params.enabled ? 1 : 0;
    t.rain_level = m_Params.enabled ? m_Params.rain_intensity : 0.0f;
    t.wind_level = m_Params.enabled ? std::clamp(m_Params.wind_speed_kmh / 90.0f, 0.0f, 1.0f) : 0.0f;
    t.thunder_level = m_CurrentThunderLevel;

    bool activeVoice = false;
    for (int v = 0; v < kMaxVoices; ++v) {
        if (m_Voices[v].active) {
            activeVoice = true;
            break;
        }
    }
    t.has_active_sound = (m_Params.enabled || activeVoice) ? 1 : 0;
    return t;
}

bool WeatherAudioSystem::HasAudibleSignal() const {
    return GetTelemetry().has_active_sound != 0;
}

} // namespace sphere::audio
