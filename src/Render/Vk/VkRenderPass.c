
#include "VKManager.h"
#include "VkRenderPass.h"

RenderPassAttachmentContext vkRenderPassAttachmentContext = {0};

VkRenderPass SetupRenderPass(VkDevice _device){
   
    LOG_INFO("Setup VkRenderPass");
 
    VkRenderPass renderPass;

    VkRenderPassCreateInfo renderPassInfo = {0};
    PopulateRenderPass(&renderPassInfo);

    if (vkCreateRenderPass(_device, &renderPassInfo, NULL, &renderPass) != VK_SUCCESS) {
        LOG_ERROR("Faild to create VkRenderPass");
        return VK_NULL_HANDLE;
    }

    return renderPass;
};

void PopulateRenderPass(VkRenderPassCreateInfo * _createInfo){

    PopulateColorAttachment(&vkRenderPassAttachmentContext.mColorAttachment);
    PopulateColorAttachmentRef(&vkRenderPassAttachmentContext.mColorAttachmentRef);
    PopulateSubpassDescription(&vkRenderPassAttachmentContext.mSubpass, &vkRenderPassAttachmentContext.mColorAttachmentRef);

    _createInfo->sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    _createInfo->attachmentCount = 1;
    _createInfo->pAttachments = &vkRenderPassAttachmentContext.mColorAttachment;
    _createInfo->subpassCount = 1;
    _createInfo->pSubpasses = &vkRenderPassAttachmentContext.mSubpass;
};

void PopulateColorAttachment(VkAttachmentDescription* _createInfo){

    // The format should match the format of the swap chain images
    _createInfo->format = gVkSwapChainHandles.mSwapChainImageFormat;
    _createInfo->samples = VK_SAMPLE_COUNT_1_BIT;
    // load op and store op determine what to do with data in the attachment before rendeing and after rendering.
        // loadOP:
            // VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents of the attachment
            // VK_ATTACHMENT_LOAD_OP_CLEAR: Clear the values to a constant at the start
            // VK_ATTACHMENT_LOAD_OP_DONT_CARE: Existing contents are undefined; we don't care about them
        // storeOP:
            // VK_ATTACHMENT_STORE_OP_STORE: Rendered contents will be stored in memory and can be read later
            // VK_ATTACHMENT_STORE_OP_DONT_CARE: Contents of the framebuffer will be undefined after the rendering operation
    _createInfo->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    _createInfo->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    // same as above but for the stencil buffer
    _createInfo->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    _createInfo->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // memory layout for images and their pixel format, some exmaples of options:
        // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color attachment
        // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: Images to be presented in the swap chain
        // VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: Images to be used as destination for a memory copy operation
    // inital layout specifies which layout the iumage will have before the render pas sbegins
    // final layout specifies what to transition to once the render pass finishes. 
    _createInfo->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // dont care what it starts as
    _createInfo->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // as long as it sready for presentation at the end.

};

void PopulateColorAttachmentRef(VkAttachmentReference* _createInfo){

    // defines what attachment index this is, this if first one so it is 0
    _createInfo->attachment = 0; // corresponds to the shaders: layout(location = 0) out vec4 outColor location
    // Since this subpass is for color, we use the color optimizer layout.
    _createInfo->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
};

void PopulateSubpassDescription(VkSubpassDescription* _createInfo, VkAttachmentReference* _ref){

    // Define this as graphics supass since there are other types.
    _createInfo->pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    _createInfo->colorAttachmentCount = 1;
    _createInfo->pColorAttachments = _ref;
}

void CleanupRenderPass(){

    LOG_INFO("Cleanup VkRenderPass");
    vkDestroyRenderPass(gVkContext.mDevice, gVkContext.mRenderPass, NULL);
    
};

/* ----------------- Begin render pass  ------------------- */

void PopulateBeginRenderPassCreateInfo(VkRenderPassBeginInfo* _createInfo, uint32_t _imageIndex){
    
    // Define what renderpass and frambuffer to use
    _createInfo->sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    _createInfo->renderPass = gVkContext.mRenderPass;
    _createInfo->framebuffer = gVkContext.mSwapChainFramebuffers[_imageIndex];
    // Define the size of the render area, should match size of the attachments for best performance.
    VkOffset2D offset = {0, 0};
    _createInfo->renderArea.offset = offset;
    _createInfo->renderArea.extent = gVkSwapChainHandles.mSwapChainExtent;
    // Defines the clear values to sue for VK_ATTACHMENT_LOAD_OP_CLEAR (load opperation for color attachmnent)
    // This gives the background the black color. 
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    _createInfo->clearValueCount = 1;
    _createInfo->pClearValues = &clearColor;
};