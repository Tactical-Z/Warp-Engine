
#include "VkDescriptorSetBuffer.h"
#include "ComponentSystems.h"
#include "VkManager.h"
#include "VkBuffer.h"  
#include <stdint.h>
#include <time.h>
#include <cglm.h>

VkDescriptorPool SetupDescriptorPool(VkDevice _device){

    LOG_INFO("Setup VkDescriptorPool");
    VkDescriptorPool descriptorPool = {0};
    VkDescriptorPoolSize poolSize[2] = {0};
    size_t numPoolsizes = sizeof(poolSize) / sizeof(poolSize[0]);
    VkDescriptorPoolCreateInfo poolInfo = {0};
    PopulateDescriptorPoolCreateInfo(poolSize, numPoolsizes, &poolInfo);

    if (vkCreateDescriptorPool(_device, &poolInfo, NULL, &descriptorPool) != VK_SUCCESS) {
        LOG_ERROR("Failed to create descriptor pool");
    }

    return descriptorPool;
};

VkDescriptorPool SetupUIDescriptorPool(VkDevice _device){

    LOG_INFO("Setup VkDescriptorPool for UI");
    VkDescriptorPool descriptorPool = {0};
    VkDescriptorPoolSize poolSize = {0};
    VkDescriptorPoolCreateInfo poolInfo = {0};
    PopulateUIDescriptorPoolCreateInfo(&poolSize, &poolInfo);

    if (vkCreateDescriptorPool(_device, &poolInfo, NULL, &descriptorPool) != VK_SUCCESS) {
        LOG_ERROR("Failed to create descriptor pool");
    }

    return descriptorPool;
};

void PopulateDescriptorPoolCreateInfo(VkDescriptorPoolSize* _poolSize, size_t _poolSizeCount, VkDescriptorPoolCreateInfo* _createInfo){
    
    // Describes which discriptor sets are going to contain and how many
    // We have two descriptiors
    // UBO
    _poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    _poolSize[0].descriptorCount = MAX_FRAMES_IN_FLIGHT;

    // Combined image sampler
    _poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    _poolSize[1].descriptorCount = MAX_FRAMES_IN_FLIGHT;

    _createInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    _createInfo->poolSizeCount = _poolSizeCount;
    _createInfo->pPoolSizes = _poolSize;
    _createInfo->maxSets = (uint32_t)MAX_FRAMES_IN_FLIGHT;
};

void PopulateUIDescriptorPoolCreateInfo(VkDescriptorPoolSize* _poolSize, VkDescriptorPoolCreateInfo* _createInfo){
    
    // Describes which discriptor sets are going to contain and how many
    // We have one descriptor for each frame.
    _poolSize->type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    _poolSize->descriptorCount = 100;

    _createInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    _createInfo->poolSizeCount = 1;
    _createInfo->pPoolSizes = _poolSize;
    _createInfo->maxSets = 100;
};

VkDescriptorSet SetupMeshDescriptorSet(VkDevice _device, MeshComponent* _mesh){

    if(!_mesh){
        LOG_ERROR("No valid mesh for Descrptor set setup");
        return VK_NULL_HANDLE;
    }

    VkTexture* tex = _mesh->mTexture ? _mesh->mTexture : gVkContext.mDefaultTexture;

    VkDescriptorSet descriptorSet = {0};
    VkDescriptorSetAllocateInfo allocInfo = {0};
    PopulateDescriptorSetsCreateInfo(&allocInfo, gVkContext.mDescriptorPool, 1, &gVkContext.mDescriptorSetLayout);
    if (vkAllocateDescriptorSets(_device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
        LOG_ERROR("Failed to allocate descriptor set");
        return VK_NULL_HANDLE;
    }

    // UBO
    VkDescriptorBufferInfo UBOBufferInfo = {0};
    UBOBufferInfo.buffer = gVkContext.mUniformBuffers[0]; // or current frame
    UBOBufferInfo.offset = 0;
    UBOBufferInfo.range = sizeof(UniformBufferObject);

    // TEXTURE
    VkDescriptorImageInfo textureInfo = {0};
    textureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    textureInfo.imageView = tex->mImageView;
    textureInfo.sampler = tex->mSampler;

    VkWriteDescriptorSet writes[2] = {0};

    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = descriptorSet;
    writes[0].dstBinding = 0;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].descriptorCount = 1;
    writes[0].pBufferInfo = &UBOBufferInfo;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstSet = descriptorSet;
    writes[1].dstBinding = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].descriptorCount = 1;
    writes[1].pImageInfo = &textureInfo;

    vkUpdateDescriptorSets(gVkContext.mDevice, 2, writes, 0, NULL);

    return descriptorSet;
};

// GLOBAL SETUP FUNCTION FOR MAX FRAMES IN FLIGHT
// VkDescriptorSet* SetupDescriptorSets(VkDevice _device){

//     LOG_INFO("Setup VkDescriptorSets");

//     VkDescriptorSet* descriptorSets = malloc(sizeof(VkDescriptorSetLayout) * MAX_FRAMES_IN_FLIGHT);

