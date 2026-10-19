#include "RainSplash.h"
#include <algorithm>

namespace sphere::weather {

void RainSplash::Emit(const Vec3& groundPos, float intensity) {
    if (m_splashes.size() >= 1024) return;
    SplashParticle p;
    p.position = groundPos;
    p.radius = 0.05f * intensity;
    p.alpha = 1.0f;
    m_splashes.push_back(p);
}

void RainSplash::Update(float deltaTime) {
    for (auto& s : m_splashes) {
        s.radius += deltaTime * 0.4f;
        s.alpha -= deltaTime * 3.5f;
    }
    m_splashes.erase(
        std::remove_if(m_splashes.begin(), m_splashes.end(), [](const SplashParticle& p) {
            return p.alpha <= 0.0f;
        }),
        m_splashes.end()
    );
}

} // namespace sphere::weather
