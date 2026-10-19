#pragma once
#include <memory>
#include <vector>
#include "WeatherState.h"
#include "shared/IGenerator.h"
#include "scene/WorldState.h"

namespace sphere {

class WeatherSystem : public IGenerator {
public:
    WeatherSystem();
    virtual ~WeatherSystem() override = default;

    virtual const std::string& GetName() const override { return m_name; }
    virtual GeneratorType GetType() const override { return GeneratorType::Weather; }
    virtual bool Generate(GeneratorContext& context, std::shared_ptr<SceneNode> outNode) override;

    void SetCondition(WeatherCondition condition);
    WeatherCondition GetCondition() const { return m_state.condition; }

    WeatherState& GetState() { return m_state; }
    const WeatherState& GetState() const { return m_state; }

    void Update(float deltaTime, WorldState& worldState);
    void TriggerLightningFlash();
    void ApplyPreset(const std::string& presetName);

private:
    std::string m_name{"ProceduralWeatherSystem"};
    WeatherState m_state{};
    float m_lightningTimer = 0.0f;
    float m_lightningDuration = 0.0f;
    float m_puddleAccumulation = 0.0f;
};

} // namespace sphere
