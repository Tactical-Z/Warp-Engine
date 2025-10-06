
#include "VKManager.h"
#include "VkCommandBuffer.h"
#include "VkRenderPass.h"
#include "VkGraphicsPipeline.h"

/* --------------- Command pool --------------------- */

VkCommandPool SetupCommandPool(VkDevice _device){
   
    LOG_INFO("Setup VkCommandPool");

    VkCommandPool commandPool = {0};
    VkCommandPoolCreateInfo poolCreateInfo = {0};
    PopulateCommandPool(&poolCreateInfo);
    if (vkCreateCommandPool(_device, &poolCreateInfo, NULL, &commandPool) != VK_SUCCESS) {
        LOG_ERROR("Faild creating vk command pool");
        return VK_NULL_HANDLE;
    }

    return commandPool;
};

void PopulateCommandPool(VkCommandPoolCreateInfo* _createInfo){

    // Two flags for command pool:
        // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: Hint that command buffers are rerecorded 
            //with new commands very often (may change memory allocation behavior)
        // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: Allow command buffers 
            // to be rerecorded individually, without this flag they all have to be reset together
    _createInfo->sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    _createInfo->flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    _createInfo->queueFamilyIndex = gVkContext.mDeviceSupportDetails.mQueueFamily.mGraphicsFamily;
};

/* --------------- Command buffer --------------------- */


int SetupCommandBuffers(VkDevice _device, VkCommandPool _pool){

    LOG_INFO("Setup VkCommandBuffers");
    gVkContext.mCommandBuffers = malloc(sizeof(VkCommandBuffer) * MAX_FRAMES_IN_FLIGHT);
    memset(gVkContext.mCommandBuffers, 0, MAX_FRAMES_IN_FLIGHT);

    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        VkCommandBufferAllocateInfo bufferCreateInfo = {0};
        PopulateCommandBuffer(&bufferCreateInfo, _pool);
    
        if (vkAllocateCommandBuffers(_device, &bufferCreateInfo, &gVkContext.mCommandBuffers[i]) != VK_SUCCESS) {
            LOG_ERROR("Faild creating vk command buffers");
           return 0;
        }
    }
    
    return 1;
};

void PopulateCommandBuffer(VkCommandBufferAllocateInfo* _createInfo, VkCommandPool _pool){
    
    // level param specifies importance of command buffer
        // VK_COMMAND_BUFFER_LEVEL_PRIMARY: Can be submitted to a queue for execution, but cannot be called from other command buffers.
        // VK_COMMAND_BUFFER_LEVEL_SECONDARY: Cannot be submitted directly, but can be called from primary command buffers.
    _createInfo->sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    _createInfo->commandPool = _pool;
    _createInfo->level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    _createInfo->commandBufferCount = 1; // only one buffer for now

};

/* --------------- Command buffer recording --------------------- */
void RecordDrawCommandBuffer(VkCommandBuffer _commandBuffer, uint32_t _imageIndex){

    // start recording ----
    VkCommandBufferBeginInfo beginCreateInfo = {0};
    PopulateCommandBufferBeginCreateInfo(&beginCreateInfo);
    if (vkBeginCommandBuffer(_commandBuffer, &beginCreateInfo) != VK_SUCCESS) {
        LOG_ERROR("Faild to begin command buffer recording");
        return;
    }

    // Start render pass ----
    VkRenderPassBeginInfo renderPassCreateInfo = {0};
    PopulateBeginRenderPassCreateInfo(&renderPassCreateInfo, _imageIndex);
    // starts the render pass, third paramater defines hpw the drawing 
    // commands within the render pass wil be provided.
        // VK_SUBPASS_CONTENTS_INLINE: The render pass commands will be embedded in the primary 
            // command buffer itself and no secondary command buffers will be executed.
        // VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS: The render pass commands will be executed from secondary command buffers.
    vkCmdBeginRenderPass(_commandBuffer, &renderPassCreateInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Bind graphics pipeline ----
    vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gVkContext.mGraphicsPipeline);

    // Set dynamic variables ----
    VkViewport viewport = {0};
    PopulateViewPort(&viewport);
    vkCmdSetViewport(_commandBuffer, 0, 1, &viewport);
    VkRect2D scissor = {0};
    PopulateScissor(&scissor);
    vkCmdSetScissor(_commandBuffer, 0, 1, &scissor);

    // Issue draw command! -----
        // vertexCount: Even though we don't have a vertex buffer, we technically still have 3 vertices to draw.
        // instanceCount: Used for instanced rendering, use 1 if you're not doing that.
        // firstVertex: Used as an offset into the vertex buffer, defines the lowest value of gl_VertexIndex.
        // firstInstance: Used as an offset for instanced rendering, defines the lowest value of gl_InstanceIndex. 
    vkCmdDraw(_commandBuffer, 3, 1, 0, 0);

    // End render pass ---- 
    vkCmdEndRenderPass(_commandBuffer);

    // End recording ---- 
    if (vkEndCommandBuffer(_commandBuffer) != VK_SUCCESS) {
        LOG_ERROR("Faild to end recoding of command buffer");
        return;
    }

};

void PopulateCommandBufferBeginCreateInfo(VkCommandBufferBeginInfo * _createInfo){
    // Flag param specifies how we want to use the buffer:
        // VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer will be rerecorded right after executing it once.
        // VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary command buffer that will be entirely within a single render pass.
        // VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: The command buffer can be resubmitted while it is also already pending execution.
    _createInfo->sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    _createInfo->flags = 0; // Optional
    // only relevent for secondary buffers, specifies which state to inhert from.
    _createInfo->pInheritanceInfo = NULL; // Optional
};

void CleanupCommandObjects(){

    LOG_INFO("Cleanup VkCommandPool and VkcommandBuffers");
    // also destroys all command buffers
    vkDestroyCommandPool(gVkContext.mDevice, gVkContext.mCommandPool, NULL);
    

    free(gVkContext.mCommandBuffers);
    gVkContext.mCommandBuffers = NULL;

};