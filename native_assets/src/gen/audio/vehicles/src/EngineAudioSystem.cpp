#include "EngineAudioSystem.h"
#include <algorithm>
#include <cmath>

namespace sphere::audio {

namespace {
constexpr float kPi = 3.14159265358979323846f;
constexpr float kTwoPi = 6.2831853071795864769f;
constexpr float kGearRatios[13] = {
    0.0f,  // N
    9.8f, 7.6f, 5.9f, 4.55f, 3.55f, 2.78f,
    2.18f, 1.72f, 1.36f, 1.08f, 0.88f, 0.74f
};

inline float SoftClipSample(float x) {
    return x / (1.0f + std::abs(x));
}
} // namespace

void BiquadState::SetLowPass(float cutoffHz, float q, float sampleRate) {
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

void BiquadState::SetBandPass(float centerHz, float q, float sampleRate) {
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

float BiquadState::Process(float x) {
    float y = b0 * x + z1;
    z1 = b1 * x - a1 * y + z2;
    z2 = b2 * x - a2 * y;
    return y;
}

EngineAudioSystem& EngineAudioSystem::Get() {
    static EngineAudioSystem instance;
    return instance;
}

EngineAudioSystem::EngineAudioSystem() {
    BakeOneShotClips();
    m_EngineBuf.resize(2048, 0.0f);
    m_TireBuf.resize(2048, 0.0f);
    m_OneShotBuf.resize(2048, 0.0f);

    float sr = static_cast<float>(kSampleRate);
    m_ExhaustLowFormant.SetBandPass(108.0f, 1.9f, sr);
    m_ExhaustMidFormant.SetBandPass(265.0f, 1.5f, sr);
    m_ExhaustCutoffLp.SetLowPass(780.0f, 0.75f, sr);
    m_TireLp.SetLowPass(320.0f, 0.707f, sr);
}

float EngineAudioSystem::NextWhiteNoise() {
    uint32_t x = m_RngState;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    m_RngState = x;
    return static_cast<float>(static_cast<int32_t>(x)) * (1.0f / 2147483648.0f);
}

float EngineAudioSystem::NextPinkNoise() {
    // Paul Kellet 7-pole pink noise filter (-3 dB/oct natural roll-off, no harsh static hiss)
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

void EngineAudioSystem::BakeOneShotClips() {
    const float sr = static_cast<float>(kSampleRate);

    // 1. Air brake pneumatic hiss (~0.52s swept bandpass pink noise + valve pop, no raw white static)
    {
        size_t len = static_cast<size_t>(sr * 0.52f);
        m_ClipAirBrake.samples.resize(len);
        BiquadState bp1, bp2;
        float b0 = 0, b1 = 0, b2 = 0;
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float env = (t < 0.014f) ? (t / 0.014f) : std::exp(-(t - 0.014f) * 8.2f);
            float w = NextWhiteNoise();
            b0 = 0.997f * b0 + w * 0.029f;
            b1 = 0.985f * b1 + w * 0.032f;
            b2 = 0.950f * b2 + w * 0.048f;
            float pink = (b0 + b1 + b2 + w * 0.18f) * 0.45f;

            if (i % 64 == 0) {
                float sweepHz = 2600.0f - std::min(1.0f, t / 0.45f) * 1450.0f;
                bp1.SetBandPass(sweepHz, 1.3f, sr);
                bp2.SetLowPass(3400.0f, 0.707f, sr);
            }
            float air = bp2.Process(bp1.Process(pink));
            float valveThump = std::sin(kTwoPi * 105.0f * t) * std::exp(-t * 44.0f) * 0.34f;
            m_ClipAirBrake.samples[i] = std::clamp((air * 1.15f + valveThump) * env, -1.0f, 1.0f);
        }
    }

    // 2. Heavy dual-tone truck air horn (D#3 + F#3 warm chorus brass ~0.72s)
    {
        size_t len = static_cast<size_t>(sr * 0.72f);
        m_ClipTruckHorn.samples.resize(len);
        BiquadState hornLp;
        hornLp.SetLowPass(1950.0f, 0.8f, sr);
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float attack = std::clamp(t / 0.028f, 0.0f, 1.0f);
            float release = std::clamp((0.72f - t) / 0.16f, 0.0f, 1.0f);
            float env = attack * release;
            float pitchDrop = 1.0f - (1.0f - release) * 0.028f;
            float f1 = 155.56f * pitchDrop;
            float f2 = 185.00f * pitchDrop;
            float sig = 0.0f;
            for (int h = 1; h <= 5; ++h) {
                float roll = 1.0f / static_cast<float>(h * h * 0.45f + 0.55f);
                sig += (std::sin(kTwoPi * f1 * h * t) + std::sin(kTwoPi * f2 * h * t)) * roll;
            }
            m_ClipTruckHorn.samples[i] = std::clamp(hornLp.Process(sig * 0.25f) * env, -1.0f, 1.0f);
        }
    }

    // 3. Matatu / urban dual-tone horn (~0.45s crisp brass pair)
    {
        size_t len = static_cast<size_t>(sr * 0.45f);
        m_ClipMatatuHorn.samples.resize(len);
        BiquadState hornLp;
        hornLp.SetLowPass(2400.0f, 0.75f, sr);
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float attack = std::clamp(t / 0.015f, 0.0f, 1.0f);
            float release = std::clamp((0.45f - t) / 0.09f, 0.0f, 1.0f);
            float env = attack * release;
            float f1 = 392.0f; // G4
            float f2 = 493.88f; // B4
            float sig = std::sin(kTwoPi * f1 * t) * 0.50f +
                        std::sin(kTwoPi * f2 * t) * 0.50f +
                        std::sin(kTwoPi * f1 * 2.0f * t) * 0.20f +
                        std::sin(kTwoPi * f2 * 2.0f * t) * 0.20f +
                        std::sin(kTwoPi * f1 * 3.0f * t) * 0.08f;
            m_ClipMatatuHorn.samples[i] = std::clamp(hornLp.Process(sig * 0.42f) * env, -1.0f, 1.0f);
        }
    }

    // 4. Pneumatic bus/cab door release + mechanical latch clunk (~0.62s)
    {
        size_t len = static_cast<size_t>(sr * 0.62f);
        m_ClipBusDoor.samples.resize(len);
        BiquadState doorBp;
        doorBp.SetBandPass(1350.0f, 1.1f, sr);
        float b0 = 0, b1 = 0;
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float hissEnv = std::clamp(t / 0.03f, 0.0f, 1.0f) * std::exp(-t * 5.6f);
            float w = NextWhiteNoise();
            b0 = 0.992f * b0 + w * 0.045f;
            b1 = 0.960f * b1 + w * 0.060f;
            float air = doorBp.Process((b0 + b1) * 0.7f);
            float clunk = std::sin(kTwoPi * 92.0f * (t - 0.01f)) * std::exp(-std::max(0.0f, t - 0.01f) * 38.0f) * 0.58f;
            float tailLatch = (t > 0.34f)
                ? std::sin(kTwoPi * 124.0f * (t - 0.34f)) * std::exp(-(t - 0.34f) * 44.0f) * 0.52f
                : 0.0f;
            m_ClipBusDoor.samples[i] = std::clamp(air * hissEnv * 1.4f + clunk + tailLatch, -1.0f, 1.0f);
        }
    }

