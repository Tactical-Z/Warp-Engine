#pragma once

#include "vulkan.h"

// Setts up the vk sync objcests and loads them directly into vk global context. 
// @return 0 on fail, 1 on success
int SetupSyncObjects(VkDevice _device);

// Populates tvk semaphore syncronisation create info. 
// @param _creatInfo is passed htrough reference and filled.
void PopulateSemaphoreCreateInfo(VkSemaphoreCreateInfo* _createInfo);

// Populates tvk semaphore syncronisation create info. 
// @param _creatInfo is passed htrough reference and filled.
void PopulateFenCereateInfo(VkFenceCreateInfo* _createInfo);

// Cleans up the Vk sync objects for image rendering.
void CleanupSyncObjects();