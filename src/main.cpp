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
	glm::mat4 view;
	glm::mat4 proj;
	float elapsedTime;
};
static CameraMatrices cam;
/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    gWindow = new GameWindow(1280, 720, "Cross-Platform GUI");
    gScreen = new GameScreen(gWindow->dimension.x / 10, gWindow->dimension.y / 10, gWindow->renderer);
	gCamera = new GameCamera(60.0f, gWindow->dimension.x / static_cast<float>(gWindow->dimension.y), 0.1f, 100.0f);
	gInput = new GameInput();
    gTimer = new GameTimer();

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
	cmdX = new VulkanCommand(deviceX->logicalDevice, queueX->queueFamilyIndex);
	syncX = new VulkanSynchronization(deviceX->logicalDevice);
	Box* box_01 = new Box(2.0f, 2.0f, 2.0f);

	descriptorList.push_back( new VulkanDescBufferUniform(&cam, sizeof(cam)) );
	((VulkanDescBufferUniform*)descriptorList[0])->allocateUniformBuffer(deviceX->logicalDevice, physicalDeviceX->physicalDevice);
	VulkanSpecializationConstant* specialConstantX = new VulkanSpecializationConstant(
		gScreen->dimension.x,
		gScreen->dimension.y
	);
	VulkanUberDescriptorSet* descriptorX = new VulkanUberDescriptorSet(deviceX->logicalDevice, descriptorList);
	VulkanGraphicsPipeline* graphicsPipelineX = new VulkanGraphicsPipeline(deviceX->logicalDevice, 
		swapChainX->swapChainExtent, swapChainX->selectedSurfaceFormat, 
		descriptorX->uberPipelineLayout, box_01, 
		specialConstantX->specializationInfo);
	cmdX->buildCommandBuffers(swapChainX, descriptorX->uberPipelineLayout,
		descriptorX->uberDescSet, graphicsPipelineX->graphicsPipeline, box_01);

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

#ifdef USE_GPU
	cam.view = gCamera->GetViewMatrix();
	cam.proj = gCamera->GetProjectionMatrix();
	cam.elapsedTime = gTimer->elapsedTime;
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
