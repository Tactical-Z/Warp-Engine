#pragma once

#include "vulkan.h"

typedef struct {
    VkAttachmentDescription mColorAttachment;
    VkAttachmentReference mColorAttachmentRef;
    VkSubpassDescription mSubpass;
} RenderPassAttachmentContext;

// Sets up an array of images view objects that can open and view each image in the vk swap chain object.
// @return An array of image views equal to the size of the swapchain image amount. 
VkRenderPass SetupRenderPass(VkDevice _device);

// Populates the construction info for the render pass.
void PopulateRenderPass(VkRenderPassCreateInfo * _createInfo);

// Gets a VkAttachmentDescription for the color attachment of the render pass.
void PopulateColorAttachment(VkAttachmentDescription* _createInfo);

// Gets a VkAttachmentReference as a color atachment ref for a sub pass.
void PopulateColorAttachmentRef(VkAttachmentReference* _createInfo);

// Gets the descriptor for the subpasses we want to use.
void PopulateSubpassDescription(VkSubpassDescription* _createInfo, VkAttachmentReference* _ref);

// Cleans up the Vk render pass
void CleanupRenderPass();

/* ----------------- Begin render pass  ------------------- */
// All functions that record commands are identified by the vkCmd prefix. 
// This populates the begin create info for starting a render pass.
void PopulateBeginRenderPassCreateInfo(VkRenderPassBeginInfo* _createInfo, uint32_t _imageIndex);