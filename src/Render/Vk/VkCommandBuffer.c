
#include "VKManager.h"
#include "UIManager.h"
#include "VkCommandBuffer.h"
#include "VkRenderPass.h"
#include "VkGraphicsPipeline.h"
#include "ComponentSystems.h"

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

VkCommandBuffer BeginSingleTimeCommands(VkDevice _device, VkCommandPool _commandPool){
    
    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = _commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
};

void EndSingleTimeCommands(VkDevice _device, VkCommandPool _commandPool, VkQueue _queue, VkCommandBuffer _commandBuffer){
    vkEndCommandBuffer(_commandBuffer);

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffer;

    vkQueueSubmit(_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_queue);

    vkFreeCommandBuffers(_device, _commandPool, 1, &_commandBuffer);
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


    // Render all Meshses
    for (int i = 0; i < gNumMeshComponents; i++) {

        MeshComponent* mesh = &gMeshComponentSystem[i];
        TransformComponent* transform  = &gTransformComponentSystem[i];

        if(transform){
            int hasTexture = 0;
            if(mesh){               
                VkTexture* tex = mesh->mTexture;
                if(tex){
                    hasTexture = 1;
                }
            }

            vkCmdPushConstants(_commandBuffer, gVkContext.mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(int), &hasTexture);

            mat4 model;
            glm_mat4_identity(model);
            glm_translate(model, transform->mPosition);
            glm_rotate(model, transform->mRotation[0], (vec3){1,0,0});
            glm_rotate(model, transform->mRotation[1], (vec3){0,1,0});
            glm_rotate(model, transform->mRotation[2], (vec3){0,0,1});
            glm_scale(model, transform->mScale);

            vkCmdPushConstants(_commandBuffer, gVkContext.mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 16, sizeof(mat4), model);
        }
       
        if(mesh){
            
            VkBuffer vertexBuffers[] = { mesh->mVertexBuffer };
            VkDeviceSize offsets[] = {0};

            vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gVkContext.mPipelineLayout, 0, 1, &mesh->mDescriptorSet, 0, NULL);

            vkCmdBindVertexBuffers(_commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(_commandBuffer, mesh->mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

            vkCmdDrawIndexed(_commandBuffer, mesh->mIndexCount, 1, 0, 0, 0);
        }
    }

    // Draw ui from ImGui
    DrawUI();

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