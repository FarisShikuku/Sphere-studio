#include "BindlessHeap.h"
#include "DescriptorIndexing.h"
#include "core/Log.h"

namespace sphere {

BindlessHeap& BindlessHeap::Get() {
    static BindlessHeap instance;
    return instance;
}

void BindlessHeap::Initialize(VkDevice device, uint32_t maxTextures) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Device = device;
    m_MaxTextures = maxTextures;

    m_Layout = DescriptorIndexing::CreateBindlessSetLayout(device, maxTextures, 256);

    VkDescriptorPoolSize poolSizes[] = {
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxTextures + 256 }
    };

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    poolInfo.maxSets = 1;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = poolSizes;

    vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_Pool);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_Pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_Layout;

    vkAllocateDescriptorSets(device, &allocInfo, &m_DescriptorSet);
    SPHERE_LOG_INFO("BindlessHeap initialized with capacity: " + std::to_string(maxTextures));
}

void BindlessHeap::Shutdown() {
    std::lock_guard<std::mutex> lock(m_Mutex);
    if (m_Device != VK_NULL_HANDLE) {
        if (m_Pool != VK_NULL_HANDLE) vkDestroyDescriptorPool(m_Device, m_Pool, nullptr);
        if (m_Layout != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(m_Device, m_Layout, nullptr);
        m_Pool = VK_NULL_HANDLE;
        m_Layout = VK_NULL_HANDLE;
        m_DescriptorSet = VK_NULL_HANDLE;
        m_Device = VK_NULL_HANDLE;
    }
}

uint32_t BindlessHeap::RegisterTexture(VkImageView imageView, VkSampler sampler) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    uint32_t slot = 0;
    if (!m_FreeSlots.empty()) {
        slot = m_FreeSlots.back();
        m_FreeSlots.pop_back();
    } else {
        slot = m_NextSlot++;
    }

    if (m_Device != VK_NULL_HANDLE && m_DescriptorSet != VK_NULL_HANDLE) {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = sampler;
        imageInfo.imageView = imageView;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_DescriptorSet;
        write.dstBinding = 0;
        write.dstArrayElement = slot;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = 1;
        write.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(m_Device, 1, &write, 0, nullptr);
    }
    return slot;
}

void BindlessHeap::UnregisterTexture(uint32_t index) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    if (index > 0) {
        m_FreeSlots.push_back(index);
    }
}

} // namespace sphere
