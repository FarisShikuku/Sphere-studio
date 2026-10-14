#include "Scene.h"

namespace sphere {
Scene::Scene() {
    m_RootNode = std::make_shared<SceneNode>();
    m_RootNode->name = "CommonSceneRoot";

    // Default primary sun light
    Light sunLight;
    sunLight.type = LightType::Directional;
    sunLight.direction = Vec3{-0.4f, -0.8f, -0.4f};
    sunLight.color = Vec3{1.0f, 0.98f, 0.92f};
    sunLight.intensity = 3.0f;
    AddLight(sunLight);
}

Scene::~Scene() {}

void Scene::Update(float deltaTime) {
    m_WorldState.time += deltaTime;
}

void Scene::Clear() {
    m_ActiveAssetNode = nullptr;
}
}\n