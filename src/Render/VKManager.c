
#include "VKManager.h"
#include "VkInstance.h"
#include "VkDebugMessenger.h"
#include "VkSurface.h"
#include "VkPhysicalDevice.h"
#include "VkDevice.h"
#include "VkSwapChain.h"
#include "VkImageViewer.h"
#include "VkGraphicsPipeline.h"
#include "VkFramebuffer.h"
#include "Math.h"

VkContext gVkContext = {0};
SwapChainHandels gVkSwapChainHandles = {0};

void InitVolk(GLFWwindow* _window){

    LOG_INFO("Starting Vk-Init"); 

    gVkContext.mInstance = CreateInstance();
    gVkContext.mDebugger = SetupDebugMessenger(gVkContext.mInstance);
    gVkContext.mSurface = SetupSurface(_window, gVkContext.mInstance);
    gVkContext.mPhysicalDevice = SetupPhysicalDevice(gVkContext.mInstance, gVkContext.mSurface);
    gVkContext.mDevice = SetupDevice(gVkContext.mInstance, gVkContext.mSurface, gVkContext.mPhysicalDevice, &gVkContext.mDeviceSupportDetails.mQueueFamily);
    gVkContext.mSwapChain = SetupSwapChain(_window, gVkContext.mDevice, gVkContext.mPhysicalDevice, gVkContext.mSurface );
    gVkContext.mNumImageViews = gVkSwapChainHandles.mNumImages;
    gVkContext.mSwapChainImageViews = SetupImageViews(gVkContext.mDevice, gVkContext.mNumImageViews);
    /*gVkContext.mGraphicsPipeline */ SetupGraphicsPipeline(gVkContext.mDevice);
    //gVkContext.mSwapChainFramebuffers = SetupFrameBuffers(gVkContext.mDevice, gVkContext.mNumImageViews);
    LOG_INFO("Finished Vk-Init");
};

/*------------------------------------------------------------------------------*/

// debuggers
int IsValidationLayersSupported(const char* const* _validationLayersSupported){
    
    // Much like with exentions, arg1 is numb layers. 
    // it is set to 0 if NUll for arg2 is passed, arg2 is the array to populate.
    uint32_t layerCount;
    if(vkEnumerateInstanceLayerProperties(&layerCount, NULL) != VK_SUCCESS){
        LOG_ERROR("Availible vulkan validation layer counting failed");
        return 0;
    }
    
    if(SHOULD_LOG){
        LOG_INFO("Num availible volkan validation layers: %i", layerCount);
    }
    
    // Allocate memory for availble layers
    VkLayerProperties* layerProperties = malloc(sizeof(VkLayerProperties) * layerCount);
    if(!layerProperties){
        LOG_ERROR("Layer properties memory allocation faild");
        return 0;
    }
    
    // Extract the avilible layers
    if(vkEnumerateInstanceLayerProperties(&layerCount, layerProperties) != VK_SUCCESS){
        LOG_ERROR("Faild to extract validation layer properties");
        free(layerProperties);
        layerProperties = NULL;
        return 0;
    }

    if(SHOULD_LOG){
        LOG_INFO("Availbile validation layers:");
        for(uint32_t i = 0; i < layerCount; i++){
            LOG_INFO("%s", layerProperties[i].layerName);
        }
    }

    // Check to make sure any layers we are asking for actually exist before returning true;
    size_t numValidationLayersSupported = sizeof(_validationLayersSupported) / sizeof(_validationLayersSupported[0]);
    for(size_t i = 0; i < numValidationLayersSupported; i++){
        int validLayer = 0;
    
        for(int j = 0; j < layerCount; j++){
            if(strcmp(_validationLayersSupported[i], layerProperties[j].layerName) == 0){
                validLayer = 1; // true
                break;
            }
        }

        // If we get here one of the layers we asked for does not exist, so it will fail
        if(!validLayer){
            free(layerProperties);
            layerProperties = NULL;
            
            if (_validationLayersSupported[i]) {
                LOG_WARN("Validation layer '%s' might not be valid or exist", _validationLayersSupported[i]);
            }

            return 0;
        }
    }

    free(layerProperties);
    layerProperties = NULL;
    return 1;
};

