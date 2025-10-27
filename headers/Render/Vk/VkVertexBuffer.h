#pragma once

#include "vulkan.h"

void SetupVertexBuffer(VkDevice _device);

void PopulateVertexBufferCreateInfo(VkBufferCreateInfo* _createInfo, int _numVertices);

void PopulateVertexMemoryAllocateInfo(VkMemoryAllocateInfo* _createInfo, VkMemoryRequirements _memRequirements);

// Finds the memory type in the vk physical device if it is available.
// @param _typeFilter is a bit field filter, used to itterate through athe bits of memproperties to find 
// the bit fliped to 1, signifying a valid memory type
// @param _properties defines the special features of the memory we are looking for to further specify the memtype. 
// @return The index of the memory type we want to use, -1 if failed to find approprtiate type. 
uint32_t FindMemoryType(uint32_t _typeFilter, VkMemoryPropertyFlags _properties);

void CleanupVertexBuffer();