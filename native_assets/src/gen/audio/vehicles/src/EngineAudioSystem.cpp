#include "EngineAudioSystem.h"
#include <algorithm>
#include <cmath>

namespace sphere::audio {

namespace {
constexpr float kTwoPi = 6.2831853071795864769f;
constexpr float kGearRatios[13] = {
    0.0f,  // N
    9.8f, 7.6f, 5.9f, 4.55f, 3.55f, 2.78f,
    2.18f, 1.72f, 1.36f, 1.08f, 0.88f, 0.74f
};
} // namespace

EngineAudioSystem& EngineAudioSystem::Get() {
    static EngineAudioSystem instance;
    return instance;
}

EngineAudioSystem::EngineAudioSystem() {
    BakeOneShotClips();
    m_EngineBuf.resize(2048, 0.0f);
    m_TireBuf.resize(2048, 0.0f);
    m_OneShotBuf.resize(2048, 0.0f);
}

float EngineAudioSystem::NextNoise() {
    // Fast xorshift32 PRNG normalized to [-1, 1]
    uint32_t x = m_RngState;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    m_RngState = x;
    return static_cast<float>(static_cast<int32_t>(x)) * (1.0f / 2147483648.0f);
}

void EngineAudioSystem::BakeOneShotClips() {
    const float sr = static_cast<float>(kSampleRate);

    // 1. Air brake pneumatic hiss (~0.52s unfiltered burst + low valve thump -> bandpass sweep)
    {
        size_t len = static_cast<size_t>(sr * 0.52f);
        m_ClipAirBrake.samples.resize(len);
        float lp = 0.0f;
        float hp = 0.0f;
        float prevIn = 0.0f;
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float env = (t < 0.015f) ? (t / 0.015f) : std::exp(-(t - 0.015f) * 8.5f);
            float n = NextNoise();
            hp = 0.82f * (hp + n - prevIn);
            prevIn = n;
            lp = lp + 0.42f * (hp - lp);
            float valveThump = std::sin(kTwoPi * 115.0f * t) * std::exp(-t * 48.0f) * 0.35f;
            m_ClipAirBrake.samples[i] = std::clamp((lp * 0.72f + valveThump) * env, -1.0f, 1.0f);
        }
    }

    // 2. Heavy dual-tone truck air horn (D#3 + F#3 rich brassy saw harmonics ~0.72s)
    {
        size_t len = static_cast<size_t>(sr * 0.72f);
        m_ClipTruckHorn.samples.resize(len);
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float attack = std::clamp(t / 0.025f, 0.0f, 1.0f);
            float release = std::clamp((0.72f - t) / 0.14f, 0.0f, 1.0f);
            float env = attack * release;
            float pitchDrop = 1.0f - (1.0f - release) * 0.03f;
            float f1 = 155.56f * pitchDrop;
            float f2 = 185.00f * pitchDrop;
            float sig = 0.0f;
            for (int h = 1; h <= 5; ++h) {
                float w = 1.0f / static_cast<float>(h * 1.15f);
                sig += (std::sin(kTwoPi * f1 * h * t) + std::sin(kTwoPi * f2 * h * t)) * w;
            }
            m_ClipTruckHorn.samples[i] = std::clamp(sig * 0.24f * env, -1.0f, 1.0f);
        }
    }

    // 3. Matatu / urban dual-tone horn (~0.45s crisp higher brass pair)
    {
        size_t len = static_cast<size_t>(sr * 0.45f);
        m_ClipMatatuHorn.samples.resize(len);
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float attack = std::clamp(t / 0.015f, 0.0f, 1.0f);
            float release = std::clamp((0.45f - t) / 0.08f, 0.0f, 1.0f);
            float env = attack * release;
            float f1 = 392.0f; // G4
            float f2 = 493.88f; // B4
            float sig = std::sin(kTwoPi * f1 * t) * 0.55f +
                        std::sin(kTwoPi * f2 * t) * 0.55f +
                        std::sin(kTwoPi * f1 * 2.0f * t) * 0.22f +
                        std::sin(kTwoPi * f2 * 2.0f * t) * 0.22f;
            m_ClipMatatuHorn.samples[i] = std::clamp(sig * 0.42f * env, -1.0f, 1.0f);
        }
    }

    // 4. Pneumatic bus/cab door release + mechanical latch clunk (~0.62s)
    {
        size_t len = static_cast<size_t>(sr * 0.62f);
        m_ClipBusDoor.samples.resize(len);
        float bp = 0.0f;
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float hissEnv = std::clamp(t / 0.03f, 0.0f, 1.0f) * std::exp(-t * 5.2f);
            float n = NextNoise();
            bp = bp * 0.78f + n * 0.22f;
            float clunk = std::sin(kTwoPi * 92.0f * (t - 0.01f)) * std::exp(-std::max(0.0f, t - 0.01f) * 38.0f) * 0.6f;
            float tailLatch = (t > 0.34f)
                ? std::sin(kTwoPi * 128.0f * (t - 0.34f)) * std::exp(-(t - 0.34f) * 45.0f) * 0.55f
                : 0.0f;
            m_ClipBusDoor.samples[i] = std::clamp(bp * hissEnv * 1.3f + clunk + tailLatch, -1.0f, 1.0f);
        }
    }

    // 5. Mechanical heavy-duty synchronized gear shift clunk (~0.11s)
    {
        size_t len = static_cast<size_t>(sr * 0.11f);
        m_ClipGearClunk.samples.resize(len);
        for (size_t i = 0; i < len; ++i) {
            float t = static_cast<float>(i) / sr;
            float thud = std::sin(kTwoPi * (140.0f - t * 400.0f) * t) * std::exp(-t * 42.0f);
            float metallic = std::sin(kTwoPi * 620.0f * t) * std::exp(-t * 68.0f) * 0.28f;
            m_ClipGearClunk.samples[i] = std::clamp((thud + metallic) * 0.45f, -1.0f, 1.0f);
        }
    }
}

