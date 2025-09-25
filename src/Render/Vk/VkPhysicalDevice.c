
#include "VKManager.h"
#include "VkSwapChain.h"
#include "VkPhysicalDevice.h"

VkPhysicalDevice SetupPhysicalDevice(VkInstance _vki, VkSurfaceKHR _surface){

    LOG_INFO("Setup Physical Device");
    int numDevices = 0;
    VkPhysicalDevice* availibleDevices = GetAvailibleDevices(_vki, &numDevices);
    VkPhysicalDevice bestDevice = GetBestSuitedPhysicalDevice(_vki, _surface, availibleDevices, numDevices);
    free(availibleDevices);
    availibleDevices = NULL;

    return bestDevice;
};

VkPhysicalDevice* GetAvailibleDevices(VkInstance _vki, int* _numDevices){

    uint32_t deviceCount = 0;
    if(vkEnumeratePhysicalDevices(_vki, &deviceCount, NULL) != VK_SUCCESS){
       LOG_ERROR("Faild to find GPUs with Vulkan support");
       return NULL;
    }

    VkPhysicalDevice* devices = malloc(sizeof(VkPhysicalDevice) * deviceCount);
    if(!devices){
       LOG_ERROR("Device memory allocation faild");
       return NULL;
    }

    // Get the devices
    if(vkEnumeratePhysicalDevices(_vki, &deviceCount, devices) != VK_SUCCESS){
       LOG_ERROR("Faild to extract devices");
       free(devices);
       devices = NULL;
       return NULL;
    }

    if(_numDevices){
        *_numDevices = deviceCount;
    }
    
    return devices;
};

VkPhysicalDevice GetBestSuitedPhysicalDevice(VkInstance _vki, VkSurfaceKHR _surface, VkPhysicalDevice* _availableDevices, int _numAvailibleDevices){

    int bestDeviceScore = 0;
    int bestDeviceIt = 0;
    
    for(int i = 0; i < _numAvailibleDevices; i++){

        if(isDeviceSuitable(_availableDevices[i], _surface)){
            int score = RateDevice(_availableDevices[i]);
            if(score > bestDeviceScore){
                bestDeviceScore = score;
                bestDeviceIt = i;
            }
        }
    }

    VkPhysicalDevice bestDevice = {0};

    if(bestDeviceScore == 0){
        LOG_ERROR("No-Devices suitable");
        return bestDevice;
    }

    bestDevice = _availableDevices[bestDeviceIt];
    return bestDevice;
};

int isDeviceSuitable(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface){

    int suitable = 0;

    // Querry to se if this device and surface can make an adaquate swapchain
    SwapChainSupportDetails swapChainForDevice = QuerySwapChainSupport(_physicalDevice, _surface);

    QueueFamilyIndices queueFamily = {0};
    if(FindQueueFamilies(_physicalDevice, _surface, &queueFamily) &&
        isDeviceExtensionSupportAvailable(_physicalDevice) &&
        isSwapChainAdaquate(&swapChainForDevice)){
        return 1;
    }

    return 0;
};

int isDeviceExtensionSupportAvailable(VkPhysicalDevice _physicalDevice){

    uint32_t extensionCount;
    if(vkEnumerateDeviceExtensionProperties(_physicalDevice, NULL, &extensionCount, NULL) != VK_SUCCESS){
        LOG_ERROR("Faild counting vkExtentions for vkDevice requirements");
        return 0;
    }

    VkExtensionProperties* availableExtensions = malloc(sizeof(VkExtensionProperties) * extensionCount);
    if(!availableExtensions){
        LOG_ERROR("Faild to allocate memory for available extentions during vkDevice extension check");
        return 0;
    }
    
    if(vkEnumerateDeviceExtensionProperties(_physicalDevice, NULL, &extensionCount, availableExtensions) != VK_SUCCESS){
        LOG_ERROR("Faild to get vkDevice extension properties");
        free(availableExtensions);
        availableExtensions = NULL;
        return 0;
    }

    int numRequiredExtnesions = sizeof(sRequiredDeviceExtensions) / sizeof(sRequiredDeviceExtensions[0]);
    if(SHOULD_LOG){
        LOG_INFO("Num device extensions: %i", extensionCount);
        LOG_INFO("Num required extensions: %i", numRequiredExtnesions);

        for(int i = 0; i < numRequiredExtnesions; i++){
            
            for(int j = 0; j < extensionCount; j++){
                 LOG_INFO("DeviceExtension '%i', '%s'", i, availableExtensions[j]);
                if(strcmp(sRequiredDeviceExtensions[i], availableExtensions[j].extensionName) == 0){
                    LOG_INFO("Found correct extension, %s", availableExtensions[j].extensionName);
                }
            }
        }
    }

    int allExtentionsValid = 0;
    for(int i = 0; i < numRequiredExtnesions; i++){
        for(int j = 0; j < extensionCount; j++){
            if(strcmp(sRequiredDeviceExtensions[i], availableExtensions[j].extensionName) == 0){
                allExtentionsValid++;
            }
            if(allExtentionsValid == numRequiredExtnesions){
                break;
            }
        }
    }

    free(availableExtensions);
    availableExtensions = NULL;

    if(allExtentionsValid == numRequiredExtnesions){
        return 1;
    } else {
        LOG_WARN("Not all required device extensions where availible for this device");
        return 0;
    }
};

int RateDevice(VkPhysicalDevice _physicalDevice){
    
    int score = 0;
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(_physicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(_physicalDevice, &deviceFeatures);

    // Rating values:
    if(!deviceFeatures.geometryShader){
        return score;
    }

    if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU){
        score += 1000;
    }

    score += deviceProperties.limits.maxImageDimension2D;

    return score;
};

int FindQueueFamilies(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface, QueueFamilyIndices* _familyQueue){

    // number of availible queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilyProperties = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    if(!queueFamilyProperties){
        LOG_ERROR("Faild to allocate memory for queue families");
        return 0;
    }

    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, queueFamilyProperties);
    
    // make sure that one of the families conforms to our needs
    int foundGraphics = 0;
    int foundPresent = 0;
    for(uint32_t i = 0; i < queueFamilyCount; i++){
        // "&" bit wise AND opperation to se if queueflags contains the vk_num asked for
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            _familyQueue->mGraphicsFamily = i;
            _familyQueue->mGraphicsFamilyQueuePriority = 1.0f;
            _familyQueue->mHasGraphicsFamily = 1;
            foundGraphics = 1;
        }

        if(_surface){
            VkBool32 presentSupport = 0;
            vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, i, _surface, &presentSupport);
            if (presentSupport) {
                _familyQueue->mPresentFamily = i;
                _familyQueue->mPresentFamilyQueuePriority = 1.0f;
                _familyQueue->mHasPresentFamily = 1;
                foundPresent = 1;
            }
        }
    }

    free(queueFamilyProperties);
    queueFamilyProperties = NULL;
    
    if (foundGraphics && foundPresent) {
        _familyQueue->mNumFamilies = (_familyQueue->mGraphicsFamily != _familyQueue->mPresentFamily) ? 2 : 1;
        return 1;
    } else {
        return 0;
    }
};

void CleanupPhysicalDevice(){
    LOG_INFO("Cleanup VkPhysicalDevice");

};
