#pragma once

#include "vulkan.h"

// Setts up the graphics pipeline for vk rendering.
void SetupGraphicsPipeline(VkDevice _device);

const char* ReadShaderFile(const char* _filePath, const char* _type);

long GetFileSize(const char* _filePath, const char* _type);

VkShaderModule SetupShaderModule(VkDevice _device, const char* _code);

void PopulateShaderModule(VkShaderModuleCreateInfo* _createInfo, const char* _code);

// Cleans up the Vk Graphics pipeline
void CleanupGraphicsPipeline();