int IsDeviceExtensionSupported(VkPhysicalDevice _physicalDevice, const char* const* _deviceExtension, uint32_t _numRequiredDeviceExtensions){
    uint32_t extensionCount = 0;
    // Get the number of available device extensions
    if (vkEnumerateDeviceExtensionProperties(_physicalDevice, NULL, &extensionCount, NULL) != VK_SUCCESS) {
        LOG_ERROR("Failed to enumerate device extensions count");
        return 0;
    }

    VkExtensionProperties* availableExtensions = malloc(sizeof(VkExtensionProperties) * extensionCount);
    if (!availableExtensions) {
        LOG_ERROR("Failed to allocate memory for device extension properties");
        return 0;
    }

    // Get the extension properties
    if (vkEnumerateDeviceExtensionProperties(_physicalDevice, NULL, &extensionCount, availableExtensions) != VK_SUCCESS) {
        LOG_ERROR("Failed to enumerate device extensions");
        free(availableExtensions);
        return 0;
    }

    if (SHOULD_LOG) {
        LOG_INFO("Num total extensions: %i",extensionCount);
        LOG_INFO("Available device extensions:");
        for (uint32_t i = 0; i < extensionCount; i++) {
            LOG_INFO("%s", availableExtensions[i].extensionName);
        }
    }

    // Check if all required extensions are supported
    for (size_t i = 0; i < _numRequiredDeviceExtensions; i++) {
        int found = 0;
        for (uint32_t j = 0; j < extensionCount; j++) {
            if (strcmp(_deviceExtension[i], availableExtensions[j].extensionName) == 0) {
                found = 1;
                break;
            }
        }

        if (!found) {
            LOG_WARN("Device extension '%s' might not be valid or exist", _deviceExtension[i]);
            free(availableExtensions);
            return 0;
        }
    }

    free(availableExtensions);
    availableExtensions = NULL;
    return 1;
};

void LogDeviceSupport(VkInstance _vki){

    // arg1 is the name null is fine, arg2 is the num exensions to fill, 
    // arg3 is the arrat to fill if null arg2 = max num extensions.
    uint32_t deviceCount = 0;
    if(vkEnumeratePhysicalDevices(_vki, &deviceCount, NULL) != VK_SUCCESS){
        LOG_ERROR("Availible vulkan GPU counting faild");
    }
    LOG_INFO("Num availible volkan GPUs: %i", deviceCount);
    
    // Allocate memory for devices
    VkPhysicalDevice* devices = malloc(sizeof(VkPhysicalDevice) * deviceCount);
    if(!devices){
        LOG_ERROR("Faild to allocate memory for vulkan GPUs");
        return;
    }
        
    // Call to get all the devices
    if(vkEnumeratePhysicalDevices(_vki, &deviceCount, devices) != VK_SUCCESS){
        LOG_ERROR("Faild to extract GPUs for log"); 
        free(devices);
        devices = NULL;
        return;
    }

    // have to extract properties instead of using device directly in comparison to extensions
    LOG_INFO("Availible GPUs:");
    for(uint32_t i = 0; i < deviceCount; i++){
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);

        LOG_INFO("Device %u, %s", i, deviceProperties.deviceName);
        // more details in device properties if needed.
    }
        
    free(devices);
    devices = NULL;
};

int CleanupVolk(){

    if(ENABLE_VALIDATION_LAYERS){
        CleanupDebugMessenger();
    }
    //CleanupFrameBuffers();
    CleanupGraphicsPipeline();
    CleanupImageViews();
    CleanupSwapChain();
    CleanupSurface();
    CleanupPhysicalDevice();
    CleanupDevice();
    CleanupInstance();

    return 0;
};

// Vk Proxy Functions ----

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, 
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, 
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    // Gets the correct address for for the extention required by the function.
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,"vkCreateDebugUtilsMessengerEXT");
    if(func != NULL){
        return func(instance,pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
};

void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
    VkDebugUtilsMessengerEXT debugMessenger, 
    const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if( func != NULL){
        func(instance, debugMessenger, pAllocator);
    }
};