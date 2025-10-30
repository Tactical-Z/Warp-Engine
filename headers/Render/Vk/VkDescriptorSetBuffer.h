#pragma once

#include "vulkan.h"

VkDescriptorPool SetupDescriptorPool(VkDevice _device); 

void PopulateDescriptorPoolCreateInfo(VkDescriptorPoolSize* _poolSize, VkDescriptorPoolCreateInfo* _createInfo);

VkDescriptorSet* SetupDescriptorSets(VkDevice _device);

void PopulateDescriptorSetsCreateInfo(VkDescriptorSetAllocateInfo* _createInfo, VkDescriptorSetLayout* _layouts);

VkDescriptorSetLayout SetupDescriptorSetLayout(VkDevice _device);

// Sub descriptor spesific for UBO
void PopulateDescriptorSetBindingUBO(VkDescriptorSetLayoutBinding* _createInfoUBO);

// The layout for the descriptor set buffer contains all sub descriptor bindings, like the UBO layout.
void PopulateDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* _createInfo, VkDescriptorSetLayoutBinding* _uboCreateInfo);

// Creaets and initalize UBO buffers for each frame in flight 
void SettupUniformBuffers(VkDevice _device);

void UpdateUniformBuffer(uint32_t _currentImage);

void CleanupDescriptorSetLayout();