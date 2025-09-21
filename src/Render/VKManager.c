
#include "VKManager.h"
#include "Math.h"

VkContext gVkContext = {0};

void InitVolk(GLFWwindow* _window, VkInstance* _inst, VkDebugUtilsMessengerEXT* _debugger, VkPhysicalDevice* _physicalDevice, VkDevice* _device, VkSurfaceKHR* _surface, VkSwapchainKHR* _swapChain){

    LOG_INFO("Starting Vk-Init"); 
    // New
    gVkContext.mInstance = CreateInstance();

    //*_inst = CreateInstance();
    *_debugger = SetupDebugMessenger(_inst);
    *_surface = SetupSurface(_window, _inst);
    *_physicalDevice = SetupPhysicalDevice(_inst,_surface);
    QueueFamilyIndices queueFamily = {0};
    *_device = SetupDevice(_inst, _surface, _physicalDevice, &queueFamily);
    *_swapChain = SetupSwapChain(_window, _device, _physicalDevice, _surface);
    
    //VkQueue graphicsQueue = {0};
    //VkQueue presentQueue = {0};
    //vkGetDeviceQueue(*_device, queueFamily.mGraphicsFamily, 0, &graphicsQueue);
    //vkGetDeviceQueue(*_device, queueFamily.mPresentFamily, 0, &presentQueue);
    LOG_INFO("Finished Vk-Init");
};

/*------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------*/

VkDebugUtilsMessengerEXT SetupDebugMessenger(VkInstance* _vki){

    if(!ENABLE_VALIDATION_LAYERS) return NULL;

    LOG_INFO("Setup VkDebugger");
    VkDebugUtilsMessengerEXT VkDebugMessenger = {0};

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {0};
    PopulateVkDebugMessengerCreateInfo(&createInfo);

    if(CreateDebugUtilsMessengerEXT(*_vki, &createInfo, NULL, &VkDebugMessenger)){
        LOG_ERROR("Faild to settup VK Debug Messenger, continuing without.");
    }

    return VkDebugMessenger;
};

void PopulateVkDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* _createInfo){

    _createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    // messageSverity field specifies for what severities the callback is called for.
    _createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    // messageType field specifies what messages to filter for.
    _createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    // specifies the pointer to the callback funtion.
    _createInfo->pfnUserCallback = debugCallback;
    _createInfo->pUserData = NULL;
};

void CheckExtensionSupport(int _shouldLog){

    if(!_shouldLog) 
        return;

    // arg1 is the name null is fine, arg2 is the num exensions to fill, 
    // arg3 is the arrat to fill if null arg2 = max num extensions.
    uint32_t extensionCount = 0;
    if(vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL) != VK_SUCCESS)
        LOG_ERROR("Availible vulkan extention counting faild");
    //debugLOG_INFO("Num availible volkan extensions: %i", extensionCount);
    
    // Allocate memory for extensions
    VkExtensionProperties* extensions = malloc(sizeof(VkExtensionProperties) * extensionCount);
    if(!extensions){
        LOG_ERROR("Faild to allocate memory for vulkan extensions");
        return;
    }
        
    // Call to get all the extension properies
    if(vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions) != VK_SUCCESS){
        LOG_ERROR("Faild to extract extensions for log"); 
        return;
    }

    LOG_INFO("Availible Extensions:");
    for(uint32_t i = 0; i < extensionCount; i++){
        LOG_INFO("Extension %u, %s", i, extensions[i].extensionName);
    }
       
    free(extensions);
    extensions = NULL;
};