//     VkDescriptorSetAllocateInfo allocInfo = {0};
//     gVkContext.mDescriptorSetLayouts = malloc(sizeof(VkDescriptorSetLayout) * MAX_FRAMES_IN_FLIGHT);
//     for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//         gVkContext.mDescriptorSetLayouts[i] = gVkContext.mDescriptorSetLayout;
//     }
//     PopulateDescriptorSetsCreateInfo(&allocInfo, gVkContext.mDescriptorSetLayouts);

//     if (vkAllocateDescriptorSets(_device, &allocInfo, descriptorSets) != VK_SUCCESS) {
//         LOG_ERROR("failed to allocate descriptor sets");
//     }

//     // now populate each descriptor after it has been allocated.
//     for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//         // specifies the buffer and region within that contains data for the descriptor
//         // UBO -------
//         VkDescriptorBufferInfo UBOBufferInfo = {0};
//         UBOBufferInfo.buffer = gVkContext.mUniformBuffers[i];
//         UBOBufferInfo.offset = 0;
//         UBOBufferInfo.range = sizeof(UniformBufferObject);

//         // Combined image sampler -------
//         VkDescriptorImageInfo CISBufferInfo = {0};
//         CISBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//         CISBufferInfo.imageView = gVkContext.mTextureImageView; // your VkImageView for the texture
//         CISBufferInfo.sampler = gVkContext.mTextureSampler;     // your VkSampler

//         // The update function also takes a write info struct that contains additional info
//         VkWriteDescriptorSet descriptorWrites[2] = {0};
//         size_t descriptorWriteSize = sizeof(descriptorWrites) / sizeof(descriptorWrites[0]);

//         descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//         descriptorWrites[0].dstSet = descriptorSets[i];
//         descriptorWrites[0].dstBinding = 0;
//         descriptorWrites[0].dstArrayElement = 0;
//         descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//         descriptorWrites[0].descriptorCount = 1;
//         descriptorWrites[0].pBufferInfo = &UBOBufferInfo;

//         descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//         descriptorWrites[1].dstSet = descriptorSets[i];
//         descriptorWrites[1].dstBinding = 1;
//         descriptorWrites[1].dstArrayElement = 0;
//         descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//         descriptorWrites[1].descriptorCount = 1;
//         descriptorWrites[1].pImageInfo = &CISBufferInfo;

//         vkUpdateDescriptorSets(_device, descriptorWriteSize, descriptorWrites, 0, NULL);
//     }

//     return descriptorSets;
// };

void PopulateDescriptorSetsCreateInfo(VkDescriptorSetAllocateInfo* _createInfo, VkDescriptorPool _descriptorPool, uint32_t _descriptorSetCount, VkDescriptorSetLayout* _layouts){
    
    _createInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    _createInfo->descriptorPool = _descriptorPool;
    _createInfo->descriptorSetCount = _descriptorSetCount;
    _createInfo->pSetLayouts = _layouts;
};

VkDescriptorSetLayout SetupDescriptorSetLayout(VkDevice _device){
    LOG_INFO("Setup VkDescriptorSetLayout");

    VkDescriptorSetLayout descriptorSetLayout = {0};
    
    // Binding 0: Uniform Buffer
    VkDescriptorSetLayoutBinding uboLayoutBinding = {0};
    PopulateDescriptorSetBindingUBO(&uboLayoutBinding);

    // Binding 1: Combined Image Sampler (Texture)
    VkDescriptorSetLayoutBinding samplerLayoutBinding = {0};
    PopulateDescriptorSetBindingCombinedImageSampler(&samplerLayoutBinding);

    // Combine Bindings
    VkDescriptorSetLayoutBinding bindings[2] = {0};
    size_t bindingCount = sizeof(bindings) / sizeof(bindings[0]);
    bindings[0] = uboLayoutBinding;
    bindings[1] = samplerLayoutBinding;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
    PopulateDescriptorSetLayout(&layoutInfo, bindings, bindingCount);

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

void PopulateDescriptorSetBindingCombinedImageSampler(VkDescriptorSetLayoutBinding* _createInfoCIS){

    _createInfoCIS->binding = 1;
    _createInfoCIS->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    _createInfoCIS->descriptorCount = 1;
    _createInfoCIS->stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    _createInfoCIS->pImmutableSamplers = NULL;

};

void PopulateDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* _createInfo, VkDescriptorSetLayoutBinding* _bindings, size_t _bindingCount){

    _createInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    _createInfo->bindingCount = _bindingCount;
    _createInfo->pBindings = _bindings;
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

    UniformBufferObject ubo = {0};

    // TODO: Change for camera veriables here!
    // VIEW - Camera look veriables
    glm_lookat(
        (vec3){0.0f,0.0f,2.0f}, // camera position
        (vec3){0.0f,0.0f,0.0f}, // camera look direction
        (vec3){0.0f,1.0f,0.0f}, // up vector
        ubo.view
    );
    
    // PROJECTION — perspective with 45° vertical FOV, near=0.1, far=10
    float aspect = (float) gVkSwapChainHandles.mSwapChainExtent.width /
                   (float) gVkSwapChainHandles.mSwapChainExtent.height;
    
    glm_perspective(glm_rad(45.f), aspect, 0.1f, 10.0f, ubo.proj);

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
    //free(gVkContext.mDescriptorSetLayouts);

    vkDestroyDescriptorSetLayout(gVkContext.mDevice, gVkContext.mDescriptorSetLayout, NULL);
};