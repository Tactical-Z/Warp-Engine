
#include "VkDescriptorSetBuffer.h"
#include "Vertex.h"
#include "VKManager.h"  
#include <stdint.h>
#include <time.h>
#include <cglm.h>

VkDescriptorPool SetupDescriptorPool(VkDevice _device){

    LOG_INFO("Setup VkDescriptorPool");
    VkDescriptorPool descriptorPool = {0};
    VkDescriptorPoolSize poolSize = {0};
    VkDescriptorPoolCreateInfo poolInfo = {0};
    PopulateDescriptorPoolCreateInfo(&poolSize, &poolInfo);

    if (vkCreateDescriptorPool(_device, &poolInfo, NULL, &descriptorPool) != VK_SUCCESS) {
        LOG_ERROR("Failed to create descriptor pool");
    }

    return descriptorPool;
};

void PopulateDescriptorPoolCreateInfo(VkDescriptorPoolSize* _poolSize, VkDescriptorPoolCreateInfo* _createInfo){
    
    // Describes which discriptor sets are going to contain and how many
    // We have one descriptor for each frame.
    _poolSize->type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    _poolSize->descriptorCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;

    _createInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    _createInfo->poolSizeCount = 1;
    _createInfo->pPoolSizes = _poolSize;
    _createInfo->maxSets = (uint32_t)MAX_FRAMES_IN_FLIGHT;
};

VkDescriptorSet* SetupDescriptorSets(VkDevice _device){

    LOG_INFO("Setup VkDescriptorSets");

    VkDescriptorSet* descriptorSets = malloc(sizeof(VkDescriptorSetLayout) * MAX_FRAMES_IN_FLIGHT);

    VkDescriptorSetAllocateInfo allocInfo = {0};
    gVkContext.mDescriptorSetLayouts = malloc(sizeof(VkDescriptorSetLayout) * MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        gVkContext.mDescriptorSetLayouts[i] = gVkContext.mDescriptorSetLayout;
    }
    PopulateDescriptorSetsCreateInfo(&allocInfo, gVkContext.mDescriptorSetLayouts);

    if (vkAllocateDescriptorSets(_device, &allocInfo, descriptorSets) != VK_SUCCESS) {
        LOG_ERROR("failed to allocate descriptor sets");
    }

    // now populate each descriptor after it has been allocated.
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        // specifies the buffer and region within that contains data for the descriptor
        VkDescriptorBufferInfo bufferInfo = {0};
        bufferInfo.buffer = gVkContext.mUniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        // The update function also takes a write info struct that contains additional info
        VkWriteDescriptorSet descriptorWrite = {0};
        // defines the update and binding. 
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        // Specify descriptor type and how many array elements we want to update
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        // configures descriptor with buffer data, fill the right one for the use case:
            // The pBufferInfo field is used for descriptors that refer to buffer data
            // pImageInfo is used for descriptors that refer to image data
            // pTexelBufferView is used for descriptors that refer to buffer view
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = NULL;
        descriptorWrite.pTexelBufferView = NULL;

        vkUpdateDescriptorSets(_device, 1, &descriptorWrite, 0, NULL);
    }

    return descriptorSets;
};

void PopulateDescriptorSetsCreateInfo(VkDescriptorSetAllocateInfo* _createInfo, VkDescriptorSetLayout* _layouts){
    _createInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    _createInfo->descriptorPool = gVkContext.mDescriptorPool;
    _createInfo->descriptorSetCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;
    _createInfo->pSetLayouts = _layouts;
};

VkDescriptorSetLayout SetupDescriptorSetLayout(VkDevice _device){
    LOG_INFO("Setup VkDescriptorSetLayout");

    VkDescriptorSetLayout descriptorSetLayout = {0};

    VkDescriptorSetLayoutBinding uboLayoutBinding = {0};
    PopulateDescriptorSetBindingUBO(&uboLayoutBinding);

    VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
    PopulateDescriptorSetLayout(&layoutInfo, &uboLayoutBinding);

    if (vkCreateDescriptorSetLayout(_device, &layoutInfo, NULL, &descriptorSetLayout) != VK_SUCCESS) {
        LOG_ERROR("failed to create descriptor set layout!");
    }

    return descriptorSetLayout;
};