void EngineAudioSystem::ConfigureVehicle(
    int32_t vehicleCategoryOrdinal,
    float enginePowerHp,
    float engineTorqueNm,
    float kerbWeightKg,
    float combinedLoadKg
) {
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

    // Air-brake pneumatic hiss on heavy brake application OR sharp air release after stopping
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
            // Starter cranking at ~260 RPM with compression oscillation
            m_Params.current_rpm = 240.0f + 55.0f * std::sin(m_StarterElapsedSec * 28.0f);
        } else if (m_StarterElapsedSec < 0.92f) {
            // Combustion catch flare up to ~1150 RPM
            float t = (m_StarterElapsedSec - 0.62f) / 0.30f;
            m_Params.current_rpm = 300.0f + std::sin(t * 2.4f) * 850.0f;
        } else {
            m_Params.ignition_state = IgnitionState::RUNNING;
            m_Params.current_rpm = m_Params.idle_rpm * 1.15f;
        }
        return;
    }

    // Effective throttle combines manual gas pedal and optional cruise control
    float effThrottle = m_Params.throttle;
    if (m_Params.cruise_drive && m_Params.brake < 0.1f) {
        effThrottle = std::max(effThrottle, (m_Params.speed_kmh < 68.0f) ? 0.58f : 0.24f);
    }

    // Mass-compensated acceleration & rolling/aero resistance
    float weightFactor = std::clamp(420.0f / std::max(220.0f, m_Params.engine_power_hp), 0.55f, 1.45f);
    float accel = (effThrottle * 16.5f / weightFactor) - m_Params.brake * 36.0f - 1.15f - m_Params.speed_kmh * 0.075f;
    m_Params.speed_kmh = std::clamp(m_Params.speed_kmh + accel * dt, 0.0f, 112.0f);
    m_Params.distance_m += (m_Params.speed_kmh / 3.6f) * dt;

    // 12-speed heavy truck / 6-speed bus automatic gearbox shifting
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
        // Free-revbing engine in Neutral when user presses gas pedal while stationary/braked
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

    // Turbo boost builds cleanly above 1150 RPM under load
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
        10.0f,
        (m_Params.current_rpm / 60.0f) * (static_cast<float>(m_Params.cylinders) * 0.5f) * m_Params.exhaust_pitch
    );

    const float dtFund = firingHz / sr;
    const float dtSub = (firingHz * 0.5f) / sr;
    const float dtHarm2 = (firingHz * 2.0f) / sr;
    const float dtHarm3 = (firingHz * 3.0f) / sr;
    const float turboHz = 1450.0f + m_Params.turbo_boost_bar * 1350.0f;
    const float dtTurbo = turboHz / sr;
    const float dtStarter = 260.0f / sr;

    const float engGain = engineAudible
        ? (0.56f + 0.28f * rpmFrac + 0.22f * m_Params.throttle)
        : 0.0f;
    const float tireGain = std::clamp(m_Params.speed_kmh / 85.0f, 0.0f, 1.0f) * 0.42f;
    const float speedCutoff = 0.14f + std::clamp(m_Params.speed_kmh / 100.0f, 0.0f, 1.0f) * 0.34f;

    for (size_t i = 0; i < count; ++i) {
        // 1. Engine oscillators with phone/emulator-audible 120Hz-1.8kHz diesel & exhaust harmonics
        float engSample = 0.0f;
        if (engineAudible) {
            m_PhaseFund += dtFund; if (m_PhaseFund >= 1.0f) m_PhaseFund -= 1.0f;
            m_PhaseSub += dtSub; if (m_PhaseSub >= 1.0f) m_PhaseSub -= 1.0f;
            m_PhaseHarm2 += dtHarm2; if (m_PhaseHarm2 >= 1.0f) m_PhaseHarm2 -= 1.0f;
            m_PhaseHarm3 += dtHarm3; if (m_PhaseHarm3 >= 1.0f) m_PhaseHarm3 -= 1.0f;
            m_PhaseTurbo += dtTurbo; if (m_PhaseTurbo >= 1.0f) m_PhaseTurbo -= 1.0f;

            float fWave = std::sin(kTwoPi * m_PhaseFund);
            float subWave = std::sin(kTwoPi * m_PhaseSub);
            float h2Wave = std::sin(kTwoPi * m_PhaseHarm2);
            float h3Wave = (2.0f * m_PhaseHarm3 - 1.0f) * 0.48f; // Warm sawtooth diesel bark
            float h4Wave = std::sin(kTwoPi * (m_PhaseHarm2 * 2.0f));
            float h6Wave = std::sin(kTwoPi * (m_PhaseHarm3 * 2.0f));

            // Diesel combustion knock bursts modulated by idle/load (punchy mid-range)
            float rawNoise = NextNoise();
            float knock = rawNoise * (0.14f + 0.18f * m_Params.throttle) * (0.45f + 0.55f * std::max(0.0f, h2Wave));

            float rawExh = subWave * 0.28f +
                           fWave * 0.44f +
                           h2Wave * 0.42f +
                           h3Wave * 0.36f +
                           h4Wave * 0.24f +
                           h6Wave * 0.14f +
                           knock;
            m_ExhaustFilterState = m_ExhaustFilterState * 0.38f + rawExh * 0.62f;

            float turboSample = 0.0f;
            if (m_Params.turbo_boost_bar > 0.12f) {
                turboSample = std::sin(kTwoPi * m_PhaseTurbo) * std::clamp((m_Params.turbo_boost_bar - 0.12f) * 0.095f, 0.0f, 0.16f);
            }

            float starterSample = 0.0f;
            if (m_Params.ignition_state == IgnitionState::STARTING && m_StarterElapsedSec < 0.68f) {
                m_PhaseStarter += dtStarter; if (m_PhaseStarter >= 1.0f) m_PhaseStarter -= 1.0f;
                float crankCycle = 0.5f + 0.5f * std::sin(kTwoPi * (m_StarterElapsedSec * 5.6f));
                float solenoidWhine = std::sin(kTwoPi * m_PhaseStarter) * 0.45f +
                                      (2.0f * m_PhaseStarter - 1.0f) * 0.35f;
                starterSample = solenoidWhine * (0.34f + 0.32f * crankCycle);
            }

            engSample = (m_ExhaustFilterState * 0.72f + rawExh * 0.38f) * engGain + turboSample + starterSample;
        }
        m_EngineBuf[i] = engSample;

        // 2. Tire road roll & wind noise proportional to road speed
        float tireSample = 0.0f;
        if (tireGain > 0.004f) {
            float n = NextNoise();
            m_TireFilterState = m_TireFilterState * (1.0f - speedCutoff) + n * speedCutoff;
            float treadHum = std::sin(kTwoPi * (56.0f + m_Params.speed_kmh * 2.25f) * (static_cast<float>(i) / sr)) * 0.22f;
            tireSample = (m_TireFilterState * 1.85f + treadHum) * tireGain;
        }
        m_TireBuf[i] = tireSample;

        // 3. RAM-cached 16-bit PCM one-shot bursts (air brake hiss, horn, door pneumatic, gear shift)
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
            osSample += clip->samples[voice.cursor++] * (voice.gain * 1.25f);
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
