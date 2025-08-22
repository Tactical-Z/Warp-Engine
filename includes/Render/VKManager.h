#pragma once

#include "WindowManager.h"
#include "vulkan.h"

#ifdef NDEBUG
    #define ENABLE_VALIDATION_LAYERS 0 // false
#else 
    #define ENABLE_VALIDATION_LAYERS 1 // true
#endif



void InitVolk(VkInstance* _inst, VkDebugUtilsMessengerEXT* _debugger);
VkInstance CreateInstance();
void PopulateVkInstanceCreateInfo(VkInstanceCreateInfo* _createInfo, VkApplicationInfo* _appInfo, VkDebugUtilsMessengerCreateInfoEXT* _initDebugCreateInfo);
VkDebugUtilsMessengerEXT SetupDebugMessenger(VkInstance* _vki);
void PopulateVkDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* _createInfo);
void CheckExtensionSupport(int _shouldLog);
const char** GetRequiredExtensions(uint32_t* _numExtensions, int _shouldLog);
int CheckValidationLayerSupport(int _shouldLog, const char* const* _validationLayers, const int _numValidationLayers);
int CleanupVolk(VkInstance* _vki, VkDebugUtilsMessengerEXT* _debugMessenger);

static const char* validationLayers[] = {
      "VK_LAYER_KHRONOS_validation"
};

// Vk Proxy Functions ----
  
// Static vulcan callback function for error message writing
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messagesSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, 
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, 
    VkDebugUtilsMessengerEXT* pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(
    VkInstance instance, 
    VkDebugUtilsMessengerEXT debugMessenger, 
    const VkAllocationCallbacks* pAllocator);