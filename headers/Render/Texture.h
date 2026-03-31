#pragma once

#include "stb_image.h"
#include "vulkan.h"
#include "stdint.h"

typedef struct ImageSize{
    uint32_t mImageWidth;
    uint32_t mImageHeight;
    VkDeviceSize mImageDeviceSize;
} ImageSize;

typedef struct VkBufferTexture {
    VkImage mBufferTextureImage;
    VkDeviceMemory mBufferTextureImageMemory;
} VkBufferTexture;

typedef struct VkTexture {
    VkImage mImage;
    VkDeviceMemory mMemory;
    VkImageView mImageView;
    VkSampler mSampler;
} VkTexture;




VkTexture* LoadTexture(const char* _filePath);

VkTexture* CreateTexture(uint8_t* _pixels);

void LoadImageBufferFromSrc(const char* _fileLocation, VkDevice _device, VkBuffer* _stagingBuffer, VkDeviceMemory* _stagingBufferMemory, VkBufferTexture* _bufferTexture);

stbi_uc* LoadImage(const char* _fileLocation, ImageSize* _imageSize);

void LoadImageBufferFromPixels(uint8_t* _pixels, VkDevice _device, VkBuffer* _stagingBuffer, VkDeviceMemory* _stagingBufferMemory, VkBufferTexture* _bufferTexture);

void TransitionImageLayout(VkImage _image, VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout);

void CopyBufferToImage(VkBuffer _buffer, VkImage _image, ImageSize _imageSize);

void CleanupTexture(VkTexture* _texture);