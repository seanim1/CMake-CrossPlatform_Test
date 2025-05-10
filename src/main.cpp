    #define SDL_MAIN_USE_CALLBACKS 1

#include "Global.h"
#include "GameWindow.h"
#include "GameScreen.h"
#include "GameCamera.h"
#include "GameInput.h"
#include "GameTimer.h"

#include "GeometryBox.h"

#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_main_impl.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_video.h>

#ifdef USE_GPU
#define VALIDATION_LAYER_VULKAN
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanSwapChain.h"
#include "VulkanCommand.h"
#include "VulkanSynchronization.h"
#include "VulkanSpecializationConstant.h"
#include "VulkanDescBufferUniform.h"
#include "VulkanUberDescriptorSet.h"
#include "VulkanGraphicsPipeline.h"

#include <vulkan/vulkan.h>
#if defined(_WIN32)
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#elif defined(__ANDROID__)
#include <vulkan/vulkan_android.h>
#elif defined(__linux__)
#include <xcb/xcb.h>
#include <vulkan/vulkan_xcb.h>
#elif defined(__linux__) && defined(USING_WAYLAND)
#include <wayland-client.h>
#include <vulkan/vulkan_wayland.h>
#elif defined(__APPLE__) // For both MacOS and iOS
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan_metal.h>
#include <vulkan/vulkan_beta.h>
#endif
#endif

static GameWindow* gWindow = nullptr;
static GameScreen* gScreen = nullptr;
static GameCamera* gCamera = nullptr;
static GameInput* gInput = nullptr;
static GameTimer* gTimer = nullptr;
static VulkanQueue* queueX = nullptr;
static VulkanDevice* deviceX = nullptr;
static VulkanSynchronization* syncX = nullptr;
static VulkanCommand* cmdX = nullptr;
static VulkanSwapChain* swapChainX = nullptr;
static std::vector<VulkanDesc*> descriptorList;
struct CameraMatrices {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;

	glm::vec3 camPos;
	float elapsedTime;
	
	glm::vec3 camDir;
	float padding_0;

	glm::vec3 dirLightDir;
	float padding_1;
};
static CameraMatrices cam;
static Box* box_01;
/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    gWindow = new GameWindow(1280, 720, "Cross-Platform GUI");
    gScreen = new GameScreen(gWindow->dimension.x, gWindow->dimension.y, gWindow->renderer);
	gCamera = new GameCamera(60.0f, gWindow->dimension.x / static_cast<float>(gWindow->dimension.y), 0.1f, 100.0f);
	gInput = new GameInput();
    gTimer = new GameTimer();
	gCamera->SetPosition(glm::vec3(2., 2., -2.));
#ifdef USE_GPU
	std::vector<const char*> requestingInstanceExtensions = {
	VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
	VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,
	};
	// VK_KHR_SURFACE_EXTENSION_NAME and platform specific surface will be provided by SDL
	// Get required Vulkan extensions
	uint32_t SDL_VulkanExtensionCount = 0;
	const char* const* SDL_VulkanExtensions = SDL_Vulkan_GetInstanceExtensions(&SDL_VulkanExtensionCount);
	SDL_Log("SDL3 Vulkan instance extensions required:");
	for (uint32_t i = 0; i < SDL_VulkanExtensionCount; i++) {
		SDL_Log("- %s", SDL_VulkanExtensions[i]);
		requestingInstanceExtensions.push_back(SDL_VulkanExtensions[i]);
	}
#if defined(__APPLE__)
	requestingInstanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME); // vkCreateInstance will fail with VK_ERROR_INCOMPATIBLE_DRIVER: https://stackoverflow.com/questions/72789012/why-does-vkcreateinstance-return-vk-error-incompatible-driver-on-macos-despite
#endif
	std::vector<const char*> requestingInstanceLayers = {
#ifdef VALIDATION_LAYER_VULKAN
		"VK_LAYER_KHRONOS_validation"
#endif
	};
	std::vector<const char*> requestedDeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
	VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
	VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME, // for VK_ACCESS_NONE in the pipeline barrier
	VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME,  // for GL_EXT_shader_explicit_arithmetic_types_float16
	VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
	VK_KHR_8BIT_STORAGE_EXTENSION_NAME,
	VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME,
	VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
	VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME, // for shader printf: GL_EXT_debug_printf
	VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME,
	VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME, // not fully supported on RX6600 (imageAtomicAdd not supported)
	//VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME,
	};
