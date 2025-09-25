#pragma once

#include "vulkan.h"

// Sets up an array of images view objects that can open and view each image in the vk swap chain object.
// @return An array of image views equal to the size of the swapchain image amount. 
VkImageView* SetupImageViews(VkDevice _device, uint32_t _numImageViews);

// Populates the construction info for each iteration of image viwer.
// @param _createInfo the create info to fill
// @param _it the current itteration of image viewer
void PopulateImageView(VkImageViewCreateInfo* _createInfo, size_t _it);

// Cleans up the VkInstance
void CleanupImageViews();