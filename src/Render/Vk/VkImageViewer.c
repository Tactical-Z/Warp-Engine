
#include "VKManager.h"
#include "VkImageViewer.h"
#include "Logger.h"

VkImageView* SetupImageViews(VkDevice _device, uint32_t _numImageViews){
   
    LOG_INFO("Setup VkImageViews");
    
    if(_numImageViews <= 0){
        LOG_WARN("No image space in swapchain to create image view.");
        return NULL;
    }

    // alocate memory and init all fields to 0;
    VkImageView* imageView = malloc(sizeof(imageView) * _numImageViews);
    memset(imageView, 0, sizeof(imageView) * _numImageViews);
    if(!imageView){
        LOG_ERROR("Image views memory allocation faild.");
    }
    
    for (size_t i = 0; i < _numImageViews; i++) {

        VkImageViewCreateInfo createInfo = {0};
    
        imageView[i] = CreateImageView(_device, gVkSwapChainHandles.mSwapChainImages[i], gVkSwapChainHandles.mSwapChainImageFormat);
    }
    return imageView;
};

VkImageView SetupTextureImageView(VkDevice _device, VkImage _textureImage){

    LOG_INFO("Setup VkTextureImageView");
    return CreateImageView(_device, _textureImage, VK_FORMAT_R8G8B8A8_SRGB);
};

VkImageView CreateImageView(VkDevice _device, VkImage _image, VkFormat _format) {
    VkImageViewCreateInfo viewInfo = {0};
    PopulateImageView(&viewInfo, _image, _format);
    
    VkImageView imageView = {0};
    if (vkCreateImageView(_device, &viewInfo, NULL, &imageView) != VK_SUCCESS) {
        LOG_ERROR("failed to create image view");
        return VK_NULL_HANDLE;
    }
    return imageView;
}

void PopulateImageView(VkImageViewCreateInfo* _createInfo, VkImage _image, VkFormat _format){

    // type and image specify how the imag eshould be interpreted. 
    _createInfo->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    _createInfo->viewType = VK_IMAGE_VIEW_TYPE_2D;
    _createInfo->pNext = NULL;
    _createInfo->flags = 0;
    _createInfo->image = _image;
    _createInfo->format = _format;
   
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

    vkDestroySampler(gVkContext.mDevice, gVkContext.mTextureSampler, NULL);
    vkDestroyImageView(gVkContext.mDevice, gVkContext.mTextureImageView, NULL);
};