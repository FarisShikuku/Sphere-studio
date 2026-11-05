add_library(vulkan_wrap STATIC
    Device.cpp
    Buffer.cpp
    Texture.cpp
    Pipeline.cpp
    PipelineBuilder.cpp
    Shader.cpp
    RenderPass.cpp
    BindlessHeap.cpp
    FrameGraph.cpp
    VulkanNativeSceneRenderer.cpp
)
target_include_directories(vulkan_wrap PUBLIC  /.. /../../base)
target_link_libraries(vulkan_wrap PUBLIC base Vulkan::Vulkan)
