# Weather Procedural & PCM Audio System (`gen/audio/weather/`)

Zero-latency hybrid C++20 / Vulkan 1.3 audio synthesis architecture for dynamic weather environments:

- **Continuous Procedural Synthesis (`WeatherAudioSystem.cpp` & GLSL `.comp`)**:
  - Rain sheets + stochastic raindrop splashes proportional to `rain_intensity`.
  - Resonant sweeping wind gusts & gale howling proportional to `wind_speed_kmh`.
  - Crystalline snow air shimmer and low atmospheric fog/mist bed.
- **Zero-Latency RAM One-Shots**:
  - Pre-baked 44.1 kHz 16-bit PCM clips (`THUNDER_CLAP_CLOSE`, `THUNDER_RUMBLE_DISTANT`, `GUST_WHOOSH`, `HAIL_BURST`) triggered directly on lightning bolt discharges.
- **No MP3/FLAC Runtime Overhead**: Soft-clipped mono/stereo PCM output with zero decode latency.
