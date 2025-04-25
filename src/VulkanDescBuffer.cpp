#include "VulkanDescBuffer.h"

void VulkanDescBuffer::createBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = cpuDataSize;
	bufferInfo.usage = usageFlags;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	// create a buffer object
	(vkCreateBuffer(logicalDevice, &bufferInfo, NULL, &buffer));
	// allocate GPU memory
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, this->memoryPropertyFlags, physicalDevice);
	(vkAllocateMemory(logicalDevice, &allocInfo, NULL, &deviceMemory));
	// Descriptor for the actual buffer resource
	descriptorBufferInfo.buffer = buffer;
	descriptorBufferInfo.range = VK_WHOLE_SIZE;
	// bind GPU memory to a buffer object (Buffer <- GPU memory)
	(vkBindBufferMemory(logicalDevice, buffer, deviceMemory, 0));
}