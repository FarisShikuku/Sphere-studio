#pragma once
#include "Camera.h"
#include "CameraController.h"
#include "Light.h"
#include "SkyModel.h"
#include "WorldState.h"
#include "SceneNode.h"
#include "SceneGraph.h"
#include "Geometry.h"
#include <vector>
#include <memory>
#include <string>
namespace sphere {
struct SceneRenderStats {
    uint32_t drawCalls = 0;
    uint32_t totalVertices = 0;
    uint32_t totalTriangles = 0;
    uint32_t visibleNodes = 0;
    uint32_t lightCount = 0;
    float frameCpuTimeMs = 0.0f;
};
// The Common Vulkan Scene where all procedural assets, skies, and weather interact
class Scene {
public:
    Scene();
    ~Scene();
    void InitializeDefaultStage();
    void Update(float deltaTime);
    void Clear();
    // Scene stage mode
    void SetStageMode(StageMode mode);
    StageMode GetStageMode() const { return m_StageMode; }
    void SetStageDimensions(float width, float length, float height);
    // Active Asset & Nodes
    void SetActiveAssetCategory(const std::string& category);
    const std::string& GetActiveAssetCategory() const { return m_ActiveCategory; }
    void SetActiveAssetNode(std::shared_ptr<SceneGraphNode> assetNode);
    std::shared_ptr<SceneGraphNode> GetActiveAssetNode() const { return m_ActiveAssetNode; }
    // Core Components
    Camera& GetCamera() { return m_Camera; }
    const Camera& GetCamera() const { return m_Camera; }
    CameraController& GetCameraController() { return m_CameraController; }
    SkyModel& GetSky() { return m_Sky; }
    const SkyModel& GetSky() const { return m_Sky; }
    WorldState& GetWorldState() { return m_WorldState; }
    SceneGraph& GetSceneGraph() { return m_SceneGraph; }
    // Lighting & Shadows
    void AddLight(const Light& light);
    void ClearLights();
    const std::vector<Light>& GetLights() const { return m_Lights; }
    Light& GetPrimarySunLight() { return m_Lights[0]; }
    // Scene Meshes & Queries
    std::shared_ptr<Mesh> GetStageMesh() const { return m_StageMesh; }
    std::shared_ptr<Mesh> GetGridLinesMesh() const { return m_GridLinesMesh; }
    std::shared_ptr<Mesh> GetCalibrationCubeMesh() const { return m_CalibrationCubeMesh; }
    std::shared_ptr<Mesh> GetSkyDomeMesh() const { return m_SkyDomeMesh; }
    const SceneRenderStats& GetRenderStats() const { return m_Stats; }
    void SetShowGrid(bool show) { m_ShowGrid = show; RebuildStageGeometry(); }
    bool GetShowGrid() const { return m_ShowGrid; }
private:
    void RebuildStageGeometry();
    StageMode m_StageMode = StageMode::FlatPiece;
    bool m_ShowGrid = false;
    float m_StageWidth = 10.0f;
    float m_StageLength = 10.0f;
    float m_StageHeight = 0.35f;
    Camera m_Camera;
    CameraController m_CameraController;
    SkyModel m_Sky;
    WorldState m_WorldState;
    SceneGraph m_SceneGraph;
    std::string m_ActiveCategory = "water";
    std::shared_ptr<SceneGraphNode> m_ActiveAssetNode = nullptr;
    std::vector<Light> m_Lights;
    std::shared_ptr<SceneNode> m_RootNode;
    // Procedural Stage Meshes
    std::shared_ptr<Mesh> m_StageMesh;
    std::shared_ptr<Mesh> m_GridLinesMesh;
    std::shared_ptr<Mesh> m_InfiniteGridMesh;
    std::shared_ptr<Mesh> m_CalibrationCubeMesh;
    std::shared_ptr<Mesh> m_SkyDomeMesh;
    SceneRenderStats m_Stats;
};
} // namespace sphere