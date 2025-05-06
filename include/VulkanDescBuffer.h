#pragma once
#include "VulkanDesc.h"

class VulkanDescBuffer : public VulkanDesc {
public:
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDescriptorBufferInfo descriptorBufferInfo{};

    VulkanDescBuffer() = default;
    VulkanDescBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice,
        const void* cpuData, VkDeviceSize cpuDataSize,
        VkBufferUsageFlags usageFlags, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags);
    ~VulkanDescBuffer() override = default;

    void allocate(VkDevice logicalDevice, VkPhysicalDevice physicalDevice,
        VkCommandPool commandPool, VkQueue graphicsQueue);

};