void PopulateDescriptorSetBindingUBO(VkDescriptorSetLayoutBinding* _createInfoUBO){
    // The first two fields specifies type of descriptor, being a uniform buffer
    _createInfoUBO->binding = 0;
    _createInfoUBO->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // last field specofoes the number of values in the array, 1 since we are only using one uniform buffer struct.
    _createInfoUBO->descriptorCount = 1;
    // In what shader we want the buffer to be availible in. 
    _createInfoUBO->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    // For image sampling descriptors
    _createInfoUBO->pImmutableSamplers = NULL;
};

void PopulateDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* _createInfo, VkDescriptorSetLayoutBinding* _uboCreateInfo){

    _createInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    _createInfo->bindingCount = 1;
    _createInfo->pBindings = _uboCreateInfo;
}

void SettupUniformBuffers(VkDevice _device){
    LOG_INFO("Setup vkUniformBuffers");
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    gVkContext.mUniformBuffers = malloc(sizeof(VkBuffer) * MAX_FRAMES_IN_FLIGHT);
    gVkContext.mUniformBuffersMemory = malloc(sizeof(VkDeviceMemory) * MAX_FRAMES_IN_FLIGHT);
    gVkContext.mUniformBuffersMapped = malloc(sizeof(void*) * MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &gVkContext.mUniformBuffers[i], &gVkContext.mUniformBuffersMemory[i]);
        // Map memory straight after creation to get pointers we can fill later
        vkMapMemory(_device, gVkContext.mUniformBuffersMemory[i], 0, bufferSize, 0, &gVkContext.mUniformBuffersMapped[i]);
    }
};

void UpdateUniformBuffer(uint32_t _currentImage){

    // TODO: Copy 'ubo' to your uniform buffer for _currentImage

    // Static start time
    static struct timespec startTime;
    static int initialized = 0;

    if (!initialized) {
        clock_gettime(CLOCK_MONOTONIC, &startTime);
        initialized = 1;
    }

    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);

    float time = (float)(currentTime.tv_sec - startTime.tv_sec)
               + (float)(currentTime.tv_nsec - startTime.tv_nsec) / 1e9f;


    UniformBufferObject ubo = {0};

    // MODEL — rotate 90 degrees per second around Z axis
    glm_mat4_identity(ubo.model);
    glm_rotate(ubo.model, glm_rad(90.0f) * time, (vec3){0.0f, 0.0f, 1.0f});

    // VIEW — look at (0,0,0) from (2,2,2), up is +Z
    glm_lookat((vec3){2.0f, 2.0f, 2.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 0.0f, 1.0f}, ubo.view);

    // PROJECTION — perspective with 45° vertical FOV, near=0.1, far=10
    float aspect = (float) gVkSwapChainHandles.mSwapChainExtent.width /
                   (float) gVkSwapChainHandles.mSwapChainExtent.height;
    glm_perspective(glm_rad(45.0f), aspect, 0.1f, 10.0f, ubo.proj);

    // Flip Y axis for Vulkan
    ubo.proj[1][1] *= -1.0f;

    memcpy(gVkContext.mUniformBuffersMapped[_currentImage], &ubo, sizeof(ubo));

};

void CleanupDescriptorSetLayout(){

    LOG_INFO("Cleanup VkDescriptorSetLayout, pool and buffers");

    vkDestroyDescriptorPool(gVkContext.mDevice, gVkContext.mDescriptorPool, NULL);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(gVkContext.mDevice, gVkContext.mUniformBuffers[i], NULL);
        vkFreeMemory(gVkContext.mDevice, gVkContext.mUniformBuffersMemory[i], NULL);
    }

    free(gVkContext.mUniformBuffers);
    free(gVkContext.mUniformBuffersMemory);
    free(gVkContext.mUniformBuffersMapped);
    free(gVkContext.mDescriptorSetLayouts);
    free(gVkContext.mDescriptorSets);

    vkDestroyDescriptorSetLayout(gVkContext.mDevice, gVkContext.mDescriptorSetLayout, NULL);
};