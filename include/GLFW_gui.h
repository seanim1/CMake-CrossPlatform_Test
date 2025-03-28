#pragma once
#include "Global.h"
#include <GLFW/glfw3.h>

// Declare the array to track key states
extern bool keys[GLFW_KEY_LAST + 1];

// Callback function for key events
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Function to initialize GLFW and set the key callback
GLFWwindow* init_GLFW();
void destroy_GLFW(GLFWwindow* window);
