#pragma once

#include "vulkan.h"

/* --------------- Command pool --------------------- */
// Actions in vulkan like draw or memory transfer happens in a buffer so vulkan can 
// optimise for all commands per frame. This function returns the command pool 
// where these commands are stored.
VkCommandPool SetupCommandPool(VkDevice _device);

// Populates the command pool create info.
// @param _creatInfo is passed through reference and filled.
void PopulateCommandPool(VkCommandPoolCreateInfo* _createInfo);

/* --------------- Command buffer --------------------- */
// Creates and executes commands from the pool.
int SetupCommandBuffers(VkDevice _device, VkCommandPool _pool);

// Populates the command buffer create info.
// @param _creatInfo is passed through reference and filled.
void PopulateCommandBuffer(VkCommandBufferAllocateInfo* _createInfo, VkCommandPool _pool);

// Does not need explicit cleanup

/* --------------- Command buffer recording --------------------- */
// This record funciton actually writes the command we want to execute to the command buffer inputed.
// Since we are recoding a draw call we need to specify the swapchain image we want to write to.
void RecordDrawCommandBuffer(VkCommandBuffer _commandBuffer, uint32_t _imageIndex);

// Populates the command buffer begin create info. Signifies the begining of a command buffer command push.
// @param _creatInfo is passed through reference and filled.
void PopulateCommandBufferBeginCreateInfo(VkCommandBufferBeginInfo * _createInfo);


// Cleans up the Vk Command objects
void CleanupCommandObjects();