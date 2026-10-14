add_library(shared STATIC
    GeneratorRegistry.cpp
    GeneratorContext.cpp
    GpuTypes.cpp
    Seed.cpp
    noise/NoiseLib.cpp
    math/SplineLib.cpp
    math/FrustumCull.cpp
    physics/PhysicsWorld.cpp
    memory/StreamingPool.cpp
    bindless/BindlessHeap.cpp
)
target_include_directories(shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../../base)
target_link_libraries(shared PUBLIC core vulkan_wrap scene)\n