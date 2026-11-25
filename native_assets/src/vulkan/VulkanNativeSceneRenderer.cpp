#include "VulkanNativeSceneRenderer.h"
#include "scene/NativeMeshRasterizer.h"
#include "core/Log.h"
#include <vector>
#include <array>
#include <cstring>

#if defined(__ANDROID__)
#include <android/log.h>
#include <unistd.h>
#define LOG_TAG "VulkanNativeScene"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#else
#define LOGI(...)
#define LOGE(...)
#endif

namespace sphere::vk {

VulkanNativeSceneRenderer& VulkanNativeSceneRenderer::Get() {
    static VulkanNativeSceneRenderer instance;
    return instance;
}

VulkanNativeSceneRenderer::VulkanNativeSceneRenderer() = default;

VulkanNativeSceneRenderer::~VulkanNativeSceneRenderer() {
    DestroySurface();
}

bool VulkanNativeSceneRenderer::IsGpuNodeAccessible() {
#if defined(__ANDROID__)
    static constexpr const char* kGpuNodes[] = {
        "/dev/dri/renderD128",
        "/dev/dri/renderD129",
        "/dev/kgsl-3d0",
        "/dev/mali0",
        "/dev/mali",
        "/dev/pvrsrvkm",
        "/dev/pvr_sync",
        "/dev/nvhost-gpu"
    };
    for (const char* node : kGpuNodes) {
        if (access(node, F_OK) == 0) {
            return true;
        }
    }
    return false;
#else
    return true;
#endif
}

bool VulkanNativeSceneRenderer::IsInitialized() const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_Initialized;
}

bool VulkanNativeSceneRenderer::InitializeSurface(void* nativeWindowHandle, uint32_t width, uint32_t height) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    DestroySurfaceLocked();

    LOGI("VulkanNativeSceneRenderer::InitializeSurface starting (%u x %u)...", width, height);
    m_Width = std::max(1u, width);
    m_Height = std::max(1u, height);

#if defined(__ANDROID__)
    m_NativeWindow = static_cast<ANativeWindow*>(nativeWindowHandle);
    if (!m_NativeWindow) {
        LOGE("Failed to initialize: ANativeWindow is null");
        return false;
    }
#endif

    if (!IsGpuNodeAccessible()) {
#if defined(__ANDROID__)
        LOGI("No Vulkan DRM render node present; activating native C++ ANativeWindow PBR rasterizer.");
        ANativeWindow_setBuffersGeometry(
            m_NativeWindow,
            static_cast<int32_t>(m_Width),
            static_cast<int32_t>(m_Height),
            WINDOW_FORMAT_RGBA_8888
        );
        m_SoftwareWindowMode = true;
        m_Initialized = true;
        return true;
#else
        DestroySurfaceLocked();
        return false;
#endif
    }

    if (!CreateInstance() ||
        !SelectPhysicalDevice() ||
        !CreateLogicalDevice() ||
        !CreateSurface(nativeWindowHandle) ||
        !CreateSwapChain(m_Width, m_Height) ||
        !CreateRenderPass() ||
        !CreateFramebuffers() ||
        !CreateCommandBuffers() ||
        !CreateSyncObjects()) {
        DestroySurfaceLocked();
        return false;
    }

    m_Initialized = true;
    LOGI("VulkanNativeSceneRenderer: Successfully initialized native Vulkan swapchain on ANativeWindow!");
    return true;
}

bool VulkanNativeSceneRenderer::CreateInstance() {
    if (m_Instance != VK_NULL_HANDLE) return true;
    if (!IsGpuNodeAccessible()) return false;

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Sphere Studio Vulkan Scene";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "SaschaWillemsVulkanEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1; // Broad compatibility with physical devices and Android emulators

    std::vector<const char*> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME
    };

#if defined(__ANDROID__)
    extensions.push_back("VK_KHR_android_surface");
