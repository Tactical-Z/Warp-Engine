
#include "VKManager.h"
#include "VkFrameBuffer.h"

VkFramebuffer* SetupFrameBuffers(VkDevice _device, uint32_t _numImages){
   
    LOG_INFO("Setup VkSwapchainFramebuffers");
    
    VkFramebuffer* framebuffer = malloc(sizeof(VkFramebuffer) * _numImages);
    if(!framebuffer){
        LOG_ERROR("Faild to allocate memory for the vkSwapChainFaramebuffer");
    }
    memset(framebuffer, 0, _numImages);

    for(size_t i = 0; i <_numImages; i++){
        
        VkFramebufferCreateInfo framebufferInfo = {0};

        PopulateFrameBuffer(&framebufferInfo, _numImages);

        if (vkCreateFramebuffer(_device, &framebufferInfo, NULL, &framebuffer[i]) != VK_SUCCESS) {
            LOG_ERROR("Faild to create vk swapchain framebuffer");
            return NULL;
        }
    }

    return framebuffer;
};

void PopulateFrameBuffer(VkFramebufferCreateInfo* _createInfo, uint32_t _numImageViews){

    for (size_t i = 0; i < _numImageViews; i++) {
       
        VkImageView attachments[] = {
            gVkContext.mSwapChainImageViews[i]
        };

        // Important to define which render pass to use since it is spesific fro each frambuffer
        // attachment count and atacchments specify the vkImageView ovjects that should be bound to the repsecive attachment in the render pass array
        // Width and hight are are the extent, layers referes to the numbe of layers in image array. our swapchain images are single images so the number is 1.
        _createInfo->sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        _createInfo->renderPass = gVkContext.mRenderPass;
        _createInfo->attachmentCount = 1;
        _createInfo->pAttachments = attachments;
        _createInfo->width = gVkSwapChainHandles.mSwapChainExtent.width;
        _createInfo->height = gVkSwapChainHandles.mSwapChainExtent.height;
        _createInfo->layers = 1;
    }
};

void CleanupFrameBuffers(){

    LOG_INFO("Cleanup VkSwapchainFramebuffers");
    for(size_t i = 0; i < gVkContext.mNumImageViews; i++){
        vkDestroyFramebuffer(gVkContext.mDevice, gVkContext.mSwapChainFramebuffers[i], NULL);
    }
    free(gVkContext.mSwapChainFramebuffers);
    gVkContext.mSwapChainFramebuffers = NULL;

};