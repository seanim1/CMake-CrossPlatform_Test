#pragma once
#include "VulkanDesc.h"

class VulkanDescBuffer : public VulkanDesc {
public:
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDescriptorBufferInfo descriptorBufferInfo{};

    VulkanDescBuffer() = default;
    ~VulkanDescBuffer() override = default;

protected:
};
