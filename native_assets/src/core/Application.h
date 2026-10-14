#pragma once
#include "Base.h"
#include "Timestep.h"
#include <string>

namespace sphere {

struct ApplicationSpecification {
    std::string title = "Sphere Studio";
    uint32_t width = 1920;
    uint32_t height = 1080;
    bool enableValidation = true;
};

class Application {
public:
    explicit Application(const ApplicationSpecification& spec);
    virtual ~Application();

    void Run();
    void Shutdown();
    void Close() { m_Running = false; }

    static Application& Get() { return *s_Instance; }

private:
    ApplicationSpecification m_Spec;
    bool m_Running = true;
    static Application* s_Instance;
};
}\n