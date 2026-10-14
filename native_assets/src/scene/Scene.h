#pragma once
#include "Camera.h"
#include "Light.h"
#include "SkyModel.h"
#include "WorldState.h"
#include "SceneNode.h"
#include <vector>
#include <memory>
#include <string>

namespace sphere {

// The Common Scene where all procedural assets are viewed
class Scene {
public:
    Scene();
    ~Scene();

    void Update(float deltaTime);
    void Clear();

    Camera& GetCamera() { return m_Camera; }
    SkyModel& GetSky() { return m_Sky; }
    WorldState& GetWorldState() { return m_WorldState; }

    void SetActiveAssetCategory(const std::string& category) { m_ActiveCategory = category; }
    const std::string& GetActiveAssetCategory() const { return m_ActiveCategory; }

    void SetActiveAssetSlot(std::shared_ptr<SceneNode> assetNode) { m_ActiveAssetNode = assetNode; }
    std::shared_ptr<SceneNode> GetActiveAssetSlot() const { return m_ActiveAssetNode; }

    void AddLight(const Light& light) { m_Lights.push_back(light); }
    const std::vector<Light>& GetLights() const { return m_Lights; }

private:
    Camera m_Camera;
    SkyModel m_Sky;
    WorldState m_WorldState;
    std::string m_ActiveCategory = "water";
    std::shared_ptr<SceneNode> m_ActiveAssetNode = nullptr;
    std::vector<Light> m_Lights;
    std::shared_ptr<SceneNode> m_RootNode;
};
}\n