const char** GetRequiredExtensions(uint32_t* _numExtensions, int _shouldLog){

    // Get required extensions for vk instance
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    *_numExtensions = glfwExtensionCount;

    if(_shouldLog){
        LOG_INFO("NumGLFWExtensionsRequired: %i", glfwExtensionCount);
        for(int i = 0; i < glfwExtensionCount; i++){
            LOG_INFO("GLFWExtensionFound: %s", glfwExtensions[i]);
        }
    }

    // Update total to include validation layer spesific extension
    uint32_t totalExtensionCount = glfwExtensionCount;
    if(ENABLE_VALIDATION_LAYERS){
        totalExtensionCount++;
    }
    *_numExtensions = totalExtensionCount;

    // Allocate memort for extension string array and fill with glfw and validation extension names.
    const char** extensions = malloc(sizeof(const char*) * totalExtensionCount);
    
    // glfw
    for(int i = 0; i < glfwExtensionCount; i++){
        extensions[i] = glfwExtensions[i];
    }

    // Validation
    if(ENABLE_VALIDATION_LAYERS){
        extensions[glfwExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        
        if(_shouldLog){
            LOG_INFO("Validation Layer Extnesion are enabled enabled, updating extension requirements");
            LOG_INFO("NumTotalExtensionsRequired: %i", totalExtensionCount);
            for(int i = 0; i < totalExtensionCount; i++){
                LOG_INFO("AllExtensionsFound: %s", extensions[i]);
            }
        }
    }

    return extensions;
};

int IsValidationLayerSupported(int _shouldLog, const char* const* _validationLayers, const int _numValidationLayers){
    // Much like with exentions, arg1 is numb layers. 
    //it is set to 0 if NUll for arg2 is passed, arg2 is the array to populate.
    uint32_t layerCount;
    if(vkEnumerateInstanceLayerProperties(&layerCount, NULL) != VK_SUCCESS){
        LOG_ERROR("Availible vulkan validation layer counting failed");
        return 0;
    }
    //debugLOG_INFO("Num availible volkan validation layers: %i", layerCount);

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

    if(_shouldLog){
        LOG_INFO("Availbile validation layers:");
        for(uint32_t i = 0; i < layerCount; i++){
            LOG_INFO("%s", layerProperties[i].layerName);
        }
    }

    // Check to make sure any layers we are asking for actually exist before returning true;
    for(int i = 0; i < _numValidationLayers; i++){
        int validLayer = 0;
        for(int j = 0; j < layerCount; j++){
            if(strcmp(_validationLayers[i], layerProperties[j].layerName) == 0){
                validLayer = 1; // true
                break;
            }
        }

        // If we get here one of the layers we asked for does not exist, so it will fail
        if(!validLayer){
            free(layerProperties);
            layerProperties = NULL;
            
            if (_validationLayers[i]) {
                LOG_WARN("Validation layer '%s' might not be valid or exist", _validationLayers[i]);
            }

            return 0;
        }
    }

    free(layerProperties);
    layerProperties = NULL;
    return 1;
};

/*------------------------------------------------------------------------------*/


VkSurfaceKHR SetupSurface(GLFWwindow* _window, VkInstance* _inst){
    LOG_INFO("Setup VkSurface");
    VkSurfaceKHR surface;

    if(glfwCreateWindowSurface(*_inst, _window, NULL, &surface) != VK_SUCCESS){
        LOG_ERROR("Faild to create window surface");
    }

    return surface;
};

/*------------------------------------------------------------------------------*/


VkPhysicalDevice SetupPhysicalDevice(VkInstance* _vki, VkSurfaceKHR* _surface){

    //LogDeviceSupport(_vki, 1);
    LOG_INFO("Setup Physical Device");
    int numAvailableDevices = 0;
    VkPhysicalDevice* availibleDevices =  GetAvailibleDevices(_vki, &numAvailableDevices);
    if(!availibleDevices){
        LOG_ERROR("Avaiible device failed");
        return VK_NULL_HANDLE;
    }
    VkPhysicalDevice bestDevice = GetBestSuitedPhysicalDevice(_vki, _surface, availibleDevices, numAvailableDevices);

    free(availibleDevices);
    availibleDevices = NULL;

    return bestDevice;
};

void LogDeviceSupport(VkInstance* _vki, int _shouldLog){

    if(!_shouldLog) 
        return;

    // arg1 is the name null is fine, arg2 is the num exensions to fill, 
    // arg3 is the arrat to fill if null arg2 = max num extensions.
    uint32_t deviceCount = 0;
    if(vkEnumeratePhysicalDevices(*_vki, &deviceCount, NULL) != VK_SUCCESS){
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
    if(vkEnumeratePhysicalDevices(*_vki, &deviceCount, devices) != VK_SUCCESS){
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

VkPhysicalDevice* GetAvailibleDevices(VkInstance* _vki, int* _numDevices){

    uint32_t deviceCount = 0;
    if(vkEnumeratePhysicalDevices(*_vki, &deviceCount, NULL) != VK_SUCCESS){
       LOG_ERROR("Faild to find GPUs with Vulkan support");
       return NULL;
    }

    VkPhysicalDevice* devices = malloc(sizeof(VkPhysicalDevice) * deviceCount);
    if(!devices){
       LOG_ERROR("Device memory allocation faild");
       return NULL;
    }

    // Get the devices
    if(vkEnumeratePhysicalDevices(*_vki, &deviceCount, devices) != VK_SUCCESS){
       LOG_ERROR("Faild to extract devices");
       free(devices);
       devices = NULL;
       return NULL;
    }

    *_numDevices = deviceCount;
    return devices;
};

VkPhysicalDevice GetBestSuitedPhysicalDevice(VkInstance* _vki, VkSurfaceKHR* _surface, VkPhysicalDevice* _availableDevices, const int _numDevices){

    int bestDeviceScore = 0;
    int bestDeviceIt = 0;

    for(int i = 0; i < _numDevices; i++){

        if(isDeviceSuitable(_availableDevices[i], _surface)){
            int score = RateDevice(_availableDevices[i]);
            if(score > bestDeviceScore){
                bestDeviceScore = score;
                bestDeviceIt = i;
            }
        }
    }

    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;

    if(bestDeviceScore == 0){
        LOG_WARN("No-Devices suitable");
        return bestDevice;
    }

    bestDevice = _availableDevices[bestDeviceIt];
    return bestDevice;
};

int isDeviceSuitable(VkPhysicalDevice _device, VkSurfaceKHR* _surface){

    int suitable = 0;

    // Querry to se if this device and surface can make an adaquate swapchain
    SwapChainSupportDetails swapChainForDevice = QuerySwapChainSupport(&_device, _surface);

    QueueFamilyIndices queueFamily = {0};
    if(FindQueueFamilies(_device, _surface, &queueFamily) &&
        isDeviceExtensionSupportAvailable(&_device, 0) &&
        isSwapChainAdaquate(&swapChainForDevice)){

        return 1;
    }

    return 0;
};

int isDeviceExtensionSupportAvailable(VkPhysicalDevice* _device, int _shouldLog){

    uint32_t extensionCount;
    if(vkEnumerateDeviceExtensionProperties(*_device, NULL, &extensionCount, NULL) != VK_SUCCESS){
        LOG_ERROR("Faild counting vkExtentions for vkDevice requirements");
        return 0;
    }

    VkExtensionProperties* availableExtensions = malloc(sizeof(VkExtensionProperties) * extensionCount);
    if(!availableExtensions){
        LOG_ERROR("Faild to allocate memory for available extentions during vkDevice extension check");
        return 0;
    }
    
    if(vkEnumerateDeviceExtensionProperties(*_device, NULL, &extensionCount, availableExtensions) != VK_SUCCESS){
        LOG_ERROR("Faild to get vkDevice extension properties");
        return 0;
    }

    int numRequiredExtnesions = sizeof(sRequiredDeviceExtensions) / sizeof(sRequiredDeviceExtensions[0]);

    if(_shouldLog){
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

int RateDevice(VkPhysicalDevice _device){
    
    int score = 0;
    if(_device){

        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(_device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(_device, &deviceFeatures);

        if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU){
            score += 1000;
        }

        score += deviceProperties.limits.maxImageDimension2D;

        if(!deviceFeatures.geometryShader){
            return 0;
        }
    }
    return score;
};

int FindQueueFamilies(VkPhysicalDevice _device, VkSurfaceKHR* _surface, QueueFamilyIndices* _familyQueue){

    // number of availible queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilyProperties = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    if(!queueFamilyProperties){
        LOG_ERROR("Faild to allocate memory for queue families");
        return 0;
    }

    vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, queueFamilyProperties);
    
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
            vkGetPhysicalDeviceSurfaceSupportKHR(_device, i, *_surface, &presentSupport);
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

VkDevice SetupDevice(VkInstance* _inst, VkSurfaceKHR* _surface, VkPhysicalDevice* _physicalDevice, QueueFamilyIndices* _familyIndices){
    
    VkDevice device = {0};

    LOG_INFO("Setup VkDevice");
    if(!FindQueueFamilies(*_physicalDevice, _surface, _familyIndices)){

        LOG_ERROR("No Physical device found");
        return VK_NULL_HANDLE;
    }
    VkDeviceCreateInfo createInfo = {0};
    // Created as an array for each 
    VkDeviceQueueCreateInfo* queueCreateInfo = GetVkDeviceQueueCreateInfo(_inst, _familyIndices);
    VkPhysicalDeviceFeatures physicalDeviceFeatures = GetVkPhysicalDeviceFeatures(_inst);
    PopulateVkDevice(&createInfo, queueCreateInfo, &_familyIndices->mNumFamilies, &physicalDeviceFeatures);

    if(vkCreateDevice(*_physicalDevice, &createInfo, NULL, &device) != VK_SUCCESS){
        LOG_ERROR("Faild to create VkInstance!");
    }

    free(queueCreateInfo);
    queueCreateInfo = NULL;

    return device;
};

VkDeviceQueueCreateInfo* GetVkDeviceQueueCreateInfo(VkInstance* _inst, QueueFamilyIndices* _family){

    // Responsible for describing the number of queues we want for a single queue family.
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

VkPhysicalDeviceFeatures GetVkPhysicalDeviceFeatures(VkInstance* _inst){

    // returns the features the physical device can use like geometry shaders.
    static VkPhysicalDeviceFeatures deviceFeatures = {0};
    return deviceFeatures;
};

void PopulateVkDevice(VkDeviceCreateInfo* _createInfo, VkDeviceQueueCreateInfo* _deviceQueueCreateInfo, int* _numQueues, VkPhysicalDeviceFeatures* _physicalDeviceFeatures){

    // The Logical Device is used to interface with the physical device through pipelines and queues.
  
    _createInfo->sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    _createInfo->pQueueCreateInfos = _deviceQueueCreateInfo;
    _createInfo->queueCreateInfoCount = *_numQueues;
    _createInfo->pEnabledFeatures = _physicalDeviceFeatures;

    uint32_t numExtensionsLayers = sizeof(sRequiredDeviceExtensions) / sizeof(sRequiredDeviceExtensions[0]);
    _createInfo->enabledExtensionCount = numExtensionsLayers;
    _createInfo->ppEnabledExtensionNames = sRequiredDeviceExtensions;
    
    // Older versions required setting valdiation layers in device and instance, this is no longer
    // neccesary, although it is good to do incase they are needed for some old functionality.
    uint32_t numValidationLayers = sizeof(sValidationLayers) / sizeof(sValidationLayers[0]);

    if(ENABLE_VALIDATION_LAYERS && !IsValidationLayerSupported(0, sValidationLayers, numValidationLayers)){
        _createInfo->enabledLayerCount = 0;
        LOG_WARN("Validation layers requested, but not availible, continueing without");
    } else if(ENABLE_VALIDATION_LAYERS && numValidationLayers >= 1){
        _createInfo->enabledLayerCount = numValidationLayers;
        _createInfo->ppEnabledLayerNames = sValidationLayers;
    } else {
        _createInfo->enabledLayerCount = 0;
    }

}

/*------------------------------------------------------------------------------*/

VkSwapchainKHR SetupSwapChain(GLFWwindow* _window, VkDevice* _device, VkPhysicalDevice* _physicalDevice, VkSurfaceKHR* _surface){
   
    LOG_INFO("Setup VkSwapChain");
    VkSwapchainKHR swapChain = {0};
    VkSwapchainCreateInfoKHR createInfo = {0};
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_physicalDevice, _surface);
    PopulateSwapChain(&createInfo, _physicalDevice, _surface, &swapChainSupport, _window);

    if (vkCreateSwapchainKHR(*_device, &createInfo, NULL, &swapChain) != VK_SUCCESS) {
        LOG_ERROR("Faild Creating swap chain");
        return VK_NULL_HANDLE;
    }

    // after creation retreive handels for updating graphics
    //VkSwapchainKHR swapChain;
    //VkFormat swapChainImageFormat;
    //VkExtent2D swapChainExtent;
    //vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    //swapChainImages.resize(imageCount);
    //vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());


    // free support detail arrays
    free(swapChainSupport.mFormats);
    swapChainSupport.mFormats = NULL;
    free(swapChainSupport.mPresentModes);
    swapChainSupport.mPresentModes = NULL;

    return swapChain;
};

void PopulateSwapChain(VkSwapchainCreateInfoKHR* _createInfo, VkPhysicalDevice* _physicalDevice, VkSurfaceKHR* _surface, SwapChainSupportDetails* _swapChainSupport , GLFWwindow* _window){

    _createInfo->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    _createInfo->surface = *_surface;

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(_swapChainSupport->mFormats,_swapChainSupport->mFormatCount);
    VkPresentModeKHR presentMode = ChooseSwapPresentationMode(_swapChainSupport->mPresentModes,_swapChainSupport->mPresentModeCount);
    VkExtent2D extent = ChooseSwapExtent(&_swapChainSupport->mCapabilities, _window);

    // the number of images we want the swapchain to have, alwaus have +1 than minimum becuase of internal proccesses.
    // but never exeed the max amount of images.
    uint32_t imageCount = _swapChainSupport->mCapabilities.minImageCount + 1;
    if(_swapChainSupport->mCapabilities.maxImageCount > 0 && imageCount > _swapChainSupport->mCapabilities.maxImageCount){
        imageCount = _swapChainSupport->mCapabilities.maxImageCount;
    }

    _createInfo->minImageCount = imageCount;
    _createInfo->imageFormat = surfaceFormat.format;
    _createInfo->imageColorSpace = surfaceFormat.colorSpace;
    _createInfo->imageExtent = extent;
    _createInfo->imageArrayLayers = 1; // always 1 unless making for stereoscopic 3D.
    _createInfo->imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //what we are using the images for, in this case it is to render so we use color attachment

    // Next we need to specify how to handle swap chain images that will be used across multiple queue families. This is the case if
        // if the graphics queue is different to the presentation queue, like for us. We will be drawing on the images in the swapchain from the graphics queue and then
        // submitting them on the presentation queue. To ways to handle, VK_SHARING_MODE_EXCLUSIVE an images is owned by one queue family at a time, best for performance.
        // or VK_SHARING_MODE_CONCURRENT where images can be shared across multiple queue families. 
        // if the queue families differ we will use the concurrent one, otherwise we use the exclusive one.
    QueueFamilyIndices indices;
    FindQueueFamilies(*_physicalDevice, _surface, &indices);
    //uint32_t queueFamilyIndices = {indices.mGraphicsFamily, &indices.mPresentFamily};
    uint32_t* queueFamilyIndices = malloc(sizeof(uint32_t)* 2);
    queueFamilyIndices[0] = indices.mGraphicsFamily;
    queueFamilyIndices[1] = indices.mPresentFamily;

    if (indices.mGraphicsFamily != indices.mPresentFamily) {
        _createInfo->imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        _createInfo->queueFamilyIndexCount = 2; // Specifies which families have ownership over a queue
        _createInfo->pQueueFamilyIndices = queueFamilyIndices; // along with this
    } else {
        _createInfo->imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        _createInfo->queueFamilyIndexCount = 0; // Optional
        _createInfo->pQueueFamilyIndices = NULL; // Optional
    }

    free(queueFamilyIndices);
    queueFamilyIndices = NULL;

    _createInfo->preTransform = _swapChainSupport->mCapabilities.currentTransform; // transform image location, no transform = current transform.
    _createInfo->compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // specifies blending with other windows, almost alawys ignore
    _createInfo->presentMode = presentMode;
    _createInfo->clipped = VK_TRUE; // if we want to disable covered pixles like by another window
    _createInfo->oldSwapchain = VK_NULL_HANDLE; // if a new swapchain is to be created the current one goes here.

};

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice* _device, VkSurfaceKHR* _surface){
    
    SwapChainSupportDetails swapChainDetails;

    // Querying for basic surface capabilities taking into account buth the physical device and the surface
    // ------------
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*_device, *_surface, &swapChainDetails.mCapabilities);

    // Querying for the supported surface formats
    // ------------
    uint32_t formatCount;
    if(vkGetPhysicalDeviceSurfaceFormatsKHR(*_device, *_surface, &formatCount, NULL) != VK_SUCCESS){
        LOG_ERROR("Faild counting number of surface formats for vkPhysicalDevice");
        return swapChainDetails;
    }

    // Allocate memory for the format array
    VkSurfaceFormatKHR* availableFormats = malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
    if(!availableFormats){
        LOG_ERROR("Faild to allocate memory for available format types during vkPhysicalDevice swapchain surface format check");
        return swapChainDetails;
    }

    if (formatCount != 0) {
        if(vkGetPhysicalDeviceSurfaceFormatsKHR(*_device, *_surface, &formatCount, availableFormats) != VK_SUCCESS){
            LOG_ERROR("Faild to get vkPhysicalDevice surface format properties");
            return swapChainDetails;
        }
        swapChainDetails.mFormatCount = formatCount;
        swapChainDetails.mFormats = availableFormats;
    }



    // Querying for the supported presentation modes works exactly the same
    // ------------
    uint32_t presentationCount;
    if(vkGetPhysicalDeviceSurfacePresentModesKHR(*_device, *_surface, &presentationCount, NULL) != VK_SUCCESS){
        LOG_ERROR("Faild counting number of presentation modes for vkPhysicalDevice");
        return swapChainDetails;
    }

    // Allocate memory for the presentation array
    VkPresentModeKHR* availablePresentationModes = malloc(sizeof(VkPresentModeKHR) * presentationCount);
    if(!availablePresentationModes){
        LOG_ERROR("Faild to allocate memory for available presentation modes during vkPhysicalDevice swapchain surface check");
        return swapChainDetails;
    }

    if (presentationCount != 0) {
        if(vkGetPhysicalDeviceSurfacePresentModesKHR(*_device, *_surface, &presentationCount, availablePresentationModes) != VK_SUCCESS){
            LOG_ERROR("Faild to get vkPhysicalDevice surface presentation mode properties");
            return swapChainDetails;
        }
        swapChainDetails.mPresentModeCount = presentationCount;
        swapChainDetails.mPresentModes = availablePresentationModes;
    }

    return swapChainDetails;
};

int isSwapChainAdaquate(SwapChainSupportDetails* _swapchain){
    int adaquate = 0;

    if(_swapchain->mFormats && _swapchain->mPresentModes){
        adaquate = 1;
    }

    return adaquate;
};

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(VkSurfaceFormatKHR* _availibleFormats, uint32_t _numFormats){

    // The swapchain format option is important for things like color depth
    // The format member specifies the color chanels and types. VK_FORMAT_B8G8R8A8_SRGB means we store B,G,R and Alpha channels
        // in order with an 8 bit unsiged int for a total of 32 bits per pixel.
    // The color space member indicates if the SRGB color space is supported or not using the VK_COLOR_SPACE_SRGB_NONLINEAR_KHR flag.
        // color space is SRGB if it has it since it results in more accurate colors and is the general standerd for images like textures.
        // that is also why we are using SRGB in the format options.  
        
    //debugLOG_INFO("Num availible vulkan swapchain formats: %i", _numFormats);
    for(uint32_t i = 0; i < _numFormats; i++){
        if(_availibleFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            _availibleFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
                return _availibleFormats[i];
        }
    }
    // if spesific option not found just use first one, can be updated to rank the most suitable
    LOG_WARN("SurfaceFormat optional settings not found, using first format item");
    return _availibleFormats[0];
};

VkPresentModeKHR ChooseSwapPresentationMode(VkPresentModeKHR* _availibleModes, uint32_t _numModes){

    // Arguably the most important setting for the swap chain becuase it represents the actual conditions
        // for showing images to the screen and has four options.
    // VK_PRESENT_MODE_IMMEDIATE_KHR - Images submitted by your application are transferred to the screen right away (can result in tearing).
    // VK_PRESENT_MODE_FIFO_KHR - swapchain acts as a queue with rendered images at the front and new images being added to the back.
        // if the queue if full then the program has to wait, this is similar to vertical sync and the momemebt the display is refreshed is known as the "Vertical Blank"
        // This is the only option guarenteed to be availible. 
    // VK_PRESENT_MODE_FIFO_RELAXED_KHR - this is only differs from the previous one by specifing if the program is late to 
        // transfer the images straight away rather than wait for the next vertical blank. (can also cause visible tearing)
    // VK_PRESENT_MODE_MAILBOX_KHR - This is the same as the second one as well, however instead of waiting when queue is full, it replaces images in the queue with new ones,
        // this is known as "tripple buffering" and results in fewer latency issues and less tearing.

    // We try for MAILBOX since it has a higher quality, (better to use FIFO for things like mobile becuase of energy use)
    for(uint32_t i = 0; i < _numModes; i++){
        if(_availibleModes[i] == VK_PRESENT_MODE_MAILBOX_KHR){
                return _availibleModes[i];
        }
    }

    // If not defalt to the most reliable.
    LOG_WARN("SwapChain presentation mode optional settings not found, using present mode FIFO");
    return VK_PRESENT_MODE_FIFO_KHR;
};

VkExtent2D ChooseSwapExtent(VkSurfaceCapabilitiesKHR* _capabilities, GLFWwindow* _window){

    // The swap chain extent is the resolution of the images in the swapchain, it is almost always exactly 
    // the same as the resolution of the window we are drawing to in pixesls. Its a bit fineky, but allows for custom and auto selection. 

    VkExtent2D actualExtent = {0};

    // if it is auto set it is not at max value
    if(_capabilities->currentExtent.width != UINT32_MAX){
        return _capabilities->currentExtent;
    } else {

        // then we get screen size and clamp it to min and max possible values
        int wnidowWidth, windowHeight;
        glfwGetFramebufferSize(_window, &wnidowWidth, &windowHeight);

        actualExtent.width = CLAMP((uint32_t)wnidowWidth, _capabilities->minImageExtent.width, _capabilities->maxImageExtent.width);
        actualExtent.height = CLAMP((uint32_t)windowHeight, _capabilities->minImageExtent.height, _capabilities->maxImageExtent.height);
    }

    return actualExtent;
};

/*------------------------------------------------------------------------------*/

int CleanupVolk(VkInstance* _vki, VkDebugUtilsMessengerEXT* _debugMessenger, VkDevice* _device,VkSurfaceKHR* _surface, VkSwapchainKHR* _swapChain){
    
    if(ENABLE_VALIDATION_LAYERS){
        DestroyDebugUtilsMessengerEXT(*_vki, *_debugMessenger, NULL);
    }
    vkDestroySwapchainKHR(*_device, *_swapChain, NULL);
    //vkDestroySurfaceKHR(*_vki,*_surface, NULL);
    vkDestroyDevice(*_device, NULL);
    //vkDestroyInstance(*_vki, NULL);

    return 0;
};

// Vk Proxy Functions ----

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messagesSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    // messageSeverity is how severe the message is
    // - "VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT": Diagnostic message
    // - "VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT": Informational message
    // - "VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT": Message about behavior
    // - "VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT": Message about behavior that is invalid
    
    // messageType relevance of message
    // - "VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT": unrelated event to the specification
    // - "VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT": indicates a possible mistake
    // - "VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT": Potential non-optimal use of Vulkan
    
    // pCallbackData contains the important data like the message itself.

    // pUserData allows writing custom data, specified during setup.

    LOG_VKINFO("VK-ValidationLayerLOG: %s", pCallbackData->pMessage);
   
    // used to test validation layers directly, always false.
    return VK_FALSE;
};

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