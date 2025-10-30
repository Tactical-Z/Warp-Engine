#pragma once

#include "WindowManager.h"
#include "vulkan.h"
#include "Math.h"

#define ENABLE_VALIDATION_LAYERS 1 // true
#define SHOULD_LOG 0 // true

#define MAX_FRAMES_IN_FLIGHT 3
extern uint32_t gCurrentFrame;

static const char* sValidationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};

static const char* sRequiredDeviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Queues -----
typedef struct {
    int mNumFamilies;
    int mHasGraphicsFamily;
    float mGraphicsFamilyQueuePriority;
    uint32_t mGraphicsFamily;
    int mHasPresentFamily;
    float mPresentFamilyQueuePriority;
    uint32_t mPresentFamily;
}QueueFamilyIndices;

typedef struct {
    QueueFamilyIndices mQueueFamily;
}DeviceSupportDetails;

// Handels -----
typedef struct {
    uint32_t mNumImages;
    VkImage* mSwapChainImages;
    VkFormat mSwapChainImageFormat;
    VkExtent2D mSwapChainExtent;
}SwapChainHandels;
extern SwapChainHandels gVkSwapChainHandles;

// Contexts ------
typedef struct {
    VkInstance mInstance;
    VkDebugUtilsMessengerEXT mDebugger;
    VkPhysicalDevice mPhysicalDevice;
    VkDevice mDevice;
    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;
    DeviceSupportDetails mDeviceSupportDetails;
    VkSurfaceKHR mSurface;
    VkSwapchainKHR mSwapChain;
    VkRenderPass mRenderPass;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;
    uint32_t mNumImageViews;
    VkImageView* mSwapChainImageViews;
    VkFramebuffer* mSwapChainFramebuffers;
    VkCommandPool mCommandPool;
    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexBufferMemory;
    VkCommandBuffer* mCommandBuffers;
    
    // sync objects
    VkSemaphore* mImageAvailableSemaphores;
    VkSemaphore* mRenderFinishedSemaphores;
    VkFence* mInFlightFences;
    int mFramebufferResized;
} VkContext;
extern VkContext gVkContext;

// Initalizes all vulkan resources and loads all relevent information into global contexts.
void InitVolk(GLFWwindow* _window);

// Checks if imputed validation layers exist for the application.
// @param _validationLayers Is a reference to the array of validation layers we are checking for.
// @return 1 if all validation layers are supported, 0 otherwise
int IsValidationLayersSupported(const char* const* _validationLayers);

// Checks if imputed device extension exist for the application.
// @param _physicalDevice Is the physical device to reference.
// @param _deviceExtension Is a reference to the array of device extensions we are checking for.
// @param _numRequiredDeviceExtensions is the number of extensions in the array.
// @return 1 if all devvice extensions are supported, 0 otherwise
int IsDeviceExtensionSupported(VkPhysicalDevice _physicalDevice, const char* const* _deviceExtension, uint32_t _numRequiredDeviceExtensions);

// Logs All Supported pshyical devices for this application.
// @param The Vk instance used for finding the suported devices.
void LogDeviceSupport(VkInstance _vki);

void DrawFrame(GLFWwindow* _window);

// Populates present infor for presenting an image from the swap chain
void PopulatePresentCreateInfo(VkPresentInfoKHR* _createInfo, uint32_t* _imageIndex, VkSwapchainKHR* _swapchains, VkSemaphore* _signalSemaphores);

// Populates the create infor for the submission struct for the draw command.
void PopulateDrawSubmitCreateInfo(VkSubmitInfo* _createInfo, VkSemaphore* _signalSemaphores, VkSemaphore* _waitSemaphores);

void CreateBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBuffer* _buffer, VkDeviceMemory* _bufferMemory);

void CopyBuffer(VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size);

// Cleans up all vulkan resources for this application. 
int CleanupVolk();

// Vk Proxy Functions ----

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, 
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, 
    VkDebugUtilsMessengerEXT* pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(
    VkInstance instance, 
    VkDebugUtilsMessengerEXT debugMessenger, 
    const VkAllocationCallbacks* pAllocator);