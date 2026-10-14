add_library(scene STATIC
    Camera.cpp
    CameraController.cpp
    Transform.cpp
    AABB.cpp
    Light.cpp
    Material.cpp
    MaterialRegistry.cpp
    Mesh.cpp
    Meshlet.cpp
    SkyModel.cpp
    SceneNode.cpp
    Scene.cpp
)
target_include_directories(scene PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../../base)
target_link_libraries(scene PUBLIC core vulkan_wrap)\n