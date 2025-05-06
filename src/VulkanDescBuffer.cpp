#include "VulkanDescBuffer.h"
VulkanDescBuffer::VulkanDescBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice,
    const void* cpuData, VkDeviceSize cpuDataSize,
    VkBufferUsageFlags usageFlags, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags)
{
    this->cpuData = cpuData;
    this->cpuDataSize = cpuDataSize;
    this->usageFlags = usageFlags | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    this->memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    uberSetLayoutBinding.descriptorType = descriptorType;
    uberSetLayoutBinding.stageFlags = stageFlags;
    uberSetLayoutBinding.binding = this->numDescriptors;
    uberSetLayoutBinding.descriptorCount = 1; // we assume "1 binding = 1 descriptor"

    this->numDescriptors++;

    bool found = false;
    // Iterate through the descriptorPoolSizes to find the type
    for (auto& poolSize : descriptorPoolSizes) {
        if (poolSize.type == descriptorType) {
            // Found the matching descriptor type, increment the descriptor count
            poolSize.descriptorCount++;
            found = true;
            break;  // No need to continue searching once we found and updated
        }
    }

    // If the descriptor type was not found, add a new entry
    if (!found) {
        descriptorPoolSizes.push_back({ descriptorType, 1 });
    }

}

void VulkanDescBuffer::allocate(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, 
    VkCommandPool commandPool, VkQueue graphicsQueue)
{
    // Staging
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VulkanResourceHelper::createBuffer(logicalDevice, physicalDevice, this->cpuDataSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer, stagingBufferMemory);

    void* addrCPU_mapped_to_memoryGPU_staged;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, this->cpuDataSize, 0, &addrCPU_mapped_to_memoryGPU_staged);
    memcpy(addrCPU_mapped_to_memoryGPU_staged, cpuData, this->cpuDataSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    VulkanResourceHelper::createBuffer(logicalDevice, physicalDevice, this->cpuDataSize,
        this->usageFlags, this->memoryPropertyFlags,
        this->buffer, this->deviceMemory);

    VulkanResourceHelper::copyBuffer(stagingBuffer, this->buffer, this->cpuDataSize,
        logicalDevice, commandPool, graphicsQueue);

    // Descriptor for the actual buffer resource
    this->descriptorBufferInfo.buffer = this->buffer;
    this->descriptorBufferInfo.range = VK_WHOLE_SIZE;

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}
