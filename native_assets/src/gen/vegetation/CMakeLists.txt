cmake_minimum_required(VERSION 3.22.1)
project(vegetation_system LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

add_library(vegetation_system STATIC
    common/VegetationMath.cpp
    trees/LSystemTree.cpp
    trees/TreeGrammar.cpp
    trees/TreeLOD.cpp
    trees/TreeImpostor.cpp
    trees/tree_types/OakTreeType.cpp
    trees/tree_types/PineTreeType.cpp
    trees/tree_types/BirchTreeType.cpp
    trees/tree_types/WillowTreeType.cpp
    trees/tree_types/MapleTreeType.cpp
    trees/tree_types/AcaciaTreeType.cpp
    trees/tree_types/SpruceTreeType.cpp
    trees/tree_types/categories/BotanicalCrownHulls.cpp
    trees/tree_types/categories/PalmAndMonocotBuilder.cpp
    trees/tree_types/categories/SucculentAndBaobabBuilder.cpp
    trees/tree_types/categories/SavannaAndBanyanBuilder.cpp
    trees/tree_types/categories/ConiferAndSpireBuilder.cpp
    trees/tree_types/categories/BroadleafOrchardAndVineBuilder.cpp
    trees/tree_types/BotanicalSpeciesTreeType.cpp
    trees/tree_types/TreeTypeRegistry.cpp
    bushes/BushGenerator.cpp
    grass/GrassBladeGen.cpp
    grass/GrassSim.cpp
    grass/GrassScatter.cpp
    flowers/FlowerGen.cpp
    crops/CropRowGen.cpp
    rocks/RockGenerator.cpp
    rocks/RockErosion.cpp
    debris/DebrisGen.cpp
    biomes/BiomeRules.cpp
    biomes/BiomeTemperate.cpp
    biomes/BiomeDesert.cpp
    biomes/BiomeForest.cpp
    biomes/BiomeTundra.cpp
    wind/WindField.cpp
    forest/Forest3DSystem.cpp
    forest/ForestCanopyGenerator.cpp
    forest/ForestEcosystemEngine.cpp
    grass/ForestGrassGenerator.cpp
    grass/RealTimeGrassEngine.cpp
    scene/Vegetation3DScene.cpp
    scene/BotanicalLeafRasterizer.cpp
    scene/BotanicalFruitAndBloomRasterizer.cpp
    scene/Botanical3DSceneRenderer.cpp
    desert/DesertCactusGenerator.cpp
    desert/Desert3DScene.cpp
    helios/HeliosCanopyEngine.cpp
    helios/HeliosPlantArchitecture.cpp
    helios/HeliosVegetationScene.cpp
    VegetationSystem.cpp
    vegetation_c_api.cpp
)

target_include_directories(vegetation_system PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/trees
    ${CMAKE_CURRENT_SOURCE_DIR}/trees/tree_types
    ${CMAKE_CURRENT_SOURCE_DIR}/trees/tree_types/categories
    ${CMAKE_CURRENT_SOURCE_DIR}/bushes
    ${CMAKE_CURRENT_SOURCE_DIR}/grass
    ${CMAKE_CURRENT_SOURCE_DIR}/forest
    ${CMAKE_CURRENT_SOURCE_DIR}/scene
    ${CMAKE_CURRENT_SOURCE_DIR}/desert
    ${CMAKE_CURRENT_SOURCE_DIR}/helios
    ${CMAKE_CURRENT_SOURCE_DIR}/flowers
    ${CMAKE_CURRENT_SOURCE_DIR}/crops
    ${CMAKE_CURRENT_SOURCE_DIR}/rocks
    ${CMAKE_CURRENT_SOURCE_DIR}/debris
    ${CMAKE_CURRENT_SOURCE_DIR}/biomes
    ${CMAKE_CURRENT_SOURCE_DIR}/wind
    ${CMAKE_CURRENT_SOURCE_DIR}/textures/spec
    ${CMAKE_CURRENT_SOURCE_DIR}/meshes/spec
    ${CMAKE_CURRENT_SOURCE_DIR}/meshes/profiles
)
