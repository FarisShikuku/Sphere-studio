#pragma once

#include <cstdint>

namespace sphere::audio {

enum class IgnitionState : int32_t {
    OFF = 0,
    STARTING = 1,
    RUNNING = 2
};

enum class AudioVehicleClass : int32_t {
    HEAVY_TRACTOR_V8 = 0,
    RIGID_LORRY_I6 = 1,
    CITY_BUS_TRANSIT = 2,
    COACH_HIGHWAY = 3,
    MINIBUS_DIESEL = 4
};

struct VehicleAudioParams {
    IgnitionState ignition_state = IgnitionState::OFF;
    AudioVehicleClass vehicle_class = AudioVehicleClass::HEAVY_TRACTOR_V8;

    float idle_rpm = 650.0f;
    float max_rpm = 2300.0f;
    float current_rpm = 0.0f;

    float throttle = 0.0f;         // 0.0 .. 1.0 (gas pedal)
    float brake = 0.0f;            // 0.0 .. 1.0 (brake pedal)
    float speed_kmh = 0.0f;        // 0.0 .. 120.0 km/h
    float distance_m = 0.0f;       // cumulative odometer / spatial world travel

    int32_t gear = 0;              // 0 = Neutral, 1..12 = Forward Gears
    int32_t cylinders = 8;
    float engine_power_hp = 520.0f;
    float exhaust_pitch = 1.0f;
    float turbo_boost_bar = 0.0f;  // 0.0 .. 2.4 bar
    float tire_slip = 0.0f;
    float trailer_load_kg = 0.0f;
    bool air_brake_active = false;
    bool cruise_drive = false;
    bool muted = false;
};

struct VehicleAudioTelemetry {
    int32_t ignition_state = 0;
    float rpm = 0.0f;
    float speed_kmh = 0.0f;
    int32_t gear = 0;
    float turbo_boost_bar = 0.0f;
    float distance_m = 0.0f;
    int32_t has_active_sound = 0;
};

} // namespace sphere::audio
