#pragma once
#include "vulkan.h"

// Setts up the debug messenger
// @param _vki is the vulkan instance used in the vk create function
VkDebugUtilsMessengerEXT SetupDebugMessenger(VkInstance _vki);

// Populates the construction info of the debug messenger.
// @param _createInfo is the create info struct reference to fill.
void PopulateVkDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* _createInfo);

// Finds all extensions supported by the application and logs them to the terminal
void LogExtensionSupport();

// Gets and returns all extensions required by the application.
// @param _numExtensions will be filled through reference with the number of required extensions.
// @return A pointer to an array of extensions names reqired by the apploication.
const char** GetRequiredExtensions(uint32_t* _numExtensions);

// Static vulcan callback function for error message writing
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messagesSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

// Cleans up the debug messenger
void CleanupDebugMessenger();