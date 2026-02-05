#include "VKManager.h"
#include "VkInstance.h"
#include "VkDebugMessenger.h"
#include "VkSurface.h"
#include "VkPhysicalDevice.h"
#include "VkDevice.h"
#include "VkSwapChain.h"
#include "VkImageViewer.h"
#include "VkRenderPass.h"
#include "VkDescriptorSetBuffer.h"
#include "VkGraphicsPipeline.h"
#include "VkFramebuffer.h"
#include "VkCommandBuffer.h"
#include "VkVertexBuffer.h"
#include "VkIndexBuffer.h"
#include "VkSyncObjects.h"
#include "Math.h"

VkContext gVkContext = {0};
SwapChainHandels gVkSwapChainHandles = {0};
uint32_t gCurrentFrame = 0;

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
    gVkContext.mRenderPass = SetupRenderPass(gVkContext.mDevice);
    gVkContext.mDescriptorSetLayout = SetupDescriptorSetLayout(gVkContext.mDevice);
    gVkContext.mGraphicsPipeline = SetupGraphicsPipeline(gVkContext.mDevice);
    gVkContext.mSwapChainFramebuffers = SetupFrameBuffers(gVkContext.mDevice, gVkContext.mNumImageViews);
    gVkContext.mCommandPool = SetupCommandPool(gVkContext.mDevice);
    SetupVertexBuffer(gVkContext.mDevice);
    SetupIndexBuffer(gVkContext.mDevice);
    SettupUniformBuffers(gVkContext.mDevice);
    gVkContext.mDescriptorPool = SetupDescriptorPool(gVkContext.mDevice);
    gVkContext.mUiDescriptorPool = SetupUIDescriptorPool(gVkContext.mDevice);
    gVkContext.mDescriptorSets = SetupDescriptorSets(gVkContext.mDevice);
    SetupCommandBuffers(gVkContext.mDevice, gVkContext.mCommandPool);
    SetupSyncObjects(gVkContext.mDevice);
    
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

void DrawFrame(GLFWwindow* _window){

    // wait until previous frame has finished so that command buffer and smaphores are availible
    vkWaitForFences(gVkContext.mDevice, 1 , &gVkContext.mInFlightFences[gCurrentFrame], VK_TRUE, UINT64_MAX);

    // get next availible image from swapchain by index
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(gVkContext.mDevice, gVkContext.mSwapChain, UINT64_MAX, gVkContext.mImageAvailableSemaphores[gCurrentFrame], VK_NULL_HANDLE, &imageIndex);

    if(result == VK_ERROR_OUT_OF_DATE_KHR){
        RecreateSwapchain(_window);
        return;
    }else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
        LOG_ERROR("Faild to aqquire swap chain image");
        return;
    }

    // reset fence so it is ready for next frame
    vkResetFences(gVkContext.mDevice, 1 ,&gVkContext.mInFlightFences[gCurrentFrame]);

    // Update Uniform buffer so it is ready for new frame
    UpdateUniformBuffer(gCurrentFrame);

    // reset command buffer, then call the draw command using the availible index.
    vkResetCommandBuffer(gVkContext.mCommandBuffers[gCurrentFrame], 0);
    RecordDrawCommandBuffer(gVkContext.mCommandBuffers[gCurrentFrame], imageIndex);

    // We can then submit the command
    VkSubmitInfo submitInfo = {0};
    VkSwapchainKHR swapChains[] = {gVkContext.mSwapChain};
    VkSemaphore signalSemaphores[] = {gVkContext.mRenderFinishedSemaphores[gCurrentFrame]};
    VkSemaphore waitSemaphores[] = {gVkContext.mImageAvailableSemaphores[gCurrentFrame]};
    PopulateDrawSubmitCreateInfo(&submitInfo, signalSemaphores, waitSemaphores);

    if (vkQueueSubmit(gVkContext.mGraphicsQueue, 1, &submitInfo, gVkContext.mInFlightFences[gCurrentFrame]) != VK_SUCCESS) {
        LOG_ERROR("Faild to subbmit draw command to command buffer");
        return;
    }

    // present the image
    VkPresentInfoKHR presentInfo = {0};
    PopulatePresentCreateInfo(&presentInfo, &imageIndex, swapChains , signalSemaphores);
    result = vkQueuePresentKHR(gVkContext.mPresentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        gVkContext.mFramebufferResized = 0;
        RecreateSwapchain(_window);
    } else if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to present swap chain image");
        return;
    }

    gCurrentFrame = (gCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
};

