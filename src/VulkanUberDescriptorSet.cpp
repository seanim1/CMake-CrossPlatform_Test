#include "VulkanUberDescriptorSet.h"

VulkanUberDescriptorSet::VulkanUberDescriptorSet(VkDevice logicalDevice, std::vector<VulkanDesc*>& descriptorList)
{
	// createDescriptorSetLayout
	std::vector<VkDescriptorSetLayoutBinding> setlayoutBindings;
	for (int i = 0; i < descriptorList.size(); ++i) {
		VkDescriptorSetLayoutBinding layoutBinding = descriptorList[i]->uberSetLayoutBinding;
		setlayoutBindings.push_back(layoutBinding);
	}
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo; memset(&descriptorSetLayoutCreateInfo, 0, sizeof(VkDescriptorSetLayoutCreateInfo));
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pBindings = setlayoutBindings.data();
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(setlayoutBindings.size());
	vkCreateDescriptorSetLayout(logicalDevice, &descriptorSetLayoutCreateInfo, NULL, &uberSetLayout);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &uberSetLayout;
	if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &uberPipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	// createDescriptorPool
	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.maxSets = 1;
	descriptorPoolInfo.poolSizeCount = (uint32_t) VulkanDesc::descriptorPoolSizes.size();
	descriptorPoolInfo.pPoolSizes = VulkanDesc::descriptorPoolSizes.data();
	vkCreateDescriptorPool(logicalDevice, &descriptorPoolInfo, NULL, &descPool);

	// createDescriptorSets
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = descPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &uberSetLayout;
	vkAllocateDescriptorSets(logicalDevice, &descriptorSetAllocateInfo, &uberDescSet);

	for (int i = 0; i < descriptorList.size(); i++) {
		descriptorList[i]->writeDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorList[i]->writeDescSet.dstSet = uberDescSet;
		descriptorList[i]->writeDescSet.descriptorType = descriptorList[i]->uberSetLayoutBinding.descriptorType;
		descriptorList[i]->writeDescSet.dstBinding = descriptorList[i]->uberSetLayoutBinding.binding;
		descriptorList[i]->writeDescSet.descriptorCount = 1; // we assume "1 binding = 1 descriptor"
		descriptorList[i]->writeDescSet.pBufferInfo = &((VulkanDescBuffer*) descriptorList[i])->descriptorBufferInfo;
		vkUpdateDescriptorSets(logicalDevice, 1, &(descriptorList[i]->writeDescSet), 0, NULL);

	}
}
