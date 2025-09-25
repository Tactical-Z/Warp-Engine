
#include "VKManager.h"
#include "VkInstance.h"
#include "VkDebugMessenger.h"

VkInstance CreateInstance(){

    LOG_INFO("Setup VkInstance");
    VkInstance instance = {0};
    // The application info contains optional info for the instance 
    // to use during init best to fill it so it can optimise behind the scenes
    VkApplicationInfo appInfo = {0};
    appInfo.pNext = NULL;                                // Pointer to an Extension of information if required
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Vulcan";
    appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // This struct is not optional, it is created to tell vulkan driver
    // which global Extensions and which validation layers to use. 
    VkInstanceCreateInfo createInfo = {0};
    VkDebugUtilsMessengerCreateInfoEXT initDebugCreateInfo = {0};
    PopulateVkInstanceCreateInfo(&createInfo, &appInfo, &initDebugCreateInfo);

    // Eveything is ready to create the instance, arg1 is the creation information, arg2 an optional pointer to any custom allocator callbacks
    // and arg3 is the instance we want to populate. The funcion returns a VKResult enum which we can error check against. 
    if(vkCreateInstance(&createInfo, NULL, &instance) != VK_SUCCESS){
        LOG_ERROR("Faild to create VkInstance!");
    }
        
    return instance;
};

void PopulateVkInstanceCreateInfo(VkInstanceCreateInfo* _createInfo, const VkApplicationInfo* _appInfo, VkDebugUtilsMessengerCreateInfoEXT* _initDebugCreateInfo){

    _createInfo->pNext = NULL;
    _createInfo->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    _createInfo->pApplicationInfo = _appInfo;

    // These paramaters spcify the desired global extentsions 
    // (Vulkan is an Agnostic API, so it needs an Extension to interface with windows)
    uint32_t numExtensions = 0;
    const char** extensions = GetRequiredExtensions(&numExtensions);
    _createInfo->enabledExtensionCount = numExtensions;
    _createInfo->ppEnabledExtensionNames = extensions;

    // Manage validation layers for the application
    uint32_t numValidationLayers = sizeof(sValidationLayers) / sizeof(sValidationLayers[0]);

    // Validation layers must be made here in instance as well as debugger
    // so that init and shutdown can be debugged correctly
    if(ENABLE_VALIDATION_LAYERS && !IsValidationLayersSupported(sValidationLayers)){
        _createInfo->enabledLayerCount = 0;
        LOG_WARN("Validation layers requested, but not availible, continueing without");
    } else if(ENABLE_VALIDATION_LAYERS && numValidationLayers >= 1){
        _createInfo->enabledLayerCount = numValidationLayers;
        _createInfo->ppEnabledLayerNames = sValidationLayers;
        
        // Creates a seperate vk debug messenger spesifically for instencing and destroying. 
        PopulateVkDebugMessengerCreateInfo(_initDebugCreateInfo);
        _createInfo->pNext = (VkDebugUtilsMessengerCreateInfoEXT*) _initDebugCreateInfo;
        
    } else {
        _createInfo->enabledLayerCount = 0;
    }

};

void CleanupInstance(){

    LOG_INFO("Cleanup VkInstance");
    vkDestroyInstance(gVkContext.mInstance, NULL);

};