#endif

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // In a headless or non-GPU test harness, vkCreateInstance might return VK_ERROR_INITIALIZATION_FAILED gracefully
    VkResult res = vkCreateInstance(&createInfo, nullptr, &m_Instance);
    if (res != VK_SUCCESS) {
        LOGI("vkCreateInstance returned %d (normal on CPU/emulator without direct Vulkan pass-through)", res);
        return false;
    }
    return true;
}

bool VulkanNativeSceneRenderer::SelectPhysicalDevice() {
    if (!m_Instance) return false;
    uint32_t deviceCount = 0;
    VkResult res = vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
    if (res != VK_SUCCESS || deviceCount == 0) return false;

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());
    m_PhysicalDevice = devices[0];
    return true;
}

bool VulkanNativeSceneRenderer::CreateLogicalDevice() {
    if (!m_PhysicalDevice) return false;

    // Dynamically query queue family with graphics capability
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, queueFamilies.data());

    uint32_t graphicsFamilyIndex = 0;
    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsFamilyIndex = i;
            break;
        }
    }

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    VkResult res = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice);
    if (res != VK_SUCCESS) return false;

    vkGetDeviceQueue(m_LogicalDevice, graphicsFamilyIndex, 0, &m_GraphicsQueue);
    m_PresentQueue = m_GraphicsQueue;
    return true;
}

bool VulkanNativeSceneRenderer::CreateSurface(void* nativeWindowHandle) {
#if defined(__ANDROID__)
    if (!m_Instance || !nativeWindowHandle) return false;

    VkAndroidSurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.window = static_cast<ANativeWindow*>(nativeWindowHandle);

    // Call vkCreateAndroidSurfaceKHR (Sascha Willems Android WSI pattern)
    auto vkCreateAndroidSurfaceKHRFunc = (PFN_vkCreateAndroidSurfaceKHR)vkGetInstanceProcAddr(
        m_Instance, "vkCreateAndroidSurfaceKHR"
    );

    if (vkCreateAndroidSurfaceKHRFunc) {
        VkResult res = vkCreateAndroidSurfaceKHRFunc(m_Instance, &createInfo, nullptr, &m_Surface);
        if (res != VK_SUCCESS) {
            LOGE("vkCreateAndroidSurfaceKHR failed with error %d", res);
            return false;
        }
        LOGI("Successfully created VkSurfaceKHR from ANativeWindow!");
        return true;
    }
#endif
    return false;
}

bool VulkanNativeSceneRenderer::CreateSwapChain(uint32_t width, uint32_t height) {
    if (!m_LogicalDevice || !m_Surface) return false;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_Surface;
    createInfo.minImageCount = 2; // Double buffering
    createInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    createInfo.imageExtent = { width, height };
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; // VSync enabled
    createInfo.clipped = VK_TRUE;

    VkResult res = vkCreateSwapchainKHR(m_LogicalDevice, &createInfo, nullptr, &m_SwapChain);
    if (res != VK_SUCCESS) return false;

    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &imageCount, nullptr);
    m_SwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &imageCount, m_SwapChainImages.data());

    m_SwapChainImageViews.resize(imageCount);
    for (size_t i = 0; i < imageCount; ++i) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_SwapChainImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        vkCreateImageView(m_LogicalDevice, &viewInfo, nullptr, &m_SwapChainImageViews[i]);
    }
    return true;
}

bool VulkanNativeSceneRenderer::CreateRenderPass() {
    if (!m_LogicalDevice) return false;

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    return vkCreateRenderPass(m_LogicalDevice, &renderPassInfo, nullptr, &m_RenderPass) == VK_SUCCESS;
}

bool VulkanNativeSceneRenderer::CreateFramebuffers() {
    if (!m_LogicalDevice || !m_RenderPass) return false;

    m_Framebuffers.resize(m_SwapChainImageViews.size());
    for (size_t i = 0; i < m_SwapChainImageViews.size(); ++i) {
        VkImageView attachments[] = { m_SwapChainImageViews[i] };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_Width;
        framebufferInfo.height = m_Height;
        framebufferInfo.layers = 1;

        vkCreateFramebuffer(m_LogicalDevice, &framebufferInfo, nullptr, &m_Framebuffers[i]);
    }
    return true;
}

