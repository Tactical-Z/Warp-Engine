
#include "VKManager.h"
#include "VkSwapChain.h"
#include "VkPhysicalDevice.h"
#include "VkImageViewer.h"
#include "VkFrameBuffer.h"
#include "UtilMath.h"

VkSwapchainKHR SetupSwapChain(GLFWwindow* _window, VkDevice _device, VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface){
   
    LOG_INFO("Setup VkSwapChain");
    VkSwapchainKHR swapChain = {0};
    VkSwapchainCreateInfoKHR createInfo = {0};
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_physicalDevice, _surface);
    uint32_t* queueFamilyIndices = malloc(sizeof(uint32_t)* NUM_QUEUE_FAMILY_INDEXES);
    PopulateSwapChain(&createInfo, _physicalDevice, _surface, swapChainSupport, queueFamilyIndices, _window);

    if (vkCreateSwapchainKHR(_device, &createInfo, NULL, &swapChain) != VK_SUCCESS) {
        LOG_ERROR("Faild Creating swap chain");
        return VK_NULL_HANDLE;
    }

    //after creation retreive handels for updating graphics
    vkGetSwapchainImagesKHR(_device, swapChain, &gVkSwapChainHandles.mNumImages, NULL);
    gVkSwapChainHandles.mSwapChainImages = malloc(sizeof(gVkSwapChainHandles.mSwapChainImages) * gVkSwapChainHandles.mNumImages);
    vkGetSwapchainImagesKHR(_device, swapChain, &gVkSwapChainHandles.mNumImages, gVkSwapChainHandles.mSwapChainImages);

    // free support detail arrays
    free(queueFamilyIndices);
    queueFamilyIndices = NULL;
    free(swapChainSupport.mFormats);
    swapChainSupport.mFormats = NULL;
    free(swapChainSupport.mPresentModes);
    swapChainSupport.mPresentModes = NULL;

    return swapChain;
};

int RecreateSwapchain(GLFWwindow* _window){
    
    LOG_INFO("Resizeing window detected, recreating swapchain");
    // if window is minimised, pause app
    int width = 0, height = 0;
    glfwGetFramebufferSize(_window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(_window, &width, &height);
        glfwWaitEvents();
    }

    // Wait until device avilible, then cleanup current swapchain
    vkDeviceWaitIdle(gVkContext.mDevice);
    CleanupFrameBuffers();
    CleanupImageViews();
    CleanupSwapChain();

    // then remake and reassign swapchain and swapchain required objects.
    gVkContext.mSwapChain = SetupSwapChain(_window, gVkContext.mDevice, gVkContext.mPhysicalDevice, gVkContext.mSurface);
    gVkContext.mNumImageViews = gVkSwapChainHandles.mNumImages;
    gVkContext.mSwapChainImageViews = SetupImageViews(gVkContext.mDevice, gVkContext.mNumImageViews);
    gVkContext.mSwapChainFramebuffers = SetupFrameBuffers(gVkContext.mDevice, gVkContext.mNumImageViews);
};

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface){
    
    SwapChainSupportDetails swapChainDetails;

    // Querying for basic surface capabilities taking into account buth the physical device and the surface
    // ------------
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, _surface, &swapChainDetails.mCapabilities);

    // Querying for the supported surface formats
    // ------------
    uint32_t formatCount;
    if(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount, NULL) != VK_SUCCESS){
        LOG_ERROR("Faild counting number of surface formats for vkPhysicalDevice");
        return swapChainDetails;
    }
    //LOG_DEBUG("Format count: %i", formatCount);

    // Allocate memory for the format array
    VkSurfaceFormatKHR* availableFormats = malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
    if(!availableFormats){
        LOG_ERROR("Faild to allocate memory for available format types during vkPhysicalDevice swapchain surface format check");
        return swapChainDetails;
    }

    if (formatCount != 0) {
        if(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount, availableFormats) != VK_SUCCESS){
            LOG_ERROR("Faild to get vkPhysicalDevice surface format properties");
            return swapChainDetails;
        }
        swapChainDetails.mFormatCount = formatCount;
        swapChainDetails.mFormats = availableFormats;
    }

    // Querying for the supported presentation modes works exactly the same
    // ------------
    uint32_t presentationCount;
    if(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &presentationCount, NULL) != VK_SUCCESS){
        LOG_ERROR("Faild counting number of presentation modes for vkPhysicalDevice");
        return swapChainDetails;
    }
    //LOG_DEBUG("presentation Count: %i", presentationCount);

    // Allocate memory for the presentation array
    VkPresentModeKHR* availablePresentationModes = malloc(sizeof(VkPresentModeKHR) * presentationCount);
    if(!availablePresentationModes){
        LOG_ERROR("Faild to allocate memory for available presentation modes during vkPhysicalDevice swapchain surface check");
        return swapChainDetails;
    }

    if (presentationCount != 0) {
        if(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &presentationCount, availablePresentationModes) != VK_SUCCESS){
            LOG_ERROR("Faild to get vkPhysicalDevice surface presentation mode properties");
            return swapChainDetails;
        }
        swapChainDetails.mPresentModeCount = presentationCount;
        swapChainDetails.mPresentModes = availablePresentationModes;
    }

    return swapChainDetails;
};

