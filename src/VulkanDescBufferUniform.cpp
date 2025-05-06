#include "VulkanDescBufferUniform.h"

VulkanDescBufferUniform::VulkanDescBufferUniform(void* cpuData, VkDeviceSize cpuDataSize,
    VkDevice logicalDevice, VkPhysicalDevice physicalDevice)
{

    this->cpuData = cpuData;
    this->cpuDataSize = cpuDataSize;
    this->usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    this->memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    
    uberSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uberSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    uberSetLayoutBinding.binding = this->numDescriptors;
    uberSetLayoutBinding.descriptorCount = 1; // we assume "1 binding = 1 descriptor"

    this->numDescriptors++;

    bool found = false;
    // Iterate through the descriptorPoolSizes to find the type
    for (auto& poolSize : descriptorPoolSizes) {
        if (poolSize.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            // Found the matching descriptor type, increment the descriptor count
            poolSize.descriptorCount++;
            found = true;
            break;  // No need to continue searching once we found and updated
        }
    }

    // If the descriptor type was not found, add a new entry
    if (!found) {
        descriptorPoolSizes.push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 });
    }
}

void VulkanDescBufferUniform::allocateUniformBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice)
{
    VulkanResourceHelper::createBuffer(logicalDevice, physicalDevice, cpuDataSize, usageFlags, memoryPropertyFlags, this->buffer, this->deviceMemory);
    // Descriptor for the actual buffer resource
    this->descriptorBufferInfo.buffer = this->buffer;
    this->descriptorBufferInfo.range = VK_WHOLE_SIZE;
    // Map persistent
    (vkMapMemory(logicalDevice, this->deviceMemory, 0, VK_WHOLE_SIZE, 0, &this->addrCPU_mapped_to_memoryGPU));
    update();
}

void VulkanDescBufferUniform::update()
{
    memcpy(this->addrCPU_mapped_to_memoryGPU, this->cpuData, this->cpuDataSize);
}
