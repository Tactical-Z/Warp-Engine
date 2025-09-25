
#include "VKManager.h"
#include "VkImageViewer.h"

VkImageView* SetupImageViews(VkDevice _device, uint32_t _numImageViews){
   
    LOG_INFO("Setup VkImageViews");
    
    if(_numImageViews <= 0){
        LOG_WARN("No image space in swapchain to create image view.");
        return NULL;
    }

    // alocate memory and init all fields to 0;
    VkImageView* VkImageView = malloc(sizeof(VkImageView) * _numImageViews);
    memset(VkImageView, 0 , _numImageViews);
    if(!VkImageView){
        LOG_ERROR("Image views memory allocation faild.");
    }
    
    for (size_t i = 0; i < _numImageViews; i++) {

        VkImageViewCreateInfo createInfo = {0};
        PopulateImageView(&createInfo, i);

        if (vkCreateImageView(_device, &createInfo, NULL, &VkImageView[i]) != VK_SUCCESS) {
            LOG_ERROR("Image View %i, faild to create", i);
        }
    }

    return VkImageView;
};

void PopulateImageView(VkImageViewCreateInfo* _createInfo, size_t _it){

    if(!gVkSwapChainHandles.mSwapChainImages){
        LOG_ERROR("Swap chain images in handle NULL");
        return;
    }
    
    // type and image specify how the imag eshould be interpreted. 
    _createInfo->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    _createInfo->pNext = NULL;
    _createInfo->flags = 0;
    _createInfo->image = gVkSwapChainHandles.mSwapChainImages[_it];
    _createInfo->format = gVkSwapChainHandles.mSwapChainImageFormat;
    _createInfo->viewType = VK_IMAGE_VIEW_TYPE_2D;

    // Components allows to swizzle the color channels around.
    _createInfo->components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    _createInfo->components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    _createInfo->components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    _createInfo->components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    // Subsource range field descirbes the image purpose and which part should be acesed.
    // For now they are color targets without any mipmaping.
    _createInfo->subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    _createInfo->subresourceRange.baseMipLevel = 0;
    _createInfo->subresourceRange.levelCount = 1;
    _createInfo->subresourceRange.baseArrayLayer = 0;
    _createInfo->subresourceRange.layerCount = 1;
};

void CleanupImageViews(){

    LOG_INFO("Cleanup VkSwapChain ImageViewers");
    for (size_t i = 0; i < gVkContext.mNumImageViews; i++){
        vkDestroyImageView(gVkContext.mDevice, gVkContext.mSwapChainImageViews[i], NULL);
    }

    free(gVkContext.mSwapChainImageViews);
    gVkContext.mSwapChainImageViews = NULL;
};