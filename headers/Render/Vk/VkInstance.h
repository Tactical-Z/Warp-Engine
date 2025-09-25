#pragma once

#include "vulkan.h"

// Creates and returns the VkInstance
VkInstance CreateInstance();

// Populates the Crate infor for the VKInstance.
// @param _createInfo is the create info struct to fill.
// @param _appInfo is a struct of app info to for create infor reference.
// @param _initDebugCreateInfo is the debug messsenger create info struct to fill if validation layers are enabled.
void PopulateVkInstanceCreateInfo(VkInstanceCreateInfo* _createInfo, const VkApplicationInfo* _appInfo, struct VkDebugUtilsMessengerCreateInfoEXT* _initDebugCreateInfo);

// Cleans up the VkInstance
void CleanupInstance();