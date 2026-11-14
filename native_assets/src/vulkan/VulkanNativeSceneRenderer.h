#pragma once
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <string>
#include "base/VulkanDevice.hpp"
#include "base/VulkanSwapChain.hpp"

#if defined(__ANDROID__)
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <vulkan/vulkan_android.h>
#endif

namespace sphere::vk {

/**
 * VulkanNativeSceneRenderer
 * 
 * Direct native C++ scene renderer inspired by Sascha Willems' Vulkan Android samples.
 * Connects directly to an Android Surface via ANativeWindow and VK_KHR_android_surface,
 * bypassing Java/Kotlin 2D Canvas completely to render directly on the GPU at native display refresh rates.
 */
struct NativeRenderParams {
    float lightPosX = 0.0f;
    float lightPosY = 2.5f;
    float lightPosZ = 1.8f;
    float lightIntensity = 2500.0f;
    float lightColorR = 1.0f;
    float lightColorG = 0.95f;
    float lightColorB = 0.88f;
    float roughness = 0.45f;
    float metallic = 0.0f;
    float objectWidth = 2.0f;
    float objectHeight = 2.0f;
    float objectDepth = 2.0f;
    uint32_t stageMode = 0; // 0 = PBR Calibration, 1 = Wall, 2 = Floor, 3 = Post
};

class VulkanNativeSceneRenderer {
public:
    VulkanNativeSceneRenderer();
    ~VulkanNativeSceneRenderer();

    static VulkanNativeSceneRenderer& Get();

    /**
     * Initializes Vulkan Instance, selects Physical Device, creates ANativeWindow surface
     * via vkCreateAndroidSurfaceKHR (Sascha Willems WSI pattern), and sets up SwapChain.
     */
    bool InitializeSurface(void* nativeWindowHandle, uint32_t width, uint32_t height);

    /**
     * Recreates SwapChain when Android SurfaceView or orientation changes.
     */
    void Resize(uint32_t width, uint32_t height);

    /**
     * Executes the GPU render pass: acquires swapchain image, binds PBR pipeline,
     * updates push constants / UBOs, records commands, and presents to ANativeWindow.
     */
    void RenderFrame(const NativeRenderParams& params);

    /**
     * Cleans up Vulkan swapchain, device, and releases ANativeWindow reference.
     */
    void DestroySurface();

    static bool IsGpuNodeAccessible();
    bool IsInitialized() const { return m_Initialized; }
    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }

private:
    bool CreateInstance();
    bool SelectPhysicalDevice();
    bool CreateLogicalDevice();
    bool CreateSurface(void* nativeWindowHandle);
    bool CreateSwapChain(uint32_t width, uint32_t height);
    bool CreateRenderPass();
    bool CreateFramebuffers();
    bool CreateCommandBuffers();
    bool CreateSyncObjects();

    bool m_Initialized = false;
    bool m_SoftwareWindowMode = false;
    uint32_t m_Width = 1080;
    uint32_t m_Height = 1920;

    VkInstance m_Instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_LogicalDevice = VK_NULL_HANDLE;
    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    VkQueue m_PresentQueue = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
    VkRenderPass m_RenderPass = VK_NULL_HANDLE;
    VkCommandPool m_CommandPool = VK_NULL_HANDLE;

#if defined(__ANDROID__)
    ANativeWindow* m_NativeWindow = nullptr;
#endif

    std::vector<VkImage> m_SwapChainImages;
    std::vector<VkImageView> m_SwapChainImageViews;
    std::vector<VkFramebuffer> m_Framebuffers;
    std::vector<VkCommandBuffer> m_CommandBuffers;

    VkSemaphore m_ImageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore m_RenderFinishedSemaphore = VK_NULL_HANDLE;
    VkFence m_InFlightFence = VK_NULL_HANDLE;

    NativeRenderParams m_CurrentParams{};
};

} // namespace sphere::vk
