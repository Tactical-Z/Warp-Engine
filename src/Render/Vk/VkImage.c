
#include "VkImage.h"
#include "VkManager.h"
#include "VkBuffer.h"
#include "Texture.h"
#include "Logger.h"

void SetupVkImage(VkDevice _device, uint32_t _width, uint32_t _height, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBufferTexture* _bufferTexture){

    LOG_INFO("Setup VkImage");
    VkImageCreateInfo imageCreateInfo = {0};
    PopulateVkImageCreateInfo(&imageCreateInfo, _width, _height, _format, _tiling, _usage);

    if (vkCreateImage(_device, &imageCreateInfo, NULL, &_bufferTexture->mBufferTextureImage) != VK_SUCCESS) {
        LOG_ERROR("Faild to create vkImage.");
        return;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(_device, _bufferTexture->mBufferTextureImage, &memRequirements);
    AllocateMemory(_device, memRequirements, _properties, &_bufferTexture->mBufferTextureImageMemory);
    if(vkBindImageMemory(_device, _bufferTexture->mBufferTextureImage, _bufferTexture->mBufferTextureImageMemory, 0) != VK_SUCCESS){
        vkDestroyImage(_device, _bufferTexture->mBufferTextureImage, NULL);
        LOG_ERROR("Faild binding VkImage memory");
        return;
    };
};

void PopulateVkImageCreateInfo(VkImageCreateInfo* _createInfo, uint32_t _texWidth, uint32_t _texHeight, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage){

    _createInfo->sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    _createInfo->imageType = VK_IMAGE_TYPE_2D;
    _createInfo->extent.width = _texWidth;
    _createInfo->extent.height = _texHeight;
    _createInfo->extent.depth = 1;
    _createInfo->mipLevels = 1;
    _createInfo->arrayLayers = 1;
    _createInfo->format = _format;
    _createInfo->tiling = _tiling;
    _createInfo->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    _createInfo->usage = _usage;
    _createInfo->samples = VK_SAMPLE_COUNT_1_BIT;
    _createInfo->sharingMode = VK_SHARING_MODE_EXCLUSIVE;
};

VkSampler SetupTextureSampler(VkDevice _device, VkPhysicalDevice _physicalDevice){
    
    LOG_INFO("Setup VkTextureSampler");
    VkSampler textureSampler = {0};

    VkSamplerCreateInfo samplerInfo = {0};
    PopulateTextureSamplerCreateInfo(&samplerInfo, _physicalDevice);

    if (vkCreateSampler(_device, &samplerInfo, NULL, &textureSampler) != VK_SUCCESS) {
        LOG_ERROR("Failed to create texture sampler");
        return VK_NULL_HANDLE;
    }
    return textureSampler;
};

void PopulateTextureSamplerCreateInfo(VkSamplerCreateInfo* _createInfo, VkPhysicalDevice _physicalDevice){

    _createInfo->sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    _createInfo->magFilter = VK_FILTER_LINEAR;
    _createInfo->minFilter = VK_FILTER_LINEAR;
    _createInfo->addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    _createInfo->addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    _createInfo->addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    _createInfo->borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    _createInfo->unnormalizedCoordinates = VK_FALSE;
    _createInfo->compareEnable = VK_FALSE;
    _createInfo->compareOp = VK_COMPARE_OP_ALWAYS;
    _createInfo->mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    _createInfo->mipLodBias = 0.0f;
    _createInfo->minLod = 0.0f;
    _createInfo->maxLod = 0.0f;

    VkPhysicalDeviceProperties properties = {0};
    vkGetPhysicalDeviceProperties(_physicalDevice, &properties);
    
    _createInfo->anisotropyEnable = VK_TRUE;
    _createInfo->maxAnisotropy = properties.limits.maxSamplerAnisotropy;
};

void CleanupImage(){

};