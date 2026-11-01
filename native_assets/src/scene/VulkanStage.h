#pragma once
#include "Scene.h"
#include "Geometry.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <string>

namespace sphere {

struct VulkanStageConfig {
    uint32_t renderWidth = 1920;
    uint32_t renderHeight = 1080;
    VkFormat colorFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
    bool enableMsaa = false;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    bool enableShadows = true;
    uint32_t shadowMapSize = 2048;
    bool enableTonemapping = true;
};

// GPU Uniform Buffer layouts for std430 descriptor sets
struct alignas(16) GlobalSceneUBO {
    Mat4 viewMatrix;
    Mat4 projMatrix;
    Mat4 viewProjMatrix;
    Mat4 lightSpaceMatrix;
    Vec4 cameraPosition;
    Vec4 sunDirection;
    Vec4 sunColor;
    Vec4 ambientSkyColor;
    Vec4 worldParams; // x: time, y: turbidity, z: stageMode, w: shadowIntensity
};

struct alignas(16) MeshPushConstants {
    Mat4 modelMatrix;
    Vec4 baseColor;
    float roughness;
    float metallic;
    float occlusion;
    uint32_t materialType;
};

class VulkanStage {
public:
    VulkanStage(const VulkanStageConfig& config = VulkanStageConfig{});
    ~VulkanStage();

    void Initialize(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue);
    void SetScene(std::shared_ptr<Scene> scene) { m_Scene = scene; }
    std::shared_ptr<Scene> GetScene() const { return m_Scene; }

    void Resize(uint32_t width, uint32_t height);
    void UpdateUniforms(float deltaTime);

    // Multi-pass rendering pipeline execution
    void RecordShadowPass(VkCommandBuffer cmd);
    void RecordSkyAtmospherePass(VkCommandBuffer cmd);
    void RecordOpaqueGeometryPass(VkCommandBuffer cmd);
    void RecordDebugGridPass(VkCommandBuffer cmd);
    void RecordPostProcessPass(VkCommandBuffer cmd);

    const VulkanStageConfig& GetConfig() const { return m_Config; }

private:
    VulkanStageConfig m_Config;
    std::shared_ptr<Scene> m_Scene;
    GlobalSceneUBO m_SceneUBO{};
    VkDevice m_Device = VK_NULL_HANDLE;
    bool m_IsInitialized = false;
};

} // namespace sphere
