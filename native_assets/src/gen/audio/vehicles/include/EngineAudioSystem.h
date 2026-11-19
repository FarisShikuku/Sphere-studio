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

/**
 * VehicleAudioMixer:
 * Zero-allocation mono PCM-16 mixing stage for combining procedural engine oscillators,
 * tire rolling noise, and RAM-cached uncompressed one-shot clips with soft limiting.
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
 * Singleton hybrid procedural + RAM-cached PCM audio synthesizer for Sphere Studio vehicles.
 * - Continuous synthesis: 4-stroke multi-cylinder firing fundamental + diesel knock harmonics +
 *   turbocharger spool + speed-proportional tire rolling rumble.
 * - Zero-latency RAM-cached 44.1kHz mono one-shots: air brake hiss, heavy dual-tone truck horn,
 *   matatu twin horn, pneumatic bus/cab door release, and mechanical gearbox clunk.
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

    void StepPhysics(float dtSeconds);

    /**
     * Advances powertrain physics, synthesizes procedural engine/tire oscillators,
     * mixes active uncompressed RAM one-shots, and writes mono 16-bit PCM samples.
     */
    void RenderAudioBlock(int16_t* outPcm16, int32_t sampleCount);

    VehicleAudioTelemetry GetTelemetry() const;
    bool HasAudibleSignal() const;

private:
    EngineAudioSystem();
    void BakeOneShotClips();
    float NextNoise();
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
    float m_TireFilterState = 0.0f;
    float m_ExhaustFilterState = 0.0f;

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

    static constexpr int kMaxVoices = 8;
    ActiveOneShotVoice m_Voices[kMaxVoices];

    std::vector<float> m_EngineBuf;
    std::vector<float> m_TireBuf;
    std::vector<float> m_OneShotBuf;
};

} // namespace sphere::audio
