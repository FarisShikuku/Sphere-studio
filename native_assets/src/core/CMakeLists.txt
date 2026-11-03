add_library(core STATIC
    Application.cpp
    Log.cpp
    Profile.cpp
)
target_include_directories(core PUBLIC  /..)
