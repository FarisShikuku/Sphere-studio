#include "RainSystem.h"
#include <algorithm>

namespace sphere::weather {

RainSystem::RainSystem() {
    m_drops.reserve(4096);
}

void RainSystem::Update(float deltaTime, const Vec3& windVector) {
    // Integrate active raindrops
    for (auto& drop : m_drops) {
        drop.position.x += (drop.velocity.x + windVector.x * m_params.windDeflection) * deltaTime;
        drop.position.y += drop.velocity.y * deltaTime;
        drop.position.z += (drop.velocity.z + windVector.z * m_params.windDeflection) * deltaTime;
        drop.life -= deltaTime;
    }

    m_drops.erase(
        std::remove_if(m_drops.begin(), m_drops.end(), [](const RainDrop& d) {
            return d.life <= 0.0f || d.position.y <= 0.0f;
        }),
        m_drops.end()
    );

    // Spawn new drops based on intensity
    m_spawnAccumulator += deltaTime * m_params.intensity * 2000.0f;
    while (m_spawnAccumulator >= 1.0f && m_drops.size() < 4096) {
        m_spawnAccumulator -= 1.0f;
        RainDrop newDrop;
        float rx = static_cast<float>(rand() % 2000 - 1000) / 100.0f;
        float rz = static_cast<float>(rand() % 2000 - 1000) / 100.0f;
        newDrop.position = Vec3{rx, 25.0f, rz};
        newDrop.velocity = Vec3{0.0f, -m_params.dropSpeed, 0.0f};
        newDrop.life = 2.0f;
        newDrop.size = m_params.dropLength;
        m_drops.push_back(newDrop);
    }
}

} // namespace sphere::weather
