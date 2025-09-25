#pragma once 

#include "vulkan.h"
#include "glfw3.h"

// Creates the vk Surface based on instance rules.
// @param _window is the GLFW window the vk surface will reference.
// @param _inst is the vk instance referenced during creation.
// @return Is the VkSurface object. 
VkSurfaceKHR SetupSurface(GLFWwindow* _window, VkInstance _inst);

// Cleans up the Vk Surface
void CleanupSurface();
