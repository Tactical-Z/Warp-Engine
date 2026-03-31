#include "Texture.h"
#include "Logger.h"
#include "VkManager.h"
#include "VkBuffer.h"
#include "VkImage.h"
#include "VkCommandBuffer.h"

// Temp image loader texter
#include "VkImageViewer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

VkTexture* LoadTexture(const char* _filePath){

    VkTexture* texture = malloc(sizeof(VkTexture));
    VkBufferTexture bufferTexture = {0};

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    LoadImageBufferFromSrc(_filePath, gVkContext.mDevice, &stagingBuffer, &stagingBufferMemory, &bufferTexture);

    texture->mImageView = SetupTextureImageView(gVkContext.mDevice, bufferTexture.mBufferTextureImage);
    texture->mSampler = SetupTextureSampler(gVkContext.mDevice, gVkContext.mPhysicalDevice);

    texture->mImage = bufferTexture.mBufferTextureImage;
    texture->mMemory = bufferTexture.mBufferTextureImageMemory;

    return texture;
};

VkTexture* CreateTexture(uint8_t* _pixels, size_t _width, size_t _height){
    VkTexture* texture = malloc(sizeof(VkTexture));
    VkBufferTexture bufferTexture = {0};
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    LoadImageBufferFromPixels(_pixels, _width, _height, gVkContext.mDevice, &stagingBuffer, &stagingBufferMemory, &bufferTexture);

    texture->mImageView = SetupTextureImageView(gVkContext.mDevice, bufferTexture.mBufferTextureImage);
    texture->mSampler = SetupTextureSampler(gVkContext.mDevice, gVkContext.mPhysicalDevice);

    texture->mImage = bufferTexture.mBufferTextureImage;
    texture->mMemory = bufferTexture.mBufferTextureImageMemory;

    return texture;
};

void LoadImageBufferFromSrc(const char* _fileLocation, VkDevice _device, VkBuffer* _stagingBuffer, VkDeviceMemory* _stagingBufferMemory, VkBufferTexture* _bufferTexture){

    ImageSize imageSize = {0};
    stbi_uc* pixels = LoadImage(_fileLocation, &imageSize);
    if (!pixels) {
        LOG_ERROR("Pixel buffer is 0");
        return;
    };
    
    CreateBuffer(imageSize.mImageDeviceSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _stagingBuffer, _stagingBufferMemory);
    UploadToMemory(_device, *_stagingBufferMemory, pixels, (size_t)(imageSize.mImageDeviceSize));
    stbi_image_free(pixels);
    
    
    SetupVkImage(_device, imageSize.mImageWidth, imageSize.mImageHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _bufferTexture);
    
    TransitionImageLayout(_bufferTexture->mBufferTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(*_stagingBuffer, _bufferTexture->mBufferTextureImage, imageSize);
    TransitionImageLayout(_bufferTexture->mBufferTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(_device, *_stagingBuffer, NULL);
    vkFreeMemory(_device, *_stagingBufferMemory, NULL);
};

stbi_uc* LoadImage(const char* _fileLocation, ImageSize* _imageSize){

    int textureWidth, textureHight, textureChannels;
    stbi_uc* pixels = stbi_load(_fileLocation, &textureWidth, &textureHight, &textureChannels, STBI_rgb_alpha);
    _imageSize->mImageWidth = textureWidth;
    _imageSize->mImageHeight = textureHight;
    _imageSize->mImageDeviceSize = textureWidth * textureHight * 4;

    if(!pixels){
        LOG_ERROR("Faild to laod image as texture: %s", _fileLocation);
        return NULL;
    }
    return pixels;
};

void LoadImageBufferFromPixels(uint8_t* _pixels, size_t _width, size_t _height, VkDevice _device, VkBuffer* _stagingBuffer, VkDeviceMemory* _stagingBufferMemory, VkBufferTexture* _bufferTexture){

    ImageSize imageSize = {0};
    imageSize.mImageWidth = _width;
    imageSize.mImageHeight = _height;
    imageSize.mImageDeviceSize = _width * _height * 4; //rgba

    if (!_pixels) {
        LOG_ERROR("Pixel buffer is 0");
        return;
    };
    
    CreateBuffer(imageSize.mImageDeviceSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _stagingBuffer, _stagingBufferMemory);
    UploadToMemory(_device, *_stagingBufferMemory, _pixels, (size_t)(imageSize.mImageDeviceSize));
    
    
    SetupVkImage(_device, imageSize.mImageWidth, imageSize.mImageHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _bufferTexture);
    
    TransitionImageLayout(_bufferTexture->mBufferTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(*_stagingBuffer, _bufferTexture->mBufferTextureImage, imageSize);
    TransitionImageLayout(_bufferTexture->mBufferTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    free(_pixels);
    vkDestroyBuffer(_device, *_stagingBuffer, NULL);
    vkFreeMemory(_device, *_stagingBufferMemory, NULL);
};

void TransitionImageLayout(VkImage _image, VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout){

    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(gVkContext.mDevice, gVkContext.mCommandPool);

    VkImageMemoryBarrier barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = _oldLayout;
    barrier.newLayout = _newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = _image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

    } else if (_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        LOG_ERROR("Unsupported image layout transition");
        return;
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier);

    EndSingleTimeCommands(gVkContext.mDevice, gVkContext.mCommandPool, gVkContext.mGraphicsQueue, commandBuffer);
};

void CopyBufferToImage(VkBuffer _buffer, VkImage _image, ImageSize _imageSize){

    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(gVkContext.mDevice, gVkContext.mCommandPool);

    VkBufferImageCopy region = {0};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset.x = 0;
    region.imageOffset.y = 0;
    region.imageOffset.z = 0;
    region.imageExtent.width = _imageSize.mImageWidth;
    region.imageExtent.height = _imageSize.mImageHeight;
    region.imageExtent.depth = 1;

    vkCmdCopyBufferToImage(commandBuffer, _buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    EndSingleTimeCommands(gVkContext.mDevice, gVkContext.mCommandPool, gVkContext.mGraphicsQueue, commandBuffer);
};

void CleanupTexture(VkTexture* _texture){

    vkDestroyImageView(gVkContext.mDevice, _texture->mImageView, NULL);
    vkDestroyImage(gVkContext.mDevice, _texture->mImage, NULL);
    vkFreeMemory(gVkContext.mDevice, _texture->mMemory, NULL);
    vkDestroySampler(gVkContext.mDevice, _texture->mSampler, NULL);
};