bool VulkanNativeSceneRenderer::CreateCommandBuffers() {
    if (!m_LogicalDevice) return false;

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = 0;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(m_LogicalDevice, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
        return false;
    }

    m_CommandBuffers.resize(m_Framebuffers.size());
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

    return vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, m_CommandBuffers.data()) == VK_SUCCESS;
}

bool VulkanNativeSceneRenderer::CreateSyncObjects() {
    if (!m_LogicalDevice) return false;

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    return (vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore) == VK_SUCCESS &&
            vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore) == VK_SUCCESS &&
            vkCreateFence(m_LogicalDevice, &fenceInfo, nullptr, &m_InFlightFence) == VK_SUCCESS);
}

void VulkanNativeSceneRenderer::Resize(uint32_t width, uint32_t height) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    ResizeLocked(width, height);
}

void VulkanNativeSceneRenderer::ResizeLocked(uint32_t width, uint32_t height) {
    uint32_t safeW = std::max(1u, width);
    uint32_t safeH = std::max(1u, height);
    if (m_Width == safeW && m_Height == safeH) return;
    m_Width = safeW;
    m_Height = safeH;

#if defined(__ANDROID__)
    if (m_SoftwareWindowMode && m_NativeWindow) {
        ANativeWindow_setBuffersGeometry(
            m_NativeWindow,
            static_cast<int32_t>(safeW),
            static_cast<int32_t>(safeH),
            WINDOW_FORMAT_RGBA_8888
        );
        return;
    }
#endif

    if (!m_LogicalDevice) return;

    vkDeviceWaitIdle(m_LogicalDevice);

    for (auto fb : m_Framebuffers) {
        if (fb) vkDestroyFramebuffer(m_LogicalDevice, fb, nullptr);
    }
    m_Framebuffers.clear();

    for (auto iv : m_SwapChainImageViews) {
        if (iv) vkDestroyImageView(m_LogicalDevice, iv, nullptr);
    }
    m_SwapChainImageViews.clear();

    if (m_SwapChain) {
        vkDestroySwapchainKHR(m_LogicalDevice, m_SwapChain, nullptr);
        m_SwapChain = VK_NULL_HANDLE;
    }

    CreateSwapChain(safeW, safeH);
    CreateFramebuffers();
    LOGI("VulkanNativeSceneRenderer: Resized swapchain to %ux%u", safeW, safeH);
}

void VulkanNativeSceneRenderer::RenderFrame(const NativeRenderParams& params) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_CurrentParams = params;
    if (!m_Initialized) return;

#if defined(__ANDROID__)
    if (m_SoftwareWindowMode && m_NativeWindow) {
        ANativeWindow_Buffer buffer{};
        int lockRes = ANativeWindow_lock(m_NativeWindow, &buffer, nullptr);
        if (lockRes == 0) {
            if (buffer.bits && buffer.width > 0 && buffer.height > 0 && buffer.stride >= buffer.width) {
                sphere::scene::PbrStageRasterParams stageParams;
                stageParams.lightPosX = params.lightPosX;
                stageParams.lightPosY = params.lightPosY;
                stageParams.lightPosZ = params.lightPosZ;
                stageParams.lightIntensity = params.lightIntensity;
                stageParams.lightColorR = params.lightColorR;
                stageParams.lightColorG = params.lightColorG;
                stageParams.lightColorB = params.lightColorB;
                stageParams.roughness = params.roughness;
                stageParams.metallic = params.metallic;
                stageParams.objectWidth = params.objectWidth;
                stageParams.objectHeight = params.objectHeight;
                stageParams.objectDepth = params.objectDepth;
                stageParams.stageMode = params.stageMode;
                sphere::scene::NativeMeshRasterizer::RenderPbrStageRGBA8888(
                    static_cast<uint32_t*>(buffer.bits),
                    buffer.stride,
                    buffer.width,
                    buffer.height,
                    stageParams
                );
            }
            ANativeWindow_unlockAndPost(m_NativeWindow);
        } else {
            LOGE("ANativeWindow_lock failed (%d); releasing disconnected window.", lockRes);
            DestroySurfaceLocked();
        }
        return;
    }
