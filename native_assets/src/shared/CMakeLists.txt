add_library(shared STATIC
    GeneratorRegistry.cpp
    GeneratorContext.cpp
    GpuTypes.cpp
    Seed.cpp
    bindless/BindlessHeap.cpp
    bindless/DescriptorIndexing.cpp
    pipelines/PipelineBuilder.cpp
    pipelines/PipelineCache.cpp
    shaders/ShaderCache.cpp
    memory/DeviceMemoryAllocator.cpp
    memory/StreamingPool.cpp
    materials/MaterialDefs.cpp
    materials/MaterialRegistry.cpp
    materials/MaterialLibrary.cpp
    math/FrustumCull.cpp
    math/HiZ.cpp
    math/HizBuild.cpp
    math/HizCull.cpp
    math/SplineLib.cpp
    noise/NoiseLib.cpp
    noise/NoiseBlue.cpp
    noise/NoiseCurl.cpp
    noise/NoiseSimplex.cpp
    noise/NoiseWorley.cpp
    physics/CollisionGroups.cpp
    physics/PhysicsWorld.cpp
    physics/PhysicsDebug.cpp
    debug/DebugDraw.cpp
    debug/DebugText.cpp
    skeletons/SkeletonGen.cpp
    skeletons/RigShared.cpp
)
target_include_directories(shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../../base)
target_link_libraries(shared PUBLIC core vulkan_wrap scene)
\n