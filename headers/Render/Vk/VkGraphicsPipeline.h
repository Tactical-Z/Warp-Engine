#pragma once

#include "vulkan.h"

// Setts up the graphics pipeline for vk rendering.
void SetupGraphicsPipeline(VkDevice _device);

uint32_t* ReadShaderFile(const char* _filePath, const char* _type, size_t* _codeSize);

long GetFileSize(const char* _filePath, const char* _type);

// Creats the shader modules by importing the binary spv(SPIR-V) shader file data. Shader modules are just a thin wrapper around the bytecode
// they can be local verriables since they linking and compilation makes the local veriables unnessasry. 
// @param _device is the vk device.
// @param _code is the bytecode of the SPIR-V as a uint32 array.
// @param _codeSize is the size of the array in bytes.
// @return Is the vk shader module. 
VkShaderModule SetupShaderModule(VkDevice _device, uint32_t* _code, size_t _codeSize);

// Populates the shader modules create info with the spv code and info. 
void PopulateShaderModule(VkShaderModuleCreateInfo* _createInfo, uint32_t* _code, size_t _codeSize);

// Assign the vertex shader modules to the pipeline using the pipeline shader stage create infor struct. 
VkPipelineShaderStageCreateInfo GetPipelineVertexShaderStageCreateInfo(VkShaderModule _vertexShaderModule);

// Assign the fragment shader modules to the pipeline using the pipeline shader stage create infor struct. 
VkPipelineShaderStageCreateInfo GetPipelineFragmentShaderStageCreateInfo(VkShaderModule _fragmentShaderModule);

// Since most of the pipeline is baked into the pipline state, we need to manualy define states that can be changed in run time.
VkPipelineDynamicStateCreateInfo GetPiplineDynamicStateCreateInfo();

// Gets the vertex imput state create info for the vk graphics pipeline.
VkPipelineVertexInputStateCreateInfo GetPipelineVertexInputStateCreateInfo();

// Cleans up the Vk Graphics pipeline
void CleanupGraphicsPipeline();