#if defined(__APPLE__)
	// From SaschaWillems - When running on iOS/macOS with MoltenVK and VK_KHR_portability_subset is defined and supported by the device, enable the extension
	requestedDeviceExtensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif
	VulkanInstance* instanceX = new VulkanInstance(&requestingInstanceExtensions, &requestingInstanceLayers);
	VulkanPhysicalDevice* physicalDeviceX = new VulkanPhysicalDevice(instanceX->instance);

	queueX = new VulkanQueue(0, 1);
	deviceX = new VulkanDevice(physicalDeviceX->physicalDevice, physicalDeviceX->computeQueueFamilyIndex, &requestedDeviceExtensions);
	queueX->initQueue(deviceX->logicalDevice);
	instanceX->initSurface(gWindow->window);
	swapChainX = new VulkanSwapChain(physicalDeviceX->physicalDevice, instanceX->surface, deviceX->logicalDevice);
	cmdX = new VulkanCommand(deviceX->logicalDevice, queueX->queueFamilyIndex, swapChainX->swapChainImages.size());
	syncX = new VulkanSynchronization(deviceX->logicalDevice);
	box_01 = new Box(1.7f, 1.7f, 1.7f);
	descriptorList.push_back( new VulkanDescBufferUniform(&cam, sizeof(cam), deviceX->logicalDevice, physicalDeviceX->physicalDevice));
	descriptorList.push_back( new VulkanDescBuffer(deviceX->logicalDevice, physicalDeviceX->physicalDevice,
		box_01->getVertexData(), box_01->getVertexCount() * box_01->getVertexStride(), 
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT));
	descriptorList.push_back(new VulkanDescBuffer(deviceX->logicalDevice, physicalDeviceX->physicalDevice,
		box_01->getIndexData(), box_01->getIndexCount() * sizeof(box_01->getIndexData()[0]),
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT));

	((VulkanDescBufferUniform*)descriptorList[0])->allocateUniformBuffer(deviceX->logicalDevice, physicalDeviceX->physicalDevice);
	((VulkanDescBuffer*)descriptorList[1])->allocate(deviceX->logicalDevice, physicalDeviceX->physicalDevice, cmdX->cmdPool, queueX->queue);
	((VulkanDescBuffer*)descriptorList[2])->allocate(deviceX->logicalDevice, physicalDeviceX->physicalDevice, cmdX->cmdPool, queueX->queue);
	
	VulkanSpecializationConstant* specialConstantX = new VulkanSpecializationConstant(
		gScreen->dimension.x,
		gScreen->dimension.y
	);
	VulkanUberDescriptorSet* descriptorX = new VulkanUberDescriptorSet(deviceX->logicalDevice, descriptorList);
	VulkanGraphicsPipeline* graphicsPipelineX = new VulkanGraphicsPipeline(physicalDeviceX->physicalDevice, deviceX->logicalDevice,
		swapChainX, swapChainX->selectedSurfaceFormat, 
		descriptorX->uberPipelineLayout, box_01, 
		specialConstantX->specializationInfo);
	cmdX->buildCommandBuffers(swapChainX, descriptorX->uberPipelineLayout,
		descriptorX->uberDescSet, graphicsPipelineX, 
		((VulkanDescBuffer*)descriptorList[1])->buffer, ((VulkanDescBuffer*)descriptorList[2])->buffer, box_01->getIndexCount());

#endif
    return SDL_APP_CONTINUE; // SDL_APP_FAILURE to indicate failure
}

/* This function runs when SDL receives input or OS-level events. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS; // Clean exit
    }
    gInput->ProcessEvent(*event);
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame. */
SDL_AppResult SDL_AppIterate(void* appstate) {
    gTimer->StartTimer();
    gInput->Update(gCamera);
	//box_01->setRotation(glm::vec3(0., gTimer->elapsedTime * 0.2, 0.1));
	//box_01->setScale(glm::vec3(0.4 * cos(gTimer->elapsedTime) + 1.2));
	//box_01->setPosition(glm::vec3(0.3 * sin(gTimer->elapsedTime), 0.3 * sin(gTimer->elapsedTime), 0.));
	//box_01->setPosition(glm::vec3(0.01, 0., 0.));
#ifdef USE_GPU
	cam.model = box_01->getModelMatrix();
	cam.view = gCamera->GetViewMatrix();
	cam.proj = gCamera->GetProjectionMatrix();
	cam.camPos = gCamera->GetPosition();
	cam.elapsedTime = gTimer->elapsedTime;
	cam.camDir = gCamera->GetDirection();
	cam.dirLightDir = glm::normalize(glm::vec3(2.5, 25.0 * cos(gTimer->elapsedTime), 25.0 * sin(gTimer->elapsedTime)));
	((VulkanDescBufferUniform*)descriptorList[0])->update();
    // Vulkan rendering goes here
	queueX->drawFrame(deviceX->logicalDevice, syncX, swapChainX->swapChain, cmdX);
#else
    gScreen->Update(gTimer->elapsedTime, gWindow->renderer);
#endif

    gTimer->EndTimer();
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    delete gTimer;
    delete gInput;
    delete gScreen;
    delete gWindow;
}
