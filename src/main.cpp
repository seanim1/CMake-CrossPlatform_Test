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
    init_GUI();
    // Create a GLFW windowed mode window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Cross-Platform GUI", nullptr, nullptr);
    // Set the key callback function
    glfwSetKeyCallback(window, keyCallback);
    initVulkan(window);
    while (!loop_GUI(window)) {

    }
    destroy_GUI(window);
    return 0;
}
