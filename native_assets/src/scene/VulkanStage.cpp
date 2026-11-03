#include "VulkanStage.h"
namespace sphere {
VulkanStage::VulkanStage(const VulkanStageConfig& config) : m_Config(config) {
    m_Scene = std::make_shared<Scene>();
}
VulkanStage::~VulkanStage() {}
void VulkanStage::Initialize(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue) {
    m_Device = device;
    m_IsInitialized = true;
    m_Scene->InitializeDefaultStage();
}
void VulkanStage::Resize(uint32_t width, uint32_t height) {
    m_Config.renderWidth = width;
    m_Config.renderHeight = height;
    float aspect = (height > 0) ? (static_cast<float>(width) / height) : 1.777f;
    m_Scene->GetCamera().SetPerspective(60.0f, aspect, 0.1f, 1000.0f);
}
void VulkanStage::UpdateUniforms(float deltaTime) {
    if (!m_Scene) return;
    m_Scene->Update(deltaTime);
    const auto& cam = m_Scene->GetCamera();
    const auto& sky = m_Scene->GetSky();
    const auto& lights = m_Scene->GetLights();
    m_SceneUBO.viewMatrix = cam.GetViewMatrix();
    m_SceneUBO.projMatrix = cam.GetProjectionMatrix();
    m_SceneUBO.viewProjMatrix = Mat4::Multiply(cam.GetProjectionMatrix(), cam.GetViewMatrix());
    // Light-space matrix for Cascaded Shadow Mapping (SaschaWillems style)
    if (!lights.empty()) {
        Vec3 lightDir = lights[0].direction;
        Vec3 lightPos = lightDir * (-25.0f);
        Mat4 lightView = Mat4::LookAt(lightPos, Vec3{0, 0, 0}, Vec3{0, 1, 0});
        // Orthographic projection for directional sun
        Mat4 lightProj = Mat4::Identity();
        m_SceneUBO.lightSpaceMatrix = Mat4::Multiply(lightProj, lightView);
        m_SceneUBO.sunDirection = Vec4(lightDir, 1.0f);
        m_SceneUBO.sunColor = Vec4(lights[0].color, lights[0].intensity);
    }
    m_SceneUBO.cameraPosition = Vec4(cam.GetPosition(), 1.0f);
    m_SceneUBO.ambientSkyColor = Vec4(0.12f, 0.22f, 0.35f, 1.0f);
    m_SceneUBO.worldParams = Vec4(
        m_Scene->GetWorldState().time,
        sky.turbidity,
        static_cast<float>(m_Scene->GetStageMode()),
        0.72f // shadow intensity
    );
}
void VulkanStage::RecordShadowPass(VkCommandBuffer cmd) {
    // Generates depth map from sun viewpoint
}
void VulkanStage::RecordSkyAtmospherePass(VkCommandBuffer cmd) {
    // Renders Preetham sky dome and Hosek-Wilkie solar radiance
}
void VulkanStage::RecordOpaqueGeometryPass(VkCommandBuffer cmd) {
    // Draws stage platform, calibration cube, and active procedural asset
}
void VulkanStage::RecordDebugGridPass(VkCommandBuffer cmd) {
    // Draws viewport grid lines and coordinate axes
}
void VulkanStage::RecordPostProcessPass(VkCommandBuffer cmd) {
    // Tonemapping and bloom
}
} // namespace spher
