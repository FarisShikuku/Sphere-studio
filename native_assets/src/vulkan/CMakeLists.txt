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
)
target_include_directories(vulkan_wrap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../../base)
target_link_libraries(vulkan_wrap PUBLIC base Vulkan::Vulkan)\n