#include "WeatherAudioSystem.h"
#include <algorithm>
#include <cmath>

namespace sphere::audio {

namespace {
constexpr float kTwoPi = 6.2831853071795864769f;
} // namespace

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
}

float WeatherAudioSystem::NextNoise() {
    uint32_t x = m_RngState;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    m_RngState = x;
    return static_cast<float>(static_cast<int32_t>(x)) * (1.0f / 2147483648.0f);
}

void WeatherAudioSystem::BakeOneShotClips() {
    const float sr = static_cast<float>(kSampleRate);

    // 1. Close Lightning Crack + Rolling Thunder Rumble (~2.6s)
    {
        size_t len = static_cast<size_t>(sr * 2.60f);
        m_ClipThunderClose.samples.resize(len);
        float lp1 = 0.0f;
        float lp2 = 0.0f;
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float n = NextNoise();
            // Crisp sharp crack in the first 120ms, followed by deep sub/mid rumble echoes
            float crackEnv = std::exp(-t * 24.0f);
            float echo1 = std::exp(-std::max(0.0f, t - 0.14f) * 2.1f) * (t > 0.10f ? 1.0f : 0.4f);
            float echo2 = (t > 0.55f) ? std::exp(-(t - 0.55f) * 1.45f) * 0.72f : 0.0f;
            float rumbleEnv = std::clamp(echo1 + echo2 * (0.65f + 0.35f * std::sin(t * 14.0f)), 0.0f, 1.2f);

            lp1 = lp1 * 0.94f + n * 0.06f;
            lp2 = lp2 * 0.86f + n * 0.14f;
            float crack = n * crackEnv * 0.85f;
            float rumble = (lp1 * 4.2f + lp2 * 2.2f + std::sin(kTwoPi * 72.0f * t) * 0.18f) * rumbleEnv;
            m_ClipThunderClose.samples[i] = std::clamp((crack + rumble) * 0.95f, -1.0f, 1.0f);
        }
    }

    // 2. Distant Rolling Thunder Boom (~3.1s)
    {
        size_t len = static_cast<size_t>(sr * 3.10f);
        m_ClipThunderDistant.samples.resize(len);
        float lp = 0.0f;
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float n = NextNoise();
            lp = lp * 0.96f + n * 0.04f;
            float attack = std::clamp(t / 0.18f, 0.0f, 1.0f);
            float decay = std::exp(-t * 1.15f);
            float rollMod = 0.65f + 0.35f * std::sin(t * 8.5f);
            m_ClipThunderDistant.samples[i] = std::clamp(lp * 4.8f * attack * decay * rollMod, -1.0f, 1.0f);
        }
    }

    // 3. Sudden Storm Gust Whoosh (~1.05s)
    {
        size_t len = static_cast<size_t>(sr * 1.05f);
        m_ClipGustWhoosh.samples.resize(len);
        float bp1 = 0.0f;
        float bp2 = 0.0f;
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float env = std::sin(3.14159265f * std::clamp(t / 1.05f, 0.0f, 1.0f));
            float centerHz = 210.0f + 280.0f * env;
            float f = 2.0f * std::sin(3.14159265f * centerHz / sr);
            float n = NextNoise() * 0.6f;
            float hp = n - bp2 - 0.32f * bp1;
            bp1 += f * hp;
            bp2 += f * bp1;
            m_ClipGustWhoosh.samples[i] = std::clamp(bp1 * env * 1.25f, -1.0f, 1.0f);
        }
    }

    // 4. Ice/Hail Tap Burst (~0.40s)
    {
        size_t len = static_cast<size_t>(sr * 0.40f);
        m_ClipHailBurst.samples.resize(len);
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float tap1 = std::sin(kTwoPi * 1450.0f * t) * std::exp(-t * 62.0f);
            float tap2 = (t > 0.12f) ? std::sin(kTwoPi * 1720.0f * (t - 0.12f)) * std::exp(-(t - 0.12f) * 68.0f) : 0.0f;
            m_ClipHailBurst.samples[i] = std::clamp((tap1 + tap2) * 0.65f, -1.0f, 1.0f);
        }
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

    const float rainGain = active ? (m_Params.rain_intensity * 0.62f) : 0.0f;
    const float windNorm = std::clamp(m_Params.wind_speed_kmh / 90.0f, 0.0f, 1.0f);
    const float windGain = active ? ((0.06f + windNorm * 0.62f) * (m_Params.storm_active ? 1.18f : 1.0f)) : 0.0f;
    const float ambGain = active ? std::max(m_Params.snow_intensity * 0.24f, m_Params.fog_density * 0.22f + 0.05f) : 0.0f;

    const float gustStep1 = 0.28f / sr;
    const float gustStep2 = 0.11f / sr;
    float thunderPeak = 0.0f;

    for (size_t i = 0; i < count; ++i) {
        float n = NextNoise();

        // 1. Rain patter + mid sheet + random droplet splashes
        float rainSample = 0.0f;
        if (rainGain > 0.005f) {
            m_RainHp = 0.72f * (m_RainHp + n - m_RainPrevIn);
            m_RainPrevIn = n;
            m_RainLp = m_RainLp * 0.88f + n * 0.12f;

            // Stochastic droplet impulse proportional to rain intensity
            float splashChance = 0.0015f + m_Params.rain_intensity * 0.0085f;
            if (std::abs(n) > (1.0f - splashChance)) {
                m_RainSplashEnv = 0.9f;
            }
            m_RainSplashEnv *= 0.988f;

            rainSample = (m_RainHp * 0.45f + m_RainLp * 1.45f + m_RainSplashEnv * n * 0.65f) * rainGain;
        }
        m_RainBuf[i] = rainSample;

        // 2. Resonant sweeping wind gusts
        float windSample = 0.0f;
        if (windGain > 0.005f) {
            m_GustPhase += gustStep1; if (m_GustPhase >= 1.0f) m_GustPhase -= 1.0f;
            m_GustPhase2 += gustStep2; if (m_GustPhase2 >= 1.0f) m_GustPhase2 -= 1.0f;
            float gustLfo = 0.55f + 0.45f * std::sin(kTwoPi * m_GustPhase) * std::cos(kTwoPi * m_GustPhase2);
            float centerHz = 160.0f + windNorm * 420.0f + gustLfo * 240.0f;
            float f = std::clamp(2.0f * std::sin(3.14159265f * centerHz / sr), 0.02f, 0.35f);
            float q = 0.22f - windNorm * 0.10f; // Sharper whistle at gale speeds
            float hp = n - m_WindBpState2 - q * m_WindBpState1;
            m_WindBpState1 += f * hp;
            m_WindBpState2 += f * m_WindBpState1;
            m_WindLow = m_WindLow * 0.95f + n * 0.05f;
            windSample = (m_WindBpState1 * 0.75f + m_WindLow * 1.85f) * windGain * (0.55f + 0.45f * gustLfo);
        }
        m_WindBuf[i] = windSample;

        // 3. Snow crystalline hush + mist atmospheric bed
        float ambSample = 0.0f;
        if (ambGain > 0.005f) {
            m_AmbienceFilter = m_AmbienceFilter * 0.92f + n * 0.08f;
            float snowAir = (n - m_AmbienceFilter) * m_Params.snow_intensity * 0.32f;
            float mistBed = m_AmbienceFilter * (1.1f + m_Params.fog_density * 1.4f);
            ambSample = (snowAir + mistBed) * ambGain;
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