void PopulatePresentCreateInfo(VkPresentInfoKHR* _createInfo, uint32_t* _imageIndex, VkSwapchainKHR* _swapchains, VkSemaphore* _signalSemaphores){

    // What semaphores to wait for
    _createInfo->sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    _createInfo->waitSemaphoreCount = 1;
    _createInfo->pWaitSemaphores = _signalSemaphores;
    // the swapchains to present images to
    _createInfo->swapchainCount = 1;
    _createInfo->pSwapchains = _swapchains;
    _createInfo->pImageIndices = _imageIndex;
    // optional for multiple swapchains to check if all swpachains where successful.
    _createInfo->pResults = NULL; // Optional
};

void PopulateDrawSubmitCreateInfo(VkSubmitInfo* _createInfo, VkSemaphore* _signalSemaphores, VkSemaphore* _waitSemaphores){

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    // First three define the semaphores to wait on before execution begins. We specify the color attachment of the graphics
    // pipeline since we need to wait for the image before applying colors. PopulateDrawSubmitInfo
    _createInfo->sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    _createInfo->waitSemaphoreCount = 1;
    _createInfo->pWaitSemaphores = _waitSemaphores;
    _createInfo->pWaitDstStageMask = waitStages;
    // Specify the command buffer to use
    _createInfo->commandBufferCount = 1;
    _createInfo->pCommandBuffers = &gVkContext.mCommandBuffers[gCurrentFrame];
    // what semaphores to signal when finished.
    _createInfo->signalSemaphoreCount = 1;
    _createInfo->pSignalSemaphores = _signalSemaphores;
};

void CreateBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBuffer* _buffer, VkDeviceMemory* _bufferMemory){
    
    VkBufferCreateInfo bufferInfo = {0};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = _size;
    bufferInfo.usage = _usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(gVkContext.mDevice, &bufferInfo, NULL, _buffer) != VK_SUCCESS) {
        LOG_ERROR("failed to create buffer");
    }

    VkMemoryRequirements memRequirements = {0};
    vkGetBufferMemoryRequirements(gVkContext.mDevice, *_buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, _properties);

    if (vkAllocateMemory(gVkContext.mDevice, &allocInfo, NULL, _bufferMemory) != VK_SUCCESS) {
        LOG_ERROR("failed to allocate buffer memory");
    }

    vkBindBufferMemory(gVkContext.mDevice, *_buffer, *_bufferMemory, 0);
};

void CopyBuffer(VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size){

    // make a temporary command buffer, since trasnfer of memory is done through the command chain, just like drawing.
    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = gVkContext.mCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer = {0};
    vkAllocateCommandBuffers(gVkContext.mDevice, &allocInfo, &commandBuffer);

    // start recording
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // calling once and waiting for completion of command, good to signify this with this flag.
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion = {0};
    copyRegion.size = _size;
    vkCmdCopyBuffer(commandBuffer, _srcBuffer, _dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    // now that the command is recorded we can submit it
    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(gVkContext.mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(gVkContext.mGraphicsQueue);

    vkFreeCommandBuffers(gVkContext.mDevice, gVkContext.mCommandPool, 1, &commandBuffer);
};

int CleanupVolk(){

    vkDeviceWaitIdle(gVkContext.mDevice);
    if(ENABLE_VALIDATION_LAYERS){
        CleanupDebugMessenger();
    }
    CleanupSyncObjects();
    CleanupIndexBuffer();
    CleanupVertexBuffer();
    CleanupCommandObjects();
    CleanupFrameBuffers();
    CleanupGraphicsPipeline();
    CleanupRenderPass();
    CleanupImageViews();
    CleanupSwapChain();
    CleanupDescriptorSetLayout();
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