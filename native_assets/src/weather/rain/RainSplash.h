#pragma once
#include <vector>
#include "scene/Transform.h"

namespace sphere::weather {

struct SplashParticle {
    Vec3 position;
    float radius;
    float alpha;
};

class RainSplash {
public:
    RainSplash() = default;
    void Emit(const Vec3& groundPos, float intensity);
    void Update(float deltaTime);
    const std::vector<SplashParticle>& GetSplashes() const { return m_splashes; }

private:
    std::vector<SplashParticle> m_splashes;
};

} // namespace sphere::weather