    // 5. Mechanical heavy-duty synchronized gear shift clunk (~0.11s)
    {
        size_t len = static_cast<size_t>(sr * 0.11f);
        m_ClipGearClunk.samples.resize(len);
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float thud = std::sin(kTwoPi * (135.0f - t * 380.0f) * t) * std::exp(-t * 42.0f);
            float metallic = std::sin(kTwoPi * 540.0f * t) * std::exp(-t * 68.0f) * 0.22f;
            m_ClipGearClunk.samples[i] = std::clamp((thud + metallic) * 0.45f, -1.0f, 1.0f);
        }
    }
}

void EngineAudioSystem::UploadCustomSample(int32_t slotId, const int16_t* pcm16, int32_t sampleCount, int32_t sampleRate) {
    if (!pcm16 || sampleCount <= 0 || sampleRate <= 0) return;
    std::lock_guard<std::mutex> lock(m_Mutex);

    std::vector<float> normalized(static_cast<size_t>(sampleCount));
    constexpr float inv32768 = 1.0f / 32768.0f;
    for (int32_t i = 0; i < sampleCount; ++i) {
        normalized[i] = static_cast<float>(pcm16[i]) * inv32768;
    }

    // Resample linearly to 44.1 kHz if needed
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
        case 0: m_ClipAirBrake.samples = std::move(resampled); break;
        case 1: m_ClipTruckHorn.samples = std::move(resampled); break;
        case 2: m_ClipBusDoor.samples = std::move(resampled); break;
        case 3: m_ClipMatatuHorn.samples = std::move(resampled); break;
        case 4: m_ClipGearClunk.samples = std::move(resampled); break;
        case 5:
            m_CustomEngineLoop.samples = std::move(resampled);
            m_CustomEngineCursor = 0.0f;
            break;
        default: break;
    }
}

