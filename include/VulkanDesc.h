#pragma once

#include <vulkan/vulkan.h>
#include <cstring>
#include <vector>

class VulkanDesc {
public:

    virtual ~VulkanDesc() = default;
    VulkanDesc()
        : cpuData(nullptr),
        cpuDataSize(0),
        usageFlags(0),
        memoryPropertyFlags(0),
        deviceMemory(VK_NULL_HANDLE),
        addrCPU_mapped_to_memoryGPU(nullptr)
    {
        memset(&uberSetLayoutBinding, 0, sizeof(uberSetLayoutBinding));
        memset(&writeDescSet, 0, sizeof(writeDescSet));
    }
    static int numDescriptors;  // Static variable to keep track of instances
    static std::vector<VkDescriptorPoolSize> descriptorPoolSizes;

    // The data common to both Buffer and Image (should be initalized before allocating device memory)
    void* cpuData; // CPU data
    VkDeviceSize cpuDataSize; // Data size
    VkBufferUsageFlags usageFlags;
    VkMemoryPropertyFlags memoryPropertyFlags;

    VkDescriptorSetLayoutBinding uberSetLayoutBinding; // we specify a binding for the uber set in a shader

    // Common members for memory management to both Buffer and Image
    // > Allocated when the resource is created
    VkDeviceMemory deviceMemory;
    // > Mapping memory to descriptor
    VkWriteDescriptorSet writeDescSet; // per-binding
    // updated every frame for Uniform
    void* addrCPU_mapped_to_memoryGPU; // host virtual address pointer to a region of a mappable memory

protected:
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice);
};
