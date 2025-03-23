#include "GLFW_gui.h"
static GLFWwindow* window;

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

    // Create a GLFW windowed mode window
    window = glfwCreateWindow(800, 600, "Cross-Platform GUI", nullptr, nullptr);

    // Set the key callback function
    glfwSetKeyCallback(window, keyCallback);
}
void destroy_GUI() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
int loop_GUI() {
    // Poll events (like keyboard or mouse input)
    glfwPollEvents();
    if (keys[GLFW_KEY_W]) std::cout << "Holding W" << std::endl;
    if (keys[GLFW_KEY_A]) std::cout << "Holding A" << std::endl;
    if (keys[GLFW_KEY_S]) std::cout << "Holding S" << std::endl;
    if (keys[GLFW_KEY_D]) std::cout << "Holding D" << std::endl;

    return glfwWindowShouldClose(window);
}