void PopulateSwapChain(VkSwapchainCreateInfoKHR* _createInfo, VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface, SwapChainSupportDetails _swapChainSupport,uint32_t* _queueFamilyIndexes, GLFWwindow* _window){

    _createInfo->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    _createInfo->surface = _surface;

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(_swapChainSupport.mFormats,_swapChainSupport.mFormatCount);
    VkPresentModeKHR presentMode = ChooseSwapPresentationMode(_swapChainSupport.mPresentModes,_swapChainSupport.mPresentModeCount);
    VkExtent2D extent = ChooseSwapExtent(_swapChainSupport.mCapabilities, _window);

    gVkSwapChainHandles.mSwapChainExtent = extent;
    gVkSwapChainHandles.mSwapChainImageFormat =  surfaceFormat.format;

    // the number of images we want the swapchain to have, alwaus have +1 than minimum becuase of internal proccesses.
    // but never exeed the max amount of images.
    uint32_t imageCount = _swapChainSupport.mCapabilities.minImageCount + 1;
    if(_swapChainSupport.mCapabilities.maxImageCount > 0 && imageCount > _swapChainSupport.mCapabilities.maxImageCount){
        imageCount = _swapChainSupport.mCapabilities.maxImageCount;
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
    QueueFamilyIndices indices = {0};
    FindQueueFamilies(_physicalDevice, _surface, &indices);
    //uint32_t queueFamilyIndices = {indices.mGraphicsFamily, &indices.mPresentFamily};
    if(NUM_QUEUE_FAMILY_INDEXES - 1 != 1){
        LOG_WARN("Change in num queue family indexes must be accounted for in swapchain creation");
    }
    _queueFamilyIndexes[NUM_QUEUE_FAMILY_INDEXES - NUM_QUEUE_FAMILY_INDEXES] = indices.mGraphicsFamily;
    _queueFamilyIndexes[NUM_QUEUE_FAMILY_INDEXES - 1] = indices.mPresentFamily;

    if (indices.mGraphicsFamily != indices.mPresentFamily) {
        _createInfo->imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        _createInfo->queueFamilyIndexCount = 2; // Specifies which families have ownership over a queue
        _createInfo->pQueueFamilyIndices = _queueFamilyIndexes; // along with this
    } else {
        _createInfo->imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        _createInfo->queueFamilyIndexCount = 0; // Optional
        _createInfo->pQueueFamilyIndices = NULL; // Optional
    }

    _createInfo->preTransform = _swapChainSupport.mCapabilities.currentTransform; // transform image location, no transform = current transform.
    _createInfo->compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // specifies blending with other windows, almost alawys ignore
    _createInfo->presentMode = presentMode;
    _createInfo->clipped = VK_TRUE; // if we want to disable covered pixles like by another window
    _createInfo->oldSwapchain = VK_NULL_HANDLE; // if a new swapchain is to be created the current one goes here.
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

    if(SHOULD_LOG){
        for(uint32_t i = 0; i < _numFormats; i++){
            LOG_DEBUG("Format: %i, color %i, asked for format: %i, color %i",_availibleFormats[i].format, _availibleFormats[i].colorSpace,VK_FORMAT_B8G8R8A8_SRGB,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
        }
    }
    
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
    // VK_PRESENT_MODE_IMMEDIATE_KHR - Images submitted by your application are @transferred to the screen right away (can result in tearing).
    // VK_PRESENT_MODE_FIFO_KHR - swapchain acts as a queue with rendered images at the front and new images being added to the back.
        // if the queue if full then the program has to wait, this is similar to vertical sync and the momemebt the display is refreshed is known as the "Vertical Blank"
        // This is the only option guarenteed to be availible. 
    // VK_PRESENT_MODE_FIFO_RELAXED_KHR - this is only differs from the previous one by specifing if the program is late to 
        // transfer the images straight away rather than wait for the next vertical blank. (can also cause visible tearing)
    // VK_PRESENT_MODE_MAILBOX_KHR - This is the same as the second one as well, however instead of waiting when queue is full, it replaces images in the queue with new ones,
        // this is known as "tripple buffering" and results in fewer latency issues and less tearing.

    if(SHOULD_LOG){
        for(uint32_t i = 0; i < _numModes; i++){
            LOG_DEBUG("Presentation mode: %i, asked for: %i",_availibleModes[i], VK_PRESENT_MODE_MAILBOX_KHR);
        }
    }

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

VkExtent2D ChooseSwapExtent(VkSurfaceCapabilitiesKHR _capabilities, GLFWwindow* _window){

    // The swap chain extent is the resolution of the images in the swapchain, it is almost always exactly 
    // the same as the resolution of the window we are drawing to in pixesls. Its a bit fineky, but allows for custom and auto selection. 

    VkExtent2D actualExtent = {0};

    // If it is auto set it is not at max value
    if(_capabilities.currentExtent.width != UINT32_MAX){
        return _capabilities.currentExtent;
    } else {

        // Then we get screen size and clamp it to min and max possible values
        int wnidowWidth, windowHeight;
        glfwGetFramebufferSize(_window, &wnidowWidth, &windowHeight);

        actualExtent.width = CLAMP((uint32_t)wnidowWidth, _capabilities.minImageExtent.width, _capabilities.maxImageExtent.width);
        actualExtent.height = CLAMP((uint32_t)windowHeight, _capabilities.minImageExtent.height, _capabilities.maxImageExtent.height);
    }

    return actualExtent;
};

void CleanupSwapChain(){
    LOG_INFO("Cleanup VkSwapChain");
    free(gVkSwapChainHandles.mSwapChainImages);
    gVkSwapChainHandles.mSwapChainImages = NULL;
    vkDestroySwapchainKHR(gVkContext.mDevice, gVkContext.mSwapChain, NULL);
};