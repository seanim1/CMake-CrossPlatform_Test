#include "VulkanCommand.h"

VulkanCommand::VulkanCommand(VkDevice logicalDevice, uint32_t queueFamilyIndex)
{
	VkCommandPoolCreateInfo cmdPoolInfo{};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = 0;
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	CATCH_ERROR(vkCreateCommandPool(logicalDevice, &cmdPoolInfo, NULL, &cmdPool));

	VkCommandBufferAllocateInfo commandBufferAllocateInfo; memset(&commandBufferAllocateInfo, 0, sizeof(VkCommandBufferAllocateInfo));
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = cmdPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = PRESENT_IMG_COUNT * CMDBUF_COUNT;
	// Command buffer for each frame
	CATCH_ERROR(vkAllocateCommandBuffers(logicalDevice, &commandBufferAllocateInfo, (VkCommandBuffer*)frameCmdBuffers));
}
static void init_vkDependencyInfo(VkDependencyInfo* dependencyInfoAddress) {
	dependencyInfoAddress->sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependencyInfoAddress->dependencyFlags = 0;
	dependencyInfoAddress->memoryBarrierCount = 0;
	dependencyInfoAddress->pMemoryBarriers = NULL;
	dependencyInfoAddress->bufferMemoryBarrierCount = 0;
	dependencyInfoAddress->pBufferMemoryBarriers = NULL;
	dependencyInfoAddress->imageMemoryBarrierCount = 0;
	dependencyInfoAddress->pImageMemoryBarriers = NULL;
}
void VulkanCommand::buildCommandBuffers(VulkanSwapChain* swapChainX, 
	VkPipelineLayout uberPipelineLayout, VkDescriptorSet uberDescSet,
	VkPipeline graphicsPipeline01, Geometry* geometry)
{
	VkDependencyInfo dependencyInfo_bar; memset(&dependencyInfo_bar, 0, sizeof(VkDependencyInfo));

	VkImageSubresourceRange subresourceRange_default;
	subresourceRange_default.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange_default.baseMipLevel = 0;
	subresourceRange_default.levelCount = 1;
	subresourceRange_default.baseArrayLayer = 0;
	subresourceRange_default.layerCount = 1;
	
	VkImageMemoryBarrier2 imgBar_ColAtt2None_ColAtt2PresentSrc; memset(&imgBar_ColAtt2None_ColAtt2PresentSrc, 0, sizeof(VkImageMemoryBarrier2));
	imgBar_ColAtt2None_ColAtt2PresentSrc.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	imgBar_ColAtt2None_ColAtt2PresentSrc.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	imgBar_ColAtt2None_ColAtt2PresentSrc.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
	imgBar_ColAtt2None_ColAtt2PresentSrc.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	imgBar_ColAtt2None_ColAtt2PresentSrc.dstAccessMask = VK_ACCESS_2_NONE;
	imgBar_ColAtt2None_ColAtt2PresentSrc.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	imgBar_ColAtt2None_ColAtt2PresentSrc.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	imgBar_ColAtt2None_ColAtt2PresentSrc.subresourceRange = subresourceRange_default;

	VkImageMemoryBarrier2 imgBar_None2ColAtt_Undef2ColAtt; memset(&imgBar_None2ColAtt_Undef2ColAtt, 0, sizeof(VkImageMemoryBarrier2));
	imgBar_None2ColAtt_Undef2ColAtt.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	imgBar_None2ColAtt_Undef2ColAtt.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
	imgBar_None2ColAtt_Undef2ColAtt.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	imgBar_None2ColAtt_Undef2ColAtt.srcAccessMask = VK_ACCESS_2_NONE;
	imgBar_None2ColAtt_Undef2ColAtt.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	imgBar_None2ColAtt_Undef2ColAtt.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imgBar_None2ColAtt_Undef2ColAtt.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	imgBar_None2ColAtt_Undef2ColAtt.subresourceRange = subresourceRange_default;

	VkCommandBuffer frameCommandBuffer{};
	for (int swapChainImageIndex = 0; swapChainImageIndex < PRESENT_IMG_COUNT; swapChainImageIndex++) {
		frameCommandBuffer = frameCmdBuffers[swapChainImageIndex];
		// Command Buffer State: Initial
		vkResetCommandBuffer(frameCommandBuffer, 0);
		VkCommandBufferBeginInfo cmdBufferBeginInfo{};
		cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		// Command Buffer State: Recording (vkCmd* commands can be used to record to the command buffer)
		vkBeginCommandBuffer(frameCommandBuffer, &cmdBufferBeginInfo);

		vkCmdBindDescriptorSets(frameCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, uberPipelineLayout, 0, 1, &uberDescSet, 0, nullptr);

		VkRenderingAttachmentInfo colorAttachment{};
		colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachment.imageView = swapChainX->swapChainImageView[swapChainImageIndex]; // Your color attachment view
		colorAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		colorAttachment.clearValue = clearColor;

		VkRenderingInfo renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.renderArea.offset = { 0, 0 };
		renderingInfo.renderArea.extent = swapChainX->swapChainExtent;
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colorAttachment;
		renderingInfo.pDepthAttachment = nullptr; // No depth
		renderingInfo.pStencilAttachment = nullptr;

		/*Bar*/imgBar_None2ColAtt_Undef2ColAtt.image = swapChainX->swapChainImages[swapChainImageIndex]; init_vkDependencyInfo(&dependencyInfo_bar);
		/*Bar*/dependencyInfo_bar.imageMemoryBarrierCount = 1; dependencyInfo_bar.pImageMemoryBarriers = &imgBar_None2ColAtt_Undef2ColAtt;
		/*Bar*/vkCmdPipelineBarrier2(frameCommandBuffer, &dependencyInfo_bar);

		vkCmdBeginRendering(frameCmdBuffers[swapChainImageIndex], &renderingInfo);

		vkCmdBindPipeline(frameCmdBuffers[swapChainImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline01);

		vkCmdDraw(frameCmdBuffers[swapChainImageIndex], 3, 1, 0, 0);

		vkCmdEndRendering(frameCmdBuffers[swapChainImageIndex]);

		/*Bar*/imgBar_ColAtt2None_ColAtt2PresentSrc.image = swapChainX->swapChainImages[swapChainImageIndex]; init_vkDependencyInfo(&dependencyInfo_bar);
		/*Bar*/dependencyInfo_bar.imageMemoryBarrierCount = 1; dependencyInfo_bar.pImageMemoryBarriers = &imgBar_ColAtt2None_ColAtt2PresentSrc;
		/*Bar*/vkCmdPipelineBarrier2(frameCommandBuffer, &dependencyInfo_bar);

		if (vkEndCommandBuffer(frameCmdBuffers[swapChainImageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
	//VkBuffer vertexBuffers[] = { vertexBuffer };
	//VkDeviceSize offsets[] = { 0 };
	//vkCmdBindVertexBuffers(frameCmdBuffers[swapChainImageIndex], 0, 1, vertexBuffers, offsets);
	//
	//vkCmdBindIndexBuffer(frameCmdBuffers[swapChainImageIndex], indexBuffer, 0, VK_INDEX_TYPE_UINT16);
	//
	//
	//vkCmdDrawIndexed(frameCmdBuffers[swapChainImageIndex], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

}