void EngineAudioSystem::ConfigureVehicle(
    int32_t vehicleCategoryOrdinal,
    float enginePowerHp,
    float engineTorqueNm,
    float kerbWeightKg,
    float combinedLoadKg
) {
    (void)engineTorqueNm;
    std::lock_guard<std::mutex> lock(m_Mutex);
    switch (vehicleCategoryOrdinal) {
        case 1:
            m_Params.vehicle_class = AudioVehicleClass::HEAVY_TRACTOR_V8;
            m_Params.cylinders = 8;
            m_Params.idle_rpm = 600.0f;
            m_Params.max_rpm = 2250.0f;
            m_Params.exhaust_pitch = 0.88f;
            break;
        case 0:
            m_Params.vehicle_class = AudioVehicleClass::RIGID_LORRY_I6;
            m_Params.cylinders = 6;
            m_Params.idle_rpm = 650.0f;
            m_Params.max_rpm = 2450.0f;
            m_Params.exhaust_pitch = 1.00f;
            break;
        case 2:
            m_Params.vehicle_class = AudioVehicleClass::CITY_BUS_TRANSIT;
            m_Params.cylinders = 6;
            m_Params.idle_rpm = 620.0f;
            m_Params.max_rpm = 2200.0f;
            m_Params.exhaust_pitch = 0.94f;
            break;
        case 3:
            m_Params.vehicle_class = AudioVehicleClass::COACH_HIGHWAY;
            m_Params.cylinders = 6;
            m_Params.idle_rpm = 580.0f;
            m_Params.max_rpm = 2350.0f;
            m_Params.exhaust_pitch = 0.90f;
            break;
        default:
            m_Params.vehicle_class = AudioVehicleClass::MINIBUS_DIESEL;
            m_Params.cylinders = 4;
            m_Params.idle_rpm = 780.0f;
            m_Params.max_rpm = 3800.0f;
            m_Params.exhaust_pitch = 1.24f;
            break;
    }
    m_Params.engine_power_hp = std::max(110.0f, enginePowerHp);
    m_Params.trailer_load_kg = std::max(0.0f, combinedLoadKg - kerbWeightKg);
    if (m_Params.ignition_state == IgnitionState::RUNNING) {
        m_Params.current_rpm = std::clamp(m_Params.current_rpm, m_Params.idle_rpm, m_Params.max_rpm);
    }
}

void EngineAudioSystem::SetIgnition(bool turnOn) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    if (turnOn) {
        if (m_Params.ignition_state == IgnitionState::OFF) {
            m_Params.ignition_state = IgnitionState::STARTING;
            m_StarterElapsedSec = 0.0f;
            m_Params.current_rpm = 220.0f;
            m_Params.gear = 0;
        }
    } else {
        m_Params.ignition_state = IgnitionState::OFF;
        m_Params.throttle = 0.0f;
        m_Params.cruise_drive = false;
        m_Params.gear = 0;
        m_Params.turbo_boost_bar = 0.0f;
    }
}

void EngineAudioSystem::SetControls(float throttle, float brake, bool cruiseDrive, bool muted) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    float nextThrottle = std::clamp(throttle, 0.0f, 1.0f);
    float nextBrake = std::clamp(brake, 0.0f, 1.0f);

    if (m_BrakeHissCooldownSec <= 0.0f) {
        if ((nextBrake > 0.55f && m_PrevBrakeInput <= 0.55f && m_Params.speed_kmh > 3.5f) ||
            (nextBrake <= 0.05f && m_PrevBrakeInput > 0.55f && m_Params.speed_kmh < 22.0f)) {
            for (int v = 0; v < kMaxVoices; ++v) {
                if (!m_Voices[v].active) {
                    m_Voices[v] = {OneShotSoundId::AIR_BRAKE_HISS, 0, 0.72f, 1.0f, true};
                    break;
                }
            }
            m_BrakeHissCooldownSec = 0.48f;
        }
    }

    m_PrevBrakeInput = nextBrake;
    m_Params.throttle = nextThrottle;
    m_Params.brake = nextBrake;
    m_Params.air_brake_active = (nextBrake > 0.1f);
    m_Params.cruise_drive = cruiseDrive;
    m_Params.muted = muted;
}

