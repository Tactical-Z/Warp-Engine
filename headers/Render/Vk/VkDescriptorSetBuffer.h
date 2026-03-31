#pragma once

#include "vulkan.h"
#include "Components.h"

VkDescriptorPool SetupDescriptorPool(VkDevice _device); 

VkDescriptorPool SetupUIDescriptorPool(VkDevice _device);

void PopulateDescriptorPoolCreateInfo(VkDescriptorPoolSize* _poolSize, size_t _poolSizeCount, VkDescriptorPoolCreateInfo* _createInfo);

void PopulateUIDescriptorPoolCreateInfo(VkDescriptorPoolSize* _poolSize, VkDescriptorPoolCreateInfo* _createInfo);

VkDescriptorSet SetupMeshDescriptorSet(VkDevice _device, MeshComponent* _mesh);

    // DEPRICATED - WAS GLOBAL
//VkDescriptorSet* SetupDescriptorSets(VkDevice _device);

void PopulateDescriptorSetsCreateInfo(VkDescriptorSetAllocateInfo* _createInfo, VkDescriptorPool _descriptorPool, uint32_t _descriptorSetCount, VkDescriptorSetLayout* _layouts);

VkDescriptorSetLayout SetupDescriptorSetLayout(VkDevice _device);

// Sub descriptor spesific for UBO
void PopulateDescriptorSetBindingUBO(VkDescriptorSetLayoutBinding* _createInfoUBO);

// Sub descriptor spesific for Combined image sampler
void PopulateDescriptorSetBindingCombinedImageSampler(VkDescriptorSetLayoutBinding* _createInfoCIS);

// The layout for the descriptor set buffer contains all sub descriptor bindings, like the UBO layout.
void PopulateDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* _createInfo, VkDescriptorSetLayoutBinding* _bindings, size_t _bindingCount);

// Creaets and initalize UBO buffers for each frame in flight 
void SettupUniformBuffers(VkDevice _device);

void UpdateUniformBuffer(uint32_t _currentImage);

void CleanupDescriptorSetLayout();