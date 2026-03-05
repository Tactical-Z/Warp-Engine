#pragma once

#include "vulkan.h"

void SetupBuffer(VkDevice _device, void* _data, VkBuffer* _buffer, VkDeviceMemory* _bufferMem, VkDeviceSize _size, VkBufferUsageFlags _usage);

void CreateBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBuffer* _buffer, VkDeviceMemory* _bufferMemory);

void CopyBuffer(VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size);

// Finds the memory type in the vk physical device if it is available.
// @param _typeFilter is a bit field filter, used to itterate through athe bits of memproperties to find 
// the bit fliped to 1, signifying a valid memory type
// @param _properties defines the special features of the memory we are looking for to further specify the memtype. 
// @return The index of the memory type we want to use, -1 if failed to find approprtiate type. 
uint32_t FindMemoryType(uint32_t _typeFilter, VkMemoryPropertyFlags _properties);

void CleanupBuffer(VkBuffer _buffer, VkDeviceMemory _bufferMem);