#pragma once

#include "WindowManager.h"
#include "vulkan.h"

#ifdef NDEBUG
    #define ENABLE_VALIDATION_LAYERS 0 // false
#else 
    #define ENABLE_VALIDATION_LAYERS 1 // true
#endif

// Move to a math file
#define CLAMP(x,lower, upper) ((x) < (lower) ? (lower) : ((x) > (upper) ? (upper) : (x)))

static const char* sRequiredDeviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static const char* sValidationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};

typedef struct {
    VkSurfaceCapabilitiesKHR mCapabilities;
    uint32_t mFormatCount;
    VkSurfaceFormatKHR* mFormats;
    uint32_t mPresentModeCount;
    VkPresentModeKHR* mPresentModes;
}SwapChainSupportDetails;

typedef struct {
    int mNumFamilies;
    int mHasGraphicsFamily;
    float mGraphicsFamilyQueuePriority;
    uint32_t mGraphicsFamily;
    int mHasPresentFamily;
    float mPresentFamilyQueuePriority;
    uint32_t mPresentFamily;
}QueueFamilyIndices;

void InitVolk(GLFWwindow* _window, VkInstance* _inst, VkDebugUtilsMessengerEXT* _debugger, VkPhysicalDevice* _physicalDevice, VkDevice* _device, VkSurfaceKHR* _surface, VkSwapchainKHR* _swapChain);
VkInstance CreateInstance();
void PopulateVkInstanceCreateInfo(VkInstanceCreateInfo* _createInfo, VkApplicationInfo* _appInfo, VkDebugUtilsMessengerCreateInfoEXT* _initDebugCreateInfo);

VkDebugUtilsMessengerEXT SetupDebugMessenger(VkInstance* _vki);
void PopulateVkDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* _createInfo);
void LogExtensionSupport(int _shouldLog);
const char** GetRequiredExtensions(uint32_t* _numExtensions, int _shouldLog);
int IsValidationLayerSupported(int _shouldLog, const char* const* _validationLayers, const int _numValidationLayers);

VkSurfaceKHR SetupSurface(GLFWwindow* _window, VkInstance* _inst);

VkPhysicalDevice SetupPhysicalDevice(VkInstance* _inst, VkSurfaceKHR* _surface);
void LogDeviceSupport(VkInstance* _vki, int _shouldLog);
VkPhysicalDevice* GetAvailibleDevices(VkInstance* _vki, int* _numDevices);
VkPhysicalDevice GetBestSuitedPhysicalDevice(VkInstance* _vki, VkSurfaceKHR* _surface, VkPhysicalDevice* _availableDevices, const int _numDevices);
int isDeviceSuitable(VkPhysicalDevice _device, VkSurfaceKHR* _surface);
int isDeviceExtensionSupportAvailable(VkPhysicalDevice* _device, int _shouldLog);
int RateDevice(VkPhysicalDevice _device);
int FindQueueFamilies(VkPhysicalDevice _device, VkSurfaceKHR* _surface, QueueFamilyIndices* _familyQueue);

VkDevice SetupDevice(VkInstance* _inst, VkSurfaceKHR* _surface, VkPhysicalDevice* _physicalDevice, QueueFamilyIndices* _familyIndices);
VkDeviceQueueCreateInfo* GetVkDeviceQueueCreateInfo(VkInstance* _inst, QueueFamilyIndices* _family);
VkPhysicalDeviceFeatures GetVkPhysicalDeviceFeatures(VkInstance* _inst);
void PopulateVkDevice(VkDeviceCreateInfo* _createInfo, VkDeviceQueueCreateInfo* _deviceQueueCreateInfo, int* _numQueues, VkPhysicalDeviceFeatures* _physicalDeviceFeatures);

VkSwapchainKHR SetupSwapChain(GLFWwindow* _window, VkDevice* _device, VkPhysicalDevice* _physicalDevice, VkSurfaceKHR* _surface);
void PopulateSwapChain(VkSwapchainCreateInfoKHR* _createInfo, VkPhysicalDevice* _physicalDevice, VkSurfaceKHR* _surface, SwapChainSupportDetails* _swapChainSupport , GLFWwindow* _window);
SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice* _device, VkSurfaceKHR* _surface);
int isSwapChainAdaquate(SwapChainSupportDetails* _swapchain);
VkSurfaceFormatKHR ChooseSwapSurfaceFormat(VkSurfaceFormatKHR* _availibleFormats, uint32_t _numFormats); 
VkPresentModeKHR ChooseSwapPresentationMode(VkPresentModeKHR* _availibleModes, uint32_t _numModes);
VkExtent2D ChooseSwapExtent(VkSurfaceCapabilitiesKHR* _capabilities, GLFWwindow* _window);

int CleanupVolk(VkInstance* _vki, VkDebugUtilsMessengerEXT* _debugMessenger, VkDevice* _device, VkSurfaceKHR* _surface, VkSwapchainKHR* _swapChain);

// helper functions
void CheckExtensionSupport(int _shouldLog);

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