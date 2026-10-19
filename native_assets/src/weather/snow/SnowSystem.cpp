#include "SnowSystem.h"
#include <cmath>
#include <algorithm>

namespace sphere::weather {

SnowSystem::SnowSystem() {
    m_flakes.reserve(4096);
}

void SnowSystem::Update(float deltaTime, const Vec3& windVector) {
    for (auto& f : m_flakes) {
        f.phase += deltaTime * m_params.flutterFrequency;
        float flutterX = std::sin(f.phase) * m_params.flutterAmplitude;
        float flutterZ = std::cos(f.phase * 0.8f) * m_params.flutterAmplitude;

        f.position.x += (windVector.x + flutterX) * deltaTime;
        f.position.y -= m_params.fallSpeed * deltaTime;
        f.position.z += (windVector.z + flutterZ) * deltaTime;
    }

    m_flakes.erase(
        std::remove_if(m_flakes.begin(), m_flakes.end(), [this](const Snowflake& f) {
            if (f.position.y <= 0.0f) {
                m_accumulatedDepth += m_params.accumulationRate * 0.001f;
                return true;
            }
            return false;
        }),
        m_flakes.end()
    );

    // Spawn snowflakes
    m_spawnAccumulator += deltaTime * m_params.intensity * 1200.0f;
    while (m_spawnAccumulator >= 1.0f && m_flakes.size() < 4096) {
        m_spawnAccumulator -= 1.0f;
        Snowflake flake;
        float rx = static_cast<float>(rand() % 3000 - 1500) / 100.0f;
        float rz = static_cast<float>(rand() % 3000 - 1500) / 100.0f;
        flake.position = Vec3{rx, 25.0f, rz};
        flake.velocity = Vec3{0.0f, -m_params.fallSpeed, 0.0f};
        flake.phase = static_cast<float>(rand() % 628) / 100.0f;
        flake.size = m_params.flakeSize;
        m_flakes.push_back(flake);
    }
}

} // namespace sphere::weather
