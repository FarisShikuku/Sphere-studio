add_library(sky STATIC
    atmosphere/PreethamModel.cpp
    sun/SunDisk.cpp
)
target_include_directories(sky PUBLIC
    
    /..
    /../../base
    /atmosphere
    /sun
)
target_link_libraries(sky PUBLIC core scene shared vulkan_wrap)
