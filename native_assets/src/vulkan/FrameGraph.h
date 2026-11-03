#pragma once
#include <vector>
#include <string>
#include <functional>
#include <vulkan/vulkan.h>
namespace sphere::vk {
class FrameGraph {
public:
    void AddPass(const std::string& name, std::function<void(VkCommandBuffer)> execute) {
        m_Passes.push_back({name, execute});
    }
    void Execute(VkCommandBuffer cmd) {
        for (auto& pass : m_Passes) {
            pass.execute(cmd);
        }
    }
private:
    struct Pass {
        std::string name;
        std::function<void(VkCommandBuffer)> execute;
    };
    std::vector<Pass> m_Passes;
};
}
