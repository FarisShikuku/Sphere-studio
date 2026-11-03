#include "core/Application.h"
#include "core/Log.h"
#include "scene/Scene.h"
#include "shared/GeneratorRegistry.h"
#include <iostream>
int main(int argc, char** argv) {
    sphere::Log::Init();
    SPHERE_LOG_INFO("Sphere Studio Engine - High Performance Vulkan 1.3 Procedural Core Initialized");
    sphere::ApplicationSpecification spec;
    spec.title = "Sphere Studio - Common Scene Viewport";
    spec.width = 1920;
    spec.height = 1080;
    spec.enableValidation = true;
    sphere::Application app(spec);
    
    // Register procedural generators
    sphere::GeneratorRegistry::Get().Initialize();
    
    app.Run();
    app.Shutdown();
    return 0;
}