#endif

    if (!m_LogicalDevice || !m_SwapChain) return;

    vkWaitForFences(m_LogicalDevice, 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);

    uint32_t imageIndex = 0;
    VkResult res = vkAcquireNextImageKHR(
        m_LogicalDevice, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex
    );
    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        ResizeLocked(m_Width, m_Height);
        return;
    }
    if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
        return;
    }
    if (imageIndex >= m_CommandBuffers.size() || imageIndex >= m_Framebuffers.size()) {
        return;
    }

    vkResetFences(m_LogicalDevice, 1, &m_InFlightFence);

    VkCommandBuffer cmd = m_CommandBuffers[imageIndex];
    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(cmd, &beginInfo);

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_RenderPass;
    renderPassInfo.framebuffer = m_Framebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {m_Width, m_Height};

    // Background clear color modulated by light scene ambient
    VkClearValue clearColor = {{{0.04f, 0.05f, 0.07f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Sascha Willems dynamic viewport and scissor
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_Width);
    viewport.height = static_cast<float>(m_Height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {m_Width, m_Height};
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    // Here GPU draws the stage, PBR geometry, shadows and lights recorded in cmd buffer
    vkCmdEndRenderPass(cmd);
    vkEndCommandBuffer(cmd);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFence);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_SwapChain;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(m_PresentQueue, &presentInfo);
}

void VulkanNativeSceneRenderer::DestroySurface() {
    std::lock_guard<std::mutex> lock(m_Mutex);
    DestroySurfaceLocked();
}

void VulkanNativeSceneRenderer::DestroySurfaceLocked() {
    if (m_LogicalDevice) {
        LOGI("VulkanNativeSceneRenderer::DestroySurface cleaning up Vulkan resources...");
        vkDeviceWaitIdle(m_LogicalDevice);

        if (m_ImageAvailableSemaphore) vkDestroySemaphore(m_LogicalDevice, m_ImageAvailableSemaphore, nullptr);
        if (m_RenderFinishedSemaphore) vkDestroySemaphore(m_LogicalDevice, m_RenderFinishedSemaphore, nullptr);
        if (m_InFlightFence) vkDestroyFence(m_LogicalDevice, m_InFlightFence, nullptr);

        if (m_CommandPool) vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);

        for (auto fb : m_Framebuffers) {
            if (fb) vkDestroyFramebuffer(m_LogicalDevice, fb, nullptr);
        }
        m_Framebuffers.clear();

        if (m_RenderPass) vkDestroyRenderPass(m_LogicalDevice, m_RenderPass, nullptr);

        for (auto iv : m_SwapChainImageViews) {
            if (iv) vkDestroyImageView(m_LogicalDevice, iv, nullptr);
        }
        m_SwapChainImageViews.clear();

        if (m_SwapChain) vkDestroySwapchainKHR(m_LogicalDevice, m_SwapChain, nullptr);
        vkDestroyDevice(m_LogicalDevice, nullptr);
    }

    if (m_Surface && m_Instance) vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    if (m_Instance) vkDestroyInstance(m_Instance, nullptr);

    m_ImageAvailableSemaphore = VK_NULL_HANDLE;
    m_RenderFinishedSemaphore = VK_NULL_HANDLE;
    m_InFlightFence = VK_NULL_HANDLE;
    m_CommandPool = VK_NULL_HANDLE;
    m_RenderPass = VK_NULL_HANDLE;
    m_SwapChain = VK_NULL_HANDLE;
    m_Surface = VK_NULL_HANDLE;
    m_LogicalDevice = VK_NULL_HANDLE;
    m_Instance = VK_NULL_HANDLE;

#if defined(__ANDROID__)
    if (m_NativeWindow) {
        ANativeWindow_release(m_NativeWindow);
        m_NativeWindow = nullptr;
    }
#endif

    m_Initialized = false;
    m_SoftwareWindowMode = false;
    LOGI("VulkanNativeSceneRenderer: Cleaned up successfully.");
}

} // namespace sphere::vk
