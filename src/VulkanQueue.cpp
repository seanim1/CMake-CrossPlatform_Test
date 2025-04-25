#include "VulkanQueue.h"
unsigned int VulkanQueue::frameIndex = 0;

VulkanQueue::VulkanQueue(uint32_t queueFamilyIndex, uint32_t queueCount)
	: queueFamilyIndex(queueFamilyIndex)
{
}
void VulkanQueue::initQueue(VkDevice logicalDevice) {
	// logicalDevice
	vkGetDeviceQueue(logicalDevice, queueFamilyIndex, 0, &queue);
}
void VulkanQueue::drawFrame(VkDevice logicalDevice, VulkanSynchronization* syncX, 
    VkSwapchainKHR swapChain, VulkanCommand* cmdX) {
    
    vkWaitForFences(logicalDevice, 1, &syncX->inFlightFence[frameIndex], VK_TRUE, UINT64_MAX);

    uint32_t swapChainImageIndex;
    vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, syncX->presentFinImgAvailSem[frameIndex], VK_NULL_HANDLE, &swapChainImageIndex);

    vkResetFences(logicalDevice, 1, &syncX->inFlightFence[frameIndex]);
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { syncX->presentFinImgAvailSem[frameIndex] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdX->frameCmdBuffers[swapChainImageIndex];

    VkSemaphore signalSemaphores[] = { syncX->renderFinishedSem[frameIndex] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(logicalDevice, 1, &syncX->inFlightFence[frameIndex]);

    if (vkQueueSubmit(queue, 1, &submitInfo, syncX->inFlightFence[frameIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &swapChainImageIndex;

    vkQueuePresentKHR(queue, &presentInfo);

    frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}
