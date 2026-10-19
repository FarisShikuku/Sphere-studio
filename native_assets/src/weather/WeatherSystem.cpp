#include "WeatherSystem.h"
#include <cmath>
#include <algorithm>

namespace sphere {

WeatherSystem::WeatherSystem() {
    SetCondition(WeatherCondition::Clear);
}

void WeatherSystem::SetCondition(WeatherCondition condition) {
    m_state.condition = condition;
    switch (condition) {
        case WeatherCondition::Clear:
            m_state.precipitationIntensity = 0.0f;
            m_state.cloudCoverage = 0.10f;
            m_state.fogDensity = 0.01f;
            m_state.windSpeed = 6.0f;
            m_state.temperatureCelsius = 22.0f;
            m_state.isStormy = false;
            break;
        case WeatherCondition::Overcast:
            m_state.precipitationIntensity = 0.0f;
            m_state.cloudCoverage = 0.85f;
            m_state.fogDensity = 0.06f;
            m_state.windSpeed = 12.0f;
            m_state.temperatureCelsius = 16.0f;
            m_state.isStormy = false;
            break;
        case WeatherCondition::Rain:
            m_state.precipitationIntensity = 0.45f;
            m_state.cloudCoverage = 0.90f;
            m_state.fogDensity = 0.12f;
            m_state.windSpeed = 18.0f;
            m_state.temperatureCelsius = 14.0f;
            m_state.isStormy = false;
            break;
        case WeatherCondition::HeavyRain:
            m_state.precipitationIntensity = 0.85f;
            m_state.cloudCoverage = 0.98f;
            m_state.fogDensity = 0.20f;
            m_state.windSpeed = 32.0f;
            m_state.temperatureCelsius = 12.0f;
            m_state.isStormy = false;
            break;
        case WeatherCondition::Snow:
            m_state.precipitationIntensity = 0.40f;
            m_state.cloudCoverage = 0.80f;
            m_state.fogDensity = 0.15f;
            m_state.windSpeed = 10.0f;
            m_state.temperatureCelsius = -2.0f;
            m_state.isStormy = false;
            break;
        case WeatherCondition::Blizzard:
            m_state.precipitationIntensity = 0.95f;
            m_state.cloudCoverage = 1.0f;
            m_state.fogDensity = 0.55f;
            m_state.windSpeed = 55.0f;
            m_state.temperatureCelsius = -8.0f;
            m_state.isStormy = true;
            break;
        case WeatherCondition::Fog:
            m_state.precipitationIntensity = 0.05f;
            m_state.cloudCoverage = 0.70f;
            m_state.fogDensity = 0.85f;
            m_state.windSpeed = 3.0f;
            m_state.temperatureCelsius = 9.0f;
            m_state.isStormy = false;
            break;
        case WeatherCondition::Thunderstorm:
            m_state.precipitationIntensity = 0.90f;
            m_state.cloudCoverage = 0.98f;
            m_state.fogDensity = 0.25f;
            m_state.windSpeed = 42.0f;
            m_state.temperatureCelsius = 15.0f;
            m_state.isStormy = true;
            break;
    }
}

void WeatherSystem::TriggerLightningFlash() {
    m_state.lightningIntensity = 1.0f;
    m_lightningDuration = 0.25f;
}

void WeatherSystem::ApplyPreset(const std::string& presetName) {
    if (presetName == "Clear") SetCondition(WeatherCondition::Clear);
    else if (presetName == "Rain") SetCondition(WeatherCondition::Rain);
    else if (presetName == "Downpour") SetCondition(WeatherCondition::HeavyRain);
    else if (presetName == "Snow") SetCondition(WeatherCondition::Snow);
    else if (presetName == "Blizzard") SetCondition(WeatherCondition::Blizzard);
    else if (presetName == "Fog") SetCondition(WeatherCondition::Fog);
    else if (presetName == "Storm") SetCondition(WeatherCondition::Thunderstorm);
}

void WeatherSystem::Update(float deltaTime, WorldState& worldState) {
    m_state.simulationTime += deltaTime;

    // Wind direction vector calculation
    float rad = m_state.windAngleDegrees * (3.14159265f / 180.0f);
    m_state.windDirection.x = std::cos(rad);
    m_state.windDirection.z = std::sin(rad);
    m_state.windDirection.y = 0.0f;

    // Puddle accumulation & wetness drainage
    if (m_state.condition == WeatherCondition::Rain ||
        m_state.condition == WeatherCondition::HeavyRain ||
        m_state.condition == WeatherCondition::Thunderstorm) {
        m_state.wetness = std::min(1.0f, m_state.wetness + deltaTime * 0.15f * m_state.precipitationIntensity);
        m_state.puddleDepth = std::min(1.0f, m_state.puddleDepth + deltaTime * 0.08f * m_state.precipitationIntensity);
    } else {
        m_state.wetness = std::max(0.0f, m_state.wetness - deltaTime * 0.02f);
        m_state.puddleDepth = std::max(0.0f, m_state.puddleDepth - deltaTime * 0.01f);
    }

    // Lightning simulation during storms
    if (m_state.isStormy) {
        m_lightningTimer += deltaTime;
        if (m_lightningTimer > 4.5f) {
            TriggerLightningFlash();
            m_lightningTimer = 0.0f;
        }
    }

    if (m_lightningDuration > 0.0f) {
        m_lightningDuration -= deltaTime;
        m_state.lightningIntensity = std::max(0.0f, m_lightningDuration / 0.25f);
    } else {
        m_state.lightningIntensity = 0.0f;
    }

    // Sync back to worldState
    worldState.windSpeed = m_state.windSpeed;
    worldState.windDirection = m_state.windDirection;
    worldState.rainIntensity = m_state.precipitationIntensity;
    worldState.wetness = m_state.wetness;
}

bool WeatherSystem::Generate(GeneratorContext& context, std::shared_ptr<SceneNode> outNode) {
    if (!outNode) return false;
    outNode->name = "WeatherSimulationCluster";
    return true;
}

} // namespace sphere
