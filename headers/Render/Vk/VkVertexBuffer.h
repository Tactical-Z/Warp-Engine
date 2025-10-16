#pragma once

#include "vulkan.h"

VkBuffer SetupVertexBuffer(VkDevice _device);

void PopulateVertexBufferCreateInfo(VkBufferCreateInfo* _createInfo, int _numVertices);

void CleanupVertexBuffer();