void EngineAudioSystem::TriggerOneShot(OneShotSoundId id, float gain) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    OneShotSoundId resolved = id;
    if (id == OneShotSoundId::TRUCK_HORN_HEAVY &&
        m_Params.vehicle_class == AudioVehicleClass::MINIBUS_DIESEL) {
        resolved = OneShotSoundId::MATATU_HORN;
    }
    for (int v = 0; v < kMaxVoices; ++v) {
        if (!m_Voices[v].active) {
            m_Voices[v] = {resolved, 0, std::clamp(gain, 0.05f, 1.5f), 1.0f, true};
            return;
        }
    }
    m_Voices[0] = {resolved, 0, std::clamp(gain, 0.05f, 1.5f), 1.0f, true};
}

void EngineAudioSystem::StepPhysics(float dtSeconds) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    StepPhysicsLocked(dtSeconds);
}

void EngineAudioSystem::StepPhysicsLocked(float dtSeconds) {
    float dt = std::clamp(dtSeconds, 0.0005f, 0.08f);
    if (m_BrakeHissCooldownSec > 0.0f) {
        m_BrakeHissCooldownSec = std::max(0.0f, m_BrakeHissCooldownSec - dt);
    }
    if (m_ShiftPulseSec > 0.0f) {
        m_ShiftPulseSec = std::max(0.0f, m_ShiftPulseSec - dt);
    }

    if (m_Params.ignition_state == IgnitionState::OFF) {
        m_Params.current_rpm = std::max(0.0f, m_Params.current_rpm - dt * 1100.0f);
        float dragDecel = 3.5f + m_Params.brake * 32.0f;
        m_Params.speed_kmh = std::max(0.0f, m_Params.speed_kmh - dragDecel * dt);
        m_Params.distance_m += (m_Params.speed_kmh / 3.6f) * dt;
        m_Params.turbo_boost_bar = std::max(0.0f, m_Params.turbo_boost_bar - dt * 3.0f);
        return;
    }

    if (m_Params.ignition_state == IgnitionState::STARTING) {
        m_StarterElapsedSec += dt;
        if (m_StarterElapsedSec < 0.62f) {
            m_Params.current_rpm = 240.0f + 55.0f * std::sin(m_StarterElapsedSec * 28.0f);
        } else if (m_StarterElapsedSec < 0.92f) {
            float t = (m_StarterElapsedSec - 0.62f) / 0.30f;
            m_Params.current_rpm = 300.0f + std::sin(t * 2.4f) * 850.0f;
        } else {
            m_Params.ignition_state = IgnitionState::RUNNING;
            m_Params.current_rpm = m_Params.idle_rpm * 1.15f;
        }
        return;
    }

    float effThrottle = m_Params.throttle;
    if (m_Params.cruise_drive && m_Params.brake < 0.1f) {
        effThrottle = std::max(effThrottle, (m_Params.speed_kmh < 68.0f) ? 0.58f : 0.24f);
    }

    float weightFactor = std::clamp(420.0f / std::max(220.0f, m_Params.engine_power_hp), 0.55f, 1.45f);
    float accel = (effThrottle * 16.5f / weightFactor) - m_Params.brake * 36.0f - 1.15f - m_Params.speed_kmh * 0.075f;
    m_Params.speed_kmh = std::clamp(m_Params.speed_kmh + accel * dt, 0.0f, 112.0f);
    m_Params.distance_m += (m_Params.speed_kmh / 3.6f) * dt;

    int maxGears = (m_Params.vehicle_class == AudioVehicleClass::HEAVY_TRACTOR_V8 ||
                    m_Params.vehicle_class == AudioVehicleClass::RIGID_LORRY_I6) ? 12 : 6;

    if (m_Params.speed_kmh < 0.8f && effThrottle < 0.08f) {
        m_Params.gear = 0;
    } else {
        int prevGear = m_Params.gear;
        if (m_Params.gear == 0) {
            m_Params.gear = 1;
        }
        float speedMs = m_Params.speed_kmh / 3.6f;
        float ratio = kGearRatios[std::clamp(m_Params.gear, 1, maxGears)];
        float coupledRpm = speedMs * ratio * 158.0f + m_Params.idle_rpm * 0.55f;

        if (coupledRpm > m_Params.max_rpm * 0.84f && m_Params.gear < maxGears) {
            m_Params.gear++;
            m_ShiftPulseSec = 0.14f;
        } else if (coupledRpm < m_Params.idle_rpm * 1.48f && m_Params.gear > 1) {
            m_Params.gear--;
            m_ShiftPulseSec = 0.12f;
        }

        if (m_Params.gear != prevGear && prevGear != 0) {
            for (int v = 0; v < kMaxVoices; ++v) {
                if (!m_Voices[v].active) {
                    m_Voices[v] = {OneShotSoundId::GEAR_SHIFT_CLUNK, 0, 0.52f, 1.0f, true};
                    break;
                }
            }
        }
    }

    float targetRpm = m_Params.idle_rpm;
    if (m_Params.gear == 0) {
        targetRpm = m_Params.idle_rpm + effThrottle * (m_Params.max_rpm - m_Params.idle_rpm) * 0.88f;
    } else {
        float speedMs = m_Params.speed_kmh / 3.6f;
        float ratio = kGearRatios[std::clamp(m_Params.gear, 1, maxGears)];
        float wheelCoupledRpm = speedMs * ratio * 158.0f + m_Params.idle_rpm * 0.65f;
        float torqueSlipBoost = effThrottle * 240.0f;
        targetRpm = std::clamp(wheelCoupledRpm + torqueSlipBoost, m_Params.idle_rpm, m_Params.max_rpm);
    }
    if (m_ShiftPulseSec > 0.02f) {
        targetRpm *= 0.86f;
    }

    float rpmRate = (targetRpm > m_Params.current_rpm) ? 5.8f : 3.6f;
    m_Params.current_rpm += (targetRpm - m_Params.current_rpm) * std::min(1.0f, rpmRate * dt);

    float normLoad = std::clamp((m_Params.current_rpm - m_Params.idle_rpm) / std::max(400.0f, m_Params.max_rpm - m_Params.idle_rpm), 0.0f, 1.0f);
    float targetBoost = (m_Params.vehicle_class == AudioVehicleClass::MINIBUS_DIESEL ? 1.4f : 2.2f) *
                        normLoad * (0.25f + 0.75f * effThrottle);
    m_Params.turbo_boost_bar += (targetBoost - m_Params.turbo_boost_bar) * std::min(1.0f, 4.5f * dt);
}

