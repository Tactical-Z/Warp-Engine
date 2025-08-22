
#include "VKManager.h"

void InitVolk(VkInstance* _inst, VkDebugUtilsMessengerEXT* _debugger){

    LOG_INFO("Starting Vk-Init"); 
    *_inst = CreateInstance();
    *_debugger = SetupDebugMessenger(_inst);
    LOG_INFO("Finished Vk-Init");
};

VkInstance CreateInstance(){

    VkInstance instance;
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

void PopulateVkInstanceCreateInfo(VkInstanceCreateInfo* _createInfo, VkApplicationInfo* _appInfo, VkDebugUtilsMessengerCreateInfoEXT* _initDebugCreateInfo){

    _createInfo->pNext = NULL;
    _createInfo->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    _createInfo->pApplicationInfo = _appInfo;

    // These paramaters spcify the desired global extentsions 
    // (Vulkan is an Agnostic API, so it needs an Extension to interface with windows)
    //CheckExtensionSupport(0);
    uint32_t numExtensions = 0;
    const char** extensions = GetRequiredExtensions(&numExtensions, 0);
    _createInfo->enabledExtensionCount = numExtensions;
    _createInfo->ppEnabledExtensionNames = extensions;

    // Manage validation layers for the application
    const char* validationLayers[] = {
        "VK_LAYER_KHRONOS_validation"
    };
    uint32_t numValidationLayers = sizeof(validationLayers) / sizeof(validationLayers[0]);

    if(ENABLE_VALIDATION_LAYERS && !CheckValidationLayerSupport(0, validationLayers, numValidationLayers)){
        _createInfo->enabledLayerCount = 0;
        LOG_WARN("Validation layers requested, but not availible, continueing without");
    } else if(ENABLE_VALIDATION_LAYERS){
        _createInfo->enabledLayerCount = numValidationLayers;
        _createInfo->ppEnabledLayerNames = validationLayers;

        // Creates a seperate vk debug messenger spesifically for instencing and destroying. 
        PopulateVkDebugMessengerCreateInfo(_initDebugCreateInfo);
        _createInfo->pNext = (VkDebugUtilsMessengerCreateInfoEXT*) _initDebugCreateInfo;

    } else {
        _createInfo->enabledLayerCount = 0;
    }

};

VkDebugUtilsMessengerEXT SetupDebugMessenger(VkInstance* _vki){

    if(!ENABLE_VALIDATION_LAYERS) return NULL;

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
    LOG_INFO("Num availible volkan extensions: %i", extensionCount);
    
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
    for(uint32_t i = 0; i < extensionCount; i++)
        LOG_INFO("%s", extensions[i].extensionName);

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

int CheckValidationLayerSupport(int _shouldLog, const char* const* _validationLayers, const int _numValidationLayers){
  
    // Much like with exentions, arg1 is numb layers. 
    //it is set to 0 if NUll for arg2 is passed, arg2 is the array to populate.
    uint32_t layerCount;
    if(vkEnumerateInstanceLayerProperties(&layerCount, NULL) != VK_SUCCESS){
        LOG_ERROR("Availible vulkan validation layer counting failed");
        return 0;
    }
    LOG_INFO("Num availible volkan validation layers: %i", layerCount);

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
                LOG_WARN("Validation layer '%s' is not valid", _validationLayers[i]);
            }

            return 0;
        }
    }

    free(layerProperties);
    layerProperties = NULL;
    return 1;
};

int CleanupVolk(VkInstance* _vki, VkDebugUtilsMessengerEXT* _debugMessenger){
    
    if(ENABLE_VALIDATION_LAYERS){
        DestroyDebugUtilsMessengerEXT(*_vki, *_debugMessenger, NULL);
    }

    vkDestroyInstance(*_vki, NULL);

    // Free extensions since they where allocated spesifically
    //free(extensions);
    //extensions = NULL;

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