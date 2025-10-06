#pragma once

#include "vulkan.h"

// Setts up the graphics pipeline for vk Frame Buffers. This is setup as an array equal in size to the Image views
// since they reference eachother
// @return An array of vk frame buffer objects equal in size to the number of image views.
VkFramebuffer* SetupFrameBuffers(VkDevice _device, uint32_t _numImages);

// Populates the framebuffer create info.
// @param _creatInfo is passed htrough reference and filled.
// @param _numImageViews is the number of images views in the swapchain
void PopulateFrameBuffer(VkFramebufferCreateInfo* _createInfo, int _imageviewIndex);

// Cleans up the Vk FrameBuffers
void CleanupFrameBuffers();