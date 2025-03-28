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
void init_GUI() {
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
}
void destroy_GUI(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}
int loop_GUI(GLFWwindow* window) {
    // Poll events (like keyboard or mouse input)
    glfwPollEvents();
    if (keys[GLFW_KEY_W]) std::cout << "Holding W" << std::endl;
    if (keys[GLFW_KEY_A]) std::cout << "Holding A" << std::endl;
    if (keys[GLFW_KEY_S]) std::cout << "Holding S" << std::endl;
    if (keys[GLFW_KEY_D]) std::cout << "Holding D" << std::endl;

    return glfwWindowShouldClose(window);
}