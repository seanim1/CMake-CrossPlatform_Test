#include "VulkanDesc.h"

// Initialize the static variable to 0 (no descriptors created initially)
int VulkanDesc::numDescriptors = 0;
std::vector<VkDescriptorPoolSize> VulkanDesc::descriptorPoolSizes;
