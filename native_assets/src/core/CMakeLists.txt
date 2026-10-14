add_library(core STATIC
    Application.cpp
    Log.cpp
    Profile.cpp
)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)\n