# Procedural & Hybrid Vehicle Audio (`gen/audio/vehicles/`)

Zero-latency C++20 / Vulkan 1.3 vehicle audio synthesizer combining:
- Continuous procedural RPM oscillator synthesis (multi-cylinder firing fundamental, diesel harmonics, turbocharger spool, speed-coupled tire roll).
- RAM-cached uncompressed 44.1 kHz mono 16-bit PCM one-shots (`AIR_BRAKE_HISS`, `TRUCK_HORN_HEAVY`, `MATATU_HORN`, `BUS_DOOR_PNEUMATIC`, `GEAR_SHIFT_CLUNK`) for zero decode latency.
- Vulkan compute shaders (`engine_osc.comp`, `granular_synth.comp`) mirroring the CPU DSP parameters.
