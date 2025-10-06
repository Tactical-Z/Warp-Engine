
#include "VKManager.h"
#include "VkDevice.h"
#include "VkPhysicalDevice.h"


VkDevice SetupDevice(VkInstance _inst, VkSurfaceKHR _surface, VkPhysicalDevice _physicalDevice, QueueFamilyIndices* _familyIndices){
    
    
    LOG_INFO("Setup VkDevice");
    // Get and update queue families
    if(!FindQueueFamilies(_physicalDevice, _surface, _familyIndices)){
        LOG_ERROR("No Physical device found");
        return VK_NULL_HANDLE;
    }
    VkDevice device = {0};
    VkDeviceCreateInfo createInfo = {0};
    // Created as an array for each 
    VkDeviceQueueCreateInfo* queueCreateInfo = GetVkDeviceQueueCreateInfo(_familyIndices);
    VkPhysicalDeviceFeatures physicalDeviceFeatures = GetVkPhysicalDeviceFeatures();
    PopulateVkDevice(&createInfo,_physicalDevice, queueCreateInfo, _familyIndices->mNumFamilies, &physicalDeviceFeatures);

    if(vkCreateDevice(_physicalDevice, &createInfo, NULL, &device) != VK_SUCCESS){
        LOG_ERROR("Faild to create Vk Logic Device");
    }

    free(queueCreateInfo);
    queueCreateInfo = NULL;

    // Get the graphics queue handle
    vkGetDeviceQueue(device, _familyIndices->mGraphicsFamily, 0, &gVkContext.mGraphicsQueue);
    vkGetDeviceQueue(device, _familyIndices->mPresentFamily, 0, &gVkContext.mPresentQueue);
    return device;
};

VkDeviceQueueCreateInfo* GetVkDeviceQueueCreateInfo(QueueFamilyIndices* _family){

    // Right now we only need a queue with graphics capabilities.

    VkDeviceQueueCreateInfo* queueCreateInfo = malloc(sizeof(VkDeviceQueueCreateInfo) * _family->mNumFamilies);
    // set all fields to 0 in the struct
    memset(queueCreateInfo, 0, sizeof(VkDeviceQueueCreateInfo) * _family->mNumFamilies);

    uint32_t index = 0;

    queueCreateInfo[index].pNext = NULL;

    // Always add graphics family
    queueCreateInfo[index].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo[index].queueFamilyIndex = _family->mGraphicsFamily;
    queueCreateInfo[index].queueCount = 1;
    queueCreateInfo[index].pQueuePriorities = &_family->mGraphicsFamilyQueuePriority;
    index++;

    // Only add present family if it's different
    if (_family->mGraphicsFamily != _family->mPresentFamily) {
        
        queueCreateInfo[index].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo[index].queueFamilyIndex = _family->mPresentFamily;
        queueCreateInfo[index].queueCount = 1;
        queueCreateInfo[index].pQueuePriorities = &_family->mPresentFamilyQueuePriority;
        index++;
    } else {
        LOG_WARN("Only one family queue");
    }

    return queueCreateInfo;
};

VkPhysicalDeviceFeatures GetVkPhysicalDeviceFeatures(){

    // returns the features the physical device can use like geometry shaders.
    static VkPhysicalDeviceFeatures deviceFeatures = {0};
    return deviceFeatures;
};

void PopulateVkDevice(VkDeviceCreateInfo* _createInfo, VkPhysicalDevice _physicalDevice, VkDeviceQueueCreateInfo* _deviceQueueCreateInfo, int _numQueues, VkPhysicalDeviceFeatures* _physicalDeviceFeatures){

    // The Logical Device is used to interface with the physical device through pipelines and queues.
    _createInfo->sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    _createInfo->pQueueCreateInfos = _deviceQueueCreateInfo;
    _createInfo->queueCreateInfoCount = _numQueues;
    _createInfo->pEnabledFeatures = _physicalDeviceFeatures;

    uint32_t numExtensionsLayersReqired = sizeof(sRequiredDeviceExtensions) / sizeof(sRequiredDeviceExtensions[0]);
    if(numExtensionsLayersReqired >= 1 && IsDeviceExtensionSupported(_physicalDevice, sRequiredDeviceExtensions,numExtensionsLayersReqired)){
        _createInfo->enabledExtensionCount = numExtensionsLayersReqired;
        _createInfo->ppEnabledExtensionNames = sRequiredDeviceExtensions;
    } else if(numExtensionsLayersReqired >= 1) {
        LOG_WARN("Device extension requested, but not availible, continueing without");
        _createInfo->enabledExtensionCount = 0;
    } else {
        _createInfo->enabledExtensionCount = 0;
    }
    
    // Older versions required setting valdiation layers in device and instance, this is no longer
    // neccesary, although it is good to do incase they are needed for some old functionality.
    uint32_t numValidationLayers = sizeof(sValidationLayers) / sizeof(sValidationLayers[0]);
    if(ENABLE_VALIDATION_LAYERS && !IsValidationLayersSupported(sValidationLayers)){
        _createInfo->enabledLayerCount = 0;
        LOG_WARN("Validation layers requested, but not availible, continueing without");
    } else if(ENABLE_VALIDATION_LAYERS && numValidationLayers >= 1){
        _createInfo->enabledLayerCount = numValidationLayers;
        _createInfo->ppEnabledLayerNames = sValidationLayers;
    } else {
        _createInfo->enabledLayerCount = 0;
    }

}

void CleanupDevice(){

    LOG_INFO("Cleanup VkDevice");
    vkDestroyDevice(gVkContext.mDevice, NULL);

};