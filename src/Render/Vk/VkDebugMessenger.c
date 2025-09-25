
#include "VKManager.h"
#include "VkDebugMessenger.h"

VkDebugUtilsMessengerEXT SetupDebugMessenger(VkInstance _vki){

    if(!ENABLE_VALIDATION_LAYERS) return NULL;

    LOG_INFO("Setup VkDebugger");
    VkDebugUtilsMessengerEXT VkDebugMessenger = {0};

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {0};
    PopulateVkDebugMessengerCreateInfo(&createInfo);

    if(CreateDebugUtilsMessengerEXT(_vki, &createInfo, NULL, &VkDebugMessenger)){
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

void LogExtensionSupport(){

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

const char** GetRequiredExtensions(uint32_t* _numExtensions){

    // Get required extensions for vk instance
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    if(_numExtensions){
        *_numExtensions = glfwExtensionCount;
    }

    if(SHOULD_LOG){
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

    if(_numExtensions){
        *_numExtensions = totalExtensionCount;
    }

    // Allocate memort for extension string array and fill with glfw and validation extension names.
    const char** extensions = malloc(sizeof(const char*) * totalExtensionCount);
    
    // glfw
    for(int i = 0; i < glfwExtensionCount; i++){
        extensions[i] = glfwExtensions[i];
    }

    // Validation
    if(ENABLE_VALIDATION_LAYERS){
        extensions[glfwExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        
        if(SHOULD_LOG){
            LOG_INFO("Validation Layer Extnesion are enabled enabled, updating extension requirements");
            LOG_INFO("NumTotalExtensionsRequired: %i", totalExtensionCount);
            for(int i = 0; i < totalExtensionCount; i++){
                LOG_INFO("AllExtensionsFound: %s", extensions[i]);
            }
        }
    }

    return extensions;
};

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

void CleanupDebugMessenger(){
    
    if(ENABLE_VALIDATION_LAYERS){
        LOG_INFO("Cleanup VkDebugger");
        DestroyDebugUtilsMessengerEXT(gVkContext.mInstance, gVkContext.mDebugger, NULL);
    }
};