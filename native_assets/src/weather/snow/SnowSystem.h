#pragma once
#include <vector>
#include "SnowParams.h"
#include "scene/Transform.h"

namespace sphere::weather {

struct Snowflake {
    Vec3 position;
    Vec3 velocity;
    float phase;
    float size;
};

class SnowSystem {
public:
    SnowSystem();
    ~SnowSystem() = default;

    void SetParams(const SnowParams& params) { m_params = params; }
    const SnowParams& GetParams() const { return m_params; }

    void Update(float deltaTime, const Vec3& windVector);
    const std::vector<Snowflake>& GetFlakes() const { return m_flakes; }
    float GetAccumulatedSnow() const { return m_accumulatedDepth; }

private:
    SnowParams m_params{};
    std::vector<Snowflake> m_flakes;
    float m_accumulatedDepth = 0.0f;
    float m_spawnAccumulator = 0.0f;
};

} // namespace sphere::weather