void EngineAudioSystem::RenderAudioBlock(int16_t* outPcm16, int32_t sampleCount) {
    if (!outPcm16 || sampleCount <= 0) return;

    std::lock_guard<std::mutex> lock(m_Mutex);
    const size_t count = static_cast<size_t>(sampleCount);
    if (m_EngineBuf.size() < count) {
        m_EngineBuf.resize(count);
        m_TireBuf.resize(count);
        m_OneShotBuf.resize(count);
    }

    const float sr = static_cast<float>(kSampleRate);
    StepPhysicsLocked(static_cast<float>(sampleCount) / sr);

    const bool engineAudible = (m_Params.ignition_state != IgnitionState::OFF) || (m_Params.current_rpm > 40.0f);
    const float rpmFrac = std::clamp(
        (m_Params.current_rpm - m_Params.idle_rpm) / std::max(300.0f, m_Params.max_rpm - m_Params.idle_rpm),
        0.0f, 1.0f
    );
    const float firingHz = std::max(
        12.0f,
        (m_Params.current_rpm / 60.0f) * (static_cast<float>(m_Params.cylinders) * 0.5f) * m_Params.exhaust_pitch
    );

    // Update biquad exhaust & tire cutoff per block smoothly (zero high-frequency static)
    float exhaustCutoffHz = 360.0f + rpmFrac * 740.0f + m_Params.throttle * 340.0f;
    m_ExhaustCutoffLp.SetLowPass(exhaustCutoffHz, 0.75f, sr);
    m_ExhaustLowFormant.SetBandPass(88.0f + rpmFrac * 75.0f, 1.85f, sr);
    m_ExhaustMidFormant.SetBandPass(225.0f + rpmFrac * 240.0f, 1.45f, sr);

    float tireCutoffHz = 140.0f + std::clamp(m_Params.speed_kmh / 100.0f, 0.0f, 1.0f) * 380.0f;
    m_TireLp.SetLowPass(tireCutoffHz, 0.707f, sr);

    const float dtFund = firingHz / sr;
    const float dtSub = (firingHz * 0.5f) / sr;
    const float dtHarm2 = (firingHz * 2.0f) / sr;
    const float dtHarm3 = (firingHz * 3.0f) / sr;
    const float turboHz = 1350.0f + m_Params.turbo_boost_bar * 1250.0f;
    const float dtTurbo = turboHz / sr;
    const float dtStarter = 220.0f / sr;
    const float treadHz = 52.0f + m_Params.speed_kmh * 1.95f;
    const float dtTread = treadHz / sr;

    const float engGain = engineAudible
        ? (0.58f + 0.28f * rpmFrac + 0.22f * m_Params.throttle)
        : 0.0f;
    const float tireGain = std::clamp(m_Params.speed_kmh / 95.0f, 0.0f, 1.0f) * 0.34f;
    const bool hasCustomEngine = !m_CustomEngineLoop.samples.empty();

    for (size_t i = 0; i < count; ++i) {
        float pink = NextPinkNoise();
        m_BrownState = m_BrownState * 0.990f + pink * 0.085f;

        // 1. Physical diesel combustion pulse synthesis (or real uploaded engine loop pitch-shifted by RPM)
        float engSample = 0.0f;
        if (engineAudible) {
            if (hasCustomEngine) {
                float pitchRatio = std::clamp(m_Params.current_rpm / std::max(300.0f, m_Params.idle_rpm), 0.45f, 3.2f);
                size_t len = m_CustomEngineLoop.samples.size();
                size_t idx0 = static_cast<size_t>(m_CustomEngineCursor) % len;
                size_t idx1 = (idx0 + 1) % len;
                float frac = m_CustomEngineCursor - std::floor(m_CustomEngineCursor);
                float rawClip = m_CustomEngineLoop.samples[idx0] * (1.0f - frac) + m_CustomEngineLoop.samples[idx1] * frac;
                m_CustomEngineCursor += pitchRatio;
                if (m_CustomEngineCursor >= static_cast<float>(len)) {
                    m_CustomEngineCursor -= static_cast<float>(len);
                }
                engSample = rawClip * engGain;
            } else {
                m_PhaseFund += dtFund; if (m_PhaseFund >= 1.0f) m_PhaseFund -= 1.0f;
                m_PhaseSub += dtSub; if (m_PhaseSub >= 1.0f) m_PhaseSub -= 1.0f;
                m_PhaseHarm2 += dtHarm2; if (m_PhaseHarm2 >= 1.0f) m_PhaseHarm2 -= 1.0f;
                m_PhaseHarm3 += dtHarm3; if (m_PhaseHarm3 >= 1.0f) m_PhaseHarm3 -= 1.0f;
                m_PhaseTurbo += dtTurbo; if (m_PhaseTurbo >= 1.0f) m_PhaseTurbo -= 1.0f;

                // Asymmetric band-limited diesel cylinder compression pulse (no aliased sawtooth, no white static)
                float pulseShape = 0.5f + 0.5f * std::cos(kTwoPi * m_PhaseFund);
                float compressionStroke = std::pow(pulseShape, 2.2f) * 2.0f - 0.65f;
                float subThump = std::sin(kTwoPi * m_PhaseSub);
                float h2Wave = std::sin(kTwoPi * m_PhaseHarm2);
                float h3Wave = std::sin(kTwoPi * m_PhaseHarm3);
                float h4Wave = std::sin(kTwoPi * (m_PhaseHarm2 * 2.0f));

                // Smooth organic exhaust rumble from pink/brown airflow modulated by cylinder stroke
                float airflowRumble = (m_BrownState * 1.45f + pink * 0.25f) * (0.45f + 0.55f * pulseShape);
                float combCore = subThump * 0.34f +
                                 compressionStroke * 0.42f +
                                 h2Wave * 0.28f +
                                 h3Wave * (0.16f + 0.14f * m_Params.throttle) +
                                 h4Wave * (0.08f + 0.08f * rpmFrac) +
                                 airflowRumble * (0.35f + 0.25f * m_Params.throttle);

                // Muffler acoustic chamber formants + biquad low-pass eliminates fizz/static
                float mufflerBody = m_ExhaustCutoffLp.Process(combCore);
                float formantLow = m_ExhaustLowFormant.Process(combCore) * 0.55f;
                float formantMid = m_ExhaustMidFormant.Process(combCore) * (0.32f + 0.25f * m_Params.throttle);
                float shapedExhaust = SoftClipSample((mufflerBody + formantLow + formantMid) * 0.92f);

                float turboSample = 0.0f;
                if (m_Params.turbo_boost_bar > 0.14f) {
                    turboSample = std::sin(kTwoPi * m_PhaseTurbo) * std::clamp((m_Params.turbo_boost_bar - 0.14f) * 0.075f, 0.0f, 0.12f);
                }

                float starterSample = 0.0f;
                if (m_Params.ignition_state == IgnitionState::STARTING && m_StarterElapsedSec < 0.68f) {
                    m_PhaseStarter += dtStarter; if (m_PhaseStarter >= 1.0f) m_PhaseStarter -= 1.0f;
                    float crankCycle = 0.5f + 0.5f * std::sin(kTwoPi * (m_StarterElapsedSec * 5.6f));
                    float solenoidWhine = std::sin(kTwoPi * m_PhaseStarter) * 0.38f +
                                          std::sin(kTwoPi * m_PhaseStarter * 2.0f) * 0.18f;
                    starterSample = solenoidWhine * (0.32f + 0.30f * crankCycle);
                }

                engSample = shapedExhaust * engGain + turboSample + starterSample;
            }
        }
        m_EngineBuf[i] = engSample;

        // 2. Smooth phase-continuous road & tire rolling rumble (fixes 43Hz block-reset click & removes static)
        float tireSample = 0.0f;
        if (tireGain > 0.004f) {
            m_PhaseTread += dtTread;
            if (m_PhaseTread >= 1.0f) m_PhaseTread -= 1.0f;
            float roadRumble = m_TireLp.Process(m_BrownState * 1.35f + pink * 0.35f);
            float treadHum = std::sin(kTwoPi * m_PhaseTread) * 0.14f;
            tireSample = (roadRumble * 1.45f + treadHum) * tireGain;
        }
        m_TireBuf[i] = tireSample;

        // 3. RAM-cached 16-bit PCM one-shot bursts (air brake, horn, door pneumatic, gear shift)
        float osSample = 0.0f;
        for (int v = 0; v < kMaxVoices; ++v) {
            ActiveOneShotVoice& voice = m_Voices[v];
            if (!voice.active) continue;
            const PcmOneShotClip* clip = nullptr;
            switch (voice.id) {
                case OneShotSoundId::AIR_BRAKE_HISS: clip = &m_ClipAirBrake; break;
                case OneShotSoundId::TRUCK_HORN_HEAVY: clip = &m_ClipTruckHorn; break;
                case OneShotSoundId::BUS_DOOR_PNEUMATIC: clip = &m_ClipBusDoor; break;
                case OneShotSoundId::MATATU_HORN: clip = &m_ClipMatatuHorn; break;
                case OneShotSoundId::GEAR_SHIFT_CLUNK: clip = &m_ClipGearClunk; break;
            }
            if (!clip || voice.cursor >= clip->samples.size()) {
                voice.active = false;
                continue;
            }
            osSample += clip->samples[voice.cursor++] * (voice.gain * 1.15f);
        }
        m_OneShotBuf[i] = osSample;
    }

    float masterGain = m_Params.muted ? 0.0f : 1.05f;
    VehicleAudioMixer::MixBlockToPcm16(
        m_EngineBuf.data(),
        m_TireBuf.data(),
        m_OneShotBuf.data(),
        sampleCount,
        masterGain,
        outPcm16
    );
}

VehicleAudioTelemetry EngineAudioSystem::GetTelemetry() const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    VehicleAudioTelemetry t;
    t.ignition_state = static_cast<int32_t>(m_Params.ignition_state);
    t.rpm = m_Params.current_rpm;
    t.speed_kmh = m_Params.speed_kmh;
    t.gear = m_Params.gear;
    t.turbo_boost_bar = m_Params.turbo_boost_bar;
    t.distance_m = m_Params.distance_m;
    bool activeVoice = false;
    for (int v = 0; v < kMaxVoices; ++v) {
        if (m_Voices[v].active) {
            activeVoice = true;
            break;
        }
    }
    t.has_active_sound = (m_Params.ignition_state != IgnitionState::OFF || m_Params.current_rpm > 40.0f || activeVoice) ? 1 : 0;
    return t;
}

bool EngineAudioSystem::HasAudibleSignal() const {
    return GetTelemetry().has_active_sound != 0;
}

} // namespace sphere::audio
