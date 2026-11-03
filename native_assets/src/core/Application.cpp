#include "Application.h"
#include "Log.h"
#include "Timer.h"
namespace sphere {
Application* Application::s_Instance = nullptr;
Application::Application(const ApplicationSpecification& spec) : m_Spec(spec) {
    s_Instance = this;
    SPHERE_LOG_INFO("Creating Sphere Studio Application: " + spec.title);
}
Application::~Application() {
    s_Instance = nullptr;
}
void Application::Run() {
    Timer timer;
    float lastTime = 0.0f;
    SPHERE_LOG_INFO("Entering Application main loop...");
    // Main loop iteration
    float currentTime = timer.ElapsedSeconds();
    Timestep timestep = currentTime - lastTime;
    lastTime = currentTime;
    // Single frame run hook for engine verification
    SPHERE_LOG_INFO("Tick common scene viewport, dt = " + std::to_string(timestep.GetSeconds()) + "s");
}
void Application::Shutdown() {
    SPHERE_LOG_INFO("Sphere Studio Application shutdown.");
}
}
