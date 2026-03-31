#pragma once

#include "vulkan.h"
#include "Texture.h"

void SetupVkImage(VkDevice _device, uint32_t _width, uint32_t _height, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBufferTexture* _bufferTexture);

void PopulateVkImageCreateInfo(VkImageCreateInfo* _createInfo, uint32_t _texWidth, uint32_t _texHeight, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage);

VkSampler SetupTextureSampler(VkDevice _device, VkPhysicalDevice _physicalDevice);

void PopulateTextureSamplerCreateInfo(VkSamplerCreateInfo* _createInfo, VkPhysicalDevice _physicalDevice);

void CleanupImage();