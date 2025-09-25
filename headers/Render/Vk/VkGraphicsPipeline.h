#pragma once

#include "vulkan.h"

// Setts up the graphics pipeline for vk rendering.
VkSwapchainKHR SetupSwapChain(GLFWwindow* _window, VkDevice _device, VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);

// Cleans up the Vk Graphics pipeline
void CleanupGraphicsPipeline();