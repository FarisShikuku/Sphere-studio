#pragma once

#include "VehicleAudioParams.h"
#include <cstdint>
#include <mutex>
#include <vector>

namespace sphere::audio {

enum class OneShotSoundId : int32_t {
    AIR_BRAKE_HISS = 0,
    TRUCK_HORN_HEAVY = 1,
    BUS_DOOR_PNEUMATIC = 2,
    MATATU_HORN = 3,
    GEAR_SHIFT_CLUNK = 4
};

struct PcmOneShotClip {
    std::vector<float> samples; // mono normalized [-1, 1] at 44.1 kHz
    uint32_t sample_rate = 44100;
};

struct ActiveOneShotVoice {
    OneShotSoundId id = OneShotSoundId::AIR_BRAKE_HISS;
    size_t cursor = 0;
    float gain = 1.0f;
    float pitch_mul = 1.0f;
    bool active = false;
};

struct BiquadState {
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
 * VehicleAudioMixer:
 * Zero-allocation mono PCM-16 mixing stage for combining procedural/sampled engine audio,
 * road/tire rolling rumble, and RAM-cached uncompressed one-shot clips with soft limiting.
 */
class VehicleAudioMixer {
public:
    static void MixBlockToPcm16(
        const float* engineSamples,
        const float* tireSamples,
        const float* oneShotSamples,
        int32_t sampleCount,
        float masterGain,
        int16_t* outPcm16
    );
};

/**
 * EngineAudioSystem:
 * Singleton hybrid physical-DSP + RAM-cached PCM audio synthesizer for Sphere Studio vehicles.
 * - Eliminates raw white-noise static via Paul Kellet pink/brown noise & biquad exhaust formants.
 * - Continuous 4-stroke compression pulse train + muffler cavity resonances + continuous tread phase.
 * - Supports optional real audio sample overrides uploaded from app assets (WAV/OGG/MP3 decoded).
 */
class EngineAudioSystem {
public:
    static constexpr uint32_t kSampleRate = 44100;

    static EngineAudioSystem& Get();

    void ConfigureVehicle(
        int32_t vehicleCategoryOrdinal,
        float enginePowerHp,
        float engineTorqueNm,
        float kerbWeightKg,
        float combinedLoadKg
    );

    void SetIgnition(bool turnOn);
    void SetControls(float throttle, float brake, bool cruiseDrive, bool muted);
    void TriggerOneShot(OneShotSoundId id, float gain = 1.0f);

    /**
     * Uploads a real decoded mono PCM16 sample into RAM cache to override procedural synthesis.
     * Keys: "truck_engine", "truck_horn", "matatu_horn", "air_brake", "bus_door", "gear_shift"
     */
    void UploadCustomSample(int32_t slotId, const int16_t* pcm16, int32_t sampleCount, int32_t sampleRate);

    void StepPhysics(float dtSeconds);
    void RenderAudioBlock(int16_t* outPcm16, int32_t sampleCount);

    VehicleAudioTelemetry GetTelemetry() const;
    bool HasAudibleSignal() const;

private:
    EngineAudioSystem();
    void BakeOneShotClips();
    float NextWhiteNoise();
    float NextPinkNoise();
    void StepPhysicsLocked(float dtSeconds);

    mutable std::mutex m_Mutex;
    VehicleAudioParams m_Params;

    // Oscillator phases [0..1)
    float m_PhaseFund = 0.0f;
    float m_PhaseSub = 0.0f;
    float m_PhaseHarm2 = 0.0f;
    float m_PhaseHarm3 = 0.0f;
    float m_PhaseTurbo = 0.0f;
    float m_PhaseStarter = 0.0f;
    float m_PhaseTread = 0.0f;
    float m_CustomEngineCursor = 0.0f;

    // Pink / Brown noise and biquad exhaust/tire filters
    float m_PinkB0 = 0.0f, m_PinkB1 = 0.0f, m_PinkB2 = 0.0f;
    float m_PinkB3 = 0.0f, m_PinkB4 = 0.0f, m_PinkB5 = 0.0f, m_PinkB6 = 0.0f;
    float m_BrownState = 0.0f;
    float m_ExhaustLpState = 0.0f;
    BiquadState m_ExhaustLowFormant;
    BiquadState m_ExhaustMidFormant;
    BiquadState m_ExhaustCutoffLp;
    BiquadState m_TireLp;

    float m_StarterElapsedSec = 0.0f;
    float m_BrakeHissCooldownSec = 0.0f;
    float m_PrevBrakeInput = 0.0f;
    float m_ShiftPulseSec = 0.0f;
    uint32_t m_RngState = 0x9E3779B9u;

    PcmOneShotClip m_ClipAirBrake;
    PcmOneShotClip m_ClipTruckHorn;
    PcmOneShotClip m_ClipBusDoor;
    PcmOneShotClip m_ClipMatatuHorn;
    PcmOneShotClip m_ClipGearClunk;
    PcmOneShotClip m_CustomEngineLoop;

    static constexpr int kMaxVoices = 8;
    ActiveOneShotVoice m_Voices[kMaxVoices];

    std::vector<float> m_EngineBuf;
    std::vector<float> m_TireBuf;
    std::vector<float> m_OneShotBuf;
};

} // namespace sphere::audio
