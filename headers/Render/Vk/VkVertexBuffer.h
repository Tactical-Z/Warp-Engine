#pragma once

#include "vulkan.h"

VkPhysicalDevice SetupVertexBuffer();

void PopulateVertexBufferCreateInfo(VkFramebufferCreateInfo* _createInfo, int _imageviewIndex);

void CleanupVertexBuffer();