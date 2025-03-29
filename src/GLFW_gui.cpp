#include "GLFW_gui.h"

// Initialize the key state array to false
bool keys[GLFW_KEY_LAST + 1] = { false };

// Callback function for key events
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key >= 0 && key < GLFW_KEY_LAST + 1) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
            std::cout << key << " PRESS" << std::endl;
        }
        else if (action == GLFW_RELEASE) {
            keys[key] = false;
            std::cout << key << " RELEASE" << std::endl;
        }
    }
}

// Function to initialize GLFW and set the key callback
GLFWwindow* init_GLFW() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Prevents OpenGL Context Creation
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    // Returns whether the Vulkan loader and an ICD have been found.
    if (glfwVulkanSupported()) {
        std::cout << "GLFW: Vulkan loader and an ICD have been found" << std::endl;
    }
    uint32_t count = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);
    printf("GLFW: Vulkan instance extensions required by GLFW:\n");
    for (uint32_t i = 0; i < count; i++) {
        printf("-  %s\n", extensions[i]);
    }
    GLFWwindow* window = glfwCreateWindow(800, 600, "Cross-Platform GUI", nullptr, nullptr); // Create a GLFW windowed mode window
    glfwSetKeyCallback(window, keyCallback); // Set the key callback function
    return window;
}
void destroy_GLFW(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}
