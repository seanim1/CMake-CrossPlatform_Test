#pragma once

#include "VulkanDescBuffer.h"

class VulkanDescBufferUniform : public VulkanDescBuffer {
public:
    VulkanDescBufferUniform(void* cpuData, VkDeviceSize cpuDataSize);
    ~VulkanDescBufferUniform() override = default;

    void allocateUniformBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice);
    void update();

};
