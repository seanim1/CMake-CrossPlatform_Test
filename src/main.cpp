#include "Global.h"
#include "GLFW_gui.h"
#include "Vulkan_gpu.h"

#ifdef _WIN32
#include "Windows_Specific.h"
#elif __linux__
#include "Linux_Specific.h"
#elif __APPLE__
#include "MacOS_Specific.h"
#endif

int main() {
    queryProcessor_crossPlatform();
    GLFWwindow* window = init_GLFW();
    initVulkan(window);
    while (!glfwWindowShouldClose(window)) {
        // Poll events (like keyboard or mouse input)
        glfwPollEvents();
        if (keys[GLFW_KEY_W]) std::cout << "Holding W" << std::endl;
        if (keys[GLFW_KEY_A]) std::cout << "Holding A" << std::endl;
        if (keys[GLFW_KEY_S]) std::cout << "Holding S" << std::endl;
        if (keys[GLFW_KEY_D]) std::cout << "Holding D" << std::endl;
    }
    destroy_GLFW(window);
    return 0;
}
