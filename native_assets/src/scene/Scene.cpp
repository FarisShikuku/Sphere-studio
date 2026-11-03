#include "Scene.h"
#include <cmath>
namespace sphere {
Scene::Scene() : m_CameraController(m_Camera) {
    m_RootNode = std::make_shared<SceneNode>();
    m_RootNode->name = "CommonSceneRoot";
    // Setup primary directional sun light
    Light sunLight;
    sunLight.type = LightType::Directional;
    sunLight.direction = Vec3{-0.4f, -0.8f, -0.4f}.Normalized();
    sunLight.color = Vec3{1.0f, 0.98f, 0.92f};
    sunLight.intensity = 3.5f;
    sunLight.castShadows = true;
    sunLight.shadowFilter = ShadowFilter::PCF_5x5;
    m_Lights.push_back(sunLight);
    // Ambient fill light from opposite hemisphere
    Light fillLight;
    fillLight.type = LightType::Directional;
    fillLight.direction = Vec3{0.3f, 0.6f, 0.3f}.Normalized();
    fillLight.color = Vec3{0.2f, 0.35f, 0.5f};
    fillLight.intensity = 0.8f;
    fillLight.castShadows = false;
    m_Lights.push_back(fillLight);
    InitializeDefaultStage();
}
Scene::~Scene() {}
void Scene::InitializeDefaultStage() {
    RebuildStageGeometry();
}
void Scene::RebuildStageGeometry() {
    // Generate base stage platform
    m_StageMesh = std::make_shared<Mesh>(
        GeometryGenerator::GenerateStagePlatform(m_StageWidth, m_StageLength, m_StageHeight, 0.06f)
    );
    // Reference grid lines
    m_GridLinesMesh = std::make_shared<Mesh>(
        GeometryGenerator::GenerateGridLines(24, 0.5f, true)
    );
    // Infinite ground plane
    m_InfiniteGridMesh = std::make_shared<Mesh>(
        GeometryGenerator::GenerateInfiniteGridPlane(150.0f, 60.0f)
    );
    // 1m Photometric Calibration Cube
    m_CalibrationCubeMesh = std::make_shared<Mesh>(
        GeometryGenerator::GenerateCalibrationCube(1.0f, 0.03f)
    );
    // Sky Dome for atmospheric scattering
    m_SkyDomeMesh = std::make_shared<Mesh>(
        GeometryGenerator::GenerateSkyDome(600.0f, 24, 48, -0.05f)
    );
    // Sync Scene Graph with chosen stage mode
    m_SceneGraph.Clear();
    auto stageNode = std::make_shared<SceneGraphNode>("StagePlatformNode");
    stageNode->mesh = m_StageMesh;
    m_SceneGraph.GetRoot()->AddChild(stageNode);
    auto gridNode = std::make_shared<SceneGraphNode>("GridLinesNode");
    gridNode->mesh = m_GridLinesMesh;
    m_SceneGraph.GetRoot()->AddChild(gridNode);
    if (m_StageMode == StageMode::CalibrationCube) {
        auto cubeNode = std::make_shared<SceneGraphNode>("CalibrationCubeNode");
        cubeNode->mesh = m_CalibrationCubeMesh;
        cubeNode->transform.position = Vec3{0.0f, 0.0f, 0.0f};
        m_SceneGraph.GetRoot()->AddChild(cubeNode);
    } else if (m_StageMode == StageMode::InfiniteGrid) {
        auto infGridNode = std::make_shared<SceneGraphNode>("InfiniteGridNode");
        infGridNode->mesh = m_InfiniteGridMesh;
        m_SceneGraph.GetRoot()->AddChild(infGridNode);
    }
    if (m_ActiveAssetNode) {
        m_SceneGraph.GetRoot()->AddChild(m_ActiveAssetNode);
    }
    m_SceneGraph.Update();
}
void Scene::SetStageMode(StageMode mode) {
    if (m_StageMode != mode) {
        m_StageMode = mode;
        RebuildStageGeometry();
    }
}
void Scene::SetStageDimensions(float width, float length, float height) {
    m_StageWidth = width;
    m_StageLength = length;
    m_StageHeight = height;
    RebuildStageGeometry();
}
void Scene::SetActiveAssetCategory(const std::string& category) {
    m_ActiveCategory = category;
}
void Scene::SetActiveAssetNode(std::shared_ptr<SceneGraphNode> assetNode) {
    m_ActiveAssetNode = assetNode;
    RebuildStageGeometry();
}
void Scene::AddLight(const Light& light) {
    m_Lights.push_back(light);
}
void Scene::ClearLights() {
    m_Lights.clear();
}
void Scene::Update(float deltaTime) {
    m_WorldState.time += deltaTime;
    // Synchronize Sun direction with SkyModel
    m_Sky.UpdateSunDirection();
    if (!m_Lights.empty()) {
        m_Lights[0].direction = m_Sky.sunDirection;
        m_Lights[0].color = m_Sky.sunColor;
        m_Lights[0].intensity = m_Sky.sunIntensity * 0.15f;
    }
    m_CameraController.Update(deltaTime);
    m_SceneGraph.Update();
    // Update scene statistics
    m_Stats.drawCalls = 0;
    m_Stats.totalVertices = 0;
    m_Stats.totalTriangles = 0;
    m_Stats.visibleNodes = 0;
    m_Stats.lightCount = static_cast<uint32_t>(m_Lights.size());
    m_SceneGraph.ForEachVisibleNode([this](SceneGraphNode& node) {
        m_Stats.visibleNodes++;
        if (node.mesh && !node.mesh->IsEmpty()) {
            m_Stats.drawCalls++;
            m_Stats.totalVertices += static_cast<uint32_t>(node.mesh->GetVertexCount());
            m_Stats.totalTriangles += static_cast<uint32_t>(node.mesh->GetTriangleCount());
        }
    });
}
void Scene::Clear() {
    m_ActiveAssetNode = nullptr;
    m_SceneGraph.Clear();
}
} // namespace sphere