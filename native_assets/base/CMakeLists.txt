add_library(base STATIC
    VulkanBuffer.cpp
    VulkanDevice.cpp
    VulkanTexture.cpp
    VulkanTools.cpp
    VulkanSwapChain.cpp
    VulkanDebug.cpp
)
target_include_directories(base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(base PUBLIC Vulkan::Vulkan)\n