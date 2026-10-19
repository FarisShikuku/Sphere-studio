#pragma once
#include <vector>
#include "RainParams.h"
#include "scene/Transform.h"

namespace sphere::weather {

struct RainDrop {
    Vec3 position;
    Vec3 velocity;
    float life;
    float size;
};

class RainSystem {
public:
    RainSystem();
    ~RainSystem() = default;

    void SetParams(const RainParams& params) { m_params = params; }
    const RainParams& GetParams() const { return m_params; }

    void Update(float deltaTime, const Vec3& windVector);
    const std::vector<RainDrop>& GetDrops() const { return m_drops; }
    uint32_t GetActiveDropCount() const { return static_cast<uint32_t>(m_drops.size()); }

private:
    RainParams m_params{};
    std::vector<RainDrop> m_drops;
    float m_spawnAccumulator = 0.0f;
};

} // namespace sphere::weather
